#pragma once

#include "stdafx.h"

#include "RzChromaSDKDefines.h"
#include "RzChromaSDKTypes.h"
#include "RzErrors.h"
#include "AppSettings.h"

using namespace ChromaSDK::Keyboard;

typedef RZRESULT(*INIT)(void);
typedef RZRESULT(*UNINIT)(void);
typedef RZRESULT(*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEHEADSETEFFECT)(ChromaSDK::Headset::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);

#ifdef _WIN64
#define CHROMASDKDLL        _T("RzChromaSDK64.dll")
#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
#endif

class RazerChroma
{
public:
	RazerChroma();
	RazerChroma(AppSettings*);
	~RazerChroma();

	void Initialize();
	bool SetLEDs(COLORREF pixels[64][256]);
	void updatedSettings(AppSettings*);

private:
	HMODULE hModule = NULL;
    CREATEEFFECT CreateEffect;
    CREATEKEYBOARDEFFECT CreateKeyboardEffect;
    CREATEMOUSEEFFECT CreateMouseEffect;
    CREATEHEADSETEFFECT CreateHeadsetEffect;

    RZEFFECTID effect;
};

