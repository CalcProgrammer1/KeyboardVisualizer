#pragma once

#include "stdafx.h"
#include "Visualizer.h"
// KeyboardVisDlg dialog

class KeyboardVisDlg : public CDialogEx
{
	DECLARE_DYNAMIC(KeyboardVisDlg)

public:
	KeyboardVisDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KeyboardVisDlg();
	virtual BOOL OnInitDialog();

	void SetVisualizer(Visualizer* v);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

private:
    afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
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
};
