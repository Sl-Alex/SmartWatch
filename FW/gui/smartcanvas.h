#ifndef SMARTCANVAS_H
#define SMARTCANVAS_H

#include "string.h"

class SmartCanvas
{
public:
    typedef enum {
        DIR_LEFT,
        DIR_RIGHT,
        DIR_UP,
        DIR_DOWN
    } ScrollDirection;

    struct StorageHeader {
        int width;
        int height;
        int size;
    };

    SmartCanvas()
        :width(0),
        height(0),
        size(0) {}

    int width;
    int height;
    int size;

    void init(int width, int height);
    bool init(int imageIndex);

    // Drawing functions
    void clear(void)
    {
        memset(pData, 0, size);
    }
    void setPix(int x, int y, char value);
    char getPix(int x, int y);
//    void drawCanvas(int x, int y, SmartCanvas * source);
    void drawCanvas(int x, int y, int xOff, int yOff, int w, int h, SmartCanvas * source);
    inline void drawCanvas(int x, int y, SmartCanvas * source)
        {drawCanvas(x, y, 0, 0, source->width, source->height, source);}
    void drawLine(int x1, int x2, int y1, int y2, char value);
    void drawHLine(int x1, int x2, int y, char value);
    void drawVLine(int x, int y1, int y2, char value);
    void drawRect(int x1, int y1, int x2, int y2, char value);
    void scrollArea(int x1, int y1, int x2, int y2, int pixels, ScrollDirection dir, bool clear);

    char * pData;
};

#endif // SMARTCANVAS_H
