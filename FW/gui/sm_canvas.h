#ifndef SM_CANVAS_H
#define SM_CANVAS_H

#include <cstring>
#include "sm_texture.h"

/// @brief Canvas class
/// @todo Probably can be merged with @ref SmTexture
class SmCanvas: public SmTexture
{
public:
    typedef enum {
        DIR_LEFT,
        DIR_RIGHT,
        DIR_UP,
        DIR_DOWN
    } ScrollDirection;

    void setPix(int x, int y, char value);
    char getPix(int x, int y);
    void drawCanvas(int x, int y, int xOff, int yOff, int w, int h, SmCanvas * source);
    inline void drawCanvas(int x, int y, SmCanvas * source)
        {drawCanvas(x, y, 0, 0, source->getWidth(), source->getHeight(), source);}
    void drawLine(int x1, int x2, int y1, int y2, char value);
    void drawHLine(int x1, int x2, int y, char value);
    void drawVLine(int x, int y1, int y2, char value);
    void drawRect(int x1, int y1, int x2, int y2, char value);
    void fillRect(int x1, int y1, int x2, int y2, char value);
    void scrollArea(int x1, int y1, int x2, int y2, int pixels, ScrollDirection dirclear);
};

#endif // SM_CANVAS_H
