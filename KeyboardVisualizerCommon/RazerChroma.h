/*---------------------------------------------------------*\
|  Definitions for Razer Chroma SDK Interface               |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef RAZER_CHROMA_H
#define RAZER_CHROMA_H

#include "..\KeyboardVisualizerVC\stdafx.h"

#include "RzChromaSDKDefines.h"
#include "RzChromaSDKTypes.h"
#include "RzErrors.h"

#include "VisualizerDefines.h"

using namespace ChromaSDK::Keyboard;

typedef RZRESULT(*INIT)(void);
typedef RZRESULT(*UNINIT)(void);
typedef RZRESULT(*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEMOUSEPADEFFECT)(ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
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
	~RazerChroma();

	void Initialize();
	bool SetLEDs(COLORREF pixels[64][256]);

private:
	HMODULE hModule = NULL;
    CREATEEFFECT CreateEffect;
    CREATEKEYBOARDEFFECT CreateKeyboardEffect;
    CREATEMOUSEEFFECT CreateMouseEffect;
    CREATEMOUSEPADEFFECT CreateMousepadEffect;
    CREATEHEADSETEFFECT CreateHeadsetEffect;

    RZEFFECTID effect;
};

#endif