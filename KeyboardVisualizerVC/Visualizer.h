#pragma once

#include "stdafx.h"
#include <math.h>
#include "chuck_fft.h"
#include "hsv.h"

#include <string>
#include <vector>

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
    VISUALIZER_PATTERN_ANIM_COLOR_WHEEL
};

typedef struct
{
    COLORREF pixels[64][256];
} vis_pixels;

class Visualizer
{
public:
	Visualizer();
	
	//Initializes the visualizer
	void Initialize();

	//Function to start thread
	void StartThread();

	//Visualizer Thread
	void VisThread();

	//Keyboard Update Thread
	void RazerChromaUpdateThread();
    void CorsairKeyboardUpdateThread();
    void MSIKeyboardUpdateThread();
    void LEDStripUpdateThread();

	//Update function
	void Update();

    //Draw Pattern
    void Visualizer::DrawPattern(VISUALIZER_PATTERN pattern, int bright, vis_pixels *pixels);

    //Add LED strip
    void AddLEDStrip(char* ledstring);
    void AddLEDStripXmas(char * ledstring);

    //Save Settings File
    void SaveSettings();

    //Compute normalization line
    void SetNormalization(float offset, float scale);

	//Calculated FFT
	float fft[256];
	
	//Amplitude of input waveform
	int amplitude;
    int avg_mode;
	int avg_size;
	int window_mode;
	int decay;
	int delay;

    //Visualizer Background
    vis_pixels pixels_bg;

    //Visualizer Foreground
    vis_pixels pixels_fg;

	//Visualizer Image 1
    vis_pixels pixels_vs1;

    //Visualizer Image 2
    vis_pixels pixels_vs2;

    //Visualizer Output Image Pointer
    vis_pixels *pixels_out;

    //Visualizer Render Image Pointer
    vis_pixels *pixels_render;

	//Background Variables
	int bkgd_bright;
	int bkgd_mode;

    //Single Color Mode
    int single_color_mode;

    //Normalization Offset and Scale
    float nrml_ofst;
    float nrml_scl;

    //Foreground Variables
    int frgd_mode;

private:
	//Background Step
	int bkgd_step;

	//FFT Variables
	float win_hanning[256];
	float win_hamming[256];
	float win_blackman[256];
	float fft_tmp[512];

	//Audio Sample Buffer
	unsigned char buffer[256];
};