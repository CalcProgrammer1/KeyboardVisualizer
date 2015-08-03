#pragma once

#include <sstream>
#include <wchar.h>
#include <string.h>
#include <Windows.h>
extern "C"
{
	#include <hidsdi.h>
}
#include <setupapi.h>
#include <cfgmgr32.h>

#define MAX_CORSAIR_ROW 92
#define MAX_CORSAIR_COLUMN 6

//! Custom effect using a grid type
typedef struct CORSAIR_GRID_EFFECT_TYPE
{
	COLORREF Key[MAX_CORSAIR_ROW][MAX_CORSAIR_COLUMN];
} CORSAIR_GRID_EFFECT_TYPE;

class CorsairKeyboard
{
public:
	CorsairKeyboard();
	~CorsairKeyboard();

	int Initialize();
	void SetLEDs(COLORREF pixels[64][256]);

private:
	HANDLE dev;
	void send_usb_msg(char * data_pkt);
};

