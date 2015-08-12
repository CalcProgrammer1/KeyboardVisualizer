#include "Visualizer.h"
#include "RazerKeyboard.h"
#include "CorsairKeyboard.h"

RazerKeyboard rkb;
//CorsairKeyboard ckb;

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
	//ckb.Initialize();

	amplitude   = 150;
	avg_size    = 4;
	bkgd_step   = 0;
	bkgd_bright = 10;
	bkgd_mode   = 2;
	delay       = 25;
	window_mode = 1;
	decay       = 80;
    frgd_mode   = 0;

	hanning(win_hanning, 256);
	hamming(win_hamming, 256);
	blackman(win_blackman, 256);

	for (int i = 0; i < 256; i++)
	{
		fft[i] = 0.0f;
		fft_nrml[i] = 0.05f + (0.50f * (i / 256.0f));
	}
}

void Visualizer::ChangeDevice()
{
    if (device != NULL)
    {
        alcCaptureStop(device);
        alcCaptureCloseDevice(device);
    }

    device = alcCaptureOpenDevice(device_list[device_idx].c_str(), 8000, AL_FORMAT_MONO8, 2048);
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
	for (int i = 0; i < 256; i += 2)
	{
		float fftmag;

		//Compute magnitude from real and imaginary components of FFT and apply simple LPF
		fftmag = (float)sqrt((fft_tmp[i] * fft_tmp[i]) + (fft_tmp[i + 1] * fft_tmp[i + 1]));

		//Limit FFT magnitude to 1.0
		if (fftmag > 1.0f)
		{
			fftmag = 1.0f;
		}

		//Update to new values only if greater than previous values
		if (fftmag > fft[i])
		{
			fft[i] = fftmag;;
		}

		//Prevent from going negative
		if (fft[i] < 0.0f)
		{
			fft[i] = 0.0f;
		}

		//Set odd indexes to match their corresponding even index, as the FFT input array uses two indices for one value (real+imaginary)
		fft[i + 1] = fft[i];
	}

	//Apply averaging over given number of values
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

void Visualizer::StartThread()
{
	_beginthread(thread, 0, this);
	_beginthread(rkbthread, 0, this);
    //_beginthread(ckbthread, 0, this);
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

				//Draw visualizer bars
                switch (frgd_mode)
                {
                case 0:
                    if (fft[x] > ((1 / 64.0f)*(64.0f - y)))
                    {
                        pixels[y][x] = RGB(255, 255, 255);
                    }
                    break;

                case 1:
                    if (fft[x] > ((1 / 64.0f)*(64.0f - y)))
                    {
                        if (y > (2 * (64 / 3)))
                        {
                            pixels[y][x] = RGB(0, 255, 0);
                        }
                        else if (y > (64 / 3))
                        {
                            pixels[y][x] = RGB(255, 255, 0);
                        }
                        else
                        {
                            pixels[y][x] = RGB(255, 0, 0);
                        }
                    }
                    break;
                }
			}
		}

		//Increment background step
		bkgd_step++;



		//Wait for the next update cycle
		Sleep(delay);
	}
}

void Visualizer::RazerKeyboardUpdateThread()
{
	while (1)
	{
		rkb.SetLEDs(pixels);
		Sleep(25);
	}
}

void Visualizer::CorsairKeyboardUpdateThread()
{
    while (1)
    {
        //ckb.SetLEDs(pixels);
        Sleep(25);
    }
}