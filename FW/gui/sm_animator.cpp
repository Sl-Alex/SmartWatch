#include "sm_animator.h"

void SmAnimator::start(int x, int y, int xOff, int yOff, int w, int h)
{
    mX = x;
    mY = y;
    mXOff = xOff;
    mYOff = yOff;
    mW = w;
    mH = h;
    mTick = 0;
    mRunning = true;
}

bool SmAnimator::tick()
{
    if (! mRunning)
        return mRunning;

    if ((mW == 0) || (mH == 0))
    {
        mRunning = false;
        return mRunning;
    }

    if ((mDir == ANIM_DIR_LEFT) || (mDir == ANIM_DIR_RIGHT))
    {
        if (mTick >= mW)
        {
            mRunning = false;
            return mRunning;
        }
        if ((mTick + mSpeed) >= mW)
        {
            mTick -= mSpeed;
            mSpeed = mW - mTick;
            mTick = mW - 1;
        }
    }
    else if ((mDir == ANIM_DIR_UP) || (mDir == ANIM_DIR_DOWN))
    {
        if (mTick >= mH)
        {
            mRunning = false;
            return mRunning;
        }
        if ((mTick + mSpeed) >= mH)
        {
            mTick -= mSpeed;
            mSpeed = mH - mTick;
            mTick = mH - 1;
        }
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

                pDest->scrollArea(mX - limit,mY,mX - mTick,mY+mH-1,mSpeed,SmCanvas::DIR_LEFT);
            }
            // Draw a part of the image
            if ((mType != ANIM_TYPE_APPEAR) && (mType != ANIM_TYPE_VIS_APPEAR))
            {
                pDest->drawCanvas(mX - mTick, mY, mXOff, mYOff, mTick  +1, mH, pSource);
            }
            else
            {
                if (mTick > 0)
                    pDest->drawCanvas(mX - mTick, mY, mXOff + mW - mTick - 1, mYOff, mSpeed  + 1, mH, pSource);
            }
            // Draw line in case of visual slide
            if (((mType == ANIM_TYPE_VIS_SLIDE) || (mType == ANIM_TYPE_VIS_APPEAR)) && (mTick != mW - 1))
            {
                pDest->drawVLine(mX - mTick,mY, mY + mH - 1, 255);
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

                pDest->scrollArea(mX + mTick,mY,mX + limit,mY+mH-1,mSpeed,SmCanvas::DIR_RIGHT);
            }
            // Draw a part of the image
            if ((mType != ANIM_TYPE_APPEAR) && (mType != ANIM_TYPE_VIS_APPEAR))
            {
                pDest->drawCanvas(mX, mY, mW + mXOff - mTick - 1, mYOff, mTick  + 1, mH, pSource);
            }
            else
            {
                if (mTick > 0)
                    pDest->drawCanvas(mX + mTick - mSpeed, mY, mXOff + mTick - mSpeed, mYOff, mSpeed + 1, mH, pSource);
            }
            // Draw line in case of visual slide
            if (((mType == ANIM_TYPE_VIS_SLIDE) || (mType == ANIM_TYPE_VIS_APPEAR))&& (mTick != mW - 1))
            {
                pDest->drawVLine(mX + mTick, mY, mY + mH - 1, 255);
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

                pDest->scrollArea(mX,mY - limit,mX+mW-1,mY - mTick,mSpeed,SmCanvas::DIR_UP);
            }
            // Draw a part of the image
            if ((mType != ANIM_TYPE_APPEAR) && (mType != ANIM_TYPE_VIS_APPEAR))
            {
                pDest->drawCanvas(mX, mY - mTick, mXOff, mYOff, mW, mTick  +1, pSource);
            }
            else
            {
                if (mTick > 0)
                    pDest->drawCanvas(mX, mY - mTick, mXOff, mYOff + mH - mTick - 1, mW, mSpeed  + 1, pSource);
            }
            // Draw line in case of visual slide
            if (((mType == ANIM_TYPE_VIS_SLIDE) || (mType == ANIM_TYPE_VIS_APPEAR)) && (mTick != mH - 1))
            {
                pDest->drawHLine(mX, mX + mW - 1, mY - mTick, 255);
            }
            break;
        case ANIM_DIR_DOWN:
            // Scroll canvas
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit >= 0)
                    limit = mH - 1 + mLimit;
                else
                    limit = mY + mLimit;

                pDest->scrollArea(mX,mY + mTick,mX+mW-1,mY + limit,mSpeed,SmCanvas::DIR_DOWN);
            }
            // Draw a part of the image
            if ((mType != ANIM_TYPE_APPEAR) && (mType != ANIM_TYPE_VIS_APPEAR))
            {
                pDest->drawCanvas(mX, mY, mXOff, mH + mYOff - mTick - 1, mW, mTick  + 1, pSource);
            }
            else
            {
                if (mTick > 0)
                    pDest->drawCanvas(mX, mY + mTick - mSpeed, mXOff, mYOff + mTick - mSpeed, mW, mSpeed + 1, pSource);
            }
            // Draw line in case of visual slide
            if (((mType == ANIM_TYPE_VIS_SLIDE) || (mType == ANIM_TYPE_VIS_APPEAR)) && (mTick != mH - 1))
            {
                pDest->drawHLine(mX, mX + mW - 1, mY + mTick, 255);
            }
            break;
    }

    mTick += mSpeed;
    if ((mDir == ANIM_DIR_LEFT) || (mDir == ANIM_DIR_RIGHT))
    {
//        if (mTick >= mW)
//            return false;
    }
    else if ((mDir == ANIM_DIR_UP) || (mDir == ANIM_DIR_DOWN))
    {
//        if (mTick >= mH)
//            return false;
    }
    mRunning = true;

    return mRunning;
}

void SmAnimator::finish(void)
{
    if (! mRunning)
        return;

    if ((mW == 0) || (mH == 0))
    {
        mRunning = false;
        return;
    }

    if ((mDir == ANIM_DIR_LEFT) || (mDir == ANIM_DIR_RIGHT))
    {
        if (mTick > 0)
        {
            mTick -= mSpeed;
            mSpeed = mW - mTick;
            mTick = mW - 1;
        }
    }
    else if ((mDir == ANIM_DIR_UP) || (mDir == ANIM_DIR_DOWN))
    {
        if (mTick > 0)
        {
            mTick -= mSpeed;
            mSpeed = mH - mTick;
            mTick = mH - 1;
        }
    }
    tick();
    mRunning = false;
}
