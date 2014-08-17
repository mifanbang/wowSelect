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


#ifndef __GWINDOW_H
#define __GWINDOW_H

#include "gui.h"



class GWindow : public GUI {
	GUI_CLASS("GWindow")

protected:
	virtual void initGUI();
	virtual bool GUIProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// event handlers
	virtual void onCreate() { };
	virtual void onClose() { };
	virtual void onAction(GUI* gui, WORD wMsg) { }

	HWND m_hOldFocus;

public:
	// constructors
	GWindow(GUI *parent = NULL);
	~GWindow();

	bool isResizable();
	void setResizable(bool set);

	void setAlpha(BYTE alpha);

	void destroyLater();
	void showModal();
};


#endif  // __GWINDOW_H
