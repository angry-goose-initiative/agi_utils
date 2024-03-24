#pragma once
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <optional>


namespace agi::vserial {

    /**
     * @brief A function to open a pseudo terminal.
     * 
     * @return Struct containing the fd and the filePath
     */

    struct Res {
        std::optional<std::string> file_path;
        std::optional<int> fd;
    };

    Res create_pseudo_serial();
} // namespace agi::vserial
