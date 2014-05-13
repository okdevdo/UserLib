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

class CTextEditorChars;
class CTextEditorUndoStack;
class CTextEditorUndoStruct;
class CTextEditor: public CControl
{
public:
	CTextEditor(LPCTSTR name = NULL);
	CTextEditor(ConstRef(CStringBuffer) name);
	virtual ~CTextEditor(void);

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	__inline const CStringBuffer& get_text() const { return m_text; }
	void set_text(const CStringBuffer& text);
	void Reset(BOOL bUpdate = FALSE);

	bool has_selection() const;

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnMenuCommand(WPARAM wParam, LPARAM lParam);
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
	LRESULT OnGetDlgCode(WPARAM wParam, LPARAM lParam);
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
	void _Edit_Delete_Selection(CTextEditorUndoStruct& undoStruct);
	void _Edit_Delete_Char(CTextEditorUndoStruct& undoStruct);
	void _Edit_Insert_Char(CTextEditorUndoStruct& undoStruct, TCHAR keyChar);
	void _Edit_Insert_Chars(CTextEditorUndoStruct& undoStruct, LPTSTR lptstr, dword len0);
	bool _EditCopyCommand();
	bool _EditDeleteCommand();
	BOOL _StartTimer();
	BOOL _StopTimer();

	CStringBuffer m_text;
	dword m_cursorPos;
	dword m_scrollPos;
	POINT m_mouseHit;
	UINT m_mouseHitCode;
	UINT m_mouseHitModifier;
	BOOL m_caretCreated;
	BOOL m_caretShown;
	BOOL m_timerEnabled;
	BOOL m_timerForward;
	BOOL m_hasselection;
	HMENU m_hPopupMenu;
	CTextEditorChars* m_chars;
	CTextEditorUndoStack* m_undostack;
};

