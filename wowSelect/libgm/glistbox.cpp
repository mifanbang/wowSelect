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


#include <algorithm>
#include "glistbox.h"



GListBox::GListBox(GUI *parent) : GUI(parent) {
	m_bSorted = false;

	initGUI();
}


GListBox::~GListBox() {
	GUI::GUIManager.unregisterGUI(this);
}


void GListBox::initGUI() {
	GRect geometry;
	geometry.x = CW_USEDEFAULT;
	geometry.y = CW_USEDEFAULT;
	geometry.width = 120;
	geometry.height = 21;

	createGUI(WS_EX_CLIENTEDGE, _T("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY, &geometry, (HMENU)GUI::GUIManager.getGUICounter(), NULL);
	GUI::GUIManager.registerGUI(this);
}


DWORD GListBox::addItem(GListItem& item) {
	DWORD ret;

	if (m_bSorted) {
		__List::iterator itr;
		DWORD dwIndex = 0;

		// get the correct index to insert
		for (itr = m_listData.begin(); itr != m_listData.end(); itr++, dwIndex++) {
			if (itr->first > item.first)
				break;
		}
		m_listData.insert(itr, item);

		ret = SendMessage(m_hWnd, LB_INSERTSTRING, (WPARAM)dwIndex, (LPARAM)item.first.c_str());
		if ((signed int)ret >= 0)
			SendMessage(m_hWnd, LB_SETITEMDATA, (WPARAM)ret, (LPARAM)item.second);
		else
			m_listData.erase(--itr);  // restore list data if LB_INSERTSTRING fails
	}
	else {
		ret = SendMessage(m_hWnd, LB_ADDSTRING, NULL, (LPARAM)item.first.c_str());
		if ((signed int)ret >= 0) {
			SendMessage(m_hWnd, LB_SETITEMDATA, (WPARAM)ret, (LPARAM)item.second);
			m_listData.push_back(item);
		}
	}

	if (ret == 0)
		setSelection(0);
	return ret;
}


DWORD GListBox::insertItem(DWORD dwIndex, GListItem& item) {
	// ignore index if m_bSorted is set
	if (m_bSorted)
		return addItem(item);

	DWORD ret = SendMessage(m_hWnd, LB_INSERTSTRING, (WPARAM)dwIndex, (LPARAM)item.first.c_str());
	if ((signed int)ret >= 0) {
		SendMessage(m_hWnd, LB_SETITEMDATA, (WPARAM)ret, (LPARAM)item.second);
		__List::iterator itr = m_listData.begin();
		for (DWORD i = 0; i < dwIndex; i++, itr++);
		m_listData.insert(itr, item);
	}

	if (ret == 0)
		setSelection(0);
	return ret;
}


bool GListBox::getItem(DWORD dwIndex, GListItem* item) {
	if (dwIndex >= m_listData.size())
		return false;

	__List::iterator itr = m_listData.begin();
	for (DWORD i = 0; i < dwIndex; i++, itr++);
	*item = *itr;

	return true;
}


LPARAM GListBox::getItemData(DWORD dwIndex) {
	return SendMessage(m_hWnd, LB_GETITEMDATA, dwIndex, NULL);
}


DWORD GListBox::setItemData(DWORD dwIndex, LPARAM dwData) {
	return SendMessage(m_hWnd, LB_SETITEMDATA, dwIndex, dwData);
}


DWORD GListBox::findItem(GListItem& item, ListBoxFindMask option) {
	if ((option & (eFindString | eFindData)) == 0)
		return (DWORD)-1;

	DWORD index = 0;
	for (__List::iterator itr = m_listData.begin(); itr != m_listData.end(); itr++, index++) {
		int result = 0;
		if ((option & eFindString) && itr->first == item.first)
			result |= eFindString;
		if ((option & eFindData) && itr->second == item.second)
			result |= eFindString;
		if (result == option)
			return index;
	}

	return (DWORD)-1;
}


DWORD GListBox::deleteItem(DWORD dwIndex) {
	DWORD ret = SendMessage(m_hWnd, LB_DELETESTRING, (WPARAM)dwIndex, NULL);
	if ((signed int)ret >= 0) {
		__List::iterator itr = m_listData.begin();
		for (DWORD i = 0; i < dwIndex; i++, itr++);
		m_listData.erase(itr);
	}
	return ret;
}


void GListBox::clear() {
	SendMessage(m_hWnd, LB_RESETCONTENT, NULL, NULL);
	m_listData.clear();
}


DWORD GListBox::getSelection() {
	return SendMessage(m_hWnd, LB_GETCURSEL, NULL, NULL);
}


void GListBox::setSelection(DWORD dwIndex) {
	SendMessage(m_hWnd, LB_SETCURSEL, (WPARAM)dwIndex, NULL);
}


void GListBox::setSorted(bool set) {
	if (m_bSorted != set) {
		if (set) {
			// drop all things and re-add them in sorted order
			SendMessage(m_hWnd, LB_RESETCONTENT, NULL, NULL);
			m_listData.sort();
			for (__List::iterator itr = m_listData.begin(); itr != m_listData.end(); itr++)
				addItem(*itr);
		}
		m_bSorted = set;
	}
}


bool GListBox::isSorted() {
	return m_bSorted;
}


DWORD GListBox::getCount() {
	return m_listData.size();
}
