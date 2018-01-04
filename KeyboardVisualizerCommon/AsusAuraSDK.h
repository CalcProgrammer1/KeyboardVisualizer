
#include "..\dependencies\AURA SDK_02_04\sdk\AURALightingSDK.h"
#include "VisualizerDefines.h"

#define ASUSAURASDKDLL "AURA_SDK.dll"

class AsusAuraSDK
{
public:
	AsusAuraSDK();
	~AsusAuraSDK();

	void Initialize();
	bool SetLEDs(COLORREF pixels[64][256]);

private:
	
	HMODULE hModule = NULL;

	bool dllLoaded = FALSE;

	EnumerateMbControllerFunc EnumerateMbController;
	SetMbModeFunc SetMbMode;
	SetMbColorFunc SetMbColor;
	GetMbColorFunc GetMbColor;
	GetMbLedCountFunc GetMbLedCount;

};

