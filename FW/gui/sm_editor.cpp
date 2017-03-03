#include "sm_editor.h"
#include "sm_display.h"

SmEditor::SmEditor()
{
    mFont.init(IDX_FW_FONT_1_MEDIUM);
    mFocused = false;
    mArrowDown.init(IDX_ICON_ARROW_DOWN);
    mArrowUp.init(IDX_ICON_ARROW_UP);
}

SmEditor::~SmEditor()
{
    for (uint32_t i = 0; i < mItems.size(); i++)
    {
        delete[] mItems[i].pText;
    }
}

void SmEditor::init(const std::vector<SmText> &items)
{
    uint32_t maxWidth = mArrowUp.getWidth();
    // Copy vector
    mItems.clear();
    mItems.reserve(items.size());
    SmText text;
    for (uint32_t i = 0; i < items.size(); i++)
    {
        // Copy strings locally
        uint16_t * data = new uint16_t[items[i].length];
        memcpy(data, items[i].pText, sizeof(uint16_t) * items[i].length);
        text.pText = data;
        text.length = items[i].length;

        // Store locally
        mItems.push_back(text);

        // Calculate width, remember maximum
        uint32_t width = mFont.getStringWidth(text);
        if (width > maxWidth)
            maxWidth = width;
    }
    SmCanvas::init(maxWidth, mFont.getFontHeight()
                   + mArrowDown.getHeight()
                   + mArrowUp.getHeight()
                   + 4);

    mSelected = 0;
    mSelMin = 0;
    mSelMax = mItems.size() - 1;
    refreshImage();
}

void SmEditor::setFocus(void)
{
    if (mFocused)
        return;

    mFocused = true;

    refreshImage();
}

void SmEditor::clearFocus(void)
{
    if (!mFocused)
        return;

    mFocused = false;

    refreshImage();
}

void SmEditor::refreshImage(void)
{
    uint32_t center = getWidth()/2;

    clear();

    if (mFocused)
    {
        drawCanvas(center - mArrowUp.getWidth()/2,0,&mArrowUp);
        drawCanvas(center - mArrowDown.getWidth()/2,getHeight()-mArrowDown.getHeight(),&mArrowDown);
    }

    uint32_t txtWidth = mFont.getStringWidth(mItems[mSelected]);
    mFont.drawText(this, center - txtWidth / 2, mArrowUp.getHeight() + 2, mItems[mSelected]);
}

void SmEditor::onKeyDown(SmHwButtons key)
{
    if (key == SM_HW_BUTTON_DOWN)
    {
        if (mSelected < mSelMax)
            mSelected++;
        else
            mSelected = mSelMin;
    }
    if (key == SM_HW_BUTTON_UP)
    {
        if (mSelected > mSelMin)
            mSelected--;
        else
            mSelected = mSelMax;
    }
    refreshImage();
}

void SmEditor::onKeyUp(SmHwButtons)
{
}

void SmEditor::setMinMax(uint32_t min, uint32_t max)
{
    bool refresh;

    if (min >= mItems.size())
        min = mItems.size() - 1;
    if (max >= mItems.size())
        max = mItems.size() - 1;

    mSelMin = min;
    mSelMax = max;

    if (mSelected < mSelMin)
    {
        refresh = true;
        mSelected = mSelMin;
    }
    if (mSelected > mSelMax)
    {
        refresh = true;
        mSelected = mSelMax;
    }

    if (refresh)
        refreshImage();
}

void SmEditor::setSelection(uint32_t selection)
{
    if (selection > mSelMax)
        selection = mSelMax;
    if (selection < mSelMin)
        selection = mSelMin;

    if (selection == mSelected)
        return;

    mSelected = selection;

    refreshImage();
}
