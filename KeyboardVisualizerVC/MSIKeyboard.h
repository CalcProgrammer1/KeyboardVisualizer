#pragma once

#include <sstream>
#include <wchar.h>
#include <string.h>
#include <Windows.h>
extern "C"
{
#include <hidsdi.h>
}
#include <setupapi.h>
#include <cfgmgr32.h>

class MSIKeyboard
{
public:
    MSIKeyboard();
    ~MSIKeyboard();

    int Initialize();
    bool SetLEDs(COLORREF pixels[64][256]);

private:
    HANDLE dev;
    void send_usb_msg(char * data_pkt);
};
