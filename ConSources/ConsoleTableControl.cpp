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
#include "ConsoleTableControl.h"

CConsoleTableControl::CConsoleTableControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole)
{
}

CConsoleTableControl::CConsoleTableControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole)
{
}

CConsoleTableControl::CConsoleTableControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole)
{
}

CConsoleTableControl::CConsoleTableControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(taborder, callback, name, title, pConsole)
{
}

CConsoleTableControl::~CConsoleTableControl(void)
{
}

void CConsoleTableControl::DrawTableControl(void)
{
}

void CConsoleTableControl::Create(COORD pos, COORD size)
{
}

void CConsoleTableControl::SetFocus(bool hasFocus)
{
}

void CConsoleTableControl::Resize()
{
}

void CConsoleTableControl::Paint(COORD bufferSize, PCHAR_INFO buffer)
{
}

void CConsoleTableControl::Move()
{
}

void CConsoleTableControl::Undo()
{
}

void CConsoleTableControl::Redo()
{
}

void CConsoleTableControl::Cut()
{
}

void CConsoleTableControl::Copy()
{
}

void CConsoleTableControl::Paste()
{
}

void CConsoleTableControl::Delete()
{
}

bool CConsoleTableControl::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableControl::KeyPress(TCHAR key, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableControl::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableControl::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableControl::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableControl::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableControl::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableControl::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableControl::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

