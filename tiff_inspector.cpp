/**
 * @file tiff_inspector.cpp
 * @brief Implementation of TIFF file inspection utilities
 */

#include "tiff_inspector.hpp"
#include <tiffio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>

void listTiffTags(const char* filename) {
    TIFF* tiff = TIFFOpen(filename, "r");
    if (!tiff) {
        std::cerr << "Could not open " << filename << std::endl;
        return;
    }

    std::cout << "Tags in file " << filename << ":" << std::endl;
    std::cout << std::left << std::setw(10) << "Tag ID"
              << std::left << std::setw(30) << "Tag Name"
              << std::left << std::setw(15) << "Data Type"
              << std::left << std::setw(10) << "Count"
              << "Value" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    for (uint32_t tagId = 0; tagId < 65535; ++tagId) {
        const TIFFField* field = TIFFFieldWithTag(tiff, tagId);
        if (!field) continue;

        const char* tagName = TIFFFieldName(field);
        TIFFDataType dataType = TIFFFieldDataType(field);

        std::cout << std::left << std::setw(10) << tagId
                  << std::left << std::setw(30) << (tagName ? tagName : "Unknown")
                  << std::left << std::setw(15) << dataType
                  << std::endl;
    }

    TIFFClose(tiff);
}

bool printTiffInfo(const char* filename) {
    TIFF* tiff = TIFFOpen(filename, "r");
    if (!tiff) {
        std::cerr << "Could not open " << filename << std::endl;
        return false;
    }
    
    std::cout << "TIFF File: " << filename << std::endl;
    std::cout << "------------------------" << std::endl;
    
    // Basic dimensions
    uint32_t width = 0, height = 0, depth = 1;
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tiff, TIFFTAG_IMAGEDEPTH, &depth);
    
    std::cout << "Dimensions: " << width << " x " << height;
    if (depth > 1) {
        std::cout << " x " << depth;
    }
    std::cout << std::endl;
    
    // Bits and samples
    uint16_t bitsPerSample = 0, samplesPerPixel = 0, sampleFormat = 0;
    TIFFGetFieldDefaulted(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    TIFFGetFieldDefaulted(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetFieldDefaulted(tiff, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
    
    std::cout << "Bits per sample: " << bitsPerSample << std::endl;
    std::cout << "Samples per pixel: " << samplesPerPixel << std::endl;
    
    // Sample format
    std::cout << "Sample format: ";
    switch (sampleFormat) {
        case SAMPLEFORMAT_UINT: std::cout << "Unsigned integer"; break;
        case SAMPLEFORMAT_INT: std::cout << "Signed integer"; break;
        case SAMPLEFORMAT_IEEEFP: std::cout << "IEEE floating point"; break;
        case SAMPLEFORMAT_VOID: std::cout << "Void (untyped data)"; break;
        case SAMPLEFORMAT_COMPLEXINT: std::cout << "Complex signed integer"; break;
        case SAMPLEFORMAT_COMPLEXIEEEFP: std::cout << "Complex IEEE floating point"; break;
        default: std::cout << "Unknown (" << sampleFormat << ")"; break;
    }
    std::cout << std::endl;
    
    // Compression
    uint16_t compression = 0;
    TIFFGetFieldDefaulted(tiff, TIFFTAG_COMPRESSION, &compression);
    std::cout << "Compression: ";
    switch (compression) {
        case COMPRESSION_NONE: std::cout << "None"; break;
        case COMPRESSION_LZW: std::cout << "LZW"; break;
        case COMPRESSION_JPEG: std::cout << "JPEG"; break;
        case COMPRESSION_PACKBITS: std::cout << "PackBits"; break;
        case COMPRESSION_DEFLATE: std::cout << "Deflate"; break;
        default: std::cout << "Other (" << compression << ")"; break;
    }
    std::cout << std::endl;
    
    // Photometric interpretation
    uint16_t photometric = 0;
    TIFFGetFieldDefaulted(tiff, TIFFTAG_PHOTOMETRIC, &photometric);
    std::cout << "Photometric: ";
    switch (photometric) {
        case PHOTOMETRIC_MINISWHITE: std::cout << "Min-is-white"; break;
        case PHOTOMETRIC_MINISBLACK: std::cout << "Min-is-black"; break;
        case PHOTOMETRIC_RGB: std::cout << "RGB"; break;
        case PHOTOMETRIC_PALETTE: std::cout << "Palette color"; break;
        case PHOTOMETRIC_MASK: std::cout << "Transparency mask"; break;
        case PHOTOMETRIC_SEPARATED: std::cout << "Separated (e.g. CMYK)"; break;
        case PHOTOMETRIC_YCBCR: std::cout << "YCbCr"; break;
        case PHOTOMETRIC_CIELAB: std::cout << "CIE L*a*b*"; break;
        case PHOTOMETRIC_ICCLAB: std::cout << "ICC L*a*b*"; break;
        case PHOTOMETRIC_ITULAB: std::cout << "ITU L*a*b*"; break;
        case PHOTOMETRIC_LOGL: std::cout << "CIE Log L"; break;
        case PHOTOMETRIC_LOGLUV: std::cout << "CIE Log L, u, v"; break;
        default: std::cout << "Other (" << photometric << ")"; break;
    }
    std::cout << std::endl;
    
    // Check if the file has embedded metadata with XAxis tag
    bool hasXAxisMetadata = false;
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("<XAxis>") != std::string::npos && line.find("<End>") != std::string::npos) {
                hasXAxisMetadata = true;
                break;
            }
        }
    }
    
    std::cout << "Contains <XAxis> metadata: " << (hasXAxisMetadata ? "Yes" : "No") << std::endl;
    
    TIFFClose(tiff);
    return true;
}