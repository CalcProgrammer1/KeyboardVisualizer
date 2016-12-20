#ifndef RAZERCHROMALINUX_H
#define RAZERCHROMALINUX_H

#include "VisualizerDefines.h"

class RazerChroma
{
public:
    RazerChroma();
    ~RazerChroma();

    void Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);

private:
    int razer_custom_fd[2];
    int razer_update_fd[2];
};

#endif // RAZERCHROMALINUX_H
