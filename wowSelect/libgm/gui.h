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


#ifndef __GUI_H
#define __GUI_H

// STL headers
#include <map>

// GMlib headers
#include "gsystem.h"

// enable visual style
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


// macros
#define GUI_CLASS(cname)	public: virtual LPCTSTR getClassName() const { return _T(cname); }

namespace std {
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif  // _UNICODE
};


class GUI;
class GUIManager;


class GUI {
protected:
	// initGUI() should call GUI::createGUI(), which will call CreateWindow()
	virtual void initGUI() = 0;
	void createGUI(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, GRect* geometry, HMENU hMenu, LPVOID lpParam);

	// override GUIProc() to receive window messages
	// note that child windows cannot receive a message
	friend LRESULT CALLBACK GWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool GUIProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// event handlers
	friend class GUIManager;

	HWND m_hWnd;
	GUI* m_lpParent;
	GRect m_grGeometry;

	static HFONT hDefaultFont;

public:
	GUI(GUI* parent = NULL);
	~GUI();

	virtual LPCTSTR getClassName() const = 0;

	// get system-related resources
	GUI* getParent() const { return m_lpParent; };
	HWND getHandle() const { return m_hWnd; };

	// window flags
	bool hasFlag(int nIndex, long dwFlag);
	void setFlag(int nIndex, long dwFlag, bool bSet);

	// geometry
	void getGeometry(GRect* rect);
	void setGeometry(GRect* rect);
	void setGeometry(int x, int y, int width, int height);
	void moveTo(int x, int y);
	void resize(int width, int height);
	void resizeClientArea(int width, int height);

	// enable/disable
	void setEnabled(bool bEnable);

	// apperance
	void show();
	void hide();
	DWORD getText(LPTSTR lpBuffer, DWORD dwSize);
	void setText(LPCTSTR lpString);


	// gui manager
	static class _GUIManager {
	private:
		std::map<HWND, GUI*> guiList;
		std::map<LPCTSTR, ATOM> windowClassList;
		GUI* mainWindow;
		DWORD dwGuiCounter;

	public:
		_GUIManager() : mainWindow(NULL), dwGuiCounter(0) { }

		ATOM registerWindowClass(LPCTSTR szName);
		void registerGUI(GUI* gui);
		void unregisterGUI(GUI* gui);
		GUI* findGUI(HWND hWnd);
		GUI* getMainWindow() const { return mainWindow; }
		void setMainWindow(GUI* gWin) { mainWindow = gWin; }
		DWORD getGUICounter() { return dwGuiCounter; }
	} GUIManager;

};



#endif  // __GUI_H
