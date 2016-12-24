/*---------------------------------------------------------*\
|  Definitions for SteelSeries Apex M800                    |
|                                                           |
|  saltybot@gmail.com, 12/13/2016                           |
\*---------------------------------------------------------*/

#ifndef STEELSERIESGAMESENSE_H
#define STEELSERIESGAMESENSE_H

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include "VisualizerDefines.h"

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

    unsigned char FeatureReportBuf[515];
};

#endif
