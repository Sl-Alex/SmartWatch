#ifndef SM_FONT_H
#define SM_FONT_H

#include "sm_image.h"

/** @addtogroup EXTERNAL_FLASH
 * @{
 *
 * @par Font element
 *
 * Font element holds a group of images.
 * It has a header with symbols count, font height, baseline and an array of offsets and symbols
 * in UCS-4 encoding. All symbol data are located then in the image element format.
 *
 * Symbols itself are not used directly by the firmware. Instead of this, firmware uses its own
 * code table in order to minimize fonts and to make an indexed access to any symbol in the font
 * instead of a search.
 *
 * For example symbol1 from the table below can be letter "A" or "z" or any other symbol. It does not
 * really matter because all conversion is done on the smartphone side, firmware knows nothing about the symbols.
 * It just receives something like symbol1 index and it knows exactly where it is stored.
 * Internal strings used by the firmware are encoded in the same manner before compilation with
 * an external tool(FlashPacker)
 *
 * @todo Write FlashPacker
 *
 * So, font layout in the @ref SmHwStorage is the following:
 *
 * Offset   | Parameter | Size | Description
 * -------- | --------- | ---- | ------
 * 0x0000   | count     | 32   | Image width in pixels
 * 0x0004   | height    | 32   | Image height in pixels
 * 0x0008   | baseLine  | 32   | Image size in bytes
 * 0x000C   | offset1   | 32   | First symbol offset
 * 0x0010   | offset2   | 32   | Second symbol offset
 * ...      | ...       | 32   | ...
 * 0xNNNN   | symbol1   | ---  | Symbol1 (UCS-4)
 * 0xNNNN+4 | symbol2   | ---  | Symbol2 (UCS-4)
 * ...      | ...       | ...  | ...
 * offset1  | image1    | ---  | Symbol1 image
 * offset2  | image2    | ---  | Symbol2 image
 * ...      | ...       | ...  | ...
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
