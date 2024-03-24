#include "../vserial.h"
#include <string>
#include <iostream>
#include <optional>
#include <fcntl.h>
#include <unistd.h>


/**
 * @brief A program that shows how the creat_psudeo_serial() function can be used. 
 * To compile, run g++ -std=c++20 vserial.cc ./examples/vserial_print.cc
 * 
 */
int main(){
    agi::vserial::Res pseudo_terminal = agi::vserial::create_pseudo_serial();
    if(pseudo_terminal.fd.has_value()){

        std::cout<<"The serial port can be found at "<<*(pseudo_terminal.file_path)<<std::endl;
        char buffer[256];
        while (true) {
            ssize_t bytesRead = read(*(pseudo_terminal.fd), buffer, sizeof(buffer));
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

        close(*(pseudo_terminal.fd));
    }else{
        std::cerr << "Error creating the serial port" << std::endl;
    }
    return 0;
}
