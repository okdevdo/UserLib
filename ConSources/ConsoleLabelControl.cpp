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
#include "ConsoleLabelControl.h"
#include "Console.h"

CConsoleLabelControl::CConsoleLabelControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole)
{
}

CConsoleLabelControl::CConsoleLabelControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole)
{
}

CConsoleLabelControl::CConsoleLabelControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole)
{
}

CConsoleLabelControl::~CConsoleLabelControl(void)
{
}

void CConsoleLabelControl::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	CConsoleControl::Create(pos, size);

	DrawLabel();
}

void CConsoleLabelControl::DrawLabel()
{
	SMALL_RECT rect;

	rect.Left = 0;
	rect.Right = m_ScreenBufferSize.X;
	rect.Top = 0;
	rect.Bottom = m_ScreenBufferSize.Y;
	if ( m_Title.IsEmpty() )
		m_Title.SetString(__FILE__LINE__ _T(" "));
	DrawString(rect, m_Title.GetString(), m_Color);
}
