/*
 * Copyright (C) 2024 Sam Graham
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "error_detection.h"

#include <climits>

uint8_t agi::error_detection::crc8(std::span<uint8_t> data) {
    constexpr uint8_t CRC8_POLYNOMIAL = 0x07; // CRC-8 Generator polynomial
    constexpr uint8_t CRC8_MSB = (1U << 7U); // Most significat bit of byte

    uint8_t crc = 0x00;

    for (const uint8_t val : data) {
        crc ^= val;
        for (int b = 0; b < CHAR_BIT; b++) {
            if (crc & CRC8_MSB) {
                crc = (crc << 1U) ^ CRC8_POLYNOMIAL; // NOLINT(*-signed-bitwise)
            } else {
                crc <<= 1U;
            }
        }
    }

    return crc;
}
