#ifndef CORSAIRCKBLINUX_H
#define CORSAIRCKBLINUX_H

#include "VisualizerDefines.h"

class CorsairCKB
{
public:
    CorsairCKB();

    void Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);

private:
    int corsair_fd;
};

#endif // CORSAIRCKBLINUX_H
