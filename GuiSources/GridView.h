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
#include "DataVector.h"

class CGridView;
class CGridViewEmptyCell: public CCppObject
{
public:
	CGridViewEmptyCell(CGridView* pGridView = NULL);
	virtual ~CGridViewEmptyCell(void);

	__inline CGridView* get_GridView() { return m_gridView; }

	__inline bool is_Selected() { return m_selected; }
	void set_Selected(bool selected);

	CGridViewEmptyCell* HitTest(POINT pt);

	void OnAdjustRects(INT right, INT bottom);
	void OnPaint(Gdiplus::Graphics* graphics);

protected:
	CGridView* m_gridView;
	bool m_selected;
	RECT m_borderRect;
};

class CGridViewRow;
class CGridViewColumn;
class CGridViewCell: public CCppObject
{
public:
	CGridViewCell(CGridView* pGridView = NULL, CGridViewRow* pGridViewRow = NULL, CGridViewColumn* pGridViewColumn = NULL, LPTSTR text = NULL, int lench = -1);
	CGridViewCell(CGridView* pGridView, CGridViewRow* pGridViewRow, CGridViewColumn* pGridViewColumn, ConstRef(CStringBuffer) text);
	virtual ~CGridViewCell(void);

	__inline CGridView* get_GridView() { return m_gridView; }
	__inline void set_GridView(CGridView* gridView) { m_gridView = gridView; }
	__inline CGridViewRow* get_Row() { return m_gridViewRow; }
	__inline void set_Row(CGridViewRow* row) { m_gridViewRow = row; }
	__inline CGridViewColumn* get_Column() { return m_gridViewColumn; }
	__inline void set_Column(CGridViewColumn* column) { m_gridViewColumn = column; }

	__inline ConstRef(CStringBuffer) get_Text() { return m_text; }
	void set_Text(ConstRef(CStringBuffer) text);
	void set_Text(LPCTSTR pText = NULL, int lench = -1);

	__inline void get_Rect(LPRECT rect) { ::CopyRect(rect, &m_borderRect); }
	void get_EditorRect(LPRECT rect);

	__inline bool is_Selected() { return m_selected; }
	void set_Selected(bool selected);

	__inline bool is_Focused() { return m_focused; }
	void set_Focused(bool focused);

	CGridViewCell* HitTest(POINT pt, INT xPos, INT yPos, INT cBottom);

	void OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth, LPINT maxHeight);
	void OnAdjustRects(INT left, INT right, INT top, INT bottom);
	void OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom);

protected:
	CGridView* m_gridView;
	CGridViewRow* m_gridViewRow;
	CGridViewColumn* m_gridViewColumn;
	CStringBuffer m_text;
	bool m_selected;
	bool m_focused;
	RECT m_textRect;
	RECT m_borderRect;
};

typedef CDataVectorT<CGridViewCell> CGridViewCells;
typedef CDataVectorT<CGridViewCell, CCppObjectLessFunctor<CGridViewCell>, CCppObjectNullFunctor<CGridViewCell>> CGridViewSelectedCells;

class CTextEditor;
class CGridViewColumn: public CCppObject
{
public:
	CGridViewColumn(CGridView* pGridView = NULL, LPCTSTR text = NULL, int lench = -1);
	CGridViewColumn(CGridView* pGridView, ConstRef(CStringBuffer) text);
	virtual ~CGridViewColumn(void);

	__inline CGridView* get_GridView() { return m_gridView; }

	__inline ConstRef(CStringBuffer) get_Text() { return m_text; }
	void set_Text(ConstRef(CStringBuffer) text);
	void set_Text(LPCTSTR pText = NULL, int lench = -1);

	__inline void get_Rect(LPRECT rect) { ::CopyRect(rect, &m_borderRect); }

	__inline bool is_Selected() { return m_selected; }
	void set_Selected(bool selected);

	__inline bool is_Editable() { return m_editable; }
	__inline void set_Editable(bool editable) { m_editable = editable; }

	__inline CControl* get_Editor() { return m_editor; }
	__inline void set_Editor(CControl* editor) { m_editor = editor; }

	CGridViewColumn* HitTest(POINT pt);

	void OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth, LPINT maxHeight);
	void OnAdjustRects(INT left, INT right, INT maxBottom);
	void OnPaint(Gdiplus::Graphics* graphics);

protected:
	CGridView* m_gridView;
	CStringBuffer m_text;
	bool m_selected;
	bool m_editable;
	RECT m_textRect;
	RECT m_borderRect;
	CControl* m_editor;
};

typedef CDataVectorT<CGridViewColumn> CGridViewColumns;
typedef CDataVectorT<CGridViewColumn, CCppObjectLessFunctor<CGridViewColumn>, CCppObjectNullFunctor<CGridViewColumn>> CGridViewSelectedColumns;

class CGridViewRow: public CCppObject
{
public:
	CGridViewRow(CGridView* pGridView = NULL, LPCTSTR text = NULL, int lench = -1);
	CGridViewRow(CGridView* pGridView, ConstRef(CStringBuffer) text);
	virtual ~CGridViewRow(void);

	__inline CGridView* get_GridView() { return m_gridView; }

	__inline ConstRef(CStringBuffer) get_Text() { return m_text; }
	void set_Text(ConstRef(CStringBuffer) text);
	void set_Text(LPCTSTR pText = NULL, int lench = -1);

	__inline dword get_CellCount() { return m_cells.Count(); }
	__inline CGridViewCell* get_Cell(dword ix) { return *(m_cells.Index(ix)); }
	CGridViewCell* get_Cell(LPCTSTR name);
	void set_Cell(dword ix, CGridViewCell* cell);
	void set_CellText(LPCTSTR name, LPCTSTR text = NULL, int lenCh = -1);
	void set_CellText(LPCTSTR name, ConstRef(CStringBuffer) text);
	int inx_Cell(CGridViewCell* cell);

	__inline bool is_Selected() { return m_selected; }
	void set_Selected(bool selected);

	CGridViewRow* HitTest(POINT pt, INT xPos, INT yPos, INT cBottom);

	void OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth, LPINT maxHeight);
	void OnAdjustRects(INT top, INT bottom, INT maxRight);
	void OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom);

protected:
	CGridView* m_gridView;
	CStringBuffer m_text;
	bool m_selected;
	RECT m_textRect;
	RECT m_borderRect;
	CGridViewCells m_cells;
};

typedef CDataVectorT<CGridViewRow> CGridViewRows;
typedef CDataVectorT<CGridViewRow, CCppObjectLessFunctor<CGridViewRow>, CCppObjectNullFunctor<CGridViewRow>> CGridViewSelectedRows;

class CGridView: public CControl
{
public:
	CGridView(LPCTSTR name = NULL);
	CGridView(ConstRef(CStringBuffer) name);
	virtual ~CGridView(void);

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);

	__inline dword get_ColumnCount() { return m_columns.Count(); }
	__inline CGridViewColumn* get_Column(dword ix) { return *(m_columns.Index(ix)); }
	CGridViewColumn* get_Column(LPCTSTR name);
	void set_Column(dword ix, CGridViewColumn* column);
	int inx_Column(CGridViewColumn* column);

	__inline dword get_RowCount() { return m_rows.Count(); }
	__inline CGridViewRow* get_Row(dword ix) { return *(m_rows.Index(ix)); }
	void set_Row(dword ix, CGridViewRow* row);
	int inx_Row(CGridViewRow* row);

	__inline dword get_SelCellCount() { return m_selCells.Count(); }
	__inline CGridViewCell* get_SelCell(dword ix) { return *(m_selCells.Index(ix)); }
	void set_SelCell(dword ix, CGridViewCell* cell);
	int inx_SelCell(CGridViewCell* cell);

	__inline dword get_SelRowCount() { return m_selRows.Count(); }
	__inline CGridViewRow* get_SelRow(dword ix) { return *(m_selRows.Index(ix)); }
	void set_SelRow(dword ix, CGridViewRow* row);
	int inx_SelRow(CGridViewRow* row);

	__inline dword get_SelColumnCount() { return m_selColumns.Count(); }
	__inline CGridViewColumn* get_SelColumn(dword ix) { return *(m_selColumns.Index(ix)); }
	void set_SelColumn(dword ix, CGridViewColumn* column);
	int inx_SelColumn(CGridViewColumn* column);

	__inline bool is_MultiSelect() { return m_multiSelect; }
	__inline void set_MultiSelect(bool multiSelect) { m_multiSelect = multiSelect; }

	__inline bool is_Editable() { return m_editable; }
	__inline void set_Editable(bool editable) { m_editable = editable; }
	bool set_Editable(LPCTSTR key, bool editable);

	__inline CGridViewCell* get_CurrentCell() { return m_currentCell; }
	void set_CurrentCell(CGridViewCell* cell);

	CGridViewCell* get_Cell(dword column, LPCTSTR key);

	__inline Gdiplus::StringFormat* get_StringFormat() { return m_stringFormat; }

	void SelectAll(bool selected);
	
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnAboutHScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnVScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnAboutVScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);

	LRESULT OnEditorReturn(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorTab(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorFuncKey(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorLostFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorEscape(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorCursorUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorCursorDown(WPARAM wParam, LPARAM lParam);

	LRESULT OnEditUndoCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditRedoCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditCutCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditCopyCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditPasteCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditDeleteCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	bool m_multiSelect;
	bool m_editable;

	CGridViewEmptyCell* m_emptyCell;
	CGridViewColumns m_columns;
	CGridViewRows m_rows;
	CGridViewSelectedCells m_selCells;
	CGridViewSelectedRows m_selRows;
	CGridViewSelectedColumns m_selColumns;

	CGridViewCell* m_currentCell;

	void _CurrentCellDown();
	void _CurrentCellUp();
	void _CurrentCellLeft();
	void _CurrentCellRight();
	void _EnsureVisible();
	void _ShowEditor();
	void _CloseEditor(BOOL bSave = TRUE);

	Gdiplus::StringFormat* m_stringFormat;
	CControl* m_editor;

};

