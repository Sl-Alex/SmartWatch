#include "sm_font.h"
#include "sm_hw_storage.h"
#include "sm_strings.h"

#define SUBST_SYMBOL 0xAE

bool SmFont::init(int index)
{
    // Get element info by index
    SmHwStorageElementInfo info;
    bool ret = SmHwStorage::getInstance()->getElementInfo(index, &info);

    if (!ret)
        return false;

    // Store font offset
    mFontOffset = info.offset;

    // Read font header
    SmHwStorage::getInstance()->readData(mFontOffset,
                                         (uint8_t *)&mSymbolCount, sizeof(mSymbolCount));
    SmHwStorage::getInstance()->readData(mFontOffset + 1*sizeof(uint32_t),
                                         (uint8_t *)&mFontHeight, sizeof(mFontHeight));
    SmHwStorage::getInstance()->readData(mFontOffset + 2*sizeof(uint32_t),
                                         (uint8_t *)&mBaseLine, sizeof(mBaseLine));
    SmHwStorage::getInstance()->readData(mFontOffset + 3*sizeof(uint32_t),
                                         (uint8_t *)&mSpacing, sizeof(mSpacing));

    // Calculate offsets
    mDataTableOffset = mFontOffset + 4*sizeof(uint32_t);
    mSymbolTableOffset = mDataTableOffset + sizeof(uint32_t) * mSymbolCount;

    return true;
}

void SmFont::drawSymbol(SmCanvas * canvas, int x, int y, uint16_t symbol)
{
    uint32_t offset;

    SmHwStorage::getInstance()->readData(mDataTableOffset + symbol * sizeof(offset),
                                         (uint8_t *)&offset, sizeof(offset));

    offset += mFontOffset;

    SmImage::initOffset(offset);

    canvas->drawCanvas(x,y,this);
}

uint32_t SmFont::getSymbolWidth(uint16_t symbol)
{
    uint32_t offset;

    SmHwStorage::getInstance()->readData(mDataTableOffset + symbol * sizeof(offset),
                                         (uint8_t *)&offset, sizeof(offset));

    SmImage::initHeaderOffset(offset + mFontOffset);
    return SmImage::getHeader()->width;
}

int SmFont::drawText(SmCanvas * canvas, int x, int y, SmText text)
{
    int start = x;
    while (text.length--)
    {
        drawSymbol(canvas, x, y, *text.pText);
        canvas->fillRect(x + getWidth(), y, x+getWidth(),y+ mFontHeight,0);
        x += getWidth() + mSpacing;
        text.pText++;
    }
    if (x > start)
        return x - mSpacing;
    else
        return x;
}

int SmFont::drawText(SmCanvas * canvas, int x, int y, char * text)
{
    int start = x;
    while (*text)
    {
        drawSymbol(canvas, x, y, *text - 0x20);
        canvas->fillRect(x + getWidth(), y, x+getWidth(),y+ mFontHeight,0);
        x += getWidth() + mSpacing;
        text++;
    }
    if (x > start)
        return x - mSpacing;
    else
        return x;
}

void SmFont::drawTextBox(SmCanvas * canvas, int x1, int y1, int x2, int y2, SmText text)
{
    // Check input parameters
    if ((x2 < x1) || ((y2 - y1) < (int)mFontHeight))
        return;

    // Calculate parameters
    int lines_max = (y2 - y1) / mFontHeight;
    int line_width = 0;
    int lines = 0;
    int cnt = 0;
    SmText line = text;
    uint32_t fitted;
    while (cnt < text.length)
    {
        uint32_t width = getStringWidth(line,x2 - x1, &fitted);
        // Check if there are fitted symbols
        if (fitted == 0)
            break;
        // Calculate line width (useful for single-line output)
        if ((int)width > line_width)
            line_width = width;
        cnt += fitted;
        lines++;
        if (lines >= lines_max)
        {
            if (cnt < text.length)
            {
                text.pText[cnt - 1] = SUBST_SYMBOL;
                text.length = cnt;
            }
            break;
        }
        line.pText = text.pText + cnt;
        line.length = text.length - cnt;
    }
    // Vertical center
    uint32_t v_pos = y1 + (y2 - y1 - mFontHeight * lines)/2;
    // Horizontal center (only for a single-line)
    uint32_t h_pos;
    h_pos = x1 + (x2 - x1 - line_width)/2;

    SmText line_text = text;
    cnt = 0;
    lines = 0;
    while (cnt < text.length)
    {
        getStringWidth(line_text,x2 - x1, &fitted);
        // Check if there are fitted symbols
        if (fitted == 0)
            break;
        line_text.length = fitted;

        cnt += fitted;
        lines++;
        drawText(canvas, h_pos, v_pos, line_text);
        //break;
        v_pos += mFontHeight;
        if (lines >= lines_max)
        {
            break;
        }
        line_text.pText = text.pText + cnt;
        line_text.length = text.length - cnt;
    }
}

uint32_t SmFont::getStringWidth(SmText text, int stopAt, uint32_t * fitted)
{
    uint32_t res = 0;
    uint32_t new_res;

    if (fitted)
        *fitted = 0;

    while (text.length--)
    {
        new_res = res + getSymbolWidth(*text.pText);

        if ((stopAt > 0) && ((int)new_res >= stopAt))
            break;

        res = new_res + mSpacing;
        text.pText++;
        if (fitted)
            (*fitted)++;
    }
    if (res)
        res -= mSpacing;

    return res;
}

uint32_t SmFont::getStringWidth(char * text, int stopAt)
{
    uint32_t res = 0;
    uint32_t new_res;
    while (*text)
    {
        new_res = res + getSymbolWidth(*text - 0x20);

        if ((stopAt > 0) && ((int)new_res >= stopAt))
            break;

        res = new_res + mSpacing;
        text++;
    }
    if (res)
        res -= mSpacing;

    return res;
}
