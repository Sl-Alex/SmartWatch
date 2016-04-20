#ifndef SMARTFONT_H
#define SMARTFONT_H

#include "smartcanvas.h"

class SmartFont: SmartCanvas
{
public:
    static bool loadFont(void);
    static int drawSymbol(SmartCanvas * canvas, int x, int y, char symbol);
    static int drawText(SmartCanvas * canvas, int x, int y, char * symbol);
    static inline SmartFont * getInstance()
    {
        if(!pInstance)
        {
            pInstance = new SmartFont();
        }
        return pInstance;
    }
private:
    static SmartFont * pInstance;
    static int symbolWidth;
    SmartFont(){}
    SmartFont(const SmartFont&);
    SmartFont& operator=(SmartFont&);
};

#endif // SMARTFONT_H
