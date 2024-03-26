/*
 * Copyright (C) 2024 Seb Atkinson
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "vserial.h"



std::variant<agi::vserial::Res, agi::vserial::Error_Res> agi::vserial::create_pseudo_serial(){
    agi::vserial::Res return_value;

    int pt_fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
     if (pt_fd < 0) {
        agi::vserial::Error_Res error_return_value;
        error_return_value.open_pt = pt_fd;
        return error_return_value;
    }

    // Grant access to the slave pseudo-terminal
    if (grantpt(pt_fd) == -1) {
        close(pt_fd);
        agi::vserial::Error_Res error_return_value;
        error_return_value.grantpt = -1;
        return return_value;
    }

    // Unlock the pseudo-terminal
    if (unlockpt(pt_fd) == -1) {
        close(pt_fd);
        agi::vserial::Error_Res error_return_value;
        error_return_value.unlockpt = -1;
        return return_value;
    }

    // Get the name of the slave pseudo-terminal
    const char* pts_name = ptsname(pt_fd);

    // Check if ptsname returned a valid string
    if (pts_name == nullptr) {
        // Return empty values if ptsname failed
        close(pt_fd);
        agi::vserial::Error_Res error_return_value;
        error_return_value.ptsname = true;
        return error_return_value;
    }

    // Convert the C-style string to a C++ std::string
    return_value.file_path = std::string(pts_name);
    return_value.fd = pt_fd;
    return return_value;
}
