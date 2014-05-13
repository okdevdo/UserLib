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
#include "ConsoleComboControl.h"
#include "ConsoleComboPopupListControl.h"
#include "Console.h"

CConsoleComboControl::CConsoleComboControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole),
	m_DropDownList(NULL)
{
}

CConsoleComboControl::CConsoleComboControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole),
	m_DropDownList(NULL)
{
}

CConsoleComboControl::CConsoleComboControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole),
	m_DropDownList(NULL)
{
}

CConsoleComboControl::CConsoleComboControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(taborder, callback, name, title, pConsole),
	m_DropDownList(NULL)
{
}

CConsoleComboControl::~CConsoleComboControl(void)
{
	if ( m_DropDownList )
		m_DropDownList->release();
}

void CConsoleComboControl::OpenDropDownList()
{
	if ( m_DropDownList )
	{
		COORD pos;

		pos.X = m_NonClientArea.Left;
		pos.Y = m_NonClientArea.Top + 1;
		if ( !(m_DropDownList->IsCreated()) )
		{
			COORD size;

			size.X = m_ScreenBufferSize.X;
			size.Y = Cast(sword, m_DropDownList->GetListItemCount() + 2);
			m_DropDownList->Create(pos, size);
		}
		else
			m_DropDownList->MoveWindow(pos, false, true);
		if ( m_Console )
			m_Console->SetConsolePopupControl(m_DropDownList);
	}
}

void CConsoleComboControl::AddComboItem(ConstRef(CStringBuffer) item)
{
	if ( !m_DropDownList )
	{
		CStringBuffer name(m_Name);

		name.AppendString(_T(".DropDownList"));
		m_DropDownList = OK_NEW_OPERATOR CConsoleComboPopupListControl(
			OK_NEW_OPERATOR CConsoleControlCallback<CConsoleComboControl>(this, 
			    &CConsoleComboControl::DropDownListCallback),
			name,
			_T("DropDownList"),
			m_Console);
	}
	m_DropDownList->AddListItem(item);
}

CStringBuffer CConsoleComboControl::GetCurrentComboItem()
{
	if ( m_DropDownList )
		return m_DropDownList->GetCurrentListItem();
	return CStringBuffer();
}

void CConsoleComboControl::SetCurrentComboItem(ConstRef(CStringBuffer) item)
{
	if ( m_DropDownList )
		m_DropDownList->SetCurrentListItem(item);
}

dword CConsoleComboControl::GetComboItemCount()
{
	if ( m_DropDownList )
		return m_DropDownList->GetListItemCount();
	return 0;
}

void CConsoleComboControl::DropDownListCallback(CConsoleControl* pControl, DWORD command)
{
	if ( !m_Console )
		return;

	CConsoleListControl* pListControl = CastDynamic(CConsoleListControl*, pControl);

	if ( !pListControl )
		return;
	switch ( command )
	{
	case CConsoleControl::ListItemSelected:
		DrawComboControl();
		PostPaintEvent();
		break;
	case CConsoleControl::ListItemClicked:
		DrawComboControl();
		m_Console->CloseConsolePopupControl();
		InvokeControlCommand(CConsoleControl::ComboItemSelected);
		break;
	}
}

void CConsoleComboControl::DrawComboControl(bool bHighLight)
{
	SMALL_RECT rect;
	CStringBuffer tmp;

	rect.Left = 0;
	rect.Right = m_ScreenBufferSize.X;
	rect.Top = 0;
	rect.Bottom = m_ScreenBufferSize.Y;
	if ( m_DropDownList )
		tmp = m_DropDownList->GetCurrentListItem();
	if ( tmp.IsEmpty() )
		tmp.SetString(__FILE__LINE__ _T(" "));
	DrawString(rect, tmp, bHighLight?m_HighLightColor:m_Color);
}

void CConsoleComboControl::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultHighlightedColor();
	m_HighLightColor = m_Console->GetDefaultColor();
	CConsoleControl::Create(pos, size);
	DrawComboControl();
}

void CConsoleComboControl::SetFocus(bool hasFocus)
{
	DrawComboControl(hasFocus);
	PostPaintEvent();
}

bool CConsoleComboControl::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleComboControl::KeyPress(TCHAR key, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) != 0 )
		return true;
	switch ( key )
	{
	case 0x20:
		OpenDropDownList();
		break;
	default:
		break;
	}
	return true;
}

bool CConsoleComboControl::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleComboControl::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	if ( !m_Console )
		return true;
	OpenDropDownList();
	return true;
}

bool CConsoleComboControl::LeftMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleComboControl::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleComboControl::RightMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleComboControl::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleComboControl::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleComboControl::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleComboControl::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

