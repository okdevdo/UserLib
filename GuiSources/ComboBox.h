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
#include "afxrsrc.h"

class CTextEditor;
class CComboPopup;
class CComboBox: public CControl
{
public:
    CComboBox(LPCTSTR name = NULL);
    CComboBox(ConstRef(CStringBuffer) name);
	virtual ~CComboBox();

	__inline bool is_Editable() { return m_editable; }
	__inline void set_Editable(bool editable) { m_editable = editable; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);

	ConstRef(CStringBuffer) get_text();
	void set_text(ConstRef(CStringBuffer) text);

	void add_Text(LPCTSTR text);
	void add_Text(ConstRef(CStringBuffer) text);

	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetDlgCode(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	LRESULT OnEditorReturn(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorTab(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorFuncKey(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorLostFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorEscape(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorCursorUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorCursorDown(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	bool m_editable;
	CStringBuffer m_text;
	CTextEditor* m_pTextEditor;
	CComboPopup* m_pPopup;
	CBitmap m_DropDown;
	RECT m_imageRect;
	RECT m_textRect;
};