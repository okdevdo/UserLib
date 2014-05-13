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
#include "DataDoubleLinkedList.h"

class CConsoleTabPageControl;
class CONSOURCES_API CConsoleTabControl: public CConsoleControl
{
public:
	CConsoleTabControl(CConstPointer name, CConsole* pConsole);
	CConsoleTabControl(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleTabControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleTabControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleTabControl(void);

	void Initialize(void);
	__inline void AddTabPage(Ptr(CConsoleTabPageControl) page) { m_Pages.Append(page); }
	void DrawTabControl(void);

	virtual void Create(COORD pos, COORD size);
	virtual void SetFocus(bool hasFocus);
	virtual void Resize();
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
	virtual bool RightMouseDown(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseDownMove(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDownMove(COORD mousePos, DWORD controlKeyState);

protected:
	typedef CDataDoubleLinkedListT<CConsoleTabPageControl> TPageControls;

	TPageControls m_Pages;
	Ptr(CConsoleTabPageControl) m_CurrentPage;
	sword m_CurrentPageNo;

	sword IndexFromPos(COORD pos);

private:
	CConsoleTabControl(void);
	CConsoleTabControl(ConstRef(CConsoleTabControl));
};

