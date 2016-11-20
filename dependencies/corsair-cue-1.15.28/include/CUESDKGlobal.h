#pragma once

#ifndef _LIB
#ifdef CORSAIR_LIGHTING_SDK_DLL
#define CORSAIR_LIGHTING_SDK_EXPORT __declspec(dllexport)
#else
#define CORSAIR_LIGHTING_SDK_EXPORT __declspec(dllimport)
#endif
#else
#define CORSAIR_LIGHTING_SDK_EXPORT
#endif
