#ifndef FLASHIMAGE_H
#define FLASHIMAGE_H

#include <fstream>
#include "pbm_image.h"

class FlashImage
{
public:
    FlashImage()
        :mWidth(0),
        mHeight(0),
        pData(0),
        mSize(0)
    {}

    ~FlashImage() { if (pData) delete[] pData; }

    /// @brief Load from PBM file. Image width and height will be taken from PBM
    void loadFromPbm(PbmImage * image);

    /// @brief Load from PBM file. Only part of the image will be taken from the PBM file.
    void loadFromPbm(PbmImage * image, uint32_t offsetX, uint32_t offsetY, uint32_t sizeX, uint32_t sizeY);

    /// @brief Get pixel value at the specific coordinates (starting from the left top)
    uint8_t getPixel(uint32_t x, uint32_t y);

    /// @brief Get image data size (in bytes)
    uint32_t getWidth(void) { return mWidth; }
    /// @brief Get image data size (in bytes)
    uint32_t getHeight(void) { return mHeight; }
    /// @brief Get image data size (in bytes)
    uint32_t getSize(void) { return mSize; }

    /// @brief Get image data
    char * getData(void) { return pData; }

private:
    uint32_t mWidth;
    uint32_t mHeight;
    char * pData;
    uint32_t mSize;
};

#endif // FLASHIMAGE_H
