#include "smartcanvas.h"
#include "smartstorage.h"

#define abs(a) ((a)>0?(a):(-(a)))

#define swap_int(a, b) { int t = a; a = b; b = t; }

inline static int calcSize(int width, int height)
{
#if (BPP == 8)
    #error "Wrong BPP"
    return width * height;
#elif (BPP == 1)
#ifdef PACK_VERT
    return width * ((height + 7)/8); // ceil() for height
#else
    return ((width + 7)/8) * height; // ceil() for width
#endif
#endif
}

void SmartCanvas::init(int width, int height)
{
    this->width = width;
    this->height = height;

    if (size)
        delete[] pData;

    size = calcSize(width, height);
    pData = new char[size];
}

bool SmartCanvas::init(int imageIndex)
{
    if (size != 0)
        delete[] pData;

    StorageHeader header;
    if (SmartStorage::getSize(imageIndex) < 0)
        return false;
    if (SmartStorage::loadData(imageIndex,0,sizeof(StorageHeader),(char *)&header) < 0)
        return false;

    if (header.size != calcSize(header.width,header.height)) return false;

    size = header.size;
    width = header.width;
    height = header.height;

    pData = new char[size];
    if (SmartStorage::loadData(imageIndex,sizeof(StorageHeader),size,pData) < 1)
        return false;

    return true;
}

void SmartCanvas::setPix(int x, int y, char value)
{
    if ((x < 0) || (x >= width)) return;
    if ((y < 0) || (y >= height)) return;
#if (BPP == 8)
    pData[y * width + x] = value;
#elif (BPP == 1)
#ifdef PACK_VERT
#warning "Not tested yet"
    if (value)
    {
        pData[(y/8)*width+x] |=   0x01 << (y % 8);
    }
    else
    {
        pData[(y/8)*width+x] &=~ (0x01 << (y % 8));
    }
#else
#warning "Not tested yet"
    if (value)
    {
        pData[y*width+x/8] |=  0x01 << (x % 8);
    }
    else
    {
        pData[y*width+x/8] &= (0x01 << (x % 8));
    }
#endif
#endif
}

char SmartCanvas::getPix(int x, int y)
{
    if ((x < 0) || (x >= width)) return 0;
    if ((y < 0) || (y >= height)) return 0;
#if (BPP == 8)
    return pData[y * width + x];
#elif (BPP == 1)
#ifdef PACK_VERT
#warning "Not tested yet"
    return (pData[(y/8)*width+x] >> (y % 8)) & 0x01;
#else
#warning "Not tested yet"
    return (pData[y*width+x/8] >> (x % 8)) & 0x01;
#endif
#endif
}

void SmartCanvas::drawCanvas(int x, int y, int xOff, int yOff, int w, int h, SmartCanvas * source)
{
    int x_left = 0;
    int x_right = w;
    int y_top = 0;
    int y_bottom = h;

    if (x < 0) x_left = -x;
    if (y < 0) y_top = -y;
    if ((x + x_right) > width) x_right = width - x;
    if ((y + y_bottom) > height) y_bottom = height - y;

    for (int _y = y_top; _y < y_bottom; _y++)
    {
        for (int _x = x_left; _x < x_right; _x++)
        {
            setPix(x+_x, y+_y, source->getPix(xOff + _x, yOff + _y));
        }
    }
}

void SmartCanvas::drawLine(int x1, int y1, int x2, int y2, char value)
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

void SmartCanvas::drawHLine(int x1, int x2, int y, char value)
{
    if (x1 > x2)
        swap_int(x1,x2);
    for (int i = x1; i <= x2; i++)
    {
        setPix(i, y, value);
    }
}

void SmartCanvas::drawVLine(int x, int y1, int y2, char value)
{
    if (y1 > y2)
        swap_int(y1,y2);
    for (int i = y1; i <= y2; i++)
    {
        setPix(x, i, value);
    }
}

void SmartCanvas::drawRect(int x1, int y1, int x2, int y2, char value)
{
    drawHLine(x1, x2, y1, value);
    drawHLine(x1, x2, y2, value);
    drawVLine(x1, y1, y2, value);
    drawVLine(x2, y1, y2, value);
}
