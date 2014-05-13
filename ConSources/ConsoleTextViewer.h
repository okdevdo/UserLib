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

#include "FilePath.h"
#include "ConSources.h"
#include "ConsoleWindow.h"

class CConsoleUndoGroup;
class CONSOURCES_API CConsoleTextViewer: public CConsoleWindow
{
public:
	CConsoleTextViewer(CConstPointer name, CConsole* pConsole);
	CConsoleTextViewer(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleTextViewer(ConstRef(CFilePath) path, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleTextViewer(void);

	void Initialize(bool _emptyText = false);
	void LoadFile(ConstRef(CFilePath) path);
	void SetText(ConstRef(CStringBuffer) _text);
	CStringBuffer GetText();
	void DrawTextView();
	void SetCursorPosEx();

	virtual void Create(COORD pos, COORD size);
	virtual void SetFocus(bool hasFocus);
	virtual void Resize();
	virtual void Scroll();

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
	CPointer ReallocLine(dword limit = 4, dword expand = 8);
	void JoinLines(bool bBeforeCurrent = true);
	void SplitLine(void);
	void DeleteKey(bool bBackSpace = false);
	void _DeleteKey(bool bBackSpace = false);
	void DeleteSelection(void);
	void _DeleteSelection(void);
	void DeleteLines(sdword at, sdword cnt);
	void InsertLines(sdword at, sdword cnt, CArray texts);
	void InsertKey(TCHAR ch);
	void _InsertKey(TCHAR ch);
	void InsertText(ConstRef(CStringBuffer) _text);
	void _InsertText(ConstRef(CStringBuffer) _text);
	CStringBuffer GetSelection();
	void Update(void);
	void _SetText();
	void _LoadFile(void);
	bool CheckVScroll(void);
	bool CheckHScroll(void);

	LCOORD m_CursorPos;
	LCOORD m_SelectionEnd;
	LCOORD m_ScrollPos;
	CStringBuffer m_Text;
	sdword m_TextLineCnt;
	CArray m_TextLines;
	sdword* m_TextLineLen;
	sdword m_TextLineLenMax;
	bool *m_TextLineAlloc;
	CFilePath m_Path;
	bool m_SelectionMode;
	Ptr(CConsoleUndoGroup) m_UndoGroup;

#ifdef __DBGPRINT__
	void PrintTextLine(sdword at);
#endif

private:
	CConsoleTextViewer();
	CConsoleTextViewer(ConstRef(CConsoleTextViewer));
};

