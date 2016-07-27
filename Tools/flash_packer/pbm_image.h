#ifndef PBMIMAGE_H
#define PBMIMAGE_H

#include <fstream>

class PbmImage
{
public:
    PbmImage()
        :mWidth(0),
        mHeight(0),
        pData(0),
        mSize(0)
    {}

    ~PbmImage() {if (pData) delete[] pData;}

    /// @brief Load image from ifstream
    bool loadFromFile(std::ifstream * fs);

    /// @brief Get pixel value at the specific coordinates (starting from the left top)
    uint8_t getPixel(uint32_t x, uint32_t y);

    uint32_t getWidth(void) { return mWidth; }
    uint32_t getHeight(void) { return mHeight; }

private:
    uint32_t mWidth;
    uint32_t mHeight;
    char * pData;
    uint32_t mSize;
};

#endif // PBMIMAGE_H
