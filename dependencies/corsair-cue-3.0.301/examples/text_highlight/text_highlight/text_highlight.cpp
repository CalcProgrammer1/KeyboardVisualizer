#define CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS

#ifdef __APPLE__
#include <CUESDK/CUESDK.h>
#else
#include <CUESDK.h>
#endif

#include <iostream>
#include <atomic>
#include <thread>
#include <string>
#include <cmath>

const char* toString(CorsairError error) 
{
	switch (error) {
	case CE_Success :
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

void highlightKey(CorsairLedId ledId)
{
	for (auto x = .0; x < 2; x += .1) {
		auto val = static_cast<int>((1 - abs(x - 1)) * 255);
		auto ledColor = CorsairLedColor{ ledId, val, val, val };
		CorsairSetLedsColors(1, &ledColor);
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
}

int main()
{
	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << "\nPress any key to quit." << std::endl;
		getchar();
		return -1;
	}
	CorsairRequestControl(CAM_ExclusiveLightingControl);

	std::cout << "Please, input a word... \n";
	std::string userInputStr;
	std::cin >> userInputStr;

	for (const auto &symbol : userInputStr) {
		auto ledId = CorsairGetLedIdForKeyName(symbol);
		if (ledId != CLI_Invalid)
			highlightKey(ledId);
	}
	return 0;
}
