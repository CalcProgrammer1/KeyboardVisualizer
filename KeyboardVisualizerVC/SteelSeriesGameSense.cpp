/*---------------------------------------------------------*\
|  Processing Code for SteelSeries Apex M800                |
|                                                           |
|  saltybot@gmail.com, 12/13/2016                           |
\*---------------------------------------------------------*/

#include "SteelSeriesGameSense.h"

#pragma comment(lib, "hid.lib")

extern "C" {
#include <hidsdi.h>
}

#include "HidUtil.h"

namespace
{

const int ROWS = 6;
const int COLS = 23;

const unsigned char KeyMap[ROWS][COLS] = {
    0xe8, 0x29, 0xff, 0x3a, 0x3b, 0x3c, 0x3d, 0xff, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0xff, 0x00, 0xff, 0xff,
    0xe9, 0x35, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2d, 0x2e, 0xff, 0x2a, 0x49, 0x4a, 0x4b, 0x53, 0x54, 0x55, 0x56,
    0xea, 0x2b, 0x14, 0x1a, 0x08, 0x15, 0x17, 0x1c, 0x18, 0x0c, 0x12, 0x13, 0x2f, 0x30, 0xff, 0x31, 0x4c, 0x4d, 0x4e, 0x5f, 0x60, 0x61, 0x57,
    0xeb, 0x39, 0x04, 0x16, 0x07, 0x09, 0x0a, 0x0b, 0x0d, 0x0e, 0x0f, 0x33, 0x34, 0xff, 0xff, 0x28, 0xff, 0xff, 0xff, 0x5c, 0x5d, 0x5e, 0xff,
    0xec, 0xe1, 0xff, 0x1d, 0x1b, 0x06, 0x19, 0x05, 0x11, 0x10, 0x36, 0x37, 0x38, 0xe5, 0xff, 0xff, 0xff, 0x52, 0xff, 0x59, 0x5a, 0x5b, 0x58,
    0xed, 0xe0, 0xe3, 0xff, 0xe2, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe6, 0xe7, 0x65, 0xef, 0xe4, 0x50, 0x51, 0x4f, 0x62, 0xff, 0x63, 0xff
};

}

SteelSeriesGameSense::SteelSeriesGameSense()
:Dev(INVALID_HANDLE_VALUE)
,FeatureReportBuf()
{
}

SteelSeriesGameSense::~SteelSeriesGameSense()
{
    unsigned char buf[33];
    memset(buf, 0, sizeof(buf));
    buf[0] = 0x00;
    buf[1] = 0x0d;
    buf[2] = 0x00;
    buf[3] = 0x00;

    HidD_SetOutputReport(Dev, buf, sizeof(buf));
}

void SteelSeriesGameSense::Initialize()
{
    Dev = GetDeviceHandle(0x1038, 0x1600, 0x0000);
    
    SetMode(0x02);
}

bool SteelSeriesGameSense::SetLEDs(COLORREF pixels[64][256])
{
    memset(FeatureReportBuf, 0, sizeof(FeatureReportBuf));
    FeatureReportBuf[0] = 0x00;
    FeatureReportBuf[1] = 0x0c;
    FeatureReportBuf[2] = 0x00;
    FeatureReportBuf[3] = 0x6e;
    FeatureReportBuf[4] = 0x00;

    int i = 5;
    int yStep = 64 / ROWS;
    int xStep = 256 / COLS;

    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < COLS; ++x)
        {
            unsigned char key = KeyMap[y][x];
            if (key != 0xff)
            {
                COLORREF color = pixels[y * yStep][x * xStep];
                FeatureReportBuf[i++] = key;
                FeatureReportBuf[i++] = GetRValue(color);
                FeatureReportBuf[i++] = GetGValue(color);
                FeatureReportBuf[i++] = GetBValue(color);
            }
        }
    }

    return (HidD_SetFeature(Dev, FeatureReportBuf, sizeof(FeatureReportBuf))) ? true : false;
}

void SteelSeriesGameSense::SetMode(unsigned char mode)
{
    if (Dev != INVALID_HANDLE_VALUE)
    {
        memset(FeatureReportBuf, 0, sizeof(FeatureReportBuf));
        FeatureReportBuf[0] = 0x00;
        FeatureReportBuf[1] = 0x0e;
        FeatureReportBuf[2] = 0x00;
        FeatureReportBuf[3] = 0x2a;
        FeatureReportBuf[4] = 0x00;

        int stride = 12;
        for (int y = 0; y < ROWS; ++y)
        {
            int i = 5;
            memset(&FeatureReportBuf[i], 0, COLS * stride);

            for (int x = 0; x < COLS; ++x)
            {
                unsigned char key = KeyMap[y][x];
                if (key != 0xff)
                {
                    FeatureReportBuf[i + 9] = mode;
                    FeatureReportBuf[i + 11] = key;
                    i += stride;
                }
            }

            FeatureReportBuf[3] = i / stride;

            HidD_SetFeature(Dev, FeatureReportBuf, sizeof(FeatureReportBuf));
            Sleep(10);
        }
    }
}