#pragma once
#include <string>
#include <vector>

struct DataPoint {
    double x;
    double y;
};

struct TiffImageData {
    std::vector<double> frequencySupport;           // length = depth used
    std::vector<std::vector<double>> imageData;     // [width*height][depth]
};

class SpectralIO {
public:
    // TXT
    std::vector<DataPoint> loadTxt(const std::string& path) const;

    // TIFF
    TiffImageData loadTiff(const std::string& path) const;
    int           countTiffPages(const std::string& path) const;
    void          checkCompression(const std::string& path) const;

    // Export
    void writeImageDataToTxt(const std::string& path,
                             const TiffImageData& data) const;

private:
    static bool isFileAccessible(const std::string& path);
    static std::vector<double> parseXAxisFromFile(const std::string& path);
};