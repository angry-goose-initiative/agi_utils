/*
 * Copyright (C) 2024 Sam Graham
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "error_detection.h"

uint8_t agi::error_detection::crc8(const uint8_t* data, std::size_t length) {
    constexpr uint8_t CRC8_POLYNOMIAL = 0x07; // CRC-8 Generator polynomial
    constexpr uint8_t CRC8_MSB = (1U << 7U); // Most significat bit of byte

    uint8_t crc = 0x00;

    for (std::size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            if (crc & CRC8_MSB) {
                crc = (crc << 1U) ^ CRC8_POLYNOMIAL;
            } else {
                crc <<= 1U;
            }
        }
    }

    return crc;
}
