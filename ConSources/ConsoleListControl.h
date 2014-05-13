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

#include "DataVector.h"

#include "ConSources.h"
#include "ConsoleControl.h"

class CONSOURCES_API CConsoleListControl: public CConsoleControl
{
public:
	CConsoleListControl(CConstPointer name, CConsole* pConsole);
	CConsoleListControl(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleListControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleListControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleListControl(void);

	void ClearListe();
	dword GetListItemCount();
	void AddListItem(ConstRef(CStringBuffer) item);
	CStringBuffer GetCurrentListItem();
	void SetCurrentListItem(ConstRef(CStringBuffer) item);

	void DrawListControl(bool bhighlight = true);

	virtual void Create(COORD pos, COORD size);
	virtual void SetFocus(bool hasFocus);
	virtual void Scroll();

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
	virtual bool WheelMouse(COORD mousePos, sword rotateCnt, DWORD controlKeyState);

protected:
	typedef CDataVectorT<CStringBuffer> CStringListe;

	CStringListe m_List;
	TListCnt m_HighlightPos;
	TListCnt m_ScrollPos;

	TListCnt IndexFromPos(COORD pos);
	void Update();

private:
	CConsoleListControl();
	CConsoleListControl(ConstRef(CConsoleListControl));
};

