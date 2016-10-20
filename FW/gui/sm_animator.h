#ifndef SM_ANIMATOR_H
#define SM_ANIMATOR_H

#include "sm_canvas.h"

/// @brief GUI canvas/image animator class
/// @brief Can move/slide one canvas/image over another
class SmAnimator
{
public:
    /// @brief Animation type
    typedef enum {
        ANIM_TYPE_SHIFT,        ///< Shift background image and replace with foreground
        ANIM_TYPE_SLIDE,        ///< Slide foreground image over background
        ANIM_TYPE_VIS_SLIDE,    ///< Slide foreground image over background and hightlight transition
        ANIM_TYPE_APPEAR,       ///< Appear foreground image over background
        ANIM_TYPE_VIS_APPEAR    ///< Appear foreground image over background and hightlight transition
    } AnimType;

    /// @brief Animation direction
    typedef enum {
        ANIM_DIR_UP,    ///< Animate to the top
        ANIM_DIR_DOWN,  ///< Animate to the bottom
        ANIM_DIR_LEFT,  ///< Animate to the left
        ANIM_DIR_RIGHT  ///< Animate to the right
    } AnimDir;

    /// @brief Constructor
    SmAnimator()
        :mTick(0),
        mX(0),
        mY(0),
        mXOff(0),
        mYOff(0),
        mW(0),
        mH(0) {}

    /// @brief Set source (foreground) and destination canvas
    inline void setDestSource(SmCanvas * dest, SmCanvas * source) {pDest = dest; pSource = source;}
    /// @brief Set animation type
    inline void setType(AnimType type) {mType = type;}
    /// @brief Set animation direction
    inline void setDirection(AnimDir direction) {mDir = direction;}
    /// @brief Set animation speed
    inline void setSpeed(int speed) {mSpeed = speed;}
    /// @brief Set shift limit.
    /// @details In case of @ref ANIM_TYPE_SHIFT shift will work only for the specified limit of pixels
    /// You can set negative number, in this case shift will work until the specified number of pixels from the background canvas edge.
    inline void setShiftLimit(int pixels) {mLimit = pixels;}
    /// @brief Start animation
    /// @param x: X coordinate
    /// @param y: Y coordinate
    /// @param xOff: Offset in the foreground image
    /// @param yOff: Offset in the foreground image
    /// @param w: foreground image area width
    /// @param h: foreground image area height
    void start(int x, int y, int xOff, int yOff, int w, int h);
    /// @brief Display a single animation tick
    /// @returns False when animation is done
    bool tick(void);
    /// @brief Finish animation immediately
    /// @todo Implement
    void finish();

private:
    SmCanvas * pDest;
    SmCanvas * pSource;

    AnimType mType;
    AnimDir mDir;

    int mSpeed; /// @todo Implement
    int mTick;
    int mLimit;
    int mX, mY, mXOff, mYOff, mW, mH;
};

#endif // SM_ANIMATOR_H
