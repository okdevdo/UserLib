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
#include "ConsoleMenuItem.h"
#include "ConsolePopupMenu.h"
#include "Console.h"

CAbstractConsoleMenuItemCallback::CAbstractConsoleMenuItemCallback()
{
}

CAbstractConsoleMenuItemCallback::CAbstractConsoleMenuItemCallback(const CAbstractConsoleMenuItemCallback&)
{
}

CAbstractConsoleMenuItemCallback::~CAbstractConsoleMenuItemCallback()
{
}

CConsoleMenuItem::CConsoleMenuItem(CConstPointer name, CConsole* pConsole):
    CConsoleWindow(name, pConsole),
	m_Callback(NULL),
	m_VirtualKeyCode(0),
	m_KeyModifiers(0),
	m_SubMenu(NULL)
{
}

CConsoleMenuItem::CConsoleMenuItem(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole),
	m_Callback(NULL),
	m_VirtualKeyCode(0),
	m_KeyModifiers(0),
	m_SubMenu(NULL)
{
}

CConsoleMenuItem::CConsoleMenuItem(CConsolePopupMenu* subMenu, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole),
	m_Callback(NULL),
	m_VirtualKeyCode(0),
	m_KeyModifiers(0),
	m_SubMenu(subMenu)
{
}

CConsoleMenuItem::CConsoleMenuItem(CAbstractConsoleMenuItemCallback* pCallback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole),
	m_Callback(pCallback),
	m_VirtualKeyCode(0),
	m_KeyModifiers(0),
	m_SubMenu(NULL)
{
}

CConsoleMenuItem::CConsoleMenuItem(WORD vkey, DWORD kmod, CAbstractConsoleMenuItemCallback* pCallback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole),
	m_Callback(pCallback),
	m_VirtualKeyCode(vkey),
	m_KeyModifiers(kmod),
	m_SubMenu(NULL)
{
}

CConsoleMenuItem::~CConsoleMenuItem(void)
{
	if ( m_Callback )
		m_Callback->release();
	if ( m_SubMenu )
		m_SubMenu->release();
}

bool CConsoleMenuItem::TestKeyCode(WORD vkey, DWORD kmod) 
{
	if ( m_VirtualKeyCode != vkey )
		return false;
#ifdef OK_SYS_WINDOWS
	kmod &= KeyStateAllMask;

	if ( (kmod & (~m_KeyModifiers)) != 0 )
		return false;
	if ( (kmod & m_KeyModifiers) == 0 )
		return false;
#endif
	return true;
}

void CConsoleMenuItem::InvokeMenuItemCommand()
{
	if ( m_Callback )
		m_Callback->invoke(this);
}

dword CConsoleMenuItem::GetTitleSize() const
{
	CStringBuffer tmp;

	tmp = CreateMenuTitle();
	return tmp.GetLength();
}

CStringBuffer CConsoleMenuItem::CreateMenuTitle() const
{
	CStringBuffer tmp;

	tmp = m_Title;
	if ( m_KeyModifiers & KeyStateCtrlMask )
	{
		TCHAR buf [2];

		tmp += _T(" CTRL+");
		buf[0] = m_VirtualKeyCode;
		buf[1] = 0;
		tmp += buf;
	}
	if ( tmp.IsEmpty() )
		tmp.SetString(__FILE__LINE__ _T(" "));
	return tmp;
}

void CConsoleMenuItem::DrawMenuItem(bool bHighlight)
{
	SMALL_RECT rect;
	CStringBuffer tmp;

	rect.Left = 0;
	rect.Top = 0;
	rect.Bottom = 1;
	rect.Right = m_ScreenBufferSize.X;
	tmp = CreateMenuTitle();
	DrawString(rect, tmp, bHighlight?m_HighLightColor:m_Color);
}

void CConsoleMenuItem::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	CConsoleWindow::Create(pos, size);
	DrawMenuItem();
}

void CConsoleMenuItem::Resize()
{
	DrawMenuItem();
}
