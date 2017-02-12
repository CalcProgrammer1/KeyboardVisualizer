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

static unsigned int keymap[6][23] =
    { {   8,   0,  16,  24,  32,  40,   0,  48,  56,  64,  72,   0,  80,  88,  96, 104, 112, 120, 128,   0,   0,   0,   0 },
      {   9,  17,  25,  33,  41,  49,  57,  65,  73,  81,  89,   0,  97, 105, 129,   0,  15,  31,  47,  63,  79,  95, 111 },
      {  10,   0,  18,  26,  34,  42,   0,  50,  58,  66,  74,  82,  90,  98, 106, 114,  23,  39,  55,  71,  87, 103, 102 },
      {  11,   0,  19,  27,  35,  43,   0,  51,  59,  67,  75,  83,  91,  99, 115,   0,   0,   0,   0,  46, 119,  78,   0 },
      {  12,   0,  36,  44,  52,  60,   0,  68,   0,  76,  84,  92, 100, 108, 124,   0,   0,  38,   0,  54,  62,  86, 118 },
      {  13,  21,  29,   0,   0,   0,   0,  45,   0,   0,   0,   0,  85,  93, 109, 117,  14,  22,  30,  70,   0,  94,   0 } };

unsigned int keymap_idx[6][23];

int PoseidonZXIndex[23];
int PoseidonZYIndex[6];

static void SetupKeyboardGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for (int x = 0; x < x_count; x++)
    {
        if (x < ((x_count) / 2))
        {
            x_idx_list[x] = (x * (SPECTROGRAPH_COLS / (x_count - 1))) + (0.5f * (SPECTROGRAPH_COLS / (x_count - 1)));
        }
        else
        {
            x_idx_list[x] = (x * (SPECTROGRAPH_COLS / (x_count - 1))) - (0.5f * (SPECTROGRAPH_COLS / (x_count - 1)));
        }

    }
    for (int y = 0; y < y_count; y++)
    {
        y_idx_list[y] = ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / y_count)) + (0.5f * (SPECTROGRAPH_ROWS / y_count));
    }
}

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

    SetupKeyboardGrid(23, 6, PoseidonZXIndex, PoseidonZYIndex);

}

bool MSIKeyboard::SetLEDs(COLORREF pixels[64][256])
{
#define POSEIDONZ_START       0x07
#define POSEIDONZ_PROFILE     0x01
#define POSEIDONZ_LED_CMD     0x0E
#define POSEIDONZ_RED_GRN_CH  0x01
#define POSEIDONZ_BLU_CH      0x02

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

    unsigned char red_grn_buf[264];
    unsigned char blu_buf[264];

    for(int i = 0; i < 264; i++)
    {
        red_grn_buf[i] = 0x00;
        blu_buf[i]     = 0x00;
    }

    red_grn_buf[0] = POSEIDONZ_START;
    red_grn_buf[1] = POSEIDONZ_LED_CMD;
    red_grn_buf[2] = POSEIDONZ_PROFILE;
    red_grn_buf[3] = POSEIDONZ_RED_GRN_CH;
    red_grn_buf[4] = 0x00;
    red_grn_buf[5] = 0x00;
    red_grn_buf[6] = 0x00;
    red_grn_buf[7] = 0x00;

    blu_buf[0] = POSEIDONZ_START;
    blu_buf[1] = POSEIDONZ_LED_CMD;
    blu_buf[2] = POSEIDONZ_PROFILE;
    blu_buf[3] = POSEIDONZ_BLU_CH;
    blu_buf[4] = 0x00;
    blu_buf[5] = 0x00;
    blu_buf[6] = 0x00;
    blu_buf[7] = 0x00;

    for(int x = 0; x < 23; x++)
    {
        for(int y = 0; y < 6; y++)
        {
            COLORREF color = pixels[PoseidonZYIndex[y]][PoseidonZXIndex[x]];
            unsigned int idx = keymap[y][x];

            if(idx != 0)
            {
                red_grn_buf[idx] = GetRValue(color);
                red_grn_buf[idx + 128 ] = GetGValue(color);
                blu_buf[idx] = GetBValue(color);
            }
        }
    }

    usb.SendToDevice(red_grn_buf, 264);
    Sleep(5);
    usb.SendToDevice(blu_buf, 264);

    return init_ok;
}
