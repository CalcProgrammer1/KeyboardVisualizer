#ifndef RAZERCHROMALINUX_H
#define RAZERCHROMALINUX_H

#include "VisualizerDefines.h"
#include <vector>

class RazerChroma
{
public:
    RazerChroma();
    ~RazerChroma();

    void Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);

private:
    std::vector<int> razer_custom_fd;
    std::vector<int> razer_update_fd;
    std::vector<int> razer_device_type;
    std::vector<char *>razer_device_serial;
};

#endif // RAZERCHROMALINUX_H
