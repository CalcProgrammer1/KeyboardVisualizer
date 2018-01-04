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
	if (dllLoaded) {

		(FARPROC&)EnumerateMbController = GetProcAddress(hModule, "EnumerateMbController");
		(FARPROC&)SetMbMode = GetProcAddress(hModule, "SetMbMode");
		(FARPROC&)SetMbColor = GetProcAddress(hModule, "SetMbColor");
		(FARPROC&)GetMbColor = GetProcAddress(hModule, "GetMbColor");
		(FARPROC&)GetMbLedCount = GetProcAddress(hModule, "GetMbLedCount");

		DWORD _count = EnumerateMbController(NULL, 0);

		MbLightControl* _mbLightCtrl = new MbLightControl[_count];
		EnumerateMbController(_mbLightCtrl, _count);

		SetMbMode(_mbLightCtrl[0], 1);

		DWORD _ledCount = GetMbLedCount(_mbLightCtrl[0]);

		BYTE* _color = new BYTE[_ledCount * 3];
		ZeroMemory(_color, _ledCount * 3);
		
		// Following logic is based on results from the Asus ROG Strix Z270I motherboard.

		for (size_t i = 0; i < _ledCount * 3; ++i)
		{
			if (i % 3 == 0) {
				_color[i] = GetRValue(pixels[0][(128 / _ledCount) *(_ledCount - (int)((i) / 3))]);
			}
			if (i % 3 == 1) {
				_color[i] = GetBValue(pixels[0][(128 / _ledCount)*(_ledCount - (int)((i-1) / 3))]);
			}
			if (i % 3 == 2) {
				_color[i] = GetGValue(pixels[0][(128 / _ledCount)*(_ledCount - (int)((i-2) / 3))]);
			}
		}

		SetMbColor(_mbLightCtrl[0], _color, _ledCount * 3);

		delete[] _color;
		delete[] _mbLightCtrl;

		return TRUE;
	}
	else
		return FALSE;
}
