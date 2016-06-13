#include "smartfont.h"

SmartFont* SmartFont::pInstance = 0;
int SmartFont::symbolWidth = -1;

bool SmartFont::loadFont(void)
{
    symbolWidth = -1;
    if (getInstance()->init(2) == false) return false;
    if (pInstance->getWidth() % 256)
        return false;
    if (pInstance->getWidth() < 256)
        return false;

    symbolWidth = pInstance->getWidth() / 256;

    return true;
}

int SmartFont::drawSymbol(SmartCanvas * canvas, int x, int y, char symbol)
{
    if ((getInstance()->getSize() % 256) != 0)
        return -1;

//    canvas->drawCanvas(x,y,
//                       symbol * symbolWidth, 0,
//                       symbolWidth,
//                       8,
//                       getInstance());
    return pInstance->getWidth();
}

int SmartFont::drawText(SmartCanvas * canvas, int x, int y, char * symbol)
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
        x_off += symbolWidth;
        if (x_off > canvas->getWidth())
        {
            x_off = canvas->getWidth();
            break;
        }
        symbol++;
    }

    return x_off - x;
}
