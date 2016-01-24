#include "LEDStrip.h"

LEDStrip::LEDStrip()
{
}


LEDStrip::~LEDStrip()
{
}

void LEDStrip::Initialize(char* portname)
{
    port = new serial_port(portname, 115200);
}

void LEDStrip::SetLEDs(COLORREF pixels[64][256])
{
    if (port != NULL)
    {
        unsigned char serial_buf[92] = { 0 };

        serial_buf[0] = 0xAA;

        for (int idx = 0; idx < 90; idx += 3)
        {
            serial_buf[idx + 1] = GetRValue(pixels[1][(int)(idx * 2.84f)]);
            serial_buf[idx + 2] = GetGValue(pixels[1][(int)(idx * 2.84f)]);
            serial_buf[idx + 3] = GetBValue(pixels[1][(int)(idx * 2.84f)]);
        }

        unsigned char sum = 0;

        for (int i = 0; i < 91; i++)
        {
            sum += serial_buf[i];
        }

        serial_buf[91] = sum;

        port->serial_write((char *)serial_buf, 92);
        port->serial_flush_tx();

        //unsigned char xmas_buf[5*25];

        //for (int idx = 0; idx < 25; idx++)
        //{
        //    unsigned int xmas_color = ((GetBValue(pixels[0][(int)(idx * 5.12f)])/16)<<8)
        //                            | ((GetGValue(pixels[0][(int)(idx * 5.12f)])/16)<<4)
        //                            | ((GetRValue(pixels[0][(int)(idx * 5.12f)])/16));

        //    xmas_buf[(idx * 5)] = 0x00;
        //    xmas_buf[(idx * 5) + 1] = idx + 1;
        //    xmas_buf[(idx * 5) + 2] = xmas_color >> 8;
        //    xmas_buf[(idx * 5) + 3] = xmas_color & 0xFF;
        //    xmas_buf[(idx * 5) + 4] = 0xFE;
        //    
        //    if (idx == 24)
        //    {
        //        xmas_buf[(idx * 5) + 4] = 0xFF;
        //    }
        //}

        //port->serial_write((char *)xmas_buf, 5*25);
        //port->serial_flush_tx();
    }
};