#ifndef DATALOADER_HPP
#define DATALOADER_HPP

#include <vector>
#include <string>
#include <variant>

struct DataPoint {
    double x;
    double y;

    // Default constructor
    DataPoint() : x(0), y(0) {}

    // Parameterized constructor
    DataPoint(double x, double y) : x(x), y(y) {}
};

struct TiffImageData {
    std::vector<double> frequencySupport;
    std::variant<std::vector<std::vector<double>>, std::vector<std::vector<std::vector<double>>>> data;
};

std::vector<DataPoint> loadDataFromTxt(const std::string& filename);
TiffImageData loadDataFromTiff(const std::string& filename);

#endif // DATALOADER_HPP