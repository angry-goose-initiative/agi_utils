/*
 * Copyright (C) 2024 Sam Graham
 * See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace agi::error_detection {

/**
 * @brief Compute 8 bit checksum on data buffer using polynomial division based CRC-8 algorithm
 * 
 * @param data
 * @return 8 bit CRC checksum
 */
uint8_t crc8(std::span<uint8_t> data);

} // namespace agi::error_detection
