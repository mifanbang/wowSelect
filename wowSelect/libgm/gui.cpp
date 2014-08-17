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


#include "gui.h"


// export GUI static members 
GUI::_GUIManager GUI::GUIManager;
HFONT GUI::hDefaultFont = NULL;



GUI::GUI(GUI* parent) {
	m_lpParent = parent;
	m_hWnd = NULL;

	// create font
	if (!hDefaultFont) {
		LOGFONT lf;
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

		// some default value of GMlib
		lf.lfHeight = -12;
		lf.lfWidth = 0;
		lf.lfWeight = FW_NORMAL;
		lf.lfItalic = lf.lfStrikeOut = lf.lfUnderline = 0;

		hDefaultFont = CreateFontIndirect(&lf);
	}
}


GUI::~GUI() {
}


void GUI::createGUI(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, GRect* geometry, HMENU hMenu, LPVOID lpParam) {
	m_hWnd = CreateWindowEx(
		dwExStyle,
		lpClassName,
		lpWindowName,
		dwStyle,
		m_grGeometry.x = geometry->x,
		m_grGeometry.y = geometry->y,
		m_grGeometry.width = geometry->width,
		m_grGeometry.height = geometry->height,
		(m_lpParent ? m_lpParent->m_hWnd : NULL),
		hMenu,
		GetModuleHandle(NULL),
		lpParam
	);

	// setup font
	SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hDefaultFont, (LPARAM)true);

	resize(geometry->width, geometry->height);
}


bool GUI::GUIProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return false;
}


bool GUI::hasFlag(int nIndex, long dwFlag) {
	long flags = GetWindowLong(m_hWnd, nIndex);
	return (flags & dwFlag) != 0;
}


void GUI::setFlag(int nIndex, long dwFlag, bool bSet) {
	long flags = GetWindowLong(m_hWnd, nIndex);
	if (bSet)
		flags |= dwFlag;
	else
		flags &= ~dwFlag;
	SetWindowLong(m_hWnd, nIndex, flags);
}


void GUI::setEnabled(bool bEnable) {
	EnableWindow(m_hWnd, bEnable);
}


void GUI::getGeometry(GRect* rect) {
	RECT tmp;
	GetWindowRect(m_hWnd, &tmp);

	rect->x = m_grGeometry.x = tmp.left;
	rect->y = m_grGeometry.x = tmp.top;
	rect->width = m_grGeometry.width = tmp.right - tmp.left;
	rect->height = m_grGeometry.height = tmp.bottom - tmp.top;
}


void GUI::setGeometry(GRect* rect) {
	moveTo(rect->x, rect->y);
	resize(rect->width, rect->height);
}


void GUI::setGeometry(int x, int y, int width, int height) {
	moveTo(x, y);
	resize(width, height);
}


void GUI::moveTo(int x, int y) {
	m_grGeometry.x = x;
	m_grGeometry.y = y;

	SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


void GUI::resize(int width, int height) {
	m_grGeometry.width = width;
	m_grGeometry.height = height;

	SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
}


void GUI::resizeClientArea(int width, int height) {
	m_grGeometry.width = width;
	m_grGeometry.height = height;

	RECT rectWindow, rectClient;
	GetWindowRect(m_hWnd, &rectWindow);
	GetClientRect(m_hWnd, &rectClient);

	LONG lBorderTop = rectClient.top - rectWindow.top;
	LONG lBorderBottom = rectWindow.bottom - rectClient.bottom;
	LONG lBorderLeft = rectClient.left - rectWindow.left;
	LONG lBorderRight = rectWindow.right - rectClient.right;

	LONG lBorderX = rectWindow.right - rectWindow.left - rectClient.right;
	LONG lBorderY = rectWindow.bottom - rectWindow.top - rectClient.bottom;

	SetWindowPos(m_hWnd, NULL, 0, 0, width + lBorderX, height + lBorderY, SWP_NOZORDER | SWP_NOMOVE);
//	SetWindowPos(m_hWnd, NULL, 0, 0, width + lBorderLeft + lBorderRight, height + lBorderTop + lBorderBottom, SWP_NOZORDER | SWP_NOMOVE);
}


void GUI::show() {
	if (m_hWnd == NULL)
		return;

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);
}


void GUI::hide() {
	if (m_hWnd == NULL)
		return;

	ShowWindow(m_hWnd, SW_HIDE);
}


DWORD GUI::getText(LPTSTR lpBuffer, DWORD dwSize) {
	return GetWindowText(m_hWnd, lpBuffer, dwSize);
}


void GUI::setText(LPCTSTR lpString) {
	SetWindowText(m_hWnd, lpString);
}


ATOM GUI::_GUIManager::registerWindowClass(LPCTSTR szName) {
	std::map<LPCTSTR, ATOM>::iterator itr = windowClassList.find(szName);

	if (itr == windowClassList.end()) {
		ATOM atom;
		WNDCLASS wndclass;

		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = GWinProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0 ;
		wndclass.hInstance = GetModuleHandle(NULL);
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH) COLOR_BTNSHADOW;
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szName;

		atom = RegisterClass(&wndclass);
		if (atom != 0)
			windowClassList[szName] = atom;
		return atom;
	}

	return itr->second;
}


void GUI::_GUIManager::registerGUI(GUI* gui) {
	if (gui == NULL)
		return;

	if (mainWindow == NULL) {
		// check if the GUI is a registered GWindow
		if (windowClassList.find(gui->getClassName()) != windowClassList.end())
			mainWindow = gui;
	}

	guiList[gui->m_hWnd] = gui;
	dwGuiCounter++;
}


void GUI::_GUIManager::unregisterGUI(GUI* gui) {
	if (gui == NULL)
		return;

	if (mainWindow == gui)
		mainWindow = NULL;

	guiList.erase(gui->m_hWnd);
}


GUI* GUI::_GUIManager::findGUI(HWND hWnd) {
	std::map<HWND, GUI*>::iterator itr = guiList.find(hWnd);

	return (itr == guiList.end() ? NULL : itr->second);
}
