#ifndef SM_EDITOR_H
#define SM_EDITOR_H

#include <vector>
#include <cstdint>
#include "sm_canvas.h"
#include "sm_font.h"
#include "sm_hw_keyboard.h"
#include "sm_hw_storage.h"

/// @brief Simple editor class
/// @details SmartWatch does not have a real keyboard, only up and down keys
/// This means that the only way to implement an editor is to move up and down
/// in the list of the defined values and to select the right one.
class SmEditor : public SmCanvas, public SmHwKeyboardIface
{
public:
    /// @brief Editor item
    struct SmEditorItem {
        uint16_t * data; ///< Item text
        uint16_t length; ///< Item text length (number of uint16_t symbols)
    };
    SmEditor();
    ~SmEditor();
    /// @brief Initialize editor from the external vector of items
    void init(const std::vector<SmEditorItem> &items);
    /// @brief Set focus on the editor
    /// @details Up/Down arrows will be shown
    void setFocus(void);
    /// @brief Clear focus
    /// @details Up/Down arrows will be hidden
    void clearFocus(void);

    /// @brief Get the minimum and the maximum item index for this editor.
    void setMinMax(uint32_t min, uint32_t max);
    /// @brief Get current selection
    uint32_t getSelection(void) { return mSelected; }
    /// @brief Set current selection
    void setSelection(uint32_t selection);

    /// @brief Keyboard event handler
    void onKeyDown(SmHwButtons key);
    /// @brief Keyboard event handler
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
