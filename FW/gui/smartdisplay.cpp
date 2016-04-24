#include "smartdisplay.h"
#include "smartcanvas.h"
#include "smartfont.h"
#include "smartstorage.h"
#include "smartanimator.h"
#include "sh1106.h"
#include "stdio.h"

static SmartCanvas * pCanvas;
static SmartCanvas * pTextCanvas;
static SmartCanvas * pImage;

//extern "C" {
SmartAnimator * pAnimator;
SmartAnimator * pAnimator2;
int imageIndex = 0;
#define IMG_OFFSET  1
#define IMG_CNT     7

void SmartDisplay_init(void)
{
    LcdInit();
    SmartStorage::init();
    pCanvas = new SmartCanvas();
    pCanvas->init(128,64);
    pCanvas->clear();
    pTextCanvas = new SmartCanvas();
    pTextCanvas->init(6*20,8);
    pTextCanvas->clear();
    SmartFont::loadFont();
    char str[20];
    sprintf(str, "0x%X", SmartStorage::readId());
    SmartFont::drawText(pCanvas,10,54,str);
    char hello[] = "Hello C++ :)";
    SmartFont::drawText(pTextCanvas,0,0,hello);
    pAnimator = new SmartAnimator();
    pAnimator->setCanvas(pCanvas);
    pAnimator->setImage(pTextCanvas);
    pAnimator->setType(SmartAnimator::ANIM_TYPE_SLIDE);
    pAnimator->setDirection(SmartAnimator::ANIM_DIR_LEFT);
    pAnimator->setSpeed(1);
    pAnimator->setShiftLimit(6*12);
    pAnimator->start(100, 4, 0, 0, 6*12, 8);
    pImage = new SmartCanvas();
    pImage->init(IMG_OFFSET + imageIndex);

    pAnimator2 = new SmartAnimator();
    pAnimator2->setCanvas(pCanvas);
    pAnimator2->setImage(pImage);
    pAnimator2->setType(SmartAnimator::ANIM_TYPE_SHIFT);
    pAnimator2->setDirection(SmartAnimator::ANIM_DIR_LEFT);
    pAnimator2->setSpeed(1);
    pAnimator2->setShiftLimit(128 - 32 - 4);
    pAnimator2->start(125, 16, 0, 0, 32, 32);
}

void SmartDisplay_setPix(int x, int y, int value)
{
    pCanvas->drawRect(0,0,127,63,1);
//    pCanvas->drawLine(0,0,127,63,1);
//    pCanvas->drawLine(0,63,127,0,1);
    //SmartFont::drawText(pCanvas,54,4,"!\"#");
    //SmartFont::drawSymbol(pCanvas,61,4,1);
}

void SmartDisplay_update(void)
{
    pAnimator->tick();
    if (!pAnimator2->tick())
    {
        imageIndex++;
        if (imageIndex >= IMG_CNT) imageIndex = 0;
        pImage->init(IMG_OFFSET + imageIndex);
        pAnimator2->start(125, 16, 0, 0, 32, 32);
    }
    LcdOut(pCanvas->pData);
}

//}
