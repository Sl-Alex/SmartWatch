#ifndef SM_EDITOR_H
#define SM_EDITOR_H

#include <vector>
#include <cstdint>
#include "sm_canvas.h"
#include "sm_font.h"
#include "sm_hw_keyboard.h"
#include "sm_hw_storage.h"

class SmEditor : public SmCanvas, SmHwKeyboardIface
{
public:
    struct SmEditorItem {
        uint16_t * data;
        uint16_t length;
    };
    SmEditor();
    ~SmEditor();
    void init(const std::vector<SmEditorItem> &items);
    void setFocus(void);
    void clearFocus(void);

    void onKeyDown(SmHwButtons key);
    void onKeyUp(SmHwButtons key);

private:
    void refreshImage(void);
    std::vector<SmEditorItem> mItems;
    SmFont mFont;
    bool mFocused;
    uint32_t mSelected;
    SmImage mArrowUp;
    SmImage mArrowDown;
};

#endif // SM_EDITOR_H
