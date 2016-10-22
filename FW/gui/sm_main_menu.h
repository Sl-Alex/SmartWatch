#ifndef SM_MAIN_MENU_H
#define SM_MAIN_MENU_H

#include "sm_canvas.h"
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_strings.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_keyboard.h"
#include "sm_hw_storage.h"

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
    const int ITEMS_COUNT = 2;

    SmCanvas * pCanvas;
    uint16_t * pMenuText;
    SmHwKeyboardIface * pParent;
    SmAnimator menuAnimator;
    SmImage * pNewImage;

    uint8_t selected;

    SmMenuItem items[2]{
        {IDX_ICON_BLUETOOTH, SM_STRING_BLUETOOTH, SM_STRING_BLUETOOTH_SZ},
        {IDX_ICON_BRIGHTNESS, SM_STRING_BRIGHTNESS, SM_STRING_BRIGHTNESS_SZ}
    };
};

#endif // SM_MAIN_MENU_H
