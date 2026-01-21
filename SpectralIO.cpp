#include "SpectralIO.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <tiffio.h>

// ---------- utils ----------
bool SpectralIO::isFileAccessible(const std::string& filename) {
    std::ifstream file(filename);
    return file.is_open();
}

// Original approach: scan file for <XAxis>...<End>
std::vector<double> SpectralIO::parseXAxisFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file for metadata: " + filename);

    std::string line, lastLineWithXAxis;
    while (std::getline(file, line)) {
        if (line.find("<XAxis>") != std::string::npos) lastLineWithXAxis = line;
    }
    if (lastLineWithXAxis.empty()) {
        throw std::runtime_error("Could not find <XAxis>…<End> in file");
    }

    std::smatch match;
    std::regex re("<XAxis>(.*?)<End>");
    if (!std::regex_search(lastLineWithXAxis, match, re)) {
        throw std::runtime_error("Malformed <XAxis>…<End> block");
    }

    std::vector<double> xs;
    std::stringstream ss(match[1].str());
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) xs.push_back(std::stod(item));
    }
    if (xs.empty()) throw std::runtime_error("Parsed empty frequency support from <XAxis>");
    return xs;
}

// ---------- TXT ----------
std::vector<DataPoint> SpectralIO::loadTxt(const std::string& filename) const {
    if (!isFileAccessible(filename))
        throw std::runtime_error("Error opening file: " + filename);

    std::vector<DataPoint> data;
    std::ifstream file(filename);

    std::string line;
    bool readData = false;
    while (std::getline(file, line)) {
        if (line.find("XYDATA") != std::string::npos) { readData = true; continue; }
        if (line.find("Extended Information") != std::string::npos) break;

        if (readData) {
            double x, y;
            std::istringstream iss(line);
            if (iss >> x >> y) data.push_back({x, y});
        }
    }
    return data;
}

// ---------- TIFF ----------
int SpectralIO::countTiffPages(const std::string& filename) const {
    TIFF* tiff = TIFFOpen(filename.c_str(), "r");
    if (!tiff) throw std::runtime_error("Error opening TIFF file: " + filename);

    int pageCount = 0;
    do { ++pageCount; } while (TIFFReadDirectory(tiff));
    TIFFClose(tiff);
    return pageCount;
}

TiffImageData SpectralIO::loadTiff(const std::string& filename) const {
    if (!isFileAccessible(filename))
        throw std::runtime_error("Error opening file: " + filename);

    // Parse frequency support
    std::vector<double> frequencySupport = parseXAxisFromFile(filename);

    // RAII handle for TIFF*
    struct TiffCloser { void operator()(TIFF* t) const { if (t) TIFFClose(t); } };
    using TiffPtr = std::unique_ptr<TIFF, TiffCloser>;
    TiffPtr tiff(TIFFOpen(filename.c_str(), "r"));
    if (!tiff) throw std::runtime_error("Error opening TIFF file: " + filename);
    TIFF* tif = tiff.get();

    if (!TIFFSetDirectory(tif, 0))
        throw std::runtime_error("Failed to set TIFF directory 0.");

    uint32_t width = 0, height = 0;
    uint16_t samplesPerPixel = 1, bitsPerSample = 8, sampleFormat = SAMPLEFORMAT_UINT;
    uint16_t planarConfig = PLANARCONFIG_CONTIG;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    if (!TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel)) samplesPerPixel = 1;
    if (!TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample)) bitsPerSample = 8;
    if (!TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleFormat)) sampleFormat = SAMPLEFORMAT_UINT;
    if (!TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarConfig)) planarConfig = PLANARCONFIG_CONTIG;

    // count depth (IFDs)
    TIFFSetDirectory(tif, 0);
    uint32_t totalPages = 0;
    do { ++totalPages; } while (TIFFReadDirectory(tif));
    TIFFSetDirectory(tif, 0);

    // use min(pages, freq bins) as depth
    const uint32_t depth = std::min<uint32_t>(totalPages,
                                              static_cast<uint32_t>(frequencySupport.size()));

    // allocate output: [width*height][depth]
    std::vector<std::vector<double>> imageData(static_cast<size_t>(width) * height,
                                               std::vector<double>(depth, 0.0));

    // row buffer (bytes)
    tsize_t rowBytes = TIFFScanlineSize(tif);
    std::vector<uint8_t> rowBuf(static_cast<size_t>(rowBytes));

    if (planarConfig != PLANARCONFIG_CONTIG)
        throw std::runtime_error("Unsupported PLANARCONFIG_SEPARATE; expected contiguous.");

    // read all directories
    for (uint32_t z = 0; z < depth; ++z) {
        if (!TIFFSetDirectory(tif, z))
            throw std::runtime_error("Failed to set TIFF directory " + std::to_string(z));

        // re-read per IFD
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
        TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
        TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
        TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarConfig);

        if (planarConfig != PLANARCONFIG_CONTIG)
            throw std::runtime_error("Unsupported PLANARCONFIG_SEPARATE at z=" + std::to_string(z));

        tsize_t thisRowBytes = TIFFScanlineSize(tif);
        if (thisRowBytes != rowBytes) {
            rowBytes = thisRowBytes;
            rowBuf.assign(static_cast<size_t>(rowBytes), 0);
        }

        for (uint32_t y = 0; y < height; ++y) {
            if (TIFFReadScanline(tif, rowBuf.data(), y) < 0)
                throw std::runtime_error("TIFFReadScanline failed at z=" + std::to_string(z) +
                                         ", y=" + std::to_string(y));

            const uint32_t pxCount = width;

            switch (sampleFormat) {
                case SAMPLEFORMAT_IEEEFP:
                    if (bitsPerSample == 32) {
                        const float* row = reinterpret_cast<const float*>(rowBuf.data());
                        for (uint32_t x = 0; x < pxCount; ++x)
                            imageData[y * width + x][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    } else if (bitsPerSample == 64) {
                        const double* row = reinterpret_cast<const double*>(rowBuf.data());
                        for (uint32_t x = 0; x < pxCount; ++x)
                            imageData[y * width + x][z] = row[x * samplesPerPixel + 0];
                    } else {
                        throw std::runtime_error("Unsupported IEEEFP BitsPerSample");
                    }
                    break;

                case SAMPLEFORMAT_UINT:
                    if (bitsPerSample == 8) {
                        const uint8_t* row = reinterpret_cast<const uint8_t*>(rowBuf.data());
                        for (uint32_t x = 0; x < pxCount; ++x)
                            imageData[y * width + x][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    } else if (bitsPerSample == 16) {
                        const uint16_t* row = reinterpret_cast<const uint16_t*>(rowBuf.data());
                        for (uint32_t x = 0; x < pxCount; ++x)
                            imageData[y * width + x][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    } else if (bitsPerSample == 32) {
                        const uint32_t* row = reinterpret_cast<const uint32_t*>(rowBuf.data());
                        for (uint32_t x = 0; x < pxCount; ++x)
                            imageData[y * width + x][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    } else {
                        throw std::runtime_error("Unsupported UINT BitsPerSample");
                    }
                    break;

                case SAMPLEFORMAT_INT:
                    if (bitsPerSample == 8) {
                        const int8_t* row = reinterpret_cast<const int8_t*>(rowBuf.data());
                        for (uint32_t x = 0; x < pxCount; ++x)
                            imageData[y * width + x][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    } else if (bitsPerSample == 16) {
                        const int16_t* row = reinterpret_cast<const int16_t*>(rowBuf.data());
                        for (uint32_t x = 0; x < pxCount; ++x)
                            imageData[y * width + x][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    } else if (bitsPerSample == 32) {
                        const int32_t* row = reinterpret_cast<const int32_t*>(rowBuf.data());
                        for (uint32_t x = 0; x < pxCount; ++x)
                            imageData[y * width + x][z] = static_cast<double>(row[x * samplesPerPixel + 0]);
                    } else {
                        throw std::runtime_error("Unsupported INT BitsPerSample");
                    }
                    break;

                default:
                    throw std::runtime_error("Unsupported SAMPLEFORMAT");
            }
        }
    }

    return {frequencySupport, imageData};
}

// ---------- export ----------
void SpectralIO::writeImageDataToTxt(const std::string& filename,
                                     const TiffImageData& tiffData) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open())
        throw std::runtime_error("Failed to open file: " + filename);

    const auto& data = tiffData.imageData;
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            outFile << row[i];
            if (i + 1 < row.size()) outFile << ' ';
        }
        outFile << '\n';
    }
    std::cout << "Image data written to " << filename << std::endl;
}

// ---------- diagnostics ----------
void SpectralIO::checkCompression(const std::string& filename) const {
    TIFF* tiff = TIFFOpen(filename.c_str(), "r");
    if (!tiff) { std::cerr << "Failed to open TIFF: " << filename << "\n"; return; }

    uint16_t compression;
    if (TIFFGetField(tiff, TIFFTAG_COMPRESSION, &compression)) {
        std::cout << "Compression type: ";
        switch (compression) {
            case COMPRESSION_NONE:   std::cout << "None (uncompressed)\n"; break;
            case COMPRESSION_LZW:    std::cout << "LZW\n"; break;
            case COMPRESSION_JPEG:   std::cout << "JPEG\n"; break;
            case COMPRESSION_PACKBITS: std::cout << "PackBits\n"; break;
            case COMPRESSION_DEFLATE:  std::cout << "Deflate (ZIP)\n"; break;
            default: std::cout << "Other (" << compression << ")\n"; break;
        }
    } else {
        std::cout << "Compression tag not found. Assuming uncompressed.\n";
    }
    TIFFClose(tiff);
}