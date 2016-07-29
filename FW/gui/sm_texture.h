#ifndef SM_TEXTURE_H_INCLUDED
#define SM_TEXTURE_H_INCLUDED

#include <cstring>
#include <cstdint>

/// @brief Texture in the native image format
/// @todo Write more about native image format
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
    void fill(uint8_t sample) { memset(pData, sample, mSize); }

    /// @brief Get texture width
    inline int getWidth(void) { return mWidth; }
    /// @brief Get texture height
    inline int getHeight(void) { return mHeight; }
    /// @brief Get texture size
    inline int getSize(void) { return mSize; }

    /// @brief Calc size
    /// @todo Do we really need this function?
    static int calcSize(int width, int height);
    
    /// @brief Get a pointer to the texture data
    inline uint8_t * getPData(void) { return pData;}

private:
    int mWidth;
    int mHeight;
    int mSize;

    uint8_t * pData;
};

#endif /* SM_TEXTURE_H_INCLUDED */
