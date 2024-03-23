/*
 * Copyright (C) 2023-2024 John Jekel
 * Copyright (C) 2023-2024 Nick Chan
 * Copyright (C) 2024      Sam Graham
 * See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace agi::loaders {

enum class Res {
    OKAY,
    FILE_OPEN_ERROR,
    FILE_FORMAT_ERROR,
    WRITE_ERROR
};

class BaseWriteFunctor {
public:
    // Returns whether the write was successful or not
    virtual bool operator()(size_t address, uint8_t data) = 0;
};

class BufferWriteFunctor : public BaseWriteFunctor {
public:
    BufferWriteFunctor() = delete;
    BufferWriteFunctor(uint8_t * buf_ptr, size_t buf_size, size_t buf_offset = 0)
        : buf_ptr_(buf_ptr)
        , buf_size_(buf_size)
        , buf_offset_(buf_offset)
    {}

    bool operator()(size_t address, uint8_t data) override {
        size_t const buf_idx = address - this->buf_offset_;
        if (buf_idx >= this->buf_size_) {
            return false;
        } else {
            this->buf_ptr_[buf_idx] = data; // NOLINT(*-pointer-arithmetic)
            return true;
        }
    }
private:
    uint8_t * buf_ptr_;
    size_t    buf_size_;
    size_t    buf_offset_;
};

Res elf_32(std::string_view file_name, BaseWriteFunctor & write_functor);

Res verilog_32(std::string_view file_name, BaseWriteFunctor & write_functor);

} // namespace agi::loaders
