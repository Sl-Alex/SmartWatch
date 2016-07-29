#include "sm_font.h"
#include "sm_hw_storage.h"

SmFont* SmFont::pInstance = 0;

int SmFont::drawSymbol(int index, SmCanvas * canvas, int x, int y, uint16_t symbol)
{
//    canvas->drawCanvas(x,y,
//                       symbol * symbolWidth, 0,
//                       symbolWidth,
//                       8,
//                       getInstance());
    return pInstance->getWidth();
}

int SmFont::drawText(SmCanvas * canvas, int x, int y, uint16_t * symbol)
{
    if (symbol == 0)
        return -1;
    if ((getInstance()->getSize() % 256) != 0)
        return -1;

    int x_off = x;

    while(*symbol)
    {
        // Draw one symbol
//        canvas->drawCanvas(x_off,y,
//                           (*symbol) * symbolWidth, 0,
//                           symbolWidth,
//                           pInstance->getHeight(),
//                           pInstance);
        // Go to the next symbol
        ///x_off += symbolWidth;
        if (x_off > canvas->getWidth())
        {
            x_off = canvas->getWidth();
            break;
        }
        symbol++;
    }

    return x_off - x;
}
