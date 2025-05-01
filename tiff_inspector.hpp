/**
 * @file tiff_inspector.h
 * @brief Header file for TIFF file inspection utilities
 * 
 * This file contains utility functions for inspecting and analyzing
 * TIFF file structures, tags, and metadata.
 */

 #ifndef TIFF_INSPECTOR_H
 #define TIFF_INSPECTOR_H
 
 #include <string>
 
 /**
  * @brief Lists all tags in a TIFF file with detailed information
  * 
  * This function opens a TIFF file and prints detailed information about
  * all tags found in the file, including tag IDs, names, data types,
  * value counts, and actual values where possible.
  * 
  * @param filename Path to the TIFF file to inspect
  */
 void listTiffTags(const char* filename);
 
 /**
  * @brief Overloaded version that accepts std::string
  * 
  * @param filename Path to the TIFF file to inspect as std::string
  */
 inline void listTiffTags(const std::string& filename) {
     listTiffTags(filename.c_str());
 }
 
 /**
  * @brief Prints basic TIFF file information
  * 
  * Displays basic metadata about a TIFF file including dimensions,
  * bit depth, compression type, and other fundamental properties.
  * 
  * @param filename Path to the TIFF file
  * @return bool True if successful, false otherwise
  */
 bool printTiffInfo(const char* filename);
 
 /**
  * @brief Overloaded version that accepts std::string
  * 
  * @param filename Path to the TIFF file as std::string
  * @return bool True if successful, false otherwise
  */
 inline bool printTiffInfo(const std::string& filename) {
     return printTiffInfo(filename.c_str());
 }
 
 /**
  * @brief Tests if a specific TIFF tag exists in a file
  * 
  * @param filename Path to the TIFF file
  * @param tagId The ID of the tag to check for
  * @return bool True if the tag exists, false otherwise
  */
 bool hasTiffTag(const char* filename, uint32_t tagId);
 
 /**
  * @brief Overloaded version that accepts std::string
  * 
  * @param filename Path to the TIFF file as std::string
  * @param tagId The ID of the tag to check for
  * @return bool True if the tag exists, false otherwise
  */
 inline bool hasTiffTag(const std::string& filename, uint32_t tagId) {
     return hasTiffTag(filename.c_str(), tagId);
 }
 
 #endif // TIFF_INSPECTOR_H