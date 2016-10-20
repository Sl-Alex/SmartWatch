#include "sm_font.h"
#include "sm_hw_storage.h"
#include "sm_strings.h"

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

    SmImage::initHeaderOffset(offset);
    return this->getWidth();
}

void SmFont::drawText(SmCanvas * canvas, int x, int y, uint16_t * text, uint16_t count)
{
    while (count--)
    {
        drawSymbol(canvas, x, y, *text);
        canvas->fillRect(x + getWidth(), y, x+getWidth(),y+ mFontHeight,0);
        x += getWidth() + mSpacing;
        text++;
    }
}

void SmFont::drawText(SmCanvas * canvas, int x, int y, char * text)
{
    while (*text)
    {
        drawSymbol(canvas, x, y, *text - 0x20);
        canvas->fillRect(x + getWidth(), y, x+getWidth(),y+ mFontHeight,0);
        x += getWidth() + mSpacing;
        text++;
    }
}

uint32_t SmFont::getStringWidth(uint16_t * text, uint16_t count)
{
    uint32_t res = 0;
    while (count--)
    {
        res += getSymbolWidth(*text);
        res += mSpacing;
        text++;
    }
    if (res)
        res -= mSpacing;

    return res;
}

uint32_t SmFont::getStringWidth(char * text)
{
    uint32_t res = 0;
    while (*text)
    {
        res += getSymbolWidth(*text - 0x20);
        res += mSpacing;
        text++;
    }
    if (res)
        res -= mSpacing;

    return res;
}
