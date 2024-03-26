/*
 * Copyright (C) 2024 Seb Atkinson
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "vserial.h"

#include <fcntl.h>
#include <unistd.h>
#include <variant>

std::variant<agi::vserial::PtInfo, agi::vserial::Error> agi::vserial::create_pseudo_serial()
{
    int const pt_fd = open("/dev/ptmx", O_RDWR | O_NOCTTY); // NOLINT(*-signed-bitwise)
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
    const char* pts_name = ptsname(pt_fd);

    // Check if ptsname returned a valid string
    if (pts_name == nullptr) {
        close(pt_fd);
        return agi::vserial::Error{
            .sys_call = agi::vserial::Error::SysCall::PTSNAME,
            .errno_val = errno
        };
    }

    return agi::vserial::PtInfo{
        .fd = pt_fd,
        .file_path = pts_name
    };
}
