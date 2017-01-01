/*---------------------------------------------------------*\
|  Processing Code for Generic LED Strip Interface          |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#include "LEDStrip.h"

#include <fstream>
#include <iostream>
#include <string>

LEDStrip::LEDStrip()
{
    num_leds = 30;
}


LEDStrip::~LEDStrip()
{
}

void LEDStrip::Initialize(char* ledstring)
{
    strcpy(led_string, ledstring);

    bool    serial      = TRUE;
    LPSTR   numleds     = NULL;
    LPSTR   source      = NULL;
    LPSTR   udpport_baud= NULL;
    LPSTR   next        = NULL;

    source = strtok_s(ledstring, ",", &next);

    //Check if we are setting up a UDP or Serial interface
    //UDP is denoted by udp:
    if (strncmp(source, "udp:", 4) == 0)
    {
        source = source + 4;
        serial = FALSE;
    }

    //Check for either the UDP port or the serial baud rate
    if (strlen(next))
    {
        udpport_baud = strtok_s(next, ",", &next);
    }

    //Check for the number of LEDs
    if (strlen(next))
    {
        numleds = strtok_s(next, ",", &next);
    }

    if (serial)
    {
        if (udpport_baud == NULL)
        {
            //Initialize with default baud rate
            InitializeSerial(source, 115200);
        }
        else
        {
            //Initialize with custom baud rate
            InitializeSerial(source, atoi(udpport_baud));
        }
    }
    else
    {
        if (udpport_baud == NULL)
        {
            //Do something
        }
        else
        {
            //Initialize UDP port
            InitializeUDP(source, udpport_baud);
        }
    }

    if (strlen(numleds))
    {
        SetNumLEDs(atoi(numleds));
    }

}

void LEDStrip::InitializeSerial(char* portname, int baud)
{
    strcpy(port_name, portname);
    baud_rate = baud;
    serialport = new serial_port(port_name, baud_rate);
    //udpport = NULL;
}

void LEDStrip::InitializeUDP(char * clientname, char * port)
{
    strcpy(client_name, clientname);
    strcpy(port_name, port);

    //udpport = new net_port(client_name, port_name);
    serialport = NULL;
}

char* LEDStrip::GetLEDString()
{
    return(led_string);
}

void LEDStrip::SetNumLEDs(int numleds)
{
    num_leds = numleds;
}

void LEDStrip::SetLEDs(COLORREF pixels[64][256])
{
    //if (serialport != NULL || udpport != NULL)
    if(serialport != NULL)
    {
        unsigned char *serial_buf;

        serial_buf = new unsigned char[(num_leds * 3) + 3];

        serial_buf[0] = 0xAA;

        for (int idx = 0; idx < (num_leds * 3); idx += 3)
        {
            int col_idx = (int)((idx * (255.0f / (3.0f * num_leds))) + (128.0f / (3.0f * num_leds)));
            serial_buf[idx + 1] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][col_idx]);
            serial_buf[idx + 2] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][col_idx]);
            serial_buf[idx + 3] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][col_idx]);
        }

        unsigned short sum = 0;

        for (int i = 0; i < (num_leds * 3) + 1; i++)
        {
            sum += serial_buf[i];
        }

        serial_buf[(num_leds * 3) + 1] = sum >> 8;
        serial_buf[(num_leds * 3) + 2] = sum & 0x00FF;

        if (serialport != NULL)
        {
            serialport->serial_write((char *)serial_buf, (num_leds * 3) + 3);
            serialport->serial_flush_tx();
        }
        //else if (udpport != NULL)
        //{
            //udpport->udp_write((char *)serial_buf, (num_leds * 3) + 3);
        //}

        delete[] serial_buf;
    }
}

void LEDStrip::SetLEDsXmas(COLORREF pixels[64][256])
{
    unsigned char xmas_buf[5*25];

    for (int idx = 0; idx < 25; idx++)
    {
        unsigned int xmas_color = ((GetBValue(pixels[ROW_IDX_BAR_GRAPH][(int)(idx * 5.12f)])/16)<<8)
                                | ((GetGValue(pixels[ROW_IDX_BAR_GRAPH][(int)(idx * 5.12f)])/16)<<4)
                                | ((GetRValue(pixels[ROW_IDX_BAR_GRAPH][(int)(idx * 5.12f)])/16));

        xmas_buf[(idx * 5)] = 0x00;
        xmas_buf[(idx * 5) + 1] = idx + 1;
        xmas_buf[(idx * 5) + 2] = xmas_color >> 8;
        xmas_buf[(idx * 5) + 3] = xmas_color & 0xFF;
        xmas_buf[(idx * 5) + 4] = 0xFE;

        if (idx == 24)
        {
            xmas_buf[(idx * 5) + 4] = 0xFF;
        }
    }

    serialport->serial_write((char *)xmas_buf, 5*25);
    serialport->serial_flush_tx();
};
