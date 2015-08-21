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

    ChromaSDK::Mousepad::CUSTOM_EFFECT_TYPE Effect = {};

    for (int x = 0; x < 8; x++)
    {
        Effect.Color[x] = pixels[x * (64 / 8)][1];
        Effect.Color[x + 7] = pixels[(7-x)*(64 / 8)][1];
    }

    CreateEffect(ChromaSDK::FIREFLY_CHROMA, ChromaSDK::CHROMA_CUSTOM, &Effect, NULL);

    ChromaSDK::Mouse::CUSTOM_EFFECT_TYPE Effect2 = {};

    for (int x = 0; x < 7; x++)
    {
        Effect2.Color[x+4] = pixels[x*(64/7)][1];
        Effect2.Color[x + 11] = pixels[x*(64 / 7)][1];
    }
    
    CreateEffect(ChromaSDK::MAMBA_CHROMA_TE, ChromaSDK::CHROMA_CUSTOM, &Effect2, NULL);
};