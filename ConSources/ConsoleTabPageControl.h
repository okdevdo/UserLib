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

#include "DataDoubleLinkedList.h"
#include "ConSources.h"
#include "ConsoleControl.h"

class CConsoleLayout;
class CONSOURCES_API CConsoleTabPageControl: public CConsoleControl
{
public:
	CConsoleTabPageControl(CConstPointer name, CConsole* pConsole);
	CConsoleTabPageControl(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleTabPageControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleTabPageControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleTabPageControl(void);

	void AddControl(CConsoleControl* pControl, COORD pos, COORD size);
	CConsoleControl* GetControl(CConstPointer name);

	virtual void Create(COORD pos, COORD size);
	virtual void SetFocus(bool hasFocus);
	virtual void Paint(COORD bufferSize, PCHAR_INFO buffer);
	virtual void Move();

	virtual void Undo();
	virtual void Redo();
	virtual void Cut();
	virtual void Copy();
	virtual void Paste();
	virtual void Delete();

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

public:
	typedef struct __tagControlData
	{
		COORD pos;
		COORD size;
		CConsoleControl* control;
	} ControlData;

protected:
	typedef CDataSDoubleLinkedListT<ControlData> ControlDataList;

	CConsoleControl* GetNextTabOrder(bool bnext = true);
	CConsoleControl* GetFirstTabOrder();

	ControlDataList m_ControlList;
	CConsoleControl* m_FocusControl;
	PosIn m_FocusControlBorderIn;
	COORD m_FocusControlLastMousePosition;
	Ptr(CConsoleLayout) m_pLayout;

private:
	CConsoleTabPageControl(void);
	CConsoleTabPageControl(ConstRef(CConsoleTabPageControl));
};

