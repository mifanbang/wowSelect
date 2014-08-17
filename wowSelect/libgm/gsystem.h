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


#ifndef __GSYSTEM_H
#define __GSYSTEM_H

#include <windows.h>
#include <tchar.h>
#include <gdiplus.h>


// self-defined WM_CREATE message, should be posted after CreateWindow returns
#define G_WM_CREATE	WM_USER + 0x1000


int GMainEventLoop();
LRESULT CALLBACK GWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


struct GRect {
	DWORD x;
	DWORD y;
	DWORD width;
	DWORD height;
};


#endif  // __GSYSTEM_H
