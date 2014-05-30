/******************************************************************************
    
	This file is part of XGuiZip, which is part of UserLib.

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

class CTabControl;
class CTreeView;
class XGuiZip: public CFrameWnd
{
public:
	XGuiZip();
	virtual ~XGuiZip();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual BOOL PreCreate(CREATESTRUCT& cs);

	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateToolbar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateStatusbar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateMenubar(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateChildren(WPARAM wParam, LPARAM lParam);
	LRESULT OnAboutCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnOpenCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnAddFilesCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnExtractCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnConfigDialogCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnInitMenuPopup(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	bool _CreateTextContextMenu();

	CTabControl* m_pTabControl;
	UINT m_PageCnt;
	CPopupMenuEx* m_pEditorContextMenu;
};

void XGuiZipViewFiles(CConstPointer zipfile, CTreeView* pTreeView);
void XZipAddFiles(CStringLiteral zipfile, WBool recursefolders, ConstRef(TMBCharList) filespecs);
void XGuiZipExtractFiles(CStringLiteral zipfile, WBool usefolders, ConstRef(TMBCharList) filespecs, Ref(CDataVectorT<CStringBuffer>) messages);
