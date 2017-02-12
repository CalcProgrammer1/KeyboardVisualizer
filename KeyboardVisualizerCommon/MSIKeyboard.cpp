/*---------------------------------------------------------*\
|  Processing Code for MSI SteelSeries Keyboard Interface   |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#include "MSIKeyboard.h"

#include "UsbDevice.h"

static UsbDevice usb;
static bool init_ok = true;

MSIKeyboard::MSIKeyboard()
{
}


MSIKeyboard::~MSIKeyboard()
{
}


void MSIKeyboard::Initialize()
{
    //Look for MSI SteelSeries Keyboard
    if (usb.OpenDevice(0x1770, 0xFF00, 0))
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

    return init_ok;
}
