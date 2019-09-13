#ifdef __APPLE__
#include <CUESDK/CUESDK.h>
#else
#include <CUESDK.h>
#endif

#include <string>
#include <unordered_map>
#include <iostream>

const std::unordered_map<CorsairKeyId, std::string> keyIdStrings = {
	{CorsairKey_Invalid, "CorsairKey_Invalid"},
	{CorsairKeyKb_G1, "CorsairKeyKb_G1"},
	{CorsairKeyKb_G2, "CorsairKeyKb_G2"},
	{CorsairKeyKb_G3, "CorsairKeyKb_G3"},
	{CorsairKeyKb_G4, "CorsairKeyKb_G4"},
	{CorsairKeyKb_G5, "CorsairKeyKb_G5"},
	{CorsairKeyKb_G6, "CorsairKeyKb_G6"},
	{CorsairKeyKb_G7, "CorsairKeyKb_G7"},
	{CorsairKeyKb_G8, "CorsairKeyKb_G8"},
	{CorsairKeyKb_G9, "CorsairKeyKb_G9"},
	{CorsairKeyKb_G10, "CorsairKeyKb_G10"},
	{CorsairKeyKb_G11, "CorsairKeyKb_G11"},
	{CorsairKeyKb_G12, "CorsairKeyKb_G12"},
	{CorsairKeyKb_G13, "CorsairKeyKb_G13"},
	{CorsairKeyKb_G14, "CorsairKeyKb_G14"},
	{CorsairKeyKb_G15, "CorsairKeyKb_G15"},
	{CorsairKeyKb_G16, "CorsairKeyKb_G16"},
	{CorsairKeyKb_G17, "CorsairKeyKb_G17"},
	{CorsairKeyKb_G18, "CorsairKeyKb_G18"},

	{CorsairKeyMouse_M1, "CorsairKeyMouse_M1"},
	{CorsairKeyMouse_M2, "CorsairKeyMouse_M2"},
	{CorsairKeyMouse_M3, "CorsairKeyMouse_M3"},
	{CorsairKeyMouse_M4, "CorsairKeyMouse_M4"},
	{CorsairKeyMouse_M5, "CorsairKeyMouse_M5"},
	{CorsairKeyMouse_M6, "CorsairKeyMouse_M6"},
	{CorsairKeyMouse_M7, "CorsairKeyMouse_M7"},
	{CorsairKeyMouse_M8, "CorsairKeyMouse_M8"},
	{CorsairKeyMouse_M9, "CorsairKeyMouse_M9"},
	{CorsairKeyMouse_M10, "CorsairKeyMouse_M10"},
	{CorsairKeyMouse_M11, "CorsairKeyMouse_M11"},
	{CorsairKeyMouse_M12, "CorsairKeyMouse_M12"}
};

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

class EventPrinter
{
public:
	void print(const CorsairEvent *e)
	{
		std::cout << "Event #" << std::to_string(++m_eventCounter) << std::endl;
		if (e->id == CEI_DeviceConnectionStatusChangedEvent) {
			printConnectionEvent(e->deviceConnectionStatusChangedEvent);
		} else if (e->id == CEI_KeyEvent) {
			printKeyEvent(e->keyEvent);
		} else {
			std::cout << "Invalid event!" << std::endl;
		}
	}

private:
	void printConnectionEvent(const CorsairDeviceConnectionStatusChangedEvent *e) const
	{
		std::cout << "Device id: " << e->deviceId 
		          << "   Status: " << (e->isConnected ? "connected" : "disconnected") << std::endl;

	}

	void printKeyEvent(const CorsairKeyEvent *e) const
	{
		std::cout << " Device id: " << e->deviceId
		          << "    Key id: " << keyIdStrings.at(e->keyId)
		          << " Key state: " << (e->isPressed ? "pressed" : "released") << std::endl;
	}

private:
	int m_eventCounter = 0;
};

int main(int argc, char *argv[])
{
	CorsairPerformProtocolHandshake();
	if (errorCheck("Handshake error")) {
		getchar();
		return -1;
	}

	const auto callback = [](void *context, const CorsairEvent *e) {
		EventPrinter *eventPrinter = static_cast<EventPrinter*>(context);
		eventPrinter->print(e);
	};

	//Context could be any class instance or any pointer. Client must ensure context is valid during callback execution
	EventPrinter context;
	CorsairSubscribeForEvents(callback, &context);
	if (errorCheck("Subscribe for events error")) {
		getchar();
		return -1;
	}

	std::cout << "Working... Press any G/M key or connect/disconnect Corsair device to see events in action" << std::endl;
	std::cout << "Press \"q\" to close program..." << std::endl;

	while (true) {
		char c = getchar();
		if (c == 'q' || c == 'Q') {
			break;
		}
	}

	CorsairUnsubscribeFromEvents();
	return 0;
}