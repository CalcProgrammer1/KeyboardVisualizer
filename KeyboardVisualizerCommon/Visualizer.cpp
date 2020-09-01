/*---------------------------------------------------------*\
|  Processing Code for Keyboard Visualizer                  |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#include "Visualizer.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*---------------------------------------------------------*\
| Global variables                                          |
\*---------------------------------------------------------*/
char *  net_string;
int     ledstrip_sections_size  = 1;
int     matrix_setup_pos;
int     matrix_setup_size;
float   fft_nrml[256];
float   fft_fltr[256];
bool    ledstrip_mirror_x       = false;
bool    ledstrip_mirror_y       = false;
bool    ledstrip_single_color   = false;
int     ledstrip_rotate_x       = 0;

/*---------------------------------------------------------*\
| Visualizer class implementation                           |
\*---------------------------------------------------------*/
Visualizer::Visualizer()
{

}

void Visualizer::InitAudioDeviceList()
{
#ifdef WIN32
    IMMDevice* pEndpoint;
    IPropertyStore* pProps;
    PROPVARIANT* varName;

    //If using WASAPI, start WASAPI loopback capture device
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pMMDeviceEnumerator);

    for (int i = 0; i < pMMDevices.size(); i++)
    {
        pMMDevices[i]->Release();
        if (i != 0)
        {
            delete audio_devices[i];
        }
    }

    pMMDevices.clear();
    audio_devices.clear();
    isCapture.clear();

    //Enumerate default audio output
    //pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pEndpoint);
    //audio_devices.push_back("Default Loopback Device");
    //pMMDevices.push_back(pEndpoint);
    //isCapture.push_back(false);

    //Enumerate audio outputs
    pMMDeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection);

    if (pMMDeviceCollection != NULL)
    {
        UINT count;
        pMMDeviceCollection->GetCount(&count);
        for (UINT i = 0; i < count; i++)
        {
            varName = new PROPVARIANT();

            //Query the item from the list
            pMMDeviceCollection->Item(i, &pEndpoint);

            //Open property store for the given item
            pEndpoint->OpenPropertyStore(STGM_READ, &pProps);

            //Get the friendly device name string
            pProps->GetValue(PKEY_Device_FriendlyName, varName);

            if (varName->pwszVal != NULL)
            {
                int len = wcslen(varName->pwszVal) + 1;
                char* new_device = new char[len + 11];
                wcstombs(new_device, varName->pwszVal, len);
                strncat(new_device, " (Loopback)", len);
                audio_devices.push_back(new_device);
                pMMDevices.push_back(pEndpoint);
                isCapture.push_back(false);
            }
            delete varName;
            pProps->Release();
        }
    }
    pMMDeviceCollection->Release();

    //Enumerate audio inputs
    pMMDeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pMMDeviceCollection);

    if (pMMDeviceCollection != NULL)
    {
        UINT count;
        pMMDeviceCollection->GetCount(&count);
        for (UINT i = 0; i < count; i++)
        {
            varName = new PROPVARIANT();

            //Query the item from the list
            pMMDeviceCollection->Item(i, &pEndpoint);

            //Open property store for the given item
            pEndpoint->OpenPropertyStore(STGM_READ, &pProps);

            //Get the friendly device name string
            pProps->GetValue(PKEY_Device_FriendlyName, varName);

            if (varName->pwszVal != NULL)
            {
                int len = wcslen(varName->pwszVal) + 1;
                char* new_device = new char[len];
                wcstombs(new_device, varName->pwszVal, len);
                audio_devices.push_back(new_device);
                pMMDevices.push_back(pEndpoint);
                isCapture.push_back(true);
            }
            delete varName;
            pProps->Release();
        }
    }
    pMMDeviceEnumerator->Release();
#else
    //If using OpenAL, start OpenAL capture on default capture device
    ALCchar* devices;
    devices = (ALCchar *) alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

    //Loop through detected capture devices and stop at the configured one
    char devicestring[512];
    char *devicep = devicestring;
    for(int i = 0; i < 512; i++)
    {
        *devicep = *devices;
        if(*devicep == '\0')
        {
            if(strlen(devicestring) > 0)
            {
                char* new_device = new char[strlen(devicestring) + 1];
                strcpy(new_device, devicestring);
                audio_devices.push_back(new_device);
            }


            i = 0;
            devices++;
            if(*devicep == '\0' && *devicestring == '\0')
            {
                break;
            }

            devicep = devicestring;
        }
        else
        {
            devices++;
            devicep++;
        }
    }
#endif
}

void Visualizer::ChangeAudioDevice()
{
#ifdef WIN32
    if (pAudioClient != NULL)
    {
        pAudioClient->Stop();
        pAudioClient->Release();
        pAudioClient = NULL;
    }

    if (audio_device_idx < audio_devices.size())
    {
        IMMDevice* pMMDevice = pMMDevices[audio_device_idx];
        pMMDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);

        pAudioClient->GetMixFormat(&waveformat);

        if (isCapture[audio_device_idx])
        {
            pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 0, 0, waveformat, 0);
        }
        else
        {
            pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, waveformat, 0);
        }

        pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);

        pAudioClient->Start();
    }
    else if (audio_devices.size() > 0)
    {
        audio_device_idx = 0;
        ChangeAudioDevice();
    }
#else
    if(device != NULL)
    {
        alcCaptureStop(device);
    }

    if(audio_device_idx < audio_devices.size())
    {
        device = alcCaptureOpenDevice(audio_devices[audio_device_idx], 10000, AL_FORMAT_MONO8, 2048);
        alcCaptureStart(device);
    }
    else if(audio_devices.size() > 0)
    {
        audio_device_idx = 0;
        ChangeAudioDevice();
    }
#endif
}

void Visualizer::Initialize()
{
    InitAudioDeviceList();

    netmode              = NET_MODE_DISABLED;
    background_timer     = 0;
    background_timeout   = 120;

    amplitude            = 100;
    anim_speed           = 100.0f;
    avg_mode             = 0;
    avg_size             = 8;
    bkgd_step            = 0;
    bkgd_bright          = 100;
    bkgd_mode            = VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL;
    delay                = 25;
    window_mode          = 1;
    decay                = 80;
    frgd_mode            = VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED;
    single_color_mode    = VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND;
    reactive_bkgd        = false;
    audio_device_idx     = 0;
    filter_constant      = 1.0f;

    settings_changed     = false;
    update_ui            = false;
    shutdown_flag        = false;

    hanning(win_hanning, 256);
    hamming(win_hamming, 256);
    blackman(win_blackman, 256);

    nrml_ofst            = 0.04f;
    nrml_scl             = 0.5f;

    pixels_render = &pixels_vs1;
    pixels_out = &pixels_vs2;

    ChangeAudioDevice();
    SetNormalization(nrml_ofst, nrml_scl);
}

void Visualizer::InitClient(char * clientstring)
{
    if (netmode == NET_MODE_DISABLED)
    {
        net_string = new char[strlen(clientstring) + 1];
        strcpy(net_string, clientstring);
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
        net_string = new char[strlen(serverstring) + 1];
        strcpy(net_string, serverstring);
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

    //Check background flags, they both should not be set
    if ((silent_bkgd == TRUE) && (reactive_bkgd == TRUE))
    {
        silent_bkgd = FALSE;
    }

    //Get file path in executable directory
//#ifdef WIN32
//    GetModuleFileName(NULL, filename, 2048);
//    strcpy(filename, std::string(filename).substr(0, std::string(filename).find_last_of("\\/")).c_str());
//    strcat(filename, "\\settings.txt");
//#else
//    char arg1[64];
//    snprintf(arg1, 64, "/proc/%d/exe", getpid());
//    readlink(arg1, filename, 1024);
//    strcpy(filename, std::string(filename).substr(0, std::string(filename).find_last_of("\\/")).c_str());
//    strcat(filename, "/settings.txt");
//#endif

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

    //Save Filter Constant
    snprintf(out_str, 1024, "fltr_const=%f\r\n", filter_constant);
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

    //Save Reactive Background Flag
    snprintf(out_str, 1024, "reactive_bkgd=%d\r\n", reactive_bkgd);
    outfile.write(out_str, strlen(out_str));

    //Save Silent Background Flag
    snprintf(out_str, 1024, "silent_bkgd=%d\r\n", silent_bkgd);
    outfile.write(out_str, strlen(out_str));

    //Save Background Timeout
    snprintf(out_str, 1024, "background_timeout=%d\r\n", background_timeout);
    outfile.write(out_str, strlen(out_str));

    //Save Audio Device Index
    snprintf(out_str, 1024, "audio_device_idx=%d\r\n", audio_device_idx);
    outfile.write(out_str, strlen(out_str));

    //Save Network Mode
    switch (netmode)
    {
    case NET_MODE_CLIENT:
        //Save Client Configuration
        snprintf(out_str, 1024, "client=%s\r\n", net_string);
        outfile.write(out_str, strlen(out_str));
        break;

    case NET_MODE_SERVER:
        //Save Server Configuration
        snprintf(out_str, 1024, "server=%s\r\n", net_string);
        outfile.write(out_str, strlen(out_str));
        break;
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
    settings_changed = true;
}

void Visualizer::SendSettings()
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
        settings.filter_constant = filter_constant;
        settings.frgd_mode = frgd_mode;
        settings.reactive_bkgd = reactive_bkgd;
        settings.silent_bkgd = silent_bkgd;
        settings.background_timeout = background_timeout;
        port->tcp_write((char *)&settings, sizeof(settings));
    }
}

void Visualizer::Update()
{
    float fft_tmp[512];

    for (int i = 0; i < 256; i++)
    {
        //Clear the buffers
        fft_tmp[i] = 0;

        //Decay previous values
        fft[i] = fft[i] * (((float)decay) / 100.0f);
    }

#ifdef WIN32
    unsigned int buffer_pos = 0;
    static float input_wave[512];

    unsigned int nextPacketSize = 1;
    unsigned int flags;

    while (nextPacketSize > 0)
    {
        float *buf;
        if (pAudioCaptureClient != NULL)
        {
            pAudioCaptureClient->GetBuffer((BYTE**)&buf, &nextPacketSize, (DWORD *)&flags, NULL, NULL);

            if (buf == NULL && nextPacketSize > 0)
            {
                pAudioClient->Stop();
                pAudioCaptureClient->Release();
                pAudioClient->Release();
                pAudioCaptureClient = NULL;
                pAudioClient = NULL;
            }
            else
            {
                for (unsigned int i = 0; i < nextPacketSize; i += 4)
                {
                    for (int j = 0; j < 255; j++)
                    {
                        input_wave[2 * j] = input_wave[2 * (j + 1)];
                        input_wave[(2 * j) + 1] = input_wave[2 * j];
                    }

                    float avg_buf = (buf[i] + buf[i + 1] + buf[i + 2] + buf[i + 3]) / 4;
                    input_wave[510] = avg_buf * 2.0f * amplitude;
                    input_wave[511] = input_wave[510];
                }

                buffer_pos += nextPacketSize / 4;
                pAudioCaptureClient->ReleaseBuffer(nextPacketSize);
            }
        }
    }

    memcpy(fft_tmp, input_wave, sizeof(input_wave));
#else
    //Only update FFT if there are at least 256 samples in the sample buffer
    int samples;

    if(device != NULL)
    {
        do
        {
            alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 1, &samples);
            Sleep(1);
        } while (samples < 512);
    }

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
    for(int i = 0; i < 256; i++)
    {
        fft_fltr[i] = fft_fltr[i] + (filter_constant * (fft[i] - fft_fltr[i]));
    }
}

void Visualizer::StartThread()
{
    //Set application running flag to TRUE before starting threads
    running = true;

    VisThread           = new std::thread(&Visualizer::VisThreadFunction, this);
    NetConnectThread    = new std::thread(&Visualizer::NetConnectThreadFunction, this);
    NetUpdateThread     = new std::thread(&Visualizer::NetUpdateThreadFunction, this);
}

void Visualizer::Shutdown()
{
    //Initialize a fade-out by setting shutdown flag to TRUE and resetting timer
    shutdown_flag = TRUE;
    background_timer = 0;

    //Wait for fade-out to complete before returning
    while (running == true)
    {
        Sleep(50);
    }
}

void DrawSolidColor(int bright, RGBColor color, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = RGB(((bright * GetRValue(color)) / 256), ((bright * GetGValue(color)) / 256), ((bright * GetBValue(color)) / 256));
        }
    }
}

void DrawVerticalBars(int bright, RGBColor * colors, int num_colors, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int idx = (int)((float)x * ((float)num_colors / 255.0f));
            pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
        }
    }
}

void DrawHorizontalBars(int bright, RGBColor * colors, int num_colors, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            if (y == ROW_IDX_BAR_GRAPH)
            {
                if (x < 128)
                {
                    int idx = (int)(num_colors - ((float)x * ((float)num_colors / 128.0f)));
                    if (idx >= num_colors)
                    {
                        idx = num_colors - 1;
                    }
                    pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
                }
                else
                {
                    int idx = (int)(((float)(x - 128) * ((float)num_colors / 128.0f)));
                    pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
                }
            }
            else
            {
                int idx = (int)(num_colors - ((float)y * ((float)num_colors / 63.0f)));
                pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
            }
        }
    }
}

void DrawRainbowSinusoidal(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int red = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f)) + 1));
            int grn = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) - (6.28f / 3)) + 1));
            int blu = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) + (6.28f / 3)) + 1));
            pixels->pixels[y][x] = RGB(((bright * red) / 256), ((bright * grn) / 256), ((bright * blu) / 256));
        }
    }
}

void DrawRainbow(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int hsv_h = ((int)(bkgd_step + (256 - x)) % 360);
            hsv_t hsv = { 0, 0, 0 };
            hsv.hue = hsv_h;
            hsv.saturation = 255;
            hsv.value = (unsigned char)bright;
            pixels->pixels[y][x] = hsv2rgb(&hsv);
        }
    }
}

void DrawColorWheel(int bright, float bkgd_step, int center_x, int center_y, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            float hue = (float)(bkgd_step + (int)(180 + atan2(y - center_y, x - center_x) * (180.0 / 3.14159)) % 360);
            hsv_t hsv2 = { 0, 0, 0 };
            hsv2.hue = (int)hue;
            hsv2.saturation = 255;
            hsv2.value = (unsigned char)bright;
            pixels->pixels[y][x] = hsv2rgb(&hsv2);
        }
    }
}

void DrawSpectrumCycle(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    hsv_t hsv2 = { 0, 0, 0 };
    hsv2.hue = (int)bkgd_step;
    hsv2.saturation = 255;
    hsv2.value = (unsigned char)bright;
    RGBColor color = hsv2rgb(&hsv2);

    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = color;
        }
    }
}

void DrawSinusoidalCycle(int bright, float bkgd_step, vis_pixels *pixels)
{
    RGBColor color;
    bright = (int)(bright * (255.0f / 100.0f));
    int red = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f)) + 1));
    int grn = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) - (6.28f / 3)) + 1));
    int blu = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) + (6.28f / 3)) + 1));
    color = RGB(((bright * red) / 256), ((bright * grn) / 256), ((bright * blu) / 256));

    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = color;
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

    int idx = (int)(64.0f - (amplitude * 62.0f));
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

void DrawSingleColorStatic(float amplitude, RGBColor in_color, vis_pixels *out_pixels)
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
        DrawSolidColor(bright, 0x000040FF, pixels);
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
        DrawSolidColor(bright, 0x00FF0040, pixels);
        break;

    case VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED:
        {
        RGBColor colors[] = { 0x0000FF00, 0x0000FFFF, 0x000000FF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_GREEN_WHITE_RED:
        {
        RGBColor colors[] = { 0x0000FF00, 0x00FFFFFF, 0x000000FF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_BLUE_CYAN_WHITE:
        {
        RGBColor colors[] = { 0x00FF0000, 0x00FFFF00, 0x00FFFFFF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RED_WHITE_BLUE:
        {
        RGBColor colors[] = { 0x000000FF, 0x00FFFFFF, 0x00FF0000 };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RAINBOW:
        {
        RGBColor colors[] = { 0x000000FF, 0x0000FFFF, 0x0000FF00, 0x00FFFF00, 0x00FF0000, 0x00FF00FF };
        DrawHorizontalBars(bright, colors, 6, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RAINBOW_INVERSE:
        {
        RGBColor colors[] = { 0x00FF00FF, 0x00FF0000, 0x00FFFF00, 0x0000FF00, 0x0000FFFF, 0x000000FF };
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
        DrawColorWheel(bright, bkgd_step, 128, 32, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_COLOR_WHEEL_2:
        DrawColorWheel(bright, bkgd_step, 128, 64, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_SPECTRUM_CYCLE:
        DrawSpectrumCycle(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_SINUSOIDAL_CYCLE:
        DrawSinusoidalCycle(bright, bkgd_step, pixels);
        break;
    }
}

void Visualizer::NetConnectThreadFunction()
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
            SendSettings();
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

void Visualizer::NetUpdateThreadFunction()
{
    int counter = 0;
    char buf[sizeof(fft_fltr)];

    while (1)
    {
        switch (netmode)
        {
        case NET_MODE_DISABLED:
            return;
            break;

        case NET_MODE_SERVER:
            port->tcp_write((char *)fft_fltr, sizeof(fft_fltr));
            if (counter++ > 30)
            {
                port->tcp_write((char *)&bkgd_step, sizeof(bkgd_step));
            }
            if (settings_changed)
            {
                SendSettings();
                settings_changed = false;
            }
            Sleep(20);
            break;

        case NET_MODE_CLIENT:
            if (port->connected)
            {
                int size = port->tcp_listen((char *)buf, sizeof(buf));

                if (size == sizeof(fft_fltr))
                {
                    memcpy(&fft_fltr, buf, sizeof(fft_fltr));
                }
                else if (size == sizeof(bkgd_step))
                {
                    memcpy(&bkgd_step, buf, sizeof(bkgd_step));
                }
                else if (size == sizeof(settings_pkt_type))
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
                    filter_constant = ((settings_pkt_type *)buf)->filter_constant;
                    frgd_mode = ((settings_pkt_type *)buf)->frgd_mode;
                    reactive_bkgd = ((settings_pkt_type *)buf)->reactive_bkgd;
                    silent_bkgd = ((settings_pkt_type *)buf)->silent_bkgd;
                    background_timeout = ((settings_pkt_type *)buf)->background_timeout;
                    SetNormalization(nrml_ofst, nrml_scl);

                    //Check background flags, they both should not be set
                    if ((silent_bkgd == TRUE) && (reactive_bkgd == TRUE))
                    {
                        silent_bkgd = FALSE;
                    }

                    update_ui = TRUE;
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

void Visualizer::VisThreadFunction()
{
    while (running == true)
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

        float brightness = fft_fltr[5];

        //If music isn't playing, fade in the single color LEDs after 2 seconds
        background_timer++;

        if (shutdown_flag == true)
        {
            if (background_timer >= background_timeout)
            {
                brightness = 0.0f;
                running = false;
            }
            else
            {
                brightness = ((background_timeout - background_timer) / (1.0f * background_timeout));
            }
        }
        else if (background_timeout > 0)
        {
            for (int i = 0; i < 128; i++)
            {
                if (fft_fltr[2 * i] >= 0.0001f)
                {
                    background_timer = 0;
                }
            }
            if (background_timer >= background_timeout)
            {
                if (background_timer >= (3 * background_timeout))
                {
                    background_timer = (3 * background_timeout);
                }
                brightness = (background_timer - background_timeout) / (2.0f * background_timeout);
            }
        }

        //Loop through all 256x64 pixels in visualization image
        for (int x = 0; x < 256; x++)
        {
            for (int y = 0; y < 64; y++)
            {
                //Draw Spectrograph Foreground
                if (fft_fltr[x] >((1 / 64.0f)*(64.0f - y)))
                {
                    if (shutdown_flag == true)
                    {
                        int in_color = pixels_fg.pixels[y][x];
                        pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                    }
                    else
                    {
                        pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                    }
                }
                else
                {
                    if(reactive_bkgd || silent_bkgd)
                    {
                        if (!silent_bkgd || ((background_timer >= background_timeout) && (background_timeout > 0)))
                        {
                            int in_color = pixels_bg.pixels[y][x];
                            pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = RGB(0, 0, 0);
                        }
                    }
                    else
                    {
                        pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                    }

                }

                //Draw Bar Graph Foreground
                if (y == ROW_IDX_BAR_GRAPH)
                {
                    if (x < 128)
                    {
                        if ((fft_fltr[5] - 0.05f) >((1 / 128.0f)*(127-x)))
                        {
                            if (shutdown_flag == true)
                            {
                                int in_color = pixels_fg.pixels[y][x];
                                pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                            }
                            else
                            {
                                pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                            }
                        }
                        else
                        {
                            if (reactive_bkgd || silent_bkgd)
                            {
                                if (!silent_bkgd || ((background_timer >= background_timeout) && (background_timeout > 0)))
                                {
                                    int in_color = pixels_bg.pixels[y][x];
                                    pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                                }
                                else
                                {
                                    pixels_render->pixels[y][x] = RGB(0, 0, 0);
                                }
                            }
                            else
                            {
                                pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                            }
                        }
                    }
                    else
                    {
                        if ((fft_fltr[5] - 0.05f) >((1 / 128.0f)*((x-128))))
                        {
                            if (shutdown_flag == true)
                            {
                                int in_color = pixels_fg.pixels[y][x];
                                pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                            }
                            else
                            {
                                pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                            }
                        }
                        else
                        {
                            if (reactive_bkgd || silent_bkgd)
                            {
                                if (!silent_bkgd || (background_timer >= background_timeout))
                                {
                                    int in_color = pixels_bg.pixels[y][x];
                                    pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                                }
                                else
                                {
                                    pixels_render->pixels[y][x] = RGB(0, 0, 0);
                                }
                            }
                            else
                            {
                                pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                            }
                        }
                    }
                }
            }
        }

        if (single_color_mode == VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND)
        {
            brightness = (bkgd_bright / 100.0f) * brightness;
        }

        if ((background_timeout <= 0 ) || (background_timer < background_timeout))
        {
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

static void UpdateOpenRGBClientListCallback(void * this_ptr)
{
    Visualizer * this_obj = (Visualizer *)this_ptr;

    this_obj->UpdateClientSettings();
}

void Visualizer::RegisterClientInfoChangeCallback(NetClientCallback new_callback, void * new_callback_arg)
{
    ClientInfoChangeCallbacks.push_back(new_callback);
    ClientInfoChangeCallbackArgs.push_back(new_callback_arg);
}

void Visualizer::ClientInfoChanged()
{
    ClientInfoChangeMutex.lock();

    /*-------------------------------------------------*\
    | Client info has changed, call the callbacks       |
    \*-------------------------------------------------*/
    for(unsigned int callback_idx = 0; callback_idx < ClientInfoChangeCallbacks.size(); callback_idx++)
    {
        ClientInfoChangeCallbacks[callback_idx](ClientInfoChangeCallbackArgs[callback_idx]);
    }

    ClientInfoChangeMutex.unlock();
}

void Visualizer::UpdateClientSettings()
{
    /*-----------------------------------------------------*\
    | Loop through all clients and make sure each has an    |
    | associated settings                                   |
    \*-----------------------------------------------------*/
    for(unsigned int client_idx = 0; client_idx < rgb_clients.size(); client_idx++)
    {
        if(client_idx < rgb_client_settings.size())
        {
            /*-----------------------------------------------------*\
            | If the client settings at this index matches the      |
            | controller at this index, continue                    |
            \*-----------------------------------------------------*/
            if(rgb_client_settings[client_idx]->client_ptr == rgb_clients[client_idx])
            {
                continue;
            }
            /*-----------------------------------------------------*\
            | Otherwise, search the rest of the client settings list|
            | to see if the client settings for this controller has |
            | been misplaced                                        |
            \*-----------------------------------------------------*/
            else
            {
                bool found_match = false;

                for(unsigned int search_client_idx = client_idx; search_client_idx < rgb_client_settings.size(); search_client_idx++)
                {
                    /*-----------------------------------------------------*\
                    | If the client settings at this index matches the      |
                    | controller at this index, swap the settings at the    |
                    | original position for those at the search position    |
                    \*-----------------------------------------------------*/
                    if(rgb_client_settings[search_client_idx]->client_ptr == rgb_clients[client_idx])
                    {
                        ClientSettingsType* tmp_settings = rgb_client_settings[search_client_idx];

                        rgb_client_settings[search_client_idx] = rgb_client_settings[client_idx];

                        rgb_client_settings[client_idx] = tmp_settings;

                        found_match = true;

                        break;
                    }
                }

                /*-----------------------------------------------------*\
                | If a matching settings was not found, create a new one|
                \*-----------------------------------------------------*/
                if(!found_match)
                {
                    ClientSettingsType* new_settings = new ClientSettingsType();

                    new_settings->client_ptr = rgb_clients[client_idx];

                    rgb_client_settings.insert(rgb_client_settings.begin() + client_idx, new_settings);
                }
            }
        }
        else
        {
            /*-----------------------------------------------------*\
            | If the settings list is smaller than the client index |
            | that means this client wasn't found in the existing   |
            | list.  Create a new settings for this client          |
            \*-----------------------------------------------------*/

            ClientSettingsType* new_settings = new ClientSettingsType();

            new_settings->client_ptr = rgb_clients[client_idx];

            rgb_client_settings.insert(rgb_client_settings.begin() + client_idx, new_settings);
        }
    }

    /*-----------------------------------------------------*\
    | At this point, the clients and settings lists should  |
    | line up.  If the settings list is longer than the     |
    | clients list, delete the extras as they are unused    |
    \*-----------------------------------------------------*/
    for(unsigned int settings_idx = rgb_clients.size(); settings_idx < rgb_client_settings.size(); settings_idx++)
    {
        rgb_client_settings.pop_back();
    }

    /*-----------------------------------------------------*\
    | Now go through each client and make sure all the      |
    | controller settings entries line up                   |
    \*-----------------------------------------------------*/
    for(unsigned int client_idx = 0; client_idx < rgb_clients.size(); client_idx++)
    {
        std::vector<RGBController *> &          controllers         = rgb_clients[client_idx]->server_controllers;
        std::vector<ControllerSettingsType *> & controller_settings = rgb_client_settings[client_idx]->controller_settings;

        for(unsigned int controller_idx = 0; controller_idx < controllers.size(); controller_idx++)
        {
            if(controller_idx < controller_settings.size())
            {
                /*-----------------------------------------------------*\
                | If the controller settings at this index matches the  |
                | controller at this index, continue                    |
                \*-----------------------------------------------------*/
                if(controller_settings[controller_idx]->controller_ptr == controllers[controller_idx])
                {
                    continue;
                }
                /*-----------------------------------------------------*\
                | Otherwise, search the rest of the client settings list|
                | to see if the client settings for this controller has |
                | been misplaced                                        |
                \*-----------------------------------------------------*/
                else
                {
                    bool found_match = false;

                    for(unsigned int search_controller_idx = controller_idx; search_controller_idx < controllers.size(); search_controller_idx++)
                    {
                        /*-----------------------------------------------------*\
                        | If the client settings at this index matches the      |
                        | controller at this index, swap the settings at the    |
                        | original position for those at the search position    |
                        \*-----------------------------------------------------*/
                        if(controller_settings[search_controller_idx]->controller_ptr == controllers[controller_idx])
                        {
                            ControllerSettingsType* tmp_settings = controller_settings[search_controller_idx];

                            controller_settings[search_controller_idx] = controller_settings[controller_idx];

                            controller_settings[controller_idx] = tmp_settings;

                            found_match = true;

                            break;
                        }
                    }

                    /*-----------------------------------------------------*\
                    | If a matching settings was not found, create a new one|
                    \*-----------------------------------------------------*/
                    if(!found_match)
                    {
                        ControllerSettingsType* new_settings = new ControllerSettingsType();

                        new_settings->controller_ptr = controllers[controller_idx];

                        controller_settings.insert(controller_settings.begin() + controller_idx, new_settings);
                    }
                }
            }
            else
            {
                /*-----------------------------------------------------*\
                | If the settings list is smaller than the client index |
                | that means this client wasn't found in the existing   |
                | list.  Create a new settings for this client          |
                \*-----------------------------------------------------*/

                ControllerSettingsType* new_settings = new ControllerSettingsType();

                new_settings->controller_ptr = controllers[controller_idx];
                new_settings->enabled        = false;

                for(int mode_idx = 0; mode_idx < controllers[controller_idx]->modes.size(); mode_idx++)
                {
                    if(controllers[controller_idx]->modes[mode_idx].name == "Direct")
                    {
                        new_settings->enabled = true;
                    }
                }

                controller_settings.insert(controller_settings.begin() + controller_idx, new_settings);
            }
        }

        /*-----------------------------------------------------*\
        | At this point, the clients and settings lists should  |
        | line up.  If the settings list is longer than the     |
        | clients list, delete the extras as they are unused    |
        \*-----------------------------------------------------*/
        for(unsigned int settings_idx = controllers.size(); settings_idx < controller_settings.size(); settings_idx++)
        {
            controller_settings.pop_back();
        }
    }

    ClientInfoChanged();
}

static bool started = false;
NetworkClient * Visualizer::OpenRGBConnect(const char * ip, unsigned short port)
{
    NetworkClient * rgb_client = new NetworkClient(rgb_controllers);

    rgb_clients.push_back(rgb_client);

    std::string titleString = "Keyboard Visualizer ";
    titleString.append(VERSION_STRING);

    rgb_client->SetIP(ip);
    rgb_client->SetName(titleString.c_str());
    rgb_client->SetPort(port);

    rgb_client->RegisterClientInfoChangeCallback(UpdateOpenRGBClientListCallback, this);

    rgb_client->StartClient();

    if(!started)
    {
        started = true;
        LEDUpdateThread     = new std::thread(&Visualizer::LEDUpdateThreadFunction, this);
    }

    return(rgb_client);
}

void Visualizer::OpenRGBDisconnect(NetworkClient * client)
{
    started = false;
    LEDUpdateThread->join();

    client->StopClient();

    for(unsigned int client_idx = 0; client_idx < rgb_clients.size(); client_idx++)
    {
        if(client == rgb_clients[client_idx])
        {
            rgb_clients.erase(rgb_clients.begin() + client_idx);
            rgb_client_settings.erase(rgb_client_settings.begin() + client_idx);
            break;
        }
    }

    if(!started)
    {
        started = true;
        LEDUpdateThread     = new std::thread(&Visualizer::LEDUpdateThreadFunction, this);
    }
}

static void SetupMatrixGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for(int x = 0; x < x_count; x++)
    {
        if(x_count < 10)
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count))) + (0.5f * (SPECTROGRAPH_COLS / (x_count))));
        }
        else if(x < ((x_count) / 2))
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) + (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }
        else
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) - (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }

    }
    for(int y = 0; y < y_count; y++)
    {
        y_idx_list[y] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / y_count)) + (0.5f * (SPECTROGRAPH_ROWS / y_count)));
    }
}

static void SetupLinearGrid(int x_count, int * x_idx_list)
{
    if((x_count % 2) == 0)
    {
        //Even number of LEDs
        for(int x = 0; x < x_count; x++)
        {
            x_idx_list[x] = (int)((float)x * (256.0f / (float)x_count)) + (128.0f / (float)x_count);
        }
    }
    else
    {
        //Odd number of LEDs
        for(int x = 0; x < x_count; x++)
        {
            if (x == (x_count / 2))
            {
                x_idx_list[x] = 128;
            }
            else if (x < ((x_count / 2) + 1))
            {
                x_idx_list[x] = (x_count / 2) + ((x + 1) * (256 / (x_count + 1)));
            }
            else
            {
                x_idx_list[x] = ((x_count / 2) + 1) + (x * (256 / (x_count + 1)));
            }

        }
    }
}

void Visualizer::LEDUpdateThreadFunction()
{
    while(started)
    {
        for(unsigned int client_idx = 0; client_idx < rgb_clients.size(); client_idx++)
        {
            if(client_idx < rgb_client_settings.size())
            {
                if(rgb_client_settings[client_idx]->client_ptr == rgb_clients[client_idx])
                {
                    std::vector<RGBController *> &          controllers           = rgb_clients[client_idx]->server_controllers;
                    std::vector<ControllerSettingsType *> & controllers_settings  = rgb_client_settings[client_idx]->controller_settings;

                    for(unsigned int controller_idx = 0; controller_idx < controllers.size(); controller_idx++)
                    {
                        if(controller_idx < controllers_settings.size())
                        {
                            if(controllers_settings[controller_idx]->controller_ptr == controllers[controller_idx])
                            {
                                RGBController *               controller            = controllers[controller_idx];
                                ControllerSettingsType *      controller_settings   = controllers_settings[controller_idx];

                                if(controller_settings->enabled)
                                {
                                    for(unsigned int zone_idx = 0; zone_idx < controller->zones.size(); zone_idx++)
                                    {
                                        int                 x_count                 = controller->zones[zone_idx].leds_count;
                                        int                 y_count                 = 0;
                                        zone_type           type                    = controller->zones[zone_idx].type;
                                        ZoneIndexType *     zone_index_map          = NULL;
                                        bool                index_map_found         = false;

                                        // If matrix type and matrix mapping is valid, get X and Y count
                                        if(type == ZONE_TYPE_MATRIX)
                                        {
                                            if(controller->zones[zone_idx].matrix_map != NULL)
                                            {
                                                x_count     = controller->zones[zone_idx].matrix_map->width;
                                                y_count     = controller->zones[zone_idx].matrix_map->height;
                                            }
                                            else
                                            {
                                                type = ZONE_TYPE_SINGLE;
                                            }
                                        }

                                        // Search all the zone index maps
                                        for(int i = 0; i < controller_settings->zones.size(); i++)
                                        {
                                            zone_index_map = &controller_settings->zones[i];

                                            if((zone_index_map->x_count == x_count) && (zone_index_map->y_count == y_count))
                                            {
                                                index_map_found = true;
                                                break;
                                            }
                                        }

                                        // If the index map doesn't exist for this zone, create it
                                        if(index_map_found == false)
                                        {
                                            ZoneIndexType *   new_index_map             = new ZoneIndexType();
                                            new_index_map->x_count                      = x_count;
                                            new_index_map->y_count                      = y_count;

                                            if(type == ZONE_TYPE_MATRIX)
                                            {
                                                new_index_map->x_index                  = new int[x_count];
                                                new_index_map->y_index                  = new int[y_count];

                                                SetupMatrixGrid(x_count, y_count, new_index_map->x_index, new_index_map->y_index);
                                            }
                                            else if(type == ZONE_TYPE_LINEAR)
                                            {
                                                new_index_map->x_index                  = new int[x_count];

                                                SetupLinearGrid(x_count, new_index_map->x_index);
                                            }

                                            controller_settings->zones.push_back(*new_index_map);

                                            zone_index_map = &controller_settings->zones[controller_settings->zones.size() - 1];
                                        }

                                        switch (controller->zones[zone_idx].type)
                                        {
                                        case ZONE_TYPE_MATRIX:
                                            for (int y = 0; y < y_count; y++)
                                            {
                                                for (int x = 0; x < x_count; x++)
                                                {
                                                    unsigned int map_idx = (y * x_count) + x;
                                                    unsigned int color_idx = controller->zones[zone_idx].matrix_map->map[map_idx];
                                                    if( color_idx != 0xFFFFFFFF )
                                                    {
                                                        controller->zones[zone_idx].colors[color_idx] = pixels_out->pixels[zone_index_map->y_index[y]][zone_index_map->x_index[x]];
                                                    }
                                                }
                                            }
                                            break;

                                        case ZONE_TYPE_SINGLE:
                                            for (int r = 0; r < x_count; r++)
                                            {
                                                controller->zones[zone_idx].colors[r] = pixels_out->pixels[ROW_IDX_SINGLE_COLOR][0];
                                            }
                                            break;

                                        case ZONE_TYPE_LINEAR:
                                            for (int x = 0; x < x_count; x++)
                                            {
                                                controller->zones[zone_idx].colors[x] = pixels_out->pixels[ROW_IDX_BAR_GRAPH][zone_index_map->x_index[x]];
                                            }
                                            break;
                                        }
                                    }
                                    controller->DeviceUpdateLEDs();
                                }
                            }
                        }
                    }
                }
            }
        }
        Sleep(delay);
    }
}
