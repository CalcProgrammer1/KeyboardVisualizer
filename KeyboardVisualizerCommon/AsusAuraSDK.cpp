/*---------------------------------------------------------*\
|  Processing Code for ASUS AURA SDK Interface              |
|                                                           |
|  Avinash Raja (avinashraja98@gmail.com), 1/4/2018         |
|  Adam Honse (calcprogrammer1@gmail.com)                   |
\*---------------------------------------------------------*/

#include "AsusAuraSDK.h"

#include "VisualizerDefines.h"

DWORD mb_ledCount;
//DWORD dram_ledCount;

BYTE* mb_color;
//BYTE* dram_color;

MbLightControl* mb_LightCtrl;
//DramLightControl* dram_LightCtrl;

AsusAuraSDK::AsusAuraSDK()
{
}


AsusAuraSDK::~AsusAuraSDK()
{
    delete[] mb_color;
    //delete[] dram_color;

    delete[] mb_LightCtrl;
    //delete[] dram_LightCtrl;
}

void AsusAuraSDK::Initialize()
{
    hModule = LoadLibraryA(ASUSAURASDKDLL);

    if (hModule)
    {
        dllLoaded = TRUE;
        (FARPROC&)EnumerateMbController = GetProcAddress(hModule, "EnumerateMbController");
        (FARPROC&)SetMbMode = GetProcAddress(hModule, "SetMbMode");
        (FARPROC&)SetMbColor = GetProcAddress(hModule, "SetMbColor");
        (FARPROC&)GetMbColor = GetProcAddress(hModule, "GetMbColor");
        (FARPROC&)GetMbLedCount = GetProcAddress(hModule, "GetMbLedCount");

#if 0
        (FARPROC&)EnumerateDram = GetProcAddress(hModule, "EnumerateDram");
        (FARPROC&)SetDramMode = GetProcAddress(hModule, "GetDramMode");
        (FARPROC&)SetDramColor = GetProcAddress(hModule, "SetDramColor");
        (FARPROC&)GetDramColor = GetProcAddress(hModule, "GetDramColor");
        (FARPROC&)GetDramLedCount = GetProcAddress(hModule, "GetDramLedCount");
#endif

        DWORD mb_count = EnumerateMbController(NULL, 0);
        //DWORD dram_count = EnumerateDram(NULL, 0);

        mb_LightCtrl = new MbLightControl[mb_count];
        //dram_LightCtrl = new DramLightControl[dram_count];

        EnumerateMbController(mb_LightCtrl, mb_count);
        //EnumerateDram(dram_LightCtrl, dram_count);

        SetMbMode(mb_LightCtrl[0], 1);
        //SetDramMode(dram_LightCtrl[0], 1);

        mb_ledCount = GetMbLedCount(mb_LightCtrl[0]);
        //dram_ledCount = GetDramLedCount(dram_LightCtrl[0]);

        mb_color = new BYTE[mb_ledCount * 3];
        //dram_color = new BYTE[dram_ledCount * 3];

        memset(mb_color, 0, mb_ledCount * 3);
        //memset(dram_color, 0, dram_ledCount * 3);
    }
    else
    {
        dllLoaded = FALSE;
    }
}

bool AsusAuraSDK::SetLEDs(COLORREF pixels[64][256])
{
    if (dllLoaded)
    {	
        // Following logic is based on results from the Asus ROG Strix Z270I motherboard.
        for (size_t i = 0; i < mb_ledCount * 3; ++i)
        {
        if (i % 3 == 0)
            {
                mb_color[i] = GetRValue(pixels[0][(128 / mb_ledCount) *(mb_ledCount - (int)((i) / 3))]);
            }
            if (i % 3 == 1)
            {
                mb_color[i] = GetBValue(pixels[0][(128 / mb_ledCount)*(mb_ledCount - (int)((i-1) / 3))]);
            }
            if (i % 3 == 2)
            {
                mb_color[i] = GetGValue(pixels[0][(128 / mb_ledCount)*(mb_ledCount - (int)((i-2) / 3))]);
            }
        }

        SetMbColor(mb_LightCtrl[0], mb_color, mb_ledCount * 3);

#if 0
        for (size_t i = 0; i < dram_ledCount * 3; ++i)
        {
            if (i % 3 == 0)
            {
                mb_color[i] = GetRValue(pixels[0][(128 / dram_ledCount) *(dram_ledCount - (int)((i) / 3))]);
            }
            if (i % 3 == 1)
            {
                mb_color[i] = GetBValue(pixels[0][(128 / dram_ledCount)*(dram_ledCount - (int)((i - 1) / 3))]);
            }
            if (i % 3 == 2)
            {
                mb_color[i] = GetGValue(pixels[0][(128 / dram_ledCount)*(dram_ledCount - (int)((i - 2) / 3))]);
            }
        }

        SetDramColor(dram_LightCtrl[0], dram_color, dram_ledCount * 3);
#endif
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
