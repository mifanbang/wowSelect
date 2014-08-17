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


#include "glabel.h"

#pragma comment(lib, "gdiplus.lib")


bool GImageLabel::bGDIPlusLoaded = false;


//----------------------------
//  GLabel
//----------------------------

GLabel::GLabel(GUI *parent) : GUI(parent) {
	initGUI();
}


GLabel::~GLabel() {
	GUI::GUIManager.unregisterGUI(this);
}


void GLabel::initGUI() {
	GRect geometry;
	geometry.x = CW_USEDEFAULT;
	geometry.y = CW_USEDEFAULT;
	geometry.width = 100;
	geometry.height = 100;

	createGUI(NULL, _T("Static"), getClassName(), WS_CHILD | WS_VISIBLE, &geometry, (HMENU)GUI::GUIManager.getGUICounter(), NULL);
	GUI::GUIManager.registerGUI(this);
}


//----------------------------
//  GImageLabel
//----------------------------

GImageLabel::GImageLabel(GUI *parent) : GUI(parent) {
	initGUI();
}


GImageLabel::~GImageLabel() {
	GUI::GUIManager.unregisterGUI(this);
}


void GImageLabel::initGUI() {
	GRect geometry;
	geometry.x = CW_USEDEFAULT;
	geometry.y = CW_USEDEFAULT;
	geometry.width = 100;
	geometry.height = 100;

	createGUI(NULL, _T("Static"), NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, &geometry, (HMENU)GUI::GUIManager.getGUICounter(), NULL);
	GUI::GUIManager.registerGUI(this);

	if (!GImageLabel::bGDIPlusLoaded) {
		// initialize GDI+
		ULONG_PTR gdiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		GImageLabel::bGDIPlusLoaded = true;
	}
}


// returns false on error, otherwise returns true
// NOTE: this method can only load resources of type RCDATA
bool GImageLabel::loadImage(LPCTSTR lpResourceName) {

	// load resource
	HMODULE hModule = GetModuleHandle(NULL);
	HRSRC hResource = FindResource(hModule, lpResourceName, RT_RCDATA);
	if (hResource == NULL)
		return false;

	DWORD dwResSize = SizeofResource(hModule, hResource);
	if (dwResSize == 0)
		return false;

	PBYTE lpResData = (PBYTE) LockResource(LoadResource(hModule, hResource));
	if (lpResData == NULL)
		return false;

	// create global object
	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, dwResSize);
	if (hBuffer == NULL)
		return false;

	PBYTE lpBuffer = (PBYTE) GlobalLock(hBuffer);
	if (lpBuffer == NULL) {
		GlobalFree(hBuffer);
		return false;
	}

	CopyMemory(lpBuffer, lpResData, dwResSize);

	// create IStream object
	IStream* stream;
	if (CreateStreamOnHGlobal(hBuffer, true, &stream) != S_OK) {
		GlobalFree(hBuffer);
		return false;
	}

	// use GDI+ to obtain HBITMAP from stream
	Gdiplus::Color color;
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromStream(stream);
	HBITMAP hBitmap, hPrevBitmap;
	bitmap->GetHBITMAP(color, &hBitmap);
	hPrevBitmap = (HBITMAP) SendMessage(m_hWnd, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);
	if (hPrevBitmap != NULL)
		DeleteObject(hPrevBitmap);

	// clean up
	DeleteObject(hBitmap);
	delete bitmap;
	stream->Release();
	GlobalUnlock(hBuffer);
	GlobalFree(hBuffer);

	return true;
}
