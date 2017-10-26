/*---------------------------------------------------------*\
|  Entry Point Code for Keyboard Visualizer Application     |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "KeyboardVisDlg.h"
#include "..\KeyboardVisualizerCommon\Visualizer.h"

#include <fstream>
#include <iostream>
#include <string>

CWinApp app;
Visualizer vis;
static boolean startminimized;


void parse_argument_string(char * argument, char * value)
{
    if (strcmp(argument, "startminimized") == 0)
    {
        startminimized = TRUE;
    }

    if (strcmp(argument, "amplitude") == 0)
    {
        vis.amplitude = atoi(value);
    }

    if (strcmp(argument, "bkgd_bright") == 0)
    {
        vis.bkgd_bright = atoi(value);
    }

    if (strcmp(argument, "avg_size") == 0)
    {
        vis.avg_size = atoi(value);
        if (vis.avg_size < 1)
        {
            vis.avg_size = 1;
        }
        else if (vis.avg_size > 128)
        {
            vis.avg_size = 128;
        }
    }

    if (strcmp(argument, "decay") == 0)
    {
        vis.decay = atoi(value);
    }

    if (strcmp(argument, "delay") == 0)
    {
        vis.delay = atoi(value);
    }

    if (strcmp(argument, "nrml_ofst") == 0)
    {
        vis.nrml_ofst = strtod(value, NULL);
    }

    if (strcmp(argument, "nrml_scl") == 0)
    {
        vis.nrml_scl = strtod(value, NULL);
    }

    if (strcmp(argument, "fltr_const") == 0)
    {
        vis.filter_constant = strtod(value, NULL);
        if (vis.filter_constant > 1.0f)
        {
            vis.filter_constant = 1.0f;
        }
        else if (vis.filter_constant < 0.0f)
        {
            vis.filter_constant = 0.0f;
        }
    }

    if (strcmp(argument, "anim_speed") == 0)
    {
        vis.anim_speed = strtod(value, NULL);
    }

    if (strcmp(argument, "window_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= 4))
        {
            vis.window_mode = atoi(value);
        }
    }

    if (strcmp(argument, "bkgd_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= VISUALIZER_NUM_PATTERNS))
        {
            vis.bkgd_mode = atoi(value);
        }
    }

    if (strcmp(argument, "frgd_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= VISUALIZER_NUM_PATTERNS))
        {
            vis.frgd_mode = atoi(value);
        }
    }

    if (strcmp(argument, "single_color_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= VISUALIZER_NUM_SINGLE_COLOR))
        {
            vis.single_color_mode = atoi(value);
        }
    }

    if (strcmp(argument, "avg_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= 1))
        {
            vis.avg_mode = atoi(value);
        }
    }

    if (strcmp(argument, "ledmatrix") == 0)
    {
        vis.BeginLEDMatrix(atoi(value));
    }

    if (strcmp(argument, "ledsections") == 0)
    {
        vis.LEDStripSections(atoi(value));
    }

    if (strcmp(argument, "ledmirrorx") == 0)
    {
        vis.LEDMirrorX();
    }

    if (strcmp(argument, "ledmirrory") == 0)
    {
        vis.LEDMirrorY();
    }

    if (strcmp(argument, "ledsinglecolor") == 0)
    {
        vis.LEDSingleColor();
    }

    if (strcmp(argument, "ledrotatex") == 0)
    {
        vis.LEDRotateX(atoi(value));
    }

    if (strcmp(argument, "ledstrip") == 0)
    {
        vis.AddLEDStrip(value);
    }

    if (strcmp(argument, "xmas") == 0)
    {
        vis.AddLEDStripXmas(value);
    }

    if (strcmp(argument, "hueplus") == 0)
    {
        vis.AddLEDStripHuePlus(value);
    }

    if (strcmp(argument, "server") == 0)
    {
        vis.InitServer(value);
    }

    if (strcmp(argument, "client") == 0)
    {
        vis.InitClient(value);
    }

    if (strcmp(argument, "reactive_bkgd") == 0)
    {
        vis.reactive_bkgd = atoi(value);

        if ((vis.silent_bkgd == TRUE) && (vis.reactive_bkgd == TRUE))
        {
            vis.silent_bkgd = FALSE;
        }
    }

    if (strcmp(argument, "silent_bkgd") == 0)
    {
        vis.silent_bkgd = atoi(value);

        if ((vis.silent_bkgd == TRUE) && (vis.reactive_bkgd == TRUE))
        {
            vis.reactive_bkgd = FALSE;
        }
    }

    if (strcmp(argument, "background_timeout") == 0)
    {
        vis.background_timeout = atoi(value);
        vis.background_timer = 0;
    }

    if (strcmp(argument, "audio_device_idx") == 0)
    {
        vis.audio_device_idx = atoi(value);
        vis.ChangeAudioDevice();
    }

    if (strcmp(argument, "razer_use_keyboard_generic_effect") == 0)
    {
        vis.SetDeviceProperty(argument, value);
    }

    if (strcmp(argument, "razer_use_headset_custom_effect") == 0)
    {
        vis.SetDeviceProperty(argument, value);
    }

    if (strcmp(argument, "razer_use_chromalink_single_color") == 0)
    {
        vis.SetDeviceProperty(argument, value);
    }

    if (strcmp(argument, "razer_disable_chromalink") == 0)
    {
        vis.SetDeviceProperty(argument, value);
    }

    if (strcmp(argument, "razer_chroma_box_mode") == 0)
    {
        vis.SetDeviceProperty(argument, value);
    }
}

boolean parse_command_line(char * command_line)
{
    LPTSTR  argument;
    LPTSTR  value;
    LPTSTR  next;
    value = strtok_s(command_line, " ", &next);

    while (value != NULL)
    {
        argument = strtok_s(value, "=", &value);

        if (strcmp(argument, "help") == 0)
        {
            printf("\r\n");
            printf("Available command line commands:\r\n");
            printf("    help              - Display this message and quit\r\n");
            //printf("    nogui             - Run visualizer with no GUI\r\n");
            printf("    startminimized    - Start in system tray\r\n");
            printf("\r\n");
            printf("Available command line arguments:\r\n");
            printf("    Usage: argument1=value1 argument2=value2 ...\r\n");
            printf("\r\n");
            printf("    amplitude         - Adjust the amplitude of the visualizer\r\n");
            printf("    bkgd_bright       - Adjust the background brightness\r\n");
            printf("    avg_size          - Number of points to average\r\n");
            printf("    decay             - Percentage of value to decay every step\r\n");
            printf("    delay             - Milliseconds between each device update\r\n");
            printf("    nrml_ofst         - Normalization offset, floating point value\r\n");
            printf("    nrml_scl          - Normalization scale, floating point value\r\n");
            printf("    fltr_const        - Low pass filter constant, floating point value 0-1\r\n");
            printf("    window_mode       - FFT windowing mode selector, values are:\r\n");
            printf("                      - 0:  No windowing\r\n");
            printf("                      - 1:  Hanning window\r\n");
            printf("                      - 2:  Hamming window\r\n");
            printf("                      - 3:  Blackman window\r\n");
            printf("    bkgd_mode         - Background mode, values are:\r\n");
            for (int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
            {
                if (i < 10)
                {
                    printf("                      - %d  %s\r\n", i, visualizer_pattern_labels[i]);
                }
                else
                {
                    printf("                      - %d %s\r\n", i, visualizer_pattern_labels[i]);
                }
            }
            printf("    frgd_mode         - Foreground mode, values are:\r\n");
            for (int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
            {
                if (i < 10)
                {
                    printf("                      - %d  %s\r\n", i, visualizer_pattern_labels[i]);
                }
                else
                {
                    printf("                      - %d %s\r\n", i, visualizer_pattern_labels[i]);
                }
            }
            printf("    single_color_mode - Single color mode, values are:\r\n");
            for (int i = 0; i < VISUALIZER_NUM_SINGLE_COLOR; i++)
            {
                if (i < 10)
                {
                    printf("                      - %d  %s\r\n", i, visualizer_single_color_labels[i]);
                }
                else
                {
                    printf("                      - %d %s\r\n", i, visualizer_single_color_labels[i]);
                }
            }
            printf("    avg_mode          - Visualizer averaging mode\r\n");
            printf("                      - 0:  Binning\r\n");
            printf("                      - 1:  Low-pass filtering\r\n");
            printf("    anim_speed        - Animation Speed (percent)\r\n");
            printf("    server            - Configure this instance as a server for synchronization\r\n");
            printf("                      -  Takes what port to serve on as argument, i.e. server=1234\r\n");
            printf("    client            - Configure this instance as a client for synchronization\r\n");
            printf("                      -  Takes the IP/hostname of the server and port as arguments,\r\n");
            printf("                      -  i.e. client=192.168.1.100,1234\r\n");
            printf("    ledstrip          - LED config strings :\r\n");
            printf("                      - Serial : ledstrip=port,baud,num_leds\r\n");
            printf("                      - (ex.ledstrip=COM1,115200,30)\r\n");
            printf("                      - UDP : ledstrip=udp:client,port,num_leds\r\n");
            printf("                      - (ex.ledstrip=udp:192.168.1.5,1234,30)\r\n");
            printf("    ledmatrix         - Add this line before ledstrip lines to create a matrix of LEDs\r\n");
            printf("                      -  Takes the number of strips to create a matrix from, treated as rows\r\n");
            printf("    xmas              - COM port, ex. xmas=COM2\r\n");
            printf("    hueplus           - HUE+ config:\r\n");
            printf("                      - hueplus=port,channel,num_leds\r\n");
            printf("                      - channel: 0 -> both channels, 1 -> channel 1, 2 -> channel 2\r\n");
            printf("                      - num_leds: Fans * 8 ex. 3 Fans -> 24\r\n");
            printf("                      - Important for Fans: If you have connected fans on both channels only count the fans on the channel with the most fans\r\n");
            printf("                                            ex. 3 Fans on Ch. 1 4 Fans on CH. 2: num_leds 32 for the 4 Fans\r\n");
            printf("                                            For best Visualizer results don`t connect on one channel 3 fans more than on the other channel\r\n");
            printf("                      - (ex. hueplus=COM4,1,24\r\n");
            printf("    razer_use_keyboard_generic_effect\r\n");
            printf("                      - Use generic effect type in the Razer SDK, allows the use of SDK-unsupported keyboards at the expense of scaling\r\n");
            printf("    razer_use_headset_custom_effect\r\n");
            printf("                      - Use custom effect for headsets, allows smooth color transitions on the Kraken V2 but disables the Kraken V1\r\n");
            printf("    razer_use_chromalink_single_color\r\n");
            printf("                      - Use the single color option for the Chroma Link SDK outputs instead of bar graph option\r\n");
            printf("    razer_disable_chromalink\r\n");
            printf("                      - Disable the Chroma Link SDK, works around a bug in the current Chroma SDK where the Chroma HDK commands are overridden\r\n");
            printf("    razer_chroma_box_mode\r\n");
            printf("                      - razer_chroma_box_mode=mode\r\n");
            printf("                      - 0:  Matrix (use 4 strips for best effect)\r\n");
            printf("                      - 1:  Bar graph per single strip\r\n");
            printf("                      - 2:  Bar graph stretched across 2 strips each\r\n");
            printf("                      - 3:  Bar graph stretched across all 4 strips\r\n");
            return FALSE;
        }

        parse_argument_string(argument, value);

        value = strtok_s(next, " ", &next);
    }
    return TRUE;
}


void parse_settings_file(char * filename)
{
    std::ifstream infile;

    //Open settings file
    infile.open(filename);

    if (infile.good())
    {
        for (std::string line; std::getline(infile, line); )
        {
            if (line == "")
            {
                continue;
            }
            if ((line[0] != ';') && (line[0] != '#') && (line[0] != '/'))
            {
                LPSTR argument;
                LPSTR value;

                value = (LPSTR)line.c_str();

                argument = strtok_s(value, "=", &value);

                parse_argument_string(argument, value);
            }
        }
    }
}


int APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
    AttachConsole(-1);
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

	afxCurrentInstanceHandle = hInst;
	afxCurrentResourceHandle = hInst;
	app.m_hInstance = hInst;

    startminimized = FALSE;

    //Initialize Visualizer
    vis.Initialize();

    //Parse Settings File in Executable Directory
    char filename[2048];
    GetModuleFileName(NULL, filename, 2048);
    strcpy(filename, std::string(filename).substr(0, std::string(filename).find_last_of("\\/")).c_str());
    strcat(filename, "\\settings.txt");
    parse_settings_file(filename);
    
    //Parse Command Line
    if (!parse_command_line(lpCmdLine))
    {
        return 0;
    }

    //Start Visualizer Threads
    vis.StartThread();

    //Create Dialog
	KeyboardVisDlg dlg;
    dlg.StartMinimized(startminimized);
	dlg.SetVisualizer(&vis);
	dlg.DoModal();

	return 0;
}