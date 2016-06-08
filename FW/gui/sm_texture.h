#ifndef SM_TEXTURE_H_INCLUDED
#define SM_TEXTURE_H_INCLUDED

#include <string.h>

class SmTexture
{
public:
    SmTexture()
        :mWidth(0),
        mHeight(0),
        mSize(0),
        pData(0) {}

    /// @brief Allocate memory and init parameters
    virtual int init(int width, int height);
    /// @brief Destructor
    virtual ~SmTexture();

    /// @brief Clears texture memory
    void clear(void) { memset(pData, 0, mSize); }

    inline int getWidth(void) { return mWidth; }
    inline int getHeight(void) { return mHeight; }
    inline int getSize(void) { return mSize; }

    static int calcSize(int width, int height);

    inline char * getPData(void) { return pData;}

private:
    int mWidth;
    int mHeight;
    int mSize;

    char * pData;
};

#endif /* SM_TEXTURE_H_INCLUDED */
