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


#ifndef __GLABEL_H
#define __GLABEL_H

#include "gui.h"



class GLabel : public GUI {
	GUI_CLASS("GLabel")

protected:
	virtual void initGUI();

public:
	// constructors
	GLabel(GUI *parent);
	~GLabel();
};


class GImageLabel : public GUI {
	GUI_CLASS("GImageLabel")

protected:
	virtual void initGUI();

	static bool bGDIPlusLoaded;

public:
	// constructors
	GImageLabel(GUI *parent);
	~GImageLabel();

	bool loadImage(LPCTSTR lpResourceName);
};

#endif  // __GLABEL_H
