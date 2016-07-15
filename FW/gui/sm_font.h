#ifndef SM_FONT_H
#define SM_FONT_H

#include "sm_image.h"

/// Font layout in memory:
/// uint32_t size
/// uint32_t count
/// uint32_t height
/// uint32_t baseLine
/// SymbolProps[count] {
///     uint32_t symbol;
///     uint8_t width;
/// }
/// uint8_t data[];

struct SmFontSymbol {
    uint32_t symbol;    ///< UTF-32 encoding
    uint8_t width;      ///< width (pixels)
};

class SmFont: SmImage
{
public:
    static bool loadFont(void);
    static int drawSymbol(SmCanvas * canvas, int x, int y, char symbol);
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
