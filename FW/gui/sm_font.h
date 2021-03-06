#ifndef SM_FONT_H
#define SM_FONT_H

#include "sm_image.h"
#include "sm_text.h"

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
 * Offset   | Parameter | Size | Description
 * ---------|-----------|------|-------------------------
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
 * offset1  | image1    | var. | Symbol1 image (see image element description)
 * offset2  | image2    | var. | Symbol2 image
 * ...      | ...       | ...  | ...
 *
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
    /// @param canvas: Canvas to draw on
    /// @param x: X coordinate on the canvas
    /// @param y: Canvas to draw on the canvas
    /// @param symbol: Symbol to output (symbol from the font code table)
    void drawSymbol(SmCanvas * canvas, int x, int y, uint16_t symbol);

    /// @brief Output text on a canvas
    /// @param canvas: Canvas to draw on
    /// @param x: X coordinate on the canvas
    /// @param y: Y coordinate on the canvas
    /// @param text: encoded UCS-2LE text
    /// @returns Number of pixels occupied by text
    int drawText(SmCanvas * canvas, int x, int y, SmText text);

    /// @brief Output ASCII text (symbols between 0x20 and 0x7E)
    int drawText(SmCanvas * canvas, int x, int y, char * text);

    /// @brief Output encoded UCS2-LE text, fit in the box
    /// @details If the text is smaller than the width then it will be centered
    /// @param canvas: Canvas to draw on
    /// @param x1: X coordinate on the left top point
    /// @param y1: Y coordinate on the left top point
    /// @param x2: X coordinate on the left top point
    /// @param y2: Y coordinate on the left top point
    /// @param text: encoded UCS-2LE text
    void drawTextBox(SmCanvas * canvas, int x1, int y1, int x2, int y2, SmText text);

    /// @brief Get symbol width
    uint32_t getSymbolWidth(uint16_t symbol);
    /// @brief Get text width, stop calculation at stopAt. Does not stop if stopAt < 0
    /// fitted can return a number of symbols for the calculated width
    uint32_t getStringWidth(SmText text, int stopAt, uint32_t * fitted);
    /// @brief Get ASCII text width, stop calculation at stopAt. Does not stop if stopAt < 0
    uint32_t getStringWidth(char * text, int stopAt);
    /// @brief Get total font height
    uint32_t getFontHeight(void) { return mFontHeight; }
    /// @brief Get font baseline position
    uint32_t getBaseLine(void) {return mBaseLine; }
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
