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
#include "PropertySheet.h"
#include "TextEditor.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"

#define SHIFTED 0x8000

CPropertySheetAbstractNode::CPropertySheetAbstractNode(CPropertySheet* pPropertySheet, TDiscriminator discriminator)
{
	m_propertySheet = pPropertySheet;
	m_discriminator = discriminator;
	SetRectEmpty(&m_borderRect);
}

CPropertySheetAbstractNode::~CPropertySheetAbstractNode(void)
{
}

CPropertySheetAbstractNode* CPropertySheetAbstractNode::HitTest(POINT pt, THitTest* pType, INT xPos, INT yPos, INT cBottom)
{
	return nullptr;
}

void CPropertySheetAbstractNode::OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxKeyWidth, LPINT maxValueWidth)
{
}

void CPropertySheetAbstractNode::OnAdjustRects(INT maxKeyWidth, INT maxValueWidth)
{
}

void CPropertySheetAbstractNode::OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom)
{
}

CPropertySheetNode::CPropertySheetNode(CPropertySheet* pPropertySheet, LPCTSTR keyStr, int keyLenCh, LPCTSTR valueStr, int valueLenCh):
	CPropertySheetAbstractNode(pPropertySheet)
{
	m_keyStr.SetString(__FILE__LINE__ keyStr, keyLenCh);
	m_valueStr.SetString(__FILE__LINE__ valueStr, valueLenCh);
	m_editable = false;
	m_modified = false;
	m_selected = false;
	m_focused = false;
	::SetRectEmpty(&m_keyRect);
	::SetRectEmpty(&m_valueRect);

	if ( m_propertySheet )
	{
		m_editable = m_propertySheet->is_Editable();
		m_propertySheet->set_Node(m_propertySheet->get_NodeCount(), this);
	}
}

CPropertySheetNode::CPropertySheetNode(CPropertySheet* pPropertySheet, const CStringBuffer& key, const CStringBuffer& value):
	CPropertySheetAbstractNode(pPropertySheet)
{
	m_keyStr = key;
	m_valueStr = value;
	m_editable = false;
	m_modified = false;
	m_selected = false;
	m_focused = false;
	::SetRectEmpty(&m_keyRect);
	::SetRectEmpty(&m_valueRect);

	if ( m_propertySheet )
	{
		m_editable = m_propertySheet->is_Editable();
		m_propertySheet->set_Node(m_propertySheet->get_NodeCount(), this);
	}
}

CPropertySheetNode::~CPropertySheetNode(void)
{
}

void CPropertySheetNode::set_KeyString(LPCTSTR pText, int lench)
{ 
	m_keyStr.SetString(__FILE__LINE__ pText,lench);
	if ( m_propertySheet )
		m_propertySheet->Update(TRUE);
}

void CPropertySheetNode::set_KeyString(const CStringBuffer& text)
{
	m_keyStr = text;
	if ( m_propertySheet )
		m_propertySheet->Update(TRUE);
}

void CPropertySheetNode::set_ValueString(LPCTSTR pText, int lench)
{ 
	m_valueStr.SetString(__FILE__LINE__ pText, lench);
	m_modified = true;
	if ( m_propertySheet )
		m_propertySheet->Update(TRUE);
}

void CPropertySheetNode::set_ValueString(const CStringBuffer& text)
{
	m_valueStr = text;
	m_modified = true;
	if ( m_propertySheet )
		m_propertySheet->Update(TRUE);
}

void CPropertySheetNode::set_Selected(bool selected)
{
	if ( m_selected != selected )
	{
		m_selected = selected;
		if ( m_propertySheet )
		{
			if ( m_selected )
				m_propertySheet->set_SelNode(m_propertySheet->get_SelNodeCount(), this);
			else
			{
				int ix = m_propertySheet->inx_SelNode(this);

				if ( ix > -1 )
					m_propertySheet->set_SelNode(ix, nullptr);
			}
			m_propertySheet->Update(FALSE);
		}
	}
}

void CPropertySheetNode::get_EditorRect(LPRECT rect)
{
	::SetRectEmpty(rect);
	if ( !m_propertySheet )
		return;

	RECT clientRect;
	RECT r;
	int xPos = m_propertySheet->get_HScrollOffset();
	int yPos = m_propertySheet->get_VScrollOffset();

	m_propertySheet->GetClientRect(&clientRect);
	::CopyRect(&r, &m_valueRect); ::OffsetRect(&r, -xPos, -yPos);
	IntersectRect(rect, &r, &clientRect);
}

CPropertySheetAbstractNode* CPropertySheetNode::HitTest(POINT pt, THitTest* pType, INT xPos, INT yPos, INT cBottom)
{
	if ( !pType )
		return nullptr;
	if ( !m_propertySheet )
		return nullptr;

	RECT keyRect; ::CopyRect(&keyRect, &m_keyRect); ::OffsetRect(&keyRect, -xPos, -yPos);
	RECT valueRect; ::CopyRect(&valueRect, &m_valueRect); ::OffsetRect(&valueRect, -xPos, -yPos);
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); ::OffsetRect(&borderRect, -xPos, -yPos);

	if ( borderRect.bottom < 0 )
		return nullptr;
	if ( borderRect.top >= cBottom )
		return nullptr;
	if ( (pt.y >= borderRect.top) && (pt.y < borderRect.bottom) && ((pt.x < keyRect.left) || (pt.x > valueRect.right)) )
		return nullptr;
	if ( PtInRect(&keyRect, pt) )
	{
		*pType = THitTestKey;
		return this;
	}
	if ( PtInRect(&valueRect, pt) )
	{
		*pType = THitTestValue;
		return this;
	}
	return nullptr;
}

void CPropertySheetNode::OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxKeyWidth, LPINT maxValueWidth)
{
	if ( !m_propertySheet )
		return;

	Gdiplus::Font* keyFont = m_propertySheet->get_Font(_T(".Key.Font.Normal"), _T("PropertySheet"));
	LPCTSTR valueSuffix = m_selected?_T(".Value.Font.Selected"):_T(".Value.Font.Normal");
	Gdiplus::Font* valueFont = m_propertySheet->get_Font(valueSuffix, _T("PropertySheet"));
	
	Gdiplus::PointF pt(Cast(Gdiplus::REAL, pRect->left), Cast(Gdiplus::REAL, pRect->top));
	Gdiplus::RectF keyRect;
	Gdiplus::RectF valueRect;
	Gdiplus::RectF borderRect;

	if ( m_keyStr.IsEmpty() )
	{
		keyRect.X = pt.X; keyRect.Y = pt.Y; keyRect.Height = keyFont->GetHeight(graphics); keyRect.Width = 10; 
	}
	else
	{
		graphics->MeasureString(m_keyStr.GetString(), -1, keyFont, pt, Gdiplus::StringFormat::GenericTypographic(), &keyRect);
		keyRect.Width += 10;
	}
	pt.X = keyRect.X + keyRect.Width;
	if ( m_valueStr.IsEmpty() )
	{
		valueRect.X = pt.X; valueRect.Y = pt.Y; valueRect.Height = valueFont->GetHeight(graphics); valueRect.Width = 10; 
	}
	else
	{
		if ( m_valueStr.GetLength() > 128 )
			graphics->MeasureString(m_valueStr.GetString(), 128, valueFont, pt, Gdiplus::StringFormat::GenericTypographic(), &valueRect);
		else
			graphics->MeasureString(m_valueStr.GetString(), -1, valueFont, pt, Gdiplus::StringFormat::GenericTypographic(), &valueRect);
		valueRect.Width += 10;
	}

	Gdiplus::REAL h = max(keyRect.Height, valueRect.Height);

	borderRect.X = keyRect.X; borderRect.Y = keyRect.Y; borderRect.Width = keyRect.Width + valueRect.Width + 8; borderRect.Height = h + 4;
	keyRect.X += 2;
	keyRect.Y += 2 + ((h - keyRect.Height) / 2);
	valueRect.X += 4;
	valueRect.Y += 2 + ((h - valueRect.Height) / 2);

	Convert2Rect(&m_keyRect, &keyRect);
	Convert2Rect(&m_valueRect, &valueRect);
	Convert2Rect(&m_borderRect, &borderRect);

	if ( m_keyRect.right > *maxKeyWidth )
		*maxKeyWidth = m_keyRect.right;
	if ( (m_valueRect.right - m_valueRect.left) > *maxValueWidth )
		*maxValueWidth = m_valueRect.right - m_valueRect.left;
	pRect->top += (m_borderRect.bottom - m_borderRect.top);
	pRect->bottom = 0;
}

void CPropertySheetNode::OnAdjustRects(INT maxKeyWidth, INT maxValueWidth)
{
	m_keyRect.right = maxKeyWidth;
	m_valueRect.left = maxKeyWidth + 4;
	m_valueRect.right = m_valueRect.left + maxValueWidth;
	m_borderRect.right = m_valueRect.right + 2;
}

void CPropertySheetNode::OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom)
{
	if ( !m_propertySheet )
		return;

	RECT keyRect; ::CopyRect(&keyRect, &m_keyRect); ::OffsetRect(&keyRect, -xPos, -yPos);
	RECT valueRect; ::CopyRect(&valueRect, &m_valueRect); ::OffsetRect(&valueRect, -xPos, -yPos);
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); ::OffsetRect(&borderRect, -xPos, -yPos);
	
	if ( borderRect.bottom < 0 )
		return;
	if ( borderRect.top >= cBottom )
		return;

	Gdiplus::RectF keyRectF; Convert2RectF(&keyRectF, &keyRect);
	Gdiplus::RectF valueRectF; Convert2RectF(&valueRectF, &valueRect);
	Gdiplus::RectF borderRectF; Convert2RectF(&borderRectF, &borderRect);

	Gdiplus::Brush* keyBackground = m_propertySheet->get_Brush(_T(".Key.BackgroundColor.Normal"), _T("PropertySheet"), Gdiplus::Color::LightGray);
	Gdiplus::Brush* keyForeground = m_propertySheet->get_Brush(_T(".Key.ForegroundColor.Normal"), _T("PropertySheet"), Gdiplus::Color::Black);

	graphics->FillRectangle(keyBackground, keyRectF);
	if ( !(m_keyStr.IsEmpty()) )
	{
		Gdiplus::Font* keyFont = m_propertySheet->get_Font(_T(".Key.Font.Normal"), _T("PropertySheet"));
		Gdiplus::PointF pt(keyRectF.X, keyRectF.Y);

		graphics->DrawString(m_keyStr.GetString(), -1, keyFont, keyRectF, Gdiplus::StringFormat::GenericTypographic(), keyForeground);
	}

	LPCTSTR suffix;
	Gdiplus::Brush* valueBackground;
	Gdiplus::Brush* valueForeground;

	suffix = m_selected?_T(".Value.BackgroundColor.Selected"):_T(".Value.BackgroundColor.Normal");
	valueBackground = m_propertySheet->get_Brush(suffix, _T("PropertySheet"), m_selected?Gdiplus::Color::Blue:Gdiplus::Color::White);
	suffix = m_selected?_T(".Value.ForegroundColor.Selected"):_T(".Value.ForegroundColor.Normal");
	valueForeground = m_propertySheet->get_Brush(suffix, _T("PropertySheet"), m_selected?Gdiplus::Color::White:Gdiplus::Color::Black);

	valueRectF.X -= 1;
	valueRectF.Width += 3;
	valueRectF.Y -= 1;
	valueRectF.Height += 3;
	graphics->FillRectangle(valueBackground, valueRectF);
	valueRectF.Width -= 1;
	valueRectF.Height -= 1;
	if ( !(m_valueStr.IsEmpty()) )
	{
		LPCTSTR valueSuffix = m_selected?_T(".Value.Font.Selected"):_T(".Value.Font.Normal");
		Gdiplus::Font* valueFont = m_propertySheet->get_Font(valueSuffix, _T("PropertySheet"));

		if ( m_valueStr.GetLength() > 128 )
			graphics->DrawString(m_valueStr.GetString(), 128, valueFont, valueRectF, Gdiplus::StringFormat::GenericTypographic(), valueForeground);
		else
			graphics->DrawString(m_valueStr.GetString(), -1, valueFont, valueRectF, Gdiplus::StringFormat::GenericTypographic(), valueForeground);
	}

	Gdiplus::Pen grayPen(Gdiplus::Color::Gray);

	graphics->DrawLine(&grayPen, borderRectF.X + borderRectF.Width, borderRectF.Y, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
	graphics->DrawLine(&grayPen, borderRectF.X + keyRectF.Width + 4, borderRectF.Y, borderRectF.X + keyRectF.Width + 4, borderRectF.Y + borderRectF.Height);
	graphics->DrawLine(&grayPen, borderRectF.X, borderRectF.Y + borderRectF.Height, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);

	if ( m_focused )
	{
		grayPen.SetDashStyle(Gdiplus::DashStyleDot);

		graphics->DrawRectangle(&grayPen, valueRectF);
	}
}

CPropertySheetTitleNode::CPropertySheetTitleNode(CPropertySheet* pPropertySheet, LPCTSTR pText, int lench):
    CPropertySheetAbstractNode(pPropertySheet, TDiscriminatorTitle)
{
	m_text.SetString(__FILE__LINE__ pText, lench);
	::SetRectEmpty(&m_textRect);

	if ( m_propertySheet )
	{
		m_propertySheet->set_Node(m_propertySheet->get_NodeCount(), this);
		m_propertySheet->Update(TRUE);
	}
}

CPropertySheetTitleNode::CPropertySheetTitleNode(CPropertySheet* pPropertySheet, ConstRef(CStringBuffer) _text):
    CPropertySheetAbstractNode(pPropertySheet, TDiscriminatorTitle)
{
	m_text = _text;
	::SetRectEmpty(&m_textRect);

	if ( m_propertySheet )
	{
		m_propertySheet->set_Node(m_propertySheet->get_NodeCount(), this);
		m_propertySheet->Update(TRUE);
	}
}

CPropertySheetTitleNode::~CPropertySheetTitleNode(void)
{
}

void CPropertySheetTitleNode::set_Text(LPCTSTR pText, int lench)
{ 
	m_text.SetString(__FILE__LINE__ pText, lench);
	if ( m_propertySheet )
		m_propertySheet->Update(TRUE);
}

void CPropertySheetTitleNode::set_Text(ConstRef(CStringBuffer) _text)
{
	m_text = _text;
	if ( m_propertySheet )
		m_propertySheet->Update(TRUE);
}

void CPropertySheetTitleNode::OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxKeyWidth, LPINT maxValueWidth)
{
	if ( !m_propertySheet )
		return;

	Gdiplus::Font* pFont = m_propertySheet->get_Font(_T(".Title.Font.Normal"), _T("PropertySheet"));
	Gdiplus::PointF pt(Cast(Gdiplus::REAL, pRect->left), Cast(Gdiplus::REAL, pRect->top));
	Gdiplus::RectF textRect;
	Gdiplus::RectF borderRect;

	if ( m_text.IsEmpty() )
	{
		textRect.X = pt.X; textRect.Y = pt.Y; textRect.Height = pFont->GetHeight(graphics); textRect.Width = 10;
	}
	else
		graphics->MeasureString(m_text.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &textRect);

	borderRect.X = textRect.X; borderRect.Y = textRect.Y; borderRect.Width = textRect.Width + 4; borderRect.Height = textRect.Height + 4;
	textRect.X += 2;
	textRect.Y += 2;

	Convert2Rect(&m_textRect, &textRect);
	Convert2Rect(&m_borderRect, &borderRect);

	pRect->top += (m_borderRect.bottom - m_borderRect.top);
	pRect->bottom = 0;
}

void CPropertySheetTitleNode::OnAdjustRects(INT maxKeyWidth, INT maxValueWidth)
{
	m_textRect.right = maxKeyWidth + 4 + maxValueWidth;
	m_borderRect.right = m_textRect.right + 2;
}

void CPropertySheetTitleNode::OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom)
{
	if ( !m_propertySheet )
		return;

	RECT textRect; ::CopyRect(&textRect, &m_textRect); ::OffsetRect(&textRect, -xPos, -yPos);
	RECT borderRect; ::CopyRect(&borderRect, &m_borderRect); ::OffsetRect(&borderRect, -xPos, -yPos);

	if ( borderRect.bottom < 0 )
		return;
	if ( borderRect.top >= cBottom )
		return;

	Gdiplus::Font* pFont = m_propertySheet->get_Font(_T(".Title.Font.Normal"), _T("PropertySheet"));
	Gdiplus::RectF textRectF;
	Gdiplus::RectF borderRectF;

	Convert2RectF(&textRectF, &textRect);
	Convert2RectF(&borderRectF, &borderRect);

	Gdiplus::Brush* brushBackground = m_propertySheet->get_Brush(_T(".Title.BackgroundColor.Normal"), _T("PropertySheet"), Gdiplus::Color::LightGray);
	Gdiplus::Brush* brushForeground = m_propertySheet->get_Brush(_T(".Title.ForegroundColor.Normal"), _T("PropertySheet"), Gdiplus::Color::Black);

	graphics->FillRectangle(brushBackground, borderRectF);
	if ( !(m_text.IsEmpty()) )
		graphics->DrawString(m_text.GetString(), -1, pFont, textRectF, Gdiplus::StringFormat::GenericTypographic(), brushForeground);

	Gdiplus::Pen grayPen(Gdiplus::Color::Gray);

	graphics->DrawLine(&grayPen, borderRectF.X + borderRectF.Width, borderRectF.Y, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
	graphics->DrawLine(&grayPen, borderRectF.X, borderRectF.Y + borderRectF.Height, borderRectF.X + borderRectF.Width, borderRectF.Y + borderRectF.Height);
}

BEGIN_MESSAGE_MAP(CControl, CPropertySheet)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MESSAGE(WM_GETDLGCODE,OnGetDlgCode)
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

CPropertySheet::CPropertySheet(LPCTSTR name):
    CControl(name),
	m_nodes(__FILE__LINE__ 64, 64),
	m_selNodes(__FILE__LINE__ 64, 64),
	m_currentNode(nullptr),
	m_multiSelect(false),
	m_editable(false),
	m_editor(nullptr)
{
}

CPropertySheet::CPropertySheet(ConstRef(CStringBuffer) name):
    CControl(name),
	m_nodes(__FILE__LINE__ 64, 64),
	m_selNodes(__FILE__LINE__ 64, 64),
	m_currentNode(nullptr),
	m_multiSelect(false),
	m_editable(false),
	m_editor(nullptr)
{
}

CPropertySheet::~CPropertySheet(void)
{
}


CPropertySheetAbstractNode* CPropertySheet::get_Node(LPCTSTR key)
{
	CPropertySheetNodeVector::Iterator it = m_nodes.Begin();
	CPropertySheetAbstractNode* result = nullptr;
	CPropertySheetAbstractNode* node = nullptr;
	CPropertySheetNode* node1 = nullptr;

	while ( it )
	{
		node = *it;
		if ( node->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
		{
			node1 = CastDynamicPtr(CPropertySheetNode, node);
			if ( node1->get_KeyString().Compare(CStringLiteral(key), 0, CStringLiteral::cIgnoreCase) == 0 )
			{
				result = node;
				break;
			}
		}
		++it;
	}
	return result;
}

void CPropertySheet::set_Node(dword ix, CPropertySheetAbstractNode* node)
{
	if ( !node )
	{
		if ( ix >= m_nodes.Count() )
			return;
		BeginUpdate();

		CPropertySheetNodeVector::Iterator it = m_nodes.Index(ix);
		CPropertySheetAbstractNode* node1 = *it;

		if ( node1->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
		{
			CPropertySheetNode* node2 = CastDynamicPtr(CPropertySheetNode, node1);

			if ( m_currentNode == node2 )
				m_currentNode = nullptr;
			if ( node2->is_Selected() )
				node2->set_Selected(false);
		}
		m_nodes.Remove(it);
		EndUpdate(TRUE);
	}
	else if ( ix >= m_nodes.Count() )
	{
		m_nodes.Append(node);
		Update(TRUE);
	}
	else
	{
		BeginUpdate();

		CPropertySheetNodeVector::Iterator it = m_nodes.Index(ix);
		CPropertySheetAbstractNode* node1 = *it;

		if ( node1->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
		{
			CPropertySheetNode* node2 = CastDynamicPtr(CPropertySheetNode, node1);

			if ( m_currentNode == node2 )
				m_currentNode = nullptr;
			if ( node2->is_Selected() )
				node2->set_Selected(false);
		}
		m_nodes.SetData(it, node);
		EndUpdate(TRUE);
	}
}

int CPropertySheet::inx_Node(CPropertySheetAbstractNode* node)
{
	CPropertySheetNodeVector::Iterator it = m_nodes.Begin();
	int ix = 0;

	while ( it )
	{
		if ( *it == node )
			return ix;
		++it;
		++ix;
	}
	return -1;
}

CStringBuffer CPropertySheet::get_NodeValueString(LPCTSTR key)
{
	CStringBuffer result;
	CPropertySheetAbstractNode* node = get_Node(key);
	CPropertySheetNode* node1 = nullptr;

	if ( node && (node->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode ) )
		node1 = CastDynamicPtr(CPropertySheetNode, node);
	if ( node1 )
		result = node1->get_ValueString();
	return result;
}

void CPropertySheet::set_NodeValueString(LPCTSTR key, LPCTSTR data, int lenCh)
{
	CPropertySheetAbstractNode* node = get_Node(key);
	CPropertySheetNode* node1 = nullptr;

	if ( node && (node->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode ) )
		node1 = CastDynamicPtr(CPropertySheetNode, node);
	if ( node1 )
		node1->set_ValueString(data, lenCh);
}

void CPropertySheet::set_NodeValueString(LPCTSTR key, ConstRef(CStringBuffer) data)
{
	CPropertySheetAbstractNode* node = get_Node(key);
	CPropertySheetNode* node1 = nullptr;

	if ( node && (node->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode ) )
		node1 = CastDynamicPtr(CPropertySheetNode, node);
	if ( node1 )
		node1->set_ValueString(data);
}

void CPropertySheet::set_SelNode(dword ix, CPropertySheetAbstractNode* node)
{
	if ( !node )
	{
		if ( ix >= m_selNodes.Count() )
			return;

		CPropertySheetSelectedNodeVector::Iterator it = m_selNodes.Index(ix);

		m_selNodes.Remove(it);
	}
	else if ( ix == m_selNodes.Count() )
		m_selNodes.Append(node);
	else
	{
		if ( ix >= m_selNodes.Count() )
			return;

		CPropertySheetSelectedNodeVector::Iterator it = m_selNodes.Index(ix);

		m_selNodes.SetData(it, node);
	}
}

int CPropertySheet::inx_SelNode(CPropertySheetAbstractNode* node)
{
	CPropertySheetNodeVector::Iterator it = m_selNodes.Begin();
	int ix = 0;

	while ( it )
	{
		if ( *it == node )
			return ix;
		++it;
		++ix;
	}
	return -1;
}


bool CPropertySheet::set_Editable(LPCTSTR key, bool editable)
{
	CPropertySheetAbstractNode* node = get_Node(key);

	if ( (!node) || (node->get_Discriminator() != CPropertySheetAbstractNode::TDiscriminatorNode) )
		return false;

	CPropertySheetNode* node1 = CastDynamicPtr(CPropertySheetNode, node);

	if ( !node1 )
		return false;

	node1->set_Editable(editable);

	return true;
}

void CPropertySheet::set_MinimumValueWidth(LONG v)
{
	m_minValueWidth = v;
	if ( is_created() )
		Update(TRUE);
}

BOOL CPropertySheet::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CPROPERTYSHEET");
	return TRUE;
}

void CPropertySheet::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".Color.Workspace"), _T("PropertySheet"));
	add_BrushKeys(_keys, _T(".Key.BackgroundColor.Normal"), _T("PropertySheet"));
	add_BrushKeys(_keys, _T(".Key.ForegroundColor.Normal"), _T("PropertySheet"));
	add_BrushKeys(_keys, _T(".Value.BackgroundColor.Selected"), _T("PropertySheet"));
	add_BrushKeys(_keys, _T(".Value.BackgroundColor.Normal"), _T("PropertySheet"));
	add_BrushKeys(_keys, _T(".Value.ForegroundColor.Selected"), _T("PropertySheet"));
	add_BrushKeys(_keys, _T(".Value.ForegroundColor.Normal"), _T("PropertySheet"));
}
	
void CPropertySheet::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".Key.Font.Normal"), _T("PropertySheet"));
	add_FontKeys(_keys, _T(".Value.Font.Selected"), _T("PropertySheet"));
	add_FontKeys(_keys, _T(".Value.Font.Normal"), _T("PropertySheet"));
}

void CPropertySheet::Refresh()
{
	Update(TRUE);
}

CWin* CPropertySheet::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CPropertySheet();

	return pWin;
}

void CPropertySheet::SelectAll(bool selected)
{
	BeginUpdate();
	if ( selected )
	{
		CPropertySheetNodeVector::Iterator it = m_nodes.Begin();

		while ( it )
		{
			CPropertySheetAbstractNode* node = *it;

			if ( node->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
			{
				CPropertySheetNode* node1 = CastDynamicPtr(CPropertySheetNode, node);

				node1->set_Selected(true);
			}
			++it;
		}
	}
	else
	{
		while ( m_selNodes.Count() )
			m_selNodes.Remove(m_selNodes.Last());
	}
	EndUpdate(TRUE);
}

void CPropertySheet::set_CurrentNode(CPropertySheetNode* node)
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

void CPropertySheet::_ItemDown()
{
	if ( !m_currentNode )
		return;

	int ix = inx_Node(m_currentNode);
	bool bContinue = true;

	if ( ix < 0 )
		return;

	if ( Cast(dword, ix) == (m_nodes.Count() - 1) )
		ix = 0;
	else
		++ix;
	BeginUpdate();
	m_currentNode->set_Focused(false);
	while ( Cast(dword, ix) < m_nodes.Count() )
	{
		CPropertySheetAbstractNode* node = *(m_nodes.Index(ix));

		if ( node->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
		{
			m_currentNode = CastDynamicPtr(CPropertySheetNode, node);
			m_currentNode->set_Focused(true);
			break;
		}
		++ix;
		if ( Cast(dword, ix) == m_nodes.Count() )
		{
			if ( !bContinue )
				break;
			bContinue = false;
			ix = 0;
		}
	}
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CPropertySheet::_ItemUp()
{
	if ( !m_currentNode )
		return;
	
	int ix = inx_Node(m_currentNode);
	bool bContinue = true;

	if ( ix < 0 )
		return;

	if ( ix == 0 )
		ix = m_nodes.Count() - 1;
	else
		--ix;
	BeginUpdate();
	m_currentNode->set_Focused(false);
	while ( ix >= 0 )
	{
		CPropertySheetAbstractNode* node = *(m_nodes.Index(ix));

		if ( node->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
		{
			m_currentNode = CastDynamicPtr(CPropertySheetNode, node);
			m_currentNode->set_Focused(true);
			break;
		}
		--ix;
		if ( ix <= 0 )
		{
			if ( !bContinue )
				break;
			bContinue = false;
			ix = m_nodes.Count() - 1;
		}
	}
	_EnsureVisible();
	EndUpdate(FALSE);
}

void CPropertySheet::_ShowEditor()
{
	if ( (!m_editable) || (!m_currentNode) || (!(m_currentNode->is_Editable())) )
		return;

	_EnsureVisible();
	if ( !m_editor )
	{
		CStringBuffer name(m_name);

		name += _T(".DefaultTextEditor");
		m_editor = OK_NEW_OPERATOR CTextEditor(name);
		m_editor->Create(m_hwnd);
	}
	else if ( !(m_editor->is_created()) )
		m_editor->Create(m_hwnd);
	else
		m_editor->set_Visible(TRUE);

	RECT valueRect;
	CStringBuffer value = m_currentNode->get_ValueString();

	m_currentNode->get_EditorRect(&valueRect);
	if ( ::IsRectEmpty(&valueRect) )
		return;
	valueRect.left -= 1;
	valueRect.right += 2;
	valueRect.top -= 1;
	valueRect.bottom += 2;
	m_editor->MoveWindow(&valueRect, FALSE);

	m_editor->set_windowtext(value);
	m_editor->set_focus();
}

void CPropertySheet::_CloseEditor(BOOL bSave)
{
	if ( (!m_editable) || (!m_currentNode) || (!(m_currentNode->is_Editable())) )
		return;
	if ( (!m_editor) || (!(m_editor->is_Visible())) )
		return;
	if ( bSave )
		m_currentNode->set_ValueString(m_editor->get_windowtext());
	m_editor->set_Visible(FALSE);
	set_focus();
}

void CPropertySheet::_EnsureVisible()
{
	if ( !m_currentNode )
		return;

	bool bUpdate = false;

	if ( get_VScrollEnabled() || get_HScrollEnabled() )
	{
		if ( get_VScrollVisible() )
		{
			RECT rect;
			RECT nodeR;

			GetClientRect(&rect);
			m_currentNode->get_Rect(&nodeR);
			set_VScrollOffset(nodeR.top - (rect.bottom / 2));
			bUpdate = true;
		}
		if ( get_HScrollVisible() )
		{
			RECT valR;

			m_currentNode->get_ValueRect(&valR);
			set_HScrollOffset(valR.left);
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

LRESULT CPropertySheet::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return -1;

	Gdiplus::Graphics graphics(hdc);
	CPropertySheetNodeVector::Iterator it;

	if ( !m_currentNode )
	{
		it = m_nodes.Begin();
		while ( it )
		{
			CPropertySheetAbstractNode* node = *it;

			if ( node->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
			{
				CPropertySheetNode* node1 = CastDynamicPtr(CPropertySheetNode, node);

				if ( node1 )
				{
					++m_update;
					node1->set_Focused(true);
					--m_update;
					m_currentNode = node1;
					break;
				}
			}
			++it;
		}
	}
	if ( m_updateRects )
	{
		INT maxKeyWidth = 0;
		INT maxValueWidth = m_minValueWidth;
		RECT svmaxClientArea;

		::CopyRect(&svmaxClientArea, &m_maxClientArea);
		::SetRectEmpty(&m_maxClientArea);
		it = m_nodes.Begin();
		while ( it )
		{
			(*it)->OnCalcRects(&graphics, &m_maxClientArea, &maxKeyWidth, &maxValueWidth);
			++it;
		}
		it = m_nodes.Begin();
		while ( it )
		{
			CPropertySheetAbstractNode* pNode = *it;

			if ( pNode->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorTitle )
			{
				RECT r;

				pNode->get_Rect(&r);
				if ( (r.right - r.left) > (maxKeyWidth + maxValueWidth) )
				{
					LONG delta = (r.right - r.left) - (maxKeyWidth + maxValueWidth);

					maxKeyWidth += delta / 2;
					maxValueWidth += (delta + 1) / 2;
				}
			}
			++it;
		}
		it = m_nodes.Begin();
		while ( it )
		{
			(*it)->OnAdjustRects(maxKeyWidth, maxValueWidth);
			++it;
		}
		m_maxClientArea.bottom = m_maxClientArea.top;
		m_maxClientArea.top = 0;
		m_maxClientArea.right = maxKeyWidth + maxValueWidth + 8;
		m_maxClientArea.left = 0;
		ShowHorzScrollBar();
		ShowVertScrollBar();
		if ( get_VScrollVisible() )
		{
			m_maxClientArea.bottom += 100;
			ShowVertScrollBar();
		}
		if ( get_HScrollVisible() )
		{
			m_maxClientArea.right += 50;
			ShowHorzScrollBar();
		}
		if ( (svmaxClientArea.bottom != m_maxClientArea.bottom) || (svmaxClientArea.right != m_maxClientArea.right) )
			get_parent()->SendMessage(WM_SIZE, 0, 0);
		m_updateRects = FALSE;
	}

	RECT clientRect;

	GetClientRect(&clientRect);
	Gdiplus::Bitmap bitmap(clientRect.right, clientRect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);

	Gdiplus::Brush* brush = get_Brush(_T(".Color.Workspace"), _T("PropertySheet"), Gdiplus::Color::Gray);

	bitmapgraphics.FillRectangle(brush, 0, 0, clientRect.right, clientRect.bottom);

	int xPos = get_HScrollOffset();
	int yPos = get_VScrollOffset();

	it = m_nodes.Begin();
	while ( it )
	{
		(*it)->OnPaint(&bitmapgraphics, xPos, yPos, clientRect.bottom);
		++it;
	}
	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);
	return 0;
}

LRESULT CPropertySheet::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPropertySheet::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ( !(is_created()) )
		return -1;
	set_focus();
	_CloseEditor();

	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	CPropertySheetAbstractNode::THitTest type;
	CPropertySheetNodeVector::Iterator it;
	CPropertySheetAbstractNode* node = nullptr;
	CPropertySheetNode* node1 = nullptr;
	int xPos = get_HScrollOffset();
	int yPos = get_VScrollOffset();
	RECT clientRect;

	GetClientRect(&clientRect);
	it = m_nodes.Begin();
	while ( it )
	{
		node = (*it)->HitTest(pt, &type, xPos, yPos, clientRect.bottom);
		if ( node )
			break;
		++it;
	}
	if ( node )
	{
		node1 = CastDynamicPtr(CPropertySheetNode, node);
		if ( (type == CPropertySheetAbstractNode::THitTestKey) || (type == CPropertySheetAbstractNode::THitTestValue) )
		{
			if ( (!m_multiSelect) || (wParam == MK_LBUTTON) )
			{
				BeginUpdate();
				SelectAll(false);
				if ( m_currentNode )
					m_currentNode->set_Focused(false);
				m_currentNode = node1;
				m_currentNode->set_Focused(true);
				EndUpdate(FALSE);
			}
			else if ( wParam == (MK_LBUTTON | MK_CONTROL) )
			{
				BeginUpdate();
				if ( m_currentNode )
					m_currentNode->set_Focused(false);
				m_currentNode = node1;
				m_currentNode->set_Focused(true);
				m_currentNode->set_Selected(!(m_currentNode->is_Selected()));
				EndUpdate(TRUE);
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
				m_currentNode->set_Focused(false);
				m_currentNode = node1;
				m_currentNode->set_Focused(true);
				if ( ix1 == ix2 )
				{
					m_currentNode->set_Selected(true);
				}
				else if ( ix1 < ix2 )
				{
					for ( ; ix1 <= ix2; ++ix1 )
					{
						CPropertySheetAbstractNode* node2 = *(m_nodes.Index(ix1));

						if ( node2->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
						{
							CPropertySheetNode* node3 = CastDynamicPtr(CPropertySheetNode, node2);

							node3->set_Selected(true);
						}
					}
				}
				else
				{
					for ( ; ix2 <= ix1; ++ix2 )
					{
						CPropertySheetAbstractNode* node2 = *(m_nodes.Index(ix2));

						if ( node2->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
						{
							CPropertySheetNode* node3 = CastDynamicPtr(CPropertySheetNode, node2);

							node3->set_Selected(true);
						}
					}
				}
				EndUpdate(TRUE);
			}
		}
	}
	return 0;
}

LRESULT CPropertySheet::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		CPropertySheetAbstractNode::THitTest type;
		CPropertySheetNodeVector::Iterator it;
		CPropertySheetAbstractNode* node = nullptr;
		CPropertySheetNode* node1 = nullptr;
		int xPos = get_HScrollOffset();
		int yPos = get_VScrollOffset();
		RECT clientRect;

		GetClientRect(&clientRect);
		it = m_nodes.Begin();
		while ( it )
		{
			node = (*it)->HitTest(pt, &type, xPos, yPos, clientRect.bottom);
			if ( node )
				break;
			++it;
		}
		if ( node )
			_ShowEditor();
	}
	return 0;
}

LRESULT CPropertySheet::OnKeyDown(WPARAM wParam, LPARAM lParam)
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
		if ( bControl || (!m_currentNode) )
			break;
		if ( m_editable && (m_currentNode->is_Editable()) )
			_ShowEditor();
		else if ( bShift )
			_ItemUp();
		else
			_ItemDown();
		break;
	case VK_DOWN:
		if ( bShift || bControl )
			break;
		_ItemDown();
		break;
	case VK_UP:
		if ( bShift || bControl )
			break;
		_ItemUp();
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
		break;
	case VK_DELETE:
		if ( !m_editable )
			break;
		if ( bShift || bControl )
			break;
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CPropertySheet::OnChar(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPropertySheet::OnMouseWheel(WPARAM wParam, LPARAM lParam)
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

LRESULT CPropertySheet::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnHScroll(wParam, lParam) )
	{
		_CloseEditor(FALSE);
		Update(FALSE);
	}
	return 0;
}

LRESULT CPropertySheet::OnAboutHScroll(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	Update(FALSE);
	return 0;
}

LRESULT CPropertySheet::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnVScroll(wParam, lParam) )
	{
		_CloseEditor(FALSE);
		Update(FALSE);
	}
	return 0;
}

LRESULT CPropertySheet::OnAboutVScroll(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	Update(FALSE);
	return 0;
}

LRESULT CPropertySheet::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPropertySheet::OnGetDlgCode(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case VK_F2:
	case VK_RETURN:
	case VK_ESCAPE:
	case VK_INSERT:
	case VK_DELETE:
		return DLGC_WANTMESSAGE;
	case VK_DOWN:
	case VK_UP:
		return DLGC_WANTARROWS;
	case VK_LEFT:
	case VK_RIGHT:
	case VK_TAB:
		return 0;
	default:
		break;
	}
	return 0;
}

LRESULT CPropertySheet::OnEditorReturn(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( !m_currentNode )
		return 0;
	if ( bControl )
		return 0;

	_CloseEditor();
	if ( bShift )
		_ItemUp();
	else
		_ItemDown();
	_ShowEditor();
	return 0;
}

LRESULT CPropertySheet::OnEditorTab(WPARAM wParam, LPARAM lParam)
{
	return OnEditorReturn(wParam, lParam);
}

LRESULT CPropertySheet::OnEditorFuncKey(WPARAM wParam, LPARAM lParam)
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

LRESULT CPropertySheet::OnEditorLostFocus(WPARAM wParam, LPARAM lParam)
{
	// _CloseEditor(FALSE);
	return 0;
}

LRESULT CPropertySheet::OnEditorEscape(WPARAM wParam, LPARAM lParam)
{
	_CloseEditor(FALSE);
	return 0;
}

LRESULT CPropertySheet::OnEditorCursorUp(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( !m_currentNode )
		return 0;
	if ( bControl || bShift )
		return 0;

	_CloseEditor();
	_ItemUp();
	_ShowEditor();
	return 0;
}

LRESULT CPropertySheet::OnEditorCursorDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	if ( !m_currentNode )
		return 0;
	if ( bControl || bShift )
		return 0;

	_CloseEditor();
	_ItemDown();
	_ShowEditor();
	return 0;
}

LRESULT CPropertySheet::OnEditUndoCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPropertySheet::OnEditRedoCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPropertySheet::OnEditCutCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPropertySheet::OnEditCopyCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPropertySheet::OnEditPasteCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPropertySheet::OnEditDeleteCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

