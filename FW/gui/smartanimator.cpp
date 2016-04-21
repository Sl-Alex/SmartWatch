#include "smartanimator.h"

void SmartAnimator::start(int x, int y, int xOff, int yOff, int w, int h)
{
    mX = x;
    mY = y;
    mXOff = xOff;
    mYOff = yOff;
    mW = w;
    mH = h;
    mTick = 0;
}

bool SmartAnimator::tick()
{
    if (mW == 0)
        return false;
    if (mH == 0)
        return false;

    if ((mDir == ANIM_DIR_LEFT) || (mDir == ANIM_DIR_RIGHT))
    {
        if (mTick >= mW)
            return false;
    }
    else if ((mDir == ANIM_DIR_UP) || (mDir == ANIM_DIR_DOWN))
    {
        if (mTick >= mH)
            return false;
    }

    switch (mDir)
    {
        case ANIM_DIR_LEFT:
            // Scroll canvas
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit > 0)
                    limit = mW - 1 + mLimit;
                else
                    limit = mX + mLimit;

                pCanvas->scrollArea(mX - limit,mY,mX - mTick,mY+mH-1,1,SmartCanvas::DIR_LEFT,false);
            }
            // Draw a part of the image
            pCanvas->drawCanvas(mX - mTick, mY, mXOff, mYOff, mTick  +1, mH, pImage);
            // Draw line in case of visual slide
            if ((mType == ANIM_TYPE_VIS_SLIDE) && (mTick != mW - 1))
            {
                pCanvas->drawVLine(mX - mTick,mY, mY + mH - 1, 255);
            }
            break;
        case ANIM_DIR_RIGHT:
            // Scroll canvas
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit > 0)
                    limit = mW - 1 + mLimit;
                else
                    limit = mX + mLimit;

                pCanvas->scrollArea(mX + mTick,mY,mX + limit,mY+mH-1,1,SmartCanvas::DIR_RIGHT,false);
            }
            // Draw a part of the image
            pCanvas->drawCanvas(mX, mY, mW + mXOff - mTick - 1, mYOff, mTick  + 1, mH, pImage);
            // Draw line in case of visual slide
            if ((mType == ANIM_TYPE_VIS_SLIDE) && (mTick != mW - 1))
            {
                pCanvas->drawVLine(mX + mTick, mY, mY + mH - 1, 255);
            }
            break;
        case ANIM_DIR_UP:
            // Scroll canvas
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit > 0)
                    limit = mH - 1 + mLimit;
                else
                    limit = mY + mLimit;

                pCanvas->scrollArea(mX,mY - limit,mX+mW-1,mY - mTick,1,SmartCanvas::DIR_UP,false);
            }
            // Draw a part of the image
            pCanvas->drawCanvas(mX, mY - mTick, mXOff, mYOff, mW, mTick  +1, pImage);
            // Draw line in case of visual slide
            if ((mType == ANIM_TYPE_VIS_SLIDE) && (mTick != mH - 1))
            {
                pCanvas->drawHLine(mX, mX + mW - 1, mY - mTick, 255);
            }
            break;
        case ANIM_DIR_DOWN:
            // Scroll canvas
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit > 0)
                    limit = mH - 1 + mLimit;
                else
                    limit = mY + mLimit;

                pCanvas->scrollArea(mX,mY + mTick,mX+mW-1,mY + limit,1,SmartCanvas::DIR_DOWN,false);
            }
            // Draw a part of the image
            pCanvas->drawCanvas(mX, mY, mXOff, mH + mYOff - mTick - 1, mW, mTick  + 1, pImage);
            // Draw line in case of visual slide
            if ((mType == ANIM_TYPE_VIS_SLIDE) && (mTick != mH - 1))
            {
                pCanvas->drawHLine(mX, mX + mW - 1, mY + mTick, 255);
            }
            break;
    }

    mTick++;
    if ((mDir == ANIM_DIR_LEFT) || (mDir == ANIM_DIR_RIGHT))
    {
        if (mTick >= mW)
            return false;
    }
    else if ((mDir == ANIM_DIR_UP) || (mDir == ANIM_DIR_DOWN))
    {
        if (mTick >= mH)
            return false;
    }

    return true;
}
