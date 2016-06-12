#ifndef SM_TEXTURE_H_INCLUDED
#define SM_TEXTURE_H_INCLUDED

#include <cstring>
#include <cstdint>

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

    inline uint8_t * getPData(void) { return pData;}

private:
    int mWidth;
    int mHeight;
    int mSize;

    uint8_t * pData;
};

#endif /* SM_TEXTURE_H_INCLUDED */
