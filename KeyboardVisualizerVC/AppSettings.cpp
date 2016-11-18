#include <fstream>
#include <string>
using namespace std;
using std::string;

#include "AppSettings.h"
string path = "settings.txt";

AppSettings::AppSettings() { }

void AppSettings::defaultSettings() {
	rememberSettingsOnExit = true;
	amplitude = 100;
	avg_mode = 0;
	avg_size = 8;
	bkgd_step = 0;
	bkgd_bright = 10;
	bkgd_mode = 0;
	delay = 50;
	window_mode = 1;
	decay = 80;
	frgd_mode = 8;
	ldstrp_pos = 0;
	nrml_ofst = 0.04f;
	nrml_scl = 0.5f;
	single_color_mode = 1;
	fireflymode = 0;
	blkwdwmode = 0;
}

void AppSettings::saveSettings() {
	ofstream fout(path);
	if (fout.is_open()) {
		fout
			<< rememberSettingsOnExit << endl
			<< amplitude << endl
			<< avg_mode << endl
			<< avg_size << endl
			<< bkgd_step << endl
			<< bkgd_bright << endl
			<< bkgd_mode << endl
			<< delay << endl
			<< window_mode << endl
			<< decay << endl
			<< frgd_mode << endl
			<< ldstrp_pos << endl
			<< nrml_ofst << endl
			<< nrml_scl << endl
			<< single_color_mode << endl
			<< fireflymode << endl
			<< blkwdwmode << endl;
		fout.close();
	}
}

void AppSettings::loadSettings() {
	ifstream fin;
	try {
		fin.open(path);
		if (fin.is_open()) {
			fin
				>> rememberSettingsOnExit
				>> amplitude
				>> avg_mode
				>> avg_size
				>> bkgd_step
				>> bkgd_bright
				>> bkgd_mode
				>> delay
				>> window_mode
				>> decay
				>> frgd_mode
				>> ldstrp_pos
				>> nrml_ofst
				>> nrml_scl
				>> single_color_mode
				>> fireflymode
				>> blkwdwmode;
			fin.close();
		} else {
			initSettingsFile();
		}
	} catch (...) {
		defaultSettings();
	}
}

void AppSettings::initSettingsFile() {
	defaultSettings();
	saveSettings();
}

bool is_file_exist(const char *fileName) {
	std::ifstream infile(fileName);
	return infile.good();
}