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
#pragma once

#include "ConSources.h"
#include "ConsoleControl.h"

class CConsoleComboPopupListControl;
class CONSOURCES_API CConsoleComboControl: public CConsoleControl
{
public:
	CConsoleComboControl(CConstPointer name, CConsole* pConsole);
	CConsoleComboControl(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleComboControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleComboControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleComboControl(void);

	void DrawComboControl(bool bHighLight = false);

	void AddComboItem(ConstRef(CStringBuffer) item);
	CStringBuffer GetCurrentComboItem();
	void SetCurrentComboItem(ConstRef(CStringBuffer) item);
	dword GetComboItemCount();

	void DropDownListCallback(CConsoleControl* pControl, DWORD command);

	virtual void Create(COORD pos, COORD size);
	virtual void SetFocus(bool hasFocus);

	virtual bool KeyDown(WORD virtualKeyCode, DWORD controlKeyState);
	virtual bool KeyPress(TCHAR key, DWORD controlKeyState);
	virtual bool KeyUp(WORD virtualKeyCode, DWORD controlKeyState);

	virtual bool LeftMouseDown(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseUp(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDown(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseUp(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseDownMove(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDownMove(COORD mousePos, DWORD controlKeyState);

protected:
	CConsoleComboPopupListControl* m_DropDownList;

	void OpenDropDownList();

private:
	CConsoleComboControl();
	CConsoleComboControl(ConstRef(CConsoleComboControl));
};
