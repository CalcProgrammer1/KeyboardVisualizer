#ifdef __APPLE__
#include <CUESDK/CUESDK.h>
#else
#include <CUESDK.h>
#endif

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <unordered_map>
#include <cmath>

using LedColorsVector = std::vector<CorsairLedColor>;
using AvailableKeys = std::unordered_map<int /*device index*/, LedColorsVector>;

const char* toString(CorsairError error)
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

AvailableKeys getAvailableKeys()
{
	AvailableKeys availableKeys;
	for (auto deviceIndex = 0; deviceIndex < CorsairGetDeviceCount(); deviceIndex++) {
		if (const auto ledPositions = CorsairGetLedPositionsByDeviceIndex(deviceIndex)) {
			LedColorsVector keys;
			for (auto i = 0; i < ledPositions->numberOfLed; i++) {
				const auto ledId = ledPositions->pLedPosition[i].ledId;
				keys.push_back(CorsairLedColor{ ledId, 0, 0, 0 });
			}
			availableKeys[deviceIndex] = keys;
		}
	}
	return availableKeys;
}

void performPulseEffect(int waveDuration, AvailableKeys &availableKeys)
{
	const auto timePerFrame = 25;
	for (auto x = .0; x < 2; x += static_cast<double>(timePerFrame) / waveDuration) {
		auto val = static_cast<int>((1 - pow(x - 1, 2)) * 255);
		for (auto &ledColorsByDeviceIndex : availableKeys) {
			auto &deviceIndex = ledColorsByDeviceIndex.first;
			auto &ledColorsVec = ledColorsByDeviceIndex.second;
			for (auto &ledColor : ledColorsVec) {
				ledColor.g = val;
			}
			CorsairSetLedsColorsBufferByDeviceIndex(deviceIndex, static_cast<int>(ledColorsVec.size()), ledColorsVec.data());
		}
		CorsairSetLedsColorsFlushBufferAsync(nullptr, nullptr);
		std::this_thread::sleep_for(std::chrono::milliseconds(timePerFrame));
	}
}

int main()
{
	CorsairPerformProtocolHandshake();
	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << "\nPress any key tro quit." << std::endl;
		getchar();
		return -1;
	}

	std::atomic_int waveDuration{ 500 };
	std::atomic_bool continueExecution{ true };

	auto availableKeys = getAvailableKeys();
	if (availableKeys.empty()) {
		return 1;
	}

	std::cout << "Working... Use \"+\" or \"-\" to increase or decrease speed.\nPress \"q\" to close program..." << std::endl;

	std::thread lightingThread([&waveDuration, &continueExecution, &availableKeys] {
		while (continueExecution) {
			performPulseEffect(waveDuration, availableKeys);
		}
	});

	while (continueExecution) {
		char c = getchar();
		switch (c) {
		case '+':
			if (waveDuration > 100)
				waveDuration -= 100;
			break;
		case '-':
			if (waveDuration < 2000)
				waveDuration += 100;
			break;
		case 'Q':
		case 'q':
			continueExecution = false;
			break;
		default:
			break;
		}
	}
	lightingThread.join();
	return 0;
}
