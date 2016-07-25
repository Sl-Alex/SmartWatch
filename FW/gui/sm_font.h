#ifndef SM_FONT_H
#define SM_FONT_H

#include "sm_image.h"

/// Font layout in memory:
/// uint32_t count
/// uint32_t height
/// uint32_t baseLine
/// SymbolProps[count] {
///     uint32_t symbol;
///     uint8_t width;
///     uint32_t size;
///     uint32_t offset;
/// }
/// uint8_t data[]; /// @offset starting from the font beginning

struct SmFontSymbol {
    uint32_t symbol;    ///< UTF-32 encoding
    uint8_t width;      ///< width (pixels)
};

class SmFont: public SmCanvas
{
public:
    /// @brief Draw symbol from font on a canvas
    /// @details index: Font index (See @ref SmHwStorageIndices)
    static int drawSymbol(int index, SmCanvas * canvas, int x, int y, uint32_t symbol);
    static int drawText(SmCanvas * canvas, int x, int y, char * symbol);
    static inline SmFont * getInstance()
    {
        if(!pInstance)
        {
            pInstance = new SmFont();
        }
        return pInstance;
    }
private:
    static SmFont * pInstance;
    //Symbol
    SmFont(){}
    SmFont(const SmFont&);
    SmFont& operator=(SmFont&);
};

#endif // SM_FONT_H
