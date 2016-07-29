#ifndef SM_CRC_H
#define SM_CRC_H

#include <cstdint>

/// @brief Singleton class for CRC8 and CRC32 calculation
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

    /// @brief Calculate CRC8 checksum
    /// @details Poly 0x32 was used. CRC result is not XOR-ed.
    static uint8_t  calc8 (uint8_t  init, const uint8_t * data, uint32_t size);
    
    /// @brief Calculate CRC32 checksum
    /// @details Poly 0x04C11DB7 was used. CRC result is not XOR-ed.
    static uint32_t calc32(uint32_t init, const uint8_t * data, uint32_t size);

private:
    SmCrc() {}
};

#endif /* SM_CRC_H */
