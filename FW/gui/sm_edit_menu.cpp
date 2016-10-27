#include "sm_edit_menu.h"
#include "sm_display.h"
#include "sm_font.h"
#include "sm_hw_storage.h"
#include "sm_strings.h"

SmEditMenu::SmEditMenu(SmHwKeyboardIface * parent, EditGroup group)
    :pParent(parent)
{
    mSelected = 0;
    mGroup = group;
    pCanvas = new SmCanvas();
    pCanvas->init(128,64);
    pCanvas->clear();

    std::vector<SmEditor::SmEditorItem> numbers = std::vector<SmEditor::SmEditorItem>{
        {SM_STRING_0,SM_STRING_0_SZ},
        {SM_STRING_1,SM_STRING_1_SZ},
        {SM_STRING_2,SM_STRING_2_SZ},
        {SM_STRING_3,SM_STRING_3_SZ},
        {SM_STRING_4,SM_STRING_4_SZ},
        {SM_STRING_5,SM_STRING_5_SZ},
        {SM_STRING_6,SM_STRING_6_SZ},
        {SM_STRING_7,SM_STRING_7_SZ},
        {SM_STRING_8,SM_STRING_8_SZ},
        {SM_STRING_9,SM_STRING_9_SZ}
    };
    std::vector<SmEditor::SmEditorItem> onOff = std::vector<SmEditor::SmEditorItem>{
        {SM_STRING_ON_SYMB,SM_STRING_ON_SYMB_SZ},
        {SM_STRING_OFF_SYMB,SM_STRING_OFF_SYMB_SZ},
    };

    switch (mGroup) {
    case EG_BLUETOOTH:
        mEditors[0].init(onOff);
        mEditors[1].init(numbers);
        mEditors[2].init(numbers);
        mEditors[3].init(numbers);
        mEditors[4].init(numbers);
        itemsCount = 5;
        mHint = SM_STRING_BT_HINT;
        mHintLen = SM_STRING_BT_HINT_SZ;
        break;
    case EG_DATE:
        mEditors[0].init(numbers);
        mEditors[1].init(numbers);
        mEditors[2].init(numbers);
        mEditors[3].init(numbers);
        mEditors[4].init(numbers);
        mEditors[5].init(numbers);
        itemsCount = 6;
        mHint = SM_STRING_DATE_HINT;
        mHintLen = SM_STRING_DATE_HINT_SZ;
        break;
    case EG_TIME:
        mEditors[0].init(numbers);
        mEditors[1].init(numbers);
        mEditors[2].init(numbers);
        mEditors[3].init(numbers);
        itemsCount = 4;
        mHint = SM_STRING_TIME_HINT;
        mHintLen = SM_STRING_TIME_HINT_SZ;
        break;
    default:
        break;
    }

    mEditors[0].setFocus();

    drawItems();
    menuAnimator.setDirection(SmAnimator::ANIM_DIR_LEFT);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(128);
    menuAnimator.setSpeed(4);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
    menuAnimator.start(127,0,0,0,128,64);

    SmHwKeyboard::getInstance()->subscribe(this);
    SmHalSysTimer::subscribe(this, 10, true);
}

SmEditMenu::~SmEditMenu()
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.finish();
        delete pCanvas;
    }

    SmHwKeyboard::getInstance()->unsubscribe(this);
    SmHalSysTimer::unsubscribe(this);
}

void SmEditMenu::onTimer(uint32_t)
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.tick();
        if (!menuAnimator.isRunning())
        {
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
        }
    }
}

void SmEditMenu::gotoNext(void)
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.finish();
        delete pCanvas;
        pCanvas = SmDisplay::getInstance()->getCanvas();
    }

    if (mSelected < itemsCount - 1)
        mSelected++;
    else
        mSelected = 0;

    for (uint32_t i = 0; i < itemsCount; i++)
    {
        if (i != mSelected)
            mEditors[i].clearFocus();
        else
            mEditors[i].setFocus();
    }

    drawItems();

/*
    menuAnimator.setDirection(SmAnimator::ANIM_DIR_UP);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(0);
    menuAnimator.setSpeed(2);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_SHIFT);
    menuAnimator.start(0,63,0,0,128,64);
*/
}

void SmEditMenu::drawItems(void)
{
//    menuAnimator.finish();
    switch (mGroup) {
    case EG_BLUETOOTH:
        pCanvas->drawCanvas(10, 10,&mEditors[0]);
        pCanvas->drawCanvas(40, 10,&mEditors[1]);
        pCanvas->drawCanvas(60, 10,&mEditors[2]);
        pCanvas->drawCanvas(80, 10,&mEditors[3]);
        pCanvas->drawCanvas(100,10,&mEditors[4]);
        break;
    case EG_DATE:
        break;
    case EG_TIME:
        break;
    default:
        break;
    }


    SmFont font;
    font.init(IDX_FW_FONT_SMALL);
    uint32_t len = font.getStringWidth(mHint, mHintLen);
    font.drawText(pCanvas, 64 - len/2, 55, mHint, mHintLen);
/*
    SmImage image;

    image.init(items[selected].imageIndex);

    pCanvas->clear();
    pCanvas->drawCanvas(16,10,&image);

    font.drawText(pCanvas, 16, 50, items[selected].pText, items[selected].symbolsCount);
*/
}

void SmEditMenu::onKeyDown(SmHwButtons key)
{
    if (key == SM_HW_BUTTON_UP)
    {
        mEditors[mSelected].onKeyDown(key);
        drawItems();
    }
    if (key == SM_HW_BUTTON_DOWN)
    {
        mEditors[mSelected].onKeyDown(key);
        drawItems();
    }
    if (key == SM_HW_BUTTON_SELECT)
    {
        gotoNext();
    }
    if (key == SM_HW_BUTTON_EXIT)
    {
        // Inform parent that we are done
        // Do nothing after this step because we will be destroyed here
        pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
    }
}

void SmEditMenu::onKeyUp(SmHwButtons)
{

}
