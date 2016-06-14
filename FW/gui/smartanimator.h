#ifndef SMARTANIMATOR_H
#define SMARTANIMATOR_H

#include "smartcanvas.h"

class SmartAnimator
{
public:
    typedef enum {
        ANIM_TYPE_SHIFT,
        ANIM_TYPE_SLIDE,
        ANIM_TYPE_VIS_SLIDE
    } AnimType;

    typedef enum {
        ANIM_DIR_UP,
        ANIM_DIR_DOWN,
        ANIM_DIR_LEFT,
        ANIM_DIR_RIGHT
    } AnimDir;

    SmartAnimator()
        :mTick(0),
        mX(0),
        mY(0),
        mXOff(0),
        mYOff(0),
        mW(0),
        mH(0) {}

    inline void setCanvas(SmartCanvas * canvas) {pCanvas = canvas;}
    inline void setImage(SmartCanvas * image) {pImage = image;}
    inline void setType(AnimType type) {mType = type;}
    inline void setDirection(AnimDir direction) {mDir = direction;}
    inline void setSpeed(int speed) {mSpeed = speed;}
    inline void setShiftLimit(int pixels) {mLimit = pixels;} // Only for ANIM_TYPE_SHIFT, negative numbers mean margin from the display border

    void start(int x, int y, int xOff, int yOff, int w, int h);
    bool tick(void);
    void finish();

private:
    SmartCanvas * pCanvas;
    SmartCanvas * pImage;

    AnimType mType;
    AnimDir mDir;

    int mSpeed; /// @todo Implement
    int mTick;
    int mLimit;
    int mX, mY, mXOff, mYOff, mW, mH;
};

#endif // SMARTANIMATOR_H
