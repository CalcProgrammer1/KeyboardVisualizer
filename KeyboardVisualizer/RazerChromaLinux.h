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
    std::vector<int> razer_fd_1;
    std::vector<int> razer_fd_2;
    std::vector<int> razer_fd_3;
    std::vector<int> razer_fd_4;
    std::vector<int> razer_device_type;
    std::vector<char *>razer_device_serial;
};

#endif // RAZERCHROMALINUX_H
