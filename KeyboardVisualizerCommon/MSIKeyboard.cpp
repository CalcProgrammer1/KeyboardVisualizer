/*---------------------------------------------------------*\
|  Processing Code for MSI SteelSeries Keyboard Interface   |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#include "MSIKeyboard.h"

#include "UsbDevice.h"

static UsbDevice usb;
static bool init_ok = true;

static unsigned int keys[] = {8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21,
                              22, 23, 24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35,
                              36, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                              50, 51, 52, 54, 55, 56, 57, 58, 59, 60, 62, 63, 64,
                              65, 66, 67, 68, 70, 71, 72, 73, 74, 75, 76, 78, 79,
                              80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92,
                              93, 94, 95, 96, 97, 98, 99, 100, 102, 103, 104, 105,
                              106, 108, 109, 111, 112, 114, 115, 117, 118, 119, 120,
                              124, 128, 129 };

MSIKeyboard::MSIKeyboard()
{
}


MSIKeyboard::~MSIKeyboard()
{
}


void MSIKeyboard::Initialize()
{
    //Look for MSI SteelSeries Keyboard
    //if (usb.OpenDevice(0x1770, 0xFF00, 0))
    if(usb.OpenDevice(0x264a, 0x3006, 0))
    {
        init_ok = true;
    }
    else
    {
        init_ok = false;
    }
}

bool MSIKeyboard::SetLEDs(COLORREF pixels[64][256])
{
    //Shout out to bparker06 for reverse engineering the MSI keyboard USB protocol!
    // https://github.com/bparker06/msi-keyboard/blob/master/keyboard.cpp for original implementation

    if(!init_ok)
    {
        return false;
    }
#if 0
    unsigned char buf[8] = { 0 };

    buf[0] = 1;
    buf[1] = 2;
    buf[2] = 64;
    buf[3] = 1;
    buf[4] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][160]);
    buf[5] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][160]);
    buf[6] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][160]);
    buf[7] = 236;

    usb.SendToDevice(buf, 8);

    buf[3] = 2;
    buf[4] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][192]);
    buf[5] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][192]);
    buf[6] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][192]);

    usb.SendToDevice(buf, 8);

    buf[3] = 3;
    buf[4] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][224]);
    buf[5] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][224]);
    buf[6] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][224]);

    usb.SendToDevice(buf, 8);

    buf[3] = 4;
    buf[4] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][192]);
    buf[5] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][192]);
    buf[6] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][192]);

    usb.SendToDevice(buf, 8);

    buf[3] = 5;

    usb.SendToDevice(buf, 8);

    buf[3] = 6;

    usb.SendToDevice(buf, 8);

    buf[3] = 7;

    usb.SendToDevice(buf, 8);
#endif

    unsigned char buf[264] = {0};

    //Send Red Packet
    for(int i = 0; i < 264; i++)
    {
        buf[i] = 0xFF;
    }

    buf[0] = 0x07;
    buf[1] = 0x09;
    buf[2] = 0x01;
    buf[3] = 0x01;

    for(int i = 0; i < 104; i++)
    {
        buf[keys[i]] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][0]);;
    }

    usb.SendToDevice(buf, 264);
    Sleep(10);
    //Send Green Packet
    for(int i = 0; i < 264; i++)
    {
        buf[i] = 0x00;
    }

    buf[0] = 0x07;
    buf[1] = 0x09;
    buf[2] = 0x01;
    buf[3] = 0x02;

    buf[8] = 0xFF;

    for(int i = 0; i < 104; i++)
    {
        buf[keys[i]] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][0]);;
    }

    usb.SendToDevice(buf, 264);
    Sleep(10);
    //Send Blue Packet
    for(int i = 0; i < 264; i++)
    {
        buf[i] = 0x11;
    }

    buf[0] = 0x07;
    buf[1] = 0x09;
    buf[2] = 0x01;
    buf[3] = 0x03;

    for(int i = 0; i < 104; i++)
    {
       buf[keys[i]] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
    }

    usb.SendToDevice(buf, 264);
    Sleep(10);
    //Send Update Packet
    for(int i = 0; i < 264; i++)
    {
        buf[i] = 0x00;
    }

    buf[0] = 0x07;
    buf[1] = 0x02;
    buf[2] = 0x01;
    buf[8] = 0x01;  //Preset to save
    buf[13] = 0x04; //Brightness?
    buf[16] = 0x08;
    buf[22] = 0x03;

    //usb.SendToDevice(buf, 264);
    return init_ok;
}
