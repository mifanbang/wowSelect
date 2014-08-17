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


#ifndef __GBUTTON_H
#define __GBUTTON_H

#include "gui.h"


// base abstraction of all sorts of buttons
class GAbstractButton : public GUI {
	GUI_CLASS("GAbstractButton")

protected:
	virtual void initGUI();

	DWORD m_dwType;

public:
	GAbstractButton(GUI *parent, DWORD dwType);
	~GAbstractButton();

	bool isChecked();
	void setChecked(bool bChecked);
};


// button
class GButton : public GAbstractButton {
	GUI_CLASS("GButton")

public:
	GButton(GUI *parent) : GAbstractButton(parent, BS_PUSHBUTTON) { };
};


// push button
class GPushButton : public GAbstractButton {
	GUI_CLASS("GPushButton")

public:
	GPushButton(GUI *parent) : GAbstractButton(parent, BS_AUTOCHECKBOX | BS_PUSHLIKE) { };
};


// checkbox
class GCheckBox : public GAbstractButton {
	GUI_CLASS("GCheckBox")

public:
	GCheckBox(GUI *parent) : GAbstractButton(parent, BS_AUTOCHECKBOX) { };
};


// radio button
class GRadioButton : public GAbstractButton {
	GUI_CLASS("GRadioButton")

public:
	GRadioButton(GUI *parent) : GAbstractButton(parent, BS_AUTORADIOBUTTON) { };
};


#endif  // __GBUTTON_H
