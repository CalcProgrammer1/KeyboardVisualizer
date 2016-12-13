/*---------------------------------------------------------*\
|  Definitions for SteelSeries Apex M800                    |
|                                                           |
|  saltybot@gmail.com, 12/13/2016                           |
\*---------------------------------------------------------*/

#ifndef STEELSERIESGAMESENSE_H
#define STEELSERIESGAMESENSE_H

#include <Windows.h>

class SteelSeriesGameSense
{
public:
    SteelSeriesGameSense();
    ~SteelSeriesGameSense();

    void Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);

private:
    void SetMode(unsigned char mode);

    HANDLE Dev;
    unsigned char FeatureReportBuf[515];
};

#endif