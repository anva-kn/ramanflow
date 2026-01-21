#include "SpectralPlotter.hpp"
#include "matplotlibcpp.h"
#include <algorithm>
#include <cstdlib>
#include <string>

namespace plt = matplotlibcpp;

void SpectralPlotter::plotSpectralData(const TiffImageData& tiffData, size_t maxSpectra) const {
    plt::figure();
    const auto& freq = tiffData.frequencySupport;
    const auto& data = tiffData.imageData;
    const size_t n = std::min(maxSpectra, data.size());
    for (size_t i = 0; i < n; ++i) {
        plt::plot(freq, data[i], {{"label", "Spectrum " + std::to_string(i+1)}});
    }
    plt::title("Spectral Data");
    plt::xlabel("Frequency (cm^{-1})");
    plt::ylabel("Intensity");
    plt::legend();
    plt::grid(true);

    if (const char* out = std::getenv("RAMANFLOW_PLOT_SAVE")) {
        if (*out) plt::save(out);
    }
    if (std::getenv("RAMANFLOW_PLOT_NOSHOW") == nullptr) {
        plt::show();
    }
}

void SpectralPlotter::plotSpectralComparison(const TiffImageData& raw,
                                             const TiffImageData& processed,
                                             size_t maxSpectra) const {
    plt::figure();
    const auto& freq = raw.frequencySupport;
    const auto& rawData = raw.imageData;
    const auto& processedData = processed.imageData;

    const size_t n = std::min({maxSpectra, rawData.size(), processedData.size()});

    for (size_t i = 0; i < n; ++i) {
        plt::named_plot("Raw " + std::to_string(i + 1), freq, rawData[i], "--");
        plt::named_plot("Processed " + std::to_string(i + 1), freq, processedData[i], "-");
    }

    plt::title("Spectral Data (Raw vs Processed)");
    plt::xlabel("Frequency (cm^{-1})");
    plt::ylabel("Intensity");
    plt::legend();
    plt::grid(true);

    if (const char* out = std::getenv("RAMANFLOW_PLOT_SAVE")) {
        if (*out) plt::save(out);
    }
    if (std::getenv("RAMANFLOW_PLOT_NOSHOW") == nullptr) {
        plt::show();
    }
}

void SpectralPlotter::plotTxtData(const std::vector<DataPoint>& data) const {
    plt::figure();
    std::vector<double> x, y;
    x.reserve(data.size()); y.reserve(data.size());
    for (const auto& p : data) { x.push_back(p.x); y.push_back(p.y); }
    plt::plot(x, y, {{"label", "TXT Data"}});
    plt::title("TXT Data Plot");
    plt::xlabel("X-axis");
    plt::ylabel("Y-axis");
    plt::legend();
    plt::grid(true);

    if (const char* out = std::getenv("RAMANFLOW_PLOT_SAVE")) {
        if (*out) plt::save(out);
    }
    if (std::getenv("RAMANFLOW_PLOT_NOSHOW") == nullptr) {
        plt::show();
    }
}
