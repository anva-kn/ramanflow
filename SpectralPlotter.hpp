#pragma once
#include "SpectralIO.hpp"

class SpectralPlotter {
public:
    void plotSpectralData(const TiffImageData& tiffData, size_t maxSpectra = 5) const;
    void plotSpectralComparison(const TiffImageData& raw,
                                const TiffImageData& processed,
                                size_t maxSpectra = 5) const;
    void plotTxtData(const std::vector<DataPoint>& data) const;
};
