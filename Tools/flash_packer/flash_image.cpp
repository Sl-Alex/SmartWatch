#include "flash_image.h"
#include <cstring>

void FlashImage::loadFromPbm(PbmImage * image)
{
    loadFromPbm(image, 0, 0, image->getWidth(), image->getHeight());
}

void FlashImage::loadFromPbm(PbmImage * image, uint32_t offsetX, uint32_t offsetY, uint32_t sizeX, uint32_t sizeY)
{
    mWidth = sizeX;
    mHeight = sizeY;

    // Calculate size for vertical packing, 1BPP
    mSize = mWidth*((mHeight + 7)/8);

    if (pData)
        delete[] pData;

    pData = new char[mSize];
    memset(pData, 0, mSize);

    // This conversion is only for the vertical packing, 1BPP
    for (uint32_t x = 0; x < mWidth; ++x)
    {
        for (uint32_t y = 0; y < mHeight; ++y)
        {
            if (image->getPixel(offsetX + x, offsetY + y))
                pData[mWidth*(y/8) + x] |= 0x01 << y % 8;
        }
    }
}
