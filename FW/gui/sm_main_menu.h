#ifndef SM_MAIN_MENU_H
#define SM_MAIN_MENU_H

#include "sm_canvas.h"
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_strings.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_keyboard.h"

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
    struct SmMenuItem {
        uint8_t imageIndex;
        uint16_t * pText;
        uint16_t symbolsCount;
    };

    SmCanvas * pCanvas;
    SmCanvas menuForeground;
    uint16_t * pMenuText;
    SmHwKeyboardIface * pParent;
    SmAnimator menuAnimator;
    SmImage * pNewImage;

    SmMenuItem cnt[2]{
        {3, SM_STRING_BLUETOOTH, SM_STRING_BLUETOOTH_SZ},
        {4, SM_STRING_BRIGHTNESS, SM_STRING_BRIGHTNESS_SZ}
    };
};

#endif // SM_MAIN_MENU_H
