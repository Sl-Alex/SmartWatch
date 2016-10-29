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

    void setMinMax(uint32_t min, uint32_t max);
    uint32_t getSelection(void) { return mSelected; }
    void setSelection(uint32_t selection);

    void onKeyDown(SmHwButtons key);
    void onKeyUp(SmHwButtons key);

private:
    void refreshImage(void);
    std::vector<SmEditorItem> mItems;
    SmFont mFont;
    bool mFocused;
    SmImage mArrowUp;
    SmImage mArrowDown;

    uint32_t mSelected;
    uint32_t mSelMin;
    uint32_t mSelMax;
};

#endif // SM_EDITOR_H
