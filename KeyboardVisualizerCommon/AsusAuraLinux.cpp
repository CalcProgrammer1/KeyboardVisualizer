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
    char *smbus_adapter_dev = "/dev/i2c-19";

    smbus_fd = open(smbus_adapter_dev, O_RDWR);

    if(smbus_fd < 0)
    {
        return;
    }

    int addr = 0x72;

    if(ioctl(smbus_fd, I2C_SLAVE, addr) < 0)
    {
        return;
    }


}

bool AsusAura::SetLEDs(COLORREF pixels[64][256])
{
    char smbus_led_data[15];

    for(int i = 0; i < 15; i+=3)
    {
        smbus_led_data[i]   = (char)0xFF;
        smbus_led_data[i+1] = (char)0xFF;
        smbus_led_data[i+2] = (char)0xFF;
    }

    i2c_smbus_write_word_data(smbus_fd, 0x00, 0x0080);
    i2c_smbus_write_block_data(smbus_fd, 0x03, 15, (__u8*)&smbus_led_data);
    i2c_smbus_write_word_data(smbus_fd, 0x00, 0xA080);
    i2c_smbus_write_byte_data(smbus_fd, 0x01, 0x01);

    return true;
}
