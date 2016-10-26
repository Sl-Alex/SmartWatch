#include "sm_editor.h"
#include "sm_display.h"

SmEditor::SmEditor()
{
    mFont.init(IDX_FW_FONT_2_MEDIUM);
    mFocused = false;
    mArrowDown.init(IDX_ICON_ARROW_DOWN);
    mArrowUp.init(IDX_ICON_ARROW_UP);
}

SmEditor::~SmEditor()
{
    for (uint32_t i = 0; i < mItems.size(); i++)
    {
        delete[] mItems[i].data;
    }
}

void SmEditor::init(const std::vector<SmEditorItem> &items)
{
    uint32_t maxWidth = 0;
    // Copy vector
    mItems.clear();
    mItems.reserve(items.size());
    for (uint32_t i = 0; i < items.size(); i++)
    {
        // Copy strings locally
        uint16_t * data = new uint16_t[items[i].length];
        memcpy(data, items[i].data, sizeof(uint16_t) * items[i].length);
        // Store locally
        mItems.emplace_back(SmEditorItem{data, items[i].length});

        // Calculate width, remember maximum
        uint32_t width = mFont.getStringWidth(data, items[i].length);
        if (width > maxWidth)
            maxWidth = width;
    }
    SmCanvas::init(maxWidth, mFont.getFontHeight()
                   + mArrowDown.getHeight()
                   + mArrowUp.getHeight()
                   + 4);

    mSelected = 0;
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

    uint32_t txtWidth = mFont.getStringWidth(mItems[mSelected].data, mItems[mSelected].length);
    mFont.drawText(this, center - txtWidth / 2, mArrowUp.getHeight() + 2, mItems[mSelected].data, mItems[mSelected].length);
}

void SmEditor::onKeyDown(SmHwButtons key)
{
    if (key == SM_HW_BUTTON_DOWN)
    {
        if (mSelected < mItems.size() - 1)
            mSelected++;
        else
            mSelected = 0;
    }
    if (key == SM_HW_BUTTON_UP)
    {
        if (mSelected > 0)
            mSelected--;
        else
            mSelected = mItems.size() - 1;
    }
    refreshImage();
}

void SmEditor::onKeyUp(SmHwButtons key)
{
}
