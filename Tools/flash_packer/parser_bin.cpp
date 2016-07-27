#include "global.h"

bool parseBin(std::ifstream &inFile, FlashElement * element)
{
    // Detect size
    std::streampos begin, end;
    begin = inFile.tellg();
    inFile.seekg(0,std::ios::end);
    end = inFile.tellg();

    inFile.seekg(0, std::ios::beg);

    // Initialize char array
    element->size = end - begin;
    element->data = new char[element->size];

    // Read the whole file
    inFile.read(element->data, element->size);

    return true;
}
