/*---------------------------------------------------------*\
|  Definitions for Logitech Gaming LED SDK Interface        |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 2/5/2017         |
\*---------------------------------------------------------*/

#ifndef LOGITECH_SDK_H
#define LOGITECH_SDK_H

#include "..\KeyboardVisualizerVC\stdafx.h"

#include "LogitechLEDLib.h"
#pragma comment(lib, "LogitechLEDLib.lib")

#include "VisualizerDefines.h"

class LogitechSDK
{
public:
    LogitechSDK();
    ~LogitechSDK();

    void Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);
};

#endif
