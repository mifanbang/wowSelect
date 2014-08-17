#include "gprogressbar.h"
#include "commctrl.h"

#pragma comment(lib, "comctl32.lib")


GProgressBar::GProgressBar(GUI *parent) : GUI(parent) {
	initGUI();
}


GProgressBar::~GProgressBar() {
	GUI::GUIManager.unregisterGUI(this);
}


void GProgressBar::initGUI() {
	GRect geometry;
	geometry.x = CW_USEDEFAULT;
	geometry.y = CW_USEDEFAULT;
	geometry.width = 120;
	geometry.height = 21;

	// initialize common controls
	INITCOMMONCONTROLSEX initCtrls = {sizeof(initCtrls)};
	initCtrls.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&initCtrls);

#ifndef PBS_SMOOTHREVERSE 
	#define PBS_SMOOTHREVERSE	0x10
#endif
	createGUI(NULL, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_SMOOTHREVERSE, &geometry, (HMENU)GUI::GUIManager.getGUICounter(), NULL);
	GUI::GUIManager.registerGUI(this);
}


UINT GProgressBar::getMaximum() {
	// return high limit, because we assume low limit is 0
	return (UINT)SendMessage(m_hWnd, PBM_GETRANGE, (WPARAM)FALSE, (LPARAM)NULL);
}


void GProgressBar::setMaximum(UINT uMax) {
	SendMessage(m_hWnd, PBM_SETRANGE32, (WPARAM)0, (LPARAM)uMax);
}


UINT GProgressBar::getPosition() {
	return (UINT)SendMessage(m_hWnd, PBM_GETPOS, (WPARAM)0, (LPARAM)0);
}


void GProgressBar::setPosition(UINT uPos) {
	SendMessage(m_hWnd, PBM_SETPOS, (WPARAM)uPos, (LPARAM)0);
}


void GProgressBar::advance(int nIncrement) {
	SendMessage(m_hWnd, PBM_DELTAPOS, (WPARAM)nIncrement, (LPARAM)0);
}
