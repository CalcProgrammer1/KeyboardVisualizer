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

//Index list for ChromaLink
int ChromaLinkXIndex[ChromaSDK::ChromaLink::MAX_LEDS];

//Index list for Chroma HDK Matrix (Chroma Box)
int ChromaBoxXIndex[16];
int ChromaBoxYIndex[4];

//Index list for individual strips bar graph Chroma HDK
int ChromaBox1BarXIndex[4][16];

//Index list for 2 strips bar graph Chroma HDK
int ChromaBox2BarXIndex[4][16];

//Index list for 4 strips bar graph Chroma HDK
int ChromaBox4BarXIndex[4][16];

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


static void SetupKeyboardGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for (int x = 0; x < x_count; x++)
    {
        if (x_count < 10)
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count))) + (0.5f * (SPECTROGRAPH_COLS / (x_count))));
        }
        else if (x < ((x_count) / 2))
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

void FillCustomGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list, ChromaSDK::CUSTOM_EFFECT_TYPE * effect, COLORREF pixels[64][256])
{
    for (int x = 0; x < x_count; x++)
    {
        for (int y = 0; y < y_count; y++)
        {
            effect->Color[y][x] = (pixels[y_idx_list[y]][x_idx_list[x]] & 0x00FFFFFF);
        }
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
    // Initialize variables
    use_keyboard_generic_effect = false;
    use_headset_custom_effect = false;
    use_chromalink_single_color = false;
    disable_chromalink = false;
    chroma_box_mode = 1;

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

            //Build index list for Chroma Box
            SetupKeyboardGrid(16, 4, ChromaBoxXIndex, ChromaBoxYIndex);

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
                DeathStalkerXIndex[x] = 128 + (x * (256 / 12) + (256 / 24));
            }

            //Build index list for ChromaLink Matrix
            for (int x = 0; x < ChromaSDK::ChromaLink::MAX_LEDS; x++)
            {
                ChromaLinkXIndex[x] = (x * (256 / ChromaSDK::ChromaLink::MAX_LEDS)) + (256 / (ChromaSDK::ChromaLink::MAX_LEDS * 2));
            }

            //Build index list for ChromaLink 1 Strip Bar Graph
            for (int y = 0; y < 4; y++)
            {
                for (int x = 0; x < 16; x++)
                {
                    ChromaBox1BarXIndex[y][x] = (x * (256 / 16)) + (256 / 32);
                }
            }

            //Build index list for ChromaLink 2 Strip Bar Graph
            for (int y = 0; y < 4; y += 2)
            {
                for (int x = 0; x < 32; x++)
                {
                    if (x < 16)
                    {
                        ChromaBox2BarXIndex[y][x] = (x * (256 / 32)) + (256 / 64);
                    }
                    else
                    {
                        ChromaBox2BarXIndex[y+1][x-16] = (x * (256 / 32)) + (256 / 64);
                    }
                }
            }

            //Build index list for ChromaLink 4 Strip Bar Graph
            for (int x = 0; x < 64; x++)
            {
                if (x < 16)
                {
                    ChromaBox4BarXIndex[0][15-x] = (x * (256 / 64)) + (256 / 128);
                }
                else if (x < 32)
                {
                    ChromaBox4BarXIndex[1][15-(x-16)] = (x * (256 / 64)) + (256 / 128);
                }
                else if (x < 48)
                {
                    ChromaBox4BarXIndex[2][x-32] = (x * (256 / 64)) + (256 / 128);
                }
                else
                {
                    ChromaBox4BarXIndex[3][x-48] = (x * (256 / 64)) + (256 / 128);
                }
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
    CreateChromaLinkEffect = (CREATECHROMALINKEFFECT)GetProcAddress(hModule, "CreateChromaLinkEffect");

    if (CreateEffect == NULL || pixels == NULL)
    {
        return FALSE;
    }
    else
    {
        //The use_generic_keyboard option uses the generic keyboard effect to apply the same pattern
        //to all Chroma SDK supported keyboards.  This effect is a workaround for missing Blade Stealth
        //Kaby Lake support in the SDK, as the generic effect seems to be the only working way to
        //support this product
        if (use_keyboard_generic_effect)
        {
            //Keyboard Effect
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE KeyboardEffect;

            FillKeyboardGrid(22, 6, BlackWidowXIndex, BlackWidowYIndex, &KeyboardEffect, pixels);
            //Set Razer "Three Headed Snake" logo to the background color of the 11th column
            KeyboardEffect.Color[0][20] = pixels[ROW_IDX_SINGLE_COLOR][11 * (256 / 22)];

            CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &KeyboardEffect, NULL);
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

            //DeathStalker Chroma
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE DeathStalkerEffect;

            for (int x = 0; x < 6; x++)
            {
                DeathStalkerEffect.Color[1][DeathStalkerXLEDIndex[x]] = (pixels[ROW_IDX_BAR_GRAPH][DeathStalkerXIndex[x]] & 0x00FFFFFF);
            }

            CreateEffect(ChromaSDK::DEATHSTALKER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &DeathStalkerEffect, NULL);
        }

        //Orbweaver Chroma
        ChromaSDK::Keypad::CUSTOM_EFFECT_TYPE OrbweaverEffect;
        
        FillKeypadGrid(5, 4, OrbweaverXIndex, OrbweaverYIndex, &OrbweaverEffect, pixels);
        
        CreateEffect(ChromaSDK::ORBWEAVER_CHROMA, ChromaSDK::CHROMA_CUSTOM, &OrbweaverEffect, NULL);
        
        //Tartarus Chroma
        ChromaSDK::Keypad::CUSTOM_EFFECT_TYPE TartarusEffect;

        TartarusEffect.Color[0][0] = pixels[ROW_IDX_SINGLE_COLOR][0];

        CreateEffect(ChromaSDK::TARTARUS_CHROMA, ChromaSDK::CHROMA_CUSTOM, &TartarusEffect, NULL);

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

        CreateEffect(ChromaSDK::OROCHI_CHROMA, ChromaSDK::CHROMA_NONE, NULL, NULL); // Quick-fix for "lazy" Orochi Chroma color transition effect, see https://gfycat.com/DiscreteDisfiguredBluetickcoonhound
        CreateMouseEffect(ChromaSDK::Mouse::CHROMA_CUSTOM2, &MouseEffect, NULL);

        //The Kraken 7.1 Chroma v1 headset (as well as possibly others) does not support custom effects.
        //To work around this, I set a NONE effect followed by a STATIC effect, which instantly updates
        //the color at the expense of a slight flicker.
        //The Kraken 7.1 Chroma v2 does support custom effects, but using them means the v1 gets no
        //pattern at all.  Until the SDK is updated to break these two devices into separate IDs,
        //I'm adding a workaround to select whether to use custom or static effects.
        if (use_headset_custom_effect)
        {
            //Kraken Chroma V2
            ChromaSDK::Headset::STATIC_EFFECT_TYPE KrakenEffect;

            KrakenEffect.Color = pixels[ROW_IDX_SINGLE_COLOR][0];

            CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_CUSTOM, &KrakenEffect, NULL);
        }
        else
        {
            //Kraken Chroma V1
            ChromaSDK::Headset::STATIC_EFFECT_TYPE KrakenEffect;

            KrakenEffect.Color = pixels[ROW_IDX_SINGLE_COLOR][0];

            CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_NONE, &KrakenEffect, NULL);
            CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_STATIC, &KrakenEffect, NULL);
        }

        if (!disable_chromalink)
        {
            //Chroma Link SDK
            ChromaSDK::ChromaLink::CUSTOM_EFFECT_TYPE ChromaLinkEffect = {};

            for (int x = 0; x < ChromaSDK::ChromaLink::MAX_LEDS; x++)
            {
                if (use_chromalink_single_color)
                {
                    ChromaLinkEffect.Color[x] = pixels[ROW_IDX_SINGLE_COLOR][ChromaLinkXIndex[x]];
                }
                else
                {
                    ChromaLinkEffect.Color[x] = pixels[ROW_IDX_BAR_GRAPH][ChromaLinkXIndex[x]];
                }

            }
            CreateChromaLinkEffect(ChromaSDK::ChromaLink::CHROMA_CUSTOM, &ChromaLinkEffect, NULL);
        }

        ChromaSDK::CUSTOM_EFFECT_TYPE ChromaBoxEffect;

        switch (chroma_box_mode)
        {
        case 0:
            FillCustomGrid(16, 4, ChromaBoxXIndex, ChromaBoxYIndex, &ChromaBoxEffect, pixels);
            break;
        default:
        case 1:
            for (int x = 0; x < 16; x++)
            {
                for (int y = 0; y < 4; y++)
                {
                    ChromaBoxEffect.Color[y][x] = pixels[ROW_IDX_BAR_GRAPH][ChromaBox1BarXIndex[y][x]];
                }
            }
            break;
        case 2:
            for (int x = 0; x < 16; x++)
            {
                for (int y = 0; y < 4; y++)
                {
                    ChromaBoxEffect.Color[y][x] = pixels[ROW_IDX_BAR_GRAPH][ChromaBox2BarXIndex[y][x]];
                }
            }
            break;
        case 3:
            for (int x = 0; x < 16; x++)
            {
                for (int y = 0; y < 4; y++)
                {
                    ChromaBoxEffect.Color[y][x] = pixels[ROW_IDX_BAR_GRAPH][ChromaBox4BarXIndex[y][x]];
                }
            }
            break;
        }
        
        CreateEffect(ChromaSDK::CHROMABOX, ChromaSDK::CHROMA_CUSTOM, &ChromaBoxEffect, NULL);

        return TRUE;
    }
};