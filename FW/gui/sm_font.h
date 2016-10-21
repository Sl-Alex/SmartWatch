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
 * in UCS-2 encoding. All symbol data are located then in the image element format.
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
 * So, font layout in the @ref SmHwStorage is the following:
 *
 * 0x0000   | count     | 32   | Number of symbols in font
 * 0x0004   | height    | 32   | Font height
 * 0x0008   | baseLine  | 32   | Font base line
 * 0x000C   | spacing   | 32   | Letter spacing (pixels)
 * 0x0010   | offset1   | 32   | First symbol offset
 * 0x0014   | offset2   | 32   | Second symbol offset
 * ...      | ...       | 32   | ...
 * 0xNNNN   | symbol1   | 16   | Symbol1 (UCS-2LE)
 * 0xNNNN+2 | symbol2   | 16   | Symbol2 (UCS-2LE)
 * ...      | ...       | ...  | ...
 * offset1  | image1    | ---  | Symbol1 image (see image element description)
 * offset2  | image2    | ---  | Symbol2 image
 * ...      | ...       | ...  | ...

 *
 * @}
 *
 */

/// @brief Font class
class SmFont: public SmImage
{
public:

    /// @brief Initialize font by the index (see @ref SmHwStorageIndices)
    bool init(int index);

    /// @brief Draw symbol from font on a canvas
    /// @params canvas: Canvas to draw on
    /// @params x: X coordinate on the canvas
    /// @params y: Canvas to draw on the canvas
    /// @params symbol: Symbol to output (symbol from the font code table)
    void drawSymbol(SmCanvas * canvas, int x, int y, uint16_t symbol);

    /// @brief Output text on a canvas
    /// @params canvas: Canvas to draw on
    /// @params x: X coordinate on the canvas
    /// @params y: Y coordinate on the canvas
    /// @params text: Pointer to the text to output
    /// @params count: Number of symbols to output
    void drawText(SmCanvas * canvas, int x, int y, uint16_t * text, uint16_t count);

    /// @brief Output ASCII text (symbols between 0x20 and 0x7E)
    void drawText(SmCanvas * canvas, int x, int y, char * text);

    /// @brief Get symbol width
    uint32_t getSymbolWidth(uint16_t symbol);
    /// @brief Get text width
    uint32_t getStringWidth(uint16_t * text, uint16_t count);
    /// @brief Get ASCII text width
    uint32_t getStringWidth(char * text);
private:
    uint32_t mFontOffset;   ///< Start address of the font element in the external flash
    // These parameters are stored in external flash
    uint32_t mSymbolCount;
    uint32_t mFontHeight;
    uint32_t mBaseLine;
    uint32_t mSpacing;
    // These parameters can be calculated from the parameters above
    uint32_t mDataTableOffset;
    uint32_t mSymbolTableOffset;
};

#endif // SM_FONT_H
