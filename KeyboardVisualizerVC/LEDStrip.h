#pragma once

#include "stdafx.h"
#include "serial_port.h"

class LEDStrip
{
public:
    LEDStrip();
    ~LEDStrip();

    void Initialize();
    void SetLEDs(COLORREF pixels[64][256]);

private:
    serial_port *port;
};

