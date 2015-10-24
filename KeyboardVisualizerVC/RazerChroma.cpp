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


bool RazerChroma::SetLEDs(COLORREF pixels[64][256]) 
{
    CreateEffect = (CREATEEFFECT)GetProcAddress(hModule, "CreateEffect");
    CreateKeyboardEffect = (CREATEKEYBOARDEFFECT)GetProcAddress(hModule, "CreateKeyboardEffect");
    CreateMouseEffect = (CREATEMOUSEEFFECT)GetProcAddress(hModule, "CreateMouseEffect");
    CreateHeadsetEffect = (CREATEHEADSETEFFECT)GetProcAddress(hModule, "CreateHeadsetEffect");

    if (CreateEffect == NULL)
    {
        return FALSE;
    }
    else
    {
        //Blackwidow Chroma
        CUSTOM_EFFECT_TYPE BlackWidowEffect;

        for (int x = 0; x < 22; x++)
        {
            for (int y = 0; y < 6; y++)
            {
                int x_idx = x * (256 / 22);
                int y_idx = y * (64 / 6) + (0.5f * (64 / 6));
                BlackWidowEffect.Color[y][x] = (pixels[y_idx][x_idx] & 0x00FFFFFF);
            }
        }

        //Set Razer "Three Headed Snake" logo to the background color of the 11th column
        BlackWidowEffect.Color[0][20] = pixels[3][11 * (256 / 22)];

        CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);

        //Blackwidow Chroma Tournament Edition
        CUSTOM_EFFECT_TYPE BlackWidowTEEffect;

        for (int x = 0; x < 18; x++)
        {
            for (int y = 0; y < 6; y++)
            {
                int x_idx = x * (256 / 18);
                int y_idx = y * (64 / 6) + (0.5f * (64 / 6));
                BlackWidowTEEffect.Color[y][x] = (pixels[y_idx][x_idx] & 0x00FFFFFF);
            }
        }

        //Set Razer "Three Headed Snake" logo to the background color of the 11th column
        BlackWidowTEEffect.Color[0][20] = pixels[3][11 * (256 / 22)];

        CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA_TE, ChromaSDK::CHROMA_CUSTOM, &BlackWidowTEEffect, NULL);

        //Firefly Chroma
        ChromaSDK::Mousepad::CUSTOM_EFFECT_TYPE FireflyEffect = {};

        for (int x = 0; x < 15; x++)
        {
            if (x < 8)
            {
                FireflyEffect.Color[x - 1] = pixels[0][7 + (x * 16)];
            }
            else
            {
                FireflyEffect.Color[x] = pixels[0][8 + (x * 16)];
            }
        }
        FireflyEffect.Color[7] = pixels[0][128];

        CreateEffect(ChromaSDK::FIREFLY_CHROMA, ChromaSDK::CHROMA_CUSTOM, &FireflyEffect, NULL);

        //Mamba Chroma Tournament Edition
        ChromaSDK::Mouse::CUSTOM_EFFECT_TYPE MambaEffect = {};

        for (int x = 0; x < 7; x++)
        {
            MambaEffect.Color[x + 4] = pixels[0][(16 * (x + 1))];
            MambaEffect.Color[x + 11] = pixels[0][(16 * (x + 1))];
        }

        //Set scroll wheel and logo LEDs to background color
        MambaEffect.Color[2] = pixels[3][16 * 5];
        MambaEffect.Color[1] = pixels[3][0];

        //CreateEffect(ChromaSDK::MAMBA_CHROMA_TE, ChromaSDK::CHROMA_CUSTOM, &MambaEffect, NULL);

        CreateMouseEffect(ChromaSDK::Mouse::CHROMA_CUSTOM, &MambaEffect, NULL);

        //DeathAdder Chroma
        //ChromaSDK::Mouse::CUSTOM_EFFECT_TYPE DeathAdderEffect = {};

        //Set scroll wheel and logo LEDs to background color
        //DeathAdderEffect.Color[2] = pixels[3][0];
        //DeathAdderEffect.Color[1] = pixels[3][0];

        //CreateEffect(ChromaSDK::DEATHADDER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &DeathAdderEffect, NULL);

        //Kraken Chroma
        ChromaSDK::Headset::STATIC_EFFECT_TYPE KrakenEffect;

        KrakenEffect.Color = pixels[3][0];

        CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_NONE, &KrakenEffect, NULL);
        CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_STATIC, &KrakenEffect, NULL);

        //DeathStalker Chroma
        CreateEffect(ChromaSDK::DEATHSTALKER_CHROMA, ChromaSDK::CHROMA_STATIC, &BlackWidowEffect, NULL);

        //Tartarus Chroma
        ChromaSDK::Keypad::CUSTOM_EFFECT_TYPE TartarusEffect;

        TartarusEffect.Color[0][0] = pixels[3][0];

        CreateEffect(ChromaSDK::TARTARUS_CHROMA, ChromaSDK::CHROMA_CUSTOM, &TartarusEffect, NULL);

        //Orbweaver Chroma
        ChromaSDK::Keypad::CUSTOM_EFFECT_TYPE OrbweaverEffect;

        for (int x = 0; x < 5; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                int x_idx = x * (256 / 5);
                int y_idx = y * (64 / 4) + (0.5f * (64 / 4));
                OrbweaverEffect.Color[y][x] = (pixels[y_idx][x_idx] & 0x00FFFFFF);
            }
        }

        CreateEffect(ChromaSDK::ORBWEAVER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &OrbweaverEffect, NULL);

        return TRUE;
    }
};