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
#include "ListView.h"
#include "TextEditor.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"

#define SHIFTED 0x8000

CListViewNode::CListViewNode(CListView* pListView, LPCTSTR pText, int lench, int ix, bool before)
{
	m_listView = pListView;
	m_text.SetString(__FILE__LINE__ pText, lench);
	_init(ix, before);
}

CListViewNode::CListViewNode(CListView* pListView, const CStringBuffer& text, int ix, bool before)
{
	m_listView = pListView;
	m_text = text;
	_init(ix, before);
}

void CListViewNode::_init(int ix, bool before)
{
	_calcTextDisplayLength();
	m_selected = false;
	m_focused = false;
	m_editable = false;
	SetRectEmpty(&m_iconRect);
	SetRectEmpty(&m_textRect);
	SetRectEmpty(&m_borderRect);
	m_imageIndex = -1;

	if ( m_listView )
	{
		m_editable = m_listView->is_Editable();
		if ( ix < 0 )
			m_listView->set_Node(m_listView->get_NodeCount(), this);
		else
			m_listView->ins_Node(ix, this, before);
		m_listView->Update(TRUE);
	}
}

void CListViewNode::_calcTextDisplayLength()
{
	CStringConstIterator it(m_text.GetString());

	it.Find(_T('\n'));
	if ( it.IsEnd() )
		m_textDisplayLength = m_text.GetLength();
	else
		m_textDisplayLength = Castdword(it.GetCurrent() - it.GetOrigin());
}

CListViewNode::~CListViewNode(void)
{
}

void CListViewNode::set_Text(LPCTSTR pText, int lench)
{ 
	m_text.SetString(__FILE__LINE__ pText, lench);
	_calcTextDisplayLength();
	if ( m_listView )
		m_listView->Update(TRUE);
}

void CListViewNode::set_Text(const CStringBuffer& text)
{ 
	m_text = text;
	_calcTextDisplayLength();
	if ( m_listView )
		m_listView->Update(TRUE);
}

void CListViewNode::set_Selected(bool selected)
{ 
	if ( m_selected != selected )
	{
		m_selected = selected;
		if ( m_listView )
		{
			if ( m_selected )
				m_listView->set_SelNode(m_listView->get_SelNodeCount(), this);
			else
			{
				int ix = m_listView->inx_SelNode(this);

				if ( ix > -1 )
					m_listView->set_SelNode(ix, nullptr);
			}
			m_listView->Update(FALSE);
		}
	}
}

void CListViewNode::set_Focused(bool focused) 
{ 
	if ( m_focused != focused )
	{
		m_focused = focused; 
		if ( m_listView ) 
		{
			m_listView->Update(FALSE);
			if ( m_focused )
				m_listView->SendNotifyMessage(NM_LISTVIEWNODEFOCUSED, (LPARAM)(this));
		}
	}
}

void CListViewNode::get_EditorRect(LPRECT rect)
{
	RECT r;

	::SetRectEmpty(&r);
	if ( !m_listView )
	{
		::CopyRect(rect, &r);
		return;
	}

	RECT clientRect;

	m_listView->GetClientRect(&clientRect);

	int xPos = m_listView->get_HScrollOffset();
	int yPos = m_listView->get_VScrollOffset();

	::CopyRect(&r, &m_borderRect); 
	r.right = r.left + clientRect.right; 
	::OffsetRect(&r, 0, -yPos);

	RECT s;

	::IntersectRect(&s, &r, &clientRect);
	::CopyRect(rect, &s);
}

bool CListViewNode::HitTest(POINT pt, LPUINT pType, INT xPos, INT yPos, INT cBottom)
{
	if ( !pType )
		return false;
	if ( !m_listView )
		return false;

	RECT iconRect; ::CopyRect(&iconRect, &m_iconRect); ::OffsetRect(&iconRect, -xPos, -yPos);
	RECT textRect; ::CopyRect(&textRect, &m_textRect); ::OffsetRect(&textRect, -xPos, -yPos);

	if ( iconRect.bottom < 0 )
		return false;
	if ( iconRect.top >= cBottom )
		return false;
	if ( (pt.y >= iconRect.top) && (pt.y < iconRect.bottom) && ((pt.x < iconRect.left) || (pt.x > textRect.right)) )
		return false;
	if ( ::PtInRect(&iconRect, pt) )
	{
		*pType = HT_ICONRECT;
		return true;
	}
	if ( ::PtInRect(&textRect, pt) )
	{
		*pType = HT_TEXTRECT;
		return true;
	}
	return false;
}

void CListViewNode::OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth)
{
	if ( !m_listView )
		return;

	Gdiplus::Font* pFont = nullptr;

	if ( m_selected )
		pFont = m_listView->get_Font(_T(".Font.Selected"), _T("ListView"));
	else
		pFont = m_listView->get_Font(_T(".Font.Normal"), _T("ListView"));

	Gdiplus::PointF pt; pt.X = (Gdiplus::REAL)(pRect->left); pt.Y = (Gdiplus::REAL)(pRect->top);
	Gdiplus::RectF textRect;

	if ( m_text.IsEmpty() )
	{
		textRect.X = pt.X; textRect.Y = pt.Y; textRect.Height = pFont->GetHeight(graphics); textRect.Width = 10;
	}
	else
		graphics->MeasureString(m_text.GetString(), m_textDisplayLength, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &textRect);

	Convert2Rect(pRect, &textRect);

	Gdiplus::Bitmap* image = nullptr;
	LONG hImage = 0;

	if ( (m_imageIndex >= 0) && (((dword)m_imageIndex) < m_listView->get_ImageCount()) )
		image = m_listView->get_Image(m_imageIndex);
	if ( image && (image->GetHeight() > 0) )
		hImage = image->GetHeight();

	::SetRect(&m_iconRect, pRect->left, pRect->top, pRect->left + hImage, (hImage > 0)?(pRect->top + hImage):(pRect->bottom));
	if ( (hImage > 0) && ((pRect->bottom - pRect->top) > hImage) )
		::OffsetRect(&m_iconRect, 0, (pRect->bottom - pRect->top - hImage) / 2);
	::OffsetRect(pRect, hImage + 2, 0);
	::CopyRect(&m_textRect, pRect);
	if ( hImage > (pRect->bottom - pRect->top) )
		::OffsetRect(&m_textRect, 0,  (hImage - (pRect->bottom - pRect->top)) / 2);
	::CopyRect(&m_borderRect, &m_textRect);
	m_borderRect.right += 4; m_borderRect.bottom += 4;
	::OffsetRect(&m_textRect, 2, 2);
	if ( m_borderRect.right > *maxWidth )
		*maxWidth = m_borderRect.right;
	::OffsetRect(pRect, -(hImage + 2), 0);
	pRect->top = max(m_iconRect.bottom, m_borderRect.bottom);
	pRect->bottom = 0;
}

void CListViewNode::OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom, INT cRight)
{
	if ( !m_listView )
		return;

	RECT iconRect; ::CopyRect(&iconRect, &m_iconRect); ::OffsetRect(&iconRect, -xPos, -yPos);
	RECT textRect; ::CopyRect(&textRect, &m_textRect); ::OffsetRect(&textRect, -xPos, -yPos);
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); borderRect.right = borderRect.left + cRight; ::OffsetRect(&borderRect, 0, -yPos);
	
	if ( iconRect.bottom < 0 )
		return;
	if ( iconRect.top >= cBottom )
		return;

	Gdiplus::Font* pFont = nullptr;
	Gdiplus::Brush* pBrushBackground = nullptr;
	Gdiplus::Brush* pBrushForeground = nullptr;

	if ( m_selected )
	{
		pFont = m_listView->get_Font(_T(".Font.Selected"), _T("ListView"));
		pBrushBackground = m_listView->get_Brush(_T(".BackgroundColor.Selected"), _T("ListView"), Gdiplus::Color::Blue);
		pBrushForeground = m_listView->get_Brush(_T(".ForegroundColor.Selected"), _T("ListView"), Gdiplus::Color::White);
	}
	else
	{
		pFont = m_listView->get_Font(_T(".Font.Normal"), _T("ListView"));
		pBrushBackground = m_listView->get_Brush(_T(".BackgroundColor.Normal"), _T("ListView"), Gdiplus::Color::White);
		pBrushForeground = m_listView->get_Brush(_T(".ForegroundColor.Normal"), _T("ListView"), Gdiplus::Color::Black);
	}

	Gdiplus::RectF iconRectF; Convert2RectF(&iconRectF, &iconRect);
	Gdiplus::RectF textRectF; Convert2RectF(&textRectF, &textRect);
	Gdiplus::RectF borderRectF; Convert2RectF(&borderRectF, &borderRect);
	Gdiplus::PointF pt; pt.X = textRectF.X; pt.Y = textRectF.Y;

	graphics->FillRectangle(pBrushBackground, borderRectF);

	Gdiplus::Bitmap* image = nullptr;
	UINT hImage = 0;

	if ( (m_imageIndex >= 0) && (((dword)m_imageIndex) < m_listView->get_ImageCount()) )
		image = m_listView->get_Image(m_imageIndex);
	if ( image && (image->GetHeight() > 0) )
		hImage = image->GetHeight();
	if ( hImage > 0 )
	{
		Gdiplus::Color transparentcolor;
		Gdiplus::ImageAttributes imAtt;

		image->GetPixel(0, hImage - 1, &transparentcolor);
		imAtt.SetColorKey(transparentcolor, transparentcolor, Gdiplus::ColorAdjustTypeBitmap);

		graphics->DrawImage(image, iconRectF, 0.0, 0.0, Cast(Gdiplus::REAL,hImage), Cast(Gdiplus::REAL,hImage), Gdiplus::UnitPixel, &imAtt);
	}
	if ( !(m_text.IsEmpty()) )
		graphics->DrawString(m_text.GetString(), m_textDisplayLength, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), pBrushForeground);
	if ( m_focused )
	{
		Gdiplus::Pen grayPen(Gdiplus::Color::Gray);
		grayPen.SetDashStyle(Gdiplus::DashStyleDot);

		graphics->DrawRectangle(&grayPen, textRectF);
	}
}

BEGIN_MESSAGE_MAP(CControl, CListView)
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
	ON_WM_MESSAGE(WM_GETDLGCODE,OnGetDlgCode)
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

CListView::CListView(LPCTSTR name):
	CControl(name),
	m_SelectMode(TSelectModeDisabled),
	m_editable(false),
	m_hasSelection(false),
	m_nodes(__FILE__LINE__ 64, 128),
	m_selNodes(__FILE__LINE__ 64, 128),
	m_images(),
	m_currentNode(nullptr),
	m_editor(nullptr)
{
}

CListView::CListView(ConstRef(CStringBuffer) name):
	CControl(name),
	m_SelectMode(TSelectModeDisabled),
	m_editable(false),
	m_hasSelection(false),
	m_nodes(__FILE__LINE__ 64, 128),
	m_selNodes(__FILE__LINE__ 64, 128),
	m_images(),
	m_currentNode(nullptr),
	m_editor(nullptr)
{
}

CListView::~CListView(void)
{
	clear();
}

BOOL CListView::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CLISTVIEW");
	return TRUE;
}

void CListView::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".Color.Workspace"), _T("ListView"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Selected"), _T("ListView"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("ListView"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Selected"), _T("ListView"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("ListView"));
}

void CListView::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".Font.Selected"), _T("ListView"));
	add_FontKeys(_keys, _T(".Font.Normal"), _T("ListView"));
}

void CListView::Refresh()
{
	Update(TRUE);
}

CWin* CListView::Clone(LONG style, DWORD exstyle)
{
	CListView* pWin = OK_NEW_OPERATOR CListView();

	if ( style & WS_BORDER )
		pWin->set_border(TRUE);
	return pWin;
}

void CListView::clear(void)
{
}

CListViewNode* CListView::get_Node(LPCTSTR key)
{
	CListViewNode* result = nullptr;
	CListViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CListViewNode* node = *it;

		if ( _tcscmp(key, node->get_Text()) == 0 )
		{
			result = node;
			break;
		}
		++it;
	}
	return result;
}

void CListView::set_Node(dword ix, CListViewNode* node)
{ 
	BeginUpdate();
	if ( !node )
	{
		if ( ix >= m_nodes.Count() )
		{
			EndUpdate(FALSE);
			return;
		}

		CListViewNode* node1 = *(m_nodes.Index(ix));

		if ( node1 == m_currentNode )
			m_currentNode = nullptr;
		if ( node1->is_Selected() )
			node1->set_Selected(false);
		m_nodes.Remove(m_nodes.Index(ix));
	}
	else if ( ix >= m_nodes.Count() )
		m_nodes.Append(node);
	else
	{
		if ( ix >= m_nodes.Count() )
		{
			EndUpdate(FALSE);
			return;
		}

		CListViewNode* node1 = *(m_nodes.Index(ix));

		if ( node1 == m_currentNode )
			m_currentNode = nullptr;
		if ( node1->is_Selected() )
			node1->set_Selected(false);
		m_nodes.SetData(m_nodes.Index(ix), node);
	}
	EndUpdate(TRUE);
}

void CListView::ins_Node(dword ix, CListViewNode* node, bool before)
{
	if ( ix >= m_nodes.Count() )
		return;
	if ( before )
		m_nodes.InsertBefore(m_nodes.Index(ix), node);
	else
		m_nodes.InsertAfter(m_nodes.Index(ix), node);
	Update(TRUE);
}

int CListView::inx_Node(CListViewNode* node)
{
	CListViewNodeVector::Iterator it = m_nodes.Begin();
	int ix = 0;

	while ( it )
	{
		if ( (*it) == node )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

void CListView::set_SelNode(dword ix, CListViewNode* node)
{ 
	if ( !node )
	{
		if ( ix >= m_selNodes.Count() )
			return;
		m_selNodes.Remove(m_selNodes.Index(ix));
	}
	else if ( ix >= m_selNodes.Count() )
		m_selNodes.Append(node);
	else
	{
		if ( ix >= m_selNodes.Count() )
			return;
		m_selNodes.SetData(m_selNodes.Index(ix), node);
	}
}

int CListView::inx_SelNode(CListViewNode* node)
{
	CListViewNodeVector::Iterator it = m_selNodes.Begin();
	int ix = 0;

	while ( it )
	{
		if ( (*it) == node )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

void CListView::SelectAll(bool selected)
{
	if ( m_SelectMode == TSelectModeDisabled )
		return;
	BeginUpdate();
	if ( selected )
	{
		CListViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			(*it)->set_Selected(selected);
			++it;
		}
	}
	else
	{
		while ( m_selNodes.Count() )
			(*(m_selNodes.Last()))->set_Selected(false);
	}
	EndUpdate(FALSE);
}

void CListView::set_SelectMode(TSelectMode _SelectMode)
{ 
	BeginUpdate();
	while ( m_selNodes.Count() )
		(*(m_selNodes.Last()))->set_Selected(false);
	EndUpdate(FALSE);
	m_SelectMode = _SelectMode;
}

void CListView::set_Editable(bool editable) 
{ 
	if ( m_editable != editable )
	{
		m_editable = editable;
		if ( !m_editable )
		{
			_CloseEditor(FALSE);

			CListViewNodeVector::Iterator it = m_nodes.Begin();

			while ( it )
			{
				(*it)->set_Editable(false);
				++it;
			}
		}
	}
}

void CListView::set_CurrentNode(CListViewNode* node)
{
	BeginUpdate();
	if ( m_currentNode )
		m_currentNode->set_Focused(false);
	m_currentNode = node;
	if ( m_currentNode )
		m_currentNode->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

LRESULT CListView::OnPaint(WPARAM wParam, LPARAM lParam)
{	
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;

	Gdiplus::Graphics graphics(hdc);

	if ( !m_currentNode )
	{
		if ( m_nodes.Count() > 0 )
		{
			m_currentNode = *(m_nodes.Index(0));
			++m_update;
			m_currentNode->set_Focused(true);
			--m_update;
		}
	}
	if ( m_updateRects )
	{
		int maxWidth = 0;
		RECT svmaxClientArea;
		bool bOnSize = false;

		::CopyRect(&svmaxClientArea, &m_maxClientArea);
		::SetRectEmpty(&m_maxClientArea);

		CListViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			(*it)->OnCalcRects(&graphics, &m_maxClientArea, &maxWidth);
			++it;
		}
		m_maxClientArea.bottom = m_maxClientArea.top;
		m_maxClientArea.top = 0;
		m_maxClientArea.right = maxWidth;
		m_maxClientArea.left = 0;
		m_updateRects = FALSE;
		ShowHorzScrollBar();
		ShowVertScrollBar();
		if ( (svmaxClientArea.bottom != m_maxClientArea.bottom) || (svmaxClientArea.right != m_maxClientArea.right) )
			get_parent()->SendMessage(WM_SIZE, 0, 0);
	}

	RECT clientRect;

	GetClientRect(&clientRect);
	Gdiplus::Bitmap bitmap(clientRect.right, clientRect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);
	Gdiplus::Brush* brush = get_Brush(_T(".Color.Workspace"), _T("ListView"), Gdiplus::Color::Gray);

	bitmapgraphics.FillRectangle(brush, 0, 0, clientRect.right, clientRect.bottom);

	int xPos = get_HScrollOffset();
	int yPos = get_VScrollOffset();

	CListViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		(*it)->OnPaint(&bitmapgraphics, xPos, yPos, clientRect.bottom, clientRect.right);
		++it;
	}

	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);

	if ( m_SelectMode != TSelectModeDisabled )
	{
		if ( (m_hasSelection && (m_selNodes.Count() == 0)) || ((!m_hasSelection) && (m_selNodes.Count() != 0)) )
		{
			SendNotifyMessage(NM_LISTVIEWSELCHANGE, (LPARAM)this);
			SendNotifyMessage(NM_SELECTIONCHANGED);
			m_hasSelection = (m_selNodes.Count() != 0);
		}
	}
	return 0;
}

LRESULT CListView::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	if ( (x < 0) && (y < 0) )
	{
		if ( m_currentNode )
			SendNotifyMessage(NM_LISTVIEWNODECONTEXTMENU, (LPARAM)m_currentNode);
		else
			SendNotifyMessage(NM_LISTVIEWCONTEXTMENU, (LPARAM)this);
		return 0;
	}

	POINT pt; pt.x = x; pt.y = y;
	UINT type = 0;
	CListViewNode* node = nullptr;
	int xPos = get_HScrollOffset();
	int yPos = get_VScrollOffset();
	RECT clientRect;
	bool bFound = false;

	ScreenToClient(&pt);
	GetClientRect(&clientRect);

	CListViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		node = *it;
		if ( node->HitTest(pt, &type, xPos, yPos, clientRect.bottom) )
		{
			bFound = true;
			break;
		}
		++it;
	}
	if ( type == HT_ICONRECT )
		return 1;
	if ( bFound )
		SendNotifyMessage(NM_LISTVIEWNODECONTEXTMENU, (LPARAM)node);
	else
		SendNotifyMessage(NM_LISTVIEWCONTEXTMENU, (LPARAM)this);
	return 0;
}

LRESULT CListView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ( !(is_created()) )
		return -1;
	set_focus();
	_CloseEditor();
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		UINT type = 0;
		CListViewNode* node = nullptr;
		int xPos = get_HScrollOffset();
		int yPos = get_VScrollOffset();
		RECT clientRect;
		bool bFound = false;

		GetClientRect(&clientRect);

		CListViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			node = *it;
			if ( node->HitTest(pt, &type, xPos, yPos, clientRect.bottom) )
			{
				bFound = true;
				break;
			}
			++it;
		}
		if ( bFound )
		{
			if ( type == HT_ICONRECT )
			{
			}
			if ( type == HT_TEXTRECT )
			{
				if ( (wParam == MK_LBUTTON) || (m_SelectMode == TSelectModeDisabled) )
				{
					BeginUpdate();
					if ( (m_SelectMode != TSelectModeDisabled) && (m_selNodes.Count() != 0) )
						SelectAll(false);
					if ( m_currentNode )
						m_currentNode->set_Focused(false);
					m_currentNode = node;
					m_currentNode->set_Focused(true);
					EndUpdate(FALSE);
				}
				else if ( wParam == (MK_LBUTTON | MK_SHIFT) )
				{
					if ( !m_currentNode )
						return 0;

					int ix1 = inx_Node(m_currentNode);

					if ( ix1 < 0 )
						return 0;

					int ix2 = inx_Node(node);

					if ( ix2 < 0 )
						return 0;

					BeginUpdate();
					if ( (m_SelectMode == TSelectModeSimple) && (m_selNodes.Count() != 0) )
						SelectAll(false);
					m_currentNode->set_Focused(false);
					m_currentNode = node;
					m_currentNode->set_Focused(true);
					if ( ix1 == ix2 )
					{
						m_currentNode->set_Selected(true);
					}
					else if ( ix1 < ix2 )
					{
						for ( ; ix1 <= ix2; ++ix1 )
							(*(m_nodes.Index(ix1)))->set_Selected(true);
					}
					else
					{
						for ( ; ix2 <= ix1; ++ix2 )
							(*(m_nodes.Index(ix2)))->set_Selected(true);
					}
					EndUpdate(FALSE);
				}
				else if ( (wParam == (MK_LBUTTON | MK_CONTROL)) && (m_SelectMode == TSelectModeMulti) )
				{
					BeginUpdate();
					if ( m_currentNode )
						m_currentNode->set_Focused(false);
					m_currentNode = node;
					m_currentNode->set_Focused(true);
					m_currentNode->set_Selected(!(m_currentNode->is_Selected()));
					EndUpdate(FALSE);
				}
			}
		}
	}
	return 0;
}

LRESULT CListView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		UINT type = 0;
		CListViewNode* node = nullptr;
		int xPos = (get_HScrollVisible())?(get_HScrollOffset()):0;
		int yPos = (get_VScrollVisible())?(get_VScrollOffset()):0;
		RECT clientRect;
		bool bFound = false;

		GetClientRect(&clientRect);

		CListViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			node = *it;
			if ( node->HitTest(pt, &type, xPos, yPos, clientRect.bottom) )
			{
				bFound = true;
				break;
			}
			++it;
		}
		if ( bFound )
		{
			_ShowEditor();
			SendNotifyMessage(NM_LISTVIEWNODEENTERED, (LPARAM)node);
		}
	}
	return 0;
}

LRESULT CListView::OnKeyDown(WPARAM wParam, LPARAM lParam)
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
	case VK_ESCAPE:
		if ( bShift || bControl )
			break;
		_CloseEditor();
		SelectAll(false);
		SendNotifyMessage(NM_LISTVIEWESCAPE, (LPARAM)this);
		break;
	case VK_RETURN:
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
		{
			_ShowEditor();
			SendNotifyMessage(NM_LISTVIEWNODEENTERED, (LPARAM)m_currentNode);
		}
		break;
	case VK_DOWN:
		_KeyDown();
		break;
	case VK_UP:
		_KeyUp();
		break;
	case VK_INSERT:
		{
			if ( !m_editable )
				break;
			if ( bControl )
				break;
			BeginUpdate();

			int ix = (m_currentNode)?(inx_Node(m_currentNode)):(-1);

			if ( m_currentNode )
				m_currentNode->set_Focused(false);
			m_currentNode = OK_NEW_OPERATOR CListViewNode(this, nullptr, -1, ix, !bShift);
			m_currentNode->set_Focused(true);
			EndUpdate(TRUE);
		}
		break;
	case VK_DELETE:
		if ( !m_editable )
			break;
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
		{
			int ix = inx_Node(m_currentNode);

			if ( ix < 0 )
				break;

			BeginUpdate();
			set_Node(ix, nullptr);

			if ( (((dword)ix) < m_nodes.Count()) || ((ix > 0) && (((dword)--ix) < m_nodes.Count())) )
			{
				m_currentNode = *(m_nodes.Index(ix));
				m_currentNode->set_Focused(true);
			}
			else
				m_currentNode = nullptr;
			EndUpdate(TRUE);
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CListView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
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
			Update(TRUE);
			bDone = true;
		}
		else if ( get_HScrollVisible() )
		{
			int pos = get_HScrollOffset();

			pos -= zDelta;
			set_HScrollOffset(pos);
			_CloseEditor(FALSE);
			Update(TRUE);
			bDone = true;
		}
	}
	if ( !bDone )
	{
		_CloseEditor(FALSE);
		return DefaultWindowProc(WM_MOUSEWHEEL, wParam, lParam);
	}
	return 0;
}

LRESULT CListView::OnAboutHScroll(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	Update(FALSE);
	return 0;
}

LRESULT CListView::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnHScroll(wParam, lParam) )
	{
		_CloseEditor(FALSE);
		Update(FALSE);
	}
	return 0;
}

LRESULT CListView::OnAboutVScroll(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	Update(FALSE);
	return 0;
}

LRESULT CListView::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnVScroll(wParam, lParam) )
	{                    
		_CloseEditor(FALSE);
		Update(FALSE);
	}
	return 0;
}

LRESULT CListView::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CListView::OnGetDlgCode(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case VK_DOWN:
	case VK_UP:
		return DLGC_WANTARROWS;
	case VK_RETURN:
		return DLGC_WANTMESSAGE;
	default:
		break;
	}
	return 0;
}

LRESULT CListView::OnEditorReturn(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( !m_currentNode )
		return 0;
	if ( bControl )
		return 0;

	_CloseEditor();
	if ( bShift )
		_CurrentNodeUp();
	else
		_CurrentNodeDown();
	_ShowEditor();
	return 0;
}

LRESULT CListView::OnEditorTab(WPARAM wParam, LPARAM lParam)
{
	OnEditorReturn(wParam, lParam);
	return 0;
}

LRESULT CListView::OnEditorFuncKey(WPARAM wParam, LPARAM lParam)
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

LRESULT CListView::OnEditorLostFocus(WPARAM wParam, LPARAM lParam)
{
	//_CloseEditor(FALSE);
	return 0;
}

LRESULT CListView::OnEditorEscape(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	return 0;
}

LRESULT CListView::OnEditorCursorUp(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( !m_currentNode )
		return 0;
	if ( bControl || bShift )
		return 0;

	_CloseEditor();
	_CurrentNodeUp();
	_ShowEditor();
	return 0;
}

LRESULT CListView::OnEditorCursorDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( !m_currentNode )
		return 0;
	if ( bControl || bShift )
		return 0;

	_CloseEditor();
	_CurrentNodeDown();
	_ShowEditor();
	return 0;
}

LRESULT CListView::OnEditUndoCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CListView::OnEditRedoCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CListView::OnEditCutCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CListView::OnEditCopyCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CListView::OnEditPasteCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CListView::OnEditDeleteCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CListView::_KeyDown()
{
	if ( !m_currentNode )
		return;

	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( bShift && (m_SelectMode == TSelectModeDisabled) )
		return;
	if ( bControl && ((m_SelectMode == TSelectModeDisabled) || (m_SelectMode == TSelectModeSimple)) )
		return;
	if ( bShift && bControl )
		return;

	int ix0 = inx_Node(m_currentNode);
	int ix1;

	if ( ix0 < 0 )
		return;

	if ( Cast(dword, ix0) == (m_nodes.Count() - 1) )
		ix1 = 0;
	else
		ix1 = ix0 + 1;

	BeginUpdate();
	if ( bControl )
		m_currentNode->set_Selected(!(m_currentNode->is_Selected()));
	else if ( bShift )
	{
		if ( m_SelectMode == TSelectModeSimple )
		{
			if ( m_currentNode->is_Selected() )
			{
				if ( _TestAllSelected() )
				{
					SelectAll(false);
					m_currentNode->set_Selected(true);
				}
			}
			else
			{
				if ( ((ix0 == 0) && (ix1 == 0)) || ((ix0 > 0) && ((*(m_nodes.Index(ix0 - 1)))->is_Selected())) )
					m_currentNode->set_Selected(true);
				else
				{
					SelectAll(false);
					m_currentNode->set_Selected(true);
				}
			}
		}
		else if ( m_currentNode->is_Selected() )
		{
			if ( _TestAllSelected() )
			{
				SelectAll(false);
				m_currentNode->set_Selected(true);
			}
		}
		else
			m_currentNode->set_Selected(true);
	}
	if ( ix0 != ix1 )
	{
		m_currentNode->set_Focused(false);
		m_currentNode = *(m_nodes.Index(ix1));
		m_currentNode->set_Focused(true);
	}
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CListView::_KeyUp()
{
	if ( !m_currentNode )
		return;

	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( bShift && (m_SelectMode == TSelectModeDisabled) )
		return;
	if ( bControl && ((m_SelectMode == TSelectModeDisabled) || (m_SelectMode == TSelectModeSimple)) )
		return;
	if ( bShift && bControl )
		return;

	int ix0 = inx_Node(m_currentNode);
	int ix1;

	if ( ix0 < 0 )
		return;

	if ( ix0 == 0 )
		ix1 = m_nodes.Count() - 1;
	else
		ix1 = ix0 - 1;

	BeginUpdate();
	if ( bControl )
		m_currentNode->set_Selected(!(m_currentNode->is_Selected()));
	else if ( bShift )
	{
		if ( m_SelectMode == TSelectModeSimple )
		{
			if ( m_currentNode->is_Selected() )
			{
				if ( _TestAllSelected() )
				{
					SelectAll(false);
					m_currentNode->set_Selected(true);
				}
			}
			else
			{
				if ( ((ix0 == 0) && (ix1 == 0)) || ((Cast(dword,ix0) < (m_nodes.Count() - 1)) && ((*(m_nodes.Index(ix0 + 1)))->is_Selected())) )
					m_currentNode->set_Selected(true);
				else
				{
					SelectAll(false);
					m_currentNode->set_Selected(true);
				}
			}
		}
		else if ( m_currentNode->is_Selected() )
		{
			if ( _TestAllSelected() )
			{
				SelectAll(false);
				m_currentNode->set_Selected(true);
			}
		}
		else
			m_currentNode->set_Selected(true);
	}
	if ( ix0 != ix1 )
	{
		m_currentNode->set_Focused(false);
		m_currentNode = *(m_nodes.Index(ix1));
		m_currentNode->set_Focused(true);
	}
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CListView::_CurrentNodeDown()
{
	if ( !m_currentNode )
		return;

	int ix = inx_Node(m_currentNode);

	if ( ix < 0 )
		return;

	if ( Cast(dword, ix) == (m_nodes.Count() - 1) )
		ix = 0;
	else
		++ix;
	BeginUpdate();
	m_currentNode->set_Focused(false);
	m_currentNode = *(m_nodes.Index(ix));
	m_currentNode->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CListView::_CurrentNodeUp()
{
	if ( !m_currentNode )
		return;

	int ix = inx_Node(m_currentNode);

	if ( ix < 0 )
		return;

	if ( ix == 0 )
		ix = m_nodes.Count() - 1;
	else
		--ix;

	BeginUpdate();
	m_currentNode->set_Focused(false);
	m_currentNode = *(m_nodes.Index(ix));
	m_currentNode->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CListView::_EnsureVisible()
{
	if ( !m_currentNode )
		return;

	bool bUpdate = false;

	if ( get_VScrollEnabled() || get_HScrollEnabled() )
	{
		RECT nodeR;

		m_currentNode->get_Rect(&nodeR);
		if ( get_VScrollVisible() )
		{
			RECT rect;

			GetClientRect(&rect);
			set_VScrollOffset(nodeR.top - (rect.bottom / 2));
			bUpdate = true;
		}
		if ( get_HScrollVisible() )
		{
			set_HScrollOffset(nodeR.left);
			bUpdate = true;
		}
		if ( bUpdate )
			Update(TRUE);
		return;
	}

	CControl* pParent = CastDynamicPtr(CControl, get_parent());

	if ( pParent )
	{
		if ( pParent->get_VScrollEnabled() || pParent->get_HScrollEnabled() )
		{
			RECT r1;

			m_currentNode->get_Rect(&r1);
			ClientToScreen(&r1);
			pParent->ScreenToClient(&r1);
			if ( pParent->get_VScrollVisible() )
			{
				RECT rect;

				pParent->GetClientRect(&rect);
				pParent->set_VScrollOffset(r1.top - (rect.bottom / 2));
				bUpdate = true;
			}
			if ( pParent->get_HScrollVisible() )
			{
				pParent->set_HScrollOffset(r1.left);
				bUpdate = true;
			}
			if ( bUpdate )
				pParent->OnSize(0, 0);
		}
	}
}

void CListView::_ShowEditor()
{
	if ( (!is_Editable()) || (!m_currentNode) || (!(m_currentNode->is_Editable())) )
		return;
	if ( !m_editor )
	{
		CStringBuffer name;

		name.AppendString(m_name);
		name.AppendString(_T(".DefaultTextEditor"));
		m_editor = OK_NEW_OPERATOR CTextEditor(name);
		m_editor->Create(m_hwnd, 1000);
	}
	else if ( !(m_editor->is_created()) )
		m_editor->Create(m_hwnd, 1000);
	else
		m_editor->set_Visible(TRUE);

	RECT borderRect;
	
	m_currentNode->get_EditorRect(&borderRect);
	OffsetRect(&borderRect, 2, 2);
	borderRect.right -= 3;
	m_editor->MoveWindow(&borderRect, FALSE);

	m_editor->set_windowtext(m_currentNode->get_Text());
	m_editor->set_focus();
}

void CListView::_CloseEditor(BOOL bSave)
{
	if ( (!is_Editable()) || (!m_editor) || (!(m_editor->is_Visible())) || (!m_currentNode) || (!(m_currentNode->is_Editable())) )
		return;
	if ( bSave )
		m_currentNode->set_Text(m_editor->get_windowtext());
	m_editor->set_Visible(FALSE);
	set_focus();
}

bool CListView::_TestAllSelected()
{
	CListViewNodeVector::Iterator it = m_nodes.Begin();
	bool bAll = true;

	while ( it )
	{
		CListViewNode* node = *it;

		if ( !(node->is_Selected()) )
		{
			bAll = false;
			break;
		}
		++it;
	}
	return bAll;
}