#pragma once

#include "SpectralIO.hpp"
#include <cstddef>
#include <vector>

enum class BaselineMethod {
    None,
    Polynomial,
    AsymmetricLeastSquares,
};

enum class SmoothingMethod {
    None,
    SavitzkyGolay,
    MovingAverage,
    Gaussian,
    Median,
};

enum class NormalizationMethod {
    None,
    MinMax,
    Mean,
    ValueAtIndex,
};

struct PreprocessOptions {
    BaselineMethod baseline = BaselineMethod::None;
    SmoothingMethod smoothing = SmoothingMethod::None;
    NormalizationMethod normalization = NormalizationMethod::None;

    int smoothingWindow = 7;
    int polynomialOrder = 3;
    double alsLambda = 1e5;
    double alsP = 0.01;
    size_t normalizationIndex = 0;
};

class SpectralPreprocessor {
public:
    std::vector<DataPoint> preprocess(const std::vector<DataPoint>& data,
                                      const PreprocessOptions& options = {}) const;

    TiffImageData preprocess(const TiffImageData& data,
                             const PreprocessOptions& options = {}) const;

    std::vector<DataPoint> baselineCorrect(const std::vector<DataPoint>& data,
                                           const PreprocessOptions& options) const;

    TiffImageData baselineCorrect(const TiffImageData& data,
                                  const PreprocessOptions& options) const;

    std::vector<DataPoint> smooth(const std::vector<DataPoint>& data,
                                  const PreprocessOptions& options) const;

    TiffImageData smooth(const TiffImageData& data,
                         const PreprocessOptions& options) const;

    std::vector<DataPoint> normalize(const std::vector<DataPoint>& data,
                                     const PreprocessOptions& options) const;

    TiffImageData normalize(const TiffImageData& data,
                            const PreprocessOptions& options) const;
};
