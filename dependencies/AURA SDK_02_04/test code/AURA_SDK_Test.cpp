#include <stdio.h>
#include <tchar.h>
#include <vector>
#include "AURALightingSDK.h"

EnumerateMbControllerFunc EnumerateMbController;
SetMbModeFunc SetMbMode;
SetMbColorFunc SetMbColor;
GetMbColorFunc GetMbColor;
GetMbLedCountFunc GetMbLedCount;

EnumerateGPUFunc EnumerateGPU;
SetGPUModeFunc SetGPUMode;
SetGPUColorFunc SetGPUColor;
GetGPULedCountFunc GetGPULedCount;

CreateClaymoreKeyboardFunc CreateClaymoreKeyboard;
SetClaymoreKeyboardModeFunc SetClaymoreKeyboardMode;
SetClaymoreKeyboardColorFunc SetClaymoreKeyboardColor;
GetClaymoreKeyboardLedCountFunc GetClaymoreKeyboardLedCount;

CreateRogMouseFunc CreateRogMouse;
SetRogMouseModeFunc SetRogMouseMode;
SetRogMouseColorFunc SetRogMouseColor;
RogMouseLedCountFunc RogMouseLedCount;

void SDK_DEMO(){


	HMODULE hLib = nullptr;

	hLib = LoadLibraryA("AURA_SDK.dll");

	if (hLib == nullptr)
		return;

	(FARPROC&)EnumerateMbController = GetProcAddress(hLib, "EnumerateMbController");
	(FARPROC&)SetMbMode = GetProcAddress(hLib, "SetMbMode");
	(FARPROC&)SetMbColor = GetProcAddress(hLib, "SetMbColor");
	(FARPROC&)GetMbColor = GetProcAddress(hLib, "GetMbColor");
	(FARPROC&)GetMbLedCount = GetProcAddress(hLib, "GetMbLedCount");

	(FARPROC&)EnumerateGPU = GetProcAddress(hLib, "EnumerateGPU");
	(FARPROC&)SetGPUMode = GetProcAddress(hLib, "SetGPUMode");
	(FARPROC&)SetGPUColor = GetProcAddress(hLib, "SetGPUColor");
	(FARPROC&)GetGPULedCount = GetProcAddress(hLib, "GetGPULedCount");

	(FARPROC&)CreateClaymoreKeyboard = GetProcAddress(hLib, "CreateClaymoreKeyboard");
	(FARPROC&)SetClaymoreKeyboardMode = GetProcAddress(hLib, "SetClaymoreKeyboardMode");
	(FARPROC&)SetClaymoreKeyboardColor = GetProcAddress(hLib, "SetClaymoreKeyboardColor");
	(FARPROC&)GetClaymoreKeyboardLedCount = GetProcAddress(hLib, "GetClaymoreKeyboardLedCount");
	(FARPROC&)EnumerateMbController = GetProcAddress(hLib, "EnumerateMbController");

	(FARPROC&)CreateRogMouse = GetProcAddress(hLib, "CreateRogMouse");
	(FARPROC&)SetRogMouseMode = GetProcAddress(hLib, "SetRogMouseMode");
	(FARPROC&)SetRogMouseColor = GetProcAddress(hLib, "SetRogMouseColor");
	(FARPROC&)RogMouseLedCount = GetProcAddress(hLib, "RogMouseLedCount");


	int loop_start_count = GetTickCount();

	printf(" 0: mb demo.\n 1: vga demo.\n 2: kb demo.\n 3: mouse demo.\n");
	fflush(stdin);
	int input_index = getchar();
	int effect_index = -1;

	if (input_index == '0'){

		// MB
		printf("Ready to demonstrate the control to the motherboard.\n");

		DWORD _count = EnumerateMbController(NULL, 0);

		MbLightControl* _mbLightCtrl = new MbLightControl[_count];
		EnumerateMbController(_mbLightCtrl, _count);

		SetMbMode(_mbLightCtrl[0], 1);

		DWORD _ledCount = GetMbLedCount(_mbLightCtrl[0]);

		BYTE* _color = new BYTE[_ledCount * 3];
		ZeroMemory(_color, _ledCount * 3);

		for (int j = 0; j < 3; j++){

			for (int t = 0; t < 125; t++){

				for (size_t i = 0; i < _ledCount * 3; ++i)
				{
					if (i % 3 == 0 && j == 0){
						_color[i] = t * 2;
					}
					if (i % 3 == 1 && j == 1){
						_color[i] = t * 2;
					}
					if (i % 3 == 2 && j == 2){
						_color[i] = t * 2;
					}
				}

				SetMbColor(_mbLightCtrl[0], _color, _ledCount * 3);
				Sleep(41);
			}
		}

		for (size_t i = 0; i < _ledCount * 3; ++i)
		{
			_color[i] = 0x00;
		}

		GetMbColor(_mbLightCtrl[0], _color, _ledCount * 3);

		delete[] _color;
		delete[] _mbLightCtrl;

	}


	if (input_index == '1'){
		printf("Ready to demonstrate the control to the VGA card.\n");
		getchar();

		DWORD count = EnumerateGPU(NULL, 0);
		if (count > 0)
		{
			GPULightControl* GpuLightCtrl = new GPULightControl[count];

			EnumerateGPU(GpuLightCtrl, count);
			DWORD t = GetGPULedCount(GpuLightCtrl[0]);
			SetGPUMode(GpuLightCtrl[0], 1);
			BYTE *color = new BYTE[t * 3];
			ZeroMemory(color, t * 3);
			for (size_t i = 0; i < t * 3; ++i)
			{
				if (i % 3 == 1)
					color[i] = 0xFF;
				else if (i % 3 == 2)
					color[i] = 0x7F;
				else
					color[i] = 0x00;
			}

			int _loop = 150;
			while (0 < _loop)
			{
				for (size_t i = 0; i < t * 3; i++)
				{
					if (color[i] >= 0xFF)
						color[i] = 0x0;
					else
						color[i] += 0x3;
				}
				SetGPUColor(GpuLightCtrl[0], color, t * 3);

				Sleep(40);

				--_loop;
			}

		}
	}

	if (input_index == '2'){
		// Keyboard
		printf("Ready to demonstrate the control to the keyboard.\n");
		getchar();

		ClaymoreKeyboardLightControl* KeyboardLightCtrl = new ClaymoreKeyboardLightControl;
		DWORD Create = CreateClaymoreKeyboard(KeyboardLightCtrl);

		if (Create > 0)
		{
			DWORD ledCount = GetClaymoreKeyboardLedCount(*KeyboardLightCtrl);
			SetClaymoreKeyboardMode(*KeyboardLightCtrl, 1); // sw mode.

			BYTE *color = new BYTE[ledCount * 3];
			ZeroMemory(color, ledCount * 3);
			for (size_t i = 0; i < ledCount * 3; ++i)
			{
				if (i % 3 == 1)
					color[i] = 0xFF;
				else if (i % 3 == 2)
					color[i] = 0x7F;
				else
					color[i] = 0x00;
			}

			int _loop = 150;
			while (0 < _loop)
			{

				for (size_t i = 0; i < ledCount * 3; i++)
				{
					if (color[i] >= 0xFF)
						color[i] = 0x0;
					else
						color[i] += 0x3;
				}
				SetClaymoreKeyboardColor(*KeyboardLightCtrl, color, ledCount * 3);

				Sleep(40);

				--_loop;
			}

		}
		else
		{
			delete KeyboardLightCtrl;
		}

		SetClaymoreKeyboardMode(*KeyboardLightCtrl, 0); //end demo, back to default mode.
	}

	if (input_index == '3'){
		// Mouse
		printf("Ready to demonstrate the control to the mouse.\n");


		RogMouseLightControl* MouseLightCtrl = new RogMouseLightControl;
		DWORD Create = CreateRogMouse(MouseLightCtrl);

		if (Create > 0)
		{
			DWORD ledCount = RogMouseLedCount(*MouseLightCtrl);
			SetRogMouseMode(*MouseLightCtrl, 1); // sw mode.

			BYTE *color = new BYTE[ledCount * 3];
			ZeroMemory(color, ledCount * 3);
			for (size_t i = 0; i < ledCount * 3; ++i)
			{
				if (i % 3 == 1)
					color[i] = 0xFF;
				else if (i % 3 == 2)
					color[i] = 0x7F;
				else
					color[i] = 0x00;
			}

			int _loop = 150;
			while (0 < _loop)
			{
				for (size_t i = 0; i < ledCount * 3; i++)
				{
					if (color[i] >= 0xFF)
						color[i] = 0x0;
					else
						color[i] += 0x3;
				}
				SetRogMouseColor(*MouseLightCtrl, color, ledCount * 3);

				Sleep(40);

				--_loop;
			}

		}
		else
		{
			delete MouseLightCtrl;
		}

		SetRogMouseMode(*MouseLightCtrl, 0); //end demo, back to default mode.
	}

	FreeLibrary(hLib);

	return;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//==============================

	while (1){
		SDK_DEMO();
		Sleep(1000);
	}
	//==============================


	return 0;
}
