#ifndef GLOBAL_H
#define GLOBAL_H

#include <fstream>
#include <vector>

struct FlashElement{
    uint32_t size;
    char * data;
};

struct FontTable{
    uint32_t index;
    uint32_t count;
    uint16_t * symbols;
};

/// @brief Read the whole file info flash element
bool parseBin(std::ifstream &inFile, FlashElement * element);
/// @brief Right now is the same as parseBin()
bool parseTxt(std::ifstream &inFile, FlashElement * element);
/// @brief Parse PBM image file and convert to the device image format
bool parsePbm(std::ifstream &inFile, FlashElement * element);
/// @brief Parse font files and create device font format
bool parseFont(std::ifstream &inFileTxt, std::ifstream &inFilePbm, FlashElement * element, FontTable * table);
/// @brief Process sm_strings template file and produce sm_strings.h
void sm_strings_process(std::ifstream &inFile, std::ofstream &outFile, std::vector<FontTable *> fontList);

/// @brief Calculate CRC32
uint32_t calcCRC32(uint32_t init, const char * data, uint32_t size);

#endif // GLOBAL_H
