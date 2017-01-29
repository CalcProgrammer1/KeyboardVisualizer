/*---------------------------------------------------------*\
|  Definitions for Keyboard Visualizer Dialog               |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef KEYBOARD_VIS_DLG_H
#define KEYBOARD_VIS_DLG_H

#include "stdafx.h"
#include "..\KeyboardVisualizerCommon\Visualizer.h"
// KeyboardVisDlg dialog

class KeyboardVisDlg : public CDialogEx
{
	DECLARE_DYNAMIC(KeyboardVisDlg)

public:
	KeyboardVisDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KeyboardVisDlg();
	virtual BOOL OnInitDialog();

	void SetVisualizer(Visualizer* v);
    void StartMinimized(boolean startmin);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

private:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    UINT_PTR timer;

protected:
    // WM_TRAYICON_EVENT handler
    afx_msg LRESULT OnTrayIconEvent(WPARAM wParam, LPARAM lParam);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnEnChangeEditAmplitude();
	afx_msg void OnEnChangeEditBackgroundBrightness();
	afx_msg void OnCbnSelchangeComboWindow();
	afx_msg void OnEnChangeEditAverageSize();
	afx_msg void OnCbnSelchangeComboBkgdMode();
	afx_msg void OnEnChangeEditDecay();
	afx_msg void OnEnChangeEditDelay();
    afx_msg void OnCbnSelchangeComboFrgdMode();
    afx_msg void OnCbnSelchangeComboAvgMode();
    afx_msg void OnCbnSelchangeComboSnglClrMode();
    afx_msg void OnEnChangedEditNrmlOfst();
    afx_msg void OnEnChangedEditNrmlScl();
    afx_msg void OnClickedSave();
    afx_msg void OnEnChangeEditAnimSpeed();
    afx_msg void OnBnClickedCheckReactiveBackground();
    afx_msg void OnCbnSelchangeComboAudioDevice();
    afx_msg void OnEnChangeEditFilterConstant();
    afx_msg void OnCbnDropdownComboAudioDevice();
    afx_msg void OnEnChangeEditBackgroundTimeout();
    afx_msg void OnBnClickedCheckSilentBackground();
};

#endif