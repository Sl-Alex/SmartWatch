#ifndef SM_NOTIFICATION_H
#define SM_NOTIFICATION_H

#include "sm_canvas.h"
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_strings.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_keyboard.h"
#include "sm_hw_storage.h"
#include "sm_edit_menu.h"

class SmNotification: public SmHalSysTimerIface, public SmHwKeyboardIface
{
public:
    SmNotification(SmHwKeyboardIface * parent);
    ~SmNotification();

    /// @brief System timer event
    void onTimer(uint32_t timeStamp);

    /// @brief onKeyDown event.
    void onKeyDown(SmHwButtons key);
    /// @brief onKeyUp event.
    void onKeyUp(SmHwButtons key);
private:

    void drawItem(void);

    struct SmMenuItem {
        uint16_t * pHeader;
        uint16_t * pText;
        uint16_t headerSymbolsCount;
        uint16_t textSymbolsCount;
    };

    SmCanvas * pCanvas;
    SmHwKeyboardIface * pParent;
    SmAnimator notificationAnimator;

};

#endif // SM_NOTIFICATION_H
