#include <iostream>
#include <string>

#ifdef __APPLE__
#include <CUESDK/CUESDK.h>
#else
#include <CUESDK.h>
#endif

const char *toString(CorsairError error)
{
	switch (error) {
	case CE_Success:
		return "CE_Success";
	case CE_ServerNotFound:
		return "CE_ServerNotFound";
	case CE_NoControl:
		return "CE_NoControl";
	case CE_ProtocolHandshakeMissing:
		return "CE_ProtocolHandshakeMissing";
	case CE_IncompatibleProtocol:
		return "CE_IncompatibleProtocol";
	case CE_InvalidArguments:
		return "CE_InvalidArguments";
	default:
		return "unknown error";
	}
}

bool errorCheck(const std::string &msg) {
	auto error = CorsairGetLastError();
	if (error != CorsairError::CE_Success) {
		std::cerr << msg << " (Error: " << toString(error) << ')' << std::endl;
		return true;
	}

	return false;
}

void printProperties(int deviceIndex)
{
	bool val = false;
	if (CorsairGetBoolPropertyValue(deviceIndex, CorsairDevicePropertyId::CDPI_Headset_MicEnabled, &val)) {
		std::cout << "Headset mic enabled val = " << val << std::endl;
	} else {
		errorCheck("CorsairGetBoolPropertyValue CDPI_Headset_MicEnabled error. ");
	}

	if (CorsairGetBoolPropertyValue(deviceIndex, CorsairDevicePropertyId::CDPI_Headset_SurroundSoundEnabled, &val)) {
		std::cout << "Headset surround enabled val = " << val << std::endl;
	} else {
		errorCheck("CorsairGetBoolPropertyValue CDPI_Headset_SurroundSoundEnabled error. ");
	}

	if (CorsairGetBoolPropertyValue(deviceIndex, CorsairDevicePropertyId::CDPI_Headset_SidetoneEnabled, &val)) {
		std::cout << "Headset sidetone enabled val = " << val << std::endl;
	} else {
		errorCheck("CorsairGetBoolPropertyValue CDPI_Headset_SidetoneEnabled error. ");
	}

	int intVal = 0;
	if (CorsairGetInt32PropertyValue(deviceIndex, CorsairDevicePropertyId::CDPI_Headset_EqualizerPreset, &intVal)) {
		std::cout << "Active headset equalize preset index = " << intVal << std::endl;
	} else {
		errorCheck("CorsairGetInt32PropertyValue CDPI_Headset_EqualizerPreset error. ");
	}

	std::cout << std::endl;
}

int main(int argc, char *argv[])
{
	CorsairPerformProtocolHandshake();
	if (errorCheck("Handshake error")) {
		getchar();
		return -1;
	}

	const auto devicesCount = CorsairGetDeviceCount();
	for (int i = 0; i < devicesCount; ++i) {
		const auto info = CorsairGetDeviceInfo(i);
		if (!info) {
			errorCheck("Get device info error");
			continue;
		}

		if (info->capsMask & CDC_PropertyLookup) {
			std::cout << "Device supports CDC_PropertyLookup, model: " << info->model << std::endl;
			printProperties(i);
		} else {
			std::cout << "Device doesn't support CDC_PropertyLookup, model: " << info->model << std::endl;
		}
	}

	getchar();
	return 0;
}
