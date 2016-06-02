#include "sm_texture.h"

int SmTexture::init(int width, int height)
{
    // Check and free data pointer
    if (pData)
        delete[] pData;

    // Calculate new parameters
    mWidth = width;
    mHeight = height;

    mSize = calcSize(mWidth, mHeight);

    pData = new char[mSize];
    return mSize;
}

SmTexture::~SmTexture()
{
    delete[] pData;
}

int SmTexture::calcSize(int width, int height)
{
#if (BPP == 8)
    return width * height;
#elif (BPP == 1)
#ifdef PACK_VERT
    return width * ((height + 7)/8); // ceil() for height
#else
    return ((width + 7)/8) * height; // ceil() for width
#endif
#endif
}
