#pragma once

#include "CUESDKGlobal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "CorsairLedIdEnum.h"
#include "CorsairKeyIdEnum.h"

#if defined(__GNUC__) || defined(__clang__)
#	define CORSAIR_LIGHTING_SDK_DEPRECATED(message) __attribute__((deprecated(message)))
#elif defined(_MSC_VER)
#	define CORSAIR_LIGHTING_SDK_DEPRECATED(message) __declspec(deprecated(message))
#else
#	pragma message("WARNING: compiler is not supported")
#	define CORSAIR_LIGHTING_SDK_DEPRECATED(message)
#endif

#define CORSAIR_LIGHTING_SDK_DEPRECATED_SETLEDSCOLORS_WITH_DIY(CUESDK_buffer_func, CUESDK_send_func) \
	CORSAIR_LIGHTING_SDK_DEPRECATED("It is not recommended to use this function with DIY-devices, coolers and memory modules."\
	" Consider using "#CUESDK_buffer_func" to fill buffer and "#CUESDK_send_func" to send data to CUE instead."\
	" To disable deprecation, use CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS")

#define CORSAIR_LIGHTING_SDK_DEPRECATED_CORSAIRREGISTERKEYPRESSCALLBACK(CUESDK_sub_event_func, CUESDK_unsub_event_func) \
	CORSAIR_LIGHTING_SDK_DEPRECATED("It is not recommended to use this function to register keypress callback."\
	" Consider using "#CUESDK_sub_event_func" for subscribing to event and "#CUESDK_unsub_event_func" for unsubcribe from event."\
	" To disable deprecation, use CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS")

#if defined(CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS)
#	undef CORSAIR_LIGHTING_SDK_DEPRECATED
#	define CORSAIR_LIGHTING_SDK_DEPRECATED(message)
#	pragma message("WARNING: CUESDK deprecation warnings were disabled")
#endif //defined CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS

	enum CorsairDeviceType		// contains list of available device types.
	{
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
	};

	enum CorsairPhysicalLayout	// contains list of available physical layouts for keyboards.
	{
		CPL_Invalid = 0,		// dummy value.

		CPL_US = 1,
		CPL_UK = 2,
		CPL_BR = 3,
		CPL_JP = 4,
		CPL_KR = 5,				// valid values for keyboard.

		CPL_Zones1 = 6,
		CPL_Zones2 = 7,
		CPL_Zones3 = 8,
		CPL_Zones4 = 9			// valid values for mouse.
	};

	enum CorsairLogicalLayout	// contains list of available logical layouts for keyboards.
	{
		CLL_Invalid = 0,		// dummy value.
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

	enum CorsairDeviceCaps		// contains list of device capabilities.
	{
		CDC_None           = 0x0000, // for devices that do not support any SDK functions.
		CDC_Lighting       = 0x0001, // for devices that has controlled lighting.
		CDC_PropertyLookup = 0x0002	 // for devices that provide current state through set of properties.
	};

	enum CorsairAccessMode		// contains list of available SDK access modes.
	{
		CAM_ExclusiveLightingControl = 0
	};

	enum CorsairError			// contains shared list of all errors which could happen during calling of Corsair* functions.
	{
		CE_Success,						// if previously called function completed successfully.
		CE_ServerNotFound,				// CUE is not running or was shut down or third-party control is disabled in CUE settings(runtime error).
		CE_NoControl,					// if some other client has or took over exclusive control (runtime error).
		CE_ProtocolHandshakeMissing,	// if developer did not perform protocol handshake(developer error).
		CE_IncompatibleProtocol,		// if developer is calling the function that is not supported by the server(either because protocol has broken by server or client or because the function is new and server is too old. Check CorsairProtocolDetails for details) (developer error).
		CE_InvalidArguments,			// if developer supplied invalid arguments to the function(for specifics look at function descriptions). (developer error).
	};

	enum CorsairChannelDeviceType				// contains list of the LED-devices which can be connected to the DIY-device.
	{
		CCDT_Invalid = 0,	// dummy value.
		CCDT_HD_Fan = 1,
		CCDT_SP_Fan = 2,
		CCDT_LL_Fan = 3,
		CCDT_ML_Fan = 4,
		CCDT_Strip = 5,
		CCDT_DAP = 6,
		CCDT_Pump = 7
	};

	enum CorsairDevicePropertyType
	{
		CDPT_Boolean = 0x1000,
		CDPT_Int32   = 0x2000
	};

	enum CorsairDevicePropertyId
	{
		CDPI_Headset_MicEnabled           = 0x1000, // indicates Mic state (On or Off).
		CDPI_Headset_SurroundSoundEnabled = 0x1001, // indicates Surround Sound state (On or Off).
		CDPI_Headset_SidetoneEnabled      = 0x1002, // indicates Sidetone state (On or Off).
		CDPI_Headset_EqualizerPreset      = 0x2000  // the number of active equalizer preset (integer, 1 - 5).
	};

	enum CorsairEventId
	{
		CEI_Invalid, //dummy value
		CEI_DeviceConnectionStatusChangedEvent,
		CEI_KeyEvent
	};

	const unsigned int CORSAIR_DEVICE_ID_MAX = 128;

	typedef char CorsairDeviceId[CORSAIR_DEVICE_ID_MAX]; // defines a character array of length CORSAIR_DEVICE_ID_MAX to store device identifier string.

	struct CorsairChannelDeviceInfo		// contains information about separate LED-device connected to the channel controlled by the DIY-device.
	{
		CorsairChannelDeviceType type;	// type of the LED-device.
		int deviceLedCount;			// number of LEDs controlled by LED-device.
	};

	struct CorsairChannelInfo				// contains information about separate channel of the DIY-device.
	{
		int totalLedsCount;					// total number of LEDs connected to the channel.
		int devicesCount;					// number of LED-devices (fans, strips, etc.) connected to the channel which is controlled by the DIY device.
		CorsairChannelDeviceInfo* devices;	// array containing information about each separate LED-device connected to the channel controlled by the DIY device. Index of the LED-device in array is same as the index of the LED-device connected to the DIY-device.
	};

	struct CorsairChannelsInfo			// contains information about channels of the DIY-devices.
	{
		int channelsCount;				// number of channels controlled by the device.
		CorsairChannelInfo* channels;	// array containing information about each separate channel of the DIY-device. Index of the channel in the array is same as index of the channel on the DIY-device.
	};

	struct CorsairDeviceInfo	// contains information about device.
	{
		CorsairDeviceType type;               // enum describing device type.
		const char* model;                    // null - terminated device model(like “K95RGB”).
		CorsairPhysicalLayout physicalLayout; // enum describing physical layout of the keyboard or mouse.
		CorsairLogicalLayout logicalLayout;   // enum describing logical layout of the keyboard as set in CUE settings.
		int capsMask;                         // mask that describes device capabilities, formed as logical “or” of CorsairDeviceCaps enum values.
		int ledsCount;                        // number of controllable LEDs on the device.
		CorsairChannelsInfo channels;         // structure that describes channels of the DIY-devices.
		CorsairDeviceId deviceId;             // null-terminated string that contains unique device identifier that uniquely identifies device at least within session
	};

	struct CorsairLedPosition	// contains led id and position of led rectangle.Most of the keys are rectangular.In case if key is not rectangular(like Enter in ISO / UK layout) it returns the smallest rectangle that fully contains the key.
	{
		CorsairLedId ledId;				// identifier of led.
		double top;
		double left;
		double height;
		double width;					// values in mm.
	};

	struct CorsairLedPositions	// contains number of leds and arrays with their positions.
	{
		int numberOfLed;				// integer value.Number of elements in following array.
		CorsairLedPosition* pLedPosition; // array of led positions.
	};

	struct CorsairLedColor		// contains information about led and its color.
	{
		CorsairLedId ledId;				// identifier of LED to set.
		int r;							// red   brightness[0..255].
		int g;							// green brightness[0..255].
		int b;							// blue  brightness[0..255].
	};

	struct CorsairProtocolDetails // contains information about SDK and CUE versions.
	{
		const char* sdkVersion;			// null - terminated string containing version of SDK(like “1.0.0.1”). Always contains valid value even if there was no CUE found.
		const char* serverVersion;		// null - terminated string containing version of CUE(like “1.0.0.1”) or NULL if CUE was not found.
		int sdkProtocolVersion;			// integer number that specifies version of protocol that is implemented by current SDK. Numbering starts from 1. Always contains valid value even if there was no CUE found.
		int serverProtocolVersion;		// integer number that specifies version of protocol that is implemented by CUE. Numbering starts from 1. If CUE was not found then this value will be 0.
		bool breakingChanges;			// boolean value that specifies if there were breaking changes between version of protocol implemented by server and client.
	};

	struct CorsairDeviceConnectionStatusChangedEvent // contains information about some device that is connected or disconnected. When user receives this event, it makes sense to reenumerate device list, because device indices may become invalid at this moment.
	{
		CorsairDeviceId deviceId; // null-terminated string that contains unique device identifier.
		bool isConnected;         // true if connected, false if disconnected.
	};

	struct CorsairKeyEvent // contains information about device where G or M key was pressed/released and the key itself.
	{
		CorsairDeviceId deviceId; // null-terminated string that contains unique device identifier.
		CorsairKeyId keyId;       // G or M key that was pressed/released.
		bool isPressed;           // true if pressed, false if released.
	};

	struct CorsairEvent // contains information about event id and event data.
	{
		CorsairEventId id; // event identifier.
		union
		{
			const CorsairDeviceConnectionStatusChangedEvent *deviceConnectionStatusChangedEvent; // when id == CEI_DeviceConnectionStatusChangedEvent contains valid pointer to structure with information about connected or disconnected device.
			const CorsairKeyEvent *keyEvent;                                                     // when id == CEI_KeyEvent contains valid pointer to structure with information about pressed or released G or M button and device where this event happened.
		};
	};

	typedef void(*CorsairEventHandler)(void *context, const CorsairEvent *event); // defines a callback that will be called by SDK to notify user about events.

	// set specified leds to some colors. The color is retained until changed by successive calls. This function does not take logical layout into account.
	CORSAIR_LIGHTING_SDK_DEPRECATED_SETLEDSCOLORS_WITH_DIY(CorsairSetLedsColorsBufferByDeviceIndex, CorsairSetLedsColorsFlushBuffer)
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSetLedsColors(int size, CorsairLedColor* ledsColors);

	// set specified LEDs to some colors. This function set LEDs colors in the buffer which is written to the devices via CorsairSetLedsColorsFlushBuffer or CorsairSetLedsColorsFlushBufferAsync.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSetLedsColorsBufferByDeviceIndex(int deviceIndex, int size, CorsairLedColor* ledsColors);

	// writes to the devices LEDs colors buffer which is previously filled by the CorsairSetLedsColorsBufferByDeviceIndex function.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSetLedsColorsFlushBuffer();

	// same as CorsairSetLedsColorsFlushBuffer but returns control to the caller immediately.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSetLedsColorsFlushBufferAsync(void (*callback)(void *context, bool result, CorsairError error), void *context);

	// get current color for the list of requested LEDs.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairGetLedsColors(int size, CorsairLedColor* ledsColors);

	// get current color for the list of requested LEDs for specified device.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairGetLedsColorsByDeviceIndex(int deviceIndex, int size, CorsairLedColor* ledsColors);

	// same as CorsairSetLedsColors but returns control to the caller immediately.
	CORSAIR_LIGHTING_SDK_DEPRECATED_SETLEDSCOLORS_WITH_DIY(CorsairSetLedsColorsBufferByDeviceIndex, CorsairSetLedsColorsFlushBufferAsync)
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSetLedsColorsAsync(int size, CorsairLedColor* ledsColors, void(*CallbackType)(void*, bool, CorsairError), void *context);

	// returns number of connected Corsair devices that support lighting control.
	CORSAIR_LIGHTING_SDK_EXPORT int CorsairGetDeviceCount();

	// returns information about device at provided index.
	CORSAIR_LIGHTING_SDK_EXPORT CorsairDeviceInfo *CorsairGetDeviceInfo(int deviceIndex);

	// provides list of keyboard LEDs with their physical positions.
	CORSAIR_LIGHTING_SDK_EXPORT CorsairLedPositions *CorsairGetLedPositions();

	// provides list of keyboard, mouse, mousemat, headset, headset stand, DIY-devices, memory module and cooler LEDs by its index with their positions.
	CORSAIR_LIGHTING_SDK_EXPORT CorsairLedPositions *CorsairGetLedPositionsByDeviceIndex(int deviceIndex);

	// retrieves led id for key name taking logical layout into account.
	CORSAIR_LIGHTING_SDK_EXPORT CorsairLedId CorsairGetLedIdForKeyName(char keyName);

	// requestes control using specified access mode. By default client has shared control over lighting so there is no need to call CorsairRequestControl unless client requires exclusive control.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairRequestControl(CorsairAccessMode accessMode);

	// checks file and protocol version of CUE to understand which of SDK functions can be used with this version of CUE.
	CORSAIR_LIGHTING_SDK_EXPORT CorsairProtocolDetails CorsairPerformProtocolHandshake();

	// returns last error that occured while using any of Corsair* functions.
	CORSAIR_LIGHTING_SDK_EXPORT CorsairError CorsairGetLastError();

	// releases previously requested control for specified access mode.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairReleaseControl(CorsairAccessMode accessMode);

	// set layer priority for this shared client.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSetLayerPriority(int priority);

	// registers a callback that will be called by SDK when some of G or M keys are pressed or released.
	CORSAIR_LIGHTING_SDK_DEPRECATED_CORSAIRREGISTERKEYPRESSCALLBACK(CorsairSubscribeForEvents, CorsairUnsubscribeFromEvents)
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairRegisterKeypressCallback(void (*CallbackType)(void *context, CorsairKeyId keyId, bool pressed), void *context);

	// reads boolean property value for device at provided index.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairGetBoolPropertyValue(int deviceIndex, CorsairDevicePropertyId propertyId, bool *propertyValue);

	// reads integer property value for device at provided index.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairGetInt32PropertyValue(int deviceIndex, CorsairDevicePropertyId propertyId, int *propertyValue);

	// registers a callback that will be called by SDK when some event happened.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairSubscribeForEvents(CorsairEventHandler onEvent, void *context);

	// unregisters callback previously registered by CorsairSubscribeForEvents call.
	CORSAIR_LIGHTING_SDK_EXPORT bool CorsairUnsubscribeFromEvents();

#ifdef __cplusplus
} //exten "C"
#endif
