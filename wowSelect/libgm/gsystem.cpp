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


#include "gsystem.h"
#include "gui.h"



int GMainEventLoop() {
	MSG msg;
	GUI* gui;

	// main event loop
	while (GetMessage(&msg, NULL, 0, 0)) {

		// process dialog intended messages, say TAB stop support
		gui = GUI::GUIManager.findGUI(msg.hwnd);
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

	return (int)msg.wParam;
}


LRESULT CALLBACK GWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	GUI* gui = GUI::GUIManager.findGUI(hWnd);
	bool handled = false;

	if (gui != NULL) {

		// check for destroy event of the main window
		if (uMsg == WM_DESTROY && gui == GUI::GUIManager.getMainWindow()) {
			PostQuitMessage(0);
			return 0;
		}

		handled = gui->GUIProc(uMsg, wParam, lParam);
	}

	if (handled)
		return 0;

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

