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
    void init(int width, int height, SmHwAbstractSpi * spi, SmHwAbstractGpio * dc, SmHwAbstractGpio * power);
    void setPix(int x, int y, int value);
    void update(void);
private:
    void sendCommand(uint8_t cmd);
    void sendCommand(uint8_t cmd, uint8_t data);
    void sendData(uint8_t * data, uint8_t size);
    void fill(uint8_t data);
    void powerOn(void);
    void powerOff(void);
    SmTexture * texture;
    SmHwAbstractSpi * mSpi;
    SmHwAbstractGpio * mDcPin;
    SmHwAbstractGpio * mPowerPin;
};

#endif /* SMDISPLAY_H */
