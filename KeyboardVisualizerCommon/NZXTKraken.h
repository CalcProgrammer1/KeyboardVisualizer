#ifndef NZXT_KRAKEN_H
#define NZXT_KRAKEN_H

#include <sstream>
#include <wchar.h>
#include <string.h>

#include "VisualizerDefines.h"

class NZXTKraken
{
public:
	NZXTKraken();
	~NZXTKraken();

	void Initialize();
	bool SetLEDs(COLORREF pixels[64][256]);
	void SetNumLEDs();

private:
	int num_leds;
	int * LEDStripXIndex;
	int * LEDStripYIndex;
};

#endif
