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
#include "afxwin.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "Docking.h"

#define SHIFTED 0x8000
//#define __DEBUG1__

static INT_PTR CALLBACK DialogWndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWin* win = nullptr;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		win = (CWin*) lParam;
		win->set_handle(hwndDlg);
		assert(::GetWindowLongPtr(hwndDlg, GWLP_USERDATA) == NULL);
		::SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG_PTR)win);
		break;
	default:
		win = (CWin*)(::GetWindowLongPtr(hwndDlg, GWLP_USERDATA));
		break;
	}

	if ( !win )
		return 0;

	switch ( uMsg )
	{
	case WM_COMMAND:
		return win->OnCommandMessage(wParam, lParam);
	case WM_NOTIFY:
		return win->OnNotifyMessage(wParam, lParam);
	default:
		return win->OnWindowMessage(uMsg, wParam, lParam);
	}
	return 0;
}

static LRESULT CALLBACK CommonWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CWin* win = nullptr;

	switch (message)
	{
	case WM_NCCREATE:
		win = (CWin*) (((LPCREATESTRUCT) lParam)->lpCreateParams);
		if ( !win )
		{
			win = (CWin*)CDialog::GetDialogControl(((LPCREATESTRUCT) lParam)->lpszClass);
			if ( win )
				win = win->Clone(((LPCREATESTRUCT) lParam)->style, ((LPCREATESTRUCT) lParam)->dwExStyle);
		}
		if ( !win )
			break;
		win->set_handle(hWnd);
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)win);
		break;
	default:
		win = (CWin*)(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		break;
	}

	if ( !win )
		return 0;

	switch ( message )
	{
	case WM_COMMAND:
		return win->OnCommandMessage(wParam, lParam);
	case WM_NOTIFY:
		return win->OnNotifyMessage(wParam, lParam);
	default:
		return win->OnWindowMessage(message, wParam, lParam);
	}
	return 0;
}

static LRESULT CALLBACK MDIChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CWin* win = nullptr;

	switch (message)
	{
	case WM_NCCREATE:
		{
			LPCREATESTRUCT lpCS = (LPCREATESTRUCT) lParam;
			LPMDICREATESTRUCT lpMDICS = (LPMDICREATESTRUCT)(lpCS->lpCreateParams);

			win = (CWin*) (lpMDICS->lParam);
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)win);

			if ( win )
				win->set_handle(hWnd);
		}
		break;
	default:
		win = (CWin*)(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		break;
	}

	if ( !win )
		return ::DefMDIChildProc(hWnd, message, wParam, lParam);

	switch ( message )
	{
	case WM_COMMAND:
		return win->OnCommandMessage(wParam, lParam);
	case WM_NOTIFY:
		return win->OnNotifyMessage(wParam, lParam);
	default:
		return win->OnWindowMessage(message, wParam, lParam);
	}
	return 0;
}

static WNDPROC prevMDIClientWndProc = nullptr;

static LRESULT CALLBACK MDIClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_WINDOWPOSCHANGING:
		{
			LPWINDOWPOS pWinpos = (LPWINDOWPOS)lParam;
			CMDIFrame* pFrame = (CMDIFrame*)(::GetWindowLongPtr(::GetParent(hWnd), GWLP_USERDATA));

			if ( pFrame->is_DockingEnabled() )
			{
				CDockInfo* pDockInfo = pFrame->get_dockinfo();
				RECT r;

				pDockInfo->get_clientrect(&r);
				pWinpos->x = r.left;
				pWinpos->cx = r.right - r.left;
				pWinpos->y = r.top;
				pWinpos->cy = r.bottom - r.top;
			}
			else
			{
				if ( CMenubar* pMenubar = pFrame->get_MenuBar() )
				{
					RECT r;

					pMenubar->GetWindowRect(&r);
					pWinpos->y += r.bottom - r.top;
					pWinpos->cy -= r.bottom - r.top;
				}
				if ( pFrame->is_ToolBarEnabled() )
				{
					CToolbar* pToolbar = pFrame->get_ToolBar();
					RECT r;

					pToolbar->GetWindowRect(&r);
					pWinpos->y += r.bottom - r.top;
					pWinpos->cy -= r.bottom - r.top;
				}
				if ( pFrame->is_StatusBarEnabled() )
				{
					CStatusbar* pStatusbar = pFrame->get_StatusBar();
					RECT r;

					pStatusbar->GetWindowRect(&r);
					pWinpos->cy -= r.bottom - r.top;
				}
			}
		}
		break;
	case WM_WINDOWPOSCHANGED:
		break;
	case WM_MOVE:
		break;
	case WM_SIZE:
		::CallWindowProc(prevMDIClientWndProc, hWnd, WM_MDIICONARRANGE, 0, 0);
		break;
	case WM_NCCALCSIZE:
		break;
	default:
		break;
	}
	return ::CallWindowProc(prevMDIClientWndProc, hWnd, message, wParam, lParam);
}

//***********************************************************
// CWin
//***********************************************************
AFX_MSGMAP* CWin::GetMessageMap() { return &_messagemap; }
AFX_MSGMAP CWin::_messagemap = { nullptr, &_message_entries[0] };

AFX_MSGMAP_ENTRY CWin::_message_entries[] = {
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCDESTROY()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_SYSCOMMAND()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

CWin::CWin(TWindowType wndwtype, LPCTSTR name): 
	m_hwnd(nullptr), 
	m_wndwtype(wndwtype), 
	m_name(__FILE__LINE__ name),
	m_createvisible(TRUE),
	m_trackMouse(FALSE)
{}

CWin::CWin(TWindowType wndwtype, ConstRef(CStringBuffer) name): 
	m_hwnd(nullptr), 
	m_wndwtype(wndwtype), 
	m_name(name),
	m_createvisible(TRUE),
	m_trackMouse(FALSE)
{}

CWin::~CWin()
{}

BOOL CWin::PreRegisterClass(WNDCLASSEX& cls)
{
	return TRUE;
}

BOOL CWin::PreCreate(CREATESTRUCT& cs)
{
	return TRUE;
}

CWin* CWin::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CWin(TAnyWindow);

	return pWin;
}

BOOL CWin::RegisterClass(WNDCLASSEX& wcex)
{
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= &CommonWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= theGuiApp->get_GuiInstance();
	wcex.lpszClassName	= _T("CWINDOWS");
	wcex.hIcon			= nullptr;
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr;
	wcex.hIconSm		= nullptr;

	if ( !PreRegisterClass(wcex) )
		return FALSE;

	WNDCLASSEX wcex1;

	::SecureZeroMemory(&wcex1, sizeof(WNDCLASSEX));
	wcex1.cbSize = sizeof(WNDCLASSEX);
	if ( !(::GetClassInfoEx(theGuiApp->get_GuiInstance(), wcex.lpszClassName, &wcex1)) )
	{
		if ( !(::RegisterClassEx(&wcex)) )
			return FALSE;
	}
	return TRUE;
}

BOOL CWin::Create(HWND parent, UINT childId)
{
	WNDCLASSEX wcex;

	if ( !RegisterClass(wcex) )
		return FALSE;

	CREATESTRUCT cs;

	cs.lpCreateParams = this;
	cs.hInstance = wcex.hInstance;
	cs.hMenu = (HMENU)childId;
	cs.hwndParent = parent;
	cs.cy = (parent == nullptr)?CW_USEDEFAULT:10;
	cs.cx = (parent == nullptr)?CW_USEDEFAULT:10;
	cs.y = (parent == nullptr)?CW_USEDEFAULT:0;
	cs.x = (parent == nullptr)?CW_USEDEFAULT:0;
	cs.style = (parent == nullptr)?(WS_OVERLAPPEDWINDOW):(WS_CHILD);
	cs.lpszName = nullptr;
	cs.lpszClass = wcex.lpszClassName;
	cs.dwExStyle = 0;

	if ( !PreCreate(cs) )
		return FALSE;

	HWND hwnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, 
		cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	if ( !hwnd )
	{
		DWORD err = ::GetLastError();

		return FALSE;
	}

	if ( (cs.style & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW )
		::ShowWindow(m_hwnd, theGuiApp->get_CmdShow());
    else if ( is_CreateVisible() )
		set_Visible(TRUE);
	if ( CastDynamicPtr(CControl, this) )
		CastDynamicPtr(CControl, this)->Update(TRUE);
	else
		InvalidateRect(nullptr, FALSE);
	if ( cs.style & WS_VSCROLL )
		::ShowScrollBar(m_hwnd, SB_VERT, FALSE);
	if ( cs.style & WS_HSCROLL )
		::ShowScrollBar(m_hwnd, SB_HORZ, FALSE);

	return TRUE;
}

CStringBuffer CWin::get_windowtext()
{
	int wlen = ::GetWindowTextLength(m_hwnd);

	CStringBuffer result(__FILE__LINE__ wlen);

	if ( wlen ) 
		::GetWindowText(m_hwnd, CastMutable(CPointer, result.GetString()), wlen + 1);
	return result;
}

BOOL CWin::set_windowtext(ConstRef(CStringBuffer) text)
{
	return ::SetWindowText(m_hwnd, text.GetString());
}

CWin* CWin::get_parent()
{
	HWND p = ::GetParent(m_hwnd);

	if ( !p ) 
	{
		switch ( m_wndwtype )
		{
		case TDockWindow:
			if ( CDockWindow* pWin = CastDynamicPtr(CDockWindow, this) )
			{
				if ( pWin->is_Floating() )
					return theGuiApp->get_MainWnd();
			}
			break;
		case TPopupWindow:
			return theGuiApp->get_MainWnd();
		default:
			break;
		}
		return nullptr;
	}
	if ( m_wndwtype == TMDIChildWindow )
	{
		p = ::GetParent(p);
		if ( !p ) 
			return nullptr;
	}
	return CastAnyPtr(CWin, ::GetWindowLongPtr(p, GWLP_USERDATA));
}

CWin* CWin::set_parent(CWin* pParent)
{
	HWND prev = ::SetParent(m_hwnd, pParent?(pParent->get_handle()):nullptr);

	if ( !prev )
		return nullptr;
	return CastAnyPtr(CWin, ::GetWindowLongPtr(prev, GWLP_USERDATA));
}

CWin* CWin::get_frame()
{
	HWND p = ::GetParent(m_hwnd);
	CWin* result;

	if ( !p ) 
	{
		switch ( m_wndwtype )
		{
		case TFrameWindow:
		case TMDIWindow:
		case TDialogWindow:
			return this;
		case TDockWindow:
			if ( CDockWindow* pWin = CastDynamicPtr(CDockWindow, this) )
			{
				if ( pWin->is_Floating() )
					return theGuiApp->get_MainWnd();
			}
			break;
		case TPopupWindow:
			return theGuiApp->get_MainWnd();
		default:
			break;
		}
		return nullptr;
	}
	if ( m_wndwtype == TMDIChildWindow )
		p = ::GetParent(p);
	while ( p )
	{
		result = CastAnyPtr(CWin, ::GetWindowLongPtr(p, GWLP_USERDATA));
		switch ( result->m_wndwtype )
		{
		case TFrameWindow:
		case TMDIWindow:
		case TDialogWindow:
			return result;
		case TMDIChildWindow:
			p = ::GetParent(p);
			break;
		case TDockWindow:
			if ( CDockWindow* pWin = CastDynamicPtr(CDockWindow, this) )
			{
				if ( pWin->is_Floating() )
					return theGuiApp->get_MainWnd();
			}
			break;
		case TPopupWindow:
			return theGuiApp->get_MainWnd();
		default:
			break;
		}
		p = ::GetParent(p);
	}
	return nullptr;
}

CWin* CWin::get_mdiChild()
{
	HWND p;
	CWin* result;

	if ( m_wndwtype == TMDIChildWindow )
		return this;
	p = ::GetParent(m_hwnd);
	while ( p )
	{
		result = CastAnyPtr(CWin, ::GetWindowLongPtr(p, GWLP_USERDATA));
		switch ( result->m_wndwtype )
		{
		case TMDIChildWindow:
			return result;
			break;
		default:
			break;
		}
		p = ::GetParent(p);
	}
	return nullptr;

}

CWin* CWin::get_childbyID(UINT commandcode)
{
	HWND hChild = ::GetDlgItem(m_hwnd, commandcode);

	if ( !hChild )
		return nullptr;
	return CastAnyPtr(CWin, ::GetWindowLongPtr(hChild, GWLP_USERDATA));
}

CWin* CWin::get_firstchild()
{
	HWND hChild = ::GetWindow(m_hwnd, GW_CHILD);

	if ( !hChild )
		return nullptr;
	return CastAnyPtr(CWin, ::GetWindowLongPtr(hChild, GWLP_USERDATA));
}

CWin* CWin::get_siblingByCmd(UINT uCmd)
{
	HWND hChild = ::GetWindow(m_hwnd, uCmd);

	if ( !hChild )
		return nullptr;
	return CastAnyPtr(CWin, ::GetWindowLongPtr(hChild, GWLP_USERDATA));
}

CWin* CWin::get_childFromPt(POINT pt)
{
	ScreenToClient(&pt);

	HWND hChild = ::ChildWindowFromPoint(m_hwnd, pt);

	if ( !hChild )
		return nullptr;
	if ( hChild == m_hwnd )
		return nullptr;
	return CastAnyPtr(CWin, ::GetWindowLongPtr(hChild, GWLP_USERDATA));
}

bool CWin::set_focus() 
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls Before set_focus %d\n", get_name().GetString(), debugID);
#endif
	if ( (!is_Visible()) || (!is_Enabled()) )
	{
#ifdef __DEBUG1__
		theGuiApp->DebugString("%ls After set_focus %d, (!is_Visible()) || (!is_Enabled())\n", get_name().GetString(), debugID);
#endif
		return false;
	}

	::SetLastError(0);

	HWND prev = ::SetFocus(m_hwnd);

	if ( (!prev) && (::GetLastError() != 0) )
	{
#ifdef __DEBUG1__
		theGuiApp->DebugString("%ls After set_focus %d, GetLastError()=%d\n", get_name().GetString(), debugID, ::GetLastError());
#endif
		return false;
	}
#ifdef __DEBUG1__
	if ( prev )
	{
		CWin* pWin = (CWin*)(::GetWindowLongPtr(prev, GWLP_USERDATA));

		if ( pWin )
			theGuiApp->DebugString("%ls set_focus %d, prev=%ls\n", get_name().GetString(), debugID, pWin->get_name().GetString());
		else
			theGuiApp->DebugString("%ls set_focus %d, prev=%x\n", get_name().GetString(), debugID, prev);
	}
#endif

	CWin* p = this;
	CWin* frame = nullptr;

	while ( p )
	{
		frame = p;
		if ( CastDynamicPtr(CToolbar, p) || CastDynamicPtr(CStatusbar, p) || CastDynamicPtr(CMenubar, p) || CastDynamicPtr(CPopup, p) || CastDynamicPtr(CDockWindow, p) )
		{
#ifdef __DEBUG1__
			theGuiApp->DebugString("%ls After set_focus %d, p=%ls\n", get_name().GetString(), debugID, p->get_name().GetString());
#endif
			return false;
		}
		p = p->get_parent();
	}
	assert(frame != nullptr);
	switch ( frame->get_windowtype() )
	{
	case TFrameWindow:
		{
			CFrameWnd* fwin = CastDynamicPtr(CFrameWnd, frame);

			assert(fwin !=  nullptr);
			fwin->set_focuswindow(this);
		}
		break;
	case TMDIWindow:
		{
			CMDIChild* cwin = CastDynamicPtr(CMDIChild, get_mdiChild());

			assert(cwin !=  nullptr);
			cwin->set_focuswindow(this);
		}
		break;
	default:
		break;
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls After set_focus %d\n", get_name().GetString(), debugID);
#endif
	return true;
}

CWin* CWin::get_focus()
{
	HWND f = ::GetFocus();

	if ( !f )
		return nullptr;
	return CastAnyPtr(CWin, ::GetWindowLongPtr(f, GWLP_USERDATA));
}

bool CWin::has_focus()
{
	HWND f = ::GetFocus();

	if ( !f )
		return false;
	return (m_hwnd == f);
}

CStringBuffer CWin::get_classname()
{
	CStringBuffer result(__FILE__LINE__ 256);

	if ( !(::GetClassName(m_hwnd, CastMutable(CPointer, (result.GetString())), 256)) )
		result.Clear();
	return result;
}

HICON CWin::get_classicon()
{
	return (HICON)(::GetClassLongPtr(m_hwnd, GCLP_HICON));
}

HICON CWin::get_classiconsm()
{
	return (HICON)(::GetClassLongPtr(m_hwnd, GCLP_HICONSM));
}

typedef struct _tagEnumChildProcCWinsetVisibleParam
{
	HWND parent;
	UINT flags;
} TEnumChildProcCWinsetVisibleParam;

static BOOL CALLBACK EnumChildProc_CWin_set_Visible(HWND hwnd, LPARAM lParam)
{
	TEnumChildProcCWinsetVisibleParam* pParam = (TEnumChildProcCWinsetVisibleParam*)lParam;

	if ( ::GetParent(hwnd) == pParam->parent )
		::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, pParam->flags);
	return TRUE;
}

BOOL CWin::set_Visible(BOOL visible, BOOL updateChildren)
{
	if ( !m_hwnd )
		return FALSE;

	UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER;

	if ( visible )
	{
		flags |= SWP_SHOWWINDOW;

		if ( !(::SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, flags)) )
			return FALSE;

		if ( updateChildren )
		{
			TEnumChildProcCWinsetVisibleParam param = { m_hwnd, flags };

			::EnumChildWindows(m_hwnd, EnumChildProc_CWin_set_Visible, (LPARAM)(&param));
		}
	}
	else
	{
		flags |= SWP_HIDEWINDOW;

		if ( updateChildren )
		{
			TEnumChildProcCWinsetVisibleParam param = { m_hwnd, flags };

			::EnumChildWindows(m_hwnd, EnumChildProc_CWin_set_Visible, (LPARAM)(&param));
		}

		if ( !(::SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, flags)) )
			return FALSE;
	}
	return TRUE;
}

void CWin::set_Capture(BOOL bEnable)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls set_Capture %d, bEnable=%d\n", get_name().GetString(), debugID, bEnable);
#endif
	if ( bEnable ) ::SetCapture(m_hwnd); else ::ReleaseCapture(); 
}


BOOL CWin::StartTrackMouse()
{
	TRACKMOUSEEVENT ev;

	ev.cbSize = sizeof(TRACKMOUSEEVENT);
	ev.dwFlags = TME_HOVER | TME_LEAVE;
	ev.hwndTrack = m_hwnd;
	ev.dwHoverTime = HOVER_DEFAULT;
	if ( ::TrackMouseEvent(&ev) )
	{
		m_trackMouse = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CWin::StopTrackMouse()
{
	TRACKMOUSEEVENT ev;

	ev.cbSize = sizeof(TRACKMOUSEEVENT);
	ev.dwFlags = TME_HOVER | TME_LEAVE | TME_CANCEL;
	ev.hwndTrack = m_hwnd;
	ev.dwHoverTime = HOVER_DEFAULT;
	if ( ::TrackMouseEvent(&ev) )
	{
		m_trackMouse = FALSE;
		return TRUE;
	}
	return FALSE;
}

Gdiplus::Font* CWin::get_Font(CStringLiteral _suffix, CStringLiteral _wndwclass)
{
	CStringBuffer name(get_name());

	name.AppendString(_suffix);
	if ( theGuiApp->get_FontManager()->has_Font(name) )
		return theGuiApp->get_FontManager()->get_Font(name);
	name.SetString(__FILE__LINE__ _wndwclass);
	name.AppendString(_suffix);
	if ( theGuiApp->get_FontManager()->has_Font(name) )
		return theGuiApp->get_FontManager()->get_Font(name);
	return theGuiApp->get_FontManager()->get_Font(nullptr);
}

Gdiplus::Brush* CWin::get_Brush(CStringLiteral _suffix, CStringLiteral _wndwclass, ConstRef(Gdiplus::Color) _default)
{
	CStringBuffer name(get_name());

	name.AppendString(_suffix);
	if ( theGuiApp->get_BrushManager()->has_Brush(name) )
		return theGuiApp->get_BrushManager()->get_Brush(name, _default);
	name.SetString(__FILE__LINE__ _wndwclass);
	name.AppendString(_suffix);
	if ( theGuiApp->get_BrushManager()->has_Brush(name) )
		return theGuiApp->get_BrushManager()->get_Brush(name, _default);
	name.SetString(__FILE__LINE__ _T("Colors"));
	name.AppendString(_suffix);
	return theGuiApp->get_BrushManager()->get_Brush(name, _default);
}

void CWin::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
}

void CWin::add_BrushKey(Ref(CDataVectorT<CStringBuffer>) _keys, ConstRef(CStringBuffer) _key)
{
	CDataVectorT<CStringBuffer>::Iterator it = _keys.FindSorted(_key);

	if ( !(it && (*it) && (_key.Compare(*it) == 0)) )
		_keys.InsertSorted(_key);
}

void CWin::add_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys, CStringLiteral _suffix, CStringLiteral _wndClass)
{
	CStringBuffer name(get_name());

	name.AppendString(_suffix);
	add_BrushKey(_keys, name);
	name.SetString(__FILE__LINE__ _wndClass);
	name.AppendString(_suffix);
	add_BrushKey(_keys, name);
	name.SetString(__FILE__LINE__ _T("Colors"));
	name.AppendString(_suffix);
	add_BrushKey(_keys, name);
}

void CWin::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
}

void CWin::add_FontKey(Ref(CDataVectorT<CStringBuffer>) _keys, ConstRef(CStringBuffer) _key)
{
	CDataVectorT<CStringBuffer>::Iterator it = _keys.FindSorted(_key);

	if ( !(it && (*it) && (_key.Compare(*it) == 0)) )
		_keys.InsertSorted(_key);
}

void CWin::add_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys, CStringLiteral _suffix, CStringLiteral _wndClass)
{
	CStringBuffer name(get_name());

	name.AppendString(_suffix);
	add_FontKey(_keys, name);
	name.SetString(__FILE__LINE__ _wndClass);
	name.AppendString(_suffix);
	add_FontKey(_keys, name);
}

void CWin::Refresh()
{
}

AFX_MSGMAP_ENTRY* CWin::_hasWindowMessage(UINT code)
{
	AFX_MSGMAP* map;
	AFX_MSGMAP_ENTRY* map_entry;

	for ( map = GetMessageMap(); map != nullptr; map = map->baseMap )
	{
		for ( map_entry = map->entries; map_entry->msg_Func != nullptr; ++map_entry )
		{
			if ( (map_entry->msg_Type == WM_MESSAGE_TYPE) && (map_entry->msg_ID == code) )
				return map_entry;
		}
	}
	return nullptr;
}

LRESULT CWin::OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	AFX_MSGMAP_ENTRY* map_entry = _hasWindowMessage(message);

	if ( map_entry )
		return (this->*(map_entry->msg_Func))(wParam, lParam);
	return DefaultWindowProc(message, wParam, lParam);
}

AFX_MSGMAP_ENTRY* CWin::_hasCommandMessage(UINT code)
{
	AFX_MSGMAP* map;
	AFX_MSGMAP_ENTRY* map_entry;

	for ( map = GetMessageMap(); map != nullptr; map = map->baseMap )
	{
		for ( map_entry = map->entries; map_entry->msg_Func != nullptr; ++map_entry )
		{
			if ( (map_entry->msg_Type == WM_COMMAND_TYPE) && (map_entry->msg_ID == WM_COMMAND)
				&& (map_entry->msg_Code <= code) && (map_entry->msg_CodeEnd >= code) )
				return map_entry;
		}
	}
	return nullptr;
}

LRESULT CWin::OnCommandMessage(WPARAM wParam, LPARAM lParam)
{
	WORD wmId = LOWORD(wParam);
	AFX_MSGMAP_ENTRY* map_entry = _hasCommandMessage(wmId);

	if ( map_entry )
		return (this->*(map_entry->msg_Func))(wParam, lParam);
	return DefaultWindowProc(WM_COMMAND, wParam, lParam);
}

AFX_MSGMAP_ENTRY* CWin::_hasNotifyMessage(UINT code)
{
	AFX_MSGMAP* map;
	AFX_MSGMAP_ENTRY* map_entry;

	for ( map = GetMessageMap(); map != nullptr; map = map->baseMap )
	{
		for ( map_entry = map->entries; map_entry->msg_Func != nullptr; ++map_entry )
		{
			if ( (map_entry->msg_Type == WM_NOTIFY_TYPE) && (map_entry->msg_ID == WM_NOTIFY)
				&& (map_entry->msg_Code <= code) && (map_entry->msg_CodeEnd >= code) )
				return map_entry;
		}
	}
	return nullptr;
}

LRESULT CWin::SendNotifyMessage(UINT code, LPARAM param)
{
	NotifyMessage hdr = { { m_hwnd, ::GetDlgCtrlID(m_hwnd), code }, param };
	CWin* parent = get_parent();

	while ( parent )
	{
		if ( parent->_hasNotifyMessage(code) )
			return ::SendMessage(parent->get_handle(), WM_NOTIFY, (WPARAM)(hdr.hdr.idFrom), (LPARAM)(&hdr));
		parent = parent->get_parent();
	}
	return -1;
}

LRESULT CWin::OnNotifyMessage(WPARAM wParam, LPARAM lParam)
{
	WORD wmCode = ((LPNMHDR)lParam)->code;
	AFX_MSGMAP_ENTRY* map_entry = _hasNotifyMessage(wmCode);

	if ( map_entry )
		return (this->*(map_entry->msg_Func))(wParam, lParam);
	return DefaultWindowProc(WM_NOTIFY, wParam, lParam);
}

LRESULT CWin::DefaultWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( m_wndwtype )
	{
	case TMDIWindow:
		return ::DefFrameProc(m_hwnd, CastDynamicPtr(CMDIFrame, this)->get_clienthandle(), message, wParam, lParam);
	case TMDIChildWindow:
		return ::DefMDIChildProc(m_hwnd, message, wParam, lParam);
	case TDialogWindow:
		return 0;
	default:
		return ::DefWindowProc(m_hwnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CWin::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnPaint %d\n", get_name().GetString(), debugID);
#endif
	hdc = BeginPaint(m_hwnd, &ps);
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls OnPaint %d, EraseFlag=%d\n", get_name().GetString(), debugID, ps.fErase);
#endif
	EndPaint(m_hwnd, &ps);
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnPaint %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

LRESULT CWin::OnNcDestroy(WPARAM wParam, LPARAM lParam)
{
	DefaultWindowProc(WM_NCDESTROY, wParam, lParam);
	::SetWindowLongPtr(m_hwnd, GWLP_USERDATA, NULL);
	delete this;
	return 0;
}

LRESULT CWin::OnEraseBackGround(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnEraseBackGround %d\n", get_name().GetString(), debugID);
#endif
	return TRUE;
}

#ifdef __DEBUG1__
static struct _tagSysCommands
{
	UINT cmd;
	CConstPointer text;
} vSysCommands[] = {
	{ SC_CLOSE, _T("SC_CLOSE") },
	{ SC_CONTEXTHELP, _T("SC_CONTEXTHELP") },
	{ SC_DEFAULT, _T("SC_DEFAULT") },
	{ SC_HOTKEY, _T("SC_HOTKEY") },
	{ SC_HSCROLL, _T("SC_HSCROLL") },
	{ SC_KEYMENU, _T("SC_KEYMENU") },
	{ SC_MAXIMIZE, _T("SC_MAXIMIZE") },
	{ SC_MINIMIZE, _T("SC_MINIMIZE") },
	{ SC_MONITORPOWER, _T("SC_MONITORPOWER") },
	{ SC_MOUSEMENU, _T("SC_MOUSEMENU") },
	{ SC_MOVE, _T("SC_MOVE") },
	{ SC_NEXTWINDOW, _T("SC_NEXTWINDOW") },
	{ SC_PREVWINDOW, _T("SC_PREVWINDOW") },
	{ SC_RESTORE, _T("SC_RESTORE") },
	{ SC_SCREENSAVE, _T("SC_SCREENSAVE") },
	{ SC_SIZE, _T("SC_SIZE") },
	{ SC_TASKLIST, _T("SC_TASKLIST") },
	{ SC_VSCROLL, _T("SC_VSCROLL") },
	{ 0, nullptr }
};
#endif

LRESULT CWin::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	int ix = 0;

	while ( vSysCommands[ix].cmd != 0 )
	{
		if ( vSysCommands[ix].cmd == (wParam & 0xFFF0) )
			break;
		++ix;
	}
	if ( vSysCommands[ix].text == nullptr )
		theGuiApp->DebugString("%ls OnSysCommand %d: wParam=0x%x, xPos=0x%x, yPos=0x%x\n", get_name().GetString(), debugID, 
		    wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	else
		theGuiApp->DebugString("%ls OnSysCommand %d: wParam=%ls, xPos=0x%x, yPos=0x%x\n", get_name().GetString(), debugID, 
		    vSysCommands[ix].text, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
#endif
	if ( (wParam & 0xFFF0) != SC_MOUSEMENU )
		return DefaultWindowProc(WM_SYSCOMMAND, wParam, lParam);
	return 0;
}

LRESULT CWin::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	CWin* pParent = get_parent();

	while ( pParent )
	{
		if ( pParent->_hasWindowMessage(WM_SYSKEYDOWN) )
			return pParent->SendMessage(WM_SYSKEYDOWN, wParam, lParam);
		pParent = pParent->get_parent();
	}
	return DefaultWindowProc(WM_SYSKEYDOWN, wParam, lParam);
}

LRESULT CWin::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	CWin* pParent = get_parent();

	while ( pParent )
	{
		if ( pParent->_hasWindowMessage(WM_SYSKEYUP) )
			return pParent->SendMessage(WM_SYSKEYUP, wParam, lParam);
		pParent = pParent->get_parent();
	}
	return DefaultWindowProc(WM_SYSKEYUP, wParam, lParam);
}

LRESULT CWin::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	CWin* pWin = (wParam == 0)?nullptr:(CWin*)(::GetWindowLongPtr((HWND)wParam, GWLP_USERDATA));
	theGuiApp->DebugString("%ls OnSetFocus %d: wParam=%ls\n", get_name().GetString(), debugID, 
		pWin?(pWin->get_name().GetString()):_T("nullptr"));
#endif
	return 0;
}

LRESULT CWin::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	CWin* pWin = (wParam == 0)?nullptr:(CWin*)(::GetWindowLongPtr((HWND)wParam, GWLP_USERDATA));
	theGuiApp->DebugString("%ls OnKillFocus %d: wParam=%ls\n", get_name().GetString(), debugID, 
		pWin?(pWin->get_name().GetString()):_T("nullptr"));
#endif
	return 0;
}

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

LRESULT CWin::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = DefaultWindowProc(WM_NCHITTEST, wParam, lParam);

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNcHitTest %d: x=0x%x, y=0x%x, ret=%s\n", get_name().GetString(), debugID, 
		GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), vNCHitTestCodes[ret+2]);
#endif
	return ret;
}

LRESULT CWin::OnMouseActivate(WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = DefaultWindowProc(WM_MOUSEACTIVATE, wParam, lParam);

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	CWin* pWin = (wParam == 0)?nullptr:(CWin*)(::GetWindowLongPtr((HWND)wParam, GWLP_USERDATA));
	theGuiApp->DebugString("%ls OnMouseActivate %d: wParam=%ls, LOWORD(lParam)=%s, HIWORD(lParam)=0x%x, ret=%d\n", get_name().GetString(), debugID, 
		pWin?(pWin->get_name().GetString()):_T("nullptr"), vNCHitTestCodes[LOWORD(lParam)+2], HIWORD(lParam), ret);
#endif
	return ret;
}

//***********************************************************
// CPopup
//***********************************************************
BEGIN_MESSAGE_MAP(CWin, CPopup)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CPopup::CPopup(LPCTSTR name): 
    CWin(TPopupWindow, name),
	m_panel(nullptr)
{
}

CPopup::CPopup(ConstRef(CStringBuffer) name): 
    CWin(TPopupWindow, name),
	m_panel(nullptr)
{
}

CPopup::~CPopup()
{}

BOOL CPopup::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = _T("CPOPUP");
	return TRUE;
}

BOOL CPopup::PreCreate(CREATESTRUCT& cs)
{
	cs.style = WS_POPUP;
	return TRUE;
}

void CPopup::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	if ( m_panel )
		m_panel->get_BrushKeys(_keys);
}

void CPopup::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	if ( m_panel )
		m_panel->get_FontKeys(_keys);
}

void CPopup::Refresh()
{
	if ( m_panel )
		m_panel->Refresh();
}

LRESULT CPopup::OnCreate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnCreate %d\n", get_name().GetString(), debugID);
#endif
	CStringBuffer name;

	name.FormatString(__FILE__LINE__ _T("%s.Panel"), m_name.GetString());
	m_panel = OK_NEW_OPERATOR CPanel(name);
	m_panel->set_border(has_ClientBorder());
	m_panel->set_HScrollEnabled(get_ClientHScrollEnabled());
	m_panel->set_VScrollEnabled(get_ClientVScrollEnabled());
	if ( !(m_panel->Create(m_hwnd, 1000)) )
		return -1;
	if ( SendMessage(WM_CREATECHILDREN, (WPARAM)(m_panel->get_handle()), 0) )
		return -1;

#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnCreate %d\n", get_name().GetString(), debugID);
#endif
	StartTimer(1000, 10);
	return 0;
}

LRESULT CPopup::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( ((LONG)wParam) == 1000 )
	{
		OnSize(0, 0);
		m_panel->OnSize(0, 0);
		StopTimer(1000);
	}
	return 0;
}

LRESULT CPopup::OnSize(WPARAM wParam, LPARAM lParam)
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
	if ( !m_panel )
		return -1;
	m_panel->MoveWindow(&r3, TRUE);
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnSize %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

//***********************************************************
// CFrameWnd
//***********************************************************
BEGIN_MESSAGE_MAP(CWin, CFrameWnd)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_NCACTIVATE()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_ENTERSIZEMOVE()
	ON_WM_EXITSIZEMOVE()
	ON_WM_INITMENUPOPUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_MESSAGE(WM_ENTERMENULOOP1,OnEnterMenuLoop)
	ON_WM_MESSAGE(WM_EXITMENULOOP1,OnExitMenuLoop)
	ON_WM_COMMMAND(IDM_CUT,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_COPY,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_DELETE,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_PASTE,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_UNDO,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_REDO,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_VIEWDEFAULTTOOLBAR,OnToggleToolbarCommand)
	ON_WM_COMMMAND(IDM_VIEWSTATUSBAR,OnToggleStatusbarCommand)
	ON_WM_COMMMAND(IDM_VIEWDOCKINFO,OnToggleDockInfoCommand)
	ON_WM_NOTIFY(NM_SELECTIONCHANGED,OnNotifySelectionChanged)
	ON_WM_NOTIFY(NM_MENUITEMHOVERED,OnNotifyMenuItemHovered)
END_MESSAGE_MAP()

CFrameWnd::CFrameWnd(LPCTSTR name): 
	CWin(TFrameWindow, name),
	m_iscreated(FALSE),
	m_clienthasborder(FALSE),
	m_clientvScrollEnabled(FALSE),
	m_clienthScrollEnabled(FALSE),
	m_toolbarenabled(FALSE),
	m_statusbarenabled(FALSE),
	m_dockingenabled(FALSE),
	m_menuloop(FALSE),
	m_cmdcode(1000),
	m_panel(nullptr),
	m_focuswindow(nullptr),
	m_toolbar(nullptr),
	m_statusbar(nullptr),
	m_menubar(nullptr),
	m_dockinfo(nullptr)
{}

CFrameWnd::CFrameWnd(ConstRef(CStringBuffer) name): 
	CWin(TFrameWindow, name),
	m_iscreated(FALSE),
	m_clienthasborder(FALSE),
	m_clientvScrollEnabled(FALSE),
	m_clienthScrollEnabled(FALSE),
	m_toolbarenabled(FALSE),
	m_statusbarenabled(FALSE),
	m_dockingenabled(FALSE),
	m_menuloop(FALSE),
	m_cmdcode(1000),
	m_panel(nullptr),
	m_focuswindow(nullptr),
	m_toolbar(nullptr),
	m_statusbar(nullptr),
	m_menubar(nullptr),
	m_dockinfo(nullptr)
{}

CFrameWnd::~CFrameWnd()
{
	if ( m_dockinfo )
		delete m_dockinfo;
}

BOOL CFrameWnd::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("CFRAMEWND");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("CFRAMEWND"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("CFRAMEWND"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL CFrameWnd::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("CFRAMEWND"), CResourceManager::WindowTitle));
	return TRUE;
}

void CFrameWnd::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	m_menubar->get_BrushKeys(_keys);
	if ( m_toolbarenabled )
		m_toolbar->get_BrushKeys(_keys);
	if ( m_statusbarenabled )
		m_statusbar->get_BrushKeys(_keys);
	if ( m_dockingenabled )
		m_dockinfo->get_BrushKeys(_keys);
	m_panel->get_BrushKeys(_keys);
}

void CFrameWnd::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	m_menubar->get_FontKeys(_keys);
	if ( m_toolbarenabled )
		m_toolbar->get_FontKeys(_keys);
	if ( m_statusbarenabled )
		m_statusbar->get_FontKeys(_keys);
	if ( m_dockingenabled )
		m_dockinfo->get_FontKeys(_keys);
	m_panel->get_FontKeys(_keys);
}

void CFrameWnd::Refresh()
{
	m_menubar->Refresh();
	if ( m_toolbarenabled )
		m_toolbar->Refresh();
	if ( m_statusbarenabled )
		m_statusbar->Refresh();
	if ( m_dockingenabled )
		m_dockinfo->Refresh();
	m_panel->Refresh();
}

static BOOL CALLBACK EnumChildProc_CFrameWnd_CloseAll(HWND hwnd, LPARAM lParam)
{
	if ( ::GetParent(hwnd) == (HWND)lParam )
		::DestroyWindow(hwnd);
	return TRUE;
}

void CFrameWnd::DestroyChildren(void)
{
	::EnumChildWindows(m_panel->get_handle(), EnumChildProc_CFrameWnd_CloseAll, (LPARAM)(m_panel->get_handle()));
	m_focuswindow = nullptr;
}

void CFrameWnd::AutoSize(void)
{
	m_menubar->AutoSize();
	if ( m_toolbarenabled )
		m_toolbar->AutoSize();
	if ( m_statusbarenabled )
		m_statusbar->AutoSize();
	if ( m_dockingenabled )
		m_dockinfo->AutoSize();
	OnSize(0, 0);
	m_panel->OnSize(0, 0);

	CWin* win = m_panel->get_firstchild();

	if ( win )
		win->set_focus();
}

LRESULT CFrameWnd::create_toolbar()
{
	if ( !m_toolbar )
	{
		CStringBuffer name(get_name());

		name.AppendString(_T(".ToolBar"));
		m_toolbar = OK_NEW_OPERATOR CToolbar(name);

		m_toolbar->set_border(TRUE);
		if ( !(m_toolbar->Create(m_hwnd, m_cmdcode++)) )
			return -1;
		if ( SendMessage(WM_CREATETOOLBAR, 0, (LPARAM)m_toolbar) )
			return -1;
	}
	return 0;
}

void CFrameWnd::set_ToolBarEnabled(BOOL b)
{
	if ( m_toolbarenabled == b )
		return;
	m_toolbarenabled = b;
	if ( m_iscreated )
	{
		if ( !m_toolbarenabled )
		{
			if ( m_toolbar )
				m_toolbar->set_Visible(FALSE);
		}
		else
		{
			if ( m_toolbar )
				m_toolbar->set_Visible(TRUE);
			else 
			{
				if ( create_toolbar() )
					return;
				m_toolbar->AutoSize();
			}
		}
		OnSize(0, 0);
	}
}

LRESULT CFrameWnd::create_statusbar()
{
	if ( !m_statusbar )
	{
		CStringBuffer name(get_name());

		name.AppendString(_T(".StatusBar"));
		m_statusbar = OK_NEW_OPERATOR CStatusbar(name);

		m_statusbar->set_border(TRUE);
		if ( !(m_statusbar->Create(m_hwnd, m_cmdcode++)) )
			return -1;
		if ( SendMessage(WM_CREATESTATUSBAR, 0, (LPARAM)m_statusbar) )
			return -1;
	}
	return 0;
}

void CFrameWnd::set_StatusBarEnabled(BOOL b)
{
	if ( m_statusbarenabled == b )
		return;
	m_statusbarenabled = b;
	if ( m_iscreated )
	{
		if ( !m_statusbarenabled )
		{
			if ( m_statusbar )
				m_statusbar->set_Visible(FALSE);
		}
		else
		{
			if ( m_statusbar )
				m_statusbar->set_Visible(TRUE);
			else
			{
				if ( create_statusbar() )
					return;
				m_statusbar->AutoSize();
			}
		}
		OnSize(0, 0);
	}
}

LRESULT CFrameWnd::create_menubar()
{
	if ( !m_menubar )
	{
		CStringBuffer name(get_name());

		name.AppendString(_T(".Menubar"));
		m_menubar = OK_NEW_OPERATOR CMenubar(name);

		m_menubar->set_border(TRUE);
		if ( !(m_menubar->Create(m_hwnd, m_cmdcode++)) )
			return -1;
		if ( SendMessage(WM_CREATEMENUBAR, 0, (LPARAM)m_menubar) )
			return -1;
	}
	return 0;
}

LRESULT CFrameWnd::create_dockinfo()
{
	if ( !m_dockinfo )
	{
		m_dockinfo = OK_NEW_OPERATOR CDockInfo(this);

		if ( SendMessage(WM_CREATEDOCKINFO, 0, (LPARAM)m_dockinfo) )
			return -1;
	}
	return 0;
}

void CFrameWnd::set_DockingEnabled(BOOL b)
{
	if ( m_dockingenabled == b )
		return;
	m_dockingenabled = b;
	if ( m_iscreated )
	{
		if ( !m_dockingenabled )
		{
			if ( m_dockinfo )
				m_dockinfo->set_Visible(FALSE);
		}
		else
		{
			if ( m_dockinfo )
				m_dockinfo->set_Visible(TRUE);
			else
			{
				if ( create_statusbar() )
					return;
				m_dockinfo->AutoSize();
			}
		}
		OnSize(0, 0);
	}
}

void CFrameWnd::update_toolbar()
{
	if ( m_toolbarenabled )
	{
		UINT cmds[] = { IDM_UNDO, IDM_REDO, IDM_CUT, IDM_COPY, IDM_DELETE, IDM_PASTE };

		for ( int ix = 0; ix < (sizeof(cmds)/sizeof(UINT)); ++ix )
			m_toolbar->set_ChildEnabled(cmds[ix], (m_focuswindow != nullptr) && (m_focuswindow->SendMessage(WM_COMMAND, MAKEWPARAM(cmds[ix], 2), 0)));
	}
}

LRESULT CFrameWnd::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT CFrameWnd::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	m_menubar->SendMessage(WM_SYSKEYDOWN, wParam, lParam);
	return 0;
}

LRESULT CFrameWnd::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	m_menubar->SendMessage(WM_SYSKEYUP, wParam, lParam);
	return 0;
}

LRESULT CFrameWnd::OnActivate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	CWin* pWin = (lParam == 0)?nullptr:(CWin*)(::GetWindowLongPtr((HWND)lParam, GWLP_USERDATA));
	theGuiApp->DebugString("%ls OnActivate %d: %s, %s, lParam=%ls\n", get_name().GetString(), debugID, 
		LOWORD(wParam)?"activated":"deactivated", HIWORD(wParam)?"minimized":"not minimized",
		pWin?(pWin->get_name().GetString()):_T("nullptr"));
#endif
	if ( HIWORD(wParam) )
	{
		if ( m_dockingenabled && m_dockinfo )
			m_dockinfo->HideFloatingWindows();
	}
	else
	{
		if ( m_dockingenabled && m_dockinfo )
			m_dockinfo->ShowFloatingWindows();
	}
	return 0;
}

LRESULT CFrameWnd::OnNcActivate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	CWin* pWin = nullptr;

	if ( ::IsWindow((HWND)lParam) )
		pWin = (CWin*)(::GetWindowLongPtr((HWND)lParam, GWLP_USERDATA));
	if ( pWin )
		theGuiApp->DebugString("%ls OnNcActivate %d: %s, lParam=%ls\n", get_name().GetString(), debugID, 
			wParam?"activated":"deactivated", pWin->get_name().GetString());
	else
		theGuiApp->DebugString("%ls OnNcActivate %d: %s, lParam=%x, IsWindow=%d\n", get_name().GetString(), debugID, 
			wParam?"activated":"deactivated", lParam, ::IsWindow((HWND)lParam));
#endif
	//if ( pWin && (!wParam) )
	//{
	//	if ( pWin->get_classname().Compare(_T("CPOPUP"), 0, CStringLiteral::cIgnoreCase) == 0 )
	//		wParam = TRUE;
	//}
	if ( m_menuloop && (!wParam) )
		wParam = TRUE;
	return DefaultWindowProc(WM_NCACTIVATE, wParam, lParam);
}

LRESULT CFrameWnd::OnActivateApp(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnActivateApp %d: %s, Thread=%x\n", get_name().GetString(), debugID, 
		wParam?"activated":"deactivated", lParam);
#endif
	if ( wParam && m_focuswindow )
		m_focuswindow->set_focus();
	return 0;
}

LRESULT CFrameWnd::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( ((LONG)wParam) == 1000 )
	{
		AutoSize();
		StopTimer(1000);
	}
	return 0;
}

LRESULT CFrameWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	RECT r1;
	RECT r2;
	RECT r3;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnSize %d\n", get_name().GetString(), debugID);
#endif
	if ( !GetClientRect(&rect) )
		return -1;
	if ( IsRectEmpty(&rect) )
		return -1;
	::CopyRect(&r3, &rect);
	if ( !m_menubar )
		return -1;
	m_menubar->GetWindowRect(&r1);
	::SetRectEmpty(&r2);
	r2.bottom = r1.bottom - r1.top;
	r2.right = rect.right;
	m_menubar->MoveWindow(&r2, TRUE);
	r3.top = r2.bottom;
	if ( is_ToolBarEnabled() )
	{
		if ( !m_toolbar )
			return -1;
		m_toolbar->GetWindowRect(&r1);
		::SetRectEmpty(&r2);
		r2.top = r3.top;
		r2.bottom = r1.bottom - r1.top + r3.top;
		r2.right = rect.right;
		m_toolbar->MoveWindow(&r2, TRUE);
		r3.top = r2.bottom;
	}
	if ( is_StatusBarEnabled() )
	{
		if ( !m_statusbar )
			return -1;
		m_statusbar->GetWindowRect(&r1);
		::SetRectEmpty(&r2);
		r2.top = rect.bottom - (r1.bottom - r1.top);
		r2.bottom = rect.bottom;
		r2.right = rect.right;
		m_statusbar->MoveWindow(&r2, TRUE);
		r3.bottom = r2.top;
	}
	if ( is_DockingEnabled() )
	{
		m_dockinfo->set_windowrect(&r3);
		m_dockinfo->get_clientrect(&r3);
	}
	if ( !m_panel )
		return -1;
	m_panel->MoveWindow(&r3, TRUE);
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

LRESULT CFrameWnd::OnSizing(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnSizing %d, wParam=%s\n", get_name().GetString(), debugID,
		vSizingCodes[wParam]);
#endif
	return TRUE;
}

LRESULT CFrameWnd::OnEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnEnterSizeMove %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

LRESULT CFrameWnd::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnExitSizeMove %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

LRESULT CFrameWnd::OnCreate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnCreate %d\n", get_name().GetString(), debugID);
#endif
	m_panel = OK_NEW_OPERATOR CPanel(_T("CFrameWnd.Panel"));
	m_panel->set_border(has_ClientBorder());
	m_panel->set_HScrollEnabled(is_ClientHScrollEnabled());
	m_panel->set_VScrollEnabled(is_ClientVScrollEnabled());
	if ( !(m_panel->Create(m_hwnd, m_cmdcode++)) )
		return -1;
	if ( create_menubar() )
		return -1;
	if ( is_ToolBarEnabled() )
	{
		if ( create_toolbar() )
			return -1;
	}
	if ( is_StatusBarEnabled() )
	{
		if ( create_statusbar() )
			return -1;
	}
	if ( is_DockingEnabled() )
	{
		if ( create_dockinfo() )
			return -1;
	}
	m_iscreated = TRUE;
	if ( SendMessage(WM_CREATECHILDREN, (WPARAM)(m_panel->get_handle()), 0) )
		return -1;

#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnCreate %d\n", get_name().GetString(), debugID);
#endif
	StartTimer(1000, 10);
	return 0;
}

void CFrameWnd::set_focuswindow(CWin* win)
{
	m_focuswindow = win;
	update_toolbar();
}

LRESULT CFrameWnd::OnInitMenuPopup(WPARAM wParam, LPARAM lParam)
{
	CPopupMenu* pMenu = CastAnyPtr(CPopupMenu, lParam);
	UINT cmds[] = { IDM_UNDO, IDM_REDO, IDM_CUT, IDM_COPY, IDM_DELETE, IDM_PASTE };

	for ( int ix = 0; ix < (sizeof(cmds)/sizeof(UINT)); ++ix )
	{
		CControl* pCtrl = pMenu->get_menuItem(cmds[ix]);

		if ( pCtrl )
			pCtrl->set_Enabled((m_focuswindow != nullptr) && (m_focuswindow->SendMessage(WM_COMMAND, MAKEWPARAM(cmds[ix], 2), 0)));
	}
	return 0;
}

LRESULT CFrameWnd::OnEnterMenuLoop(WPARAM wParam, LPARAM lParam)
{
	m_menuloop = TRUE;
	return 0;
}

LRESULT CFrameWnd::OnExitMenuLoop(WPARAM wParam, LPARAM lParam)
{
	m_menuloop = FALSE;
	CWin* pCurFocus = CWin::get_focus();

	if ( m_focuswindow != pCurFocus )
		m_focuswindow->set_focus();
	if ( m_statusbarenabled )
		m_statusbar->set_text(CStatusbar::TStatusStyleStatusMessages);
	return 0;
}

LRESULT CFrameWnd::OnEditForwardCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls EditForwardCommand%d %d\n", get_name().GetString(), LOWORD(wParam), debugID);
#endif

	CWin* pCurFocus = CWin::get_focus();

	if ( m_focuswindow != pCurFocus )
		m_focuswindow->set_focus();

	if ( m_focuswindow )
		m_focuswindow->SendMessage(WM_COMMAND, wParam, lParam);
	return 0;
}

LRESULT CFrameWnd::OnNotifySelectionChanged(WPARAM wParam, LPARAM lParam)
{
	Ptr(NotifyMessage) phdr = CastAnyPtr(NotifyMessage, lParam);
	CWin* pWin = CastAnyPtr(CWin, ::GetWindowLongPtr(phdr->hdr.hwndFrom, GWLP_USERDATA));

	m_focuswindow = pWin;
	update_toolbar();
	return 0;
}

LRESULT CFrameWnd::OnNotifyMenuItemHovered(WPARAM wParam, LPARAM lParam)
{
	if ( m_statusbarenabled )
	{
		Ptr(NotifyMessage) phdr = CastAnyPtr(NotifyMessage, lParam);
		CControl* pControl = CastAnyPtr(CControl, phdr->param);

		if ( CStatic* pStatic = CastDynamicPtr(CStatic, pControl) )
		{
			CStringBuffer text(__FILE__LINE__ 256);

			::LoadString(theGuiApp->get_GuiInstance(), pStatic->get_childID(), CastMutable(CPointer, text.GetString()), 256);
			m_statusbar->set_text(CStatusbar::TStatusStyleStatusMessages, text);
		}
	}
	return 0;
}

LRESULT CFrameWnd::OnToggleToolbarCommand(WPARAM wParam, LPARAM lParam)
{
	set_ToolBarEnabled(!(is_ToolBarEnabled()));
	return 0;
}

LRESULT CFrameWnd::OnToggleStatusbarCommand(WPARAM wParam, LPARAM lParam)
{
	set_StatusBarEnabled(!(is_StatusBarEnabled()));
	return 0;
}

LRESULT CFrameWnd::OnToggleDockInfoCommand(WPARAM wParam, LPARAM lParam)
{
	set_DockingEnabled(!(is_DockingEnabled()));
	return 0;
}

//***********************************************************
// CMDIFrame
//***********************************************************
BEGIN_MESSAGE_MAP(CWin, CMDIFrame)
	ON_WM_TIMER()
	ON_WM_NCACTIVATE()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_INITMENUPOPUP()
	ON_WM_MESSAGE(WM_ENTERMENULOOP1,OnEnterMenuLoop)
	ON_WM_MESSAGE(WM_EXITMENULOOP1,OnExitMenuLoop)
	ON_WM_MESSAGE(WM_MDICHILDSTATUS,OnChildStatus)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_COMMMAND(IDM_CUT,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_COPY,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_DELETE,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_PASTE,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_UNDO,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_REDO,OnEditForwardCommand)
	ON_WM_COMMMAND(IDM_TILEHORIZ,OnTileHorizCommand)
	ON_WM_COMMMAND(IDM_TILEVERT,OnTileVertCommand)
	ON_WM_COMMMAND(IDM_CASCADE,OnCascadeCommand)
	ON_WM_COMMMAND(IDM_CLOSEALL,OnCloseAllCommand)
	ON_WM_COMMMAND(IDM_VIEWDEFAULTTOOLBAR,OnToggleToolbarCommand)
	ON_WM_COMMMAND(IDM_VIEWSTATUSBAR,OnToggleStatusbarCommand)
	ON_WM_COMMMAND(IDM_VIEWDOCKINFO,OnToggleDockInfoCommand)
	ON_WM_NOTIFY(NM_SELECTIONCHANGED,OnNotifySelectionChanged)
	ON_WM_NOTIFY(NM_MENUITEMHOVERED,OnNotifyMenuItemHovered)
END_MESSAGE_MAP()

CMDIFrame::CMDIFrame(LPCTSTR name): 
	CWin(TMDIWindow, name), 
	m_iscreated(FALSE),
	m_toolbarenabled(FALSE),
	m_statusbarenabled(FALSE),
	m_dockingenabled(FALSE),
	m_menuloop(FALSE),
	m_cmdcode(1000),
	m_pCurrentChild(nullptr),
	m_childmaximized(FALSE),
	m_childcnt(0),
	m_clientHWnd(nullptr),
	m_toolbar(nullptr),
	m_statusbar(nullptr),
	m_menubar(nullptr),
	m_dockinfo(nullptr)
{
}

CMDIFrame::CMDIFrame(ConstRef(CStringBuffer) name): 
	CWin(TMDIWindow, name), 
	m_iscreated(FALSE),
	m_toolbarenabled(FALSE),
	m_statusbarenabled(FALSE),
	m_dockingenabled(FALSE),
	m_menuloop(FALSE),
	m_cmdcode(1000),
	m_pCurrentChild(nullptr),
	m_childmaximized(FALSE),
	m_childcnt(0),
	m_clientHWnd(nullptr),
	m_toolbar(nullptr),
	m_statusbar(nullptr),
	m_menubar(nullptr),
	m_dockinfo(nullptr)
{
}

CMDIFrame::~CMDIFrame()
{
	if ( m_dockinfo )
		delete m_dockinfo;
}

BOOL CMDIFrame::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("CMDIFRAME");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("CMDIFRAME"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("CMDIFRAME"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL CMDIFrame::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("CMDIFRAME"), CResourceManager::WindowTitle));
	return TRUE;
}

LRESULT CMDIFrame::create_toolbar()
{
	if ( !m_toolbar )
	{
		CStringBuffer name(get_name());

		name.AppendString(_T(".ToolBar"));
		m_toolbar = OK_NEW_OPERATOR CToolbar(name);

		m_toolbar->set_border(TRUE);
		if ( !(m_toolbar->Create(m_hwnd, m_cmdcode++)) )
			return -1;
		if ( SendMessage(WM_CREATETOOLBAR, 0, (LPARAM)m_toolbar) )
			return -1;
	}
	return 0;
}

void CMDIFrame::set_ToolBarEnabled(BOOL b)
{
	if ( m_toolbarenabled == b )
		return;
	m_toolbarenabled = b;
	if ( m_iscreated )
	{
		if ( !m_toolbarenabled )
		{
			if ( m_toolbar )
				m_toolbar->set_Visible(FALSE);
		}
		else
		{
			if ( m_toolbar )
				m_toolbar->set_Visible(TRUE);
			else if ( create_toolbar() )
				return;
		}
	}
}

LRESULT CMDIFrame::create_statusbar()
{
	if ( !m_statusbar )
	{
		CStringBuffer name(get_name());

		name.AppendString(_T(".StatusBar"));
		m_statusbar = OK_NEW_OPERATOR CStatusbar(name);

		m_statusbar->set_border(TRUE);
		if ( !(m_statusbar->Create(m_hwnd, m_cmdcode++)) )
			return -1;
		if ( SendMessage(WM_CREATESTATUSBAR, 0, (LPARAM)m_statusbar) )
			return -1;
	}
	return 0;
}

void CMDIFrame::set_StatusBarEnabled(BOOL b)
{
	if ( m_statusbarenabled == b )
		return;
	m_statusbarenabled = b;
	if ( m_iscreated )
	{
		if ( !m_statusbarenabled )
		{
			if ( m_statusbar )
				m_statusbar->set_Visible(FALSE);
		}
		else
		{
			if ( m_statusbar )
				m_statusbar->set_Visible(TRUE);
			else if ( create_statusbar() )
				return;
		}
	}
}

LRESULT CMDIFrame::create_menubar()
{
	if ( !m_menubar )
	{
		CStringBuffer name(get_name());

		name.AppendString(_T(".Menubar"));
		m_menubar = OK_NEW_OPERATOR CMenubar(name);
		m_menubar->set_border(TRUE);
		if ( !(m_menubar->Create(m_hwnd, m_cmdcode++)) )
			return -1;
		if ( SendMessage(WM_CREATEMENUBAR, 0, (LPARAM)m_menubar) )
			return -1;
	}
	return 0;
}

LRESULT CMDIFrame::create_dockinfo()
{
	if ( !m_dockinfo )
	{
		m_dockinfo = OK_NEW_OPERATOR CDockInfo(this);

		if ( SendMessage(WM_CREATEDOCKINFO, 0, (LPARAM)m_dockinfo) )
			return -1;
	}
	return 0;
}

void CMDIFrame::set_DockingEnabled(BOOL b)
{
	if ( m_dockingenabled == b )
		return;
	m_dockingenabled = b;
	if ( m_iscreated )
	{
		if ( !m_dockingenabled )
		{
			if ( m_dockinfo )
				m_dockinfo->set_Visible(FALSE);
		}
		else
		{
			if ( m_dockinfo )
				m_dockinfo->set_Visible(TRUE);
			else
			{
				if ( create_statusbar() )
					return;
				m_dockinfo->AutoSize();
			}
		}
		OnSize(0, 0);
	}
}

CWin* CMDIFrame::get_activeChild(BOOL* bMaximized)
{
	HWND activeChild;

	activeChild = (HWND)(::SendMessage(m_clientHWnd, WM_MDIGETACTIVE, 0, (LPARAM)bMaximized));
	if ( !activeChild )
		return nullptr;
	return (CWin*)(::GetWindowLongPtr(activeChild, GWLP_USERDATA));
}

LRESULT CMDIFrame::OnActivate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	CWin* pWin = (lParam == 0)?nullptr:(CWin*)(::GetWindowLongPtr((HWND)lParam, GWLP_USERDATA));
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnActivate %d: %s, %s, lParam=%ls\n", get_name().GetString(), debugID, 
		LOWORD(wParam)?"activated":"deactivated", HIWORD(wParam)?"minimized":"not minimized",
		pWin?(pWin->get_name().GetString()):_T("nullptr"));
#endif
	return 0;
}

LRESULT CMDIFrame::OnNcActivate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	CWin* pWin = nullptr;

	if ( ::IsWindow((HWND)lParam) )
		pWin = (CWin*)(::GetWindowLongPtr((HWND)lParam, GWLP_USERDATA));
	if ( pWin )
		theGuiApp->DebugString("%ls OnNcActivate %d: %s, lParam=%ls\n", get_name().GetString(), debugID, 
			wParam?"activated":"deactivated", pWin->get_name().GetString());
	else
		theGuiApp->DebugString("%ls OnNcActivate %d: %s, lParam=%x, IsWindow=%d\n", get_name().GetString(), debugID, 
			wParam?"activated":"deactivated", lParam, ::IsWindow((HWND)lParam));
#endif
	//if ( pWin && (!wParam) )
	//{
	//	if ( pWin->get_classname().Compare(_T("CPOPUP"), 0, CStringLiteral::cIgnoreCase) == 0 )
	//		wParam = TRUE;
	//}
	if ( m_menuloop && (!wParam) )
		wParam = TRUE;
	return DefaultWindowProc(WM_NCACTIVATE, wParam, lParam);
}

LRESULT CMDIFrame::OnActivateApp(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnActivateApp %d: %s, Thread=%x\n", get_name().GetString(), debugID, 
		wParam?"activated":"deactivated", lParam);
#endif
	return 0;
}

typedef struct _tagEnumChildProcCMDIFrameAddWindowMenuParam
{
	HWND parent;
	CPopupMenu* pMenu;
	bool bFirst;

	_tagEnumChildProcCMDIFrameAddWindowMenuParam(HWND _parent, CPopupMenu* _pMenu):
	    parent(_parent), pMenu(_pMenu), bFirst(true) {}

} TEnumChildProcCMDIFrameAddWindowMenuParam;

static BOOL CALLBACK EnumChildProc_CMDIFrame_AddWindowMenu(HWND hwnd, LPARAM lParam)
{
	Ptr(TEnumChildProcCMDIFrameAddWindowMenuParam) pParam = CastAnyPtr(TEnumChildProcCMDIFrameAddWindowMenuParam, lParam);

	if ( ::GetParent(hwnd) == pParam->parent )
	{
		CWin* pWin = CastAnyPtr(CWin, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if ( CMDIChild* pChild = CastDynamicPtr(CMDIChild, pWin) )
		{
			CStringBuffer text(pChild->get_windowtext());

			if ( text.GetLength() > 16 )
			{
				text.DeleteString(16, text.GetLength()-16);
				text.AppendString(_T("..."));
			}
			if ( pParam->bFirst )
			{
				pParam->pMenu->add_menuitem(0); // separator
				pParam->bFirst = false;
			}
			pParam->pMenu->add_menuitem(pChild->get_childID(), text);
		}
	}
	return TRUE;
}

LRESULT CMDIFrame::OnInitMenuPopup(WPARAM wParam, LPARAM lParam)
{
	CPopupMenu* pMenu = CastAnyPtr(CPopupMenu, lParam);
	CMDIChild* pclient = CastDynamicPtr(CMDIChild, get_activeChild());

	if ( pclient && (pMenu->get_menutype() == CPopupMenu::TMenuTypeEdit) )
		pclient->SendMessage(WM_INITMENUPOPUP, wParam, lParam);
	else if ( pMenu->get_menutype() == CPopupMenu::TMenuTypeWindow )
	{
		pMenu->reset_menuitems();

		TEnumChildProcCMDIFrameAddWindowMenuParam param(m_clientHWnd, pMenu);

		::EnumChildWindows(m_clientHWnd, EnumChildProc_CMDIFrame_AddWindowMenu, (LPARAM)(&param));
	}
	return 0;
}

LRESULT CMDIFrame::OnEnterMenuLoop(WPARAM wParam, LPARAM lParam)
{
	CMDIChild* pclient = CastDynamicPtr(CMDIChild, get_activeChild());

	m_menuloop = TRUE;
	if ( pclient )
		pclient->SendMessage(WM_ENTERMENULOOP1, wParam, lParam);
	return 0;
}

LRESULT CMDIFrame::OnExitMenuLoop(WPARAM wParam, LPARAM lParam)
{
	CMDIChild* pclient = CastDynamicPtr(CMDIChild, get_activeChild());

	m_menuloop = FALSE;
	if ( pclient )
		pclient->SendMessage(WM_EXITMENULOOP1, wParam, lParam);
	if ( m_statusbarenabled )
		m_statusbar->set_text(CStatusbar::TStatusStyleStatusMessages);
	return 0;
}

#ifdef __DEBUG1__
static CConstPointer vStatusCodes[] = {
	_T(""),
	_T("MCS_CREATED"),
	_T("MCS_DESTROYED"),
	_T("MCS_MAXIMIZED"),
	_T("MCS_MINIMIZED"),
	_T("MCS_RESTORED"),
	_T("MCS_ACTIVATED"),
	_T("MCS_DEACTIVATED")
};
#endif

LRESULT CMDIFrame::OnChildStatus(WPARAM wParam, LPARAM lParam)
{
	CMDIChild* pChild = CastAnyPtr(CMDIChild, lParam);

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnChildStatus %d, wParam=%ls, Child=%ls\n", get_name().GetString(), debugID, vStatusCodes[wParam], pChild->get_name().GetString());
#endif
	switch ( wParam )
	{
	case MCS_CREATED:
		++m_childcnt;
		break;
	case MCS_DESTROYED:
		--m_childcnt;
		break;
	case MCS_MAXIMIZED:
		if ( m_pCurrentChild == pChild )
		{
			m_childmaximized = TRUE;
			m_menubar->set_MDIMenubar(TRUE, pChild);
		}
		break;
	case MCS_MINIMIZED:
		if ( m_pCurrentChild == pChild )
		{
			m_childmaximized = FALSE;
			m_menubar->set_MDIMenubar(FALSE);
		}
		break;
	case MCS_RESTORED:
		if ( m_pCurrentChild == pChild )
		{
			m_childmaximized = FALSE;
			m_menubar->set_MDIMenubar(FALSE);
		}
		break;
	case MCS_ACTIVATED:
		m_pCurrentChild = pChild;
		if ( m_pCurrentChild->get_sizestatus() == SIZE_MAXIMIZED )
		{
			m_childmaximized = TRUE;
			m_menubar->set_MDIMenubar(TRUE, pChild);
		}
		break;
	case MCS_DEACTIVATED:
		if ( m_pCurrentChild == pChild )
		{
			m_childmaximized = FALSE;
			m_menubar->set_MDIMenubar(FALSE);
		}
		m_pCurrentChild = nullptr;
		break;
	default:
		break;
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnChildStatus %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

LRESULT CMDIFrame::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( ((LONG)wParam) == 1000 )
	{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls Refresh %d\n", get_name().GetString(), debugID);
#endif
	    m_menubar->AutoSize();
		if ( m_toolbarenabled )
			m_toolbar->AutoSize();
		if ( m_statusbarenabled )
			m_statusbar->AutoSize();

		RECT r;

		GetWindowRect(&r);
		++(r.bottom);
		SetWindowPos(&r, SWP_NOACTIVATE | SWP_NOREDRAW);

		UINT cmds[] = { IDM_UNDO, IDM_REDO, IDM_CUT, IDM_COPY, IDM_DELETE, IDM_PASTE };

		for ( int ix = 0; ix < (sizeof(cmds)/sizeof(UINT)); ++ix )
			m_toolbar->set_ChildEnabled(cmds[ix], FALSE);
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls Refresh %d\n", get_name().GetString(), debugID);
#endif
		StopTimer(1000);
	}
	return 0;
}

LRESULT CMDIFrame::OnCreate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnCreate %d\n", get_name().GetString(), debugID);
#endif
	if ( create_menubar() )
		return -1;
	if ( is_ToolBarEnabled() )
	{
		if ( create_toolbar() )
			return -1;
	}
	if ( is_StatusBarEnabled() )
	{
		if ( create_statusbar() )
			return -1;
	}
	if ( is_DockingEnabled() )
	{
		if ( create_dockinfo() )
			return -1;
	}

	CLIENTCREATESTRUCT ccs; 
 
	ccs.hWindowMenu = nullptr;
	ccs.idFirstChild = IDM_WINDOWCHILD; 
 
	m_clientHWnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, _T("MDICLIENT"), (LPCTSTR) nullptr, 
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | MDIS_ALLCHILDSTYLES, 
		0, 0, 0, 0, m_hwnd, (HMENU) nullptr, theGuiApp->get_GuiInstance(), (LPSTR) &ccs); 
 
	if ( !m_clientHWnd )
		return -1;

	prevMDIClientWndProc = (WNDPROC)(::SetWindowLongPtr(m_clientHWnd, GWLP_WNDPROC, (LONG)MDIClientWndProc));
#ifdef __DEBUG1__
	LONG test = ::GetWindowLongPtr(m_clientHWnd, GWLP_USERDATA);
	theGuiApp->DebugString("%ls OnCreate %d, GetWindowLongPtr(m_clientHWnd, GWLP_USERDATA)=%x\n", get_name().GetString(), debugID, test);
#endif
	m_iscreated = TRUE;

#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnCreate %d\n", get_name().GetString(), debugID);
#endif
	StartTimer(1000, 10);
	return 0;
}

LRESULT CMDIFrame::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	RECT r1;
	RECT r2;
	RECT r3;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnSize %d\n", get_name().GetString(), debugID);
#endif
	if ( !GetClientRect(&rect) )
		return -1;
	if ( IsRectEmpty(&rect) )
		return -1;
	if ( !m_menubar )
		return -1;
	m_menubar->GetWindowRect(&r1);
	::SetRectEmpty(&r2);
	r2.bottom = r1.bottom - r1.top;
	r2.right = rect.right;
	m_menubar->MoveWindow(&r2, TRUE);
	::CopyRect(&r3, &rect);
	r3.top += r1.bottom - r1.top;
	if ( is_ToolBarEnabled() )
	{
		if ( !m_toolbar )
			return -1;
		m_toolbar->GetWindowRect(&r1);
		::SetRectEmpty(&r2);
		r2.top = r3.top;
		r2.bottom = r2.top + r1.bottom - r1.top;
		r2.right = rect.right;
		m_toolbar->MoveWindow(&r2, TRUE);
		r3.top += r1.bottom - r1.top;
	}
	if ( is_StatusBarEnabled() )
	{
		if ( !m_statusbar )
			return -1;
		m_statusbar->GetWindowRect(&r1);
		::SetRectEmpty(&r2);
		r2.top = rect.bottom - (r1.bottom - r1.top);
		r2.bottom = rect.bottom;
		r2.right = rect.right;
		m_statusbar->MoveWindow(&r2, TRUE);
		r3.bottom -= r1.bottom - r1.top;
	}
	if ( is_DockingEnabled() )
	{
		if ( !m_dockinfo )
			return -1;
		m_dockinfo->set_windowrect(&r3);
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnSize %d\n", get_name().GetString(), debugID);
#endif
	return DefaultWindowProc(WM_SIZE, wParam, lParam);
}

LRESULT CMDIFrame::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	::SetWindowLongPtr(m_clientHWnd, GWLP_WNDPROC, (LONG)prevMDIClientWndProc);
	::PostQuitMessage(0);
	return 0;
}

LRESULT CMDIFrame::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	m_menubar->SendMessage(WM_SYSKEYDOWN, wParam, lParam);
	return 0;
}

LRESULT CMDIFrame::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	m_menubar->SendMessage(WM_SYSKEYUP, wParam, lParam);
	return 0;
}

LRESULT CMDIFrame::OnEditForwardCommand(WPARAM wParam, LPARAM lParam)
{
	CMDIChild* pclient = CastDynamicPtr(CMDIChild, get_activeChild());

	if ( !pclient )
		return 1;
	if ( !(pclient->get_focuswindow()) )
		return 1;

	CWin* pcurfocus = CWin::get_focus();

	if ( pcurfocus != pclient->get_focuswindow() )
		pclient->get_focuswindow()->set_focus();

	pclient->get_focuswindow()->SendMessage(WM_COMMAND, wParam, lParam);
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls EditForwardCommand%d %d\n", get_name().GetString(), LOWORD(wParam), debugID);
#endif
	return 0;
}

LRESULT CMDIFrame::OnTileHorizCommand(WPARAM wParam, LPARAM lParam)
{
	::SendMessage(m_clientHWnd, WM_MDITILE, MDITILE_HORIZONTAL, NULL);
	return 0;
}

LRESULT CMDIFrame::OnTileVertCommand(WPARAM wParam, LPARAM lParam)
{
	::SendMessage(m_clientHWnd, WM_MDITILE, MDITILE_VERTICAL, NULL);
	return 0;
}

LRESULT CMDIFrame::OnCascadeCommand(WPARAM wParam, LPARAM lParam)
{
	::SendMessage(m_clientHWnd, WM_MDICASCADE, MDITILE_ZORDER, NULL);
	return 0;
}

static BOOL CALLBACK EnumChildProc_CMDIFrame_CloseAll(HWND hwnd, LPARAM lParam)
{
	if ( ::GetParent(hwnd) == (HWND)lParam )
		::SendMessage((HWND)lParam, WM_MDIDESTROY, (WPARAM)hwnd, NULL);
	return TRUE;
}

LRESULT CMDIFrame::OnCloseAllCommand(WPARAM wParam, LPARAM lParam)
{
	::EnumChildWindows(m_clientHWnd, EnumChildProc_CMDIFrame_CloseAll, (LPARAM)m_clientHWnd);
	return 0;
}

LRESULT CMDIFrame::OnNotifySelectionChanged(WPARAM wParam, LPARAM lParam)
{
	Ptr(NotifyMessage) phdr = CastAnyPtr(NotifyMessage, lParam);
	CWin* pWin = CastAnyPtr(CWin, ::GetWindowLongPtr(phdr->hdr.hwndFrom, GWLP_USERDATA));
	CWin* pFocusWin = nullptr;
	CMDIChild* pclient = CastDynamicPtr(CMDIChild, get_activeChild());

	if ( pclient )
		pFocusWin = pclient->get_focuswindow();
	if ( pWin != pFocusWin )
	{
		pWin->set_focus();
		return 0;
	}

	UINT cmds[] = { IDM_UNDO, IDM_REDO, IDM_CUT, IDM_COPY, IDM_DELETE, IDM_PASTE };

	for ( int ix = 0; ix < (sizeof(cmds)/sizeof(UINT)); ++ix )
		m_toolbar->set_ChildEnabled(cmds[ix], (pFocusWin != nullptr) && (pFocusWin->SendMessage(WM_COMMAND, MAKEWPARAM(cmds[ix], 2), 0)));
	return 0;
}

LRESULT CMDIFrame::OnNotifyMenuItemHovered(WPARAM wParam, LPARAM lParam)
{
	if ( m_statusbarenabled )
	{
		Ptr(NotifyMessage) phdr = CastAnyPtr(NotifyMessage, lParam);
		CControl* pControl = CastAnyPtr(CControl, phdr->param);

		if ( CStatic* pStatic = CastDynamicPtr(CStatic, pControl) )
		{
			CStringBuffer text(__FILE__LINE__ 256);

			::LoadString(theGuiApp->get_GuiInstance(), pStatic->get_childID(), CastMutable(CPointer, text.GetString()), 256);
			m_statusbar->set_text(CStatusbar::TStatusStyleStatusMessages, text);
		}
	}
	return 0;
}

LRESULT CMDIFrame::OnToggleToolbarCommand(WPARAM wParam, LPARAM lParam)
{
	set_ToolBarEnabled(!(is_ToolBarEnabled()));
	return 0;
}

LRESULT CMDIFrame::OnToggleStatusbarCommand(WPARAM wParam, LPARAM lParam)
{
	set_StatusBarEnabled(!(is_StatusBarEnabled()));
	return 0;
}

LRESULT CMDIFrame::OnToggleDockInfoCommand(WPARAM wParam, LPARAM lParam)
{
	set_DockingEnabled(!(is_DockingEnabled()));
	return 0;
}

//***********************************************************
// CMDIChild
//***********************************************************
BEGIN_MESSAGE_MAP(CWin, CMDIChild)
	ON_WM_INITMENUPOPUP()
	ON_WM_MESSAGE(WM_ENTERMENULOOP1,OnEnterMenuLoop)
	ON_WM_MESSAGE(WM_EXITMENULOOP1,OnExitMenuLoop)
	ON_WM_NCACTIVATE()
	ON_WM_CHILDACTIVATE()
	ON_WM_MESSAGE(WM_MDIACTIVATE, OnMDIActivate)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CMDIChild::CMDIChild(LPCTSTR name): 
	CWin(TMDIChildWindow, name),
	m_clienthasborder(FALSE),
	m_clientvScrollEnabled(FALSE),
	m_clienthScrollEnabled(FALSE),
	m_menuloop(FALSE),
	m_sizestatus(SIZE_RESTORED),
	m_panel(nullptr),
	m_focuswindow(nullptr)
{
}

CMDIChild::CMDIChild(ConstRef(CStringBuffer) name): 
	CWin(TMDIChildWindow, name),
	m_clienthasborder(FALSE),
	m_clientvScrollEnabled(FALSE),
	m_clienthScrollEnabled(FALSE),
	m_menuloop(FALSE),
	m_sizestatus(SIZE_RESTORED),
	m_panel(nullptr),
	m_focuswindow(nullptr)
{
}

CMDIChild::~CMDIChild()
{
}

BOOL CMDIChild::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("CMDICHILD");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("CMDICHILD"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("CMDICHILD"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL CMDIChild::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("CMDICHILD"), CResourceManager::WindowTitle));
	return TRUE;
}

BOOL CMDIChild::RegisterClass(WNDCLASSEX& wcex)
{
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= &MDIChildWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= theGuiApp->get_GuiInstance();
	wcex.lpszClassName	= _T("CWINDOWS");
	wcex.hIcon			= nullptr;
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= nullptr; // (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr;
	wcex.hIconSm		= nullptr;

	if ( !PreRegisterClass(wcex) )
		return FALSE;

	WNDCLASSEX wcex1;

	::SecureZeroMemory(&wcex1, sizeof(WNDCLASSEX));
	wcex1.cbSize = sizeof(WNDCLASSEX);
	if ( !(::GetClassInfoEx(theGuiApp->get_GuiInstance(), wcex.lpszClassName, &wcex1)) )
	{
		if ( !(::RegisterClassEx(&wcex)) )
			return FALSE;
	}
	return TRUE;
}

BOOL CMDIChild::Create(HWND parent)
{
	WNDCLASSEX wcex;

	if ( !RegisterClass(wcex) )
		return  FALSE;

	CREATESTRUCT cs;

	::SecureZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hInstance = wcex.hInstance;
	cs.lpszClass = wcex.lpszClassName;
	cs.hwndParent = parent;
	cs.lpCreateParams = this;
	cs.x = CW_USEDEFAULT;
	cs.y = CW_USEDEFAULT;
	cs.cx = CW_USEDEFAULT;
	cs.cy = CW_USEDEFAULT;
	if ( !PreCreate(cs) )
		return FALSE;

	HWND activeChild;
	BOOL isMaximized;

	activeChild = (HWND)::SendMessage(parent, WM_MDIGETACTIVE, 0, (LPARAM)(&isMaximized));

	isMaximized |= cs.style & WS_MAXIMIZE;
	cs.style = cs.style & ~WS_MAXIMIZE;

	cs.style |= WS_VISIBLE | WS_OVERLAPPEDWINDOW;
	cs.dwExStyle |= WS_EX_MDICHILD;

	m_hwnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, 
		cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, 
		cs.hInstance, cs.lpCreateParams);

	if (!m_hwnd)
		return FALSE;
	SetWindowPos(nullptr, SWP_FRAMECHANGED);

	if ( isMaximized )
		::SendMessage(parent, WM_MDIMAXIMIZE, (WPARAM)m_hwnd, NULL);
	else
		::SendMessage(parent, WM_MDIRESTORE, (WPARAM)m_hwnd, NULL);
	return TRUE;
}

static BOOL CALLBACK EnumChildProc_CMDIChild_DestroyChildren(HWND hwnd, LPARAM lParam)
{
	if ( ::GetParent(hwnd) == (HWND)lParam )
		::DestroyWindow(hwnd);
	return TRUE;
}

void CMDIChild::DestroyChildren(void)
{
	::EnumChildWindows(m_panel->get_handle(), EnumChildProc_CMDIChild_DestroyChildren, (LPARAM)(m_panel->get_handle()));
	m_focuswindow = nullptr;
}

void CMDIChild::AutoSize(void)
{
	m_panel->OnSize(0, 0);
	reset_focus();
}

void CMDIChild::reset_focus(void)
{
	if ( !m_focuswindow )
	{
		CWin* win = m_panel->get_firstchild();

		if ( win )
			win->set_focus();
	}
	else if ( m_focuswindow != CWin::get_focus() )
		m_focuswindow->set_focus();
}

LRESULT CMDIChild::OnInitMenuPopup(WPARAM wParam, LPARAM lParam)
{
	CPopupMenu* pMenu = CastAnyPtr(CPopupMenu, lParam);
	UINT cmds[] = { IDM_UNDO, IDM_REDO, IDM_CUT, IDM_COPY, IDM_DELETE, IDM_PASTE };

	for ( int ix = 0; ix < (sizeof(cmds)/sizeof(UINT)); ++ix )
	{
		CControl* pCtrl = pMenu->get_menuItem(cmds[ix]);

		if ( pCtrl )
			pCtrl->set_Enabled((m_focuswindow != nullptr) && (m_focuswindow->SendMessage(WM_COMMAND, MAKEWPARAM(cmds[ix], 2), 0)));
	}
	return 0;
}

LRESULT CMDIChild::OnEnterMenuLoop(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnEnterMenuLoop %d\n", get_name().GetString(), debugID);
#endif
	m_menuloop = TRUE;
	return 0;
}

LRESULT CMDIChild::OnExitMenuLoop(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnExitMenuLoop %d\n", get_name().GetString(), debugID);
#endif
	m_menuloop = FALSE;
	reset_focus();
	return 0;
}

LRESULT CMDIChild::OnChildActivate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnChildActivate %d\n", get_name().GetString(), debugID);
#endif
	if ( !is_Iconic() )
		reset_focus();
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnChildActivate %d\n", get_name().GetString(), debugID);
#endif
	return DefaultWindowProc(WM_CHILDACTIVATE, wParam, lParam);
}

LRESULT CMDIChild::OnMDIActivate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	CWin* pActivated = (lParam == 0)?nullptr:(CWin*)(::GetWindowLongPtr((HWND)lParam, GWLP_USERDATA));
	CWin* pDeactivated = (wParam == 0)?nullptr:(CWin*)(::GetWindowLongPtr((HWND)wParam, GWLP_USERDATA));
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnMDIActivate %d: Activated=%ls, Deactivated=%ls\n", get_name().GetString(), debugID, 
		pActivated?(pActivated->get_name().GetString()):_T("nullptr"), pDeactivated?(pDeactivated->get_name().GetString()):_T("nullptr"));
#endif
	if ( m_hwnd == (HWND)lParam )
	{
		// this child is activated
		reset_focus();
		get_frame()->SendMessage(WM_MDICHILDSTATUS, MCS_ACTIVATED, (LPARAM)this);
	}
	if ( m_hwnd == (HWND)wParam )
	{
		// this child is deactivated
		get_frame()->SendMessage(WM_MDICHILDSTATUS, MCS_DEACTIVATED, (LPARAM)this);
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnMDIActivate %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

LRESULT CMDIChild::OnNcActivate(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	CWin* pWin = nullptr;

	if ( ::IsWindow((HWND)lParam) )
		pWin = (CWin*)(::GetWindowLongPtr((HWND)lParam, GWLP_USERDATA));
	if ( pWin )
		theGuiApp->DebugString("Before %ls OnNcActivate %d: %s, lParam=%ls\n", get_name().GetString(), debugID, 
			wParam?"activated":"deactivated", pWin->get_name().GetString());
	else
		theGuiApp->DebugString("Before %ls OnNcActivate %d: %s, lParam=%x, IsWindow=%d\n", get_name().GetString(), debugID, 
			wParam?"activated":"deactivated", lParam, ::IsWindow((HWND)lParam));
#endif
	//if ( pWin && (!wParam) )
	//{
	//	if ( pWin->get_classname().Compare(_T("CPOPUP"), 0, CStringLiteral::cIgnoreCase) == 0 )
	//		wParam = TRUE;
	//}
	if ( wParam && (!m_menuloop) )
		reset_focus();
	if ( m_menuloop && (!wParam) )
		wParam = TRUE;
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnNcActivate %d\n", get_name().GetString(), debugID);
#endif
	return DefaultWindowProc(WM_NCACTIVATE, wParam, lParam);
}

LRESULT CMDIChild::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( ((LONG)wParam) == 1000 )
	{
		AutoSize();
		StopTimer(1000);
	}
	return 0;
}

LRESULT CMDIChild::OnCreate(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer name(get_name());

	name.AppendString(_T(".Panel"));

	m_panel = OK_NEW_OPERATOR CPanel(name);
	m_panel->set_border(has_ClientBorder());
	m_panel->set_HScrollEnabled(get_ClientHScrollEnabled());
	m_panel->set_VScrollEnabled(get_ClientVScrollEnabled());
	if ( !(m_panel->Create(m_hwnd, 1000)) )
		return -1;
	if ( SendMessage(WM_CREATECHILDREN, (WPARAM)(m_panel->get_handle()), 0) )
		return -1;
	get_frame()->SendMessage(WM_MDICHILDSTATUS, MCS_CREATED, (LPARAM)this);
	StartTimer(1000, 20);
	return 0;
}

LRESULT CMDIChild::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	get_frame()->SendMessage(WM_MDICHILDSTATUS, MCS_DESTROYED, (LPARAM)this);
	return 0;
}

LRESULT CMDIChild::OnSize(WPARAM wParam, LPARAM lParam)
{
	DefaultWindowProc(WM_SIZE, wParam, lParam);

	m_sizestatus = Castdword(wParam);
	switch ( m_sizestatus )
	{
	case SIZE_MAXIMIZED:
		get_frame()->SendMessage(WM_MDICHILDSTATUS, MCS_MAXIMIZED, (LPARAM)this);
		break;
	case SIZE_MINIMIZED:
		get_frame()->SendMessage(WM_MDICHILDSTATUS, MCS_MINIMIZED, (LPARAM)this);
		break;
	case SIZE_RESTORED:
		get_frame()->SendMessage(WM_MDICHILDSTATUS, MCS_RESTORED, (LPARAM)this);
		break;
	default:
		break;
	}

	if ( m_sizestatus != SIZE_MINIMIZED )
	{
		RECT rect;

		if ( !GetClientRect(&rect) )
			return 1;
		if ( IsRectEmpty(&rect) )
			return 1;
		if ( !m_panel )
			return 1;
		m_panel->MoveWindow(&rect, TRUE);
	}
	return 0;
}

void CMDIChild::set_focuswindow(CWin* win)
{
	m_focuswindow = win;

	CMDIFrame* pFrame = CastDynamicPtr(CMDIFrame, get_frame());

	if ( !pFrame )
		return;

	if ( pFrame->is_ToolBarEnabled() )
	{
		CToolbar* pToolbar = pFrame->get_ToolBar();

		if ( !pToolbar )
			return;

		UINT cmds[] = { IDM_UNDO, IDM_REDO, IDM_CUT, IDM_COPY, IDM_DELETE, IDM_PASTE };

		for ( int ix = 0; ix < (sizeof(cmds)/sizeof(UINT)); ++ix )
			pToolbar->set_ChildEnabled(cmds[ix], (m_focuswindow != nullptr) && (m_focuswindow->SendMessage(WM_COMMAND, MAKEWPARAM(cmds[ix], 2), 0)));
	}
}

//***********************************************************
// CDialog
//***********************************************************
BEGIN_MESSAGE_MAP(CWin, CDialog)
END_MESSAGE_MAP()

CDialog::TDialogControls CDialog::m_DialogControls(__FILE__LINE__ 64, 64);

CDialog::CDialog(LPCTSTR name, LPCTSTR resID, CWin* pParent):
	CWin(TDialogWindow, name),
	m_resID(resID),
	m_pParent(pParent)
{
}

CDialog::CDialog(ConstRef(CStringBuffer) name, LPCTSTR resID, CWin* pParent):
	CWin(TDialogWindow, name),
	m_resID(resID),
	m_pParent(pParent)
{
}

CDialog::~CDialog()
{
}

INT_PTR CDialog::DoModal()
{
	return ::DialogBoxParam(theGuiApp->get_GuiInstance(), m_resID, m_pParent?(m_pParent->get_handle()):nullptr, DialogWndProc, (LPARAM)this);
}

void CDialog::CenterWindow()
{
	HWND hwndOwner; 
	RECT rc, rcDlg, rcOwner;

    if ((hwndOwner = ::GetParent(m_hwnd)) == nullptr) 
    {
        hwndOwner = ::GetDesktopWindow(); 
    }

    ::GetWindowRect(hwndOwner, &rcOwner); 
    ::GetWindowRect(m_hwnd, &rcDlg); 
    ::CopyRect(&rc, &rcOwner); 

    ::OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
    ::OffsetRect(&rc, -rc.left, -rc.top); 
    ::OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 

    ::SetWindowPos(m_hwnd, 
                 HWND_TOP, 
                 rcOwner.left + (rc.right / 2), 
                 rcOwner.top + (rc.bottom / 2), 
                 0, 0,
                 SWP_NOSIZE);
}

void CDialog::AddDialogControl(CControl* pControl)
{
	Ptr(TDialogControl) item = OK_NEW_OPERATOR TDialogControl;
	WNDCLASSEX wcex;

	pControl->RegisterClass(wcex);
	item->pControl = pControl;
	item->m_classname.SetString(__FILE__LINE__ wcex.lpszClassName);
	m_DialogControls.InsertSorted(item);
}

CControl* CDialog::GetDialogControl(LPCTSTR className)
{
	TDialogControl item;

	item.m_classname.SetString(__FILE__LINE__ className);

	TDialogControls::Iterator it = m_DialogControls.FindSorted(&item);

	if (it)
		return (*it)->pControl;
	return nullptr;
}

void CDialog::RemoveDialogControls()
{
	m_DialogControls.Close();
}

//***********************************************************
// CControl
//***********************************************************
BEGIN_MESSAGE_MAP(CWin, CControl)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CControl::CControl(LPCTSTR name): 
    CWin(TControlWindow, name),
	m_hasborder(FALSE),
	m_update(0),
	m_updateRects(FALSE),
	m_updateRectsRequested(FALSE),
	m_vScrollEnabled(FALSE),
	m_hScrollEnabled(FALSE),
	m_hUserVScroll(nullptr),
	m_hUserHScroll(nullptr),
	m_hScrollVisible(FALSE),
	m_vScrollVisible(FALSE),
	m_insideHScroll(FALSE),
	m_insideVScroll(FALSE),
	m_sizeHAlign(TSizeHAlignLeft),
	m_layoutX(0),
	m_sizeSpecX(TSizeSpecifierDefaultSize),
	m_layoutSizeX(0),
	m_sizeVAlign(TSizeVAlignTop),
	m_layoutY(0),
	m_sizeSpecY(TSizeSpecifierDefaultSize),
	m_layoutSizeY(0)
{
	::SetRectEmpty(&m_maxClientArea);
	::SetRectEmpty(&m_Margins);
}

CControl::CControl(ConstRef(CStringBuffer) name): 
    CWin(TControlWindow, name),
	m_hasborder(FALSE),
	m_update(0),
	m_updateRects(FALSE),
	m_updateRectsRequested(FALSE),
	m_vScrollEnabled(FALSE),
	m_hScrollEnabled(FALSE),
	m_hUserVScroll(nullptr),
	m_hUserHScroll(nullptr),
	m_hScrollVisible(FALSE),
	m_vScrollVisible(FALSE),
	m_insideHScroll(FALSE),
	m_insideVScroll(FALSE),
	m_sizeHAlign(TSizeHAlignLeft),
	m_layoutX(0),
	m_sizeSpecX(TSizeSpecifierDefaultSize),
	m_layoutSizeX(0),
	m_sizeVAlign(TSizeVAlignTop),
	m_layoutY(0),
	m_sizeSpecY(TSizeSpecifierDefaultSize),
	m_layoutSizeY(0)
{
	::SetRectEmpty(&m_maxClientArea);
	::SetRectEmpty(&m_Margins);
}

CControl::~CControl()
{
}

BOOL CControl::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CCONTROL");
	return TRUE;
}

BOOL CControl::PreCreate(CREATESTRUCT& cs)
{
	DWORD styles = WS_CHILD | WS_CLIPCHILDREN;

	if ( m_hasborder ) 
		styles |= WS_BORDER;
	if ( m_vScrollEnabled && (m_hUserVScroll == nullptr) )
		styles |= WS_VSCROLL;
	if ( m_hScrollEnabled && (m_hUserHScroll == nullptr) )
		styles |= WS_HSCROLL;
	cs.style = styles;
	return TRUE;
}

CWin* CControl::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CControl();

	return pWin;
}

typedef struct _tagEnumChildProcCControlBrushKeysParam
{
	HWND parent;
	Ref(CDataVectorT<CStringBuffer>) keys;

	_tagEnumChildProcCControlBrushKeysParam(HWND _parent, Ref(CDataVectorT<CStringBuffer>) _keys):
	    parent(_parent), keys(_keys) {}
} TEnumChildProcCControlBrushKeysParam;

static BOOL CALLBACK EnumChildProc_CControl_BrushKeys(HWND hwnd, LPARAM lParam)
{
	TEnumChildProcCControlBrushKeysParam* param = CastAnyPtr(TEnumChildProcCControlBrushKeysParam, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CControl* pChild = (CControl*)(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		pChild->get_BrushKeys(param->keys);
	}
	return TRUE;
}

void CControl::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	TEnumChildProcCControlBrushKeysParam param(m_hwnd, _keys);

	::EnumChildWindows(m_hwnd, EnumChildProc_CControl_BrushKeys, (LPARAM)(&param));

	AFX_MSGMAP_ENTRY* map_entry1 = _hasWindowMessage(WM_PAINT);
	AFX_MSGMAP_ENTRY* map_entry;
	bool bFound = false;

	for ( map_entry = CControl::_messagemap.entries; map_entry->msg_Func != nullptr; ++map_entry )
	{
		if ( (map_entry->msg_Type == WM_MESSAGE_TYPE) && (map_entry->msg_ID == WM_PAINT) )
		{
			bFound = (map_entry == map_entry1);
			break;
		}
	}
	if ( bFound )
	{
		CStringBuffer name(get_name());

		name.AppendString(_T(".Color.Workspace"));
		add_BrushKey(_keys, name);
		name.SetString(__FILE__LINE__ _T("Colors.Color.Workspace"));
		add_BrushKey(_keys, name);
	}
}

typedef struct _tagEnumChildProcCControlFontKeysParam
{
	HWND parent;
	Ref(CDataVectorT<CStringBuffer>) keys;

	_tagEnumChildProcCControlFontKeysParam(HWND _parent, Ref(CDataVectorT<CStringBuffer>) _keys):
	    parent(_parent), keys(_keys) {}
} TEnumChildProcCControlFontKeysParam;

static BOOL CALLBACK EnumChildProc_CControl_FontKeys(HWND hwnd, LPARAM lParam)
{
	TEnumChildProcCControlFontKeysParam* param = CastAnyPtr(TEnumChildProcCControlFontKeysParam, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CControl* pChild = (CControl*)(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		pChild->get_FontKeys(param->keys);
	}
	return TRUE;
}

void CControl::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	TEnumChildProcCControlFontKeysParam param(m_hwnd, _keys);

	::EnumChildWindows(m_hwnd, EnumChildProc_CControl_FontKeys, (LPARAM)(&param));
}

void CControl::Refresh()
{
	UpdateChildren(TRUE);
}

BOOL CControl::BeginUpdate()
{
	if (!m_hwnd)
		return FALSE;

	++m_update;
	return TRUE;
}

BOOL CControl::EndUpdate(BOOL updateRects)
{
	if (!m_hwnd)
		return FALSE;

	--m_update;
	return Update(updateRects);
}

BOOL CControl::Update(BOOL updateRects)
{
	if (!m_hwnd)
		return FALSE;

	if ( !m_update )
	{
		if ( m_updateRectsRequested )
		{
			m_updateRects = TRUE;
			m_updateRectsRequested = FALSE;
		}
		else
			m_updateRects = updateRects;
		if ( !InvalidateRect(nullptr, FALSE) )
			return FALSE;
		if ( !UpdateWindow() )
			return FALSE;
	}
	else if ( updateRects )
		m_updateRectsRequested = TRUE;
	return TRUE;
}

typedef struct _tagEnumChildProcCControlUpdateChildrenParam
{
	HWND parent;
	BOOL updateRects;

	_tagEnumChildProcCControlUpdateChildrenParam(HWND _parent, BOOL _updateRects):
	    parent(_parent), updateRects(_updateRects) {}
} TEnumChildProcCControlUpdateChildrenParam;

static BOOL CALLBACK EnumChildProc_CControl_UpdateChildren(HWND hwnd, LPARAM lParam)
{
	TEnumChildProcCControlUpdateChildrenParam* param = CastAnyPtr(TEnumChildProcCControlUpdateChildrenParam, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CControl* pChild = (CControl*)(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		pChild->set_UpdateRects(param->updateRects);
		pChild->SendMessage(WM_PAINT, 0, 0);
		//pChild->Update(param->updateRects);
	}
	return TRUE;
}

void CControl::UpdateChildren(BOOL updateRects)
{
	TEnumChildProcCControlUpdateChildrenParam param(m_hwnd, updateRects);

	::EnumChildWindows(m_hwnd, EnumChildProc_CControl_UpdateChildren, (LPARAM)(&param));
}

int CControl::get_VScrollOffset()
{ 
	if ( (!m_hwnd) || (!m_vScrollEnabled) || (!m_vScrollVisible) )
		return 0;

	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO); 
	si.fMask = SIF_POS;
	if ( m_hUserVScroll )
		GetScrollInfo (m_hUserVScroll, SB_CTL, &si);
	else
		GetScrollInfo (m_hwnd, SB_VERT, &si);
	return si.nPos; 
}

void CControl::set_VScrollOffset(int scrollOffset)
{ 
	if ( (!m_hwnd) || (!m_vScrollEnabled) || (!m_vScrollVisible) )
		return;

	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO); 
	si.fMask = SIF_POS;
	si.nPos = scrollOffset;
	if ( m_hUserVScroll )
		SetScrollInfo (m_hUserVScroll, SB_CTL, &si, TRUE);
	else
		SetScrollInfo (m_hwnd, SB_VERT, &si, TRUE);
}

int CControl::get_VScrollWidth()
{
	if ( (!m_hwnd) || (!m_vScrollEnabled) || (!m_vScrollVisible) )
		return 0;

	SCROLLBARINFO sbi;

	::SecureZeroMemory(&sbi, sizeof(SCROLLBARINFO));
	sbi.cbSize = sizeof(SCROLLBARINFO);
	if ( m_hUserVScroll )
		GetScrollBarInfo (m_hUserVScroll, OBJID_CLIENT, &sbi);
	else
		GetScrollBarInfo (m_hwnd, OBJID_VSCROLL, &sbi);
	return (sbi.rcScrollBar.right - sbi.rcScrollBar.left);
}

int CControl::get_HScrollOffset()
{ 
	if ( (!m_hwnd) || (!m_hScrollEnabled) || (!m_hScrollVisible) )
		return 0;

	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO); 
	si.fMask = SIF_POS;
	if ( m_hUserHScroll )
		GetScrollInfo (m_hUserHScroll, SB_CTL, &si);
	else
		GetScrollInfo (m_hwnd, SB_HORZ, &si);
	return si.nPos; 
}

void CControl::set_HScrollOffset(int scrollOffset)
{ 
	if ( (!m_hwnd) || (!m_hScrollEnabled) || (!m_hScrollVisible) )
		return;

	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO); 
	si.fMask = SIF_POS;
	si.nPos = scrollOffset;
	if ( m_hUserHScroll )
		SetScrollInfo (m_hUserHScroll, SB_CTL, &si, TRUE);
	else
		SetScrollInfo (m_hwnd, SB_HORZ, &si, TRUE);
}

int CControl::get_HScrollHeight()
{
	if ( (!m_hwnd) || (!m_hScrollEnabled) || (!m_hScrollVisible) )
		return 0;

	SCROLLBARINFO sbi;

	::SecureZeroMemory(&sbi, sizeof(SCROLLBARINFO));
	sbi.cbSize = sizeof(SCROLLBARINFO);
	if ( m_hUserHScroll )
		GetScrollBarInfo (m_hUserHScroll, OBJID_CLIENT, &sbi);
	else
		GetScrollBarInfo (m_hwnd, OBJID_HSCROLL, &sbi);
	return (sbi.rcScrollBar.bottom - sbi.rcScrollBar.top);
}

BOOL CControl::ShowVertScrollBar()
{
	if ( (!m_hwnd) || (!m_vScrollEnabled) )
		return TRUE;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls ShowVertScrollBar %d\n", get_name().GetString(), debugID);
#endif
	m_insideVScroll = TRUE;

	RECT clientRect;

	GetClientRect(&clientRect);
	if ( m_maxClientArea.bottom > clientRect.bottom )
	{
#ifdef __DEBUG1__
	theGuiApp->DebugString("In %ls ShowVertScrollBar %d m_maxClientArea.bottom > clientRect.bottom\n", get_name().GetString(), debugID);
#endif
		SCROLLINFO si;

		if ( !m_vScrollVisible )
		{
			if ( m_hUserVScroll )
				::ShowScrollBar(m_hUserVScroll, SB_CTL, TRUE);
			else
				::ShowScrollBar(m_hwnd, SB_VERT, TRUE);
			m_vScrollVisible = TRUE;
		}
		si.cbSize = sizeof(si); 
		si.fMask  = SIF_RANGE | SIF_PAGE; 
		si.nMin   = 0; 
		si.nMax   = m_maxClientArea.bottom; 
		si.nPage  = clientRect.bottom;
		if ( m_hUserVScroll )
			::SetScrollInfo(m_hUserVScroll, SB_CTL, &si, TRUE); 
		else
			::SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE); 
	}
	else if ( m_vScrollVisible )
	{
		if ( m_hUserVScroll )
			::ShowScrollBar(m_hUserVScroll, SB_CTL, FALSE);
		else
			::ShowScrollBar(m_hwnd, SB_VERT, FALSE);
		m_vScrollVisible = FALSE;
	}

	m_insideVScroll = FALSE;
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls ShowVertScrollBar %d\n", get_name().GetString(), debugID);
#endif

	return TRUE;
}

BOOL CControl::ShowHorzScrollBar()
{
	if ( (!m_hwnd) || (!m_hScrollEnabled) )
		return TRUE;

	m_insideHScroll = TRUE;

	RECT clientRect;

	GetClientRect(&clientRect);
	if ( m_maxClientArea.right > clientRect.right )
	{
		SCROLLINFO si;

		if ( !m_hScrollVisible )
		{
			if ( m_hUserHScroll )
				::ShowScrollBar(m_hUserHScroll, SB_CTL, TRUE);
			else
				::ShowScrollBar(m_hwnd, SB_HORZ, TRUE);
			m_hScrollVisible = TRUE;
		}
		si.cbSize = sizeof(si); 
		si.fMask  = SIF_RANGE | SIF_PAGE; 
		si.nMin   = 0; 
		si.nMax   = m_maxClientArea.right; 
		si.nPage  = clientRect.right;
		if ( m_hUserHScroll )
			::SetScrollInfo(m_hUserHScroll, SB_CTL, &si, TRUE);
		else
			::SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
	}
	else if ( m_hScrollVisible )
	{
		if ( m_hUserHScroll )
			::ShowScrollBar(m_hUserHScroll, SB_CTL, FALSE);
		else
			::ShowScrollBar(m_hwnd, SB_HORZ, FALSE);
		m_hScrollVisible = FALSE;
	}

	m_insideHScroll = FALSE;

	return TRUE;
}

LRESULT CControl::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	if ( (!m_hwnd) || (!m_hScrollEnabled) || (!m_hScrollVisible) )
		return 0;

	SCROLLINFO si;
	int xPos;

	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;
	if ( m_hUserHScroll )
		GetScrollInfo (m_hUserHScroll, SB_CTL, &si);
	else
		GetScrollInfo (m_hwnd, SB_HORZ, &si);
	xPos = si.nPos;
	switch ( LOWORD(wParam) )
	{
	case SB_LEFT:
		si.nPos = si.nMin;
		break;
	case SB_RIGHT:
		si.nPos = si.nMax;
		break;
	case SB_LINELEFT: 
		si.nPos -= 10;
		break;
	case SB_LINERIGHT: 
		si.nPos += 10;
		break;
	case SB_PAGELEFT:
		si.nPos -= si.nPage;
		break;
	case SB_PAGERIGHT:
		si.nPos += si.nPage;
		break;
	case SB_THUMBTRACK: 
		si.nPos = si.nTrackPos;
		break;
	default:
		break;
	}
	si.fMask = SIF_POS;
	if ( m_hUserHScroll )
	{
		SetScrollInfo (m_hUserHScroll, SB_CTL, &si, TRUE);
		GetScrollInfo (m_hUserHScroll, SB_CTL, &si);
	}
	else
	{
		SetScrollInfo (m_hwnd, SB_HORZ, &si, TRUE);
		GetScrollInfo (m_hwnd, SB_HORZ, &si);
	}
	return (si.nPos != xPos);
}

LRESULT CControl::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	if ( (!m_hwnd) || (!m_vScrollEnabled) || (!m_vScrollVisible) )
		return 0;

	SCROLLINFO si;
	int yPos;

	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;
	if ( m_hUserVScroll )
		GetScrollInfo (m_hUserVScroll, SB_CTL, &si);
	else
		GetScrollInfo (m_hwnd, SB_VERT, &si);
	yPos = si.nPos;
	switch ( LOWORD(wParam) )
	{
	case SB_TOP:
		si.nPos = si.nMin;
		break;
	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;
	case SB_LINEUP:
		si.nPos -= 10;
		break;
	case SB_LINEDOWN:
		si.nPos += 10;
		break;
	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;
	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	default:
		break; 
	}
	si.fMask = SIF_POS;
	if ( m_hUserVScroll )
	{
		SetScrollInfo (m_hUserVScroll, SB_CTL, &si, TRUE);
		GetScrollInfo (m_hUserVScroll, SB_CTL, &si);
	}
	else
	{
		SetScrollInfo (m_hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo (m_hwnd, SB_VERT, &si);
	}
	return (si.nPos != yPos);
}

typedef struct _tagEnumChildProcCControlRenumberLayoutParam
{
	HWND parent;
	bool xAxis;
	WORD cnt;

	_tagEnumChildProcCControlRenumberLayoutParam(HWND _parent, bool _xAxis):
	    parent(_parent), xAxis(_xAxis), cnt(0) {}
} TEnumChildProcCControlRenumberLayoutParam;

static BOOL CALLBACK EnumChildProc_CControl_RenumberLayout(HWND hwnd, LPARAM lParam)
{
	TEnumChildProcCControlRenumberLayoutParam* param = CastAnyPtr(TEnumChildProcCControlRenumberLayoutParam, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CControl* child = CastAnyPtr(CControl, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if ( param->xAxis )
			child->set_LayoutX(param->cnt);
		else
			child->set_LayoutY(param->cnt);
		++(param->cnt);
	}
	return TRUE;
}

void CControl::RenumberLayout(bool xAxis)
{
	TEnumChildProcCControlRenumberLayoutParam param(m_hwnd, xAxis);

	::EnumChildWindows(m_hwnd, EnumChildProc_CControl_RenumberLayout, (LPARAM)(&param));
	OnSize(0, 0);
}

typedef struct _tagEnumChildProcCControlCountChildrenParam
{
	HWND parent;
	WORD cnt;

	_tagEnumChildProcCControlCountChildrenParam(HWND _parent):
	    parent(_parent), cnt(0) {}
} TEnumChildProcCControlCountChildrenParam;

static BOOL CALLBACK EnumChildProc_CControl_CountChildren(HWND hwnd, LPARAM lParam)
{
	TEnumChildProcCControlCountChildrenParam* param = CastAnyPtr(TEnumChildProcCControlCountChildrenParam, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CControl* child = CastAnyPtr(CControl, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if ( child )
			++(param->cnt);
	}
	return TRUE;
}

int CControl::CountChildren()
{
	TEnumChildProcCControlCountChildrenParam param(m_hwnd);

	::EnumChildWindows(m_hwnd, EnumChildProc_CControl_CountChildren, (LPARAM)(&param));
	return param.cnt;
}

class CControlLessFunctor
{
public:
	bool operator()(ConstPtr(CControl) r1, ConstPtr(CControl) r2) const
	{
		if (r1->get_LayoutX() < r2->get_LayoutX())
			return true;
		if (r1->get_LayoutX() > r2->get_LayoutX())
			return false;
		if (r1->get_LayoutY() < r2->get_LayoutY())
			return true;
		return false;
	}
};

typedef CDataVectorT<CControl, CControlLessFunctor, CCppObjectNullFunctor<CControl>> TChildren;

typedef struct _tagEnumChildProcCControlOnSizeParam
{
	HWND parent;
	Ptr(TChildren) children;

	_tagEnumChildProcCControlOnSizeParam(HWND _parent, Ptr(TChildren) _children) :
	    parent(_parent), children(_children) {}
} TEnumChildProcCControlOnSizeParam;

static BOOL CALLBACK EnumChildProc_CControl_OnSize(HWND hwnd, LPARAM lParam)
{
	TEnumChildProcCControlOnSizeParam* param = CastAnyPtr(TEnumChildProcCControlOnSizeParam, lParam);

	if ( ::GetParent(hwnd) == param->parent )
	{
		CControl* child = CastAnyPtr(CControl, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if ( child->is_Visible() )
			param->children->Append(child);
	}
	return TRUE;
}

LRESULT CControl::OnSize(WPARAM wParam, LPARAM lParam)
{
	if ( !m_hwnd )
		return 1;
	if ( m_insideHScroll || m_insideVScroll )
		return 0;
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnSize %d\n", get_name().GetString(), debugID);
#endif
	if ( ::GetWindow(m_hwnd, GW_CHILD) )
	{
		TChildren children(__FILE__LINE__ 32, 32);
		TEnumChildProcCControlOnSizeParam param(m_hwnd, &children);

		::EnumChildWindows(m_hwnd, EnumChildProc_CControl_OnSize, (LPARAM)&param);
		if ( children.Count() > 0 )
		{
#ifdef __DEBUG1__
			theGuiApp->DebugString("%ls OnSize %d, children.Count()=%d\n", get_name().GetString(), debugID, children.Count());
#endif
			children.Sort();

			WORD fmaxX = 0;
			WORD fmaxY = 0;
			TChildren::Iterator it = children.Begin();

			while ( it )
			{
				CControl* elem = *it;

				if ( elem->get_LayoutX() > fmaxX )
					fmaxX = elem->get_LayoutX();
				if ( elem->get_LayoutY() > fmaxY )
					fmaxY = elem->get_LayoutY();
				++it;
			}

			CDataVectorT<sdword> size_X(__FILE__LINE__ fmaxX + 1, 1);
			CDataVectorT<sdword> size_Y(__FILE__LINE__ fmaxY + 1, 1);

			for ( int ix = 0; ix <= fmaxX; ++ix )
				size_X.Append(0);
			for ( int ix = 0; ix <= fmaxY; ++ix )
				size_Y.Append(0);

			int xPos = get_HScrollOffset();
			int yPos = get_VScrollOffset();
			RECT rect;
			RECT rect1;
			bool bRelativePercentSizeX = false;
			bool bRelativePercentSizeY = false;

			GetClientRect(&rect);
			::CopyRect(&rect1, &rect);
#ifdef __DEBUG1__
			theGuiApp->DebugString("%ls OnSize %d, GetClientRect.right=%d, GetClientRect.bottom=%d\n", get_name().GetString(), debugID, rect.right, rect.bottom);
#endif
			it = children.Begin();
			while ( it )
			{
				CControl* control = *it;
				RECT r; 
				
				control->get_MaxClientRect(&r);
				switch ( control->get_SizeSpecX() )
				{
				case CControl::TSizeSpecifierDefaultSize:
					if ( r.right == 0 )
						r.right = 100;
					break;
				case CControl::TSizeSpecifierFixedSize:
					r.right = control->get_LayoutSizeX();
					break;
				case CControl::TSizeSpecifierPercentSize:
					r.right = (rect.right * control->get_LayoutSizeX()) / 100;
					break;
				case CControl::TSizeSpecifierRelativePercentSize:
					bRelativePercentSizeX = true;
					r.right = 0;
					break;
				}
				if ( control->get_LayoutX() == 0 )
				{
					if ( r.right > (*(size_X.Index(0))) )
						size_X.SetData(size_X.Index(0), r.right);
				}
				else
				{
					r.left = (*(size_X.Index(control->get_LayoutX() - 1)));
					r.right += r.left;
					if ( r.right > (*(size_X.Index(control->get_LayoutX()))) )
						size_X.SetData(size_X.Index(control->get_LayoutX()), r.right);
				}
				switch ( control->get_SizeSpecY() )
				{
				case CControl::TSizeSpecifierDefaultSize:
					if ( r.bottom == 0 )
						r.bottom = 100;
					break;
				case CControl::TSizeSpecifierFixedSize:
					r.bottom = control->get_LayoutSizeY();
					break;
				case CControl::TSizeSpecifierPercentSize:
					r.bottom = (rect.bottom * control->get_LayoutSizeY()) / 100;
					break;
				case CControl::TSizeSpecifierRelativePercentSize:
					bRelativePercentSizeY = true;
					r.bottom = 0;
					break;
				}
				if ( control->get_LayoutY() == 0 )
				{
					if ( r.bottom > (*(size_Y.Index(0))) )
						size_Y.SetData(size_Y.Index(0), r.bottom);
				}
				else
				{
					r.top = (*(size_Y.Index(control->get_LayoutY() - 1)));
					r.bottom += r.top;
					if ( r.bottom > (*(size_Y.Index(control->get_LayoutY()))) )
						size_Y.SetData(size_Y.Index(control->get_LayoutY()), r.bottom);
				}
				++it;
			}
			if ( bRelativePercentSizeX )
				rect1.right -= (*(size_X.Index((fmaxX))));
			if ( bRelativePercentSizeY )
				rect1.bottom -= (*(size_X.Index((fmaxY))));
			if ( bRelativePercentSizeX || bRelativePercentSizeY )
			{
				it = children.Begin();
				while ( it )
				{
					CControl* control = *it;
					RECT r; 
				
					control->get_MaxClientRect(&r);
					switch ( control->get_SizeSpecX() )
					{
					case CControl::TSizeSpecifierDefaultSize:
						if ( r.right == 0 )
							r.right = 100;
						break;
					case CControl::TSizeSpecifierFixedSize:
						r.right = control->get_LayoutSizeX();
						break;
					case CControl::TSizeSpecifierPercentSize:
						r.right = (rect.right * control->get_LayoutSizeX()) / 100;
						break;
					case CControl::TSizeSpecifierRelativePercentSize:
						r.right = (rect1.right * control->get_LayoutSizeX()) / 100;
						break;
					}
					if ( control->get_LayoutX() == 0 )
					{
						if ( r.right > (*(size_X.Index(0))) )
							size_X.SetData(size_X.Index(0), r.right);
					}
					else
					{
						r.left = (*(size_X.Index(control->get_LayoutX() - 1)));
						r.right += r.left;
						if ( r.right > (*(size_X.Index(control->get_LayoutX()))) )
							size_X.SetData(size_X.Index(control->get_LayoutX()), r.right);
					}
					switch ( control->get_SizeSpecY() )
					{
					case CControl::TSizeSpecifierDefaultSize:
						if ( r.bottom == 0 )
							r.bottom = 100;
						break;
					case CControl::TSizeSpecifierFixedSize:
						r.bottom = control->get_LayoutSizeY();
						break;
					case CControl::TSizeSpecifierPercentSize:
						r.bottom = (rect.bottom * control->get_LayoutSizeY()) / 100;
						break;
					case CControl::TSizeSpecifierRelativePercentSize:
						r.bottom = (rect1.bottom * control->get_LayoutSizeY()) / 100;
						break;
					}
					if ( control->get_LayoutY() == 0 )
					{
						if ( r.bottom > (*(size_Y.Index(0))) )
							size_Y.SetData(size_Y.Index(0), r.bottom);
					}
					else
					{
						r.top = (*(size_Y.Index(control->get_LayoutY() - 1)));
						r.bottom += r.top;
						if ( r.bottom > (*(size_Y.Index(control->get_LayoutY()))) )
							size_Y.SetData(size_Y.Index(control->get_LayoutY()), r.bottom);
					}
					++it;
				}
			}
			it = children.Begin();
			while ( it )
			{
				CControl* control = *it;
				RECT r; 
				
				control->get_MaxClientRect(&r);
				switch ( control->get_SizeSpecX() )
				{
				case CControl::TSizeSpecifierDefaultSize:
					if ( r.right == 0 )
						r.right = 100;
					break;
				case CControl::TSizeSpecifierFixedSize:
					r.right = control->get_LayoutSizeX();
					break;
				case CControl::TSizeSpecifierPercentSize:
					r.right = (rect.right * control->get_LayoutSizeX()) / 100;
					break;
				case CControl::TSizeSpecifierRelativePercentSize:
					r.right = (rect1.right * control->get_LayoutSizeX()) / 100;
					break;
				}
				if ( control->get_LayoutX() == 0 )
				{
					if ( control->get_SizeHAlign() == TSizeHAlignRight )
					{
						r.left = rect.right - (*(size_X.Index(fmaxX)));
						r.right += r.left;
					}
				}
				else if ( control->get_SizeHAlign() == TSizeHAlignRight )
				{
					r.left = rect.right - (*(size_X.Index(fmaxX))) + (*(size_X.Index(control->get_LayoutX() - 1)));
					r.right += r.left;
				}
				else
				{
					r.left = (*(size_X.Index(control->get_LayoutX() - 1)));
					r.right += r.left;
				}
				switch ( control->get_SizeSpecY() )
				{
				case CControl::TSizeSpecifierDefaultSize:
					if ( r.bottom == 0 )
						r.bottom = 100;
					break;
				case CControl::TSizeSpecifierFixedSize:
					r.bottom = control->get_LayoutSizeY();
					break;
				case CControl::TSizeSpecifierPercentSize:
					r.bottom = (rect.bottom * control->get_LayoutSizeY()) / 100;
					break;
				case CControl::TSizeSpecifierRelativePercentSize:
					r.bottom = (rect1.bottom * control->get_LayoutSizeY()) / 100;
					break;
				}
				if ( control->get_LayoutY() == 0 )
				{
					if ( control->get_SizeVAlign() == TSizeVAlignBottom )
					{
						r.top = rect.bottom - (*(size_Y.Index(fmaxY)));
						r.bottom += r.top;
					}
				}
				else if ( control->get_SizeVAlign() == TSizeVAlignBottom )
				{
					r.top = rect.bottom - (*(size_Y.Index(fmaxY))) + (*(size_Y.Index(control->get_LayoutY() - 1)));
					r.bottom += r.top;
				}
				else
				{
					r.top = (*(size_Y.Index(control->get_LayoutY() - 1)));
					r.bottom += r.top;
				}
				::OffsetRect(&r, -xPos, -yPos);
#ifdef __DEBUG1__
				theGuiApp->DebugString("%ls OnSize %d, before Movewindow, control:%ls, r.left=%d, r.right=%d, r.top=%d, r.bottom=%d, \n", 
					get_name().GetString(), debugID, control->get_name().GetString(), r.left, r.right, r.top, r.bottom);
#endif
				control->MoveWindow(&r, TRUE);
#ifdef __DEBUG1__
				theGuiApp->DebugString("%ls OnSize %d, after Movewindow\n", get_name().GetString(), debugID);
#endif
				++it;
			}
			::SetRectEmpty(&m_maxClientArea);
			m_maxClientArea.right = (*(size_X.Index((fmaxX))));
			m_maxClientArea.bottom = (*(size_Y.Index((fmaxY))));

#ifdef __DEBUG1__
			theGuiApp->DebugString("%ls OnSize %d, m_maxClientArea.right=%d, m_maxClientArea.bottom=%d\n", get_name().GetString(), debugID,
				m_maxClientArea.right, m_maxClientArea.bottom);
#endif
		}
		children.Close();
	}
	if ( IsRectEmpty(&m_maxClientArea) )
	{
#ifdef __DEBUG1__
		theGuiApp->DebugString("After %ls OnSize %d, IsRectEmpty(&m_maxClientArea)\n", get_name().GetString(), debugID);
#endif
		return 1;
	}
	if ( !ShowVertScrollBar() )
	{
#ifdef __DEBUG1__
		theGuiApp->DebugString("After %ls OnSize %d, !ShowVertScrollBar()\n", get_name().GetString(), debugID);
#endif
		return 1;
	}
	if ( !ShowHorzScrollBar() )
	{
#ifdef __DEBUG1__
		theGuiApp->DebugString("After %ls OnSize %d, !ShowHorzScrollBar()\n", get_name().GetString(), debugID);
#endif
		return 1;
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnSize %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

void CControl::PaintWorkspace(Gdiplus::Graphics* graphics, Gdiplus::Brush* brush)
{
	RECT r;
	RECT r1;
	RECT r2;

	GetClientRect(&r);
	::SetRectEmpty(&r1);
	::SetRectEmpty(&r2);

	if ( m_maxClientArea.right < r.right )
	{
		if ( m_maxClientArea.bottom < r.bottom )
		{
			r1.left = m_maxClientArea.right;
			r1.right = r.right;
			r1.bottom = m_maxClientArea.bottom;
			r2.top = m_maxClientArea.bottom;
			r2.bottom = r.bottom;
			r2.right = r.right;
		}
		else
		{
			r1.left = m_maxClientArea.right;
			r1.right = r.right;
			r1.bottom = r.bottom;
		}
	}
	else if ( m_maxClientArea.bottom < r.bottom )
	{
		r1.right = r.right;
		r1.top = m_maxClientArea.bottom;
		r1.bottom = r.bottom;
	}
	if ( !(::IsRectEmpty(&r1)) )
		graphics->FillRectangle(brush, r1.left, r1.top, r1.right - r1.left, r1.bottom - r1.top);
	if ( !(::IsRectEmpty(&r2)) )
		graphics->FillRectangle(brush, r2.left, r2.top, r2.right - r2.left, r2.bottom - r2.top);
}

LRESULT CControl::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = ::BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;

	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Brush* brush;
	CStringBuffer name(get_name());

	name.AppendString(_T(".Color.Workspace"));
	if ( theGuiApp->get_BrushManager()->has_Brush(name) )
		brush = theGuiApp->get_BrushManager()->get_Brush(name, Gdiplus::Color::Gray);
	else
		brush = theGuiApp->get_BrushManager()->get_Brush(_T("Colors.Color.Workspace"), Gdiplus::Color::Gray);

	PaintWorkspace(&graphics, brush);

	::EndPaint(m_hwnd, &ps);
	m_updateRects = FALSE;
	return 0;
}

//***********************************************************
// CPanel
//***********************************************************
BEGIN_MESSAGE_MAP(CControl, CPanel)
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

CPanel::CPanel(LPCTSTR name): CControl(name)
{
}

CPanel::CPanel(ConstRef(CStringBuffer) name): CControl(name)
{
}

CPanel::~CPanel()
{}

BOOL CPanel::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("CPANEL");
	return TRUE;
}

LRESULT CPanel::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	if ( (!fwKeys) && zDelta )
	{
		if ( get_VScrollVisible() )
		{
			int pos = get_VScrollOffset();

			pos -= zDelta;
			set_VScrollOffset(pos);
			return CControl::OnSize(0, 0);
		}
		if ( get_HScrollVisible() )
		{
			int pos = get_HScrollOffset();

			pos -= zDelta;
			set_HScrollOffset(pos);
			return CControl::OnSize(0, 0);
		}
	}
	return DefaultWindowProc(WM_MOUSEWHEEL, wParam, lParam);
}

static BOOL CALLBACK EnumChildProc_CPanel_OnHScroll(HWND hwnd, LPARAM lParam)
{
	if ( ::GetParent(hwnd) == (HWND)lParam )
		::SendMessage(hwnd, WM_ABOUTHSCROLL, 0, 0);
	return TRUE;
}

LRESULT CPanel::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	::EnumChildWindows(m_hwnd, EnumChildProc_CPanel_OnHScroll, (LPARAM)m_hwnd);
	if ( CControl::OnHScroll(wParam, lParam) )
		return CControl::OnSize(0, 0);
	return 0;
}

static BOOL CALLBACK EnumChildProc_CPanel_OnVScroll(HWND hwnd, LPARAM lParam)
{
	if ( ::GetParent(hwnd) == (HWND)lParam )
		::SendMessage(hwnd, WM_ABOUTVSCROLL, 0, 0);
	return TRUE;
}

LRESULT CPanel::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	::EnumChildWindows(m_hwnd, EnumChildProc_CPanel_OnVScroll, (LPARAM)m_hwnd);
	if ( CControl::OnVScroll(wParam, lParam) )
		return CControl::OnSize(0, 0);
	return 0;
}

//***********************************************************
// CToolbar
//***********************************************************
BEGIN_MESSAGE_MAP(CControl, CToolbar)
	ON_WM_PAINT()
	ON_WM_NOTIFY(NM_BUTTONCLICKED, OnNotifyButtonClicked)
END_MESSAGE_MAP()

CToolbar::CToolbar(LPCTSTR name): 
    CControl(name),
	m_cnt(0)
{
}

CToolbar::CToolbar(ConstRef(CStringBuffer) name): 
    CControl(name),
	m_cnt(0)
{
}

CToolbar::~CToolbar()
{
}

BOOL CToolbar::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("CTOOLBAR");
	return TRUE;
}

void CToolbar::AddButtonControl(UINT commandcode)
{
	if ( m_cnt >= m_imagesnormal.get_bitmapcnt() )
		return;

	CStringBuffer name;
	RECT margins = { 2, 2, 2, 2 };

	name.FormatString(__FILE__LINE__ _T("%s.Button%d"), m_name.GetString(), commandcode);

	CStatic* pButton = OK_NEW_OPERATOR CStatic(name);
	CBitmap iconN(m_imagesnormal.get_bitmap(m_cnt));
	CBitmap iconD(m_imagesdisabled.get_bitmap(m_cnt));
	CBitmap iconH(m_imageshot.get_bitmap(m_cnt));

	pButton->set_iconnormal(iconN);
	pButton->set_icondisabled(iconD);
	pButton->set_iconhot(iconH);

	pButton->set_Margins(margins);
	pButton->set_LayoutX(Castword(m_cnt));
	pButton->set_halign(CStatic::hcenter);
	pButton->set_valign(CStatic::vcenter);
	pButton->set_style(CStatic::sToolbarButton);

	pButton->Create(m_hwnd, commandcode);
	++m_cnt;
}

void CToolbar::AutoSize()
{
	RECT r;

	OnSize(0, 0);
	get_parent()->GetClientRect(&r);
	r.bottom = m_maxClientArea.bottom;
	MoveWindow(&r, TRUE);
}

void CToolbar::set_ChildEnabled(UINT commandcode, BOOL bEnable)
{
	CControl* pCtrl = CastDynamicPtr(CControl, get_childbyID(commandcode));

	if ( pCtrl )
	{
		pCtrl->set_Enabled(bEnable);
		pCtrl->Update(FALSE);
	}
}

LRESULT CToolbar::OnNotifyButtonClicked(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR phdr = (LPNMHDR)lParam;

	get_frame()->SendMessage(WM_COMMAND, MAKEWPARAM(phdr->idFrom, 0), 0);
	return 0;
}

LRESULT CToolbar::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);

	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Brush* pBackgroundBrush = get_Brush(_T(".BackgroundColor.Normal"), _T("Toolbar"), Gdiplus::Color::White);
	RECT r;

	GetClientRect(&r);
	graphics.FillRectangle(pBackgroundBrush, r.left, r.top, r.right - r.left, r.bottom - r.top);

	::EndPaint(m_hwnd, &ps);
	m_updateRects = FALSE;
	return 0;
}

//***********************************************************
// CStatusbar
//***********************************************************
static HHOOK hKeyboardProc_StatusInfo = nullptr;

static LRESULT CALLBACK KeyboardProc_StatusInfo(int code, WPARAM wParam, LPARAM lParam)
{
	if ( code < 0 )
		return ::CallNextHookEx(hKeyboardProc_StatusInfo, code, wParam, lParam);

	switch ( wParam )
	{
	case VK_CAPITAL:
	case VK_NUMLOCK:
		{
			SHORT vNUM = ::GetKeyState(VK_NUMLOCK);
			SHORT vCaps = ::GetKeyState(VK_CAPITAL);
			CStringBuffer buf;

			if ( vNUM & 1 )
				buf.AppendString(_T("NUM "));
			if ( vCaps & 1 )
				buf.AppendString(_T("CAPS "));
			if ( buf.IsEmpty() )
				buf.AppendString(_T("  "));

			CWin* pFrameWin = theGuiApp->get_MainWnd();

			switch ( pFrameWin->get_windowtype() )
			{
			case CWin::TFrameWindow:
				{
					CFrameWnd* pFrame = CastDynamicPtr(CFrameWnd, pFrameWin);

					if ( pFrame )
					{
						CStatusbar* pStatusbar = pFrame->get_StatusBar();

						if ( pFrame->is_StatusBarEnabled() )
							pStatusbar->set_text(CStatusbar::TStatusStyleKeyboardStatus, buf);
					}
				}
				break;
			case CWin::TMDIWindow:
				{
					CMDIFrame* pFrame = CastDynamicPtr(CMDIFrame, pFrameWin);

					if ( pFrame )
					{
						CStatusbar* pStatusbar = pFrame->get_StatusBar();

						if ( pFrame->is_StatusBarEnabled() )
							pStatusbar->set_text(CStatusbar::TStatusStyleKeyboardStatus, buf);
					}
				}
				break;
			}
		}
		break;
	}
	return ::CallNextHookEx(hKeyboardProc_StatusInfo, code, wParam, lParam);
}

BEGIN_MESSAGE_MAP(CControl, CStatusbar)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

Ref(CStatusbar::TStatusInfoReleaseFunctor) CStatusbar::TStatusInfoReleaseFunctor::operator()(Ptr(TStatusInfo) pData)
{
	switch (pData->style)
	{
	case CStatusbar::TStatusStyleClock:
		pStatusbar->StopTimer(1000);
		break;
	case CStatusbar::TStatusStyleKeyboardStatus:
		::UnhookWindowsHookEx(hKeyboardProc_StatusInfo);
		break;
	}
	pData->defaultText.Clear();
	pData->release();
	return *this;
}

CStatusbar::CStatusbar(LPCTSTR name) :
    CControl(name),
	m_statusinfo(__FILE__LINE__ 16, 16)
{
}

CStatusbar::CStatusbar(ConstRef(CStringBuffer) name): 
    CControl(name),
	m_statusinfo(__FILE__LINE__ 16, 16)
{
}

CStatusbar::~CStatusbar()
{
}

BOOL CStatusbar::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = _T("CSTATUSBAR");
	return TRUE;
}

void CStatusbar::AutoSize()
{
	RECT r;

	OnSize(0, 0);
	get_parent()->GetClientRect(&r);
	r.top = r.bottom - m_maxClientArea.bottom;
	MoveWindow(&r, TRUE);
}

UINT CStatusbar::add_item(TStatusStyle style, UINT width, LPCTSTR defaulttext, int lench)
{
	CStringBuffer sText(__FILE__LINE__ defaulttext, lench);

	return add_item(style, width, sText);
}

UINT CStatusbar::add_item(TStatusStyle style, UINT width, ConstRef(CStringBuffer) defaulttext)
{
	Ptr(TStatusInfo) info = OK_NEW_OPERATOR TStatusInfo;
	CStringBuffer name;
	dword cnt = m_statusinfo.Count();

	name.FormatString(__FILE__LINE__ _T("%s.StatusItem%d"), m_name.GetString(), cnt);
	
	CStatic* pStatic = OK_NEW_OPERATOR CStatic(name);

	pStatic->set_LayoutX(Castword(cnt));
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(width);
	pStatic->set_text(defaulttext);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_style(CStatic::sLabel);
	switch ( style )
	{
	case TStatusStyleKeyboardStatus:
		{
			SHORT vNUM = ::GetKeyState(VK_NUMLOCK);
			SHORT vCaps = ::GetKeyState(VK_CAPITAL);
			CStringBuffer buf;

			if ( vNUM & 1 )
				buf.AppendString(_T("NUM "));
			if ( vCaps & 1 )
				buf.AppendString(_T("CAPS "));
			if ( buf.IsEmpty() )
				buf.AppendString(_T("  "));
			pStatic->set_text(buf);
			pStatic->set_SizeHAlign(CControl::TSizeHAlignRight);
		}
		break;
	case TStatusStyleClock:
		pStatic->set_SizeHAlign(CControl::TSizeHAlignRight);
		break;
	}
	pStatic->Create(m_hwnd, cnt + 1000);

	info->style = style;
	info->width = width;
	info->defaultText = defaulttext;
	info->pControl = CastDynamicPtr(CControl, pStatic);

	m_statusinfo.Append(info);

	switch ( style )
	{
	case TStatusStyleKeyboardStatus:
		hKeyboardProc_StatusInfo = ::SetWindowsHookEx(WH_KEYBOARD, KeyboardProc_StatusInfo, nullptr, ::GetCurrentThreadId());
		break;
	case TStatusStyleClock:
		StartTimer(1000, 1000);
		break;
	}

	return cnt;
}

void CStatusbar::set_text(CStatusbar::TStatusStyle style, LPCTSTR text, int lench)
{
	CStringBuffer sText(__FILE__LINE__ text, lench);

	set_text(style, sText);
}

void CStatusbar::set_text(CStatusbar::TStatusStyle style, ConstRef(CStringBuffer) text)
{
	TStatusInfo info;

	info.style = style;

	TStatusInfoVector::Iterator it = m_statusinfo.FindSorted(&info);

	if (it)
	{
		CStatic* pStatic = CastDynamicPtr(CStatic, (*it)->pControl);

		if ( text.IsEmpty() )
			pStatic->set_text((*it)->defaultText);
		else
			pStatic->set_text(text);
	}
	Update(TRUE);
}

void CStatusbar::set_text(UINT ix, LPCTSTR text, int lench)
{
	CStringBuffer sText(__FILE__LINE__ text, lench);

	set_text(ix, sText);
}

void CStatusbar::set_text(UINT ix, ConstRef(CStringBuffer) text)
{
	TStatusInfoVector::Iterator it = m_statusinfo.Index(ix);

	if ( it )
	{
		CStatic* pStatic = CastDynamicPtr(CStatic, (*it)->pControl);

		if ( text.IsEmpty() )
			pStatic->set_text((*it)->defaultText);
		else
			pStatic->set_text(text);
	}
	Update(TRUE);
}

LRESULT CStatusbar::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 1000 )
	{
		SYSTEMTIME tm;
		CStringBuffer buf;

		::GetLocalTime(&tm);
		buf.FormatString(__FILE__LINE__ _T("%02d.%02d.%04d %02d:%02d:%02d"), tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond);
		set_text(TStatusStyleClock, buf);
	}
	return 0;
}

LRESULT CStatusbar::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	TStatusInfoReleaseFunctor rD;

	rD.pStatusbar = this;
	rD.hKeyboardProc_StatusInfo = hKeyboardProc_StatusInfo;
	m_statusinfo.Close(rD);
	return 0;
}

LRESULT CStatusbar::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);

	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Brush* pBackgroundBrush = get_Brush(_T(".BackgroundColor.Normal"), _T("Statusbar"), Gdiplus::Color::White);
	RECT r;

	GetClientRect(&r);
	graphics.FillRectangle(pBackgroundBrush, r.left, r.top, r.right - r.left, r.bottom - r.top);

	::EndPaint(m_hwnd, &ps);
	m_updateRects = FALSE;
	return 0;
}

//***********************************************************
// CPopupMenu
//***********************************************************
BEGIN_MESSAGE_MAP(CPopup, CPopupMenu)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CAPTURECHANGED()
	ON_WM_NOTIFY(NM_BUTTONMOUSEMOVE, OnNotifyButtonMove)
END_MESSAGE_MAP()

CPopupMenu::CPopupMenu(LPCTSTR name): 
	CPopup(name),
	m_menutype(TMenuTypeUser),
	m_fixeditemcnt(0),
	m_itemcnt(0),
	m_pMenubar(nullptr),
	m_pTrackWindow(nullptr),
	m_hasFirstAutoSize(FALSE),
	m_pCurrentMenuItem(nullptr),
	m_hasCaptureReleaseExpected(FALSE)
{
}

CPopupMenu::CPopupMenu(ConstRef(CStringBuffer) name): 
	CPopup(name),
	m_menutype(TMenuTypeUser),
	m_fixeditemcnt(0),
	m_itemcnt(0),
	m_pMenubar(nullptr),
	m_pTrackWindow(nullptr),
	m_hasFirstAutoSize(FALSE),
	m_pCurrentMenuItem(nullptr),
	m_hasCaptureReleaseExpected(FALSE)
{
}

CPopupMenu::~CPopupMenu()
{}

BOOL CPopupMenu::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("CPOPUPMENU");
	return TRUE;
}

void CPopupMenu::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	if ( m_panel )
		m_panel->get_BrushKeys(_keys);
}

void CPopupMenu::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	if ( m_panel )
		m_panel->get_FontKeys(_keys);
}

void CPopupMenu::Refresh()
{
	if ( m_panel )
		m_panel->Refresh();
}

typedef struct _tagEnumChildProcFindMnemonicParam
{
	HWND parent;
	UINT vkey;
	CControl* pControl;

	_tagEnumChildProcFindMnemonicParam(HWND _parent, UINT _vkey):
	    parent(_parent), vkey(_vkey), pControl(nullptr) {}

} TEnumChildProcFindMnemonicParam;

static BOOL CALLBACK EnumChildProc_FindMnemonic(HWND hwnd, LPARAM lParam)
{
	Ptr(TEnumChildProcFindMnemonicParam) pParam = CastAnyPtr(TEnumChildProcFindMnemonicParam, lParam);

	if ( ::GetParent(hwnd) == pParam->parent )
	{
		CWin* pWin = CastAnyPtr(CWin, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));
		CStatic* pStatic;

		if ( pStatic = CastDynamicPtr(CStatic, pWin) )
		{
			if ( pStatic->get_Mnemonic() == pParam->vkey )
			{
				pParam->pControl = CastDynamicPtr(CControl, pStatic);
				return FALSE;
			}
		}
	}
	return TRUE;
}

void CPopupMenu::set_hovered(bool bHovered)
{
	CStatic* pStatic = CastDynamicPtr(CStatic, m_pCurrentMenuItem);

	if ( pStatic )
	{
		if ( ((pStatic->is_hovered()) && (!bHovered)) || ((!(pStatic->is_hovered())) && bHovered) )
		{
			if ( bHovered )
				pStatic->set_clicked(false);
			pStatic->set_hovered(bHovered);
			pStatic->Update(FALSE);
			if ( bHovered )
				SendNotifyMessage(NM_MENUITEMHOVERED, (LPARAM)pStatic);
		}
	}
}

void CPopupMenu::set_clicked(bool bClicked)
{
	CStatic* pStatic = CastDynamicPtr(CStatic, m_pCurrentMenuItem);

	if ( pStatic )
	{
		if ( ((pStatic->is_clicked()) && (!bClicked)) || ((!(pStatic->is_clicked())) && bClicked) )
		{
			if ( bClicked )
				pStatic->set_hovered(false);
			pStatic->set_clicked(bClicked);
			pStatic->Update(FALSE);
		}
	}
}

void CPopupMenu::CreateFromHMENU(HMENU hmenu)
{
	int mCnt = ::GetMenuItemCount(hmenu);
	CBitmap emptyimage(MAKEINTRESOURCE(IDB_EMPTYBITMAP), IMAGE_BITMAP);

	for ( int ix = 0; ix < mCnt; ++ix )
	{
		MENUITEMINFO mi;
		CStringBuffer name;

		name.FormatString(__FILE__LINE__ _T("%s.MenuItem%d"), m_name.GetString(), ix);
		::SecureZeroMemory(&mi, sizeof(MENUITEMINFO));
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask = MIIM_STRING;
		if ( ::GetMenuItemInfo(hmenu, ix, TRUE, &mi) && (mi.cch > 0) )
		{
			mi.cch++;

			CStringBuffer mText(__FILE__LINE__ mi.cch);

			mi.dwTypeData = CastAny(LPTSTR, CastMutable(CPointer, mText.GetString()));
			mi.fMask |= MIIM_ID;
			::GetMenuItemInfo(hmenu, ix, TRUE, &mi);

			CStatic* pStatic = OK_NEW_OPERATOR CStatic(name);

			pStatic->set_LayoutY(ix);
			pStatic->set_text(mText);
			pStatic->set_position(CStatic::pright);
			pStatic->set_halign(CStatic::hleft);
			pStatic->set_valign(CStatic::vcenter);
			pStatic->set_style(CStatic::sMenuItem);
			pStatic->set_iconnormal(emptyimage);
			pStatic->set_CreateVisible(FALSE);
			pStatic->Create(m_panel->get_handle(), mi.wID);
		}
		else // separator
		{
			CStatic* pStatic = OK_NEW_OPERATOR CStatic(name);

			pStatic->set_LayoutY(ix);
			pStatic->set_position(CStatic::pright);
			pStatic->set_halign(CStatic::hleft);
			pStatic->set_valign(CStatic::vcenter);
			pStatic->set_style(CStatic::sMenuItem);
			pStatic->set_iconnormal(emptyimage);
			pStatic->set_CreateVisible(FALSE);
			pStatic->Create(m_panel->get_handle(), 0);
		}
	}
	m_fixeditemcnt = mCnt;
	m_itemcnt = mCnt;
}

void CPopupMenu::TakeOverCapture()
{
	set_Capture(TRUE);
	m_hasCaptureReleaseExpected = FALSE;
}

static BOOL CALLBACK EnumChildProc_CMenu_AutoSize(HWND hwnd, LPARAM lParam)
{
	CControl* panel = (CControl*)lParam;

	if ( ::GetParent(hwnd) == panel->get_handle() )
	{
		CControl* pMenuItem = CastAnyPtr(CControl, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));
		RECT r1; panel->get_MaxClientRect(&r1);
		RECT r2; pMenuItem->get_MaxClientRect(&r2);

		pMenuItem->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
		pMenuItem->set_LayoutSizeX(r1.right);

		pMenuItem->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
		pMenuItem->set_LayoutSizeY(r2.bottom);

		pMenuItem->Update(TRUE);
	}
	return TRUE;
}

void CPopupMenu::AutoSize(POINT pt)
{
	RECT r;
	RECT r2;

	set_Visible(TRUE);
	if ( !m_hasFirstAutoSize )
	{
		m_panel->set_Visible(TRUE, TRUE);
		::SetRect(&r, -500, -500, 0, 0);
		MoveWindow(&r);
		OnSize(0, 0);
		m_panel->UpdateChildren(TRUE);
		m_panel->OnSize(0, 0);
		::EnumChildWindows(m_panel->get_handle(), EnumChildProc_CMenu_AutoSize, (LPARAM)m_panel);
		m_panel->OnSize(0, 0);
		m_panel->get_MaxClientRect(&r2);
		::SetRect(&r, 0, 0, r2.right, r2.bottom);
		m_hasFirstAutoSize = TRUE;
	}
	else
	{
		GetWindowRect(&r2);
		::SetRect(&r, 0, 0, r2.right - r2.left, r2.bottom - r2.top);
	}
	::OffsetRect(&r, pt.x, pt.y);
	MoveWindow(&r, TRUE);
}

void CPopupMenu::Reset()
{
	if ( m_pCurrentMenuItem )
	{
		set_hovered(false);
		set_clicked(false);
		m_pCurrentMenuItem = nullptr;
	}
	set_Visible(FALSE);
}

bool CPopupMenu::TestMnemonic(UINT vKey)
{
	TEnumChildProcFindMnemonicParam param(m_panel->get_handle(), vKey);

	::EnumChildWindows(m_panel->get_handle(), EnumChildProc_FindMnemonic, (LPARAM)(&param));
	return param.pControl != nullptr;
}

CControl* CPopupMenu::get_menuItem(UINT commandCode)
{
	CWin* pCtrl = m_panel->get_childbyID(commandCode);

	if ( !pCtrl )
		return nullptr;
	return CastDynamicPtr(CControl, pCtrl);
}

void CPopupMenu::add_menuitem(UINT commandCode, LPCTSTR text, int chlen)
{
	CStringBuffer sText(__FILE__LINE__ text, chlen);

	add_menuitem(commandCode, sText);
}

void CPopupMenu::add_menuitem(UINT commandCode, ConstRef(CStringBuffer) text)
{
	CBitmap emptyimage(MAKEINTRESOURCE(IDB_EMPTYBITMAP), IMAGE_BITMAP);
	CStringBuffer name;

	name.FormatString(__FILE__LINE__ _T("%s.MenuItem%d"), m_name.GetString(), m_itemcnt);
	
	CStatic* pStatic = OK_NEW_OPERATOR CStatic(name);

	pStatic->set_LayoutY(m_itemcnt);
	if ( !(text.IsEmpty()) )
		pStatic->set_text(text);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_style(CStatic::sMenuItem);
	pStatic->set_iconnormal(emptyimage);
	pStatic->set_CreateVisible(FALSE);
	pStatic->Create(m_panel->get_handle(), commandCode);
	if ( text.IsEmpty() )
		pStatic->set_Enabled(FALSE);
	++m_itemcnt;
	m_hasFirstAutoSize = FALSE;
}

typedef struct _tagEnumChildProcCMenuDestroyItemsParam
{
	HWND parent;
	int fixedcnt;

	_tagEnumChildProcCMenuDestroyItemsParam(HWND _parent, int _fixedcnt):
	    parent(_parent), fixedcnt(_fixedcnt) {}

} TEnumChildProcCMenuDestroyItemsParam;

static BOOL CALLBACK EnumChildProc_CMenu_DestroyItems(HWND hwnd, LPARAM lParam)
{
	Ptr(TEnumChildProcCMenuDestroyItemsParam) pParam = CastAnyPtr(TEnumChildProcCMenuDestroyItemsParam, lParam);

	if ( ::GetParent(hwnd) == pParam->parent )
	{
		CWin* pWin = CastAnyPtr(CWin, ::GetWindowLongPtr(hwnd, GWLP_USERDATA));
		CControl* pControl;

		if ( pControl = CastDynamicPtr(CControl, pWin) )
		{
			if ( pControl->get_LayoutY() > pParam->fixedcnt )
				pControl->DestroyWindow();
		}
	}
	return TRUE;
}

void CPopupMenu::reset_menuitems(void)
{
	TEnumChildProcCMenuDestroyItemsParam param(m_panel->get_handle(), m_fixeditemcnt);

	::EnumChildWindows(m_panel->get_handle(), EnumChildProc_CMenu_DestroyItems, (LPARAM)(&param));
	m_itemcnt = m_fixeditemcnt;
	m_hasFirstAutoSize = FALSE;
}

LRESULT CPopupMenu::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnLButtonDown %d\n", get_name().GetString(), debugID);
#endif
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	ClientToScreen(&pt);
	if ( has_Capture() )
	{
		if ( m_pCurrentMenuItem )
		{
			RECT r;

			set_hovered(false);
			m_pCurrentMenuItem->GetWindowRect(&r);
			if ( PtInRect(&r, pt) )
			{
				if ( m_pCurrentMenuItem->is_Enabled() )
					set_clicked(true);
				return 0;
			}
		}
	}
	return 0;
}

LRESULT CPopupMenu::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnLButtonUp %d\n", get_name().GetString(), debugID);
#endif
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	ClientToScreen(&pt);
	if ( has_Capture() )
	{
		if ( m_pCurrentMenuItem )
		{
			RECT r;

			set_clicked(false);
			m_pCurrentMenuItem->GetWindowRect(&r);
			if ( PtInRect(&r, pt) )
			{
				if ( m_pCurrentMenuItem->is_Enabled() )
				{
					UINT childID = m_pCurrentMenuItem->get_childID();

					m_hasCaptureReleaseExpected = TRUE;
					set_Capture(FALSE);
					m_hasCaptureReleaseExpected = FALSE;
					Reset();
					if ( m_pMenubar )
					{
						m_pMenubar->Reset();
						get_frame()->SendMessage(WM_COMMAND, MAKEWPARAM(childID, 0), 0);
					}
					else if ( m_pTrackWindow )
						m_pTrackWindow->SendMessage(WM_COMMAND, MAKEWPARAM(childID, 0), 0);
				}
				return 0;
			}
		}
		m_hasCaptureReleaseExpected = TRUE;
		set_Capture(FALSE);
		m_hasCaptureReleaseExpected = FALSE;
		Reset();
		if ( m_pMenubar )
			m_pMenubar->Reset();
	}
	return 0;
}

LRESULT CPopupMenu::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnMouseMove %d\n", get_name().GetString(), debugID);
#endif
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	ClientToScreen(&pt);
	if ( has_Capture() )
	{
		CControl* pCtrl;
		CControl* svCurrentMenuItem = m_pCurrentMenuItem;

		if ( m_pCurrentMenuItem )
		{
			RECT r;

			m_pCurrentMenuItem->GetWindowRect(&r);
			if ( ::PtInRect(&r, pt) )
			{
				set_hovered(true);
				return 0;
			}

			pCtrl = (CControl*)(m_pCurrentMenuItem->get_siblingByCmd(GW_HWNDPREV));
			if ( pCtrl )
			{
				pCtrl->GetWindowRect(&r);
				if ( ::PtInRect(&r, pt) )
				{
					set_hovered(false);
					m_pCurrentMenuItem = pCtrl;
					set_hovered(true);
					return 0;
				}
			}
			pCtrl = (CControl*)(m_pCurrentMenuItem->get_siblingByCmd(GW_HWNDNEXT));
			if ( pCtrl )
			{
				pCtrl->GetWindowRect(&r);
				if ( ::PtInRect(&r, pt) )
				{
					set_hovered(false);
					m_pCurrentMenuItem = pCtrl;
					set_hovered(true);
					return 0;
				}
			}
			m_pCurrentMenuItem = nullptr;
		}
		if ( !m_pCurrentMenuItem )
		{
			pCtrl = (CControl*)(m_panel->get_childFromPt(pt));

			if ( pCtrl )
			{
				m_pCurrentMenuItem = svCurrentMenuItem;
				if ( m_pCurrentMenuItem != pCtrl )
					set_hovered(false);
				m_pCurrentMenuItem = pCtrl;
				set_hovered(true);
				return 0;
			}
		}
		if ( !m_pCurrentMenuItem )
		{
			if ( m_pMenubar )
			{
				m_hasCaptureReleaseExpected = TRUE;
				m_pMenubar->TakeOverCapture();
				m_hasCaptureReleaseExpected = FALSE;
			}
		}
		if ( !m_pCurrentMenuItem )
			m_pCurrentMenuItem = svCurrentMenuItem;
	}
	return 0;
}

LRESULT CPopupMenu::OnNotifyButtonMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNotifyButtonMove %d\n", get_name().GetString(), debugID);
#endif
	Ptr(NotifyMessage) phdr = CastAnyPtr(NotifyMessage, lParam);

	m_pCurrentMenuItem = CastAnyPtr(CControl, phdr->param);
	set_hovered(true);
	set_Capture(TRUE);
	m_hasCaptureReleaseExpected = FALSE;
	return 0;
}

LRESULT CPopupMenu::OnCaptureChanged(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnCaptureChanged %d, hasCaptureReleaseExpected=%d\n", get_name().GetString(), debugID, m_hasCaptureReleaseExpected);
#endif
	if ( !m_hasCaptureReleaseExpected )
		Reset();
	m_hasCaptureReleaseExpected = FALSE;
	return 0;
}

LRESULT CPopupMenu::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls Before OnKeyDown %d, wParam=%x\n", get_name().GetString(), debugID, wParam);
#endif
	if ( bShift || bControl )
		return 0;
	if ( !has_Capture() )
		set_Capture(TRUE);
	if ( !has_focus() )
		set_focus();
	switch ( wParam )
	{
	case VK_RIGHT:
	case VK_LEFT:
		if ( m_pMenubar )
		{
			if ( has_Capture() )
			{
				m_hasCaptureReleaseExpected = TRUE;
				set_Capture(FALSE);
				m_hasCaptureReleaseExpected = FALSE;
			}
			m_pMenubar->PostMessage(WM_KEYDOWN, wParam, lParam);
		}
		break;
	case VK_RETURN:
	case VK_SPACE:
		if ( m_pCurrentMenuItem && (m_pCurrentMenuItem->is_Enabled()) )
		{
			UINT childID = m_pCurrentMenuItem->get_childID();

			if ( has_Capture() )
			{
				m_hasCaptureReleaseExpected = TRUE;
				set_Capture(FALSE);
				m_hasCaptureReleaseExpected = FALSE;
			}
			Reset();
			if ( m_pMenubar )
			{
				m_pMenubar->Reset();
				get_frame()->SendMessage(WM_COMMAND, MAKEWPARAM(childID, 0), 0);
			}
			else if ( m_pTrackWindow )
				m_pTrackWindow->SendMessage(WM_COMMAND, MAKEWPARAM(childID, 0), 0);
		}
		break;
	case VK_DOWN:
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls OnKeyDown %d, VK_DOWN, m_pCurrentMenuItem=%x\n", get_name().GetString(), debugID, m_pCurrentMenuItem);
#endif
		if ( m_pCurrentMenuItem )
		{
			CControl* pCtrl = (CControl*)(m_pCurrentMenuItem->get_siblingByCmd(GW_HWNDNEXT));

			set_hovered(false);
			if ( pCtrl )
			{
				m_pCurrentMenuItem = pCtrl;
				set_hovered(true);
				break;
			}
		}
		{
			CControl* pCtrl = (CControl*)(m_panel->get_firstchild());

			if ( pCtrl )
			{
				m_pCurrentMenuItem = pCtrl;
				set_hovered(true);
			}
		}
		break;
	case VK_UP:
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls OnKeyDown %d, VK_UP, m_pCurrentMenuItem=%x\n", get_name().GetString(), debugID, m_pCurrentMenuItem);
#endif
		if ( m_pCurrentMenuItem )
		{
			CControl* pCtrl = (CControl*)(m_pCurrentMenuItem->get_siblingByCmd(GW_HWNDPREV));

			set_hovered(false);
			if ( pCtrl )
			{
				m_pCurrentMenuItem = pCtrl;
				set_hovered(true);
				break;
			}
		}
		{
			CControl* pCtrl = (CControl*)(m_panel->get_firstchild());

			if ( pCtrl )
			{
				pCtrl = (CControl*)(pCtrl->get_siblingByCmd(GW_HWNDLAST));

				if ( pCtrl )
				{
					m_pCurrentMenuItem = pCtrl;
					set_hovered(true);
				}
			}
		}
		break;
	case VK_ESCAPE:
		if ( m_pMenubar )
		{
			if ( has_Capture() )
			{
				m_hasCaptureReleaseExpected = TRUE;
				set_Capture(FALSE);
				m_hasCaptureReleaseExpected = FALSE;
			}
			m_pMenubar->PostMessage(WM_KEYDOWN, wParam, lParam);
		}
		else
			Reset();
		break;
	default:
		{
			TEnumChildProcFindMnemonicParam param(m_panel->get_handle(), Castdword(wParam));

			::EnumChildWindows(m_panel->get_handle(), EnumChildProc_FindMnemonic, (LPARAM)(&param));
			set_hovered(false);
			if ( !(param.pControl) )
			{
				if ( m_pMenubar && m_pMenubar->TestMnemonic(Castdword(wParam)) )
				{
					if ( has_Capture() )
					{
						m_hasCaptureReleaseExpected = TRUE;
						set_Capture(FALSE);
						m_hasCaptureReleaseExpected = FALSE;
					}
					m_pMenubar->PostMessage(WM_KEYDOWN, wParam, lParam);
				}
				break;
			}
			m_pCurrentMenuItem = param.pControl;
			set_hovered(true);
		}
		break;
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls After OnKeyDown %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

//***********************************************************
// CPopupMenuEx
//***********************************************************
BEGIN_MESSAGE_MAP(CPopupMenu, CPopupMenuEx)
END_MESSAGE_MAP()

CPopupMenuEx::CPopupMenuEx(LPCTSTR name): 
	CPopupMenu(name)
{
}

CPopupMenuEx::CPopupMenuEx(ConstRef(CStringBuffer) name): 
	CPopupMenu(name)
{
}

CPopupMenuEx::~CPopupMenuEx()
{}

BOOL CPopupMenuEx::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("CPOPUPMENUEX");
	return TRUE;
}

BOOL CPopupMenuEx::PreCreate(CREATESTRUCT& cs)
{
	cs.style = WS_POPUP;
	cs.dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	return TRUE;
}

void CPopupMenuEx::set_Image(int imageIndex, UINT commandCode)
{
	if ( (imageIndex >= 0) && (Cast(dword, imageIndex) >= m_imagesnormal.get_bitmapcnt()) )
		return;

	CControl *pCtrl = get_menuItem(commandCode);

	if ( pCtrl )
	{
		CStatic* pStatic = CastDynamicPtr(CStatic, pCtrl);

		if ( pStatic )
		{
			pStatic->set_iconnormal(m_imagesnormal.get_bitmap(imageIndex));
			pStatic->set_icondisabled(m_imagesdisabled.get_bitmap(imageIndex));
			pStatic->set_iconhot(m_imageshot.get_bitmap(imageIndex));
		}
	}
}

void CPopupMenuEx::set_Enabled(UINT commandCode, bool bEnable)
{
	CControl *pCtrl = get_menuItem(commandCode);

	if ( pCtrl )
		pCtrl->set_Enabled(bEnable);
}

void CPopupMenuEx::Track(POINT pt, CWin* wndw)
{
	set_trackwindow(wndw);
	AutoSize(pt);
	TakeOverCapture();
	get_frame()->SendMessage(WM_ENTERMENULOOP, 0, 0);
}

void CPopupMenuEx::Reset()
{
	CPopupMenu::Reset();
	get_frame()->SendMessage(WM_EXITMENULOOP, 0, 0);
	get_trackwindow()->set_focus();
}

//***********************************************************
// CMenubar
//***********************************************************
BEGIN_MESSAGE_MAP(CControl, CMenubar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_KEYDOWN()
	ON_WM_NOTIFY(NM_BUTTONMOUSEMOVE, OnNotifyButtonMove)
END_MESSAGE_MAP()

CMenubar::CMenubar(LPCTSTR name): 
	CControl(name), 
	m_menus(__FILE__LINE__ 32, 32),
	m_pCurrentMenu(nullptr),
	m_pCurrentMenuBarItem(nullptr),
	m_hasCaptureReleaseExpected(FALSE),
	m_hasLoopNoticed(FALSE),
	m_isMDI(FALSE),
	m_MDIButtonMode(0),
	m_CurrentMDIButton(-1),
	m_pMDIChild(nullptr)
{
}

CMenubar::CMenubar(ConstRef(CStringBuffer) name): 
	CControl(name), 
	m_menus(__FILE__LINE__ 32, 32),
	m_pCurrentMenu(nullptr),
	m_pCurrentMenuBarItem(nullptr),
	m_hasCaptureReleaseExpected(FALSE),
	m_hasLoopNoticed(FALSE),
	m_isMDI(FALSE),
	m_MDIButtonMode(0),
	m_CurrentMDIButton(-1),
	m_pMDIChild(nullptr)
{
}

CMenubar::~CMenubar()
{
}

BOOL CMenubar::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("CMENUBAR");
	return TRUE;
}

void CMenubar::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".Color.Workspace"), _T("MenuBar"));
	CControl::get_BrushKeys(_keys);
	CMenuVector::Iterator it = m_menus.Begin();

	while ( it )
	{
		(*it)->get_BrushKeys(_keys);
		++it;
	}
}

void CMenubar::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	CControl::get_FontKeys(_keys);
	CMenuVector::Iterator it = m_menus.Begin();

	while ( it )
	{
		(*it)->get_FontKeys(_keys);
		++it;
	}
}

void CMenubar::Refresh()
{
	CControl::Refresh();
	CMenuVector::Iterator it = m_menus.Begin();

	while ( it )
	{
		(*it)->Refresh();
		++it;
	}
}

int CMenubar::inx_Menu(CPopupMenu* pMenu)
{
	for ( TListIndex ix = 0; Cast(TListCnt,ix) < m_menus.Count(); ++ix )
	{
		if ( (*(m_menus.Index(ix))) == pMenu )
			return ix;
	}
	return -1;
}

void CMenubar::set_hovered(bool bHovered)
{
	CStatic* pStatic = CastDynamicPtr(CStatic, m_pCurrentMenuBarItem);

	if ( pStatic )
	{
		if ( ((pStatic->is_hovered()) && (!bHovered)) || ((!(pStatic->is_hovered())) && bHovered) )
		{
			if ( bHovered )
				pStatic->set_clicked(false);
			pStatic->set_hovered(bHovered);
			pStatic->Update(FALSE);
		}
	}
}

void CMenubar::set_clicked(bool bClicked)
{
	CStatic* pStatic = CastDynamicPtr(CStatic, m_pCurrentMenuBarItem);

	if ( pStatic )
	{
		if ( ((pStatic->is_clicked()) && (!bClicked)) || ((!(pStatic->is_clicked())) && bClicked) )
		{
			if ( bClicked )
				pStatic->set_hovered(false);
			pStatic->set_clicked(bClicked);
			pStatic->Update(FALSE);
		}
	}
}

void CMenubar::CreateFromHMENU(HMENU hmenu)
{
	int mCnt = ::GetMenuItemCount(hmenu);
	RECT rMargins;

	::SetRect(&rMargins, 4, 0, 4, 0);
	for ( int ix = 0; ix < mCnt; ++ix )
	{
		MENUITEMINFO mi;

		::SecureZeroMemory(&mi, sizeof(MENUITEMINFO));
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask = MIIM_STRING;
		if ( ::GetMenuItemInfo(hmenu, ix, TRUE, &mi) && (mi.cch > 0) )
		{
			mi.cch++;

			CStringBuffer mText(__FILE__LINE__ mi.cch);
			CStringBuffer name1;
			CStringBuffer name2;

			name1.FormatString(__FILE__LINE__ _T("%s.MenuItem%d"), m_name.GetString(), ix);
			name2.FormatString(__FILE__LINE__ _T("%s.PopupMenu%d"), m_name.GetString(), ix);
			mi.dwTypeData = CastAny(LPTSTR, CastMutable(CPointer, mText.GetString()));
			::GetMenuItemInfo(hmenu, ix, TRUE, &mi);

			CStatic* pStatic = OK_NEW_OPERATOR CStatic(name1);

			pStatic->set_LayoutX(ix);
			pStatic->set_text(mText);
			pStatic->set_style(CStatic::sMenuItem);
			pStatic->set_Margins(rMargins);
			pStatic->Create(m_hwnd, 1000+ix);

			CPopupMenu* pMenu = OK_NEW_OPERATOR CPopupMenu(name2);

			pMenu->set_menubar(this);
			pMenu->set_CreateVisible(FALSE);
			pMenu->set_ClientBorder(TRUE);
			pMenu->Create(get_frame()->get_handle());

			HMENU submenu = ::GetSubMenu(hmenu, ix);

			if ( submenu )
				pMenu->CreateFromHMENU(submenu);
			m_menus.Append(pMenu);
		}
	}
}

void CMenubar::AutoSize()
{
	RECT r;

	OnSize(0, 0);
	get_parent()->GetClientRect(&r);
	r.bottom = m_maxClientArea.bottom;
	MoveWindow(&r, TRUE);
}

void CMenubar::TakeOverCapture()
{
	POINT pt;
	CControl* pCtrl;

	::GetCursorPos(&pt);

	pCtrl = (CControl*)(get_childFromPt(pt));
	if ( pCtrl )
	{
		if ( m_pCurrentMenuBarItem != pCtrl )
		{
			set_Capture(TRUE);
			set_hovered(false);
			m_pCurrentMenuBarItem = pCtrl;
			set_hovered(true);
			if ( m_pCurrentMenu )
			{
				int ix = inx_Menu(m_pCurrentMenu);

				if ( (ix >= 0) && (((m_pCurrentMenuBarItem->get_childID()) - 1000) != ix) )
				{
					m_pCurrentMenu->Reset();
					OpenPopupMenu(m_pCurrentMenuBarItem);
				}
			}
			else
				OpenPopupMenu(m_pCurrentMenuBarItem);
		}
	}
}

void CMenubar::Reset()
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls Reset %d\n", get_name().GetString(), debugID);
#endif
	if ( m_pCurrentMenu )
	{
		if ( m_pCurrentMenu->is_Visible() )
			m_pCurrentMenu->Reset();
		m_pCurrentMenu = nullptr;
	}
	if ( m_pCurrentMenuBarItem )
	{
		set_hovered(false);
		set_clicked(false);
		m_pCurrentMenuBarItem = nullptr;
	}
	if ( m_hasLoopNoticed )
	{
		get_frame()->SendMessage(WM_EXITMENULOOP1, 0, 0);
		m_hasLoopNoticed = FALSE;
	}
}

bool CMenubar::TestMnemonic(UINT vKey)
{
	TEnumChildProcFindMnemonicParam param(m_hwnd, vKey);

	::EnumChildWindows(m_hwnd, EnumChildProc_FindMnemonic, (LPARAM)(&param));
	return param.pControl != nullptr;
}

void CMenubar::set_Image(int imageIndex, UINT commandCode)
{
	if ( (imageIndex >= 0) && (Cast(dword, imageIndex) >= m_imagesnormal.get_bitmapcnt()) )
		return;

	CMenuVector::Iterator it = m_menus.Begin();

	while ( it )
	{
		CControl *pCtrl = (*it)->get_menuItem(commandCode);

		if ( pCtrl )
		{
			CStatic* pStatic = CastDynamicPtr(CStatic, pCtrl);

			if ( pStatic )
			{
				pStatic->set_iconnormal(m_imagesnormal.get_bitmap(imageIndex));
				pStatic->set_icondisabled(m_imagesdisabled.get_bitmap(imageIndex));
				pStatic->set_iconhot(m_imageshot.get_bitmap(imageIndex));
			}
			return;
		}
		++it;
	}
}

void CMenubar::set_Enabled(UINT commandCode, bool bEnable)
{
	CMenuVector::Iterator it = m_menus.Begin();

	while ( it )
	{
		CControl *pCtrl = (*it)->get_menuItem(commandCode);

		if ( pCtrl )
			pCtrl->set_Enabled(bEnable);
		++it;
	}
}

LRESULT CMenubar::OpenPopupMenu(CControl* pMenuItem)
{
	RECT r1;
	POINT pt;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls Before OpenPopupMenu %d\n", get_name().GetString(), debugID);
#endif
	m_pCurrentMenuBarItem = pMenuItem;
	m_pCurrentMenu = nullptr;

	int ix = m_pCurrentMenuBarItem->get_childID() - 1000;

	if ( ix >= 0 )
	{
		m_pCurrentMenu = *(m_menus.Index(ix));
		m_pCurrentMenuBarItem->GetWindowRect(&r1);
		pt.x = r1.left;
		pt.y = r1.bottom;
		get_frame()->SendMessage(WM_INITMENUPOPUP, 0, (LPARAM)m_pCurrentMenu);
		m_pCurrentMenu->AutoSize(pt);
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls After OpenPopupMenu %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

void CMenubar::set_MDIMenubar(BOOL b, CMDIChild* pChild)
{
	if ( b )
	{
		m_pMDIChild = pChild;

		CBitmap bitmap(m_pMDIChild->get_classiconsm());
		CStatic* pStatic;

		if ( !m_isMDI )
		{
			RECT rMargins;
			RECT r;
			CStringBuffer name;

			::SetRect(&rMargins, 4, 0, 4, 0);
			name.FormatString(__FILE__LINE__ _T("%s.SystemMenu"), m_name.GetString());

			pStatic = OK_NEW_OPERATOR CStatic(name);
			pStatic->set_LayoutX(0);
			pStatic->set_iconnormal(bitmap);
			pStatic->set_style(CStatic::sMenuItem);
			pStatic->set_Margins(rMargins);
			pStatic->Create(m_hwnd, 999);
			pStatic->SetWindowPos(HWND_TOP, &r, SWP_NOMOVE | SWP_NOSIZE);
			RenumberLayout(true);

			m_isMDI = TRUE;
			m_CurrentMDIButton = -1;
			m_MDIButtonMode = 0;
			UpdateChildren(FALSE);
			Update(FALSE);
		}
		else if ( pStatic = CastDynamicPtr(CStatic, get_firstchild()) )
		{
			pStatic->set_iconnormal(bitmap);
			pStatic->Update(FALSE);
			m_CurrentMDIButton = -1;
			m_MDIButtonMode = 0;
			Update(FALSE);
		}
	}
	else if ( m_isMDI )
	{
		m_isMDI = FALSE;
		m_CurrentMDIButton = -1;
		m_MDIButtonMode = 0;
		m_pMDIChild = nullptr;

		CWin* pWin = get_firstchild();

		if ( pWin )
		{
			pWin->DestroyWindow();
			RenumberLayout(true);
			Update(FALSE);
		}
	}
}

void CMenubar::DrawMDIButton(Gdiplus::Graphics* pGraphics, int iButton, UINT uState)
{
	switch (uState)
	{
	case 0:
		{
			// Draw a grey outline
			Gdiplus::Pen pen(Gdiplus::Color::Gray);

			pGraphics->DrawLine(&pen, m_MDIButton[iButton].left, m_MDIButton[iButton].bottom, m_MDIButton[iButton].right, m_MDIButton[iButton].bottom);
			pGraphics->DrawLine(&pen, m_MDIButton[iButton].right, m_MDIButton[iButton].bottom, m_MDIButton[iButton].right, m_MDIButton[iButton].top);
			pGraphics->DrawLine(&pen, m_MDIButton[iButton].right, m_MDIButton[iButton].top, m_MDIButton[iButton].left, m_MDIButton[iButton].top);
			pGraphics->DrawLine(&pen, m_MDIButton[iButton].left, m_MDIButton[iButton].top, m_MDIButton[iButton].left, m_MDIButton[iButton].bottom);
		}
		break;
	case 1:
		{
			// Draw outline, white at top, black on bottom
			Gdiplus::Pen pen1(Gdiplus::Color::Black);
			Gdiplus::Pen pen2(Gdiplus::Color::White);

			pGraphics->DrawLine(&pen1, m_MDIButton[iButton].left, m_MDIButton[iButton].bottom, m_MDIButton[iButton].right, m_MDIButton[iButton].bottom);
			pGraphics->DrawLine(&pen1, m_MDIButton[iButton].right, m_MDIButton[iButton].bottom, m_MDIButton[iButton].right, m_MDIButton[iButton].top);
			pGraphics->DrawLine(&pen2, m_MDIButton[iButton].right, m_MDIButton[iButton].top, m_MDIButton[iButton].left, m_MDIButton[iButton].top);
			pGraphics->DrawLine(&pen2, m_MDIButton[iButton].left, m_MDIButton[iButton].top, m_MDIButton[iButton].left, m_MDIButton[iButton].bottom);
		}

		break;
	case 2:
		{
			// Draw outline, black on top, white on bottom
			Gdiplus::Pen pen1(Gdiplus::Color::White);
			Gdiplus::Pen pen2(Gdiplus::Color::Black);

			pGraphics->DrawLine(&pen1, m_MDIButton[iButton].left, m_MDIButton[iButton].bottom, m_MDIButton[iButton].right, m_MDIButton[iButton].bottom);
			pGraphics->DrawLine(&pen1, m_MDIButton[iButton].right, m_MDIButton[iButton].bottom, m_MDIButton[iButton].right, m_MDIButton[iButton].top);
			pGraphics->DrawLine(&pen2, m_MDIButton[iButton].right, m_MDIButton[iButton].top, m_MDIButton[iButton].left, m_MDIButton[iButton].top);
			pGraphics->DrawLine(&pen2, m_MDIButton[iButton].left, m_MDIButton[iButton].top, m_MDIButton[iButton].left, m_MDIButton[iButton].bottom);
		}
		break;
	}

	Gdiplus::Pen pen(Gdiplus::Color::Black);

	switch (iButton)
	{
	case 0:
		// Manually Draw Minimise button
		pGraphics->DrawLine(&pen, m_MDIButton[0].left + 4, m_MDIButton[0].bottom - 4, m_MDIButton[0].right - 4, m_MDIButton[0].bottom - 4);
		pGraphics->DrawLine(&pen, m_MDIButton[0].left + 4, m_MDIButton[0].bottom - 5, m_MDIButton[0].right - 4, m_MDIButton[0].bottom - 5);
		break;
	case 1:
		// Manually Draw Restore Button
		pGraphics->DrawLine(&pen, m_MDIButton[1].left + 3, m_MDIButton[1].top + 7, m_MDIButton[1].left + 3, m_MDIButton[1].bottom - 4);
		pGraphics->DrawLine(&pen, m_MDIButton[1].left + 3, m_MDIButton[1].bottom - 4, m_MDIButton[1].right - 6, m_MDIButton[1].bottom - 4);
		pGraphics->DrawLine(&pen, m_MDIButton[1].right - 6, m_MDIButton[1].bottom - 4, m_MDIButton[1].right - 6, m_MDIButton[1].top + 7);
		pGraphics->DrawLine(&pen, m_MDIButton[1].right - 6, m_MDIButton[1].top + 7, m_MDIButton[1].left + 3, m_MDIButton[1].top + 7);

		pGraphics->DrawLine(&pen, m_MDIButton[1].left + 3, m_MDIButton[1].top + 8, m_MDIButton[1].right - 6, m_MDIButton[1].top + 8);

		pGraphics->DrawLine(&pen, m_MDIButton[1].left + 5, m_MDIButton[1].top + 7, m_MDIButton[1].left + 5, m_MDIButton[1].top + 4);
		pGraphics->DrawLine(&pen, m_MDIButton[1].left + 5, m_MDIButton[1].top + 4, m_MDIButton[1].right - 4, m_MDIButton[1].top + 4);
		pGraphics->DrawLine(&pen, m_MDIButton[1].right - 4, m_MDIButton[1].top + 4, m_MDIButton[1].right - 4, m_MDIButton[1].bottom - 6);
		pGraphics->DrawLine(&pen, m_MDIButton[1].right - 4, m_MDIButton[1].bottom - 6, m_MDIButton[1].right - 6, m_MDIButton[1].bottom - 6);

		pGraphics->DrawLine(&pen, m_MDIButton[1].left + 5, m_MDIButton[1].top + 5, m_MDIButton[1].right - 4, m_MDIButton[1].top + 5);
		break;
	case 2:
		// Manually Draw Close Button
		pGraphics->DrawLine(&pen, m_MDIButton[2].left + 4, m_MDIButton[2].top + 5, m_MDIButton[2].right - 4, m_MDIButton[2].bottom - 3);
		pGraphics->DrawLine(&pen, m_MDIButton[2].left + 5, m_MDIButton[2].top + 5, m_MDIButton[2].right - 4, m_MDIButton[2].bottom - 4);
		pGraphics->DrawLine(&pen, m_MDIButton[2].left + 4, m_MDIButton[2].top + 6, m_MDIButton[2].right - 5, m_MDIButton[2].bottom - 3);
		pGraphics->DrawLine(&pen, m_MDIButton[2].right - 5, m_MDIButton[2].top + 5, m_MDIButton[2].left + 3, m_MDIButton[2].bottom - 3);
		pGraphics->DrawLine(&pen, m_MDIButton[2].right - 5, m_MDIButton[2].top + 6, m_MDIButton[2].left + 4, m_MDIButton[2].bottom - 3); 
		pGraphics->DrawLine(&pen, m_MDIButton[2].right - 6, m_MDIButton[2].top + 5, m_MDIButton[2].left + 3, m_MDIButton[2].bottom - 4);
		break;
	}
}

LRESULT CMenubar::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;

	GetClientRect(&rc);

	hdc = ::BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;
	
	Gdiplus::Graphics graphics(hdc);

	Gdiplus::Bitmap bitmap(rc.right, rc.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);
	Gdiplus::Brush* brush = get_Brush(_T(".Color.Workspace"), _T("MenuBar"), Gdiplus::Color::Gray);

	PaintWorkspace(&bitmapgraphics, brush);
	if ( m_isMDI )
	{
		int cx = ::GetSystemMetrics(SM_CXSMICON);
		int cy = ::GetSystemMetrics(SM_CYSMICON);
		int gap = 4;

		rc.right -= gap;
		for (int i = 0 ; i < 3 ; ++i)
		{
			int left = rc.right - (i+1)*cx - gap*(i+1);
			int top = rc.bottom/2 - cy/2;
			int right = rc.right - i*cx - gap*(i+1);
			int bottom = rc.bottom/2 + cy/2;
			::SetRect(&m_MDIButton[2 - i], left, top, right, bottom);
		}
		DrawMDIButton(&bitmapgraphics, 0, (m_CurrentMDIButton == 0)?m_MDIButtonMode:0);
		DrawMDIButton(&bitmapgraphics, 1, (m_CurrentMDIButton == 1)?m_MDIButtonMode:0);
		DrawMDIButton(&bitmapgraphics, 2, (m_CurrentMDIButton == 2)?m_MDIButtonMode:0);
	}
	graphics.DrawImage(&bitmap, 0, 0);
	::EndPaint(m_hwnd, &ps);
	m_updateRects = FALSE;
	return 0;
}

LRESULT CMenubar::OnNotifyButtonMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnNotifyButtonMove %d\n", get_name().GetString(), debugID);
#endif
	Ptr(NotifyMessage) phdr = CastAnyPtr(NotifyMessage, lParam);

	m_pCurrentMenuBarItem = CastAnyPtr(CControl, phdr->param);
	set_hovered(true);
	set_focus();
	set_Capture(TRUE);
	m_hasCaptureReleaseExpected = FALSE;
	m_pCurrentMenu = nullptr;

	if ( !m_hasLoopNoticed )
	{
		get_frame()->SendMessage(WM_ENTERMENULOOP1, 0, 0);
		m_hasLoopNoticed = TRUE;
	}
	return 0;
}

LRESULT CMenubar::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnLButtonDown %d\n", get_name().GetString(), debugID);
#endif
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if ( m_isMDI && (m_CurrentMDIButton > -1) && (wParam == MK_LBUTTON) )
	{
		if ( ::PtInRect(&m_MDIButton[m_CurrentMDIButton], pt) )
		{
			m_MDIButtonMode = 2;
			Update(FALSE);
			return 0;
		}
		m_CurrentMDIButton = -1;
		m_MDIButtonMode = 0;
		Update(FALSE);
	}
	if ( has_Capture() && (wParam == MK_LBUTTON) )
	{
		if ( m_pCurrentMenuBarItem )
		{
			RECT r;

			ClientToScreen(&pt);
			m_pCurrentMenuBarItem->GetWindowRect(&r);
			if ( ::PtInRect(&r, pt) )
				set_clicked(true);
		}
	}
	return 0;
}

LRESULT CMenubar::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnLButtonUp %d\n", get_name().GetString(), debugID);
#endif
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if ( m_isMDI && (m_CurrentMDIButton > -1) && (wParam == 0) )
	{
		if ( ::PtInRect(&m_MDIButton[m_CurrentMDIButton], pt) )
		{
			m_MDIButtonMode = 0;
			Update(FALSE);
			switch ( m_CurrentMDIButton )
			{
			case 0: // Minimize
				m_pMDIChild->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				break;
			case 1: // Restore
				m_pMDIChild->SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
				break;
			case 2: // Close
				m_pMDIChild->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
				break;
			}
			return 0;
		}
		m_CurrentMDIButton = -1;
		m_MDIButtonMode = 0;
		Update(FALSE);
	}
	if ( has_Capture() && (wParam == 0) )
	{
		if ( m_pCurrentMenuBarItem )
		{
			RECT r;

			set_clicked(false);
			ClientToScreen(&pt);
			m_pCurrentMenuBarItem->GetWindowRect(&r);
			if ( ::PtInRect(&r, pt) )
			{
				if ( m_pCurrentMenu )
				{
					int ix = inx_Menu(m_pCurrentMenu);

					m_pCurrentMenu->Reset();
					m_pCurrentMenu = nullptr;
					if ( ix == ((m_pCurrentMenuBarItem->get_childID()) - 1000) )
						return 0;
				}
				OpenPopupMenu(m_pCurrentMenuBarItem);
				return 0;
			}
			set_hovered(false);
			m_pCurrentMenuBarItem = nullptr;
		}
		if ( !m_pCurrentMenuBarItem )
		{
			Reset();
			m_hasCaptureReleaseExpected = TRUE;
			set_Capture(FALSE);
			m_hasCaptureReleaseExpected = FALSE;
			if ( m_hasLoopNoticed )
			{
				get_frame()->SendMessage(WM_EXITMENULOOP1, 0, 0);
				m_hasLoopNoticed = FALSE;
			}
		}
	}
	return 0;
}

LRESULT CMenubar::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnMouseMove %d\n", get_name().GetString(), debugID);
#endif
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if ( m_isMDI && (wParam == 0) )
	{
		for ( int ix = 0; ix < 3; ++ix )
		{
			if ( ::PtInRect(&m_MDIButton[ix], pt) )
			{
				m_CurrentMDIButton = ix;
				m_MDIButtonMode = 1;
				Update(FALSE);
				return 0;
			}
		}
	}
	if ( has_Capture() && (wParam == 0) )
	{
		CControl* pCtrl;
		CControl* svCurrentMenuBarItem = m_pCurrentMenuBarItem;

		ClientToScreen(&pt);
		if ( m_pCurrentMenuBarItem )
		{
			RECT r;

			m_pCurrentMenuBarItem->GetWindowRect(&r);
			if ( ::PtInRect(&r, pt) )
			{
				if ( m_pCurrentMenu && (!(m_pCurrentMenu->is_Visible())) )
					m_pCurrentMenu = nullptr;
				set_hovered(true);
				return 0;
			}
			pCtrl = (CControl*)(m_pCurrentMenuBarItem->get_siblingByCmd(GW_HWNDPREV));
			if ( pCtrl ) 
			{
				pCtrl->GetWindowRect(&r);
				if ( ::PtInRect(&r, pt) )
				{
					set_hovered(false);
					m_pCurrentMenuBarItem = pCtrl;
					if ( m_pCurrentMenu && (!(m_pCurrentMenu->is_Visible())) )
						m_pCurrentMenu = nullptr;
					if ( m_pCurrentMenu )
					{
						m_pCurrentMenu->Reset();
						OpenPopupMenu(m_pCurrentMenuBarItem);
					}
					set_hovered(true);
					return 0;
				}
			}
			pCtrl = (CControl*)(m_pCurrentMenuBarItem->get_siblingByCmd(GW_HWNDNEXT));
			if ( pCtrl ) 
			{
				pCtrl->GetWindowRect(&r);
				if ( ::PtInRect(&r, pt) )
				{
					set_hovered(false);
					m_pCurrentMenuBarItem = pCtrl;
					if ( m_pCurrentMenu && (!(m_pCurrentMenu->is_Visible())) )
						m_pCurrentMenu = nullptr;
					if ( m_pCurrentMenu )
					{
						m_pCurrentMenu->Reset();
						OpenPopupMenu(m_pCurrentMenuBarItem);
					}
					set_hovered(true);
					return 0;
				}
			}
			m_pCurrentMenuBarItem = nullptr;
		}
		if ( !m_pCurrentMenuBarItem )
		{
			pCtrl = (CControl*)(get_childFromPt(pt));
			if ( pCtrl )
			{
				if ( pCtrl != svCurrentMenuBarItem )
				{
					m_pCurrentMenuBarItem = svCurrentMenuBarItem;
					set_hovered(false);
				}
				m_pCurrentMenuBarItem = pCtrl;
				if ( m_pCurrentMenu && (!(m_pCurrentMenu->is_Visible())) )
					m_pCurrentMenu = nullptr;
				if ( m_pCurrentMenu )
				{
					int ix = inx_Menu(m_pCurrentMenu);

					if ( ix != ((m_pCurrentMenuBarItem->get_childID()) - 1000) )
					{
						m_pCurrentMenu->Reset();
						OpenPopupMenu(m_pCurrentMenuBarItem);
					}
				}
				set_hovered(true);
				return 0;
			}
		}
		if ( !m_pCurrentMenuBarItem )
		{
			if ( m_pCurrentMenu && (m_pCurrentMenu->is_Visible()) )
			{
				RECT r;

				m_pCurrentMenu->GetWindowRect(&r);
				if ( PtInRect(&r, pt) )
				{
					m_pCurrentMenuBarItem = svCurrentMenuBarItem;
					set_hovered(false);
					m_hasCaptureReleaseExpected = TRUE;
					m_pCurrentMenu->TakeOverCapture();
					m_hasCaptureReleaseExpected = FALSE;
					return 0;
				}
			}
		}
		if ( !m_pCurrentMenuBarItem )
			m_pCurrentMenuBarItem = svCurrentMenuBarItem;
	}
	return 0;
}

LRESULT CMenubar::OnCaptureChanged(WPARAM wParam, LPARAM lParam)
{
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls OnCaptureChanged %d, hasCaptureReleaseExpected=%d\n", get_name().GetString(), debugID, m_hasCaptureReleaseExpected);
#endif
	if ( !m_hasCaptureReleaseExpected )
		Reset();
	m_hasCaptureReleaseExpected = FALSE;
	return 0;
}

LRESULT CMenubar::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls Before OnSysKeyDown %d, wParam=%x\n", get_name().GetString(), debugID, wParam);
#endif
	if ( bShift || bControl )
		return 0;
	switch ( wParam )
	{
	case VK_F10:
	case VK_MENU:
		break;
	default:
		if ( TestBitN(lParam, 29) )
		{
			TEnumChildProcFindMnemonicParam param(m_hwnd, Castdword(wParam));

			_InitSysKey();
			::EnumChildWindows(m_hwnd, EnumChildProc_FindMnemonic, (LPARAM)(&param));
			if ( !(param.pControl) )
			{
				if ( m_pCurrentMenu && m_pCurrentMenu->TestMnemonic(Castdword(wParam)) )
				{
					if ( has_Capture() )
					{
						m_hasCaptureReleaseExpected = TRUE;
						set_Capture(FALSE);
						m_hasCaptureReleaseExpected = FALSE;
					}
					m_pCurrentMenu->PostMessage(WM_KEYDOWN, wParam, lParam);
				}
				break;
			}
			set_hovered(false);
			if ( m_pCurrentMenu )
				m_pCurrentMenu->Reset();
			OpenPopupMenu(param.pControl);
			set_focus();
		}
		break;
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls After OnSysKeyDown %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

LRESULT CMenubar::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls Before OnSysKeyUp %d, wParam=%x\n", get_name().GetString(), debugID, wParam);
#endif
	if ( bShift || bControl )
		return 0;
	switch ( wParam )
	{
	case VK_F10:
	case VK_MENU:
		_InitSysKey();
		break;
	default:
		break;
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls After OnSysKeyUp %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

void CMenubar::_InitSysKey()
{
	if ( !has_focus() )
		set_focus();
	if ( !has_Capture() )
	{
		if ( m_pCurrentMenu )
		{
			if ( !(m_pCurrentMenu->has_Capture()) )
				set_Capture(TRUE);
		}
		else
			set_Capture(TRUE);
	}
	if ( !m_pCurrentMenuBarItem )
	{
		m_pCurrentMenuBarItem = CastDynamicPtr(CControl, get_firstchild());
		set_hovered(true);
	}
	if ( !m_hasLoopNoticed )
	{
		get_frame()->SendMessage(WM_ENTERMENULOOP1, 0, 0);
		m_hasLoopNoticed = TRUE;
	}
}

LRESULT CMenubar::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("%ls Before OnKeyDown %d, wParam=%x\n", get_name().GetString(), debugID, wParam);
#endif
	if ( bShift || bControl )
		return 0;
	if ( !has_Capture() )
		set_Capture(TRUE);
	if ( !has_focus() )
		set_focus();
	switch ( wParam )
	{
	case VK_LEFT:
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls OnKeyDown %d, VK_LEFT, m_pCurrentMenuBarItem=%x\n", get_name().GetString(), debugID, m_pCurrentMenuBarItem);
#endif
		if ( m_pCurrentMenuBarItem )
		{
			CControl* pCtrl = (CControl*)(m_pCurrentMenuBarItem->get_siblingByCmd(GW_HWNDPREV));

			if ( !pCtrl )
			{
				pCtrl = (CControl*)(get_firstchild());
				if ( pCtrl )
					pCtrl = (CControl*)(pCtrl->get_siblingByCmd(GW_HWNDLAST));
			}
			if ( pCtrl )
			{
				set_hovered(false);
				m_pCurrentMenuBarItem = pCtrl;
				if ( m_pCurrentMenu )
				{
					m_pCurrentMenu->Reset();
					OpenPopupMenu(m_pCurrentMenuBarItem);
					set_focus();
				}
				else
					set_hovered(true);
			}
		}
		break;
	case VK_RIGHT:
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls OnKeyDown %d, VK_RIGHT, m_pCurrentMenuBarItem=%x\n", get_name().GetString(), debugID, m_pCurrentMenuBarItem);
#endif
		if ( m_pCurrentMenuBarItem )
		{
			CControl* pCtrl = (CControl*)(m_pCurrentMenuBarItem->get_siblingByCmd(GW_HWNDNEXT));

			if ( !pCtrl )
				pCtrl = (CControl*)(get_firstchild());
			if ( pCtrl )
			{
				set_hovered(false);
				m_pCurrentMenuBarItem = pCtrl;
				if ( m_pCurrentMenu )
				{
					m_pCurrentMenu->Reset();
					OpenPopupMenu(m_pCurrentMenuBarItem);
					set_focus();
				}
				else
					set_hovered(true);
			}
		}
		break;
	case VK_DOWN:
	case VK_RETURN:
		if ( m_pCurrentMenuBarItem )
		{
			set_hovered(false);
			if ( !m_pCurrentMenu )
				OpenPopupMenu(m_pCurrentMenuBarItem);
			if ( has_Capture() )
			{
				m_hasCaptureReleaseExpected = TRUE;
				set_Capture(FALSE);
				m_hasCaptureReleaseExpected = FALSE;
			}
			m_pCurrentMenu->PostMessage(WM_KEYDOWN, VK_DOWN, lParam);
		}
		break;
	case VK_UP:
		if ( m_pCurrentMenuBarItem )
		{
			set_hovered(false);
			if ( !m_pCurrentMenu )
				OpenPopupMenu(m_pCurrentMenuBarItem);
			if ( has_Capture() )
			{
				m_hasCaptureReleaseExpected = TRUE;
				set_Capture(FALSE);
				m_hasCaptureReleaseExpected = FALSE;
			}
			m_pCurrentMenu->PostMessage(WM_KEYDOWN, VK_UP, lParam);
		}
		break;
	case VK_ESCAPE:
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls OnKeyDown %d, VK_ESCAPE.1, m_pCurrentMenuBarItem=%x, m_pCurrentMenu=%x\n", get_name().GetString(), debugID, m_pCurrentMenuBarItem, m_pCurrentMenu);
#endif
		if ( m_pCurrentMenu )
		{
			if ( m_pCurrentMenu->is_Visible() )
			{
				m_pCurrentMenu->Reset();
				set_focus();
				m_pCurrentMenu = nullptr;
				set_hovered(true);
				break;
			}
			m_pCurrentMenu = nullptr;
		}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls OnKeyDown %d, VK_ESCAPE.2, m_pCurrentMenuBarItem=%x, m_pCurrentMenu=%x\n", get_name().GetString(), debugID, m_pCurrentMenuBarItem, m_pCurrentMenu);
#endif
		if ( m_pCurrentMenuBarItem )
		{
			set_hovered(false);
			m_pCurrentMenuBarItem = nullptr;
		}
		set_Capture(FALSE);
		if ( m_hasLoopNoticed )
		{
			get_frame()->SendMessage(WM_EXITMENULOOP1, 0, 0);
			m_hasLoopNoticed = FALSE;
		}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls OnKeyDown %d, After VK_ESCAPE, m_pCurrentMenuBarItem=%x, m_pCurrentMenu=%x\n", get_name().GetString(), debugID, m_pCurrentMenuBarItem, m_pCurrentMenu);
#endif
		break;
	default:
		{
			TEnumChildProcFindMnemonicParam param(m_hwnd, Castdword(wParam));

			::EnumChildWindows(m_hwnd, EnumChildProc_FindMnemonic, (LPARAM)(&param));
			if ( !(param.pControl) )
			{
				if ( m_pCurrentMenu && m_pCurrentMenu->TestMnemonic(Castdword(wParam)) )
				{
					if ( has_Capture() )
					{
						m_hasCaptureReleaseExpected = TRUE;
						set_Capture(FALSE);
						m_hasCaptureReleaseExpected = FALSE;
					}
					m_pCurrentMenu->PostMessage(WM_KEYDOWN, wParam, lParam);
				}
				break;
			}
			set_hovered(false);
			if ( m_pCurrentMenu )
				m_pCurrentMenu->Reset();
			OpenPopupMenu(param.pControl);
			set_focus();
		}
		break;
	}
#ifdef __DEBUG1__
	theGuiApp->DebugString("%ls After OnKeyDown %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

//***********************************************************
// CStatic
//***********************************************************
BEGIN_MESSAGE_MAP(CControl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SIZE()
	ON_WM_MESSAGE(WM_GETTEXT,OnGetText)
	ON_WM_MESSAGE(WM_GETTEXTLENGTH,OnGetTextLength)
	ON_WM_MESSAGE(WM_SETTEXT,OnSetText)
	ON_WM_COMMMAND(IDM_CUT,OnEditNotImplCommand)
	ON_WM_COMMMAND(IDM_COPY,OnCopyCommand)
	ON_WM_COMMMAND(IDM_DELETE,OnEditNotImplCommand)
	ON_WM_COMMMAND(IDM_PASTE,OnEditNotImplCommand)
	ON_WM_COMMMAND(IDM_UNDO,OnEditNotImplCommand)
	ON_WM_COMMMAND(IDM_REDO,OnEditNotImplCommand)
END_MESSAGE_MAP()

CStatic::CStatic(LPCTSTR name): 
	CControl(name),
	m_style(sLabel),
	m_position(pnone),
	m_halign(hleft),
	m_valign(vtop),
	m_ampersandpos(-1),
	m_tabulatorpos(-1),
	m_hovered(false),
	m_clicked(false)
{
	::SetRectEmpty(&m_iconRect);
	::SetRectEmpty(&m_textRect);
}

CStatic::CStatic(ConstRef(CStringBuffer) name): 
	CControl(name),
	m_style(sLabel),
	m_position(pnone),
	m_halign(hleft),
	m_valign(vtop),
	m_ampersandpos(-1),
	m_tabulatorpos(-1),
	m_hovered(false),
	m_clicked(false)
{
	::SetRectEmpty(&m_iconRect);
	::SetRectEmpty(&m_textRect);
}

CStatic::~CStatic()
{
}

BOOL CStatic::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CSTATIC");
	return TRUE;
}

CWin* CStatic::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CStatic();

	return pWin;
}

void CStatic::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	switch ( m_style )
	{
	case sLabel:
		add_BrushKeys(_keys, _T(".BackgroundColor.Disabled"), _T("Label"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Disabled"), _T("Label"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Clicked"), _T("Label"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Clicked"), _T("Label"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Hovered"), _T("Label"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Hovered"), _T("Label"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("Label"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("Label"));
		add_BrushKeys(_keys, _T(".Color.HoverEffect"), _T("Label"));
		break;
	case sButton:
		add_BrushKeys(_keys, _T(".BackgroundColor.Disabled"), _T("Button"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Disabled"), _T("Button"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Clicked"), _T("Button"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Clicked"), _T("Button"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Hovered"), _T("Button"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Hovered"), _T("Button"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("Button"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("Button"));
		add_BrushKeys(_keys, _T(".Color.HoverEffect"), _T("Button"));
		break;
	case sToolbarButton:
		add_BrushKeys(_keys, _T(".BackgroundColor.Disabled"), _T("ToolbarButton"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Disabled"), _T("ToolbarButton"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Clicked"), _T("ToolbarButton"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Clicked"), _T("ToolbarButton"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Hovered"), _T("ToolbarButton"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Hovered"), _T("ToolbarButton"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("ToolbarButton"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("ToolbarButton"));
		add_BrushKeys(_keys, _T(".Color.HoverEffect"), _T("ToolbarButton"));
		break;
	case sMenuItem:
		add_BrushKeys(_keys, _T(".BackgroundColor.Disabled"), _T("MenuItem"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Disabled"), _T("MenuItem"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Clicked"), _T("MenuItem"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Clicked"), _T("MenuItem"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Hovered"), _T("MenuItem"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Hovered"), _T("MenuItem"));
		add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("MenuItem"));
		add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("MenuItem"));
		add_BrushKeys(_keys, _T(".Color.HoverEffect"), _T("MenuItem"));
		break;
	}
}

void CStatic::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	switch ( m_style )
	{
	case sLabel:
		add_FontKeys(_keys, _T(".Font.Disabled"), _T("Label"));
		add_FontKeys(_keys, _T(".Font.Clicked"), _T("Label"));
		add_FontKeys(_keys, _T(".Font.Hovered"), _T("Label"));
		add_FontKeys(_keys, _T(".Font.Normal"), _T("Label"));
		break;
	case sButton:
		add_FontKeys(_keys, _T(".Font.Disabled"), _T("Button"));
		add_FontKeys(_keys, _T(".Font.Clicked"), _T("Button"));
		add_FontKeys(_keys, _T(".Font.Hovered"), _T("Button"));
		add_FontKeys(_keys, _T(".Font.Normal"), _T("Button"));
		break;
	case sToolbarButton:
		add_FontKeys(_keys, _T(".Font.Disabled"), _T("ToolbarButton"));
		add_FontKeys(_keys, _T(".Font.Clicked"), _T("ToolbarButton"));
		add_FontKeys(_keys, _T(".Font.Hovered"), _T("ToolbarButton"));
		add_FontKeys(_keys, _T(".Font.Normal"), _T("ToolbarButton"));
		break;
	case sMenuItem:
		add_FontKeys(_keys, _T(".Font.Disabled"), _T("MenuItem"));
		add_FontKeys(_keys, _T(".Font.Clicked"), _T("MenuItem"));
		add_FontKeys(_keys, _T(".Font.Hovered"), _T("MenuItem"));
		add_FontKeys(_keys, _T(".Font.Normal"), _T("MenuItem"));
		break;
	}
}

void CStatic::Refresh()
{
	Update(TRUE);
}

void CStatic::set_text(LPCTSTR text, int lench)
{
	m_text.SetString(__FILE__LINE__ text, lench);
	if ( is_created() )
		Update(TRUE);
}

void CStatic::set_text(ConstRef(CStringBuffer) text)
{
	m_text = text;
	if ( is_created() )
		Update(TRUE);
}

mbchar CStatic::get_Mnemonic()
{
	if ( m_ampersandpos < 0 )
		return 0;
	
	CStringConstIterator it(m_text);

	return m_toupper(it[m_ampersandpos]);
}

LRESULT CStatic::OnGetText(WPARAM wParam, LPARAM lParam)
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

LRESULT CStatic::OnGetTextLength(WPARAM wParam, LPARAM lParam)
{
	return get_text().GetLength();
}

LRESULT CStatic::OnSetText(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer text(__FILE__LINE__ CastAny(CPointer, lParam));

	set_text(text);
	return TRUE;
}

LRESULT CStatic::OnPaint(WPARAM wParam, LPARAM lParam)
{
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

	Gdiplus::Bitmap* pIcon = nullptr;
	Gdiplus::Font* pFont = nullptr;
	Gdiplus::Brush* pBrushBackground = nullptr;
	Gdiplus::Brush* pBrushForeground = nullptr;
	Gdiplus::Brush* pBrushHover = nullptr;

	pIcon = m_iconN.get_bitmap();
	if ( (!(is_Enabled())) && (m_iconD.get_bitmap() != nullptr) )
		pIcon = m_iconD.get_bitmap();
	else if ( (m_hovered || m_clicked) && (m_iconH.get_bitmap() != nullptr) )
		pIcon = m_iconH.get_bitmap();

	if ( !(is_Enabled()) )
	{
		switch ( m_style )
		{
		case sLabel:
			pFont = get_Font(_T(".Font.Disabled"), _T("Label"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Disabled"), _T("Label"), Gdiplus::Color::LightGray);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Disabled"), _T("Label"), Gdiplus::Color::DarkGray);
			break;
		case sButton:
			pFont = get_Font(_T(".Font.Disabled"), _T("Button"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Disabled"), _T("Button"), Gdiplus::Color::LightGray);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Disabled"), _T("Button"), Gdiplus::Color::DarkGray);
			break;
		case sToolbarButton:
			pFont = get_Font(_T(".Font.Disabled"), _T("ToolbarButton"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Disabled"), _T("ToolbarButton"), Gdiplus::Color::LightGray);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Disabled"), _T("ToolbarButton"), Gdiplus::Color::DarkGray);
			break;
		case sMenuItem:
			pFont = get_Font(_T(".Font.Disabled"), _T("MenuItem"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Disabled"), _T("MenuItem"), Gdiplus::Color::LightGray);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Disabled"), _T("MenuItem"), Gdiplus::Color::DarkGray);
			break;
		}
	}
	else if ( m_clicked )
	{
		switch ( m_style )
		{
		case sLabel:
			pFont = get_Font(_T(".Font.Clicked"), _T("Label"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Clicked"), _T("Label"), Gdiplus::Color::Blue);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Clicked"), _T("Label"), Gdiplus::Color::White);
			break;
		case sButton:
			pFont = get_Font(_T(".Font.Clicked"), _T("Button"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Clicked"), _T("Button"), Gdiplus::Color::Blue);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Clicked"), _T("Button"), Gdiplus::Color::White);
			break;
		case sToolbarButton:
			pFont = get_Font(_T(".Font.Clicked"), _T("ToolbarButton"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Clicked"), _T("ToolbarButton"), Gdiplus::Color::Blue);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Clicked"), _T("ToolbarButton"), Gdiplus::Color::White);
			break;
		case sMenuItem:
			pFont = get_Font(_T(".Font.Clicked"), _T("MenuItem"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Clicked"), _T("MenuItem"), Gdiplus::Color::Blue);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Clicked"), _T("MenuItem"), Gdiplus::Color::White);
			break;
		}
	}
	else if ( m_hovered )
	{
		Gdiplus::Color vColor(Gdiplus::Color::Blue);
		Gdiplus::Color vColor1(Gdiplus::Color::MakeARGB(128, vColor.GetR(), vColor.GetG(), vColor.GetB()));

		switch ( m_style )
		{
		case sLabel:
			pFont = get_Font(_T(".Font.Hovered"), _T("Label"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Hovered"), _T("Label"), Gdiplus::Color::White);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Hovered"), _T("Label"), Gdiplus::Color::Black);
			pBrushHover = get_Brush(_T(".Color.HoverEffect"), _T("Label"), vColor1);
			break;
		case sButton:
			pFont = get_Font(_T(".Font.Hovered"), _T("Button"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Hovered"), _T("Button"), Gdiplus::Color::White);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Hovered"), _T("Button"), Gdiplus::Color::Black);
			pBrushHover = get_Brush(_T(".Color.HoverEffect"), _T("Button"), vColor1);
			break;
		case sToolbarButton:
			pFont = get_Font(_T(".Font.Hovered"), _T("ToolbarButton"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Hovered"), _T("ToolbarButton"), Gdiplus::Color::White);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Hovered"), _T("ToolbarButton"), Gdiplus::Color::Black);
			pBrushHover = get_Brush(_T(".Color.HoverEffect"), _T("ToolbarButton"), vColor1);
			break;
		case sMenuItem:
			pFont = get_Font(_T(".Font.Hovered"), _T("MenuItem"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Hovered"), _T("MenuItem"), Gdiplus::Color::White);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Hovered"), _T("MenuItem"), Gdiplus::Color::Black);
			pBrushHover = get_Brush(_T(".Color.HoverEffect"), _T("MenuItem"), vColor1);
			break;
		}
	}
	else
	{
		switch ( m_style )
		{
		case sLabel:
			pFont = get_Font(_T(".Font.Normal"), _T("Label"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Normal"), _T("Label"), Gdiplus::Color::White);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Normal"), _T("Label"), Gdiplus::Color::Black);
			break;
		case sButton:
			pFont = get_Font(_T(".Font.Normal"), _T("Button"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Normal"), _T("Button"), Gdiplus::Color::White);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Normal"), _T("Button"), Gdiplus::Color::Black);
			break;
		case sToolbarButton:
			pFont = get_Font(_T(".Font.Normal"), _T("ToolbarButton"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Normal"), _T("ToolbarButton"), Gdiplus::Color::White);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Normal"), _T("ToolbarButton"), Gdiplus::Color::Black);
			break;
		case sMenuItem:
			pFont = get_Font(_T(".Font.Normal"), _T("MenuItem"));
			pBrushBackground = get_Brush(_T(".BackgroundColor.Normal"), _T("MenuItem"), Gdiplus::Color::White);
			pBrushForeground = get_Brush(_T(".ForegroundColor.Normal"), _T("MenuItem"), Gdiplus::Color::Black);
			break;
		}
	}
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls OnPaint %d\n", get_name().GetString(), debugID);
#endif
	if ( m_updateRects )
	{
#ifdef __DEBUG1__
		theGuiApp->DebugString("In %ls OnPaint, m_updateRects == TRUE %d\n", get_name().GetString(), debugID);
#endif
		RECT r;

		::SetRectEmpty(&r);
		if ( m_sizeSpecX == TSizeSpecifierFixedSize )
			r.right = m_layoutSizeX - m_Margins.left - m_Margins.right;
		if ( m_sizeSpecY == TSizeSpecifierFixedSize )
			r.bottom = m_layoutSizeY - m_Margins.top - m_Margins.bottom;
		if ( pIcon )
		{
			::SetRectEmpty(&m_iconRect);
			m_iconRect.bottom = pIcon->GetHeight();
			m_iconRect.right = pIcon->GetWidth();
		}
		if ( !(m_text.IsEmpty()) )
		{
			Gdiplus::RectF rectF(0.0, 0.0, 0.0, 0.0);
			Gdiplus::PointF pt(0.0, 0.0);
			CStringConstIterator it(m_text.GetString());

			if ( m_ampersandpos < 0 )
			{
				it.Find(_T('&'));
				if ( !(it.IsEnd()) )
				{
					m_ampersandpos = Castdword(it.GetCurrent() - it.GetOrigin());
					m_text.DeleteString(m_ampersandpos);
				}
			}
			if ( m_tabulatorpos < 0 )
			{
				it.Find(_T('\t'));
				if ( !(it.IsEnd()) )
				{
					m_tabulatorpos = Castdword(it.GetCurrent() - it.GetOrigin());
					m_text.DeleteString(m_tabulatorpos);
				}
			}
			graphics.MeasureString(m_text.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &rectF);
			Convert2Rect(&m_textRect, &rectF);
			m_textRect.bottom += 2;
			if ( m_tabulatorpos >= 0 )
				m_textRect.right += 10;
		}
		if ( pIcon && (!(m_text.IsEmpty())) )
		{
			if ( (r.right == 0) || (r.bottom == 0) )
			{
				if ( (m_position == pleft) || (m_position == pright) )
				{
					if ( r.right == 0 )
						r.right = m_iconRect.right + m_textRect.right;
					if ( r.bottom == 0 )
						r.bottom = Max(m_iconRect.bottom, m_textRect.bottom);
				}
				else
				{
					if ( r.right == 0 )
						r.right = Max(m_iconRect.right, m_textRect.right);
					if ( r.bottom == 0 )
						r.bottom = m_iconRect.bottom + m_textRect.bottom;
				}
			}
			if ( (m_position == pleft) && (m_halign == hleft) && (m_valign == vtop) )
			{
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pleft) && (m_halign == hleft) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, 0, (r.bottom - m_textRect.bottom) / 2);
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pleft) && (m_halign == hleft) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, 0, r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pleft) && (m_halign == hcenter) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, (r.right - (m_textRect.right + m_iconRect.right)) / 2, 0);
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pleft) && (m_halign == hcenter) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, (r.right - (m_textRect.right + m_iconRect.right)) / 2, (r.bottom - m_textRect.bottom) / 2);
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pleft) && (m_halign == hcenter) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, (r.right - (m_textRect.right + m_iconRect.right)) / 2, r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pleft) && (m_halign == hright) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, r.right - (m_textRect.right + m_iconRect.right), 0);
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pleft) && (m_halign == hright) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, r.right - (m_textRect.right + m_iconRect.right), (r.bottom - m_textRect.bottom) / 2);
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pleft) && (m_halign == hright) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, r.right - (m_textRect.right + m_iconRect.right), r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hleft) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, m_iconRect.right, 0);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hleft) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, m_iconRect.right, (r.bottom - m_textRect.bottom) / 2);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hleft) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, m_iconRect.right, r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hcenter) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, ((r.right - (m_textRect.right + m_iconRect.right)) / 2) + m_iconRect.right, 0);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hcenter) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, ((r.right - (m_textRect.right + m_iconRect.right)) / 2) + m_iconRect.right, (r.bottom - m_textRect.bottom) / 2);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hcenter) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, ((r.right - (m_textRect.right + m_iconRect.right)) / 2) + m_iconRect.right, r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hright) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, (r.right - (m_textRect.right + m_iconRect.right)) + m_iconRect.right, 0);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hright) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, (r.right - (m_textRect.right + m_iconRect.right)) + m_iconRect.right, (r.bottom - m_textRect.bottom) / 2);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == pright) && (m_halign == hright) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, (r.right - (m_textRect.right + m_iconRect.right)) + m_iconRect.right, r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			}
			else if ( (m_position == ptop) && (m_halign == hleft) && (m_valign == vtop) )
			{
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == ptop) && (m_halign == hleft) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, 0, (r.bottom - m_textRect.bottom - m_iconRect.bottom) / 2);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == ptop) && (m_halign == hleft) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, 0, r.bottom - m_textRect.bottom - m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == ptop) && (m_halign == hcenter) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, 0);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == ptop) && (m_halign == hcenter) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, (r.bottom - m_textRect.bottom - m_iconRect.bottom) / 2);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == ptop) && (m_halign == hcenter) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, r.bottom - m_textRect.bottom - m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == ptop) && (m_halign == hright) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, r.right - m_textRect.right, 0);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == ptop) && (m_halign == hright) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, r.right - m_textRect.right, (r.bottom - m_textRect.bottom - m_iconRect.bottom) / 2);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == ptop) && (m_halign == hright) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, r.right - m_textRect.right, r.bottom - m_textRect.bottom - m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			}
			else if ( (m_position == pbottom) && (m_halign == hleft) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, 0, m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
			else if ( (m_position == pbottom) && (m_halign == hleft) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, 0, ((r.bottom - m_textRect.bottom - m_iconRect.bottom) / 2) + m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
			else if ( (m_position == pbottom) && (m_halign == hleft) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, 0, r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
			else if ( (m_position == pbottom) && (m_halign == hcenter) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
			else if ( (m_position == pbottom) && (m_halign == hcenter) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, ((r.bottom - m_textRect.bottom - m_iconRect.bottom) / 2) + m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
			else if ( (m_position == pbottom) && (m_halign == hcenter) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
			else if ( (m_position == pbottom) && (m_halign == hright) && (m_valign == vtop) )
			{
				::OffsetRect(&m_textRect, r.right - m_textRect.right, m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
			else if ( (m_position == pbottom) && (m_halign == hright) && (m_valign == vcenter) )
			{
				::OffsetRect(&m_textRect, r.right - m_textRect.right, ((r.bottom - m_textRect.bottom - m_iconRect.bottom) / 2) + m_iconRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
			else if ( (m_position == pbottom) && (m_halign == hright) && (m_valign == vbottom) )
			{
				::OffsetRect(&m_textRect, r.right - m_textRect.right, r.bottom - m_textRect.bottom);
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			}
		}
		else if ( pIcon )
		{
			if ( (r.right == 0) || (r.bottom == 0) )
			{
				if ( r.right == 0 )
					r.right = m_iconRect.right;
				if ( r.bottom == 0 )
					r.bottom = m_iconRect.bottom;
			}
			if ( (m_halign == hleft) && (m_valign == vcenter) )
				::OffsetRect(&m_iconRect, 0, (r.bottom - m_iconRect.bottom) / 2);
			else if ( (m_halign == hleft) && (m_valign == vbottom) )
				::OffsetRect(&m_iconRect, 0, r.bottom - m_iconRect.bottom);
			else if ( (m_halign == hcenter) && (m_valign == vtop) )
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, 0);
			else if ( (m_halign == hcenter) && (m_valign == vcenter) )
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, (r.bottom - m_iconRect.bottom) / 2);
			else if ( (m_halign == hcenter) && (m_valign == vbottom) )
				::OffsetRect(&m_iconRect, (r.right - m_iconRect.right) / 2, r.bottom - m_iconRect.bottom);
			else if ( (m_halign == hright) && (m_valign == vtop) )
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, 0);
			else if ( (m_halign == hright) && (m_valign == vcenter) )
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, (r.bottom - m_iconRect.bottom) / 2);
			else if ( (m_halign == hright) && (m_valign == vbottom) )
				::OffsetRect(&m_iconRect, r.right - m_iconRect.right, r.bottom - m_iconRect.bottom);
		}
		else if ( !(m_text.IsEmpty()) )
		{
			if ( (r.right == 0) || (r.bottom == 0) )
			{
				if ( r.right == 0 )
					r.right = m_textRect.right;
				if ( r.bottom == 0 )
					r.bottom = m_textRect.bottom;
			}
			if ( (m_halign == hleft) && (m_valign == vcenter) )
				::OffsetRect(&m_textRect, 0, (r.bottom - m_textRect.bottom) / 2);
			else if ( (m_halign == hleft) && (m_valign == vbottom) )
				::OffsetRect(&m_textRect, 0, r.bottom - m_textRect.bottom);
			else if ( (m_halign == hcenter) && (m_valign == vtop) )
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, 0);
			else if ( (m_halign == hcenter) && (m_valign == vcenter) )
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, (r.bottom - m_textRect.bottom) / 2);
			else if ( (m_halign == hcenter) && (m_valign == vbottom) )
				::OffsetRect(&m_textRect, (r.right - m_textRect.right) / 2, r.bottom - m_textRect.bottom);
			else if ( (m_halign == hright) && (m_valign == vtop) )
				::OffsetRect(&m_textRect, r.right - m_textRect.right, 0);
			else if ( (m_halign == hright) && (m_valign == vcenter) )
				::OffsetRect(&m_textRect, r.right - m_textRect.right, (r.bottom - m_textRect.bottom) / 2);
			else if ( (m_halign == hright) && (m_valign == vbottom) )
				::OffsetRect(&m_textRect, r.right - m_textRect.right, r.bottom - m_textRect.bottom);
		}
		::OffsetRect(&m_iconRect, m_Margins.left, m_Margins.top);
		::OffsetRect(&m_textRect, m_Margins.left, m_Margins.top);
		r.right += m_Margins.right + m_Margins.left;
		r.bottom += m_Margins.bottom + m_Margins.top;
		::CopyRect(&m_maxClientArea, &r);
		if ( (m_sizeSpecX != TSizeSpecifierFixedSize) && (m_sizeSpecY != TSizeSpecifierFixedSize) )
		{
			if ( m_hasborder )
			{
				m_maxClientArea.bottom += 2;
				m_maxClientArea.right += 2;
			}
			m_maxClientArea.bottom += 4;
			m_maxClientArea.right += 4;
		}
		m_updateRects = FALSE;
	}

	Gdiplus::RectF borderRectF;

	Convert2RectF(&borderRectF, &clientRect);
	bitmapgraphics.FillRectangle(pBrushBackground, borderRectF);
	if ( pIcon )
	{
		Gdiplus::Color transparentcolor;
		Gdiplus::ImageAttributes imAtt;
		Gdiplus::Rect destRect;

		pIcon->GetPixel(0, pIcon->GetHeight() - 1, &transparentcolor);
		imAtt.SetColorKey(transparentcolor, transparentcolor, Gdiplus::ColorAdjustTypeBitmap);
		destRect.X = m_iconRect.left;
		destRect.Y = m_iconRect.top;
		destRect.Height = m_iconRect.bottom - m_iconRect.top;
		destRect.Width = m_iconRect.right - m_iconRect.left;

		bitmapgraphics.DrawImage(pIcon, destRect, 0, 0, 
			pIcon->GetWidth(), 
			pIcon->GetHeight(), Gdiplus::UnitPixel, &imAtt);
	}
	if ( !(m_text.IsEmpty()) )
	{
		Gdiplus::PointF pt(Cast(Gdiplus::REAL,m_textRect.left), Cast(Gdiplus::REAL,m_textRect.top));

		drawString(&bitmapgraphics, pFont, pt, pBrushForeground);
	}
	if ( pBrushHover )
		bitmapgraphics.FillRectangle(pBrushHover, borderRectF);
	else if ( m_hovered )
	{
		Gdiplus::Pen grayPen(Gdiplus::Color::Gray);

		borderRectF.Y += 1; borderRectF.Height -= 6;
		borderRectF.X += 1; borderRectF.Width -= 6;
		grayPen.SetDashStyle(Gdiplus::DashStyleDot);

		bitmapgraphics.DrawRectangle(&grayPen, borderRectF);
	}
	
	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls OnPaint %d\n", get_name().GetString(), debugID);
#endif
	return 0;
}

void CStatic::drawString(Gdiplus::Graphics* pGraphics, Gdiplus::Font* pFont, Gdiplus::PointF& pt, Gdiplus::Brush* brush)
{
	if ( (m_ampersandpos < 0) && (m_tabulatorpos < 0) )
	{
		pGraphics->DrawString(m_text.GetString(), -1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), brush);
		return;
	}
	if ( m_ampersandpos < 0 )
	{
		Gdiplus::RectF borderRectF;
		Gdiplus::RectF imageRectF;
		Gdiplus::RectF r1;
		Gdiplus::RectF r2;
		Gdiplus::PointF pt1;

		Convert2RectF(&borderRectF, &m_maxClientArea);
		Convert2RectF(&imageRectF, &m_iconRect);

		pGraphics->MeasureString(m_text.GetString(), m_tabulatorpos, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &r1);
		pGraphics->DrawString(m_text.GetString(), m_tabulatorpos, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), brush);

		pt1.X = pt.X + r1.Width;
		pt1.Y = pt.Y;
		pGraphics->MeasureString(m_text.GetString() + m_tabulatorpos, -1, pFont, pt1, Gdiplus::StringFormat::GenericTypographic(), &r2);
		pt1.X += borderRectF.Width - pt1.X - r2.Width - m_Margins.right - 7;
		if ( m_position == pleft )
			pt1.X += m_iconRect.left - m_iconRect.right;
		pGraphics->DrawString(m_text.GetString() + m_tabulatorpos, -1, pFont, pt1, Gdiplus::StringFormat::GenericTypographic(), brush);
		return;
	}
	if ( m_tabulatorpos < 0 )
	{
		Gdiplus::RectF r1;
		Gdiplus::RectF r2;
		Gdiplus::PointF pt1;
		Gdiplus::PointF pt2;
		Gdiplus::FontFamily fontFamily;
		pFont->GetFamily(&fontFamily);
		Gdiplus::Font pFont1(&fontFamily, pFont->GetSize(), pFont->GetStyle() | Gdiplus::FontStyleUnderline, pFont->GetUnit());

		pGraphics->MeasureString(m_text.GetString(), m_ampersandpos, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &r1);
		pGraphics->DrawString(m_text.GetString(), m_ampersandpos, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), brush);

		pt1.X = pt.X + r1.Width;
		pt1.Y = pt.Y;
		pGraphics->MeasureString(m_text.GetString() + m_ampersandpos, 1, &pFont1, pt1, Gdiplus::StringFormat::GenericTypographic(), &r2);
		pGraphics->DrawString(m_text.GetString() + m_ampersandpos, 1, &pFont1, pt1, Gdiplus::StringFormat::GenericTypographic(), brush);
		pt2.X = pt1.X + r2.Width;
		pt2.Y = pt1.Y;
		pGraphics->DrawString(m_text.GetString() + m_ampersandpos + 1, -1, pFont, pt2, Gdiplus::StringFormat::GenericTypographic(), brush);
		return;
	}
	if ( m_ampersandpos < m_tabulatorpos )
	{
		Gdiplus::RectF borderRectF;
		Gdiplus::RectF imageRectF;
		Gdiplus::RectF r1;
		Gdiplus::RectF r2;
		Gdiplus::RectF r3;
		Gdiplus::RectF r4;
		Gdiplus::PointF pt1;
		Gdiplus::PointF pt2;
		Gdiplus::PointF pt3;
		Gdiplus::FontFamily fontFamily;

		pFont->GetFamily(&fontFamily);

		Gdiplus::Font pFont1(&fontFamily, pFont->GetSize(), pFont->GetStyle() | Gdiplus::FontStyleUnderline, pFont->GetUnit());
		INT len;

		pGraphics->MeasureString(m_text.GetString(), m_ampersandpos, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &r1);
		pGraphics->DrawString(m_text.GetString(), m_ampersandpos, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), brush);

		pt1.X = pt.X + r1.Width;
		pt1.Y = pt.Y;
		pGraphics->MeasureString(m_text.GetString() + m_ampersandpos, 1, &pFont1, pt1, Gdiplus::StringFormat::GenericTypographic(), &r2);
		pGraphics->DrawString(m_text.GetString() + m_ampersandpos, 1, &pFont1, pt1, Gdiplus::StringFormat::GenericTypographic(), brush);
		pt2.X = pt1.X + r2.Width;
		pt2.Y = pt1.Y;
		len = m_tabulatorpos - m_ampersandpos - 1;
		pGraphics->MeasureString(m_text.GetString() + m_ampersandpos + 1, len, &pFont1, pt2, Gdiplus::StringFormat::GenericTypographic(), &r3);
		pGraphics->DrawString(m_text.GetString() + m_ampersandpos + 1, len, pFont, pt2, Gdiplus::StringFormat::GenericTypographic(), brush);

		Convert2RectF(&borderRectF, &m_maxClientArea);
		Convert2RectF(&imageRectF, &m_iconRect);

		pt3.X = pt2.X + r3.Width;
		pt3.Y = pt2.Y;
		pGraphics->MeasureString(m_text.GetString() + m_tabulatorpos, -1, pFont, pt3, Gdiplus::StringFormat::GenericTypographic(), &r4);
		pt3.X += borderRectF.Width - pt3.X - r4.Width - m_Margins.right - 7;
		if ( m_position == pleft )
			pt3.X += m_iconRect.left - m_iconRect.right;
		pGraphics->DrawString(m_text.GetString() + m_tabulatorpos, -1, pFont, pt3, Gdiplus::StringFormat::GenericTypographic(), brush);
		return;
	}
}

LRESULT CStatic::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch ( m_style )
	{
	case sLabel:
	case sMenuItem:
		break;
	default:
		{
			bool bUpdate = false;

			set_focus();
			if ( get_TrackMouse() )
				StopTrackMouse();
			if ( m_hovered )
			{
				m_hovered = false;
				bUpdate = true;
			}
			if ( wParam == MK_LBUTTON )
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

				m_clicked = true;
				bUpdate = true;
				StartTrackMouse();
			}
			if ( bUpdate )
				Update(FALSE);
		}
		break;
	}
	return 0;
}

LRESULT CStatic::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{
		switch ( m_style )
		{
		case sLabel:
		case sMenuItem:
			break;
		default:
			if ( get_TrackMouse() )
				StopTrackMouse();
			m_clicked = false;
			Update(FALSE);
			SendNotifyMessage(NM_BUTTONCLICKED, (LPARAM)this);
			break;
		}
	}
	return 0;
}

LRESULT CStatic::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{
		switch ( m_style )
		{
		case sLabel:
		case sMenuItem:
			break;
		default:
			if ( get_TrackMouse() )
				StopTrackMouse();
			m_clicked = false;
			Update(FALSE);
			SendNotifyMessage(NM_BUTTONDOUBLECLICKED, (LPARAM)this);
			break;
		}
	}
	return 0;
}

LRESULT CStatic::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{
		switch ( m_style )
		{
		case sLabel:
			break;
		case sMenuItem:
			SendNotifyMessage(NM_BUTTONMOUSEMOVE, (LPARAM)this);
			break;
		default:
			if ( !(get_TrackMouse()) )
				StartTrackMouse();
			break;
		}
		
	}
	return 0;
}

LRESULT CStatic::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	set_TrackMouse(FALSE);
	m_hovered = false;
	m_clicked = false;
	Update(FALSE);
	return 0;
}

LRESULT CStatic::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{
		set_focus();
		m_hovered = true;
		Update(FALSE);
	}
	return 0;
}

LRESULT CStatic::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch ( m_style )
	{
	case sLabel:
	case sMenuItem:
		break;
	default:
		{
			BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
			BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

			switch ( wParam )
			{
			case VK_SPACE:
				if ( bShift || bControl )
					break;
				if ( get_TrackMouse() )
					StopTrackMouse();
				m_hovered = false;
				m_clicked = true;
				Update(FALSE);
				break;
			default:
				break;
			}
		}
		break;
	}
	return 0;
}

LRESULT CStatic::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == VK_F10 )
		return DefaultWindowProc(WM_KEYUP, wParam, lParam);

	switch ( m_style )
	{
	case sLabel:
	case sMenuItem:
		break;
	default:
		{
			BOOL bShift = GetKeyState(VK_SHIFT) & SHIFTED;
			BOOL bControl = GetKeyState(VK_CONTROL) & SHIFTED;

			switch ( wParam )
			{
			case VK_SPACE:
				if ( bShift || bControl )
					break;
				m_hovered = false;
				m_clicked = false;
				Update(FALSE);
				SendNotifyMessage(NM_BUTTONCLICKED, (LPARAM)this);
				break;
			default:
				break;
			}
		}
		break;
	}
	return 0;
}

LRESULT CStatic::OnEditNotImplCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CStatic::OnCopyCommand(WPARAM wParam, LPARAM lParam)
{
	if ( HIWORD(wParam) > 1 )
		return TRUE;

	if ( !OpenClipboard(theGuiApp->get_MainWnd()->get_handle()) ) 
        return 0; 
    EmptyClipboard();

	int cch = m_text.GetLength();

	if ( cch > 0 )
	{
		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR));

		if ( hglbCopy ) 
		{
			LPTSTR lptstrCopy = (LPTSTR)(GlobalLock(hglbCopy));

			MoveMemory(lptstrCopy, m_text.GetString(), cch * sizeof(TCHAR));
			lptstrCopy[cch] = (TCHAR)0;
			GlobalUnlock(hglbCopy);
 
			SetClipboardData(CF_UNICODETEXT, hglbCopy); 
		}
	}

	HGLOBAL hIconBitmap = m_iconN.get_clipboarddata();

	if ( hIconBitmap )
		SetClipboardData(CF_DIB, hIconBitmap); 

	SendNotifyMessage(NM_SELECTIONCHANGED);

	CloseClipboard(); 
	return 0;
}

LRESULT CStatic::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//***********************************************************
// CPictureBox
//***********************************************************
BEGIN_MESSAGE_MAP(CControl, CPictureBox)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_COMMMAND(IDM_CUT,OnEditNotImplCommand)
	ON_WM_COMMMAND(IDM_COPY,OnCopyCommand)
	ON_WM_COMMMAND(IDM_DELETE,OnEditNotImplCommand)
	ON_WM_COMMMAND(IDM_PASTE,OnEditNotImplCommand)
	ON_WM_COMMMAND(IDM_UNDO,OnEditNotImplCommand)
	ON_WM_COMMMAND(IDM_REDO,OnEditNotImplCommand)
END_MESSAGE_MAP()

CPictureBox::CPictureBox(LPCTSTR name): 
	CControl(name),
	m_filename(),
	m_picture(nullptr)
{
}

CPictureBox::CPictureBox(ConstRef(CStringBuffer) name): 
	CControl(name),
	m_filename(),
	m_picture(nullptr)
{
}

CPictureBox::~CPictureBox()
{
	if ( m_picture ) 
		delete m_picture;
}

BOOL CPictureBox::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CPICTUREBOX");
	return TRUE;
}

CWin* CPictureBox::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CPictureBox();

	return pWin;
}

void CPictureBox::set_picture(const Gdiplus::Image* picture)
{
	m_filename.Clear();
	if ( m_picture ) 
		delete m_picture;
	m_picture = picture;
}

void CPictureBox::set_picture(LPCTSTR filename)
{
	m_filename.SetString(__FILE__LINE__ filename);
	if ( m_picture ) 
		delete m_picture;
	m_picture = Gdiplus::Image::FromFile(filename);
}

LRESULT CPictureBox::OnPaint(WPARAM wParam, LPARAM lParam)
{
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
	Gdiplus::Brush* brush = get_Brush(_T(".Color.Workspace"), _T("PictureBox"), Gdiplus::Color::Gray);

	if ( m_updateRects )
	{
		::SetRectEmpty(&m_maxClientArea);
		if ( m_picture )
		{
			m_maxClientArea.bottom = CastMutablePtr(Gdiplus::Image, m_picture)->GetHeight();
			m_maxClientArea.right = CastMutablePtr(Gdiplus::Image, m_picture)->GetWidth();
		}
		m_updateRects = FALSE;
	}

	Gdiplus::RectF borderRectF;
	RECT r;

	GetClientRect(&r);
	Convert2RectF(&borderRectF, &r);

	bitmapgraphics.FillRectangle(brush, borderRectF);
	if ( m_picture )
		bitmapgraphics.DrawImage(CastMutablePtr(Gdiplus::Image, m_picture), 0, 0);

	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);
	return 0;
}

LRESULT CPictureBox::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	set_focus();
	//MessageBox(theGuiApp->get_MainWnd()->get_handle(), _T("CPictureBox::OnLButtonDown"), _T("Test"), MB_ICONEXCLAMATION | MB_OK);
	return 0;
}

LRESULT CPictureBox::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPictureBox::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	MessageBox(theGuiApp->get_MainWnd()->get_handle(), _T("CPictureBox::OnLButtonDblClk"), _T("Test"), MB_ICONEXCLAMATION | MB_OK);
	return 0;
}

LRESULT CPictureBox::OnEditNotImplCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPictureBox::OnCopyCommand(WPARAM wParam, LPARAM lParam)
{
	if ( HIWORD(wParam) > 1 )
		return TRUE;

	return 0;
}

LRESULT CPictureBox::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//***********************************************************
// CSplitter
//***********************************************************
BEGIN_MESSAGE_MAP(CControl, CSplitter)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CSplitter::CSplitter(LPCTSTR name): 
	CControl(name),
	m_direction(TDirectionHorizontal),
	m_cursor(nullptr),
	m_firstControl(nullptr),
	m_secondControl(nullptr)
{
	m_mouseLastPos.x = 0;
	m_mouseLastPos.y = 0;
}

CSplitter::CSplitter(ConstRef(CStringBuffer) name): 
	CControl(name),
	m_direction(TDirectionHorizontal),
	m_cursor(nullptr),
	m_firstControl(nullptr),
	m_secondControl(nullptr)
{
	m_mouseLastPos.x = 0;
	m_mouseLastPos.y = 0;
}

CSplitter::~CSplitter()
{
}

void CSplitter::set_Direction(TDirection dir)
{
	m_direction = dir;
	switch ( m_direction )
	{
	case TDirectionHorizontal:
		m_cursor = LoadCursor(nullptr, MAKEINTRESOURCE(IDC_SIZENS));
		break;
	case TDirectionVertical:
		m_cursor = LoadCursor(nullptr, MAKEINTRESOURCE(IDC_SIZEWE));
		break;
	}
}

BOOL CSplitter::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW;
	cls.hCursor         = nullptr;
	cls.lpszClassName	= _T("CSPLITTER");
	return TRUE;
}

LRESULT CSplitter::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		ClientToScreen(&pt);
		m_mouseLastPos = pt;
		set_Capture(TRUE);
	}
	return 0;
}

LRESULT CSplitter::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if ( has_Capture() )
		set_Capture(FALSE);
	return 0;
}

LRESULT CSplitter::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( (!m_firstControl) || (!m_secondControl) )
		return 1;
	if ( (wParam == MK_LBUTTON) && has_Capture() )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		LONG delta;
		LONG size;

		ClientToScreen(&pt);

		switch ( m_direction )
		{
		case TDirectionHorizontal:
			delta = pt.y - m_mouseLastPos.y;
			size = m_firstControl->get_LayoutSizeY();
			size += delta;
			m_firstControl->set_LayoutSizeY(size);
			break;
		case TDirectionVertical:
			delta = pt.x - m_mouseLastPos.x;
			size = m_firstControl->get_LayoutSizeX();
			size += delta;
			m_firstControl->set_LayoutSizeX(size);
			break;
		}
		get_parent()->SendMessage(WM_SIZE, 0, 0);
		m_mouseLastPos = pt;
	}
	return 0;
}

LRESULT CSplitter::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if ( m_cursor )
		::SetCursor(m_cursor);
	return 0;
}

LRESULT CSplitter::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//***********************************************************
// CScrollbar
//***********************************************************
BEGIN_MESSAGE_MAP(CControl, CScrollbar)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MESSAGE(SBM_ENABLE_ARROWS, OnEnableArrows)
	ON_WM_MESSAGE(SBM_GETPOS, OnGetPos)
	ON_WM_MESSAGE(SBM_GETRANGE, OnGetRange)
	ON_WM_MESSAGE(SBM_GETSCROLLBARINFO, OnGetScrollBarInfo)
	ON_WM_MESSAGE(SBM_GETSCROLLINFO, OnGetScrollInfo)
	ON_WM_MESSAGE(SBM_SETPOS, OnSetPos)
	ON_WM_MESSAGE(SBM_SETRANGE, OnSetRange)
	ON_WM_MESSAGE(SBM_SETRANGEREDRAW, OnSetRangeRedraw)
	ON_WM_MESSAGE(SBM_SETSCROLLINFO, OnSetScrollInfo)
END_MESSAGE_MAP()

CScrollbar::CScrollbar(LPCTSTR name): 
	CControl(name),
	m_direction(TDirectionHorizontal),
	m_ScrollWindow(nullptr),
	m_iRect(0),
	m_EnableArrows(0),
	m_Minimum(0),
	m_Maximum(0),
	m_Position(0),
	m_Page(0),
	m_bTrackPos(false),
	m_TrackPos(0),
	m_ArrowBottom(MAKEINTRESOURCE(IDB_ARROWBOTTOM), IMAGE_BITMAP),
	m_ArrowLeft(MAKEINTRESOURCE(IDB_ARROWLEFT), IMAGE_BITMAP),
	m_ArrowRight(MAKEINTRESOURCE(IDB_ARROWRIGHT), IMAGE_BITMAP),
	m_ArrowTop(MAKEINTRESOURCE(IDB_ARROWTOP), IMAGE_BITMAP)
{
	m_mouseLastPos.x = 0;
	m_mouseLastPos.y = 0;
	::SetRectEmpty(&m_TopRightArrow);
	::SetRectEmpty(&m_UpRightPageArea);
	::SetRectEmpty(&m_Thumb);
	::SetRectEmpty(&m_DownLeftPageArea);
	::SetRectEmpty(&m_BottomLeftArrow);
}

CScrollbar::CScrollbar(ConstRef(CStringBuffer) name): 
	CControl(name),
	m_direction(TDirectionHorizontal),
	m_ScrollWindow(nullptr),
	m_iRect(0),
	m_EnableArrows(0),
	m_Minimum(0),
	m_Maximum(0),
	m_Position(0),
	m_Page(0),
	m_bTrackPos(false),
	m_TrackPos(0),
	m_ArrowBottom(MAKEINTRESOURCE(IDB_ARROWBOTTOM), IMAGE_BITMAP),
	m_ArrowLeft(MAKEINTRESOURCE(IDB_ARROWLEFT), IMAGE_BITMAP),
	m_ArrowRight(MAKEINTRESOURCE(IDB_ARROWRIGHT), IMAGE_BITMAP),
	m_ArrowTop(MAKEINTRESOURCE(IDB_ARROWTOP), IMAGE_BITMAP)
{
	m_mouseLastPos.x = 0;
	m_mouseLastPos.y = 0;
	::SetRectEmpty(&m_TopRightArrow);
	::SetRectEmpty(&m_UpRightPageArea);
	::SetRectEmpty(&m_Thumb);
	::SetRectEmpty(&m_DownLeftPageArea);
	::SetRectEmpty(&m_BottomLeftArrow);
}

CScrollbar::~CScrollbar()
{
}

LRESULT CScrollbar::_SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( m_ScrollWindow )
		return m_ScrollWindow->SendMessage(message, wParam, lParam);
	else
		return get_parent()->SendMessage(message, wParam, lParam);
}

void CScrollbar::_set_Visible(BOOL bVisible)
{
	set_Visible(bVisible);
	if ( m_ScrollWindow )
	{
		switch ( m_direction )
		{
		case TDirectionHorizontal:
			m_ScrollWindow->set_HScrollVisible(bVisible);
			break;
		case TDirectionVertical:
			m_ScrollWindow->set_VScrollVisible(bVisible);
			break;
		}
	}
	else
	{
		CControl * pParent = CastDynamicPtr(CControl, get_parent());

		if ( pParent )
		{
			switch ( m_direction )
			{
			case TDirectionHorizontal:
				pParent->set_HScrollVisible(bVisible);
				break;
			case TDirectionVertical:
				pParent->set_VScrollVisible(bVisible);
				break;
			}
		}
	}
}

BOOL CScrollbar::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW;
	cls.lpszClassName	= _T("CSCROLLBAR");
	return TRUE;
}

LRESULT CScrollbar::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;
	
	Gdiplus::Graphics graphics(hdc);
	RECT clientRect;

	GetClientRect(&clientRect);

	if ( m_updateRects )
	{
		::SetRectEmpty(&m_maxClientArea);
		switch ( m_direction )
		{
		case TDirectionHorizontal:
			m_maxClientArea.bottom = 17;
			m_maxClientArea.right = clientRect.right;
			break;
		case TDirectionVertical:
			m_maxClientArea.bottom = clientRect.bottom;
			m_maxClientArea.right = 17;
			break;
		}
		m_updateRects = FALSE;
	}

	Gdiplus::Bitmap bitmap(clientRect.right, clientRect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);
	Gdiplus::Brush* workspaceBrush = get_Brush(_T(".Color.Workspace"), _T("Scrollbar"), Gdiplus::Color::Gray);
	Gdiplus::Brush* thumbBrush = get_Brush(_T(".Color.Thumb"), _T("Scrollbar"), Gdiplus::Color::DarkGray);
	Gdiplus::Bitmap* pBottomLeftArrow = nullptr;
	Gdiplus::Bitmap* pTopRightArrow = nullptr;
	Gdiplus::Pen blackpen(Gdiplus::Color::Black);
	Gdiplus::RectF borderRectF;
	double pos = (Cast(double, m_Position) - Cast(double, m_Minimum)) / (Cast(double, m_Maximum) - Cast(double, m_Minimum));
	double page = Cast(double, m_Page) / (Cast(double, m_Maximum) - Cast(double, m_Minimum));
	Gdiplus::Color vColor(Gdiplus::Color::Blue);
	Gdiplus::Color vColor1(Gdiplus::Color::MakeARGB(128, vColor.GetR(), vColor.GetG(), vColor.GetB()));
	Gdiplus::Brush* pBrushHover = get_Brush(_T(".Color.HoverEffect"), _T("Scrollbar"), vColor1);

	Convert2RectF(&borderRectF, &clientRect);
	bitmapgraphics.FillRectangle(workspaceBrush, borderRectF);

	switch ( m_direction )
	{
	case TDirectionHorizontal:
		pBottomLeftArrow = m_ArrowLeft.get_bitmap();
		pTopRightArrow = m_ArrowRight.get_bitmap();
		::SetRect(&m_BottomLeftArrow, 0, 0, 17, 16);
		::SetRect(&m_TopRightArrow, clientRect.right - 17, 0, clientRect.right - 1, 16);
		page *= (Cast(double, clientRect.right) - 34.0);
		if ( page < 5.0 )
			page = 5.0;
		pos = (pos * (Cast(double, clientRect.right) - 34.0 - page)) + 17.0;
		::SetRect(&m_Thumb, Cast(LONG, floor(pos + 0.5)), 0, Cast(LONG, floor(pos + page + 0.5)), 16);
		::SetRect(&m_DownLeftPageArea, 17, 0, m_Thumb.left, 16);
		::SetRect(&m_UpRightPageArea, m_Thumb.right, 0, clientRect.right - 17, 16);
		break;
	case TDirectionVertical:
		pBottomLeftArrow = m_ArrowBottom.get_bitmap();
		pTopRightArrow = m_ArrowTop.get_bitmap();
		::SetRect(&m_TopRightArrow, 0, 0, 16, 17);
		::SetRect(&m_BottomLeftArrow, 0, clientRect.bottom - 17, 16, clientRect.bottom - 1);
		page *= (Cast(double, clientRect.bottom) - 34.0);
		if ( page < 5.0 )
			page = 5.0;
		pos = (pos * (Cast(double, clientRect.bottom) - 34.0 - page)) + 17.0;
		::SetRect(&m_Thumb, 0, Cast(LONG, floor(pos + 0.5)), 16, Cast(LONG, floor(pos + page + 0.5)));
		::SetRect(&m_UpRightPageArea, 0, 17, 16, m_Thumb.top);
		::SetRect(&m_DownLeftPageArea, 0, m_Thumb.bottom, 16, clientRect.bottom - 17);
		break;
	}
	Convert2RectF(&borderRectF, &m_BottomLeftArrow);
	if ( pBottomLeftArrow )
	{
		Gdiplus::Color transparentcolor;
		Gdiplus::ImageAttributes imAtt;

		pBottomLeftArrow->GetPixel(0, pBottomLeftArrow->GetHeight() - 1, &transparentcolor);
		imAtt.SetColorKey(transparentcolor, transparentcolor, Gdiplus::ColorAdjustTypeBitmap);

		bitmapgraphics.DrawImage(pBottomLeftArrow, borderRectF, 0.0, 0.0, 
			Cast(Gdiplus::REAL, pBottomLeftArrow->GetWidth()), 
			Cast(Gdiplus::REAL, pBottomLeftArrow->GetHeight()), Gdiplus::UnitPixel, &imAtt);
	}
	bitmapgraphics.DrawRectangle(&blackpen, borderRectF);
	if ( m_iRect == 5 )
		bitmapgraphics.FillRectangle(pBrushHover, borderRectF);
	Convert2RectF(&borderRectF, &m_TopRightArrow);
	if ( pTopRightArrow )
	{
		Gdiplus::Color transparentcolor;
		Gdiplus::ImageAttributes imAtt;

		pTopRightArrow->GetPixel(0, pTopRightArrow->GetHeight() - 1, &transparentcolor);
		imAtt.SetColorKey(transparentcolor, transparentcolor, Gdiplus::ColorAdjustTypeBitmap);

		bitmapgraphics.DrawImage(pTopRightArrow, borderRectF, 0.0, 0.0, 
			Cast(Gdiplus::REAL, pTopRightArrow->GetWidth()), 
			Cast(Gdiplus::REAL, pTopRightArrow->GetHeight()), Gdiplus::UnitPixel, &imAtt);
	}
	bitmapgraphics.DrawRectangle(&blackpen, borderRectF);
	if ( m_iRect == 1 )
		bitmapgraphics.FillRectangle(pBrushHover, borderRectF);
	Convert2RectF(&borderRectF, &m_Thumb);
	bitmapgraphics.FillRectangle(thumbBrush, borderRectF);
	bitmapgraphics.DrawRectangle(&blackpen, borderRectF);
	Convert2RectF(&borderRectF, &m_DownLeftPageArea);
	bitmapgraphics.DrawRectangle(&blackpen, borderRectF);
	if ( m_iRect == 4 )
		bitmapgraphics.FillRectangle(pBrushHover, borderRectF);
	Convert2RectF(&borderRectF, &m_UpRightPageArea);
	bitmapgraphics.DrawRectangle(&blackpen, borderRectF);
	if ( m_iRect == 2 )
		bitmapgraphics.FillRectangle(pBrushHover, borderRectF);

	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);
	return 0;
}

LRESULT CScrollbar::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CScrollbar::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		m_bTrackPos = false;
		m_iRect = 0;
		m_mouseLastPos = pt;
		StopTimer(1000);
		switch ( m_direction )
		{
		case TDirectionHorizontal:
			if ( ::PtInRect(&m_BottomLeftArrow, pt) )
			{
				if ( m_DownLeftPageArea.right <= m_DownLeftPageArea.left )
				{
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_LEFT,0), (LPARAM)(get_handle()));
					break;
				}
				_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_LINELEFT,0), (LPARAM)(get_handle()));
				m_iRect = 5;
				StartTimer(1000, 500);
				break;
			}
			if ( ::PtInRect(&m_DownLeftPageArea, pt) )
			{
				_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_PAGELEFT,0), (LPARAM)(get_handle()));
				m_iRect = 4;
				StartTimer(1000, 500);
				break;
			}
			if ( ::PtInRect(&m_Thumb, pt) )
			{
				m_bTrackPos = true;
				break;
			}
			if ( ::PtInRect(&m_UpRightPageArea, pt) )
			{
				_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_PAGERIGHT,0), (LPARAM)(get_handle()));
				m_iRect = 2;
				StartTimer(1000, 500);
				break;
			}
			if ( ::PtInRect(&m_TopRightArrow, pt) )
			{
				if ( m_UpRightPageArea.right <= m_UpRightPageArea.left )
				{
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_RIGHT,0), (LPARAM)(get_handle()));
					break;
				}
				_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_LINERIGHT,0), (LPARAM)(get_handle()));
				m_iRect = 1;
				StartTimer(1000, 500);
				break;
			}
			break;
		case TDirectionVertical:
			if ( ::PtInRect(&m_BottomLeftArrow, pt) )
			{
				if ( m_DownLeftPageArea.bottom <= m_DownLeftPageArea.top )
				{
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_BOTTOM,0), (LPARAM)(get_handle()));
					break;
				}
				_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN,0), (LPARAM)(get_handle()));
				m_iRect = 5;
				StartTimer(1000, 500);
				break;
			}
			if ( ::PtInRect(&m_DownLeftPageArea, pt) )
			{
				_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_PAGEDOWN,0), (LPARAM)(get_handle()));
				m_iRect = 4;
				StartTimer(1000, 500);
				break;
			}
			if ( ::PtInRect(&m_Thumb, pt) )
			{
				m_bTrackPos = true;
				break;
			}
			if ( ::PtInRect(&m_UpRightPageArea, pt) )
			{
				_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_PAGEUP,0), (LPARAM)(get_handle()));
				m_iRect = 2;
				StartTimer(1000, 500);
				break;
			}
			if ( ::PtInRect(&m_TopRightArrow, pt) )
			{
				if ( m_UpRightPageArea.bottom <= m_UpRightPageArea.top )
				{
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_TOP,0), (LPARAM)(get_handle()));
					break;
				}
				_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP,0), (LPARAM)(get_handle()));
				m_iRect = 1;
				StartTimer(1000, 500);
				break;
			}
			break;
		}
	}
	return 0;
}

LRESULT CScrollbar::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_iRect = 0;
	StopTimer(1000);
	Update(FALSE);
	return 0;
}

LRESULT CScrollbar::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( (wParam == MK_LBUTTON) && m_bTrackPos )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		double page = Cast(double, m_Page) / (Cast(double, m_Maximum) - Cast(double, m_Minimum));
		LONG thumb;
		LONG trackpos;
		RECT clientRect;

		GetClientRect(&clientRect);
		switch ( m_direction )
		{
		case TDirectionHorizontal:
			page *= (Cast(double, clientRect.right) - 34.0);
			if ( page < 5.0 )
				page = 5.0;
			thumb = m_Thumb.left + (pt.x - m_mouseLastPos.x);
			if ( thumb < 17 )
				thumb = 17;
			else if ( thumb > (clientRect.right - 17 - Cast(LONG, floor(page + 0.5))) )
				thumb = (clientRect.right - 17 - Cast(LONG, floor(page + 0.5)));
			trackpos = (Cast(LONG, floor(((Cast(double, (thumb - 17)) / (Cast(double, clientRect.right - 34) - page)) * Cast(double, (m_Maximum - m_Minimum))) + 0.5))) + m_Minimum;
			if ( trackpos != m_TrackPos )
			{
				m_TrackPos = trackpos;
				_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK,m_TrackPos), (LPARAM)(get_handle()));
				m_mouseLastPos = pt;
			}
			break;
		case TDirectionVertical:
			page *= (Cast(double, clientRect.bottom) - 34.0);
			if ( page < 5.0 )
				page = 5.0;
			thumb = m_Thumb.top + (pt.y - m_mouseLastPos.y);
			if ( thumb < 17 )
				thumb = 17;
			else if ( thumb > (clientRect.bottom - 17 - Cast(LONG, floor(page + 0.5))) )
				thumb = (clientRect.bottom - 17 - Cast(LONG, floor(page + 0.5)));
			trackpos = (Cast(LONG, floor(((Cast(double, (thumb - 17)) / (Cast(double, clientRect.bottom - 34) - page)) * Cast(double, (m_Maximum - m_Minimum))) + 0.5))) + m_Minimum;
			if ( trackpos != m_TrackPos )
			{
				m_TrackPos = trackpos;
				_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK,m_TrackPos), (LPARAM)(get_handle()));
				m_mouseLastPos = pt;
			}
			break;
		}
	}
	return 0;
}

LRESULT CScrollbar::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 1000 )
	{
		BOOL bLButton = ::GetKeyState(VK_LBUTTON) & SHIFTED;

		if ( !bLButton )
		{
			m_iRect = 0;
			StopTimer(1000);
			Update(FALSE);
			return 0;
		}

		POINT pt;
		RECT r;

		::GetCursorPos(&pt);
		::SetRect(&r, m_mouseLastPos.x - 2, m_mouseLastPos.y - 2, m_mouseLastPos.x + 2, m_mouseLastPos.y + 2);
		ClientToScreen(&r);
		if ( !(::PtInRect(&r, pt)) )
		{
			m_iRect = 0;
			StopTimer(1000);
			Update(FALSE);
			return 0;
		}
		switch ( m_direction )
		{
		case TDirectionHorizontal:
			switch ( m_iRect )
			{
			case 5:
				if ( m_DownLeftPageArea.right <= m_DownLeftPageArea.left )
				{
					StopTimer(1000);
					m_iRect = 0;
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_LEFT,0), (LPARAM)(get_handle()));
				}
				else
				{
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_LINELEFT,0), (LPARAM)(get_handle()));
					StartTimer(1000, 100);
				}
				break;
			case 4:
				if ( m_DownLeftPageArea.right <= m_DownLeftPageArea.left )
				{
					StopTimer(1000);
					m_iRect = 0;
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_LEFT,0), (LPARAM)(get_handle()));
				}
				else
				{
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_PAGELEFT,0), (LPARAM)(get_handle()));
					StartTimer(1000, 500);
				}
				break;
			case 2:
				if ( m_UpRightPageArea.right <= m_UpRightPageArea.left )
				{
					StopTimer(1000);
					m_iRect = 0;
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_RIGHT,0), (LPARAM)(get_handle()));
				}
				else
				{
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_PAGERIGHT,0), (LPARAM)(get_handle()));
					StartTimer(1000, 500);
				}
				break;
			case 1:
				if ( m_UpRightPageArea.right <= m_UpRightPageArea.left )
				{
					StopTimer(1000);
					m_iRect = 0;
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_RIGHT,0), (LPARAM)(get_handle()));
				}
				else
				{
					_SendMessage(WM_HSCROLL, MAKEWPARAM(SB_LINERIGHT,0), (LPARAM)(get_handle()));
					StartTimer(1000, 100);
				}
				break;
			default:
				StopTimer(1000);
				m_iRect = 0;
				break;
			}
			break;
		case TDirectionVertical:
			switch ( m_iRect )
			{
			case 5:
				if ( m_DownLeftPageArea.bottom <= m_DownLeftPageArea.top )
				{
					StopTimer(1000);
					m_iRect = 0;
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_BOTTOM,0), (LPARAM)(get_handle()));
				}
				else
				{
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN,0), (LPARAM)(get_handle()));
					StartTimer(1000, 100);
				}
				break;
			case 4:
				if ( m_DownLeftPageArea.bottom <= m_DownLeftPageArea.top )
				{
					StopTimer(1000);
					m_iRect = 0;
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_BOTTOM,0), (LPARAM)(get_handle()));
				}
				else
				{
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_PAGEDOWN,0), (LPARAM)(get_handle()));
					StartTimer(1000, 500);
				}
				break;
			case 2:
				if ( m_UpRightPageArea.bottom <= m_UpRightPageArea.top )
				{
					StopTimer(1000);
					m_iRect = 0;
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_TOP,0), (LPARAM)(get_handle()));
				}
				else
				{
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_PAGEUP,0), (LPARAM)(get_handle()));
					StartTimer(1000, 500);
				}
				break;
			case 1:
				if ( m_UpRightPageArea.bottom <= m_UpRightPageArea.top )
				{
					StopTimer(1000);
					m_iRect = 0;
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_TOP,0), (LPARAM)(get_handle()));
				}
				else
				{
					_SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP,0), (LPARAM)(get_handle()));
					StartTimer(1000, 100);
				}
				break;
			default:
				StopTimer(1000);
				m_iRect = 0;
				break;
			}
			break;
		}
	}
	return 0;
}

LRESULT CScrollbar::OnSize(WPARAM wParam, LPARAM lParam)
{
	Update(TRUE);
	return 0;
}

LRESULT CScrollbar::OnEnableArrows(WPARAM wParam, LPARAM lParam)
{
	LRESULT result = TRUE;

	switch ( m_direction )
	{
	case TDirectionHorizontal:
		switch ( wParam )
		{
		case ESB_DISABLE_BOTH:
		case ESB_DISABLE_LEFT:
		case ESB_DISABLE_RIGHT:
		case ESB_ENABLE_BOTH:
			m_EnableArrows = Castword(wParam);
			Update(FALSE);
			break;
		default:
			result = FALSE;
			break;
		}
		break;
	case TDirectionVertical:
		switch ( wParam )
		{
		case ESB_DISABLE_BOTH:
		case ESB_DISABLE_DOWN:
		case ESB_DISABLE_UP:
		case ESB_ENABLE_BOTH:
			m_EnableArrows = Castword(wParam);
			Update(FALSE);
			break;
		default:
			result = FALSE;
			break;
		}
	default:
		result = FALSE;
		break;
	}
	return result;
}

LRESULT CScrollbar::OnGetPos(WPARAM wParam, LPARAM lParam)
{
	return m_Position;
}

LRESULT CScrollbar::OnGetRange(WPARAM wParam, LPARAM lParam)
{
	DerefAnyPtr(int, wParam) = m_Minimum;
	DerefAnyPtr(int, lParam) = m_Maximum;
	return 0;
}

LRESULT CScrollbar::OnGetScrollBarInfo(WPARAM wParam, LPARAM lParam)
{
	LPSCROLLBARINFO sbi = CastAnyPtr(SCROLLBARINFO, lParam);

	if ( sbi->cbSize != sizeof(SCROLLBARINFO) )
	{
		::SetLastError(E_INVALIDARG);
		return FALSE;
	}

	RECT rW;

	GetWindowRect(&rW);
	::CopyRect(&(sbi->rcScrollBar), &rW);

	for ( int ix = 0; ix < 6; ++ix )
		sbi->rgstate[ix] = 0;
	switch ( m_direction )
	{
	case TDirectionHorizontal:
		sbi->dxyLineButton = rW.bottom - rW.top;
		sbi->xyThumbTop = m_Thumb.left;
		sbi->xyThumbBottom = m_Thumb.right;
		if ( m_UpRightPageArea.right <= m_UpRightPageArea.left )
			sbi->rgstate[2] = STATE_SYSTEM_INVISIBLE;
		if ( m_DownLeftPageArea.right <= m_DownLeftPageArea.left )
			sbi->rgstate[4] = STATE_SYSTEM_INVISIBLE;
		if ( (m_iRect != 0) && (m_iRect != 3) )
			sbi->rgstate[m_iRect] |= STATE_SYSTEM_PRESSED;
		break;
	case TDirectionVertical:
		sbi->dxyLineButton = rW.right - rW.left;
		sbi->xyThumbTop = m_Thumb.top;
		sbi->xyThumbBottom = m_Thumb.bottom;
		if ( m_UpRightPageArea.bottom <= m_UpRightPageArea.top )
			sbi->rgstate[2] = STATE_SYSTEM_INVISIBLE;
		if ( m_DownLeftPageArea.bottom <= m_DownLeftPageArea.top )
			sbi->rgstate[4] = STATE_SYSTEM_INVISIBLE;
		if ( (m_iRect != 0) && (m_iRect != 3) )
			sbi->rgstate[m_iRect] |= STATE_SYSTEM_PRESSED;
		break;
	default:
		::SetLastError(E_INVALIDARG);
		return FALSE;
	}
	if ( !(is_Visible()) )
		sbi->rgstate[0] = STATE_SYSTEM_INVISIBLE;

	::SetLastError(0);
	return TRUE;
}

LRESULT CScrollbar::OnGetScrollInfo(WPARAM wParam, LPARAM lParam)
{
	LPSCROLLINFO si = CastAnyPtr(SCROLLINFO, lParam);

	if ( (si->cbSize != sizeof(SCROLLINFO)) || ((si->fMask & SIF_ALL) == 0) )
		return FALSE;
	if ( si->fMask & SIF_RANGE )
	{
		si->nMax = m_Maximum;
		si->nMin = m_Minimum;
	}
	if ( si->fMask & SIF_PAGE )
		si->nPage = m_Page;
	if ( si->fMask & SIF_POS )
		si->nPos = m_Position;
	if ( si->fMask & SIF_TRACKPOS )
		si->nTrackPos = m_TrackPos;
	return TRUE;
}

LRESULT CScrollbar::OnSetPos(WPARAM wParam, LPARAM lParam)
{
	int iNewPos = Castsdword(wParam);

	if ( iNewPos < m_Minimum )
		iNewPos = m_Minimum;
	if ( iNewPos > m_Maximum )
		iNewPos = m_Maximum;
	m_Position = iNewPos;
	if ( lParam )
		Update(FALSE);
	return m_Position;
}

LRESULT CScrollbar::OnSetRange(WPARAM wParam, LPARAM lParam)
{
	m_Minimum = Castsdword(wParam);
	m_Maximum = Castsdword(lParam);

	if ( m_Maximum < m_Minimum )
		m_Maximum = m_Minimum;
	if ( (m_Maximum - m_Minimum) > MAXLONG )
		m_Maximum = m_Minimum + MAXLONG;
	if ( m_Position < m_Minimum )
		m_Position = m_Minimum;
	if ( m_Position > m_Maximum )
		m_Position = m_Maximum;
	if ( m_Maximum == m_Minimum )
	{
		if ( is_Visible() )
			_set_Visible(FALSE);
	}
	else if ( !(is_Visible()) )
		_set_Visible(TRUE);
	return m_Position;
}

LRESULT CScrollbar::OnSetRangeRedraw(WPARAM wParam, LPARAM lParam)
{
	LRESULT result = OnSetRange(wParam, lParam);

	if ( is_Visible() )
		Update(FALSE);
	return result;
}

LRESULT CScrollbar::OnSetScrollInfo(WPARAM wParam, LPARAM lParam)
{
	LPSCROLLINFO si = CastAnyPtr(SCROLLINFO, lParam);

	if ( (si->cbSize != sizeof(SCROLLINFO)) || ((si->fMask & (SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE)) == 0) )
		return FALSE;
	if ( si->fMask & SIF_RANGE )
	{
		m_Maximum = si->nMax;
		m_Minimum = si->nMin;
		if ( m_Maximum < m_Minimum )
			m_Maximum = m_Minimum;
		if ( (Cast(UINT, m_Maximum) - m_Minimum) > MAXLONG )
			m_Maximum = m_Minimum + MAXLONG;
		if ( m_Position < m_Minimum )
			m_Position = m_Minimum;
		if ( m_Position > m_Maximum )
			m_Position = m_Maximum;
		if ( m_Maximum == m_Minimum )
		{
			if ( si->fMask & SIF_DISABLENOSCROLL )
			{
				if ( is_Enabled() )
					set_Enabled(FALSE);
			}
			else if ( is_Visible() )
				_set_Visible(FALSE);
		}
		else if ( !(is_Visible()) )
			_set_Visible(TRUE);
		else if ( !(is_Enabled()) )
			set_Enabled(TRUE);
	}
	if ( si->fMask & SIF_PAGE )
	{
		m_Page = si->nPage;
		if ( m_Page > Cast(UINT, (Cast(UINT, m_Maximum) - m_Minimum)) )
			m_Page = m_Maximum - m_Minimum;
	}
	if ( si->fMask & SIF_POS )
	{
		m_Position = si->nPos;
		if ( m_Position < m_Minimum )
			m_Position = m_Minimum;
		if ( m_Position > m_Maximum )
			m_Position = m_Maximum;
	}
	if ( wParam )
		Update(FALSE);
	return m_Position;
}

