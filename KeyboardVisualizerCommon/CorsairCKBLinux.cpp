#include "CorsairCKBLinux.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

CorsairCKB::CorsairCKB()
{

}

void CorsairCKB::Initialize()
{
/*
    char activate_cmd[] = "activate\n";
    char dither_cmd[] = "dither 1\n";

    corsair_fd = open("/dev/input/ckb1/cmd",O_WRONLY);

    if(corsair_fd != -1)
    {
        write(corsair_fd, activate_cmd, strlen(activate_cmd));
        write(corsair_fd, dither_cmd, strlen(dither_cmd));
    }
*/
}

bool CorsairCKB::SetLEDs(COLORREF pixels[64][256])
{
    return false;
}
