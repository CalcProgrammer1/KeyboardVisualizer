/*---------------------------------------------------------*\
|  Processing Code for Corsair CUE SDK Interface            |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

/* TODO
 * Add mouse support [DONE]
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

std::vector<iCUEDevice> _iCUEDevList;
iCUEDevice _iCUEDev;
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
	_iCUEDev.model = CorsairGetDeviceInfo(_iCUEDev.devID)->model;
	_iCUEDev.type = CorsairGetDeviceInfo(_iCUEDev.devID)->type;
	_iCUEDev.positions = CorsairGetLedPositionsByDeviceIndex(_iCUEDev.devID);

	_iCUEDev.colors = new CorsairLedColor[_iCUEDev.positions->numberOfLed];
	_iCUEDev.x_idx = new int[_iCUEDev.positions->numberOfLed];
	_iCUEDev.y_idx = new int[_iCUEDev.positions->numberOfLed];
	_iCUEDev.led_idx = new CorsairLedId[_iCUEDev.positions->numberOfLed];

	_iCUEDevList.push_back(_iCUEDev);
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

		/*
		enum CorsairDeviceType // contains list of available device types.
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
			if (CorsairGetDeviceInfo(i)->type == CDT_Keyboard)
			{
				_iCUEDev.devID = i;
				Setup();
			}
			else if (CorsairGetDeviceInfo(i)->type == CDT_Mouse)
			{
				_iCUEDev.devID = i;
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

		for (int a = 0; a < _iCUEDevList.size(); a++)  if (&_iCUEDevList[a]) {
			double _width = getKeyboardWidth(_iCUEDevList[a].positions);
			double _height = getKeyboardHeight(_iCUEDevList[a].positions);

			for (int i = 0; i < _iCUEDevList[a].positions->numberOfLed; i++)
			{
				_iCUEDevList[a].led_idx[i] = _iCUEDevList[a].positions->pLedPosition[i].ledId;

				if (strcmp(_iCUEDevList[a].model, "STRAFE RGB MK.2") == 0)
				{
					// I don't know if the incorrect positioning only applies to the STRAFE RGB MK.2 so I added that workaround
					_iCUEDevList[a].x_idx[i] = (int)(SPECTROGRAPH_END * (_iCUEDevList[a].positions->pLedPosition[i].left / _width)) - 10;
				}
				else
				{
					_iCUEDevList[a].x_idx[i] = (int)(SPECTROGRAPH_END * (_iCUEDevList[a].positions->pLedPosition[i].left / _width));
				}

				_iCUEDevList[a].y_idx[i] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (SPECTROGRAPH_ROWS * (_iCUEDevList[a].positions->pLedPosition[i].top / _height)) + (1.0f * (SPECTROGRAPH_ROWS / _height)));
			}

			for (int i = 0; i < _iCUEDevList[a].positions->numberOfLed; i++)
			{
				COLORREF _color = pixels[_iCUEDevList[a].y_idx[i]][_iCUEDevList[a].x_idx[i]];
				_iCUEDevList[a].colors[i].r = GetRValue(_color);
				_iCUEDevList[a].colors[i].g = GetGValue(_color);
				_iCUEDevList[a].colors[i].b = GetBValue(_color);
				_iCUEDevList[a].colors[i].ledId = _iCUEDevList[a].led_idx[i];
			}

			CorsairSetLedsColorsBufferByDeviceIndex(_iCUEDevList[a].devID, _iCUEDevList[a].positions->numberOfLed, _iCUEDevList[a].colors);
			CorsairSetLedsColorsFlushBuffer();
		}

		return TRUE;
#endif
	}
}