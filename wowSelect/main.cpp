/*
 *  wowSelect - A World of Warcraft launcher enabling users to switch between servers to connect to
 *  Copyright (C) 2010-2014 Mifan Bang <http://debug.tw>.
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


// Windows SDK headers
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

// STL headers
#include <algorithm>
#include <string>
#include <vector>

// libgm headers
#include "libgm/gwindow.h"
#include "libgm/glabel.h"
#include "libgm/gbutton.h"
#include "libgm/gcombobox.h"
#include "libgm/glistbox.h"
#include "libgm/gedit.h"


// macros
#define APP_NAME	L"wowSelect"
#define APP_VERSION	L"1.0.1"
#define APP_TITLE	APP_NAME L" " APP_VERSION


// data types
typedef std::pair<std::wstring, std::wstring> WowServerEntry;
typedef std::vector<WowServerEntry> WowServerList;

// function pointer types
typedef BOOL (WINAPI *WriteProcessMemoryPtr)(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten);


class ErrorTrace {
public:
	ErrorTrace() {
		ZeroMemory(m_szTask, sizeof(m_szTask));
		dwErrorCode = NO_ERROR;
	}

	void getTask(LPTSTR lpszTask, DWORD dwSize) {
		_tcsncpy_s(lpszTask, dwSize, m_szTask, _TRUNCATE);
	}

	void setTask(LPCTSTR lpszTask) {
		_tcsncpy_s(m_szTask, sizeof(m_szTask) / sizeof(m_szTask[0]), lpszTask, _TRUNCATE);
	}

	DWORD getLastError() {
		return dwErrorCode;
	}

	void setLastError(DWORD dwCode) {
		dwErrorCode = dwCode;
	}

	bool hasError() {
		return dwErrorCode != NO_ERROR;
	}

	static const unsigned int s_nMaxTaskLen = 256;


protected:
	TCHAR m_szTask[s_nMaxTaskLen];
	DWORD dwErrorCode;
};


class WowSelectConfigWindow : public GWindow {
	GUI_CLASS("WowSelectConfigWindow")

public:
	WowSelectConfigWindow(GUI* parent = NULL) : GWindow(parent) {
		setText(L"Configure Test Servers");
		resizeClientArea(320, 195);
		setResizable(false);

		m_labelAlias = new GLabel(this);
		m_labelAlias->setText(L"Server:");
		m_labelAlias->moveTo(20, 19);
		m_labelAlias->resize(50, 17);

		m_editAlias = new GEdit(this);
		m_editAlias->moveTo(65, 15);
		m_editAlias->resize(230, 19);

		m_labelHost = new GLabel(this);
		m_labelHost->setText(L"Host/IP:");
		m_labelHost->moveTo(20, 44);
		m_labelHost->resize(50, 17);

		m_editHost = new GEdit(this);
		m_editHost->moveTo(65, 40);
		m_editHost->resize(230, 19);

		m_listServers = new GListBox(this);
		m_listServers->moveTo(15, 80);
		m_listServers->resize(185, 100);
		m_listServers->setSorted(true);

		m_buttonSave = new GButton(this);
		m_buttonSave->setText(L"Save");
		m_buttonSave->moveTo(215, 78);
		m_buttonSave->resize(90, 24);

		m_buttonDelete = new GButton(this);
		m_buttonDelete->setText(L"Delete");
		m_buttonDelete->moveTo(215, 108);
		m_buttonDelete->resize(90, 24);

		m_buttonClose = new GButton(this);
		m_buttonClose->setText(L"Close");
		m_buttonClose->moveTo(215, 156);
		m_buttonClose->resize(90, 24);

		if (parent) {
			// setup position based on parant's
			GRect rect;
			parent->getGeometry(&rect);
			moveTo(rect.x + 25, rect.y + 25);
		}
		else {
			// put self to the center of the screen
			HWND hDesktop = GetDesktopWindow();
			RECT rectScr, rectWin;
			GetWindowRect(hDesktop, &rectScr);
			GetWindowRect(getHandle(), &rectWin);
			moveTo((rectScr.right - (rectWin.right - rectWin.left)) >> 1, (rectScr.bottom - (rectWin.bottom - rectWin.top)) >> 1);
		}
	}

	~WowSelectConfigWindow() {
		DWORD count = m_listServers->getCount();
		for (DWORD i = 0; i < count; i++) {
			std::wstring* str = (std::wstring*) m_listServers->getItemData(0);
			delete str;
			m_listServers->deleteItem(0);
		}

		delete m_editAlias;
		delete m_editHost;
		delete m_labelAlias;
		delete m_labelHost;
		delete m_listServers; 
		delete m_buttonSave;
		delete m_buttonDelete;
		delete m_buttonClose;
	}

	void setServerList(WowServerList* list) {
		if (list == NULL)
			return;

		m_listServers->clear();
		for (WowServerList::iterator itr = list->begin(); itr != list->end(); itr++)
			m_listServers->addItem(GListItem(itr->first, (LPARAM)new std::wstring(itr->second)) );

		m_listServers->setSelection(-1);  // clear selection
	}

	void getServerList(WowServerList* list) {
		if (list == NULL)
			return;

		DWORD count = m_listServers->getCount();
		GListItem item;
		list->clear();
		for (DWORD i = 0; i < count; i++) {
			if (m_listServers->getItem(i, &item))
				list->push_back(WowServerEntry(item.first, *(std::wstring*)item.second) );
		}
	}


protected:
	bool GUIProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_COMMAND:
			{
				UINT msg = HIWORD(wParam);
				HANDLE handle = (HANDLE)lParam;
				if (handle == m_listServers->getHandle())
					onListBoxCommand(msg);
				break;
			}
		}

		return GWindow::GUIProc(uMsg, wParam, lParam);
	};

	void onAction(GUI* gui, WORD wMsg) {
		if (gui == m_buttonSave) {
			wchar_t szServerAlias[256];
			wchar_t szServerHost[256];
			m_editAlias->getText(szServerAlias, sizeof(szServerAlias) / sizeof(szServerAlias[0]));
			m_editHost->getText(szServerHost, sizeof(szServerHost) / sizeof(szServerHost[0]));

			if (wcslen(szServerAlias) == 0 || wcslen(szServerHost) == 0)
				return;

			GListItem item(szServerAlias);
			DWORD index = m_listServers->findItem(item, GListBox::eFindString);
			if (index == (DWORD)-1) {
				item.second = (LPARAM) new std::wstring(szServerHost);
				index = m_listServers->addItem(item);
				m_listServers->setSelection(index);
			}
			else {
				std::wstring* str = (std::wstring*) m_listServers->getItemData(index);
				if ((int)str != LB_ERR)
					*str = szServerHost;
			}
		}
		else if (gui == m_buttonDelete) {
			DWORD dwSel = m_listServers->getSelection();
			if ((signed int)dwSel >= 0) {
				GListItem item;
				m_listServers->getItem(dwSel, &item);
				m_listServers->deleteItem(dwSel);
				delete (std::wstring*)item.second;

				// clear edit controls
				m_editAlias->setText(NULL);
				m_editHost->setText(NULL);
			}
		}
		else if (gui == m_buttonClose) {
			destroyLater();
		}
	}

	void onListBoxCommand(UINT uMsg) {
		DWORD dwSel = m_listServers->getSelection();

		switch (uMsg) {
			case LBN_SETFOCUS:
			{
				if (dwSel != (DWORD)-1)
					break;
				else if (m_listServers->getCount() <= 0)
					break;

				// if no selection has been made before, select the first one
				m_listServers->setSelection(0);
				dwSel = 0;
				// we don't break here and continue with work done for LBN_SELCHANGE
			}

			case LBN_SELCHANGE:
			{
				if ((signed int)dwSel >= 0) {
					GListItem item;
					m_listServers->getItem(dwSel, &item);
					m_editAlias->setText(item.first.c_str());
					m_editHost->setText( ((std::wstring*)item.second)->c_str());
				}
				break;
			}

			default:
				break;
		}
	}


	// gui
	GEdit* m_editAlias;
	GEdit* m_editHost;
	GLabel* m_labelAlias;
	GLabel* m_labelHost;
	GListBox* m_listServers; 
	GButton* m_buttonSave;
	GButton* m_buttonDelete;
	GButton* m_buttonClose;
};


class WowSelectMainWindow : public GWindow {
	GUI_CLASS("WowSelectMainWindow")

public:
	WowSelectMainWindow() {
		CoInitialize(NULL);

		initLiveServers();
		readTestServers();
		checkErrorTrace();
		readConfig();
		checkErrorTrace();

		setText(APP_TITLE);
		resizeClientArea(320, 270);
		setResizable(false);

		m_labelLogo = new GImageLabel(this);
		m_labelLogo->loadImage(L"logo");
		m_labelLogo->moveTo(10, 10);

		m_labelServer = new GLabel(this);
		m_labelServer->setText(L"Server:");
		m_labelServer->moveTo(15, 199);

		m_comboServers = new GComboBox(this);
		m_comboServers->moveTo(57, 195);
		m_comboServers->resize(246, 22);
		updateServerList();
		SetFocus(m_comboServers->getHandle());

		m_buttonShortcut = new GButton(this);
		m_buttonShortcut->setText(L"Shortcut");
		m_buttonShortcut->moveTo(20, 230);
		m_buttonShortcut->resize(90, 24);

		m_buttonConfig = new GButton(this);
		m_buttonConfig->setText(L"Configure");
		m_buttonConfig->moveTo(115, 230);
		m_buttonConfig->resize(90, 24);

		m_buttonRun = new GButton(this);
		m_buttonRun->setText(L"Run");
		m_buttonRun->moveTo(210, 230);
		m_buttonRun->resize(90, 24);

		// put self to the center of the screen
		HWND hDesktop = GetDesktopWindow();
		RECT rectScr, rectWin;
		GetWindowRect(hDesktop, &rectScr);
		GetWindowRect(getHandle(), &rectWin);
		moveTo((rectScr.right - (rectWin.right - rectWin.left)) >> 1, (rectScr.bottom - (rectWin.bottom - rectWin.top)) >> 1);
	}

	~WowSelectMainWindow() {
		delete m_labelLogo;
		delete m_labelServer;
		delete m_comboServers;
		delete m_buttonShortcut;
		delete m_buttonConfig;
		delete m_buttonRun;
	}

	DWORD launchWowServer(const wchar_t* lpszServer) {
		DWORD retCode;
		wchar_t path[MAX_PATH];

		getTempFilePath(path, sizeof(path) / sizeof(path[0]));
		makeRealmlist(path, lpszServer);
		retCode = launchWowWtf(path);
		DeleteFile(path);

		return retCode;
	}

	// return true on success; false otherwise.
	static bool WINAPI getTempFilePath(wchar_t* lpszBuffer, DWORD dwLength) {
		wchar_t buffer[MAX_PATH];
		wchar_t randomStr[MAX_PATH];

		srand(GetTickCount());
		swprintf_s(randomStr, sizeof(randomStr) / sizeof(randomStr[0]), L"\\_WS_%X%X%X.dat", rand(), rand(), rand());
		ZeroMemory(buffer, sizeof(buffer));

		GetTempPath(sizeof(buffer) / sizeof(buffer[0]), buffer);
		wcsncat_s(buffer, sizeof(buffer) / sizeof(buffer[0]), randomStr, _TRUNCATE);
		return wcsncpy_s(lpszBuffer, dwLength, buffer, _TRUNCATE) == 0;
	}

	static DWORD WINAPI getWowDirectory(wchar_t* lpPath, DWORD length) {
		DWORD retCode = NO_ERROR;
		HKEY hRegKey;
		DWORD dwSize = MAX_PATH;
		wchar_t szPath[MAX_PATH];

		retCode = RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Blizzard Entertainment\\World of Warcraft", &hRegKey);
		if (retCode == NO_ERROR) {
			retCode = RegQueryValueEx(hRegKey, L"InstallPath", NULL, NULL, (PBYTE)szPath, &dwSize);
			if (retCode == NO_ERROR) {
				wcsncpy_s(lpPath, length, szPath, MAX_PATH);
				lpPath[length - 1] = NULL;
			}
			RegCloseKey(hRegKey);
		}
		return retCode;
	}


protected:
	void onAction(GUI* gui, WORD wMsg) {
		if (gui == m_buttonShortcut) {
			DWORD selection = m_comboServers->getSelection();
			if (selection > 1 && selection != 2 + m_listLiveSrv.size()) {  // a valid server must be selected
				if (selection < 2 + m_listLiveSrv.size())
					makeShortcut(m_listLiveSrv[selection - 2]);
				else
					makeShortcut(m_listTestSrv[selection - m_listLiveSrv.size() - 3]);

				if (!checkErrorTrace())
					infoMessageBox(L"A shortcut was successfully created on your desktop.");
			}
		}
		else if (gui == m_buttonConfig) {
			WowSelectConfigWindow w(this);

			w.setServerList(&m_listTestSrv);
			w.showModal();
			w.getServerList(&m_listTestSrv);

			updateServerList();
			writeTestServers();
			checkErrorTrace();
		}
		else if (gui == m_buttonRun) {
			DWORD selection = m_comboServers->getSelection();

			if (selection == 0) {  // default, just create normal WoW process
				if (launchWowWtf(NULL))
					destroyLater();
				checkErrorTrace();
			}
			else if (selection > 1 && selection < 2 + m_listLiveSrv.size()) {  // live servers
				wchar_t path[MAX_PATH];
				getTempFilePath(path, sizeof(path) / sizeof(path[0]));
				makeRealmlist(path, m_listLiveSrv[selection - 2].second.c_str());
				if (launchWowWtf(path))
					destroyLater();
				DeleteFile(path);
				checkErrorTrace();
			}
			else if (selection > 2 + m_listLiveSrv.size()) {  // test servers
				wchar_t path[MAX_PATH];
				getTempFilePath(path, sizeof(path) / sizeof(path[0]));
				makeRealmlist(path, m_listTestSrv[selection - m_listLiveSrv.size() - 3].second.c_str());
				if (launchWowWtf(path))
					destroyLater();
				DeleteFile(path);
				checkErrorTrace();
			}
		}
	}

	void initLiveServers() {
		m_listLiveSrv.clear();
		m_listLiveSrv.push_back(WowServerEntry(L"Europe", L"eu.logon.worldofwarcraft.com"));
		m_listLiveSrv.push_back(WowServerEntry(L"Korea", L"kr.logon.worldofwarcraft.com"));
		m_listLiveSrv.push_back(WowServerEntry(L"Taiwan", L"tw.logon.worldofwarcraft.com"));
		m_listLiveSrv.push_back(WowServerEntry(L"United States", L"us.logon.worldofwarcraft.com"));
		std::sort(m_listLiveSrv.begin(), m_listLiveSrv.end());
	}

	bool readTestServers() {
		m_listTestSrv.clear();

		HKEY hKey;
		DWORD dwDispo, dwResult;
		m_errorTrace.setTask(L"Reading test server list from registry");
		dwResult = RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\mifan\\wowSelect\\TestServers", NULL, NULL, NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDispo);
		m_errorTrace.setLastError(dwResult);
		if (dwResult != NO_ERROR)
			return false;

		DWORD dwIndex;
		for (dwIndex = 0; ; dwIndex++) {
			wchar_t szValueName[256], szValueData[256];
			DWORD dwLenName = sizeof(szValueName) / sizeof(szValueName[0]);
			DWORD dwLenData = sizeof(szValueData) - 1;
			DWORD dwType;
			dwResult = RegEnumValue(hKey, dwIndex, szValueName, &dwLenName, NULL, &dwType, (LPBYTE)szValueData, &dwLenData);

			if (dwResult == NO_ERROR && dwType == REG_SZ) {
				szValueData[dwLenData] = '\0';
				m_listTestSrv.push_back(WowServerEntry(szValueName, szValueData));
			}
			else if (dwResult != ERROR_MORE_DATA) {
				break;
			}
		}
		RegCloseKey(hKey);

		std::sort(m_listTestSrv.begin(), m_listTestSrv.end());
		return true;
	}

	bool writeTestServers() {
		// delete all saved test servers if exist
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\mifan\\wowSelect\\TestServers", NULL, KEY_READ | KEY_WRITE, &hKey) == NO_ERROR) {
			RegCloseKey(hKey);
			SHDeleteKey(HKEY_CURRENT_USER, L"SOFTWARE\\mifan\\wowSelect\\TestServers");
		}
		
		DWORD dwDispo, dwResult;
		m_errorTrace.setTask(L"Writing test server list to registry");
		dwResult = RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\mifan\\wowSelect\\TestServers", NULL, NULL, NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDispo);
		m_errorTrace.setLastError(dwResult);
		if (dwResult != NO_ERROR)
			return false;

		for (WowServerList::iterator itr = m_listTestSrv.begin(); itr != m_listTestSrv.end(); itr++) {
			RegSetValueEx(hKey, itr->first.c_str(), NULL, REG_SZ, (const BYTE*)itr->second.c_str(), itr->second.length() * sizeof(wchar_t));
		}
		RegCloseKey(hKey);

		return true;
	}

	// read the last settings in WTF\Config.wtf
	bool readConfig() {
		// default settings: US
		m_strPatchSrv = "us.version.worldofwarcraft.com";
		m_strPortal = "us";

		DWORD retCode;
		wchar_t szWowDir[MAX_PATH];
		m_errorTrace.setTask(L"Obtaining WoW installation directory");
		retCode = getWowDirectory(szWowDir, MAX_PATH);
		m_errorTrace.setLastError(retCode);
		if (retCode == NO_ERROR) {
			HANDLE hFile;
			wcsncat_s(szWowDir, MAX_PATH, L"\\WTF\\Config.wtf", _TRUNCATE);
			m_errorTrace.setTask(L"Reading WoW configuration file");
			hFile = CreateFile(szWowDir, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
			m_errorTrace.setLastError(GetLastError());
			if (hFile != INVALID_HANDLE_VALUE) {
				DWORD count;
				DWORD size = GetFileSize(hFile, NULL);
				char* buffer = new char[size + 1];
				char* ptrHead = buffer;
				char* ptrTail;
				ReadFile(hFile, buffer, size, &count, NULL);
				buffer[size] = '\0';

				// pattern:  SET patchlist "us.version.worldofwarcraft.com"
				if (ptrHead = strstr(buffer, "patchlist \"")) {
					ptrHead += 11;  // strlen("patchlist \"") = 11
					if (ptrTail = strstr(ptrHead, "\"")) {
						*ptrTail = '\0';
						m_strPatchSrv = ptrHead;
						ptrHead = ptrTail + 1;
					}
				}

				// pattern:  SET portal "us"
				if (ptrHead = strstr(buffer, "portal \"")) {
					ptrHead += 8;  // strlen("portal \"") = 8
					if (ptrTail = strstr(ptrHead, "\"")) {
						*ptrTail = '\0';
						m_strPortal = ptrHead;
						ptrHead = ptrTail + 1;
					}
				}

				delete[] buffer;
				CloseHandle(hFile);
			}
		}

		return !m_errorTrace.hasError();
	}

	void updateServerList() {
		if (m_comboServers) {
			m_comboServers->clear();
			m_comboServers->addItem(L"Default");

			// live servers
			if (m_listLiveSrv.size() > 0)
				m_comboServers->addItem(L"----------------------------");
			for (WowServerList::iterator itr = m_listLiveSrv.begin(); itr != m_listLiveSrv.end(); itr++)
				m_comboServers->addItem(itr->first.c_str());

			// test servers
			if (m_listTestSrv.size() > 0)
				m_comboServers->addItem(L"----------------------------");
			for (WowServerList::iterator itr = m_listTestSrv.begin(); itr != m_listTestSrv.end(); itr++) {
				wchar_t buffer[256];
				swprintf_s(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%s (%s)", itr->second.c_str(), itr->first.c_str());
				m_comboServers->addItem(buffer);
			}
		}
	}

	// make shortcut on desktop
	// return true on success, false otherwise
	bool makeShortcut(WowServerEntry &server) {
		DWORD retCode = NO_ERROR;
		IShellLink* lpShellLink;
		m_errorTrace.setTask(L"Creating instance of IShellLink");
		retCode = (DWORD) CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&lpShellLink);
		m_errorTrace.setLastError(retCode);
		if (retCode != S_OK)
			return false;

		// set command line and show state
		int argc;
		wchar_t** argv = CommandLineToArgvW(GetCommandLine(), &argc);
		lpShellLink->SetPath(argv[0]);
		lpShellLink->SetArguments(server.second.c_str());
		lpShellLink->SetShowCmd(SW_SHOWMAXIMIZED);

		wchar_t szExePath[MAX_PATH];
		m_errorTrace.setTask(L"Obtaining WoW installation directory");
		retCode = getWowDirectory(szExePath, MAX_PATH);
		m_errorTrace.setLastError(retCode);
		if (retCode == NO_ERROR) {
			// set icon
			SetCurrentDirectory(szExePath);
			wcsncat_s(szExePath, MAX_PATH, L"\\WoW.exe", _TRUNCATE);
			lpShellLink->SetIconLocation(szExePath, 0);

			// set shortcut path and save
			HKEY hRegKey;
			DWORD dwLength = MAX_PATH;
			wchar_t lpszDesktop[MAX_PATH];
			m_errorTrace.setTask(L"Obtaining path to desktop through registry");
			retCode = RegOpenKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", &hRegKey);
			m_errorTrace.setLastError(retCode);
			if (retCode == NO_ERROR) {
				retCode = RegQueryValueEx(hRegKey, L"Desktop", NULL, NULL, (PBYTE)lpszDesktop, &dwLength);
				m_errorTrace.setLastError(retCode);
				if (retCode == NO_ERROR) {
					std::wstring strDesktop = lpszDesktop;
					strDesktop += L"\\World of Warcraft - ";
					strDesktop += server.first;
					strDesktop += L".lnk";

					IPersistFile* lpPersistFile;
					m_errorTrace.setTask(L"Creating instance of IPersisFile");
					retCode = lpShellLink->QueryInterface(IID_IPersistFile, (void**)&lpPersistFile);
					m_errorTrace.setLastError(retCode);
					lpPersistFile->Save(strDesktop.c_str(), FALSE);
					lpPersistFile->Release();
				}
				RegCloseKey(hRegKey);
			}
		}
		lpShellLink->Release();

		return !m_errorTrace.hasError();
	}

	DWORD makeRealmlist(const wchar_t* lpszPath, const wchar_t* lpszRealm) {
		HANDLE hFile = CreateFile(lpszPath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return GetLastError();

		std::string realm;
		for (; *lpszRealm; lpszRealm++)
			realm.push_back((char)*lpszRealm);

		std::string buffer;
		DWORD dwCount;
		buffer += "set realmlist ";
		buffer += realm;
		buffer += "\r\n";  // realmlist
		buffer += "set patchlist ";
		buffer += m_strPatchSrv;
		buffer += "\r\n";  // patchlist
		buffer += "set realmlistbn \"\"\r\n";  // realmlistbn
		buffer += "set portal ";
		buffer += m_strPortal;  // portal

		WriteFile(hFile, buffer.c_str(), buffer.size(), &dwCount, NULL);
		CloseHandle(hFile);

		return NO_ERROR;
	}

	// return true on success, false otherwise
	bool launchWowWtf(const wchar_t* lpszWtfPath) {
		DWORD retCode;

		wchar_t szExePath[MAX_PATH];
		m_errorTrace.setTask(L"Obtaining WoW installation directory");
		retCode = getWowDirectory(szExePath, MAX_PATH);
		m_errorTrace.setLastError(retCode);
		if (retCode != NO_ERROR)
			return false;
		SetCurrentDirectory(szExePath);
		wcsncat_s(szExePath, MAX_PATH, L"\\WoW.exe", _TRUNCATE);

		// create process
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMAXIMIZED;  // show WoW window maximized
		ZeroMemory(&pi, sizeof(pi));
		m_errorTrace.setTask(L"Creating WoW process");
		if (lpszWtfPath == NULL) {  // launch WoW normally if the given WTF path is NULL
			BOOL bRetCode = CreateProcess(szExePath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
			m_errorTrace.setLastError(bRetCode ? NO_ERROR : GetLastError());
			return !m_errorTrace.hasError();
		}
		else {  // otherwise laumch WoW and attach to it
			BOOL bRetCode = CreateProcess(szExePath, NULL, NULL, NULL, FALSE, DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi);
			m_errorTrace.setLastError(bRetCode ? NO_ERROR : GetLastError());
			if (m_errorTrace.hasError())
				return false;
		}

		// some data in the WoW process
		HANDLE hWtfFile = INVALID_HANDLE_VALUE;
		DWORD addrCreateFileA = (DWORD) GetProcAddress(GetModuleHandle(L"kernel32"), "CreateFileA");
		DWORD addrCloseHandle = (DWORD) GetProcAddress(GetModuleHandle(L"kernel32"), "CloseHandle");
		DWORD addrWowReadWtf = NULL;
		DWORD addrWowCloseWtf = NULL;

		// prepare to debug
		DEBUG_EVENT dbgEvent;
		LPBYTE lpImageBase = 0;
		DebugSetProcessKillOnExit(TRUE);
		bool bDoLoop = true;
		while (bDoLoop) {
			DWORD dwContinueStatus = DBG_CONTINUE;  // we defaultly continue
			m_errorTrace.setTask(L"Waiting for a debug event");
			if (WaitForDebugEvent(&dbgEvent, INFINITE) == 0) {
				m_errorTrace.setLastError(GetLastError());
				DebugActiveProcessStop(pi.dwProcessId);
				TerminateProcess(pi.hProcess, 0);
				return false;
			}
			m_errorTrace.setLastError(NO_ERROR);

			switch (dbgEvent.dwDebugEventCode) {
				case CREATE_PROCESS_DEBUG_EVENT:
				{
					CloseHandle(dbgEvent.u.CreateProcessInfo.hFile);

					// install hardware breakpoint when just attached
					CONTEXT ctx;
					ZeroMemory(&ctx, sizeof(ctx));
					ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
					GetThreadContext(pi.hThread, &ctx);
					ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
					ctx.Dr0 = addrCreateFileA;
					ctx.Dr7 = 0x1;
					SetThreadContext(pi.hThread, &ctx);

					break;
				}

				case EXCEPTION_DEBUG_EVENT:
				{
					if (dbgEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP) {  // hardware breakpoint triggered
						CONTEXT ctx;
						ZeroMemory(&ctx, sizeof(ctx));
						ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_DEBUG_REGISTERS | CONTEXT_INTEGER;  // get general and debug registers and EFLAGS
						GetThreadContext(pi.hThread, &ctx);

						if ((DWORD)dbgEvent.u.Exception.ExceptionRecord.ExceptionAddress == addrCreateFileA) {
							// step through stack and find parameter containing file path
							char buffer[1024 + 1];
							DWORD dwCount, dwRetAddr, dwPathAddr;
							ReadProcessMemory(pi.hProcess, (LPVOID)(ctx.Esp), &dwRetAddr, 4, &dwCount);  // read return address
							ReadProcessMemory(pi.hProcess, (LPVOID)(ctx.Esp + 4), &dwPathAddr, 4, &dwCount);  // read pointer to file path
							ReadProcessMemory(pi.hProcess, (LPVOID)dwPathAddr, buffer, 1024, &dwCount);  // read file path
							buffer[1024] = '\0';

							// only "realmlist.wtf" matters
							if (strstr(buffer, "realmlist.wtf")) {
								ctx.Dr0 = addrWowReadWtf = dwRetAddr;  // move breakpoint from CreateFileA() to the return address

								// overwrite pointer to file path
								char szPath[MAX_PATH];
								int i;
								for (i = 0; i < sizeof(szPath) - 1; i++)
									szPath[i] = (lpszWtfPath[i] & 0xFF);
								szPath[i] = '\0';
								LPWSTR lpRemotePath = (LPWSTR) VirtualAllocEx(pi.hProcess, NULL, sizeof(szPath), MEM_COMMIT, PAGE_READWRITE);

								// this is to bypass the stupid Avira Anti-Virus' false alert
								WriteProcessMemoryPtr WriteProcessMem = (WriteProcessMemoryPtr) GetProcAddress(GetModuleHandle(L"kernel32"), "WriteProcessMemory");

								WriteProcessMem(pi.hProcess, lpRemotePath, szPath, sizeof(szPath), &dwCount);  // write data
								WriteProcessMem(pi.hProcess, (LPVOID)(ctx.Esp + 4), &lpRemotePath, 4, &dwCount);  // modify stack
							}
						}
						else if ((DWORD)dbgEvent.u.Exception.ExceptionRecord.ExceptionAddress == addrWowReadWtf) {
							hWtfFile = (HANDLE)ctx.Eax;  // take return value of CreateFileA()
							if (hWtfFile == INVALID_HANDLE_VALUE) {
								ctx.Dr0 = 0;  // remove break point
								bDoLoop = false;  // quit debug loop
							}
							else {
								ctx.Dr0 = addrCloseHandle;  // move breakpoint from return address to CloseHandle()
							}
						}
						else if ((DWORD)dbgEvent.u.Exception.ExceptionRecord.ExceptionAddress == addrCloseHandle) {
							// step through stack and find parameter containing file path
							DWORD dwCount, dwRetAddr;
							HANDLE hFileToClose;
							ReadProcessMemory(pi.hProcess, (LPVOID)(ctx.Esp), &dwRetAddr, 4, &dwCount);  // read return address
							ReadProcessMemory(pi.hProcess, (LPVOID)(ctx.Esp + 4), &hFileToClose, 4, &dwCount);  // read parameter

							// check if the about-to-close file is "realmlist.wtf"
							if (hFileToClose == hWtfFile)
								ctx.Dr0 = addrWowCloseWtf = dwRetAddr;  // move breakpoint from CloseHandle() to return address
						}
						else if ((DWORD)dbgEvent.u.Exception.ExceptionRecord.ExceptionAddress == addrWowCloseWtf) {
							ctx.Dr0 = 0;  // remove break point
							bDoLoop = false;  // quit debug loop
						}
						// this condition must be trigger by the Trap Flag in EFLAGS register
						else {
							ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_DEBUG_REGISTERS;  // set debug registers and EFLAGS
							ctx.EFlags &= ~0x100;  // clear trap flag
							ctx.Dr6 = 0;  // clear DR6
							ctx.Dr7 = 0x1;  // re-enable break point
							SetThreadContext(pi.hThread, &ctx);
							break;
						}

						// NOTE: we use Trap Flag instead of Resume Flag because RF is broken in the stupid Windows 2000 and XP
						if (ctx.Dr0 > 0)
							ctx.EFlags |= 0x100;  // set Trap Flag as long as DR0 is a valid address

						ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_DEBUG_REGISTERS;  // set debug registers and EFLAGS
						ctx.Dr6 = 0;  // clear DR6
						ctx.Dr7 = 0;  // disable break point
						SetThreadContext(pi.hThread, &ctx);
						break;
					}
					else if (dbgEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT) {
						break;  // simply continue
					}

					dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;  // forword if exception is other than a hardware breakpoint
					break;
				}

				case EXIT_PROCESS_DEBUG_EVENT:
				{
					bDoLoop = false;
					break;
				}

				default:
					break;
			}
			ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, dwContinueStatus);
		}
		DebugSetProcessKillOnExit(FALSE);
		DebugActiveProcessStop(pi.dwProcessId);  // detach

		return true;
	}

	// return true if an error has occured, false otherwise
	bool checkErrorTrace() {
		if (m_errorTrace.hasError()) {
			wchar_t* lpszErrMsg;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				m_errorTrace.getLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(wchar_t*)&lpszErrMsg,
				0,
				NULL
			);

			wchar_t* buffer = new wchar_t[wcslen(lpszErrMsg) + m_errorTrace.s_nMaxTaskLen + 128];
			wchar_t task[m_errorTrace.s_nMaxTaskLen];
			m_errorTrace.getTask(task, sizeof(task) / sizeof(task[0]));
			wsprintf(buffer, L"An error has occurred.\n\nOperation: %s.\nCode: 0x%x\nDetail: %s", task, m_errorTrace.getLastError(), lpszErrMsg);
			LocalFree(lpszErrMsg);
			MessageBox(getHandle(), buffer, APP_NAME, MB_OK | MB_ICONERROR);

			delete[] buffer;
			return true;
		}
		return false;
	}

	void infoMessageBox(const wchar_t* msg) {
		MessageBox(getHandle(), msg, APP_NAME, MB_OK | MB_ICONINFORMATION);
	}


	// gui
	GImageLabel* m_labelLogo;
	GLabel* m_labelServer;
	GComboBox* m_comboServers;
	GButton* m_buttonShortcut;
	GButton* m_buttonConfig;
	GButton* m_buttonRun;

	// data
	WowServerList m_listLiveSrv;
	WowServerList m_listTestSrv;
	std::string m_strPatchSrv;
	std::string m_strPortal;

	// error tracing
	ErrorTrace m_errorTrace;
};


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int argc = 0;
	wchar_t** argv = CommandLineToArgvW(GetCommandLine(), &argc);

	WowSelectMainWindow window;
	if (argc == 1)
		window.show();
	else
		return window.launchWowServer(argv[1]);

	return GMainEventLoop();
}
