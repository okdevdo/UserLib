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
#include "ConsoleWindowMenu.h"
#include "Console.h"
#include "ConsoleMenuItem.h"

CConsoleWindowMenu::CConsoleWindowMenu(CConstPointer name, CConsole* pConsole):
    CConsolePopupMenu(name, pConsole)
{
}

CConsoleWindowMenu::CConsoleWindowMenu(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsolePopupMenu(name, title, pConsole)
{
}

CConsoleWindowMenu::CConsoleWindowMenu(CConsoleMainMenu* mainMenu, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsolePopupMenu(mainMenu, name, title, pConsole)
{
}

CConsoleWindowMenu::~CConsoleWindowMenu(void)
{
	ClearMenus();
}

void CConsoleWindowMenu::WindowMenuCallback(CConsoleMenuItem* pMenu)
{
	if ( !m_Console )
		return;

	CConsole::CConsoleWindowList::Iterator it;
	CConsoleWindow* pWindow;

	it = m_Console->GetConsoleWindowList().Begin();
	while ( it )
	{
		pWindow = *it;
		if ( pMenu->GetTitle().Compare(pWindow->GetTitle()) == 0 )
		{
			m_Console->BringToFront(it);
			break;
		}
		++it;
	}
}

void CConsoleWindowMenu::Initialize()
{
	if ( !m_Console )
		return;

	CConsole::CConsoleWindowList::Iterator it;
	CConsoleWindow* pWindow;
	CStringBuffer name;
	sword cnt;

	it = m_Console->GetConsoleWindowList().Begin();
	cnt = 0;
	while ( it )
	{
		pWindow = *it;
		name.FormatString(__FILE__LINE__ _T("WindowMenuItem%d"), cnt);
		AddMenuItem(OK_NEW_OPERATOR CConsoleMenuItem(
			OK_NEW_OPERATOR CConsoleMenuItemCallback<CConsoleWindowMenu>(this, &CConsoleWindowMenu::WindowMenuCallback),
			name,
			pWindow->GetTitle(),
			m_Console));
		++it;
		++cnt;
	}
}

void CConsoleWindowMenu::Create(COORD pos, COORD size)
{
	Initialize();
	CConsolePopupMenu::Create(pos, size);
}

void CConsoleWindowMenu::Move()
{
	SMALL_RECT rect = GetNonClientArea();
	COORD pos;

	pos.X = rect.Left;
	pos.Y = rect.Top;

	ClearMenus();
	Initialize();
	CConsolePopupMenu::Create(pos, m_ScreenBufferSize);
}
