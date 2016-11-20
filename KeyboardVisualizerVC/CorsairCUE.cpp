#include "CorsairCUE.h"

#define CORSAIR_CUE_ENABLED

boolean failed;

#ifdef CORSAIR_CUE_ENABLED
#include <algorithm>

#pragma comment(lib, "CUESDK_2013.lib")
#pragma comment(lib, "CUESDK.x64_2013.lib")

CorsairLedColor * colors;
CorsairLedPositions * positions;
int * x_idx;
int * y_idx;
CorsairLedId * led_idx;
#endif

CorsairCUE::CorsairCUE()
{
}


CorsairCUE::~CorsairCUE()
{
}

#ifdef CORSAIR_CUE_ENABLED
double getKeyboardHeight(CorsairLedPositions *ledPositions)
{
    const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
        [](const CorsairLedPosition &clp1, const CorsairLedPosition &clp2) {
        return clp1.top < clp2.top;
    });
    return minmaxLeds.second->top + minmaxLeds.second->height - minmaxLeds.first->top;
}

double getKeyboardWidth(CorsairLedPositions *ledPositions)
{
    const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
        [](const CorsairLedPosition &clp1, const CorsairLedPosition &clp2) {
        return clp1.left < clp2.left;
    });
    return minmaxLeds.second->left + minmaxLeds.second->width - minmaxLeds.first->left;
}
#endif

void CorsairCUE::Initialize()
{
#ifdef CORSAIR_CUE_ENABLED
    CorsairPerformProtocolHandshake();
    if (const auto error = CorsairGetLastError())
    {
        failed = TRUE;
    }
    else
    {
        failed = FALSE;

        positions = CorsairGetLedPositions();

        colors = new CorsairLedColor[positions->numberOfLed];
        x_idx = new int[positions->numberOfLed];
        y_idx = new int[positions->numberOfLed];
        led_idx = new CorsairLedId[positions->numberOfLed];

        double width = getKeyboardWidth(positions);
        double height = getKeyboardHeight(positions);

        for (int i = 0; i < positions->numberOfLed; i++)
        {
            led_idx[i] = positions->pLedPosition[i].ledId;
            x_idx[i] = 255.0f * (positions->pLedPosition[i].left / width);
            y_idx[i] = 4 + (60.0f * (positions->pLedPosition[i].top / height));
        }

        failed = FALSE;
    }
#else
    failed = TRUE;
#endif
}

bool CorsairCUE::SetLEDs(COLORREF pixels[64][256])
{
    if (failed)
    {
        return FALSE;
    }
    else
    {
#ifdef CORSAIR_CUE_ENABLED
        for (int i = 0; i < positions->numberOfLed; i++)
        {
            COLORREF color = pixels[y_idx[i]][x_idx[i]];
            colors[i].r = GetRValue(color);
            colors[i].g = GetGValue(color);
            colors[i].b = GetBValue(color);
            colors[i].ledId = led_idx[i];
        }

        CorsairSetLedsColors(positions->numberOfLed, colors);
        return TRUE;
#endif
    }
}