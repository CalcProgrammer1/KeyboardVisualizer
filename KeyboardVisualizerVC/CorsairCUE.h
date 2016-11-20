#pragma once

#include "stdafx.h"

#include "CUESDK.h"

class CorsairCUE
{
public:
	CorsairCUE();
	~CorsairCUE();

	void Initialize();
	bool SetLEDs(COLORREF pixels[64][256]);
};

