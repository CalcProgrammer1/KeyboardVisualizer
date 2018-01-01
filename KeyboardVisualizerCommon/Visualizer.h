/*---------------------------------------------------------*\
|  Definitions for Keyboard Visualizer                      |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <math.h>
#include <fstream>
#include <string>
#include <vector>

//Project includes
#include "VisualizerDefines.h"
#include "chuck_fft.h"
#include "hsv.h"
#include "net_port.h"

//If building on Windows, use WASAPI
#ifdef WIN32
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>

//If not building on Windows, use OpenAL
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

typedef struct
{
    int amplitude;
    int avg_mode;
    int avg_size;
    int window_mode;
    int decay;
    int delay;
    float anim_speed;
    int bkgd_bright;
    int bkgd_mode;
    int single_color_mode;
    float nrml_ofst;
    float nrml_scl;
    float filter_constant;
    int frgd_mode;
    bool reactive_bkgd;
    bool silent_bkgd;
    unsigned int background_timeout;
} settings_pkt_type;

typedef int net_mode;
enum
{
    NET_MODE_DISABLED,
    NET_MODE_CLIENT,
    NET_MODE_SERVER
};

class Visualizer
{
public:
    Visualizer();

    //Initializes the visualizer
    void Initialize();

    //Initialize the audio devices list
    void InitAudioDeviceList();

    //Function to start thread
    void StartThread();

    //Visualizer Thread
    void VisThread();

    //Network Threads
    void NetConnectThread();
    void NetUpdateThread();

#ifdef WIN32
    //Update threads for devices supported only under Windows
    void CmKeyboardUpdateThread();
    void LogitechSDKUpdateThread();
	void AuraSDKUpdateThread();
#else

#endif
    //Update threads for devices supported on both Windows and Linux
    void RazerChromaUpdateThread();
    void CorsairKeyboardUpdateThread();
    void SteelSeriesKeyboardUpdateThread();
    void MSIKeyboardUpdateThread();
    void PoseidonZRGBKeyboardUpdateThread();
    void LEDStripUpdateThread();

    //Called when settings changed
    void OnSettingsChanged();

    //Send Settings to network clients
    void SendSettings();

    //Initialize Server
    void InitServer(char * serverstring);

    //Initialize CLient
    void InitClient(char * clientstring);

    //Change Audio Device
    void ChangeAudioDevice();

    //Update function
    void Update();

    //Draw Pattern
    void DrawPattern(VISUALIZER_PATTERN pattern, int bright, vis_pixels *pixels);

    //Shut Down
    void Shutdown();

    //Add LED strip
    void BeginLEDMatrix(int size);
    void AddLEDStrip(char* ledstring);
    void AddLEDStripXmas(char * ledstring);
    void AddLEDStripHuePlus(char * ledstring);

    //Function to set custom properties for devices
    void SetDeviceProperty(char * devprop, char * argument);

    //Save Settings File
    void SaveSettings();

    //Compute normalization line
    void SetNormalization(float offset, float scale);

    //Calculated FFT
    float fft[256];

    //Amplitude of input waveform
    int amplitude;
    int avg_mode;
    int avg_size;
    int window_mode;
    int decay;
    int delay;
    unsigned int audio_device_idx;

    //Flag to update UI
    bool update_ui;

    //Visualizer Background
    vis_pixels pixels_bg;

    //Visualizer Foreground
    vis_pixels pixels_fg;

    //Visualizer Image 1
    vis_pixels pixels_vs1;

    //Visualizer Image 2
    vis_pixels pixels_vs2;

    //Visualizer Output Image Pointer
    vis_pixels *pixels_out;

    //Visualizer Render Image Pointer
    vis_pixels *pixels_render;

    //Background Variables
    float anim_speed;
    int bkgd_bright;
    int bkgd_mode;
    bool reactive_bkgd;
    bool silent_bkgd;
    unsigned int background_timeout;
    unsigned int background_timer;

    void LEDStripSections(int sections);
    void LEDMirrorX();
    void LEDMirrorY();
    void LEDSingleColor();
    void LEDRotateX(int rotate);

    //Single Color Mode
    int single_color_mode;

    //Normalization Offset and Scale
    float nrml_ofst;
    float nrml_scl;

    //Filter Constant
    float filter_constant;

    //Foreground Variables
    int frgd_mode;

    //Audio Device List
    std::vector<char *> audio_devices;

private:
#ifdef WIN32
    //WASAPI objects if building for Windows
    IMMDeviceEnumerator *pMMDeviceEnumerator;
    std::vector<IMMDevice *> pMMDevices;
    std::vector<bool> isCapture;
    IMMDeviceCollection *pMMDeviceCollection;
    IAudioClient *pAudioClient;
    IAudioCaptureClient *pAudioCaptureClient;
    WAVEFORMATEX *waveformat;
#else
    //Audio Device Pointer
    ALCdevice *device;
#endif

    //Background Step
    float bkgd_step;

    //Network Port Pointer
    net_port * port;

    //Network Mode
    net_mode netmode;

    //FFT Variables
    float win_hanning[256];
    float win_hamming[256];
    float win_blackman[256];
    float fft_tmp[512];

    //Settings Changed Flag
    bool settings_changed;

    //Application Running Flag
    bool running;

    //Shutting Down Flag
    bool shutdown_flag;

    //Audio Sample Buffer
    unsigned char buffer[256];

    void DrawSingleColorForeground(float amplitude, vis_pixels *fg_pixels, vis_pixels *out_pixels);
};

#endif
