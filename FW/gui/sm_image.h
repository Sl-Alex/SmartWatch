#ifndef SM_IMAGE_H
#define SM_IMAGE_H

#include <cstring>
#include "sm_canvas.h"

class SmImage: public SmCanvas
{
public:
    typedef enum {
        DIR_LEFT,
        DIR_RIGHT,
        DIR_UP,
        DIR_DOWN
    } ScrollDirection;

    struct ImageHeader {
        int width;
        int height;
        int size;
    };

    virtual bool init(int imageIndex);
};

#endif // SM_IMAGE_H
