#pragma once
#include "SDKDLL.h"

//==================================================================================================
// Note:these files belong to Cooler Master RGB SDK, do not remove or edit ~@_@~
//	SDKDLL.h
//	SDKDLL.dll
//	SDKDLL.lib
//==================================================================================================

class CmKeyboard
{
public:
	CmKeyboard();
	~CmKeyboard();

	int Initialize();
	bool SetLEDs(COLORREF pixels[64][256]);

};