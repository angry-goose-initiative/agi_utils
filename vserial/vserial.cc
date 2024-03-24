#include "vserial.h"



agi::vserial::Res agi::vserial::create_pseudo_serial(){
    int pt;
    
    agi::vserial::Res return_value;

    pt = open("/dev/ptmx", O_RDWR | O_NOCTTY);
    if (pt < 0)
    {
        perror("open /dev/ptmx");
        return_value.file_path = std::nullopt;
        return_value.fd = std::nullopt;
        return return_value;
    }

    grantpt(pt);
    unlockpt(pt);

    const char* pts_name = ptsname(pt);
    
    // Check if ptsname returned a valid string
    if (pts_name == nullptr) {
        // Return an empty string if ptsname failed
        return_value.file_path = std::nullopt;
        return_value.fd = std::nullopt;
    }

    // Convert the C-style string to a C++ std::string
    return_value.file_path = std::string(pts_name);
    return_value.fd = pt;
    return return_value;
}
