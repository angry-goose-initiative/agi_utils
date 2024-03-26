/*
 * Copyright (C) 2024 Seb Atkinson
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "vserial.h"

#include <array>
#include <fcntl.h>
#include <unistd.h>
#include <variant>

std::variant<agi::vserial::PtInfo, agi::vserial::Error> agi::vserial::create_pseudo_serial() {
    const int pt_fd = open("/dev/ptmx", O_RDWR | O_NOCTTY); // NOLINT(*-signed-bitwise)
    if (pt_fd < 0) {
        return agi::vserial::Error{
            .sys_call = agi::vserial::Error::SysCall::OPEN,
            .errno_val = errno
        };
    }

    // Grant access to the slave pseudo-terminal
    if (grantpt(pt_fd) < 0) {
        close(pt_fd);
        return agi::vserial::Error{
            .sys_call = agi::vserial::Error::SysCall::GRANTPT,
            .errno_val = errno
        };
    }

    // Unlock the pseudo-terminal
    if (unlockpt(pt_fd) < 0) {
        close(pt_fd);
        return agi::vserial::Error{
            .sys_call = agi::vserial::Error::SysCall::UNLOCKPT,
            .errno_val = errno
        };
    }

    // Get the name of the slave pseudo-terminal
    constexpr int MAX_PTSNAME_LEN = 100;
    std::array<char, MAX_PTSNAME_LEN> pts_name{};

    if (ptsname_r(pt_fd, pts_name.data(), pts_name.size()) < 0) {
        close(pt_fd);
        return agi::vserial::Error{
            .sys_call = agi::vserial::Error::SysCall::PTSNAME,
            .errno_val = errno
        };
    }

    return agi::vserial::PtInfo{
        .file_path = pts_name.data(),
        .fd = pt_fd
    };
}
