#pragma once

#include "stdafx.h"

#include "RzChromaSDKDefines.h"
#include "RzChromaSDKTypes.h"
#include "RzErrors.h"

using namespace ChromaSDK::Keyboard;

typedef RZRESULT(*INIT)(void);
typedef RZRESULT(*UNINIT)(void);
typedef RZRESULT(*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);

#ifdef _WIN64
#define CHROMASDKDLL        _T("RzChromaSDK64.dll")
#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
#endif

class RazerKeyboard
{
public:
	RazerKeyboard();
	~RazerKeyboard();

	void Initialize();
	void SetLEDs(COLORREF pixels[64][256]);

private:
	HMODULE hModule = NULL;
    CREATEEFFECT CreateEffect;

    RZEFFECTID effect;

    CUSTOM_EFFECT_TYPE Grid;
};

