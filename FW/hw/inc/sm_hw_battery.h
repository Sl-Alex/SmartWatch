#ifndef SM_HW_BATTERY_H_INCLUDED
#define SM_HW_BATTERY_H_INCLUDED

#include <cstdint>

class SmHwBattery
{
public:
    static void init(void);
    static uint32_t readValue(void);
};


#endif /* SM_HW_BATTERY_H_INCLUDED */
