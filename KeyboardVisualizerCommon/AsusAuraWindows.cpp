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
#include <iostream>
#include "inpout32.h"

#pragma comment(lib, "inpout32.lib")

//Index list for Aura zone
int AuraXIndex[5];

//Index lists for Trident Z RGB
int TridentZRGBXIndex[5];
int TridentZRGBYIndex[4];


/*
* Data for SMBus Messages
*/
#define I2C_SMBUS_BLOCK_MAX     32				/* As specified in SMBus standard */
union i2c_smbus_data {
	uint8_t		byte;
	uint16_t	word;
	uint8_t		block[I2C_SMBUS_BLOCK_MAX + 2]; /* block[0] is used for length */
												/* and one more for user-space compatibility */
};

/* i2c_smbus_xfer read or write markers */
#define I2C_SMBUS_READ  1
#define I2C_SMBUS_WRITE 0

/* SMBus transaction types (size parameter in the above functions)
Note: these no longer correspond to the (arbitrary) PIIX4 internal codes! */
#define I2C_SMBUS_QUICK             0
#define I2C_SMBUS_BYTE              1
#define I2C_SMBUS_BYTE_DATA         2
#define I2C_SMBUS_WORD_DATA         3
#define I2C_SMBUS_PROC_CALL         4
#define I2C_SMBUS_BLOCK_DATA        5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7           /* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

/* PIIX4 SMBus address offsets */
#define SMBHSTSTS (0 + piix4_smba)
#define SMBHSLVSTS (1 + piix4_smba)
#define SMBHSTCNT (2 + piix4_smba)
#define SMBHSTCMD (3 + piix4_smba)
#define SMBHSTADD (4 + piix4_smba)
#define SMBHSTDAT0 (5 + piix4_smba)
#define SMBHSTDAT1 (6 + piix4_smba)
#define SMBBLKDAT (7 + piix4_smba)
#define SMBSLVCNT (8 + piix4_smba)
#define SMBSHDWCMD (9 + piix4_smba)
#define SMBSLVEVT (0xA + piix4_smba)
#define SMBSLVDAT (0xC + piix4_smba)

#define MAX_TIMEOUT     5000

/* PIIX4 constants */
#define PIIX4_QUICK             0x00
#define PIIX4_BYTE              0x04
#define PIIX4_BYTE_DATA         0x08
#define PIIX4_WORD_DATA         0x0C
#define PIIX4_BLOCK_DATA        0x14

uint16_t piix4_smba = 0x0B00;

//Logic adapted from piix4_access() in i2c-piix4.c
uint32_t i2c_access_piix4(uint8_t addr, bool read_write, uint8_t command, int size, int length, i2c_smbus_data* data)
{
	int i, status, timeout;

	timeout = 0;

	switch (size)
	{
	case I2C_SMBUS_BYTE_DATA:
		Out32(SMBHSTADD, (addr << 1) | read_write);
		Out32(SMBHSTCMD, command);
		if (read_write == I2C_SMBUS_WRITE)
		{
			Out32(SMBHSTDAT0, data->byte);
		}
		size = PIIX4_BYTE_DATA;
		break;
	case I2C_SMBUS_WORD_DATA:
		Out32(SMBHSTADD, (addr << 1) | read_write);
		Out32(SMBHSTCMD, command);
		if (read_write == I2C_SMBUS_WRITE)
		{
			Out32(SMBHSTDAT0, data->word & 0xFF);
			Out32(SMBHSTDAT1, (data->word & 0xFF00) >> 8);
		}
		size = PIIX4_WORD_DATA;
		break;
	case I2C_SMBUS_BLOCK_DATA:
		Out32(SMBHSTADD, (addr << 1) | read_write);
		Out32(SMBHSTCMD, command);
		if (read_write == I2C_SMBUS_WRITE)
		{
			if (length == 0 || length > I2C_SMBUS_BLOCK_MAX)
			{
				return 0;
			}
			Out32(SMBHSTDAT0, length);
			Inp32(SMBHSTCNT);
			for (i = 0; i < length; i++)
			{
				Out32(SMBBLKDAT, data->block[i]);
			}
		}
		size = PIIX4_BLOCK_DATA;
		break;
	default:
		return 0;
	}

	Out32(SMBHSTCNT, (size & 0x1C));
	int temp = Inp32(SMBHSTSTS);

	if (temp != 0x00)
	{
		Out32(SMBHSTSTS, temp);
	}

	temp = Inp32(SMBHSTCNT);
	Out32(SMBHSTCNT, temp | 0x040);

	temp = 0;
	while ((++timeout < MAX_TIMEOUT) && temp <= 1)
	{
		temp = Inp32(SMBHSTSTS);
	}

	if ((read_write == I2C_SMBUS_WRITE) || (size == PIIX4_QUICK))
	{
		return 0;
	}

	switch (size)
	{
	case PIIX4_BYTE:
	case PIIX4_BYTE_DATA:
		data->byte = Inp32(SMBHSTDAT0);
		break;
	case PIIX4_WORD_DATA:
		data->word = Inp32(SMBHSTDAT0) + (Inp32(SMBHSTDAT1) << 8);
		break;
	case PIIX4_BLOCK_DATA:
		length = Inp32(SMBHSTDAT0);
		if (length == 0 || length > I2C_SMBUS_BLOCK_MAX)
		{
			return 0;
		}
		Inp32(SMBHSTCNT);
		for (i = 0; i < length; i++)
		{
			data->block[i] = Inp32(SMBBLKDAT);
		}
		break;
	}

	return 0;
}

void i2c_smbus_write_byte_data(uint8_t addr, uint8_t command, uint8_t byte)
{
	i2c_access_piix4(addr, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, 0, (i2c_smbus_data*)&byte);
}

void i2c_smbus_write_word_data(uint8_t addr, uint8_t command, uint16_t word)
{
	i2c_access_piix4(addr, I2C_SMBUS_WRITE, command, I2C_SMBUS_WORD_DATA, 0, (i2c_smbus_data*)&word);
}

void i2c_smbus_write_block_data(uint8_t addr, uint8_t command, uint8_t length, uint8_t* block_data)
{
	i2c_access_piix4(addr, I2C_SMBUS_WRITE, command, I2C_SMBUS_BLOCK_DATA, length, (i2c_smbus_data*)block_data);
}

uint8_t i2c_smbus_read_byte_data(uint8_t addr, uint8_t command)
{
	uint8_t ret_val;
	i2c_access_piix4(addr, I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA, 0, (i2c_smbus_data*)&ret_val);
	return ret_val;
}

void WriteAuraRegister(uint8_t aura_device, uint16_t aura_register, uint8_t val)
{
	//Write Aura register
	i2c_smbus_write_word_data(aura_device, 0x00, ((aura_register << 8) & 0xFF00) | ((aura_register >> 8) & 0x00FF));

	//Write Aura value
	i2c_smbus_write_byte_data(aura_device, 0x01, val);
}

void WriteAuraColors(uint8_t aura_device, uint8_t* colors)
{
	//Write Aura register (0x8000 for colors)
	i2c_smbus_write_word_data(aura_device, 0x00, 0x0080);

	//Write Aura color array
	i2c_smbus_write_block_data(aura_device, 0x03, 15, colors);
}

uint8_t ReadAuraRegister(uint8_t aura_device, uint16_t aura_register)
{
	//Write Aura register
	i2c_smbus_write_word_data(aura_device, 0x00, ((aura_register << 8) & 0xFF00) | ((aura_register >> 8) & 0x00FF));

	//Read Aura value
	return(i2c_smbus_read_byte_data(aura_device, 0x81));
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
    //Set Music Mode
    //  0x8020 = 0x01
    //  0x8021 = 0x01
    //  0x8025 = 0xFF
	piix4_smba = 0x0B00;
    WriteAuraRegister(0x70, 0x8020, 0x01);
    WriteAuraRegister(0x70, 0x8021, 0x01);
    WriteAuraRegister(0x70, 0x8025, 0xFF);

    WriteAuraRegister(0x71, 0x8020, 0x01);
    WriteAuraRegister(0x71, 0x8021, 0x01);
    WriteAuraRegister(0x71, 0x8025, 0xFF);

    WriteAuraRegister(0x72, 0x8020, 0x01);
    WriteAuraRegister(0x72, 0x8021, 0x01);
    WriteAuraRegister(0x72, 0x8025, 0xFF);

    WriteAuraRegister(0x73, 0x8020, 0x01);
    WriteAuraRegister(0x73, 0x8021, 0x01);
    WriteAuraRegister(0x73, 0x8025, 0xFF);

	piix4_smba = 0x0B20;
    WriteAuraRegister(0x4E, 0x8020, 0x01);
    WriteAuraRegister(0x4E, 0x8021, 0x01);
    WriteAuraRegister(0x4E, 0x8025, 0xFF);

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
    uint8_t aura_led_colors[5][15];

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

	piix4_smba = 0x0B00;
    WriteAuraColors(0x70, aura_led_colors[3]);
    WriteAuraRegister(0x70, 0x80A0, 0x01);

    WriteAuraColors(0x71, aura_led_colors[1]);
    WriteAuraRegister(0x71, 0x80A0, 0x01);

    WriteAuraColors(0x72, aura_led_colors[2]);
    WriteAuraRegister(0x72, 0x80A0, 0x01);

    WriteAuraColors(0x73, aura_led_colors[0]);
    WriteAuraRegister(0x73, 0x80A0, 0x01);

	piix4_smba = 0x0B20;
    WriteAuraColors(0x4E, aura_led_colors[4]);
    WriteAuraRegister(0x4E, 0x80A0, 0x01);

    return true;
}
