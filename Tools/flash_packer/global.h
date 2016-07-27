#ifndef GLOBAL_H
#define GLOBAL_H

#include <fstream>

struct FlashElement{
    uint32_t size;
    char * data;
};

/// @brief Read the whole file info flash element
bool parseBin(std::ifstream &inFile, FlashElement * element);

bool parseTxt(std::ifstream &inFile, FlashElement * element);

bool parsePbm(std::ifstream &inFile, FlashElement * element);

bool parseFont(std::ifstream &inFileTxt, std::ifstream &inFilePbm, FlashElement * element);

#endif // GLOBAL_H
