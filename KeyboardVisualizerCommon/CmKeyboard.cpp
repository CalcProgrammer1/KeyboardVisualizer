//==================================================================================================
// 2016/7/23 Sen Zhao pz140202@gmail.com 
// Powered by Cooler Master SDK v160630
//==================================================================================================

#include "CmKeyboard.h"
#include "VisualizerDefines.h"

#define COOLER_MASTER_ENABLED

#ifdef COOLER_MASTER_ENABLED
#include  <iostream>

static COLOR_MATRIX cm;
bool INITED = false; 
int device = -1;	// device type : 0- Masterkeys Pro L ; 1- Masterkeys Pro S
bool isplug;	

int dev_column; 
int y_idx_list[6];
int col;

static float LED_TABLE[6][22] = {
/*	R\C	0		1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		16		17		18		19		20		21	*/
/* 0 */	{0,		2,		3,		4,		5,		-1,		6.5,	7.5,	8.5,	9.5,	-1,		11,		12,		13,		14,		15.25,	16.25,	17.25,	18.5,	19.5,	20.5,	21.5},
/* 1 */ {0,		1,		2,		3,		4,		5,		6,		7,		8,		9,		10,		11,		12,		-1,		13.5,	15.25,	16.25,	17.25,	18.5,	19.5,	20.5,	21.5},
/* 2 */ {0.25,	1.5,	2.5,	3.5,	4.5,	5.5,	6.5,	7.5,	8.5,	9.5,	10.5,	11.5,	12.5,	-1,		13.75,	15.25,	16.25,	17.25,	18.5,	19.5,	20.5,	21.5,},
/* 3 */ {0.375,	1.75,	2.75,	3.75,	4.75,	5.75,	6.75,	7.75,	8.75,	9.75,	10.75,	11.75,	-1,		-1,		13.375,	-1,		-1,		-1,		18.5,	19.5,	20.5,	-1},
/* 4 */ {0.875, -1,		2.25,	3.25,	4.25,	5.25,	6.25,	7.25,	8.25,	9.25,	10.25,	11.25,	-1,		-1,		13.125,	-1,		16.25,	-1,		18.5,	19.5,	20.5,	21.5},
/* 5 */ {0.125,	1.375,	2.625,	-1,		-1,		-1,		6.375,	-1,		-1,		-1,		10.125,	11.375,	12.625, -1,		13.875,	15.25,	16.25,	17.25,	19.5,	-1,		20.5,	-1}
};
#endif

CmKeyboard::CmKeyboard()
{
}

CmKeyboard::~CmKeyboard()
{
#ifdef COOLER_MASTER_ENABLED
	EnableLedControl(false);
#endif
}

void CmKeyboard::Initialize()
{
#ifdef COOLER_MASTER_ENABLED
	//device selection
	SetControlDevice(DEV_MKeys_S);
	isplug = IsDevicePlug();
	if (isplug == 1)
    {
		device = 1;
	}
	else
    {
		SetControlDevice(DEV_MKeys_L);
		isplug = IsDevicePlug();
		if (isplug == 1)
        {
			device = 0;
		}
	}

    for (int y = 0; y < 6; y++)
    {
        y_idx_list[y] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / 6)) + (0.5f * (SPECTROGRAPH_ROWS / 6)));
    }

	// count "col" by device type
	if (device == 0)
    {
		dev_column = 22;
		col = (int)(SPECTROGRAPH_END / 22);
	}
	else if (device == 1)
    {
		dev_column = 18;
		col = (int)(SPECTROGRAPH_END / 18);
	}

	// finish init
	if (isplug == 1)
    {
		EnableLedControl(true);
		RefreshLed(true);
		INITED = true;
	}
#endif
}

bool CmKeyboard::SetLEDs(COLORREF pixels[64][256])
{
#ifdef COOLER_MASTER_ENABLED
	if (INITED == true)
    {
		for (int i = 0; i < 6; i++)
        {
			for (int j = 0; j < dev_column; j++)
            {
				if (LED_TABLE[i][j] >= 0)
                {
					cm.KeyColor[i][j].r = GetRValue(pixels[y_idx_list[i]][(int)(col * LED_TABLE[i][j]) + col]);
					cm.KeyColor[i][j].g = GetGValue(pixels[y_idx_list[i]][(int)(col * LED_TABLE[i][j]) + col]);
					cm.KeyColor[i][j].b = GetBValue(pixels[y_idx_list[i]][(int)(col * LED_TABLE[i][j]) + col]);
				}
				
			}
		}
		if (SetAllLedColor(cm) == false)
        {
			SetAllLedColor(cm);
		}
	}

	return INITED;
#else
    return FALSE;
#endif
}