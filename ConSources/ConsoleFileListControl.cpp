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
#include "ConsoleFileListControl.h"
#include "Console.h"

CConsoleFileListControl::CConsoleFileListControl(CConstPointer name, CConsole* pConsole):
    CConsoleListControl(name, pConsole),
	m_Path()
{
}

CConsoleFileListControl::CConsoleFileListControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleListControl(name, title, pConsole),
	m_Path()
{
}

CConsoleFileListControl::CConsoleFileListControl(CConstPointer path, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleListControl(callback, name, title, pConsole),
	m_Path(__FILE__LINE__ path)
{
}

CConsoleFileListControl::CConsoleFileListControl(word taborder, CConstPointer path, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleListControl(taborder, callback, name, title, pConsole),
	m_Path(__FILE__LINE__ path)
{
}

CConsoleFileListControl::~CConsoleFileListControl(void)
{
}

void CConsoleFileListControl::CreateListe()
{
	try
	{
		m_DirIter.Open(m_Path);

		while ( m_DirIter )
		{
			if ( (!(m_DirIter.is_SubDir())) && (!(m_DirIter.is_Hidden())) && (!(m_DirIter.is_System())) )
				AddListItem(m_DirIter.get_Name());
			++m_DirIter;
		}
		m_DirIter.Close();
		if ( m_Created )
		{
			m_ScrollBarVInfo.SetScrollBarInfo(GetListItemCount(), m_ScreenBufferSize.Y - 2);
			DrawVerticalScrollBar();
			m_ScrollPos = 0;
			m_HighlightPos = 0;
		}
	}
	catch ( CDirectoryIteratorException* )
	{
	}
}

void CConsoleFileListControl::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	m_hasBorder = true;
	m_BorderStyle = singleborderstyle;
	m_hasTitle = true;
	m_TitleStyle = controltitlebarstyle;
	m_hasScrollbar = true;
	m_ScrollBarStyle = verticalscrollbarstyle;
	CreateListe();
	m_ScrollBarVInfo.SetScrollBarInfo(GetListItemCount(), size.Y - 2);

	CConsoleListControl::Create(pos, size);
}
