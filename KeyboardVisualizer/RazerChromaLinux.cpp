#include "RazerChromaLinux.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

enum
{
    RAZER_NO_DEVICE,
    RAZER_BLACKWIDOW_CHROMA,
    RAZER_ORNATA_CHROMA,
    RAZER_TARTARUS_CHROMA,
    RAZER_DEATHADDER_CHROMA,
    RAZER_MAMBA_TOURNAMENT_EDITION_CHROMA,
    RAZER_FIREFLY_CHROMA,
    RAZER_NUM_DEVICES
};

//Index lists for BlackWidow
int BlackWidowXIndex[22];
int BlackWidowYIndex[6];

//Index list for Firefly
int FireflyIndex[15];

RazerChroma::RazerChroma()
{

}

RazerChroma::~RazerChroma()
{

}

void SetupKeyboardGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for (int x = 0; x < x_count; x++)
    {
        x_idx_list[x] = x * (SPECTROGRAPH_COLS / x_count);
    }
    for (int y = 0; y < y_count; y++)
    {
        y_idx_list[y] = ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / y_count)) + (0.5f * (SPECTROGRAPH_ROWS / y_count));
    }
}

void RazerChroma::Initialize()
{
    int device_type = RAZER_NO_DEVICE;
    char driver_path[512];
    DIR *dir;
    struct dirent *ent;
    int test_fd;
    bool done = false;
    int driver_to_read = 0;

    while(driver_to_read < 3)
    {
        switch(driver_to_read)
        {
        case 0:
            strcpy(driver_path, "/sys/bus/hid/drivers/razerkbd/");
            break;

        case 1:
            strcpy(driver_path, "/sys/bus/hid/drivers/razermouse/");
            break;

        case 2:
            strcpy(driver_path, "/sys/bus/hid/drivers/razerfirefly/");
            break;
        }

        done = false;

        dir = opendir(driver_path);

        if(dir == NULL)
        {
            continue;
        }

        ent = readdir(dir);

        while(ent != NULL)
        {
            if(ent->d_type == DT_DIR || ent->d_type == DT_LNK)
            {
                if(!strcmp(ent->d_name, "."))
                {
                    if(done == false)
                    {
                        done = true;
                    }
                    else
                    {
                        break;
                    }
                }
                else if(!strcmp(ent->d_name, "..") || !strcmp(ent->d_name, "module"))
                {
                }
                else
                {
                    int device_type;
                    char device_string[1024];
                    strcpy(device_string, driver_path);
                    strcat(device_string, ent->d_name);
                    strcat(device_string, "/device_type");
                    test_fd = open(device_string, O_RDONLY);

                    if(test_fd != NULL)
                    {
                        read(test_fd, device_string, sizeof(device_string));
                        close(test_fd);

                        device_type = RAZER_NO_DEVICE;

                        if(!strncmp(device_string, "Razer BlackWidow Chroma", strlen("Razer BlackWidow Chroma")))
                        {
                            //Device is Razer BlackWidow Chroma
                            printf("BlackWidow Chroma Detected\r\n");

                            device_type = RAZER_BLACKWIDOW_CHROMA;
                        }
                        else if(!strncmp(device_string, "Razer Ornata Chroma", strlen("Razer Ornata Chroma")))
                        {
                            //Device is Razer Ornata Chroma
                            printf("Ornata Chroma Detected\r\n");

                            device_type = RAZER_ORNATA_CHROMA;
                        }
                        else if(!strncmp(device_string, "Razer Tartarus Chroma", strlen("Razer Tartarus Chroma")))
                        {
                            //Device is Razer Tartarus Chroma
                            printf("Tartarus Chroma Detected\r\n");

                            device_type = RAZER_TARTARUS_CHROMA;
                        }
                        else if(!strncmp(device_string, "Razer DeathAdder Chroma", strlen("Razer DeathAdder Chroma")))
                        {
                            //Device is Razer DeathAdder Chroma
                            printf("DeathAdder Chroma Detected\r\n");

                            device_type = RAZER_DEATHADDER_CHROMA;
                        }
                        else if(!strncmp(device_string, "Razer Mamba Tournament Edition", strlen("Razer Mamba Tournament Edition")))
                        {
                            //Device is Razer Mamba Tournament Edition
                            printf("Mamba Tournament Edition Detected\r\n");

                            device_type = RAZER_MAMBA_TOURNAMENT_EDITION_CHROMA;
                        }
                        else if(!strncmp(device_string, "Razer Firefly", strlen("Razer Firefly")))
                        {
                            //Device is Razer Firefly
                            printf("Firefly Detected\r\n");

                            device_type = RAZER_FIREFLY_CHROMA;
                        }

                        if(device_type != RAZER_NO_DEVICE)
                        {
                            strcpy(device_string, driver_path);
                            strcat(device_string, ent->d_name);
                            strcat(device_string, "/device_serial");
                            test_fd = open(device_string, O_RDONLY);

                            printf("Reading serial from %s\r\n", device_string);
                            if(test_fd != NULL)
                            {
                                //Device has a serial number
                                bool match = false;

                                //Serial numbers are 15 characters
                                char * serial = new char[16];
                                read(test_fd, serial, 15);
                                close(test_fd);

                                //Null terminate for printing
                                serial[15] = '\0';

                                //Make sure this serial number isn't already registered
                                for(int i = 0; i < razer_device_serial.size(); i++)
                                {
                                    printf("Comparing serial %s with %s\r\n", serial, razer_device_serial[i]);
                                    if(strncmp(serial, razer_device_serial[i], 15) == 0)
                                    {
                                        match = true;
                                    }
                                }

                                if(match)
                                {
                                    printf("Ignoring duplicate serial %s\r\n", serial);
                                }
                                else
                                {
                                    printf("Unique serial %s detected\r\n", serial);

                                    switch(device_type)
                                    {
                                    //Devices with custom effect type and matrix
                                    case RAZER_BLACKWIDOW_CHROMA:
                                    case RAZER_ORNATA_CHROMA:
                                    case RAZER_FIREFLY_CHROMA:
                                    case RAZER_MAMBA_TOURNAMENT_EDITION_CHROMA:
                                        {
                                            //Device is unique, go ahead and register it
                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/matrix_custom_frame");
                                            int custom_fd = open(device_string, O_WRONLY);

                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/matrix_effect_custom");
                                            int update_fd = open(device_string, O_WRONLY);

                                            if(custom_fd && update_fd)
                                            {
                                                razer_device_serial.push_back(serial);
                                                razer_device_type.push_back(device_type);
                                                razer_custom_fd.push_back(custom_fd);
                                                razer_update_fd.push_back(update_fd);
                                            }
                                        }
                                        break;

                                    //Devices with only static effect
                                    case RAZER_TARTARUS_CHROMA:
                                        {
                                            //Device is unique, go ahead and register it
                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/matrix_effect_static");
                                            int custom_fd = open(device_string, O_WRONLY);

                                            //We need a placeholder for the unused update fd to keep the vectors the same length
                                            int update_fd = 0;

                                            if(custom_fd)
                                            {
                                                razer_device_serial.push_back(serial);
                                                razer_device_type.push_back(device_type);
                                                razer_custom_fd.push_back(custom_fd);
                                                razer_update_fd.push_back(update_fd);
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            ent = readdir(dir);
        }
        driver_to_read++;
    }


    //Build index list for BlackWidow
    SetupKeyboardGrid(22, 6, BlackWidowXIndex, BlackWidowYIndex);

    //Build index list for Firefly
    for (int x = 0; x < 15; x++)
    {
        if (x == 7)
        {
            FireflyIndex[x] = 128;
        }
        else if (x < 8)
        {
            FireflyIndex[x] = 7 + ((x+1) * 16);
        }
        else
        {
            FireflyIndex[x] = 8 + (x * 16);
        }
    }
}

bool RazerChroma::SetLEDs(COLORREF pixels[64][256])
{
//    if(!razer_custom_fd || !razer_update_fd)
//    {
//        return false;
//    }
//    else
    {
        for(int i = 0; i < razer_custom_fd.size(); i++)
        {
            switch(razer_device_type[i])
            {
            case RAZER_ORNATA_CHROMA:
            case RAZER_BLACKWIDOW_CHROMA:
                {
                    char BlackWidowEffect[((3 * 22)) + 3];

                    BlackWidowEffect[1] = 0;
                    BlackWidowEffect[2] = 21;

                    for(int y = 0; y < 6; y++)
                    {
                        BlackWidowEffect[0] = y;

                        for(int x = 0; x < 22; x++)
                        {
                            BlackWidowEffect[3 + (x * 3)] = GetRValue(pixels[BlackWidowYIndex[y]][BlackWidowXIndex[x]]);
                            BlackWidowEffect[4 + (x * 3)] = GetGValue(pixels[BlackWidowYIndex[y]][BlackWidowXIndex[x]]);
                            BlackWidowEffect[5 + (x * 3)] = GetBValue(pixels[BlackWidowYIndex[y]][BlackWidowXIndex[x]]);

                            if(y == 0 && x == 20)
                            {
                                BlackWidowEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][11 * (256 / 22)]);
                                BlackWidowEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][11 * (256 / 22)]);
                                BlackWidowEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][11 * (256 / 22)]);
                            }
                        }

                        write(razer_custom_fd[i], &BlackWidowEffect, sizeof(BlackWidowEffect));
                    }
                    write(razer_update_fd[i], &BlackWidowEffect, 1);
                }
                break;

            case RAZER_FIREFLY_CHROMA:
                {
                    char FireflyEffect[(3 * 15) + 3];

                    FireflyEffect[0] = 0;
                    FireflyEffect[1] = 0;
                    FireflyEffect[2] = 14;

                    for(int x = 0; x < 15; x++)
                    {
                        FireflyEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][FireflyIndex[x]]);
                        FireflyEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][FireflyIndex[x]]);
                        FireflyEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][FireflyIndex[x]]);
                    }

                    FireflyEffect[sizeof(FireflyEffect)-1] = '\n';

                    write(razer_custom_fd[i], &FireflyEffect, sizeof(FireflyEffect));
                    write(razer_update_fd[i], &FireflyEffect, 1);
                }
                break;

            case RAZER_MAMBA_TOURNAMENT_EDITION_CHROMA:
                {
                    char MambaTEEffect[(3 * 16) + 3];

                    MambaTEEffect[0] = 0;
                    MambaTEEffect[1] = 0;
                    MambaTEEffect[2] = 15;

                    for(int x = 0; x < 16; x++)
                    {
                        if(x < 7)
                        {
                            MambaTEEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][x * (128 / 7)]);
                            MambaTEEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][x * (128 / 7)]);
                            MambaTEEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][x * (128 / 7)]);
                        }
                        else if(x < 14)
                        {
                            MambaTEEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][(x - 7) * (128 / 7)]);
                            MambaTEEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][(x - 7) * (128 / 7)]);
                            MambaTEEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][(x - 7) * (128 / 7)]);
                        }
                        else if(x == 14)
                        {
                            MambaTEEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][14 * 5]);
                            MambaTEEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][14 * 5]);
                            MambaTEEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][14 * 5]);
                        }
                        else if(x == 15)
                        {
                            MambaTEEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                            MambaTEEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                            MambaTEEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                        }
                    }

                    write(razer_custom_fd[i], &MambaTEEffect, sizeof(MambaTEEffect));
                    write(razer_update_fd[i], &MambaTEEffect, 1);
                }
                break;

            case RAZER_TARTARUS_CHROMA:
                {
                    char TartarusEffect[3];

                    TartarusEffect[0] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                    TartarusEffect[1] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                    TartarusEffect[2] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][0]);

                    write(razer_custom_fd[i], &TartarusEffect, sizeof(TartarusEffect));
                }
                break;
            }
        }
        return true;
    }
}
