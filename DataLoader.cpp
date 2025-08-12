#include "DataLoader.hpp"
#include "tiff_inspector.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <regex>
#include <tiffio.h>
#include <variant>
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

bool isFileAccessible(const std::string &filename)
{
    std::ifstream file(filename);
    return file.is_open();
}

std::vector<DataPoint> loadDataFromTxt(const std::string &filename)
{
    if (!isFileAccessible(filename))
    {
        throw std::runtime_error("Error opening file: " + filename);
    }
    std::vector<DataPoint> data;
    std::ifstream file(filename);

    std::string line;
    bool readData = false;

    while (std::getline(file, line))
    {
        if (line.find("XYDATA") != std::string::npos)
        {
            readData = true;
            continue;
        }
        if (line.find("Extended Information") != std::string::npos)
        {
            break;
        }
        if (readData)
        {
            double x, y;
            std::istringstream iss(line);
            if (iss >> x >> y)
            {
                data.push_back({x, y});
            }
        }
    }

    file.close();
    return data;
}

// ########################################
// Helper: parse <XAxis>…<End> by scanning the TIFF file as text (original approach)
static std::vector<double> parseXAxisFromFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Error opening file for metadata: " + filename);
    }

    std::string line, lastLineWithXAxis;
    while (std::getline(file, line))
    {
        if (line.find("<XAxis>") != std::string::npos)
        {
            lastLineWithXAxis = line;
        }
    }

    if (lastLineWithXAxis.empty())
    {
        throw std::runtime_error("Could not find frequency support data (<XAxis>…<End>) in file");
    }

    std::smatch match;
    std::regex regex("<XAxis>(.*?)<End>");
    if (!std::regex_search(lastLineWithXAxis, match, regex))
    {
        throw std::runtime_error("Malformed <XAxis>…<End> block");
    }

    std::vector<double> xs;
    std::stringstream ss(match[1].str());
    std::string item;
    while (std::getline(ss, item, ','))
    {
        if (!item.empty())
        {
            xs.push_back(std::stod(item));
        }
    }
    if (xs.empty())
    {
        throw std::runtime_error("Parsed empty frequency support from <XAxis>");
    }
    return xs;
}

// Helper: count pages (IFDs)
static uint32_t count_pages(TIFF *tiff)
{
    uint32_t n = 0;
    do
    {
        ++n;
    } while (TIFFReadDirectory(tiff));
    return n;
}

// Helper: convert one row to doubles according to sample format
template <typename T>
static inline double sample_to_double(const T &v) { return static_cast<double>(v); }

TiffImageData loadDataFromTiff(const std::string &filename)
{
    if (!isFileAccessible(filename))
    {
        throw std::runtime_error("Error opening file: " + filename);
    }

    TIFF *tiff = TIFFOpen(filename.c_str(), "r");
    if (!tiff)
    {
        throw std::runtime_error("Error opening TIFF file: " + filename);
    }

    // Read basic tags from the FIRST directory (z=0)
    if (!TIFFSetDirectory(tiff, 0))
    {
        TIFFClose(tiff);
        throw std::runtime_error("Failed to set TIFF directory 0.");
    }

    uint32_t width = 0, height = 0;
    uint16_t samplesPerPixel = 1, bitsPerSample = 8, sampleFormat = SAMPLEFORMAT_UINT;
    uint16_t planarConfig = PLANARCONFIG_CONTIG;

    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
    if (!TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel))
        samplesPerPixel = 1;
    if (!TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample))
        bitsPerSample = 8;
    if (!TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &sampleFormat))
        sampleFormat = SAMPLEFORMAT_UINT;
    if (!TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &planarConfig))
        planarConfig = PLANARCONFIG_CONTIG;

    // Parse frequency support from ImageDescription (preferred over raw file scan)
    std::vector<double> frequencySupport = parseXAxisFromFile(filename);

    // Count depth (number of directories)
    // Move to last, count, then return to 0
    TIFFSetDirectory(tiff, 0);
    uint32_t totalPages = count_pages(tiff);
    TIFFSetDirectory(tiff, 0); // rewind to first after counting

    // Optionally, cap depth to frequencySupport length if your data is 1 slice per frequency
    uint32_t depth = std::min<uint32_t>(totalPages, static_cast<uint32_t>(frequencySupport.size()));

    // Allocate output: [width*height][depth]
    std::vector<std::vector<double>> imageData(static_cast<size_t>(width) * height,
                                               std::vector<double>(depth, 0.0));

    // Prepare a byte buffer exactly the size libtiff expects
    const tsize_t rowBytes = TIFFScanlineSize(tiff);
    std::vector<uint8_t> rowBuf(static_cast<size_t>(rowBytes));

    // Validate layout we can handle
    if (planarConfig != PLANARCONFIG_CONTIG)
    {
        TIFFClose(tiff);
        throw std::runtime_error("Unsupported PLANARCONFIG_SEPARATE; expected contiguous.");
    }
    if (samplesPerPixel < 1)
        samplesPerPixel = 1; // be defensive

    // Iterate over depth (directories)
    for (uint32_t z = 0; z < depth; ++z)
    {
        if (!TIFFSetDirectory(tiff, z))
        {
            TIFFClose(tiff);
            throw std::runtime_error("Failed to set TIFF directory " + std::to_string(z));
        }

        // (Re-read per-IFD tags in case they vary)
        TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
        TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
        TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
        TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
        TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &planarConfig);

        if (planarConfig != PLANARCONFIG_CONTIG)
        {
            TIFFClose(tiff);
            throw std::runtime_error("Unsupported PLANARCONFIG_SEPARATE at z=" + std::to_string(z));
        }

        const tsize_t thisRowBytes = TIFFScanlineSize(tiff);
        if (static_cast<tsize_t>(rowBuf.size()) != thisRowBytes)
        {
            rowBuf.assign(static_cast<size_t>(thisRowBytes), 0);
        }

        // Read each row
        for (uint32_t y = 0; y < height; ++y)
        {
            if (TIFFReadScanline(tiff, rowBuf.data(), y) < 0)
            {
                TIFFClose(tiff);
                throw std::runtime_error("TIFFReadScanline failed at z=" + std::to_string(z) +
                                         ", y=" + std::to_string(y));
            }

            // Interpret the row according to bits/sample format
            // We read the FIRST sample if SamplesPerPixel > 1
            const uint32_t pxCount = width;
            switch (sampleFormat)
            {
            case SAMPLEFORMAT_IEEEFP:
                if (bitsPerSample == 32)
                {
                    const float *row = reinterpret_cast<const float *>(rowBuf.data());
                    for (uint32_t x = 0; x < pxCount; ++x)
                    {
                        uint32_t flat = y * width + x;
                        imageData[flat][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    }
                }
                else if (bitsPerSample == 64)
                {
                    const double *row = reinterpret_cast<const double *>(rowBuf.data());
                    for (uint32_t x = 0; x < pxCount; ++x)
                    {
                        uint32_t flat = y * width + x;
                        imageData[flat][z] = row[x * samplesPerPixel + 0];
                    }
                }
                else
                {
                    TIFFClose(tiff);
                    throw std::runtime_error("Unsupported IEEEFP BitsPerSample at z=" + std::to_string(z));
                }
                break;

            case SAMPLEFORMAT_UINT:
                if (bitsPerSample == 8)
                {
                    const uint8_t *row = reinterpret_cast<const uint8_t *>(rowBuf.data());
                    for (uint32_t x = 0; x < pxCount; ++x)
                    {
                        uint32_t flat = y * width + x;
                        imageData[flat][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    }
                }
                else if (bitsPerSample == 16)
                {
                    const uint16_t *row = reinterpret_cast<const uint16_t *>(rowBuf.data());
                    for (uint32_t x = 0; x < pxCount; ++x)
                    {
                        uint32_t flat = y * width + x;
                        imageData[flat][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    }
                }
                else if (bitsPerSample == 32)
                {
                    const uint32_t *row = reinterpret_cast<const uint32_t *>(rowBuf.data());
                    for (uint32_t x = 0; x < pxCount; ++x)
                    {
                        uint32_t flat = y * width + x;
                        imageData[flat][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    }
                }
                else
                {
                    TIFFClose(tiff);
                    throw std::runtime_error("Unsupported UINT BitsPerSample at z=" + std::to_string(z));
                }
                break;

            case SAMPLEFORMAT_INT:
                if (bitsPerSample == 8)
                {
                    const int8_t *row = reinterpret_cast<const int8_t *>(rowBuf.data());
                    for (uint32_t x = 0; x < pxCount; ++x)
                    {
                        uint32_t flat = y * width + x;
                        imageData[flat][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    }
                }
                else if (bitsPerSample == 16)
                {
                    const int16_t *row = reinterpret_cast<const int16_t *>(rowBuf.data());
                    for (uint32_t x = 0; x < pxCount; ++x)
                    {
                        uint32_t flat = y * width + x;
                        imageData[flat][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    }
                }
                else if (bitsPerSample == 32)
                {
                    const int32_t *row = reinterpret_cast<const int32_t *>(rowBuf.data());
                    for (uint32_t x = 0; x < pxCount; ++x)
                    {
                        uint32_t flat = y * width + x;
                        imageData[flat][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    }
                }
                else
                {
                    TIFFClose(tiff);
                    throw std::runtime_error("Unsupported INT BitsPerSample at z=" + std::to_string(z));
                }
                break;

            default:
                TIFFClose(tiff);
                throw std::runtime_error("Unsupported SAMPLEFORMAT at z=" + std::to_string(z));
            }
        }
    }

    TIFFClose(tiff);
    return {frequencySupport, imageData};
}

int countTiffPages(const std::string &filename)
{
    TIFF *tiff = TIFFOpen(filename.c_str(), "r");
    if (!tiff)
    {
        throw std::runtime_error("Error opening TIFF file: " + filename);
    }

    int pageCount = 0;
    do
    {
        ++pageCount;
    } while (TIFFReadDirectory(tiff)); // Iterate through all directories (pages)

    TIFFClose(tiff);
    return pageCount;
}

void writeImageDataToTxt(const std::string &filename, const TiffImageData &tiffData)
{
    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    const auto &data = tiffData.imageData;

    // Write each row of the 2D matrix to the file
    for (const auto &row : data)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            outFile << row[i];
            if (i < row.size() - 1)
            {
                outFile << " "; // Separate values with a space
            }
        }
        outFile << "\n"; // Newline after each row
    }

    outFile.close();
    std::cout << "Image data written to " << filename << std::endl;
}

void checkCompression(const std::string &filename)
{
    TIFF *tiff = TIFFOpen(filename.c_str(), "r");
    if (!tiff)
    {
        std::cerr << "Failed to open TIFF file: " << filename << std::endl;
        return;
    }

    uint16_t compression;
    if (TIFFGetField(tiff, TIFFTAG_COMPRESSION, &compression))
    {
        std::cout << "Compression type: ";
        switch (compression)
        {
        case COMPRESSION_NONE:
            std::cout << "None (uncompressed)" << std::endl;
            break;
        case COMPRESSION_LZW:
            std::cout << "LZW" << std::endl;
            break;
        case COMPRESSION_JPEG:
            std::cout << "JPEG" << std::endl;
            break;
        case COMPRESSION_PACKBITS:
            std::cout << "PackBits" << std::endl;
            break;
        case COMPRESSION_DEFLATE:
            std::cout << "Deflate (ZIP)" << std::endl;
            break;
        default:
            std::cout << "Other (" << compression << ")" << std::endl;
            break;
        }
    }
    else
    {
        std::cout << "Compression tag not found. Assuming uncompressed." << std::endl;
    }

    TIFFClose(tiff);
}

void plotSpectralData(const TiffImageData &tiffData)
{
    const auto &freqSupport = tiffData.frequencySupport;
    const auto &data = tiffData.imageData; // Assuming imageData is the 2D matrix

    // Plot the first 5 spectra (or fewer if there are less than 5)
    for (size_t i = 0; i < std::min<size_t>(5, data.size()); ++i)
    {
        plt::plot(freqSupport, data[i], {{"label", "Spectrum " + std::to_string(i + 1)}});
    }

    // Add plot labels and legend
    plt::title("Spectral Data");
    plt::xlabel("Frequency (cm^{-1})");
    plt::ylabel("Intensity");
    plt::legend();
    plt::grid(true);
    plt::show();
}

void plotTxtData(const std::vector<DataPoint> &data)
{
    std::vector<double> x, y;

    // Separate the data into x and y vectors
    for (const auto &point : data)
    {
        x.push_back(point.x);
        y.push_back(point.y);
    }

    // Plot the data using matplotlibcpp
    plt::plot(x, y, {{"label", "TXT Data"}});
    plt::title("TXT Data Plot");
    plt::xlabel("X-axis");
    plt::ylabel("Y-axis");
    plt::legend();
    plt::grid(true);
    plt::show();
}

int main()
{
    try
    {
        // ---- TXT test (optional) ----
        const std::string txtFile   = "1_min_batch1.txt";
        const std::string tiffFile  = "1_3min_b3_50X50_spectral_mapping_1.tif";
        const std::string outputTxt = "imageData_v3.txt";

        std::vector<DataPoint> txtData = loadDataFromTxt(txtFile);
        std::cout << "Loaded TXT data with " << txtData.size() << " data points from " << txtFile << ".\n";

        // plotTxtData(txtData); // optional

        // ---- TIFF info ----
        int pageCount = countTiffPages(tiffFile);         // use the filename-based counter you already have
        std::cout << "TIFF \"" << tiffFile << "\" has " << pageCount << " pages (depth candidates).\n";

        // checkCompression(tiffFile); // optional diagnostics

        // ---- Load 3D TIFF (depth = IFDs) ----
        TiffImageData tiffData = loadDataFromTiff(tiffFile);

        const size_t nSpectra = tiffData.imageData.size();                 // width * height
        const size_t depth    = (tiffData.frequencySupport.size());        // after loader caps to min(pages, freq)
        std::cout << "Loaded TIFF stack as spectra matrix: "
                  << nSpectra << " spectra × " << depth << " depth (freq bins).\n";

        if (static_cast<size_t>(pageCount) != depth) {
            std::cerr << "Warning: TIFF pages (" << pageCount
                      << ") != frequency bins (" << depth
                      << "). Using min(pages, bins) as depth.\n";
        }

        // ---- Export & (optional) plot ----
        writeImageDataToTxt(outputTxt, tiffData);
        std::cout << "Wrote image data to: " << outputTxt << "\n";

        plotSpectralData(tiffData); // optional
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}