/******************************************************************************
    
	This file is part of XGuiTest, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#pragma once

#include "resource.h"
#include "afxwin.h"

class COpenDialog: public CDialog
{
public:
	COpenDialog(LPCTSTR name = NULL, LPCTSTR resID = NULL, CWin* pParent = NULL);
	COpenDialog(ConstRef(CStringBuffer) name, LPCTSTR resID = NULL, CWin* pParent = NULL);
	virtual ~COpenDialog();

	LRESULT OnEraseBackGround(WPARAM wParam, LPARAM lParam);
	LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	LRESULT OnOKCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCancelCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnListViewNodeEntered(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class CTabControl;
class XGuiTest: public CFrameWnd
{
public:
	XGuiTest();
	virtual ~XGuiTest();

	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateToolbar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateStatusbar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateMenubar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateDockInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);
	LRESULT OnAboutCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnConfigDialogCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateDockWindowCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorContextMenu(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	bool _CreateTextContextMenu();

	CTabControl* m_pTabControl;
	UINT m_PageCnt;
	CPopupMenuEx* m_pEditorContextMenu;
};

class XGuiMDITest: public CMDIFrame
{
public:
	XGuiMDITest();
	virtual ~XGuiMDITest();

	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateToolbar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateStatusbar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateMenubar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateDockInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnAboutCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateDockWindowCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class XGuiMDIChildTest1: public CMDIChild
{
public:
	XGuiMDIChildTest1();
	virtual ~XGuiMDIChildTest1();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class XGuiMDIChildTest2: public CMDIChild
{
public:
	XGuiMDIChildTest2();
	virtual ~XGuiMDIChildTest2();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class XGuiMDIChildTest3: public CMDIChild
{
public:
	XGuiMDIChildTest3();
	virtual ~XGuiMDIChildTest3();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class XGuiMDIChildTest4: public CMDIChild
{
public:
	XGuiMDIChildTest4();
	virtual ~XGuiMDIChildTest4();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class XGuiMDIChildTest5: public CMDIChild
{
public:
	XGuiMDIChildTest5();
	virtual ~XGuiMDIChildTest5();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class XGuiMDIChildTest6: public CMDIChild
{
public:
	XGuiMDIChildTest6();
	virtual ~XGuiMDIChildTest6();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

