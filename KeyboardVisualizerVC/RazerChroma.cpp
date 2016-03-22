#include "RazerChroma.h"

#include <iostream>

//Index lists for BlackWidow
int BlackWidowXIndex[22];
int BlackWidowYIndex[6];

//Index lists for BlackWidow TE
int BlackWidowTEXIndex[18];
int BlackWidowTEYIndex[6];

//Index lists for Blade Stealth
int BladeStealthXIndex[16];
int BladeStealthYIndex[6];

//Index list for Firefly
int FireflyIndex[15];

//Index list for mice (Mamba TE, DeathAdder)
int MouseXIndex[9][7];
int MouseYIndex[9][7];

//Index list for DeathStalker
static int DeathStalkerXLEDIndex[] = { 1, 4, 8, 12, 15, 18 };
int DeathStalkerXIndex[6];

//Index lists for Orbweaver
int OrbweaverXIndex[5];
int OrbweaverYIndex[4];

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
            //Initialize the SDK
			Init();

            //Build index list for BlackWidow
            for (int x = 0; x < 22; x++)
            {
                int x_idx = x * (256 / 22);
                BlackWidowXIndex[x] = x_idx;
            }
            for (int y = 0; y < 6; y++)
            {
                int y_idx = y * (64 / 6) + (0.5f * (64 / 6));
                BlackWidowYIndex[y] = y_idx;
            }

            //Build index list for BlackWidow TE
            for (int x = 0; x < 18; x++)
            {
                int x_idx = x * (256 / 18);
                BlackWidowTEXIndex[x] = x_idx;
            }
            for (int y = 0; y < 6; y++)
            {
                int y_idx = y * (64 / 6) + (0.5f * (64 / 6));
                BlackWidowTEYIndex[y] = y_idx;
            }

            //Build index list for Blade Stealth
            for (int x = 0; x < 16; x++)
            {
                int x_idx = x * (256 / 16);
                BladeStealthXIndex[x] = x_idx;
            }
            for (int y = 0; y < 6; y++)
            {
                int y_idx = y * (64 / 6) + (0.5f * (64 / 6));
                BladeStealthYIndex[y] = y_idx;
            }

            //Build index list for Firefly
            for (int x = 0; x < 15; x++)
            {
                if (x == 7)
                {
                    FireflyIndex[x] = 128;
                }
                else if (x < 8)
                {
                    FireflyIndex[x] = 7 + ((x+1) * 16);
                }
                else
                {
                    FireflyIndex[x] = 8 + (x * 16);
                }
            }

            //Build index list for mice
            for (int x = 0; x < 7; x++)
            {
                for (int y = 0; y < 9; y++)
                {
                    //Set scroll wheel LED
                    if ((x == 3) && (y == 2))
                    {
                        MouseXIndex[y][x] = 0;
                        MouseYIndex[y][x] = 3;
                    }
                    //Set logo LED
                    else if ((x == 3) && (y == 7))
                    {
                        MouseXIndex[y][x] = 14 * 5;
                        MouseYIndex[y][x] = 3;
                    }
                    //Set keypad LED
                    else if ((x == 3) && (y == 4))
                    {
                        MouseXIndex[y][x] = 14 * 5;
                        MouseYIndex[y][x] = 3;
                    }
                    //Set side LEDs
                    else if (((x == 0) || (x == 6)) && (y > 0) && (y < 8))
                    {
                        MouseXIndex[y][x] = 14 * ( y );
                        MouseYIndex[y][x] = 0;
                    }
                    //Set bottom LEDs
                    else if ((y == 8) && (x > 0) && (x < 6))
                    {
                        if ((x == 1) || (x == 5))
                        {
                            MouseXIndex[y][x] = 14 * (y);
                            MouseYIndex[y][x] = 0;
                        }
                        else if ((x == 2) || (x == 4))
                        {
                            MouseXIndex[y][x] = 14 * (y + 1);
                            MouseYIndex[y][x] = 0;
                        }
                        else if (x == 3)
                        {
                            MouseXIndex[y][x] = 14 * (y + 2);
                            MouseYIndex[y][x] = 0;
                        }
                    }
                }
            }

            //Build index list for DeathStalker
            for (int x = 0; x < 6; x++)
            {
                DeathStalkerXIndex[x] = 128 + (x * (256 / 12));
            }

            //Build index list for OrbWeaver
            for (int x = 0; x < 5; x++)
            {
                OrbweaverXIndex[x] = x * (256 / 5);
            }
            for (int y = 0; y < 4; y++)
            {
                OrbweaverYIndex[y] = y * (64 / 4) + (0.5f * (64 / 4));
            }
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
        ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE BlackWidowEffect;

        for (int x = 0; x < 22; x++)
        {
            for (int y = 0; y < 6; y++)
            {
                BlackWidowEffect.Color[y][x] = (pixels[BlackWidowYIndex[y]][BlackWidowXIndex[x]] & 0x00FFFFFF);
            }
        }

        //Set Razer "Three Headed Snake" logo to the background color of the 11th column
        BlackWidowEffect.Color[0][20] = pixels[3][11 * (256 / 22)];

        CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);

        CreateEffect(ChromaSDK::BLACKWIDOW_X_CHROMA, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);
        CreateEffect(ChromaSDK::BLACKWIDOW_X_TE_CHROMA, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);
        CreateEffect(ChromaSDK::OVERWATCH_KEYBOARD, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);

        //Blackwidow Chroma Tournament Edition
        ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE BlackWidowTEEffect;

        for (int x = 0; x < 18; x++)
        {
            for (int y = 0; y < 6; y++)
            {
                BlackWidowTEEffect.Color[y][x] = (pixels[BlackWidowTEYIndex[y]][BlackWidowTEXIndex[x]] & 0x00FFFFFF);
            }
        }

        //Set Razer "Three Headed Snake" logo to the background color of the 11th column
        BlackWidowTEEffect.Color[0][20] = pixels[3][11 * (256 / 22)];

        CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA_TE, ChromaSDK::CHROMA_CUSTOM, &BlackWidowTEEffect, NULL);

        //Blade Stealth
        ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE BladeStealthEffect;

        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 6; y++)
            {
                BladeStealthEffect.Color[y][x] = (pixels[BladeStealthYIndex[y]][BladeStealthXIndex[x]] & 0x00FFFFFF);
            }
        }

        CreateEffect(ChromaSDK::BLADE_STEALTH, ChromaSDK::CHROMA_CUSTOM, &BladeStealthEffect, NULL);

        //Firefly Chroma
        ChromaSDK::Mousepad::CUSTOM_EFFECT_TYPE FireflyEffect = {};

        for (int x = 0; x < 15; x++)
        {
            FireflyEffect.Color[x] = pixels[0][FireflyIndex[x]];
        }

        CreateEffect(ChromaSDK::FIREFLY_CHROMA, ChromaSDK::CHROMA_CUSTOM, &FireflyEffect, NULL);

        //Mamba Chroma Tournament Edition
        ChromaSDK::Mouse::CUSTOM_EFFECT_TYPE2 MouseEffect = {};

        for (int x = 0; x < 7; x++)
        {
            for (int y = 0; y < 9; y++)
            {
                MouseEffect.Color[y][x] = pixels[MouseYIndex[y][x]][MouseXIndex[y][x]];
            }
        }

        CreateMouseEffect(ChromaSDK::Mouse::CHROMA_CUSTOM2, &MouseEffect, NULL);

        //Kraken Chroma
        ChromaSDK::Headset::STATIC_EFFECT_TYPE KrakenEffect;

        KrakenEffect.Color = pixels[3][0];

        CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_NONE, &KrakenEffect, NULL);
        CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_STATIC, &KrakenEffect, NULL);

        //DeathStalker Chroma
        
        ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE DeathStalkerEffect;

        for (int x = 0; x < 6; x++)
        {
            DeathStalkerEffect.Color[1][DeathStalkerXLEDIndex[x]] = (pixels[0][DeathStalkerXIndex[x]] & 0x00FFFFFF);
        }

        CreateEffect(ChromaSDK::DEATHSTALKER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &DeathStalkerEffect, NULL);

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
                OrbweaverEffect.Color[y][x] = (pixels[OrbweaverYIndex[y]][OrbweaverXIndex[x]] & 0x00FFFFFF);
            }
        }

        CreateEffect(ChromaSDK::ORBWEAVER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &OrbweaverEffect, NULL);

        return TRUE;
    }
};