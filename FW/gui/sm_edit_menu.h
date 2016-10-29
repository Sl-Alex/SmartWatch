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
    enum EditGroup
    {
        EG_BLUETOOTH = 0,
        EG_DATE,
        EG_TIME
    };

    SmEditMenu(SmHwKeyboardIface * parent, EditGroup group );
    ~SmEditMenu();

    /// @brief System timer event
    void onTimer(uint32_t timeStamp);

    /// @brief onKeyDown event.
    void onKeyDown(SmHwButtons key);
    /// @brief onKeyUp event.
    void onKeyUp(SmHwButtons key);

    /// @brief get Result for the specified editor
    uint8_t getResult(uint8_t editorNum);
private:

    void gotoNext(void);
    void drawItems(void);

    static const int ITEMS_COUNT_MAX = 6;

    SmCanvas * pCanvas;
    SmHwKeyboardIface * pParent;
    SmAnimator menuAnimator;
    EditGroup mGroup;
    uint8_t itemsCount;

    SmEditor mEditors[ITEMS_COUNT_MAX];

    uint16_t * mHint;
    uint8_t mHintLen;

    uint8_t mSelected;

    bool mModified;
};

#endif // SM_EDIT_MENU_H
