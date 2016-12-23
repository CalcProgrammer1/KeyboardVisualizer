/*---------------------------------------------------------*\
|  Processing Code for Keyboard Visualizer                  |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#include "Visualizer.h"
#include "string.h"

//Thread functions have different types in Windows and Linux
#ifdef WIN32
#define THREAD static void
#else
#define THREAD static void*
#endif

//Include pthread and Unix standard libraries if not building for Windows
#ifndef WIN32
#include "pthread.h"
#include "unistd.h"
#endif

#include "RazerChromaLinux.h"
//#include "CorsairCUE.h"
//#include "CorsairCKBLinux.h"
//#include "CmKeyboard.h"
//#include "SteelSeriesGameSense.h"
//#include "MSIKeyboard.h"
#include "LEDStrip.h"

//File includes
#include <fstream>

RazerChroma rkb;
//CorsairCKB ckb;
//CorsairCUE ckb;
//CmKeyboard cmkb;
//SteelSeriesGameSense skb;
//MSIKeyboard mkb;
std::vector<LEDStrip *> str;
std::vector<LEDStrip *> xmas;

//WASAPI objects if building for Windows
#ifdef WIN32
IMMDeviceEnumerator *pMMDeviceEnumerator;
IMMDevice *pMMDevice;
IMMDeviceCollection *pMMDeviceCollection;
IAudioClient *pAudioClient;
IAudioCaptureClient *pAudioCaptureClient;
WAVEFORMATEX *waveformat;
#endif

int single_color_timeout;
float fft_nrml[256];

//Thread starting static function
THREAD thread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->VisThread();
}

THREAD netconthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->NetConnectThread();
}
THREAD netupdthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->NetUpdateThread();
}

THREAD rkbthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->RazerChromaUpdateThread();
}

THREAD ckbthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->CorsairKeyboardUpdateThread();
}

/*
THREAD skbthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->SteelSeriesKeyboardUpdateThread();
}

THREAD cmkbthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->CmKeyboardUpdateThread();
}

THREAD mkbthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->MSIKeyboardUpdateThread();
}
*/
THREAD lsthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->LEDStripUpdateThread();
}

Visualizer::Visualizer()
{

}

void Visualizer::AddLEDStrip(char* ledstring)
{
    //Scan through already registered LED strips and
    //verify that the port name is not already in use
    for (int i = 0; i < str.size(); i++)
    {
        if (strcmp(str[i]->GetLEDString(), ledstring) == 0)
        {
            return;
        }
    }
    for (int i = 0; i < xmas.size(); i++)
    {
        if (strcmp(xmas[i]->GetLEDString(), ledstring) == 0)
        {
            return;
        }
    }

    LEDStrip *newstr = new LEDStrip();
    newstr->Initialize(ledstring);
    str.push_back(newstr);
}

void Visualizer::AddLEDStripXmas(char* ledstring)
{
    //Scan through already registered LED strips and
    //verify that the port name is not already in use
    for (int i = 0; i < str.size(); i++)
    {
        if (strcmp(str[i]->GetLEDString(), ledstring) == 0)
        {
            return;
        }
    }
    for (int i = 0; i < xmas.size(); i++)
    {
        if (strcmp(xmas[i]->GetLEDString(), ledstring) == 0)
        {
            return;
        }
    }

    LEDStrip *newstr = new LEDStrip();
    newstr->Initialize(ledstring);
    xmas.push_back(newstr);
}

void Visualizer::Initialize()
{
#ifdef WIN32
    //If using WASAPI, start WASAPI loopback capture device
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pMMDeviceEnumerator);
    pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pMMDevice);
    pMMDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);

    pAudioClient->GetMixFormat(&waveformat);

    pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, waveformat, 0);
    pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);

    pAudioClient->Start();

#else
    //If using OpenAL, start OpenAL capture on default capture device
    ALCchar* devices;
    devices = (ALCchar *) alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
    device = alcCaptureOpenDevice(devices, 10000, AL_FORMAT_MONO8, 2048);
    alcCaptureStart(device);
#endif

    rkb.Initialize();
    //ckb.Initialize();
    //cmkb.Initialize();
    //skb.Initialize();
    //mkb.Initialize();

    netmode              = NET_MODE_DISABLED;
    single_color_timeout = 0;

    amplitude            = 100;
    anim_speed           = 100.0f;
    avg_mode             = 0;
    avg_size             = 8;
    bkgd_step            = 0;
    bkgd_bright          = 100;
    bkgd_mode            = VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL;
    delay                = 50;
    window_mode          = 1;
    decay                = 80;
    frgd_mode            = VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED;
    single_color_mode    = VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND;

    hanning(win_hanning, 256);
    hamming(win_hamming, 256);
    blackman(win_blackman, 256);

    nrml_ofst            = 0.04f;
    nrml_scl             = 0.5f;

    pixels_render = &pixels_vs1;
    pixels_out = &pixels_vs2;

    SetNormalization(nrml_ofst, nrml_scl);
}

void Visualizer::InitClient(char * clientstring)
{
    if (netmode == NET_MODE_DISABLED)
    {
        LPSTR client_name;
        LPSTR port_name;
        client_name = strtok_s(clientstring, ",", &port_name);

        netmode = NET_MODE_CLIENT;
        port = new net_port();
        port->tcp_client(client_name, port_name);
    }
}

void Visualizer::InitServer(char * serverstring)
{
    if (netmode == NET_MODE_DISABLED)
    {
        netmode = NET_MODE_SERVER;
        port = new net_port();
        port->tcp_server(serverstring);
    }
}

void Visualizer::SaveSettings()
{
    std::ofstream outfile;
    char filename[2048];
    char out_str[1024];

    //Get file path in executable directory
#ifdef WIN32
    GetModuleFileName(NULL, filename, 2048);
    strcpy(filename, std::string(filename).substr(0, std::string(filename).find_last_of("\\/")).c_str());
    strcat(filename, "\\settings.txt");
#else
    char arg1[64];
    snprintf(arg1, 64, "/proc/%d/exe", getpid());
    readlink(arg1, filename, 1024);
    strcpy(filename, std::string(filename).substr(0, std::string(filename).find_last_of("\\/")).c_str());
    strcat(filename, "/settings.txt");
#endif

    //Open settings file
    outfile.open(filename);

    //Save Amplitude
    snprintf(out_str, 1024, "amplitude=%d\r\n", amplitude);
    outfile.write(out_str, strlen(out_str));

    //Save Background Brightness
    snprintf(out_str, 1024, "bkgd_bright=%d\r\n", bkgd_bright);
    outfile.write(out_str, strlen(out_str));

    //Save Average Size
    snprintf(out_str, 1024, "avg_size=%d\r\n", avg_size);
    outfile.write(out_str, strlen(out_str));

    //Save Decay
    snprintf(out_str, 1024, "decay=%d\r\n", decay);
    outfile.write(out_str, strlen(out_str));

    //Save Delay
    snprintf(out_str, 1024, "delay=%d\r\n", delay);
    outfile.write(out_str, strlen(out_str));

    //Save Normalization Offset
    snprintf(out_str, 1024, "nrml_ofst=%f\r\n", nrml_ofst);
    outfile.write(out_str, strlen(out_str));

    //Save Normalization Scale
    snprintf(out_str, 1024, "nrml_scl=%f\r\n", nrml_scl);
    outfile.write(out_str, strlen(out_str));

    //Save Window Mode
    snprintf(out_str, 1024, "window_mode=%d\r\n", window_mode);
    outfile.write(out_str, strlen(out_str));

    //Save Background Mode
    snprintf(out_str, 1024, "bkgd_mode=%d\r\n", bkgd_mode);
    outfile.write(out_str, strlen(out_str));

    //Save Foreground Mode
    snprintf(out_str, 1024, "frgd_mode=%d\r\n", frgd_mode);
    outfile.write(out_str, strlen(out_str));

    //Save Single Color Mode
    snprintf(out_str, 1024, "single_color_mode=%d\r\n", single_color_mode);
    outfile.write(out_str, strlen(out_str));

    //Save Averaging Mode
    snprintf(out_str, 1024, "avg_mode=%d\r\n", avg_mode);
    outfile.write(out_str, strlen(out_str));

    //Save Animation Speed
    snprintf(out_str, 1024, "anim_speed=%f\r\n", anim_speed);
    outfile.write(out_str, strlen(out_str));

    //Save LED Strip Configurations
    for (int i = 0; i < str.size(); i++)
    {
        //Save LED Strip Configuration
        snprintf(out_str, 1024, "ledstrip=%s\r\n", str[i]->GetLEDString());
        outfile.write(out_str, strlen(out_str));
    }

    //Save Xmas Strip Configurations
    for (int i = 0; i < xmas.size(); i++)
    {
        //Save Xmas Strip Configuration
        snprintf(out_str, 1024, "xmas=%s\r\n", xmas[i]->GetLEDString());
        outfile.write(out_str, strlen(out_str));
    }

    //Close Output File
    outfile.close();
}

void Visualizer::SetNormalization(float offset, float scale)
{
    for (int i = 0; i < 256; i++)
    {
        fft[i] = 0.0f;
        fft_nrml[i] = offset + (scale * (i / 256.0f));
    }
}

void Visualizer::OnSettingsChanged()
{
    if (netmode == NET_MODE_SERVER)
    {
        settings_pkt_type settings;
        settings.amplitude = amplitude;
        settings.avg_mode = avg_mode;
        settings.avg_size = avg_size;
        settings.window_mode = window_mode;
        settings.decay = decay;
        settings.delay = delay;
        settings.anim_speed = anim_speed;
        settings.bkgd_bright = bkgd_bright;
        settings.bkgd_mode = bkgd_mode;
        settings.single_color_mode = single_color_mode;
        settings.nrml_ofst = nrml_ofst;
        settings.nrml_scl = nrml_scl;
        settings.frgd_mode = frgd_mode;
        settings.bkgd_step = bkgd_step;

        port->tcp_write((char *)&settings, sizeof(settings));
    }
}

void Visualizer::Update()
{
    static float input_wave[512];
    float fft_tmp[512];

    unsigned int buffer_pos = 0;

    for (int i = 0; i < 256; i++)
    {
        //Clear the buffers
        fft_tmp[i] = 0;

        //Decay previous values
        fft[i] = fft[i] * (((float)decay) / 100.0f);
    }

#ifdef WIN32
    unsigned int nextPacketSize = 1;
    unsigned int flags;

    while(nextPacketSize > 0 )
    {
        float *buf;
        pAudioCaptureClient->GetBuffer((BYTE**)&buf, &nextPacketSize, (DWORD *)&flags, NULL, NULL);

        for (int i = 0; i < nextPacketSize; i+=4)
        {
            for (int j = 0; j < 255; j++)
            {
                input_wave[2 * j] = input_wave[2 * (j + 1)];
                input_wave[(2 * j) + 1] = input_wave[2 * j];
            }
            input_wave[510] = buf[i] * 2.0f * amplitude;
            input_wave[511] = input_wave[510];
        }

        buffer_pos += nextPacketSize/4;
        pAudioCaptureClient->ReleaseBuffer(nextPacketSize);
    }

    memcpy(fft_tmp, input_wave, sizeof(input_wave));

#else
    //Only update FFT if there are at least 256 samples in the sample buffer
    int samples;

    do
    {
        alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 1, &samples);
        Sleep(1);
    } while (samples < 512);

    //Capture 256 audio samples
    alcCaptureSamples(device, (ALCvoid *)buffer, 256);

    //Scale the input into the FFT processing array
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] = (buffer[i / 2] - 128.0f) * (amplitude / 128.0f);
    }
#endif

    //Apply selected window
    switch (window_mode)
    {
    case 0:
        break;

    case 1:
        apply_window(fft_tmp, win_hanning, 256);
        break;

    case 2:
        apply_window(fft_tmp, win_hamming, 256);
        break;

    case 3:
        apply_window(fft_tmp, win_blackman, 256);
        break;

    default:
        break;
    }

    //Run the FFT calculation
    rfft(fft_tmp, 256, 1);

    fft_tmp[0] = fft_tmp[2];

    apply_window(fft_tmp, fft_nrml, 256);

    //Compute FFT magnitude
    for (int i = 0; i < 128; i += 2)
    {
        float fftmag;

        //Compute magnitude from real and imaginary components of FFT and apply simple LPF
        fftmag = (float)sqrt((fft_tmp[i] * fft_tmp[i]) + (fft_tmp[i + 1] * fft_tmp[i + 1]));

        //Apply a slight logarithmic filter to minimize noise from very low amplitude frequencies
        fftmag = ( 0.5f * log10(1.1f * fftmag) ) + ( 0.9f * fftmag );

        //Limit FFT magnitude to 1.0
        if (fftmag > 1.0f)
        {
            fftmag = 1.0f;
        }

        //Update to new values only if greater than previous values
        if (fftmag > fft[i*2])
        {
            fft[i*2] = fftmag;;
        }

        //Prevent from going negative
        if (fft[i*2] < 0.0f)
        {
            fft[i*2] = 0.0f;
        }

        //Set odd indexes to match their corresponding even index, as the FFT input array uses two indices for one value (real+imaginary)
        fft[(i * 2) + 1] = fft[i * 2];
        fft[(i * 2) + 2] = fft[i * 2];
        fft[(i * 2) + 3] = fft[i * 2];
    }

    if (avg_mode == 0)
    {
        //Apply averaging over given number of values
        int k;
        float sum1 = 0;
        float sum2 = 0;
        for (k = 0; k < avg_size; k++)
        {
            sum1 += fft[k];
            sum2 += fft[255 - k];
        }
        //Compute averages for end bars
        sum1 = sum1 / k;
        sum2 = sum2 / k;
        for (k = 0; k < avg_size; k++)
        {
            fft[k] = sum1;
            fft[255 - k] = sum2;
        }
        for (int i = 0; i < (256 - avg_size); i += avg_size)
        {
            float sum = 0;
            for (int j = 0; j < avg_size; j += 1)
            {
                sum += fft[i + j];
            }

            float avg = sum / avg_size;

            for (int j = 0; j < avg_size; j += 1)
            {
                fft[i + j] = avg;
            }
        }
    }
    else if(avg_mode == 1)
    {
        for (int i = 0; i < avg_size; i++)
        {
            float sum1 = 0;
            float sum2 = 0;
            int j;
            for (j = 0; j <= i + avg_size; j++)
            {
                sum1 += fft[j];
                sum2 += fft[255 - j];
            }
            fft[i] = sum1 / j;
            fft[255 - i] = sum2 / j;
        }
        for (int i = avg_size; i < 256 - avg_size; i++)
        {
            float sum = 0;
            for (int j = 1; j <= avg_size; j++)
            {
                sum += fft[i - j];
                sum += fft[i + j];
            }
            sum += fft[i];

            fft[i] = sum / (2 * avg_size + 1);
        }
    }
}

void Visualizer::StartThread()
{
#ifdef WIN32
    _beginthread(thread, 0, this);
    _beginthread(netconthread, 0, this);
    _beginthread(netupdthread, 0, this);
    _beginthread(rkbthread, 0, this);
    _beginthread(ckbthread, 0, this);
    //_beginthread(cmkbthread, 0, this);
    //_beginthread(skbthread, 0, this);
    //_beginthread(mkbthread, 0, this);
    _beginthread(lsthread, 0, this);

#else
    pthread_t threads[10];
    
    pthread_create(&threads[0], NULL, &thread, this);
    pthread_create(&threads[1], NULL, &netconthread, this);
    pthread_create(&threads[2], NULL, &netupdthread, this);
    pthread_create(&threads[3], NULL, &rkbthread, this);
    pthread_create(&threads[4], NULL, &ckbthread, this);
    pthread_create(&threads[5], NULL, &lsthread, this);
#endif
}

void DrawSolidColor(int bright, COLORREF color, vis_pixels *pixels)
{
    bright = bright * (255.0f / 100.0f);
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = RGB(((bright * GetRValue(color)) / 256), ((bright * GetGValue(color)) / 256), ((bright * GetBValue(color)) / 256));
        }
    }
}

void DrawVerticalBars(int bright, COLORREF * colors, int num_colors, vis_pixels *pixels)
{
    bright = bright * (255.0f / 100.0f);
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int idx = (float)x * ((float)num_colors / 255.0f);
            pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
        }
    }
}

void DrawHorizontalBars(int bright, COLORREF * colors, int num_colors, vis_pixels *pixels)
{
    bright = bright * (255.0f / 100.0f);
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            if (y == ROW_IDX_BAR_GRAPH)
            {
                if (x < 128)
                {
                    int idx = num_colors - ((float)x * ((float)num_colors / 128.0f));
                    if (idx >= num_colors)
                    {
                        idx = num_colors - 1;
                    }
                    pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
                }
                else
                {
                    int idx = ((float)(x - 128) * ((float)num_colors / 128.0f));
                    pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
                }
            }
            else
            {
                int idx = num_colors - ((float)y * ((float)num_colors / 63.0f));
                pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
            }
        }
    }
}

void DrawRainbowSinusoidal(int bright, int bkgd_step, vis_pixels *pixels)
{
    bright = bright * (255.0f / 100.0f);
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int red = 127 * (sin((((((int)(x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f)) + 1);
            int grn = 127 * (sin((((((int)(x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) - (6.28f / 3)) + 1);
            int blu = 127 * (sin((((((int)(x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) + (6.28f / 3)) + 1);
            pixels->pixels[y][x] = RGB(((bright * red) / 256), ((bright * grn) / 256), ((bright * blu) / 256));
        }
    }
}

void DrawRainbow(int bright, int bkgd_step, vis_pixels *pixels)
{
    bright = bright * (255.0f / 100.0f);
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int hsv_h = ((bkgd_step + (256 - x)) % 360);
            hsv_t hsv = { hsv_h, 255, bright };
            pixels->pixels[y][x] = hsv2rgb(&hsv);
        }
    }
}

void DrawColorWheel(int bright, int bkgd_step, vis_pixels *pixels)
{
    bright = bright * (255.0f / 100.0f);
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            float hue = bkgd_step + (int)(180 + atan2(y - 32.1, x - 128.1) * (180.0 / 3.14159)) % 360;
            hsv_t hsv2 = { hue, 255, bright };
            pixels->pixels[y][x] = hsv2rgb(&hsv2);
        }
    }
}

void DrawSpectrumCycle(int bright, int bkgd_step, vis_pixels *pixels)
{
    bright = bright * (255.0f / 100.0f);
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            hsv_t hsv2 = { bkgd_step, 255, bright };
            pixels->pixels[y][x] = hsv2rgb(&hsv2);
        }
    }
}

void DrawSingleColorBackground(float amplitude, vis_pixels *bg_pixels, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    for (int x = 0; x < 256; x++)
    {
        int in_color = bg_pixels->pixels[ROW_IDX_SINGLE_COLOR][x];
        int out_color = RGB(((amplitude * GetRValue(in_color))), ((amplitude * GetGValue(in_color))), ((amplitude * GetBValue(in_color))));
        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void Visualizer::DrawSingleColorForeground(float amplitude, vis_pixels *fg_pixels, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    int idx = 64.0f - (amplitude * 62.0f);
    int in_color = fg_pixels->pixels[idx][0];
    int out_color = RGB(((amplitude * GetRValue(in_color))), ((amplitude * GetGValue(in_color))), ((amplitude * GetBValue(in_color))));
    for (int x = 0; x < 256; x++)
    {
        if (frgd_mode >= VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL)
        {
            in_color = fg_pixels->pixels[ROW_IDX_SINGLE_COLOR][x];
            out_color = RGB(((amplitude * GetRValue(in_color))), ((amplitude * GetGValue(in_color))), ((amplitude * GetBValue(in_color))));
        }

        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void DrawSingleColorStatic(float amplitude, COLORREF in_color, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    int out_color = RGB(((amplitude * GetRValue(in_color))), ((amplitude * GetGValue(in_color))), ((amplitude * GetBValue(in_color))));
    for (int x = 0; x < 256; x++)
    {
        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void Visualizer::DrawPattern(VISUALIZER_PATTERN pattern, int bright, vis_pixels *pixels)
{
    switch (pattern)
    {
    case VISUALIZER_PATTERN_SOLID_BLACK:
        DrawSolidColor(bright, 0x00000000, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_WHITE:
        DrawSolidColor(bright, 0x00FFFFFF, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_RED:
        DrawSolidColor(bright, 0x000000FF, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_ORANGE:
        DrawSolidColor(bright, 0x000080FF, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_YELLOW:
        DrawSolidColor(bright, 0x0000FFFF, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_GREEN:
        DrawSolidColor(bright, 0x0000FF00, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_CYAN:
        DrawSolidColor(bright, 0x00FFFF00, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_BLUE:
        DrawSolidColor(bright, 0x00FF0000, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_PURPLE:
        DrawSolidColor(bright, 0x00FF00FF, pixels);
        break;

    case VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED:
        {
        COLORREF colors[] = { 0x0000FF00, 0x0000FFFF, 0x000000FF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_GREEN_WHITE_RED:
        {
        COLORREF colors[] = { 0x0000FF00, 0x00FFFFFF, 0x000000FF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_BLUE_CYAN_WHITE:
        {
        COLORREF colors[] = { 0x00FF0000, 0x00FFFF00, 0x00FFFFFF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RED_WHITE_BLUE:
        {
        COLORREF colors[] = { 0x000000FF, 0x00FFFFFF, 0x00FF0000 };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RAINBOW:
        {
        COLORREF colors[] = { 0x000000FF, 0x0000FFFF, 0x0000FF00, 0x00FFFF00, 0x00FF0000, 0x00FF00FF };
        DrawHorizontalBars(bright, colors, 6, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RAINBOW_INVERSE:
        {
        COLORREF colors[] = { 0x00FF00FF, 0x00FF0000, 0x00FFFF00, 0x0000FF00, 0x0000FFFF, 0x000000FF };
        DrawHorizontalBars(bright, colors, 6, pixels);
        }
        break;

    case VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL:
        DrawRainbowSinusoidal(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_RAINBOW_HSV:
        DrawRainbow(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_COLOR_WHEEL:
        DrawColorWheel(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_SPECTRUM_CYCLE:
        DrawSpectrumCycle(bright, bkgd_step, pixels);
        break;
    }
}

void Visualizer::NetConnectThread()
{
    while (1)
    {
        switch (netmode)
        {
        case NET_MODE_DISABLED:
            return;
            break;

        case NET_MODE_SERVER:
            //Listen for new clients
            port->tcp_server_listen();

            //When a new client connects, send settings
            OnSettingsChanged();
            break;

        case NET_MODE_CLIENT:
            //Try to connect to server
            port->tcp_client_connect();

            //Wait 1 second between tries;
            Sleep(1000);
            break;
        }
    }
}

void Visualizer::NetUpdateThread()
{
    int counter = 0;
    char buf[sizeof(fft)];

    while (1)
    {
        switch (netmode)
        {
        case NET_MODE_DISABLED:
            return;
            break;

        case NET_MODE_SERVER:
            port->tcp_write((char *)fft, sizeof(fft));
            if (counter++ > 60)
            {
                OnSettingsChanged();
            }
            Sleep(20);
            break;

        case NET_MODE_CLIENT:
            if (port->connected)
            {
                if (port->tcp_listen((char *)buf, sizeof(buf)) == sizeof(settings_pkt_type))
                {
                    amplitude = ((settings_pkt_type *)buf)->amplitude;
                    avg_mode = ((settings_pkt_type *)buf)->avg_mode;
                    avg_size = ((settings_pkt_type *)buf)->avg_size;
                    window_mode = ((settings_pkt_type *)buf)->window_mode;
                    decay = ((settings_pkt_type *)buf)->decay;
                    delay = ((settings_pkt_type *)buf)->delay;
                    anim_speed = ((settings_pkt_type *)buf)->anim_speed;
                    bkgd_bright = ((settings_pkt_type *)buf)->bkgd_bright;
                    bkgd_mode = ((settings_pkt_type *)buf)->bkgd_mode;
                    single_color_mode = ((settings_pkt_type *)buf)->single_color_mode;
                    nrml_ofst = ((settings_pkt_type *)buf)->nrml_ofst;
                    nrml_scl = ((settings_pkt_type *)buf)->nrml_scl;
                    frgd_mode = ((settings_pkt_type *)buf)->frgd_mode;
                    bkgd_step = ((settings_pkt_type *)buf)->bkgd_step;
                }
                else
                {
                    memcpy(&fft, buf, sizeof(fft));
                }
            }
            else
            {
                Sleep(10);
            }
            break;
        }
    }
}

void Visualizer::VisThread()
{
    float red, grn, blu;

    while (1)
    {
        if (!(netmode == NET_MODE_CLIENT && port->connected))
        {
            Update();
        }

        //Overflow background step
        if (bkgd_step >= 360.0f) bkgd_step = 0.0f;
        if (bkgd_step < 0.0f) bkgd_step = 360.0f;

        //Draw active background
        DrawPattern(bkgd_mode, bkgd_bright, &pixels_bg);

        //Draw active foreground
        DrawPattern(frgd_mode, 100, &pixels_fg);

        //Loop through all 256x64 pixels in visualization image
        for (int x = 0; x < 256; x++)
        {
            for (int y = 0; y < 64; y++)
            {
                float brightness = bkgd_bright * (255.0f / 100.0f);

                //Draw Spectrograph Foreground
                if (fft[x] >((1 / 64.0f)*(64.0f - y)))
                {
                    pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                }
                else
                {
                    pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                }

                //Draw Bar Graph Foreground
                if (y == ROW_IDX_BAR_GRAPH)
                {
                    if (x < 128)
                    {
                        if ((fft[5] - 0.05f) >((1 / 128.0f)*(127-x)))
                        {
                            pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                        }
                    }
                    else
                    {
                        if ((fft[5] - 0.05f) >((1 / 128.0f)*((x-128))))
                        {
                            pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                        }
                    }
                }
            }
        }


        //If music isn't playing, fade in the single color LEDs after 2 seconds
        float brightness = fft[5];
        single_color_timeout++;
        for (int i = 0; i < 128; i++)
        {
            if (fft[2 * i] >= 0.0001f)
            {
                single_color_timeout = 0;
            }
        }
        if (single_color_timeout >= 120)
        {
            if (single_color_timeout >= 360.0f)
            {
                single_color_timeout = 360.0f;
            }
            if (single_color_mode == VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND)
            {
                brightness = (single_color_timeout - 120) / 240.0f;
            }
            else
            {
                brightness = (bkgd_bright / 100.0f) * (single_color_timeout - 120) / 240.0f;
            }
        }

        //Draw brightness based visualizer for single LED devices
        switch (single_color_mode)
        {
        case VISUALIZER_SINGLE_COLOR_BLACK:
            DrawSingleColorStatic(brightness, 0x00000000, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_WHITE:
            DrawSingleColorStatic(brightness, 0x00FFFFFF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_RED:
            DrawSingleColorStatic(brightness, 0x000000FF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_ORANGE:
            DrawSingleColorStatic(brightness, 0x000080FF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_YELLOW:
            DrawSingleColorStatic(brightness, 0x0000FFFF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_GREEN:
            DrawSingleColorStatic(brightness, 0x0000FF00, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_CYAN:
            DrawSingleColorStatic(brightness, 0x00FFFF00, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_BLUE:
            DrawSingleColorStatic(brightness, 0x00FF0000, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_PURPLE:
            DrawSingleColorStatic(brightness, 0x00FF00FF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_BACKGROUND:
            //Intentionally do nothing, leave the background unmodified
            break;

        case VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND:
            DrawSingleColorBackground(brightness, &pixels_bg, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND:
            DrawSingleColorForeground(brightness, &pixels_fg, pixels_render);
            break;
        }

        //Swap buffers
        if (pixels_render == &pixels_vs1)
        {
            pixels_render = &pixels_vs2;
            pixels_out = &pixels_vs1;
        }
        else
        {
            pixels_render = &pixels_vs1;
            pixels_out = &pixels_vs2;
        }

        //Increment background step
        bkgd_step = bkgd_step += (anim_speed / 100.0f);

        //Wait 15ms (~60fps)
        Sleep(15);
    }
}

void Visualizer::RazerChromaUpdateThread()
{
    while (rkb.SetLEDs(pixels_out->pixels))
    {
        Sleep(delay);
    }
}

void Visualizer::CorsairKeyboardUpdateThread()
{
    //while (ckb.SetLEDs(pixels_out->pixels))
    {
        Sleep(delay);
    }
}

/*
void Visualizer::CmKeyboardUpdateThread()
{
    while (cmkb.SetLEDs(pixels_out->pixels))
    {
        Sleep(delay);
    }
}

void Visualizer::SteelSeriesKeyboardUpdateThread()
{
    while (skb.SetLEDs(pixels_out->pixels))
    {
        Sleep(delay);
    }
}

void Visualizer::MSIKeyboardUpdateThread()
{
    while (mkb.SetLEDs(pixels_out->pixels))
    {
        Sleep(delay);
    }
}
*/
void Visualizer::LEDStripUpdateThread()
{
    if (str.size() > 0 || xmas.size() > 0)
    {
        while (TRUE)
        {
            for (int i = 0; i < str.size(); i++)
            {
                str[i]->SetLEDs(pixels_out->pixels);
            }

            for (int i = 0; i < xmas.size(); i++)
            {
                xmas[i]->SetLEDsXmas(pixels_out->pixels);
            }

            if (delay < 15)
            {
                Sleep(15);
            }
            else
            {
                Sleep(delay);
            }
        }
    }
}
