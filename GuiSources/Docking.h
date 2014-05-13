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

#include "afxwin.h"

class CDockInfo;
class CDockWindow: public CWin
{
public:
	enum TDockingStyle
	{
		TDockingStyleNone,
		TDockingStyleLeft,
		TDockingStyleTop,
		TDockingStyleRight,
		TDockingStyleBottom
	};

	typedef struct _tagNeighbour
	{
		TDockingStyle style;
		CDockWindow* win;

		_tagNeighbour(CDockWindow* _win):
		    style(TDockingStyleLeft), win(_win) {}
		_tagNeighbour(TDockingStyle _style, CDockWindow* _win):
		    style(_style), win(_win) {}
	} TNeighbour;

	typedef CDataSVectorT<TNeighbour> TNeighbourVector;

public:
	CDockWindow(LPCTSTR name = NULL);
	CDockWindow(ConstRef(CStringBuffer) name);
	virtual ~CDockWindow();

	__inline TDockingStyle get_dockingstyle() { return m_dockingstyle; }
	__inline void set_dockingstyle(TDockingStyle style) { m_dockingstyle = style; }

	__inline BOOL is_Floating() { return m_isFloating; }
	__inline void set_Floating(BOOL b) { m_isFloating = b; }

	__inline CControl* get_Client() { return m_control; }
	__inline void set_Client(CControl* pControl) { m_control = pControl; }

	__inline CDockInfo* get_DockInfo() { return m_pDockInfo; }
	__inline void set_DockInfo(CDockInfo* pDockInfo) { m_pDockInfo = pDockInfo; }

	__inline ConstRef(CStringBuffer) get_Caption() { return m_caption; }
	void set_Caption(ConstRef(CStringBuffer) caption);

	__inline UINT get_SavedChildID() { return m_childID; }

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

	void Dock(LPRECT r);
	void UnDock(LPRECT r);

	__inline bool has_Neighbours() { return m_neighbours.Count() > 0; }
	bool has_Neighbours(TDockingStyle style);
	__inline TNeighbourVector::Iterator get_Neighbours() { return m_neighbours.Begin(); }
	TNeighbourVector::Iterator get_FirstNeighbour(TDockingStyle style);
	TNeighbourVector::Iterator get_NextNeighbour(TDockingStyle style, TNeighbourVector::Iterator it);
	void AdjustNeighbours(TDockingStyle style);
	void CopyNeighbours(CDockWindow* pSource);
	void MoveNeighbours(TDockingStyle style, LONG offset);
	void SizeNeighbours(TDockingStyle style, LONG offset);
	void ClearNeighbours();
	void ClearNeighbours(TDockingStyle style);
	void AppendNeighbour(TDockingStyle style, CDockWindow* win);
	void RemoveNeighbour(CDockWindow* win);

	static CStringBuffer DockingStyle2String(TDockingStyle style);
	static TDockingStyle String2DockingStyle(ConstRef(CStringLiteral) style);

	LRESULT OnNcActivate(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcCalcSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnSizing(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnterSizeMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnTabPageDragged(WPARAM wParam, LPARAM lParam);
	LRESULT OnTabPageSelected(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void NcHitTest(POINT pt);

	CStringBuffer m_caption;
	TNeighbourVector m_neighbours;
	TDockingStyle m_dockingstyle;
	RECT m_ncmargins;
	LONG m_nccaption;
	CControl* m_control;
	POINT m_mouseclick;
	RECT m_FloatingRect;
	BOOL m_isFloating;
	CDockInfo* m_pDockInfo;
	UINT m_childID;
	BOOL m_isSizing;
	INT m_SizingEdge;
};

class CDockWindowVector: public CDataVectorT<CDockWindow>
{
public:
	CDockWindowVector(DECL_FILE_LINE TListCnt max, TListCnt exp);

	void StartTransact();
	void Rollback();
	void Commit();

	bool CheckSize(CDockWindow::TDockingStyle style, bool isNeighbour, POINT delta);
	void Resize(CDockWindow::TDockingStyle style, bool isNeighbour, POINT delta, RECT clientR, bool* bClientRUpdate);

protected:
	TListCnt m_transact[4];
	TListIndex m_curtransact;
};

class CDockTarget;
class CDockTargetCenter;
class CDockHint;
class CDockInfo: public CCppObject
{
public:
	enum TDockTarget
	{
		TDockTargetLeft,
		TDockTargetTop,
		TDockTargetRight,
		TDockTargetBottom
	};

public:
	CDockInfo(CWin* pFrame = NULL);
	virtual ~CDockInfo();

	void SaveStatus();
	void LoadStatus();

	void set_Visible(BOOL visible);
	void set_windowrect(LPRECT r);
	void get_clientrect(LPRECT r);

	CDockWindow* CreateFloating(LPCTSTR name = NULL, LPRECT rect = NULL);
	CDockWindow* CreateDocked(LPCTSTR name = NULL, LPRECT rect = NULL);

	void Destroy(CDockWindow* pWindow);
	void AutoSize();
	void UpdateFrame(CDockWindow* pWin);

	void ShowDockTargets(CDockWindow* pWin);
	void CheckDockTargets(LPPOINT pt, bool bLast = false);
	void HideDockTargets();

	void ShowFloatingWindows();
	void HideFloatingWindows();

	bool TestSizable(CDockWindow* pWin, INT edge);
	bool Resize(CDockWindow* pWin, INT edge, POINT ptDelta);

	void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();

protected:
	void FindClientNeighboursLeft2RightTop(RECT r1);
	void FindClientNeighboursLeft2RightBottom(RECT r1);
	void FindClientNeighboursTop2DownLeft(RECT r1);
	void FindClientNeighboursTop2DownRight(RECT r1);
	void FindClientNeighboursRight2LeftTop(RECT r1);
	void FindClientNeighboursRight2LeftBottom(RECT r1);
	void FindClientNeighboursDown2TopLeft(RECT r1);
	void FindClientNeighboursDown2TopRight(RECT r1);
	bool TestSizableLeftTop(CDockWindow* pCurWin, CDockWindowVector* pCurWins, CDockWindow* pCurLeftNeighbour, CDockWindowVector* pCurNeighbours, LONG edge);
	bool TestSizableLeftBottom(CDockWindow* pCurWin, CDockWindowVector* pCurWins, CDockWindow* pCurLeftNeighbour, CDockWindowVector* pCurNeighbours, LONG edge);
	bool TestSizableTopLeft(CDockWindow* pCurWin, CDockWindowVector* pCurWins, CDockWindow* pCurTopNeighbour, CDockWindowVector* pCurNeighbours, LONG edge);
	bool TestSizableTopRight(CDockWindow* pCurWin, CDockWindowVector* pCurWins, CDockWindow* pCurTopNeighbour, CDockWindowVector* pCurNeighbours, LONG edge);
	bool TestSizableClientLeft(CDockWindow* pWin, CDockWindowVector* pCurWins, CDockWindowVector* pCurNeighbours);
	bool TestSizableClientRight(CDockWindow* pWin, CDockWindowVector* pCurWins, CDockWindowVector* pCurNeighbours);
	bool TestSizableClientTop(CDockWindow* pWin, CDockWindowVector* pCurWins, CDockWindowVector* pCurNeighbours);
	bool TestSizableClientBottom(CDockWindow* pWin, CDockWindowVector* pCurWins, CDockWindowVector* pCurNeighbours);
	UINT get_cmdcode();

	CDockWindowVector m_dockwindows;
	CDockWindowVector m_floatingdockwindows;
	CDockWindow* m_pdockwin;
	CDockWindow* m_pundockwin;
	CDockWindow* m_pCurrentFloatingWindow;
	RECT m_windowRect;
	RECT m_clientRect;
	CWin* m_pFrame;
	CDockTargetCenter* m_pTargetCenter;
	CDockTarget* m_pTarget[4];
	CDockHint* m_pHint;
	BOOL m_shutdown;
};