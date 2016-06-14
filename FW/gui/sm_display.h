#ifndef SMDISPLAY_H
#define SMDISPLAY_H

#include "sm_texture.h"
#include "sm_hal_abstract_gpio.h"
#include "sm_hal_abstract_spi.h"
#include "sm_hw_powermgr.h"

class SmDisplay: public SmHwPowerMgrIface
{
public:
    SmDisplay();
    void init(int width, int height, SmHalAbstractSpi * spi, SmHalAbstractGpio * dc, SmHalAbstractGpio * power, SmHalAbstractGpio * reset);
    void setPix(int x, int y, int value);
    void update(void);
    void fill(uint8_t data);
private:
    void sendCommand(uint8_t cmd);
    void sendCommand(uint8_t cmd, uint8_t data);
    void sendData(uint8_t * data, uint8_t size);
    void powerOn(void);
    void powerOff(void);
    void onSleep(void);
    void onWake(void);
    SmTexture * texture;
    SmHalAbstractSpi * mSpi;
    SmHalAbstractGpio * mDcPin;
    SmHalAbstractGpio * mPowerPin;
    SmHalAbstractGpio * mResetPin;
};

#endif /* SMDISPLAY_H */
