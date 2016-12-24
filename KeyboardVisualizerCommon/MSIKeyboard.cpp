/*---------------------------------------------------------*\
|  Processing Code for MSI SteelSeries Keyboard Interface   |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#include "MSIKeyboard.h"

#include "UsbDevice.h"

static UsbDevice usb;
static bool init_ok = TRUE;

MSIKeyboard::MSIKeyboard()
{
}


MSIKeyboard::~MSIKeyboard()
{
}


void MSIKeyboard::send_usb_msg(char * data_pkt)
{
    char usb_pkt[8];
    for (int i = 1; i < 9; i++)
    {
        usb_pkt[i] = data_pkt[i - 1];
    }
    HidD_SetFeature(dev, data_pkt, 8);
}

void MSIKeyboard::Initialize()
{
    //Look for MSI SteelSeries Keyboard
    if (usb.OpenDevice(0x1770, 0xFF00, 0))
    {
        init_ok == TRUE;
    }
    else
    {
        init_ok = FALSE;
    }
}

bool MSIKeyboard::SetLEDs(COLORREF pixels[64][256])
{
    //Shout out to bparker06 for reverse engineering the MSI keyboard USB protocol!
    // https://github.com/bparker06/msi-keyboard/blob/master/keyboard.cpp for original implementation

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

    return init_ok;
}