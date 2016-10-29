#include "sm_edit_menu.h"
#include "sm_display.h"
#include "sm_font.h"
#include "sm_hw_storage.h"
#include "sm_strings.h"
#include "sm_hal_rtc.h"

SmEditMenu::SmEditMenu(SmHwKeyboardIface * parent, EditGroup group)
    :pParent(parent)
{
    mSelected = 0;
    mGroup = group;

    mModified = false;
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

    SmHalRtc::SmHalRtcTime datetime = SmHalRtc::getInstance()->getDateTime();
    switch (mGroup) {
    case EG_BLUETOOTH:
        // On-Off
        mEditors[0].init(onOff);
        // Pin
        mEditors[1].init(numbers);
        mEditors[2].init(numbers);
        mEditors[3].init(numbers);
        mEditors[4].init(numbers);
        itemsCount = 5;
        mHint = SM_STRING_BT_HINT;
        mHintLen = SM_STRING_BT_HINT_SZ;
        break;
    case EG_DATE:
        // Date
        mEditors[0].init(numbers);
        mEditors[1].init(numbers);
        // Month
        mEditors[2].init(numbers);
        mEditors[3].init(numbers);
        // Year
        mEditors[4].init(numbers);
        mEditors[5].init(numbers);
        // Set default date
        mEditors[0].setSelection(datetime.day / 10);
        mEditors[1].setSelection(datetime.day % 10);
        // Set default month
        mEditors[2].setSelection(datetime.month / 10);
        mEditors[3].setSelection(datetime.month % 10);
        // Set default year
        mEditors[4].setSelection((datetime.year % 100) / 10);
        mEditors[5].setSelection(datetime.year % 10);
        // Static min-max (will not be changed)
        mEditors[2].setMinMax(0,1);
        itemsCount = 6;
        mHint = SM_STRING_DATE_HINT;
        mHintLen = SM_STRING_DATE_HINT_SZ;
        break;
    case EG_TIME:
        // Hours
        mEditors[0].init(numbers);
        mEditors[1].init(numbers);
        // Minutes
        mEditors[2].init(numbers);
        mEditors[3].init(numbers);
        // Static min-max (will not be changed)
        mEditors[0].setMinMax(0,2);
        mEditors[2].setMinMax(0,5);
        // Set default time
        mEditors[0].setSelection(datetime.hour / 10);
        mEditors[1].setSelection(datetime.hour % 10);
        mEditors[2].setSelection(datetime.minute / 10);
        mEditors[3].setSelection(datetime.minute % 10);
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
        pCanvas->drawCanvas(10, 10,&mEditors[0]);
        pCanvas->drawCanvas(25, 10,&mEditors[1]);
        pCanvas->drawCanvas(50, 10,&mEditors[2]);
        pCanvas->drawCanvas(65, 10,&mEditors[3]);
        pCanvas->drawCanvas(90,10,&mEditors[4]);
        pCanvas->drawCanvas(105,10,&mEditors[5]);
        break;
    case EG_TIME:
        pCanvas->drawCanvas(30, 10,&mEditors[0]);
        pCanvas->drawCanvas(45, 10,&mEditors[1]);
        pCanvas->drawCanvas(70, 10,&mEditors[2]);
        pCanvas->drawCanvas(85, 10,&mEditors[3]);
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
    if ((key == SM_HW_BUTTON_UP)||(key == SM_HW_BUTTON_DOWN))
    {
        mModified = true;
        mEditors[mSelected].onKeyDown(key);

        uint32_t day;
        uint32_t month;
        uint32_t year;
        uint32_t maxday;

        switch (mGroup) {
        case EG_BLUETOOTH:

            break;
        case EG_DATE:
            day   = mEditors[0].getSelection()*10 + mEditors[1].getSelection();
            month = mEditors[2].getSelection()*10 + mEditors[3].getSelection();
            year  = 2000 + mEditors[4].getSelection()*10 + mEditors[5].getSelection();
            maxday = SmHalRtc::getInstance()->getDaysOfMonth(year,month);
            // Check month
            if (month < 10)
                mEditors[3].setMinMax(1,9);
            else
                mEditors[3].setMinMax(0,2);
            // Update month
            month = mEditors[2].getSelection()*10 + mEditors[3].getSelection();
            // Check day
            if (day < 10)
                mEditors[1].setMinMax(1,9);
            else if (day > maxday)
            {
                mEditors[0].setMinMax(0,maxday/10);
                mEditors[1].setMinMax(0,maxday%10);
            }
            else
            {
                mEditors[0].setMinMax(0,maxday/10);
                mEditors[1].setMinMax(0,9);
            }
            // Update day
            day   = mEditors[0].getSelection()*10 + mEditors[1].getSelection();
            break;
        case EG_TIME:
            if (mEditors[0].getSelection() == 2)
                mEditors[1].setMinMax(0,3);
            else
                mEditors[1].setMinMax(0,9);
            break;
        default:
            break;
        }
        drawItems();
    }
    if (key == SM_HW_BUTTON_SELECT)
    {
        gotoNext();
    }
    if (key == SM_HW_BUTTON_EXIT)
    {
        if (mModified)
        {
            SmHalRtc::SmHalRtcTime datetime = SmHalRtc::getInstance()->getDateTime();
            switch (mGroup) {
            case EG_BLUETOOTH:

                break;
            case EG_DATE:
                datetime.day   = mEditors[0].getSelection()*10 + mEditors[1].getSelection();
                datetime.month = mEditors[2].getSelection()*10 + mEditors[3].getSelection();
                datetime.year = 2000 + mEditors[0].getSelection()*10 + mEditors[1].getSelection();
                SmHalRtc::getInstance()->setDateTime(datetime);
                break;
            case EG_TIME:
                datetime.hour = mEditors[0].getSelection()*10 + mEditors[1].getSelection();
                datetime.minute = mEditors[2].getSelection()*10 + mEditors[3].getSelection();
                SmHalRtc::getInstance()->setDateTime(datetime);
                break;
            default:
                break;
            }
        }
        // Inform parent that we are done
        // Do nothing after this step because we will be destroyed here
        pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
    }
}

void SmEditMenu::onKeyUp(SmHwButtons)
{

}
