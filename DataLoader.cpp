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
#include "matplotlibcpp.h" // Add this include
namespace plt = matplotlibcpp;

bool isFileAccessible(const std::string& filename) {
    std::ifstream file(filename);
    return file.is_open();
}

std::vector<DataPoint> loadDataFromTxt(const std::string& filename) {
    if (!isFileAccessible(filename)) {
        throw std::runtime_error("Error opening file: " + filename);
    }
    std::vector<DataPoint> data;
    std::ifstream file(filename);

    std::string line;
    bool readData = false;

    while (std::getline(file, line)) {
        if (line.find("XYDATA") != std::string::npos) {
            readData = true;
            continue;
        }
        if (line.find("Extended Information") != std::string::npos) {
            break;
        }
        if (readData) {
            double x, y;
            std::istringstream iss(line);
            if (iss >> x >> y) {
                data.push_back({x, y});
            }
        }
    }

    file.close();
    return data;
}

// TiffImageData loadDataFromTiff(const std::string& filename) {
//     if (!isFileAccessible(filename)) {
//         throw std::runtime_error("Error opening file: " + filename);
//     }

//     TIFF* tiff = TIFFOpen(filename.c_str(), "r");
//     if (!tiff) {
//         throw std::runtime_error("Error opening TIFF file: " + filename);
//     }

//     uint32_t width, height, depth = 1;
//     TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
//     TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
//     TIFFGetField(tiff, TIFFTAG_IMAGEDEPTH, &depth);

//     std::variant<std::vector<std::vector<double>>, std::vector<std::vector<std::vector<double>>>> data;
//     if (depth == 1) {
//         std::vector<std::vector<double>> imageData(height, std::vector<double>(width));
//         for (uint32_t row = 0; row < height; ++row) {
//             TIFFReadScanline(tiff, imageData[row].data(), row);
//         }
//         data = imageData;
//     } else {
//         std::vector<std::vector<std::vector<double>>> imageData(depth, std::vector<std::vector<double>>(height, std::vector<double>(width)));
//         for (uint32_t z = 0; z < depth; ++z) {
//             for (uint32_t row = 0; row < height; ++row) {
//                 TIFFReadScanline(tiff, imageData[z][row].data(), row + z * height);
//             }
//         }
//         data = imageData;
//     }

//     TIFFClose(tiff);

//     std::ifstream file(filename, std::ios::binary);
//     std::string line;
//     std::vector<double> frequencySupport;

//     while (std::getline(file, line)) {
//         std::smatch match;
//         std::regex regex("<XAxis>(.*?)<End>");
//         if (std::regex_search(line, match, regex)) {
//             std::string f_sup_str = match[1].str();
//             std::istringstream iss(f_sup_str);
//             std::string number;
//             while (std::getline(iss, number, ',')) {
//                 frequencySupport.push_back(std::stod(number));
//             }
//             break;
//         }
//     }

//     return {frequencySupport, data};
// }
TiffImageData loadDataFromTiff(const std::string& filename) {
    if (!isFileAccessible(filename)) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    // 1. First, read the frequency support data
    std::vector<double> frequencySupport;
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error opening file for metadata: " + filename);
        }
        
        // Find the last line with the XAxis data
        std::string line;
        std::string lastLineWithXAxis;
        
        while (std::getline(file, line)) {
            if (line.find("<XAxis>") != std::string::npos) {
                lastLineWithXAxis = line;
            }
        }
        
        if (!lastLineWithXAxis.empty()) {
            std::smatch match;
            std::regex regex("<XAxis>(.*?)<End>");
            if (std::regex_search(lastLineWithXAxis, match, regex)) {
                std::string f_sup_str = match[1].str();
                std::stringstream ss(f_sup_str);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    frequencySupport.push_back(std::stod(item));
                }
            }
        }
        
        if (frequencySupport.empty()) {
            throw std::runtime_error("Could not find frequency support data in file");
        }
    }
    
    // 2. Now open the TIFF for image data
    TIFF* tiff = TIFFOpen(filename.c_str(), "r");
    if (!tiff) {
        throw std::runtime_error("Error opening TIFF file: " + filename);
    }
    
    // Get basic TIFF dimensions
    uint32_t width, height, samplesPerPixel;
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    if (samplesPerPixel == 0) samplesPerPixel = 1; // Default if not specified
    
    // Read the TIFF data into a buffer
    size_t bufferSize = width * height * samplesPerPixel;
    std::vector<double> buffer(bufferSize);
    
    for (uint32_t row = 0; row < height; ++row) {
        TIFFReadScanline(tiff, buffer.data() + (row * width * samplesPerPixel), row);
    }
    
    TIFFClose(tiff);
    
    // 3. Determine if this is a 3D image based on the frequency support size
    // and available data dimensions
    uint32_t depth = frequencySupport.size();
    std::variant<std::vector<std::vector<double>>, std::vector<std::vector<std::vector<double>>>> data;
    
    if (depth == 1 || (width * height * samplesPerPixel) == buffer.size()) {
        // This is a 2D image (single slice)
        std::vector<std::vector<double>> imageData(height, std::vector<double>(width));
        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                imageData[y][x] = buffer[y * width + x];
            }
        }
        data = imageData;
    } else {
        // This is a 3D image - we need to reshape it according to the Python logic
        // In Python: data_reshaped = data.transpose(2, 1, 0).reshape(-1, shape)
        
        // First, calculate the correct depth
        // In your Python code, this comes from data.shape[0]
        if (buffer.size() % (width * height) == 0) {
            depth = buffer.size() / (width * height);
        } else {
            // Fall back to frequency support size
            depth = frequencySupport.size();
        }
        
        // Create 3D structure
        std::vector<std::vector<std::vector<double>>> imageData(
            depth, std::vector<std::vector<double>>(
                height, std::vector<double>(width)));
        
        // The reshaping is effectively taking each slice of the 3D volume
        for (uint32_t z = 0; z < depth; ++z) {
            for (uint32_t y = 0; y < height; ++y) {
                for (uint32_t x = 0; x < width; ++x) {
                    size_t index = z * (width * height) + y * width + x;
                    if (index < buffer.size()) {
                        imageData[z][y][x] = buffer[index];
                    }
                }
            }
        }
        
        data = imageData;
    }
    
    return {frequencySupport, data};
}

void plotSpectralData(const TiffImageData& tiffData) {
    const auto& freqSupport = tiffData.frequencySupport;

    if (std::holds_alternative<std::vector<std::vector<double>>>(tiffData.data)) {
        // 2D data case
        const auto& data = std::get<std::vector<std::vector<double>>>(tiffData.data);

        for (size_t i = 0; i < std::min<size_t>(5, data.size()); i++) {
            plt::plot(freqSupport, data[i], {{"label", "Spectrum " + std::to_string(i + 1)}});
        }
        plt::title("Spectral Data");
        plt::xlabel("Frequency (cm^{-1})");
        plt::ylabel("Intensity");
        plt::legend();
        plt::grid(true);
        plt::show();
    } else {
        // 3D data case
        const auto& data = std::get<std::vector<std::vector<std::vector<double>>>>(tiffData.data);

        for (int i = 0; i < 5 && i < static_cast<int>(data.size()); i++) {
            std::vector<double> intensity;
            for (size_t j = 0; j < freqSupport.size(); j++) {
                intensity.push_back(data[i][0][j]); // Plot from the first row of each depth
            }
            plt::plot(freqSupport, intensity, {{"label", "Position " + std::to_string(i + 1)}});
        }
        plt::title("3D Spectral Data (First 5 Spectra)");
        plt::xlabel("Frequency (cm^{-1})");
        plt::ylabel("Intensity");
        plt::legend();
        plt::grid(true);
        plt::show();
    }
}

void plotTxtData(const std::vector<DataPoint>& data) {
    std::vector<double> x, y;

    // Separate the data into x and y vectors
    for (const auto& point : data) {
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

int main() {
    try {
        // Test loading data from TXT file
        std::vector<DataPoint> txtData = loadDataFromTxt("1_min_batch1.txt");
        std::cout << "Loaded TXT data with " << txtData.size() << " data points." << std::endl;

        // Plot the TXT data
        plotTxtData(txtData);

        const std::string tiffFile = "1_3min_b3_50X50_spectral_mapping_1.tif";

        // List all tags
        std::cout << "Inspecting TIFF file tags..." << std::endl;
        listTiffTags(tiffFile.c_str());

        // Print summary info
        // printTiffInfo(tiffFile.c_str());

        // Test loading data from TIFF file
        TiffImageData tiffData = loadDataFromTiff("1_3min_b3_50X50_spectral_mapping_1.tif");
        std::cout << "Loaded TIFF image with frequency support size: " << tiffData.frequencySupport.size() << std::endl;

        if (std::holds_alternative<std::vector<std::vector<double>>>(tiffData.data)) {
            auto& imageData = std::get<std::vector<std::vector<double>>>(tiffData.data);
            std::cout << "Loaded 2D TIFF image with data size: " << imageData.size() << "x" << imageData[0].size() << std::endl;
        } else {
            auto& imageData = std::get<std::vector<std::vector<std::vector<double>>>>(tiffData.data);
            std::cout << "Loaded 3D TIFF image with data size: " << imageData.size() << "x" << imageData[0].size() << "x" << imageData[0][0].size() << std::endl;
        }

        // Plot the spectral data
        plotSpectralData(tiffData);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}