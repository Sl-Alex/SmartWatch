#include <vector>
#include <iostream>
#include "global.h"
#include "pbm_image.h"
#include "flash_image.h"

bool parseFont(std::ifstream &inFileTxt, std::ifstream &inFilePbm, FlashElement * element, FontTable * table)
{
    element->size = 0;

    // These font parameters will be stored in a flash element
    uint32_t symbolsCount = 0;
    uint32_t height = 0;
    uint32_t baseLine = 0;

    // Detect file size
    std::streampos begin, end;
    begin = inFileTxt.tellg();
    inFileTxt.seekg(0,std::ios::end);
    end = inFileTxt.tellg();

    // Reset text file to the beginning
    inFileTxt.seekg(0, std::ios::beg);

    // Check for the number of bytes, it must be even
    if (((end - begin) % 2) != 0)
        return false;

    // Calculate symbol count
    symbolsCount = (end - begin)/2;

    // Load original PBM image
    PbmImage fontImage;
    if (fontImage.loadFromFile(&inFilePbm) == false)
    {
        return false;
    }

    // Get font height
    height = fontImage.getHeight() - 1;

    // Get font baseline (it is the black pixel position in the first column)
    for (uint32_t i = 0; i < fontImage.getHeight(); ++i)
    {
        if (fontImage.getPixel(0,i) != 0)
            baseLine = i - 1;
    }

    uint32_t sym_start = 1;
    uint32_t sym_end = sym_start;

    std::vector<FlashImage *> imageVector;
    // Parse symbols from the PBM file
    while (true)
    {
        // Search for the symbol delimiter (black pixel in the first row)
        sym_end = UINT32_MAX;
        for (uint32_t i = sym_start; i < fontImage.getWidth(); ++i)
        {
            if (fontImage.getPixel(i, 0))
            {
                sym_end = i - 1;
                break;
            }
        }
        if (sym_end == UINT32_MAX)
        {
            sym_end = fontImage.getWidth() - 1;
        }

        FlashImage * symbolImage = new FlashImage();
        symbolImage->loadFromPbm(&fontImage, sym_start, 1, sym_end - sym_start + 1, height);
        imageVector.push_back(symbolImage);

        //symbolsCount++;
        sym_start = sym_end + 2;

        if (sym_start >= fontImage.getWidth())
            break;
    }
    if (symbolsCount != imageVector.size())
    {
        std::cout << "Text file contains " << symbolsCount << " symbols, but image file contains " << imageVector.size() << std::endl;
        for (auto it = imageVector.begin(); it != imageVector.end(); ++it)
        {
            delete *it;
        }
    }

    // Calculate all offsets
    uint32_t offsetOffset = sizeof (symbolsCount) +
            sizeof(height) +
            sizeof(baseLine);
    uint32_t symbolOffset = offsetOffset + sizeof(uint32_t)*symbolsCount;
    uint32_t dataOffset = symbolOffset + sizeof(uint16_t)*symbolsCount;

    // Calculate total element size
    element->size = dataOffset;
    for (auto it = imageVector.begin(); it != imageVector.end(); ++it)
    {
        element->size += 3*sizeof(uint32_t);
        element->size += (*it)->getSize();
    }

    // Reserve complete memory
    element->data = new char[element->size];

    // Write header
    *((uint32_t *)(element->data)) = symbolsCount;
    *((uint32_t *)(element->data + sizeof(uint32_t))) = height;
    *((uint32_t *)(element->data + 2*sizeof(uint32_t))) = baseLine;

    // Write offsets, symbols and symbols data
    for (auto it = imageVector.begin(); it != imageVector.end(); ++it)
    {
        FlashImage * image = *it;

        // Write offset
        memcpy(element->data + offsetOffset, &dataOffset, sizeof(dataOffset));
        offsetOffset += sizeof(uint32_t);
        // Write data
        uint32_t imWidth, imHeight, imSize;
        imWidth = image->getWidth();
        imHeight = image->getHeight();
        imSize = image->getSize();
        memcpy(element->data + dataOffset, &imWidth, sizeof(uint32_t));
        memcpy(element->data + dataOffset + sizeof(uint32_t), &imHeight, sizeof(uint32_t));
        memcpy(element->data + dataOffset + 2*sizeof(uint32_t), &imSize, sizeof(uint32_t));
        memcpy(element->data + dataOffset + 3*sizeof(uint32_t), image->getData(), imSize);
        // Calculate ext offset
        dataOffset += image->getSize() + 3*sizeof(uint32_t);

        // delete FlashImage
        delete image;
    }
    // Write symbols
    table->count = symbolsCount;
    table->symbols = new uint16_t[symbolsCount];
    uint32_t num = 0;
    for (auto it = imageVector.begin(); it != imageVector.end(); ++it)
    {
        uint16_t symbol;
        inFileTxt.read((char *)&symbol, sizeof(symbol));
        // Store symbol
        table->symbols[num++] = symbol;
        memcpy(element->data + symbolOffset, &symbol, sizeof(symbol));
        symbolOffset += sizeof(symbol);
    }
    imageVector.clear();

    // Font layout in flash
    // 0x0000   | count     | 32   | Number of symbols in font
    // 0x0004   | height    | 32   | Font height
    // 0x0008   | baseLine  | 32   | Font base line
    // 0x000C   | offset1   | 32   | First symbol offset
    // 0x0010   | offset2   | 32   | Second symbol offset
    // ...      | ...       | 32   | ...
    // 0xNNNN   | symbol1   | 16   | Symbol1 (UCS-2LE)
    // 0xNNNN+2 | symbol2   | 16   | Symbol2 (UCS-2LE)
    // ...      | ...       | ...  | ...
    // offset1  | image1    | ---  | Symbol1 image (see parser_pbm for image layout)
    // offset2  | image2    | ---  | Symbol2 image
    // ...      | ...       | ...  | ...

    return true;
}
