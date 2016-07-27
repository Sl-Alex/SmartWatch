#include "flash_image.h"
#include <cstring>

void FlashImage::loadFromPbm(PbmImage * image)
{
    mWidth = image->getWidth();
    mHeight = image->getHeight();

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
            if (image->getPixel(x, y))
                pData[mWidth*(y/8) + x] |= 0x01 << y % 8;
        }
    }
}
