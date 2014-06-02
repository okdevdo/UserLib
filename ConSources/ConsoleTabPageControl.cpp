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
#include "ConsoleTabPageControl.h"
#include "Console.h"
#include "ConsoleLayout.h"

CConsoleTabPageControl::CConsoleTabPageControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole),
	m_ControlList(__FILE__LINE__0),
	m_FocusControl(NULL),
	m_FocusControlBorderIn(PosInNone),
	m_FocusControlLastMousePosition(),
	m_pLayout(NULL)
{
}

CConsoleTabPageControl::CConsoleTabPageControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole),
	m_ControlList(__FILE__LINE__0),
	m_FocusControl(NULL),
	m_FocusControlBorderIn(PosInNone),
	m_FocusControlLastMousePosition(),
	m_pLayout(NULL)
{
}

CConsoleTabPageControl::CConsoleTabPageControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole),
	m_ControlList(__FILE__LINE__0),
	m_FocusControl(NULL),
	m_FocusControlBorderIn(PosInNone),
	m_FocusControlLastMousePosition(),
	m_pLayout(NULL)
{
}

CConsoleTabPageControl::CConsoleTabPageControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(taborder, callback, name, title, pConsole),
	m_ControlList(__FILE__LINE__0),
	m_FocusControl(NULL),
	m_FocusControlBorderIn(PosInNone),
	m_FocusControlLastMousePosition(),
	m_pLayout(NULL)
{
}

CConsoleTabPageControl::~CConsoleTabPageControl(void)
{
	if ( m_pLayout )
		m_pLayout->release();
}

void CConsoleTabPageControl::AddControl(CConsoleControl* pControl, COORD pos, COORD size)
{
	Ptr(ControlData) data = OK_NEW_OPERATOR ControlData;

	data->pos = pos;
	data->size = size;
	data->control = pControl;

	m_ControlList.InsertSorted(data);
}

CConsoleControl* CConsoleTabPageControl::GetControl(CConstPointer name)
{
	ControlData data;

	data.control = OK_NEW_OPERATOR CConsoleControl(name, NULL);
	ControlDataList::Iterator it = m_ControlList.FindSorted(&data);

	if (m_ControlList.MatchSorted(it, &data))
		return (*it)->control;
	return NULL;
}

CConsoleControl* CConsoleTabPageControl::GetNextTabOrder(bool bnext)
{
	if ( !m_FocusControl )
		return NULL;

	ControlDataList::Iterator it;
	ControlData* pData;
	CConsoleControl* pCtrl;
	word curtaborder = m_FocusControl->GetTabOrder();

	if ( bnext )
	{
		CConsoleControl* pCtrlNext = NULL;
		word nexttaborder = USHRT_MAX;

		it = m_ControlList.Begin();
		while ( it )
		{
			pData = *it;
			pCtrl = pData->control;
			if ( (pCtrl->IsTabStop()) 
				&& (pCtrl->GetTabOrder() > curtaborder)
				&& (pCtrl->GetTabOrder() < nexttaborder) )
			{
				nexttaborder = pCtrl->GetTabOrder();
				pCtrlNext = pCtrl;
			}
			++it;
		}
		if ( !pCtrlNext )
		{
			nexttaborder = USHRT_MAX;
			it = m_ControlList.Begin();
			while ( it )
			{
				pData = *it;
				pCtrl = pData->control;
				if ( (pCtrl->IsTabStop()) && (pCtrl->GetTabOrder() < nexttaborder) )
				{
					nexttaborder = pCtrl->GetTabOrder();
					pCtrlNext = pCtrl;
				}
				++it;
			}
		}
		return pCtrlNext;
	}
	else
	{
		CConsoleControl* pCtrlPrev = NULL;
		word prevtaborder = 0;

		it = m_ControlList.Begin();
		while ( it )
		{
			pData = *it;
			pCtrl = pData->control;
			if ( (pCtrl->IsTabStop()) 
				&& (pCtrl->GetTabOrder() < curtaborder)
				&& (pCtrl->GetTabOrder() >= prevtaborder) )
			{
				prevtaborder = pCtrl->GetTabOrder();
				pCtrlPrev = pCtrl;
			}
			++it;
		}
		if ( !pCtrlPrev )
		{
			prevtaborder = 0;
			it = m_ControlList.Begin();
			while ( it )
			{
				pData = *it;
				pCtrl = pData->control;
				if ( (pCtrl->IsTabStop()) && (pCtrl->GetTabOrder() > prevtaborder) )
				{
					prevtaborder = pCtrl->GetTabOrder();
					pCtrlPrev = pCtrl;
				}
				++it;
			}
		}
		return pCtrlPrev;
	}
}

CConsoleControl* CConsoleTabPageControl::GetFirstTabOrder()
{
	ControlDataList::Iterator it = m_ControlList.Begin();
	ControlData* pData;
	CConsoleControl* focusCtrl = NULL;
	word taborder = USHRT_MAX;

	while ( it )
	{
		pData = *it;

		if ( (pData->control->IsTabStop()) && (pData->control->GetTabOrder() < taborder) )
		{
			focusCtrl = pData->control;
			taborder = focusCtrl->GetTabOrder();
		}
		++it;
	}
	return focusCtrl;
}

void CConsoleTabPageControl::SetFocus(bool hasFocus)
{

}

void CConsoleTabPageControl::Create(COORD pos, COORD size)
{
	CConsoleWindow::Create(pos, size);

	if ( m_hasBorder )
	{
		pos.X += 1;
		pos.Y += 1;
		size.X -= 2;
		size.Y -= 2;
	}
	if ( PtrCheck(m_pLayout) )
	{
		ControlDataList::Iterator it = m_ControlList.Begin();
		ControlData* pData;
		COORD pos1;

		while ( it )
		{
			pData = *it;
			pos1.X = pos.X + pData->pos.X;
			pos1.Y = pos.Y + pData->pos.Y;
			pData->control->Create(pos1, pData->size);
			++it;
		}
	}
	else
		m_pLayout->Create(pos, size);
	m_FocusControl = GetFirstTabOrder();
	if ( m_FocusControl )
		m_FocusControl->SetFocus(true);
}

void CConsoleTabPageControl::Move()
{
	COORD pos;

	pos.X = m_ClientArea.Left;
	pos.Y = m_ClientArea.Top;
	if ( PtrCheck(m_pLayout) )
	{
		ControlDataList::Iterator it = m_ControlList.Begin();
		ControlData* pData;
		COORD pos1;

		while ( it )
		{
			pData = *it;
			pos1.X = pos.X + pData->pos.X;
			pos1.Y = pos.Y + pData->pos.Y;
			pData->control->MoveWindow(pos1, false, true);
			++it;
		}
	}
	else
		m_pLayout->Move(pos);
}

void CConsoleTabPageControl::Paint(COORD bufferSize, PCHAR_INFO buffer)
{
	CConsoleWindow::Paint(bufferSize, buffer);

	ControlDataList::Iterator it = m_ControlList.Begin();
	ControlData* pData;

	while ( it )
	{
		pData = *it;
		pData->control->Paint(bufferSize, buffer);
		++it;
	}
}

void CConsoleTabPageControl::Undo()
{
	if ( m_FocusControl )
		m_FocusControl->Undo();
}

void CConsoleTabPageControl::Redo()
{
	if ( m_FocusControl )
		m_FocusControl->Redo();
}

void CConsoleTabPageControl::Cut()
{
	if ( m_FocusControl )
		m_FocusControl->Cut();
}

void CConsoleTabPageControl::Copy()
{
	if ( m_FocusControl )
		m_FocusControl->Copy();
}

void CConsoleTabPageControl::Paste()
{
	if ( m_FocusControl )
		m_FocusControl->Paste();
}

void CConsoleTabPageControl::Delete()
{
	if ( m_FocusControl )
		m_FocusControl->Delete();
}

bool CConsoleTabPageControl::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	switch ( virtualKeyCode )
	{
	CASE_KEY_TAB:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			CConsoleControl* pNextCtrl = GetNextTabOrder();

			if ( pNextCtrl )
			{
				if ( m_FocusControl )
					m_FocusControl->SetFocus(false);
				m_FocusControl = pNextCtrl;
				m_FocusControl->SetFocus(true);
			}
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0) 
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			CConsoleControl* pPrevCtrl = GetNextTabOrder(false);

			if ( pPrevCtrl )
			{
				if ( m_FocusControl )
					m_FocusControl->SetFocus(false);
				m_FocusControl = pPrevCtrl;
				m_FocusControl->SetFocus(true);
			}
		}
		break;
	default:
		if ( m_FocusControl )
		{
			if ( !(m_FocusControl->KeyDown(virtualKeyCode, controlKeyState)) )
				return false;
		}
		break;
	}
	return true;
}

bool CConsoleTabPageControl::KeyPress(TCHAR key, DWORD controlKeyState)
{
	if ( m_FocusControl )
	{
		if ( !(m_FocusControl->KeyPress(key, controlKeyState)) )
			return false;
	}
	return true;
}

bool CConsoleTabPageControl::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	if ( m_FocusControl )
	{
		if ( !(m_FocusControl->KeyUp(virtualKeyCode, controlKeyState)) )
			return false;
	}
	return true;
}

bool CConsoleTabPageControl::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	ControlDataList::Iterator it;
	ControlData* pData;
	CConsoleControl* pCtrl;

	it = m_ControlList.Begin();
	while ( it )
	{
		pData = *it;
		pCtrl = pData->control;
		if ( pCtrl->IsTabStop() )
		{
			if ( (pCtrl->HasBorder()) && (pCtrl->IsPosInNonClientRect(mousePos)) )
			{
				if ( m_FocusControl )
					m_FocusControl->SetFocus(false);
				m_FocusControl = pCtrl;
				m_FocusControl->SetFocus(true);

				if ( !(m_FocusControl->HasScrollbar()) )
					break;

				m_FocusControlBorderIn = m_FocusControl->PosInNonClientRect(mousePos);

				switch ( m_FocusControlBorderIn )
				{
				case PosInNone:
					break;
				case PosInHScrollLineLeft:
				case PosInHScrollPageLeft:
				case PosInHScrollPageRight:
				case PosInHScrollLineRight:
					m_FocusControl->DrawHorizontalScrollBar(m_FocusControlBorderIn);
					m_FocusControlBorderIn = PosInNone;
					break;
				case PosInHScrollThumb:
					m_FocusControlLastMousePosition = mousePos;
					break;
				case PosInVScrollLineUp:
				case PosInVScrollPageUp:
				case PosInVScrollPageDown:
				case PosInVScrollLineDown:
					m_FocusControl->DrawVerticalScrollBar(m_FocusControlBorderIn);
					m_FocusControlBorderIn = PosInNone;
					break;
				case PosInVScrollThumb:
					m_FocusControlLastMousePosition = mousePos;
					break;
				default:
					break;
				}
				break;
			}
			else if ( pCtrl->IsPosInClientRect(mousePos) )
			{
				if ( m_FocusControl )
					m_FocusControl->SetFocus(false);
				m_FocusControl = pCtrl;
				m_FocusControl->SetFocus(true);
				return m_FocusControl->LeftMouseDown(mousePos, controlKeyState);
			}
		}
		++it;
	}
	return true;
}

bool CConsoleTabPageControl::LeftMouseUp(COORD mousePos, DWORD controlKeyState)
{
	m_FocusControlBorderIn = PosInNone;
	return true;
}

bool CConsoleTabPageControl::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTabPageControl::RightMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTabPageControl::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	if ( m_FocusControl )
	{
		if ( m_FocusControl->IsPosInClientRect(mousePos) )
			return m_FocusControl->LeftMouseDoubleDown(mousePos, controlKeyState);
	}
	return true;
}

bool CConsoleTabPageControl::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTabPageControl::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	if ( m_FocusControl )
	{
		switch ( m_FocusControlBorderIn )
		{
		case PosInNone:
			return m_FocusControl->LeftMouseDownMove(mousePos, controlKeyState);
		case PosInHScrollThumb:
			{
				sword deltaX = mousePos.X - m_FocusControlLastMousePosition.X;

				m_FocusControlLastMousePosition = mousePos;
				m_FocusControl->MoveHScrollThumb(deltaX);
			}
			break;
		case PosInVScrollThumb:
			{
				sword deltaY = mousePos.Y - m_FocusControlLastMousePosition.Y;

				m_FocusControlLastMousePosition = mousePos;
				m_FocusControl->MoveVScrollThumb(deltaY);
			}
			break;
        default:
            break;
		}
	}
	return true;
}

bool CConsoleTabPageControl::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}
