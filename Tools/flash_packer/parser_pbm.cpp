#include "global.h"
#include <iostream>
#include "pbm_image.h"
#include "flash_image.h"

/// @todo Implement
bool parsePbm(std::ifstream &inFile, FlashElement * element)
{
    element->size = 0;
    element->data = 0;

    // Load original PBM image
    PbmImage orig_image;
    if (orig_image.loadFromFile(&inFile) == false)
    {
        return false;
    }

    FlashImage conv_image;
    conv_image.loadFromPbm(&orig_image);

    // Image layout in flash
    // Offset       | Parameter | Size  | Description
    // ------------ | --------- | ----- | ------
    // 0x0000       | width     | 32    | Image width in pixels
    // 0x0004       | height    | 32    | Image height in pixels
    // 0x0008       | size      | 32    | Image size in bytes

    // Create a new array
    element->size = sizeof(uint32_t) * 3 + conv_image.getSize();
    char * pData = new char[element->size];
    // Fill flash image header
    *((uint32_t *)(pData)) = orig_image.getWidth();
    *((uint32_t *)(pData + sizeof(uint32_t))) = orig_image.getHeight();
    *((uint32_t *)(pData + 2*sizeof(uint32_t))) = conv_image.getSize();
    // Fill flash image data
    memcpy(pData + 3*sizeof(uint32_t), conv_image.getData(), conv_image.getSize());

    element->data = pData;
    return true;
}
