/*
 * Copyright (C) 2023-2024 John Jekel
 * Copyright (C) 2023-2024 Nick Chan
 * Copyright (C) 2024      Sam Graham
 * See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

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
    BufferWriteFunctor(std::span<uint8_t> buffer, size_t offset = 0) // NOLINT(*explicit*)
        : buffer_(buffer)
        , offset_(offset)
    {}

    bool operator()(size_t address, uint8_t data) override {
        size_t const buf_idx = address - this->offset_;
        if (buf_idx >= this->buffer_.size()) {
            return false;
        } else {
            this->buffer_[buf_idx] = data;
            return true;
        }
    }
private:
    std::span<uint8_t> buffer_;
    size_t             offset_;
};

Res elf_32(std::string const & file_name, BaseWriteFunctor & write_functor);

Res verilog_32(std::string const & file_name, BaseWriteFunctor & write_functor);

} // namespace agi::loaders
