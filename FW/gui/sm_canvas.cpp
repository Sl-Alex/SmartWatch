#include "sm_canvas.h"

#define abs(a) ((a)>0?(a):(-(a)))

#define swap_int(a, b) { int t = a; a = b; b = t; }

void SmCanvas::setPix(int x, int y, char value)
{
    if ((x < 0) || (x >= getWidth())) return;
    if ((y < 0) || (y >= getHeight())) return;
#if (BPP == 8)
    getPData()[y * getWidth() + x] = value;
#elif (BPP == 1)
#ifdef PACK_VERT
    if (value)
    {
        getPData()[(y/8)*getWidth()+x] |=   0x01 << (y % 8);
    }
    else
    {
        getPData()[(y/8)*getWidth()+x] &=~ (0x01 << (y % 8));
    }
#else
#warning "Not tested yet"
    if (value)
    {
        getPData()[y*getWidth()+x/8] |=  0x01 << (x % 8);
    }
    else
    {
        getPData()[y*getWidth()+x/8] &= (0x01 << (x % 8));
    }
#endif
#endif
}

char SmCanvas::getPix(int x, int y)
{
    if ((x < 0) || (x >= getWidth())) return 0;
    if ((y < 0) || (y >= getHeight())) return 0;
#if (BPP == 8)
    return getPData()[y * getWidth() + x];
#elif (BPP == 1)
#ifdef PACK_VERT
    return (getPData()[(y/8)*getWidth()+x] >> (y % 8)) & 0x01;
#else
#warning "Not tested yet"
    return (getPData()[y*getWidth()+x/8] >> (x % 8)) & 0x01;
#endif
#endif
}

void SmCanvas::drawCanvas(int x, int y, int xOff, int yOff, int w, int h, SmCanvas * source)
{
    int x_left = 0;
    int x_right = w;
    int y_top = 0;
    int y_bottom = h;

    if (x < 0) x_left = -x;
    if (y < 0) y_top = -y;
    if ((x + x_right) > getWidth()) x_right = getWidth() - x;
    if ((y + y_bottom) > getHeight()) y_bottom = getHeight() - y;

    for (int _y = y_top; _y < y_bottom; _y++)
    {
        for (int _x = x_left; _x < x_right; _x++)
        {
            setPix(x+_x, y+_y, source->getPix(xOff + _x, yOff + _y));
        }
    }
}

void SmCanvas::drawLine(int x1, int y1, int x2, int y2, char value)
{
    if (x1 == x2)
    {
        drawVLine(x1,y1,y2,value);
        return;
    }
    if (y1 == y2)
    {
        drawHLine(x1,x2,y1,value);
        return;
    }

    int steep = abs(y2 - y1) > abs(x2 - x1);
    if (steep)
    {
        swap_int(x1, y1);
        swap_int(x2, y2);
    }

    if (x1 > x2)
    {
        swap_int(x1, x2);
        swap_int(y1, y2);
    }

    int dx, dy;
    dx = x2 - x1;
    dy = abs(y2 - y1);

    int err = dx / 2;
    int ystep;

    if (y1 < y2)
    {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x1<=x2; x1++)
    {
        if (steep)
        {
            setPix(y1, x1, value);
        } else {
            setPix(x1, y1, value);
        }
        err -= dy;
        if (err < 0)
        {
            y1 += ystep;
            err += dx;
        }
    }
}

void SmCanvas::drawHLine(int x1, int x2, int y, char value)
{
    if (x1 > x2)
        swap_int(x1,x2);
    for (int i = x1; i <= x2; i++)
    {
        setPix(i, y, value);
    }
}

void SmCanvas::drawVLine(int x, int y1, int y2, char value)
{
    if (y1 > y2)
        swap_int(y1,y2);
    for (int i = y1; i <= y2; i++)
    {
        setPix(x, i, value);
    }
}

void SmCanvas::drawRect(int x1, int y1, int x2, int y2, char value)
{
    drawHLine(x1, x2, y1, value);
    drawHLine(x1, x2, y2, value);
    drawVLine(x1, y1, y2, value);
    drawVLine(x2, y1, y2, value);
}

void SmCanvas::fillRect(int x1, int y1, int x2, int y2, char value)
{
    for (int x = x1; x <= x2; x++)
    {
        for (int y = y1; y <= y2; y++)
        {
            setPix(x,y,value);
        }
    }
}

void SmCanvas::scrollArea(int x1, int y1, int x2, int y2, int pixels, ScrollDirection dir)
{
    int _x1, _y1;
    int _x2, _y2;
    switch( dir )
    {
        case DIR_LEFT:
            _x1 = x1;
            _x2 = x2 - pixels;
            _y1 = y1;
            _y2 = y2;
            if (_x2 < _x1)
            {
                // Fill x1 to x2 with blank
            }
            else
            {
                for (int x = _x1; x <= _x2; x++)
                {
                    for (int y = _y1; y <= _y2; y++)
                    setPix(x, y, getPix(x + pixels, y));
                }
            }
            break;
        case DIR_RIGHT:
            _x1 = x1 + pixels;
            _x2 = x2;
            _y1 = y1;
            _y2 = y2;
            if (_x2 < _x1)
            {
                // Fill x1 to x2 with blank
            }
            else
            {
                for (int x = _x2; x >= _x1; x--)
                {
                    for (int y = _y1; y <= _y2; y++)
                    setPix(x, y, getPix(x - pixels, y));
                }
            }
            break;
        case DIR_UP:
            _x1 = x1;
            _x2 = x2;
            _y1 = y1;
            _y2 = y2 - pixels;
            if (_y2 < _y1)
            {
                // Fill y1 to y2 with blank
            }
            else
            {
                for (int y = _y1; y <= _y2; y++)
                {
                    for (int x = _x1; x <= _x2; x++)
                    setPix(x, y, getPix(x, y + pixels));
                }
            }
            break;
        case DIR_DOWN:
            _x1 = x1;
            _x2 = x2;
            _y1 = y1 + pixels;
            _y2 = y2;
            if (_y2 < _y1)
            {
                // Fill x1 to x2 with blank
            }
            else
            {
                for (int y = _y2; y >= _y1; y--)
                {
                    for (int x = _x1; x <= _x2; x++)
                    setPix(x, y, getPix(x, y - pixels));
                }
            }
            break;
    }
}
