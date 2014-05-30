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
#include "stdafx.h"
#include "GridView.h"
#include "TextEditor.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"

#define SHIFTED 0x8000

CGridViewEmptyCell::CGridViewEmptyCell(CGridView* pGridView):
    m_gridView(pGridView),
	m_selected(false)
{
	::SetRectEmpty(&m_borderRect);
}

CGridViewEmptyCell::~CGridViewEmptyCell(void)
{
}

void CGridViewEmptyCell::set_Selected(bool selected)
{
	if ( m_selected != selected )
	{
		m_selected = selected;
		if ( m_gridView )
		{
			m_gridView->BeginUpdate();
			for ( dword ix = 0; ix < m_gridView->get_RowCount(); ++ix )
				m_gridView->get_Row(ix)->set_Selected(selected);
			for ( dword ix = 0; ix < m_gridView->get_ColumnCount(); ++ix )
				m_gridView->get_Column(ix)->set_Selected(selected);
			m_gridView->EndUpdate(FALSE);
		}
	}
}

CGridViewEmptyCell* CGridViewEmptyCell::HitTest(POINT pt)
{
	if ( !m_gridView )
		return NULL;

	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect);

	if ( (pt.y >= borderRect.top) && (pt.y < borderRect.bottom) && ((pt.x < borderRect.left) || (pt.x > borderRect.right)) )
		return NULL;
	if ( ::PtInRect(&borderRect, pt) )
		return this;
	return NULL;
}

void CGridViewEmptyCell::OnAdjustRects(INT right, INT bottom)
{
	m_borderRect.left = 0;
	m_borderRect.right = right;
	m_borderRect.top = 0;
	m_borderRect.bottom = bottom;
}

void CGridViewEmptyCell::OnPaint(Gdiplus::Graphics* graphics)
{
	if ( !m_gridView )
		return;

	Gdiplus::RectF borderRectF;
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect);

	Convert2RectF(&borderRectF, &borderRect);
	if ( m_selected )
	{
		Gdiplus::Brush* brush = m_gridView->get_Brush(_T(".GridEmptyCell.BackgroundColor.Selected"), _T("GridView"), Gdiplus::Color::Blue);

		graphics->FillRectangle(brush, borderRectF);
	}
	else
	{
		Gdiplus::Brush* brush = m_gridView->get_Brush(_T(".GridEmptyCell.BackgroundColor.Normal"), _T("GridView"), Gdiplus::Color::LightGray);

		graphics->FillRectangle(brush, borderRectF);
	}

	Gdiplus::Pen grayPen(Gdiplus::Color::Gray);

	graphics->DrawLine(&grayPen, borderRectF.X + borderRectF.Width, borderRectF.Y, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
	graphics->DrawLine(&grayPen, borderRectF.X, borderRectF.Y + borderRectF.Height, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
}

CGridViewCell::CGridViewCell(CGridView* pGridView, CGridViewRow* pGridViewRow, CGridViewColumn* pGridViewColumn, LPTSTR text, int lench):
	m_gridView(pGridView),
	m_gridViewRow(pGridViewRow),
	m_gridViewColumn(pGridViewColumn),
	m_text(__FILE__LINE__ text, lench),
	m_selected(false),
	m_focused(false)
{
	::SetRectEmpty(&m_textRect);
	::SetRectEmpty(&m_borderRect);
	if ( m_gridViewRow )
		m_gridViewRow->set_Cell(m_gridViewRow->get_CellCount(), this);
}

CGridViewCell::CGridViewCell(CGridView* pGridView, CGridViewRow* pGridViewRow, CGridViewColumn* pGridViewColumn, ConstRef(CStringBuffer) text):
	m_gridView(pGridView),
	m_gridViewRow(pGridViewRow),
	m_gridViewColumn(pGridViewColumn),
	m_text(text),
	m_selected(false),
	m_focused(false)
{
	::SetRectEmpty(&m_textRect);
	::SetRectEmpty(&m_borderRect);
	if ( m_gridViewRow )
		m_gridViewRow->set_Cell(m_gridViewRow->get_CellCount(), this);
}

CGridViewCell::~CGridViewCell(void)
{
}

void CGridViewCell::set_Text(ConstRef(CStringBuffer) text)
{
	m_text = text;
	if ( m_gridView )
		m_gridView->Update(TRUE);
}

void CGridViewCell::set_Text(LPCTSTR pText, int lench)
{
	m_text.SetString(__FILE__LINE__ pText, lench);
	if ( m_gridView )
		m_gridView->Update(TRUE);
}

void CGridViewCell::get_EditorRect(LPRECT rect)
{
	::SetRectEmpty(rect);
	if ( !m_gridView )
		return;

	RECT clientRect;
	RECT r;
	int xPos = m_gridView->get_HScrollOffset();
	int yPos = m_gridView->get_VScrollOffset();

	m_gridView->GetClientRect(&clientRect);
	::CopyRect(&r, &m_borderRect); ::OffsetRect(&r, -xPos, -yPos);
	::IntersectRect(rect, &r, &clientRect);
}

void CGridViewCell::set_Selected(bool selected)
{
	if ( m_selected != selected )
	{
		m_selected = selected;
		if ( m_gridView )
		{
			if ( m_selected )
				m_gridView->set_SelCell(m_gridView->get_SelCellCount(), this);
			else
			{
				int ix = m_gridView->inx_SelCell(this);

				if ( ix > -1 )
					m_gridView->set_SelCell(ix, NULL);
			}
			m_gridView->Update(FALSE);
		}
	}
}

void CGridViewCell::set_Focused(bool focused)
{
	if ( m_focused != focused )
	{
		m_focused = focused;
		if ( m_gridView )
		{
			m_gridView->Update(FALSE);
			if ( m_focused )
				m_gridView->SendNotifyMessage(NM_GRIDVIEWCELLFOCUSED, (LPARAM)this);
			else
				m_gridView->SendNotifyMessage(NM_GRIDVIEWCELLFOCUSLOST, (LPARAM)this);
		}
	}
}

CGridViewCell* CGridViewCell::HitTest(POINT pt, INT xPos, INT yPos, INT cBottom)
{
	if ( !m_gridView )
		return NULL;

	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); ::OffsetRect(&borderRect, -xPos, -yPos);

	if ( borderRect.bottom < 0 )
		return NULL;
	if ( borderRect.top >= cBottom )
		return NULL;
	if ( (pt.y >= borderRect.top) && (pt.y < borderRect.bottom) && ((pt.x < borderRect.left) || (pt.x > borderRect.right)) )
		return NULL;
	if ( ::PtInRect(&borderRect, pt) )
		return this;
	return NULL;
}

void CGridViewCell::OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth, LPINT maxHeight)
{
	if ( !m_gridView )
		return;

	LPCTSTR suffix = m_selected?_T(".GridCell.Font.Selected"):_T(".GridCell.Font.Normal");
	Gdiplus::Font* pFont = m_gridView->get_Font(suffix, _T("GridView"));
	Gdiplus::PointF pt; pt.X = Cast(Gdiplus::REAL, pRect->left); pt.Y = Cast(Gdiplus::REAL, pRect->top);
	Gdiplus::RectF textRectF;

	if ( m_text.IsEmpty() )
	{
		textRectF.X = pt.X; textRectF.Y = pt.Y; textRectF.Height = pFont->GetHeight(graphics); textRectF.Width = 10;
	}
	else
	{
		graphics->MeasureString(m_text.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &textRectF);
		textRectF.Width += 10;
	}
	Convert2Rect(pRect, &textRectF);

	::CopyRect(&m_textRect, pRect);
	::CopyRect(&m_borderRect, &m_textRect);
	m_borderRect.right += 4; m_borderRect.bottom += 4;
	m_textRect.left += 2; m_textRect.right += 2; m_textRect.top += 2; m_textRect.bottom += 2; 
	if ( m_borderRect.right > *maxWidth )
		*maxWidth = m_borderRect.right;
	if ( m_borderRect.bottom > *maxHeight )
		*maxHeight = m_borderRect.bottom;
	pRect->top = pRect->bottom + 4;
	pRect->bottom = 0;
}

void CGridViewCell::OnAdjustRects(INT left, INT right, INT top, INT bottom)
{
	m_borderRect.left = left;
	m_borderRect.right = right;
	m_borderRect.top = top;
	m_borderRect.bottom = bottom;
	m_textRect.left = m_borderRect.left + 2;
	m_textRect.right = m_borderRect.right - 2;
	m_textRect.top = m_borderRect.top + 2;
	m_textRect.bottom = m_borderRect.bottom - 2;
}

void CGridViewCell::OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom)
{
	if ( !m_gridView )
		return;

	RECT textRect; ::CopyRect(&textRect, &m_textRect); ::OffsetRect(&textRect, -xPos, -yPos);
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); ::OffsetRect(&borderRect, -xPos, -yPos);
	
	if ( borderRect.bottom < 0 )
		return;
	if ( borderRect.top >= cBottom )
		return;

	Gdiplus::Font* pFont;
	Gdiplus::Brush* backgroundbrush;
	Gdiplus::Brush* foregroundbrush;
	Gdiplus::RectF textRectF;
	Gdiplus::RectF borderRectF;
	LPCTSTR suffix;

	suffix = m_selected?_T(".GridCell.Font.Selected"):_T(".GridCell.Font.Normal");
	pFont = m_gridView->get_Font(suffix, _T("GridView"));
	suffix = m_selected?_T(".GridCell.BackgroundColor.Selected"):_T(".GridCell.BackgroundColor.Normal");
	backgroundbrush = m_gridView->get_Brush(suffix, _T("GridView"), m_selected?Gdiplus::Color::Blue:Gdiplus::Color::White);
	suffix = m_selected?_T(".GridCell.ForegroundColor.Selected"):_T(".GridCell.ForegroundColor.Normal");
	foregroundbrush = m_gridView->get_Brush(suffix, _T("GridView"), m_selected?Gdiplus::Color::White:Gdiplus::Color::Black);

	Convert2RectF(&borderRectF, &borderRect);
	borderRectF.Y += 1; borderRectF.Height -= 1;
	borderRectF.X += 1; borderRectF.Width -= 1;
	Convert2RectF(&textRectF, &textRect);
	graphics->FillRectangle(backgroundbrush, borderRectF);
	if ( !(m_text.IsEmpty()) )
		graphics->DrawString(m_text.GetString(), -1, pFont, textRectF, m_gridView->get_StringFormat(), foregroundbrush);

	Gdiplus::Pen grayPen(Gdiplus::Color::Gray);

	graphics->DrawLine(&grayPen, borderRectF.X + borderRectF.Width, borderRectF.Y, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
	graphics->DrawLine(&grayPen, borderRectF.X, borderRectF.Y + borderRectF.Height, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);

	if ( m_focused )
	{
		borderRectF.Y += 0; borderRectF.Height -= 1;
		borderRectF.X += 0; borderRectF.Width -= 1;
		grayPen.SetDashStyle(Gdiplus::DashStyleDot);

		graphics->DrawRectangle(&grayPen, borderRectF);
	}
}

CGridViewColumn::CGridViewColumn(CGridView* pGridView, LPCTSTR text, int lench):
	m_gridView(pGridView),
	m_text(__FILE__LINE__ text, lench),
	m_selected(false),
	m_editable(false),
	m_editor(NULL)
{
	::SetRectEmpty(&m_textRect);
	::SetRectEmpty(&m_borderRect);
	if ( m_gridView )
		m_gridView->set_Column(m_gridView->get_ColumnCount(), this);
}

CGridViewColumn::CGridViewColumn(CGridView* pGridView, ConstRef(CStringBuffer) text):
	m_gridView(pGridView),
	m_text(text),
	m_selected(false),
	m_editable(false),
	m_editor(NULL)
{
	::SetRectEmpty(&m_textRect);
	::SetRectEmpty(&m_borderRect);
	if ( m_gridView )
		m_gridView->set_Column(m_gridView->get_ColumnCount(), this);
}

CGridViewColumn::~CGridViewColumn(void)
{
}

void CGridViewColumn::set_Text(ConstRef(CStringBuffer) text)
{
	m_text = text;
	if ( m_gridView )
		m_gridView->Update(TRUE);
}

void CGridViewColumn::set_Text(LPCTSTR pText, int lench)
{
	m_text.SetString(__FILE__LINE__ pText, lench);
	if ( m_gridView )
		m_gridView->Update(TRUE);
}

void CGridViewColumn::set_Selected(bool selected)
{
	bool delta = (m_selected != selected);

	m_selected = selected;
	if ( m_gridView )
	{
		if ( delta )
		{
			if ( m_selected )
				m_gridView->set_SelColumn(m_gridView->get_SelColumnCount(), this);
			else
			{
				int ix = m_gridView->inx_SelColumn(this);

				if ( ix > -1 )
					m_gridView->set_SelColumn(ix, NULL);
			}
		}

		int index = m_gridView->inx_Column(this);

		m_gridView->BeginUpdate();
		if ( index > -1 )
		{
			for ( dword ix = 0; ix < m_gridView->get_RowCount(); ++ix )
			{
				CGridViewRow* row = m_gridView->get_Row(ix);

				if ( Cast(dword, index) < row->get_CellCount() )
				{
					CGridViewCell* cell = row->get_Cell(index);

					if ( cell->is_Selected() != m_selected )
						cell->set_Selected(m_selected);
				}
			}
		}
		m_gridView->EndUpdate(FALSE);
	}
}

CGridViewColumn* CGridViewColumn::HitTest(POINT pt)
{
	if ( !m_gridView )
		return NULL;

	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect);

	if ( (pt.y >= borderRect.top) && (pt.y < borderRect.bottom) && ((pt.x < borderRect.left) || (pt.x > borderRect.right)) )
		return NULL;
	if ( ::PtInRect(&borderRect, pt) )
		return this;
	return NULL;
}

void CGridViewColumn::OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth, LPINT maxHeight)
{
	if ( !m_gridView )
		return;

	LPCTSTR suffix = m_selected?_T(".GridColumn.Font.Selected"):_T(".GridColumn.Font.Normal");
	Gdiplus::Font* pFont = m_gridView->get_Font(suffix, _T("GridView"));
	Gdiplus::PointF pt; pt.X = Cast(Gdiplus::REAL, pRect->left); pt.Y = Cast(Gdiplus::REAL, pRect->top);
	Gdiplus::RectF textRectF;

	if ( m_text.IsEmpty() )
	{
		textRectF.X = pt.X; textRectF.Y = pt.Y; textRectF.Height = pFont->GetHeight(graphics); textRectF.Width = 10;
	}
	else
	{
		graphics->MeasureString(m_text.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &textRectF);
		textRectF.Width += 10;
	}
	Convert2Rect(pRect, &textRectF);

	CopyRect(&m_textRect, pRect);
	CopyRect(&m_borderRect, &m_textRect);
	m_borderRect.right += 4; m_borderRect.bottom += 4;
	m_textRect.left += 2; m_textRect.right += 2; m_textRect.top += 2; m_textRect.bottom += 2; 
	if ( m_borderRect.right > *maxWidth )
		*maxWidth = m_borderRect.right;
	if ( m_borderRect.bottom > *maxHeight )
		*maxHeight = m_borderRect.bottom;
	pRect->top = pRect->bottom + 4;
	pRect->bottom = 0;
}

void CGridViewColumn::OnAdjustRects(INT left, INT right, INT maxBottom)
{
	m_borderRect.top = 0;
	m_borderRect.left = left;
	m_borderRect.bottom = maxBottom;
	m_borderRect.right = right;
	m_textRect.left = left + 2;
	m_textRect.right = right - 2;
	m_textRect.top = m_borderRect.top + 2;
	m_textRect.bottom = m_borderRect.bottom - 2;
}

void CGridViewColumn::OnPaint(Gdiplus::Graphics* graphics)
{
	if ( !m_gridView )
		return;

	RECT textRect; ::CopyRect(&textRect, &m_textRect);
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect);
	
	Gdiplus::Font* pFont;
	Gdiplus::Brush* backgroundbrush;
	Gdiplus::Brush* foregroundbrush;
	Gdiplus::RectF textRectF;
	Gdiplus::RectF borderRectF;
	LPCTSTR suffix;

	suffix = m_selected?_T(".GridColumn.Font.Selected"):_T(".GridColumn.Font.Normal");
	pFont = m_gridView->get_Font(suffix, _T("GridView"));
	suffix = m_selected?_T(".GridColumn.BackgroundColor.Selected"):_T(".GridColumn.BackgroundColor.Normal");
	backgroundbrush = m_gridView->get_Brush(suffix, _T("GridView"), m_selected?Gdiplus::Color::Blue:Gdiplus::Color::LightGray);
	suffix = m_selected?_T(".GridColumn.ForegroundColor.Selected"):_T(".GridColumn.ForegroundColor.Normal");
	foregroundbrush = m_gridView->get_Brush(suffix, _T("GridView"), m_selected?Gdiplus::Color::White:Gdiplus::Color::Black);

	Convert2RectF(&borderRectF, &borderRect);
	borderRectF.X += 1; borderRectF.Width -= 1;
	Convert2RectF(&textRectF, &textRect);

	graphics->FillRectangle(backgroundbrush, borderRectF);
	if ( !(m_text.IsEmpty()) )
		graphics->DrawString(m_text.GetString(), -1, pFont, textRectF, m_gridView->get_StringFormat(), foregroundbrush);

	Gdiplus::Pen grayPen(Gdiplus::Color::Gray);

	graphics->DrawLine(&grayPen, borderRectF.X + borderRectF.Width, borderRectF.Y, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
	graphics->DrawLine(&grayPen, borderRectF.X, borderRectF.Y + borderRectF.Height, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
}

static void __stdcall TDeleteFunc_CGridViewCells( ConstPointer data, Pointer context )
{
	Ptr(CGridViewCell) p = CastAnyPtr(CGridViewCell, CastMutable(Pointer, data));

	delete p;
}

CGridViewRow::CGridViewRow(CGridView* pGridView, LPCTSTR text, int lench):
	m_gridView(pGridView),
	m_text(__FILE__LINE__ text, lench),
	m_selected(false),
	m_cells(__FILE__LINE__ 32, 64)
{
	::SetRectEmpty(&m_textRect);
	::SetRectEmpty(&m_borderRect);
	if ( m_gridView )
		m_gridView->set_Row(m_gridView->get_RowCount(), this);
}

CGridViewRow::CGridViewRow(CGridView* pGridView, ConstRef(CStringBuffer) text):
	m_gridView(pGridView),
	m_text(text),
	m_selected(false),
	m_cells(__FILE__LINE__ 32, 64)
{
	::SetRectEmpty(&m_textRect);
	::SetRectEmpty(&m_borderRect);
	if ( m_gridView )
		m_gridView->set_Row(m_gridView->get_RowCount(), this);
}

CGridViewRow::~CGridViewRow(void)
{
}

void CGridViewRow::set_Text(ConstRef(CStringBuffer) text)
{
	m_text = text;
	if ( m_gridView )
		m_gridView->Update(TRUE);
}

void CGridViewRow::set_Text(LPCTSTR pText, int lench)
{
	m_text.SetString(__FILE__LINE__ pText, lench);
	if ( m_gridView )
		m_gridView->Update(TRUE);
}

CGridViewCell* CGridViewRow::get_Cell(LPCTSTR name)
{
	if ( (!m_gridView) || (!name) )
		return NULL;

	CGridViewColumn* column = m_gridView->get_Column(name);
		
	if ( !column )
		return NULL;

	int colIndex = m_gridView->inx_Column(column);

	if ( (colIndex < 0) || (Cast(dword, colIndex) >= m_cells.Count()) )
		return NULL;

	return get_Cell(colIndex);
}

void CGridViewRow::set_Cell(dword ix, CGridViewCell* cell)
{
	if ( !cell )
	{
		if ( ix >= m_cells.Count() )
			return;

		CGridViewCell* cell1 = get_Cell(ix);

		if ( cell1->is_Selected() )
			cell1->set_Selected(false);
		m_cells.Remove(m_cells.Index(ix));
	}
	else if ( ix >= m_cells.Count() )
		m_cells.Append(cell);
	else
	{
		if ( ix >= m_cells.Count() )
			return;

		CGridViewCell* cell1 = get_Cell(ix);

		if ( cell1->is_Selected() )
			cell1->set_Selected(false);
		delete cell1;
		m_cells.SetData(m_cells.Index(ix), cell);
	}
	if ( m_gridView )
		m_gridView->Update(TRUE);
}

void CGridViewRow::set_CellText(LPCTSTR name, LPCTSTR text, int lenCh)
{
	CGridViewCell* cell = get_Cell(name);

	if ( !cell )
		return;

	cell->set_Text(text, lenCh);
}

void CGridViewRow::set_CellText(LPCTSTR name, ConstRef(CStringBuffer) text)
{
	CGridViewCell* cell = get_Cell(name);

	if ( !cell )
		return;

	cell->set_Text(text);
}

int CGridViewRow::inx_Cell(CGridViewCell* cell)
{
	CGridViewCells::Iterator it = m_cells.Begin();
	int ix = 0;

	while ( it )
	{
		if ( (*it) == cell )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

void CGridViewRow::set_Selected(bool selected)
{
	bool delta = (m_selected != selected);

	m_selected = selected;
	if ( m_gridView )
	{
		if ( delta )
		{
			if ( m_selected )
				m_gridView->set_SelRow(m_gridView->get_SelRowCount(), this);
			else
			{
				int ix = m_gridView->inx_SelRow(this);

				if ( ix > -1 )
					m_gridView->set_SelRow(ix, NULL);
			}
		}
		m_gridView->BeginUpdate();

		CGridViewCells::Iterator it = m_cells.Begin();

		while ( it )
		{
			CGridViewCell* cell = *it;

			if ( cell->is_Selected() != m_selected )
				cell->set_Selected(m_selected);
			++it;
		}
		m_gridView->EndUpdate(FALSE);
	}
}

CGridViewRow* CGridViewRow::HitTest(POINT pt, INT xPos, INT yPos, INT cBottom)
{
	if ( !m_gridView )
		return NULL;

	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); ::OffsetRect(&borderRect, -xPos, -yPos);

	if ( borderRect.bottom < 0 )
		return NULL;
	if ( borderRect.top >= cBottom )
		return NULL;
	if ( (pt.y >= borderRect.top) && (pt.y < borderRect.bottom) && ((pt.x < borderRect.left) || (pt.x > borderRect.right)) )
		return NULL;
	if ( ::PtInRect(&borderRect, pt) )
		return this;
	return NULL;
}

void CGridViewRow::OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth, LPINT maxHeight)
{
	if ( !m_gridView )
		return;

	LPCTSTR suffix = m_selected?_T(".GridRow.Font.Selected"):_T(".GridRow.Font.Normal");
	Gdiplus::Font* pFont = m_gridView->get_Font(suffix, _T("GridView"));
	Gdiplus::PointF pt; pt.X = Cast(Gdiplus::REAL, pRect->left); pt.Y = Cast(Gdiplus::REAL, pRect->top);
	Gdiplus::RectF textRectF;

	if ( m_text.IsEmpty() )
	{
		textRectF.X = pt.X; textRectF.Y = pt.Y; textRectF.Height = pFont->GetHeight(graphics); textRectF.Width = 10;
	}
	else
	{
		graphics->MeasureString(m_text.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &textRectF);
		textRectF.Width += 10;
	}
	Convert2Rect(pRect, &textRectF);

	CopyRect(&m_textRect, pRect);
	CopyRect(&m_borderRect, &m_textRect);
	m_borderRect.right += 4; m_borderRect.bottom += 4;
	m_textRect.left += 2; m_textRect.right += 2; m_textRect.top += 2; m_textRect.bottom += 2; 
	if ( m_borderRect.right > *maxWidth )
		*maxWidth = m_borderRect.right;
	if ( m_borderRect.bottom > *maxHeight )
		*maxHeight = m_borderRect.bottom;
	pRect->top = pRect->bottom + 4;
	pRect->bottom = 0;
}

void CGridViewRow::OnAdjustRects(INT top, INT bottom, INT maxRight)
{
	m_borderRect.top = top;
	m_borderRect.left = 0;
	m_borderRect.bottom = bottom;
	m_borderRect.right = maxRight;
	m_textRect.left = m_borderRect.left + 2;
	m_textRect.right = m_borderRect.right - 2;
	m_textRect.top = m_borderRect.top + 2;
	m_textRect.bottom = m_borderRect.bottom - 2;
}

void CGridViewRow::OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom)
{
	if ( !m_gridView )
		return;

	RECT textRect; ::CopyRect(&textRect, &m_textRect); ::OffsetRect(&textRect, -xPos, -yPos);
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); ::OffsetRect(&borderRect, -xPos, -yPos);
	
	if ( borderRect.bottom < 0 )
		return;
	if ( borderRect.top >= cBottom )
		return;

	Gdiplus::Font* pFont;
	Gdiplus::Brush* backgroundbrush;
	Gdiplus::Brush* foregroundbrush;
	Gdiplus::RectF textRectF;
	Gdiplus::RectF borderRectF;
	LPCTSTR suffix;

	suffix = m_selected?_T(".GridRow.Font.Selected"):_T(".GridRow.Font.Normal");
	pFont = m_gridView->get_Font(suffix, _T("GridView"));
	suffix = m_selected?_T(".GridRow.BackgroundColor.Selected"):_T(".GridRow.BackgroundColor.Normal");
	backgroundbrush = m_gridView->get_Brush(suffix, _T("GridView"), m_selected?Gdiplus::Color::Blue:Gdiplus::Color::LightGray);
	suffix = m_selected?_T(".GridRow.ForegroundColor.Selected"):_T(".GridRow.ForegroundColor.Normal");
	foregroundbrush = m_gridView->get_Brush(suffix, _T("GridView"), m_selected?Gdiplus::Color::White:Gdiplus::Color::Black);

	Convert2RectF(&borderRectF, &borderRect);
	borderRectF.Y += 1; borderRectF.Height -= 1;
	Convert2RectF(&textRectF, &textRect);

	graphics->FillRectangle(backgroundbrush, borderRectF);
	if ( !(m_text.IsEmpty()) )
		graphics->DrawString(m_text.GetString(), -1, pFont, textRectF, m_gridView->get_StringFormat(), foregroundbrush);

	Gdiplus::Pen grayPen(Gdiplus::Color::Gray);

	graphics->DrawLine(&grayPen, borderRectF.X + borderRectF.Width, borderRectF.Y, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
	graphics->DrawLine(&grayPen, borderRectF.X, borderRectF.Y + borderRectF.Height, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
}

BEGIN_MESSAGE_MAP(CControl, CGridView)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MESSAGE(WM_ABOUTHSCROLL,OnAboutHScroll)
	ON_WM_MESSAGE(WM_ABOUTVSCROLL,OnAboutVScroll)
	ON_WM_COMMMAND(IDM_CUT,OnEditCutCommand)
	ON_WM_COMMMAND(IDM_COPY,OnEditCopyCommand)
	ON_WM_COMMMAND(IDM_DELETE,OnEditDeleteCommand)
	ON_WM_COMMMAND(IDM_PASTE,OnEditPasteCommand)
	ON_WM_COMMMAND(IDM_UNDO,OnEditUndoCommand)
	ON_WM_COMMMAND(IDM_REDO,OnEditRedoCommand)
	ON_WM_NOTIFY(NM_EDITORRETURN,OnEditorReturn)
	ON_WM_NOTIFY(NM_EDITORTAB,OnEditorTab)
	ON_WM_NOTIFY(NM_EDITORFUNCKEY,OnEditorFuncKey)
	ON_WM_NOTIFY(NM_EDITORLOSTFOCUS,OnEditorLostFocus)
	ON_WM_NOTIFY(NM_EDITORESCAPE,OnEditorEscape)
	ON_WM_NOTIFY(NM_EDITORCURSORUP,OnEditorCursorUp)
	ON_WM_NOTIFY(NM_EDITORCURSORDOWN,OnEditorCursorDown)
END_MESSAGE_MAP()

static void __stdcall TDeleteFunc_Empty( ConstPointer data, Pointer context )
{
}

static void __stdcall TDeleteFunc_CGridViewRows( ConstPointer data, Pointer context )
{
	Ptr(CGridViewRow) p = CastAnyPtr(CGridViewRow, CastMutable(Pointer, data));

	delete p;
}

static void __stdcall TDeleteFunc_CGridViewColumns( ConstPointer data, Pointer context )
{
	Ptr(CGridViewColumn) p = CastAnyPtr(CGridViewColumn, CastMutable(Pointer, data));

	delete p;
}

CGridView::CGridView(LPCTSTR name):
    CControl(name),
	m_multiSelect(false),
	m_editable(false),
	m_emptyCell(NULL),
	m_columns(__FILE__LINE__ 32, 64),
	m_rows(__FILE__LINE__ 64, 128),
	m_selCells(__FILE__LINE__ 256, 1024),
	m_selRows(__FILE__LINE__ 64, 128),
	m_selColumns(__FILE__LINE__ 32, 64),
	m_currentCell(NULL),
	m_stringFormat(NULL),
	m_editor(NULL)
{
	m_emptyCell = OK_NEW_OPERATOR CGridViewEmptyCell(this);
	m_stringFormat = new Gdiplus::StringFormat(Gdiplus::StringFormat::GenericTypographic());
	m_stringFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter);
}

CGridView::CGridView(ConstRef(CStringBuffer) name):
    CControl(name),
	m_multiSelect(false),
	m_editable(false),
	m_emptyCell(NULL),
	m_columns(__FILE__LINE__ 32, 64),
	m_rows(__FILE__LINE__ 64, 128),
	m_selCells(__FILE__LINE__ 256, 1024),
	m_selRows(__FILE__LINE__ 64, 128),
	m_selColumns(__FILE__LINE__ 32, 64),
	m_currentCell(NULL),
	m_stringFormat(NULL),
	m_editor(NULL)
{
	m_emptyCell = OK_NEW_OPERATOR CGridViewEmptyCell(this);
	m_stringFormat = new Gdiplus::StringFormat(Gdiplus::StringFormat::GenericTypographic());
	m_stringFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter);
}

CGridView::~CGridView(void)
{
	if ( m_emptyCell )
		delete m_emptyCell;
	if ( m_stringFormat )
		delete m_stringFormat;
}

BOOL CGridView::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CGRIDVIEW");
	return TRUE;
}

void CGridView::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".Color.Workspace"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridEmptyCell.BackgroundColor.Selected"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridEmptyCell.BackgroundColor.Normal"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridCell.BackgroundColor.Normal"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridCell.BackgroundColor.Selected"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridCell.ForegroundColor.Normal"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridCell.ForegroundColor.Selected"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridColumn.BackgroundColor.Normal"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridColumn.BackgroundColor.Selected"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridColumn.ForegroundColor.Normal"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridColumn.ForegroundColor.Selected"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridRow.BackgroundColor.Normal"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridRow.BackgroundColor.Selected"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridRow.ForegroundColor.Normal"), _T("GridView"));
	add_BrushKeys(_keys, _T(".GridRow.ForegroundColor.Selected"), _T("GridView"));
}

void CGridView::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".GridCell.Font.Selected"), _T("GridView"));
	add_FontKeys(_keys, _T(".GridCell.Font.Normal"), _T("GridView"));
	add_FontKeys(_keys, _T(".GridColumn.Font.Selected"), _T("GridView"));
	add_FontKeys(_keys, _T(".GridColumn.Font.Normal"), _T("GridView"));
	add_FontKeys(_keys, _T(".GridRow.Font.Selected"), _T("GridView"));
	add_FontKeys(_keys, _T(".GridRow.Font.Normal"), _T("GridView"));
}

void CGridView::Refresh()
{
	Update(TRUE);
}

CWin* CGridView::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CGridView();

	return pWin;
}

CGridViewColumn* CGridView::get_Column(LPCTSTR name)
{
	CGridViewColumns::Iterator it = m_columns.Begin();
	CStringLiteral name1(name);

	while ( it )
	{
		CGridViewColumn* column = *it;

		if ( column->get_Text().Compare(name1, 0, CStringLiteral::cIgnoreCase) == 0 )
			return column;
		++it;
	}
	return NULL;
}

void CGridView::set_Column(dword ix, CGridViewColumn* column)
{
	if ( !column )
	{
		if ( ix >= m_columns.Count() )
			return;

		CGridViewColumn* column1 = get_Column(ix);
		CGridViewRows::Iterator it = m_rows.Begin();

		if ( column1->is_Selected() )
			column1->set_Selected(false);
		while ( it )
		{
			CGridViewRow* row = *it;

			if ( ix < row->get_CellCount() )
			{
				if ( m_currentCell == row->get_Cell(ix) )
					m_currentCell = NULL;
				row->set_Cell(ix, NULL);
			}
			++it;
		}
		m_columns.Remove(m_columns.Index(ix));
	}
	else if ( ix >= m_columns.Count() )
		m_columns.Append(column);
	else
	{
		CGridViewColumn* column1 = get_Column(ix);
		CGridViewRows::Iterator it = m_rows.Begin();

		if ( column1->is_Selected() )
			column1->set_Selected(false);
		while ( it )
		{
			CGridViewRow* row = *it;

			if ( ix < row->get_CellCount() )
			{
				CGridViewCell* cell = row->get_Cell(ix);

				cell->set_Column(column);
			}
			++it;
		}
		delete column1;
		m_columns.SetData(m_columns.Index(ix), column);
	}
	Update(TRUE);
}

int CGridView::inx_Column(CGridViewColumn* column)
{
	CGridViewColumns::Iterator it = m_columns.Begin();
	int ix = 0;

	while ( it )
	{
		if ( (*it) == column )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

void CGridView::set_Row(dword ix, CGridViewRow* row)
{
	if ( !row )
	{
		if ( ix >= m_rows.Count() )
			return;

		CGridViewRow* row1 = get_Row(ix);

		if ( row1->is_Selected() )
			row1->set_Selected(false);
		if ( m_currentCell )
		{
			int jx = row1->inx_Cell(m_currentCell);

			if ( jx > -1 )
				m_currentCell = NULL;
		}
		m_rows.Remove(m_rows.Index(ix));
	}
	else if ( ix >= m_rows.Count() )
		m_rows.Append(row);
	else
	{
		CGridViewRow* row1 = get_Row(ix);

		if ( row1->is_Selected() )
			row1->set_Selected(false);
		if ( m_currentCell )
		{
			int jx = row1->inx_Cell(m_currentCell);

			if ( jx > -1 )
				m_currentCell = NULL;
		}
		delete row1;
		m_rows.SetData(m_rows.Index(ix), row);
	}
	Update(TRUE);
}

int CGridView::inx_Row(CGridViewRow* row)
{
	CGridViewRows::Iterator it = m_rows.Begin();
	int ix = 0;

	while ( it )
	{
		if ( (*it) == row )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

void CGridView::set_SelCell(dword ix, CGridViewCell* cell)
{
	if ( !cell )
	{
		if ( ix >= m_selCells.Count() )
			return;
		m_selCells.Remove(m_selCells.Index(ix));
	}
	else if ( ix >= m_selCells.Count() )
		m_selCells.Append(cell);
	else
		m_selCells.SetData(m_selCells.Index(ix), cell);
}

int CGridView::inx_SelCell(CGridViewCell* cell)
{
	CGridViewCells::Iterator it = m_selCells.Begin();
	int ix = 0;

	while ( it )
	{
		if ( (*it) == cell )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

void CGridView::set_SelRow(dword ix, CGridViewRow* row)
{
	if ( !row )
	{
		if ( ix >= m_selRows.Count() )
			return;
		m_selRows.Remove(m_selRows.Index(ix));
	}
	else if ( ix >= m_selRows.Count() )
		m_selRows.Append(row);
	else
		m_selRows.SetData(m_selRows.Index(ix), row);
}

int CGridView::inx_SelRow(CGridViewRow* row)
{
	CGridViewRows::Iterator it = m_selRows.Begin();
	int ix = 0;

	while ( it )
	{
		if ( (*it) == row )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

void CGridView::set_SelColumn(dword ix, CGridViewColumn* column)
{
	if ( !column )
	{
		if ( ix >= m_selColumns.Count() )
			return;
		m_selColumns.Remove(m_selColumns.Index(ix));
	}
	else if ( ix >= m_selColumns.Count() )
		m_selColumns.Append(column);
	else
		m_selColumns.SetData(m_selColumns.Index(ix), column);
}

int CGridView::inx_SelColumn(CGridViewColumn* column)
{
	CGridViewColumns::Iterator it = m_selColumns.Begin();
	int ix = 0;

	while ( it )
	{
		if ( (*it) == column )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

bool CGridView::set_Editable(LPCTSTR key, bool editable)
{
	CGridViewColumn* column = get_Column(key);

	if ( !column )
		return false;

	column->set_Editable(editable);
	return true;
}

void CGridView::set_CurrentCell(CGridViewCell* cell)
{
	BeginUpdate();
	if ( m_currentCell )
		m_currentCell->set_Focused(false);
	m_currentCell = cell;
	if ( m_currentCell )
		m_currentCell->set_Focused(true);
	EndUpdate(FALSE);
}

CGridViewCell* CGridView::get_Cell(dword column, LPCTSTR key)
{
	if ( column >= m_columns.Count() )
		return NULL;

	CGridViewRows::Iterator it = m_rows.Begin();

	while ( it )
	{
		CGridViewRow* row = *it;

		if ( column < row->get_CellCount() )
		{
			CGridViewCell* cell = row->get_Cell(column);

			bool keyIsEmpty = ConstStrEmpty(key);
			bool cellTextIsEmpty = cell->get_Text().IsEmpty();

			if ( (keyIsEmpty && cellTextIsEmpty)
				|| ((!keyIsEmpty) && (!cellTextIsEmpty) && (cell->get_Text().Compare(key, 0, CStringLiteral::cIgnoreCase) == 0)) )
				return cell;
		}
		++it;
	}
	return NULL;
}

void CGridView::SelectAll(bool selected)
{
	m_emptyCell->set_Selected(selected);
}

LRESULT CGridView::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;

	Gdiplus::Graphics graphics(hdc);

	if ( !m_currentCell )
	{
		if ( (get_RowCount() > 0) && (get_Row(0)->get_CellCount() > 0) )
		{
			m_currentCell = get_Row(0)->get_Cell(Cast(dword, 0));
			++m_update;
			m_currentCell->set_Focused(true);
			--m_update;
		}
		else
			SendNotifyMessage(NM_GRIDVIEWFOCUSED, (LPARAM)this);
	}

	if ( m_updateRects )
	{
		RECT svmaxClientArea;
		LPINT maxWidth = (LPINT)(TFalloc((get_ColumnCount() + 1) * sizeof(INT)));
		LPINT maxHeight = (LPINT)(TFalloc((get_RowCount() + 1) * sizeof(INT)));
		LPRECT maxClientArea = (LPRECT)(TFalloc((get_ColumnCount() + 1) * sizeof(RECT)));

		::SecureZeroMemory(maxWidth, (get_ColumnCount() + 1) * sizeof(INT));
		::SecureZeroMemory(maxHeight, (get_RowCount() + 1) * sizeof(INT));
		::SecureZeroMemory(maxClientArea, (get_ColumnCount() + 1) * sizeof(RECT));
		::CopyRect(&svmaxClientArea, &m_maxClientArea);

		CGridViewRows::Iterator itRows = m_rows.Begin();
		dword ix = 0;

		while ( itRows )
		{
			CGridViewRow* row = *itRows;

			row->OnCalcRects(&graphics, maxClientArea, maxWidth, (maxHeight + ix + 1));
			++ix;
			++itRows;
		}

		CGridViewColumns::Iterator itCols = m_columns.Begin();

		ix = 0;
		while ( itCols )
		{
			CGridViewColumn* col = *itCols;

			(maxClientArea + ix + 1)->left = *(maxWidth + ix);
			col->OnCalcRects(&graphics, (maxClientArea + ix + 1), (maxWidth + ix + 1), maxHeight);
			++ix;
			++itCols;
		}
		ix = 0;
		while ( ix < get_RowCount() )
		{
			*(maxHeight + ix + 1) += *maxHeight;
			++ix;
		}
		ix = 0;
		itRows = m_rows.Begin();
		while ( itRows )
		{
			CGridViewRow* row = *itRows;
			INT maxHeightDelta = *(maxHeight + ix + 1);

			for ( dword jx = 0; jx < row->get_CellCount(); ++jx )
			{
				INT maxWidthDelta = *(maxWidth + jx + 1);

				(maxClientArea + jx + 1)->left = *(maxWidth + jx);
				(maxClientArea + jx + 1)->top = *(maxHeight + ix);
				row->get_Cell(jx)->OnCalcRects(&graphics, (maxClientArea + jx + 1), (maxWidth + jx + 1), (maxHeight + ix + 1));
				maxWidthDelta = *(maxWidth + jx + 1) - maxWidthDelta;
				if ( maxWidthDelta > 0 )
				{
					for ( dword kx = jx + 2; kx <= row->get_CellCount(); ++kx )
						*(maxWidth + kx) += maxWidthDelta;
				}
			}
			maxHeightDelta = *(maxHeight + ix + 1) - maxHeightDelta;
			if ( maxHeightDelta > 0 )
			{
				for ( dword kx = ix + 2; kx <= get_RowCount(); ++kx )
					*(maxHeight + kx) += maxHeightDelta;
			}
			++ix;
			++itRows;
		}
		ix = 0;
		itCols = m_columns.Begin();
		while ( itCols )
		{
			CGridViewColumn* col = *itCols;

			col->OnAdjustRects(*(maxWidth + ix), *(maxWidth + ix + 1), *maxHeight);
			++ix;
			++itCols;
		}
		ix = 0;
		itRows = m_rows.Begin();
		while ( itRows )
		{
			CGridViewRow* row = *itRows;

			row->OnAdjustRects(*(maxHeight + ix), *(maxHeight + ix + 1), *maxWidth);
			for ( dword jx = 0; jx < row->get_CellCount(); ++jx )
				row->get_Cell(jx)->OnAdjustRects(*(maxWidth + jx), *(maxWidth + jx + 1), *(maxHeight + ix), *(maxHeight + ix + 1));
			++ix;
			++itRows;
		}
		m_emptyCell->OnAdjustRects(*maxWidth, *maxHeight);
		m_maxClientArea.bottom = *(maxHeight + get_RowCount());
		m_maxClientArea.top = 0;
		m_maxClientArea.right = *(maxWidth + get_ColumnCount());
		m_maxClientArea.left = 0;
		TFfree((LPVOID)maxWidth);
		TFfree((LPVOID)maxHeight);
		TFfree((LPVOID)maxClientArea);
		ShowVertScrollBar();
		ShowHorzScrollBar();
		if ( (svmaxClientArea.bottom != m_maxClientArea.bottom) || (svmaxClientArea.right != m_maxClientArea.right) )
			get_parent()->SendMessage(WM_SIZE, 0, 0);
		m_updateRects = FALSE;
	}

	RECT clientRect;

	GetClientRect(&clientRect);
	Gdiplus::Bitmap bitmap(clientRect.right, clientRect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);

	Gdiplus::Brush* brush = get_Brush(_T(".Color.Workspace"), _T("GridView"), Gdiplus::Color::Gray);

	bitmapgraphics.FillRectangle(brush, 0, 0, clientRect.right, clientRect.bottom);

	int xPos = get_HScrollOffset();
	int yPos = get_VScrollOffset();

	for ( dword ix = 0; ix < get_RowCount(); ++ix )
	{
		CGridViewRow* row = get_Row(ix);

		row->OnPaint(&bitmapgraphics, xPos, yPos, clientRect.bottom);
		for ( dword jx = 0; jx < row->get_CellCount(); ++jx )
			row->get_Cell(jx)->OnPaint(&bitmapgraphics, xPos, yPos, clientRect.bottom);
	}
	for ( dword ix = 0; ix < get_ColumnCount(); ++ix )
		get_Column(ix)->OnPaint(&bitmapgraphics);
	m_emptyCell->OnPaint(&bitmapgraphics);

	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);
	return 0;
}

LRESULT CGridView::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ( !is_created() )
		return 0;
	set_focus();
	if ( wParam & MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		int xPos = get_HScrollOffset();
		int yPos = get_VScrollOffset();
		RECT clientRect;

		GetClientRect(&clientRect);
		if ( m_emptyCell->HitTest(pt) )
		{
			if ( wParam == MK_LBUTTON )
			{
				if ( !m_multiSelect )
				{
					if ( (get_RowCount() > 0) && (get_Row(0)->get_CellCount() > 0) )
					{
						BeginUpdate();
						SelectAll(false);
						if ( m_currentCell )
							m_currentCell->set_Focused(false);
						m_currentCell = get_Row(0)->get_Cell(Cast(dword, 0));
						m_currentCell->set_Focused(true);
						EndUpdate(FALSE);
					}
				}
				else
					m_emptyCell->set_Selected(!(m_emptyCell->is_Selected()));
				return 0;
			}
		}
		CGridViewColumns::Iterator itCol = m_columns.Begin();
		CGridViewColumn* column = NULL;

		while ( itCol )
		{
			column = (*itCol)->HitTest(pt);
			if ( column )
				break;
			++itCol;
		}
		if ( column )
		{
			if ( !m_multiSelect )
			{
				int jx = inx_Column(column);

				if ( jx < 0 )
					return -1;
				if ( (get_RowCount() > 0) && (get_Row(0)->get_CellCount() > Cast(dword, jx)) )
				{
					BeginUpdate();
					SelectAll(false);
					if ( m_currentCell )
						m_currentCell->set_Focused(false);
					m_currentCell = get_Row(0)->get_Cell(jx);
					m_currentCell->set_Focused(true);
					EndUpdate(FALSE);
				}
			}
			else if ( wParam == MK_LBUTTON )
			{
				BeginUpdate();
				SelectAll(false);
				column->set_Selected(true);
				EndUpdate(FALSE);
			}
			else if ( wParam == (MK_LBUTTON | MK_CONTROL) )
				column->set_Selected(!(column->is_Selected()));
			else if ( wParam == (MK_LBUTTON | MK_SHIFT) )
			{
				if ( !m_currentCell )
					return -1;

				int ix1 = inx_Column(column);

				if ( ix1 < 0 )
					return -1;

				int ix2 = inx_Column(m_currentCell->get_Column());

				if ( ix2 < 0 )
					return -1;

				if ( ix1 == ix2 )
					column->set_Selected(true);
				else if ( ix1 < ix2 )
				{
					BeginUpdate();
					for ( ; ix1 <= ix2; ++ix1 )
						get_Column(ix1)->set_Selected(true);
					EndUpdate(FALSE);
				}
				else
				{
					BeginUpdate();
					for ( ; ix2 <= ix1; ++ix2 )
						get_Column(ix2)->set_Selected(true);
					EndUpdate(FALSE);
				}
			}
			return 0;
		}

		CGridViewRows::Iterator itRow = m_rows.Begin();
		CGridViewRow* row = NULL;

		while ( itRow )
		{
			row = (*itRow)->HitTest(pt, xPos, yPos, clientRect.bottom);
			if ( row )
				break;
			++itRow;
		}
		if ( row )
		{
			if ( !m_multiSelect )
			{
				int jx = inx_Row(row);

				if ( jx < 0 )
					return -1;
				if ( (get_RowCount() > 0) && (get_Row(jx)->get_CellCount() > 0) )
				{
					BeginUpdate();
					SelectAll(false);
					if ( m_currentCell )
						m_currentCell->set_Focused(false);
					m_currentCell = get_Row(jx)->get_Cell(Cast(dword, 0));
					m_currentCell->set_Focused(true);
					EndUpdate(FALSE);
				}
			}
			else if ( wParam == MK_LBUTTON )
			{
				BeginUpdate();
				SelectAll(false);
				row->set_Selected(true);
				EndUpdate(FALSE);
			}
			else if ( wParam == (MK_LBUTTON | MK_CONTROL) )
				row->set_Selected(!(row->is_Selected()));
			else if ( wParam == (MK_LBUTTON | MK_SHIFT) )
			{
				if ( !m_currentCell )
					return -1;
				
				int ix1 = inx_Row(row);

				if ( ix1 < 0 )
					return -1;

				int ix2 = inx_Row(m_currentCell->get_Row());

				if ( ix2 < 0 )
					return -1;

				if ( ix1 == ix2 )
					row->set_Selected(true);
				else if ( ix1 < ix2 )
				{
					BeginUpdate();
					for ( ; ix1 <= ix2; ++ix1 )
						get_Row(ix1)->set_Selected(true);
					EndUpdate(FALSE);
				}
				else
				{
					BeginUpdate();
					for ( ; ix2 <= ix1; ++ix2 )
						get_Row(ix2)->set_Selected(true);
					EndUpdate(FALSE);
				}
			}
			return 0;
		}

		CGridViewCell* cell = NULL;

		itRow = m_rows.Begin();
		while ( itRow )
		{
			row = *itRow;
			for ( dword jx = 0; jx < row->get_CellCount(); ++jx )
			{
				cell = row->get_Cell(jx)->HitTest(pt, xPos, yPos, clientRect.bottom);
				if ( cell )
					break;
			}
			if ( cell )
				break;
			++itRow;
		}
		if ( cell )
		{
			if ( (!m_multiSelect) || (wParam == MK_LBUTTON) )
			{
				BeginUpdate();
				SelectAll(false);
				if ( m_currentCell )
					m_currentCell->set_Focused(false);
				m_currentCell = cell;
				m_currentCell->set_Focused(true);
				EndUpdate(FALSE);
			}
			else if ( wParam == (MK_LBUTTON | MK_CONTROL) )
			{
				BeginUpdate();
				if ( m_currentCell )
					m_currentCell->set_Focused(false);
				m_currentCell = cell;
				m_currentCell->set_Focused(true);
				m_currentCell->set_Selected(!(m_currentCell->is_Selected()));
				EndUpdate(FALSE);
			}
			else if ( wParam == (MK_LBUTTON | MK_SHIFT) )
			{
				if ( !m_currentCell )
					return -1;

				int ix1 = inx_Row(m_currentCell->get_Row());

				if ( ix1 < 0 )
					return -1;

				int ix2 = inx_Row(cell->get_Row());

				if ( ix2 < 0 )
					return -1;

				int jx1 = inx_Column(m_currentCell->get_Column());

				if ( jx1 < 0 )
					return -1;

				int jx2 = inx_Column(cell->get_Column());

				if ( jx2 < 0 )
					return -1;

				BeginUpdate();
				m_currentCell->set_Focused(false);
				m_currentCell = cell;
				m_currentCell->set_Focused(true);
				if ( ix1 == ix2 )
				{
					if ( jx1 == jx2 )
						m_currentCell->set_Selected(true);
					else if ( jx1 < jx2 )
					{
						for ( ; jx1 <= jx2; ++jx1 )
							get_Row(ix1)->get_Cell(jx1)->set_Selected(true);
					}
					else
					{
						for ( ; jx2 <= jx1; ++jx2 )
							get_Row(ix1)->get_Cell(jx2)->set_Selected(true);
					}
				}
				else if ( ix1 < ix2 )
				{
					if ( jx1 == jx2 )
					{
						for ( ; ix1 <= ix2; ++ix1 )
							get_Row(ix1)->get_Cell(jx1)->set_Selected(true);
					}
					else if ( jx1 < jx2 )
					{
						int jx0 = jx1;

						for ( ; ix1 <= ix2; ++ix1 )
							for ( jx1 = jx0; jx1 <= jx2; ++jx1 )
								get_Row(ix1)->get_Cell(jx1)->set_Selected(true);
					}
					else
					{
						int jx0 = jx2;

						for ( ; ix1 <= ix2; ++ix1 )
							for ( jx2 = jx0; jx2 <= jx1; ++jx2 )
								get_Row(ix1)->get_Cell(jx2)->set_Selected(true);
					}
				}
				else if ( jx1 == jx2 )
				{
					for ( ; ix2 <= ix1; ++ix2 )
						get_Row(ix2)->get_Cell(jx1)->set_Selected(true);
				}
				else if ( jx1 < jx2 )
				{
					int jx0 = jx1;

					for ( ; ix2 <= ix1; ++ix2 )
						for ( jx1 = jx0; jx1 <= jx2; ++jx1 )
							get_Row(ix2)->get_Cell(jx1)->set_Selected(true);
				}
				else
				{
					int jx0 = jx2;

					for ( ; ix2 <= ix1; ++ix2 )
						for ( jx2 = jx0; jx2 <= jx1; ++jx2 )
							get_Row(ix2)->get_Cell(jx2)->set_Selected(true);
				}
				EndUpdate(FALSE);
			}
			return 0;
		}
	}
	return 0;
}

LRESULT CGridView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		int xPos = get_HScrollOffset();
		int yPos = get_VScrollOffset();
		RECT clientRect;

		GetClientRect(&clientRect);

		CGridViewRows::Iterator itRow = m_rows.Begin();
		CGridViewRow* row = NULL;
		CGridViewCell* cell = NULL;

		while ( itRow )
		{
			row = *itRow;
			for ( dword jx = 0; jx < row->get_CellCount(); ++jx )
			{
				cell = row->get_Cell(jx)->HitTest(pt, xPos, yPos, clientRect.bottom);
				if ( cell )
					break;
			}
			if ( cell )
				break;
			++itRow;
		}
		if ( cell )
		{
			if ( m_currentCell )
				_CloseEditor();
			BeginUpdate();
			cell->set_Selected(false);
			if ( m_currentCell )
				m_currentCell->set_Focused(false);
			m_currentCell = cell;
			m_currentCell->set_Focused(true);
			EndUpdate(FALSE);
			_ShowEditor();
		}
	}
	return 0;
}

LRESULT CGridView::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	switch ( wParam )
	{
	case VK_F2:
		if ( bShift || bControl )
			break;
		_ShowEditor();
		break;
	case VK_RETURN:
		if ( m_currentCell )
		{
			SendNotifyMessage(NM_GRIDVIEWCELLENTERED, (LPARAM)m_currentCell);
			_ShowEditor();
		}
		break;
	case VK_DOWN:
		if ( bShift || bControl )
			break;
		_CurrentCellDown();
		break;
	case VK_UP:
		if ( bShift || bControl )
			break;
		_CurrentCellUp();
		break;
	case VK_LEFT:
		if ( bShift || bControl )
			break;
		_CurrentCellLeft();
		break;
	case VK_RIGHT:
		if ( bShift || bControl )
			break;
		_CurrentCellRight();
		break;
	case VK_ESCAPE:
		if ( bShift || bControl )
			break;
		break;
	case VK_INSERT:
		if ( !m_editable )
			break;
		if ( !(bShift || bControl) )
		{
			CStringBuffer buffer;

			buffer.FormatString(__FILE__LINE__ _T("Row %d"), get_RowCount());

			BeginUpdate();
			if ( m_currentCell )
				m_currentCell->set_Focused(false);

			CGridViewRow* newRow = OK_NEW_OPERATOR CGridViewRow(this, buffer);
			CGridViewColumns::Iterator itCol = m_columns.Begin();

			while ( itCol )
			{
				CGridViewCell* cell = OK_NEW_OPERATOR CGridViewCell(this, newRow, *itCol);

				++itCol;
			}
			if ( newRow->get_CellCount() > 0 )
			{
				m_currentCell = newRow->get_Cell(Cast(dword, 0));
				m_currentCell->set_Focused(true);
			}
			else
				m_currentCell = NULL;
			SendNotifyMessage(NM_GRIDVIEWROWINSERTED, (LPARAM)newRow);
			EndUpdate(TRUE);
		}
		break;
	case VK_DELETE:
		if ( !m_editable )
			break;
		if ( bShift || bControl )
			break;
		if ( m_currentCell )
		{
			CGridViewRow* currentRow = m_currentCell->get_Row();

			if ( !currentRow )
				break;

			int ix = inx_Row(currentRow);

			if ( ix < 0 )
				break;

			int jx = inx_Column(m_currentCell->get_Column());

			if ( jx < 0 )
				break;

			BeginUpdate();
			m_currentCell->set_Focused(false);
			set_Row(ix, NULL);
			SendNotifyMessage(NM_GRIDVIEWROWDELETED, (LPARAM)currentRow);

			if ( (Cast(dword, ix) < get_RowCount()) || ((ix > 0) && (Cast(dword, --ix) < get_RowCount())) )
			{
				m_currentCell = get_Row(ix)->get_Cell(jx);
				m_currentCell->set_Focused(true);
			}
			else
			{
				m_currentCell = NULL;
				SendNotifyMessage(NM_GRIDVIEWFOCUSED, (LPARAM)this);
			}
			EndUpdate(TRUE);
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CGridView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	bool bDone = false;

	if ( (!fwKeys) && zDelta )
	{
		if ( get_VScrollVisible() )
		{
			int pos = get_VScrollOffset();

			pos -= zDelta;
			set_VScrollOffset(pos);
			_CloseEditor(FALSE);
			Update(FALSE);
			bDone = true;
		}
		else if ( get_HScrollVisible() )
		{
			int pos = get_HScrollOffset();

			pos -= zDelta;
			set_HScrollOffset(pos);
			_CloseEditor(FALSE);
			Update(FALSE);
			bDone = true;
		}
	}
	if ( !bDone )
		return DefaultWindowProc(WM_MOUSEWHEEL, wParam, lParam);
	return 0;
}

LRESULT CGridView::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnHScroll(wParam, lParam) )
	{
		_CloseEditor(FALSE);
		Update(FALSE);
	}
	return 0;
}

LRESULT CGridView::OnAboutHScroll(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	Update(FALSE);
	return 0;
}

LRESULT CGridView::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnVScroll(wParam, lParam) )
	{
		_CloseEditor(FALSE);
		Update(FALSE);
	}
	return 0;
}

LRESULT CGridView::OnAboutVScroll(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	Update(FALSE);
	return 0;
}

LRESULT CGridView::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnEditorReturn(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( bControl )
		return 0;
	_CloseEditor();
	if ( bShift )
		_CurrentCellUp();
	else
		_CurrentCellDown();
	_ShowEditor();
	return 0;
}

LRESULT CGridView::OnEditorTab(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( bControl )
		return 0;
	_CloseEditor();
	if ( bShift )
		_CurrentCellUp();
	else
		_CurrentCellDown();
	_ShowEditor();
	return 0;
}

LRESULT CGridView::OnEditorFuncKey(WPARAM wParam, LPARAM lParam)
{
	Ptr(NotifyMessage) pHdr = CastAnyPtr(NotifyMessage, lParam);
	Ptr(TNotifyMessageEditorFuncKey) param = CastAnyPtr(TNotifyMessageEditorFuncKey, pHdr->param);

	switch ( param->funcKey )
	{
	case VK_F2:
		_CloseEditor();
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CGridView::OnEditorLostFocus(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	return 0;
}

LRESULT CGridView::OnEditorEscape(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	return 0;
}

LRESULT CGridView::OnEditorCursorUp(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnEditorCursorDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnEditUndoCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnEditRedoCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnEditCutCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnEditCopyCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnEditPasteCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CGridView::OnEditDeleteCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CGridView::_CurrentCellDown()
{
	if ( !m_currentCell )
		return;

	int ix = inx_Row(m_currentCell->get_Row());

	if ( ix < 0 )
		return;

	int jx = get_Row(ix)->inx_Cell(m_currentCell);

	if ( jx < 0 )
		return;

	if ( Cast(dword, ix) == (get_RowCount() - 1) )
		ix = -1;
	if ( Cast(dword, jx) >= get_Row(++ix)->get_CellCount() )
		return;

	BeginUpdate();
	m_currentCell->set_Focused(false);
	m_currentCell = get_Row(ix)->get_Cell(jx);
	m_currentCell->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CGridView::_CurrentCellUp()
{
	if ( !m_currentCell )
		return;

	int ix = inx_Row(m_currentCell->get_Row());

	if ( ix < 0 )
		return;

	int jx = get_Row(ix)->inx_Cell(m_currentCell);

	if ( jx < 0 )
		return;

	if ( ix == 0 )
		ix = get_RowCount();
	if ( Cast(dword, jx) >= get_Row(--ix)->get_CellCount() )
		return;

	BeginUpdate();
	m_currentCell->set_Focused(false);
	m_currentCell = get_Row(ix)->get_Cell(jx);
	m_currentCell->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CGridView::_CurrentCellLeft()
{
	if ( !m_currentCell )
		return;

	int ix = inx_Row(m_currentCell->get_Row());

	if ( ix < 0 )
		return;

	int jx = get_Row(ix)->inx_Cell(m_currentCell);

	if ( jx < 0 )
		return;

	if ( (--jx) < 0 )
	{
		if ( (--ix) < 0 )
			ix = get_RowCount() - 1;
		jx = get_Row(ix)->get_CellCount() - 1;
		if ( jx < 0 )
			return;
	}
	BeginUpdate();
	m_currentCell->set_Focused(false);
	m_currentCell = get_Row(ix)->get_Cell(jx);
	m_currentCell->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CGridView::_CurrentCellRight()
{
	if ( !m_currentCell )
		return;

	int ix = inx_Row(m_currentCell->get_Row());

	if ( ix < 0 )
		return;

	int jx = get_Row(ix)->inx_Cell(m_currentCell);

	if ( jx < 0 )
		return;

	if ( Cast(dword, ++jx) >= get_Row(ix)->get_CellCount() )
	{
		if ( Cast(dword, ++ix) >= get_RowCount() )
			ix = 0;
		jx = 0;
		if ( Cast(dword, jx) >= get_Row(ix)->get_CellCount() )
			return;
	}
	BeginUpdate();
	m_currentCell->set_Focused(false);
	m_currentCell = get_Row(ix)->get_Cell(jx);
	m_currentCell->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CGridView::_EnsureVisible()
{
	if ( !m_currentCell )
		return;

	if ( get_VScrollVisible() )
	{
		RECT r1;
		RECT r2;

		GetClientRect(&r1);
		m_currentCell->get_Rect(&r2);
		set_VScrollOffset(r2.top - (r1.bottom / 2));
	}
	if ( get_HScrollVisible() )
	{
		RECT r2;

		m_currentCell->get_Rect(&r2);
		set_HScrollOffset(r2.left);
	}
}

void CGridView::_ShowEditor()
{
	if ( (!m_editable) || (!m_currentCell) || (!(m_currentCell->get_Column())) || (!(m_currentCell->get_Column()->is_Editable()))  )
		return;

	CControl* editor = NULL;

	if ( m_currentCell->get_Column()->get_Editor() )
	{
		editor = m_currentCell->get_Column()->get_Editor();
		if ( !(editor->is_created()) )
			editor->Create(m_hwnd, 1000);
		else
			editor->set_Visible(TRUE);
	}
	else 
	{
		if ( !m_editor )
		{
			CStringBuffer name(get_name());

			name.AppendString(_T(".DefaultTextEditor"));
			m_editor = OK_NEW_OPERATOR CTextEditor(name);
			m_editor->Create(m_hwnd, 1000);
		}
		else
			m_editor->set_Visible(TRUE);
		editor = m_editor;
	}

	RECT borderRect;
	
	m_currentCell->get_EditorRect(&borderRect);
	borderRect.top += 1; borderRect.left += 1;
	editor->MoveWindow(&borderRect, FALSE);
	editor->BeginUpdate();
	editor->set_windowtext(m_currentCell->get_Text());
	editor->EndUpdate(TRUE);
	editor->set_focus();
}

void CGridView::_CloseEditor(BOOL bSave)
{
	if ( (!m_editable) || (!m_currentCell) || (!(m_currentCell->get_Column())) || (!(m_currentCell->get_Column()->is_Editable()))  )
		return;

	CControl* editor = NULL;

	if ( m_currentCell->get_Column()->get_Editor() )
		editor = m_currentCell->get_Column()->get_Editor();
	else
		editor = m_editor;
	if ( (!editor) || (!(editor->is_Visible())) )
		return;
	if ( bSave )
		m_currentCell->set_Text(editor->get_windowtext());
	editor->set_Visible(FALSE);
	set_focus();
}
