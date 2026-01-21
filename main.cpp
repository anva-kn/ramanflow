#include "SpectralIO.hpp"
#include "SpectralPipeline.hpp"
#include "SpectralPlotter.hpp"
#include "SpectralPreprocessor.hpp"
#include <iostream>
#include <tiffio.h>
#include <cstdarg>
#include <cstdlib>
#include <filesystem>
#include <string>

// --- Custom warning handler to suppress only null-byte warnings ---
void selectiveTiffWarningHandler(const char* module, const char* fmt, va_list ap) {
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, ap);

    std::string msg(buffer);
    if (msg.find("does not end in null byte") != std::string::npos) {
        // Skip these specific warnings
        return;
    }

    // Otherwise, print normally
    if (module)
        std::cerr << module << ": " << msg;
    else
        std::cerr << msg;
}

static void ensureMatplotlibConfigDir() {
    if (std::getenv("MPLCONFIGDIR") != nullptr) return;

    std::error_code ec;
    const auto dir = std::filesystem::temp_directory_path(ec) / "ramanflow-mplconfig";
    if (ec) return;

    std::filesystem::create_directories(dir, ec);
    if (ec) return;

    const std::string dirStr = dir.string();
    setenv("MPLCONFIGDIR", dirStr.c_str(), 1);
}

int main() {
    // Set our selective handler for warnings
    TIFFSetWarningHandler(selectiveTiffWarningHandler);
    // Keep default error handler so errors are still shown
    // (TIFFSetErrorHandler(nullptr) would disable errors completely)

    try {
        ensureMatplotlibConfigDir();

        SpectralIO io;
        SpectralPipeline pipeline(io);

        // TXT example (optional)
        auto txt = io.loadTxt("1_min_batch1.txt");
        std::cout << "TXT points: " << txt.size() << "\n";

        // TIFF → TXT export
        pipeline.process("1_3min_b3_50X50_spectral_mapping_1.tif",
                         "imageData_v3.txt");

        // Plot
        SpectralPlotter plotter;
        auto tiffData = io.loadTiff("1_3min_b3_50X50_spectral_mapping_1.tif");
        SpectralPreprocessor preprocessor;
        PreprocessOptions preprocessOptions;
        preprocessOptions.smoothing = SmoothingMethod::MovingAverage;
        preprocessOptions.smoothingWindow = 7;
        preprocessOptions.normalization = NormalizationMethod::MinMax;

        auto processedTiffData = preprocessor.preprocess(tiffData, preprocessOptions);
        plotter.plotSpectralData(processedTiffData);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
