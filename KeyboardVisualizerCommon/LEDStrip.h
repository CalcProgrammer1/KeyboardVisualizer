/*---------------------------------------------------------*\
|  Definitions for Generic LED Strip Interface              |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef LED_STRIP_H
#define LED_STRIP_H

#include "net_port.h"
#include "serial_port.h"

#include "VisualizerDefines.h"

#ifndef TRUE
#define TRUE true
#define FALSE false
#endif

class LEDStrip
{
public:
    LEDStrip();
    ~LEDStrip();

    void Initialize(char* ledstring, int matrix_size, int matrix_pos, int sections, bool mirror_x, bool mirror_y);
    void InitializeHuePlus(char * ledstring);
    void InitializeSerial(char* portname, int baud);
    void InitializeUDP(char* clientname, char* port);
    char* GetLEDString();
    void SetNumLEDs(int numleds, int matrix_size, int matrix_pos, int sections, bool mirror_x, bool mirror_y);
    void SetLEDs(COLORREF pixels[64][256]);
    void SetLEDsXmas(COLORREF pixels[64][256]);
    void SetLEDsHuePlus(COLORREF pixels[64][256]);

private:
    int baud_rate;
    int num_leds;
    int fans;
    int channel;
    const int hueSize = 125;

    int * LEDStripXIndex;
    int * LEDStripYIndex;

    char led_string[1024];
    char port_name[128];
    char client_name[1024];
    serial_port *serialport;
    net_port *udpport;
};

#endif
