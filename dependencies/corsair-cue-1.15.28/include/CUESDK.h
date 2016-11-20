#pragma once

#include "CUESDKGlobal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "CorsairLedIdEnum.h"

	enum CorsairDeviceType		// contains list of available device types
	{
		CDT_Unknown = 0,
		CDT_Mouse = 1,
		CDT_Keyboard = 2,
		CDT_Headset = 3
	};

	enum CorsairPhysicalLayout	// contains list of available physical layouts for keyboards
	{
		CPL_Invalid = 0,		// dummy value

		CPL_US = 1,
		CPL_UK = 2,
		CPL_BR = 3,
		CPL_JP = 4,
		CPL_KR = 5,				// valid values for keyboard

		CPL_Zones1 = 6,
		CPL_Zones2 = 7,
		CPL_Zones3 = 8,
		CPL_Zones4 = 9,			// valid values for mouse 
	};

	enum CorsairLogicalLayout	// contains list of available logical layouts for keyboards
	{
		CLL_Invalid = 0,		// dummy value
		CLL_US_Int = 1,
		CLL_NA = 2,
		CLL_EU = 3,
		CLL_UK = 4,
		CLL_BE = 5,
		CLL_BR = 6,
		CLL_CH = 7,
		CLL_CN = 8,
		CLL_DE = 9,
		CLL_ES = 10,
		CLL_FR = 11,
		CLL_IT = 12,
		CLL_ND = 13,
		CLL_RU = 14,
		CLL_JP = 15,
		CLL_KR = 16,
		CLL_TW = 17,
		CLL_MEX = 18
	};

	enum CorsairDeviceCaps		// contains list of device capabilities
	{
		CDC_None = 0,				// for devices that do not support any SDK functions
		CDC_Lighting = 1				// for devices that has controlled lighting
	};

	enum CorsairAccessMode		// contains list of available SDK access modes
	{
		CAM_ExclusiveLightingControl = 0
	};

	enum CorsairError			// contains shared list of all errors which could happen during calling of Corsair* functions
	{
		CE_Success,						// if previously called function completed successfully
		CE_ServerNotFound,				// CUE is not running or was shut down or third-party control is disabled in CUE settings(runtime error)
		CE_NoControl,					// if some other client has or took over exclusive control (runtime error)
		CE_ProtocolHandshakeMissing,	// if developer did not perform protocol handshake(developer error)
		CE_IncompatibleProtocol,		// if developer is calling the function that is not supported by the server(either because protocol has broken by server or client or because the function is new and server is too old. Check CorsairProtocolDetails for details) (developer error)
		CE_InvalidArguments,			// if developer supplied invalid arguments to the function(for specifics look at function descriptions). (developer error)
	};

	struct CorsairDeviceInfo	// contains information about device
	{
		CorsairDeviceType type;			// enum describing device type
		const char* model;					// null - terminated device model(like “K95RGB”)
		CorsairPhysicalLayout physicalLayout; // enum describing physical layout of the keyboard or mouse
		CorsairLogicalLayout logicalLayout;  // enum describing logical layout of the keyboard as set in CUE settings
		int capsMask;					// mask that describes device capabilities, formed as logical “or” of CorsairDeviceCaps enum values
	};

	struct CorsairLedPosition	// contains led id and position of led rectangle.Most of the keys are rectangular.In case if key is not rectangular(like Enter in ISO / UK layout) it returns the smallest rectangle that fully contains the key
	{
		CorsairLedId ledId;				// identifier of led
		double top;
		double left;
		double height;
		double width;					// values in mm
	};

	struct CorsairLedPositions	// contains number of leds and arrays with their positions
	{
		int numberOfLed;				// integer value.Number of elements in following array
		CorsairLedPosition* pLedPosition;  // array of led positions
	};

	struct CorsairLedColor		//  contains information about led and its color
	{
		CorsairLedId ledId;				// identifier of LED to set
		int r;							// red   brightness[0..255]
		int g;							// green brightness[0..255]
		int b;							// blue  brightness[0..255]
	};

	struct CorsairProtocolDetails // contains information about SDK and CUE versions
	{
		const char* sdkVersion;			// null - terminated string containing version of SDK(like “1.0.0.1”). Always contains valid value even if there was no CUE found
		const char* serverVersion;		// null - terminated string containing version of CUE(like “1.0.0.1”) or NULL if CUE was not found.
		int sdkProtocolVersion;			// integer number that specifies version of protocol that is implemented by current SDK. Numbering starts from 1. Always contains valid value even if there was no CUE found
		int serverProtocolVersion;		// integer number that specifies version of protocol that is implemented by CUE. Numbering starts from 1. If CUE was not found then this value will be 0
		bool breakingChanges;			// boolean value that specifies if there were breaking changes between version of protocol implemented by server and client
	};



	// set specified leds to some colors.The color is retained until changed by successive calls.This function does not take logical layout into account
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSetLedsColors(int size, CorsairLedColor* ledsColors);

	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSetLedsColorsAsync(int size, CorsairLedColor* ledsColors, void(*CallbackType)(void*, bool, CorsairError), void *context);

	// returns number of connected Corsair devices that support lighting control.
	CORSAIR_LIGHTING_SDK_EXPORT int CorsairGetDeviceCount();

	// returns information about device at provided index
	CORSAIR_LIGHTING_SDK_EXPORT CorsairDeviceInfo *CorsairGetDeviceInfo(int deviceIndex);

	// provides list of keyboard LEDs with their physical positions.
	CORSAIR_LIGHTING_SDK_EXPORT CorsairLedPositions *CorsairGetLedPositions();

	// retrieves led id for key name taking logical layout into account.
	CORSAIR_LIGHTING_SDK_EXPORT CorsairLedId CorsairGetLedIdForKeyName(char keyName);

	//  requestes control using specified access mode. By default client has shared control over lighting so there is no need to call CorsairRequestControl unless client requires exclusive control
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairRequestControl(CorsairAccessMode accessMode);

	// checks file and protocol version of CUE to understand which of SDK functions can be used with this version of CUE
	CORSAIR_LIGHTING_SDK_EXPORT CorsairProtocolDetails CorsairPerformProtocolHandshake();

	// returns last error that occured while using any of Corsair* functions
	CORSAIR_LIGHTING_SDK_EXPORT CorsairError CorsairGetLastError();

	//releases previously requested control for specified access mode
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairReleaseControl(CorsairAccessMode accessMode);

#ifdef __cplusplus
} //exten "C"
#endif
