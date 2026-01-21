#include "SpectralPreprocessor.hpp"

#include <algorithm>
#include <numeric>

namespace {

std::vector<double> movingAverage(const std::vector<double>& y, int window) {
    if (window <= 1 || y.size() < 2) return y;

    const int w = (window % 2 == 0) ? (window + 1) : window;
    const int half = w / 2;

    std::vector<double> out;
    out.resize(y.size());

    for (size_t i = 0; i < y.size(); ++i) {
        const size_t start = (i < static_cast<size_t>(half)) ? 0 : (i - static_cast<size_t>(half));
        const size_t end = std::min(y.size() - 1, i + static_cast<size_t>(half));
        double sum = 0.0;
        for (size_t j = start; j <= end; ++j) sum += y[j];
        out[i] = sum / static_cast<double>(end - start + 1);
    }

    return out;
}

std::vector<double> minMaxNormalize(const std::vector<double>& y) {
    if (y.empty()) return y;
    const auto [minIt, maxIt] = std::minmax_element(y.begin(), y.end());
    const double minV = *minIt;
    const double maxV = *maxIt;
    const double denom = maxV - minV;
    if (denom == 0.0) return std::vector<double>(y.size(), 0.0);

    std::vector<double> out;
    out.reserve(y.size());
    for (double v : y) out.push_back((v - minV) / denom);
    return out;
}

std::vector<double> meanNormalize(const std::vector<double>& y) {
    if (y.empty()) return y;
    const double mean = std::accumulate(y.begin(), y.end(), 0.0) / static_cast<double>(y.size());
    if (mean == 0.0) return y;
    std::vector<double> out;
    out.reserve(y.size());
    for (double v : y) out.push_back(v / mean);
    return out;
}

std::vector<double> valueAtIndexNormalize(const std::vector<double>& y, size_t index) {
    if (y.empty()) return y;
    if (index >= y.size()) return y;
    const double denom = y[index];
    if (denom == 0.0) return y;
    std::vector<double> out;
    out.reserve(y.size());
    for (double v : y) out.push_back(v / denom);
    return out;
}

std::vector<DataPoint> withY(const std::vector<DataPoint>& data, const std::vector<double>& y) {
    std::vector<DataPoint> out = data;
    const size_t n = std::min(out.size(), y.size());
    for (size_t i = 0; i < n; ++i) out[i].y = y[i];
    return out;
}

std::vector<double> extractY(const std::vector<DataPoint>& data) {
    std::vector<double> y;
    y.reserve(data.size());
    for (const auto& p : data) y.push_back(p.y);
    return y;
}

} // namespace

std::vector<DataPoint> SpectralPreprocessor::preprocess(
    const std::vector<DataPoint>& data,
    const PreprocessOptions& options) const {
    auto out = data;
    if (options.baseline != BaselineMethod::None) {
        out = baselineCorrect(out, options);
    }
    if (options.smoothing != SmoothingMethod::None) {
        out = smooth(out, options);
    }
    if (options.normalization != NormalizationMethod::None) {
        out = normalize(out, options);
    }
    return out;
}

TiffImageData SpectralPreprocessor::preprocess(
    const TiffImageData& data,
    const PreprocessOptions& options) const {
    auto out = data;
    if (options.baseline != BaselineMethod::None) {
        out = baselineCorrect(out, options);
    }
    if (options.smoothing != SmoothingMethod::None) {
        out = smooth(out, options);
    }
    if (options.normalization != NormalizationMethod::None) {
        out = normalize(out, options);
    }
    return out;
}

std::vector<DataPoint> SpectralPreprocessor::baselineCorrect(
    const std::vector<DataPoint>& data,
    const PreprocessOptions& options) const {
    (void)options;
    return data;
}

TiffImageData SpectralPreprocessor::baselineCorrect(
    const TiffImageData& data,
    const PreprocessOptions& options) const {
    (void)options;
    return data;
}

std::vector<DataPoint> SpectralPreprocessor::smooth(
    const std::vector<DataPoint>& data,
    const PreprocessOptions& options) const {
    switch (options.smoothing) {
        case SmoothingMethod::None:
            return data;
        case SmoothingMethod::MovingAverage: {
            const auto y = extractY(data);
            const auto y2 = movingAverage(y, options.smoothingWindow);
            return withY(data, y2);
        }
        case SmoothingMethod::SavitzkyGolay:
        case SmoothingMethod::Gaussian:
        case SmoothingMethod::Median:
            return data;
    }
    return data;
}

TiffImageData SpectralPreprocessor::smooth(
    const TiffImageData& data,
    const PreprocessOptions& options) const {
    if (options.smoothing == SmoothingMethod::None) return data;

    TiffImageData out = data;
    for (auto& spectrum : out.imageData) {
        switch (options.smoothing) {
            case SmoothingMethod::MovingAverage:
                spectrum = movingAverage(spectrum, options.smoothingWindow);
                break;
            case SmoothingMethod::SavitzkyGolay:
            case SmoothingMethod::Gaussian:
            case SmoothingMethod::Median:
            case SmoothingMethod::None:
                break;
        }
    }
    return out;
}

std::vector<DataPoint> SpectralPreprocessor::normalize(
    const std::vector<DataPoint>& data,
    const PreprocessOptions& options) const {
    switch (options.normalization) {
        case NormalizationMethod::None:
            return data;
        case NormalizationMethod::MinMax: {
            const auto y = extractY(data);
            return withY(data, minMaxNormalize(y));
        }
        case NormalizationMethod::Mean: {
            const auto y = extractY(data);
            return withY(data, meanNormalize(y));
        }
        case NormalizationMethod::ValueAtIndex: {
            const auto y = extractY(data);
            return withY(data, valueAtIndexNormalize(y, options.normalizationIndex));
        }
    }
    return data;
}

TiffImageData SpectralPreprocessor::normalize(
    const TiffImageData& data,
    const PreprocessOptions& options) const {
    if (options.normalization == NormalizationMethod::None) return data;

    TiffImageData out = data;
    for (auto& spectrum : out.imageData) {
        switch (options.normalization) {
            case NormalizationMethod::MinMax:
                spectrum = minMaxNormalize(spectrum);
                break;
            case NormalizationMethod::Mean:
                spectrum = meanNormalize(spectrum);
                break;
            case NormalizationMethod::ValueAtIndex:
                spectrum = valueAtIndexNormalize(spectrum, options.normalizationIndex);
                break;
            case NormalizationMethod::None:
                break;
        }
    }
    return out;
}
