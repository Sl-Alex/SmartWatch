#ifndef SM_MAIN_MENU_H
#define SM_MAIN_MENU_H

#include "sm_canvas.h"
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_strings.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_keyboard.h"
#include "sm_hw_storage.h"
#include "sm_edit_menu.h"

class SmMainMenu: public SmHalSysTimerIface, public SmHwKeyboardIface
{
public:
    SmMainMenu(SmHwKeyboardIface * parent);
    ~SmMainMenu();

    /// @brief System timer event
    void onTimer(uint32_t timeStamp);

    /// @brief onKeyDown event.
    void onKeyDown(SmHwButtons key);
    /// @brief onKeyUp event.
    void onKeyUp(SmHwButtons key);
private:

    void gotoNext(void);
    void gotoPrev(void);
    void drawItem(void);

    struct SmMenuItem {
        uint8_t imageIndex;
        uint16_t * pText;
        uint16_t symbolsCount;
    };
    static const int ITEMS_COUNT = 3;

    SmCanvas * pCanvas;
    SmHwKeyboardIface * pParent;
    SmAnimator menuAnimator;

    SmEditMenu * pEditMenu;

    uint8_t selected;

    SmMenuItem items[ITEMS_COUNT]{
        {IDX_ICON_BLUETOOTH, SM_STRING_BLUETOOTH, SM_STRING_BLUETOOTH_SZ},
        {IDX_ICON_DATE, SM_STRING_DATE, SM_STRING_DATE_SZ},
        {IDX_ICON_TIME, SM_STRING_TIME, SM_STRING_TIME_SZ}
    };
};

#endif // SM_MAIN_MENU_H
