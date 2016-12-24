#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#ifdef LIBUSB
#include <usb.h>
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
#ifdef LIBUSB
    struct usb_device*        device
    struct usb_device_handle* handle;
#elif defined(WIN32)
    HANDLE                    handle;
#endif
    
};
#endif