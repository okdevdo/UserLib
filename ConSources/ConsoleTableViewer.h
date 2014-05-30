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

class CConsoleControl;
class CONSOURCES_API CConsoleTableViewer: public CConsoleWindow
{
public:
	class DataItem: public CCppObject
	{
	public:
		DataItem(TListIndex row = -1, TListIndex col = -1): m_row(row), m_col(col) {}
		DataItem(TListIndex row, TListIndex col, ConstRef(CStringBuffer) data): m_row(row), m_col(col), m_data(data) {}
		virtual ~DataItem() {}

		__inline TListIndex get_row() const { return m_row; }
		__inline void set_row(TListIndex v) { m_row = v; }
		__inline TListIndex get_col() const { return m_col; }
		__inline void set_col(TListIndex v) { m_col = v; }
		__inline ConstRef(CStringBuffer) get_data() const { return m_data; }
		__inline void set_data(ConstRef(CStringBuffer) v) { m_data = v; }

	protected:
		TListIndex m_row;
		TListIndex m_col;
		CStringBuffer m_data;
	};

	typedef CDataVectorT<DataItem> DataItemVector;

	class DataColumn: public CCppObject
	{
	public:
		enum DataType
		{
			DataTypeFixedString,
			DataTypeVarString,
			DataTypeNumeric,
			DataTypeFloat,
			DataTypeIdentity,
			DataTypeLogical,
			DataTypeDate,
			DataTypeTime
		};

	public:
		DataColumn(TListIndex col = -1): 
		    m_col(col), m_datatype(DataTypeFixedString), m_alignment(CConsoleWindow::AlignmentLeft), m_decimals(0), m_width(0), m_maxwidth(0), m_control(NULL) {}
		DataColumn(TListIndex col, ConstRef(CStringBuffer) title): 
		    m_col(col), m_datatype(DataTypeFixedString), m_alignment(CConsoleWindow::AlignmentLeft), m_decimals(0), m_width(0), m_maxwidth(0), m_title(title), m_control(NULL) {}
		virtual ~DataColumn();

		__inline TListIndex get_col() const { return m_col; }
		__inline void set_col(TListIndex v) { m_col = v; }
		__inline DataType get_datatype() const { return m_datatype; }
		__inline void set_datatype(DataType v) { m_datatype = v; }
		__inline CConsoleWindow::Alignment get_alignment() const { return m_alignment; }
		__inline void set_alignment(CConsoleWindow::Alignment v) { m_alignment = v; }
		__inline TListCnt get_decimals() const { return m_decimals; }
		__inline void set_decimals(TListCnt v) { m_decimals = v; }
		__inline TListCnt get_width() const { return m_width; }
		__inline void set_width(TListCnt v) { m_width = v; }
		__inline TListCnt get_maxwidth() const { return m_maxwidth; }
		__inline void set_maxwidth(TListCnt v) { m_maxwidth = v; }
		__inline ConstRef(CStringBuffer) get_title() const { return m_title; }
		__inline void set_title(ConstRef(CStringBuffer) v) { m_title = v; }
		__inline Ptr(CConsoleControl) get_control() const { return m_control; }
		__inline void set_control(Ptr(CConsoleControl) ctrl) { m_control = ctrl; }

	protected:
		TListIndex m_col;
		DataType m_datatype;
		CConsoleWindow::Alignment m_alignment;
		TListCnt m_decimals;
		TListCnt m_width;
		TListCnt m_maxwidth;
		CStringBuffer m_title;
		Ptr(CConsoleControl) m_control;
	};

	class DataColumnVector: public CDataVectorT<DataColumn>
	{
	public:
		DataColumnVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp): CDataVectorT<DataColumn>(ARGS_FILE_LINE cnt, exp) {}
		virtual ~DataColumnVector() {}

		TListCnt get_width() const;
		Ptr(DataColumn) get_column(sdword start, Ref(sdword) pos) const;
		__inline Ptr(DataColumn) get_column(TListIndex ix) const { Iterator it = Index(ix); return *it; }

		void draw_titles(sdword start, SMALL_RECT r, Ptr(CConsoleWindow) w);

	private:
		DataColumnVector();
	};

	class DataRow: public CCppObject
	{
	public:
		DataRow(TListIndex row = -1, TListCnt height = 0) : m_row(row), m_height(height), m_items(__FILE__LINE__ 16, 16), m_columns(__FILE__LINE__ 16, 16) {}
		DataRow(TListIndex row, TListCnt height, ConstRef(CStringBuffer) title, ConstRef(DataColumnVector) cols): 
			m_row(row), m_height(height), m_title(title), m_items(__FILE__LINE__ 16, 16), m_columns(cols) {}
		virtual ~DataRow();

		__inline TListIndex get_row() const { return m_row; }
		__inline void set_row(TListIndex v) { m_row = v; }
		__inline TListCnt get_height() const { return m_height; }
		__inline void set_height(TListCnt v) { m_height = v; }
		__inline ConstRef(CStringBuffer) get_title() const { return m_title; }
		__inline void set_title(ConstRef(CStringBuffer) v) { m_title = v; }
		__inline ConstRef(DataColumnVector) get_columns() const { return m_columns; }

		__inline void add_item(Ptr(DataItem) item) { m_items.Append(item); }
		__inline Ptr(DataItem) get_item(TListIndex ix) { DataItemVector::Iterator it = m_items.Index(ix); return *it; }

	protected:
		TListIndex m_row;
		TListCnt m_height;
		CStringBuffer m_title;
		DataItemVector m_items;
		DataColumnVector m_columns;
	};

	class DataRowVector: public CDataVectorT<DataRow>
	{
	public:
		DataRowVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp): CDataVectorT<DataRow>(ARGS_FILE_LINE cnt, exp) {}

		TListCnt get_height() const;
		Ptr(DataItem) get_item(LCOORD start, Ref(LCOORD) pos, Ref(Ptr(DataColumn)) col);

		void draw_items(LCOORD start, SMALL_RECT r, sdword cpos, sdword spos, Ptr(CConsoleWindow) w);

	private:
		DataRowVector();
	};

public:
	CConsoleTableViewer(CConstPointer name, CConsole* pConsole);
	CConsoleTableViewer(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleTableViewer(ConstRef(CFilePath) path, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleTableViewer(void);

	void Initialize();
	__inline void AddColumn(Ptr(DataColumn) col) { m_cols.Append(col); }
	__inline void AddRow(Ptr(DataRow) row) { m_rows.Append(row); }
	void LoadFile(ConstRef(CFilePath) path);
	void DrawTableView();
	void SetCursorPosEx();
	void Update(void);

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
	void _LoadFile(void);
	void StartEdit(void);
	bool CheckVScroll(void);
	bool CheckHScroll(void);

	CFilePath m_Path;
	DataRowVector m_rows;
	DataColumnVector m_cols;
	LCOORD m_CursorPos;
	LCOORD m_SelectionEnd;
	LCOORD m_ScrollPos;
	bool m_SelectionMode;
	Ptr(CConsoleControl) m_control;
};

