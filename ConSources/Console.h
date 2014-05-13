/******************************************************************************
    
	This file is part of ConSources, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published 
	by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#pragma once

#include "DataDoubleLinkedList.h"
#include "WinException.h"
#include "ConSources.h"

class CConsoleWindow;
class CConsoleMainMenu;
class CConsolePopupMenu;
class CConsoleDialog;
class CONSOURCES_API CConsole: public CCppObject
{
public:
	typedef CDataDoubleLinkedListT<CConsoleWindow> CConsoleWindowList;

public:
	CConsole(void);
	virtual ~CConsole(void);

	void Start();
	void Exit();

	void Run();

	void CreateConsoleWindow(CConsoleWindow* pWindow);
	__inline CConsoleWindow* GetTopMostWindow() { return (m_WindowList.Count() == 0)?NULL:(*(m_WindowList.Begin())); }
	__inline Ref(CConsoleWindowList) GetConsoleWindowList() { return m_WindowList; }
	void BringToFront(CConsoleWindowList::Iterator it);

	void SetConsoleMainMenu(CConsoleMainMenu* pMainMenu);
	__inline CConsoleMainMenu* GetConsoleMainMenu() { return m_MainMenu; }

	void SetConsolePopupControl(CConsoleWindow* pControl);
	__inline CConsoleWindow* GetConsolePopupControl() { return m_PopupControl; }
	void CloseConsolePopupControl();

	void CreateConsolePopupMenu(CConsolePopupMenu* pPopupMenu, COORD pos);
	CConsolePopupMenu* GetCurrentConsolePopupMenu();
	void CloseConsolePopupMenu();

	void CreateConsoleModalDialog(CConsoleDialog* pDialog, COORD size);
	CConsoleDialog* GetCurrentConsoleModalDialog();
	void CloseConsoleModalDialog(CConsoleWindow* pWindow = NULL);

	void PostPaintEvent(CConsoleWindow* pWindow, bool repaintall = false);
	void MaximizeWindow(CConsoleWindow* pWindow);
	void CloseWindow(CConsoleWindow* pWindow);

	void ShowCursor();
	void SetCursorPos(COORD pos);
	void HideCursor();

	__inline bool HasMainMenu() { return (m_MainMenu != NULL); }

	color_t GetDefaultColor();
	color_t GetDefaultHighlightedColor();

	COORD GetScreenBufferSize() const { return m_ScreenBufferSize; }

protected:

#ifdef OK_SYS_WINDOWS
	bool DispatchMouseEvent(ConstRef(MOUSE_EVENT_RECORD) mouseEvent);
	bool DispatchKeyEvent(ConstRef(KEY_EVENT_RECORD) keyEvent);
	bool DispatchInputRecord(ConstRef(INPUT_RECORD) input);
#endif
#ifdef OK_SYS_UNIX
	bool DispatchMouseEvent(MEVENT* mouseEvent);
	bool DispatchKeyEvent(int keyEvent);
	bool DispatchInputRecord(int input);
#endif

	bool m_Continue;
	bool m_HasColors;
	bool m_HasMouse;
#ifdef OK_SYS_WINDOWS
	HANDLE m_StdInput;
	DWORD m_StdInputSaveOldMode;
	HANDLE m_StdOutput;
	HANDLE m_ConsoleOutput;
#endif
	COORD m_ScreenBufferSize;
	PCHAR_INFO m_ScreenBuffer;
	COORD m_LastWindowPos;
	CConsoleMainMenu* m_MainMenu;
	CConsoleWindowList m_WindowList;
	CConsoleWindowList m_PopupMenus;
	CConsoleWindowList m_ModalDialogs;
	CConsoleWindowList m_DeleteModalDialogs;
	CConsoleWindow* m_PopupControl;
	bool m_PopupControlRemove;
};

DECL_WINEXCEPTION(CONSOURCES_API, CConsoleException, CWinException)
