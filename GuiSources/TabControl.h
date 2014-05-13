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

class CTabPage: public CPanel
{
public:
	CTabPage(LPCTSTR name = NULL);
	CTabPage(ConstRef(CStringBuffer) name);
	virtual ~CTabPage(void);

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);

	__inline ConstRef(CStringBuffer) get_Text() { return m_text; }
	void set_Text(LPCTSTR pText, int lench = -1);
	void set_Text(ConstRef(CStringBuffer) text);

	__inline bool is_Selected() { return m_selected; }
	void set_Selected(bool selected);

	__inline void get_TitleRect(LPRECT rect) { ::CopyRect(rect, &m_titleRect); }
	__inline void set_TitleRect(LPRECT rect) { ::CopyRect(&m_titleRect, rect); }

	void calc_TitleRect(Gdiplus::Graphics* pGraphics, LPPOINT pPt, LONG* maxHeight, LONG* maxWidth);
	void paint_TitleRect(Gdiplus::Graphics* pGraphics);

	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	CStringBuffer m_text;
	RECT m_titleRect;
	bool m_selected;
};

class CTabControl: public CControl
{
public:
	enum TTitleOrientation
	{
		TTitleOrientationTop,
		TTitleOrientationBottom
	};

public:
	CTabControl(LPCTSTR name = NULL);
	CTabControl(ConstRef(CStringBuffer) name);
	virtual ~CTabControl(void);

	__inline TTitleOrientation get_TitleOrientation() { return m_TitleOrientation; }
	__inline void set_TitleOrientation(TTitleOrientation v) { m_TitleOrientation = v; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);

	__inline CTabPage* get_CurrentTabPage() { return m_currentTabPage; }
	void set_CurrentTabPage(CTabPage* page);

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetDlgCode(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	TTitleOrientation m_TitleOrientation;
	RECT m_titleBarRect;
	POINT m_mouseclick;

	CTabPage* m_currentTabPage;
};

