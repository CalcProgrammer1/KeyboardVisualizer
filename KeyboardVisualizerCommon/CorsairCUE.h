/*---------------------------------------------------------*\
|  Definitions for Corsair CUE SDK Interface                |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef CORSAIR_CUE_H
#define CORSAIR_CUE_H

#include "..\KeyboardVisualizerVC\stdafx.h"

#include "CUESDK.h"

#include "VisualizerDefines.h"

class CorsairCUE
{
public:
	CorsairCUE();
	~CorsairCUE();

	void Initialize();
	bool SetLEDs(COLORREF pixels[64][256]);
};

#endif