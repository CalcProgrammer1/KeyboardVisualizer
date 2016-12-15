/*---------------------------------------------------------*\
|  Definitions for SteelSeries Apex M800                    |
|                                                           |
|  saltybot@gmail.com, 12/13/2016                           |
\*---------------------------------------------------------*/

#ifndef STEELSERIESGAMESENSE_H
#define STEELSERIESGAMESENSE_H

#include <Windows.h>

#include <vector>

enum SS_KEYBOARD_LAYOUT
{
    SS_APEX_M800_LAYOUT_US,
    SS_APEX_M800_LAYOUT_UK
};

class SteelSeriesGameSense
{
public:
    SteelSeriesGameSense();
    ~SteelSeriesGameSense();

    void Initialize();
    void SetKeyboardLayout(SS_KEYBOARD_LAYOUT layout);
    bool SetLEDs(COLORREF pixels[64][256]);

private:
    void ResetLighting();
    void SetMode(unsigned char mode);

    HANDLE Dev;
    unsigned char FeatureReportBuf[515];
    const std::vector<std::vector<unsigned char>> *KeyMap;
};

#endif