#define CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS

#ifdef __APPLE__
#include <CUESDK/CUESDK.h>
#else
#include <CUESDK.h>
#endif

#include <iostream>
#include <algorithm>
#include <atomic>
#include <thread>
#include <vector>
#include <string>

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

double getKeyboardWidth(CorsairLedPositions *ledPositions)
{
	const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
												[](const CorsairLedPosition &clp1, const CorsairLedPosition &clp2) {
		return clp1.left < clp2.left;
	});
	return minmaxLeds.second->left + minmaxLeds.second->width - minmaxLeds.first->left;
}

void chooseLayerPriority()
{
	std::string layer;
	std::cout << "Change your layer's priority 0-255 (default 128):";
	std::cin >> layer;

	int priority = 128;
	try {
		priority = std::stoi(layer);
	}
	catch (std::exception &e) {
		std::cout << "Layer priority hasn't changed: " << e.what() << std::endl;
		return;
	}
	CorsairSetLayerPriority(priority);
}

int main()
{
	CorsairPerformProtocolHandshake();
	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << "\nPress any key to quit." << std::endl;
		getchar();
		return -1;
	}

	const auto ledPositions = CorsairGetLedPositions();
	if (!ledPositions || ledPositions->numberOfLed < 0) {
		return 1;
	}

	std::cout << "Working... Press \"q\" to close program...\n";
	std::cout << "Press \"p\" to change layer priority\n";

	chooseLayerPriority();

	std::atomic_bool continueExecution{true};
	std::thread lightingThread([&continueExecution, &ledPositions] {
		const auto keyboardWidth = getKeyboardWidth(ledPositions);
		const auto numberOfSteps = 50;
		int n = 0;
		while (continueExecution) {
			std::vector<CorsairLedColor> vec;
			const auto currWidth = double(keyboardWidth) * (n % (numberOfSteps + 1)) / numberOfSteps;

			for (auto i = 0; i < ledPositions->numberOfLed; i++) {
				const auto ledPos = ledPositions->pLedPosition[i];
				auto ledColor = CorsairLedColor();
				ledColor.ledId = ledPos.ledId;
				if (ledPos.left < currWidth) {
					ledColor.r = 255;
					ledColor.g = 0;
					ledColor.b = 0;
				}
				vec.push_back(ledColor);
			}
			CorsairSetLedsColors(static_cast<int>(vec.size()), vec.data());

			std::this_thread::sleep_for(std::chrono::milliseconds(25));
			++n;
		}
	});

	while(continueExecution) {
		char c = getchar();
		switch (c) {
		case 'Q':
		case 'q':
			continueExecution = false;
			break;
		case 'P' :
		case 'p':
			chooseLayerPriority();
			break;
		default:
			break;
		}
	}

	lightingThread.join();

	return 0;
}
