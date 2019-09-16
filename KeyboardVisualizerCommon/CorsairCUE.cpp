/*---------------------------------------------------------*\
|  Processing Code for Corsair CUE SDK Interface            |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

/* TODO
 * Add mice support [DONE]
 * Fix incorrect rainbow/gradient positioning [NEEDS VALIDATION]
 * Update deprecated lines [DONE]
 * Fix iCUE 3.19 issue [DONE]
 */

#include "CorsairCUE.h"
#include "VisualizerDefines.h"

#define CORSAIR_CUE_ENABLED

static boolean failed;

#ifdef CORSAIR_CUE_ENABLED
#include <algorithm>
#include <vector>

#pragma comment(lib, "CUESDK_2013.lib")
#pragma comment(lib, "CUESDK.x64_2013.lib")

struct CueDevice
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

std::vector<CueDevice> _CueDeviceList;
CueDevice _CueDevice;
#endif

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

void CorsairCUE::Setup()
{
	_CueDevice.model = CorsairGetDeviceInfo(_CueDevice.devID)->model;
	_CueDevice.type = CorsairGetDeviceInfo(_CueDevice.devID)->type;
	_CueDevice.positions = CorsairGetLedPositionsByDeviceIndex(_CueDevice.devID);

	_CueDevice.colors = new CorsairLedColor[_CueDevice.positions->numberOfLed];
	_CueDevice.x_idx = new int[_CueDevice.positions->numberOfLed];
	_CueDevice.y_idx = new int[_CueDevice.positions->numberOfLed];
	_CueDevice.led_idx = new CorsairLedId[_CueDevice.positions->numberOfLed];

	_CueDeviceList.push_back(_CueDevice);
}

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
		
		for (auto i = 0; i < CorsairGetDeviceCount(); i++) {
			if (CorsairGetDeviceInfo(i)->type == CDT_Keyboard)
			{
				_CueDevice.devID = i;
				Setup();
			}
			else if (CorsairGetDeviceInfo(i)->type == CDT_Mouse)
			{
				_CueDevice.devID = i;
				Setup();
			}
		}

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

		for (int a = 0; a < _CueDeviceList.size(); a++)  if (&_CueDeviceList[a]) {
			double _width = getKeyboardWidth(_CueDeviceList[a].positions);
			double _height = getKeyboardHeight(_CueDeviceList[a].positions);

			for (int i = 0; i < _CueDeviceList[a].positions->numberOfLed; i++)
			{
				_CueDeviceList[a].led_idx[i] = _CueDeviceList[a].positions->pLedPosition[i].ledId;

				if (strcmp(_CueDeviceList[a].model, "STRAFE RGB MK.2") == 0)
				{
					// I don't know if the incorrect positioning only applies to the STRAFE RGB MK.2 so I added that workaround
					_CueDeviceList[a].x_idx[i] = (int)(SPECTROGRAPH_END * (_CueDeviceList[a].positions->pLedPosition[i].left / _width)) - 10;
				}
				else
				{
					_CueDeviceList[a].x_idx[i] = (int)(SPECTROGRAPH_END * (_CueDeviceList[a].positions->pLedPosition[i].left / _width));
				}

				_CueDeviceList[a].y_idx[i] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (SPECTROGRAPH_ROWS * (_CueDeviceList[a].positions->pLedPosition[i].top / _height)) + (1.0f * (SPECTROGRAPH_ROWS / _height)));
			}

			for (int i = 0; i < _CueDeviceList[a].positions->numberOfLed; i++)
			{
				COLORREF _color = pixels[_CueDeviceList[a].y_idx[i]][_CueDeviceList[a].x_idx[i]];
				_CueDeviceList[a].colors[i].r = GetRValue(_color);
				_CueDeviceList[a].colors[i].g = GetGValue(_color);
				_CueDeviceList[a].colors[i].b = GetBValue(_color);
				_CueDeviceList[a].colors[i].ledId = _CueDeviceList[a].led_idx[i];
			}

			CorsairSetLedsColorsBufferByDeviceIndex(_CueDeviceList[a].devID, _CueDeviceList[a].positions->numberOfLed, _CueDeviceList[a].colors);
			CorsairSetLedsColorsFlushBuffer();
		}

		return TRUE;
#endif
	}
}