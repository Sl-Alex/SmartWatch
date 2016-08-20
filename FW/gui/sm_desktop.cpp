#include "sm_desktop.h"

/// @todo Implement all services initialization
void SmDesktop::init(void)
{
    SmHwBattery::getInstance()->init();
    SmHalSysTimer::subscribe(this, 10, true);
}

void SmDesktop::onTimer(uint32_t timeStamp)
{
    mBatteryLevel = SmHwBattery::getInstance()->getValue();
}
