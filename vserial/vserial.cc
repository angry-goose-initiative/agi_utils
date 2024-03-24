#include "vserial.h"



agi::vserial::Res agi::vserial::create_pseudo_serial(){
    agi::vserial::Res return_value;

    int pt = open("/dev/ptmx", O_RDWR | O_NOCTTY);
     if (pt < 0) {
        perror("open /dev/ptmx");
        return_value.file_path = std::nullopt;
        return_value.fd = std::nullopt;
        return return_value;
    }

    // Grant access to the slave pseudo-terminal
    if (grantpt(pt) == -1) {
        perror("grantpt");
        close(pt);
        return_value.file_path = std::nullopt;
        return_value.fd = std::nullopt;
        return return_value;
    }

    // Unlock the pseudo-terminal
    if (unlockpt(pt) == -1) {
        perror("unlockpt");
        close(pt);
        return_value.file_path = std::nullopt;
        return_value.fd = std::nullopt;
        return return_value;
    }

    // Get the name of the slave pseudo-terminal
    const char* pts_name = ptsname(pt);

    // Check if ptsname returned a valid string
    if (pts_name == nullptr) {
        // Return empty values if ptsname failed
        close(pt);
        return_value.file_path = std::nullopt;
        return_value.fd = std::nullopt;
        return return_value;
    }

    // Convert the C-style string to a C++ std::string
    return_value.file_path = std::string(pts_name);
    return_value.fd = pt;
    return return_value;
}
