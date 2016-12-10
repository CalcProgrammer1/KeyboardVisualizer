#pragma once

#include "stdafx.h"
#include "serial_port.h"
#include "udp_port.h"

class LEDStrip
{
public:
    LEDStrip();
    ~LEDStrip();

    void Initialize(char* led_string);
    void InitializeSerial(char* portname, int baud);
    void InitializeUDP(char* clientname, char* port);
    char* GetPortName();
    void SetNumLEDs(int numleds);
    void SetLEDs(COLORREF pixels[64][256]);
    void SetLEDsXmas(COLORREF pixels[64][256]);

private:
    int baud_rate;
    int num_leds;
    char port_name[128];
    char client_name[1024];
    serial_port *serialport;
    udp_port *udpport;
};

