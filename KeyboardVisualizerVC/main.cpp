#include "stdafx.h"
#include "resource.h"
#include "KeyboardVisDlg.h"
#include "Visualizer.h"

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

    if (strcmp(argument, "window_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= 4))
        {
            vis.window_mode = atoi(value);
        }
    }

    if (strcmp(argument, "bkgd_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= 12))
        {
            vis.bkgd_mode = atoi(value);
        }
    }

    if (strcmp(argument, "frgd_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= 13))
        {
            vis.frgd_mode = atoi(value);
        }
    }

    if (strcmp(argument, "single_color_mode") == 0)
    {
        if ((atoi(value) >= 0) && (atoi(value) <= 10))
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

    if (strcmp(argument, "ledstrip") == 0)
    {
        vis.AddLEDStrip(value);
    }

    if (strcmp(argument, "xmas") == 0)
    {
        vis.AddLEDStripXmas(value);
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
            printf("    window_mode       - FFT windowing mode selector, values are:\r\n");
            printf("                      - 0:  No windowing\r\n");
            printf("                      - 1:  Hanning window\r\n");
            printf("                      - 2:  Hamming window\r\n");
            printf("                      - 3:  Blackman window\r\n");
            printf("    bkgd_mode         - Background mode, values are:\r\n");
            printf("                      - 0:  None (black)\r\n");
            printf("                      - 1:  Original (sinusoidal rainbow effect)\r\n");
            printf("                      - 2:  Rainbow (HSV rainbow effect)\r\n");
            printf("                      - 3:  Color Wheel\r\n");
            printf("                      - 4:  Follow foreground effect\r\n");
            printf("                      - 5:  White\r\n");
            printf("                      - 6:  Red\r\n");
            printf("                      - 7:  Orange\r\n");
            printf("                      - 8:  Yellow\r\n");
            printf("                      - 9:  Green\r\n");
            printf("                      - 10: Cyan\r\n");
            printf("                      - 11: Blue\r\n");
            printf("                      - 12: Purple\r\n");
            printf("    frgd_mode         - Foreground mode, values are:\r\n");
            printf("                      - 0:  White\r\n");
            printf("                      - 1:  Red\r\n");
            printf("                      - 2:  Orange\r\n");
            printf("                      - 3:  Yellow\r\n");
            printf("                      - 4:  Green\r\n");
            printf("                      - 5:  Cyan\r\n");
            printf("                      - 6:  Blue\r\n");
            printf("                      - 7:  Purple\r\n");
            printf("                      - 8:  Green/Yellow/Red\r\n");
            printf("                      - 9:  Green/White/Red\r\n");
            printf("                      - 10: White/Cyan/Blue\r\n");
            printf("                      - 11: Red/White/Blue\r\n");
            printf("                      - 12: Rainbow\r\n");
            printf("                      - 13: Rainbow Inverse\r\n");
            printf("    single_color_mode - Single color mode, values are:\r\n");
            printf("                      - 0:  None (black)\r\n");
            printf("                      - 1:  Follow foreground effect\r\n");
            printf("                      - 2:  Follow background effect\r\n");
            printf("                      - 3:  White\r\n");
            printf("                      - 4:  Red\r\n");
            printf("                      - 5:  Orange\r\n");
            printf("                      - 6:  Yellow\r\n");
            printf("                      - 7:  Green\r\n");
            printf("                      - 8:  Cyan\r\n");
            printf("                      - 9:  Blue\r\n");
            printf("                      - 10: Purple\r\n");
            printf("    avg_mode          - Visualizer averaging mode\r\n");
            printf("                      - 0:  Binning\r\n");
            printf("                      - 1:  Low-pass filtering\r\n");
            printf("    ledstrip          - COM port, ex. ledstrip=COM1\r\n");
            printf("    xmas              - COM port, ex. xmas=COM2\r\n");
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