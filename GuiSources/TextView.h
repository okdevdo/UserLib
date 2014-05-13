/******************************************************************************
    
	This file is part of GuiSources, which is part of UserLib.

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

#include "afxwin.h"

class CTextViewChars;
class CTextViewUndoStack;
class CTextViewUndoStruct;
class CTextView: public CControl
{
public:
	CTextView(LPCTSTR name = NULL);
	CTextView(ConstRef(CStringBuffer) name);
	virtual ~CTextView(void);

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	dword get_tabsize() const;
	void set_tabsize(dword tabsize);

	CStringBuffer get_text() const;
	void set_text(const CStringBuffer& text);
	void Refresh(BOOL bUpdate = FALSE);

	bool has_selection() const;

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnChar(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnVScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	LRESULT OnEditUndoCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditRedoCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditCutCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditCopyCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditPasteCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditDeleteCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void _Edit_Delete_Selection(CTextViewUndoStruct& undoStruct);
	void _Edit_Delete_Char(CTextViewUndoStruct& undoStruct);
	void _Edit_Insert_Char(CTextViewUndoStruct& undoStruct, TCHAR keyChar);
	void _Edit_Insert_Chars(CTextViewUndoStruct& undoStruct, LPCTSTR lptstr, dword len0);
	bool _EditCopyCommand();
	bool _EditDeleteCommand();
	BOOL _StartTimer();
	BOOL _StopTimer();

	enum TTimerDirection
	{
		TTimerDirectionNone,
		TTimerDirectionNorth,
		TTimerDirectionSouth,
		TTimerDirectionWest,
		TTimerDirectionEast
	};

	POINT m_cursorPos;
	POINT m_cursorPosOld;
	POINT m_scrollPos;
	POINT m_mouseHit;
	UINT m_mouseHitCode;
	UINT m_mouseHitModifier;
	BOOL m_caretCreated;
	BOOL m_caretShown;
	BOOL m_timerEnabled;
	TTimerDirection m_timerDirection;
	BOOL m_insidePaint;
	BOOL m_hasselection;
	CTextViewChars* m_chars;
	CTextViewUndoStack* m_undostack;
};

