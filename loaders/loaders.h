#pragma once

#include <string_view>
#include <cstdint>

namespace agi_util::loaders {

enum class Res {OKAY, ERROR};

Res elf_32(std::string_view file_name, uint8_t * buf_ptr, size_t buf_size, ssize_t offset);

} // namespace agi_utils::loaders
