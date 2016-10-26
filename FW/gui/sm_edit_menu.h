#ifndef SM_EDIT_MENU_H
#define SM_EDIT_MENU_H

#include "sm_canvas.h"
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_strings.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_keyboard.h"
#include "sm_hw_storage.h"
#include "sm_editor.h"

class SmEditMenu: public SmHalSysTimerIface, public SmHwKeyboardIface
{
public:
    SmEditMenu(SmHwKeyboardIface * parent);
    ~SmEditMenu();

    /// @brief System timer event
    void onTimer(uint32_t timeStamp);

    /// @brief onKeyDown event.
    void onKeyDown(SmHwButtons key);
    /// @brief onKeyUp event.
    void onKeyUp(SmHwButtons key);
private:

    void gotoNext(void);
    void drawItems(void);

    static const int ITEMS_COUNT = 4;

    SmCanvas * pCanvas;
    SmHwKeyboardIface * pParent;
    SmAnimator menuAnimator;

    SmEditor mEditors[ITEMS_COUNT];

    uint8_t mSelected;
};

#endif // SM_EDIT_MENU_H
