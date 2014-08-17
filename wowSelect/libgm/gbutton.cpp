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


#include "gbutton.h"



GAbstractButton::GAbstractButton(GUI *parent, DWORD dwType) : GUI(parent) {
	m_dwType = dwType;

	initGUI();
}


GAbstractButton::~GAbstractButton() {
	GUI::GUIManager.unregisterGUI(this);
}


void GAbstractButton::initGUI() {
	GRect geometry;
	geometry.x = CW_USEDEFAULT;
	geometry.y = CW_USEDEFAULT;
	geometry.width = 100;
	geometry.height = 24;

	createGUI(NULL, _T("Button"), getClassName(), WS_CHILD | WS_VISIBLE | WS_TABSTOP | m_dwType, &geometry, (HMENU)GUI::GUIManager.getGUICounter(), NULL);
	GUI::GUIManager.registerGUI(this);
}


bool GAbstractButton::isChecked() {
	return SendMessage(m_hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}


void GAbstractButton::setChecked(bool bChecked) {
	SendMessage(m_hWnd, BM_SETCHECK, (bChecked ? BST_CHECKED : BST_UNCHECKED), 0);
}
