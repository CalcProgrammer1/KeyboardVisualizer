/*---------------------------------------------------------*\
|  Common Definitions for Keyboard Visualizer               |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef VISUALIZER_DEFINES_H
#define VISUALIZER_DEFINES_H

#ifdef WIN32
#ifndef _WINDOWS_
#include "..\KeyboardVisualizerVC\stdafx.h"
#endif
#else
//Includes for non-Windows builds
#include <unistd.h>

//Define Windows types for non-Windows Builds
typedef unsigned int    COLORREF;
typedef unsigned char   BYTE;
typedef bool            boolean;
#define RGB(r, g, b)    (((unsigned char)r) | ((unsigned char)g << 8) | ((unsigned char)b << 16))
#define GetRValue(rgb)  ((unsigned char) (rgb))
#define GetGValue(rgb)  ((unsigned char) ((rgb) >> 8))
#define GetBValue(rgb)  ((unsigned char) ((rgb) >> 16))
#define Sleep(ms)       (usleep(ms * 1000))
#define LPSTR           char *
#define strtok_s        strtok_r
#endif

#define RGB2BGR(a_ulColor) (a_ulColor & 0xFF000000) | ((a_ulColor & 0xFF0000) >> 16) | (a_ulColor & 0x00FF00) | ((a_ulColor & 0x0000FF) << 16)

//Special purpose row indices
#define ROW_IDX_BAR_GRAPH           0
#define ROW_IDX_SINGLE_COLOR        1
#define ROW_IDX_SPECTROGRAPH_TOP    2

#define SPECTROGRAPH_COLS           (256)
#define SPECTROGRAPH_END            (SPECTROGRAPH_COLS - 1)
#define SPECTROGRAPH_ROWS           (64 - ROW_IDX_SPECTROGRAPH_TOP)

typedef unsigned int VISUALIZER_PATTERN;
enum
{
    //Static Solid Colors
    VISUALIZER_PATTERN_SOLID_BLACK,
    VISUALIZER_PATTERN_SOLID_WHITE,
    VISUALIZER_PATTERN_SOLID_RED,
    VISUALIZER_PATTERN_SOLID_ORANGE,
    VISUALIZER_PATTERN_SOLID_YELLOW,
    VISUALIZER_PATTERN_SOLID_GREEN,
    VISUALIZER_PATTERN_SOLID_CYAN,
    VISUALIZER_PATTERN_SOLID_BLUE,
    VISUALIZER_PATTERN_SOLID_PURPLE,
    //Static Color Patterns
    VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED,
    VISUALIZER_PATTERN_STATIC_GREEN_WHITE_RED,
    VISUALIZER_PATTERN_STATIC_BLUE_CYAN_WHITE,
    VISUALIZER_PATTERN_STATIC_RED_WHITE_BLUE,
    VISUALIZER_PATTERN_STATIC_RAINBOW,
    VISUALIZER_PATTERN_STATIC_RAINBOW_INVERSE,
    //Animated Patterns
    VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL,
    VISUALIZER_PATTERN_ANIM_RAINBOW_HSV,
    VISUALIZER_PATTERN_ANIM_COLOR_WHEEL,
    VISUALIZER_PATTERN_ANIM_COLOR_WHEEL_2,
    VISUALIZER_PATTERN_ANIM_SPECTRUM_CYCLE,
    VISUALIZER_PATTERN_ANIM_SINUSOIDAL_CYCLE,

    //Number of Patterns
    VISUALIZER_NUM_PATTERNS
};

const char * const visualizer_pattern_labels[VISUALIZER_NUM_PATTERNS] =
{
    "Black",
    "White",
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Purple",
    "Green/Yellow/Red",
    "Green/White/Red",
    "Blue/Cyan/White",
    "Red/White/Blue",
    "Rainbow Bars",
    "Rainbow Bars Inverse",
    "Original",
    "Rainbow",
    "Color Wheel",
    "Color Wheel 2",
    "Spectrum Cycle",
    "Sinusoidal Cycle"
};

typedef unsigned int VISUALIZER_SINGLE_COLOR;
enum
{
    //Basic Colors
    VISUALIZER_SINGLE_COLOR_BLACK,
    VISUALIZER_SINGLE_COLOR_WHITE,
    VISUALIZER_SINGLE_COLOR_RED,
    VISUALIZER_SINGLE_COLOR_ORANGE,
    VISUALIZER_SINGLE_COLOR_YELLOW,
    VISUALIZER_SINGLE_COLOR_GREEN,
    VISUALIZER_SINGLE_COLOR_CYAN,
    VISUALIZER_SINGLE_COLOR_BLUE,
    VISUALIZER_SINGLE_COLOR_PURPLE,

    VISUALIZER_SINGLE_COLOR_BACKGROUND,
    VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND,
    VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND,

    //Number of Single Color Modes
    VISUALIZER_NUM_SINGLE_COLOR
};

const char * const visualizer_single_color_labels[VISUALIZER_NUM_SINGLE_COLOR] =
{
    "Black",
    "White",
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Purple",
    "Background",
    "Follow Background",
    "Follow Foreground"
};

typedef struct
{
    COLORREF pixels[64][256];
} vis_pixels;

#endif
