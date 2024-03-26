/*
 * Copyright (C) 2024 Seb Atkinson
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "../vserial.h"
#include <string>
#include <iostream>
#include <variant>
#include <fcntl.h>
#include <unistd.h>


/**
 * @brief A program that shows how the creat_psudeo_serial() function can be used. 
 * To compile, run g++ -std=c++20 vserial.cc ./examples/vserial_print.cc
 * 
 */
int main(){
    std::variant<agi::vserial::Res, agi::vserial::Error_Res> pseudo_terminal_return = agi::vserial::create_pseudo_serial();
    if(std::holds_alternative<agi::vserial::Res>(pseudo_terminal_return)){
        agi::vserial::Res pseudo_terminal = std::get<agi::vserial::Res>(pseudo_terminal_return);
        std::cout<<"The serial port can be found at "<<pseudo_terminal.file_path<<std::endl;
        char buffer[256];
        while (true) {
            ssize_t bytesRead = read(pseudo_terminal.fd, buffer, sizeof(buffer));
            if (bytesRead > 0) {
                // Data received, do something with it
                std::cout<<buffer<<std::flush;
            } else if (bytesRead == 0) {
                // No more data to read, exit loop
                break;
            } else {
                // Error occurred
                std::cerr << "Error reading from serial port" << std::endl;
                break;
            }
        }
        close(pseudo_terminal.fd);
    }else{
        agi::vserial::Error_Res pseudo_terminal_error = std::get<agi::vserial::Error_Res>(pseudo_terminal_return);
        if(pseudo_terminal_error.open_pt != 0){
            std::cout<<"Error opening /dev/ptmx"<<std::endl;
        }else if(pseudo_terminal_error.grantpt != 0){
            std::cout<<"Error calling grantpt"<<std::endl;
        }else if(pseudo_terminal_error.unlockpt != 0){
            std::cout<<"Error calling unlockpt"<<std::endl;
        }else if(pseudo_terminal_error.ptsname){
            std::cout<<"Error calling ptsname"<<std::endl;
        }
    }
    return 0;
}
