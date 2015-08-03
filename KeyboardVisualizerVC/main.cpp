#include "stdafx.h"
#include "resource.h"
#include "KeyboardVisDlg.h"
#include "Visualizer.h"

CWinApp app;
Visualizer vis;

int APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
	afxCurrentInstanceHandle = hInst;
	afxCurrentResourceHandle = hInst;
	app.m_hInstance = hInst;

	vis.Initialize();
	vis.StartThread();

	KeyboardVisDlg dlg;
	dlg.SetVisualizer(&vis);
	dlg.DoModal();

	return 0;
}