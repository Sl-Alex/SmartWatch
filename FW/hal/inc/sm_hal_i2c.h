#ifndef SM_HAL_I2C_H
#define SM_HAL_I2C_H

#include "stm32f10x.h"

/// @brief Built-in I<sup>2</sup>C abstraction
class SmHalI2c
{
public:
    /// @brief Initialize I<sup>2</sup>C HW.
    void init(void);
    
    /// @brief Reset I<sup>2</sup>C bus
    /// @details Checks for I<sup>2</sup>C bus problems. Resets bus and built-in peripheral if SDA is low.
    /// @param complete: Reset built-in peripheral even if everything is fine.
    void reset(bool complete);
    
    /// @brief Transfer data to the I<sup>2</sup>C device
    /// @param addr: Device address in 7-bit format, RW flag will be added automatically
    /// @param wr: Pointer to the write buffer
    /// @param wr_size: Write buffer size (bytes)
    /// @param rd: Pointer to the read buffer
    /// @param rd_size: Read buffer size (bytes)
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

#endif // SM_HAL_I2C_H
