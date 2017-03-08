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

class SmNotification: public SmHwKeyboardIface
{
public:
    SmNotification(SmHwKeyboardIface * parent, SmText header, SmText text);
    ~SmNotification();

    /// @brief onKeyDown event.
    void onKeyDown(SmHwButtons key);
    /// @brief onKeyUp event.
    void onKeyUp(SmHwButtons key);

    inline void addNotification(SmText header, SmText text)
    {
        /// @TODO Check how many notifications do we have
        mHeader.push_back(header);
        mText.push_back(text);
        drawItem();
#ifndef PC_SOFTWARE
        SmHwPowerMgr::getInstance()->allowSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_MENU,
                                                SmHwPowerMgr::SleepTimeout::SM_HW_SLEEP_EXTRA_LONG);
#endif
    }
private:

    void drawItem(void);

    SmCanvas * pCanvas;
    SmHwKeyboardIface * pParent;

    std::vector<SmText> mHeader;
    std::vector<SmText> mText;
};

#endif // SM_NOTIFICATION_H
