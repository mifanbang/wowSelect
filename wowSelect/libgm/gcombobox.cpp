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


#include "gcombobox.h"



GComboBox::GComboBox(GUI *parent) : GUI(parent) {
	initGUI();
}


GComboBox::~GComboBox() {
	GUI::GUIManager.unregisterGUI(this);
}


void GComboBox::initGUI() {
	GRect geometry;
	geometry.x = CW_USEDEFAULT;
	geometry.y = CW_USEDEFAULT;
	geometry.width = 120;
	geometry.height = 21;

	createGUI(WS_EX_CLIENTEDGE, _T("Combobox"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST, &geometry, (HMENU)GUI::GUIManager.getGUICounter(), NULL);
	GUI::GUIManager.registerGUI(this);
}


DWORD GComboBox::addItem(LPCTSTR lpString) {
	DWORD ret = SendMessage(m_hWnd, CB_ADDSTRING, NULL, (LPARAM)lpString);
	if (ret == 0)
		setSelection(0);
	return ret;
}


DWORD GComboBox::getItemString(DWORD dwIndex, LPTSTR lpBuffer, DWORD dwSize) {
	int nLength = SendMessage(m_hWnd, CB_GETLBTEXTLEN, (WPARAM)dwIndex, NULL);
	if (nLength <= 0)
		return nLength;

	LPTSTR lpTemp = new TCHAR[nLength + 1];  // nLength doesn't include the terminating NULL character
	ZeroMemory(lpTemp, (nLength + 1) * sizeof(TCHAR));
	SendMessage(m_hWnd, CB_GETLBTEXT, (WPARAM)dwIndex, (LPARAM)lpTemp);
	_tcsncpy_s(lpBuffer, dwSize, lpTemp, _TRUNCATE);
	delete[] lpTemp;

	return _tcslen(lpBuffer) + 1;
}


DWORD GComboBox::insertItem(LPCTSTR lpString, DWORD dwIndex) {
	DWORD ret = SendMessage(m_hWnd, CB_INSERTSTRING, (WPARAM)dwIndex, (LPARAM)lpString);
	if (ret == 0)
		setSelection(0);
	return ret;
}


DWORD GComboBox::deleteItem(DWORD dwIndex) {
	return SendMessage(m_hWnd, CB_DELETESTRING, (WPARAM)dwIndex, NULL);
}


void GComboBox::clear() {
	SendMessage(m_hWnd, CB_RESETCONTENT, NULL, NULL);
}


DWORD GComboBox::getSelection() {
	return SendMessage(m_hWnd, CB_GETCURSEL, NULL, NULL);
}


void GComboBox::setSelection(DWORD dwIndex) {
	SendMessage(m_hWnd, CB_SETCURSEL, (WPARAM)dwIndex, NULL);
}


DWORD GComboBox::getCount() {
	return SendMessage(m_hWnd, CB_GETCOUNT, NULL, NULL);
}
