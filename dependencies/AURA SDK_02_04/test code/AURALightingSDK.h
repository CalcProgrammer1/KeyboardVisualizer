#pragma once

#include <Windows.h>


typedef void* MbLightControl;
typedef void* GPULightControl;
typedef void* ClaymoreKeyboardLightControl;
typedef void* RogMouseLightControl;


typedef DWORD(WINAPI* EnumerateMbControllerFunc)(MbLightControl handles[], DWORD size);
typedef DWORD(WINAPI* SetMbModeFunc) (MbLightControl handle, DWORD mode);
typedef DWORD(WINAPI* SetMbColorFunc) (MbLightControl handle, BYTE* color, DWORD size);
typedef DWORD(WINAPI* GetMbColorFunc) (MbLightControl handle, BYTE* color, DWORD size);
typedef DWORD(WINAPI* GetMbLedCountFunc)(MbLightControl handle);

typedef DWORD(WINAPI* EnumerateGPUFunc)(GPULightControl handles[], DWORD size);
typedef DWORD(WINAPI* SetGPUModeFunc) (GPULightControl handle, DWORD mode);
typedef DWORD(WINAPI* SetGPUColorFunc) (GPULightControl handle, BYTE* color, DWORD size);
typedef DWORD(WINAPI* GetGPULedCountFunc)(GPULightControl handle);

typedef DWORD(WINAPI* CreateClaymoreKeyboardFunc)(ClaymoreKeyboardLightControl* handle);
typedef DWORD(WINAPI* SetClaymoreKeyboardModeFunc) (ClaymoreKeyboardLightControl handle, DWORD mode);
typedef DWORD(WINAPI* SetClaymoreKeyboardColorFunc) (ClaymoreKeyboardLightControl handle, BYTE* color, DWORD size);
typedef DWORD(WINAPI* GetClaymoreKeyboardLedCountFunc)(ClaymoreKeyboardLightControl handle);

typedef DWORD(WINAPI* CreateRogMouseFunc)(RogMouseLightControl* handle);
typedef DWORD(WINAPI* SetRogMouseModeFunc) (RogMouseLightControl handle, DWORD mode);
typedef DWORD(WINAPI* SetRogMouseColorFunc) (RogMouseLightControl handle, BYTE* color, DWORD size);
typedef DWORD(WINAPI* RogMouseLedCountFunc)(RogMouseLightControl handle);
