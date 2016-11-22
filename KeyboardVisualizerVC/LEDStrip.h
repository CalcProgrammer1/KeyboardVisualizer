#pragma once

#include "stdafx.h"
#include "serial_port.h"

class LEDStrip
{
public:
    LEDStrip();
    ~LEDStrip();

    void Initialize(char* portname);
    char* GetPortName();
    void SetLEDs(COLORREF pixels[64][256]);
    void SetLEDsXmas(COLORREF pixels[64][256]);

private:
    char port_name[128];
    serial_port *port;
};

