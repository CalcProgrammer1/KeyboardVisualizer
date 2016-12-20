#include "RazerChromaLinux.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

//Index list for Firefly
int FireflyIndex[15];

RazerChroma::RazerChroma()
{

}

RazerChroma::~RazerChroma()
{

}

void RazerChroma::Initialize()
{
    DIR *dir;
    struct dirent *ent;
    int test_fd;
    bool done = false;

    //Initialize all keyboards
    dir = opendir("/sys/bus/hid/drivers/razerkbd/");

    if(dir != NULL)
    {
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
                    char device_type_path[2048];
                    strcpy(device_type_path, "/sys/bus/hid/drivers/razerkbd/");
                    strcat(device_type_path, ent->d_name);
                    strcat(device_type_path, "/device_type");
                    test_fd = open(device_type_path, O_RDONLY);

                    if(test_fd != NULL)
                    {
                        char device_type_str[1024];
                        read(test_fd, device_type_str, sizeof(device_type_str));

                        if(!strncmp(device_type_str, "Razer BlackWidow Chroma", strlen("Razer BlackWidow Chroma")))
                        {
                            //Device is Razer BlackWidow Chroma
                            printf("BlackWidow Chroma Detected\r\n");
                        }
                        else if(!strncmp(device_type_str, "Razer Tartarus Chroma", strlen("Razer Tartarus Chroma")))
                        {
                            //Device is Razer Tartarus Chroma
                            printf("Tartarus Chroma Detected\r\n");
                        }
                    }
                }
            }
            ent = readdir(dir);
        }
    }

    done = false;

    dir = opendir("/sys/bus/hid/drivers/razermouse/");

    //Initialize all mice
    if(dir != NULL)
    {
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
                    char device_type_path[2048];
                    strcpy(device_type_path, "/sys/bus/hid/drivers/razermouse/");
                    strcat(device_type_path, ent->d_name);
                    strcat(device_type_path, "/device_type");
                    test_fd = open(device_type_path, O_RDONLY);

                    if(test_fd != NULL)
                    {
                        char device_type_str[1024];
                        read(test_fd, device_type_str, sizeof(device_type_str));

                        if(!strncmp(device_type_str, "Razer Mamba Tournament Edition", strlen("Razer Mamba Tournament Edition")))
                        {
                            //Device is Razer Mamba Tournament Edition
                            printf("Mamba Tournament Edition Detected\r\n");
                        }
                    }
                }
            }
            ent = readdir(dir);
        }
    }

    razer_custom_fd[0] = open("/sys/bus/hid/drivers/razerfirefly/0003:1532:0C00.0011/matrix_custom_frame",O_WRONLY);
    razer_update_fd[0] = open("/sys/bus/hid/drivers/razerfirefly/0003:1532:0C00.0011/matrix_effect_custom",O_WRONLY);

    razer_custom_fd[1] = open("/sys/bus/hid/drivers/razermouse/0003:1532:0046.0005/matrix_custom_frame",O_WRONLY);
    razer_update_fd[1] = open("/sys/bus/hid/drivers/razermouse/0003:1532:0046.0005/matrix_effect_custom",O_WRONLY);


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

        //FireflyEffect[sizeof(FireflyEffect)-1] = '\n';

        write(razer_custom_fd[0], &FireflyEffect, sizeof(FireflyEffect));
        write(razer_update_fd[0], &FireflyEffect, 1);

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

        write(razer_custom_fd[1], &MambaTEEffect, sizeof(MambaTEEffect));
        write(razer_update_fd[1], &MambaTEEffect, 1);
        return true;
    }
}
