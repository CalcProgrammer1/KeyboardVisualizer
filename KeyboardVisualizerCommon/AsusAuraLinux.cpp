#include "AsusAuraLinux.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "i2c-dev.h"
#include <sys/ioctl.h>

AsusAura::AsusAura()
{

}

void AsusAura::Initialize()
{
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

    int addr = 0x72;

    if(ioctl(smbus_fd, I2C_SLAVE, addr) < 0)
    {
        return;
    }

    addr = 0x4E;

    if(ioctl(smbus_fd2, I2C_SLAVE, addr) < 0)
    {
        return;
    }

}

bool AsusAura::SetLEDs(COLORREF pixels[64][256])
{
    char smbus_led_data[15];

    for(int i = 0; i < 15; i+=3)
    {
        COLORREF color = pixels[ROW_IDX_BAR_GRAPH][255 - i * (128 / 15)];
        smbus_led_data[i]   = (char)GetRValue(color);
        smbus_led_data[i+1] = (char)GetBValue(color);
        smbus_led_data[i+2] = (char)GetGValue(color);
    }

    ioctl(smbus_fd, I2C_SLAVE, 0x72);
    i2c_smbus_write_word_data(smbus_fd, 0x00, 0x0080);
    i2c_smbus_write_block_data(smbus_fd, 0x03, 15, (__u8*)&smbus_led_data);
    i2c_smbus_write_word_data(smbus_fd, 0x00, 0xA080);
    i2c_smbus_write_byte_data(smbus_fd, 0x01, 0x01);

    ioctl(smbus_fd, I2C_SLAVE, 0x73);
    i2c_smbus_write_word_data(smbus_fd, 0x00, 0x0080);
    i2c_smbus_write_block_data(smbus_fd, 0x03, 15, (__u8*)&smbus_led_data);
    i2c_smbus_write_word_data(smbus_fd, 0x00, 0xA080);
    i2c_smbus_write_byte_data(smbus_fd, 0x01, 0x01);

    i2c_smbus_write_word_data(smbus_fd2, 0x00, 0x0080);
    i2c_smbus_write_block_data(smbus_fd2, 0x03, 15, (__u8*)&smbus_led_data);
    i2c_smbus_write_word_data(smbus_fd2, 0x00, 0xA080);
    i2c_smbus_write_byte_data(smbus_fd2, 0x01, 0x01);
    return true;
}
