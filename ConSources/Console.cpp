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
#include "CON_PCH.H"
#include "Console.h"
#include "ConsoleWindow.h"
#include "ConsoleMainMenu.h"
#include "ConsolePopupMenu.h"
#include "ConsoleDialog.h"
#ifdef OK_SYS_UNIX
#include "ConsoleClipboard.h"
#endif
#include "ConsoleUndoManager.h"

IMPL_WINEXCEPTION(CConsoleException, CWinException)

#ifdef OK_SYS_WINDOWS
static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CConsoleException(ARGS_FILE_LINE _T("in %s CConsoleException"), func, CWinException::WinExtError);
}
#endif
#ifdef OK_SYS_UNIX
static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CConsoleException(ARGS_FILE_LINE _T("in %s CConsoleException"), func, CWinException::CRunTimeError);
}
#endif

CConsole::CConsole(void):
    m_Continue(true),
	m_HasColors(false),
	m_HasMouse(false),
#ifdef OK_SYS_WINDOWS
	m_StdInput(INVALID_HANDLE_VALUE),
	m_StdInputSaveOldMode(0),
    m_StdOutput(INVALID_HANDLE_VALUE),
	m_ConsoleOutput(INVALID_HANDLE_VALUE),
#endif
	m_ScreenBuffer(nullptr),
	m_MainMenu(),
	m_WindowList(__FILE__LINE__0),
	m_PopupMenus(__FILE__LINE__0),
	m_ModalDialogs(__FILE__LINE__0),
	m_DeleteModalDialogs(__FILE__LINE__0),
	m_PopupControl(nullptr),
	m_PopupControlRemove(false)
{
	m_ScreenBufferSize.X = 0;
	m_ScreenBufferSize.Y = 0;
	m_LastWindowPos.X = 0;
	m_LastWindowPos.Y = 0;
}

CConsole::~CConsole(void)
{
	if ( m_ScreenBuffer )
		TFfree(m_ScreenBuffer);
}

void CConsole::Start()
{
#ifdef OK_SYS_WINDOWS
	m_StdInput = GetStdHandle(STD_INPUT_HANDLE);
    if ( (m_StdInput == INVALID_HANDLE_VALUE) || (m_StdInput == nullptr) )
	{
		m_StdInput = INVALID_HANDLE_VALUE;
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));
	}

	if ( !GetConsoleMode(m_StdInput, &m_StdInputSaveOldMode) )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));

    if ( !SetConsoleMode(m_StdInput, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT) ) 
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));

	m_StdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if ( (m_StdOutput == INVALID_HANDLE_VALUE) || (m_StdOutput == nullptr) )
	{
		m_StdOutput = INVALID_HANDLE_VALUE;
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));
	}

	m_ConsoleOutput = CreateConsoleScreenBuffer( 
		GENERIC_READ | GENERIC_WRITE,       // read/write access 
		FILE_SHARE_READ | FILE_SHARE_WRITE, // shared 
		nullptr,                               // default security attributes 
		CONSOLE_TEXTMODE_BUFFER,            // must be TEXTMODE 
		nullptr);                              // reserved; must be nullptr 
	if ( m_ConsoleOutput == INVALID_HANDLE_VALUE )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));

    if ( !SetConsoleActiveScreenBuffer(m_ConsoleOutput) ) 
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));

	CONSOLE_CURSOR_INFO cursorInfo;

	if ( !GetConsoleCursorInfo(m_ConsoleOutput, &cursorInfo) )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));
	cursorInfo.bVisible = FALSE;
	if ( !SetConsoleCursorInfo(m_ConsoleOutput, &cursorInfo) )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));

#ifdef OK_COMP_GNUC
    m_ScreenBufferSize.X = 80;
    m_ScreenBufferSize.Y = 25;
#endif
#ifdef OK_COMP_MSC
	CONSOLE_SCREEN_BUFFER_INFOEX screenInfoEx;
	bool bChanged = false;

	screenInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	if ( !GetConsoleScreenBufferInfoEx(m_ConsoleOutput, &screenInfoEx) )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));

	if ( screenInfoEx.dwSize.X < 80 )
	{
		screenInfoEx.dwSize.X = 80;
		bChanged = true;
	}
	if ( screenInfoEx.dwSize.Y < 50 )
	{
		screenInfoEx.dwSize.Y = 50;
		bChanged = true;
	}
	if ( bChanged )
	{
		screenInfoEx.srWindow.Bottom = screenInfoEx.srWindow.Top + screenInfoEx.dwSize.Y - 1;
		screenInfoEx.srWindow.Right = screenInfoEx.srWindow.Left + screenInfoEx.dwSize.X - 1;
		if ( !SetConsoleScreenBufferInfoEx(m_ConsoleOutput, &screenInfoEx) )
			ThrowDefaultException(__FILE__LINE__ _T("CConsole::Start"));
	}
	m_ScreenBufferSize = screenInfoEx.dwSize;
#endif
	m_HasColors = true;
	m_HasMouse = true;
#endif
#ifdef OK_SYS_UNIX
	initscr(); 
	cbreak(); 
	noecho();
	nonl();
	keypad(stdscr, TRUE);

	if ( has_colors() )
	{
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		init_pair(2, COLOR_BLUE, COLOR_WHITE);
		bkgdset(0x20 | COLOR_PAIR(1));
		m_HasColors = true;
	}
	if ( has_mouse() )
	{
		mousemask(ALL_MOUSE_EVENTS, nullptr);
		m_HasMouse = true;
	}
	getmaxyx(stdscr, m_ScreenBufferSize.Y, m_ScreenBufferSize.X);
#endif
	m_ScreenBuffer = CastAnyPtr(CHAR_INFO, TFalloc(
	    m_ScreenBufferSize.X * m_ScreenBufferSize.Y * sizeof(CHAR_INFO)));

	m_LastWindowPos.X = 0;
	m_LastWindowPos.Y = 0;
}

void CConsole::ShowCursor()
{
#ifdef OK_SYS_WINDOWS
	CONSOLE_CURSOR_INFO cursorInfo;

	if ( !GetConsoleCursorInfo(m_ConsoleOutput, &cursorInfo) )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::ShowCursor"));
	if ( cursorInfo.bVisible )
		return;
	cursorInfo.bVisible = TRUE;
	if ( !SetConsoleCursorInfo(m_ConsoleOutput, &cursorInfo) )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::ShowCursor"));
#endif
}

void CConsole::SetCursorPos(COORD pos)
{
	if ( (pos.X < 0) || (pos.Y < 0) || (pos.X > (m_ScreenBufferSize.X - 1)) || (pos.Y > (m_ScreenBufferSize.Y - 1)) )
	{
		HideCursor();
		return;
	}
	ShowCursor();
#ifdef OK_SYS_WINDOWS
	if (!SetConsoleCursorPosition(m_ConsoleOutput, pos))
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::SetCursorPos"));
#endif
#ifdef OK_SYS_UNIX
	move(pos.Y, pos.X);
	refresh();
#endif
}

void CConsole::HideCursor()
{
#ifdef OK_SYS_WINDOWS
	CONSOLE_CURSOR_INFO cursorInfo;

	if ( !GetConsoleCursorInfo(m_ConsoleOutput, &cursorInfo) )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::HideCursor"));
	if ( !(cursorInfo.bVisible) )
		return;
	cursorInfo.bVisible = FALSE;
	if ( !SetConsoleCursorInfo(m_ConsoleOutput, &cursorInfo) )
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::HideCursor"));
#endif
}

void CConsole::Exit()
{
#ifdef OK_SYS_WINDOWS
	if (m_StdInput != INVALID_HANDLE_VALUE)
	{
		if ( !SetConsoleMode(m_StdInput, m_StdInputSaveOldMode) ) 
			ThrowDefaultException(__FILE__LINE__ _T("CConsole::Exit"));
		m_StdInput = INVALID_HANDLE_VALUE;
	}
	if ( m_StdOutput != INVALID_HANDLE_VALUE )
	{
		if ( !SetConsoleActiveScreenBuffer(m_StdOutput) ) 
			ThrowDefaultException(__FILE__LINE__ _T("CConsole::Exit"));
		m_StdOutput = INVALID_HANDLE_VALUE;
	}
	if ( m_ConsoleOutput != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_ConsoleOutput);
		m_ConsoleOutput = INVALID_HANDLE_VALUE;
	}
#endif
#ifdef OK_SYS_UNIX
	echo();
	endwin();

	CConsoleClipboard::FreeInstance();
#endif
	CConsoleUndoManager::FreeInstance();
}

void CConsole::Run()
{
#ifdef OK_SYS_WINDOWS
	INPUT_RECORD inputBuffer[128];
	DWORD cNumRead;
	DWORD ix;

	while ( m_Continue )
	{
		if ( m_PopupControlRemove )
		{
			m_PopupControl = nullptr;
			PostPaintEvent(nullptr, true);
			m_PopupControlRemove = false;
		}
		while ( m_DeleteModalDialogs.Count() > 0 )
		{
			CConsoleWindowList::Iterator it = m_DeleteModalDialogs.Begin();

			m_DeleteModalDialogs.Remove(it);
		}

		if ( !ReadConsoleInput( 
			m_StdInput,      // input buffer handle 
			inputBuffer,     // buffer to read into 
			128,             // size of read buffer 
			&cNumRead) )     // number of records read 
			ThrowDefaultException(__FILE__LINE__ _T("CConsole::Run"));

		for ( ix = 0; ix < cNumRead; ++ix )
		{
			if ( !DispatchInputRecord(inputBuffer[ix]) )
			{
				m_Continue = false;
				break;
			}
		}
	}
#endif
#ifdef OK_SYS_UNIX
	int input;

	while ( m_Continue )
	{
		if ( m_PopupControlRemove )
		{
			m_PopupControl = nullptr;
			PostPaintEvent(nullptr, true);
			m_PopupControlRemove = false;
		}
		while ( m_DeleteModalDialogs.Count() > 0 )
		{
			CConsoleWindowList::Iterator it = m_DeleteModalDialogs.Begin();

			m_DeleteModalDialogs.Remove(it);
		}

		input = getch();
		if ( input == ERR )
		{
			ThrowDefaultException(__FILE__LINE__ _T("CConsole::Run"));
			m_Continue = false;
		}
		else if ( !DispatchInputRecord(input) )
			m_Continue = false;
	}

#endif
}

#ifdef OK_SYS_WINDOWS
bool CConsole::DispatchMouseEvent(ConstRef(MOUSE_EVENT_RECORD) mouseEvent)
{
	switch ( mouseEvent.dwEventFlags )
	{
	case 0:
		if ( (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
			 || (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) )
		{
			if ( m_PopupControl )
			{
				if ( m_PopupControl->IsPosInClientRect(mouseEvent.dwMousePosition) 
					 || m_PopupControl->IsPosInNonClientRect(mouseEvent.dwMousePosition) )
					 return m_PopupControl->MouseEventProc(mouseEvent);
				else
					CloseConsolePopupControl();
			}

			if ( m_ModalDialogs.Count() > 0 )
			{
				CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

				return pDialog->MouseEventProc(mouseEvent);
			}

			if ( m_PopupMenus.Count() > 0 )
			{
				CConsolePopupMenu* pPopupMenu = CastDynamicPtr(CConsolePopupMenu, *(m_PopupMenus.Begin()));
				bool hasFired = false;

				if ( NotPtrCheck(pPopupMenu) && (pPopupMenu->IsPosInClientRect(mouseEvent.dwMousePosition) 
					 || pPopupMenu->IsPosInNonClientRect(mouseEvent.dwMousePosition)) )
				{
					 pPopupMenu->MouseEventProc(mouseEvent);
					 hasFired = true;
				}
				CloseConsolePopupMenu();
				if ( hasFired )
					return true;
			}

			if ( m_MainMenu && (m_MainMenu->IsPosInClientRect(mouseEvent.dwMousePosition)) )
				return m_MainMenu->MouseEventProc(mouseEvent);

			if ( m_WindowList.Count() > 0 )
			{
				CConsoleWindowList::Iterator it(m_WindowList.Begin());
				CConsoleWindow* pWindow = *it;

				if ( pWindow->IsPosInClientRect(mouseEvent.dwMousePosition) 
					 || pWindow->IsPosInNonClientRect(mouseEvent.dwMousePosition) )
					 return pWindow->MouseEventProc(mouseEvent);
				 ++it;
				 while ( it )
				 {
					 pWindow = *it;
					 if ( pWindow->IsPosInClientRect(mouseEvent.dwMousePosition) 
						 || pWindow->IsPosInNonClientRect(mouseEvent.dwMousePosition) )
					 {
						 BringToFront(pWindow);
						 return true;
					 }
					 ++it;
				 }
			}
		}
		else if ( mouseEvent.dwButtonState == 0 )
		{
			if ( m_PopupControl )
				return m_PopupControl->MouseEventProc(mouseEvent);

			if ( m_ModalDialogs.Count() > 0 )
			{
				CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

				return pDialog->MouseEventProc(mouseEvent);
			}

			if ( m_WindowList.Count() > 0 )
			{
				CConsoleWindow* pWindow = *(m_WindowList.Begin());

				pWindow->MouseEventProc(mouseEvent);
			}
		}
		break;
	case DOUBLE_CLICK:
		if ( (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
			 || (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) )
		{
			if ( m_PopupControl )
				return m_PopupControl->MouseEventProc(mouseEvent);

			if ( m_ModalDialogs.Count() > 0 )
			{
				CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

				return pDialog->MouseEventProc(mouseEvent);
			}

			if ( m_WindowList.Count() > 0 )
			{
				CConsoleWindow* pWindow = *(m_WindowList.Begin());

				pWindow->MouseEventProc(mouseEvent);
			}
		}
		break;
	case MOUSE_MOVED:
		if ( (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
			 || (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) )
		{
			if ( m_PopupControl )
				return m_PopupControl->MouseEventProc(mouseEvent);

			if ( m_ModalDialogs.Count() > 0 )
			{
				CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

				return pDialog->MouseEventProc(mouseEvent);
			}

			if ( m_WindowList.Count() > 0 )
			{
				CConsoleWindow* pWindow = *(m_WindowList.Begin());

				pWindow->MouseEventProc(mouseEvent);
			}
		}
		break;
	case MOUSE_WHEELED:
		if ( _Lowword(mouseEvent.dwButtonState) == 0 )
		{
			if ( m_PopupControl )
				return m_PopupControl->MouseEventProc(mouseEvent);

			if ( m_ModalDialogs.Count() > 0 )
			{
				CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

				return pDialog->MouseEventProc(mouseEvent);
			}

			if ( m_WindowList.Count() > 0 )
			{
				CConsoleWindow* pWindow = *(m_WindowList.Begin());

				pWindow->MouseEventProc(mouseEvent);
			}
		}
		break;
	default:
		break;
	}
	return true;
}

bool CConsole::DispatchKeyEvent(ConstRef(KEY_EVENT_RECORD) keyEvent)
{
	if ( keyEvent.bKeyDown 
		&& ((keyEvent.dwControlKeyState & CConsoleWindow::KeyStateAllMask) == CConsoleWindow::KeyStateAltMask)
		&& (keyEvent.wVirtualKeyCode == VK_F4) )
		return false;

	if ( keyEvent.bKeyDown 
		&& ((keyEvent.dwControlKeyState & CConsoleWindow::KeyStateAllMask) == 0)
		&& (keyEvent.wVirtualKeyCode == VK_F10) )
	{
		if ( m_MainMenu )
			m_MainMenu->OpenFirstPopupMenu();
		return true;
	}

	if ( keyEvent.bKeyDown )
	{
		if ( m_MainMenu && m_MainMenu->TestKeyCode(keyEvent.wVirtualKeyCode, keyEvent.dwControlKeyState) )
			return true;
	}

	CConsolePopupMenu* pMenu = GetCurrentConsolePopupMenu();

	if ( pMenu )
	{
		if ( keyEvent.bKeyDown 
			&& ((keyEvent.dwControlKeyState & CConsoleWindow::KeyStateAllMask) == 0)
			&& (keyEvent.wVirtualKeyCode == VK_ESCAPE) )
		{
			CloseConsolePopupMenu();
			return true;
		}
		return pMenu->KeyEventProc(keyEvent);
	}

	if ( m_PopupControl )
	{
		if ( keyEvent.bKeyDown 
			&& ((keyEvent.dwControlKeyState & CConsoleWindow::KeyStateAllMask) == 0)
			&& (keyEvent.wVirtualKeyCode == VK_ESCAPE) )
		{
			CloseConsolePopupControl();
			return true;
		}
		return m_PopupControl->KeyEventProc(keyEvent);
	}

	if ( m_ModalDialogs.Count() > 0 )
	{
		CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

		if ( keyEvent.bKeyDown 
			&& ((keyEvent.dwControlKeyState & CConsoleWindow::KeyStateAllExceptCtrlMask) == 0)
			&& ((keyEvent.dwControlKeyState & CConsoleWindow::KeyStateCtrlMask) != 0)
			&& (keyEvent.wVirtualKeyCode == VK_F4) )
		{
			CloseConsoleModalDialog(pDialog);
			return true;
		}
		return pDialog->KeyEventProc(keyEvent);
	}

	if ( m_WindowList.Count() > 0 )
	{	
		CConsoleWindow* pWindow = *(m_WindowList.Begin());

		if ( keyEvent.bKeyDown 
			&& ((keyEvent.dwControlKeyState & CConsoleWindow::KeyStateAllExceptCtrlMask) == 0)
			&& ((keyEvent.dwControlKeyState & CConsoleWindow::KeyStateCtrlMask) != 0)
			&& (keyEvent.wVirtualKeyCode == VK_F4) )
		{
			CloseWindow(pWindow);
			return true;
		}
		return pWindow->KeyEventProc(keyEvent);
	}
	return true;
}

bool CConsole::DispatchInputRecord(ConstRef(INPUT_RECORD) input)
{
	switch ( input.EventType )
	{
	case KEY_EVENT:
		if ( !DispatchKeyEvent(input.Event.KeyEvent) )
			return false;
		break;
	case MOUSE_EVENT:
		if ( !DispatchMouseEvent(input.Event.MouseEvent) )
			return false;
		break;
	case WINDOW_BUFFER_SIZE_EVENT:
		{
#ifdef OK_COMP_GNUC
            m_ScreenBufferSize.X = 80;
            m_ScreenBufferSize.Y = 25;
#endif
#ifdef OK_COMP_MSC
			CONSOLE_SCREEN_BUFFER_INFOEX screenInfoEx;
			bool bChanged = false;

			screenInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
			if ( !GetConsoleScreenBufferInfoEx(m_ConsoleOutput, &screenInfoEx) )
				ThrowDefaultException(__FILE__LINE__ _T("CConsole::DispatchInputRecord"));

			if ( screenInfoEx.dwSize.X < 80 )
			{
				screenInfoEx.dwSize.X = 80;
				bChanged = true;
			}
			if ( screenInfoEx.dwSize.Y < 50 )
			{
				screenInfoEx.dwSize.Y = 50;
				bChanged = true;
			}
			if ( bChanged )
			{
				screenInfoEx.srWindow.Bottom = screenInfoEx.srWindow.Top + screenInfoEx.dwSize.Y - 1;
				screenInfoEx.srWindow.Right = screenInfoEx.srWindow.Left + screenInfoEx.dwSize.X - 1;
				if ( !SetConsoleScreenBufferInfoEx(m_ConsoleOutput, &screenInfoEx) )
					ThrowDefaultException(__FILE__LINE__ _T("CConsole::DispatchInputRecord"));
			}
			m_ScreenBufferSize = screenInfoEx.dwSize;
#endif
			if ( m_ScreenBuffer )
				TFfree(m_ScreenBuffer);
			m_ScreenBuffer = CastAnyPtr(CHAR_INFO, TFalloc(
				m_ScreenBufferSize.X * m_ScreenBufferSize.Y * sizeof(CHAR_INFO)));
			if ( m_MainMenu )
			{
				COORD size;

				size.X = m_ScreenBufferSize.X;
				size.Y = 1;
				m_MainMenu->ResizeWindow(size, false);
			}
			PostPaintEvent(nullptr, true);
		}
		break;
	default:
		break;
	}
	return true;
}
#endif
#ifdef OK_SYS_UNIX
bool CConsole::DispatchMouseEvent(MEVENT* mouseEvent)
{
	return true;
}

bool CConsole::DispatchKeyEvent(int keyEvent)
{
	int vKey = 0;
	int keyMod = 0;

	switch ( keyEvent )
	{
	case 0x09:
		vKey = 0x09;
		break;
	case 0x0A:
	case 0x0D:
		vKey = KEY_ENTER;
		break;
	case 0x1B:
		vKey = 0x1B;
		break;
	default:
		if ( keyEvent < 0x20 )
		{
			vKey = keyEvent + 0x40;
			keyMod = CConsoleWindow::KeyStateCtrlMask;
		}
		else
			vKey = keyEvent;
		break;
	}
	if ( vKey == KEY_F(10) )
	{
		if ( m_MainMenu )
			m_MainMenu->OpenFirstPopupMenu();
		return true;
	}

	if ( m_MainMenu && m_MainMenu->TestKeyCode(vKey, keyMod) )
		return true;

	CConsolePopupMenu* pMenu = GetCurrentConsolePopupMenu();

	if ( pMenu )
	{
		if ( vKey == 0x1B )
		{
			CloseConsolePopupMenu();
			return true;
		}
		return pMenu->KeyEventProc(vKey, keyMod);
	}

	if ( m_PopupControl )
	{
		if ( vKey == 0x1B )
		{
			CloseConsolePopupControl();
			return true;
		}
		return m_PopupControl->KeyEventProc(vKey, keyMod);
	}

	if ( m_ModalDialogs.Count() > 0 )
	{
		CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

		if ( vKey == KEY_F(4) )
		{
			CloseConsoleModalDialog(pDialog);
			return true;
		}
		return pDialog->KeyEventProc(vKey, keyMod);
	}

	if ( m_WindowList.Count() > 0 )
	{	
		CConsoleWindow* pWindow = *(m_WindowList.Begin());

		if ( vKey == KEY_F(4) )
		{
			CloseWindow(pWindow);
			return true;
		}
		return pWindow->KeyEventProc(vKey, keyMod);
	}
	if ( vKey == KEY_F(4) )
		return false;
	return true;
}

bool CConsole::DispatchInputRecord(int input)
{
	switch ( input )
	{
	case KEY_MOUSE:
		{
			MEVENT mouseEvent;

			if ( getmouse(&mouseEvent) == ERR )
				ThrowDefaultException(__FILE__LINE__ _T("CConsole::DispatchInputRecord"));
			if ( !DispatchMouseEvent(&mouseEvent) )
				return false;
		}
		break;
	case KEY_RESIZE:
		getmaxyx(stdscr, m_ScreenBufferSize.Y, m_ScreenBufferSize.X);
		if ( m_ScreenBuffer )
			TFfree(m_ScreenBuffer);
		m_ScreenBuffer = CastAnyPtr(CHAR_INFO, TFalloc(
			m_ScreenBufferSize.X * m_ScreenBufferSize.Y * sizeof(CHAR_INFO)));
		if ( m_MainMenu )
		{
			COORD size;

			size.X = m_ScreenBufferSize.X;
			size.Y = 1;
			m_MainMenu->ResizeWindow(size, false);
		}
		PostPaintEvent(nullptr, true);
		break;
	default:
		if ( !DispatchKeyEvent(input) )
			return false;
		break;
	}
	return true;
}
#endif

void CConsole::BringToFront(CConsoleWindow* pWindow)
{
	CConsoleWindowNullList::Iterator it = m_WindowList.Find<CCppObjectEqualFunctor<CConsoleWindow> >(pWindow);

	if (!it)
		return;
	m_WindowList.Remove(it);
	m_WindowList.Prepend(pWindow);

	PostPaintEvent(pWindow);
	pWindow->SetFocus(true);
}

void CConsole::CreateConsoleWindow(CConsoleWindow* pWindow)
{
	if ( m_ModalDialogs.Count() == 0 )
	{
		if ( m_WindowList.Count() > 0 )
		{
			CConsoleWindow* pWindow = *(m_WindowList.Begin());

			pWindow->SetFocus(false);
		}
	}
	else
	{
		CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

		pDialog->SetFocus(false);
	}
	assert(pWindow != nullptr);

	if ( NotPtrCheck(m_MainMenu) && (m_LastWindowPos.Y == 0) )
		m_LastWindowPos.Y = 1;

	COORD sz = m_ScreenBufferSize;

	sz.X /= 2;
	sz.Y /= 2;
	m_WindowList.Prepend(pWindow);
	pWindow->Create(m_LastWindowPos, sz);
	PostPaintEvent(pWindow);
	pWindow->SetFocus(true);

	m_LastWindowPos.X += 2;
	m_LastWindowPos.Y += 2;

	if ( (m_LastWindowPos.X > (m_ScreenBufferSize.X - 3))
		|| (m_LastWindowPos.Y > (m_ScreenBufferSize.Y - 3)) )
	{
		m_LastWindowPos.X = 0;
		m_LastWindowPos.Y = NotPtrCheck(m_MainMenu)?1:0;
	}
}

void CConsole::SetConsoleMainMenu(CConsoleMainMenu* pMainMenu)
{
	assert(pMainMenu != nullptr);

	COORD pos;
	COORD sz;

	m_MainMenu = pMainMenu;
	pos.X = 0;
	pos.Y = 0;
	sz.X = m_ScreenBufferSize.X;
	sz.Y = 1;
	m_MainMenu->Create(pos, sz);
	PostPaintEvent(m_MainMenu, true);
}

void CConsole::SetConsolePopupControl(CConsoleWindow* pControl)
{
	PostPaintEvent(pControl);
	m_PopupControl = pControl;
}

void CConsole::CloseConsolePopupControl()
{
	m_PopupControlRemove = true;
}

void CConsole::CreateConsolePopupMenu(CConsolePopupMenu* pPopupMenu, COORD pos)
{
	assert(pPopupMenu != nullptr);

	m_PopupMenus.Prepend(pPopupMenu);
	if ( pPopupMenu->IsCreated() )
		pPopupMenu->MoveWindow(pos, true, true);
	else
	{
		pPopupMenu->Create(pos, m_ScreenBufferSize);
		PostPaintEvent(pPopupMenu);
	}
}

CConsolePopupMenu* CConsole::GetCurrentConsolePopupMenu()
{
	if ( m_PopupMenus.Count() == 0 )
		return nullptr;
	return CastDynamicPtr(CConsolePopupMenu, *(m_PopupMenus.Begin()));
}

void CConsole::CloseConsolePopupMenu()
{
	if ( m_PopupMenus.Count() > 0 )
	{
		m_PopupMenus.Remove(m_PopupMenus.Begin());
		PostPaintEvent(nullptr, true);
	}
}

void CConsole::CreateConsoleModalDialog(CConsoleDialog* pDialog, COORD size)
{
	if ( m_ModalDialogs.Count() == 0 )
	{
		if ( m_WindowList.Count() > 0 )
		{
			CConsoleWindow* pWindow = *(m_WindowList.Begin());

			pWindow->SetFocus(false);
		}
	}
	else
	{
		CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

		pDialog->SetFocus(false);
	}
	assert(pDialog != nullptr);
	COORD pos;

	pos.X = (m_ScreenBufferSize.X - size.X) / 2;
	pos.Y = (m_ScreenBufferSize.Y - size.Y) / 2;
	if ( NotPtrCheck(m_MainMenu) )
	{
		if ( pos.Y < 1 )
			pos.Y = 1;
	}
	else
	{
		if ( pos.Y < 0 )
			pos.Y = 0;
	}
	m_ModalDialogs.Prepend(pDialog);

	pDialog->Create(pos, size);
	PostPaintEvent(pDialog);
	pDialog->SetFocus(true);
}

CConsoleDialog* CConsole::GetCurrentConsoleModalDialog()
{
	if ( m_ModalDialogs.Count() > 0 )
	{
		CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

		return pDialog;
	}
	return nullptr;
}

void CConsole::CloseConsoleModalDialog(CConsoleWindow* pWindow)
{
	if ( m_ModalDialogs.Count() > 0 )
	{
		CConsoleWindowNullList::Iterator it = 
			PtrCheck(pWindow) ? (m_ModalDialogs.Begin()) : (m_ModalDialogs.Find<CCppObjectEqualFunctor<CConsoleWindow>>(pWindow));

		if ( it )
		{
			CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *it);

			pDialog->SetFocus(false);
			m_ModalDialogs.Remove(it);
			m_DeleteModalDialogs.Append(pDialog);
			PostPaintEvent(nullptr, true);
		}
		else
			return;
	}
	if ( m_ModalDialogs.Count() == 0 )
	{
		if ( m_WindowList.Count() > 0 )
		{
			CConsoleWindow* pWindow = *(m_WindowList.Begin());

			pWindow->SetFocus(true);
		}
	}
	else
	{
		CConsoleDialog* pDialog = CastDynamicPtr(CConsoleDialog, *(m_ModalDialogs.Begin()));

		pDialog->SetFocus(true);
	}
}

void CConsole::PostPaintEvent(CConsoleWindow* pWindow, bool repaintall)
{
	assert(m_ScreenBuffer != nullptr);

	if ( repaintall )
	{
		sword max = m_ScreenBufferSize.X * m_ScreenBufferSize.Y;
		sword ix;
		PCHAR_INFO p;

		for ( ix = 0, p = m_ScreenBuffer; ix < max; ++ix, ++p )
		{
#ifdef _UNICODE
			p->Char.UnicodeChar = 0x0020;
#else
			p->Char.AsciiChar = 0x20;
#endif
			p->Attributes = GetDefaultColor();
		}

		if ( m_MainMenu )
			m_MainMenu->Paint(m_ScreenBufferSize, m_ScreenBuffer);

		CConsoleWindowNullList::Iterator it;
		CConsoleWindow* pWindow1;

		it = m_WindowList.Last();
		while ( it )
		{
			pWindow1 = *it;
			if ( pWindow1 == pWindow )
				pWindow = nullptr;
			pWindow1->Paint(m_ScreenBufferSize, m_ScreenBuffer);
			--it;
		}

		it = m_ModalDialogs.Last();
		while ( it )
		{
			pWindow1 = *it;
			if ( pWindow1 == pWindow )
				pWindow = nullptr;
			pWindow1->Paint(m_ScreenBufferSize, m_ScreenBuffer);
			--it;
		}

		it = m_PopupMenus.Last();
		while ( it )
		{
			pWindow1 = *it;
			if ( pWindow1 == pWindow )
				pWindow = nullptr;
			pWindow1->Paint(m_ScreenBufferSize, m_ScreenBuffer);
			--it;
		}
	}
	if ( NotPtrCheck(pWindow) )
		pWindow->Paint(m_ScreenBufferSize, m_ScreenBuffer);

#ifdef OK_SYS_WINDOWS
	COORD lefttop;
	SMALL_RECT rect;

	lefttop.X = 0;
	lefttop.Y = 0;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = m_ScreenBufferSize.X - 1;
	rect.Bottom = m_ScreenBufferSize.Y - 1;

	if ( !WriteConsoleOutput( 
			m_ConsoleOutput,        // screen buffer to write to 
			m_ScreenBuffer,         // buffer to copy from 
			m_ScreenBufferSize,     // col-row size of chiBuffer 
			lefttop,                // top left src cell in chiBuffer 
			&rect) )                // dest. screen buffer rectangle 
		ThrowDefaultException(__FILE__LINE__ _T("CConsole::PostPaintEvent"));
#endif
#ifdef OK_SYS_UNIX
	for ( int y = 0; y < m_ScreenBufferSize.Y; ++y )
	{
		WORD ix = m_ScreenBufferSize.X * y;

		for ( int x = 0; x < m_ScreenBufferSize.X; ++x, ++ix )
		{
			PCHAR_INFO p = m_ScreenBuffer + ix;
			chtype ch = p->Char.AsciiChar;

			ch |= p->Attributes;
			mvaddch(y, x, ch);
		}
	}
#endif
}

void CConsole::MaximizeWindow(CConsoleWindow* pWindow)
{
	COORD possz;

	possz.X = 0;
	possz.Y = 0;
	if ( m_MainMenu )
		possz.Y = 1;
	pWindow->MoveWindow(possz, false, true);

	possz.X = m_ScreenBufferSize.X;
	possz.Y = m_ScreenBufferSize.Y;
	if ( m_MainMenu )
		--(possz.Y);
	pWindow->ResizeWindow(possz, false);
	PostPaintEvent(nullptr, true);
}

void CConsole::CloseWindow(CConsoleWindow* pWindow)
{
	CConsoleWindowNullList::Iterator it;

	it = m_ModalDialogs.Find<CCppObjectEqualFunctor<CConsoleWindow> >(pWindow);
	if ( it )
	{
		CloseConsoleModalDialog(*it);
		return;
	}

	it = m_WindowList.Find<CCppObjectEqualFunctor<CConsoleWindow> >(pWindow);
	if ( it )
	{
		(*it)->SetFocus(false);
		m_WindowList.Remove<CCppObjectReleaseFunctor<CConsoleWindow> >(it);
		PostPaintEvent(nullptr, true);
		if ( m_WindowList.Count() > 0 )
		{
			CConsoleWindow* pWindow = *(m_WindowList.Begin());

			pWindow->SetFocus(true);
		}
	}
}

color_t CConsole::GetDefaultColor()
{
#ifdef OK_SYS_WINDOWS
	return BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
#endif
#ifdef OK_SYS_UNIX
	if ( m_HasColors )
		return COLOR_PAIR(1);
	return 0;
#endif
}

color_t CConsole::GetDefaultHighlightedColor()
{
#ifdef OK_SYS_WINDOWS
	return FOREGROUND_BLUE | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
#endif
#ifdef OK_SYS_UNIX
	if ( m_HasColors )
		return COLOR_PAIR(2);
	return 0;
#endif
}
