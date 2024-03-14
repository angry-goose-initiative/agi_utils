#pragma once

#include <string_view>
#include <cstdint>

namespace agi::loaders {

enum class Res {
    OKAY,
    FILE_OPEN_ERROR,
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

    virtual bool operator()(size_t address, uint8_t data) {
        size_t const buf_idx = address - this->buf_offset_;
        if (buf_idx >= this->buf_size_) {
            return false;
        } else {
            this->buf_ptr_[buf_idx] = data;
            return true;
        }
    }
private:
    uint8_t * buf_ptr_;
    size_t    buf_size_;
    size_t    buf_offset_;
};

Res elf_32(std::string_view file_name, BaseWriteFunctor & write_functor);

} // namespace agi_utils::loaders
