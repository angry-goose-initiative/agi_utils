#include "vserial.h"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <optional>

using namespace agi::vserial;

std::optional<std::string> getFileName(){
        int pt;

    pt = open("/dev/ptmx", O_RDWR | O_NOCTTY);
    if (pt < 0)
    {
        perror("open /dev/ptmx");
        return std::nullopt;
    }

    grantpt(pt);
    unlockpt(pt);

    const char* pts_name = ptsname(pt);
    
    // Check if ptsname returned a valid string
    if (pts_name == nullptr) {
        // Return an empty string if ptsname failed
        return std::nullopt;
    }

    // Convert the C-style string to a C++ std::string
    return std::string(pts_name);
}
