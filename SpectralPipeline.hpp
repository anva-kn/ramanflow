#pragma once
#include "SpectralIO.hpp"
#include <iostream>

class SpectralPipeline {
public:
    explicit SpectralPipeline(SpectralIO io = {}) : io_(std::move(io)) {}

    void process(const std::string& tiffFile,
                 const std::string& outTxtFile) {
        const int pages = io_.countTiffPages(tiffFile);
        std::cout << "TIFF has " << pages << " pages.\n";

        auto tiffData = io_.loadTiff(tiffFile);
        std::cout << "Loaded " << tiffData.imageData.size()
                  << " spectra × " << tiffData.frequencySupport.size() << " depth.\n";

        io_.writeImageDataToTxt(outTxtFile, tiffData);
    }

private:
    SpectralIO io_;
};
