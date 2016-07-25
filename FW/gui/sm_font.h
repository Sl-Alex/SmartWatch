#ifndef SM_FONT_H
#define SM_FONT_H

#include "sm_image.h"

/** @addtogroup EXTERNAL_FLASH
 * @{
 *
 * @par Font element
 *
 * Font element holds a group of images.
 * It has a header with symbols count, font height, baseline and an array of offsets for
 * each symbol. Each symbol has an image element layout.
 * So, font layout in the @ref SmHwStorage is the following:
 *
 * Offset  | Parameter | Size | Description
 * ------- | --------- | ---- | ------
 * 0x0000  | count     | 32   | Image width in pixels
 * 0x0004  | height    | 32   | Image height in pixels
 * 0x0008  | baseLine  | 32   | Image size in bytes
 * 0x000C  | offset1   | 32   | First symbol offset
 * 0x0010  | offset2   | 32   | Second symbol offset
 * ...     | ...       | 32   | ...
 * offset1 | symbol1   | ---  | Symbol1 image
 * offset2 | symbol2   | ---  | Symbol2 image
 * ...     | ...       | ...  | ...
 *
 * @}
 *
 */

/// @brief Font class
/// @todo Implement properly
class SmFont: public SmImage
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
