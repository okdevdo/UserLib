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
#include "ConsoleControl.h"

CAbstractConsoleControlCallback::CAbstractConsoleControlCallback()
{
}

CAbstractConsoleControlCallback::CAbstractConsoleControlCallback(const CAbstractConsoleControlCallback&)
{
}

CAbstractConsoleControlCallback::~CAbstractConsoleControlCallback()
{
}

CConsoleControl::CConsoleControl(CConstPointer name, CConsole* pConsole):
    CConsoleWindow(name, pConsole),
    m_Callback(nullptr),
	m_TabStop(false),
	m_TabOrder(0)
{
}

CConsoleControl::CConsoleControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole),
    m_Callback(nullptr),
	m_TabStop(false),
	m_TabOrder(0)
{
}

CConsoleControl::CConsoleControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole),
    m_Callback(callback),
	m_TabStop(false),
	m_TabOrder(0)
{
}

CConsoleControl::CConsoleControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole),
    m_Callback(callback),
	m_TabStop(true),
	m_TabOrder(taborder)
{
}

CConsoleControl::~CConsoleControl(void)
{
	if ( m_Callback )
		delete m_Callback;
}

void CConsoleControl::InvokeControlCommand(DWORD command)
{
	if ( !m_Callback )
		return;
	m_Callback->invoke(this, command);
}

void CConsoleControl::Create(COORD pos, COORD size)
{
	CConsoleWindow::Create(pos, size);
}
