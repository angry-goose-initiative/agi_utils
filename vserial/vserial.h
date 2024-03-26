#pragma once
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <variant>

namespace agi::vserial {

    /**
     * @brief A function to open a pseudo terminal.
     * 
     * @return Struct containing the fd and the filePath
     */

    struct Res {
        std::string file_path;
        int fd;
    };


    struct Error_Res {
        int open_pt = 0;
        int grantpt = 0;
        int unlockpt = 0;
        bool ptsname = false;
    };


    std::variant<agi::vserial::Res, agi::vserial::Error_Res> create_pseudo_serial();
} // namespace agi::vserial
