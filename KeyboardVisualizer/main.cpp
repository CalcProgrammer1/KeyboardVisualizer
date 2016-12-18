#include "KeyboardVisDlg.h"
#include <QApplication>
#include "Visualizer.h"
#include "VisualizerDefines.h"

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>

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

    if (strcmp(argument, "ledstrip") == 0)
    {
        vis.AddLEDStrip(value);
    }

    if (strcmp(argument, "xmas") == 0)
    {
        vis.AddLEDStripXmas(value);
    }

    if (strcmp(argument, "server") == 0)
    {
        vis.InitServer(value);
    }

    if (strcmp(argument, "client") == 0)
    {
        vis.InitClient(value);
    }
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
                char * argument;
                char * value;

                value = (char *)line.c_str();

                argument = strtok_r(value, "=", &value);

                parse_argument_string(argument, value);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    startminimized = FALSE;

    //Initialize Visualizer
    vis.Initialize();

    //Get file path in executable directory
    char filename[2048];
    //GetModuleFileName(NULL, filename, 2048);
    char arg1[64];
    snprintf(arg1, 64, "/proc/%d/exe", getpid());
    readlink(arg1, filename, 1024);
    strcpy(filename, std::string(filename).substr(0, std::string(filename).find_last_of("\\/")).c_str());
    //strcat(filename, "\\settings.txt");
    strcat(filename, "/settings.txt");
    parse_settings_file(filename);

    //Parse Command Line
    //if (!parse_command_line(lpCmdLine))
    //{
    //    return 0;
    //}

    //Start Visualizer Threads
    vis.StartThread();

    //Create Dialog
    Ui::KeyboardVisDlg dlg;
    dlg.StartMinimized(startminimized);
    dlg.SetVisualizer(&vis);
    dlg.show();

    return a.exec();
}
