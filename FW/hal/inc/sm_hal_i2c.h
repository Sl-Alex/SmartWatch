#ifndef SM_HAL_I2C_H_INCLUDED
#define SM_HAL_I2C_H_INCLUDED

#include "stm32f10x.h"

class SmHalI2c
{
public:
    void init(void);
    void reset(bool complete);
    bool transfer(uint8_t addr, uint8_t * wr, uint8_t wr_size, uint8_t * rd, uint8_t rd_size);

    SmHalI2c(SmHalI2c const&) = delete;
    void operator=(SmHalI2c const&) = delete;
    static SmHalI2c* getInstance()
    {
        static SmHalI2c instance; // Guaranteed to be destroyed.
                                       // Instantiated on first use.
        return &instance;
    }
private:
    SmHalI2c() {}
    bool stop(void);
    bool start(void);
    bool sendAddrRd(uint16_t addr);
    bool sendAddrWr(uint16_t addr);
    bool writeData(uint8_t * data, uint32_t datasize);
    bool checkEvent(uint32_t event);
    bool getFlagStatus(uint32_t flag);
};

#endif /* SM_HAL_I2C_H_INCLUDED */
