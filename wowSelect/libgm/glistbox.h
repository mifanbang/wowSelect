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


#ifndef __GLISTBOX_H
#define __GLISTBOX_H

#include <string>
#include <list>
#include "gui.h"



// we use LPARAM in order to support memory pointer under 64-bit environments
class GListItem : public std::pair<std::tstring, LPARAM> {
public:
	GListItem() : std::pair<std::tstring, LPARAM>(_T(""), NULL) { };
	GListItem(std::tstring str) : std::pair<std::tstring, LPARAM>(str, NULL) { };
	GListItem(std::tstring str, LPARAM data) : std::pair<std::tstring, LPARAM>(str, data) { };

	~GListItem() {};
};


class GListBox : public GUI {
	GUI_CLASS("GListBox")

private:
	typedef std::list<GListItem> __List;

	__List m_listData;
	bool m_bSorted;

protected:
	virtual void initGUI();

public:
	enum ListBoxFindMask { eFindString = 0x1, eFindData = 0x2 };

	// constructors
	GListBox(GUI *parent);
	~GListBox();

	DWORD addItem(GListItem& item);
	DWORD insertItem(DWORD dwIndex, GListItem& item);
	bool getItem(DWORD dwIndex, GListItem* item);
	LPARAM getItemData(DWORD dwIndex);
	DWORD setItemData(DWORD dwIndex, LPARAM dwData);
	DWORD findItem(GListItem& item, ListBoxFindMask option);
	DWORD deleteItem(DWORD dwIndex);
	void clear();

	DWORD getSelection();
	void setSelection(DWORD dwIndex);

	void setSorted(bool set);
	bool isSorted();

	DWORD getCount();
};


#endif  // __GLISTBOX_H
