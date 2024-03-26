/*
 * Copyright (C) 2024 Seb Atkinson
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "../vserial.h"

#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <variant>

/**
 * @brief A program that shows how the creat_psudeo_serial() function can be used. 
 * To compile, run g++ -std=c++20 vserial.cc ./examples/vserial_print.cc
 * 
 */
int main()
{
    auto const pt_return = agi::vserial::create_pseudo_serial();
    if (std::holds_alternative<agi::vserial::PtInfo>(pt_return)) {
        auto const pt_info = std::get<agi::vserial::PtInfo>(pt_return);
        std::cout << "The serial port can be found at " << pt_info.file_path << std::endl;
        char buffer[256];
        while (true) {
            ssize_t const bytes_read = read(pt_info.fd, buffer, sizeof(buffer));
            if (bytes_read > 0) {
                // Data received, do something with it
                std::cout << buffer << std::flush;
            } else if (bytes_read == 0) {
                // No more data to read, exit loop
                break;
            } else {
                // Error occurred
                std::cerr << "Error reading from serial port" << std::endl;
                break;
            }
        }
        close(pt_info.fd);
    } else {
        auto const pt_error = std::get<agi::vserial::Error>(pt_return);
        switch (pt_error.sys_call) {
            case agi::vserial::Error::SysCall::OPEN:
                std::cout << "Error opening /dev/ptmx" << std::endl;
                break;
            case agi::vserial::Error::SysCall::GRANTPT:
                std::cout << "Error calling grantpt" << std::endl;
                break;
            case agi::vserial::Error::SysCall::UNLOCKPT:
                std::cout << "Error calling unlockpt" << std::endl;
                break;
            case agi::vserial::Error::SysCall::PTSNAME:
                std::cout << "Error calling ptsname" << std::endl;
        }
    }
    return 0;
}
