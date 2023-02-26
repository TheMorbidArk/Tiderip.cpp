//
// Created by MorbidArk on 2023/2/21.
//

#pragma once

#include <cstdint>

[[maybe_unused]]
uint32_t getByteNumOfEncodeUtf8(int value);

[[maybe_unused]]
uint32_t getByteNumOfDecodeUtf8(uint8_t byte);

[[maybe_unused]]
uint8_t encodeUtf8(uint8_t *buf, int value);

[[maybe_unused]]
int decodeUtf8(const uint8_t *bytePtr, uint32_t length);