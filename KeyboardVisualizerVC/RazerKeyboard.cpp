#include "RazerKeyboard.h"

#include <iostream>

RazerKeyboard::RazerKeyboard()
{
}


RazerKeyboard::~RazerKeyboard()
{
    if (hModule)
    {
        UNINIT UnInit = (UNINIT)GetProcAddress(hModule, "UnInit");
        if (UnInit)
        {
            UnInit();
        }
    }
}

void RazerKeyboard::Initialize()
{
	// Dynamically loads the Chroma SDK library.
	hModule = LoadLibrary(CHROMASDKDLL);
	if (hModule)
	{
		INIT Init = (INIT)GetProcAddress(hModule, "Init");
		if (Init)
		{
			Init();
		}
	}
    
}

void RazerKeyboard::SetLEDs(COLORREF pixels[64][256])
{
	for (int x = 0; x < 22; x++)
	{
		for (int y = 0; y < 6; y++)
		{
			int x_idx = x * (256 / 22);
			int y_idx = y * (64 / 6) + (0.5f * (64 / 6));
            Grid.Color[y][x] = (pixels[y_idx][x_idx] & 0x00FFFFFF);
		}
	}

    CreateEffect = (CREATEEFFECT)GetProcAddress(hModule, "CreateEffect");

    if(CreateEffect != NULL)
    CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA, ChromaSDK::CHROMA_CUSTOM, &Grid, NULL);
};