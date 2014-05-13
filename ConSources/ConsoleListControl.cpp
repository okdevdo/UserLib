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
#include "ConsoleListControl.h"
#include "Console.h"

CConsoleListControl::CConsoleListControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole),
	m_List(__FILE__LINE__ 32, 16),
	m_HighlightPos(0),
	m_ScrollPos(0)
{
}

CConsoleListControl::CConsoleListControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole),
	m_List(__FILE__LINE__ 32, 16),
	m_HighlightPos(0),
	m_ScrollPos(0)
{
}

CConsoleListControl::CConsoleListControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole),
	m_List(__FILE__LINE__ 32, 16),
	m_HighlightPos(0),
	m_ScrollPos(0)
{
}

CConsoleListControl::CConsoleListControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(taborder, callback, name, title, pConsole),
	m_List(__FILE__LINE__ 32, 16),
	m_HighlightPos(0),
	m_ScrollPos(0)
{
}

CConsoleListControl::~CConsoleListControl(void)
{
}

void CConsoleListControl::ClearListe()
{
	m_List.Close();
	m_List.Open(__FILE__LINE__ 32, 16);
	m_HighlightPos = 0;
	m_ScrollPos = 0;
}

dword CConsoleListControl::GetListItemCount()
{
	return m_List.Count();
}

void CConsoleListControl::AddListItem(ConstRef(CStringBuffer) item)
{
	m_List.Append(item);
}

CStringBuffer CConsoleListControl::GetCurrentListItem()
{
	CStringListe::Iterator it = m_List.Index(m_HighlightPos);
	CStringBuffer tmp;

	if ( it )
		tmp = *it;
	return tmp;
}

void CConsoleListControl::SetCurrentListItem(ConstRef(CStringBuffer) item)
{
	CStringListe::Iterator it = m_List.Begin();

	m_HighlightPos = 0;
	while ( it )
	{
		if ( s_strcmp(item.GetString(), (*it).GetString()) == 0 )
			break;
		++m_HighlightPos;
		++it;
	}
}

void CConsoleListControl::DrawListControl(bool bhighlight)
{
	SMALL_RECT rect;
	CStringListe::Iterator it = m_List.Begin();
	CStringBuffer value;
	TListCnt cnt = 0;
	sword offb = m_hasBorder?1:0;
	sword maxt = m_ClientAreaSize.Y + offb;

	rect.Left = offb;
	rect.Top = offb;
	rect.Right = m_ClientAreaSize.X + offb;
	rect.Bottom = rect.Top + 1;
	while ( it )
	{
		if ( cnt >= m_ScrollPos )
		{
			value = *it;
		    if ( value.IsEmpty() )
				value.SetString(__FILE__LINE__ _T(" "));
			if ( bhighlight && (m_HighlightPos == cnt) )
				DrawString(rect, value, m_HighLightColor);
			else
				DrawString(rect, value, m_Color);
			++(rect.Top);
			++(rect.Bottom);
			if ( rect.Top == maxt )
				break;
		}
		++cnt;
		++it;
	}
	if ( rect.Top < maxt )
	{
		value.SetString(__FILE__LINE__ _T(" "));
		rect.Bottom = maxt;
		DrawString(rect, value, m_Color);
	}
}

TListCnt CConsoleListControl::IndexFromPos(COORD pos)
{
	SMALL_RECT rect = m_ClientArea;
	CStringListe::Iterator it = m_List.Begin();
	TListCnt cnt = 0;

	rect.Bottom = rect.Top + 1;
	while ( it )
	{
		if ( cnt >= m_ScrollPos )
		{
			if ( (pos.X >= rect.Left) 
				&& (pos.X < rect.Right)
				&& (pos.Y >= rect.Top) 
				&& (pos.Y < rect.Bottom) )
				return cnt;
			++(rect.Top);
			++(rect.Bottom);
			if ( rect.Top >= m_ClientArea.Bottom )
				break;
		}
		++cnt;
		++it;
	}
	return 0;
}

void CConsoleListControl::Create(COORD pos, COORD size)
{
	CConsoleControl::Create(pos, size);

	m_HighlightPos = 0;
	m_ScrollPos = 0;
	DrawListControl(false);
}

void CConsoleListControl::SetFocus(bool hasFocus)
{
	DrawListControl(hasFocus);
	PostPaintEvent();
}

void CConsoleListControl::Scroll()
{
	m_ScrollPos = m_ScrollBarVInfo.Current;
	DrawListControl();
}

void CConsoleListControl::Update()
{
	if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
	{
		m_ScrollBarVInfo.Current = m_ScrollPos;
		DrawVerticalScrollBar();
	}
	DrawListControl();
	PostPaintEvent();
	InvokeControlCommand(ListItemSelected);
}

bool CConsoleListControl::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;
	switch ( virtualKeyCode )
	{
    CASE_KEY_PAGEDOWN:
		if ( (m_List.Count() > Cast(TListCnt, m_ClientAreaSize.Y)) && (m_HighlightPos < (m_List.Count() - Cast(TListCnt, m_ClientAreaSize.Y) - 1)) )
		{
			m_HighlightPos += Cast(TListCnt, m_ClientAreaSize.Y - 1);
			if ( m_HighlightPos < m_ScrollPos )
				m_ScrollPos = m_HighlightPos;
			else if ( (m_HighlightPos - m_ScrollPos) >= Cast(TListCnt, m_ClientAreaSize.Y) )
				m_ScrollPos = m_HighlightPos - (m_ClientAreaSize.Y / 2);
		}
		else
		{
			m_HighlightPos = 0;
			m_ScrollPos = 0;
		}
		Update();
		break;
	CASE_KEY_DOWN:
		if ( m_HighlightPos < (m_List.Count() - 1) )
		{
			++m_HighlightPos;
			if ( m_HighlightPos < m_ScrollPos )
				m_ScrollPos = m_HighlightPos;
			else if ( (m_HighlightPos - m_ScrollPos) >= Cast(TListCnt, m_ClientAreaSize.Y) )
				m_ScrollPos = m_HighlightPos - (m_ClientAreaSize.Y / 2);
		}
		else
		{
			m_HighlightPos = 0;
			m_ScrollPos = 0;
		}
		Update();
		break;
    CASE_KEY_PAGEUP:
		if ( m_List.Count() <= Cast(TListCnt, m_ClientAreaSize.Y) )
		{
			m_HighlightPos = 0;
			m_ScrollPos = 0;
		}
		else if ( m_HighlightPos >= Cast(TListCnt, m_ClientAreaSize.Y) )
		{
			m_HighlightPos -= Cast(TListCnt, m_ClientAreaSize.Y - 1);
			if ( m_HighlightPos < m_ScrollPos )
				m_ScrollPos = m_HighlightPos;
			else if ( (m_HighlightPos - m_ScrollPos) >= Cast(TListCnt, m_ClientAreaSize.Y) )
				m_ScrollPos = m_HighlightPos - (m_ClientAreaSize.Y / 2);
		}
		else
		{
			m_HighlightPos = m_List.Count() - 1;
			if ( m_HighlightPos < m_ScrollPos )
				m_ScrollPos = m_HighlightPos;
			else if ( (m_HighlightPos - m_ScrollPos) >= Cast(TListCnt, m_ClientAreaSize.Y) )
				m_ScrollPos = m_HighlightPos - (m_ClientAreaSize.Y / 2);
		}
		Update();
		break;
	CASE_KEY_UP:
		if ( m_HighlightPos > 0 )
		{
			--m_HighlightPos;
			if ( m_HighlightPos < m_ScrollPos )
				m_ScrollPos = m_HighlightPos;
			else if ( (m_HighlightPos - m_ScrollPos) >= Cast(TListCnt, m_ClientAreaSize.Y) )
				m_ScrollPos = m_HighlightPos - (m_ClientAreaSize.Y / 2);
		}
		else
		{
			m_HighlightPos = m_List.Count() - 1;
			if ( m_HighlightPos < m_ScrollPos )
				m_ScrollPos = m_HighlightPos;
			else if ( (m_HighlightPos - m_ScrollPos) >= Cast(TListCnt, m_ClientAreaSize.Y) )
				m_ScrollPos = m_HighlightPos - (m_ClientAreaSize.Y / 2);
		}
		Update();
		break;
	default:
		break;
	}
	return true;
}

bool CConsoleListControl::KeyPress(TCHAR key, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;
	switch ( key )
	{
	case 0x20:
		InvokeControlCommand(ListItemClicked);
		break;
	default:
		break;
	}
	return true;
}

bool CConsoleListControl::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleListControl::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;

	m_HighlightPos = IndexFromPos(mousePos);
	DrawListControl();
	PostPaintEvent();
	InvokeControlCommand(ListItemSelected);
	return true;
}

bool CConsoleListControl::LeftMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleListControl::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleListControl::RightMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleListControl::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	InvokeControlCommand(ListItemClicked);
	return true;
}

bool CConsoleListControl::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleListControl::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleListControl::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleListControl::WheelMouse(COORD mousePos, sword rotateCnt, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;

	sdword delta = Cast(sdword, m_HighlightPos) - (rotateCnt / 10);

	if ( delta < 0 )
		m_HighlightPos = 0;
	else if ( delta >= Cast(sdword, m_List.Count()) )
		m_HighlightPos = m_List.Count() - 1;
	else
		m_HighlightPos = delta;
	if ( m_HighlightPos < m_ScrollPos )
		m_ScrollPos = m_HighlightPos;
	else if ( Cast(sword, m_HighlightPos - m_ScrollPos) > (m_ScreenBufferSize.Y - 2) )
		m_ScrollPos = m_HighlightPos - ((m_ScreenBufferSize.Y - 2) / 2);
	Update();
	return true;
}