#pragma once

class AppSettings {
public:
	AppSettings();

	//Initialize default settings
	void defaultSettings();

	//Creates a default settings file if it does not exist
	void initSettingsFile();

	//Loads settings from file
	void loadSettings();

	//Saves settings to file
	void saveSettings();

	//_____ Settings _____//
	//Amplitude of input waveform
	int amplitude;
	int avg_mode;
	int avg_size;
	int window_mode;
	int decay;
	int delay;
	int ldstrp_pos;
	//Background Variables
	int bkgd_bright;
	int bkgd_mode;
	int bkgd_step;
	//Single Color Mode
	int single_color_mode;
	//Normalization Offset and Scale
	float nrml_ofst;
	float nrml_scl;
	//Foreground Variables
	int frgd_mode;
	//Device specific modes
	int fireflymode;
	int blkwdwmode;
	//App settings
	bool rememberSettingsOnExit;
};