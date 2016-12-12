/*---------------------------------------------------------*\
|  Processing Code for MSI SteelSeries Keyboard Interface   |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#include "MSIKeyboard.h"

static bool init_ok = TRUE;

MSIKeyboard::MSIKeyboard()
{
}


MSIKeyboard::~MSIKeyboard()
{
}


#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")

//==================================================================================================
// Code by http://www.reddit.com/user/chrisgzy
//==================================================================================================
static bool IsMatchingDevice(wchar_t *pDeviceID, unsigned int uiVID, unsigned int uiPID, unsigned int uiMI)
{
    unsigned int uiLocalVID = 0, uiLocalPID = 0, uiLocalMI = 0;

    LPWSTR pszNextToken = 0;
    LPWSTR pszToken = wcstok(pDeviceID, L"\\#&");
    while (pszToken)
    {
        std::wstring tokenStr(pszToken);
        if (tokenStr.find(L"VID_", 0, 4) != std::wstring::npos)
        {
            std::wistringstream iss(tokenStr.substr(4));
            iss >> std::hex >> uiLocalVID;
        }
        else if (tokenStr.find(L"PID_", 0, 4) != std::wstring::npos)
        {
            std::wistringstream iss(tokenStr.substr(4));
            iss >> std::hex >> uiLocalPID;
        }
        else if (tokenStr.find(L"MI_", 0, 3) != std::wstring::npos)
        {
            std::wistringstream iss(tokenStr.substr(3));
            iss >> std::hex >> uiLocalMI;
        }

        pszToken = wcstok(0, L"\\#&");
    }

    if (uiVID != uiLocalVID || uiPID != uiLocalPID || uiMI != uiLocalMI)
        return false;

    return true;
}

//==================================================================================================
// Code by http://www.reddit.com/user/chrisgzy
//==================================================================================================
static HANDLE GetDeviceHandle(unsigned int uiVID, unsigned int uiPID, unsigned int uiMI)
{
    const GUID GUID_DEVINTERFACE_HID = { 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 };
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_HID, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
        return 0;

    HANDLE hReturn = 0;

    SP_DEVINFO_DATA deviceData = { 0 };
    deviceData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (unsigned int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &deviceData); ++i)
    {
        wchar_t wszDeviceID[MAX_DEVICE_ID_LEN];
        if (CM_Get_Device_IDW(deviceData.DevInst, wszDeviceID, MAX_DEVICE_ID_LEN, 0))
            continue;

        if (!IsMatchingDevice(wszDeviceID, uiVID, uiPID, uiMI))
            continue;

        SP_INTERFACE_DEVICE_DATA interfaceData = { 0 };
        interfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

        if (!SetupDiEnumDeviceInterfaces(hDevInfo, &deviceData, &GUID_DEVINTERFACE_HID, 0, &interfaceData))
            break;

        DWORD dwRequiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, 0, 0, &dwRequiredSize, 0);

        SP_INTERFACE_DEVICE_DETAIL_DATA *pData = (SP_INTERFACE_DEVICE_DETAIL_DATA *)new unsigned char[dwRequiredSize];
        pData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, pData, dwRequiredSize, 0, 0))
        {
            delete pData;
            break;
        }

        HANDLE hDevice = CreateFile(pData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
        if (hDevice == INVALID_HANDLE_VALUE)
        {
            delete pData;
            break;
        }

        hReturn = hDevice;
        break;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return hReturn;
}

void MSIKeyboard::send_usb_msg(char * data_pkt)
{
    char usb_pkt[8];
    for (int i = 1; i < 9; i++)
    {
        usb_pkt[i] = data_pkt[i - 1];
    }
    HidD_SetFeature(dev, data_pkt, 8);
}

int MSIKeyboard::Initialize()
{
    //Look for MSI SteelSeries Keyboard
    dev = GetDeviceHandle(0x1770, 0xFF00, 0);
    if (dev == NULL)
    {
        init_ok = FALSE;
        return 0;
    }
}

bool MSIKeyboard::SetLEDs(COLORREF pixels[64][256])
{
    //Shout out to bparker06 for reverse engineering the MSI keyboard USB protocol!
    // https://github.com/bparker06/msi-keyboard/blob/master/keyboard.cpp for original implementation

    char buf[8] = { 0 };

    buf[0] = 1;
    buf[1] = 2;
    buf[2] = 64;
    buf[3] = 1;
    buf[4] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][160]);
    buf[5] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][160]);
    buf[6] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][160]);
    buf[7] = 236;

    send_usb_msg(buf);

    buf[3] = 2;
    buf[4] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][192]);
    buf[5] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][192]);
    buf[6] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][192]);

    send_usb_msg(buf);

    buf[3] = 3;
    buf[4] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][224]);
    buf[5] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][224]);
    buf[6] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][224]);

    send_usb_msg(buf);

    return init_ok;
}