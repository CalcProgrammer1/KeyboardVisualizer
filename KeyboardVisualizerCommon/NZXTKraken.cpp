
#include "NZXTKraken.h"

#include "UsbDevice.h"

static UsbDevice usb;
static bool init_ok = true;

NZXTKraken::NZXTKraken()
{
}


NZXTKraken::~NZXTKraken()
{
}


void NZXTKraken::Initialize()
{
	if (usb.OpenDevice(0x1e71, 0x170e, 0))
	{
		init_ok = true;
	}
	else
	{
		init_ok = false;
	}
	SetNumLEDs();
}

void NZXTKraken::SetNumLEDs()
{
	int y_index = ROW_IDX_BAR_GRAPH;

	num_leds = 9;

	LEDStripXIndex = new int[num_leds];
	LEDStripYIndex = new int[num_leds];

	
	for (int i = 0; i < num_leds; i++)
	{
		int x_index = i ;

		LEDStripYIndex[i] = y_index;


		if (x_index == (num_leds / 2))
		{
			LEDStripXIndex[i] = 128;
		}
		else if (x_index < ((num_leds / 2) + 1))
		{
			LEDStripXIndex[i] = (num_leds / 2) + ((x_index + 1) * (256 / (num_leds + 1)));
		}
		else
		{
			LEDStripXIndex[i] = ((num_leds / 2) + 1) + (x_index * (256 / (num_leds + 1)));
		}

	}
}

bool NZXTKraken::SetLEDs(COLORREF pixels[64][256])
{

	if (!init_ok)
	{
		return false;
	}

	unsigned char buf[65] = { 0 };

	buf[0] = 0x02;
	buf[1] = 0x4c;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x02;

	for (int x = 0; x < 27; x += 3)
	{
		if (x != 0 && x < 12) {
			int pixel_idx = (x - 3) / 3;
			COLORREF color = pixels[LEDStripYIndex[pixel_idx]][LEDStripXIndex[pixel_idx]];
			buf[5 + x] = GetRValue(color);
			buf[6 + x] = GetGValue(color);
			buf[7 + x] = GetBValue(color);
		}
		else
			if (x == 12) {
				int pixel_idx = (x) / 3;
				COLORREF color = pixels[LEDStripYIndex[pixel_idx]][LEDStripXIndex[pixel_idx]];
				buf[5] = GetGValue(color);
				buf[6] = GetRValue(color);
				buf[7] = GetBValue(color);
				pixel_idx = (x - 3) / 3;
				color = pixels[LEDStripYIndex[pixel_idx]][LEDStripXIndex[pixel_idx]];
				buf[5 + x] = GetRValue(color);
				buf[6 + x] = GetGValue(color);
				buf[7 + x] = GetBValue(color);
			}
			else
				if (x > 12) {
					int pixel_idx = (x) / 3;
					COLORREF color = pixels[LEDStripYIndex[pixel_idx]][LEDStripXIndex[pixel_idx]];
					buf[5 + x] = GetRValue(color);
					buf[6 + x] = GetGValue(color);
					buf[7 + x] = GetBValue(color);
				}


	}


	usb.SendToDevice(buf, 65);

	return init_ok;
}
