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
#include "ConsoleTabControl.h"
#include "ConsoleTabPageControl.h"

static void __stdcall CConsoleTabPageControlDeleteFunc( ConstPointer data, Pointer context )
{
	CConsoleTabPageControl* p = CastAnyPtr(CConsoleTabPageControl, CastMutable(Pointer, data));

	p->release();
}

CConsoleTabControl::CConsoleTabControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole),
	m_Pages(__FILE__LINE__0),
	m_CurrentPage(NULL),
	m_CurrentPageNo(-1)
{
}

CConsoleTabControl::CConsoleTabControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole),
	m_Pages(__FILE__LINE__0),
	m_CurrentPage(NULL),
	m_CurrentPageNo(-1)
{
}

CConsoleTabControl::CConsoleTabControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole),
	m_Pages(__FILE__LINE__0),
	m_CurrentPage(NULL),
	m_CurrentPageNo(-1)
{
}

CConsoleTabControl::CConsoleTabControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(taborder, callback, name, title, pConsole),
	m_Pages(__FILE__LINE__0),
	m_CurrentPage(NULL),
	m_CurrentPageNo(-1)
{
}

CConsoleTabControl::~CConsoleTabControl(void)
{
	m_Pages.Close(CConsoleTabPageControlDeleteFunc, NULL);
}

void CConsoleTabControl::Initialize(void)
{
	m_Pages.Close(CConsoleTabPageControlDeleteFunc, NULL);
	m_Pages.Open(__FILE__LINE__0);
	m_CurrentPage = NULL;
	m_CurrentPageNo = -1;
}

sword CConsoleTabControl::IndexFromPos(COORD pos)
{
	SMALL_RECT rect = m_ClientArea;
	TPageControls::Iterator it = m_Pages.Begin();
	sword lRight = m_ClientAreaSize.X / Cast(sword, m_Pages.Count());
	sword cnt = 0;

	rect.Bottom = rect.Top + 1;
	rect.Right = rect.Left + lRight;
	while ( it )
	{
		if ( (pos.X >= rect.Left) 
			&& (pos.X < rect.Right)
			&& (pos.Y >= rect.Top) 
			&& (pos.Y < rect.Bottom) )
			return cnt;
		rect.Left = rect.Right;
		rect.Right += lRight;
		++cnt;
		++it;
	}
	return -1;
}

void CConsoleTabControl::DrawTabControl(void)
{
	SMALL_RECT rect;
	TPageControls::Iterator it = m_Pages.Begin();
	sword offb = m_hasBorder?1:0;
	sword lRight = m_ClientAreaSize.X / Cast(sword, m_Pages.Count());

	rect.Left = offb;
	rect.Top = offb;
	rect.Bottom = rect.Top + 1;
	rect.Right = lRight + offb;
	while ( it )
	{
		DrawString(rect, (*it)->GetTitle(), m_Color);
		rect.Left = rect.Right - offb;
		rect.Right += lRight;
		++it;
	}
	if ( m_CurrentPage ) 
	{
		COORD pos;

		pos.X = m_ClientArea.Left;
		pos.Y = m_ClientArea.Top + 1;
		if ( !(m_CurrentPage->IsCreated()) )
		{
			COORD sz;

			sz.X = m_ClientAreaSize.X;
			sz.Y = m_ClientAreaSize.Y - 1;
			m_CurrentPage->Create(pos, sz);
		}
		else
			m_CurrentPage->MoveWindow(pos, false, true);
	}
}

void CConsoleTabControl::Create(COORD pos, COORD size)
{
	CConsoleControl::Create(pos, size);

	if ( m_Pages.Count() > 0 )
	{
		m_CurrentPageNo = 0;
		m_CurrentPage = *(m_Pages.Index(m_CurrentPageNo));
	}
	else
	{
		m_CurrentPageNo = -1;
		m_CurrentPage = NULL;
	}
	DrawTabControl();
}

void CConsoleTabControl::SetFocus(bool hasFocus)
{
}

void CConsoleTabControl::Resize()
{
}

void CConsoleTabControl::Paint(COORD bufferSize, PCHAR_INFO buffer)
{
	CConsoleControl::Paint(bufferSize, buffer);
	if ( m_CurrentPage )
		m_CurrentPage->Paint(bufferSize, buffer);
}

void CConsoleTabControl::Move()
{
	DrawTabControl();
}

void CConsoleTabControl::Undo()
{
	if ( m_CurrentPage )
		m_CurrentPage->Undo();
}

void CConsoleTabControl::Redo()
{
	if ( m_CurrentPage )
		m_CurrentPage->Redo();
}

void CConsoleTabControl::Cut()
{
	if ( m_CurrentPage )
		m_CurrentPage->Cut();
}

void CConsoleTabControl::Copy()
{
	if ( m_CurrentPage )
		m_CurrentPage->Copy();
}

void CConsoleTabControl::Paste()
{
	if ( m_CurrentPage )
		m_CurrentPage->Paste();
}

void CConsoleTabControl::Delete()
{
	if ( m_CurrentPage )
		m_CurrentPage->Delete();
}

bool CConsoleTabControl::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	switch ( virtualKeyCode )
	{
    CASE_KEY_F12:
		if ( (controlKeyState & KeyStateAllMask) != 0 )
			return true;
		if ( m_CurrentPageNo < Cast(sword, m_Pages.Count() - 1) )
			++m_CurrentPageNo;
		else
			m_CurrentPageNo = 0;
		m_CurrentPage = *(m_Pages.Index(m_CurrentPageNo));
		DrawTabControl();
		PostPaintEvent();
		break;
	default:
		if ( m_CurrentPage )
			return m_CurrentPage->KeyDown(virtualKeyCode, controlKeyState);
		break;
	}
	return true;
}

bool CConsoleTabControl::KeyPress(TCHAR key, DWORD controlKeyState)
{
	if ( m_CurrentPage )
		return m_CurrentPage->KeyPress(key, controlKeyState);
	return true;
}

bool CConsoleTabControl::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	if ( m_CurrentPage )
		return m_CurrentPage->KeyUp(virtualKeyCode, controlKeyState);
	return true;
}

bool CConsoleTabControl::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
	{
		if ( m_CurrentPage )
			return m_CurrentPage->LeftMouseDown(mousePos, controlKeyState);
		return true;
	}

	sword ix = IndexFromPos(mousePos);

	if ( ix < 0 )
	{
		if ( m_CurrentPage )
			return m_CurrentPage->LeftMouseDown(mousePos, controlKeyState);
		return true;
	}
	m_CurrentPageNo = ix;
	m_CurrentPage = *(m_Pages.Index(m_CurrentPageNo));
	DrawTabControl();
	PostPaintEvent();
	return true;
}

bool CConsoleTabControl::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTabControl::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTabControl::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTabControl::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTabControl::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

