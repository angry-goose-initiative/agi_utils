#pragma once

#include <cstdint>

namespace agi::error_detection {

/**
 * @brief Compute 8 bit checksum on data buffer using polynomial division based CRC-8 algorithm
 * 
 * @param data, length
 * @return 8 bit CRC checksum
 */
uint8_t crc8(const uint8_t* data, std::size_t length);

} // namespace agi::loaders
