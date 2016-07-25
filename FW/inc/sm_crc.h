#ifndef SM_CRC_H
#define SM_CRC_H

#include <cstdint>

class SmCrc
{
public:
    SmCrc(SmCrc const&) = delete;
    void operator=(SmCrc const&) = delete;
    static SmCrc* getInstance()
    {
        static SmCrc instance; // Guaranteed to be destroyed.
                                        // Instantiated on first use.
        return &instance;
    }

    static uint8_t  calc8 (uint8_t  init, const uint8_t * data, uint32_t size);
    static uint32_t calc32(uint32_t init, const uint8_t * data, uint32_t size);

private:
    SmCrc() {}
};

#endif /* SM_CRC_H */
