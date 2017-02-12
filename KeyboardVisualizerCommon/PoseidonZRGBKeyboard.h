/*---------------------------------------------------------*\
|  Definitions for Thermaltake Poseidon Z RGB Keyboard      |
|  Interface                                                |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 2/12/2017        |
\*---------------------------------------------------------*/

#ifndef POSEIDON_ZRGB_KEYBOARD_H
#define POSEIDON_ZRGB_KEYBOARD_H

#include <sstream>
#include <wchar.h>
#include <string.h>

#include "VisualizerDefines.h"

class PoseidonZRGBKeyboard
{
public:
    PoseidonZRGBKeyboard();
    ~PoseidonZRGBKeyboard();

    void Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);
};

#endif
