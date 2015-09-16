#include "LEDStrip.h"

LEDStrip::LEDStrip()
{
}


LEDStrip::~LEDStrip()
{
}

void LEDStrip::Initialize()
{
    port = new serial_port("COM6", 115200);
}

void LEDStrip::SetLEDs(COLORREF pixels[64][256])
{
    unsigned char serial_buf[92] = { 0 };

    serial_buf[0] = 0xAA;

    for (int idx = 0; idx < 90; idx+=3)
    {
        serial_buf[idx + 1] = GetRValue(pixels[0][(int)(idx * 2.84f)]);
        serial_buf[idx + 2] = GetGValue(pixels[0][(int)(idx * 2.84f)]);
        serial_buf[idx + 3] = GetBValue(pixels[0][(int)(idx * 2.84f)]);
    }

    unsigned char sum = 0;

    for (int i = 0; i < 91; i++)
    {
        sum += serial_buf[i];
    }

    serial_buf[91] = sum;

    port->serial_write((char *)serial_buf, 92);
    port->serial_flush_rx();
};