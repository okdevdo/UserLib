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
#include "ConsoleMainMenu.h"
#include "ConsolePopupMenu.h"
#include "ConsoleMenuItem.h"
#include "Console.h"

CConsoleMainMenu::CConsoleMainMenu(CConstPointer name, CConsole* pConsole):
    CConsoleWindow(name, pConsole),
	m_MenuItemList(__FILE__LINE__0)
{
}

CConsoleMainMenu::CConsoleMainMenu(CConstPointer name, CConstPointer title, CConsole* pConsole ):
    CConsoleWindow(name, title, pConsole),
	m_MenuItemList(__FILE__LINE__0)
{
}

CConsoleMainMenu::~CConsoleMainMenu(void)
{
}

void CConsoleMainMenu::MenuNavigation(CConsolePopupMenu* popupMenu, CAbstractConsoleMenuNavigationCallback::TNavigation command)
{
	switch ( command )
	{
	case CAbstractConsoleMenuNavigationCallback::NavigationRight:
		{
			CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
			CConsoleMenuItem* pMenuItem;
			bool bFound = false;
			bool bDone = false;

			while ( it )
			{
				pMenuItem = *it;
				if ( (!bFound) && pMenuItem->HasSubMenu() && (pMenuItem->GetSubMenu() == popupMenu) )
					bFound = true;
				else if ( bFound )
				{
					OpenPopupMenu(pMenuItem);
					bDone = true;
					break;
				}
				++it;
			}
			if ( !bDone )
			{
				if ( !bFound )
					break;
				OpenFirstPopupMenu();
			}
		}
		break;
	case CAbstractConsoleMenuNavigationCallback::NavigationLeft:
		{
			CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
			CConsoleMenuItem* pMenuItem;
			CConsoleMenuItem* pBeforeMenu = nullptr;
			bool bFound = false;

			while ( it )
			{
				pMenuItem = *it;
				if ( (!bFound) && pMenuItem->HasSubMenu() && (pMenuItem->GetSubMenu() == popupMenu) )
				{
					bFound = true;
					break;
				}
				pBeforeMenu = pMenuItem;
				++it;
			}
			if ( bFound )
			{
				if ( PtrCheck(pBeforeMenu) )
				{
					it = m_MenuItemList.Last();
					pBeforeMenu = *it;
				}
				OpenPopupMenu(pBeforeMenu);
			}
		}
		break;
	default:
		break;
	}
}

void CConsoleMainMenu::AddMenuItem(CConsoleMenuItem* pMenuItem)
{
	m_MenuItemList.Append(pMenuItem);
}

bool CConsoleMainMenu::TestKeyCode(WORD vkey, DWORD kmod)
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
				if ( pMenuItem->HasSubMenu() )
					OpenPopupMenu(pMenuItem);
				else
					pMenuItem->InvokeMenuItemCommand();
				return true;
			}
			else if ( pMenuItem->HasSubMenu() )
			{
				CConsolePopupMenu* popupMenu = pMenuItem->GetSubMenu();

				if ( popupMenu->TestKeyCode(vkey, kmod) )
					return true;
			}
			++it;
		}
	}
	return false;
}

void CConsoleMainMenu::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();

	CConsoleWindow::Create(pos, size);

	if ( m_MenuItemList.Count() > 0 )
	{
		size.X /= Cast(sword, m_MenuItemList.Count());
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;

		while ( it )
		{
			pMenuItem = *it;
			pMenuItem->Create(pos, size);
			if ( pMenuItem->HasSubMenu() )
			{
				CConsolePopupMenu* pPopupMenu = pMenuItem->GetSubMenu();
				COORD pos1;

				pos1.X = pMenuItem->GetNonClientArea().Left;
				pos1.Y = pMenuItem->GetNonClientArea().Bottom;
				pPopupMenu->Create(pos1, m_Console->GetScreenBufferSize());
			}
			pos.X += size.X;
			++it;
		}
	}
}

void CConsoleMainMenu::Paint(COORD bufferSize, PCHAR_INFO buffer)
{
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

void CConsoleMainMenu::Resize()
{
	if ( m_MenuItemList.Count() > 0 )
	{
		sword sizeX = m_ScreenBufferSize.X / Cast(sword, m_MenuItemList.Count());
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;
		COORD pos;
		COORD size;

		pos.X = 0;
		pos.Y = 0;
		size.X = sizeX;
		size.Y = 1;
		while ( it )
		{
			pMenuItem = *it;
			pMenuItem->MoveWindow(pos, false, true);
			pMenuItem->ResizeWindow(size);
			pos.X += size.X;
			++it;
		}
	}
}

void CConsoleMainMenu::OpenFirstPopupMenu()
{
	if ( m_MenuItemList.Count() > 0 )
	{
		CConsoleMenuItemList::Iterator it(m_MenuItemList.Begin());
		CConsoleMenuItem* pMenuItem;

		if ( it )
		{
			pMenuItem = *it;
			OpenPopupMenu(pMenuItem);
		}
	}
}

void CConsoleMainMenu::OpenPopupMenu(CConsoleMenuItem* pMenuItem)
{
	if ( pMenuItem->HasSubMenu() )
	{
		if ( m_Console )
		{
			SMALL_RECT rect = pMenuItem->GetNonClientArea();
			COORD pos;

			pos.X = rect.Left;
			pos.Y = rect.Top + 1;
			while ( NotPtrCheck(m_Console->GetCurrentConsolePopupMenu()) )
				m_Console->CloseConsolePopupMenu();
			m_Console->CreateConsolePopupMenu(pMenuItem->GetSubMenu(), pos);
		}
	}
}

bool CConsoleMainMenu::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleMainMenu::KeyPress(TCHAR key, DWORD controlKeyState)
{
	return true;
}

bool CConsoleMainMenu::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleMainMenu::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
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
				if ( pMenuItem->HasSubMenu() )
					OpenPopupMenu(pMenuItem);
				else
					pMenuItem->InvokeMenuItemCommand();
				break;
			}
			++it;
		}
	}
	return true;
}

bool CConsoleMainMenu::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleMainMenu::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleMainMenu::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleMainMenu::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleMainMenu::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

