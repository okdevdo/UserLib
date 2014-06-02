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
#pragma once

#include "afxrsrc.h"
#include "DataVector.h"

struct AFX_MSGMAP_ENTRY;

struct AFX_MSGMAP
{
	AFX_MSGMAP* baseMap;
	AFX_MSGMAP_ENTRY* entries;
};

#define DECLARE_MESSAGE_MAP() \
private: \
    static AFX_MSGMAP_ENTRY _message_entries[]; \
protected: \
    static AFX_MSGMAP _messagemap; \
	virtual AFX_MSGMAP* GetMessageMap();

#define BEGIN_MESSAGE_MAP(beforeClass, currentClass) \
	AFX_MSGMAP* currentClass::GetMessageMap() { return &_messagemap; } \
	AFX_MSGMAP currentClass::_messagemap = { &beforeClass::_messagemap, &currentClass::_message_entries[0] }; \
	AFX_MSGMAP_ENTRY currentClass::_message_entries[] = {

#define END_MESSAGE_MAP() \
        { 0, 0, 0, 0, NULL } \
    };

#define WM_MESSAGE_TYPE 1
#define WM_COMMAND_TYPE 2
#define WM_NOTIFY_TYPE 3

#define ON_WM_ACTIVATE() { WM_MESSAGE_TYPE, WM_ACTIVATE, 0, 0, (AFX_MESSAGEFUNC)(&OnActivate) },
#define ON_WM_ACTIVATEAPP() { WM_MESSAGE_TYPE, WM_ACTIVATEAPP, 0, 0, (AFX_MESSAGEFUNC)(&OnActivateApp) },
#define ON_WM_CHILDACTIVATE() { WM_MESSAGE_TYPE, WM_CHILDACTIVATE, 0, 0, (AFX_MESSAGEFUNC)(&OnChildActivate) },
#define ON_WM_MOUSEACTIVATE() { WM_MESSAGE_TYPE, WM_MOUSEACTIVATE, 0, 0, (AFX_MESSAGEFUNC)(&OnMouseActivate) },
#define ON_WM_CREATE() { WM_MESSAGE_TYPE, WM_CREATE, 0, 0, (AFX_MESSAGEFUNC)(&OnCreate) },
#define ON_WM_DESTROY() { WM_MESSAGE_TYPE, WM_DESTROY, 0, 0, (AFX_MESSAGEFUNC)(&OnDestroy) },
#define ON_WM_ENTERSIZEMOVE() { WM_MESSAGE_TYPE, WM_ENTERSIZEMOVE, 0, 0, (AFX_MESSAGEFUNC)(&OnEnterSizeMove) },
#define ON_WM_EXITSIZEMOVE() { WM_MESSAGE_TYPE, WM_EXITSIZEMOVE, 0, 0, (AFX_MESSAGEFUNC)(&OnExitSizeMove) },
#define ON_WM_SIZE() { WM_MESSAGE_TYPE, WM_SIZE, 0, 0, (AFX_MESSAGEFUNC)(&OnSize) },
#define ON_WM_SIZING() { WM_MESSAGE_TYPE, WM_SIZING, 0, 0, (AFX_MESSAGEFUNC)(&OnSizing) },
#define ON_WM_MOVE() { WM_MESSAGE_TYPE, WM_MOVE, 0, 0, (AFX_MESSAGEFUNC)(&OnMove) },
#define ON_WM_MOVING() { WM_MESSAGE_TYPE, WM_MOVING, 0, 0, (AFX_MESSAGEFUNC)(&OnMoving) },
#define ON_WM_WINDOWPOSCHANGING() { WM_MESSAGE_TYPE, WM_WINDOWPOSCHANGING, 0, 0, (AFX_MESSAGEFUNC)(&OnWindowPosChanging) },
#define ON_WM_WINDOWPOSCHANGED() { WM_MESSAGE_TYPE, WM_WINDOWPOSCHANGED, 0, 0, (AFX_MESSAGEFUNC)(&OnWindowPosChanged) },
#define ON_WM_PAINT() { WM_MESSAGE_TYPE, WM_PAINT, 0, 0, (AFX_MESSAGEFUNC)(&OnPaint) },
#define ON_WM_SETFOCUS() { WM_MESSAGE_TYPE, WM_SETFOCUS, 0, 0, (AFX_MESSAGEFUNC)(&OnSetFocus) },
#define ON_WM_KILLFOCUS() { WM_MESSAGE_TYPE, WM_KILLFOCUS, 0, 0, (AFX_MESSAGEFUNC)(&OnKillFocus) },
#define ON_WM_TIMER() { WM_MESSAGE_TYPE, WM_TIMER, 0, 0, (AFX_MESSAGEFUNC)(&OnTimer) },
#define ON_WM_NCHITTEST() { WM_MESSAGE_TYPE, WM_NCHITTEST, 0, 0, (AFX_MESSAGEFUNC)(&OnNcHitTest) },
#define ON_WM_NCCALCSIZE() { WM_MESSAGE_TYPE, WM_NCCALCSIZE, 0, 0, (AFX_MESSAGEFUNC)(&OnNcCalcSize) },
#define ON_WM_NCCREATE() { WM_MESSAGE_TYPE, WM_NCCREATE, 0, 0, (AFX_MESSAGEFUNC)(&OnNcCreate) },
#define ON_WM_NCDESTROY() { WM_MESSAGE_TYPE, WM_NCDESTROY, 0, 0, (AFX_MESSAGEFUNC)(&OnNcDestroy) },
#define ON_WM_NCACTIVATE() { WM_MESSAGE_TYPE, WM_NCACTIVATE, 0, 0, (AFX_MESSAGEFUNC)(&OnNcActivate) },
#define ON_WM_NCPAINT() { WM_MESSAGE_TYPE, WM_NCPAINT, 0, 0, (AFX_MESSAGEFUNC)(&OnNcPaint) },
#define ON_WM_HSCROLL() { WM_MESSAGE_TYPE, WM_HSCROLL, 0, 0, (AFX_MESSAGEFUNC)(&OnHScroll) },
#define ON_WM_VSCROLL() { WM_MESSAGE_TYPE, WM_VSCROLL, 0, 0, (AFX_MESSAGEFUNC)(&OnVScroll) },
#define ON_WM_MOUSEWHEEL() { WM_MESSAGE_TYPE, WM_MOUSEWHEEL, 0, 0, (AFX_MESSAGEFUNC)(&OnMouseWheel) },
#define ON_WM_MOUSEMOVE() { WM_MESSAGE_TYPE, WM_MOUSEMOVE, 0, 0, (AFX_MESSAGEFUNC)(&OnMouseMove) },
#define ON_WM_MOUSELEAVE() { WM_MESSAGE_TYPE, WM_MOUSELEAVE, 0, 0, (AFX_MESSAGEFUNC)(&OnMouseLeave) },
#define ON_WM_MOUSEHOVER() { WM_MESSAGE_TYPE, WM_MOUSEHOVER, 0, 0, (AFX_MESSAGEFUNC)(&OnMouseHover) },
#define ON_WM_LBUTTONDOWN() { WM_MESSAGE_TYPE, WM_LBUTTONDOWN, 0, 0, (AFX_MESSAGEFUNC)(&OnLButtonDown) },
#define ON_WM_LBUTTONUP() { WM_MESSAGE_TYPE, WM_LBUTTONUP, 0, 0, (AFX_MESSAGEFUNC)(&OnLButtonUp) },
#define ON_WM_LBUTTONDBLCLK() { WM_MESSAGE_TYPE, WM_LBUTTONDBLCLK, 0, 0, (AFX_MESSAGEFUNC)(&OnLButtonDblClk) },
#define ON_WM_RBUTTONDOWN() { WM_MESSAGE_TYPE, WM_RBUTTONDOWN, 0, 0, (AFX_MESSAGEFUNC)(&OnRButtonDown) },
#define ON_WM_RBUTTONUP() { WM_MESSAGE_TYPE, WM_RBUTTONUP, 0, 0, (AFX_MESSAGEFUNC)(&OnRButtonUp) },
#define ON_WM_RBUTTONDBLCLK() { WM_MESSAGE_TYPE, WM_RBUTTONDBLCLK, 0, 0, (AFX_MESSAGEFUNC)(&OnRButtonDblClk) },
#define ON_WM_MBUTTONDOWN() { WM_MESSAGE_TYPE, WM_MBUTTONDOWN, 0, 0, (AFX_MESSAGEFUNC)(&OnMButtonDown) },
#define ON_WM_MBUTTONUP() { WM_MESSAGE_TYPE, WM_MBUTTONUP, 0, 0, (AFX_MESSAGEFUNC)(&OnMButtonUp) },
#define ON_WM_MBUTTONDBLCLK() { WM_MESSAGE_TYPE, WM_MBUTTONDBLCLK, 0, 0, (AFX_MESSAGEFUNC)(&OnMButtonDblClk) },
#define ON_WM_NCMOUSEMOVE() { WM_MESSAGE_TYPE, WM_NCMOUSEMOVE, 0, 0, (AFX_MESSAGEFUNC)(&OnNcMouseMove) },
#define ON_WM_NCMOUSELEAVE() { WM_MESSAGE_TYPE, WM_NCMOUSELEAVE, 0, 0, (AFX_MESSAGEFUNC)(&OnNcMouseLeave) },
#define ON_WM_NCMOUSEHOVER() { WM_MESSAGE_TYPE, WM_NCMOUSEHOVER, 0, 0, (AFX_MESSAGEFUNC)(&OnNcMouseHover) },
#define ON_WM_NCLBUTTONDOWN() { WM_MESSAGE_TYPE, WM_NCLBUTTONDOWN, 0, 0, (AFX_MESSAGEFUNC)(&OnNcLButtonDown) },
#define ON_WM_NCLBUTTONUP() { WM_MESSAGE_TYPE, WM_NCLBUTTONUP, 0, 0, (AFX_MESSAGEFUNC)(&OnNcLButtonUp) },
#define ON_WM_NCLBUTTONDBLCLK() { WM_MESSAGE_TYPE, WM_NCLBUTTONDBLCLK, 0, 0, (AFX_MESSAGEFUNC)(&OnNcLButtonDblClk) },
#define ON_WM_NCRBUTTONDOWN() { WM_MESSAGE_TYPE, WM_NCRBUTTONDOWN, 0, 0, (AFX_MESSAGEFUNC)(&OnNcRButtonDown) },
#define ON_WM_NCRBUTTONUP() { WM_MESSAGE_TYPE, WM_NCRBUTTONUP, 0, 0, (AFX_MESSAGEFUNC)(&OnNcRButtonUp) },
#define ON_WM_NCRBUTTONDBLCLK() { WM_MESSAGE_TYPE, WM_NCRBUTTONDBLCLK, 0, 0, (AFX_MESSAGEFUNC)(&OnNcRButtonDblClk) },
#define ON_WM_NCMBUTTONDOWN() { WM_MESSAGE_TYPE, WM_NCMBUTTONDOWN, 0, 0, (AFX_MESSAGEFUNC)(&OnNcMButtonDown) },
#define ON_WM_NCMBUTTONUP() { WM_MESSAGE_TYPE, WM_NCMBUTTONUP, 0, 0, (AFX_MESSAGEFUNC)(&OnNcMButtonUp) },
#define ON_WM_NCMBUTTONDBLCLK() { WM_MESSAGE_TYPE, WM_NCMBUTTONDBLCLK, 0, 0, (AFX_MESSAGEFUNC)(&OnNcMButtonDblClk) },
#define ON_WM_KEYDOWN() { WM_MESSAGE_TYPE, WM_KEYDOWN, 0, 0, (AFX_MESSAGEFUNC)(&OnKeyDown) },
#define ON_WM_KEYUP() { WM_MESSAGE_TYPE, WM_KEYUP, 0, 0, (AFX_MESSAGEFUNC)(&OnKeyUp) },
#define ON_WM_CHAR() { WM_MESSAGE_TYPE, WM_CHAR, 0, 0, (AFX_MESSAGEFUNC)(&OnChar) },
#define ON_WM_INITMENU() { WM_MESSAGE_TYPE, WM_INITMENU, 0, 0, (AFX_MESSAGEFUNC)(&OnInitMenu) },
#define ON_WM_INITMENUPOPUP() { WM_MESSAGE_TYPE, WM_INITMENUPOPUP, 0, 0, (AFX_MESSAGEFUNC)(&OnInitMenuPopup) },
#define ON_WM_SYSCOMMAND() { WM_MESSAGE_TYPE, WM_SYSCOMMAND, 0, 0, (AFX_MESSAGEFUNC)(&OnSysCommand) },
#define ON_WM_SYSKEYDOWN() { WM_MESSAGE_TYPE, WM_SYSKEYDOWN, 0, 0, (AFX_MESSAGEFUNC)(&OnSysKeyDown) },
#define ON_WM_SYSKEYUP() { WM_MESSAGE_TYPE, WM_SYSKEYUP, 0, 0, (AFX_MESSAGEFUNC)(&OnSysKeyUp) },
#define ON_WM_ERASEBKGND() { WM_MESSAGE_TYPE, WM_ERASEBKGND, 0, 0, (AFX_MESSAGEFUNC)(&OnEraseBackGround) },
#define ON_WM_CONTEXTMENU() { WM_MESSAGE_TYPE, WM_CONTEXTMENU, 0, 0, (AFX_MESSAGEFUNC)(&OnContextMenu) },
#define ON_WM_MENUCOMMAND() { WM_MESSAGE_TYPE, WM_MENUCOMMAND, 0, 0, (AFX_MESSAGEFUNC)(&OnMenuCommand) },
#define ON_WM_CAPTURECHANGED() { WM_MESSAGE_TYPE, WM_CAPTURECHANGED, 0, 0, (AFX_MESSAGEFUNC)(&OnCaptureChanged) },
#define ON_WM_SETCURSOR() { WM_MESSAGE_TYPE, WM_SETCURSOR, 0, 0, (AFX_MESSAGEFUNC)(&OnSetCursor) },

#define ON_WM_MESSAGE(id, fxn) { WM_MESSAGE_TYPE, (id), 0, 0, (AFX_MESSAGEFUNC)(&fxn) },

#define ON_WM_COMMMAND(code, fxn)  { WM_COMMAND_TYPE, WM_COMMAND, (code), (code), (AFX_MESSAGEFUNC)(&fxn) },
#define ON_WM_COMMMAND_RANGE(code1, code2, fxn)  { WM_COMMAND_TYPE, WM_COMMAND, (code1), (code2), (AFX_MESSAGEFUNC)(&fxn) },

typedef struct tagNotifyMessage
{
	NMHDR hdr;
	LPARAM param;
} NotifyMessage;

#define ON_WM_NOTIFY(code, fxn)  { WM_NOTIFY_TYPE, WM_NOTIFY, (code), (code), (AFX_MESSAGEFUNC)(&fxn) },

class CControl;
typedef struct tagNotifyMessageEditorFuncKey
{
	CControl* pEditor;
	WPARAM funcKey;

	tagNotifyMessageEditorFuncKey(CControl* _pEditor, WPARAM _funcKey):
	     pEditor(_pEditor), funcKey(_funcKey) {}
} TNotifyMessageEditorFuncKey;

typedef struct tagNotifyMessageEditorContextMenu
{
	CControl* pEditor;
	POINT pt;

	tagNotifyMessageEditorContextMenu(CControl* _pEditor, POINT _pt):
	     pEditor(_pEditor), pt(_pt) {}
} TNotifyMessageEditorContextMenu;

class CToolbar;
class CStatusbar;
class CMenubar;
class CWin: public CCppObject
{
public:
	enum TWindowType
	{
		TAnyWindow,
		TFrameWindow,
		TMDIWindow,
		TMDIChildWindow,
		TDialogWindow,
		TPopupWindow,
		TDockWindow,
		TControlWindow
	};

public:
	CWin(TWindowType wndwtype = TAnyWindow, LPCTSTR name = NULL);
	CWin(TWindowType wndwtype, ConstRef(CStringBuffer) name);
	virtual ~CWin();

	__inline TWindowType get_windowtype() { return m_wndwtype; }
	__inline ConstRef(CStringBuffer) get_Name() { return m_name; }
	__inline ConstRef(CStringBuffer) get_name() { return m_name; }
	__inline void set_name(ConstRef(CStringBuffer) name) { m_name = name; }

	__inline BOOL is_CreateVisible() { return m_createvisible; }
	__inline void set_CreateVisible(BOOL visible) { m_createvisible = visible; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);

	BOOL RegisterClass(WNDCLASSEX& wcex);
	BOOL Create(HWND parent = NULL, UINT childId = 0);

	__inline HWND get_handle() { return m_hwnd; }
	__inline void set_handle(HWND hwnd) { m_hwnd = hwnd; }

	__inline bool is_created() const { return (m_hwnd != NULL); }

	CStringBuffer get_windowtext();
	BOOL set_windowtext(ConstRef(CStringBuffer) text);

	__inline HWND get_parenthandle() { return ::GetParent(m_hwnd); }
	__inline HWND set_parenthandle(HWND parenthwnd) { return ::SetParent(m_hwnd, parenthwnd); }
	CWin* get_parent();
	CWin* set_parent(CWin* pParent);
	CWin* get_frame();
	CWin* get_mdiChild();
	CWin* get_childbyID(UINT commandcode);
	__inline UINT get_childID() { return ::GetDlgCtrlID(m_hwnd); }
	__inline UINT set_childID(UINT id) { return Castdword(SetWindowLong(GWLP_ID, id)); } 
	CWin* get_firstchild();
	CWin* get_siblingByCmd(UINT uCmd);
	CWin* get_childFromPt(POINT pt);

	bool set_focus();
	static CWin* get_focus();
	bool has_focus();

	CStringBuffer get_classname();
	HICON get_classicon();
	HICON get_classiconsm();

	__inline BOOL is_AllVisible() { return ::IsWindowVisible(m_hwnd); }
	__inline BOOL is_Visible() { return ::GetWindowLongPtr(m_hwnd, GWL_STYLE) & WS_VISIBLE; }
	BOOL set_Visible(BOOL visible, BOOL updateChildren = FALSE);

	__inline BOOL is_Enabled() { return ::IsWindowEnabled(m_hwnd); }
	__inline void set_Enabled(BOOL bEnable) { ::EnableWindow(m_hwnd, bEnable); }

	__inline BOOL is_Iconic() { return ::IsIconic(m_hwnd); }

	__inline BOOL has_Capture() { return (::GetCapture() == m_hwnd); }
	void set_Capture(BOOL bEnable);

	__inline BOOL get_TrackMouse() { return m_trackMouse; }
	__inline void set_TrackMouse(BOOL b) { m_trackMouse = b; }

	BOOL StartTrackMouse();
	BOOL StopTrackMouse();

	Gdiplus::Font* get_Font(CStringLiteral _suffix, CStringLiteral _wndwclass);
	Gdiplus::Brush* get_Brush(CStringLiteral _suffix, CStringLiteral _wndwclass, ConstRef(Gdiplus::Color) _default);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	__inline LONG_PTR GetWindowLong(int nIndex) { return ::GetWindowLongPtr(m_hwnd, nIndex); }
	__inline LONG_PTR SetWindowLong(int nIndex, LONG_PTR value) { return ::SetWindowLongPtr(m_hwnd, nIndex, value); }
	__inline BOOL ScreenToClient(LPPOINT pt) { return ::ScreenToClient(m_hwnd, pt); }
	__inline BOOL ScreenToClient(LPRECT pRect) { ::SetLastError(0); int res = ::MapWindowPoints(NULL, m_hwnd, (LPPOINT)pRect, 2); if ( ((res == 0) && (::GetLastError() == 0)) || (res != 0) ) return TRUE; return FALSE; }
	__inline BOOL ClientToScreen(LPPOINT pt) { return ::ClientToScreen(m_hwnd, pt); }
	__inline BOOL ClientToScreen(LPRECT pRect) { ::SetLastError(0); int res = ::MapWindowPoints(m_hwnd, NULL, (LPPOINT)pRect, 2); if ( ((res == 0) && (::GetLastError() == 0)) || (res != 0) ) return TRUE; return FALSE; }
	__inline BOOL MoveWindow(LPRECT rect, BOOL repaint = FALSE) { return ::MoveWindow(m_hwnd, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, repaint); }
	__inline BOOL SetWindowPos(LPRECT rect, UINT flags) { if ( rect ) return ::SetWindowPos(m_hwnd, NULL, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, flags | SWP_NOZORDER); else return ::SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, flags | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); }
	__inline BOOL SetWindowPos(HWND hInsertAfter, LPRECT rect, UINT flags) { if ( rect ) return ::SetWindowPos(m_hwnd, hInsertAfter, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, flags); else return ::SetWindowPos(m_hwnd, hInsertAfter, 0, 0, 0, 0, flags | SWP_NOMOVE | SWP_NOSIZE); }
	__inline BOOL SetWindowPos(CWin* pWinInsertAfter, LPRECT rect, UINT flags) { if ( rect ) return ::SetWindowPos(m_hwnd, (pWinInsertAfter)?(pWinInsertAfter->get_handle()):NULL, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, flags); else return ::SetWindowPos(m_hwnd, (pWinInsertAfter)?(pWinInsertAfter->get_handle()):NULL, 0, 0, 0, 0, flags | SWP_NOMOVE | SWP_NOSIZE); }
	__inline BOOL UpdateWindow() { return ::UpdateWindow(m_hwnd); }
	__inline BOOL InvalidateRect(const LPRECT lpRect, BOOL bErase) { return ::InvalidateRect(m_hwnd, lpRect, bErase); }
	__inline BOOL DestroyWindow() { return ::DestroyWindow(m_hwnd); }
	__inline BOOL GetClientRect(LPRECT lpRect) { return ::GetClientRect(m_hwnd, lpRect); }
	__inline BOOL GetWindowRect(LPRECT lpRect) { return ::GetWindowRect(m_hwnd, lpRect); }
	__inline BOOL GetParentRect(LPRECT lpRect) { if ( !::GetWindowRect(m_hwnd, lpRect) ) return FALSE; CWin* pParent = get_parent(); if ( PtrCheck(pParent) || (!(pParent->ScreenToClient(lpRect))) ) return FALSE; return TRUE; }
	__inline LRESULT SendMessage(UINT message, WPARAM wParam, LPARAM lParam) { return ::SendMessage(m_hwnd, message, wParam, lParam); }
	__inline BOOL PostMessage(UINT message, WPARAM wParam, LPARAM lParam) { return ::PostMessage(m_hwnd, message, wParam, lParam); }
	__inline BOOL StartTimer(UINT_PTR nIDEvent, UINT nTimeElapsed) { return ::SetTimer(m_hwnd, nIDEvent, nTimeElapsed, NULL) == nIDEvent; }
	__inline BOOL StopTimer(UINT_PTR nIDEvent) { return ::KillTimer(m_hwnd, nIDEvent); }

	LRESULT SendNotifyMessage(UINT code, LPARAM param = NULL);

	LRESULT OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommandMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotifyMessage(WPARAM wParam, LPARAM lParam);
	LRESULT DefaultWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT OnEraseBackGround(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseActivate(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	AFX_MSGMAP_ENTRY* _hasNotifyMessage(UINT code);
	AFX_MSGMAP_ENTRY* _hasCommandMessage(UINT code);
	AFX_MSGMAP_ENTRY* _hasWindowMessage(UINT code);
	void add_BrushKey(Ref(CDataVectorT<CStringBuffer>) _keys, ConstRef(CStringBuffer) _key);
	void add_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys, CStringLiteral _suffix, CStringLiteral _wndClass);
	void add_FontKey(Ref(CDataVectorT<CStringBuffer>) _keys, ConstRef(CStringBuffer) _key);
	void add_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys, CStringLiteral _suffix, CStringLiteral _wndClass);

	HWND m_hwnd;
	TWindowType m_wndwtype;
	CStringBuffer m_name;
	BOOL m_createvisible;
	BOOL m_trackMouse;
};

typedef LRESULT (CWin::* AFX_MESSAGEFUNC)(WPARAM wParam, LPARAM lParam);

struct AFX_MSGMAP_ENTRY
{
    WORD msg_Type;
    WORD msg_ID;
	WORD msg_Code;
	WORD msg_CodeEnd;
	AFX_MESSAGEFUNC msg_Func;
};

class CPopup: public CWin
{
public:
	CPopup(LPCTSTR name = NULL);
	CPopup(ConstRef(CStringBuffer) name);
	virtual ~CPopup();

	__inline BOOL has_ClientBorder() { return m_clienthasborder; };
	__inline void set_ClientBorder(BOOL b) { m_clienthasborder = b; }

	__inline BOOL get_ClientVScrollEnabled() { return m_clientvScrollEnabled; }
	__inline void set_ClientVScrollEnabled(BOOL b) { m_clientvScrollEnabled = b; }

	__inline BOOL get_ClientHScrollEnabled() { return m_clienthScrollEnabled; }
	__inline void set_ClientHScrollEnabled(BOOL b) { m_clienthScrollEnabled = b; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	__inline CControl* get_Client() { return m_panel; }

	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_clienthasborder;
	BOOL m_clientvScrollEnabled;
	BOOL m_clienthScrollEnabled;
	CControl* m_panel;
};

class CDockInfo;
class CFrameWnd: public CWin
{
public:
	CFrameWnd(LPCTSTR name = NULL);
	CFrameWnd(ConstRef(CStringBuffer) name);
	virtual ~CFrameWnd();

	__inline BOOL has_ClientBorder() { return m_clienthasborder; };
	__inline void set_ClientBorder(BOOL b) { m_clienthasborder = b; }

	__inline BOOL is_ClientVScrollEnabled() { return m_clientvScrollEnabled; }
	__inline void set_ClientVScrollEnabled(BOOL b) { m_clientvScrollEnabled = b; }

	__inline BOOL is_ClientHScrollEnabled() { return m_clienthScrollEnabled; }
	__inline void set_ClientHScrollEnabled(BOOL b) { m_clienthScrollEnabled = b; }

	__inline BOOL is_ToolBarEnabled() { return m_toolbarenabled; }
	void set_ToolBarEnabled(BOOL b);

	__inline BOOL is_StatusBarEnabled() { return m_statusbarenabled; }
	void set_StatusBarEnabled(BOOL b);

	__inline BOOL is_DockingEnabled() { return m_dockingenabled; }
	void set_DockingEnabled(BOOL b);

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	void DestroyChildren(void);
	void AutoSize(void);

	__inline CWin* get_focuswindow() { return m_focuswindow; }
	void set_focuswindow(CWin* win);

	__inline CControl* get_Client() { return m_panel; }
	__inline CToolbar* get_ToolBar() { return m_toolbar; }
	__inline CStatusbar* get_StatusBar() { return m_statusbar; }
	__inline CMenubar* get_MenuBar() { return m_menubar; }
	__inline CDockInfo* get_dockinfo() { return m_dockinfo; }

	__inline UINT get_cmdcode() { return m_cmdcode++; }

	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcActivate(WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(WPARAM wParam, LPARAM lParam);
	LRESULT OnActivateApp(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnSizing(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnterSizeMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnInitMenuPopup(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnterMenuLoop(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitMenuLoop(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditForwardCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnToggleToolbarCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnToggleStatusbarCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnToggleDockInfoCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotifySelectionChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotifyMenuItemHovered(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	LRESULT create_toolbar();
	LRESULT create_statusbar();
	LRESULT create_menubar();
	LRESULT create_dockinfo();
	void update_toolbar();

	BOOL m_iscreated;
	BOOL m_clienthasborder;
	BOOL m_clientvScrollEnabled;
	BOOL m_clienthScrollEnabled;
	BOOL m_toolbarenabled;
	BOOL m_statusbarenabled;
	BOOL m_dockingenabled;
	BOOL m_menuloop;
	UINT m_cmdcode;
	CControl* m_panel;
	CWin* m_focuswindow;
	CToolbar* m_toolbar;
	CStatusbar* m_statusbar;
	CMenubar* m_menubar;
	CDockInfo* m_dockinfo;
};

class CMDIChild;
class CMDIFrame: public CWin
{
public:
	CMDIFrame(LPCTSTR name = NULL);
	CMDIFrame(ConstRef(CStringBuffer) name);
	virtual ~CMDIFrame();

	__inline BOOL is_ToolBarEnabled() { return m_toolbarenabled; }
	void set_ToolBarEnabled(BOOL b);

	__inline BOOL is_StatusBarEnabled() { return m_statusbarenabled; }
	void set_StatusBarEnabled(BOOL b);

	__inline BOOL is_DockingEnabled() { return m_dockingenabled; }
	void set_DockingEnabled(BOOL b);

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	__inline HWND get_clienthandle() { return m_clientHWnd; }

	__inline CToolbar* get_ToolBar() { return m_toolbar; }
	__inline CStatusbar* get_StatusBar() { return m_statusbar; }
	__inline CMenubar* get_MenuBar() { return m_menubar; }
	__inline CDockInfo* get_dockinfo() { return m_dockinfo; }

	CWin* get_activeChild(BOOL* bMaximized = NULL);

	__inline BOOL get_menuloop() { return m_menuloop; }
	__inline BOOL get_childmaximized() { return m_childmaximized; }
	__inline UINT get_childcnt() { return m_childcnt; }

	__inline UINT get_cmdcode() { return m_cmdcode++; }

	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnChildStatus(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcActivate(WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(WPARAM wParam, LPARAM lParam);
	LRESULT OnActivateApp(WPARAM wParam, LPARAM lParam);
	LRESULT OnInitMenuPopup(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnterMenuLoop(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitMenuLoop(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditForwardCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnTileHorizCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnTileVertCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCascadeCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseAllCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnToggleToolbarCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnToggleStatusbarCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnToggleDockInfoCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotifySelectionChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotifyMenuItemHovered(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	LRESULT create_toolbar();
	LRESULT create_statusbar();
	LRESULT create_menubar();
	LRESULT create_dockinfo();

	BOOL m_iscreated;
	BOOL m_toolbarenabled;
	BOOL m_statusbarenabled;
	BOOL m_dockingenabled;
	BOOL m_menuloop;
	UINT m_cmdcode;
	CMDIChild* m_pCurrentChild;
	BOOL m_childmaximized;
	UINT m_childcnt;
	HWND m_clientHWnd;
	CToolbar* m_toolbar;
	CStatusbar* m_statusbar;
	CMenubar* m_menubar;
	CDockInfo* m_dockinfo;
};

class CMDIChild: public CWin
{
public:
	CMDIChild(LPCTSTR name = NULL);
	CMDIChild(ConstRef(CStringBuffer) name);
	virtual ~CMDIChild();

	__inline BOOL has_ClientBorder() { return m_clienthasborder; };
	__inline void set_ClientBorder(BOOL b) { m_clienthasborder = b; }

	__inline BOOL get_ClientVScrollEnabled() { return m_clientvScrollEnabled; }
	__inline void set_ClientVScrollEnabled(BOOL b) { m_clientvScrollEnabled = b; }

	__inline BOOL get_ClientHScrollEnabled() { return m_clienthScrollEnabled; }
	__inline void set_ClientHScrollEnabled(BOOL b) { m_clienthScrollEnabled = b; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);
	BOOL Create(HWND parent = NULL); // parent == CMDIFrame::get_clienthandle()

	void DestroyChildren(void);
	void AutoSize(void);

	__inline CControl* get_Client() { return m_panel; }

	__inline CWin* get_focuswindow() { return m_focuswindow; }
	void set_focuswindow(CWin* win);

	__inline UINT get_sizestatus() { return m_sizestatus; }

	LRESULT OnInitMenuPopup(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnterMenuLoop(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitMenuLoop(WPARAM wParam, LPARAM lParam);
	LRESULT OnChildActivate(WPARAM wParam, LPARAM lParam);
	LRESULT OnMDIActivate(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcActivate(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void reset_focus(void);
	BOOL RegisterClass(WNDCLASSEX& wcex);

	BOOL m_clienthasborder;
	BOOL m_clientvScrollEnabled;
	BOOL m_clienthScrollEnabled;
	BOOL m_menuloop;
	UINT m_sizestatus;
	CControl* m_panel;
	CWin* m_focuswindow;
};

class CControl: public CWin
{
public:
	enum TSizeSpecifier {
		TSizeSpecifierDefaultSize,
		TSizeSpecifierFixedSize,
		TSizeSpecifierPercentSize,
		TSizeSpecifierRelativePercentSize
	};

	enum TSizeHAlign {
		TSizeHAlignLeft,
		TSizeHAlignRight
	};

	enum TSizeVAlign {
		TSizeVAlignTop,
		TSizeVAlignBottom
	};

public:
	CControl(LPCTSTR name = NULL);
	CControl(ConstRef(CStringBuffer) name);
	virtual ~CControl();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	__inline BOOL has_border() { return m_hasborder; };
	__inline void set_border(BOOL b) { m_hasborder = b; }

	BOOL BeginUpdate();
	BOOL EndUpdate(BOOL updateRects);
	BOOL Update(BOOL updateRects);
	void UpdateChildren(BOOL updateRects);

	__inline BOOL get_UpdateRects() { return m_updateRects; }
	__inline void set_UpdateRects(BOOL updateRects) { m_updateRects = updateRects; }

	__inline BOOL get_VScrollEnabled() { return m_vScrollEnabled; }
	__inline void set_VScrollEnabled(BOOL b) { m_vScrollEnabled = b; }

	__inline BOOL get_VScrollVisible() { return m_vScrollVisible; }
	__inline void set_VScrollVisible(BOOL b) { m_vScrollVisible = b; }
	__inline HWND get_UserVScroll() { return m_hUserVScroll; }
	__inline void set_UserVScroll(HWND hwnd) { m_hUserVScroll = hwnd; }

	int get_VScrollOffset();
	void set_VScrollOffset(int scrollOffset);
	int get_VScrollWidth();

	__inline BOOL get_HScrollEnabled() { return m_hScrollEnabled; }
	__inline void set_HScrollEnabled(BOOL b) { m_hScrollEnabled = b; }

	__inline BOOL get_HScrollVisible() { return m_hScrollVisible; }
	__inline void set_HScrollVisible(BOOL b) { m_hScrollVisible = b; }
	__inline HWND get_UserHScroll() { return m_hUserHScroll; }
	__inline void set_UserHScroll(HWND hwnd) { m_hUserHScroll = hwnd; }

	int get_HScrollOffset();
	void set_HScrollOffset(int scrollOffset);
	int get_HScrollHeight();

	BOOL ShowVertScrollBar();
	BOOL ShowHorzScrollBar();

	__inline void get_MaxClientRect(LPRECT r) { ::CopyRect(r, &m_maxClientArea); }
	
	__inline TSizeHAlign get_SizeHAlign() const { return m_sizeHAlign; }
	__inline void set_SizeHAlign(TSizeHAlign align) { m_sizeHAlign = align; }

	__inline WORD get_LayoutX() const { return m_layoutX; }
	__inline void set_LayoutX(WORD layout) { m_layoutX = layout; }

	__inline TSizeSpecifier get_SizeSpecX() const { return m_sizeSpecX; }
	__inline void set_SizeSpecX(TSizeSpecifier spec) { m_sizeSpecX = spec; }

	__inline LONG get_LayoutSizeX() const { return m_layoutSizeX; }
	__inline void set_LayoutSizeX(LONG size) { m_layoutSizeX = size; }

	__inline TSizeVAlign get_SizeVAlign() const { return m_sizeVAlign; }
	__inline void set_SizeVAlign(TSizeVAlign align) { m_sizeVAlign = align; }

	__inline WORD get_LayoutY() const { return m_layoutY; }
	__inline void set_LayoutY(WORD layout) { m_layoutY = layout; }

	__inline TSizeSpecifier get_SizeSpecY() const { return m_sizeSpecY; }
	__inline void set_SizeSpecY(TSizeSpecifier spec) { m_sizeSpecY = spec; }

	__inline LONG get_LayoutSizeY() const { return m_layoutSizeY; }
	__inline void set_LayoutSizeY(LONG size) { m_layoutSizeY = size; }

	__inline RECT get_Margins() const { return m_Margins; }
	__inline void set_Margins(RECT rect) { m_Margins = rect; }

	void RenumberLayout(bool xAxis);
	int CountChildren();
	
	LRESULT OnHScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnVScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void PaintWorkspace(Gdiplus::Graphics* graphics, Gdiplus::Brush* brush);

	BOOL m_hasborder;
	UINT m_update;
	BOOL m_updateRects;
	BOOL m_updateRectsRequested;
	BOOL m_vScrollEnabled;
	BOOL m_hScrollEnabled;
	HWND m_hUserVScroll;
	HWND m_hUserHScroll;
	BOOL m_hScrollVisible;
	BOOL m_vScrollVisible;
	BOOL m_insideHScroll;
	BOOL m_insideVScroll;
	RECT m_maxClientArea;
	TSizeHAlign m_sizeHAlign;
	WORD m_layoutX;
	TSizeSpecifier m_sizeSpecX;
	LONG m_layoutSizeX;
	TSizeVAlign m_sizeVAlign;
	WORD m_layoutY;
	TSizeSpecifier m_sizeSpecY;
	LONG m_layoutSizeY;
	RECT m_Margins;
};

class CDialog : public CWin
{
public:
	class TDialogControl : public CCppObject
	{
	public:
		CStringBuffer m_classname;
		CCppObjectPtr<CControl> pControl;

		__inline ConstRef(CStringBuffer) get_Name() const { return m_classname; }
	};

	typedef CDataVectorT<TDialogControl, CStringByNameLessFunctor<TDialogControl> > TDialogControls;

public:
	CDialog(LPCTSTR name = NULL, LPCTSTR resID = NULL, CWin* pParent = NULL);
	CDialog(ConstRef(CStringBuffer) name, LPCTSTR resID = NULL, CWin* pParent = NULL);
	virtual ~CDialog();

	__inline LPCTSTR get_resID() { return m_resID; }
	__inline CWin* get_dialogOwner() { return m_pParent; }

	INT_PTR DoModal();
	void CenterWindow();

	static void AddDialogControl(CControl* pControl);
	static CControl* GetDialogControl(LPCTSTR className);
	static void RemoveDialogControls();

	DECLARE_MESSAGE_MAP()

protected:
	LPCTSTR m_resID;
	CWin* m_pParent;

	static TDialogControls m_DialogControls;
};

class CPanel : public CControl
{
public:
	CPanel(LPCTSTR name = NULL);
	CPanel(ConstRef(CStringBuffer) name);
	virtual ~CPanel();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);

	LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnVScroll(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class CToolbar: public CControl
{
public:
	CToolbar(LPCTSTR name = NULL);
	CToolbar(ConstRef(CStringBuffer) name);
	virtual ~CToolbar();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);

	__inline const CImageList& get_ImagesNormal() const { return m_imagesnormal; }
	__inline void set_ImagesNormal(const CImageList& list) { m_imagesnormal = list; }

	__inline const CImageList& get_ImagesDisabled() const { return m_imagesdisabled; }
	__inline void set_ImagesDisabled(const CImageList& list) { m_imagesdisabled = list; }

	__inline const CImageList& get_ImagesHot() const { return m_imageshot; }
	__inline void set_ImagesHot(const CImageList& list) { m_imageshot = list; }

	void AddButtonControl(UINT commandcode);
	void AutoSize();

	void set_ChildEnabled(UINT commandcode, BOOL bEnable);

	LRESULT OnNotifyButtonClicked(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	CImageList m_imagesnormal;
	CImageList m_imagesdisabled;
	CImageList m_imageshot;
	dword m_cnt;
};

class CStatusbar: public CControl
{
public:
	enum TStatusStyle
	{
		TStatusStyleUserItem,
		TStatusStyleStatusMessages,
		TStatusStyleClock,
		TStatusStyleKeyboardStatus
	};

	class TStatusInfo: public CCppObject
	{
	public:
		TStatusStyle style;
		UINT width;
		CStringBuffer defaultText;
		CControl* pControl;

	};

	class TStatusInfoLessFunctor
	{
	public:
		bool operator()(ConstPtr(TStatusInfo) elem1, ConstPtr(TStatusInfo) elem2) const
		{
			if (elem1->style < elem2->style)
				return true;
			return false;
		}
	};

	class TStatusInfoReleaseFunctor
	{
	public:
		Ref(TStatusInfoReleaseFunctor) operator()(Ptr(TStatusInfo) pData);

		Ptr(CStatusbar) pStatusbar;
		HHOOK hKeyboardProc_StatusInfo;
	};

	typedef CDataVectorT<TStatusInfo, TStatusInfoLessFunctor, CCppObjectReleaseFunctor<TStatusInfo> > TStatusInfoVector;

public:
	CStatusbar(LPCTSTR name = NULL);
	CStatusbar(ConstRef(CStringBuffer) name);
	virtual ~CStatusbar();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);

	void AutoSize();

	UINT add_item(TStatusStyle style, UINT width, LPCTSTR defaulttext = NULL, int lench = -1);
	UINT add_item(TStatusStyle style, UINT width, ConstRef(CStringBuffer) defaulttext);
	void set_text(TStatusStyle style, LPCTSTR text = NULL, int lench = -1);
	void set_text(TStatusStyle style, ConstRef(CStringBuffer) text);
	void set_text(UINT ix, LPCTSTR text = NULL, int lench = -1);
	void set_text(UINT ix, ConstRef(CStringBuffer) text);

	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	TStatusInfoVector m_statusinfo;
};

class CPopupMenu: public CPopup
{
public:
	enum TMenuType
	{
		TMenuTypeUser,
		TMenuTypeFile,
		TMenuTypeEdit,
		TMenuTypeView,
		TMenuTypeWindow
	};

public:
	CPopupMenu(LPCTSTR name = NULL);
	CPopupMenu(ConstRef(CStringBuffer) name);
	virtual ~CPopupMenu();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	void CreateFromHMENU(HMENU hmenu);
	void AutoSize(POINT pt);
	void TakeOverCapture();
	virtual void Reset();
	bool TestMnemonic(UINT vKey);

	__inline CMenubar* get_menubar() { return m_pMenubar; }
	__inline void set_menubar(CMenubar* p) { m_pMenubar = p; }

	__inline CWin* get_trackwindow() { return m_pTrackWindow; }
	__inline void set_trackwindow(CWin* p) { m_pTrackWindow = p; }

	__inline TMenuType get_menutype() { return m_menutype; }
	__inline void set_menutype(TMenuType type) { m_menutype = type; }

	CControl* get_menuItem(UINT commandCode);

	void add_menuitem(UINT commandCode, LPCTSTR text = NULL, int chlen = -1);
	void add_menuitem(UINT commandCode, ConstRef(CStringBuffer) text);
	void reset_menuitems(void);

	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotifyButtonMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnCaptureChanged(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void set_hovered(bool bHovered);
	void set_clicked(bool bClicked);

	TMenuType m_menutype;
	int m_fixeditemcnt;
	int m_itemcnt;
	CMenubar* m_pMenubar;
	CWin* m_pTrackWindow;
	BOOL m_hasFirstAutoSize;
	CControl* m_pCurrentMenuItem;
	BOOL m_hasCaptureReleaseExpected;
};

class CPopupMenuEx: public CPopupMenu
{
public:
	CPopupMenuEx(LPCTSTR name = NULL);
	CPopupMenuEx(ConstRef(CStringBuffer) name);
	virtual ~CPopupMenuEx();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	__inline const CImageList& get_ImagesNormal() const { return m_imagesnormal; }
	__inline void set_ImagesNormal(const CImageList& list) { m_imagesnormal = list; }

	__inline const CImageList& get_ImagesDisabled() const { return m_imagesdisabled; }
	__inline void set_ImagesDisabled(const CImageList& list) { m_imagesdisabled = list; }

	__inline const CImageList& get_ImagesHot() const { return m_imageshot; }
	__inline void set_ImagesHot(const CImageList& list) { m_imageshot = list; }

	void set_Image(int imageIndex, UINT commandCode);
	void set_Enabled(UINT commandCode, bool bEnable);

	void Track(POINT pt, CWin* wndw);
	virtual void Reset();

	DECLARE_MESSAGE_MAP()

protected:
	CImageList m_imagesnormal;
	CImageList m_imagesdisabled;
	CImageList m_imageshot;
};

typedef CDataVectorT<CPopupMenu, CCppObjectLessFunctor<CPopupMenu>, CCppObjectNullFunctor<CPopupMenu>> CMenuVector;

class CMenubar: public CControl
{
public:
	CMenubar(LPCTSTR name = NULL);
	CMenubar(ConstRef(CStringBuffer) name);
	virtual ~CMenubar();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	__inline const CImageList& get_ImagesNormal() const { return m_imagesnormal; }
	__inline void set_ImagesNormal(const CImageList& list) { m_imagesnormal = list; }

	__inline const CImageList& get_ImagesDisabled() const { return m_imagesdisabled; }
	__inline void set_ImagesDisabled(const CImageList& list) { m_imagesdisabled = list; }

	__inline const CImageList& get_ImagesHot() const { return m_imageshot; }
	__inline void set_ImagesHot(const CImageList& list) { m_imageshot = list; }

	void CreateFromHMENU(HMENU hmenu);
	void AutoSize();
	void TakeOverCapture();
	void Reset();
	bool TestMnemonic(UINT vKey);

	__inline CPopupMenu* get_Menu(int ix) { if ( (ix < 0) || (Cast(TListCnt,ix) >= m_menus.Count()) ) return NULL; return *(m_menus.Index(ix)); }
	int inx_Menu(CPopupMenu* pMenu);

	void set_Image(int imageIndex, UINT commandCode);
	void set_Enabled(UINT commandCode, bool bEnable);

	__inline BOOL get_MDIMenubar() { return m_isMDI; }
	void set_MDIMenubar(BOOL b, CMDIChild* pChild = NULL);

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotifyButtonMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnCaptureChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	LRESULT OpenPopupMenu(CControl* pMenuItem);
	void DrawMDIButton(Gdiplus::Graphics* pGraphics, int iButton, UINT uState);
	void set_hovered(bool bHovered);
	void set_clicked(bool bClicked);
	void _InitSysKey();

	CImageList m_imagesnormal;
	CImageList m_imagesdisabled;
	CImageList m_imageshot;
	CMenuVector m_menus;
	CPopupMenu* m_pCurrentMenu;
	CControl* m_pCurrentMenuBarItem;
	BOOL m_hasCaptureReleaseExpected;
	BOOL m_hasLoopNoticed;
	BOOL m_isMDI;
	RECT m_MDIButton[3];
	UINT m_MDIButtonMode;
	int m_CurrentMDIButton;
	CMDIChild* m_pMDIChild;
};

class CStatic: public CControl
{
public:
	enum TStyle { sLabel, sButton, sToolbarButton, sMenuItem };
	enum TPosition { pnone, pleft, pright, ptop, pbottom };
	enum THAlign { hleft, hcenter, hright };
	enum TVAlign { vtop, vcenter, vbottom };

public:
	CStatic(LPCTSTR name = NULL);
	CStatic(ConstRef(CStringBuffer) name);
	virtual ~CStatic();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	__inline TStyle get_style() { return m_style; }
	__inline void set_style(TStyle s) { m_style = s; }

	__inline TPosition get_position() { return m_position; }
	__inline void set_position(TPosition p) { m_position = p; }

	__inline THAlign get_halign() { return m_halign; }
	__inline void set_halign(THAlign p) { m_halign = p; }

	__inline TVAlign get_valign() { return m_valign; }
	__inline void set_valign(TVAlign p) { m_valign = p; }

	__inline ConstRef(CStringBuffer) get_text() const { return m_text; }
	void set_text(LPCTSTR text, int lench = -1);
	void set_text(ConstRef(CStringBuffer) text);

	__inline const CBitmap& get_iconnormal() const { return m_iconN; }
	__inline void set_iconnormal(const CBitmap& bitmap) { m_iconN = bitmap; }

	__inline const CBitmap& get_icondisabled() const { return m_iconD; }
	__inline void set_icondisabled(const CBitmap& bitmap) { m_iconD = bitmap; }

	__inline const CBitmap& get_iconhot() const { return m_iconH; }
	__inline void set_iconhot(const CBitmap& bitmap) { m_iconH = bitmap; }

	__inline bool is_hovered() { return m_hovered; }
	__inline void set_hovered(bool b) { m_hovered = b; }

	__inline bool is_clicked() { return m_clicked; }
	__inline void set_clicked(bool b) { m_clicked = b; }

	mbchar get_Mnemonic();

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditNotImplCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCopyCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void drawString(Gdiplus::Graphics* pGraphics, Gdiplus::Font* pFont, Gdiplus::PointF& pt, Gdiplus::Brush* brush);

	CStringBuffer m_text;
	CBitmap m_iconN;
	CBitmap m_iconD;
	CBitmap m_iconH;
	TStyle m_style;
	TPosition m_position;
	THAlign m_halign;
	TVAlign m_valign;
	RECT m_iconRect;
	RECT m_textRect;
	int m_ampersandpos;
	int m_tabulatorpos;
	bool m_hovered;
	bool m_clicked;
};

class CPictureBox: public CControl
{
public:
	CPictureBox(LPCTSTR name = NULL);
	CPictureBox(ConstRef(CStringBuffer) name);
	virtual ~CPictureBox();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);

	__inline ConstRef(CStringBuffer) get_filename() const { return m_filename; }

	__inline const Gdiplus::Image* get_picture() const { return m_picture; }
	void set_picture(const Gdiplus::Image* picture);
	void set_picture(LPCTSTR filename);

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditNotImplCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCopyCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	CStringBuffer m_filename;
	const Gdiplus::Image* m_picture;
};

class CSplitter: public CControl
{
public:
	enum TDirection
	{
		TDirectionHorizontal,
		TDirectionVertical
	};
public:
	CSplitter(LPCTSTR name = NULL);
	CSplitter(ConstRef(CStringBuffer) name);
	virtual ~CSplitter();

	__inline TDirection get_Direction() { return m_direction; }
	void set_Direction(TDirection dir);

	__inline CControl* get_FirstControl() { m_firstControl; }
	__inline void set_FirstControl(CControl* control) { m_firstControl = control; }

	__inline CControl* get_SecondControl() { m_secondControl; }
	__inline void set_SecondControl(CControl* control) { m_secondControl = control; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);

	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	TDirection m_direction;
	HCURSOR m_cursor;
	CControl* m_firstControl;
	CControl* m_secondControl;
	POINT m_mouseLastPos;
};

class CScrollbar: public CControl
{
public:
	enum TDirection
	{
		TDirectionHorizontal,
		TDirectionVertical
	};
public:
	CScrollbar(LPCTSTR name = NULL);
	CScrollbar(ConstRef(CStringBuffer) name);
	virtual ~CScrollbar();

	__inline TDirection get_Direction() { return m_direction; }
	__inline void set_Direction(TDirection dir) { m_direction = dir; }

	__inline CControl* get_ScrollWindow() { return m_ScrollWindow; }
	__inline void set_ScrollWindow(CControl* p) { m_ScrollWindow = p; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);	
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnableArrows(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetPos(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetRange(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetScrollBarInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetScrollInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetPos(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetRange(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetRangeRedraw(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetScrollInfo(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	LRESULT _SendMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void _set_Visible(BOOL bVisible);

	TDirection m_direction;
	CControl* m_ScrollWindow;
	POINT m_mouseLastPos;
	UINT m_iRect;
	RECT m_TopRightArrow;
	RECT m_UpRightPageArea;
	RECT m_Thumb;
	RECT m_DownLeftPageArea;
	RECT m_BottomLeftArrow;
	WORD m_EnableArrows;
	int m_Minimum;
	int m_Maximum;
	int m_Position;
	UINT m_Page;
	bool m_bTrackPos;
	int m_TrackPos;

	CBitmap m_ArrowBottom;
	CBitmap m_ArrowLeft;
	CBitmap m_ArrowRight;
	CBitmap m_ArrowTop;
};
