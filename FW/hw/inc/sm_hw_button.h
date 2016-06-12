#ifndef SM_HW_BUTTON_H_INCLUDED
#define SM_HW_BUTTON_H_INCLUDED

#include <cstdint>
#include "sm_hal_abstract_gpio.h"
#include "sm_hal_sys_timer.h"

class SmHwButton: public SmHalSysTimerIface
{
public:
    void init(SmHalAbstractGpio * pin);
private:
    void onTimer(uint32_t timeStamp) {}
    SmHalAbstractGpio * mGpio;
};


#endif /* SM_HW_BUTTON_H_INCLUDED */
