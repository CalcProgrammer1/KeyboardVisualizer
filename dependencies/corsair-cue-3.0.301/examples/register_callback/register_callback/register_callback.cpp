#ifdef __APPLE__
#include <CUESDK/CUESDK.h>
#else
#include <CUESDK.h>
#endif

#include <cstdio>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

struct Context
{
	CorsairKeyId m_lastPressedKey;
};

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

const std::unordered_map<CorsairKeyId, CorsairLedId> keyIdToLedId = {
	{CorsairKey_Invalid, CLI_Invalid},

	{CorsairKeyKb_G1, CLK_G1},
	{CorsairKeyKb_G2, CLK_G2},
	{CorsairKeyKb_G3, CLK_G3},
	{CorsairKeyKb_G4, CLK_G4},
	{CorsairKeyKb_G5, CLK_G5},
	{CorsairKeyKb_G6, CLK_G6},
	{CorsairKeyKb_G7, CLK_G7},
	{CorsairKeyKb_G8, CLK_G8},
	{CorsairKeyKb_G9, CLK_G9},
	{CorsairKeyKb_G10, CLK_G10},
	{CorsairKeyKb_G11, CLK_G11},
	{CorsairKeyKb_G12, CLK_G12},
	{CorsairKeyKb_G13, CLK_G13},
	{CorsairKeyKb_G14, CLK_G14},
	{CorsairKeyKb_G15, CLK_G15},
	{CorsairKeyKb_G16, CLK_G16},
	{CorsairKeyKb_G17, CLK_G17},
	{CorsairKeyKb_G18, CLK_G18},

	{CorsairKeyMouse_M1, CLM_1},
	{CorsairKeyMouse_M2, CLM_2},
	{CorsairKeyMouse_M3, CLM_3},
	{CorsairKeyMouse_M4, CLM_4},
	{CorsairKeyMouse_M5, CLM_5},
	{CorsairKeyMouse_M6, CLM_6},
	{CorsairKeyMouse_M7, CLK_A},
	{CorsairKeyMouse_M8, CLK_B},
	{CorsairKeyMouse_M9, CLK_C},
	{CorsairKeyMouse_M10, CLK_D},
	{CorsairKeyMouse_M11, CLK_E},
	{CorsairKeyMouse_M12, CLK_F}
};

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

int main()
{
	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << "\nPress any key to quit." << std::endl;
		getchar();
		return -1;
	}

	std::cout << "Working... Press \"q\" to close program...\n";

	auto callback = [] (void *context, CorsairKeyId keyId, bool pressed) {
		auto ledId = keyIdToLedId.at(keyId);
		CorsairLedColor ledColor = {ledId, 0, 0};
		CorsairGetLedsColors(1, &ledColor);
		Context *ctx = static_cast<Context*>(context);
		ctx->m_lastPressedKey = keyId;
		std::cout << "Key " << (pressed ? "pressed: " : "released: ") << keyIdStrings.at(keyId) << ", color: RGB(" << ledColor.r << ',' << ledColor.g << ',' << ledColor.b << ')' << std::endl;
	};

	//Context could be any class instance or any pointer. Client must ensure context is valid during callback execution
	Context context;
	CorsairRegisterKeypressCallback(callback, &context);

	while (true) {
		char c = getchar();
		if (c == 'q' || c == 'Q') {
			break;
		}
	}

	return 0;
}
