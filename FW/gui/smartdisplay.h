#ifndef SMARTDISPLAY_H
#define SMARTDISPLAY_H

/*
class SmartDisplay
{
public:
    static void init(void);
    static void update(void);
    static SmartCanvas * canvas;
private:
    static SmartDisplay * pInstance;
    SmartDisplay(){}
    SmartDisplay(const SmartDisplay&);
    SmartDisplay& operator=(SmartDisplay&);
    static inline SmartDisplay * getInstance()
    {
        if(!pInstance)
        {
            pInstance = new SmartDisplay();
            canvas = new SmartCanvas();
        }
        return pInstance;
    }
};

*/
#ifdef __cplusplus
extern "C" {
#endif
void SmartDisplay_init();
void SmartDisplay_update();
void SmartDisplay_setPix(int x, int y, int value);
#ifdef __cplusplus
}
#endif

#endif /* SMARTDISPLAY_H */
