#include "CorsairCKBLinux.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

const static int led_matrix_c[7][22]
{//Col Pos:   0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21
/*Row 0*/   { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 137, 8,   255, 255, 20,  255, 255 },
/*Row 1*/   { 255, 0,   255, 12,  24,  36,  48,  60,  72,  84,  96,  108, 120, 132, 6,   18,  30,  42,  32,  44,  56,  68  }, //68
/*Row 2*/   { 255, 1,   13,  25,  37,  49,  61,  73,  85,  97,  109, 121, 133, 7,   31,  54,  66,  78,  80,  92,  104, 116 }, //116
/*Row 3*/   { 255, 2,   14,  26,  38,  50,  62,  74,  86,  98,  110, 122, 134, 90,  102, 43,  55,  67,  9,   21,  33,  128 }, //128
/*Row 4*/   { 255, 3,   15,  27,  39,  51,  63,  75,  87,  99,  111, 123, 135, 114, 126, 255, 255, 255, 57,  69,  81,  128 },
/*Row 5*/   { 255, 4,   16,  28,  40,  52,  64,  76,  88,  100, 112, 124, 136, 255, 79,  255, 103, 255, 93,  105, 117, 140 },
/*Row 6*/   { 255, 5,   17,  29,  255, 255, 255, 53,  255, 255, 255, 89,  101, 113, 91,  115, 127, 139, 255, 129, 141, 140 }
};

CorsairCKB::CorsairCKB()
{

}

void CorsairCKB::Initialize()
{
    char activate_cmd[] = "activate\n";
    char dither_cmd[] = "dither 1\n";

    corsair_fd = open("/dev/input/ckb1/cmd",O_WRONLY);

    if(corsair_fd != -1)
    {
        write(corsair_fd, activate_cmd, strlen(activate_cmd));
        write(corsair_fd, dither_cmd, strlen(dither_cmd));
    }
}

bool CorsairCKB::SetLEDs(COLORREF pixels[64][256])
{
    return false;
}
