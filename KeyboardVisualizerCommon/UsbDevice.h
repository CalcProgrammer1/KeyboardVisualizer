#ifndef USB_DEVICE_H
#define USB_DEVICE_H


#define HIDAPI


#include <cstdio>

#include "hidapi.h"



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
