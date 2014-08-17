/*
 *  gmlib - a lightweight and dependency-free Win32 gui library
 *  Copyright (C) 2009-2010 Mifan Bang <http://debug.tw>.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "gwindow.h"



GWindow::GWindow(GUI *parent) : GUI(parent) {
	m_hOldFocus = NULL;

	GUI::GUIManager.registerWindowClass(getClassName());
	initGUI();
}


GWindow::~GWindow() {
	// send WM_CLOSE to the window procedure
	CallWindowProc(GWinProc, m_hWnd, WM_CLOSE, NULL, NULL);
}


void GWindow::initGUI() {
	GRect geometry;
	geometry.x = CW_USEDEFAULT;
	geometry.y = CW_USEDEFAULT;
	geometry.width = 400;
	geometry.height = 400;

	// CreateWindow
	createGUI(NULL, getClassName(), getClassName(), WS_OVERLAPPEDWINDOW | WS_TABSTOP, &geometry, NULL, this);
	GUI::GUIManager.registerGUI(this);

	// produce a self-defined G_WM_CREATE message so that GWinProc will relay to GWindow::GUIProc later
	PostMessage(m_hWnd, G_WM_CREATE, NULL, NULL);
}


bool GWindow::GUIProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case G_WM_CREATE:
		onCreate();
		break;

	case WM_CLOSE:
		onClose();
		destroyLater();
		break;

	case WM_DESTROY:
		GUIManager.unregisterGUI(this);
		break;

	// child GUI event
	case WM_COMMAND:
		// read MSDN for meaning of lParam when uMsg==WM_COMMAND
		if (lParam != 0) {
			GUI* gui = GUI::GUIManager.findGUI((HWND)lParam);
			if (gui != NULL)
				onAction(gui, (wParam >> 16) & 0xFFFF);
		}
		break;

	case WM_ACTIVATE:
		// we store HWND instead of GUI to be more general
		if ((wParam & 0xFFFF) == WA_INACTIVE) {
			if (GetFocus())
				m_hOldFocus = GetFocus();
		}
		else if (m_hOldFocus) {
			SetFocus(m_hOldFocus);
		}
		break;

	default:
		return GUI::GUIProc(uMsg, wParam, lParam);
	}

	return true;
}


bool GWindow::isResizable() {
	return hasFlag(GWL_STYLE, WS_SIZEBOX);
}


void GWindow::setResizable(bool set) {
	if (set == isResizable())
		return;

	RECT rectWindow;
	GetWindowRect(m_hWnd, &rectWindow);

	int nBorderX = GetSystemMetrics(SM_CXSIZEFRAME) - GetSystemMetrics(SM_CXFIXEDFRAME);
	int nBorderY = GetSystemMetrics(SM_CYSIZEFRAME) - GetSystemMetrics(SM_CYFIXEDFRAME);
	if (set) {
		nBorderX = -nBorderX;
		nBorderY = -nBorderY;
	}

	setFlag(GWL_STYLE, WS_THICKFRAME, set);
	setFlag(GWL_STYLE, WS_MAXIMIZEBOX, set);
	SetWindowPos(
		m_hWnd,
		NULL,
		rectWindow.left + nBorderX,
		rectWindow.top + nBorderY,
		rectWindow.right - rectWindow.left - (nBorderX << 1),
		rectWindow.bottom - rectWindow.top - (nBorderY << 1),
		SWP_FRAMECHANGED | SWP_NOZORDER
	);

	getGeometry(&m_grGeometry);
}


void GWindow::setAlpha(BYTE alpha) {
	setFlag(GWL_EXSTYLE, WS_EX_LAYERED, true);  // enable layered window style
	SetLayeredWindowAttributes(m_hWnd, RGB(0, 0, 0), alpha, LWA_ALPHA);  // requires Windows 2000+
}


void GWindow::destroyLater() {
	PostMessage(getHandle(), WM_DESTROY, NULL, NULL);
}


void GWindow::showModal() {
	if (m_lpParent == NULL) {
		show();
		return;
	}

	m_lpParent->setEnabled(false);
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	MSG msg;
	while (true) {
		GetMessage(&msg, NULL, 0, 0);

		// test for end criteria
		if (LOWORD(msg.message) == WM_QUIT) {
			PostMessage(NULL, WM_QUIT, 0, 0);
			break;
		}
		if (LOWORD(msg.message) == WM_DESTROY && msg.hwnd == getHandle()) {
			m_lpParent->setEnabled(true);
			DispatchMessage(&msg);
			break;
		}

		// process dialog intended messages, say TAB stop support
		GUI* gui = GUI::GUIManager.findGUI(msg.hwnd);
		if (gui != NULL) {
			if (gui->getParent() != NULL) {
				if (IsDialogMessage(GetAncestor(msg.hwnd, GA_ROOTOWNER), &msg))
					continue;
			}
			else {
				if (IsDialogMessage(msg.hwnd, &msg))
					continue;
			}
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
