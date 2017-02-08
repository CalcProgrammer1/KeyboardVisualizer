/*---------------------------------------------------------*\
|  Processing Code for Logitech Gaming LED SDK Interface    |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 2/5/2017         |
\*---------------------------------------------------------*/

#include "LogitechSDK.h"
#include "VisualizerDefines.h"

static boolean failed;

//Index lists for BlackWidow
int LogitechXIndex[21];
int LogitechYIndex[6];

static void SetupKeyboardGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for (int x = 0; x < x_count; x++)
    {
        if (x_count < 10)
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count))) + (0.5f * (SPECTROGRAPH_COLS / (x_count))));
        }
        else if (x < ((x_count) / 2))
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) + (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }
        else
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) - (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }

    }
    for (int y = 0; y < y_count; y++)
    {
        y_idx_list[y] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / y_count)) + (0.5f * (SPECTROGRAPH_ROWS / y_count)));
    }
}

LogitechSDK::LogitechSDK()
{
}


LogitechSDK::~LogitechSDK()
{
}

void LogitechSDK::Initialize()
{
    failed = !LogiLedInit();

    if (!failed)
    {
        SetupKeyboardGrid(21, 6, LogitechXIndex, LogitechYIndex);
    }
}

bool LogitechSDK::SetLEDs(COLORREF pixels[64][256])
{
    if (failed)
    {
        return FALSE;
    }
    else
    {
        unsigned char LogitechBitmap[LOGI_LED_BITMAP_SIZE];

        for (int y = 0; y < 6; y++)
        {
            for (int x = 0; x < 21; x++)
            {
                ((int *)LogitechBitmap)[(21 * y) + x] = RGB2BGR(pixels[LogitechYIndex[y]][LogitechXIndex[x]]) | 0xFF000000;
            }
        }

        LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB);
        if (!LogiLedSetLightingFromBitmap(LogitechBitmap))
        {
            failed = TRUE;
            return FALSE;
        }

        COLORREF single_color = pixels[ROW_IDX_SINGLE_COLOR][0];

        LogiLedSetTargetDevice(LOGI_DEVICETYPE_RGB | LOGI_DEVICETYPE_MONOCHROME);
        if (!LogiLedSetLighting(GetRValue(single_color) / 2.55f, GetGValue(single_color) / 2.55f, GetBValue(single_color) / 2.55f))
        {
            failed = TRUE;
            return FALSE;
        }

        return TRUE;
    }
}