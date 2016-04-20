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
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit > 0)
                    limit = mW - 1 + mLimit;
                else
                    limit = mX + mLimit;

                for (int x = mX - limit; x < mX - mTick; x++)
                {
                    for (int y = 0; y < mH; y++)
                    {
                        pCanvas->setPix(x, mY + y, pCanvas->getPix(x + mXOff + 1, mY + mYOff + y));
                    }
                }
            }
            for(int x = 0; x < mTick + 1; x++)
            {
                if ((mType == ANIM_TYPE_VIS_SLIDE) && (x == 0) && (mTick != mW - 1))
                {
                    for(int y = 0; y < mH; y++)
                    {
                        pCanvas->setPix(mX - mTick, mY + y, 255);
                    }
                }
                else for(int y = 0; y < mH; y++)
                {
                    pCanvas->setPix(mX - mTick + x, mY + y, pImage->getPix(x + mXOff,y + mYOff));
                }
            }
            break;
        case ANIM_DIR_RIGHT:
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit > 0)
                    limit = mW - 1 + mLimit;
                else
                    limit = mX + mLimit;

                for (int x = mX + limit; x > mX + mTick - 1; x--)
                {
                    for (int y = 0; y < mH; y++)
                    {
                        pCanvas->setPix(x + 1, mY + y, pCanvas->getPix(x + mXOff, mY + mYOff + y));
                    }
                }
            }
            for(int x = 0; x < mTick + 1; x++)
            {
                if ((mType == ANIM_TYPE_VIS_SLIDE) && (x == mTick) && (mTick != mW - 1))
                {
                    for(int y = 0; y < mH; y++)
                    {
                        pCanvas->setPix(mX + mTick, mY + y, 255);
                    }
                }
                else for(int y = 0; y < mH; y++)
                {
                    pCanvas->setPix(mX + x, mY + y, pImage->getPix(mW + x + mXOff - mTick - 1,y + mYOff));
                }
            }
            break;
        case ANIM_DIR_UP:
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit > 0)
                    limit = mH - 1 + mLimit;
                else
                    limit = mY + mLimit;

                for (int y = mY - limit; y < mY - mTick; y++)
                {
                    for (int x = 0; x < mW; x++)
                    {
                        pCanvas->setPix(mX + x, y, pCanvas->getPix(mX + mXOff + x, y + mYOff + 1));
                    }
                }
            }
            for(int y = 0; y < mTick + 1; y++)
            {
                if ((mType == ANIM_TYPE_VIS_SLIDE) && (y == 0) && (mTick != mH - 1))
                {
                    for(int x = 0; x < mW; x++)
                    {
                        pCanvas->setPix(mX + x, mY - mTick, 255);
                    }
                }
                else for(int x = 0; x < mW; x++)
                {
                    pCanvas->setPix(mX + x, mY - mTick + y, pImage->getPix(x + mXOff,y + mYOff));
                }
            }
            break;
        case ANIM_DIR_DOWN:
            if (mType == ANIM_TYPE_SHIFT)
            {
                int limit;

                if (mLimit > 0)
                    limit = mH - 1 + mLimit;
                else
                    limit = mY + mLimit;

                for (int y = mY + limit; y > mY + mTick - 1; y--)
                {
                    for (int x = 0; x < mW; x++)
                    {
                        pCanvas->setPix(mX + x, y + 1, pCanvas->getPix(mX + mXOff + x, y + mYOff));
                    }
                }
            }
            for(int y = 0; y < mTick + 1; y++)
            {
                if ((mType == ANIM_TYPE_VIS_SLIDE) && (y == mTick) && (mTick != mH - 1))
                {
                    for(int x = 0; x < mW; x++)
                    {
                        pCanvas->setPix(mX + x, mY + mTick, 255);
                    }
                }
                else for(int x = 0; x < mW; x++)
                {
                    pCanvas->setPix(mX + x, mY + y, pImage->getPix(x + mXOff,mH + mYOff + y - mTick - 1));
                }
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
