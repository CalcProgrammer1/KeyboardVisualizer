/*---------------------------------------------------------*\
|  Processing Code for Corsair CUE SDK Interface            |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

/* TODO
 * Add mouse support [DONE]
 * Fix incorrect rainbow/gradient position [NEEDS VALIDATION]
 * Update deprecated lines [DONE]
 * Fix iCUE 3.19 issue [DONE]
 */

#include "CorsairCUE.h"
#include "VisualizerDefines.h"

#define CORSAIR_CUE_ENABLED

static boolean failed;

#ifdef CORSAIR_CUE_ENABLED
#include <algorithm>

#pragma comment(lib, "CUESDK_2013.lib")
#pragma comment(lib, "CUESDK.x64_2013.lib")

struct iCUEDevice
{
	const char* model;
	CorsairDeviceType type;
	CorsairLedColor* colors;
	CorsairLedPositions* positions;
	int* x_idx;
	int* y_idx;
	int devID = 0;
	CorsairLedId* led_idx;
};
#endif

iCUEDevice _mouse;
iCUEDevice _keyboard;

CorsairCUE::CorsairCUE()
{
}


CorsairCUE::~CorsairCUE()
{
}

#ifdef CORSAIR_CUE_ENABLED
double getKeyboardHeight(CorsairLedPositions* ledPositions)
{
	const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
		[](const CorsairLedPosition& clp1, const CorsairLedPosition& clp2) {
			return clp1.top < clp2.top;
		});
	return minmaxLeds.second->top + minmaxLeds.second->height - minmaxLeds.first->top;
}

double getKeyboardWidth(CorsairLedPositions* ledPositions)
{
	const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
		[](const CorsairLedPosition& clp1, const CorsairLedPosition& clp2) {
			return clp1.left < clp2.left;
		});
	return minmaxLeds.second->left + minmaxLeds.second->width - minmaxLeds.first->left;
}
#endif

void CorsairCUE::Initialize()
{
#ifdef CORSAIR_CUE_ENABLED
	CorsairPerformProtocolHandshake();
	if (const auto error = CorsairGetLastError())
	{
		failed = TRUE;
	}
	else
	{
		failed = FALSE;

		/*
		enum CorsairDeviceType
		// contains list of available device types.
		CDT_Unknown = 0,
		CDT_Mouse = 1,
		CDT_Keyboard = 2,
		CDT_Headset = 3,
		CDT_MouseMat = 4,
		CDT_HeadsetStand = 5,
		CDT_CommanderPro = 6,
		CDT_LightingNodePro = 7,
		CDT_MemoryModule = 8,
		CDT_Cooler = 9
		*/

		for (auto i = 0; i < CorsairGetDeviceCount(); i++) {
			if (CorsairGetDeviceInfo(i)->type == 2)
			{
				_keyboard.devID = i;
			}
			else if (CorsairGetDeviceInfo(i)->type == 1)
			{
				_mouse.devID = i;
			}
		}

		// keyboard		
		_keyboard.model = CorsairGetDeviceInfo(_keyboard.devID)->model;
		_keyboard.type = CorsairGetDeviceInfo(_keyboard.devID)->type;
		_keyboard.positions = CorsairGetLedPositionsByDeviceIndex(_keyboard.devID); //keyboard

		_keyboard.colors = new CorsairLedColor[_keyboard.positions->numberOfLed];
		_keyboard.x_idx = new int[_keyboard.positions->numberOfLed];
		_keyboard.y_idx = new int[_keyboard.positions->numberOfLed];
		_keyboard.led_idx = new CorsairLedId[_keyboard.positions->numberOfLed];

		// mouse
		_mouse.model = CorsairGetDeviceInfo(_mouse.devID)->model;
		_mouse.type = CorsairGetDeviceInfo(_mouse.devID)->type;
		_mouse.positions = CorsairGetLedPositionsByDeviceIndex(_mouse.devID); //mouse

		_mouse.colors = new CorsairLedColor[_mouse.positions->numberOfLed];
		_mouse.x_idx = new int[_mouse.positions->numberOfLed];
		_mouse.y_idx = new int[_mouse.positions->numberOfLed];
		_mouse.led_idx = new CorsairLedId[_mouse.positions->numberOfLed];

		failed = FALSE;
	}
#else
	failed = TRUE;
#endif
}

bool CorsairCUE::SetLEDs(COLORREF pixels[64][256])
{
	if (failed)
	{
		return FALSE;
	}
	else
	{
#ifdef CORSAIR_CUE_ENABLED

		// keyboard
		double _keyboard_width = getKeyboardWidth(_keyboard.positions);
		double _keyboard_height = getKeyboardHeight(_keyboard.positions);

		for (int i = 0; i < _keyboard.positions->numberOfLed; i++)
		{
			_keyboard.led_idx[i] = _keyboard.positions->pLedPosition[i].ledId;
			
			if (strcmp (_keyboard.model, "STRAFE RGB MK.2") == 0) // Don't know if the incorrect position only applies to the STRAFE RGB MK.2 so I added that workaround
			{
				_keyboard.x_idx[i] = (int)(SPECTROGRAPH_END * (_keyboard.positions->pLedPosition[i].left / _keyboard_width)) - 10;
			}
			else
			{
				_keyboard.x_idx[i] = (int)(SPECTROGRAPH_END * (_keyboard.positions->pLedPosition[i].left / _keyboard_width));
			}

			_keyboard.y_idx[i] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (SPECTROGRAPH_ROWS * (_keyboard.positions->pLedPosition[i].top / _keyboard_height)) + (1.0f * (SPECTROGRAPH_ROWS / _keyboard_height)));
		}

		for (int i = 0; i < _keyboard.positions->numberOfLed; i++)
		{
			COLORREF _keyboard_color = pixels[_keyboard.y_idx[i]][_keyboard.x_idx[i]];
			_keyboard.colors[i].r = GetRValue(_keyboard_color);
			_keyboard.colors[i].g = GetGValue(_keyboard_color);
			_keyboard.colors[i].b = GetBValue(_keyboard_color);
			_keyboard.colors[i].ledId = _keyboard.led_idx[i];
		}

		CorsairSetLedsColorsBufferByDeviceIndex(_keyboard.devID, _keyboard.positions->numberOfLed, _keyboard.colors);
		CorsairSetLedsColorsFlushBuffer();

		// mouse
		double _mouse_width = getKeyboardWidth(_mouse.positions);
		double _mouse_height = getKeyboardHeight(_mouse.positions);

		for (int i = 0; i < _mouse.positions->numberOfLed; i++)
		{
			_mouse.led_idx[i] = _mouse.positions->pLedPosition[i].ledId;
			_mouse.x_idx[i] = (int)(SPECTROGRAPH_END * (_mouse.positions->pLedPosition[i].left / _mouse_width));
			_mouse.y_idx[i] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (SPECTROGRAPH_ROWS * (_mouse.positions->pLedPosition[i].top / _mouse_height)) + (0.5f * (SPECTROGRAPH_ROWS / _mouse_height)));
		}

		for (int i = 0; i < _mouse.positions->numberOfLed; i++)
		{
			COLORREF _mouse_color = pixels[_mouse.y_idx[i]][_mouse.x_idx[i]];
			_mouse.colors[i].r = GetRValue(_mouse_color);
			_mouse.colors[i].g = GetGValue(_mouse_color);
			_mouse.colors[i].b = GetBValue(_mouse_color);
			_mouse.colors[i].ledId = _mouse.led_idx[i];
		}

		CorsairSetLedsColorsBufferByDeviceIndex(_mouse.devID, _mouse.positions->numberOfLed, _mouse.colors);
		CorsairSetLedsColorsFlushBuffer();

		return TRUE;
#endif
	}
}