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
#include "TreeView.h"
#include "TextEditor.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"

#define SHIFTED 0x8000

static void __stdcall TDeleteFunc_SelectedTreeViewNodes( ConstPointer data, Pointer context )
{
}

static void __stdcall TDeleteFunc_TreeViewNodes( ConstPointer data, Pointer context )
{
	Ptr(CTreeViewNode) p = CastAnyPtr(CTreeViewNode, CastMutable(Pointer, data));

	delete p;
}

static sword __stdcall TSearchAndSortFunc_FindNode( ConstPointer ArrayItem, ConstPointer DataItem )
{
	Ptr(CTreeViewNode) pNode = CastAnyPtr(CTreeViewNode, CastMutable(Pointer, ArrayItem));
	LPCTSTR pParam = CastAny(LPCTSTR, DataItem);

	return pNode->get_Text().Compare(CStringLiteral(pParam), 0, CStringLiteral::cIgnoreCase);
}

CTreeViewNode::CTreeViewNode(CTreeView* pTreeView, LPCTSTR text, int lench, int ix, bool before):
	m_nodes(__FILE__LINE__ 32, 64)
{
	m_treeView = pTreeView;
	m_parent = NULL;
	m_text.SetString(__FILE__LINE__ text, lench);
	_init(ix, before);
}

CTreeViewNode::CTreeViewNode(CTreeView* pTreeView, ConstRef(CStringBuffer) text, int ix, bool before):
	m_nodes(__FILE__LINE__ 32, 64)
{
	m_treeView = pTreeView;
	m_parent = NULL;
	m_text.SetString(__FILE__LINE__ text);
	_init(ix, before);
}

CTreeViewNode::CTreeViewNode(CTreeViewNode* pParent, LPCTSTR text, int lench, int ix, bool before):
	m_nodes(__FILE__LINE__ 32, 64)
{
	if ( pParent )
		m_treeView = pParent->get_TreeView();
	else
		m_treeView = NULL;
	m_parent = pParent;
	m_text.SetString(__FILE__LINE__ text, lench);
	_init(ix, before);
}

CTreeViewNode::CTreeViewNode(CTreeViewNode* pParent, ConstRef(CStringBuffer) text, int ix, bool before):
	m_nodes(__FILE__LINE__ 32, 64)
{
	if ( pParent )
		m_treeView = pParent->get_TreeView();
	else
		m_treeView = NULL;
	m_parent = pParent;
	m_text.SetString(__FILE__LINE__ text);
	_init(ix, before);
}

CTreeViewNode::~CTreeViewNode(void)
{
	CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node = *it;

		if ( node->is_Selected() )
			node->set_Selected(false);
		++it;
	}
}

void CTreeViewNode::_init(int ix, bool before)
{
	m_pattern.SetString(__FILE__LINE__ _T("%s"));
	m_displayText.FormatString(__FILE__LINE__ m_pattern.GetString(), m_text.GetString());
	m_voidTag = NULL;
	m_expanded = false;
	m_selected = false;
	m_focused = false;
	m_editable = false;
	m_nodesLoaded = false;
	::SetRectEmpty(&m_iconRect);
	::SetRectEmpty(&m_textRect);
	::SetRectEmpty(&m_borderRect);
	m_plusImageIndex = 0;
	m_minusImageIndex = 1;

	if ( m_parent )
	{
		if ( ix < 0 )
			m_parent->set_Node(m_parent->get_NodeCount(), this);
		else
			m_parent->ins_Node(ix, this, before);
	}
	if ( m_treeView )
	{
		m_editable = m_treeView->is_Editable();
		if ( !m_parent )
		{
			if ( ix < 0 )
				m_treeView->set_Node(m_treeView->get_NodeCount(), this);
			else
				m_treeView->ins_Node(ix, this, before);
		}
		m_treeView->Update(TRUE);
	}
}

void CTreeViewNode::set_Text(LPCTSTR pText, int lench)
{
	m_text.SetString(__FILE__LINE__ pText, lench);
	m_displayText.FormatString(__FILE__LINE__ m_pattern.GetString(), m_text.GetString());
	if ( m_treeView )
		m_treeView->Update(TRUE);
}

void CTreeViewNode::set_Text(ConstRef(CStringBuffer) text)
{
	m_text = text;
	m_displayText.FormatString(__FILE__LINE__ m_pattern.GetString(), m_text.GetString());
	if ( m_treeView )
		m_treeView->Update(TRUE);
}

void CTreeViewNode::set_Pattern(LPCTSTR pText, int lench)
{
	m_pattern.SetString(__FILE__LINE__ pText, lench);
	if ( m_pattern.IsEmpty() )
		m_pattern.SetString(__FILE__LINE__ _T("%s"));
	m_displayText.FormatString(__FILE__LINE__ m_pattern.GetString(), m_text.GetString());
	if ( m_treeView )
		m_treeView->Update(TRUE);
}

void CTreeViewNode::set_Pattern(ConstRef(CStringBuffer) text)
{
	m_pattern = text;
	if ( m_pattern.IsEmpty() )
		m_pattern.SetString(__FILE__LINE__ _T("%s"));
	m_displayText.FormatString(__FILE__LINE__ m_pattern.GetString(), m_text.GetString());
	if ( m_treeView )
		m_treeView->Update(TRUE);
}

void CTreeViewNode::set_Node(dword ix, CTreeViewNode* node)
{
	if ( m_treeView )
	{
		m_treeView->BeginUpdate();
		if ( !node )
		{
			if ( ix >= m_nodes.Count() )
			{
				m_treeView->EndUpdate(FALSE);
				return;
			}

			CTreeViewNode* node1 = *(m_nodes.Index(ix));

			node1->set_Focused(false);
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
				m_treeView->EndUpdate(FALSE);
				return;
			}

			CTreeViewNode* node1 = *(m_nodes.Index(ix));

			node1->set_Focused(false);
			if ( node1->is_Selected() )
				node1->set_Selected(false);
			delete node1;
			m_nodes.SetData(m_nodes.Index(ix), node);
		}
		m_treeView->EndUpdate(TRUE);
	}
}

void CTreeViewNode::ins_Node(dword ix, CTreeViewNode* node, bool before)
{
	if ( ix >= m_nodes.Count() )
		return;
	node->set_TreeView(m_treeView);
	node->set_Parent(this);
	if ( before )
		m_nodes.InsertBefore(m_nodes.Index(ix), node);
	else
		m_nodes.InsertAfter(m_nodes.Index(ix), node);
	if ( m_treeView )
		m_treeView->Update(TRUE);
}

int CTreeViewNode::inx_Node(CTreeViewNode* node)
{
	CTreeViewNodeVector::Iterator it = m_nodes.Begin();
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

CTreeViewNode* CTreeViewNode::get_Node(LPCTSTR nodeString)
{
	CTreeViewNode* result = NULL;
	CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node1 = *it;

		if ( (!(node1->get_Text().IsEmpty())) && (s_strcmp(nodeString, node1->get_Text().GetString()) == 0) )
		{
			result = node1;
			break;
		}
		result = node1->get_Node(nodeString);
		if ( result )
			break;
		++it;
	}
	return result;
}

CTreeViewNode* CTreeViewNode::get_NodeByStringTag(LPCTSTR tag)
{
	CTreeViewNode* result = NULL;
	CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node1 = *it;

		if ( (!(node1->get_StringTag().IsEmpty())) && (s_strcmp(tag, node1->get_StringTag().GetString()) == 0) )
		{
			result = node1;
			break;
		}
		result = node1->get_NodeByStringTag(tag);
		if ( result )
			break;
		++it;
	}
	return result;
}

CTreeViewNode* CTreeViewNode::get_NodeByVoidTag(LPVOID tag)
{
	CTreeViewNode* result = NULL;
	CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node1 = *it;

		if ( tag == node1->get_VoidTag() )
		{
			result = node1;
			break;
		}
		result = node1->get_NodeByVoidTag(tag);
		if ( result )
			break;
		++it;
	}
	return result;
}

void CTreeViewNode::find_Node(bool bSortDirs, LPCTSTR nodeString, bool bDir, int* pIndex, bool* pBefore)
{
	if ( get_NodeCount() == 0 )
	{
		*pIndex = -1;
		*pBefore = false;
		return;
	}
	if ( !bSortDirs )
	{
		CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.FindSorted(CastAnyPtr(CTreeViewNode, CastMutable(LPTSTR, nodeString)), TSearchAndSortFunc_FindNode);

		if ( (Cast(LSearchResultType, it).offset == (get_NodeCount() - 1)) && (TSearchAndSortFunc_FindNode(*it, nodeString) < 0) )
		{
			*pIndex = -1;
			*pBefore = false;
		}
		else
		{
			*pBefore = true;
			*pIndex = Cast(LSearchResultType, it).offset;
		}
		return;
	}

	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	if ( !bDir )
	{
		while ( it )
		{
			CTreeViewNode* node1 = *it;

			if ( node1->get_NodeCount() == 0 )
				break;
			++it;
		}
	}
	while ( it )
	{
		CTreeViewNode* node1 = *it;

		if ( bDir && (node1->get_NodeCount() == 0) )
		{
			*pBefore = true;
			*pIndex = inx_Node(node1);
			return;
		}
		if ( node1->get_Text().Compare(CStringLiteral(nodeString), 0, CStringLiteral::cIgnoreCase) > 0 )
		{
			*pBefore = true;
			*pIndex = inx_Node(node1);
			return;
		}
		++it;
	}
	*pIndex = -1;
	*pBefore = false;
}
CTreeViewNode* CTreeViewNode::get_NextSibling()
{
	if ( m_parent )
	{
		int ix = m_parent->inx_Node(this);

		if ( (ix < 0) || (((dword)ix) >= (m_parent->get_NodeCount() - 1)) )
			return NULL;
		++ix;
		return m_parent->get_Node(ix);
	}
	else if ( m_treeView )
	{
		int ix = m_treeView->inx_Node(this);

		if ( (ix < 0) || (((dword)ix) >= (m_treeView->get_NodeCount() - 1)) )
			return NULL;
		++ix;
		return m_treeView->get_Node(ix);
	}
	return NULL;
}

void CTreeViewNode::set_Expanded(bool expanded)
{
	if ( m_nodes.Count() > 0 ) 
	{
		if ( m_expanded != expanded )
		{
			m_expanded = expanded;
			if ( m_treeView )
			{
				m_treeView->BeginUpdate();
				if ( m_treeView->is_VirtualLoad() )
				{
					if ( m_expanded )
					{
						if ( m_nodesLoaded )
						{
							CTreeViewNodeVector::Iterator it = m_nodes.Begin();

							while ( it )
							{
								CTreeViewNode* node = *it;

								if ( !(node->has_NodesLoaded()) )
									m_treeView->SendNotifyMessage(NM_TREEVIEWNODELOADNEEDED, (LPARAM)node);
								++it;
							}
						}
						else
							m_treeView->SendNotifyMessage(NM_TREEVIEWNODELOADNEEDED, (LPARAM)this);
					}
				}
				if ( !m_expanded )
				{
					CTreeViewNode* pNode = get_CurrentNode();

					if ( pNode && (pNode != this) )
						m_treeView->set_CurrentNode(this);
				}
				m_treeView->SendNotifyMessage(NM_TREEVIEWNODEEXPANDED, (LPARAM)this);
				m_treeView->EndUpdate(TRUE);
			}
		}
	}
}

void CTreeViewNode::set_Focused(bool focused)
{
	if ( m_focused != focused )
	{
		m_focused = focused; 
		if ( m_treeView ) 
		{
			m_treeView->Update(FALSE);
			if ( m_focused )
				m_treeView->SendNotifyMessage(NM_TREEVIEWNODEFOCUSED, (LPARAM)(this));
		}
	}
}

CTreeViewNode* CTreeViewNode::get_CurrentNode()
{
	if ( is_Focused() )
		return this;

	CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node = *it;
		CTreeViewNode* node1 = node->get_CurrentNode();

		if ( node1 )
			return node1;
		++it;
	}
	return NULL;
}

void CTreeViewNode::set_Selected(bool selected)
{
	if ( m_selected != selected )
	{
		m_selected = selected; 
		if ( m_treeView ) 
		{
			if ( m_selected )
				m_treeView->set_SelNode(m_treeView->get_SelNodeCount(), this);
			else
			{
				int ix = m_treeView->inx_SelNode(this);

				if ( ix > -1 )
					m_treeView->set_SelNode(ix, NULL);
			}
			m_treeView->Update(FALSE);
		}
	}
}

void CTreeViewNode::get_EditorRect(LPRECT rect)
{
	RECT r;

	SetRectEmpty(&r);
	if ( !m_treeView )
	{
		::CopyRect(rect, &r);
		return;
	}

	RECT clientRect;

	m_treeView->GetClientRect(&clientRect);

	int xPos = m_treeView->get_HScrollOffset();
	int yPos = m_treeView->get_VScrollOffset();

	::CopyRect(&r, &m_borderRect); 
	::OffsetRect(&r, -xPos, -yPos);

	RECT s;

	::IntersectRect(&s, &r, &clientRect);
	::CopyRect(rect, &s);
}

void CTreeViewNode::SelectAll(bool selected)
{
	if ( m_treeView ) 
	{
		m_treeView->BeginUpdate();
		set_Selected(selected);

		CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node = *it;

			node->SelectAll(selected);
			++it;
		}
		m_treeView->EndUpdate(FALSE);
	}
}

void CTreeViewNode::ExpandAll(bool expand)
{
	if ( m_treeView ) 
	{
		if ( m_nodes.Count() == 0 )
			return;
		m_treeView->BeginUpdate();
		set_Expanded(expand);

		CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node = *it;

			node->ExpandAll(expand);
			++it;
		}
		m_treeView->EndUpdate(TRUE);
	}
}

CTreeViewNode* CTreeViewNode::HitTest(POINT pt, LPUINT pType, INT xPos, INT yPos, INT cBottom)
{
	if ( !pType )
		return NULL;
	if ( !m_treeView )
		return NULL;

	RECT iconRect; CopyRect(&iconRect, &m_iconRect); OffsetRect(&iconRect, -xPos, -yPos);
	RECT textRect; CopyRect(&textRect, &m_textRect); OffsetRect(&textRect, -xPos, -yPos);

	if ( iconRect.bottom >= 0 )
	{
		if ( iconRect.top >= cBottom )
			return NULL;
		if ( (pt.y >= iconRect.top) && (pt.y < iconRect.bottom) && ((pt.x < iconRect.left) || (pt.x > textRect.right)) )
			return NULL;
		if ( PtInRect(&iconRect, pt) )
		{
			*pType = HT_ICONRECT;
			return this;
		}
		if ( PtInRect(&textRect, pt) )
		{
			*pType = HT_TEXTRECT;
			return this;
		}
	}
	if ( m_expanded )
	{
		CTreeViewNode* node = NULL;
		CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node1 = *it;

			node = node1->HitTest(pt, pType, xPos, yPos, cBottom);
			if ( node )
				break;
			++it;
		}
		return node;
	}
	return NULL;
}

void CTreeViewNode::OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth)
{
	if ( !m_treeView )
		return;

	Gdiplus::Font* pFont = NULL;

	if ( m_selected )
		pFont = m_treeView->get_Font(_T(".Font.Selected"), _T("TreeView"));
	else
		pFont = m_treeView->get_Font(_T(".Font.Normal"), _T("TreeView"));

	Gdiplus::PointF pt; pt.X = (Gdiplus::REAL)(pRect->left); pt.Y = (Gdiplus::REAL)(pRect->top);
	Gdiplus::RectF textRectF;

	if ( m_displayText.IsEmpty() )
	{
		textRectF.X = pt.X; textRectF.Y = pt.Y; textRectF.Height = pFont->GetHeight(graphics); textRectF.Width = 30;
	}
	else
		graphics->MeasureString(m_displayText.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &textRectF);
	Convert2Rect(pRect, &textRectF);

	Gdiplus::Image* imagePlus = m_treeView->get_Image(m_plusImageIndex);
	Gdiplus::Image* imageMinus = m_treeView->get_Image(m_minusImageIndex);
	UINT hImage = 0;

	if ( (imagePlus != NULL) && (imagePlus->GetHeight() > 0) && (imageMinus != NULL) && (imageMinus->GetHeight() > 0) )
		hImage = Max(imagePlus->GetHeight(), imageMinus->GetHeight());

	UINT h = pRect->bottom - pRect->top + 4;

	if ( hImage > h )
		h = hImage;

	::SetRect(&m_iconRect, pRect->left, pRect->top, pRect->left + h, pRect->top + h);
	::OffsetRect(pRect, h, 0);
	::CopyRect(&m_textRect, pRect);
	::OffsetRect(&m_textRect, 0, (h - pRect->bottom + pRect->top - 4) / 2);
	::CopyRect(&m_borderRect, &m_textRect);
	m_borderRect.right += 4; m_borderRect.bottom += 4;
	::OffsetRect(&m_textRect, 2, 2);
	if ( m_borderRect.right > *maxWidth )
		*maxWidth = m_borderRect.right;
	pRect->top += h;
	pRect->bottom = 0;
	if ( m_expanded )
	{
		CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node = *it;

			node->OnCalcRects(graphics, pRect, maxWidth);
			++it;
		}
	}
	pRect->left -= h;
	pRect->right -= h;
}

BOOL CTreeViewNode::OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom)
{
	if ( !m_treeView )
		return FALSE;

	RECT iconRect; ::CopyRect(&iconRect, &m_iconRect); ::OffsetRect(&iconRect, -xPos, -yPos);
	RECT textRect; ::CopyRect(&textRect, &m_textRect); ::OffsetRect(&textRect, -xPos, -yPos);
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); ::OffsetRect(&borderRect, -xPos, -yPos);
	
	if ( iconRect.bottom >= 0 )
	{
		if ( iconRect.top >= cBottom )
			return FALSE;

		Gdiplus::Font* pFont = NULL;
		Gdiplus::Brush* pBrushBackground = NULL;
		Gdiplus::Brush* pBrushForeground = NULL;

		if ( m_selected )
		{
			pFont = m_treeView->get_Font(_T(".Font.Selected"), _T("TreeView"));
			pBrushBackground = m_treeView->get_Brush(_T(".BackgroundColor.Selected"), _T("TreeView"), Gdiplus::Color::Blue);
			pBrushForeground = m_treeView->get_Brush(_T(".ForegroundColor.Selected"), _T("TreeView"), Gdiplus::Color::White);
		}
		else
		{
			pFont = m_treeView->get_Font(_T(".Font.Normal"), _T("TreeView"));
			pBrushBackground = m_treeView->get_Brush(_T(".BackgroundColor.Normal"), _T("TreeView"), Gdiplus::Color::White);
			pBrushForeground = m_treeView->get_Brush(_T(".ForegroundColor.Normal"), _T("TreeView"), Gdiplus::Color::Black);
		}
		
		Gdiplus::RectF borderRectF;
		Gdiplus::RectF textRectF;
		Gdiplus::PointF pt;

		Convert2RectF(&borderRectF, &borderRect);
		Convert2RectF(&textRectF, &textRect);
		pt.X = textRectF.X; pt.Y = textRectF.Y;

		Gdiplus::Image* imagePlus = m_treeView->get_Image(m_plusImageIndex);
		Gdiplus::Image* imageMinus = m_treeView->get_Image(m_minusImageIndex);
		UINT hImage = 0;

		if ( (imagePlus != NULL) && (imagePlus->GetHeight() > 0) && (imageMinus != NULL) && (imageMinus->GetHeight() > 0) )
			hImage = Max(imagePlus->GetHeight(), imageMinus->GetHeight());

		if ( m_nodes.Count() > 0 )
		{
			if ( hImage > 0 )
			{
				if ( m_expanded )
					graphics->DrawImage(imageMinus, iconRect.left, iconRect.top, iconRect.right - iconRect.left, iconRect.bottom - iconRect.top);
				else
					graphics->DrawImage(imagePlus, iconRect.left, iconRect.top, iconRect.right - iconRect.left, iconRect.bottom - iconRect.top);
			}
			else
			{
				Gdiplus::Pen blackPen(Gdiplus::Color::Black);

				INT h = iconRect.bottom - iconRect.top;
				INT h2 = 2 + iconRect.top;
				INT h4 = h - 2 + iconRect.top;
				INT h42 = (h - 2) / 2 + iconRect.top + 1;

				INT l = iconRect.right - iconRect.left;
				INT l2 = 2 + iconRect.left;
				INT l4 = l - 2 + iconRect.left;
				INT l42 = (l - 2) / 2 + iconRect.left + 1;

				graphics->DrawLine(&blackPen, l2, h2, l2, h4);
				graphics->DrawLine(&blackPen, l2, h2, l4, h2);
				graphics->DrawLine(&blackPen, l4, h4, l2, h4);
				graphics->DrawLine(&blackPen, l4, h4, l4, h2);

				graphics->DrawLine(&blackPen, l2, h42, l4, h42);

				if ( !m_expanded )
					graphics->DrawLine(&blackPen, l42, h2, l42, h4);
			}
		}

		graphics->FillRectangle(pBrushBackground, borderRectF);
		if ( !(m_displayText.IsEmpty()) )
			graphics->DrawString(m_displayText.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), pBrushForeground);
		if ( m_focused )
		{
			Gdiplus::Pen grayPen(Gdiplus::Color::Gray);
			grayPen.SetDashStyle(Gdiplus::DashStyleDot);

			graphics->DrawRectangle(&grayPen, textRectF);
		}
	}
	if ( m_expanded )
	{
		CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node = *it;

			if ( !(node->OnPaint(graphics, xPos, yPos, cBottom)) )
				return FALSE;
			++it;
		}
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CControl, CTreeView)
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

CTreeView::CTreeView(LPCTSTR name):
	CControl(name),
	m_multiSelect(false),
	m_editable(false),
	m_virtualLoad(false),
	m_nodes(__FILE__LINE__ 32, 64),
	m_selNodes(__FILE__LINE__ 32, 64),
	m_currentNode(NULL),
	m_editor(NULL)
{
}

CTreeView::CTreeView(ConstRef(CStringBuffer) name):
	CControl(name),
	m_multiSelect(false),
	m_editable(false),
	m_virtualLoad(false),
	m_nodes(__FILE__LINE__ 32, 64),
	m_selNodes(__FILE__LINE__ 32, 64),
	m_currentNode(NULL),
	m_editor(NULL)
{
}

CTreeView::~CTreeView(void)
{
}

BOOL CTreeView::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CTREEVIEW");
	return TRUE;
}

void CTreeView::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".Color.Workspace"), _T("TreeView"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Selected"), _T("TreeView"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("TreeView"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Selected"), _T("TreeView"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("TreeView"));
}

void CTreeView::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".Font.Selected"), _T("TreeView"));
	add_FontKeys(_keys, _T(".Font.Normal"), _T("TreeView"));
}

void CTreeView::Refresh()
{
	Update(TRUE);
}

CWin* CTreeView::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CTreeView();

	return pWin;
}

void CTreeView::_CurrentNodeDown()
{
	if ( !m_currentNode )
		return;

	if ( (m_currentNode->get_NodeCount() > 0) 
		&& (m_currentNode->is_Expanded()) )
	{
		BeginUpdate();
		m_currentNode->set_Focused(false);
		m_currentNode = m_currentNode->get_Node(Cast(dword, 0));
		m_currentNode->set_Focused(true);
		_EnsureVisible();
		EndUpdate(FALSE);
		return;
	}

	int ix = -1;
	CTreeViewNode* parent = m_currentNode->get_Parent();

	if ( parent )
		ix = parent->inx_Node(m_currentNode);
	else
		ix = inx_Node(m_currentNode);
	if ( ix < 0 )
		return;
	while ( parent )
	{
		if ( ((dword)ix) == (parent->get_NodeCount() - 1) )
		{
			CTreeViewNode* parent1 = parent->get_Parent();

			if ( parent1 )
				ix = parent1->inx_Node(parent);
			else
				ix = inx_Node(parent);
			if ( ix < 0 )
				return;
			parent = parent1;
		}
		else
		{
			++ix;
			break;
		}
	}
	if ( !parent )
	{
		if ( ((dword)ix) == (m_nodes.Count() - 1) )
			ix = 0;
		else
			++ix;
	}
	BeginUpdate();
	m_currentNode->set_Focused(false);
	if ( parent )
		m_currentNode = parent->get_Node(ix);
	else
		m_currentNode = get_Node(ix);
	m_currentNode->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CTreeView::_CurrentNodeUp()
{
	if ( !m_currentNode )
		return;

	int ix = -1;
	CTreeViewNode* parent = m_currentNode->get_Parent();

	if ( parent )
		ix = parent->inx_Node(m_currentNode);
	else
		ix = inx_Node(m_currentNode);
	if ( ix < 0 )
		return;
	if ( parent )
	{
		if ( ix == 0 )
		{
			BeginUpdate();
			m_currentNode->set_Focused(false);
			m_currentNode = parent;
			m_currentNode->set_Focused(true);
			_EnsureVisible();
			EndUpdate(FALSE);
			return;
		}
		else
			--ix;
	}
	else if ( ix == 0 )
		ix = m_nodes.Count() - 1;
	else
		--ix;
	BeginUpdate();
	m_currentNode->set_Focused(false);
	if ( parent )
		m_currentNode = parent->get_Node(ix);
	else
		m_currentNode = get_Node(ix);
	while ( (m_currentNode->get_NodeCount() > 0) 
		&& (m_currentNode->is_Expanded()) )
		m_currentNode = m_currentNode->get_Node(m_currentNode->get_NodeCount() - 1);
	m_currentNode->set_Focused(true);
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CTreeView::_EnsureVisible()
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
				pParent->SendMessage(WM_SIZE, 0, 0);
		}
	}
}

void CTreeView::_ShowEditor()
{
	if ( (!m_editable) || (!m_currentNode) || (!(m_currentNode->is_Editable())) )
		return;
	if ( !m_editor )
	{
		CStringBuffer name(__FILE__LINE__ m_name.GetLength() + 18 + 1);

		name.AppendString(m_name.GetString(), m_name.GetLength());
		name.AppendString(_T(".DefaultTextEditor"), 18);
		m_editor = OK_NEW_OPERATOR CTextEditor(name);
		m_editor->Create(m_hwnd, 1000);
	}
	else if ( !(m_editor->is_created()) )
		m_editor->Create(m_hwnd, 1000);
	else
		m_editor->set_Visible(TRUE);

	RECT borderRect;

	m_currentNode->get_EditorRect(&borderRect);
	borderRect.top += 1; borderRect.left += 1;
	m_editor->MoveWindow(&borderRect, FALSE);

	m_editor->set_windowtext(m_currentNode->get_Text());
	m_editor->set_focus();
}

void CTreeView::_CloseEditor(BOOL bSave)
{
	if ( (!m_editable) || (!m_editor) || (!(m_editor->is_Visible())) || (!m_currentNode) || (!(m_currentNode->is_Editable())) )
		return;
	if ( bSave )
		m_currentNode->set_Text(m_editor->get_windowtext());
	m_editor->set_Visible(FALSE);
	set_focus();
}

void CTreeView::set_Node(dword ix, CTreeViewNode* node)
{
	BeginUpdate();
	if ( !node )
	{
		if ( ix >= m_nodes.Count() )
		{
			EndUpdate(FALSE);
			return;
		}

		CTreeViewNode* node1 = *(m_nodes.Index(ix));

		node1->set_Focused(false);
		if ( m_currentNode == node1 )
			m_currentNode = NULL;
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

		CTreeViewNode* node1 = *(m_nodes.Index(ix));

		node1->set_Focused(false);
		if ( m_currentNode == node1 )
			m_currentNode = NULL;
		if ( node1->is_Selected() )
			node1->set_Selected(false);
		delete node1;
		m_nodes.SetData(m_nodes.Index(ix), node);
	}
	EndUpdate(TRUE);
}

void CTreeView::ins_Node(dword ix, CTreeViewNode* node, bool before)
{
	if ( ix >= m_nodes.Count() )
		return;
	node->set_TreeView(this);
	node->set_Parent(NULL);
	if ( before )
		m_nodes.InsertBefore(m_nodes.Index(ix), node);
	else
		m_nodes.InsertAfter(m_nodes.Index(ix), node);
	Update(TRUE);
}

int CTreeView::inx_Node(CTreeViewNode* node)
{
	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();
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

CTreeViewNode* CTreeView::get_Node(LPCTSTR nodeString)
{
	CTreeViewNode* result = NULL;
	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node1 = *it;

		if ( (!(node1->get_Text().IsEmpty())) && (s_strcmp(nodeString, node1->get_Text().GetString()) == 0) )
		{
			result = node1;
			break;
		}
		result = node1->get_Node(nodeString);
		if ( result )
			break;
		++it;
	}
	return result;
}

CTreeViewNode* CTreeView::get_NodeByStringTag(LPCTSTR tag)
{
	CTreeViewNode* result = NULL;
	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node1 = *it;

		if ( (!(node1->get_StringTag().IsEmpty())) && (s_strcmp(tag, node1->get_StringTag().GetString()) == 0) )
		{
			result = node1;
			break;
		}
		result = node1->get_NodeByStringTag(tag);
		if ( result )
			break;
		++it;
	}
	return result;
}

CTreeViewNode* CTreeView::get_NodeByVoidTag(LPVOID tag)
{
	CTreeViewNode* result = NULL;
	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node1 = *it;

		if ( tag == node1->get_VoidTag() )
		{
			result = node1;
			break;
		}
		result = node1->get_NodeByVoidTag(tag);
		if ( result )
			break;
		++it;
	}
	return result;
}

void CTreeView::find_Node(bool bSortDirs, LPCTSTR nodeString, bool bDir, int* pIndex, bool* pBefore)
{
	if ( get_NodeCount() == 0 )
	{
		*pIndex = -1;
		*pBefore = false;
		return;
	}
	if ( !bSortDirs )
	{
		CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.FindSorted(CastAnyPtr(CTreeViewNode, CastMutable(LPTSTR, nodeString)), TSearchAndSortFunc_FindNode);

		if ( (Cast(LSearchResultType, it).offset == (get_NodeCount() - 1)) && (TSearchAndSortFunc_FindNode(*it, nodeString) < 0) )
		{
			*pIndex = -1;
			*pBefore = false;
		}
		else
		{
			*pBefore = true;
			*pIndex = Cast(LSearchResultType, it).offset;
		}
		return;
	}

	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	if ( !bDir )
	{
		while ( it )
		{
			CTreeViewNode* node1 = *it;

			if ( node1->get_NodeCount() == 0 )
				break;
			++it;
		}
	}
	while ( it )
	{
		CTreeViewNode* node1 = *it;

		if ( bDir && (node1->get_NodeCount() == 0) )
		{
			*pBefore = true;
			*pIndex = inx_Node(node1);
			return;
		}
		if ( node1->get_Text().Compare(CStringLiteral(nodeString), 0, CStringLiteral::cIgnoreCase) > 0 )
		{
			*pBefore = true;
			*pIndex = inx_Node(node1);
			return;
		}
		++it;
	}
	*pIndex = -1;
	*pBefore = false;
}

void CTreeView::set_SelNode(dword ix, CTreeViewNode* node)
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

int CTreeView::inx_SelNode(CTreeViewNode* node)
{
	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_selNodes.Begin();
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

void CTreeView::set_CurrentNode(CTreeViewNode* node)
{
	BeginUpdate();
	if ( m_currentNode )
		m_currentNode->set_Focused(false);
	m_currentNode = node;
	if ( m_currentNode )
		m_currentNode->set_Focused(true);
	_EnsureVisible();
	EndUpdate(TRUE);
}

void CTreeView::move_CurrentNode(CTreeViewNode* otherParent, int pos)
{
	CTreeViewNode* node = m_currentNode;

	if ( !node )
		return;

	CTreeViewNode* thisParent = node->get_Parent();
	int ix;

	BeginUpdate();
	if ( thisParent )
	{
		ix = thisParent->inx_Node(node);

		if ( ix < 0 )
		{
			EndUpdate(FALSE);
			return;
		}
		thisParent->set_Node(ix, NULL);
	}
	else
	{
		ix = inx_Node(node);

		if ( ix < 0 )
		{
			EndUpdate(FALSE);
			return;
		}
		set_Node(ix, NULL);
	}
	node->set_Parent(otherParent);
	if ( otherParent )
	{
		if ( (pos < 0) || (Cast(dword, pos) >= otherParent->get_NodeCount()) )
			otherParent->set_Node(otherParent->get_NodeCount(), node);
		else
			otherParent->ins_Node(pos, node);
	}
	else
	{
		if ( (pos < 0) || (Cast(dword, pos) >= get_NodeCount()) )
			set_Node(get_NodeCount(), node);
		else
			ins_Node(pos, node);
	}
	EndUpdate(TRUE);
	_EnsureVisible();
}

void CTreeView::SelectAll(bool selected)
{
	BeginUpdate();
	if ( selected )
	{
		CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node = *it;

			node->SelectAll(true);
			++it;
		}
	}
	else
	{
		while ( m_selNodes.Count() )
			(*(m_selNodes.Index(0)))->set_Selected(false);
	}
	EndUpdate(FALSE);
}

void CTreeView::ExpandAll(bool expand)
{
	BeginUpdate();

	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node = *it;

		node->ExpandAll(expand);
		++it;
	}
	EndUpdate(FALSE);
}

LRESULT CTreeView::OnPaint(WPARAM wParam, LPARAM lParam)
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

		::CopyRect(&svmaxClientArea, &m_maxClientArea);
		::SetRectEmpty(&m_maxClientArea);

		CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node = *it;

			node->OnCalcRects(&graphics, &m_maxClientArea, &maxWidth);
			++it;
		}
		m_maxClientArea.bottom = m_maxClientArea.top;
		m_maxClientArea.top = 0;
		m_maxClientArea.right = maxWidth;
		m_maxClientArea.left = 0;
		m_updateRects = FALSE;
		if ( m_sizeSpecX == TSizeSpecifierDefaultSize )
			m_maxClientArea.right += 10;
		ShowHorzScrollBar();
		ShowVertScrollBar();
		if ( (m_maxClientArea.right != svmaxClientArea.right) || (m_maxClientArea.bottom != svmaxClientArea.bottom) )
			get_parent()->SendMessage(WM_SIZE, 0, 0);
	}

	RECT clientRect;

	GetClientRect(&clientRect);

	Gdiplus::Bitmap bitmap(clientRect.right, clientRect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);

	Gdiplus::Brush* brush = get_Brush(_T(".Color.Workspace"), _T("TreeView"), Gdiplus::Color::Gray);
	Gdiplus::Brush* pBrushBackground = get_Brush(_T(".BackgroundColor.Normal"), _T("TreeView"), Gdiplus::Color::White);

	bitmapgraphics.FillRectangle(brush, 0, 0, clientRect.right, clientRect.bottom);
	bitmapgraphics.FillRectangle(pBrushBackground, 0, 0, m_maxClientArea.right, m_maxClientArea.bottom);

	int xPos = get_HScrollOffset();
	int yPos = get_VScrollOffset();

	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node = *it;

		if ( !(node->OnPaint(&bitmapgraphics, xPos, yPos, clientRect.bottom)) )
			break;
		++it;
	}
	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);
	return 0;
}

LRESULT CTreeView::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	if ( (x < 0) && (y < 0) )
	{
		if ( m_currentNode )
			SendNotifyMessage(NM_TREEVIEWNODECONTEXTMENU, (LPARAM)m_currentNode);
		else
			SendNotifyMessage(NM_TREEVIEWCONTEXTMENU, (LPARAM)this);
		return 0;
	}

	POINT pt; pt.x = x; pt.y = y;
	UINT type = 0;
	CTreeViewNode* node = NULL;
	int xPos = get_HScrollOffset();
	int yPos = get_VScrollOffset();
	RECT clientRect;

	ScreenToClient(&pt);
	GetClientRect(&clientRect);

	CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

	while ( it )
	{
		CTreeViewNode* node1 = *it;

		node = node1->HitTest(pt, &type, xPos, yPos, clientRect.bottom);
		if ( node )
			break;
		++it;
	}
	if ( type == HT_ICONRECT )
		return 1;
	if ( node )
		SendNotifyMessage(NM_TREEVIEWNODECONTEXTMENU, (LPARAM)node);
	else
		SendNotifyMessage(NM_TREEVIEWCONTEXTMENU, (LPARAM)this);
	return 0;
}

LRESULT CTreeView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ( !m_hwnd )
		return 1;
	set_focus();
	if ( wParam & MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		UINT type = 0;
		CTreeViewNode* node = NULL;
		int xPos = get_HScrollOffset();
		int yPos = get_VScrollOffset();
		RECT clientRect;

		GetClientRect(&clientRect);

		CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node1 = *it;

			node = node1->HitTest(pt, &type, xPos, yPos, clientRect.bottom);
			if ( node )
				break;
			++it;
		}
		if ( node )
		{
			if ( type == HT_ICONRECT )
			{
				node->set_Expanded(!(node->is_Expanded()));
			}
			if ( type == HT_TEXTRECT )
			{
				if ( (!m_multiSelect) || (wParam == MK_LBUTTON)  )
				{
					BeginUpdate();
					SelectAll(false);
					if ( m_currentNode )
						m_currentNode->set_Focused(false);
					m_currentNode = node;
					m_currentNode->set_Focused(true);
					EndUpdate(FALSE);
				}
				else if ( wParam == (MK_LBUTTON | MK_CONTROL) )
				{
					BeginUpdate();
					if ( m_currentNode )
						m_currentNode->set_Focused(false);
					m_currentNode = node;
					m_currentNode->set_Focused(true);
					m_currentNode->set_Selected(!(m_currentNode->is_Selected()));
					EndUpdate(FALSE);
				}
				else if ( wParam == (MK_LBUTTON | MK_SHIFT) )
				{
					if ( !m_currentNode )
						return 1;

					int ix1 = -1;
					CTreeViewNode* parent1 = m_currentNode->get_Parent();

					if ( parent1 )
						ix1 = parent1->inx_Node(m_currentNode);
					else
						ix1 = inx_Node(m_currentNode);
					if ( ix1 < 0 )
						return 1;

					int ix2 = -1;
					CTreeViewNode* parent2 = node->get_Parent();

					if ( parent2 )
						ix2 = parent2->inx_Node(node);
					else
						ix2 = inx_Node(node);
					if ( ix2 < 0 )
						return 1;

					BeginUpdate();
					m_currentNode->set_Focused(false);
					m_currentNode = node;
					m_currentNode->set_Focused(true);
					if ( parent1 == parent2 )
					{
						if ( parent1 )
						{
							if ( ix1 == ix2 )
							{
								m_currentNode->set_Selected(true);
							}
							else if ( ix1 < ix2 )
							{
								for ( ; ix1 <= ix2; ++ix1 )
									parent1->get_Node(ix1)->set_Selected(true);
							}
							else
							{
								for ( ; ix2 <= ix1; ++ix2 )
									parent1->get_Node(ix2)->set_Selected(true);
							}
						}
						else
						{
							if ( ix1 == ix2 )
							{
								m_currentNode->set_Selected(true);
							}
							else if ( ix1 < ix2 )
							{
								for ( ; ix1 <= ix2; ++ix1 )
									get_Node(ix1)->set_Selected(true);
							}
							else
							{
								for ( ; ix2 <= ix1; ++ix2 )
									get_Node(ix2)->set_Selected(true);
							}
						}
					}
					EndUpdate(FALSE);
				}
			}
		}
	}
	return 0;
}

LRESULT CTreeView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		UINT type = 0;
		CTreeViewNode* node = NULL;
		int xPos = get_HScrollOffset();
		int yPos = get_VScrollOffset();
		RECT clientRect;

		GetClientRect(&clientRect);

		CTreeViewNode::CTreeViewNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CTreeViewNode* node1 = *it;

			node = node1->HitTest(pt, &type, xPos, yPos, clientRect.bottom);
			if ( node )
				break;
			++it;
		}
		if ( (node == m_currentNode) && (type == HT_TEXTRECT) )
			_ShowEditor();
	}
	return 0;
}

LRESULT CTreeView::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

	switch ( wParam )
	{
	case VK_F2:
		if ( bShift || bControl )
			break;
		_ShowEditor();
		break;
	case VK_OEM_PLUS:
		if ( bShift && (!bControl) )
		{
			if ( m_currentNode )
				m_currentNode->ExpandAll(true);
			break;
		}
		if ( !(bShift || bControl) )
		{
			if ( m_currentNode )
				m_currentNode->set_Expanded(true);
			break;
		}
		break;
	case VK_OEM_MINUS:
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
			m_currentNode->set_Expanded(false);
		break;
	case VK_ADD:
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
			m_currentNode->set_Expanded(true);
		break;
	case VK_SUBTRACT:
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
			m_currentNode->set_Expanded(false);
		break;
	case VK_MULTIPLY:
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
			m_currentNode->ExpandAll(true);
		break;
	case VK_DIVIDE:
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
			m_currentNode->ExpandAll(false);
		break;
	case VK_LEFT:
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
		{
			CTreeViewNode* parent = m_currentNode->get_Parent();

			if ( !parent )
				break;
			BeginUpdate();
			m_currentNode->set_Focused(false);
			m_currentNode = parent;
			m_currentNode->set_Focused(true);
			_EnsureVisible();
			EndUpdate(FALSE);
		}
		break;
	case VK_RIGHT:
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
		{
			if ( (m_currentNode->get_NodeCount() == 0) 
				|| (!(m_currentNode->is_Expanded())) )
				break;
			BeginUpdate();
			m_currentNode->set_Focused(false);
			m_currentNode = m_currentNode->get_Node(Cast(dword, 0));
			m_currentNode->set_Focused(true);
			_EnsureVisible();
			EndUpdate(FALSE);
		}
		break;
	case VK_DOWN:
		if ( bShift || bControl )
			break;
		_CurrentNodeDown();
		break;
	case VK_UP:
		if ( bShift || bControl )
			break;
		_CurrentNodeUp();
		break;
	case VK_ESCAPE:
		if ( bShift || bControl )
			break;
		break;
	case VK_INSERT:
		if ( !m_editable )
			break;
		if ( bShift || bControl )
			break;
		BeginUpdate();	
		if ( m_currentNode )
			m_currentNode->set_Focused(false);
		if ( m_currentNode->get_Parent() )
			m_currentNode = OK_NEW_OPERATOR CTreeViewNode(m_currentNode->get_Parent(), _T("<empty>"), 7);
		else
			m_currentNode = OK_NEW_OPERATOR CTreeViewNode(this, _T("<empty>"), 7);
		m_currentNode->set_Focused(true);
		_EnsureVisible();
		EndUpdate(TRUE);
		_ShowEditor();
		break;
	case VK_DELETE:
		if ( !m_editable )
			break;
		if ( bShift || bControl )
			break;
		if ( m_currentNode )
		{
			int ix = -1;
			CTreeViewNode* parent = m_currentNode->get_Parent();

			if ( parent )
				ix = parent->inx_Node(m_currentNode);
			else
				ix = inx_Node(m_currentNode);
			if ( ix < 0 )
				break;
			BeginUpdate();
			if ( parent )
			{
				parent->set_Node(ix, NULL);

				if ( (((dword)ix) < parent->get_NodeCount()) || ((ix > 0) && (((dword)--ix) < parent->get_NodeCount())) )
					m_currentNode = parent->get_Node(ix);
				else
					m_currentNode = parent;
				m_currentNode->set_Focused(true);
			}
			else
			{
				set_Node(ix, NULL);

				if ( (((dword)ix) < m_nodes.Count()) || ((ix > 0) && (((dword)--ix) < m_nodes.Count())) )
				{
					m_currentNode = get_Node(ix);
					m_currentNode->set_Focused(true);
				}
				else
					m_currentNode = NULL;
			}
			_EnsureVisible();
			EndUpdate(TRUE);
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CTreeView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
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

LRESULT CTreeView::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnHScroll(wParam, lParam) )
	{
		_CloseEditor(FALSE);
		Update(FALSE);
	}
	return 0;
}

LRESULT CTreeView::OnAboutHScroll(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	Update(FALSE);
	return 0;
}

LRESULT CTreeView::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnVScroll(wParam, lParam) )
	{                    
		_CloseEditor(FALSE);
		Update(FALSE);
	}
	return 0;
}

LRESULT CTreeView::OnAboutVScroll(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	Update(FALSE);
	return 0;
}

LRESULT CTreeView::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTreeView::OnEditorReturn(WPARAM wParam, LPARAM lParam)
{
	if ( !m_currentNode )
		return 1;

	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

	_CloseEditor();
	if ( bShift )
		_CurrentNodeUp();
	else
		_CurrentNodeDown();
	_ShowEditor();
	return 0;
}

LRESULT CTreeView::OnEditorTab(WPARAM wParam, LPARAM lParam)
{
	if ( !m_currentNode )
		return 1;

	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

	_CloseEditor();
	if ( bShift )
		_CurrentNodeUp();
	else
		_CurrentNodeDown();
	_ShowEditor();
	return 0;
}

LRESULT CTreeView::OnEditorFuncKey(WPARAM wParam, LPARAM lParam)
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

LRESULT CTreeView::OnEditorLostFocus(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	return 0;
}

LRESULT CTreeView::OnEditorEscape(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	return 0;
}

LRESULT CTreeView::OnEditorCursorUp(WPARAM wParam, LPARAM lParam)
{
	if ( !m_currentNode )
		return 1;

	_CloseEditor();
	_CurrentNodeUp();
	_ShowEditor();
	return 0;
}

LRESULT CTreeView::OnEditorCursorDown(WPARAM wParam, LPARAM lParam)
{
	if ( !m_currentNode )
		return 1;

	_CloseEditor();
	_CurrentNodeDown();
	_ShowEditor();
	return 0;
}

LRESULT CTreeView::OnEditUndoCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTreeView::OnEditRedoCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTreeView::OnEditCutCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTreeView::OnEditCopyCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTreeView::OnEditPasteCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTreeView::OnEditDeleteCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
