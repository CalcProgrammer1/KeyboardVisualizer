/*---------------------------------------------------------*\
|  Definitions for ASUS AURA SDK Interface                  |
|                                                           |
|  Avinash Raja (avinashraja98@gmail.com), 1/4/2018	        |
|  Adam Honse (calcprogrammer1@gmail.com)	                |
\*---------------------------------------------------------*/

#include "AURALightingSDK.h"
#include "VisualizerDefines.h"

#define ASUSAURASDKDLL "AURA_SDK.dll"

class AsusAuraSDK
{
public:
    AsusAuraSDK();
    ~AsusAuraSDK();

    void Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);

private:
    
    HMODULE hModule = NULL;

    bool dllLoaded = FALSE;

    EnumerateMbControllerFunc EnumerateMbController;
    SetMbModeFunc SetMbMode;
    SetMbColorFunc SetMbColor;
    GetMbColorFunc GetMbColor;
    GetMbLedCountFunc GetMbLedCount;

    EnumerateDramFunc EnumerateDram;
    SetDramModeFunc SetDramMode;
    SetDramColorFunc SetDramColor;
    GetDramColorFunc GetDramColor;
    GetDramLedCountFunc GetDramLedCount;
};

