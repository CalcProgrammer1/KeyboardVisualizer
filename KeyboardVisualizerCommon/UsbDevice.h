#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#ifndef WIN32
#define HIDAPI
#endif

#include <cstdio>
#ifdef HIDAPI
#include <hidapi/hidapi.h>
#elif defined(WIN32)
#include <sstream>
#include <wchar.h>
#include <string.h>
#include <Windows.h>
#include <hidsdi.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#endif

class UsbDevice
{
public:
    UsbDevice();

    bool OpenDevice(unsigned short vendor, unsigned short product, unsigned int MI);

    bool SendToDevice(unsigned char* data, unsigned int length);

private:
#ifdef HIDAPI
    hid_device*        device;
#elif defined(WIN32)
    HANDLE             handle;
#endif
    
};
#endif
