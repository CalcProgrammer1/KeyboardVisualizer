#pragma once

#ifndef _LIB
#   if defined(_WIN32) || defined(WIN32)
#       if defined(CORSAIR_LIGHTING_SDK_DLL) || defined(CUESDK_EXPORTS)
#           define CORSAIR_LIGHTING_SDK_EXPORT __declspec(dllexport)
#       else
#           define CORSAIR_LIGHTING_SDK_EXPORT __declspec(dllimport)
#       endif
#   else
#       define CORSAIR_LIGHTING_SDK_EXPORT __attribute__((visibility("default")))
#   endif // WIN32
#else
#   define CORSAIR_LIGHTING_SDK_EXPORT
#endif // !_LIB
