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
#include "ConsolePopupMenu.h"
#include "ConsoleMainMenu.h"
#include "ConsoleMenuItem.h"
#include "Console.h"

CAbstractConsoleMenuNavigationCallback::CAbstractConsoleMenuNavigationCallback()
{
}

CAbstractConsoleMenuNavigationCallback::CAbstractConsoleMenuNavigationCallback(const CAbstractConsoleMenuNavigationCallback&)
{
}

CAbstractConsoleMenuNavigationCallback::~CAbstractConsoleMenuNavigationCallback()
{
}

static void __stdcall MenuItemListDeleteFunc( ConstPointer data, Pointer context )
{
	CConsoleMenuItem* p = CastAnyPtr(CConsoleMenuItem, CastMutable(Pointer, data));

	p->release();
}

CConsolePopupMenu::CConsolePopupMenu(CConstPointer name, CConsole* pConsole):
    CConsoleWindow(name, pConsole),
	m_MenuItemList(__FILE__LINE__0),
	m_MenuCurrent(),
	m_Callback(NULL)
{
}

CConsolePopupMenu::CConsolePopupMenu(CConstPointer name, CConstPointer title, CConsole* pConsole ):
    CConsoleWindow(name, title, pConsole),
	m_MenuItemList(__FILE__LINE__0),
	m_MenuCurrent(),
	m_Callback(NULL)
{
}

CConsolePopupMenu::CConsolePopupMenu(CConsoleMainMenu* mainMenu, CConstPointer name, CConstPointer title, CConsole* pConsole ):
    CConsoleWindow(name, title, pConsole),
	m_MenuItemList(__FILE__LINE__0),
	m_MenuCurrent(),
	m_Callback(NULL)
{
	if ( mainMenu )
		m_Callback = OK_NEW_OPERATOR CConsoleMenuNavigationCallback<CConsoleMainMenu>(
		    mainMenu, &CConsoleMainMenu::MenuNavigation);
}

CConsolePopupMenu::~CConsolePopupMenu(void)
{
	m_MenuItemList.Close();
	if ( m_Callback )
		m_Callback->release();
}

void CConsolePopupMenu::DoMenuItem(CConsoleMenuItem* pMenuItem)
{
	if ( pMenuItem->HasSubMenu() )
	{
		if ( m_Console )
		{
			SMALL_RECT rect = pMenuItem->GetNonClientArea();
			COORD pos;

			pos.X = rect.Right;
			pos.Y = rect.Top;
			m_Console->CreateConsolePopupMenu(pMenuItem->GetSubMenu(), pos);
		}
	}
	else
	{
		while ( NotPtrCheck(m_Console->GetCurrentConsolePopupMenu()) )
			m_Console->CloseConsolePopupMenu();
		pMenuItem->InvokeMenuItemCommand();
	}
}

void CConsolePopupMenu::ClearMenus()
{
	m_MenuItemList.Close();
	m_MenuItemList.Open(__FILE__LINE__0);
}

void CConsolePopupMenu::AddMenuItem(CConsoleMenuItem* pMenuItem)
{
	m_MenuItemList.Append(pMenuItem);
}

bool CConsolePopupMenu::TestKeyCode(WORD vkey, DWORD kmod)
{
	if ( m_MenuItemList.Count() > 0 )
	{
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;

		while ( it )
		{
			pMenuItem = *it;
			if ( pMenuItem->TestKeyCode(vkey, kmod) )
			{
				DoMenuItem(pMenuItem);
				return true;
			}
			++it;
		}
	}
	return false;
}

void CConsolePopupMenu::DrawMenuItemList()
{
	if ( m_MenuItemList.Count() > 0 )
	{
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;

		while ( it )
		{
			pMenuItem = *it;
			pMenuItem->DrawMenuItem(m_MenuCurrent == it);
			++it;
		}
	}
}

void CConsolePopupMenu::Create(COORD pos, COORD size)
{
	COORD size1;

	size1.X = 0;
	size1.Y = 0;
	if ( m_MenuItemList.Count() > 0 )
	{
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;
		sword sz;

		while ( it )
		{
			pMenuItem = *it;
			sz = Cast(sword, pMenuItem->GetTitleSize());
			if ( sz > size1.X )
				size1.X = sz;
			++(size1.Y);
			++it;
		}
	}
	size1.X += 2;
	size1.Y += 2;

	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	m_hasBorder = true;
	m_BorderStyle = singleborderstyle;

	CConsoleWindow::Create(pos, size1);

	if ( m_MenuItemList.Count() > 0 )
	{
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;
		COORD pos2;
		COORD size2;

		pos2.X = m_ClientArea.Left;
		pos2.Y = m_ClientArea.Top;
		size2.X = m_ScreenBufferSize.X - 2;
		size2.Y = 1;
		while ( it )
		{
			pMenuItem = *it;
			pMenuItem->Create(pos2, size2);
			++(pos2.Y);
			++it;
		}
		m_MenuCurrent = m_MenuItemList.Begin();
		pMenuItem = *m_MenuCurrent;
		pMenuItem->DrawMenuItem(true);
	}
}

void CConsolePopupMenu::Paint(COORD bufferSize, PCHAR_INFO buffer)
{
	CConsoleWindow::Paint(bufferSize, buffer);

	if ( m_MenuItemList.Count() > 0 )
	{
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;

		while ( it )
		{
			pMenuItem = *it;
			pMenuItem->Paint(bufferSize, buffer);
			++it;
		}
	}
}

bool CConsolePopupMenu::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;
	switch ( virtualKeyCode )
	{
	CASE_KEY_RIGHT:
		if ( m_Callback )
			m_Callback->invoke(this, CAbstractConsoleMenuNavigationCallback::NavigationRight);
		break;
	CASE_KEY_LEFT:
		if ( m_Callback )
			m_Callback->invoke(this, CAbstractConsoleMenuNavigationCallback::NavigationLeft);
		break;
	CASE_KEY_DOWN:
		if ( m_MenuCurrent == m_MenuItemList.Last() )
			m_MenuCurrent = m_MenuItemList.Begin();
		else
			++m_MenuCurrent;
		DrawMenuItemList();
		PostPaintEvent();
		break;
	CASE_KEY_UP:
		if ( m_MenuCurrent == m_MenuItemList.Begin() )
			m_MenuCurrent = m_MenuItemList.Last();
		else
			--m_MenuCurrent;
		DrawMenuItemList();
		PostPaintEvent();
		break;
    CASE_KEY_RETURN:
	CASE_KEY_SPACE:
		{
			CConsoleMenuItem* pMenuItem = *m_MenuCurrent;

			if ( NotPtrCheck(pMenuItem) )
				DoMenuItem(pMenuItem);
		}
		break;
	default:
		break;
	}
	return true;
}

bool CConsolePopupMenu::KeyPress(TCHAR key, DWORD controlKeyState)
{
	return true;
}

bool CConsolePopupMenu::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsolePopupMenu::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	if ( m_MenuItemList.Count() > 0 )
	{
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;

		while ( it )
		{
			pMenuItem = *it;
			if ( pMenuItem->IsPosInClientRect(mousePos) )
			{
				DoMenuItem(pMenuItem);
				break;
			}
			++it;
		}
	}
	return true;
}

bool CConsolePopupMenu::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsolePopupMenu::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsolePopupMenu::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsolePopupMenu::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsolePopupMenu::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

