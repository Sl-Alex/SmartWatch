#ifndef SM_HW_MOTOR_H_INCLUDED
#define SM_HW_MOTOR_H_INCLUDED

#include "sm_hal_abstract_gpio.h"
#include "sm_hal_sys_timer.h"

class SmHwMotor: public SmHalSysTimerIface
{
public:
    void init(SmHalAbstractGpio * pin);
private:
    void onTimer(uint32_t timeStamp);
    SmHalAbstractGpio * mGpio;
};

#endif /* SM_HW_MOTOR_H_INCLUDED */
