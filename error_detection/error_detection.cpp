#include "error_detection.hpp"

namespace agi::error_detection {

uint8_t crc8(const uint8_t* data, std::size_t length) {
    static const uint8_t CRC8_POLYNOMIAL = 0x07; // CRC-8 Generator polynomial
    static const uint8_t CRC8_MSB = (1 << 7); // Most significat bit of byte

    uint8_t crc = 0x00;

    for (std::size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            if (crc & CRC8_MSB) {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

} // namespace agi::error_detection
