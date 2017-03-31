/*---------------------------------------------------------*\
|  Definitions for Asus Aura Interface                      |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef ASUSAURALINUX_H
#define ASUSAURALINUX_H

#include "VisualizerDefines.h"

class AsusAura
{
public:
    AsusAura();

    void Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);

private:
    int smbus_fd;
    int smbus_fd2;
};

#endif // ASUSAURALINUX_H
