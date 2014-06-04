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
#include "TabControl.h"
#include "GuiApplication.h"
#include "Configuration.h"
#include "ResourceManager.h"
#include "Resource.h"

#define SHIFTED 0x8000

BEGIN_MESSAGE_MAP(CPanel, CTabPage)
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CTabPage::CTabPage(LPCTSTR name):
	CPanel(name),
	m_selected(false)
{
	::SetRectEmpty(&m_titleRect);
}

CTabPage::CTabPage(ConstRef(CStringBuffer) name):
	CPanel(name),
	m_selected(false)
{
	::SetRectEmpty(&m_titleRect);
}

CTabPage::~CTabPage(void)
{
}

BOOL CTabPage::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = _T("CTABPAGE");
	return TRUE;
}

void CTabPage::set_Text(LPCTSTR pText, int lench)
{
	m_text.SetString(__FILE__LINE__ pText, lench);
	if ( is_created() )
	{
		CControl* pParent = CastDynamicPtr(CControl, get_parent());

		if ( pParent )
			pParent->Update(TRUE);
	}
}

void CTabPage::set_Text(ConstRef(CStringBuffer) text)
{
	m_text.SetString(__FILE__LINE__ text);
	if ( is_created() )
	{
		CControl* pParent = CastDynamicPtr(CControl, get_parent());

		if ( pParent )
			pParent->Update(TRUE);
	}
}

void CTabPage::set_Selected(bool selected)
{
	if ( m_selected != selected )
	{
		m_selected = selected;

		if ( !is_created() )
			return;

		set_Visible(m_selected?TRUE:FALSE);

		if ( m_selected )
		{
			CTabControl* pParent = CastDynamicPtr(CTabControl, get_parent());

			if ( pParent )
				pParent->OnSize(0, 0);
			set_VScrollOffset(0);
			set_HScrollOffset(0);
			OnSize(0, 0);
			if ( pParent )
				pParent->SendNotifyMessage(NM_TABPAGESELECTED, (LPARAM)this);
		}
	}
}

void CTabPage::calc_TitleRect(Gdiplus::Graphics* pGraphics, LPPOINT pPt, LONG* maxHeight, LONG* maxWidth)
{
	if ( (m_text.IsEmpty()) || (!is_created()) )
		return;

	CTabControl* pParent = CastDynamicPtr(CTabControl, get_parent());

	if ( !pParent )
		return;

	RECT r;
	Gdiplus::RectF rF;

	pParent->GetClientRect(&r);
	Convert2RectF(&rF, &r);

	Gdiplus::Font* pFont = nullptr;

	if ( m_selected )
		pFont = pParent->get_Font(_T(".Title.Font.Selected"), _T("TabControl"));
	else
		pFont = pParent->get_Font(_T(".Title.Font.Normal"), _T("TabControl"));

	Gdiplus::PointF pt(Cast(Gdiplus::REAL, pPt->x), Cast(Gdiplus::REAL, pPt->y));
	Gdiplus::RectF rectF;

	pGraphics->MeasureString(m_text.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &rectF);
	if ( (rectF.X > 0.0) && ((rectF.X + rectF.Width) > rF.Width) )
	{
		rectF.X = 0;
		rectF.Y = Cast(Gdiplus::REAL, *maxHeight);
	}
	Convert2Rect(&m_titleRect, &rectF);
	m_titleRect.bottom += 5;
	m_titleRect.right += 6 + ((m_titleRect.bottom - m_titleRect.top) / 2);

	pPt->x = m_titleRect.right;
	pPt->y = m_titleRect.top;
	if ( m_titleRect.bottom > *maxHeight )
		*maxHeight = m_titleRect.bottom;
	if ( m_titleRect.right > *maxWidth )
		*maxWidth = m_titleRect.right;
}

void CTabPage::paint_TitleRect(Gdiplus::Graphics* pGraphics)
{
	if ( (m_text.IsEmpty()) || (!is_created()) )
		return;

	CTabControl* pParent = CastDynamicPtr(CTabControl, get_parent());

	if ( !pParent )
		return;

	Gdiplus::PointF pt(Cast(Gdiplus::REAL, m_titleRect.left + 2 + ((m_titleRect.bottom - m_titleRect.top) / 2)), Cast(Gdiplus::REAL, m_titleRect.top + 2));
	Gdiplus::Brush* pBackgroundColorNormal = pParent->get_Brush(_T(".BackgroundColor.Normal"), _T("TabControl"), Gdiplus::Color::White);
	Gdiplus::Brush* pBackgroundColorSelected = nullptr;
	Gdiplus::Brush* pForegroundColor = nullptr;

	if ( m_selected )
	{
		pBackgroundColorSelected = pParent->get_Brush(_T(".BackgroundColor.Selected"),  _T("TabControl"), Gdiplus::Color::Blue);
		pForegroundColor = pParent->get_Brush(_T(".ForegroundColor.Selected"),  _T("TabControl"), Gdiplus::Color::White);
	}
	else
		pForegroundColor = pParent->get_Brush(_T(".ForegroundColor.Normal"),  _T("TabControl"), Gdiplus::Color::Black);

	Gdiplus::Pen blackPen(Gdiplus::Color::Black);
	Gdiplus::RectF titleRectF;

	Convert2RectF(&titleRectF, &m_titleRect);
	pGraphics->FillRectangle(pBackgroundColorNormal,titleRectF);
	titleRectF.Height -= 1;
	titleRectF.Width -= 1;

	Gdiplus::REAL offset = titleRectF.Height / 2;
	Gdiplus::GraphicsPath path;

	if ( pParent->get_TitleOrientation() == CTabControl::TTitleOrientationTop)
	{
		Gdiplus::PointF pts[] = {	
			Gdiplus::PointF(titleRectF.X + offset, titleRectF.Y),
			Gdiplus::PointF(titleRectF.X + titleRectF.Width, titleRectF.Y),
			Gdiplus::PointF(titleRectF.X + titleRectF.Width, titleRectF.Y + titleRectF.Height),
			Gdiplus::PointF(titleRectF.X, titleRectF.Y + titleRectF.Height),
			Gdiplus::PointF(titleRectF.X, titleRectF.Y + offset),
			Gdiplus::PointF(titleRectF.X + offset, titleRectF.Y)
		};

		path.AddLines(pts, sizeof(pts) / sizeof(Gdiplus::PointF) );
	}
	else
	{
		Gdiplus::PointF pts[] = {	
			Gdiplus::PointF(titleRectF.X, titleRectF.Y),
			Gdiplus::PointF(titleRectF.X + titleRectF.Width, titleRectF.Y),
			Gdiplus::PointF(titleRectF.X + titleRectF.Width, titleRectF.Y + titleRectF.Height),
			Gdiplus::PointF(titleRectF.X + offset, titleRectF.Y + titleRectF.Height),
			Gdiplus::PointF(titleRectF.X, titleRectF.Y + offset),
			Gdiplus::PointF(titleRectF.X, titleRectF.Y)
		};

		path.AddLines(pts, sizeof(pts) / sizeof(Gdiplus::PointF) );
	}

	if ( m_selected )
		pGraphics->FillPath(pBackgroundColorSelected, &path);
	pGraphics->DrawPath(&blackPen, &path);

	Gdiplus::Font* pFont = nullptr;

	if ( m_selected )
		pFont = pParent->get_Font(_T(".Title.Font.Selected"), _T("TabControl"));
	else
		pFont = pParent->get_Font(_T(".Title.Font.Normal"), _T("TabControl"));

	pGraphics->DrawString(m_text.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), pForegroundColor);
}

LRESULT CTabPage::OnSize(WPARAM wParam, LPARAM lParam)
{
	if ( !m_selected )
		return 0;
	return CPanel::OnSize(wParam, lParam);
}

LRESULT CTabPage::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	set_focus();
	return 0;
}

BEGIN_MESSAGE_MAP(CControl, CTabControl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_MESSAGE(WM_GETDLGCODE,OnGetDlgCode)
END_MESSAGE_MAP()

CTabControl::CTabControl(LPCTSTR name):
    CControl(name),
	m_TitleOrientation(TTitleOrientationTop),
	m_currentTabPage(nullptr)
{
	::SetRectEmpty(&m_titleBarRect);
	m_mouseclick.x = 0;
	m_mouseclick.y = 0;
}

CTabControl::CTabControl(ConstRef(CStringBuffer) name):
    CControl(name),
	m_TitleOrientation(TTitleOrientationTop),
	m_currentTabPage(nullptr)
{
	::SetRectEmpty(&m_titleBarRect);
	m_mouseclick.x = 0;
	m_mouseclick.y = 0;
}

CTabControl::~CTabControl(void)
{
}

BOOL CTabControl::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = _T("CTABCONTROL");
	return TRUE;
}

void CTabControl::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	CControl::get_BrushKeys(_keys);
	add_BrushKeys(_keys, _T(".Color.Workspace"), _T("TabControl"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("TabControl"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Selected"),  _T("TabControl"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("TabControl"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Selected"),  _T("TabControl"));
}

void CTabControl::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	CControl::get_FontKeys(_keys);
	add_FontKeys(_keys, _T(".Title.Font.Selected"), _T("TabControl"));
	add_FontKeys(_keys, _T(".Title.Font.Normal"), _T("TabControl"));
}

void CTabControl::Refresh()
{
	CControl::Refresh();
	Update(TRUE);
}

CWin* CTabControl::Clone(LONG style, DWORD exstyle)
{
	CTabControl* pWin = OK_NEW_OPERATOR CTabControl();

	if ( style & WS_BORDER )
		pWin->set_border(TRUE);
	return pWin;
}

void CTabControl::set_CurrentTabPage(CTabPage* page)
{
	if ( m_currentTabPage )
		m_currentTabPage->set_Selected(false);
	m_currentTabPage = page;
	if ( m_currentTabPage )
		m_currentTabPage->set_Selected(true);
	else
		SendNotifyMessage(NM_TABPAGESELECTED, NULL);
}

typedef struct _tagEnumChildrenCTabControlOnPaint1Param
{
	HWND parent;
	Gdiplus::Graphics* graphics;
	LONG maxHeight;
	LONG maxWidth;
	POINT pt; 

	_tagEnumChildrenCTabControlOnPaint1Param(HWND _parent, Gdiplus::Graphics* _graphics):
	    parent(_parent), graphics(_graphics), maxHeight(0), maxWidth(0) { pt.x = 0; pt.y = 0; }
} TEnumChildrenCTabControlOnPaint1Param;

static BOOL CALLBACK EnumChildProc_CTabControl_OnPaint1(HWND hwnd, LPARAM lParam)
{
	TEnumChildrenCTabControlOnPaint1Param* param = CastAnyPtr(TEnumChildrenCTabControlOnPaint1Param, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CTabPage* child = CastAnyPtr(CTabPage, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		child->calc_TitleRect(param->graphics, &(param->pt), &(param->maxHeight), &(param->maxWidth));
	}
	return TRUE;
}

typedef struct _tagEnumChildrenCTabControlOnPaint1aParam
{
	HWND parent;
	LONG deltaHeight;

	_tagEnumChildrenCTabControlOnPaint1aParam(HWND _parent, LONG _deltaHeight):
	    parent(_parent), deltaHeight(_deltaHeight) {}
} TEnumChildrenCTabControlOnPaint1aParam;

static BOOL CALLBACK EnumChildProc_CTabControl_OnPaint1a(HWND hwnd, LPARAM lParam)
{
	TEnumChildrenCTabControlOnPaint1aParam* param = CastAnyPtr(TEnumChildrenCTabControlOnPaint1aParam, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CTabPage* child = CastAnyPtr(CTabPage, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));
		RECT r;

		child->get_TitleRect(&r);
		::OffsetRect(&r, 0, param->deltaHeight);
		child->set_TitleRect(&r);
	}
	return TRUE;
}

typedef struct _tagEnumChildrenCTabControlOnPaint2Param
{
	HWND parent;
	Gdiplus::Graphics* graphics;

	_tagEnumChildrenCTabControlOnPaint2Param(HWND _parent, Gdiplus::Graphics* _graphics):
	    parent(_parent), graphics(_graphics) {}
} TEnumChildrenCTabControlOnPaint2Param;

static BOOL CALLBACK EnumChildProc_CTabControl_OnPaint2(HWND hwnd, LPARAM lParam)
{
	TEnumChildrenCTabControlOnPaint2Param* param = CastAnyPtr(TEnumChildrenCTabControlOnPaint2Param, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CTabPage* child = CastAnyPtr(CTabPage, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		child->paint_TitleRect(param->graphics);
	}
	return TRUE;
}

LRESULT CTabControl::OnPaint(WPARAM wParam, LPARAM lParam)
{
	if ( !is_created() )
		return 1;

	if ( !m_currentTabPage )
	{
		CTabPage* pFirstChild = CastDynamicPtr(CTabPage, get_firstchild());

		if ( pFirstChild )
			m_currentTabPage = pFirstChild;
	}
	if ( m_currentTabPage && (!(m_currentTabPage->is_Selected())) )
	{
		++m_update;
		m_currentTabPage->set_Selected(true);
		--m_update;
	}

	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;

	Gdiplus::Graphics graphics(hdc);
	RECT clientRect;

	GetClientRect(&clientRect);
	Gdiplus::Bitmap bitmap(clientRect.right, clientRect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);

	if ( m_updateRects )
	{
		TEnumChildrenCTabControlOnPaint1Param param(m_hwnd, &bitmapgraphics);

		::EnumChildWindows(m_hwnd, EnumChildProc_CTabControl_OnPaint1, (LPARAM)(&param));
		::SetRectEmpty(&m_titleBarRect);
		if ( m_TitleOrientation == TTitleOrientationBottom )
		{
			m_titleBarRect.bottom = clientRect.bottom;
			m_titleBarRect.top = m_titleBarRect.bottom - param.maxHeight;

			TEnumChildrenCTabControlOnPaint1aParam param1(m_hwnd, m_titleBarRect.top);

			::EnumChildWindows(m_hwnd, EnumChildProc_CTabControl_OnPaint1a, (LPARAM)(&param1));
		}
		else
			m_titleBarRect.bottom = param.maxHeight;
		m_titleBarRect.right = param.maxWidth;
		::CopyRect(&m_maxClientArea, &m_titleBarRect);
		++m_update;
		OnSize(0, 0);
		--m_update;
		m_updateRects = FALSE;
	}

	Gdiplus::Brush* brush = get_Brush(_T(".Color.Workspace"), _T("TabControl"), Gdiplus::Color::Gray);

	if ( ::IsRectEmpty(&m_titleBarRect) )
	{
		Gdiplus::RectF clientRectF;

		Convert2RectF(&clientRectF, &clientRect);
		bitmapgraphics.FillRectangle(brush,clientRectF);
	}
	else
	{
		Gdiplus::RectF titleRectF;

		Convert2RectF(&titleRectF, &m_titleBarRect);
		titleRectF.Width = Cast(Gdiplus::REAL, clientRect.right);
		bitmapgraphics.FillRectangle(brush,titleRectF);
	}

	TEnumChildrenCTabControlOnPaint2Param param(m_hwnd, &bitmapgraphics);

	::EnumChildWindows(m_hwnd, EnumChildProc_CTabControl_OnPaint2, (LPARAM)(&param));
	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);
	return 0;
}

typedef struct _tagEnumChildrenCTabControlOnLButtonDownParam
{
	HWND parent;
	POINT pt;
	CTabPage* result;

	_tagEnumChildrenCTabControlOnLButtonDownParam(HWND _parent, POINT& _pt):
	    parent(_parent), pt(_pt), result(nullptr) {}
} TEnumChildrenCTabControlOnLButtonDownParam;

static BOOL CALLBACK EnumChildProc_CTabControl_OnLButtonDown(HWND hwnd, LPARAM lParam)
{
	TEnumChildrenCTabControlOnLButtonDownParam* param = CastAnyPtr(TEnumChildrenCTabControlOnLButtonDownParam, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CTabPage* child = CastAnyPtr(CTabPage, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));
		RECT r;

		child->get_TitleRect(&r);
		if ( ::PtInRect(&r, param->pt) )
		{
			param->result = child;
			return FALSE;
		}
	}
	return TRUE;
}

LRESULT CTabControl::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ( !is_created() )
		return 1;
	set_focus();
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		TEnumChildrenCTabControlOnLButtonDownParam param(m_hwnd, pt);

		::EnumChildWindows(m_hwnd, EnumChildProc_CTabControl_OnLButtonDown, (LPARAM)(&param));
		if ( param.result )
		{
			if ( param.result != m_currentTabPage )
			{
				BeginUpdate();
				if ( m_currentTabPage )
					m_currentTabPage->set_Selected(false);
				m_currentTabPage = param.result;
				m_currentTabPage->set_Selected(true);
				EndUpdate(FALSE);
			}
			m_mouseclick = pt;
			set_Capture(TRUE);
		}
	}
	return 0;
}

LRESULT CTabControl::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( (wParam == MK_LBUTTON) && has_Capture() )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		TEnumChildrenCTabControlOnLButtonDownParam param(m_hwnd, pt);

		::EnumChildWindows(m_hwnd, EnumChildProc_CTabControl_OnLButtonDown, (LPARAM)(&param));
		if ( param.result )
		{
			if ( param.result != m_currentTabPage )
			{
				RECT r1;
				RECT r2;

				param.result->get_TitleRect(&r1);
				m_currentTabPage->get_TitleRect(&r2);
				if ( r1.left < r2.left )
					param.result->SetWindowPos(m_currentTabPage, nullptr, 0);
				else if ( r1.left > r2.left )
					m_currentTabPage->SetWindowPos(param.result, nullptr, 0);
				else if ( r1.top < r2.top )
					param.result->SetWindowPos(m_currentTabPage, nullptr, 0);
				else
					m_currentTabPage->SetWindowPos(param.result, nullptr, 0);
				Update(TRUE);
			}
		}
		else if ( ::PtInRect(&m_titleBarRect, pt) )
		{
			m_currentTabPage->SetWindowPos(HWND_BOTTOM, nullptr, 0);
			Update(TRUE);
		}
		else
		{
			RECT r;

			::CopyRect(&r, &m_titleBarRect);
			::InflateRect(&r, 20, 20);
			if ( !(::PtInRect(&r, pt)) && m_currentTabPage )
			{
				if ( SendNotifyMessage(NM_TABPAGEDRAGGED, (LPARAM)m_currentTabPage) )
					set_Capture(FALSE);
			}
		}
	}
	return 0;
}

LRESULT CTabControl::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if ( has_Capture() )
		set_Capture(FALSE);
	return 0;
}

LRESULT CTabControl::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ( !is_created() )
		return 1;

	BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

	switch ( wParam )
	{
	case VK_LEFT:
		if ( !(bShift || bControl) )
		{
			if ( !m_currentTabPage )
			{
				CTabPage* pFirstChild = CastDynamicPtr(CTabPage, get_firstchild());

				if ( pFirstChild )
				{
					m_currentTabPage = pFirstChild;
					m_currentTabPage->set_Selected(true);
				}
				break;
			}

			CTabPage* pPrevPage = CastDynamicPtr(CTabPage, m_currentTabPage->get_siblingByCmd(GW_HWNDPREV));

			if ( !pPrevPage )
				pPrevPage = CastDynamicPtr(CTabPage, m_currentTabPage->get_siblingByCmd(GW_HWNDLAST));
			if ( !pPrevPage )
				break;
			BeginUpdate();
			m_currentTabPage->set_Selected(false);
			m_currentTabPage = pPrevPage;
			m_currentTabPage->set_Selected(true);
			EndUpdate(FALSE);
		}
		break;
	case VK_RIGHT:
		if ( !(bShift || bControl) )
		{
			if ( !m_currentTabPage )
			{
				CTabPage* pFirstChild = CastDynamicPtr(CTabPage, get_firstchild());

				if ( pFirstChild )
				{
					m_currentTabPage = pFirstChild;
					m_currentTabPage->set_Selected(true);
				}
				break;
			}
			CTabPage* pNextPage = CastDynamicPtr(CTabPage, m_currentTabPage->get_siblingByCmd(GW_HWNDNEXT));

			if ( !pNextPage )
				pNextPage = CastDynamicPtr(CTabPage, m_currentTabPage->get_siblingByCmd(GW_HWNDFIRST));
			if ( !pNextPage )
				break;
			BeginUpdate();
			m_currentTabPage->set_Selected(false);
			m_currentTabPage = pNextPage;
			m_currentTabPage->set_Selected(true);
			EndUpdate(FALSE);
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CTabControl::OnSize(WPARAM wParam, LPARAM lParam)
{
	if ( !is_created() )
		return 1;

	RECT rect;

	GetClientRect(&rect);

	if ( ::IsRectEmpty(&rect) )
		return 1;
	Update(TRUE);
	if ( m_TitleOrientation == TTitleOrientationTop )
		rect.top = m_titleBarRect.bottom;
	else
		rect.bottom = m_titleBarRect.top;

	if ( m_currentTabPage )
		m_currentTabPage->MoveWindow(&rect, TRUE);
	return 0;
}

LRESULT CTabControl::OnGetDlgCode(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case VK_LEFT:
	case VK_RIGHT:
		return DLGC_WANTARROWS;
	default:
		break;
	}
	return 0;
}


