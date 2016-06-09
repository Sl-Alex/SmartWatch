#ifndef SMDISPLAY_H
#define SMDISPLAY_H

#include "sm_texture.h"
#include "sm_hw_abstract_gpio.h"
#include "sm_hw_abstract_spi.h"

class SmDisplay
{
public:
    SmDisplay();
    ~SmDisplay();
    void init(int width, int height);
    void setInterface(SmHwAbstractSpi * spi, SmHwAbstractGpio * dc);
    void setPix(int x, int y, int value);
    void update(void);
private:
    void sendCommand(uint8_t cmd);
    void sendCommand(uint8_t cmd, uint8_t data);
    SmTexture * texture;
    SmHwAbstractSpi * mSpi;
    SmHwAbstractGpio * mDcPin;
};

#endif /* SMDISPLAY_H */
