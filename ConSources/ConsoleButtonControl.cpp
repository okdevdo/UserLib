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
#include "ConsoleButtonControl.h"
#include "Console.h"

CConsoleButtonControl::CConsoleButtonControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole)
{
}

CConsoleButtonControl::CConsoleButtonControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole)
{
}

CConsoleButtonControl::CConsoleButtonControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole)
{
}

CConsoleButtonControl::CConsoleButtonControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(taborder, callback, name, title, pConsole)
{
}

CConsoleButtonControl::~CConsoleButtonControl(void)
{
}

void CConsoleButtonControl::DrawButton(bool bHighlighted)
{
	SMALL_RECT r;

	r.Left = 1;
	r.Right = m_ScreenBufferSize.X - 1;
	r.Top = 1;
	r.Bottom = r.Top + 1;

	CStringBuffer t(m_Title);
	WULong len = m_ScreenBufferSize.X - 2;

	while ( t.GetLength() < len )
	{
		t.PrependString(_T(" "));
		if ( t.GetLength() < len )
			t.AppendString(_T(" "));
	}
	DrawString(r, t.GetString(), bHighlighted?m_HighLightColor:m_Color);
}

void CConsoleButtonControl::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	m_hasBorder = true;
	m_BorderStyle = singleborderstyle;
	CConsoleControl::Create(pos, size);
	DrawButton();
}

void CConsoleButtonControl::SetFocus(bool hasFocus)
{
	DrawButton(hasFocus);
	PostPaintEvent();
}

bool CConsoleButtonControl::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleButtonControl::KeyPress(TCHAR key, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;
	switch ( key )
	{
	case 0x20:
		InvokeControlCommand(ButtonClicked);
		break;
	default:
		break;
	}
	return true;
}

bool CConsoleButtonControl::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleButtonControl::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;
	InvokeControlCommand(ButtonClicked);
	return true;
}

bool CConsoleButtonControl::LeftMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleButtonControl::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleButtonControl::RightMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleButtonControl::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleButtonControl::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleButtonControl::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleButtonControl::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

