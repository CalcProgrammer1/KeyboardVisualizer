// KeyboardVisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KeyboardVisDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <process.h>

// KeyboardVisDlg dialog

#define RGB2BGR(a_ulColor) (a_ulColor & 0xFF000000) | ((a_ulColor & 0xFF0000) >> 16) | (a_ulColor & 0x00FF00) | ((a_ulColor & 0x0000FF) << 16)

IMPLEMENT_DYNAMIC(KeyboardVisDlg, CDialogEx)

Visualizer* vis;

KeyboardVisDlg::KeyboardVisDlg(CWnd* pParent)
	: CDialogEx(IDD_RAZER_CHROMA_DLG, pParent)
{
}

void KeyboardVisDlg::SetVisualizer(Visualizer* v)
{
	vis = v;
}

KeyboardVisDlg::~KeyboardVisDlg()
{
}

void KeyboardVisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL KeyboardVisDlg::OnInitDialog()
{
	SetDlgItemInt(IDC_EDIT_AMPLITUDE, vis->amplitude);
	SetDlgItemInt(IDC_EDIT_BACKGROUND_BRIGHTNESS, vis->bkgd_bright);
	SetDlgItemInt(IDC_EDIT_AVERAGE_SIZE, vis->avg_size);
	SetDlgItemInt(IDC_EDIT_DECAY, vis->decay);
	SetDlgItemInt(IDC_EDIT_DELAY, vis->delay);

	CComboBox* windowBox = (CComboBox*)GetDlgItem(IDC_COMBO_WINDOW);
	windowBox->AddString("None");
	windowBox->AddString("Hanning");
	windowBox->AddString("Hamming");
	windowBox->AddString("Blackman");
	windowBox->SetCurSel(vis->window_mode);

	CComboBox* bkgdModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_BKGD_MODE);
	bkgdModeBox->AddString("None");
	bkgdModeBox->AddString("Original");
	bkgdModeBox->AddString("Rainbow");
	bkgdModeBox->SetCurSel(vis->bkgd_mode);

    CComboBox* frgdModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_FRGD_MODE);
    frgdModeBox->AddString("White");
    frgdModeBox->AddString("Green/Yellow/Red");
    frgdModeBox->SetCurSel(vis->frgd_mode);

    CComboBox* audioDeviceBox = (CComboBox*)GetDlgItem(IDC_COMBO_AUDIO_DEVICE);
    for (int i = 0; i < vis->device_list.size(); i++)
    {
        audioDeviceBox->AddString(vis->device_list[i].c_str());
    }
    audioDeviceBox->SetCurSel(vis->device_idx);

	SetTimer(1, 25, NULL);

	return TRUE;
}

void KeyboardVisDlg::OnTimer(UINT nIDEvent)
{
    COLORREF pixels_bgr[64][256];

    //CreateBitmap uses BGR color layout, convert from RGB
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels_bgr[y][x] = RGB2BGR(vis->pixels[y][x]);
        }
    }

	HBITMAP hBitmap = CreateBitmap(256, 64, 1, 32, pixels_bgr);
	((CStatic *)GetDlgItem(IDC_IMAGE_VISUALIZATION))->SetBitmap(hBitmap);
	DeleteObject(hBitmap);
}

BEGIN_MESSAGE_MAP(KeyboardVisDlg, CDialogEx)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_AMPLITUDE, &KeyboardVisDlg::OnEnChangeEditAmplitude)
	ON_EN_CHANGE(IDC_EDIT_BACKGROUND_BRIGHTNESS, &KeyboardVisDlg::OnEnChangeEditBackgroundBrightness)
	ON_CBN_SELCHANGE(IDC_COMBO_WINDOW, &KeyboardVisDlg::OnCbnSelchangeComboWindow)
	ON_EN_CHANGE(IDC_EDIT_AVERAGE_SIZE, &KeyboardVisDlg::OnEnChangeEditAverageSize)
	ON_CBN_SELCHANGE(IDC_COMBO_BKGD_MODE, &KeyboardVisDlg::OnCbnSelchangeComboBkgdMode)
	ON_EN_CHANGE(IDC_EDIT_DECAY, &KeyboardVisDlg::OnEnChangeEditDecay)
	ON_EN_CHANGE(IDC_EDIT_DELAY, &KeyboardVisDlg::OnEnChangeEditDelay)
    ON_CBN_SELCHANGE(IDC_COMBO_FRGD_MODE, &KeyboardVisDlg::OnCbnSelchangeComboFrgdMode)
    ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_DEVICE, &KeyboardVisDlg::OnCbnSelchangeComboAudioDevice)
END_MESSAGE_MAP()

void KeyboardVisDlg::OnEnChangeEditAmplitude()
{
	vis->amplitude = GetDlgItemInt(IDC_EDIT_AMPLITUDE, 0, 0);
}


void KeyboardVisDlg::OnEnChangeEditBackgroundBrightness()
{
	vis->bkgd_bright = (int)((GetDlgItemInt(IDC_EDIT_BACKGROUND_BRIGHTNESS, 0, 0) / 100.0f) * 255.0f);
}


void KeyboardVisDlg::OnCbnSelchangeComboWindow()
{
	vis->window_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_WINDOW))->GetCurSel();
}


void KeyboardVisDlg::OnEnChangeEditAverageSize()
{
	vis->avg_size = (int)GetDlgItemInt(IDC_EDIT_AVERAGE_SIZE, 0, 0);
}


void KeyboardVisDlg::OnCbnSelchangeComboBkgdMode()
{
	vis->bkgd_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_BKGD_MODE))->GetCurSel();
}


void KeyboardVisDlg::OnEnChangeEditDecay()
{
	vis->decay = (int)GetDlgItemInt(IDC_EDIT_DECAY, 0, 0);
}


void KeyboardVisDlg::OnEnChangeEditDelay()
{
	vis->delay = (int)GetDlgItemInt(IDC_EDIT_DELAY, 0, 0);
}


void KeyboardVisDlg::OnCbnSelchangeComboFrgdMode()
{
    vis->frgd_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_FRGD_MODE))->GetCurSel();
}


void KeyboardVisDlg::OnCbnSelchangeComboAudioDevice()
{
    int new_device = ((CComboBox*)GetDlgItem(IDC_COMBO_AUDIO_DEVICE))->GetCurSel();
    if (new_device != vis->device_idx)
    {
        vis->device_idx = new_device;
        vis->ChangeDevice();
    }
}
