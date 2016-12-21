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
    RAZER_DEATHSTALKER_CHROMA,
    RAZER_ORNATA_CHROMA,
    RAZER_TARTARUS_CHROMA,
    RAZER_DEATHADDER_CHROMA,
    RAZER_DIAMONDBACK_CHROMA,
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
                        if(!strncmp(device_string, "Razer DeathStalker Chroma", strlen("Razer DeathStalker Chroma")))
                        {
                            //Device is Razer DeathStalker Chroma
                            printf("DeathStalker Chroma Detected\r\n");

                            device_type = RAZER_DEATHSTALKER_CHROMA;
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
                        else if(!strncmp(device_string, "Razer Diamondback Chroma", strlen("Razer Diamondback Chroma")))
                        {
                            //Device is Razer Diamondback Chroma
                            printf("Diamondback Chroma Detected\r\n");

                            device_type = RAZER_DIAMONDBACK_CHROMA;
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
                                    case RAZER_DEATHSTALKER_CHROMA:
                                    case RAZER_ORNATA_CHROMA:
                                    case RAZER_FIREFLY_CHROMA:
                                    case RAZER_MAMBA_TOURNAMENT_EDITION_CHROMA:
                                    case RAZER_DIAMONDBACK_CHROMA:
                                        {
                                            //Device is unique, go ahead and register it
                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/matrix_custom_frame");
                                            int fd_1 = open(device_string, O_WRONLY);

                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/matrix_effect_custom");
                                            int fd_2 = open(device_string, O_WRONLY);

                                            //We need placeholders for the unused update fds to keep the vectors the same length
                                            int fd_3 = 0;
                                            int fd_4 = 0;

                                            if(fd_1 && fd_2)
                                            {
                                                razer_device_serial.push_back(serial);
                                                razer_device_type.push_back(device_type);
                                                razer_fd_1.push_back(fd_1);
                                                razer_fd_2.push_back(fd_2);
                                                razer_fd_3.push_back(fd_3);
                                                razer_fd_4.push_back(fd_4);
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
                                            int fd_1 = open(device_string, O_WRONLY);

                                            //We need placeholders for the unused update fds to keep the vectors the same length
                                            int fd_2 = 0;
                                            int fd_3 = 0;
                                            int fd_4 = 0;

                                            if(fd_1)
                                            {
                                                razer_device_serial.push_back(serial);
                                                razer_device_type.push_back(device_type);
                                                razer_fd_1.push_back(fd_1);
                                                razer_fd_2.push_back(fd_2);
                                                razer_fd_3.push_back(fd_3);
                                                razer_fd_4.push_back(fd_4);
                                            }
                                        }
                                        break;

                                    //Devices with logo LED rgb and effect parameters
                                    case RAZER_DEATHADDER_CHROMA:
                                        {
                                            //Device is unique, go ahead and register it
                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/logo_led_rgb");
                                            int fd_1 = open(device_string, O_WRONLY);

                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/logo_led_effect");
                                            int fd_2 = open(device_string, O_WRONLY);

                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/scroll_led_rgb");
                                            int fd_3 = open(device_string, O_WRONLY);

                                            strcpy(device_string, driver_path);
                                            strcat(device_string, ent->d_name);
                                            strcat(device_string, "/scroll_led_effect");
                                            int fd_4 = open(device_string, O_WRONLY);

                                            if(fd_1 && fd_2 && fd_3 && fd_4)
                                            {
                                                razer_device_serial.push_back(serial);
                                                razer_device_type.push_back(device_type);
                                                razer_fd_1.push_back(fd_1);
                                                razer_fd_2.push_back(fd_2);
                                                razer_fd_3.push_back(fd_3);
                                                razer_fd_4.push_back(fd_4);
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
//    if(!razer_fd_1 || !razer_fd_2)
//    {
//        return false;
//    }
//    else
    {
        for(int i = 0; i < razer_fd_1.size(); i++)
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

                        write(razer_fd_1[i], &BlackWidowEffect, sizeof(BlackWidowEffect));
                    }
                    write(razer_fd_2[i], &BlackWidowEffect, 1);
                }
                break;

            case RAZER_DEATHSTALKER_CHROMA:
                {
                    char DeathStalkerEffect[(3 * 12) + 3];

                    DeathStalkerEffect[0] = 0;
                    DeathStalkerEffect[1] = 0;
                    DeathStalkerEffect[2] = 11;

                    for(int x = 0; x < 12; x++)
                    {
                        DeathStalkerEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][(11 - x) * (128 / 12)]);
                        DeathStalkerEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][(11 - x) * (128 / 12)]);
                        DeathStalkerEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][(11 - x) * (128 / 12)]);
                    }

                    write(razer_fd_1[i], &DeathStalkerEffect, sizeof(DeathStalkerEffect));
                    write(razer_fd_2[i], &DeathStalkerEffect, 1);
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

                    write(razer_fd_1[i], &FireflyEffect, sizeof(FireflyEffect));
                    write(razer_fd_2[i], &FireflyEffect, 1);
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

                    write(razer_fd_1[i], &MambaTEEffect, sizeof(MambaTEEffect));
                    write(razer_fd_2[i], &MambaTEEffect, 1);
                }
                break;

            case RAZER_DIAMONDBACK_CHROMA:
                {
                    char DiamondbackEffect[(3 * 21) + 3];

                    DiamondbackEffect[0] = 0;
                    DiamondbackEffect[1] = 0;
                    DiamondbackEffect[2] = 20;

                    for(int x = 0; x < 21; x++)
                    {
                        if(x < 9)
                        {
                            DiamondbackEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][(x * (128 / 9)) + (128/18)]);
                            DiamondbackEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][(x * (128 / 9)) + (128/18)]);
                            DiamondbackEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][(x * (128 / 9)) + (128/18)]);
                        }
                        else if(x == 9)
                        {
                            DiamondbackEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][128]);
                            DiamondbackEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][128]);
                            DiamondbackEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][128]);
                        }
                        else if(x < 19)
                        {
                            DiamondbackEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_BAR_GRAPH][((18 - x) * (128 / 9)) + (128/18)]);
                            DiamondbackEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_BAR_GRAPH][((18 - x) * (128 / 9)) + (128/18)]);
                            DiamondbackEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_BAR_GRAPH][((18 - x) * (128 / 9)) + (128/18)]);
                        }
                        else if(x == 19)
                        {
                            DiamondbackEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][14 * 5]);
                            DiamondbackEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][14 * 5]);
                            DiamondbackEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][14 * 5]);
                        }
                        else if(x == 20)
                        {
                            DiamondbackEffect[3 + (x * 3)] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                            DiamondbackEffect[4 + (x * 3)] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                            DiamondbackEffect[5 + (x * 3)] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                        }
                    }

                    write(razer_fd_1[i], &DiamondbackEffect, sizeof(DiamondbackEffect));
                    write(razer_fd_2[i], &DiamondbackEffect, 1);
                }
                break;

            case RAZER_TARTARUS_CHROMA:
                {
                    char TartarusEffect[3];

                    TartarusEffect[0] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                    TartarusEffect[1] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                    TartarusEffect[2] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][0]);

                    write(razer_fd_1[i], &TartarusEffect, sizeof(TartarusEffect));
                }
                break;

            case RAZER_DEATHADDER_CHROMA:
                {
                    char DeathAdderEffect[3];

                    DeathAdderEffect[0] = 3;
                    write(razer_fd_2[i], &DeathAdderEffect, 1);
                    write(razer_fd_4[i], &DeathAdderEffect, 1);

                    DeathAdderEffect[0] = GetRValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                    DeathAdderEffect[1] = GetGValue(pixels[ROW_IDX_SINGLE_COLOR][0]);
                    DeathAdderEffect[2] = GetBValue(pixels[ROW_IDX_SINGLE_COLOR][0]);

                    write(razer_fd_1[i], &DeathAdderEffect, sizeof(DeathAdderEffect));
                    write(razer_fd_3[i], &DeathAdderEffect, sizeof(DeathAdderEffect));

                    DeathAdderEffect[0] = 0;
                    write(razer_fd_2[i], &DeathAdderEffect, 1);
                    write(razer_fd_4[i], &DeathAdderEffect, 1);
                }
                break;
            }
        }
        return true;
    }
}
