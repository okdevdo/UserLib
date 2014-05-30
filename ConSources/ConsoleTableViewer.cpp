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
#include "CON_PCH.H"
#include "ConsoleTableViewer.h"
#include "Console.h"
#include "ConsoleTextControl.h"
#include "DataBase.h"

static void __stdcall DataRow_DeleteFunc( ConstPointer data, Pointer context )
{
	CConsoleTableViewer::DataRow* p = CastAnyPtr(CConsoleTableViewer::DataRow, CastMutable(Pointer, data));

	p->release();
}

static void __stdcall DataColumn_DeleteFunc( ConstPointer data, Pointer context )
{
	CConsoleTableViewer::DataColumn* p = CastAnyPtr(CConsoleTableViewer::DataColumn, CastMutable(Pointer, data));

	p->release();
}

static void __stdcall DataItem_DeleteFunc( ConstPointer data, Pointer context )
{
	CConsoleTableViewer::DataItem* p = CastAnyPtr(CConsoleTableViewer::DataItem, CastMutable(Pointer, data));

	p->release();
}

CConsoleTableViewer::DataRow::~DataRow()
{
	m_items.Close();
	m_columns.Close();
}

CConsoleTableViewer::DataColumn::~DataColumn()
{
	if ( NotPtrCheck(m_control) )
		m_control->release();
}

TListCnt CConsoleTableViewer::DataColumnVector::get_width() const
{
	Iterator it = Begin();
	TListCnt result = 0;

	while ( it )
	{
		result += (*it)->get_width() + 1;
		++it;
	}
	return result;
}

Ptr(CConsoleTableViewer::DataColumn) CConsoleTableViewer::DataColumnVector::get_column(sdword start, Ref(sdword) pos) const
{
	sdword pos0 = start;
	sdword pos1 = pos0;
	Iterator it = Begin();

	while ( it )
	{
		Ptr(DataColumn) col = *it;

		pos0 += Cast(sdword, col->get_width()) + 1;
		if ( pos < pos0 )
		{
			pos = pos1;
			return col;
		}
		pos1 = pos0;
		++it;
	}
	return NULL;
}

void CConsoleTableViewer::DataColumnVector::draw_titles(sdword start, SMALL_RECT r, Ptr(CConsoleWindow) w)
{
	CConstPointer text;
	SMALL_RECT r0;
	sdword r0r;
	sdword r0l;
	bool bLast = false;
	bool bFirst = true;
	Iterator it = Begin();

	r0.Top = r.Top;
	r0.Bottom = r.Bottom;
	r0r = r.Left;
	r0l = r.Left;
	while ( it && (!bLast) )
	{
		Ptr(DataColumn) col = *it;

		if ( bFirst )
		{
			r0r += col->get_width() + 1;
			if ( (start + r.Left) < r0r )
			{
				text = col->get_title().GetString();
				if ( (start - r0l + r.Left) >= Cast(sdword, col->get_title().GetLength()) )
					text = _T(" ");
				else
					text += start - r0l + r.Left;
				r0.Left = r.Left;
				r0.Right = Cast(sword, r0r - start);
				if ( r0.Right >= r.Right )
				{
					r0.Right = r.Right;
					bLast = true;
				}
				w->DrawString(r0, text, w->GetColor());
				r0.Left = r0.Right;
				if ( r0.Right < r.Right )
				{
					++(r0.Right);
					w->DrawString(r0, _T(" "), w->GetColor());
					r0.Left = r0.Right;
				}
				bFirst = false;
			}
			r0l = r0r;
		}
		else
		{
			r0.Right += Cast(sword, col->get_width());
			if ( r0.Right >= r.Right )
			{
				r0.Right = r.Right;
				bLast = true;
			}
			text = col->get_title().GetString();
			w->DrawString(r0, text, w->GetColor());
			r0.Left = r0.Right;
			if ( r0.Right < r.Right )
			{
				++(r0.Right);
				w->DrawString(r0, _T(" "), w->GetColor());
				r0.Left = r0.Right;
			}
		}
		++it;
	}
	if ( r0.Right < r.Right )
	{
		r0.Right = r.Right;
		w->DrawString(r0, _T(" "), w->GetColor());
	}
}

TListCnt CConsoleTableViewer::DataRowVector::get_height() const
{
	Iterator it = Begin();
	TListCnt result = 0;

	while ( it )
	{
		result += (*it)->get_height();
		++it;
	}
	return result;
}

Ptr(CConsoleTableViewer::DataItem) CConsoleTableViewer::DataRowVector::get_item(LCOORD start, Ref(LCOORD) pos, Ref(Ptr(DataColumn)) col)
{
	Iterator it0 = Index(pos.Y - start.Y);

	if ( it0 )
	{
		Ptr(DataRow) row = *it0;
		sdword pos0 = pos.X;

		col = row->get_columns().get_column(start.X, pos0);

		if ( PtrCheck(col) )
			return NULL;
		pos.X = pos0;
		return row->get_item(col->get_col());
	}
	return NULL;
}

void CConsoleTableViewer::DataRowVector::draw_items(LCOORD start, SMALL_RECT r, sdword cpos, sdword spos, Ptr(CConsoleWindow) w)
{
	CConstPointer text;
	CStringBuffer tmp;
	SMALL_RECT r0;
	Iterator it0 = Index(start.Y);
	bool bHighLight = false;

	r0.Top = r.Top;
	r0.Bottom = r0.Top + 1;
	while ( it0 )
	{
		Ptr(DataRow) row = *it0;

		if ( cpos == spos )
			bHighLight = (cpos == (start.Y + r0.Top - 1));
		else if ( cpos < spos )
			bHighLight = (cpos <= (start.Y + r0.Top - 1)) && ((start.Y + r0.Top - 1) <= spos);
		else
			bHighLight = (spos <= (start.Y + r0.Top - 1)) && ((start.Y + r0.Top - 1) <= cpos);

		r0.Bottom = r0.Top + 1;
		if ( r0.Bottom > r.Bottom )
			break;

		bool bLast = false;
		bool bFirst = true;
		sdword r0r = r.Left;
		sdword r0l = r.Left;
		DataColumnVector::Iterator it = row->get_columns().Begin();

		while ( it && (!bLast) )
		{
			Ptr(DataColumn) col = *it;
			Ptr(DataItem) item = row->get_item(col->get_col());

			if ( bFirst )
			{
				r0r += Cast(sdword, col->get_width()) + 1;
				if ( (start.X + r.Left) < r0r )
				{
					text = item->get_data().GetString();
					tmp = w->GetAlignedText(text, col->get_width(), col->get_alignment());
					text = tmp.GetString();
					if ( (start.X - r0l + r.Left) >= Cast(sdword, tmp.GetLength()) )
						text = _T(" ");
					else
						text += start.X - r0l + r.Left;
					r0.Left = r.Left;
					r0.Right = Cast(sword, r0r - start.X);
					if ( r0.Right >= r.Right )
					{
						r0.Right = r.Right;
						bLast = true;
					}
					w->DrawString(r0, text, bHighLight?(w->GetHighLightColor()):(w->GetColor()));
					r0.Left = r0.Right;
					if ( r0.Right < r.Right )
					{
						++(r0.Right);
						w->DrawString(r0, _T(" "), bHighLight?(w->GetHighLightColor()):(w->GetColor()));
						r0.Left = r0.Right;
					}
					bFirst = false;
				}
				r0l = r0r;
			}
			else
			{
				r0.Right += Cast(sword, col->get_width());
				if ( r0.Right >= r.Right )
				{
					r0.Right = r.Right;
					bLast = true;
				}
				text = item->get_data().GetString();
				tmp = w->GetAlignedText(text, col->get_width(), col->get_alignment());
				w->DrawString(r0, tmp.GetString(), bHighLight?(w->GetHighLightColor()):(w->GetColor()));
				r0.Left = r0.Right;
				if ( r0.Right < r.Right )
				{
					++(r0.Right);
					w->DrawString(r0, _T(" "), bHighLight?(w->GetHighLightColor()):(w->GetColor()));
					r0.Left = r0.Right;
				}
			}
			++it;
		}
		if ( r0.Right < r.Right )
		{
			r0.Right = r.Right;
			w->DrawString(r0, _T(" "), bHighLight?(w->GetHighLightColor()):(w->GetColor()));
		}
		r0.Top = r0.Bottom;
		++it0;
	}
	if ( r0.Bottom <= r.Bottom )
	{
		r0.Left = r.Left;
		r0.Right = r.Right;
		r0.Bottom = r.Bottom;
		w->DrawString(r0, _T(" "), w->GetColor());
	}
}

CConsoleTableViewer::CConsoleTableViewer(CConstPointer name, CConsole* pConsole):
    CConsoleWindow(name, pConsole), m_Path(), m_rows(__FILE__LINE__ 16, 16), m_cols(__FILE__LINE__ 16, 16)
{
}

CConsoleTableViewer::CConsoleTableViewer(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole), m_Path(), m_rows(__FILE__LINE__ 16, 16), m_cols(__FILE__LINE__ 16, 16)
{
}

CConsoleTableViewer::CConsoleTableViewer(ConstRef(CFilePath) path, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleWindow(name, title, pConsole), m_Path(path), m_rows(__FILE__LINE__ 16, 16), m_cols(__FILE__LINE__ 16, 16)
{
}

CConsoleTableViewer::~CConsoleTableViewer(void)
{
	m_rows.Close();
	m_cols.Close();
}

void CConsoleTableViewer::StartEdit(void)
{
	LCOORD start;
	LCOORD pos;
	Ptr(DataColumn) col = NULL;

	start.X = m_ClientArea.Left - m_NonClientArea.Left;
	start.Y = m_ClientArea.Top - m_NonClientArea.Top;
	pos = m_CursorPos; 
	Ptr(DataItem) item = m_rows.get_item(start, pos, col);

	if ( PtrCheck(item) || PtrCheck(col) )
		return;
	if ( Ptr(CConsoleTextControl) ctrl = CastDynamicPtr(CConsoleTextControl, col->get_control()) )
	{
		COORD pos0;

		if ( (pos.X == 1) && (m_ScrollPos.X == 0) )
			pos0.X = 1;
		else
			pos0.X = Cast(sword, pos.X - m_ScrollPos.X + 1);
		pos0.Y = Cast(sword, pos.Y - m_ScrollPos.Y + 2);
		if ( !(ctrl->IsCreated()) )
		{
			COORD sz;

			sz.X = Cast(sword, col->get_width());
			sz.Y = 1;
			ctrl->Create(pos0, sz);
		}
		ctrl->SetText(item->get_data());
		ctrl->MoveWindow(pos0, true, true);
		ctrl->SetFocus(true);
		m_control = ctrl;
	}
}

void CConsoleTableViewer::Update(void)
{
	if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
	{
		m_ScrollBarHInfo.SetScrollBarInfo(m_cols.get_width(), m_ClientAreaSize.X);
		m_ScrollBarHInfo.Current = m_ScrollPos.X;
		DrawHorizontalScrollBar();
	}
	if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
	{
		m_ScrollBarVInfo.SetScrollBarInfo(m_rows.get_height(), m_ClientAreaSize.Y);
		m_ScrollBarVInfo.Current = m_ScrollPos.Y;
		DrawVerticalScrollBar();
	}
	DrawTableView();
	PostPaintEvent();
	SetCursorPosEx();
}

void CConsoleTableViewer::Initialize()
{
	m_CursorPos.X = 0;
	m_CursorPos.Y = 0;
	m_SelectionEnd.X = 0;
	m_SelectionEnd.Y = 0;
	m_ScrollPos.X = 0;
	m_ScrollPos.Y = 0;
	m_SelectionMode = false;
	m_control = NULL;

	m_rows.Close();
	m_cols.Close();

	m_rows.Open(__FILE__LINE__ 16, 16);
	m_cols.Open(__FILE__LINE__ 16, 16);
}

void CConsoleTableViewer::_LoadFile(void)
{
	CDBase dbfile;
	dword ix;

	try
	{
		dbfile.Open(m_Path);
		for ( ix = 0; ix < dbfile.GetFieldCnt(); ++ix )
		{
			CStringBuffer fname = dbfile.GetFieldName(ix);
			dword fnlen = fname.GetLength();
			byte ftyp;
			byte flen;
			byte fdec;

			dbfile.GetFieldInfo(ix, &ftyp, &flen, &fdec);

			DataColumn::DataType dtyp;
			CConsoleWindow::Alignment dalign;
			TListCnt dlen;
			TListCnt dmax;
			TListCnt ddec;

			switch ( ftyp )
			{
			case 'C':
				dtyp = DataColumn::DataTypeFixedString;
				dlen = (flen > 30)?30:flen;
				dmax = flen;
				dalign = CConsoleWindow::AlignmentLeft;
				ddec = 0;
				break;
			case 'M':
				dtyp = DataColumn::DataTypeVarString;
				dlen = 30;
				dmax = UINT_MAX;
				dalign = CConsoleWindow::AlignmentLeft;
				ddec = 0;
				break;
			case 'N':
				dtyp = DataColumn::DataTypeNumeric;
				dlen = flen;
				dmax = flen;
				dalign = CConsoleWindow::AlignmentRight;
				ddec = fdec;
				break;
			case 'F':
				dtyp = DataColumn::DataTypeFloat;
				dlen = flen;
				dmax = flen;
				dalign = CConsoleWindow::AlignmentRight;
				ddec = fdec;
				break;
			case 'I':
				dtyp = DataColumn::DataTypeIdentity;
				dlen = flen;
				dmax = flen;
				dalign = CConsoleWindow::AlignmentRight;
				ddec = 0;
				break;
			case 'L':
				dtyp = DataColumn::DataTypeLogical;
				dlen = flen;
				dmax = flen;
				dalign = CConsoleWindow::AlignmentLeft;
				ddec = 0;
				break;
			case 'D':
				dtyp = DataColumn::DataTypeDate;
				dlen = flen;
				dmax = flen;
				dalign = CConsoleWindow::AlignmentLeft;
				ddec = 0;
				break;
			case 'T':
				dtyp = DataColumn::DataTypeTime;
				dlen = flen;
				dmax = flen;
				dalign = CConsoleWindow::AlignmentLeft;
				ddec = 0;
				break;
			}

			Ptr(DataColumn) col = OK_NEW_OPERATOR DataColumn(ix, fname);

			col->set_datatype(dtyp);
			col->set_alignment(dalign);
			col->set_decimals(ddec);
			if ( dlen < fnlen )
			{
				dlen = fnlen;
				if ( dmax < dlen )
					dmax = dlen;
			}
			col->set_width(dlen);
			col->set_maxwidth(dmax);
			col->set_control(OK_NEW_OPERATOR CConsoleTextControl(fname, m_Console));

			m_cols.Append(col);
		}
		for ( ix = 0; ix < dbfile.GetRecordCnt(); ++ix )
		{
			dbfile.Read(ix);

			Ptr(DataRow) row = OK_NEW_OPERATOR DataRow(ix, 1, CStringBuffer(__FILE__LINE__ _T("")), m_cols);
			CStringBuffer strValue;
			int intValue;
			double numValue;
			DataColumnVector::Iterator it = m_cols.Begin();

			while ( it )
			{
				Ptr(DataColumn) col = *it;

				switch ( col->get_datatype() )
				{
				case DataColumn::DataTypeFixedString:
					strValue = dbfile.GetTextField(col->get_col());
					break;
				case DataColumn::DataTypeVarString:
					strValue = dbfile.GetTextField(col->get_col());
					break;
				case DataColumn::DataTypeNumeric:
					numValue = dbfile.GetNumericField(col->get_col());
					strValue.FormatString(__FILE__LINE__ _T("%.*lf"), col->get_decimals(), numValue);
					break;
				case DataColumn::DataTypeFloat:
					numValue = dbfile.GetNumericField(col->get_col());
					strValue.FormatString(__FILE__LINE__ _T("%.*lf"), col->get_decimals(), numValue);
					break;
				case DataColumn::DataTypeIdentity:
					numValue = dbfile.GetNumericField(col->get_col());
					strValue.FormatString(__FILE__LINE__ _T("%.*lf"), col->get_decimals(), numValue);
					break;
				case DataColumn::DataTypeLogical:
					intValue = dbfile.GetLogicalField(col->get_col());
					strValue.FormatString(__FILE__LINE__ _T("%ld"), intValue);
					break;
				case DataColumn::DataTypeDate:
					strValue = dbfile.GetDateField(col->get_col());
					break;
				case DataColumn::DataTypeTime:
					strValue = dbfile.GetTimeField(col->get_col());
					break;
				default:
					strValue.Clear();
					break;
				}

				Ptr(DataItem) item = OK_NEW_OPERATOR DataItem(ix, col->get_col(), strValue);

				row->add_item(item);
				++it;
			}
			m_rows.Append(row);
		}
		dbfile.Close();
	}
	catch ( CDBaseException* )
	{
		dbfile.Close();
		Initialize();
	}
}

void CConsoleTableViewer::LoadFile(ConstRef(CFilePath) path)
{
	Initialize();
	m_Path = path;
	_LoadFile();
	Update();
}

void CConsoleTableViewer::DrawTableView()
{
	SMALL_RECT rect;
	sword off = m_hasBorder?1:0;

	rect.Top = off;
	rect.Bottom = rect.Top + 1;
	rect.Left = off;
	rect.Right = m_ClientAreaSize.X + off;
	m_cols.draw_titles(m_ScrollPos.X, rect, this);
	rect.Top = rect.Bottom;
	rect.Bottom = rect.Top + m_ClientAreaSize.Y - 1;
	rect.Left = off;
	rect.Right = m_ClientAreaSize.X + off;
	m_rows.draw_items(m_ScrollPos, rect, m_CursorPos.Y, m_SelectionEnd.Y, this);
}

void CConsoleTableViewer::SetCursorPosEx()
{
	COORD pos;
	bool bHideCursor = false;

	pos.X = Cast(sword, m_CursorPos.X - m_ScrollPos.X) + m_ClientArea.Left;
	if ( (pos.X < m_ClientArea.Left) || (pos.X >= m_ClientArea.Right) )
		bHideCursor = true;
	pos.Y = Cast(sword, m_CursorPos.Y - m_ScrollPos.Y) + m_ClientArea.Top;
	if ( (pos.Y < m_ClientArea.Top) || (pos.Y >= m_ClientArea.Bottom) )
		bHideCursor = true;
	if ( bHideCursor )
		HideCursor();
	else
	{
		ShowCursor();
		SetCursorPos(pos);
	}
}

void CConsoleTableViewer::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	m_hasBorder = true;
	m_BorderStyle = doubleborderstyle;
	m_hasTitle = true;
	m_TitleStyle = windowtitlebarstyle;
	m_hasScrollbar = true;
	m_ScrollBarStyle = bothscrollbarstyle;
	if ( m_Path.get_Path().IsEmpty() )
		Initialize();
	else
	{
		Initialize();
		_LoadFile();
	}
	m_ScrollBarVInfo.SetScrollBarInfo(m_rows.get_height(), size.Y - 2);
	m_ScrollBarHInfo.SetScrollBarInfo(m_cols.get_width(), size.X - 2);
	CConsoleWindow::Create(pos, size);

	DrawTableView();
	SetCursorPosEx();
}

void CConsoleTableViewer::SetFocus(bool hasFocus)
{
	if ( hasFocus )
		SetCursorPosEx();
	else
		HideCursor();
}

void CConsoleTableViewer::Resize()
{
	if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		m_ScrollBarVInfo.SetScrollBarInfo(m_rows.get_height(), m_ClientAreaSize.Y);
	if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		m_ScrollBarHInfo.SetScrollBarInfo(m_cols.get_width(), m_ClientAreaSize.X);
	Scroll();
}

void CConsoleTableViewer::Scroll()
{
	if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		m_ScrollPos.Y = m_ScrollBarVInfo.Current;
	if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		m_ScrollPos.X = m_ScrollBarHInfo.Current;
	DrawTableView();
	SetCursorPosEx();
	m_control = NULL;
}

bool CConsoleTableViewer::CheckVScroll(void)
{
	if ( (m_CursorPos.Y < m_ScrollPos.Y) || (m_CursorPos.Y >= (m_ScrollPos.Y + m_ClientAreaSize.Y)) )
	{
		m_ScrollPos.Y = m_CursorPos.Y - (m_ClientAreaSize.Y / 2);
		if ( m_ScrollPos.Y < 0 )
			m_ScrollPos.Y = 0;
		if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		{
			m_ScrollBarVInfo.Current = m_ScrollPos.Y;
			DrawVerticalScrollBar();
		}
		return true;
	}
	return false;
}

bool CConsoleTableViewer::CheckHScroll(void)
{
	if ( (m_CursorPos.X < m_ScrollPos.X) || (m_CursorPos.X >= (m_ScrollPos.X + m_ClientAreaSize.X)) )
	{
		m_ScrollPos.X = m_CursorPos.X - (m_ClientAreaSize.X / 2);
		if ( m_ScrollPos.X < 0 )
			m_ScrollPos.X = 0;
		if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		{
			m_ScrollBarHInfo.Current = m_ScrollPos.X;
			DrawHorizontalScrollBar();
		}
		return true;
	}
	return false;
}

void CConsoleTableViewer::Undo()
{
	if ( NotPtrCheck(m_control) )
	{
		m_control->Undo();
		return;
	}
}

void CConsoleTableViewer::Redo()
{
	if ( NotPtrCheck(m_control) )
	{
		m_control->Redo();
		return;
	}
}

void CConsoleTableViewer::Cut()
{
	if ( NotPtrCheck(m_control) )
	{
		m_control->Cut();
		return;
	}
}

void CConsoleTableViewer::Copy()
{
	if ( NotPtrCheck(m_control) )
	{
		m_control->Copy();
		return;
	}
}

void CConsoleTableViewer::Paste()
{
	if ( NotPtrCheck(m_control) )
	{
		m_control->Paste();
		return;
	}
}

void CConsoleTableViewer::Delete()
{
	if ( NotPtrCheck(m_control) )
	{
		m_control->Delete();
		return;
	}
}

bool CConsoleTableViewer::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	if ( NotPtrCheck(m_control) )
	{
		switch ( virtualKeyCode )
		{
        CASE_KEY_ESCAPE:
			if ( (controlKeyState & KeyStateAllMask) == 0 )
			{
				m_control = NULL;
				PostPaintEvent();
				SetCursorPosEx();
			}
			break;
        CASE_KEY_RETURN:
			if ( (controlKeyState & KeyStateAllMask) == 0 )
			{
				m_control = NULL;
				PostPaintEvent();
				SetCursorPosEx();
			}
			break;
		CASE_KEY_F2:
			if ( (controlKeyState & KeyStateAllMask) == 0 )
			{
				m_control = NULL;
				PostPaintEvent();
				SetCursorPosEx();
			}
			break;
		default:
			return m_control->KeyDown(virtualKeyCode, controlKeyState);
		}
		return true;
	}
	switch ( virtualKeyCode )
	{
    CASE_KEY_HOME:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			m_CursorPos.X = 0;
			m_ScrollPos.X = 0;
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			CheckVScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0) )
		{
			m_CursorPos.X = 0;
			m_CursorPos.Y = 0;
			m_ScrollPos.X = 0;
			m_ScrollPos.Y = 0;
			m_SelectionEnd.X = m_CursorPos.X;
			m_SelectionEnd.Y = m_CursorPos.Y;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarVInfo.Current = m_ScrollPos.Y;
				DrawVerticalScrollBar();
			}
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.X = 0;
			m_ScrollPos.X = 0;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			CheckVScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.X = 0;
			m_CursorPos.Y = 0;
			m_ScrollPos.X = 0;
			m_ScrollPos.Y = 0;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarVInfo.Current = m_ScrollPos.Y;
				DrawVerticalScrollBar();
			}
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_END:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			m_CursorPos.X = m_cols.get_width();
			CheckHScroll();
			CheckVScroll();
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			m_CursorPos.Y = m_rows.get_height();
			m_ScrollPos.Y = m_rows.get_height() - (m_ClientAreaSize.Y / 2);
			m_CursorPos.X = 0;
			m_ScrollPos.X = 0;
			m_SelectionEnd.X = m_CursorPos.X;
			m_SelectionEnd.Y = m_CursorPos.Y;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarVInfo.Current = m_ScrollPos.Y;
				DrawVerticalScrollBar();
			}
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.X = m_cols.get_width();
			CheckHScroll();
			CheckVScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_rows.get_height();
			m_ScrollPos.Y = m_rows.get_height() - (m_ClientAreaSize.Y / 2);
			m_CursorPos.X = 0;
			m_ScrollPos.X = 0;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarVInfo.Current = m_ScrollPos.Y;
				DrawVerticalScrollBar();
			}
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_LEFT:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bUpdate = false;

			if ( m_CursorPos.X > 0 )
			{
				sdword pos = m_CursorPos.X;
				Ptr(DataColumn) col = m_cols.get_column(m_ClientArea.Left - m_NonClientArea.Left, pos);

				if ( PtrCheck(col) )
					--(m_CursorPos.X);
				else if ( pos == m_CursorPos.X )
				{
					if ( col->get_col() > 0 )
					{
						Ptr(DataColumn) col1 = m_cols.get_column(col->get_col() - 1);

						m_CursorPos.X -= col1->get_width() + 1;
					}
				}
				else
					m_CursorPos.X = pos;
				if ( m_CursorPos.X == 1 )
					m_CursorPos.X = 0;
			}
			if ( !m_SelectionMode )
			{
				if ( m_SelectionEnd.Y != m_CursorPos.Y )
					bUpdate = true;
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bUpdate )
			{
				DrawTableView();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			bool bUpdate = false;

			if ( m_CursorPos.X > 0 )
			{
				sdword pos = m_CursorPos.X;
				Ptr(DataColumn) col = m_cols.get_column(m_ClientArea.Left - m_NonClientArea.Left, pos);

				if ( PtrCheck(col) )
					--(m_CursorPos.X);
				else if ( pos == m_CursorPos.X )
				{
					if ( col->get_col() > 0 )
					{
						Ptr(DataColumn) col1 = m_cols.get_column(col->get_col() - 1);

						m_CursorPos.X -= col1->get_width() + 1;
					}
				}
				else
					m_CursorPos.X = pos;
				if ( m_CursorPos.X == 1 )
					m_CursorPos.X = 0;
			}
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bUpdate )
			{
				DrawTableView();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
		}
		break;
	CASE_KEY_RIGHT:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bUpdate = false;

			if ( m_CursorPos.X < Cast(sdword, m_cols.get_width()) )
			{
				sdword pos = m_CursorPos.X;
				Ptr(DataColumn) col = m_cols.get_column(m_ClientArea.Left - m_NonClientArea.Left, pos);

				if ( PtrCheck(col) )
					++(m_CursorPos.X);
				else
					m_CursorPos.X = pos + col->get_width() + 1;
				if ( CheckHScroll() )
					bUpdate = true;
				if ( CheckVScroll() )
					bUpdate = true;
			}
			if ( !m_SelectionMode )
			{
				if ( m_SelectionEnd.Y != m_CursorPos.Y )
					bUpdate = true;
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bUpdate )
			{
				DrawTableView();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			bool bUpdate = false;

			if ( m_CursorPos.X < Cast(sdword, m_cols.get_width()) )
			{
				sdword pos = m_CursorPos.X;
				Ptr(DataColumn) col = m_cols.get_column(m_ClientArea.Left - m_NonClientArea.Left, pos);

				if ( PtrCheck(col) )
					++(m_CursorPos.X);
				else
					m_CursorPos.X = pos + col->get_width() + 1;
			}
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bUpdate )
			{
				DrawTableView();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
		}
		break;
	CASE_KEY_UP:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			if ( m_CursorPos.Y > 0 )
				--(m_CursorPos.Y);
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			CheckHScroll();
			CheckVScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y;
			m_SelectionEnd.Y = m_CursorPos.Y;
			m_SelectionEnd.X = m_CursorPos.X;
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			if ( m_CursorPos.Y > 0 )
				--(m_CursorPos.Y);
			CheckHScroll();
			CheckVScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y;
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_DOWN:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			if ( m_CursorPos.Y < Cast(sdword, m_rows.get_height()) )
				++(m_CursorPos.Y);
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			CheckHScroll();
			CheckVScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			if ( m_CursorPos.Y > Cast(sdword,m_rows.get_height()) )
				m_CursorPos.Y = m_rows.get_height();
			m_SelectionEnd.Y = m_CursorPos.Y;
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			if ( m_CursorPos.Y < Cast(sdword, m_rows.get_height()) )
				++(m_CursorPos.Y);
			CheckHScroll();
			CheckVScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			if ( m_CursorPos.Y > Cast(sdword,m_rows.get_height()) )
				m_CursorPos.Y = m_rows.get_height();
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_PAGEDOWN:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			if ( m_CursorPos.Y < Cast(sdword, m_rows.get_height()) )
			{
				if ( m_CursorPos.Y != (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2)) )
					m_CursorPos.Y = (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2));
				m_CursorPos.Y += m_ClientAreaSize.Y - 2;
				if ( m_CursorPos.Y > Cast(sdword,m_rows.get_height()) )
					m_CursorPos.Y = m_rows.get_height();
			}
			CheckHScroll();
			CheckVScroll();
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			if ( m_CursorPos.Y > Cast(sdword, m_rows.get_height()) )
				m_CursorPos.Y = m_rows.get_height();
			m_SelectionEnd.Y = m_CursorPos.Y;
			m_SelectionEnd.X = m_CursorPos.X;
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			if ( m_CursorPos.Y < Cast(sdword, m_rows.get_height()) )
			{
				if ( m_CursorPos.Y != (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2)) )
					m_CursorPos.Y = (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2));
				m_CursorPos.Y += m_ClientAreaSize.Y - 2;
				if ( m_CursorPos.Y > Cast(sdword,m_rows.get_height()) )
					m_CursorPos.Y = m_rows.get_height();
			}
			CheckHScroll();
			CheckVScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			if ( m_CursorPos.Y > Cast(sdword,m_rows.get_height()) )
				m_CursorPos.Y = m_rows.get_height();
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_PAGEUP:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			if ( m_CursorPos.Y >= 0 )
			{
				if ( m_CursorPos.Y != (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2)) )
					m_CursorPos.Y = (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2));
				m_CursorPos.Y -= m_ClientAreaSize.Y - 2;
				if ( m_CursorPos.Y < 0 )
					m_CursorPos.Y = 0;
			}
			CheckVScroll();
			CheckHScroll();
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y;
			m_SelectionEnd.Y = m_CursorPos.Y;
			m_SelectionEnd.X = m_CursorPos.X;
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			if ( m_CursorPos.Y >= 0 )
			{
				if ( m_CursorPos.Y != (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2)) )
					m_CursorPos.Y = (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2));
				m_CursorPos.Y -= m_ClientAreaSize.Y - 2;
				if ( m_CursorPos.Y < 0 )
					m_CursorPos.Y = 0;
			}
			CheckVScroll();
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y;
			CheckHScroll();
			DrawTableView();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
    CASE_KEY_DELETE:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			Update();
		}
		break;
    CASE_KEY_BACKSPACE:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			Update();
		}
		break;
	CASE_KEY_TAB:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		break;
	CASE_KEY_RETURN:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		break;
	CASE_KEY_ESCAPE:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		break;
	case _T('T'):
		if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0) )
			m_SelectionMode = !m_SelectionMode;
		break;
    CASE_KEY_F2:
		StartEdit();
		break;
	default:
		break;
	}
	return true;
}

bool CConsoleTableViewer::KeyPress(TCHAR key, DWORD controlKeyState)
{
	if ( NotPtrCheck(m_control) )
		return m_control->KeyPress(key, controlKeyState);
	return true;
}

bool CConsoleTableViewer::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	if ( NotPtrCheck(m_control) )
		return m_control->KeyUp(virtualKeyCode, controlKeyState);
	return true;
}

bool CConsoleTableViewer::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	bool bUpdate = false;

	if ( NotPtrCheck(m_control) )
	{
		if ( m_control->IsPosInClientRect(mousePos) )
			return m_control->LeftMouseDown(mousePos, controlKeyState);
		m_control = NULL;
		bUpdate = true;
	}
	if ( (controlKeyState & KeyStateAllMask) == 0 )
	{
		sdword value;

		value = m_ScrollPos.X + mousePos.X - m_ClientArea.Left;
		m_CursorPos.X = (value < 0)?0:value;
		value = m_ScrollPos.Y + mousePos.Y - m_ClientArea.Top;
		m_CursorPos.Y = (value < 0)?0:value;
		m_SelectionEnd.X = m_CursorPos.X;
		m_SelectionEnd.Y = m_CursorPos.Y;
		DrawTableView();
		PostPaintEvent();
		SetCursorPosEx();
		return true;
	}
	if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0) && ((controlKeyState & KeyStateShiftMask) != 0) )
	{
		sdword value;

		value = m_ScrollPos.X + mousePos.X - m_ClientArea.Left;
		m_SelectionEnd.X = (value < 0)?0:value;
		value = m_ScrollPos.Y + mousePos.Y - m_ClientArea.Top;
		m_SelectionEnd.Y = (value < 0)?0:value;
		DrawTableView();
		PostPaintEvent();
		SetCursorPosEx();
		return true;
	}
	if ( bUpdate )
	{
		PostPaintEvent();
		SetCursorPosEx();
	}
	return true;
}

bool CConsoleTableViewer::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableViewer::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) == 0 )
		StartEdit();
	return true;
}

bool CConsoleTableViewer::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTableViewer::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) == 0 )
	{
		sdword value;

		value = m_ScrollPos.X + mousePos.X - m_ClientArea.Left;
		if ( value < 0 )
		{
			m_SelectionEnd.X = 0;
			m_ScrollPos.X = 0;
		}
		else if ( value <= m_ScrollPos.X )
		{
			if ( m_ScrollPos.X > 0 )
				--(m_ScrollPos.X);
			m_SelectionEnd.X = m_ScrollPos.X;
		}
		else if ( value >= (m_ScrollPos.X + m_ClientAreaSize.X - 1) )
		{
			if ( m_ScrollPos.X < (Cast(sdword,m_cols.get_width()) - (m_ClientAreaSize.X / 2)) )
				++(m_ScrollPos.X);
			m_SelectionEnd.X = m_ScrollPos.X + m_ClientAreaSize.X - 1;
		}
		else
			m_SelectionEnd.X = value;
		value = m_ScrollPos.Y + mousePos.Y - m_ClientArea.Top;
		if ( value < 0 )
		{
			m_SelectionEnd.Y = 0;
			m_ScrollPos.Y = 0;
		}
		else if ( value <= m_ScrollPos.Y )
		{
			if ( m_ScrollPos.Y > 0 )
				--(m_ScrollPos.Y);
			m_SelectionEnd.Y = m_ScrollPos.Y;
		}
		else if ( value >= (m_ScrollPos.Y + m_ClientAreaSize.Y - 1) )
		{
			if ( m_ScrollPos.Y < (Cast(sdword,m_rows.get_height()) - (m_ClientAreaSize.Y / 2)) )
				++(m_ScrollPos.Y);
			m_SelectionEnd.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
		}
		else
			m_SelectionEnd.Y = value;
		Update();
		return true;
	}
	return true;
}

bool CConsoleTableViewer::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

