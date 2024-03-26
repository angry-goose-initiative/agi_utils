/*
 * Copyright (C) 2024 Seb Atkinson
 * See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

#include <string>
#include <variant>

namespace agi::vserial {

/**
 * @brief A function to open a pseudo terminal.
 * 
 * @return Struct containing the fd and the file path
 */

struct PtInfo {
    std::string file_path;
    int fd;
};

struct Error {
    enum class SysCall {
        OPEN,
        GRANTPT,
        UNLOCKPT,
        PTSNAME
    };
    SysCall sys_call;
    int     errno_val;
};

std::variant<agi::vserial::PtInfo, agi::vserial::Error> create_pseudo_serial();

} // namespace agi::vserial
