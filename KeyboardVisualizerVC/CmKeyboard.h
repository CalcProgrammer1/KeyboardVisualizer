//==================================================================================================
// 2016/7/23 Sen Zhao pz140202@gmail.com 
// Powered by Cooler Master SDK v160630
//==================================================================================================

#ifndef CM_KEYBOARD_H
#define CM_KEYBOARD_H

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

	void Initialize();
	bool SetLEDs(COLORREF pixels[64][256]);

};

#endif