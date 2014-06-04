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
#include "ComboBox.h"
#include "ListView.h"
#include "TextEditor.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"

#define SHIFTED 0x8000

class CComboPopup: public CPopup
{
public:
    CComboPopup(LPCTSTR name = nullptr);
    CComboPopup(ConstRef(CStringBuffer) name);
	virtual ~CComboPopup();

	__inline CComboBox* get_ComboBox() { return m_pComboBox; }
	__inline void set_ComboBox(CComboBox* pComboBox) { m_pComboBox = pComboBox; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	__inline dword get_NodeCount() { if ( m_pListView ) return m_pListView->get_NodeCount(); return 0; }
	__inline CListViewNode* get_Node(dword ix) { if ( !m_pListView ) return nullptr; return m_pListView->get_Node(ix); }
	__inline CListViewNode* get_Node(LPCTSTR key) { if ( !m_pListView ) return nullptr; return m_pListView->get_Node(key); }
	__inline void set_Node(dword ix, CListViewNode* node) { if ( m_pListView ) m_pListView->set_Node(ix, node); }
	__inline void ins_Node(dword ix, CListViewNode* node, bool before = true) { if ( m_pListView ) m_pListView->ins_Node(ix, node, before); }
	__inline int inx_Node(CListViewNode* node) { if ( m_pListView ) return m_pListView->inx_Node(node); return -1; }
	__inline void clear(void) { if ( m_pListView ) m_pListView->clear(); }
	__inline CListViewNode* create_Node(LPCTSTR text) { if ( m_pListView ) return OK_NEW_OPERATOR CListViewNode(m_pListView, text); return nullptr; }
	__inline CListViewNode* create_Node(ConstRef(CStringBuffer) text) { if ( m_pListView ) return OK_NEW_OPERATOR CListViewNode(m_pListView, text); return nullptr; }

	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);

	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);

	LRESULT OnListViewNodeFocused(WPARAM wParam, LPARAM lParam);
	LRESULT OnListViewNodeEntered(WPARAM wParam, LPARAM lParam);
	LRESULT OnListViewEscape(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	CListView* m_pListView;
	CScrollbar* m_pHScrollbar;
	CScrollbar* m_pVScrollbar;
	CComboBox* m_pComboBox;
};

BEGIN_MESSAGE_MAP(CPopup, CComboPopup)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
	ON_WM_NOTIFY(NM_LISTVIEWNODEFOCUSED,OnListViewNodeFocused)
	ON_WM_NOTIFY(NM_LISTVIEWNODEENTERED,OnListViewNodeEntered)
	ON_WM_NOTIFY(NM_LISTVIEWESCAPE,OnListViewEscape)
END_MESSAGE_MAP()

CComboPopup::CComboPopup(LPCTSTR name):
	CPopup(name),
	m_pListView(nullptr),
	m_pHScrollbar(nullptr),
	m_pVScrollbar(nullptr),
	m_pComboBox(nullptr)
{
}

CComboPopup::CComboPopup(ConstRef(CStringBuffer) name):
	CPopup(name),
	m_pListView(nullptr),
	m_pHScrollbar(nullptr),
	m_pVScrollbar(nullptr),
	m_pComboBox(nullptr)
{
}

CComboPopup::~CComboPopup()
{
}

BOOL CComboPopup::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName   = _T("CCOMBOPOPUP");
	return TRUE;
}

BOOL CComboPopup::PreCreate(CREATESTRUCT& cs)
{
	cs.style = WS_POPUP;
	cs.dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	return TRUE;
}

LRESULT CComboPopup::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer name(get_name());

	name.AppendString(_T(".HScrollbar"));
	m_pHScrollbar = OK_NEW_OPERATOR CScrollbar(name);
	m_pHScrollbar->set_Direction(CScrollbar::TDirectionHorizontal);
	m_pHScrollbar->set_SizeSpecX(CControl::TSizeSpecifierRelativePercentSize);
	m_pHScrollbar->set_LayoutSizeX(100);
	m_pHScrollbar->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	m_pHScrollbar->set_LayoutSizeY(17);
	m_pHScrollbar->set_LayoutY(1);
	m_pHScrollbar->set_CreateVisible(FALSE);
	m_pHScrollbar->Create(CastAny(HWND, wParam), 1001);

	name = get_name();
	name.AppendString(_T(".VScrollbar"));
	m_pVScrollbar = OK_NEW_OPERATOR CScrollbar(name);
	m_pVScrollbar->set_Direction(CScrollbar::TDirectionVertical);
	m_pVScrollbar->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	m_pVScrollbar->set_LayoutSizeX(17);
	m_pVScrollbar->set_SizeSpecY(CControl::TSizeSpecifierRelativePercentSize);
	m_pVScrollbar->set_LayoutSizeY(100);
	m_pVScrollbar->set_LayoutX(1);
	m_pVScrollbar->set_CreateVisible(FALSE);
	m_pVScrollbar->Create(CastAny(HWND, wParam), 1002);

	name = get_name();
	name.AppendString(_T(".ListView"));
	m_pListView = OK_NEW_OPERATOR CListView(name);
	m_pListView->set_SizeSpecX(CControl::TSizeSpecifierRelativePercentSize);
	m_pListView->set_LayoutSizeX(100);
	m_pListView->set_SizeSpecY(CControl::TSizeSpecifierRelativePercentSize);
	m_pListView->set_LayoutSizeY(100);
	m_pListView->set_HScrollEnabled(TRUE);
	m_pListView->set_UserHScroll(m_pHScrollbar->get_handle());
	m_pListView->set_VScrollEnabled(TRUE);
	m_pListView->set_UserVScroll(m_pVScrollbar->get_handle());

	name.AppendString(_T(".Color.Workspace"));
	if ( !(theGuiApp->get_BrushManager()->has_Brush(name)) )
		theGuiApp->get_BrushManager()->set_Brush(name, m_pListView->get_Brush(_T(".BackgroundColor.Normal"), _T("ListView"), Gdiplus::Color::White));
	m_pListView->Create(CastAny(HWND, wParam), 1000);
	m_pHScrollbar->set_ScrollWindow(m_pListView);
	m_pVScrollbar->set_ScrollWindow(m_pListView);
	return 0;
}

LRESULT CComboPopup::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ( (wParam == MK_LBUTTON) && has_Capture() )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		RECT r;

		ClientToScreen(&pt);
		m_pListView->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pListView->ScreenToClient(&pt);
			m_pListView->SendMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
		m_pVScrollbar->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pVScrollbar->ScreenToClient(&pt);
			m_pVScrollbar->SendMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
		m_pHScrollbar->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pHScrollbar->ScreenToClient(&pt);
			m_pHScrollbar->SendMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
		GetWindowRect(&r);
		if ( !(::PtInRect(&r, pt)) )
		{
			set_Capture(FALSE);
			set_Visible(FALSE);
			if ( m_pComboBox )
				m_pComboBox->set_focus();
		}
	}
	return 0;
}

LRESULT CComboPopup::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if ( (wParam == 0) && has_Capture() )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		RECT r;

		ClientToScreen(&pt);
		m_pListView->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pListView->ScreenToClient(&pt);
			m_pListView->SendMessage(WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
		m_pVScrollbar->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pVScrollbar->ScreenToClient(&pt);
			m_pVScrollbar->SendMessage(WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
		m_pHScrollbar->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pHScrollbar->ScreenToClient(&pt);
			m_pHScrollbar->SendMessage(WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
	}
	return 0;
}

LRESULT CComboPopup::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( (wParam == MK_LBUTTON) && has_Capture() )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		RECT r;

		ClientToScreen(&pt);
		m_pListView->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pListView->ScreenToClient(&pt);
			m_pListView->SendMessage(WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
		}
		else
		{
			GetWindowRect(&r);

			if ( !(::PtInRect(&r, pt)) )
			{
				set_Capture(FALSE);
				set_Visible(FALSE);
				if ( m_pComboBox )
					m_pComboBox->set_focus();
			}
		}
	}
	return 0;
}

LRESULT CComboPopup::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( (wParam == MK_LBUTTON) && has_Capture() )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		RECT r;

		ClientToScreen(&pt);
		m_pVScrollbar->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pVScrollbar->ScreenToClient(&pt);
			m_pVScrollbar->SendMessage(WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
		m_pHScrollbar->GetWindowRect(&r);
		if ( ::PtInRect(&r, pt) )
		{
			m_pHScrollbar->ScreenToClient(&pt);
			m_pHScrollbar->SendMessage(WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
	}
	return 0;
}

LRESULT CComboPopup::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	set_Capture(FALSE);
	set_Visible(FALSE);
	if ( m_pComboBox )
		m_pComboBox->set_focus();
	return get_frame()->SendMessage(WM_SYSKEYDOWN, wParam, lParam);
}

LRESULT CComboPopup::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	return get_frame()->SendMessage(WM_SYSKEYUP, wParam, lParam);
}

LRESULT CComboPopup::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

	if ( bShift || bControl )
		return 0;
	if ( !has_Capture() )
		set_Capture(TRUE);
	switch ( wParam )
	{
	case VK_ESCAPE:
		set_Capture(FALSE);
		set_Visible(FALSE);
		if ( m_pComboBox )
			m_pComboBox->set_focus();
		break;
	default:
		m_pListView->SendMessage(WM_KEYDOWN, wParam, lParam);
		break;
	}
	return 0;
}

LRESULT CComboPopup::OnListViewNodeFocused(WPARAM wParam, LPARAM lParam)
{
	Ptr(NotifyMessage) phdr = CastAnyPtr(NotifyMessage, lParam);
	Ptr(CListViewNode) pNode = CastAnyPtr(CListViewNode, phdr->param);

	if ( m_pComboBox )
		m_pComboBox->set_text(pNode->get_Text());
	return 0;
}

LRESULT CComboPopup::OnListViewNodeEntered(WPARAM wParam, LPARAM lParam)
{
	Ptr(NotifyMessage) phdr = CastAnyPtr(NotifyMessage, lParam);
	Ptr(CListViewNode) pNode = CastAnyPtr(CListViewNode, phdr->param);

	set_Capture(FALSE);
	set_Visible(FALSE);
	if ( m_pComboBox )
	{
		m_pComboBox->set_text(pNode->get_Text());
		m_pComboBox->set_focus();
	}
	return 0;
}

LRESULT CComboPopup::OnListViewEscape(WPARAM wParam, LPARAM lParam)
{
	set_Capture(FALSE);
	set_Visible(FALSE);
	if ( m_pComboBox )
		m_pComboBox->set_focus();
	return 0;
}

BEGIN_MESSAGE_MAP(CControl, CComboBox)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_MESSAGE(WM_GETDLGCODE,OnGetDlgCode)
	ON_WM_MESSAGE(WM_GETTEXT,OnGetText)
	ON_WM_MESSAGE(WM_GETTEXTLENGTH,OnGetTextLength)
	ON_WM_MESSAGE(WM_SETTEXT,OnSetText)
	ON_WM_NOTIFY(NM_EDITORRETURN,OnEditorReturn)
	ON_WM_NOTIFY(NM_EDITORTAB,OnEditorTab)
	ON_WM_NOTIFY(NM_EDITORFUNCKEY,OnEditorFuncKey)
	ON_WM_NOTIFY(NM_EDITORLOSTFOCUS,OnEditorLostFocus)
	ON_WM_NOTIFY(NM_EDITORESCAPE,OnEditorEscape)
	ON_WM_NOTIFY(NM_EDITORCURSORUP,OnEditorCursorUp)
	ON_WM_NOTIFY(NM_EDITORCURSORDOWN,OnEditorCursorDown)
END_MESSAGE_MAP()

CComboBox::CComboBox(LPCTSTR name):
	CControl(name),
	m_editable(false),
	m_pTextEditor(nullptr),
	m_pPopup(nullptr),
	m_DropDown(MAKEINTRESOURCE(IDB_COMBOARROW), IMAGE_BITMAP)
{
}

CComboBox::CComboBox(ConstRef(CStringBuffer) name):
	CControl(name),
	m_editable(false),
	m_pTextEditor(nullptr),
	m_pPopup(nullptr),
	m_DropDown(MAKEINTRESOURCE(IDB_COMBOARROW), IMAGE_BITMAP)
{
}

CComboBox::~CComboBox()
{
}

BOOL CComboBox::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName   = _T("CCOMBOBOX");
	return TRUE;
}

void CComboBox::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("ComboBox"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("ComboBox"));
	if ( m_pPopup )
		m_pPopup->get_BrushKeys(_keys);
	if ( m_pTextEditor )
		m_pTextEditor->get_BrushKeys(_keys);
}

void CComboBox::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".Font.Normal"), _T("ComboBox"));
	if ( m_pPopup )
		m_pPopup->get_FontKeys(_keys);
	if ( m_pTextEditor )
		m_pTextEditor->get_FontKeys(_keys);
}

void CComboBox::Refresh()
{
	Update(TRUE);
	if ( m_pPopup )
		m_pPopup->Refresh();
	if ( m_pTextEditor )
		m_pTextEditor->Refresh();
}

CWin* CComboBox::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CComboBox();

	return pWin;
}

ConstRef(CStringBuffer) CComboBox::get_text()
{
	if ( m_pTextEditor && (m_pTextEditor->is_Visible()) )
		m_text = m_pTextEditor->get_windowtext();
	return m_text;
}

void CComboBox::set_text(ConstRef(CStringBuffer) text)
{
	m_text = text;
	if ( is_created() )
	{
		Update(TRUE);
		if ( m_pTextEditor && (m_pTextEditor->is_Visible()) )
			m_pTextEditor->set_windowtext(m_text);
	}
}

void CComboBox::add_Text(LPCTSTR text)
{
	if ( !(is_created()) )
		return;
	m_pPopup->create_Node(text);
}

void CComboBox::add_Text(ConstRef(CStringBuffer) text)
{
	if ( !(is_created()) )
		return;
	m_pPopup->create_Node(text);
}

LRESULT CComboBox::OnGetText(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer text = get_text();
	CStringBuffer text1;
	dword len;

	if ( text.GetSize() <= wParam )
		text1 = text;
	else
		text.SubString(0, Castdword(wParam - 1), text1);
	len = text1.GetSize();
	s_memcpy(lParam, CastMutable(CPointer, text1.GetString()), len * szchar);
	return len - 1;
}

LRESULT CComboBox::OnGetTextLength(WPARAM wParam, LPARAM lParam)
{
	return get_text().GetLength();
}

LRESULT CComboBox::OnSetText(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer text(__FILE__LINE__ CastAny(CPointer, lParam));

	set_text(text);
	return TRUE;
}

LRESULT CComboBox::OnGetDlgCode(WPARAM wParam, LPARAM lParam)
{
	return DLGC_WANTALLKEYS;
}

LRESULT CComboBox::OnCreate(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer name(get_name());

	if ( is_Editable() )
	{
		name.AppendString(_T(".DefaultTextEditor"));
		m_pTextEditor = OK_NEW_OPERATOR CTextEditor(name);

		m_pTextEditor->set_CreateVisible(FALSE);
		m_pTextEditor->Create(m_hwnd, 1000);

		name = get_name();
	}
	name.AppendString(_T(".ComboPopup"));
	m_pPopup = OK_NEW_OPERATOR CComboPopup(name);

	m_pPopup->set_ClientBorder(TRUE);
	m_pPopup->set_ComboBox(this);
	m_pPopup->set_CreateVisible(FALSE);
	m_pPopup->Create();

	return 0;
}

LRESULT CComboBox::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;

	Gdiplus::Graphics graphics(hdc);
	Gdiplus::PointF pt(0.0, 0.0);
	Gdiplus::RectF rectF;
	Gdiplus::Bitmap* image = m_DropDown.get_bitmap();
	Gdiplus::Font* pFont = get_Font(_T(".Font.Normal"), _T("ComboBox"));

	if ( m_updateRects )
	{
		if ( image && (image->GetHeight() > 0) )
			::SetRect(&m_imageRect, 0, 0, image->GetWidth(), image->GetHeight());

		::SetRectEmpty(&m_textRect);
		if ( m_text.IsEmpty() )
		{
			Gdiplus::REAL fontHeight = pFont->GetHeight(&graphics);

			m_textRect.bottom = (LONG)(floor(fontHeight + 0.5));
			m_textRect.right = 10;
		}
		else
		{
			graphics.MeasureString(m_text, -1, pFont, pt, &rectF);
			Convert2Rect(&m_textRect, &rectF);
		}
		::SetRectEmpty(&m_maxClientArea);
		m_maxClientArea.bottom = Max(m_textRect.bottom, m_imageRect.bottom);
		m_maxClientArea.top = 0;
		m_maxClientArea.right = m_textRect.right + m_imageRect.right;
		m_maxClientArea.left = 0;
		m_updateRects = FALSE;
	}

	Gdiplus::Brush* pBrushBackground = get_Brush(_T(".BackgroundColor.Normal"), _T("ComboBox"), Gdiplus::Color::White);
	Gdiplus::Brush* pBrushForeground = get_Brush(_T(".ForegroundColor.Normal"), _T("ComboBox"), Gdiplus::Color::Black);
	RECT clientrect;

	GetClientRect(&clientrect);
	Convert2RectF(&rectF, &clientrect);

	graphics.FillRectangle(pBrushBackground, rectF);

	if ( !(m_text.IsEmpty()) )
		graphics.DrawString(m_text, -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), pBrushForeground);

	if ( image && (image->GetHeight() > 0) )
	{
		Gdiplus::Color transparentcolor;
		Gdiplus::ImageAttributes imAtt;
		Gdiplus::Rect destRect;

		image->GetPixel(0, image->GetHeight() - 1, &transparentcolor);
		imAtt.SetColorKey(transparentcolor, transparentcolor, Gdiplus::ColorAdjustTypeBitmap);
		destRect.X = clientrect.right - image->GetWidth();
		destRect.Y = 0;
		destRect.Height = image->GetHeight();
		destRect.Width = image->GetWidth();

		::OffsetRect(&m_imageRect, -(m_imageRect.left), 0);
		::OffsetRect(&m_imageRect, destRect.X, 0);

		graphics.DrawImage(image, destRect, 0, 0, 
			image->GetWidth(), 
			image->GetHeight(), Gdiplus::UnitPixel, &imAtt);
	}
	EndPaint(m_hwnd, &ps);
	return 0;
}

LRESULT CComboBox::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	set_focus();
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		if ( ::PtInRect(&m_imageRect, pt) )
		{
			if ( m_pPopup )
			{
				if ( !(m_pPopup->is_Visible()) )
				{
					RECT r;

					GetClientRect(&r);
					::OffsetRect(&r, 0, m_textRect.bottom);
					r.bottom = r.top + 100;
					ClientToScreen(&r);
					m_pPopup->SetWindowPos(&r, SWP_SHOWWINDOW);
					m_pPopup->set_Capture(TRUE);
				}
				else
					m_pPopup->set_Visible(FALSE);
			}
		}
	}
	return 0;
}

LRESULT CComboBox::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		if ( m_pTextEditor && (::PtInRect(&m_textRect, pt)) )
		{
			RECT r;

			GetClientRect(&r);
			r.bottom = m_textRect.bottom;
			r.right = m_imageRect.left;
			m_pTextEditor->SetWindowPos(&r, SWP_SHOWWINDOW);
			m_pTextEditor->set_windowtext(m_text);
			m_pTextEditor->set_focus();
		}
	}
	return 0;
}

LRESULT CComboBox::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

	switch ( wParam )
	{
	case VK_UP:
	case VK_DOWN:
		if ( bShift )
			break;
		if ( bControl )
		{
			SendNotifyMessage((wParam == VK_UP)?NM_EDITORCURSORUP:NM_EDITORCURSORDOWN, (LPARAM)this);
			break;
		}
		if ( !(m_pPopup->is_Visible()) )
		{
			RECT r;

			GetClientRect(&r);
			::OffsetRect(&r, 0, m_textRect.bottom);
			r.bottom = r.top + 100;
			ClientToScreen(&r);
			m_pPopup->SetWindowPos(&r, SWP_SHOWWINDOW);
			m_pPopup->set_Capture(TRUE);
		}
		break;
	case VK_RETURN:
		if ( bShift )
			break;
		if ( bControl )
		{
			SendNotifyMessage(NM_EDITORRETURN, (LPARAM)this);
			break;
		}
		if ( m_pTextEditor ) 
		{
			RECT r;

			GetClientRect(&r);
			r.bottom = m_textRect.bottom;
			r.right = m_imageRect.left;
			m_pTextEditor->SetWindowPos(&r, SWP_SHOWWINDOW);
			m_pTextEditor->set_windowtext(m_text);
			m_pTextEditor->set_focus();
		}
		else
			SendNotifyMessage(NM_EDITORRETURN, (LPARAM)this);
		break;
	case VK_F2:
		if ( bShift || bControl )
			return 0;
		if ( m_pTextEditor ) 
		{
			RECT r;

			GetClientRect(&r);
			r.bottom = m_textRect.bottom;
			r.right = m_imageRect.left;
			m_pTextEditor->SetWindowPos(&r, SWP_SHOWWINDOW);
			m_pTextEditor->set_windowtext(m_text);
			m_pTextEditor->set_focus();
		}
		else
		{
			TNotifyMessageEditorFuncKey param(this, VK_F2);

			SendNotifyMessage(NM_EDITORFUNCKEY, (LPARAM)(&param));
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CComboBox::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CComboBox::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	if ( m_pPopup )
		m_pPopup->DestroyWindow();
	return 0;
}

LRESULT CComboBox::OnEditorReturn(WPARAM wParam, LPARAM lParam)
{
	m_text = m_pTextEditor->get_windowtext();
	m_pTextEditor->set_Visible(FALSE);
	Update(TRUE);
	if ( m_pPopup->is_Visible() )
		m_pPopup->set_Visible(FALSE);
	SendNotifyMessage(NM_EDITORRETURN, (LPARAM)this);
	return 0;
}

LRESULT CComboBox::OnEditorTab(WPARAM wParam, LPARAM lParam)
{
	m_text = m_pTextEditor->get_windowtext();
	m_pTextEditor->set_Visible(FALSE);
	Update(TRUE);
	if ( m_pPopup->is_Visible() )
		m_pPopup->set_Visible(FALSE);
	SendNotifyMessage(NM_EDITORTAB, (LPARAM)this);
	return 0;
}

LRESULT CComboBox::OnEditorFuncKey(WPARAM wParam, LPARAM lParam)
{
	Ptr(NotifyMessage) pHdr = CastAnyPtr(NotifyMessage, lParam);
	Ptr(TNotifyMessageEditorFuncKey) param = CastAnyPtr(TNotifyMessageEditorFuncKey, pHdr->param);

	switch ( param->funcKey )
	{
	case VK_F2:
		{
			m_text = m_pTextEditor->get_windowtext();
			m_pTextEditor->set_Visible(FALSE);
			Update(TRUE);
			if ( m_pPopup->is_Visible() )
				m_pPopup->set_Visible(FALSE);

			TNotifyMessageEditorFuncKey param(this, VK_F2);

			SendNotifyMessage(NM_EDITORFUNCKEY, (LPARAM)(&param));
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CComboBox::OnEditorLostFocus(WPARAM wParam, LPARAM lParam)
{
	m_pTextEditor->set_Visible(FALSE);
	set_focus();
	return 0;
}

LRESULT CComboBox::OnEditorEscape(WPARAM wParam, LPARAM lParam)
{
	m_pTextEditor->set_Visible(FALSE);
	set_focus();
	return 0;
}

LRESULT CComboBox::OnEditorCursorUp(WPARAM wParam, LPARAM lParam)
{
	RECT r;

	GetClientRect(&r);
	::OffsetRect(&r, 0, m_textRect.bottom);
	r.bottom = r.top + 100;
	ClientToScreen(&r);
	m_pPopup->SetWindowPos(&r, SWP_SHOWWINDOW);
	m_pPopup->set_Capture(TRUE);
	m_text = m_pTextEditor->get_windowtext();
	m_pTextEditor->set_Visible(FALSE);
	Update(TRUE);
	return 0;
}

LRESULT CComboBox::OnEditorCursorDown(WPARAM wParam, LPARAM lParam)
{
	RECT r;

	GetClientRect(&r);
	::OffsetRect(&r, 0, m_textRect.bottom);
	r.bottom = r.top + 100;
	ClientToScreen(&r);
	m_pPopup->SetWindowPos(&r, SWP_SHOWWINDOW);
	m_pPopup->set_Capture(TRUE);
	m_text = m_pTextEditor->get_windowtext();
	m_pTextEditor->set_Visible(FALSE);
	Update(TRUE);
	return 0;
}
