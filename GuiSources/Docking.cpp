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
#include "Docking.h"
#include "GuiApplication.h"
#include "Configuration.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "TabControl.h"

#ifdef __DEBUG1__
#include "ListView.h"
#endif

#define SHIFTED 0x8000

#ifdef __DEBUG1__
static const char * vNCHitTestCodes[] = {
	"HTERROR",
	"HTTRANSPARENT",
	"HTNOWHERE",
	"HTCLIENT",
	"HTCAPTION",
	"HTSYSMENU",
	"HTGROWBOX",
	"HTMENU",
	"HTHSCROLL",
	"HTVSCROLL",
	"HTMINBUTTON",
	"HTMAXBUTTON",
	"HTLEFT",
	"HTRIGHT",
	"HTTOP",
	"HTTOPLEFT",
	"HTTOPRIGHT",
	"HTBOTTOM",
	"HTBOTTOMLEFT",
	"HTBOTTOMRIGHT",
	"HTBORDER",
	"HTOBJECT",
	"HTCLOSE",
	"HTHELP"
};
#endif

//***********************************************************
// CDockWindow
//***********************************************************
class CDockWindowNeighbourEqualFunctor
{
public:
	bool operator()(ConstPtr(CDockWindow::TNeighbour) pArray, ConstPtr(CDockWindow::TNeighbour) pData) const
	{
		return pArray->win == pData->win;
	}
};

class CDockWindowNeighbourFullEqualFunctor
{
public:
	bool operator()(ConstPtr(CDockWindow::TNeighbour) pArray, ConstPtr(CDockWindow::TNeighbour) pData) const
	{
		return (pArray->win == pData->win) && (pArray->style == pData->style);
	}
};

BEGIN_MESSAGE_MAP(CWin, CDockWindow)
	ON_WM_NCACTIVATE()
	ON_WM_NCCREATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_NCHITTEST()
	ON_WM_NCDESTROY()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_ENTERSIZEMOVE()
	ON_WM_EXITSIZEMOVE()
	ON_WM_NOTIFY(NM_TABPAGEDRAGGED, OnTabPageDragged)
	ON_WM_NOTIFY(NM_TABPAGESELECTED, OnTabPageSelected)
END_MESSAGE_MAP()

CDockWindow::CDockWindow(LPCTSTR name): 
    CWin(TDockWindow, name),
	m_neighbours(__FILE__LINE__ 16, 16),
	m_dockingstyle(TDockingStyleNone),
	m_control(nullptr),
	m_isFloating(FALSE),
	m_pDockInfo(nullptr),
	m_childID(0),
	m_isSizing(FALSE),
	m_SizingEdge(HTNOWHERE)
{
}

CDockWindow::CDockWindow(ConstRef(CStringBuffer) name): 
    CWin(TDockWindow, name),
	m_neighbours(__FILE__LINE__ 16, 16),
	m_dockingstyle(TDockingStyleNone),
	m_control(nullptr),
	m_isFloating(FALSE),
	m_pDockInfo(nullptr),
	m_childID(0),
	m_isSizing(FALSE),
	m_SizingEdge(HTNOWHERE)
{
}

CDockWindow::~CDockWindow()
{
}

BOOL CDockWindow::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = _T("CDOCKWINDOW");
	return TRUE;
}

BOOL CDockWindow::PreCreate(CREATESTRUCT& cs)
{
	m_childID = CastAny(UINT, cs.hMenu);
	if ( m_isFloating )
	{
		cs.style = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
		cs.dwExStyle = WS_EX_TOOLWINDOW;
		cs.hwndParent = nullptr;
		cs.hMenu = nullptr;
		cs.x = 100;
		cs.y = 100;
	}
	else
	{
		cs.style = WS_CHILD;
		cs.dwExStyle = 0;
	}
	cs.cx = 100;
	cs.cy = 100;
	return TRUE;
}

void CDockWindow::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".DockedTitle.BackgroundColor.Normal"), _T("DockWindow"));
	add_BrushKeys(_keys, _T(".DockedTitle.ForegroundColor.Normal"), _T("DockWindow"));
	m_control->get_BrushKeys(_keys);
}

void CDockWindow::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".DockedTitle.Font.Normal"), _T("DockWindow"));
	m_control->get_FontKeys(_keys);
}

void CDockWindow::Refresh()
{
	::RedrawWindow(m_hwnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
	m_control->Refresh();
}

void CDockWindow::set_Caption(ConstRef(CStringBuffer) caption)
{
	m_caption = caption;
	if ( is_created() )
	{
		if ( m_isFloating )
			set_windowtext(m_caption);
		else
			SetWindowPos(nullptr, SWP_DRAWFRAME);
	}
}

void CDockWindow::Dock(LPRECT r)
{
	CWin* frame = get_frame();
	DWORD style = WS_CHILD | WS_VISIBLE;
	RECT r1;

	assert(frame != nullptr);
	m_isFloating = FALSE;
	SetWindowLong(GWL_STYLE, style);
	SetWindowLong(GWL_EXSTYLE, 0);
	SetWindowLong(GWLP_ID, m_childID);
	set_parent(frame);
	::CopyRect(&r1, r);
	frame->ScreenToClient(&r1);
	SetWindowPos(&r1, SWP_FRAMECHANGED);
}

void CDockWindow::UnDock(LPRECT r)
{
	DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE;
	DWORD exstyle = WS_EX_TOOLWINDOW;

	m_isFloating = TRUE;
	SetWindowLong(GWL_STYLE, style);
	SetWindowLong(GWL_EXSTYLE, exstyle);
	SetWindowLong(GWLP_ID, 0);
	set_parenthandle(0);
	SetWindowPos(HWND_TOPMOST, r, SWP_NOSIZE | SWP_FRAMECHANGED);
	set_windowtext(m_caption);
}

bool CDockWindow::has_Neighbours(CDockWindow::TDockingStyle style)
{
	TNeighbourVector::Iterator it = m_neighbours.Begin();

	while ( it )
	{
		TNeighbour* pNeighbour = *it;

		if ( pNeighbour->style == style )
			return true;
		++it;
	}
	return false;
}

CDockWindow::TNeighbourVector::Iterator CDockWindow::get_FirstNeighbour(CDockWindow::TDockingStyle style)
{
	TNeighbourVector::Iterator it = m_neighbours.Begin();

	while ( it )
	{
		TNeighbour* pNeighbour = *it;

		if ( pNeighbour->style == style )
			return it;
		++it;
	}
	return it;
}

CDockWindow::TNeighbourVector::Iterator CDockWindow::get_NextNeighbour(CDockWindow::TDockingStyle style, CDockWindow::TNeighbourVector::Iterator it)
{
	++it;
	while ( it )
	{
		TNeighbour* pNeighbour = *it;

		if ( pNeighbour->style == style )
			return it;
		++it;
	}
	return it;
}

void CDockWindow::AdjustNeighbours(TDockingStyle style)
{
	TNeighbourVector::Iterator it;
	CDockWindow* pWin;
	CDockWindow* pWin1;
	RECT r1;
	RECT r2;
	bool bAny;

	GetParentRect(&r1);
	switch ( style )
	{
	case TDockingStyleLeft:
		it = get_FirstNeighbour(TDockingStyleLeft);
		if ( !it )
		{
			it = get_FirstNeighbour(TDockingStyleTop);
			while ( it )
			{
				TNeighbour* pN = *it;

				CopyNeighbours(pN->win);
				it = get_NextNeighbour(TDockingStyleTop, it);
			}
			it = get_FirstNeighbour(TDockingStyleBottom);
			while ( it )
			{
				TNeighbour* pN = *it;

				CopyNeighbours(pN->win);
				it = get_NextNeighbour(TDockingStyleBottom, it);
			}
			break;
		}
		pWin = (*it)->win;
		ClearNeighbours(TDockingStyleLeft);
		pWin->GetParentRect(&r2);
		if ( (r1.left == r2.right) && (((r2.top <= r1.top) && (r2.bottom > r1.top)) || ((r2.top > r1.top) && (r2.top < r1.bottom))) )
		{
			AppendNeighbour(TDockingStyleLeft, pWin);
			pWin->AppendNeighbour(TDockingStyleRight, this);
		}
		pWin1 = pWin;
		while ( true )
		{
			bAny = false;
			it = pWin1->get_FirstNeighbour(TDockingStyleTop);
			while ( it )
			{
				TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( (r1.left == r2.right) && (((r2.top <= r1.top) && (r2.bottom > r1.top)) || ((r2.top > r1.top) && (r2.top < r1.bottom))) )
				{
					AppendNeighbour(TDockingStyleLeft, pN->win);
					pN->win->AppendNeighbour(TDockingStyleRight, this);
					pWin1 = pN->win;
					bAny = true;
					break;
				}
				it = pWin1->get_NextNeighbour(TDockingStyleTop, it);
			}
			if ( !bAny )
				break;
		}
		pWin1 = pWin;
		while ( true )
		{
			bAny = false;
			it = pWin1->get_FirstNeighbour(TDockingStyleBottom);
			while ( it )
			{
				TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( (r1.left == r2.right) && (((r2.top <= r1.top) && (r2.bottom > r1.top)) || ((r2.top > r1.top) && (r2.top < r1.bottom))) )
				{
					AppendNeighbour(TDockingStyleLeft, pN->win);
					pN->win->AppendNeighbour(TDockingStyleRight, this);
					pWin1 = pN->win;
					bAny = true;
					break;
				}
				it = pWin1->get_NextNeighbour(TDockingStyleBottom, it);
			}
			if ( !bAny )
				break;
		}
		break;
	case TDockingStyleTop:
		it = get_FirstNeighbour(TDockingStyleTop);
		if ( !it )
		{
			it = get_FirstNeighbour(TDockingStyleLeft);
			while ( it )
			{
				TNeighbour* pN = *it;

				CopyNeighbours(pN->win);
				it = get_NextNeighbour(TDockingStyleLeft, it);
			}
			it = get_FirstNeighbour(TDockingStyleRight);
			while ( it )
			{
				TNeighbour* pN = *it;

				CopyNeighbours(pN->win);
				it = get_NextNeighbour(TDockingStyleRight, it);
			}
			break;
		}
		pWin = (*it)->win;
		ClearNeighbours(TDockingStyleTop);
		pWin->GetParentRect(&r2);
		if ( (r1.top == r2.bottom) && (((r2.left <= r1.left) && (r2.right > r1.left)) || ((r2.left > r1.left) && (r2.left < r1.right))) )
		{
			AppendNeighbour(TDockingStyleTop, pWin);
			pWin->AppendNeighbour(TDockingStyleBottom, this);
		}
		pWin1 = pWin;
		while ( true )
		{
			bAny = false;
			it = pWin1->get_FirstNeighbour(TDockingStyleLeft);
			while ( it )
			{
				TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( (r1.top == r2.bottom) && (((r2.left <= r1.left) && (r2.right > r1.left)) || ((r2.left > r1.left) && (r2.left < r1.right))) )
				{
					AppendNeighbour(TDockingStyleTop, pN->win);
					pN->win->AppendNeighbour(TDockingStyleBottom, this);
					pWin1 = pN->win;
					bAny = true;
					break;
				}
				it = pWin1->get_NextNeighbour(TDockingStyleLeft, it);
			}
			if ( !bAny )
				break;
		}
		pWin1 = pWin;
		while ( true )
		{
			bAny = false;
			it = pWin1->get_FirstNeighbour(TDockingStyleRight);
			while ( it )
			{
				TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( (r1.top == r2.bottom) && (((r2.left <= r1.left) && (r2.right > r1.left)) || ((r2.left > r1.left) && (r2.left < r1.right))) )
				{
					AppendNeighbour(TDockingStyleTop, pN->win);
					pN->win->AppendNeighbour(TDockingStyleBottom, this);
					pWin1 = pN->win;
					bAny = true;
					break;
				}
				it = pWin1->get_NextNeighbour(TDockingStyleRight, it);
			}
			if ( !bAny )
				break;
		}
		break;
	case TDockingStyleRight:
		it = get_FirstNeighbour(TDockingStyleRight);
		if ( !it )
		{
			it = get_FirstNeighbour(TDockingStyleTop);
			while ( it )
			{
				TNeighbour* pN = *it;

				CopyNeighbours(pN->win);
				it = get_NextNeighbour(TDockingStyleTop, it);
			}
			it = get_FirstNeighbour(TDockingStyleBottom);
			while ( it )
			{
				TNeighbour* pN = *it;

				CopyNeighbours(pN->win);
				it = get_NextNeighbour(TDockingStyleBottom, it);
			}
			break;
		}
		pWin = (*it)->win;
		ClearNeighbours(TDockingStyleRight);
		pWin->GetParentRect(&r2);
		if ( (r1.right == r2.left) && (((r2.top <= r1.top) && (r2.bottom > r1.top)) || ((r2.top > r1.top) && (r2.top < r1.bottom))) )
		{
			AppendNeighbour(TDockingStyleRight, pWin);
			pWin->AppendNeighbour(TDockingStyleLeft, this);
		}
		pWin1 = pWin;
		while ( true )
		{
			bAny = false;
			it = pWin1->get_FirstNeighbour(TDockingStyleTop);
			while ( it )
			{
				TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( (r1.right == r2.left) && (((r2.top <= r1.top) && (r2.bottom > r1.top)) || ((r2.top > r1.top) && (r2.top < r1.bottom))) )
				{
					AppendNeighbour(TDockingStyleRight, pN->win);
					pN->win->AppendNeighbour(TDockingStyleLeft, this);
					pWin1 = pN->win;
					bAny = true;
					break;
				}
				it = pWin1->get_NextNeighbour(TDockingStyleTop, it);
			}
			if ( !bAny )
				break;
		}
		pWin1 = pWin;
		while ( true )
		{
			bAny = false;
			it = pWin1->get_FirstNeighbour(TDockingStyleBottom);
			while ( it )
			{
				TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( (r1.right == r2.left) && (((r2.top <= r1.top) && (r2.bottom > r1.top)) || ((r2.top > r1.top) && (r2.top < r1.bottom))) )
				{
					AppendNeighbour(TDockingStyleRight, pN->win);
					pN->win->AppendNeighbour(TDockingStyleLeft, this);
					pWin1 = pN->win;
					bAny = true;
					break;
				}
				it = pWin1->get_NextNeighbour(TDockingStyleBottom, it);
			}
			if ( !bAny )
				break;
		}
		break;
	case TDockingStyleBottom:
		it = get_FirstNeighbour(TDockingStyleBottom);
		if ( !it )
		{
			it = get_FirstNeighbour(TDockingStyleLeft);
			while ( it )
			{
				TNeighbour* pN = *it;

				CopyNeighbours(pN->win);
				it = get_NextNeighbour(TDockingStyleLeft, it);
			}
			it = get_FirstNeighbour(TDockingStyleRight);
			while ( it )
			{
				TNeighbour* pN = *it;

				CopyNeighbours(pN->win);
				it = get_NextNeighbour(TDockingStyleRight, it);
			}
			break;
		}
		pWin = (*it)->win;
		ClearNeighbours(TDockingStyleBottom);
		pWin->GetParentRect(&r2);
		if ( (r1.bottom == r2.top) && (((r2.left <= r1.left) && (r2.right > r1.left)) || ((r2.left > r1.left) && (r2.left < r1.right))) )
		{
			AppendNeighbour(TDockingStyleBottom, pWin);
			pWin->AppendNeighbour(TDockingStyleTop, this);
		}
		pWin1 = pWin;
		while ( true )
		{
			bAny = false;
			it = pWin1->get_FirstNeighbour(TDockingStyleLeft);
			while ( it )
			{
				TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( (r1.bottom == r2.top) && (((r2.left <= r1.left) && (r2.right > r1.left)) || ((r2.left > r1.left) && (r2.left < r1.right))) )
				{
					AppendNeighbour(TDockingStyleBottom, pN->win);
					pN->win->AppendNeighbour(TDockingStyleTop, this);
					pWin1 = pN->win;
					bAny = true;
					break;
				}
				it = pWin1->get_NextNeighbour(TDockingStyleLeft, it);
			}
			if ( !bAny )
				break;
		}
		pWin1 = pWin;
		while ( true )
		{
			bAny = false;
			it = pWin1->get_FirstNeighbour(TDockingStyleRight);
			while ( it )
			{
				TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( (r1.bottom == r2.top) && (((r2.left <= r1.left) && (r2.right > r1.left)) || ((r2.left > r1.left) && (r2.left < r1.right))) )
				{
					AppendNeighbour(TDockingStyleBottom, pN->win);
					pN->win->AppendNeighbour(TDockingStyleTop, this);
					pWin1 = pN->win;
					bAny = true;
					break;
				}
				it = pWin1->get_NextNeighbour(TDockingStyleRight, it);
			}
			if ( !bAny )
				break;
		}
		break;
	}
}

void CDockWindow::CopyNeighbours(CDockWindow* pSource)
{
	TNeighbourVector::Iterator it = pSource->get_Neighbours();
	RECT r1;
	RECT r2;

	GetParentRect(&r1);
	while ( it )
	{
		TNeighbour* pNeighbour = *it;

		pNeighbour->win->GetParentRect(&r2);
		switch ( pNeighbour->style )
		{
		case TDockingStyleLeft:
			if ( (r1.left == r2.right) && (((r2.top <= r1.top) && (r2.bottom > r1.top)) || ((r2.top > r1.top) && (r2.top < r1.bottom))) )
			{
				AppendNeighbour(TDockingStyleLeft, pNeighbour->win);
				pNeighbour->win->AppendNeighbour(TDockingStyleRight, this);
			}
			break;
		case TDockingStyleTop:
			if ( (r1.top == r2.bottom) && (((r2.left <= r1.left) && (r2.right > r1.left)) || ((r2.left > r1.left) && (r2.left < r1.right))) )
			{
				AppendNeighbour(TDockingStyleTop, pNeighbour->win);
				pNeighbour->win->AppendNeighbour(TDockingStyleBottom, this);
			}
			break;
		case TDockingStyleRight:
			if ( (r1.right == r2.left) && (((r2.top <= r1.top) && (r2.bottom > r1.top)) || ((r2.top > r1.top) && (r2.top < r1.bottom))) )
			{
				AppendNeighbour(TDockingStyleRight, pNeighbour->win);
				pNeighbour->win->AppendNeighbour(TDockingStyleLeft, this);
			}
			break;
		case TDockingStyleBottom:
			if ( (r1.bottom == r2.top) && (((r2.left <= r1.left) && (r2.right > r1.left)) || ((r2.left > r1.left) && (r2.left < r1.right))) )
			{
				AppendNeighbour(TDockingStyleBottom, pNeighbour->win);
				pNeighbour->win->AppendNeighbour(TDockingStyleTop, this);
			}
			break;
		}
		++it;
	}
}

void CDockWindow::MoveNeighbours(TDockingStyle style, LONG offset)
{
	TNeighbourVector::Iterator it = m_neighbours.Begin();

	while ( it )
	{
		TNeighbour* pNeighbour = *it;
		RECT r1;

		if ( pNeighbour->style == style )
		{
			pNeighbour->win->GetParentRect(&r1);
			switch ( style )
			{
			case TDockingStyleLeft:
				switch ( pNeighbour->win->get_dockingstyle() )
				{
				case TDockingStyleLeft:
					break;
				case TDockingStyleTop:
					r1.right -= offset;
					break;
				case TDockingStyleRight:
					r1.left -= offset;
					r1.right -= offset;
					pNeighbour->win->MoveNeighbours(TDockingStyleLeft, offset);
					break;
				case TDockingStyleBottom:
					r1.right -= offset;
					break;
				}
				break;
			case TDockingStyleTop:
				switch ( pNeighbour->win->get_dockingstyle() )
				{
				case TDockingStyleLeft:
					r1.bottom -= offset;
					break;
				case TDockingStyleTop:
					break;
				case TDockingStyleRight:
					r1.bottom -= offset;
					break;
				case TDockingStyleBottom:
					r1.top -= offset;
					r1.bottom -= offset;
					pNeighbour->win->MoveNeighbours(TDockingStyleTop, offset);
					break;
				}
				break;
			case TDockingStyleRight:
				switch ( pNeighbour->win->get_dockingstyle() )
				{
				case TDockingStyleLeft:
					r1.left += offset;
					r1.right += offset;
					pNeighbour->win->MoveNeighbours(TDockingStyleRight, offset);
					break;
				case TDockingStyleTop:
					r1.left += offset;
					break;
				case TDockingStyleRight:
					break;
				case TDockingStyleBottom:
					r1.left += offset;
					break;
				}
				break;
			case TDockingStyleBottom:
				switch ( pNeighbour->win->get_dockingstyle() )
				{
				case TDockingStyleLeft:
					r1.top += offset;
					break;
				case TDockingStyleTop:
					r1.top += offset;
					r1.bottom += offset;
					pNeighbour->win->MoveNeighbours(TDockingStyleBottom, offset);
					break;
				case TDockingStyleRight:
					r1.top += offset;
					break;
				case TDockingStyleBottom:
					break;
				}
				break;
			}
			pNeighbour->win->MoveWindow(&r1, TRUE);
		}	
		++it;
	}
}

void CDockWindow::SizeNeighbours(TDockingStyle style, LONG offset)
{
	TNeighbourVector::Iterator it = m_neighbours.Begin();

	while ( it )
	{
		TNeighbour* pNeighbour = *it;
		RECT r1;

		if ( pNeighbour->style == style )
		{
			pNeighbour->win->GetParentRect(&r1);
			switch ( style )
			{
			case TDockingStyleLeft:
				r1.right -= offset;
				break;
			case TDockingStyleTop:
				r1.bottom -= offset;
				break;
			case TDockingStyleRight:
				r1.left += offset;
				break;
			case TDockingStyleBottom:
				r1.top += offset;
				break;
			}
			pNeighbour->win->MoveWindow(&r1, TRUE);
		}	
		++it;
	}
}

void CDockWindow::ClearNeighbours()
{
	TNeighbourVector::Iterator it = m_neighbours.Begin();
#ifdef __DEBUG1__
	CDockWindowVector delPtr(16,16);
#endif

	while ( it )
	{
		CDockWindow* pWin = (*it)->win;

#ifdef __DEBUG1__
		delPtr.Append(pWin);
#endif
		pWin->RemoveNeighbour(this);
		++it;
	}

#ifdef __DEBUG1__
	CDockWindowVector::Iterator it1 = delPtr.Begin();

	while ( it1 )
	{
		CDockWindow* pWin = *it1;

		RemoveNeighbour(pWin);
		++it1;
	}
	delPtr.Close();
#else
	m_neighbours.Close();
	m_neighbours.Open(__FILE__LINE__ 16,16);
#endif
}

void CDockWindow::ClearNeighbours(TDockingStyle style)
{
	TNeighbourVector::Iterator it = get_FirstNeighbour(style);
	CDockWindowVector delPtr(__FILE__LINE__ 16,16);

	while ( it )
	{
		CDockWindow* pWin = (*it)->win;

		pWin->RemoveNeighbour(this);
		delPtr.Append(pWin);
		it = get_NextNeighbour(style, it);
	}

	CDockWindowVector::Iterator it1 = delPtr.Begin();

	while ( it1 )
	{
		CDockWindow* pWin = *it1;

		RemoveNeighbour(pWin);
		++it1;
	}
}

void CDockWindow::AppendNeighbour(TDockingStyle style, CDockWindow* win)
{
	TNeighbour nb(style, win);
	TNeighbourVector::Iterator it = m_neighbours.Find<CDockWindowNeighbourFullEqualFunctor>(&nb);

	if ( it )
		return;
	it = m_neighbours.Find<CDockWindowNeighbourEqualFunctor>(&nb);
	assert(!it);
#ifdef __DEBUG1__
	CTabControl* pTabControl = CastDynamicPtr(CTabControl, m_control);
	CTabPage* pTabPage = CastDynamicPtr(CTabPage, pTabControl->get_childbyID(1000));
	CListView* pListView = CastDynamicPtr(CListView, pTabPage->get_childbyID(1000));
	CListViewNode* pNode = OK_NEW_OPERATOR CListViewNode(pListView, win->get_name());
#endif
	m_neighbours.Append(OK_NEW_OPERATOR TNeighbour(nb));
}

void CDockWindow::RemoveNeighbour(CDockWindow* win)
{
	TNeighbour nb(win);
	TNeighbourVector::Iterator it = m_neighbours.Find<CDockWindowNeighbourEqualFunctor>(&nb);

	if ( it )
	{
		m_neighbours.Remove(it);
#ifdef __DEBUG1__
	CTabControl* pTabControl = CastDynamicPtr(CTabControl, m_control);
	CTabPage* pTabPage = CastDynamicPtr(CTabPage, pTabControl->get_childbyID(1000));
	CListView* pListView = CastDynamicPtr(CListView, pTabPage->get_childbyID(1000));
	CListViewNode* pNode = pListView->get_Node(win->get_name().GetString());
	assert(pNode);
	int ix = pListView->inx_Node(pNode);
	assert(ix >= 0);
	pListView->set_Node(ix, nullptr);
#endif
	}
}

CStringBuffer CDockWindow::DockingStyle2String(TDockingStyle style)
{
	CStringBuffer tmp;

	switch ( style )
	{
	case TDockingStyleLeft:
		tmp.SetString(__FILE__LINE__ _T("TDockingStyleLeft"));
		break;
	case TDockingStyleTop:
		tmp.SetString(__FILE__LINE__ _T("TDockingStyleTop"));
		break;
	case TDockingStyleRight:
		tmp.SetString(__FILE__LINE__ _T("TDockingStyleRight"));
		break;
	case TDockingStyleBottom:
		tmp.SetString(__FILE__LINE__ _T("TDockingStyleBottom"));
		break;
	}
	return tmp;
}

CDockWindow::TDockingStyle CDockWindow::String2DockingStyle(ConstRef(CStringLiteral) style)
{
	if ( style.Compare(_T("TDockingStyleLeft"), 0, CStringLiteral::cIgnoreCase) == 0 )
		return TDockingStyleLeft;
	if ( style.Compare(_T("TDockingStyleTop"), 0, CStringLiteral::cIgnoreCase) == 0 )
		return TDockingStyleTop;
	if ( style.Compare(_T("TDockingStyleRight"), 0, CStringLiteral::cIgnoreCase) == 0 )
		return TDockingStyleRight;
	if ( style.Compare(_T("TDockingStyleBottom"), 0, CStringLiteral::cIgnoreCase) == 0 )
		return TDockingStyleBottom;
	return TDockingStyleNone;
}

LRESULT CDockWindow::OnNcCreate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNcCreate %d\n", get_name().GetString(), debugID);
#endif
	m_nccaption = 17;
	m_ncmargins.top = 3;
	m_ncmargins.bottom = 3;
	m_ncmargins.left = 3;
	m_ncmargins.right = 3;
	if ( m_isFloating )
		return DefaultWindowProc(WM_NCCREATE, wParam, lParam);
	return TRUE;
}

LRESULT CDockWindow::OnNcActivate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNcActivate %d, wParam=%s, lParam=%x\n", get_name().GetString(), debugID,
		wParam?"activated":"deactivated", lParam);
#endif
	if ( !m_isFloating )
		return TRUE;
	else
		return DefaultWindowProc(WM_NCACTIVATE, wParam, lParam);
}

LRESULT CDockWindow::OnNcCalcSize(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
		int debugID = theGuiApp->get_DebugID();
		theGuiApp->DebugString("%ls OnNcCalcSize %d, wParam=%d\n", get_name().GetString(), debugID, wParam);
#endif
	if ( !m_isFloating )
	{
		if ( wParam )
		{
			LPNCCALCSIZE_PARAMS csparams = CastAnyPtr(NCCALCSIZE_PARAMS, lParam);

			csparams->rgrc[0].top += m_nccaption + m_ncmargins.top;
			csparams->rgrc[0].bottom -= m_ncmargins.bottom;
			csparams->rgrc[0].left += m_ncmargins.left;
			csparams->rgrc[0].right -= m_ncmargins.right;

		}
		else
		{
			LPRECT prect = CastAnyPtr(RECT, lParam);

			prect->top += m_nccaption + m_ncmargins.top;
			prect->bottom -= m_ncmargins.bottom;
			prect->left += m_ncmargins.left;
			prect->right -= m_ncmargins.right;
		}
		return 0;
	}
	return DefaultWindowProc(WM_NCCALCSIZE, wParam, lParam);
}

LRESULT CDockWindow::OnNcPaint(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
		int debugID = theGuiApp->get_DebugID();
		theGuiApp->DebugString("%ls OnNcPaint %d\n", get_name().GetString(), debugID);
#endif
	if ( !m_isFloating )
	{
		HDC hdc = ::GetWindowDC(m_hwnd);
		DWORD ret;

		if ( !hdc )
		{
			ret = ::GetLastError();
#ifdef __DEBUG1__
			theGuiApp->DebugString("%ls OnNcPaint.1 %d, error=%d\n", get_name().GetString(), debugID, ret);
#endif
			return 0;
		}
		if ( hdc )
		{
			RECT r;
			RECT r1;
			RECT r2;
			RECT r3;
			RECT r4;
			RECT r5;

			GetWindowRect(&r);
			r.bottom -= r.top;
			r.top = 0;
			r.right -= r.left;
			r.left = 0;
			::CopyRect(&r1, &r);
			r1.bottom = m_nccaption + m_ncmargins.top;
			::CopyRect(&r2, &r);
			r2.top = m_nccaption + m_ncmargins.top;
			r2.bottom -= m_ncmargins.bottom;
			r2.right = m_ncmargins.left;
			::CopyRect(&r3, &r);
			r3.top = m_nccaption + m_ncmargins.top;
			r3.bottom -= m_ncmargins.bottom;
			r3.left = r3.right - m_ncmargins.right;
			::CopyRect(&r4, &r);
			r4.top = r4.bottom - m_ncmargins.bottom;
			::CopyRect(&r5, &r1);
			r5.left += m_ncmargins.left;
			r5.right -= m_ncmargins.right;
			r5.top += m_ncmargins.top;

			Gdiplus::Graphics graphics(hdc);
			Gdiplus::Brush* brush0 = get_Brush(_T(".DockedTitle.BackgroundColor.Normal"), _T("DockWindow"), Gdiplus::Color::LightGray);

			graphics.FillRectangle(brush0, r1.left, r1.top, r1.right - r1.left, r1.bottom - r1.top);
			graphics.FillRectangle(brush0, r2.left, r2.top, r2.right - r2.left, r2.bottom - r2.top);
			graphics.FillRectangle(brush0, r3.left, r3.top, r3.right - r3.left, r3.bottom - r3.top);
			graphics.FillRectangle(brush0, r4.left, r4.top, r4.right - r4.left, r4.bottom - r4.top);
			if ( !(m_caption.IsEmpty()) )
			{
				Gdiplus::Brush* brush1 = get_Brush(_T(".DockedTitle.ForegroundColor.Normal"), _T("DockWindow"), Gdiplus::Color::Black);
				Gdiplus::Font* pFont = get_Font(_T(".DockedTitle.Font.Normal"), _T("DockWindow"));
				Gdiplus::RectF rF;

				Convert2RectF(&rF, &r5);
				graphics.DrawString(m_caption.GetString(), -1, pFont, rF, Gdiplus::StringFormat::GenericTypographic(), brush1);
			}
			::ReleaseDC(m_hwnd, hdc);
		}
		return 0;
	}
	return DefaultWindowProc(WM_NCPAINT, wParam, lParam);
}

LRESULT CDockWindow::OnNcDestroy(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNcDestroy %d\n", get_name().GetString(), debugID);
#endif
	if ( m_isFloating )
		DefaultWindowProc(WM_NCDESTROY, wParam, lParam);
	if ( m_pDockInfo )
		m_pDockInfo->Destroy(this);
	::SetWindowLongPtr(m_hwnd, GWLP_USERDATA, NULL);
	delete this;
	return 0;
}

LRESULT CDockWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnLButtonDown %d, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, 
		GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
#endif
	return 0;
}

LRESULT CDockWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnLButtonUp %d, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, 
		GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
#endif
	if ( has_Capture() )
	{
		set_Capture(FALSE);
		if ( m_isFloating )
		{
			if ( m_pDockInfo )
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				CDockInfo* pDockInfo = m_pDockInfo;

				ClientToScreen(&pt);
				pDockInfo->CheckDockTargets(&pt, true);
				pDockInfo->HideDockTargets();
			}

		}
	}
	return 0;
}

void CDockWindow::NcHitTest(POINT pt)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls Before NcHitTest %d, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, 
		pt.x, pt.y);
#endif
	RECT r;
	RECT rtop;
	RECT rtopright;
	RECT rtopleft;
	RECT rleft;
	RECT rright;
	RECT rbottom;
	RECT rbottomright;
	RECT rbottomleft;

	GetWindowRect(&r);

	::CopyRect(&rtop, &r);
	rtop.bottom = rtop.top + m_ncmargins.top;
	rtop.left += m_ncmargins.left;
	rtop.right -= m_ncmargins.right;

	::CopyRect(&rtopright, &r);
	rtopright.bottom = rtopright.top + m_ncmargins.top;
	rtopright.left = rtopright.right - m_ncmargins.right;

	::CopyRect(&rtopleft, &r);
	rtopleft.bottom = rtopleft.top + m_ncmargins.top;
	rtopleft.right = rtopleft.left + m_ncmargins.left;

	::CopyRect(&rleft, &r);
	rleft.top += m_ncmargins.top;
	rleft.right = rleft.left + m_ncmargins.left;
	rleft.bottom -= m_ncmargins.bottom;

	::CopyRect(&rright, &r);
	rright.top += m_ncmargins.top;
	rright.left = rright.right - m_ncmargins.right;
	rright.bottom -= m_ncmargins.bottom;

	::CopyRect(&rbottom, &r);
	rbottom.top = rbottom.bottom - m_ncmargins.bottom;
	rbottom.left += m_ncmargins.left;
	rbottom.right -= m_ncmargins.right;

	::CopyRect(&rbottomright, &r);
	rbottomright.top = rbottomright.bottom - m_ncmargins.bottom;
	rbottomright.left = rbottomright.right - m_ncmargins.right;

	::CopyRect(&rbottomleft, &r);
	rbottomleft.top = rbottomleft.bottom - m_ncmargins.bottom;
	rbottomleft.right = rbottomleft.left + m_ncmargins.left;

	if ( ::PtInRect(&rtop, pt) )
	{
		m_SizingEdge = HTBORDER;
		if ( m_pDockInfo && m_pDockInfo->TestSizable(this, HTTOP) )
		{
			m_SizingEdge = HTTOP;
			SetCursor(LoadCursor(nullptr, IDC_SIZENS));
		}
	}
	else if ( ::PtInRect(&rtopleft, pt) )
	{
		m_SizingEdge = HTBORDER;
		if ( m_pDockInfo && m_pDockInfo->TestSizable(this, HTTOPLEFT) )
		{
			m_SizingEdge = HTTOPLEFT;
			SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
		}
	}
	else if ( ::PtInRect(&rtopright, pt) )
	{
		m_SizingEdge = HTBORDER;
		if ( m_pDockInfo && m_pDockInfo->TestSizable(this, HTTOPRIGHT) )
		{
			m_SizingEdge = HTTOPRIGHT;
			SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
		}
	}
	else if ( ::PtInRect(&rleft, pt) )
	{
		m_SizingEdge = HTBORDER;
		if ( m_pDockInfo && m_pDockInfo->TestSizable(this, HTLEFT) )
		{
			m_SizingEdge = HTLEFT;
			SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
		}
	}
	else if ( ::PtInRect(&rright, pt) )
	{
		m_SizingEdge = HTBORDER;
		if ( m_pDockInfo && m_pDockInfo->TestSizable(this, HTRIGHT) )
		{
			m_SizingEdge = HTRIGHT;
			SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
		}
	}
	else if ( ::PtInRect(&rbottom, pt) )
	{
		m_SizingEdge = HTBORDER;
		if ( m_pDockInfo && m_pDockInfo->TestSizable(this, HTBOTTOM) )
		{
			m_SizingEdge = HTBOTTOM;
			SetCursor(LoadCursor(nullptr, IDC_SIZENS));
		}
	}
	else if ( ::PtInRect(&rbottomright, pt) )
	{
		m_SizingEdge = HTBORDER;
		if ( m_pDockInfo && m_pDockInfo->TestSizable(this, HTBOTTOMRIGHT) )
		{
			m_SizingEdge = HTBOTTOMRIGHT;
			SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
		}
	}
	else if ( ::PtInRect(&rbottomleft, pt) )
	{
		m_SizingEdge = HTBORDER;
		if ( m_pDockInfo && m_pDockInfo->TestSizable(this, HTBOTTOMLEFT) )
		{
			m_SizingEdge = HTBOTTOMLEFT;
			SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
		}
	}
	else
	{
		m_SizingEdge = HTNOWHERE;
		SetCursor(LoadCursor(nullptr, IDC_ARROW));
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls After NcHitTest %d, m_SizingEdge=%s\n", get_name().GetString(), debugID, 
		vNCHitTestCodes[m_SizingEdge+2]);
#endif
}

LRESULT CDockWindow::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNcLButtonDown %d, wParam=%s, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, 
		vNCHitTestCodes[wParam+2], GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
#endif
	if ( wParam == HTCAPTION )
	{
		if ( !m_isFloating )
		{
			m_mouseclick.x = GET_X_LPARAM(lParam);
			m_mouseclick.y = GET_Y_LPARAM(lParam);
			set_Capture(TRUE);
			m_SizingEdge = HTCAPTION;
		}
		else 
		{
			RECT r;
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			GetWindowRect(&r);

			m_FloatingRect.left = r.left;
			m_FloatingRect.top = r.top;
			m_FloatingRect.right = pt.x;
			m_FloatingRect.bottom = pt.y;
			set_Capture(TRUE);
			if ( m_pDockInfo )
			{
				m_pDockInfo->ShowDockTargets(this);
				m_pDockInfo->CheckDockTargets(&pt);
			}
		}
		return 0;
	}
	if ( (wParam == HTBORDER) && (!m_isFloating) )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		NcHitTest(pt);
		if ( (m_SizingEdge != HTNOWHERE) && (m_SizingEdge != HTBORDER) )
		{
			m_mouseclick = pt;
			set_Capture(TRUE);
		}
		return 0;
	}
	return DefaultWindowProc(WM_NCLBUTTONDOWN, wParam, lParam);
}

LRESULT CDockWindow::OnNcLButtonUp(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNcLButtonUp %d, wParam=%s, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, 
		vNCHitTestCodes[wParam+2], GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
#endif
	if ( !m_isFloating )
		return 0;
	return DefaultWindowProc(WM_NCLBUTTONUP, wParam, lParam);
}

LRESULT CDockWindow::OnNcMouseMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNcMouseMove %d, wParam=%s, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, 
		vNCHitTestCodes[wParam+2], GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
#endif
	if ( !m_isFloating )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		NcHitTest(pt);
		return 0;
	}
	return DefaultWindowProc(WM_NCMOUSEMOVE, wParam, lParam);
}

LRESULT CDockWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( has_Capture() && (wParam == MK_LBUTTON) )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		ClientToScreen(&pt);

#ifdef __DEBUG1__
		int debugID = theGuiApp->get_DebugID();
		theGuiApp->DebugString("%ls Before OnMouseMove %d, wParam=%x, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, 
			wParam, pt.x, pt.y);
#endif
		if ( !m_isFloating  )
		{
			if ( m_SizingEdge == HTCAPTION )
			{
				if ( (abs(m_mouseclick.x - pt.x) > 10) || (abs(m_mouseclick.y - pt.y) > 10) )
				{
					//set_Capture(FALSE);
#ifdef __DEBUG1__
					theGuiApp->DebugString("%ls OnMouseMove.1 %d, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, pt.x, pt.y);
#endif
					RECT r;

					GetWindowRect(&r);

					m_FloatingRect.left = r.left;
					m_FloatingRect.top = r.top;
					m_FloatingRect.right = pt.x;
					m_FloatingRect.bottom = pt.y;

					UnDock(&m_FloatingRect);
					if ( m_pDockInfo )
					{
						m_pDockInfo->UpdateFrame(this);
						m_pDockInfo->ShowDockTargets(this);
						m_pDockInfo->CheckDockTargets(&pt);
					}
				}
			}
			else if ( (m_SizingEdge != HTNOWHERE) && (m_SizingEdge != HTBORDER) )
			{
				POINT pt1 = { pt.x - m_mouseclick.x, pt.y - m_mouseclick.y };
				bool bResize = false;

#ifdef __DEBUG1__
				theGuiApp->DebugString("%ls OnMouseMove.2 %d, m_SizingEdge=%s, x=0x%x, y=0x%x\n", get_name().GetString(), debugID, 
					vNCHitTestCodes[m_SizingEdge+2], pt1.x, pt1.y);
#endif
				switch ( m_SizingEdge )
				{
				case HTLEFT:
				case HTRIGHT:
					if ( (pt1.x > 4) || (pt1.x < -4) )
						bResize = true;
					break;
				case HTTOP:
				case HTBOTTOM:
					if ( (pt1.y > 4) || (pt1.y < -4) )
						bResize = true;
					break;
				case HTTOPLEFT:
				case HTTOPRIGHT:
				case HTBOTTOMLEFT:
				case HTBOTTOMRIGHT:
					if ( (pt1.x > 4) || (pt1.x < -4) || (pt1.y > 4) || (pt1.y < -4) )
						bResize = true;
					break;
				}
				if ( bResize )
				{
					if ( m_pDockInfo )
					{
						if ( !(m_pDockInfo->Resize(this, m_SizingEdge, pt1)) )
							::SetCursorPos(m_mouseclick.x, m_mouseclick.y);
						else
							m_mouseclick = pt;
					}
				}
			}
		}
		else
		{
			m_FloatingRect.left += pt.x - m_FloatingRect.right;
			m_FloatingRect.top += pt.y - m_FloatingRect.bottom;

			SetWindowPos(&m_FloatingRect, SWP_NOSIZE);
			m_FloatingRect.right = pt.x;
			m_FloatingRect.bottom = pt.y;
			if ( m_pDockInfo )
				m_pDockInfo->CheckDockTargets(&pt);
		}
	}
	return 0;
}

LRESULT CDockWindow::OnCreate(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDockWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	RECT r3;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnSize %d\n", get_name().GetString(), debugID);
#endif
	if ( !GetClientRect(&rect) )
		return -1;
	if ( ::IsRectEmpty(&rect) )
		return -1;
	::CopyRect(&r3, &rect);
	if ( !m_control )
		return -1;
	m_control->MoveWindow(&r3, TRUE);
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnSize %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

#ifdef __DEBUG1__
static const char* vSizingCodes[] = {
	"",
	"WMSZ_LEFT", 
	"WMSZ_RIGHT",
	"WMSZ_TOP",
	"WMSZ_TOPLEFT",
	"WMSZ_TOPRIGHT",
	"WMSZ_BOTTOM",
	"WMSZ_BOTTOMLEFT",
	"WMSZ_BOTTOMRIGHT"
};
#endif

LRESULT CDockWindow::OnSizing(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	CWin* frame = get_frame();
	RECT r;

	::CopyRect(&r, (LPRECT)lParam);
	theGuiApp->DebugString("%ls OnSizing.1 %d, wParam=%s, l=0x%x, r=0x%x, t=0x%x, b=0x%x\n", get_name().GetString(), debugID,
		vSizingCodes[wParam], r.left, r.right, r.top, r.bottom);
	GetWindowRect(&r);
	//frame->ScreenToClient(&r);
	theGuiApp->DebugString("%ls OnSizing.2 %d, wParam=%s, l=0x%x, r=0x%x, t=0x%x, b=0x%x\n", get_name().GetString(), debugID,
		vSizingCodes[wParam], r.left, r.right, r.top, r.bottom);
#endif
	return TRUE;
}

LRESULT CDockWindow::OnEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnEnterSizeMove %d\n", get_name().GetString(), debugID);
#endif
	m_isSizing = TRUE;
	return 0;
}

LRESULT CDockWindow::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnExitSizeMove %d\n", get_name().GetString(), debugID);
#endif
	m_isSizing = FALSE;
	return 0;
}

LRESULT CDockWindow::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = DefaultWindowProc(WM_NCHITTEST, wParam, lParam);

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnNcHitTest %d\n", get_name().GetString(), debugID);
#endif
	if ( (!m_isFloating) && (ret == HTNOWHERE) )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		RECT r;
		RECT rcaption;

		GetWindowRect(&r);

		::CopyRect(&rcaption, &r);
		rcaption.top += m_ncmargins.top;
		rcaption.left += m_ncmargins.left;
		rcaption.right -= m_ncmargins.right;
		rcaption.bottom = rcaption.top + m_nccaption;

		if ( ::PtInRect(&rcaption, pt) )
			ret = HTCAPTION;
		else
			ret = HTBORDER;
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnNcHitTest %d: x=0x%x, y=0x%x, ret=%s\n", get_name().GetString(), debugID, 
		GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), vNCHitTestCodes[ret+2]);
#endif
	return ret;
}

LRESULT CDockWindow::OnTabPageDragged(WPARAM wParam, LPARAM lParam)
{
	if ( !m_pDockInfo )
		return 0;

	NotifyMessage* pNM = CastAnyPtr(NotifyMessage, lParam);
	CTabPage* pTabPageDragged = CastAnyPtr(CTabPage, pNM->param);
	CTabControl* pTabControlDragged = CastDynamicPtr(CTabControl, pTabPageDragged->get_parent());
	RECT r;
	POINT pt;

	pTabPageDragged->GetWindowRect(&r);
	::GetCursorPos(&pt);

	CDockWindow* pdockwindow = m_pDockInfo->CreateFloating(nullptr, &r);
	CTabControl* pTabControl = CastDynamicPtr(CTabControl, pdockwindow->get_Client());
	CStringBuffer name;

	pTabPageDragged->set_parenthandle(pTabControl->get_handle());
	pTabPageDragged->set_childID(1000);
	name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), pTabControl->get_name(), pTabPageDragged->get_childID());
	pTabPageDragged->set_name(name);
	pTabPageDragged->set_Text(name);
	pTabControl->set_CurrentTabPage(nullptr);
	pTabControl->SendMessage(WM_SIZE, 0, 0);
	pdockwindow->SendMessage(WM_SIZE, 0, 0);

	pTabControlDragged->set_CurrentTabPage(nullptr);
	pTabControlDragged->SendMessage(WM_SIZE, 0, 0);

	pdockwindow->GetWindowRect(&r);
	pt.x = r.left + ((r.right - r.left) / 2);
	pt.y = r.top + 5;
	::SetCursorPos(pt.x, pt.y);
	pdockwindow->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, POINTTOPOINTS(pt));
	pdockwindow->ScreenToClient(&pt);
	pdockwindow->PostMessage(WM_MOUSEMOVE, MK_LBUTTON, POINTTOPOINTS(pt));
	return TRUE;
}

LRESULT CDockWindow::OnTabPageSelected(WPARAM wParam, LPARAM lParam)
{
	NotifyMessage* pNM = CastAnyPtr(NotifyMessage, lParam);
	CTabPage* pTabPageSelected = CastAnyPtr(CTabPage, pNM->param);

	if ( pTabPageSelected )
		set_Caption(pTabPageSelected->get_Text());
	else
		set_Caption(CStringBuffer(__FILE__LINE__ _T("")));
	return 0;
}

//***********************************************************
// CDockTargets
//***********************************************************
class CDockTarget : public CPopup
{
public:
	CDockTarget(LPCTSTR name = nullptr);
	CDockTarget(ConstRef(CStringBuffer) name);
	virtual ~CDockTarget();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	virtual BOOL CheckTarget(LPPOINT pt);
	virtual void SetInitialPos(LPRECT r);

	__inline bool is_Hovered() { return m_bHovered; }
	__inline void set_Hovered(bool b) { if ( m_bHovered != b ) { m_bHovered = b; if ( is_created() ) InvalidateRect(nullptr, FALSE); } }

	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void DrawImage(Gdiplus::Graphics* graphics, Ref(CBitmap) bitmap, INT x, INT y, bool bPaintTransparent, bool bHovered);

	CBitmap m_bmImage;
	bool m_bHovered;
};

BEGIN_MESSAGE_MAP(CPopup, CDockTarget)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CDockTarget::CDockTarget(LPCTSTR name):
	CPopup(name),
	m_bHovered(false)
{
}

CDockTarget::CDockTarget(ConstRef(CStringBuffer) name):
	CPopup(name),
	m_bHovered(false)
{
}

CDockTarget::~CDockTarget()
{
}

void CDockTarget::DrawImage(Gdiplus::Graphics* graphics, Ref(CBitmap) bitmap, INT x, INT y, bool bPaintTransparent, bool bHovered)
{
	if ( Gdiplus::Bitmap* pBitmap = bitmap.get_bitmap() )
	{
		Gdiplus::ImageAttributes imAtt;
		Gdiplus::Rect destRect;

		if ( bPaintTransparent )
		{
			Gdiplus::Color transparentcolor;

			pBitmap->GetPixel(0, pBitmap->GetHeight() - 1, &transparentcolor);
			imAtt.SetColorKey(transparentcolor, transparentcolor, Gdiplus::ColorAdjustTypeBitmap);
		}
		destRect.X = x;
		destRect.Y = y;
		destRect.Height = pBitmap->GetHeight();
		destRect.Width = pBitmap->GetWidth();

		graphics->DrawImage(pBitmap, destRect, 0, 0, 
			pBitmap->GetWidth(), 
			pBitmap->GetHeight(), Gdiplus::UnitPixel, &imAtt);

		if ( bHovered )
		{
			Gdiplus::Color vColor(Gdiplus::Color::Blue);
			Gdiplus::Color vColor1(Gdiplus::Color::MakeARGB(128, vColor.GetR(), vColor.GetG(), vColor.GetB()));
			Gdiplus::SolidBrush brush(vColor1);

			graphics->FillRectangle(&brush, destRect);
		}
	}
}

BOOL CDockTarget::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = _T("CDOCKTARGET");
	return TRUE;
}

BOOL CDockTarget::PreCreate(CREATESTRUCT& cs)
{
	cs.style = WS_POPUP;
	cs.dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	cs.x = 0;
	cs.y = 0;
	cs.cx = 0;
	cs.cy = 0;
	if ( Gdiplus::Bitmap* pBitmap = m_bmImage.get_bitmap() )
	{
		cs.cx = pBitmap->GetWidth();
		cs.cy = pBitmap->GetHeight();
	}
	return TRUE;
}

BOOL CDockTarget::CheckTarget(LPPOINT pt)
{
	POINT pt1 = { pt->x, pt->y };
	RECT r;

	ScreenToClient(&pt1);
	GetClientRect(&r);
	if ( ::PtInRect(&r, pt1) )
	{
		set_Hovered(true);
		return TRUE;
	}
	set_Hovered(false);
	return FALSE;
}

void CDockTarget::SetInitialPos(LPRECT r)
{
}

LRESULT CDockTarget::OnCreate(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDockTarget::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;
	
	Gdiplus::Graphics graphics(hdc);

	DrawImage(&graphics, m_bmImage, 0, 0, false, m_bHovered);
	EndPaint(m_hwnd, &ps);
	return 0;
}

class CDockTargetCenter : public CDockTarget
{
public:
	CDockTargetCenter();
	virtual ~CDockTargetCenter();

	virtual BOOL CheckTarget(LPPOINT pt);
	virtual void SetInitialPos(LPRECT r);

	__inline int is_IHovered() { return m_iHovered; }
	__inline void set_IHovered(int b) { if ( m_iHovered != b ) { m_iHovered = b; if ( is_created() ) InvalidateRect(nullptr, FALSE); } }

	__inline bool is_OverClientArea() { return m_isOverClientArea; }
	__inline void set_OverClientArea(bool b) { if ( m_isOverClientArea != b ) { m_isOverClientArea = b; if ( is_created() ) InvalidateRect(nullptr, FALSE); } }

	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	CBitmap m_bmImageLeft;
	CBitmap m_bmImageTop;
	CBitmap m_bmImageMiddle;
	CBitmap m_bmImageRight;
	CBitmap m_bmImageBottom;
	int m_iHovered;
	bool m_isOverClientArea;
};

BEGIN_MESSAGE_MAP(CDockTarget, CDockTargetCenter)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CDockTargetCenter::CDockTargetCenter():
	CDockTarget(_T("CDockTargetCenter")),
	m_iHovered(-1)
{
	m_bmImage = CBitmap(MAKEINTRESOURCE(IDB_SDCENTER), IMAGE_BITMAP);
	m_bmImageLeft = CBitmap(MAKEINTRESOURCE(IDB_SDLEFT), IMAGE_BITMAP);
	m_bmImageTop = CBitmap(MAKEINTRESOURCE(IDB_SDTOP), IMAGE_BITMAP);
	m_bmImageMiddle = CBitmap(MAKEINTRESOURCE(IDB_SDMIDDLE), IMAGE_BITMAP);
	m_bmImageRight = CBitmap(MAKEINTRESOURCE(IDB_SDRIGHT), IMAGE_BITMAP);
	m_bmImageBottom = CBitmap(MAKEINTRESOURCE(IDB_SDBOTTOM), IMAGE_BITMAP);
}

CDockTargetCenter::~CDockTargetCenter()
{
}

BOOL CDockTargetCenter::CheckTarget(LPPOINT pt)
{
	POINT pt1 = { pt->x, pt->y };
	RECT rcLeft = { 0, 29, 31, 58 };
	RECT rcTop = { 29, 0, 58, 31 };
	RECT rcMiddle = { 31, 31, 56, 57 };
	RECT rcRight = { 55, 29, 87, 58 };
	RECT rcBottom = { 29, 55, 58, 87 };

	ScreenToClient(&pt1);
	if ( ::PtInRect(&rcLeft, pt1) )
	{
		set_IHovered(0);
		return 1;
	}
	if ( ::PtInRect(&rcTop, pt1) )
	{
		set_IHovered(1);
		return 2;
	}
	if ( (!m_isOverClientArea) && (::PtInRect(&rcMiddle, pt1)) )
	{
		set_IHovered(2);
		return 3;
	}
	if ( ::PtInRect(&rcRight, pt1) )
	{
		set_IHovered(3);
		return 4;
	}
	if ( ::PtInRect(&rcBottom, pt1) )
	{
		set_IHovered(4);
		return 5;
	}
	set_IHovered(-1);
	return FALSE;
}

void CDockTargetCenter::SetInitialPos(LPRECT r)
{
	RECT r1;
	RECT r2;

	GetWindowRect(&r1);
	::SetRectEmpty(&r2);
	r2.left = r->left + ((r->right - r->left - r1.right + r1.left) / 2);
	r2.top = r->top + ((r->bottom - r->top - r1.bottom + r1.top) / 2);
	if ( (r1.left != r2.left) || (r1.top != r2.top) || (!(is_Visible())) )
	{
		if ( is_Visible() )
			SetWindowPos(&r2, SWP_NOSIZE);
		else
			SetWindowPos(HWND_TOPMOST, &r2, SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}

LRESULT CDockTargetCenter::OnCreate(WPARAM wParam, LPARAM lParam)
{
	POINT ptArray[16] = { {0,29}, {22, 29}, {29, 22}, {29, 0},
		                    {58, 0}, {58, 22}, {64, 29}, {87, 29},
		                    {87, 58}, {64, 58}, {58, 64}, {58, 87},
		                    {29, 87}, {29, 64}, {23, 58}, {0, 58} };

	HRGN hRgn = ::CreatePolygonRgn(ptArray, 16, WINDING);
	int iResult = ::SetWindowRgn(m_hwnd, hRgn, FALSE);
	if (!iResult && hRgn)
		::DeleteObject(hRgn);
	return 0;
}

LRESULT CDockTargetCenter::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;
	
	Gdiplus::Graphics graphics(hdc);

	DrawImage(&graphics, m_bmImage, 0, 0, true, false);
	DrawImage(&graphics, m_bmImageLeft, 0, 29, true, (m_iHovered == 0));
	DrawImage(&graphics, m_bmImageTop, 29, 0, true, (m_iHovered == 1));
	if ( !m_isOverClientArea )
		DrawImage(&graphics, m_bmImageMiddle, 31, 31, true, (m_iHovered == 2));
	DrawImage(&graphics, m_bmImageRight, 55, 29, true, (m_iHovered == 3));
	DrawImage(&graphics, m_bmImageBottom, 29, 55, true, (m_iHovered == 4));

	EndPaint(m_hwnd, &ps);
	return 0;
}

class CDockTargetLeft : public CDockTarget
{
public:
	CDockTargetLeft();

	virtual void SetInitialPos(LPRECT r);
};

CDockTargetLeft::CDockTargetLeft():
    CDockTarget(_T("CDockTargetLeft"))
{
	m_bmImage = CBitmap(MAKEINTRESOURCE(IDB_SDLEFT), IMAGE_BITMAP);
}

void CDockTargetLeft::SetInitialPos(LPRECT r)
{
	RECT r1;
	RECT r2;

	GetWindowRect(&r1);
	::SetRectEmpty(&r2);
	r2.left = r->left + 20;
	r2.top = r->top + ((r->bottom - r->top - r1.bottom + r1.top) / 2);
	SetWindowPos(HWND_TOPMOST, &r2, SWP_NOSIZE | SWP_SHOWWINDOW);
}

class CDockTargetTop : public CDockTarget
{
public:
	CDockTargetTop();

	virtual void SetInitialPos(LPRECT r);
};

CDockTargetTop::CDockTargetTop():
    CDockTarget(_T("CDockTargetTop"))
{
	m_bmImage = CBitmap(MAKEINTRESOURCE(IDB_SDTOP), IMAGE_BITMAP);
}

void CDockTargetTop::SetInitialPos(LPRECT r)
{
	RECT r1;
	RECT r2;

	GetWindowRect(&r1);
	::SetRectEmpty(&r2);
	r2.left = r->left + ((r->right - r->left- r1.right + r1.left) / 2);
	r2.top = r->top + 20;
	SetWindowPos(HWND_TOPMOST, &r2, SWP_NOSIZE | SWP_SHOWWINDOW);
}

class CDockTargetRight : public CDockTarget
{
public:
	CDockTargetRight();

	virtual void SetInitialPos(LPRECT r);
};

CDockTargetRight::CDockTargetRight():
    CDockTarget(_T("CDockTargetRight"))
{
	m_bmImage = CBitmap(MAKEINTRESOURCE(IDB_SDRIGHT), IMAGE_BITMAP);
}

void CDockTargetRight::SetInitialPos(LPRECT r)
{
	RECT r1;
	RECT r2;

	GetWindowRect(&r1);
	::SetRectEmpty(&r2);
	r2.left = r->right - r1.right + r1.left - 20;
	r2.top = r->top + ((r->bottom - r->top - r1.bottom + r1.top) / 2);
	SetWindowPos(HWND_TOPMOST, &r2, SWP_NOSIZE | SWP_SHOWWINDOW);
}

class CDockTargetBottom : public CDockTarget
{
public:
	CDockTargetBottom();

	virtual void SetInitialPos(LPRECT r);
};

CDockTargetBottom::CDockTargetBottom():
    CDockTarget(_T("CDockTargetBottom"))
{
	m_bmImage = CBitmap(MAKEINTRESOURCE(IDB_SDBOTTOM), IMAGE_BITMAP);
}

void CDockTargetBottom::SetInitialPos(LPRECT r)
{
	RECT r1;
	RECT r2;

	GetWindowRect(&r1);
	::SetRectEmpty(&r2);
	r2.left = r->left + ((r->right - r->left- r1.right + r1.left) / 2);
	r2.top = r->bottom - r1.bottom + r1.top - 20;
	SetWindowPos(HWND_TOPMOST, &r2, SWP_NOSIZE | SWP_SHOWWINDOW);
}

//***********************************************************
// CDockHint
//***********************************************************
class CDockHint : public CPopup
{
public:
	CDockHint();
	virtual ~CDockHint();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	void SetPos(LPRECT r);

	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPopup, CDockHint)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CDockHint::CDockHint():
	CPopup(_T("CDockHint"))
{
}

CDockHint::~CDockHint()
{
}

BOOL CDockHint::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = _T("CDOCKHINT");
	return TRUE;
}

BOOL CDockHint::PreCreate(CREATESTRUCT& cs)
{
	cs.style = WS_POPUP;
	cs.dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	cs.x = 0;
	cs.y = 0;
	cs.cx = 100;
	cs.cy = 100;
	return TRUE;
}

void CDockHint::SetPos(LPRECT r)
{
	RECT r1;

	GetWindowRect(&r1);
	if ( !(::EqualRect(&r1, r)) || (!(is_Visible())) )
	{
		UINT flags = is_Visible()?0:SWP_SHOWWINDOW;

		SetWindowPos(r, flags);
	}
}

LRESULT CDockHint::OnCreate(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDockHint::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;
	
	RECT r;

	GetClientRect(&r);

	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Rect destRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
	Gdiplus::Color vColor(Gdiplus::Color::LightBlue);
	Gdiplus::Color vColor1(Gdiplus::Color::MakeARGB(128, vColor.GetR(), vColor.GetG(), vColor.GetB()));
	Gdiplus::SolidBrush brush(vColor1);

	graphics.FillRectangle(&brush, destRect);

	EndPaint(m_hwnd, &ps);
	return 0;
}

//***********************************************************
// CDockWindowVector
//***********************************************************
CDockWindowVector::CDockWindowVector(DECL_FILE_LINE TListCnt max, TListCnt exp):
	super(ARGS_FILE_LINE max, exp),
	m_curtransact(-1)
{
}

CDockWindowVector::~CDockWindowVector()
{
}

void CDockWindowVector::StartTransact()
{
	++m_curtransact;
	m_transact[m_curtransact] = Count();
}

void CDockWindowVector::Rollback()
{
	while ( m_transact[m_curtransact] < Count() )
		Remove(Last());
	--m_curtransact;
}

void CDockWindowVector::Commit()
{
	--m_curtransact;
}

bool CDockWindowVector::CheckSize(CDockWindow::TDockingStyle style, bool isNeighbour, POINT delta)
{
	Iterator it = Begin();
	RECT r;

	while ( it )
	{
		CDockWindow* pWin = *it;

		pWin->GetParentRect(&r);
		switch ( style )
		{
		case CDockWindow::TDockingStyleLeft:
			if ( isNeighbour )
			{
				r.right += delta.x;
				if ( ((r.right - r.left) < 100) && (delta.x < 0) )
					return false;
			}
			else
			{
				r.left += delta.x;
				if ( ((r.right - r.left) < 100) && (delta.x > 0) )
					return false;
			}
			break;
		case CDockWindow::TDockingStyleTop:
			if ( isNeighbour )
			{
				r.bottom += delta.y;
				if ( ((r.bottom - r.top) < 100) && (delta.y < 0) )
					return false;
			}
			else
			{
				r.top += delta.y;
				if ( ((r.bottom - r.top) < 100) && (delta.y > 0) )
					return false;
			}
			break;
		case CDockWindow::TDockingStyleRight:
			if ( isNeighbour )
			{
				r.left += delta.x;
				if ( ((r.right - r.left) < 100) && (delta.x > 0) )
					return false;
			}
			else
			{
				r.right += delta.x;
				if ( ((r.right - r.left) < 100) && (delta.x < 0) )
					return false;
			}
			break;
		case CDockWindow::TDockingStyleBottom:
			if ( isNeighbour )
			{
				r.top += delta.y;
				if ( ((r.bottom - r.top) < 100) && (delta.y > 0) )
					return false;
			}
			else
			{
				r.bottom += delta.y;
				if ( ((r.bottom - r.top) < 100) && (delta.y < 0) )
					return false;
			}
			break;
		}
		++it;
	}
	return true;
}

void CDockWindowVector::Resize(CDockWindow::TDockingStyle style, bool isNeighbour, POINT delta, RECT clientR, bool* bClientRUpdate)
{
	Iterator it = Begin();
	RECT r;
	LONG vmax = LONG_MAX;
	CDockWindow* pmax = nullptr;
	LONG vmin = 0;
	CDockWindow* pmin = nullptr;

	while ( it )
	{
		CDockWindow* pWin = *it;

		pWin->GetParentRect(&r);
		switch ( style )
		{
		case CDockWindow::TDockingStyleLeft:
		case CDockWindow::TDockingStyleRight:
			if ( r.top < vmax )
			{
				pmax = pWin;
				vmax = r.top;
			}
			if ( r.bottom > vmin )
			{
				pmin = pWin;
				vmin = r.bottom;
			}
			break;
		case CDockWindow::TDockingStyleTop:
		case CDockWindow::TDockingStyleBottom:
			if ( r.left < vmax )
			{
				pmax = pWin;
				vmax = r.left;
			}
			if ( r.right > vmin )
			{
				pmin = pWin;
				vmin = r.right;
			}
			break;
		}
		switch ( style )
		{
		case CDockWindow::TDockingStyleLeft:
			if ( isNeighbour )
			{
				if ( r.right == clientR.right )
					*bClientRUpdate = true;
				r.right += delta.x;
			}
			else
			{
				if ( r.left == clientR.right )
					*bClientRUpdate = true;
				r.left += delta.x;
			}
			break;
		case CDockWindow::TDockingStyleTop:
			if ( isNeighbour )
			{
				if ( r.bottom == clientR.bottom )
					*bClientRUpdate = true;
				r.bottom += delta.y;
			}
			else
			{
				if ( r.top == clientR.bottom )
					*bClientRUpdate = true;
				r.top += delta.y;
			}
			break;
		case CDockWindow::TDockingStyleRight:
			if ( isNeighbour )
			{
				if ( r.left == clientR.left )
					*bClientRUpdate = true;
				r.left += delta.x;
			}
			else
			{
				if ( r.right == clientR.left )
					*bClientRUpdate = true;
				r.right += delta.x;
			}
			break;
		case CDockWindow::TDockingStyleBottom:
			if ( isNeighbour )
			{
				if ( r.top == clientR.top )
					*bClientRUpdate = true;
				r.top += delta.y;
			}
			else
			{
				if ( r.bottom == clientR.top )
					*bClientRUpdate = true;
				r.bottom += delta.y;
			}
			break;
		}
		pWin->MoveWindow(&r, TRUE);
		++it;
	}
	switch ( style )
	{
	case CDockWindow::TDockingStyleLeft:
	case CDockWindow::TDockingStyleRight:
		if ( pmax )
			pmax->AdjustNeighbours(CDockWindow::TDockingStyleTop);
		if ( pmin )
			pmin->AdjustNeighbours(CDockWindow::TDockingStyleBottom);
		break;
	case CDockWindow::TDockingStyleTop:
	case CDockWindow::TDockingStyleBottom:
		if ( pmax )
			pmax->AdjustNeighbours(CDockWindow::TDockingStyleLeft);
		if ( pmin )
			pmin->AdjustNeighbours(CDockWindow::TDockingStyleRight);
		break;
	}
}

//***********************************************************
// CDockInfo
//***********************************************************
class FloatingWindowForEachFunctor
{
public:
	bool operator()(Ptr(CDockWindow) r1)
	{
		r1->DestroyWindow();
		return true;
	}
};

typedef struct _tagFindDockWindowByNameParams
{
	CStringLiteral name;
	CDockWindow* pResult;

	_tagFindDockWindowByNameParams(ConstRef(CStringLiteral) _name):
		name(_name), pResult(nullptr) {}
} TFindDockWindowByNameParams;

class FindDockWindowByNameForEachFunctor
{
public:
	FindDockWindowByNameForEachFunctor(ConstRef(CStringLiteral) _name) :
		_params(_name)
	{}

	bool operator()(Ptr(CDockWindow) r1)
	{
		if (_params.name.Compare(r1->get_name(), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			_params.pResult = r1;
			return false;
		}
		return true;
	}

	TFindDockWindowByNameParams _params;
};

typedef struct _tagFindDockWindowParams
{
	POINT pt;
	CDockWindow* pResult;
	bool bAnyNonFloatingWindows;

	_tagFindDockWindowParams(POINT _pt) :
		pt(_pt), pResult(nullptr), bAnyNonFloatingWindows(false) {}
} TFindDockWindowParams;

class FindDockWindowForEachFunctor
{
public:
	FindDockWindowForEachFunctor(POINT pt) :
		_params(pt)
	{}

	bool operator()(Ptr(CDockWindow) r1)
	{
		RECT r;

		_params.bAnyNonFloatingWindows = true;
		r1->GetWindowRect(&r);
		if (::PtInRect(&r, _params.pt))
		{
			_params.pResult = r1;
			return false;
		}
		return true;
	}

	TFindDockWindowParams _params;
};

typedef struct _tagSortDockWindowsParams
{
	enum TSortOrder {
		TSortOrderLeft,
		TSortOrderTop,
		TSortOrderRight,
		TSortOrderBottom
	};

	TSortOrder sortorder;
	bool inverse;

	_tagSortDockWindowsParams(TSortOrder _sortorder, bool _inverse = false) :
		sortorder(_sortorder), inverse(_inverse) {}

} TSortDockWindowsParams;

class SortDockWindowsLessFunctor
{
public:
	SortDockWindowsLessFunctor(TSortDockWindowsParams::TSortOrder _sortorder, bool _inverse = false):
		params(_sortorder, _inverse)
	{}

	bool operator()(ConstPtr(CDockWindow) pWin1, ConstPtr(CDockWindow) pWin2) const
	{
		RECT r1;
		RECT r2;

		CastMutablePtr(CDockWindow, pWin1)->GetWindowRect(&r1);
		CastMutablePtr(CDockWindow, pWin2)->GetWindowRect(&r2);
		switch (params.sortorder)
		{
		case TSortDockWindowsParams::TSortOrderLeft:
			if (r1.left < r2.left)
				return params.inverse;
			break;
		case TSortDockWindowsParams::TSortOrderTop:
			if (r1.top < r2.top)
				return params.inverse;
			break;
		case TSortDockWindowsParams::TSortOrderRight:
			if (r1.right < r2.right)
				return params.inverse;
			break;
		case TSortDockWindowsParams::TSortOrderBottom:
			if (r1.bottom < r2.bottom)
				return params.inverse;
			break;
		}
		return false;
	}

	TSortDockWindowsParams params;
};

typedef struct _tagSASFindDockWindowParams
{
	POINT pt;
	CDockWindow* pResult;
	bool bAnyNonFloatingWindows;

	_tagSASFindDockWindowParams(POINT _pt):
	    pt(_pt), pResult(nullptr), bAnyNonFloatingWindows(false) {}
} TSASFindDockWindowParams;

static sword __stdcall TSearchAndSortUserFunc_FindDockWindow( ConstPointer ArrayItem, ConstPointer DataItem, ConstPointer context )
{
	CDockWindow* pWin = CastAnyPtr(CDockWindow, CastMutable(Pointer, ArrayItem));
	TSASFindDockWindowParams* pParams = CastAnyPtr(TSASFindDockWindowParams, CastMutable(Pointer, context));
	RECT r;

	pParams->bAnyNonFloatingWindows = true;
	pWin->GetWindowRect(&r);
	if ( ::PtInRect(&r, pParams->pt) )
	{
		pParams->pResult = pWin;
		return 0;
	}
	return 1;
}

typedef struct _tagSASFindDockWindowByNameParams
{
	CStringLiteral name;
	CDockWindow* pResult;

	_tagSASFindDockWindowByNameParams(ConstRef(CStringLiteral) _name):
	    name(_name), pResult(nullptr) {}
} TSASFindDockWindowByNameParams;

static sword __stdcall TSearchAndSortUserFunc_FindDockWindowByName( ConstPointer ArrayItem, ConstPointer DataItem, ConstPointer context )
{
	CDockWindow* pWin = CastAnyPtr(CDockWindow, CastMutable(Pointer, ArrayItem));
	TSASFindDockWindowByNameParams* pParams = CastAnyPtr(TSASFindDockWindowByNameParams, CastMutable(Pointer, context));

	if ( pParams->name.Compare(pWin->get_name(), 0, CStringLiteral::cIgnoreCase) == 0 )
	{
		pParams->pResult = pWin;
		return 0;
	}
	return 1;
}

typedef struct _tagSASSortDockWindowsParams
{
	enum TSortOrder {
		TSortOrderLeft,
		TSortOrderTop,
		TSortOrderRight,
		TSortOrderBottom
	};

	TSortOrder sortorder;
	bool inverse;

	_tagSASSortDockWindowsParams(TSortOrder _sortorder, bool _inverse = false):
	    sortorder(_sortorder), inverse(_inverse) {}

} TSASSortDockWindowsParams;

static sword __stdcall TSearchAndSortUserFunc_SortDockWindows( ConstPointer ArrayItem, ConstPointer DataItem, ConstPointer context )
{
	CDockWindow* pWin1 = CastAnyPtr(CDockWindow, CastMutable(Pointer, ArrayItem));
	CDockWindow* pWin2 = CastAnyPtr(CDockWindow, CastMutable(Pointer, DataItem));
	TSASSortDockWindowsParams* params = CastAnyPtr(TSASSortDockWindowsParams, CastMutable(Pointer, context));
	RECT r1;
	RECT r2;
	sword lessResult = (params->inverse)?1:-1;
	sword greaterResult = (params->inverse)?-1:1;

	pWin1->GetWindowRect(&r1);
	pWin2->GetWindowRect(&r2);
	switch ( params->sortorder )
	{
	case TSASSortDockWindowsParams::TSortOrderLeft:
		if ( r1.left < r2.left )
			return lessResult;
		if ( r1.left == r2.left )
			return 0;
		break;
	case TSASSortDockWindowsParams::TSortOrderTop:
		if ( r1.top < r2.top )
			return lessResult;
		if ( r1.top == r2.top )
			return 0;
		break;
	case TSASSortDockWindowsParams::TSortOrderRight:
		if ( r1.right < r2.right )
			return lessResult;
		if ( r1.right == r2.right )
			return 0;
		break;
	case TSASSortDockWindowsParams::TSortOrderBottom:
		if ( r1.bottom < r2.bottom )
			return lessResult;
		if ( r1.bottom == r2.bottom )
			return 0;
		break;
	}
	return greaterResult;
}

CDockInfo::CDockInfo(CWin* pFrame):
    m_dockwindows(__FILE__LINE__ 16,16),
	m_floatingdockwindows(__FILE__LINE__ 16,16),
	m_pdockwin(nullptr),
	m_pundockwin(nullptr),
	m_pCurrentFloatingWindow(nullptr),
	m_pFrame(pFrame),
	m_pTargetCenter(nullptr),
	m_pHint(nullptr),
	m_shutdown(FALSE)
{
	for ( int ix = TDockTargetLeft; ix <= TDockTargetBottom; ++ix )
		m_pTarget[ix] = nullptr;
	::SetRectEmpty(&m_windowRect);
	::SetRectEmpty(&m_clientRect);
}

CDockInfo::~CDockInfo()
{
	m_shutdown = TRUE;
	m_floatingdockwindows.ForEach<FloatingWindowForEachFunctor>();
	if ( m_pTargetCenter )
	{
		m_pTargetCenter->DestroyWindow();
		for ( int ix = TDockTargetLeft; ix <= TDockTargetBottom; ++ix )
			m_pTarget[ix]->DestroyWindow();
		m_pHint->DestroyWindow();
	}
}

void CDockInfo::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	CDockWindowVector::Iterator it = m_dockwindows.Begin();

	while ( it )
	{
		(*it)->get_BrushKeys(_keys);
		++it;
	}
	it = m_floatingdockwindows.Begin();
	while ( it )
	{
		(*it)->get_BrushKeys(_keys);
		++it;
	}
}

void CDockInfo::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	CDockWindowVector::Iterator it = m_dockwindows.Begin();

	while ( it )
	{
		(*it)->get_FontKeys(_keys);
		++it;
	}
	it = m_floatingdockwindows.Begin();
	while ( it )
	{
		(*it)->get_FontKeys(_keys);
		++it;
	}
}

void CDockInfo::Refresh()
{
	CDockWindowVector::Iterator it = m_dockwindows.Begin();

	while ( it )
	{
		(*it)->Refresh();
		++it;
	}
	it = m_floatingdockwindows.Begin();
	while ( it )
	{
		(*it)->Refresh();
		++it;
	}
}

UINT CDockInfo::get_cmdcode()
{
	UINT cmdcode = 1100;

	if ( !m_pFrame )
		return cmdcode;
	switch ( m_pFrame->get_windowtype() )
	{
	case CWin::TFrameWindow:
		{
			CFrameWnd* pFrame = CastDynamicPtr(CFrameWnd, m_pFrame);

			if ( pFrame )
				cmdcode = pFrame->get_cmdcode();
		}
		break;
	case CWin::TMDIWindow:
		{
			CMDIFrame* pFrame = CastDynamicPtr(CMDIFrame, m_pFrame);

			if ( pFrame )
				cmdcode = pFrame->get_cmdcode();
		}
		break;
	}
	return cmdcode;
}

void CDockInfo::SaveStatus()
{
	CStringBuffer nfile = theGuiApp->config()->GetValue(_T("Application.Name"), _T("XGuiTest"));
	CStringBuffer nkey;

	nfile.AppendString(_T("User."));
	nkey = nfile;
	nkey.AppendString(_T("Docking.Count"));
	theGuiApp->config()->SetLongValue(nkey, m_dockwindows.Count());
	nkey = nfile;
	nkey.AppendString(_T("Docking.WindowRect"));
	theGuiApp->config()->SetLongValues(nkey, 4, m_windowRect.left, m_windowRect.right, m_windowRect.top, m_windowRect.bottom);
	nkey = nfile;
	nkey.AppendString(_T("Docking.ClientRect"));
	theGuiApp->config()->SetLongValues(nkey, 4, m_clientRect.left, m_clientRect.right, m_clientRect.top, m_clientRect.bottom);

	CDockWindowVector::Iterator it = m_dockwindows.Begin();
	WLong cnt = 0;

	while ( it )
	{
		CDockWindow* pDW = *it;
		RECT r;

		pDW->GetParentRect(&r);
		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.Name"), nfile.GetString(), cnt);
		theGuiApp->config()->SetValue(nkey, pDW->get_name());
		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.ParentRect"), nfile.GetString(), cnt);
		theGuiApp->config()->SetLongValues(nkey, 4, r.left, r.right, r.top, r.bottom);
		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.DockingStyle"), nfile.GetString(), cnt);
		theGuiApp->config()->SetValue(nkey, CDockWindow::DockingStyle2String(pDW->get_dockingstyle()));

		CDockWindow::TNeighbourVector::Iterator it1 = pDW->get_Neighbours();
		CAbstractConfiguration::Values values(__FILE__LINE__ 16, 16);

		while ( it1 )
		{
			CDockWindow::TNeighbour* pNB = *it1;
			CStringBuffer value;

			value = CDockWindow::DockingStyle2String(pNB->style);
			value += _T(":");
			value += pNB->win->get_name();
			values.Append(value);
			++it1;
		}
		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.Neighbours"), nfile.GetString(), cnt);
		theGuiApp->config()->SetValues(nkey, values);

		CTabControl* pTabControl = CastDynamicPtr(CTabControl, pDW->get_Client());

		if ( pTabControl )
		{
			CTabPage* pTabPage = CastDynamicPtr(CTabPage, pTabControl->get_firstchild());
			CAbstractConfiguration::Values values1(__FILE__LINE__ 16, 16);

			while ( pTabPage )
			{
				values1.Append(pTabPage->get_Text());
				pTabPage = CastDynamicPtr(CTabPage, pTabPage->get_siblingByCmd(GW_HWNDNEXT));
			}
			nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.TabPages"), nfile.GetString(), cnt);
			theGuiApp->config()->SetValues(nkey, values1);
		}
		++it;
		++cnt;
	}

	nkey = nfile;
	nkey.AppendString(_T("Floating.Count"));
	theGuiApp->config()->SetLongValue(nkey, m_floatingdockwindows.Count());

	it = m_floatingdockwindows.Begin();
	cnt = 0;
	while ( it )
	{
		CDockWindow* pDW = *it;
		RECT r;

		pDW->GetWindowRect(&r);
		nkey.FormatString(__FILE__LINE__ _T("%sFloating.Window%d.Name"), nfile.GetString(), cnt);
		theGuiApp->config()->SetValue(nkey, pDW->get_name());
		nkey.FormatString(__FILE__LINE__ _T("%sFloating.Window%d.WindowRect"), nfile.GetString(), cnt);
		theGuiApp->config()->SetLongValues(nkey, 4, r.left, r.right, r.top, r.bottom);

		CTabControl* pTabControl = CastDynamicPtr(CTabControl, pDW->get_Client());

		if ( pTabControl )
		{
			CTabPage* pTabPage = CastDynamicPtr(CTabPage, pTabControl->get_firstchild());
			CAbstractConfiguration::Values values1(__FILE__LINE__ 16, 16);

			while ( pTabPage )
			{
				values1.Append(pTabPage->get_Text());
				pTabPage = CastDynamicPtr(CTabPage, pTabPage->get_siblingByCmd(GW_HWNDNEXT));
			}
			nkey.FormatString(__FILE__LINE__ _T("%sFloating.Window%d.TabPages"), nfile.GetString(), cnt);
			theGuiApp->config()->SetValues(nkey, values1);
		}
		++it;
		++cnt;
	}
}

void CDockInfo::LoadStatus()
{
	CStringBuffer nfile = theGuiApp->config()->GetValue(_T("Application.Name"), _T("XGuiTest"));
	CStringBuffer nkey;
	WLong cnt;

	nfile.AppendString(_T("User."));
	nkey = nfile;
	nkey.AppendString(_T("Docking.Count"));
	cnt = theGuiApp->config()->GetLongValue(nkey, 0);
	nkey = nfile;
	nkey.AppendString(_T("Docking.WindowRect"));
	theGuiApp->config()->GetLongValues(nkey, 4, &(m_windowRect.left), &(m_windowRect.right), &(m_windowRect.top), &(m_windowRect.bottom));
	nkey = nfile;
	nkey.AppendString(_T("Docking.ClientRect"));
	theGuiApp->config()->GetLongValues(nkey, 4, &(m_clientRect.left), &(m_clientRect.right), &(m_clientRect.top), &(m_clientRect.bottom));

	for ( WLong ix = 0; ix < cnt; ++ix )
	{
		CDockWindow* pDW;
		CStringBuffer name;
		RECT r;

		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.Name"), nfile.GetString(), ix);
		name = theGuiApp->config()->GetValue(nkey);
		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.ParentRect"), nfile.GetString(), ix);
		theGuiApp->config()->GetLongValues(nkey, 4, &(r.left), &(r.right), &(r.top), &(r.bottom));

		pDW = CreateDocked(name, &r);

		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.DockingStyle"), nfile.GetString(), ix);
		pDW->set_dockingstyle(CDockWindow::String2DockingStyle(theGuiApp->config()->GetValue(nkey)));

		CTabControl* pTabControl = CastDynamicPtr(CTabControl, pDW->get_Client());

		if ( pTabControl )
		{
			CAbstractConfiguration::Values values(__FILE__LINE__ 16, 16);
			CAbstractConfiguration::Values::Iterator it;
			CStringBuffer name1 = pTabControl->get_name();
			WLong jx = 0;

			nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.TabPages"), nfile.GetString(), ix);
			theGuiApp->config()->GetValues(nkey, values);

			it = values.Begin();
			while ( it )
			{
				CStringBuffer tmp = *it;
				CStringBuffer name2;

				name2.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), name1.GetString(), 1000 + jx);

				CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name2);

				pPage->set_Text(tmp);
				pPage->set_CreateVisible(FALSE);
				pPage->set_HScrollEnabled(TRUE);
				pPage->set_VScrollEnabled(TRUE);
				pPage->set_border(TRUE);
				pPage->Create(pTabControl->get_handle(), 1000 + jx);
				++it;
				++jx;
			}
		}
	}

	for ( WLong ix = 0; ix < cnt; ++ix )
	{
		CStringBuffer name;

		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.Name"), nfile.GetString(), ix);
		name = theGuiApp->config()->GetValue(nkey);

		FindDockWindowByNameForEachFunctor arg(name);

		m_dockwindows.ForEach<FindDockWindowByNameForEachFunctor>(arg);
		if ( !(arg._params.pResult) )
			continue;

		CAbstractConfiguration::Values values(__FILE__LINE__ 16, 16);
		CAbstractConfiguration::Values::Iterator it;

		nkey.FormatString(__FILE__LINE__ _T("%sDocking.Window%d.Neighbours"), nfile.GetString(), ix);
		theGuiApp->config()->GetValues(nkey, values);

		it = values.Begin();
		while ( it )
		{
			CStringBuffer tmp = *it;
			CPointer tmpa[2];
			dword tmpc;

			tmp.Split(_T(":"), tmpa, 2, &tmpc);

			CDockWindow::TDockingStyle style = CDockWindow::String2DockingStyle(tmpa[0]);
			FindDockWindowByNameForEachFunctor arg1(tmpa[1]);

			m_dockwindows.ForEach<FindDockWindowByNameForEachFunctor>(arg);
			arg._params.pResult->AppendNeighbour(style, arg1._params.pResult);

			++it;
		}
	}

	nkey = nfile;
	nkey.AppendString(_T("Floating.Count"));
	cnt = theGuiApp->config()->GetLongValue(nkey, 0);
	for ( WLong ix = 0; ix < cnt; ++ix )
	{
		CDockWindow* pDW;
		CStringBuffer name;
		RECT r;

		nkey.FormatString(__FILE__LINE__ _T("%sFloating.Window%d.Name"), nfile.GetString(), ix);
		name = theGuiApp->config()->GetValue(nkey);
		nkey.FormatString(__FILE__LINE__ _T("%sFloating.Window%d.WindowRect"), nfile.GetString(), ix);
		theGuiApp->config()->GetLongValues(nkey, 4, &(r.left), &(r.right), &(r.top), &(r.bottom));

		pDW = CreateFloating(name, &r);

		CTabControl* pTabControl = CastDynamicPtr(CTabControl, pDW->get_Client());

		if ( pTabControl )
		{
			CAbstractConfiguration::Values values(__FILE__LINE__ 16, 16);
			CAbstractConfiguration::Values::Iterator it;
			CStringBuffer name1 = pTabControl->get_name();
			WLong jx = 0;

			nkey.FormatString(__FILE__LINE__ _T("%sFloating.Window%d.TabPages"), nfile.GetString(), ix);
			theGuiApp->config()->GetValues(nkey, values);

			it = values.Begin();
			while ( it )
			{
				CStringBuffer tmp = *it;
				CStringBuffer name2;

				name2.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), name1.GetString(), 1000 + jx);

				CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name2);

				pPage->set_Text(name2);
				pPage->set_CreateVisible(FALSE);
				pPage->set_HScrollEnabled(TRUE);
				pPage->set_VScrollEnabled(TRUE);
				pPage->set_border(TRUE);
				pPage->Create(pTabControl->get_handle(), 1000 + jx);
				++it;
				++jx;
			}
		}
		pDW->SendMessage(WM_SIZE, 0, 0);
	}

	CDockWindowVector::Iterator it = m_dockwindows.Begin();

	while ( it )
	{
		CDockWindow* pDW = *it;
		CStringBuffer name;

		name.FormatString(__FILE__LINE__ _T("%s.DockWindow%d"), m_pFrame->get_name().GetString(), pDW->get_SavedChildID());
		pDW->set_name(name);

		CTabControl* pTabControl = CastDynamicPtr(CTabControl, pDW->get_Client());

		if ( pTabControl )
		{
			name.AppendString(_T(".TabControl"));
			pTabControl->set_name(name);

			CTabPage* pPage = CastDynamicPtr(CTabPage, pTabControl->get_firstchild());

			while ( pPage )
			{
				CStringBuffer name2;

				name2.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), name.GetString(), pPage->get_childID());
				pPage->set_name(name2);
				pPage->set_Text(name2);
				pPage = CastDynamicPtr(CTabPage, pPage->get_siblingByCmd(GW_HWNDNEXT));
			}
		}
		++it;
	}

	it = m_floatingdockwindows.Begin();
	while ( it )
	{
		CDockWindow* pDW = *it;
		CStringBuffer name;

		name.FormatString(__FILE__LINE__ _T("%s.DockWindow%d"), m_pFrame->get_name().GetString(), pDW->get_SavedChildID());
		pDW->set_name(name);

		CTabControl* pTabControl = CastDynamicPtr(CTabControl, pDW->get_Client());

		if ( pTabControl )
		{
			name.AppendString(_T(".TabControl"));
			pTabControl->set_name(name);

			CTabPage* pPage = CastDynamicPtr(CTabPage, pTabControl->get_firstchild());

			while ( pPage )
			{
				CStringBuffer name2;

				name2.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), name.GetString(), pPage->get_childID());
				pPage->set_name(name2);
				pPage->set_Text(name2);
				pPage = CastDynamicPtr(CTabPage, pPage->get_siblingByCmd(GW_HWNDNEXT));
			}
		}
		++it;
	}

}

CDockWindow* CDockInfo::CreateFloating(LPCTSTR name, LPRECT rect)
{
	if ( !m_pFrame )
		return nullptr;

	UINT cmdcode = get_cmdcode();
	CStringBuffer name0(__FILE__LINE__ name);

	if ( name0.IsEmpty() )
		name0.FormatString(__FILE__LINE__ _T("%s.DockWindow%d"), m_pFrame->get_name().GetString(), cmdcode);

	CDockWindow* pdockwindow = OK_NEW_OPERATOR CDockWindow(name0);
	RECT r1;

	pdockwindow->set_Floating(TRUE);
	pdockwindow->Create(m_pFrame->get_handle(), cmdcode);
	if ( !rect )
	{
		m_pFrame->GetWindowRect(&r1);
		r1.left += (r1.right - r1.left) / 2;
		r1.top += (r1.bottom - r1.top) / 2;
		pdockwindow->SetWindowPos(HWND_TOPMOST, &r1, SWP_NOSIZE | SWP_SHOWWINDOW);
	}
	else
		pdockwindow->SetWindowPos(HWND_TOPMOST, rect, SWP_SHOWWINDOW);

	pdockwindow->set_DockInfo(this);
	m_floatingdockwindows.Append(pdockwindow);

	name0.AppendString(_T(".TabControl"));

	CTabControl* pTabControl = OK_NEW_OPERATOR CTabControl(name0);

	pTabControl->set_TitleOrientation(CTabControl::TTitleOrientationBottom);
	pTabControl->Create(pdockwindow->get_handle(), 1000);
	pdockwindow->set_Client(pTabControl);

	return pdockwindow;
}

CDockWindow* CDockInfo::CreateDocked(LPCTSTR name, LPRECT rect)
{
	if ( (!m_pFrame) || (!rect) )
		return nullptr;

	UINT cmdcode = get_cmdcode();
	CStringBuffer name0(__FILE__LINE__ name);

	if ( name0.IsEmpty() )
		name0.FormatString(__FILE__LINE__ _T("%s.DockWindow%d"), m_pFrame->get_name().GetString(), cmdcode);

	CDockWindow* pdockwindow = OK_NEW_OPERATOR CDockWindow(name0);

	pdockwindow->set_Floating(FALSE);
	pdockwindow->Create(m_pFrame->get_handle(), cmdcode);
	pdockwindow->SetWindowPos(rect, SWP_SHOWWINDOW);

	pdockwindow->set_DockInfo(this);
	m_dockwindows.Append(pdockwindow);

	name0.AppendString(_T(".TabControl"));

	CTabControl* pTabControl = OK_NEW_OPERATOR CTabControl(name0);

	pTabControl->set_TitleOrientation(CTabControl::TTitleOrientationBottom);
	pTabControl->Create(pdockwindow->get_handle(), 1000);
	pdockwindow->set_Client(pTabControl);

	return pdockwindow;
}

void CDockInfo::Destroy(CDockWindow* pWindow)
{
	if ( m_shutdown )
		return;
	if ( pWindow->is_Floating() )
	{
		CDockWindowVector::Iterator it = m_floatingdockwindows.Find<CCppObjectEqualFunctor<CDockWindow> >(pWindow);

		if ( it )
			m_floatingdockwindows.Remove(it);
	}
}

void CDockInfo::AutoSize()
{
}

void CDockInfo::set_Visible(BOOL visible)
{
}

void CDockInfo::set_windowrect(LPRECT r)
{
	if ( ::IsRectEmpty(&m_windowRect) )
	{
		::CopyRect(&m_clientRect, r);
		::CopyRect(&m_windowRect, r);
	}
	if ( !(::EqualRect(&m_windowRect, r)) )
	{
		if ( m_dockwindows.Count() == 0 )
		{
			::CopyRect(&m_clientRect, r);
			::CopyRect(&m_windowRect, r);
		}
		else if ( ((r->right - r->left) > 200) && ((r->bottom - r->top) > 200) )
		{
			LONG offsetX = r->left - m_windowRect.left;
			LONG offsetY = r->top - m_windowRect.top;
			WDouble deltaPixelX = ((WDouble)((r->right - r->left) - (m_windowRect.right - m_windowRect.left))) / ((WDouble)(m_windowRect.right - m_windowRect.left));
			WDouble deltaPixelY = ((WDouble)((r->bottom - r->top) - (m_windowRect.bottom - m_windowRect.top))) / ((WDouble)(m_windowRect.bottom - m_windowRect.top));
			CDockWindowVector::Iterator it = m_dockwindows.Begin();
			RECT r1;

			while ( it )
			{
				CDockWindow* pWin = *it;

				pWin->GetParentRect(&r1);
				r1.left += (LONG)floor(((r1.left - m_windowRect.left) * deltaPixelX) + 0.5) + offsetX;
				if ( r1.right == m_windowRect.right )
					r1.right = r->right;
				else
					r1.right += (LONG)floor(((r1.right - m_windowRect.left) * deltaPixelX) + 0.5) + offsetX;
				r1.top += (LONG)floor(((r1.top - m_windowRect.top) * deltaPixelY) + 0.5) + offsetY;
				if ( r1.bottom == m_windowRect.bottom )
					r1.bottom = r->bottom;
				else
					r1.bottom += (LONG)floor(((r1.bottom - m_windowRect.top) * deltaPixelY) + 0.5) + offsetY;
				pWin->MoveWindow(&r1, TRUE);
				++it;
			}
			m_clientRect.left += (LONG)floor(((m_clientRect.left - m_windowRect.left) * deltaPixelX) + 0.5) + offsetX;
			if ( m_clientRect.right == m_windowRect.right )
				m_clientRect.right = r->right;
			else
				m_clientRect.right += (LONG)floor(((m_clientRect.right - m_windowRect.left) * deltaPixelX) + 0.5) + offsetX;
			m_clientRect.top += (LONG)floor(((m_clientRect.top - m_windowRect.top) * deltaPixelY) + 0.5) + offsetY;
			if ( m_clientRect.bottom == m_windowRect.bottom )
				m_clientRect.bottom = r->bottom;
			else
				m_clientRect.bottom += (LONG)floor(((m_clientRect.bottom - m_windowRect.top) * deltaPixelY) + 0.5) + offsetY;
			::CopyRect(&m_windowRect, r);
		}
	}
	if ( m_pundockwin )
	{
		if ( m_dockwindows.Count() == 0 )
			::CopyRect(&m_clientRect, r);
		else
		{
			RECT r1;
			RECT rClient;

			::CopyRect(&rClient, &m_clientRect);
			m_pFrame->ClientToScreen(&rClient);
			m_pundockwin->GetWindowRect(&r1);
			if ( !(m_pundockwin->has_Neighbours()) )
			{
				if ( r1.left == rClient.right )
					m_clientRect.right += r1.right - r1.left;
				else if ( r1.top == rClient.bottom )
					m_clientRect.bottom += r1.bottom - r1.top;
				else if ( r1.right == rClient.left )
					m_clientRect.left -= r1.right - r1.left;
				else
					m_clientRect.top -= r1.bottom - r1.top;
			}
			else
			{
				if ( (r1.left == rClient.right) && (r1.top == rClient.top) && (r1.bottom == rClient.bottom) )
					m_clientRect.right += r1.right - r1.left;
				else if ( (r1.top == rClient.bottom) && (r1.left == rClient.left) && (r1.right == rClient.right) )
					m_clientRect.bottom += r1.bottom - r1.top;
				else if ( r1.right == rClient.left && (r1.top == rClient.top) && (r1.bottom == rClient.bottom) )
					m_clientRect.left -= r1.right - r1.left;
				else if ( (r1.bottom == rClient.top) && (r1.left == rClient.left) && (r1.right == rClient.right) )
					m_clientRect.top -= r1.bottom - r1.top;
				else
				{
					CDockWindow::TNeighbourVector::Iterator it = m_pundockwin->get_Neighbours();
					RECT r2;
					bool bFound = false;

					while ( it )
					{
						CDockWindow::TNeighbour* pN = *it;

						pN->win->GetWindowRect(&r2);
						switch ( pN->style )
						{
						case CDockWindow::TDockingStyleLeft:
							if ( (r1.top == r2.top) && (r1.bottom == r2.bottom) )
							{
								r2.right += r1.right - r1.left;
								m_pFrame->ScreenToClient(&r2);
								pN->win->MoveWindow(&r2, TRUE);
								pN->win->CopyNeighbours(m_pundockwin);
								bFound = true;
							}
							break;
						case CDockWindow::TDockingStyleTop:
							if ( (r1.left == r2.left) && (r1.right == r2.right) )
							{
								r2.bottom += r1.bottom - r1.top;
								m_pFrame->ScreenToClient(&r2);
								pN->win->MoveWindow(&r2, TRUE);
								pN->win->CopyNeighbours(m_pundockwin);
								bFound = true;
							}
							break;
						case CDockWindow::TDockingStyleRight:
							if ( (r1.top == r2.top) && (r1.bottom == r2.bottom) )
							{
								r2.left -= r1.right - r1.left;
								m_pFrame->ScreenToClient(&r2);
								pN->win->MoveWindow(&r2, TRUE);
								pN->win->CopyNeighbours(m_pundockwin);
								bFound = true;
							}
							break;
						case CDockWindow::TDockingStyleBottom:
							if ( (r1.left == r2.left) && (r1.right == r2.right) )
							{
								r2.top -= r1.bottom - r1.top;
								m_pFrame->ScreenToClient(&r2);
								pN->win->MoveWindow(&r2, TRUE);
								pN->win->CopyNeighbours(m_pundockwin);
								bFound = true;
							}
							break;
						}
						if ( bFound )
							break;
						++it;
					}
					if ( !bFound )
					{
						LONG sz = 0;

						it = m_pundockwin->get_Neighbours();
						while ( it )
						{
							CDockWindow::TNeighbour* pN = *it;

							pN->win->GetWindowRect(&r2);
							switch ( pN->style )
							{
							case CDockWindow::TDockingStyleLeft:
								if ( r1.left == rClient.right )
								{
									if ( sz == 0 )
									{
										sz = rClient.bottom - rClient.top;
										m_clientRect.right += r1.right - r1.left;
									}
									sz += r2.bottom - r2.top;
									r2.right += r1.right - r1.left;
									m_pFrame->ScreenToClient(&r2);
									pN->win->MoveWindow(&r2, TRUE);
									pN->win->CopyNeighbours(m_pundockwin);
									if ( sz == (r1.bottom - r1.top) )
										bFound = true;
								}
								break;
							case CDockWindow::TDockingStyleTop:
								if ( r1.top == rClient.bottom )
								{
									if ( sz == 0 )
									{
										sz = rClient.right - rClient.left;
										m_clientRect.bottom += r1.bottom - r1.top;
									}
									sz += r2.right - r2.left;
									r2.bottom += r1.bottom - r1.top;
									m_pFrame->ScreenToClient(&r2);
									pN->win->MoveWindow(&r2, TRUE);
									pN->win->CopyNeighbours(m_pundockwin);
									if ( sz == (r1.right - r1.left) )
										bFound = true;
								}
								break;
							case CDockWindow::TDockingStyleRight:
								if ( r1.right == rClient.left )
								{
									if ( sz == 0 )
									{
										sz = rClient.bottom - rClient.top;
										m_clientRect.left -= r1.right - r1.left;
									}
									sz += r2.bottom - r2.top;
									r2.left -= r1.right - r1.left;
									m_pFrame->ScreenToClient(&r2);
									pN->win->MoveWindow(&r2, TRUE);
									pN->win->CopyNeighbours(m_pundockwin);
									if ( sz == (r1.bottom - r1.top) )
										bFound = true;
								}
								break;
							case CDockWindow::TDockingStyleBottom:
								if ( r1.bottom == rClient.top )
								{
									if ( sz == 0 )
									{
										sz = rClient.right - rClient.left;
										m_clientRect.top -= r1.bottom - r1.top;
									}
									sz += r2.right - r2.left;
									r2.top -= r1.bottom - r1.top;
									m_pFrame->ScreenToClient(&r2);
									pN->win->MoveWindow(&r2, TRUE);
									pN->win->CopyNeighbours(m_pundockwin);
									if ( sz == (r1.right - r1.left) )
										bFound = true;
								}
								break;
							}
							if ( bFound )
								break;
							++it;
						}
					}
					if ( !bFound )
					{
						RECT sz;

						::SetRectEmpty(&sz);
						it = m_pundockwin->get_Neighbours();
						while ( it )
						{
							CDockWindow::TNeighbour* pN = *it;

							pN->win->GetWindowRect(&r2);
							switch ( pN->style )
							{
							case CDockWindow::TDockingStyleLeft:
								sz.left += r2.bottom - r2.top;
								break;
							case CDockWindow::TDockingStyleTop:
								sz.top += r2.right - r2.left;
								break;
							case CDockWindow::TDockingStyleRight:
								sz.right += r2.bottom - r2.top;
								break;
							case CDockWindow::TDockingStyleBottom:
								sz.bottom += r2.right - r2.left;
								break;
							}
							++it;
						}
						if ( sz.left == (r1.bottom - r1.top) )
						{
							it = m_pundockwin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);

							while ( it ) 
							{
								CDockWindow::TNeighbour* pN = *it;

								pN->win->GetWindowRect(&r2);
								r2.right += r1.right - r1.left;
								m_pFrame->ScreenToClient(&r2);
								pN->win->MoveWindow(&r2, TRUE);
								pN->win->CopyNeighbours(m_pundockwin);
								it = m_pundockwin->get_NextNeighbour(CDockWindow::TDockingStyleLeft, it);
							}
						}
						else if ( sz.top == (r1.right - r1.left) )
						{
							it = m_pundockwin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);

							while ( it ) 
							{
								CDockWindow::TNeighbour* pN = *it;

								pN->win->GetWindowRect(&r2);
								r2.bottom += r1.bottom - r1.top;
								m_pFrame->ScreenToClient(&r2);
								pN->win->MoveWindow(&r2, TRUE);
								pN->win->CopyNeighbours(m_pundockwin);
								it = m_pundockwin->get_NextNeighbour(CDockWindow::TDockingStyleTop, it);
							}
						}
						else if ( sz.right == (r1.bottom - r1.top) )
						{
							it = m_pundockwin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);

							while ( it ) 
							{
								CDockWindow::TNeighbour* pN = *it;

								pN->win->GetWindowRect(&r2);
								r2.left -= r1.right - r1.left;
								m_pFrame->ScreenToClient(&r2);
								pN->win->MoveWindow(&r2, TRUE);
								pN->win->CopyNeighbours(m_pundockwin);
								it = m_pundockwin->get_NextNeighbour(CDockWindow::TDockingStyleRight, it);
							}
						}
						else if ( sz.bottom == (r1.right - r1.left) )
						{
							it = m_pundockwin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);

							while ( it ) 
							{
								CDockWindow::TNeighbour* pN = *it;

								pN->win->GetWindowRect(&r2);
								r2.top -= r1.bottom - r1.top;
								m_pFrame->ScreenToClient(&r2);
								pN->win->MoveWindow(&r2, TRUE);
								pN->win->CopyNeighbours(m_pundockwin);
								it = m_pundockwin->get_NextNeighbour(CDockWindow::TDockingStyleBottom, it);
							}
						}
					}
				}
			}
		}
		m_pundockwin->ClearNeighbours();
		m_floatingdockwindows.Append(m_pundockwin);
		m_pundockwin = nullptr;
	}
	if ( m_pdockwin )
	{
		m_dockwindows.Append(m_pdockwin);
		m_pdockwin = nullptr;
	}
}

void CDockInfo::get_clientrect(LPRECT r)
{
	::CopyRect(r, &m_clientRect);
}

void CDockInfo::UpdateFrame(CDockWindow* pWin)
{
	if ( pWin->is_Floating() )
	{
		CDockWindowVector::Iterator it = m_dockwindows.Find<CCppObjectEqualFunctor<CDockWindow>>(pWin);

		if ( it )
			m_dockwindows.Remove(it);
		m_pundockwin = pWin;
		//pWin->ClearNeighbours();
		//m_floatingdockwindows.Append(pWin);
	}
	else
	{
		CDockWindowVector::Iterator it = m_floatingdockwindows.Find<CCppObjectEqualFunctor<CDockWindow>>(pWin);

		if ( it )
			m_floatingdockwindows.Remove(it);
		m_pdockwin = pWin;
		//m_dockwindows.Append(pWin);
	}
	m_pFrame->SendMessage(WM_SIZE, 0, 0);
}

void CDockInfo::ShowDockTargets(CDockWindow* pWin)
{
	m_pCurrentFloatingWindow = pWin;
	if ( !m_pTargetCenter )
	{
		m_pTargetCenter = OK_NEW_OPERATOR CDockTargetCenter;

		m_pTargetCenter->set_CreateVisible(FALSE);
		m_pTargetCenter->Create();

		m_pTarget[TDockTargetLeft] = OK_NEW_OPERATOR CDockTargetLeft;
		m_pTarget[TDockTargetTop] = OK_NEW_OPERATOR CDockTargetTop;
		m_pTarget[TDockTargetRight] = OK_NEW_OPERATOR CDockTargetRight;
		m_pTarget[TDockTargetBottom] = OK_NEW_OPERATOR CDockTargetBottom;

		for ( int ix = TDockTargetLeft; ix <= TDockTargetBottom; ++ix )
		{
			m_pTarget[ix]->set_CreateVisible(FALSE);
			m_pTarget[ix]->Create();
		}

		m_pHint = OK_NEW_OPERATOR CDockHint;

		m_pHint->set_CreateVisible(FALSE);
		m_pHint->Create();
	}
}

void CDockInfo::FindClientNeighboursLeft2RightTop(RECT r1)
{
	POINT pt = { r1.left + 10, r1.top - 10 };
	FindDockWindowForEachFunctor arg(pt);
	RECT r3;
	RECT r4;

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	while ( arg._params.pResult )
	{
		m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleTop, arg._params.pResult);
		arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleBottom, m_pCurrentFloatingWindow);
		arg._params.pResult->GetWindowRect(&r3);
		if ( r3.right < r1.right )
		{
			CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
			bool bFound = false;

			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				if ( pN->style == CDockWindow::TDockingStyleRight )
				{
					pN->win->GetWindowRect(&r4);
					if ( (r4.bottom == r1.top) && (r4.left <= r1.right) )
					{
						arg._params.pResult = pN->win;
						bFound = true;
						break;
					}
				}
				++it;
			}
			if ( !bFound )
				break;
		}
		else
			break;
	}
}

void CDockInfo::FindClientNeighboursLeft2RightBottom(RECT r1)
{
	POINT pt = { r1.left + 10, r1.bottom + 10 };
	FindDockWindowForEachFunctor arg(pt);
	RECT r3;
	RECT r4;

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	while (arg._params.pResult)
	{
		m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleBottom, arg._params.pResult);
		arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleTop, m_pCurrentFloatingWindow);
		arg._params.pResult->GetWindowRect(&r3);
		if ( r3.right < r1.right )
		{
			CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
			bool bFound = false;

			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				if ( pN->style == CDockWindow::TDockingStyleRight )
				{
					pN->win->GetWindowRect(&r4);
					if ( (r4.top == r1.bottom) && (r4.left <= r1.right) )
					{
						arg._params.pResult = pN->win;
						bFound = true;
						break;
					}
				}
				++it;
			}
			if ( !bFound )
				break;
		}
		else
			break;
	}
}

void CDockInfo::FindClientNeighboursTop2DownLeft(RECT r1)
{
	POINT pt = { r1.left - 10, r1.top + 10 };
	FindDockWindowForEachFunctor arg(pt);
	RECT r3;
	RECT r4;

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	while (arg._params.pResult)
	{
		m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleLeft, arg._params.pResult);
		arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleRight, m_pCurrentFloatingWindow);
		arg._params.pResult->GetWindowRect(&r3);
		if ( r3.bottom < r1.bottom )
		{
			CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
			bool bFound = false;

			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				if ( pN->style == CDockWindow::TDockingStyleRight )
				{
					pN->win->GetWindowRect(&r4);
					if ( (r4.right == r1.left) && (r4.top <= r1.bottom) )
					{
						arg._params.pResult = pN->win;
						bFound = true;
						break;
					}
				}
				++it;
			}
			if ( !bFound )
				break;
		}
		else
			break;
	}
}

void CDockInfo::FindClientNeighboursTop2DownRight(RECT r1)
{
	POINT pt = { r1.right + 10, r1.top + 10 };
	FindDockWindowForEachFunctor arg(pt);
	RECT r3;
	RECT r4;

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	while (arg._params.pResult)
	{
		m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleRight, arg._params.pResult);
		arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleLeft, m_pCurrentFloatingWindow);
		arg._params.pResult->GetWindowRect(&r3);
		if ( r3.bottom < r1.bottom )
		{
			CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
			bool bFound = false;

			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				if ( pN->style == CDockWindow::TDockingStyleRight )
				{
					pN->win->GetWindowRect(&r4);
					if ( (r4.left == r1.right) && (r4.top <= r1.bottom) )
					{
						arg._params.pResult = pN->win;
						bFound = true;
						break;
					}
				}
				++it;
			}
			if ( !bFound )
				break;
		}
		else
			break;
	}
}

void CDockInfo::FindClientNeighboursRight2LeftTop(RECT r1)
{
	POINT pt = { r1.right - 10, r1.top - 10 };
	FindDockWindowForEachFunctor arg(pt);
	RECT r3;
	RECT r4;

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	while (arg._params.pResult)
	{
		m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleTop, arg._params.pResult);
		arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleBottom, m_pCurrentFloatingWindow);
		arg._params.pResult->GetWindowRect(&r3);
		if ( r3.left > r1.left )
		{
			CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
			bool bFound = false;

			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				if ( pN->style == CDockWindow::TDockingStyleLeft )
				{
					pN->win->GetWindowRect(&r4);
					if ( (r4.bottom == r1.top) && (r4.right >= r1.left) )
					{
						arg._params.pResult = pN->win;
						bFound = true;
						break;
					}
				}
				++it;
			}
			if ( !bFound )
				break;
		}
		else
			break;
	}
}

void CDockInfo::FindClientNeighboursRight2LeftBottom(RECT r1)
{
	POINT pt = { r1.right - 10, r1.bottom + 10 };
	FindDockWindowForEachFunctor arg(pt);
	RECT r3;
	RECT r4;

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	while (arg._params.pResult)
	{
		m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleBottom, arg._params.pResult);
		arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleTop, m_pCurrentFloatingWindow);
		arg._params.pResult->GetWindowRect(&r3);
		if ( r3.left > r1.left )
		{
			CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
			bool bFound = false;

			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				if ( pN->style == CDockWindow::TDockingStyleLeft )
				{
					pN->win->GetWindowRect(&r4);
					if ( (r4.top == r1.bottom) && (r4.right >= r1.left) )
					{
						arg._params.pResult = pN->win;
						bFound = true;
						break;
					}
				}
				++it;
			}
			if ( !bFound )
				break;
		}
		else
			break;
	}
}

void CDockInfo::FindClientNeighboursDown2TopLeft(RECT r1)
{
	POINT pt = { r1.left - 10, r1.bottom - 10 };
	FindDockWindowForEachFunctor arg(pt);
	RECT r3;
	RECT r4;

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	while (arg._params.pResult)
	{
		m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleLeft, arg._params.pResult);
		arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleRight, m_pCurrentFloatingWindow);
		arg._params.pResult->GetWindowRect(&r3);
		if ( r3.top > r1.top )
		{
			CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
			bool bFound = false;

			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				if ( pN->style == CDockWindow::TDockingStyleTop )
				{
					pN->win->GetWindowRect(&r4);
					if ( (r4.right == r1.left) && (r4.bottom <= r1.top) )
					{
						arg._params.pResult = pN->win;
						bFound = true;
						break;
					}
				}
				++it;
			}
			if ( !bFound )
				break;
		}
		else
			break;
	}
}

void CDockInfo::FindClientNeighboursDown2TopRight(RECT r1)
{
	POINT pt = { r1.right + 10, r1.bottom - 10 };
	FindDockWindowForEachFunctor arg(pt);
	RECT r3;
	RECT r4;

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	while (arg._params.pResult)
	{
		m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleRight, arg._params.pResult);
		arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleLeft, m_pCurrentFloatingWindow);
		arg._params.pResult->GetWindowRect(&r3);
		if ( r3.top > r1.top )
		{
			CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
			bool bFound = false;

			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				if ( pN->style == CDockWindow::TDockingStyleTop )
				{
					pN->win->GetWindowRect(&r4);
					if ( (r4.left == r1.right) && (r4.bottom <= r1.top) )
					{
						arg._params.pResult = pN->win;
						bFound = true;
						break;
					}
				}
				++it;
			}
			if ( !bFound )
				break;
		}
		else
			break;
	}
}

void CDockInfo::CheckDockTargets(LPPOINT pt, bool bLast)
{
	RECT rWindow;
	RECT rClient;
	RECT r1;
	RECT r2;
	RECT r3;
	RECT r4;
	RECT r5;

	::CopyRect(&rWindow, &m_windowRect);
	m_pFrame->ClientToScreen(&rWindow);
	::CopyRect(&rClient, &m_clientRect);
	m_pFrame->ClientToScreen(&rClient);

	if ( !(::PtInRect(&rWindow, *pt)) )
	{
		if ( m_pTargetCenter->is_Visible() )
			m_pTargetCenter->set_Visible(FALSE);
		if ( m_pTarget[TDockTargetLeft]->is_Visible() )
			for ( int ix = TDockTargetLeft; ix <= TDockTargetBottom; ++ix )
				m_pTarget[ix]->set_Visible(FALSE);
		return;
	}
	if ( !(m_pTarget[TDockTargetLeft]->is_Visible()) )
	{
		for ( int ix = TDockTargetLeft; ix <= TDockTargetBottom; ++ix )
			m_pTarget[ix]->SetInitialPos(&rWindow);
	}

	FindDockWindowForEachFunctor arg(*pt);

	m_dockwindows.ForEach<FindDockWindowForEachFunctor>(arg);

	if ( arg._params.bAnyNonFloatingWindows )
	{
		if ( arg._params.pResult )
		{
			arg._params.pResult->GetWindowRect(&r1);
			m_pTargetCenter->GetWindowRect(&r2);
			r3.left = r1.left + ((r1.right - r1.left - r2.right + r2.left) / 2);
			r3.top = r1.top + ((r1.bottom - r1.top - r2.bottom + r2.top) / 2);
			r3.right = r3.left + r2.right - r2.left;
			r3.bottom = r3.top + r2.bottom - r2.top;
			for ( int ix = TDockTargetLeft; ix <= TDockTargetBottom; ++ix )
			{
				m_pTarget[ix]->GetWindowRect(&r4);
				if ( ::IntersectRect(&r5, &r4, &r3) )
				{
					switch ( ix )
					{
					case TDockTargetLeft:
					case TDockTargetRight:
						r1.bottom -= (r4.bottom - r4.top + r2.bottom - r2.top) * 2;
						break;
					case TDockTargetTop:
					case TDockTargetBottom:
						r1.right -= (r4.right - r4.left + r2.right - r2.left) * 2;
						break;
					}
					break;
				}
			}
			m_pTargetCenter->SetInitialPos(&r1);

			CTabControl* pTabCtrl1 = CastDynamicPtr(CTabControl, m_pCurrentFloatingWindow->get_Client());
			CTabControl* pTabCtrl2 = CastDynamicPtr(CTabControl, arg._params.pResult->get_Client());

			m_pTargetCenter->set_OverClientArea((!pTabCtrl1) || (!pTabCtrl2));
		}
		else
		{
			m_pTargetCenter->SetInitialPos(&rClient);
			m_pTargetCenter->set_OverClientArea(true);
		}
	}
	else if ( m_pTargetCenter->is_Visible() )
		m_pTargetCenter->set_Visible(FALSE);
	if ( m_pTarget[TDockTargetLeft]->CheckTarget(pt) )
	{
		m_pCurrentFloatingWindow->GetWindowRect(&r2);
		::CopyRect(&r1, &rWindow);
		if ( (r2.right - r2.left) > (rClient.right - rClient.left - 100) )
		{
			if ( (rClient.right - rClient.left) < 101 )
			{
				if ( m_pHint->is_Visible() )
					m_pHint->set_Visible(FALSE);
				return;
			}
			r1.right = r1.left + rClient.right - rClient.left - 100;
		}
		else
			r1.right = r1.left + r2.right - r2.left;
		if ( bLast )
		{
			if ( m_pHint->is_Visible() )
				m_pHint->set_Visible(FALSE);
			m_pCurrentFloatingWindow->set_dockingstyle(CDockWindow::TDockingStyleLeft);
			if ( arg._params.bAnyNonFloatingWindows )
			{
				SortDockWindowsLessFunctor arg1(TSortDockWindowsParams::TSortOrderLeft);

				m_dockwindows.Sort<SortDockWindowsLessFunctor>(arg1);

				CDockWindowVector::Iterator it = m_dockwindows.Begin();
				
				while ( it )
				{
					CDockWindow* pWin = *it;

					pWin->GetWindowRect(&r2);
					if ( r2.left != rWindow.left )
						break;
					m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleRight, pWin);
					pWin->AppendNeighbour(CDockWindow::TDockingStyleLeft, m_pCurrentFloatingWindow);
					++it;
				}
			}
			m_pCurrentFloatingWindow->Dock(&r1);
			m_pCurrentFloatingWindow->MoveNeighbours(CDockWindow::TDockingStyleRight, r1.right - r1.left);
			m_clientRect.left += r1.right - r1.left;
			UpdateFrame(m_pCurrentFloatingWindow);
			return;
		}
		m_pHint->SetPos(&r1);
		return;
	}
	if ( m_pTarget[TDockTargetTop]->CheckTarget(pt) )
	{
		m_pCurrentFloatingWindow->GetWindowRect(&r2);
		::CopyRect(&r1, &rWindow);
		if ( (r2.bottom - r2.top) > (rClient.bottom - rClient.top - 100) )
		{
			if ( (rClient.bottom - rClient.top) < 101 )
			{
				if ( m_pHint->is_Visible() )
					m_pHint->set_Visible(FALSE);
				return;
			}
			r1.bottom = r1.top + rClient.bottom - rClient.top - 100;
		}
		else
			r1.bottom = r1.top + r2.bottom - r2.top;
		if ( bLast )
		{
			if ( m_pHint->is_Visible() )
				m_pHint->set_Visible(FALSE);
			m_pCurrentFloatingWindow->set_dockingstyle(CDockWindow::TDockingStyleTop);
			if ( arg._params.bAnyNonFloatingWindows )
			{
				SortDockWindowsLessFunctor arg1(TSortDockWindowsParams::TSortOrderTop);

				m_dockwindows.Sort<SortDockWindowsLessFunctor>(arg1);

				CDockWindowVector::Iterator it = m_dockwindows.Begin();
				
				while ( it )
				{
					CDockWindow* pWin = *it;

					pWin->GetWindowRect(&r2);
					if ( r2.top != rWindow.top )
						break;
					m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleBottom, pWin);
					pWin->AppendNeighbour(CDockWindow::TDockingStyleTop, m_pCurrentFloatingWindow);
					++it;
				}
			}
			m_pCurrentFloatingWindow->Dock(&r1);
			m_pCurrentFloatingWindow->MoveNeighbours(CDockWindow::TDockingStyleBottom, r1.bottom - r1.top);
			m_clientRect.top += r1.bottom - r1.top;
			UpdateFrame(m_pCurrentFloatingWindow);
			return;
		}
		m_pHint->SetPos(&r1);
		return;
	}
	if ( m_pTarget[TDockTargetRight]->CheckTarget(pt) )
	{
		m_pCurrentFloatingWindow->GetWindowRect(&r2);
		::CopyRect(&r1, &rWindow);
		if ( (r2.right - r2.left) > (rClient.right - rClient.left - 100) )
		{
			if ( (rClient.right - rClient.left) < 101 )
			{
				if ( m_pHint->is_Visible() )
					m_pHint->set_Visible(FALSE);
				return;
			}
			r1.left = r1.right - (rClient.right - rClient.left - 100);
		}
		else
			r1.left = r1.right - (r2.right - r2.left);
		if ( bLast )
		{
			if ( m_pHint->is_Visible() )
				m_pHint->set_Visible(FALSE);
			m_pCurrentFloatingWindow->set_dockingstyle(CDockWindow::TDockingStyleRight);
			if ( arg._params.bAnyNonFloatingWindows )
			{
				SortDockWindowsLessFunctor arg1(TSortDockWindowsParams::TSortOrderRight, true);

				m_dockwindows.Sort<SortDockWindowsLessFunctor>(arg1);

				CDockWindowVector::Iterator it = m_dockwindows.Begin();
				
				while ( it )
				{
					CDockWindow* pWin = *it;

					pWin->GetWindowRect(&r2);
					if ( r2.right != rWindow.right )
						break;
					m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleLeft, pWin);
					pWin->AppendNeighbour(CDockWindow::TDockingStyleRight, m_pCurrentFloatingWindow);
					++it;
				}
			}
			m_pCurrentFloatingWindow->Dock(&r1);
			m_pCurrentFloatingWindow->MoveNeighbours(CDockWindow::TDockingStyleLeft, r1.right - r1.left);
			m_clientRect.right -= r1.right - r1.left;
			UpdateFrame(m_pCurrentFloatingWindow);
			return;
		}
		m_pHint->SetPos(&r1);
		return;
	}
	if ( m_pTarget[TDockTargetBottom]->CheckTarget(pt) )
	{
		m_pCurrentFloatingWindow->GetWindowRect(&r2);
		::CopyRect(&r1, &rWindow);
		if ( (r2.bottom - r2.top) > (rClient.bottom - rClient.top - 100) )
		{
			if ( (rClient.bottom - rClient.top) < 101 )
			{
				if ( m_pHint->is_Visible() )
					m_pHint->set_Visible(FALSE);
				return;
			}
			r1.top = r1.bottom - (rClient.bottom - rClient.top - 100);
		}
		else
			r1.top = r1.bottom - (r2.bottom - r2.top);
		if ( bLast )
		{
			if ( m_pHint->is_Visible() )
				m_pHint->set_Visible(FALSE);
			m_pCurrentFloatingWindow->set_dockingstyle(CDockWindow::TDockingStyleBottom);
			if ( arg._params.bAnyNonFloatingWindows )
			{
				SortDockWindowsLessFunctor arg1(TSortDockWindowsParams::TSortOrderBottom, true);

				m_dockwindows.Sort<SortDockWindowsLessFunctor>(arg1);

				CDockWindowVector::Iterator it = m_dockwindows.Begin();
				
				while ( it )
				{
					CDockWindow* pWin = *it;

					pWin->GetWindowRect(&r2);
					if ( r2.bottom != rWindow.bottom )
						break;
					m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleTop, pWin);
					pWin->AppendNeighbour(CDockWindow::TDockingStyleBottom, m_pCurrentFloatingWindow);
					++it;
				}
			}
			m_pCurrentFloatingWindow->Dock(&r1);
			m_pCurrentFloatingWindow->MoveNeighbours(CDockWindow::TDockingStyleTop, r1.bottom - r1.top);
			m_clientRect.bottom -= r1.bottom - r1.top;
			UpdateFrame(m_pCurrentFloatingWindow);
			return;
		}
		m_pHint->SetPos(&r1);
		return;
	}
	if ( m_pTargetCenter->is_Visible() )
	{
		switch ( m_pTargetCenter->CheckTarget(pt) )
		{
		case 1: // Left
			if ( arg._params.pResult )
			{
				arg._params.pResult->GetWindowRect(&r1);
				if ( (r1.right - r1.left) <= 150 )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					return;
				}
				::CopyRect(&r3, &r1);
				m_pCurrentFloatingWindow->GetWindowRect(&r2);
				if ( (r2.right - r2.left) > (r1.right - r1.left) )
					r1.right = r1.left + ((r1.right - r1.left) / 2);
				else
					r1.right = r1.left + r2.right - r2.left;
				if ( bLast )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					m_pCurrentFloatingWindow->set_dockingstyle(arg._params.pResult->get_dockingstyle());
					m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleRight, arg._params.pResult);
					if ( arg._params.pResult->has_Neighbours() )
					{
						CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
						CDockWindowVector delPtr(__FILE__LINE__ 16,16);

						while ( it )
						{
							CDockWindow::TNeighbour* pN = *it;

							switch ( pN->style )
							{
							case CDockWindow::TDockingStyleLeft:
								m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleLeft, pN->win);
								pN->win->RemoveNeighbour(arg._params.pResult);
								pN->win->AppendNeighbour(CDockWindow::TDockingStyleRight, m_pCurrentFloatingWindow);
								delPtr.Append(pN->win);
								break;
							case CDockWindow::TDockingStyleTop:
								pN->win->GetWindowRect(&r4);
								if ( r4.right <= r1.right )
								{
									delPtr.Append(pN->win);
									pN->win->RemoveNeighbour(arg._params.pResult);
								}
								if ( r4.left <= r1.right )
								{
									pN->win->AppendNeighbour(CDockWindow::TDockingStyleBottom, m_pCurrentFloatingWindow);
									m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleTop, pN->win);
								}
								break;
							case CDockWindow::TDockingStyleRight:
								break;
							case CDockWindow::TDockingStyleBottom:
								pN->win->GetWindowRect(&r4);
								if ( r4.right <= r1.right )
								{
									delPtr.Append(pN->win);
									pN->win->RemoveNeighbour(arg._params.pResult);
								}
								if ( r4.left <= r1.right )
								{
									pN->win->AppendNeighbour(CDockWindow::TDockingStyleTop, m_pCurrentFloatingWindow);
									m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleBottom, pN->win);
								}
								break;
							}
							++it;
						}

						CDockWindowVector::Iterator it2 = delPtr.Begin();

						while ( it2 )
						{
							arg._params.pResult->RemoveNeighbour(*it2);
							++it2;
						}
					}
					arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleLeft, m_pCurrentFloatingWindow);
					m_pCurrentFloatingWindow->Dock(&r1);
					m_pCurrentFloatingWindow->SizeNeighbours(CDockWindow::TDockingStyleRight, r1.right - r1.left);
					UpdateFrame(m_pCurrentFloatingWindow);
					return;
				}
				m_pHint->SetPos(&r1);
				return;
			}
			else
			{
				if ( (rClient.right - rClient.left) <= 150 )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					return;
				}
				::CopyRect(&r1, &rClient);
				m_pCurrentFloatingWindow->GetWindowRect(&r2);
				if ( (r2.right - r2.left) > (r1.right - r1.left) )
					r1.right = r1.left + ((r1.right - r1.left) / 2);
				else
					r1.right = r1.left + r2.right - r2.left;
				if ( bLast )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					m_pCurrentFloatingWindow->set_dockingstyle(CDockWindow::TDockingStyleLeft);

					if ( r1.top > rWindow.top )
						FindClientNeighboursLeft2RightTop(r1);
					if ( r1.left > rWindow.left )
						FindClientNeighboursTop2DownLeft(r1);
					if ( r1.bottom < rWindow.bottom )
						FindClientNeighboursLeft2RightBottom(r1);
					m_pCurrentFloatingWindow->Dock(&r1);
					m_clientRect.left += r1.right - r1.left;
					UpdateFrame(m_pCurrentFloatingWindow);
					return;
				}
				m_pHint->SetPos(&r1);
				return;
			}
			break;
		case 2: // Top
			if ( arg._params.pResult )
			{
				arg._params.pResult->GetWindowRect(&r1);
				if ( (r1.bottom - r1.top) <= 150 )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					return;
				}
				::CopyRect(&r3, &r1);
				m_pCurrentFloatingWindow->GetWindowRect(&r2);
				if ( (r2.bottom - r2.top) > (r1.bottom - r1.top) )
					r1.bottom = r1.top + ((r1.bottom - r1.top) / 2);
				else
					r1.bottom = r1.top + r2.bottom - r2.top;
				if ( bLast )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					m_pCurrentFloatingWindow->set_dockingstyle(arg._params.pResult->get_dockingstyle());
					m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleBottom, arg._params.pResult);
					if ( arg._params.pResult->has_Neighbours() )
					{
						CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
						CDockWindowVector delPtr(__FILE__LINE__ 16,16);

						while ( it )
						{
							CDockWindow::TNeighbour* pN = *it;

							switch ( pN->style )
							{
							case CDockWindow::TDockingStyleLeft:
								pN->win->GetWindowRect(&r4);
								if ( r4.bottom <= r1.bottom )
								{
									delPtr.Append(pN->win);
									pN->win->RemoveNeighbour(arg._params.pResult);
								}
								if ( r4.top <= r1.bottom )
								{
									pN->win->AppendNeighbour(CDockWindow::TDockingStyleRight, m_pCurrentFloatingWindow);
									m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleLeft, pN->win);
								}
								break;
							case CDockWindow::TDockingStyleTop:
								m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleTop, pN->win);
								pN->win->RemoveNeighbour(arg._params.pResult);
								pN->win->AppendNeighbour(CDockWindow::TDockingStyleBottom, m_pCurrentFloatingWindow);
								delPtr.Append(pN->win);
								break;
							case CDockWindow::TDockingStyleRight:
								pN->win->GetWindowRect(&r4);
								if ( r4.bottom <= r1.bottom )
								{
									delPtr.Append(pN->win);
									pN->win->RemoveNeighbour(arg._params.pResult);
								}
								if ( r4.top <= r1.bottom )
								{
									pN->win->AppendNeighbour(CDockWindow::TDockingStyleLeft, m_pCurrentFloatingWindow);
									m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleRight, pN->win);
								}
								break;
							case CDockWindow::TDockingStyleBottom:
								break;
							}
							++it;
						}

						CDockWindowVector::Iterator it2 = delPtr.Begin();

						while ( it2 )
						{
							arg._params.pResult->RemoveNeighbour(*it2);
							++it2;
						}
					}
					arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleTop, m_pCurrentFloatingWindow);
					m_pCurrentFloatingWindow->Dock(&r1);
					m_pCurrentFloatingWindow->SizeNeighbours(CDockWindow::TDockingStyleBottom, r1.bottom - r1.top);
					UpdateFrame(m_pCurrentFloatingWindow);
					return;
				}
				m_pHint->SetPos(&r1);
				return;
			}
			else
			{
				if ( (rClient.bottom - rClient.top) <= 150 )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					return;
				}
				::CopyRect(&r1, &rClient);
				m_pCurrentFloatingWindow->GetWindowRect(&r2);
				if ( (r2.bottom - r2.top) > (r1.bottom - r1.top) )
					r1.bottom = r1.top + ((r1.bottom - r1.top) / 2);
				else
					r1.bottom = r1.top + r2.bottom - r2.top;
				if ( bLast )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					m_pCurrentFloatingWindow->set_dockingstyle(CDockWindow::TDockingStyleTop);

					if ( r1.top > rWindow.top )
						FindClientNeighboursLeft2RightTop(r1);
					if ( r1.left > rWindow.left )
						FindClientNeighboursTop2DownLeft(r1);
					if ( r1.right < rWindow.right )
						FindClientNeighboursTop2DownRight(r1);
					m_pCurrentFloatingWindow->Dock(&r1);
					m_clientRect.top += r1.bottom - r1.top;
					UpdateFrame(m_pCurrentFloatingWindow);
					return;
				}
				m_pHint->SetPos(&r1);
				return;
			}
			break;
		case 3: // Middle
			if ( arg._params.pResult && bLast )
			{
				CTabControl* pTabCtrl1 = CastDynamicPtr(CTabControl, m_pCurrentFloatingWindow->get_Client());
				CTabControl* pTabCtrl2 = CastDynamicPtr(CTabControl, arg._params.pResult->get_Client());

				if ( pTabCtrl1 && pTabCtrl2 )
				{
					CTabPage* pPage = CastDynamicPtr(CTabPage, pTabCtrl2->get_firstchild());
					CTabPage* pPage1;
					UINT maxId = 999;
					CStringBuffer name;

					while ( pPage )
					{
						if ( pPage->get_childID() > maxId )
							maxId = pPage->get_childID();
						pPage = CastDynamicPtr(CTabPage, pPage->get_siblingByCmd(GW_HWNDNEXT));
					}
					pPage = CastDynamicPtr(CTabPage, pTabCtrl1->get_firstchild());
					while ( pPage )
					{
						pPage1 = CastDynamicPtr(CTabPage, pPage->get_siblingByCmd(GW_HWNDNEXT));
						pPage->set_parenthandle(pTabCtrl2->get_handle());
						pPage->set_childID(++maxId);
						name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), pTabCtrl2->get_name(), pPage->get_childID());
						pPage->set_name(name);
						pPage->set_Text(name);
						pPage->SetWindowPos(HWND_BOTTOM, nullptr, 0);
						pPage->set_Selected(false);
						pPage = pPage1;
					}
					pTabCtrl2->SendMessage(WM_SIZE, 0, 0);
					m_pCurrentFloatingWindow->DestroyWindow();
				}
			}
			break;
		case 4: // Right
			if ( arg._params.pResult )
			{
				arg._params.pResult->GetWindowRect(&r1);
				if ( (r1.right - r1.left) <= 150 )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					return;
				}
				::CopyRect(&r3, &r1);
				m_pCurrentFloatingWindow->GetWindowRect(&r2);
				if ( (r2.right - r2.left) > (r1.right - r1.left) )
					r1.left = r1.right - ((r1.right - r1.left) / 2);
				else
					r1.left = r1.right - (r2.right - r2.left);
				if ( bLast )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					m_pCurrentFloatingWindow->set_dockingstyle(arg._params.pResult->get_dockingstyle());
					m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleLeft, arg._params.pResult);
					if ( arg._params.pResult->has_Neighbours() )
					{
						CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
						CDockWindowVector delPtr(__FILE__LINE__ 16, 16);

						while ( it )
						{
							CDockWindow::TNeighbour* pN = *it;

							switch ( pN->style )
							{
							case CDockWindow::TDockingStyleLeft:
								break;
							case CDockWindow::TDockingStyleTop:
								pN->win->GetWindowRect(&r4);
								if ( r4.left >= r1.left )
								{
									delPtr.Append(pN->win);
									pN->win->RemoveNeighbour(arg._params.pResult);
								}
								if ( r4.right >= r1.left )
								{
									pN->win->AppendNeighbour(CDockWindow::TDockingStyleBottom, m_pCurrentFloatingWindow);
									m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleTop, pN->win);
								}
								break;
							case CDockWindow::TDockingStyleRight:
								m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleRight, pN->win);
								pN->win->RemoveNeighbour(arg._params.pResult);
								pN->win->AppendNeighbour(CDockWindow::TDockingStyleLeft, m_pCurrentFloatingWindow);
								delPtr.Append(pN->win);
								break;
							case CDockWindow::TDockingStyleBottom:
								pN->win->GetWindowRect(&r4);
								if ( r4.left >= r1.left )
								{
									delPtr.Append(pN->win);
									pN->win->RemoveNeighbour(arg._params.pResult);
								}
								if ( r4.right >= r1.left )
								{
									pN->win->AppendNeighbour(CDockWindow::TDockingStyleTop, m_pCurrentFloatingWindow);
									m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleBottom, pN->win);
								}
								break;
							}
							++it;
						}

						CDockWindowVector::Iterator it2 = delPtr.Begin();

						while ( it2 )
						{
							arg._params.pResult->RemoveNeighbour(*it2);
							++it2;
						}
					}
					arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleRight, m_pCurrentFloatingWindow);
					m_pCurrentFloatingWindow->Dock(&r1);
					m_pCurrentFloatingWindow->SizeNeighbours(CDockWindow::TDockingStyleLeft, r1.right - r1.left);
					UpdateFrame(m_pCurrentFloatingWindow);
					return;
				}
				m_pHint->SetPos(&r1);
				return;
			}
			else
			{
				if ( (rClient.right - rClient.left) <= 150 )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					return;
				}
				::CopyRect(&r1, &rClient);
				m_pCurrentFloatingWindow->GetWindowRect(&r2);
				if ( (r2.right - r2.left) > (r1.right - r1.left) )
					r1.left = r1.right - ((r1.right - r1.left) / 2);
				else
					r1.left = r1.right - (r2.right - r2.left);
				if ( bLast )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					m_pCurrentFloatingWindow->set_dockingstyle(CDockWindow::TDockingStyleRight);

					if ( r1.top > rWindow.top )
						FindClientNeighboursRight2LeftTop(r1);
					if ( r1.right < rWindow.right )
						FindClientNeighboursTop2DownRight(r1);
					if ( r1.bottom < rWindow.bottom )
						FindClientNeighboursRight2LeftBottom(r1);
					m_pCurrentFloatingWindow->Dock(&r1);
					m_clientRect.right -= r1.right - r1.left;
					UpdateFrame(m_pCurrentFloatingWindow);
					return;
				}
				m_pHint->SetPos(&r1);
				return;
			}
			break;
		case 5: // Bottom
			if ( arg._params.pResult )
			{
				arg._params.pResult->GetWindowRect(&r1);
				if ( (r1.bottom - r1.top) <= 150 )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					return;
				}
				::CopyRect(&r3, &r1);
				m_pCurrentFloatingWindow->GetWindowRect(&r2);
				if ( (r2.bottom - r2.top) > (r1.bottom - r1.top) )
					r1.top = r1.bottom - ((r1.bottom - r1.top) / 2);
				else
					r1.top = r1.bottom - (r2.bottom - r2.top);
				if ( bLast )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					m_pCurrentFloatingWindow->set_dockingstyle(arg._params.pResult->get_dockingstyle());
					m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleTop, arg._params.pResult);
					if ( arg._params.pResult->has_Neighbours() )
					{
						CDockWindow::TNeighbourVector::Iterator it = arg._params.pResult->get_Neighbours();
						CDockWindowVector delPtr(__FILE__LINE__ 16, 16);

						while ( it )
						{
							CDockWindow::TNeighbour* pN = *it;

							switch ( pN->style )
							{
							case CDockWindow::TDockingStyleLeft:
								pN->win->GetWindowRect(&r4);
								if ( r4.top >= r1.top )
								{
									delPtr.Append(pN->win);
									pN->win->RemoveNeighbour(arg._params.pResult);
								}
								if ( r4.bottom >= r1.top )
								{
									pN->win->AppendNeighbour(CDockWindow::TDockingStyleRight, m_pCurrentFloatingWindow);
									m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleLeft, pN->win);
								}
								break;
							case CDockWindow::TDockingStyleTop:
								break;
							case CDockWindow::TDockingStyleRight:
								pN->win->GetWindowRect(&r4);
								if ( r4.top >= r1.top )
								{
									delPtr.Append(pN->win);
									pN->win->RemoveNeighbour(arg._params.pResult);
								}
								if ( r4.bottom >= r1.top )
								{
									pN->win->AppendNeighbour(CDockWindow::TDockingStyleLeft, m_pCurrentFloatingWindow);
									m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleRight, pN->win);
								}
								break;
							case CDockWindow::TDockingStyleBottom:
								m_pCurrentFloatingWindow->AppendNeighbour(CDockWindow::TDockingStyleBottom, pN->win);
								pN->win->RemoveNeighbour(arg._params.pResult);
								pN->win->AppendNeighbour(CDockWindow::TDockingStyleTop, m_pCurrentFloatingWindow);
								delPtr.Append(pN->win);
								break;
							}
							++it;
						}

						CDockWindowVector::Iterator it2 = delPtr.Begin();

						while ( it2 )
						{
							arg._params.pResult->RemoveNeighbour(*it2);
							++it2;
						}
					}
					arg._params.pResult->AppendNeighbour(CDockWindow::TDockingStyleBottom, m_pCurrentFloatingWindow);
					m_pCurrentFloatingWindow->Dock(&r1);
					m_pCurrentFloatingWindow->SizeNeighbours(CDockWindow::TDockingStyleTop, r1.bottom - r1.top);
					UpdateFrame(m_pCurrentFloatingWindow);
					return;
				}
				m_pHint->SetPos(&r1);
				return;
			}
			else
			{
				if ( (rClient.bottom - rClient.top) <= 150 )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					return;
				}
				::CopyRect(&r1, &rClient);
				m_pCurrentFloatingWindow->GetWindowRect(&r2);
				if ( (r2.bottom - r2.top) > (r1.bottom - r1.top) )
					r1.top = r1.bottom - ((r1.bottom - r1.top) / 2);
				else
					r1.top = r1.bottom - (r2.bottom - r2.top);
				if ( bLast )
				{
					if ( m_pHint->is_Visible() )
						m_pHint->set_Visible(FALSE);
					m_pCurrentFloatingWindow->set_dockingstyle(CDockWindow::TDockingStyleBottom);

					if ( r1.bottom < rWindow.bottom )
						FindClientNeighboursLeft2RightBottom(r1);
					if ( r1.left > rWindow.left )
						FindClientNeighboursDown2TopLeft(r1);
					if ( r1.right < rWindow.right )
						FindClientNeighboursDown2TopRight(r1);
					m_pCurrentFloatingWindow->Dock(&r1);
					m_clientRect.bottom -= r1.bottom - r1.top;
					UpdateFrame(m_pCurrentFloatingWindow);
					return;
				}
				m_pHint->SetPos(&r1);
				return;
			}
			break;
		default:
			break;
		}
	}
	if ( m_pHint->is_Visible() )
		m_pHint->set_Visible(FALSE);
}

void CDockInfo::HideDockTargets()
{
	if ( !m_pTargetCenter )
		return;
	m_pCurrentFloatingWindow = nullptr;
	if ( m_pTargetCenter->is_Visible() )
		m_pTargetCenter->set_Visible(FALSE);
	if ( m_pTarget[TDockTargetLeft]->is_Visible() )
		for ( int ix = TDockTargetLeft; ix <= TDockTargetBottom; ++ix )
			m_pTarget[ix]->set_Visible(FALSE);
}

class ShowFloatingWindowsForEachFunctor
{
public:
	bool operator()(Ptr(CDockWindow) pWin)
	{
		if ( !(pWin->is_Visible()) )
			pWin->set_Visible(TRUE);
		return true;
	}
};

void CDockInfo::ShowFloatingWindows()
{
	m_floatingdockwindows.ForEach<ShowFloatingWindowsForEachFunctor>();
}

class HideFloatingWindowsForEachFunctor
{
public:
	bool operator()(Ptr(CDockWindow) pWin)
	{
		if (pWin->is_Visible())
			pWin->set_Visible(FALSE);
		return true;
	}
};

void CDockInfo::HideFloatingWindows()
{
	m_floatingdockwindows.ForEach<HideFloatingWindowsForEachFunctor>();
}

bool CDockInfo::TestSizableLeftTop(CDockWindow* pCurWin, CDockWindowVector* pCurWins, CDockWindow* pCurLeftNeighbour, CDockWindowVector* pCurNeighbours, LONG edge)
{
	CDockWindow::TNeighbourVector::Iterator it;
	CDockWindow::TNeighbourVector::Iterator it1;
	RECT r;
	RECT r1;
	RECT r2;
	bool bResultTop = false;
	bool bNeighbour = false;
	bool bAll = true;

	if ( pCurWins )
		pCurWins->StartTransact();
	if ( pCurNeighbours )
		pCurNeighbours->StartTransact();
	pCurWin->GetParentRect(&r);
	pCurLeftNeighbour->GetParentRect(&r1);
	while ( true )
	{
		if ( bAll || (!bNeighbour) )
		{
			it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( edge == r2.left )
					break;
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleTop, it);
			}
		}
		if ( bAll || bNeighbour )
		{
			it1 = pCurLeftNeighbour->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
			while ( it1 )
			{
				CDockWindow::TNeighbour* pN = *it1;

				pN->win->GetParentRect(&r2);
				if ( edge == r2.right )
					break;
				it1 = pCurLeftNeighbour->get_NextNeighbour(CDockWindow::TDockingStyleTop, it1);
			}
		}
		bAll = false;
		if ( r.top < r1.top )
		{
			if ( !it1 )
				break;
			pCurLeftNeighbour = (*it1)->win;
			if ( pCurNeighbours )
				pCurNeighbours->Append(pCurLeftNeighbour);
			pCurLeftNeighbour->GetParentRect(&r1);
			bNeighbour = true;
		}
		else if ( r.top > r1.top )
		{
			if ( !it )
				break;
			pCurWin = (*it)->win;
			if ( pCurWins )
				pCurWins->Append(pCurWin);
			pCurWin = (*it)->win;
			pCurWin->GetParentRect(&r);
			bNeighbour = false;
		}
		else
		{
			if ( (!it) && (!it1) )
			{
				bResultTop = true;
				break;
			}
			if ( (!it) || (!it1) )
				break;
			pCurWin = (*it)->win;
			if ( pCurWins )
				pCurWins->Append(pCurWin);
			pCurWin->GetParentRect(&r);
			pCurLeftNeighbour = (*it1)->win;
			if ( pCurNeighbours )
				pCurNeighbours->Append(pCurLeftNeighbour);
			pCurLeftNeighbour->GetParentRect(&r1);
			bAll = true;
		}
	}
	if ( bResultTop )
	{
		if ( pCurWins )
			pCurWins->Commit();
		if ( pCurNeighbours )
			pCurNeighbours->Commit();
	}
	else
	{
		if ( pCurWins )
			pCurWins->Rollback();
		if ( pCurNeighbours )
			pCurNeighbours->Rollback();
	}
	return bResultTop;
}

bool CDockInfo::TestSizableLeftBottom(CDockWindow* pCurWin, CDockWindowVector* pCurWins, CDockWindow* pCurLeftNeighbour, CDockWindowVector* pCurNeighbours, LONG edge)
{
	CDockWindow::TNeighbourVector::Iterator it;
	CDockWindow::TNeighbourVector::Iterator it1;
	RECT r;
	RECT r1;
	RECT r2;
	bool bResultBottom = false;
	bool bNeighbour = false;
	bool bAll = true;

	if ( pCurWins )
		pCurWins->StartTransact();
	if ( pCurNeighbours )
		pCurNeighbours->StartTransact();
	pCurWin->GetParentRect(&r);
	pCurLeftNeighbour->GetParentRect(&r1);
	while ( true )
	{
		if ( bAll || (!bNeighbour) )
		{
			it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( edge == r2.left )
					break;
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleBottom, it);
			}
		}
		if ( bAll || bNeighbour )
		{
			it1 = pCurLeftNeighbour->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
			while ( it1 )
			{
				CDockWindow::TNeighbour* pN = *it1;

				pN->win->GetParentRect(&r2);
				if ( edge == r2.right )
					break;
				it1 = pCurLeftNeighbour->get_NextNeighbour(CDockWindow::TDockingStyleBottom, it1);
			}
		}
		bAll = false;
		if ( r.bottom > r1.bottom )
		{
			if ( !it1 )
				break;
			pCurLeftNeighbour = (*it1)->win;
			if ( pCurNeighbours )
				pCurNeighbours->Append(pCurLeftNeighbour);
			pCurLeftNeighbour->GetParentRect(&r1);
			bNeighbour = true;
		}
		else if ( r.bottom < r1.bottom )
		{
			if ( !it )
				break;
			pCurWin = (*it)->win;
			if ( pCurWins )
				pCurWins->Append(pCurWin);
			pCurWin->GetParentRect(&r);
			bNeighbour = false;
		}
		else
		{
			if ( (!it) && (!it1) )
			{
				bResultBottom = true;
				break;
			}
			if ( (!it) || (!it1) )
				break;
			pCurWin = (*it)->win;
			if ( pCurWins )
				pCurWins->Append(pCurWin);
			pCurWin->GetParentRect(&r);
			pCurLeftNeighbour = (*it1)->win;
			if ( pCurNeighbours )
				pCurNeighbours->Append(pCurLeftNeighbour);
			pCurLeftNeighbour->GetParentRect(&r1);
			bAll = true;
		}
	}
	if ( bResultBottom )
	{
		if ( pCurWins )
			pCurWins->Commit();
		if ( pCurNeighbours )
			pCurNeighbours->Commit();
	}
	else
	{
		if ( pCurWins )
			pCurWins->Rollback();
		if ( pCurNeighbours )
			pCurNeighbours->Rollback();
	}
	return bResultBottom;
}

bool CDockInfo::TestSizableTopLeft(CDockWindow* pCurWin, CDockWindowVector* pCurWins, CDockWindow* pCurTopNeighbour, CDockWindowVector* pCurNeighbours, LONG edge)
{
	CDockWindow::TNeighbourVector::Iterator it;
	CDockWindow::TNeighbourVector::Iterator it1;
	RECT r;
	RECT r1;
	RECT r2;
	bool bResultLeft = false;
	bool bNeighbour = false;
	bool bAll = true;

	if ( pCurWins )
		pCurWins->StartTransact();
	if ( pCurNeighbours )
		pCurNeighbours->StartTransact();
	pCurWin->GetParentRect(&r);
	pCurTopNeighbour->GetParentRect(&r1);
	while ( true )
	{
		if ( bAll || (!bNeighbour) )
		{
			it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( edge == r2.top )
					break;
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleLeft, it);
			}
		}
		if ( bAll || bNeighbour )
		{
			it1 = pCurTopNeighbour->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
			while ( it1 )
			{
				CDockWindow::TNeighbour* pN = *it1;

				pN->win->GetParentRect(&r2);
				if ( edge == r2.bottom )
					break;
				it1 = pCurTopNeighbour->get_NextNeighbour(CDockWindow::TDockingStyleLeft, it1);
			}
		}
		bAll = false;
		if ( r.left < r1.left )
		{
			if ( !it1 )
				break;
			pCurTopNeighbour = (*it1)->win;
			if ( pCurNeighbours )
				pCurNeighbours->Append(pCurTopNeighbour);
			pCurTopNeighbour->GetParentRect(&r1);
			bNeighbour = true;
		}
		else if ( r.left > r1.left )
		{
			if ( !it )
				break;
			pCurWin = (*it)->win;
			if ( pCurWins )
				pCurWins->Append(pCurWin);
			pCurWin = (*it)->win;
			pCurWin->GetParentRect(&r);
			bNeighbour = false;
		}
		else
		{
			if ( (!it) && (!it1) )
			{
				bResultLeft = true;
				break;
			}
			if ( (!it) || (!it1) )
				break;
			pCurWin = (*it)->win;
			if ( pCurWins )
				pCurWins->Append(pCurWin);
			pCurWin->GetParentRect(&r);
			pCurTopNeighbour = (*it1)->win;
			if ( pCurNeighbours )
				pCurNeighbours->Append(pCurTopNeighbour);
			pCurTopNeighbour->GetParentRect(&r1);
			bAll = true;
		}
	}
	if ( bResultLeft )
	{
		if ( pCurWins )
			pCurWins->Commit();
		if ( pCurNeighbours )
			pCurNeighbours->Commit();
	}
	else
	{
		if ( pCurWins )
			pCurWins->Rollback();
		if ( pCurNeighbours )
			pCurNeighbours->Rollback();
	}
	return bResultLeft;
}

bool CDockInfo::TestSizableTopRight(CDockWindow* pCurWin, CDockWindowVector* pCurWins, CDockWindow* pCurTopNeighbour, CDockWindowVector* pCurNeighbours, LONG edge)
{
	CDockWindow::TNeighbourVector::Iterator it;
	CDockWindow::TNeighbourVector::Iterator it1;
	RECT r;
	RECT r1;
	RECT r2;
	bool bResultRight = false;
	bool bNeighbour = false;
	bool bAll = true;

	if ( pCurWins )
		pCurWins->StartTransact();
	if ( pCurNeighbours )
		pCurNeighbours->StartTransact();
	pCurWin->GetParentRect(&r);
	pCurTopNeighbour->GetParentRect(&r1);
	while ( true )
	{
		if ( bAll || (!bNeighbour) )
		{
			it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
			while ( it )
			{
				CDockWindow::TNeighbour* pN = *it;

				pN->win->GetParentRect(&r2);
				if ( edge == r2.top )
					break;
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleRight, it);
			}
		}
		if ( bAll || bNeighbour )
		{
			it1 = pCurTopNeighbour->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
			while ( it1 )
			{
				CDockWindow::TNeighbour* pN = *it1;

				pN->win->GetParentRect(&r2);
				if ( edge == r2.bottom )
					break;
				it1 = pCurTopNeighbour->get_NextNeighbour(CDockWindow::TDockingStyleRight, it1);
			}
		}
		bAll = false;
		if ( r.right > r1.right )
		{
			if ( !it1 )
				break;
			pCurTopNeighbour = (*it1)->win;
			if ( pCurNeighbours )
				pCurNeighbours->Append(pCurTopNeighbour);
			pCurTopNeighbour->GetParentRect(&r1);
			bNeighbour = true;
		}
		else if ( r.right < r1.right )
		{
			if ( !it )
				break;
			pCurWin = (*it)->win;
			if ( pCurWins )
				pCurWins->Append(pCurWin);
			pCurWin = (*it)->win;
			pCurWin->GetParentRect(&r);
			bNeighbour = false;
		}
		else
		{
			if ( (!it) && (!it1) )
			{
				bResultRight = true;
				break;
			}
			if ( (!it) || (!it1) )
				break;
			pCurWin = (*it)->win;
			if ( pCurWins )
				pCurWins->Append(pCurWin);
			pCurWin->GetParentRect(&r);
			pCurTopNeighbour = (*it1)->win;
			if ( pCurNeighbours )
				pCurNeighbours->Append(pCurTopNeighbour);
			pCurTopNeighbour->GetParentRect(&r1);
			bAll = true;
		}
	}
	if ( bResultRight )
	{
		if ( pCurWins )
			pCurWins->Commit();
		if ( pCurNeighbours )
			pCurNeighbours->Commit();
	}
	else
	{
		if ( pCurWins )
			pCurWins->Rollback();
		if ( pCurNeighbours )
			pCurNeighbours->Rollback();
	}
	return bResultRight;
}

bool CDockInfo::TestSizableClientLeft(CDockWindow* pWin, CDockWindowVector* pCurWins, CDockWindowVector* pCurNeighbours)
{
	CDockWindow::TNeighbourVector::Iterator it;
	CDockWindow* pCurWin;
	bool bAny;
	bool bResult;
	bool bResult1;
	bool bResult2;
	RECT r;
	RECT r1;

	pWin->GetParentRect(&r);
	if ( pCurWins )
		pCurWins->Append(pWin);
	if ( (r.top < m_clientRect.top) || (r.bottom > m_clientRect.bottom) )
	{
		it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
		bResult1 = false;
		bResult2 = false;
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.bottom == m_clientRect.top )
			{
				if ( pCurNeighbours )
					pCurNeighbours->Append((*it)->win);
				bResult1 = TestSizableLeftTop(pWin, pCurWins, (*it)->win, pCurNeighbours, m_clientRect.right);
			}
			if ( r1.top == m_clientRect.bottom )
			{
				if ( pCurNeighbours )
					pCurNeighbours->Append((*it)->win);
				bResult2 = TestSizableLeftBottom(pWin, pCurWins, (*it)->win, pCurNeighbours, m_clientRect.right);
			}
			it = pWin->get_NextNeighbour(CDockWindow::TDockingStyleLeft, it);
		}
		if ( (!bResult1) && (!bResult2) )
			return false;
	}
	if ( r.top >= m_clientRect.top )
	{
		bAny = false;
		bResult = false;
		pCurWin = pWin;
		it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.left == m_clientRect.right )
			{
				bAny = true;
				pCurWin = pN->win;
				if ( pCurWins )
					pCurWins->Append(pCurWin);
				if ( r1.top < m_clientRect.top )
				{
					CDockWindow::TNeighbourVector::Iterator it1 = pN->win->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);

					while ( it1 )
					{
						CDockWindow::TNeighbour* pN1 = *it1;

						pN1->win->GetParentRect(&r1);
						if ( r1.bottom == m_clientRect.top )
							break;
						it1 = pN->win->get_NextNeighbour(CDockWindow::TDockingStyleLeft, it1);
					}
					assert(it1);
					if ( pCurNeighbours )
						pCurNeighbours->Append((*it1)->win);
					bResult = TestSizableLeftTop(pCurWin, pCurWins, (*it1)->win, pCurNeighbours, m_clientRect.right);
					break;
				}
				else if ( r1.top == m_clientRect.top )
				{
					bResult = true;
					break;
				}
				it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
			}
			else
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleTop, it);
		}
		if ( bAny && (!bResult) )
			return false;
	}
	if ( r.bottom <= m_clientRect.bottom )
	{
		bAny = false;
		bResult = false;
		pCurWin = pWin;
		it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.left == m_clientRect.right )
			{
				bAny = true;
				pCurWin = pN->win;
				if ( pCurWins )
					pCurWins->Append(pCurWin);
				if ( r1.bottom > m_clientRect.bottom )
				{
					CDockWindow::TNeighbourVector::Iterator it1 = pN->win->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);

					while ( it1 )
					{
						CDockWindow::TNeighbour* pN1 = *it1;

						pN1->win->GetParentRect(&r1);
						if ( r1.top == m_clientRect.bottom )
							break;
						it1 = pN->win->get_NextNeighbour(CDockWindow::TDockingStyleLeft, it1);
					}
					assert(it1);
					if ( pCurNeighbours )
						pCurNeighbours->Append((*it1)->win);
					bResult = TestSizableLeftBottom(pCurWin, pCurWins, (*it1)->win, pCurNeighbours, m_clientRect.right);
					break;
				}
				else if ( r1.bottom == m_clientRect.bottom )
				{
					bResult = true;
					break;
				}
				it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
			}
			else
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleBottom, it);
		}
		if ( bAny && (!bResult) )
			return false;
	}
	return true;
}

bool CDockInfo::TestSizableClientRight(CDockWindow* pWin, CDockWindowVector* pCurWins, CDockWindowVector* pCurNeighbours)
{
	CDockWindow::TNeighbourVector::Iterator it;
	CDockWindow* pCurWin;
	bool bAny;
	bool bResult;
	bool bResult1;
	bool bResult2;
	RECT r;
	RECT r1;

	pWin->GetParentRect(&r);
	if ( pCurWins )
		pCurWins->Append(pWin);
	if ( (r.top < m_clientRect.top) || (r.bottom > m_clientRect.bottom) )
	{
		it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
		bResult1 = false;
		bResult2 = false;
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.bottom == m_clientRect.top )
			{
				if ( pCurNeighbours )
					pCurNeighbours->Append((*it)->win);
				bResult1 = TestSizableLeftTop((*it)->win, pCurNeighbours, pWin, pCurWins, m_clientRect.left);
			}
			if ( r1.top == m_clientRect.bottom )
			{
				if ( pCurNeighbours )
					pCurNeighbours->Append((*it)->win);
				bResult2 = TestSizableLeftBottom((*it)->win, pCurNeighbours, pWin, pCurWins, m_clientRect.left);
			}
			it = pWin->get_NextNeighbour(CDockWindow::TDockingStyleRight, it);
		}
		if ( (!bResult1) && (!bResult2) )
			return false;
	}
	if ( r.top >= m_clientRect.top )
	{
		bAny = false;
		bResult = false;
		pCurWin = pWin;
		it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.right == m_clientRect.left )
			{
				bAny = true;
				pCurWin = pN->win;
				if ( pCurWins )
					pCurWins->Append(pCurWin);
				if ( r1.top < m_clientRect.top )
				{
					CDockWindow::TNeighbourVector::Iterator it1 = pN->win->get_FirstNeighbour(CDockWindow::TDockingStyleRight);

					while ( it1 )
					{
						CDockWindow::TNeighbour* pN1 = *it1;

						pN1->win->GetParentRect(&r1);
						if ( r1.bottom == m_clientRect.top )
							break;
						it1 = pN->win->get_NextNeighbour(CDockWindow::TDockingStyleRight, it1);
					}
					assert(it1);
					if ( pCurNeighbours )
						pCurNeighbours->Append((*it1)->win);
					bResult = TestSizableLeftTop((*it1)->win, pCurNeighbours, pCurWin, pCurWins, m_clientRect.left);
					break;
				}
				else if ( r1.top == m_clientRect.top )
				{
					bResult = true;
					break;
				}
				it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
			}
			else
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleTop, it);
		}
		if ( bAny && (!bResult) )
			return false;
	}
	if ( r.bottom <= m_clientRect.bottom )
	{
		bAny = false;
		bResult = false;
		pCurWin = pWin;
		it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.right == m_clientRect.left )
			{
				bAny = true;
				pCurWin = pN->win;
				if ( pCurWins )
					pCurWins->Append(pCurWin);
				if ( r1.bottom > m_clientRect.bottom )
				{
					CDockWindow::TNeighbourVector::Iterator it1 = pN->win->get_FirstNeighbour(CDockWindow::TDockingStyleRight);

					while ( it1 )
					{
						CDockWindow::TNeighbour* pN1 = *it1;

						pN1->win->GetParentRect(&r1);
						if ( r1.top == m_clientRect.bottom )
							break;
						it1 = pN->win->get_NextNeighbour(CDockWindow::TDockingStyleRight, it1);
					}
					assert(it1);
					if ( pCurNeighbours )
						pCurNeighbours->Append((*it1)->win);
					bResult = TestSizableLeftBottom((*it1)->win, pCurNeighbours, pCurWin, pCurWins, m_clientRect.left);
					break;
				}
				else if ( r1.bottom == m_clientRect.bottom )
				{
					bResult = true;
					break;
				}
				it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
			}
			else
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleBottom, it);
		}
		if ( bAny && (!bResult) )
			return false;
	}
	return true;
}

bool CDockInfo::TestSizableClientTop(CDockWindow* pWin, CDockWindowVector* pCurWins, CDockWindowVector* pCurNeighbours)
{
	CDockWindow::TNeighbourVector::Iterator it;
	CDockWindow* pCurWin;
	bool bAny;
	bool bResult;
	bool bResult1;
	bool bResult2;
	RECT r;
	RECT r1;

	pWin->GetParentRect(&r);
	if ( pCurWins )
		pCurWins->Append(pWin);
	if ( (r.left < m_clientRect.left) || (r.right > m_clientRect.right) )
	{
		it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
		bResult1 = false;
		bResult2 = false;
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.right == m_clientRect.left )
			{
				if ( pCurNeighbours )
					pCurNeighbours->Append((*it)->win);
				bResult1 = TestSizableTopLeft(pWin, pCurWins, (*it)->win, pCurNeighbours, m_clientRect.bottom);
			}
			if ( r1.left == m_clientRect.right )
			{
				if ( pCurNeighbours )
					pCurNeighbours->Append((*it)->win);
				bResult2 = TestSizableTopRight(pWin, pCurWins, (*it)->win, pCurNeighbours, m_clientRect.bottom);
			}
			it = pWin->get_NextNeighbour(CDockWindow::TDockingStyleTop, it);
		}
		if ( (!bResult1) && (!bResult2) )
			return false;
	}
	if ( r.left >= m_clientRect.left )
	{
		bAny = false;
		bResult = false;
		pCurWin = pWin;
		it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.top == m_clientRect.bottom )
			{
				bAny = true;
				pCurWin = pN->win;
				if ( pCurWins )
					pCurWins->Append(pCurWin);
				if ( r1.left < m_clientRect.left )
				{
					CDockWindow::TNeighbourVector::Iterator it1 = pN->win->get_FirstNeighbour(CDockWindow::TDockingStyleTop);

					while ( it1 )
					{
						CDockWindow::TNeighbour* pN1 = *it1;

						pN1->win->GetParentRect(&r1);
						if ( r1.right == m_clientRect.left )
							break;
						it1 = pN->win->get_NextNeighbour(CDockWindow::TDockingStyleTop, it1);
					}
					assert(it1);
					if ( pCurNeighbours )
						pCurNeighbours->Append((*it1)->win);
					bResult = TestSizableTopLeft(pCurWin, pCurWins, (*it1)->win, pCurNeighbours, m_clientRect.bottom);
					break;
				}
				else if ( r1.left == m_clientRect.left )
				{
					bResult = true;
					break;
				}
				it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
			}
			else
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleLeft, it);
		}
		if ( bAny && (!bResult) )
			return false;
	}
	if ( r.right <= m_clientRect.right )
	{
		bAny = false;
		bResult = false;
		pCurWin = pWin;
		it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.top == m_clientRect.bottom )
			{
				bAny = true;
				pCurWin = pN->win;
				if ( pCurWins )
					pCurWins->Append(pCurWin);
				if ( r1.right > m_clientRect.right )
				{
					CDockWindow::TNeighbourVector::Iterator it1 = pN->win->get_FirstNeighbour(CDockWindow::TDockingStyleTop);

					while ( it1 )
					{
						CDockWindow::TNeighbour* pN1 = *it1;

						pN1->win->GetParentRect(&r1);
						if ( r1.left == m_clientRect.right )
							break;
						it1 = pN->win->get_NextNeighbour(CDockWindow::TDockingStyleTop, it1);
					}
					assert(it1);
					if ( pCurNeighbours )
						pCurNeighbours->Append((*it1)->win);
					bResult = TestSizableTopRight(pCurWin, pCurWins, (*it1)->win, pCurNeighbours, m_clientRect.bottom);
					break;
				}
				else if ( r1.right == m_clientRect.right )
				{
					bResult = true;
					break;
				}
				it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
			}
			else
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleRight, it);
		}
		if ( bAny && (!bResult) )
			return false;
	}
	return true;
}

bool CDockInfo::TestSizableClientBottom(CDockWindow* pWin, CDockWindowVector* pCurWins, CDockWindowVector* pCurNeighbours)
{
	CDockWindow::TNeighbourVector::Iterator it;
	CDockWindow* pCurWin;
	bool bAny;
	bool bResult;
	bool bResult1;
	bool bResult2;
	RECT r;
	RECT r1;

	pWin->GetParentRect(&r);
	if ( pCurWins )
		pCurWins->Append(pWin);
	if ( (r.left < m_clientRect.left) || (r.right > m_clientRect.right) )
	{
		it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
		bResult1 = false;
		bResult2 = false;
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.right == m_clientRect.left )
			{
				if ( pCurNeighbours )
					pCurNeighbours->Append((*it)->win);
				bResult1 = TestSizableTopLeft((*it)->win, pCurNeighbours, pWin, pCurWins, m_clientRect.top);
			}
			if ( r.left == m_clientRect.right )
			{
				if ( pCurNeighbours )
					pCurNeighbours->Append((*it)->win);
				bResult2 = TestSizableTopRight((*it)->win, pCurNeighbours, pWin, pCurWins, m_clientRect.top);
			}
			it = pWin->get_NextNeighbour(CDockWindow::TDockingStyleBottom, it);
		}
		if ( (!bResult1) && (!bResult2) )
			return false;
	}
	if ( r.left >= m_clientRect.left )
	{
		bAny = false;
		bResult = false;
		pCurWin = pWin;
		it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.bottom == m_clientRect.top )
			{
				bAny = true;
				pCurWin = pN->win;
				if ( pCurWins )
					pCurWins->Append(pCurWin);
				if ( r1.left < m_clientRect.left )
				{
					CDockWindow::TNeighbourVector::Iterator it1 = pN->win->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);

					while ( it1 )
					{
						CDockWindow::TNeighbour* pN1 = *it1;

						pN1->win->GetParentRect(&r1);
						if ( r1.right == m_clientRect.left )
							break;
						it1 = pN->win->get_NextNeighbour(CDockWindow::TDockingStyleBottom, it1);
					}
					assert(it1);
					if ( pCurNeighbours )
						pCurNeighbours->Append((*it1)->win);
					bResult = TestSizableTopLeft((*it1)->win, pCurNeighbours, pCurWin, pCurWins, m_clientRect.top);
					break;
				}
				else if ( r1.left == m_clientRect.left )
				{
					bResult = true;
					break;
				}
				it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
			}
			else
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleLeft, it);
		}
		if ( bAny && (!bResult) )
			return false;
	}
	if ( r.right <= m_clientRect.right )
	{
		bAny = false;
		bResult = false;
		pCurWin = pWin;
		it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
		while ( it )
		{
			CDockWindow::TNeighbour* pN = *it;

			pN->win->GetParentRect(&r1);
			if ( r1.bottom == m_clientRect.top )
			{
				bAny = true;
				pCurWin = pN->win;
				if ( pCurWins )
					pCurWins->Append(pCurWin);
				if ( r1.right > m_clientRect.right )
				{
					CDockWindow::TNeighbourVector::Iterator it1 = pN->win->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);

					while ( it1 )
					{
						CDockWindow::TNeighbour* pN1 = *it1;

						pN1->win->GetParentRect(&r1);
						if ( r1.left == m_clientRect.right )
							break;
						it1 = pN->win->get_NextNeighbour(CDockWindow::TDockingStyleBottom, it1);
					}
					assert(it1);
					if ( pCurNeighbours )
						pCurNeighbours->Append((*it1)->win);
					bResult = TestSizableTopRight((*it1)->win, pCurNeighbours, pCurWin, pCurWins, m_clientRect.top);
					break;
				}
				else if ( r1.right == m_clientRect.right )
				{
					bResult = true;
					break;
				}
				it = pCurWin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
			}
			else
				it = pCurWin->get_NextNeighbour(CDockWindow::TDockingStyleRight, it);
		}
		if ( bAny && (!bResult) )
			return false;
	}
	return true;
}

bool CDockInfo::TestSizable(CDockWindow* pWin, INT edge)
{
	RECT r;
	RECT r1;

	pWin->GetParentRect(&r);
	switch ( edge )
	{
	case HTLEFT:
		if ( r.left == m_windowRect.left )
			return false;
		if ( r.left == m_clientRect.right )
			return TestSizableClientLeft(pWin, nullptr, nullptr);
		{
			CDockWindow::TNeighbourVector::Iterator it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
			CDockWindow* pLeftNeighbour = nullptr;
			bool bResultTop0 = false;
			bool bResultBottom0 = false;
			bool bResultTop1 = false;
			bool bResultBottom1 = false;

			if ( it )
				pLeftNeighbour = (*it)->win;
			else
				return false;
			pLeftNeighbour->GetParentRect(&r1);
			if ( (r.top == r1.top) && (r.bottom == r1.bottom) )
			{
				bResultTop0 = true;
				bResultBottom0 = true;
			}
			bResultTop1 = TestSizableLeftTop(pWin, nullptr, pLeftNeighbour, nullptr, r.left);
			bResultBottom1 = TestSizableLeftBottom(pWin, nullptr, pLeftNeighbour, nullptr, r.left);
			return (bResultTop0 && bResultBottom0) || (bResultTop1 && bResultBottom1);
		}
		break;
	case HTRIGHT:
		if ( r.right == m_windowRect.right )
			return false;
		if ( r.right == m_clientRect.left )
			return TestSizableClientRight(pWin, nullptr, nullptr);
		{
			CDockWindow::TNeighbourVector::Iterator it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
			CDockWindow* pRightNeighbour = nullptr;
			bool bResultTop0 = false;
			bool bResultBottom0 = false;
			bool bResultTop1 = false;
			bool bResultBottom1 = false;

			if ( it )
				pRightNeighbour = (*it)->win;
			else
				return false;
			pRightNeighbour->GetParentRect(&r1);
			if ( (r.top == r1.top) && (r.bottom == r1.bottom) )
			{
				bResultTop0 = true;
				bResultBottom0 = true;
			}
			bResultTop1 = TestSizableLeftTop(pRightNeighbour, nullptr, pWin, nullptr, r.right);
			bResultBottom1 = TestSizableLeftBottom(pRightNeighbour, nullptr, pWin, nullptr, r.right);
			return (bResultTop0 && bResultBottom0) || (bResultTop1 && bResultBottom1);
		}
		break;
	case HTTOP:
		if ( r.top == m_windowRect.top )
			return false;
		if ( r.top == m_clientRect.bottom )
			return TestSizableClientTop(pWin, nullptr, nullptr);
		{
			CDockWindow::TNeighbourVector::Iterator it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
			CDockWindow* pTopNeighbour = nullptr;
			bool bResultLeft0 = false;
			bool bResultRight0 = false;
			bool bResultLeft1 = false;
			bool bResultRight1 = false;

			if ( it )
				pTopNeighbour = (*it)->win;
			else
				return false;
			pTopNeighbour->GetParentRect(&r1);
			if ( (r.left == r1.left) && (r.right == r1.right) )
			{
				bResultLeft0 = true;
				bResultRight0 = true;
			}
			bResultLeft1 = TestSizableTopLeft(pWin, nullptr, pTopNeighbour, nullptr, r.top);
			bResultRight1 = TestSizableTopRight(pWin, nullptr, pTopNeighbour, nullptr, r.top);
			return (bResultLeft0 && bResultRight0) || (bResultLeft1 && bResultRight1);
		}
		break;
	case HTTOPLEFT:
		break;
	case HTTOPRIGHT:
		break;
	case HTBOTTOM:
		if ( r.bottom == m_windowRect.bottom )
			return false;
		if ( r.bottom == m_clientRect.top )
			return TestSizableClientBottom(pWin, nullptr, nullptr);
		{
			CDockWindow::TNeighbourVector::Iterator it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
			CDockWindow* pBottomNeighbour = nullptr;
			bool bResultLeft0 = false;
			bool bResultRight0 = false;
			bool bResultLeft1 = false;
			bool bResultRight1 = false;

			if ( it )
				pBottomNeighbour = (*it)->win;
			else
				return false;
			pBottomNeighbour->GetParentRect(&r1);
			if ( (r.left == r1.left) && (r.right == r1.right) )
			{
				bResultLeft0 = true;
				bResultRight0 = true;
			}
			bResultLeft1 = TestSizableTopLeft(pBottomNeighbour, nullptr, pWin, nullptr, r.bottom);
			bResultRight1 = TestSizableTopRight(pBottomNeighbour, nullptr, pWin, nullptr, r.bottom);
			return (bResultLeft0 && bResultRight0) || (bResultLeft1 && bResultRight1);
		}
		break;
	case HTBOTTOMLEFT:
		break;
	case HTBOTTOMRIGHT:
		break;
	}
	return 0;
}

bool CDockInfo::Resize(CDockWindow* pWin, INT edge, POINT ptDelta)
{
	CDockWindowVector resizeCurWins(__FILE__LINE__ 16, 16);
	CDockWindowVector resizeCurNeighbours(__FILE__LINE__ 16, 16);
	RECT r;
	RECT r1;
	bool bUpdateClientRect = false;
	bool bResult = false;

	pWin->GetParentRect(&r);
	switch ( edge )
	{
	case HTLEFT:
		if ( r.left == m_windowRect.left )
			break;
		if ( r.left == m_clientRect.right )
		{
			bool bResult = TestSizableClientLeft(pWin, &resizeCurWins, &resizeCurNeighbours);

			if ( !bResult )
				break;
			bUpdateClientRect = true;
		}
		else
		{
			CDockWindow::TNeighbourVector::Iterator it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleLeft);
			CDockWindow* pLeftNeighbour = nullptr;
			bool bResultTop0 = false;
			bool bResultBottom0 = false;
			bool bResultTop1 = false;
			bool bResultBottom1 = false;

			if ( it )
				pLeftNeighbour = (*it)->win;
			else
				break;
			resizeCurWins.Append(pWin);
			resizeCurNeighbours.Append(pLeftNeighbour);
			pLeftNeighbour->GetParentRect(&r1);
			if ( (r.top == r1.top) && (r.bottom == r1.bottom) )
			{
				bResultTop0 = true;
				bResultBottom0 = true;
			}
			resizeCurWins.StartTransact();
			resizeCurNeighbours.StartTransact();
			bResultTop1 = TestSizableLeftTop(pWin, &resizeCurWins, pLeftNeighbour, &resizeCurNeighbours, r.left);
			if ( bResultTop1 )
				bResultBottom1 = TestSizableLeftBottom(pWin, &resizeCurWins, pLeftNeighbour, &resizeCurNeighbours, r.left);
			if ( (!bResultTop1) || (!bResultBottom1) )
			{
				resizeCurWins.Rollback();
				resizeCurNeighbours.Rollback();
				if ( (!bResultTop0) || (!bResultBottom0) )
					break;
			}
			else
			{
				resizeCurWins.Commit();
				resizeCurNeighbours.Commit();
			}
		}
		if ( resizeCurWins.CheckSize(CDockWindow::TDockingStyleLeft, false, ptDelta)
			&& resizeCurNeighbours.CheckSize(CDockWindow::TDockingStyleLeft, true, ptDelta) )
		{
			resizeCurWins.Resize(CDockWindow::TDockingStyleLeft, false, ptDelta, m_clientRect, &bUpdateClientRect);
			resizeCurNeighbours.Resize(CDockWindow::TDockingStyleLeft, true, ptDelta, m_clientRect, &bUpdateClientRect);
			if ( bUpdateClientRect )
			{
				m_clientRect.right += ptDelta.x;
				m_pFrame->SendMessage(WM_SIZE, 0, 0);
			}
			bResult = true;
		}
		break;
	case HTRIGHT:
		if ( r.right == m_windowRect.right )
			break;
		if ( r.right == m_clientRect.left )
		{
			bool bResult = TestSizableClientRight(pWin, &resizeCurWins, &resizeCurNeighbours);

			if ( !bResult )
				break;
			bUpdateClientRect = true;
		}
		else
		{
			CDockWindow::TNeighbourVector::Iterator it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleRight);
			CDockWindow* pRightNeighbour = nullptr;
			bool bResultTop0 = false;
			bool bResultBottom0 = false;
			bool bResultTop1 = false;
			bool bResultBottom1 = false;

			if ( it )
				pRightNeighbour = (*it)->win;
			else
				break;
			resizeCurWins.Append(pWin);
			resizeCurNeighbours.Append(pRightNeighbour);
			pRightNeighbour->GetParentRect(&r1);
			if ( (r.top == r1.top) && (r.bottom == r1.bottom) )
			{
				bResultTop0 = true;
				bResultBottom0 = true;
			}
			resizeCurWins.StartTransact();
			resizeCurNeighbours.StartTransact();
			bResultTop1 = TestSizableLeftTop(pRightNeighbour, &resizeCurNeighbours, pWin, &resizeCurWins, r.right);
			if ( bResultTop1 )
				bResultBottom1 = TestSizableLeftBottom(pRightNeighbour, &resizeCurNeighbours, pWin, &resizeCurWins, r.right);
			if ( (!bResultTop1) || (!bResultBottom1) )
			{
				resizeCurWins.Rollback();
				resizeCurNeighbours.Rollback();
				if ( (!bResultTop0) || (!bResultBottom0) )
					break;
			}
			else
			{
				resizeCurWins.Commit();
				resizeCurNeighbours.Commit();
			}
		}
		if ( resizeCurWins.CheckSize(CDockWindow::TDockingStyleRight, false, ptDelta)
			&& resizeCurNeighbours.CheckSize(CDockWindow::TDockingStyleRight, true, ptDelta) )
		{
			resizeCurWins.Resize(CDockWindow::TDockingStyleRight, false, ptDelta, m_clientRect, &bUpdateClientRect);
			resizeCurNeighbours.Resize(CDockWindow::TDockingStyleRight, true, ptDelta, m_clientRect, &bUpdateClientRect);
			if ( bUpdateClientRect )
			{
				m_clientRect.left += ptDelta.x;
				m_pFrame->SendMessage(WM_SIZE, 0, 0);
			}
			bResult = true;
		}
		break;
	case HTTOP:
		if ( r.top == m_windowRect.top )
			break;
		if ( r.top == m_clientRect.bottom )
		{
			bool bResult = TestSizableClientTop(pWin, &resizeCurWins, &resizeCurNeighbours);

			if ( !bResult )
				break;
			bUpdateClientRect = true;
		}
		else
		{
			CDockWindow::TNeighbourVector::Iterator it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleTop);
			CDockWindow* pTopNeighbour = nullptr;
			bool bResultLeft0 = false;
			bool bResultRight0 = false;
			bool bResultLeft1 = false;
			bool bResultRight1 = false;

			if ( it )
				pTopNeighbour = (*it)->win;
			else
				break;
			resizeCurWins.Append(pWin);
			resizeCurNeighbours.Append(pTopNeighbour);
			pTopNeighbour->GetParentRect(&r1);
			if ( (r.left == r1.left) && (r.right == r1.right) )
			{
				bResultLeft0 = true;
				bResultRight0 = true;
			}
			resizeCurWins.StartTransact();
			resizeCurNeighbours.StartTransact();
			bResultLeft1 = TestSizableTopLeft(pWin, &resizeCurWins, pTopNeighbour, &resizeCurNeighbours, r.top);
			if ( bResultLeft1 )
				bResultRight1 = TestSizableTopRight(pWin, &resizeCurWins, pTopNeighbour, &resizeCurNeighbours, r.top);
			if ( (!bResultLeft1) || (!bResultRight1) )
			{
				resizeCurWins.Rollback();
				resizeCurNeighbours.Rollback();
				if ( (!bResultLeft0) || (!bResultRight0) )
					break;
			}
			else
			{
				resizeCurWins.Commit();
				resizeCurNeighbours.Commit();
			}
		}
		if ( resizeCurWins.CheckSize(CDockWindow::TDockingStyleTop, false, ptDelta)
			&& resizeCurNeighbours.CheckSize(CDockWindow::TDockingStyleTop, true, ptDelta) )
		{
			resizeCurWins.Resize(CDockWindow::TDockingStyleTop, false, ptDelta, m_clientRect, &bUpdateClientRect);
			resizeCurNeighbours.Resize(CDockWindow::TDockingStyleTop, true, ptDelta, m_clientRect, &bUpdateClientRect);
			if ( bUpdateClientRect )
			{
				m_clientRect.bottom += ptDelta.y;
				m_pFrame->SendMessage(WM_SIZE, 0, 0);
			}
			bResult = true;
		}
		break;
	case HTTOPLEFT:
		break;
	case HTTOPRIGHT:
		break;
	case HTBOTTOM:
		if ( r.bottom == m_windowRect.bottom )
			break;
		if ( r.bottom == m_clientRect.top )
		{
			bool bResult = TestSizableClientBottom(pWin, &resizeCurWins, &resizeCurNeighbours);

			if ( !bResult )
				break;
			bUpdateClientRect = true;
		}
		else
		{
			CDockWindow::TNeighbourVector::Iterator it = pWin->get_FirstNeighbour(CDockWindow::TDockingStyleBottom);
			CDockWindow* pBottomNeighbour = nullptr;
			bool bResultLeft0 = false;
			bool bResultRight0 = false;
			bool bResultLeft1 = false;
			bool bResultRight1 = false;

			if ( it )
				pBottomNeighbour = (*it)->win;
			else
				break;
			resizeCurWins.Append(pWin);
			resizeCurNeighbours.Append(pBottomNeighbour);
			pBottomNeighbour->GetParentRect(&r1);
			if ( (r.left == r1.left) && (r.right == r1.right) )
			{
				bResultLeft0 = true;
				bResultRight0 = true;
			}
			resizeCurWins.StartTransact();
			resizeCurNeighbours.StartTransact();
			bResultLeft1 = TestSizableTopLeft(pBottomNeighbour, &resizeCurNeighbours, pWin, &resizeCurWins, r.bottom);
			if ( bResultLeft1 )
				bResultRight1 = TestSizableTopRight(pBottomNeighbour, &resizeCurNeighbours, pWin, &resizeCurWins, r.bottom);
			if ( (!bResultLeft1) || (!bResultRight1) )
			{
				resizeCurWins.Rollback();
				resizeCurNeighbours.Rollback();
				if ( (!bResultLeft0) || (!bResultRight0) )
					break;
			}
			else
			{
				resizeCurWins.Commit();
				resizeCurNeighbours.Commit();
			}
		}
		if ( resizeCurWins.CheckSize(CDockWindow::TDockingStyleBottom, false, ptDelta)
			&& resizeCurNeighbours.CheckSize(CDockWindow::TDockingStyleBottom, true, ptDelta) )
		{
			resizeCurWins.Resize(CDockWindow::TDockingStyleBottom, false, ptDelta, m_clientRect, &bUpdateClientRect);
			resizeCurNeighbours.Resize(CDockWindow::TDockingStyleBottom, true, ptDelta, m_clientRect, &bUpdateClientRect);
			if ( bUpdateClientRect )
			{
				m_clientRect.top += ptDelta.y;
				m_pFrame->SendMessage(WM_SIZE, 0, 0);
			}
			bResult = true;
		}
		break;
	case HTBOTTOMLEFT:
		break;
	case HTBOTTOMRIGHT:
		break;
	}
	return bResult;
}