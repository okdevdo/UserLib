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
#include "ConsoleScrollbarControl.h"

CConsoleScrollbarControl::CConsoleScrollbarControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole),
	m_Minimum(0),
	m_Maximum(100),
	m_Current(0),
	m_LineStep(3),
	m_PageStep(12)
{
}

CConsoleScrollbarControl::CConsoleScrollbarControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole),
	m_Minimum(0),
	m_Maximum(100),
	m_Current(0),
	m_LineStep(3),
	m_PageStep(12)
{
}

CConsoleScrollbarControl::CConsoleScrollbarControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole),
	m_Minimum(0),
	m_Maximum(100),
	m_Current(0),
	m_LineStep(3),
	m_PageStep(12)
{
}

CConsoleScrollbarControl::~CConsoleScrollbarControl(void)
{
}

void CConsoleScrollbarControl::DrawScrollbarControl()
{

}

void CConsoleScrollbarControl::Create(COORD pos, COORD size)
{
	CConsoleControl::Create(pos, size);
}
