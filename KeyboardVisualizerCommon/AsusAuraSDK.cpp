#include "AsusAuraSDK.h"

#include "VisualizerDefines.h"



AsusAuraSDK::AsusAuraSDK()
{
}


AsusAuraSDK::~AsusAuraSDK()
{
}

void AsusAuraSDK::Initialize()
{
	hModule = LoadLibraryA(ASUSAURASDKDLL);
	if (hModule)
	{
		dllLoaded = TRUE;
	}
	else
	{
		dllLoaded = FALSE;
	}
}

bool AsusAuraSDK::SetLEDs(COLORREF pixels[64][256])
{
	return false;
}
