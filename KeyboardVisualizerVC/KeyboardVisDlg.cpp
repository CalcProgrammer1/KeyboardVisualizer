// KeyboardVisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KeyboardVisDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <process.h>

// KeyboardVisDlg dialog

// tray icon ID
#define ID_SYSTEMTRAY 0x1000

// custom message ID
#define WM_TRAYICON_EVENT (WM_APP + 1)

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
    NOTIFYICONDATA Tray;

    Tray.cbSize = sizeof(Tray);
    Tray.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON));
    Tray.hWnd = GetSafeHwnd();
    strcpy(Tray.szTip, "Keyboard Visualizer");
    Tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    Tray.uID = ID_SYSTEMTRAY;
    Tray.uCallbackMessage = WM_TRAYICON_EVENT;

    Shell_NotifyIcon(NIM_ADD, &Tray);

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
    bkgdModeBox->AddString("Color Wheel");
	bkgdModeBox->SetCurSel(vis->bkgd_mode);

    CComboBox* frgdModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_FRGD_MODE);
    frgdModeBox->AddString("White");
    frgdModeBox->AddString("Red");
    frgdModeBox->AddString("Orange");
    frgdModeBox->AddString("Yellow");
    frgdModeBox->AddString("Green");
    frgdModeBox->AddString("Cyan");
    frgdModeBox->AddString("Blue");
    frgdModeBox->AddString("Purple");
    frgdModeBox->AddString("Green/Yellow/Red");
    frgdModeBox->AddString("Blue/Cyan/White");
    frgdModeBox->AddString("Red/White/Blue");
    frgdModeBox->AddString("Rainbow");
    frgdModeBox->AddString("Rainbow Inverse");
    frgdModeBox->SetCurSel(vis->frgd_mode);

    CComboBox* snglClrModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_SNGL_CLR_MODE);
    snglClrModeBox->AddString("None");
    snglClrModeBox->AddString("Follow Foreground");
    snglClrModeBox->AddString("Follow Background");
    snglClrModeBox->AddString("White");
    snglClrModeBox->AddString("Red");
    snglClrModeBox->AddString("Orange");
    snglClrModeBox->AddString("Yellow");
    snglClrModeBox->AddString("Green");
    snglClrModeBox->AddString("Cyan");
    snglClrModeBox->AddString("Blue");
    snglClrModeBox->AddString("Purple");
    snglClrModeBox->SetCurSel(vis->single_color_mode);

    CComboBox* avgModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_AVG_MODE);
    avgModeBox->AddString("Binning");
    avgModeBox->AddString("Low Pass");
    avgModeBox->SetCurSel(vis->avg_mode);

	timer = SetTimer(1, 25, NULL);

	return TRUE;
}

void KeyboardVisDlg::OnDestroy()
{
    NOTIFYICONDATA Tray;

    Tray.cbSize = sizeof(Tray);
    Tray.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON));
    Tray.hWnd = GetSafeHwnd();
    strcpy(Tray.szTip, "Keyboard Visualizer");
    Tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    Tray.uID = ID_SYSTEMTRAY;
    Tray.uCallbackMessage = WM_TRAYICON_EVENT;

    Shell_NotifyIcon(NIM_DELETE, &Tray);
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

LRESULT KeyboardVisDlg::OnTrayIconEvent(WPARAM wParam, LPARAM lParam)
{
    if ((UINT)wParam != ID_SYSTEMTRAY)
        return ERROR_SUCCESS;

    switch ((UINT)lParam)
    {
        case WM_LBUTTONUP:
        {
            if (IsWindowVisible())
            {
                KillTimer(timer);
                ShowWindow(SW_HIDE);
            }
            else
            {
                ShowWindow(SW_SHOW);
                timer = SetTimer(1, 25, NULL);
            }
            break;
        }
    }

    return ERROR_SUCCESS;
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
    ON_WM_DESTROY()
    ON_MESSAGE(WM_TRAYICON_EVENT, OnTrayIconEvent)
    ON_CBN_SELCHANGE(IDC_COMBO_AVG_MODE, &KeyboardVisDlg::OnCbnSelchangeComboAvgMode)
    ON_CBN_SELCHANGE(IDC_COMBO_SNGL_CLR_MODE, &KeyboardVisDlg::OnCbnSelchangeComboSnglClrMode)
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


void KeyboardVisDlg::OnCbnSelchangeComboAvgMode()
{
    vis->avg_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_AVG_MODE))->GetCurSel();
}


void KeyboardVisDlg::OnCbnSelchangeComboSnglClrMode()
{
    vis->single_color_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_SNGL_CLR_MODE))->GetCurSel();
}
