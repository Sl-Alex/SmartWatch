#ifndef SMDISPLAY_H
#define SMDISPLAY_H

#include "sm_texture.h"
#include "sm_hal_abstract_gpio.h"
#include "sm_hal_abstract_spi.h"

class SmDisplay
{
public:
    SmDisplay();
    ~SmDisplay();
    void init(int width, int height, SmHalAbstractSpi * spi, SmHalAbstractGpio * dc, SmHalAbstractGpio * power);
    void setPix(int x, int y, int value);
    void update(void);
    void fill(uint8_t data);
private:
    void sendCommand(uint8_t cmd);
    void sendCommand(uint8_t cmd, uint8_t data);
    void sendData(uint8_t * data, uint8_t size);
    void powerOn(void);
    void powerOff(void);
    SmTexture * texture;
    SmHalAbstractSpi * mSpi;
    SmHalAbstractGpio * mDcPin;
    SmHalAbstractGpio * mPowerPin;
};

#endif /* SMDISPLAY_H */
