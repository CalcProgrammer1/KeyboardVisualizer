#include "Visualizer.h"
#include "RazerKeyboard.h"
#include "CorsairKeyboard.h"
#include "LEDStrip.h"

RazerKeyboard rkb;
CorsairKeyboard ckb;
//LEDStrip str;

//Thread starting static function
static void thread(void *param)
{
	Visualizer* vis = static_cast<Visualizer*>(param);
	vis->VisThread();
}

static void rkbthread(void *param)
{
	Visualizer* vis = static_cast<Visualizer*>(param);
	vis->RazerKeyboardUpdateThread();
}

static void ckbthread(void *param)
{
    Visualizer* vis = static_cast<Visualizer*>(param);
    vis->CorsairKeyboardUpdateThread();
}

Visualizer::Visualizer()
{

}

float fft_nrml[256];

void Visualizer::Initialize()
{
    ALCchar* devices;
    ALCchar* devstr;
    ALCchar* next;
    int len = 0;

	devices = (ALCchar *) alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
	
    devstr = devices;
    next = devices + 1;

    while ( (devstr) && (*devstr != '\0') && next && (*next != '\0') )
    {
        device_list.push_back(devstr);
        len = strlen(devstr);
        devstr += (len + 1);
        next += (len + 2);
    }
    
    device_idx = 0;

    ChangeDevice();

    rkb.Initialize();
	ckb.Initialize();
    //str.Initialize();

	amplitude   = 100;
    avg_mode    = 0;
    avg_size    = 8;
	bkgd_step   = 0;
	bkgd_bright = 10;
	bkgd_mode   = 0;
	delay       = 50;
	window_mode = 1;
	decay       = 80;
    frgd_mode   = 1;

	hanning(win_hanning, 256);
	hamming(win_hamming, 256);
	blackman(win_blackman, 256);

	for (int i = 0; i < 256; i++)
	{
		fft[i] = 0.0f;
		fft_nrml[i] = 0.040f + (0.60f * (i / 256.0f));
	}
}

void Visualizer::ChangeDevice()
{
    if (device != NULL)
    {
        alcCaptureStop(device);
        alcCaptureCloseDevice(device);
    }

    device = alcCaptureOpenDevice(device_list[device_idx].c_str(), 16000, AL_FORMAT_MONO8, 2048);
    alcCaptureStart(device);
}

void Visualizer::Update()
{
	float fft_tmp[512];

	//Audio Sample Buffer
	unsigned char buffer[256];

	for (int i = 0; i < 256; i++)
	{
		//Clear the buffers
		buffer[i] = 0;
		fft_tmp[i] = 0;

		//Decay previous values
		fft[i] = fft[i] * (((float)decay) / 100.0f);
	}

	//Only update FFT if there are at least 256 samples in the sample buffer
	int samples;

	do
	{
        if (device != NULL)
        {
            alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 1, &samples);
        }
        else
        {
            break;
        }
		Sleep(1);
	} while (samples < 256);
	
	//Capture 256 audio samples
    if (device != NULL)
    {
        alcCaptureSamples(device, (ALCvoid *)buffer, 256);
    }

	//Scale the input into the FFT processing array
	for (int i = 0; i < 512; i++)
	{
		fft_tmp[i] = (buffer[i / 2] - 128.0f) * (amplitude / 128.0f);
	}

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

        //fftmag = 10 * log10(fftmag);

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
	_beginthread(thread, 0, this);
	_beginthread(rkbthread, 0, this);
    _beginthread(ckbthread, 0, this);
}

COLORREF Visualizer::GetAmplitudeColor(int amplitude, int range)
{
    COLORREF color;

    int val = ((float)amplitude / (float)range) * 100.0f;

    switch (frgd_mode)
    {
    //White
    case 0:
        color = RGB(255, 255, 255);
        break;

    //Green/Yellow/Red
    case 1:
        if (val > 66)
        {
            color = RGB(0, 255, 0);
        }
        else if (val > 33)
        {
            color = RGB(255, 255, 0);
        }
        else
        {
            color = RGB(255, 0, 0);
        }
        break;

    //White/Cyan/Blue
    case 2:
        if (val > 66)
        {
            color = RGB(0, 0, 255);
        }
        else if (val > 33)
        {
            color = RGB(0, 255, 255);
        }
        else
        {
            color = RGB(255, 255, 255);
        }
        break;

    //Red/White/Blue
    case 3:
        if (val > 66)
        {
            color = RGB(0, 0, 255);
        }
        else if (val > 33)
        {
            color = RGB(255, 255, 255);
        }
        else
        {
            color = RGB(255, 0, 0);
        }
        break;

    //Rainbow
    case 4:
        if (val > 83)
        {
            color = RGB(255, 0, 0);
        }
        else if (val > 66)
        {
            color = RGB(255, 255, 0);
        }
        else if (val > 50)
        {
            color = RGB(0, 255, 0);
        }
        else if (val > 33)
        {
            color = RGB(0, 255, 255);
        }
        else if (val > 16)
        {
            color = RGB(0, 0, 255);
        }
        else
        {
            color = RGB(255, 0, 255);
        }
        break;

    //Rainbow Inverse
    case 5:
        if (val > 83)
        {
            color = RGB(255, 0, 255);
        }
        else if (val > 66)
        {
            color = RGB(0, 0, 255);
        }
        else if (val > 50)
        {
            color = RGB(0, 255, 255);
        }
        else if (val > 33)
        {
            color = RGB(0, 255, 0);
        }
        else if (val > 16)
        {
            color = RGB(255, 255, 0);
        }
        else
        {
            color = RGB(255, 0, 0);
        }
        break;
    }
    return(color);
}

void Visualizer::VisThread()
{
	float red, grn, blu;

	while (1)
	{
		Update();

		//Overflow background step
		if (bkgd_step >= 360) bkgd_step = 0;

		//Loop through all 256x64 pixels in visualization image
		for (int x = 0; x < 256; x++)
		{
			for (int y = 0; y < 64; y++)
			{
				//Draw active background
				switch (bkgd_mode)
				{
				case 0:
					pixels[y][x] = RGB(0, 0, 0);
					break;

				case 1:
					red = (sin((((((int)(x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f)) + 1);
					grn = (sin((((((int)(x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) - (6.28f / 3)) + 1);
					blu = (sin((((((int)(x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) + (6.28f / 3)) + 1);
					pixels[y][x] = RGB(bkgd_bright * red, bkgd_bright * grn, bkgd_bright * blu);
					break;

				case 2:
					int hsv_h = ((bkgd_step + (256 - x)) % 360);
					hsv_t hsv = { hsv_h, 255, bkgd_bright };
					pixels[y][x] = hsv2rgb(&hsv);
					break;
				}

                if (y > 3)
                {
                    if (fft[x] >((1 / 64.0f)*(64.0f - y)))
                    {
                        pixels[y][x] = GetAmplitudeColor(y, 64);
                    }
                }

                if (y < 2)
                {
                    if (x < 128)
                    {
                        if ((fft[5] - 0.05f) >((1 / 128.0f)*(127-x)))
                        {
                            pixels[y][x] = GetAmplitudeColor(x, 128);
                        }
                    }
                    else
                    {
                        if ((fft[5] - 0.05f) >((1 / 128.0f)*((x-128))))
                        {
                            pixels[y][x] = GetAmplitudeColor(127-(x-128), 128);
                        }
                    }
                }
			}
		}

		//Increment background step
		bkgd_step++;
	}
}

void Visualizer::RazerKeyboardUpdateThread()
{
	while (1)
	{
		rkb.SetLEDs(pixels);
		Sleep(delay);
	}
}

void Visualizer::CorsairKeyboardUpdateThread()
{
    while (1)
    {
        ckb.SetLEDs(pixels);
        Sleep(delay);
    }
}