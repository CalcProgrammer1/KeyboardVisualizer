#include "RazerChroma.h"

#include <iostream>

RazerChroma::RazerChroma()
{
}


RazerChroma::~RazerChroma()
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

void RazerChroma::Initialize()
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
static int red = 0;
bool RazerChroma::SetLEDs(COLORREF pixels[64][256])
{
    CreateEffect = (CREATEEFFECT)GetProcAddress(hModule, "CreateEffect");
    CreateMouseEffect = (CREATEMOUSEEFFECT)GetProcAddress(hModule, "CreateMouseEffect");

    if (CreateEffect == NULL)
    {
        return FALSE;
    }
    else
    {
        //Blackwidow Chroma
        CUSTOM_EFFECT_TYPE Grid;

        for (int x = 0; x < 22; x++)
        {
            for (int y = 0; y < 6; y++)
            {
                int x_idx = x * (256 / 22);
                int y_idx = y * (64 / 6) + (0.5f * (64 / 6));
                Grid.Color[y][x] = (pixels[y_idx][x_idx] & 0x00FFFFFF);
            }
        }

        //Set Razer "Three Headed Snake" logo to the background color of the 11th column
        Grid.Color[0][20] = pixels[3][11 * (256 / 22)];

        CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA, ChromaSDK::CHROMA_CUSTOM, &Grid, NULL);

        //Blackwidow Chroma Tournament Edition
        CUSTOM_EFFECT_TYPE Grid2;

        for (int x = 0; x < 18; x++)
        {
            for (int y = 0; y < 6; y++)
            {
                int x_idx = x * (256 / 18);
                int y_idx = y * (64 / 6) + (0.5f * (64 / 6));
                Grid2.Color[y][x] = (pixels[y_idx][x_idx] & 0x00FFFFFF);
            }
        }

        //Set Razer "Three Headed Snake" logo to the background color of the 11th column
        Grid2.Color[0][20] = pixels[3][11 * (256 / 22)];

        CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA_TE, ChromaSDK::CHROMA_CUSTOM, &Grid2, NULL);

        //Firefly Chroma
        ChromaSDK::Mousepad::CUSTOM_EFFECT_TYPE Effect = {};

        for (int x = 0; x < 15; x++)
        {
            if (x < 8)
            {
                Effect.Color[x - 1] = pixels[0][7 + (x * 16)];
            }
            else
            {
                Effect.Color[x] = pixels[0][8 + (x * 16)];
            }
        }
        Effect.Color[7] = pixels[0][128];

        CreateEffect(ChromaSDK::FIREFLY_CHROMA, ChromaSDK::CHROMA_CUSTOM, &Effect, NULL);

        //Mamba Chroma Tournament Edition
        ChromaSDK::Mouse::CUSTOM_EFFECT_TYPE Effect2 = {};

        for (int x = 0; x < 7; x++)
        {
            Effect2.Color[x + 4] = pixels[0][(16 * (x + 1))];
            Effect2.Color[x + 11] = pixels[0][(16 * (x + 1))];
        }

        //Set scroll wheel and logo LEDs to background color
        Effect2.Color[2] = pixels[3][16 * 5];
        Effect2.Color[1] = pixels[3][0];

        CreateEffect(ChromaSDK::MAMBA_CHROMA_TE, ChromaSDK::CHROMA_CUSTOM, &Effect2, NULL);

        //DeathAdder Chroma
        ChromaSDK::Mouse::CUSTOM_EFFECT_TYPE Effect3 = {};

        //Set scroll wheel and logo LEDs to background color
        Effect3.Color[2] = pixels[3][16 * 5];
        Effect3.Color[1] = pixels[3][0];

        CreateEffect(ChromaSDK::DEATHADDER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &Effect3, NULL);
        //CreateMouseEffect(ChromaSDK::Mouse::CHROMA_CUSTOM, &Effect3, NULL);
        return TRUE;
    }
};