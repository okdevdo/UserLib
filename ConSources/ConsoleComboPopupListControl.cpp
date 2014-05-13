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
#include "ConsoleComboPopupListControl.h"
#include "Console.h"

CConsoleComboPopupListControl::CConsoleComboPopupListControl(CConstPointer name, CConsole* pConsole):
    CConsoleListControl(name, pConsole)
{
}

CConsoleComboPopupListControl::CConsoleComboPopupListControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleListControl(name, title, pConsole)
{
}

CConsoleComboPopupListControl::CConsoleComboPopupListControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleListControl(callback, name, title, pConsole)
{
}

CConsoleComboPopupListControl::CConsoleComboPopupListControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleListControl(taborder, callback, name, title, pConsole)
{
}

CConsoleComboPopupListControl::~CConsoleComboPopupListControl(void)
{
}

void CConsoleComboPopupListControl::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	m_hasBorder = true;
	m_BorderStyle = singleborderstyle;

	CConsoleListControl::Create(pos, size);
}

bool CConsoleComboPopupListControl::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;
	switch ( virtualKeyCode )
	{
    CASE_KEY_PAGEDOWN:
	CASE_KEY_DOWN:
    CASE_KEY_PAGEUP:
    CASE_KEY_UP:
		CConsoleListControl::KeyDown(virtualKeyCode, controlKeyState);
		break;
	CASE_KEY_ESCAPE:
		if ( m_Console )
			m_Console->CloseConsolePopupControl();
		break;
    CASE_KEY_RETURN:
		InvokeControlCommand(ListItemClicked);
		break;
	default:
		break;
	}
	return true;
}
