/*---------------------------------------------------------*\
|  Common Definitions for Keyboard Visualizer               |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef VISUALIZER_DEFINES_H
#define VISUALIZER_DEFINES_H

//Special purpose row indices
#define ROW_IDX_BAR_GRAPH       0
#define ROW_IDX_SINGLE_COLOR    1

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
    VISUALIZER_PATTERN_ANIM_SPECTRUM_CYCLE,

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
    "Spectrum Cycle"
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