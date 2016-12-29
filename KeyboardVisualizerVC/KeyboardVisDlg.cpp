/*---------------------------------------------------------*\
|  Processing Code for Keyboard Visualizer Dialog           |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

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
boolean startminimized;
boolean firstrun;

KeyboardVisDlg::KeyboardVisDlg(CWnd* pParent)
	: CDialogEx(IDD_KEYBOARD_VISUALIZER_DLG, pParent)
{
    startminimized = FALSE;
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
    Tray.hIcon = (HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_SHARED);//LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON));
    Tray.hWnd = GetSafeHwnd();
    strcpy(Tray.szTip, "Keyboard Visualizer");
    Tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    Tray.uID = ID_SYSTEMTRAY;
    Tray.uCallbackMessage = WM_TRAYICON_EVENT;

    Shell_NotifyIcon(NIM_ADD, &Tray);

    char nrml_ofst_str[64];
    char nrml_scl_str[64];
    char anim_speed_str[64];

    snprintf(nrml_ofst_str, 64, "%f", vis->nrml_ofst);
    snprintf(nrml_scl_str,  64, "%f", vis->nrml_scl);
    snprintf(anim_speed_str, 64, "%f", vis->anim_speed);

	SetDlgItemInt(IDC_EDIT_AMPLITUDE, vis->amplitude);
	SetDlgItemInt(IDC_EDIT_BACKGROUND_BRIGHTNESS, vis->bkgd_bright);
	SetDlgItemInt(IDC_EDIT_AVERAGE_SIZE, vis->avg_size);
	SetDlgItemInt(IDC_EDIT_DECAY, vis->decay);
	SetDlgItemInt(IDC_EDIT_DELAY, vis->delay);
    SetDlgItemText(IDC_EDIT_ANIM_SPEED, anim_speed_str);
    SetDlgItemText(IDC_EDIT_NRML_OFST, nrml_ofst_str);
    SetDlgItemText(IDC_EDIT_NRML_SCL, nrml_scl_str);

	CComboBox* windowBox = (CComboBox*)GetDlgItem(IDC_COMBO_WINDOW);
	windowBox->AddString("None");
	windowBox->AddString("Hanning");
	windowBox->AddString("Hamming");
	windowBox->AddString("Blackman");
	windowBox->SetCurSel(vis->window_mode);

	CComboBox* bkgdModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_BKGD_MODE);
    for (int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        bkgdModeBox->AddString(visualizer_pattern_labels[i]);
    }
	bkgdModeBox->SetCurSel(vis->bkgd_mode);

    CComboBox* frgdModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_FRGD_MODE);
    for (int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        frgdModeBox->AddString(visualizer_pattern_labels[i]);
    }
    frgdModeBox->SetCurSel(vis->frgd_mode);

    CComboBox* snglClrModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_SNGL_CLR_MODE);
    for (int i = 0; i < VISUALIZER_NUM_SINGLE_COLOR; i++)
    {
        snglClrModeBox->AddString(visualizer_single_color_labels[i]);
    }
    snglClrModeBox->SetCurSel(vis->single_color_mode);

    CComboBox* avgModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_AVG_MODE);
    avgModeBox->AddString("Binning");
    avgModeBox->AddString("Low Pass");
    avgModeBox->SetCurSel(vis->avg_mode);

	timer = SetTimer(1, 25, NULL);

    firstrun = TRUE;

	return TRUE;
}

void KeyboardVisDlg::StartMinimized(boolean startmin)
{
    startminimized = startmin;
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
    if (startminimized && firstrun)
    {
        firstrun = FALSE;
        ShowWindow(SW_HIDE);
    }

    COLORREF pixels_bgr[64][256];

    //CreateBitmap uses BGR color layout, convert from RGB
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            if (vis->pixels_out != NULL)
            {
                pixels_bgr[y][x] = RGB2BGR(vis->pixels_out->pixels[y][x]);
            }
        }
    }

	HBITMAP hBitmap = CreateBitmap(256, 64, 1, 32, pixels_bgr);
	((CStatic *)GetDlgItem(IDC_IMAGE_VISUALIZATION))->SetBitmap(hBitmap);
	DeleteObject(hBitmap);

    if (vis->update_ui)
    {
        char nrml_ofst_str[64];
        char nrml_scl_str[64];
        char anim_speed_str[64];

        vis->update_ui = false;

        snprintf(nrml_ofst_str, 64, "%f", vis->nrml_ofst);
        snprintf(nrml_scl_str, 64, "%f", vis->nrml_scl);
        snprintf(anim_speed_str, 64, "%f", vis->anim_speed);

        SetDlgItemInt(IDC_EDIT_AMPLITUDE, vis->amplitude);
        SetDlgItemInt(IDC_EDIT_BACKGROUND_BRIGHTNESS, vis->bkgd_bright);
        SetDlgItemInt(IDC_EDIT_AVERAGE_SIZE, vis->avg_size);
        SetDlgItemInt(IDC_EDIT_DECAY, vis->decay);
        SetDlgItemInt(IDC_EDIT_DELAY, vis->delay);
        SetDlgItemText(IDC_EDIT_ANIM_SPEED, anim_speed_str);
        SetDlgItemText(IDC_EDIT_NRML_OFST, nrml_ofst_str);
        SetDlgItemText(IDC_EDIT_NRML_SCL, nrml_scl_str);

        CComboBox* windowBox = (CComboBox*)GetDlgItem(IDC_COMBO_WINDOW);
        windowBox->SetCurSel(vis->window_mode);

        CComboBox* bkgdModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_BKGD_MODE);
        bkgdModeBox->SetCurSel(vis->bkgd_mode);

        CComboBox* frgdModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_FRGD_MODE);
        frgdModeBox->SetCurSel(vis->frgd_mode);

        CComboBox* snglClrModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_SNGL_CLR_MODE);
        snglClrModeBox->SetCurSel(vis->single_color_mode);

        CComboBox* avgModeBox = (CComboBox*)GetDlgItem(IDC_COMBO_AVG_MODE);
        avgModeBox->SetCurSel(vis->avg_mode);
    }
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
    ON_EN_CHANGE(IDC_EDIT_NRML_OFST, &KeyboardVisDlg::OnEnChangedEditNrmlOfst)
    ON_EN_CHANGE(IDC_EDIT_NRML_SCL, &KeyboardVisDlg::OnEnChangedEditNrmlScl)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &KeyboardVisDlg::OnClickedSave)
    ON_EN_CHANGE(IDC_EDIT_ANIM_SPEED, &KeyboardVisDlg::OnEnChangeEditAnimSpeed)
END_MESSAGE_MAP()

void KeyboardVisDlg::OnEnChangeEditAmplitude()
{
	vis->amplitude = GetDlgItemInt(IDC_EDIT_AMPLITUDE, 0, 0);
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnEnChangeEditBackgroundBrightness()
{
	vis->bkgd_bright = (int)(GetDlgItemInt(IDC_EDIT_BACKGROUND_BRIGHTNESS, 0, 0));
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnCbnSelchangeComboWindow()
{
	vis->window_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_WINDOW))->GetCurSel();
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnEnChangeEditAverageSize()
{
	vis->avg_size = (int)GetDlgItemInt(IDC_EDIT_AVERAGE_SIZE, 0, 0);
    if (vis->avg_size < 1)
    {
        vis->avg_size = 1;
    }
    else if (vis->avg_size > 128)
    {
        vis->avg_size = 128;
    }
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnCbnSelchangeComboBkgdMode()
{
	vis->bkgd_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_BKGD_MODE))->GetCurSel();
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnEnChangeEditDecay()
{
	vis->decay = (int)GetDlgItemInt(IDC_EDIT_DECAY, 0, 0);
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnEnChangeEditDelay()
{
	vis->delay = (int)GetDlgItemInt(IDC_EDIT_DELAY, 0, 0);
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnCbnSelchangeComboFrgdMode()
{
    vis->frgd_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_FRGD_MODE))->GetCurSel();
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnCbnSelchangeComboAvgMode()
{
    vis->avg_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_AVG_MODE))->GetCurSel();
    vis->OnSettingsChanged();
}


void KeyboardVisDlg::OnCbnSelchangeComboSnglClrMode()
{
    vis->single_color_mode = ((CComboBox*)GetDlgItem(IDC_COMBO_SNGL_CLR_MODE))->GetCurSel();
    vis->OnSettingsChanged();
}

void KeyboardVisDlg::OnEnChangedEditNrmlOfst()
{
    char val[64];
    GetDlgItemText(IDC_EDIT_NRML_OFST, (LPTSTR)&val, 64);
    vis->nrml_ofst = strtod(val, NULL);
    vis->SetNormalization(vis->nrml_ofst, vis->nrml_scl);
    vis->OnSettingsChanged();
}

void KeyboardVisDlg::OnEnChangedEditNrmlScl()
{
    char val[64];
    GetDlgItemText(IDC_EDIT_NRML_SCL, (LPTSTR)&val, 64);
    vis->nrml_scl = strtod(val, NULL);
    vis->SetNormalization(vis->nrml_ofst, vis->nrml_scl);
    vis->OnSettingsChanged();
}

void KeyboardVisDlg::OnClickedSave()
{
    vis->SaveSettings();
}

void KeyboardVisDlg::OnEnChangeEditAnimSpeed()
{
    char val[64];
    GetDlgItemText(IDC_EDIT_ANIM_SPEED, (LPTSTR)&val, 64);
    vis->anim_speed = strtod(val, NULL);
    vis->OnSettingsChanged();
}
