/*---------------------------------------------------------*\
|  Processing Code for Asus Aura Interface                  |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 3/28/2017        |
\*---------------------------------------------------------*/

#include "AsusAuraLinux.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "i2c-dev.h"
#include <sys/ioctl.h>

//Index list for Aura zone
int AuraXIndex[5];

//Index lists for Trident Z RGB
int TridentZRGBXIndex[5];
int TridentZRGBYIndex[4];

void WriteAuraRegister(int handle, char aura_device, short aura_register, char val)
{
    //Tell I2C host which slave address to transfer to
    ioctl(handle, I2C_SLAVE, aura_device);

    //Write Aura register
    i2c_smbus_write_word_data(handle, 0x00, ((aura_register << 8) & 0xFF00) || ((aura_register >> 8) & 0x00FF));

    //Write Aura value
    i2c_smbus_write_byte_data(handle, 0x01, val);
}

void WriteAuraColors(int handle, char aura_device, char* colors)
{
    //Tell I2C host which slave address to transfer to
    ioctl(handle, I2C_SLAVE, aura_device);

    //Write Aura register (0x8000 for colors)
    i2c_smbus_write_word_data(handle, 0x00, 0x0080);

    //Write Aura color array
    i2c_smbus_write_block_data(handle, 0x03, 15, (__u8*)colors);
}

char ReadAuraRegister(int handle, char aura_device, short aura_register)
{
    //Tell I2C host which slave address to transfer to
    ioctl(handle, I2C_SLAVE, aura_device);

    //Write Aura register
    i2c_smbus_write_word_data(handle, 0x00, ((aura_register << 8) & 0xFF00) || ((aura_register >> 8) & 0x00FF));

    //Write Aura value
    return(i2c_smbus_read_byte_data(handle, 0x81));
}

static void SetupKeyboardGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for (int x = 0; x < x_count; x++)
    {
        if (x_count < 10)
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count))) + (0.5f * (SPECTROGRAPH_COLS / (x_count))));
        }
        else if (x < ((x_count) / 2))
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) + (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }
        else
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) - (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }

    }
    for (int y = 0; y < y_count; y++)
    {
        y_idx_list[y] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / y_count)) + (0.5f * (SPECTROGRAPH_ROWS / y_count)));
    }
}

AsusAura::AsusAura()
{

}

void AsusAura::Initialize()
{
    //Set i2c busses here
    char *smbus_adapter_dev = "/dev/i2c-0";
    char *smbus_adapter_dev2 = "/dev/i2c-4";

    smbus_fd = open(smbus_adapter_dev, O_RDWR);
    smbus_fd2 = open(smbus_adapter_dev2, O_RDWR);

    if(smbus_fd < 0)
    {
        return;
    }

    if(smbus_fd2 < 0)
    {
        return;
    }

    //Set Music Mode
    //  0x8020 = 0x01
    //  0x8021 = 0x01
    //  0x8025 = 0xFF
    WriteAuraRegister(smbus_fd,  0x70, 0x8020, 0x01);
    WriteAuraRegister(smbus_fd,  0x70, 0x8021, 0x01);
    WriteAuraRegister(smbus_fd,  0x70, 0x8025, 0xFF);

    WriteAuraRegister(smbus_fd,  0x71, 0x8020, 0x01);
    WriteAuraRegister(smbus_fd,  0x71, 0x8021, 0x01);
    WriteAuraRegister(smbus_fd,  0x71, 0x8025, 0xFF);

    WriteAuraRegister(smbus_fd,  0x72, 0x8020, 0x01);
    WriteAuraRegister(smbus_fd,  0x72, 0x8021, 0x01);
    WriteAuraRegister(smbus_fd,  0x72, 0x8025, 0xFF);

    WriteAuraRegister(smbus_fd,  0x73, 0x8020, 0x01);
    WriteAuraRegister(smbus_fd,  0x73, 0x8021, 0x01);
    WriteAuraRegister(smbus_fd,  0x73, 0x8025, 0xFF);

    WriteAuraRegister(smbus_fd2, 0x4E, 0x8020, 0x01);
    WriteAuraRegister(smbus_fd2, 0x4E, 0x8021, 0x01);
    WriteAuraRegister(smbus_fd2, 0x4E, 0x8025, 0xFF);

    //Build index list for Trident Z RGB
    SetupKeyboardGrid(5, 4, TridentZRGBXIndex, TridentZRGBYIndex);

    //Build index list Aura zone
    for (int x = 0; x < 5; x++)
    {
        AuraXIndex[x] = (x * (128 / 5) + (128 / 10));
    }
}

bool AsusAura::SetLEDs(COLORREF pixels[64][256])
{
    char aura_led_colors[5][15];

    //Treat the motherboard audio LEDs as a bar graph
    for(int x = 0; x < 5; x++)
    {
        COLORREF color = pixels[ROW_IDX_BAR_GRAPH][AuraXIndex[x]];
        aura_led_colors[4][3*x  ]     = (char)GetRValue(color);
        aura_led_colors[4][3*x+1]     = (char)GetBValue(color);
        aura_led_colors[4][3*x+2]     = (char)GetGValue(color);
    }

    //Treat 4 sticks of Trident Z RGB RAM as a sideways spectrograph
    for(int x = 0; x < 5; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            COLORREF color = pixels[TridentZRGBYIndex[y]][TridentZRGBXIndex[x]];
            aura_led_colors[y][3*x  ] = (char)GetRValue(color);
            aura_led_colors[y][3*x+1] = (char)GetBValue(color);
            aura_led_colors[y][3*x+2] = (char)GetGValue(color);
        }
    }

    WriteAuraColors(smbus_fd, 0x70, aura_led_colors[3]);
    WriteAuraRegister(smbus_fd, 0x70, 0x80A0, 0x01);

    WriteAuraColors(smbus_fd, 0x71, aura_led_colors[1]);
    WriteAuraRegister(smbus_fd, 0x71, 0x80A0, 0x01);

    WriteAuraColors(smbus_fd, 0x72, aura_led_colors[2]);
    WriteAuraRegister(smbus_fd, 0x72, 0x80A0, 0x01);

    WriteAuraColors(smbus_fd, 0x73, aura_led_colors[0]);
    WriteAuraRegister(smbus_fd, 0x73, 0x80A0, 0x01);

    WriteAuraColors(smbus_fd2, 0x4E, aura_led_colors[4]);
    WriteAuraRegister(smbus_fd, 0x70, 0x80A0, 0x01);

    return true;
}
