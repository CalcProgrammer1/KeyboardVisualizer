#pragma once

#include "stdafx.h"
#include "serial_port.h"

class LEDStrip
{
public:
    LEDStrip();
    ~LEDStrip();

    void Initialize(char* portname);
    void SetLEDs(COLORREF pixels[64][256]);

private:
    serial_port *port;
};

