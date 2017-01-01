/*---------------------------------------------------------*\
|  Processing Code for Razer Chroma SDK Interface           |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

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

//Index list for Razer Core
int CoreXIndex[8];

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


void SetupKeyboardGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for (int x = 0; x < x_count; x++)
    {
        if (x < ((x_count) / 2))
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) + (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }
        else
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) - (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }
        
    }
    for (int y = 0; y < y_count; y++)
    {
        y_idx_list[y] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / y_count)) + (0.5f * (SPECTROGRAPH_ROWS / y_count)));
    }
}


void FillKeyboardGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list, ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE * effect, COLORREF pixels[64][256])
{
    for (int x = 0; x < x_count; x++)
    {
        for (int y = 0; y < y_count; y++)
        {
            effect->Color[y][x] = (pixels[y_idx_list[y]][x_idx_list[x]] & 0x00FFFFFF);
        }
    }
}


void FillKeypadGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list, ChromaSDK::Keypad::CUSTOM_EFFECT_TYPE * effect, COLORREF pixels[64][256])
{
    for (int x = 0; x < x_count; x++)
    {
        for (int y = 0; y < y_count; y++)
        {
            effect->Color[y][x] = (pixels[y_idx_list[y]][x_idx_list[x]] & 0x00FFFFFF);
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
            SetupKeyboardGrid(22, 6, BlackWidowXIndex, BlackWidowYIndex);

            //Build index list for BlackWidow TE
            SetupKeyboardGrid(18, 6, BlackWidowTEXIndex, BlackWidowTEYIndex);

            //Build index list for Blade Stealth
            SetupKeyboardGrid(16, 6, BladeStealthXIndex, BladeStealthYIndex);

            //Build index list for OrbWeaver
            SetupKeyboardGrid(5, 4, OrbweaverXIndex, OrbweaverYIndex);

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
                        MouseYIndex[y][x] = ROW_IDX_SINGLE_COLOR;
                    }
                    //Set logo LED
                    else if ((x == 3) && (y == 7))
                    {
                        MouseXIndex[y][x] = 14 * 5;
                        MouseYIndex[y][x] = ROW_IDX_SINGLE_COLOR;
                    }
                    //Set keypad LED
                    else if ((x == 3) && (y == 4))
                    {
                        MouseXIndex[y][x] = 14 * 5;
                        MouseYIndex[y][x] = ROW_IDX_SINGLE_COLOR;
                    }
                    //Set side LEDs
                    else if (((x == 0) || (x == 6)) && (y > 0) && (y < 8))
                    {
                        MouseXIndex[y][x] = 14 * ( y );
                        MouseYIndex[y][x] = ROW_IDX_BAR_GRAPH;
                    }
                    //Set bottom LEDs
                    else if ((y == 8) && (x > 0) && (x < 6))
                    {
                        if ((x == 1) || (x == 5))
                        {
                            MouseXIndex[y][x] = 14 * (y);
                            MouseYIndex[y][x] = ROW_IDX_BAR_GRAPH;
                        }
                        else if ((x == 2) || (x == 4))
                        {
                            MouseXIndex[y][x] = 14 * (y + 1);
                            MouseYIndex[y][x] = ROW_IDX_BAR_GRAPH;
                        }
                        else if (x == 3)
                        {
                            MouseXIndex[y][x] = 14 * (y + 2);
                            MouseYIndex[y][x] = ROW_IDX_BAR_GRAPH;
                        }
                    }
                }
            }

            //Build index list for Core
            for (int x = 0; x < 8; x++)
            {
                CoreXIndex[x] = (x * (256 / 8)) + (256 / 16);
            }

            //Build index list for DeathStalker
            for (int x = 0; x < 6; x++)
            {
                DeathStalkerXIndex[x] = 128 + (x * (256 / 12));
            }
		}
	}
}


bool RazerChroma::SetLEDs(COLORREF pixels[64][256]) 
{
    CreateEffect = (CREATEEFFECT)GetProcAddress(hModule, "CreateEffect");
    CreateKeyboardEffect = (CREATEKEYBOARDEFFECT)GetProcAddress(hModule, "CreateKeyboardEffect");
    CreateMouseEffect = (CREATEMOUSEEFFECT)GetProcAddress(hModule, "CreateMouseEffect");
    CreateMousepadEffect = (CREATEMOUSEPADEFFECT)GetProcAddress(hModule, "CreateMousepadEffect");
    CreateHeadsetEffect = (CREATEHEADSETEFFECT)GetProcAddress(hModule, "CreateHeadsetEffect");

    if (CreateEffect == NULL || pixels == NULL)
    {
        return FALSE;
    }
    else
    {
        //Blackwidow Chroma
        ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE BlackWidowEffect;

        FillKeyboardGrid(22, 6, BlackWidowXIndex, BlackWidowYIndex, &BlackWidowEffect, pixels);

        //Set Razer "Three Headed Snake" logo to the background color of the 11th column
        BlackWidowEffect.Color[0][20] = pixels[ROW_IDX_SINGLE_COLOR][11 * (256 / 22)];

        CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);

        CreateEffect(ChromaSDK::BLACKWIDOW_X_CHROMA, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);
        CreateEffect(ChromaSDK::BLACKWIDOW_X_TE_CHROMA, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);
        CreateEffect(ChromaSDK::OVERWATCH_KEYBOARD, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);
        CreateEffect(ChromaSDK::ORNATA_CHROMA, ChromaSDK::CHROMA_CUSTOM, &BlackWidowEffect, NULL);
        
        //Blackwidow Chroma Tournament Edition
        ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE BlackWidowTEEffect;

        FillKeyboardGrid(18, 6, BlackWidowTEXIndex, BlackWidowTEYIndex, &BlackWidowTEEffect, pixels);

        //Set Razer "Three Headed Snake" logo to the background color of the 11th column
        BlackWidowTEEffect.Color[0][20] = pixels[ROW_IDX_SINGLE_COLOR][11 * (256 / 22)];

        CreateEffect(ChromaSDK::BLACKWIDOW_CHROMA_TE, ChromaSDK::CHROMA_CUSTOM, &BlackWidowTEEffect, NULL);

        //Blade Stealth and Blade 14
        ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE BladeStealthEffect;

        FillKeyboardGrid(16, 6, BladeStealthXIndex, BladeStealthYIndex, &BladeStealthEffect, pixels);

        CreateEffect(ChromaSDK::BLADE_STEALTH, ChromaSDK::CHROMA_CUSTOM, &BladeStealthEffect, NULL);
        CreateEffect(ChromaSDK::BLADE, ChromaSDK::CHROMA_CUSTOM, &BladeStealthEffect, NULL);

        //Orbweaver Chroma
        ChromaSDK::Keypad::CUSTOM_EFFECT_TYPE OrbweaverEffect;

        FillKeypadGrid(5, 4, OrbweaverXIndex, OrbweaverYIndex, &OrbweaverEffect, pixels);

        CreateEffect(ChromaSDK::ORBWEAVER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &OrbweaverEffect, NULL);
        
        //Firefly Chroma
        ChromaSDK::Mousepad::CUSTOM_EFFECT_TYPE FireflyEffect = {};

        for (int x = 0; x < 15; x++)
        {
            FireflyEffect.Color[x] = pixels[ROW_IDX_BAR_GRAPH][FireflyIndex[x]];
        }

        CreateMousepadEffect(ChromaSDK::Mousepad::CHROMA_CUSTOM, &FireflyEffect, NULL);

        //Razer Core
        ChromaSDK::Mousepad::CUSTOM_EFFECT_TYPE CoreEffect = {};

        for (int x = 0; x < 8; x++)
        {
            CoreEffect.Color[x+3] = pixels[ROW_IDX_BAR_GRAPH][CoreXIndex[x]];
        }

        //Razer Core internal LEDs are a single color zone
        CoreEffect.Color[2] = pixels[ROW_IDX_SINGLE_COLOR][0];

        CreateEffect(ChromaSDK::CORE_CHROMA, ChromaSDK::CHROMA_CUSTOM, &CoreEffect, NULL);

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

        KrakenEffect.Color = pixels[ROW_IDX_SINGLE_COLOR][0];

        CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_NONE, &KrakenEffect, NULL);
        CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_STATIC, &KrakenEffect, NULL);

        //DeathStalker Chroma
        
        ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE DeathStalkerEffect;

        for (int x = 0; x < 6; x++)
        {
            DeathStalkerEffect.Color[1][DeathStalkerXLEDIndex[x]] = (pixels[ROW_IDX_BAR_GRAPH][DeathStalkerXIndex[x]] & 0x00FFFFFF);
        }

        CreateEffect(ChromaSDK::DEATHSTALKER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &DeathStalkerEffect, NULL);

        //Tartarus Chroma
        ChromaSDK::Keypad::CUSTOM_EFFECT_TYPE TartarusEffect;

        TartarusEffect.Color[0][0] = pixels[ROW_IDX_SINGLE_COLOR][0];

        CreateEffect(ChromaSDK::TARTARUS_CHROMA, ChromaSDK::CHROMA_CUSTOM, &TartarusEffect, NULL);

        return TRUE;
    }
};