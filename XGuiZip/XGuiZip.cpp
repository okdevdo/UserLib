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
#include "stdafx.h"
#include "afxwin.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "Configuration.h"
#include "DirectoryIterator.h"
#include "Dialogs.h"
#include "XGuiZip.h"
#include "ComboBox.h"
#include "TabControl.h"
#include "ListView.h"
#include "TreeView.h"
#include "GridView.h"
#include "PropertySheet.h"
#include "TextEditor.h"
#include "TextView.h"
#include "TextControl.h"
#include "Docking.h"

static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			::EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

	case WM_DESTROY:
		return (INT_PTR)TRUE;

	default:
		break;
	}
	return (INT_PTR)FALSE;
}

BEGIN_MESSAGE_MAP(CFrameWnd, XGuiZip)
	ON_WM_DESTROY()
	ON_WM_INITMENUPOPUP()
	ON_WM_MESSAGE(WM_CREATETOOLBAR, OnCreateToolbar)
	ON_WM_MESSAGE(WM_CREATESTATUSBAR, OnCreateStatusbar)
	ON_WM_MESSAGE(WM_CREATEMENUBAR, OnCreateMenubar)
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
	ON_WM_COMMMAND(IDM_ABOUT, OnAboutCommand)
	ON_WM_COMMMAND(IDM_NEW, OnNewCommand)
	ON_WM_COMMMAND(IDM_OPEN, OnOpenCommand)
	ON_WM_COMMMAND(IDM_ADDFILES, OnAddFilesCommand)
	ON_WM_COMMMAND(IDM_EXTRACT, OnExtractCommand)
	ON_WM_COMMMAND(IDM_EXIT, OnExitCommand)
	ON_WM_COMMMAND(IDM_CONFIGDIALOG,OnConfigDialogCommand)
	ON_WM_NOTIFY(NM_EDITORCONTEXTMENU, OnEditorContextMenu)
END_MESSAGE_MAP()

XGuiZip::XGuiZip(): 
	CFrameWnd(_T("XGuiZip")),
	m_pTabControl(nullptr),
	m_PageCnt(0),
	m_pEditorContextMenu(nullptr)
{
}

XGuiZip::~XGuiZip()
{
}

BOOL XGuiZip::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("XGUIZIP");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("CFRAMEWND"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("CFRAMEWND"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL XGuiZip::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("CFRAMEWND"), CResourceManager::WindowTitle));
	return TRUE;
}

LRESULT XGuiZip::OnInitMenuPopup(WPARAM wParam, LPARAM lParam)
{
	CFrameWnd::OnInitMenuPopup(wParam, lParam);

	CPopupMenu* pMenu = CastAnyPtr(CPopupMenu, lParam);
	UINT cmds[] = { IDM_ADDFILES, IDM_EXTRACT };

	for ( int ix = 0; ix < (sizeof(cmds)/sizeof(UINT)); ++ix )
	{
		CControl* pCtrl = pMenu->get_menuItem(cmds[ix]);

		if ( pCtrl )
		{
			switch ( cmds[ix] )
			{
			case IDM_ADDFILES:
				pCtrl->set_Enabled(m_pTabControl->get_CurrentTabPage() != nullptr);
				break;
			case IDM_EXTRACT:
				{
					CTabPage* pCurPage = m_pTabControl->get_CurrentTabPage();

					if ( pCurPage )
					{
						CTreeView* pTreeView = CastDynamicPtr(CTreeView, pCurPage->get_childbyID(1000));

						if ( pTreeView )
						{
							pCtrl->set_Enabled(pTreeView->get_NodeCount() > 0);
							break;
						}
					}
					pCtrl->set_Enabled(FALSE);
				}
				break;
			}
		}
	}
	return 0;
}

LRESULT XGuiZip::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	CFrameWnd::OnDestroy(wParam, lParam);

	if ( m_pEditorContextMenu )
		m_pEditorContextMenu->DestroyWindow();

	CDialog::RemoveDialogControls();

	return 0;
}

LRESULT XGuiZip::OnCreateToolbar(WPARAM wParam, LPARAM lParam)
{
	CToolbar* pToolbar = CastAnyPtr(CToolbar, lParam);

	CImageList lNormal((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_NORM)), IMAGE_BITMAP);
	CImageList lDisabled((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_DIS)), IMAGE_BITMAP);
	CImageList lHot((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_HOT)), IMAGE_BITMAP);

	pToolbar->set_ImagesNormal(lNormal);
	pToolbar->set_ImagesDisabled(lDisabled);
	pToolbar->set_ImagesHot(lHot);

	pToolbar->AddButtonControl(IDM_NEW);
	pToolbar->AddButtonControl(IDM_OPEN);
	pToolbar->AddButtonControl(IDM_SAVE);
	pToolbar->AddButtonControl(IDM_CUT);
	pToolbar->AddButtonControl(IDM_COPY);
	pToolbar->AddButtonControl(IDM_PASTE);
	pToolbar->AddButtonControl(IDM_PRINT);
	pToolbar->AddButtonControl(IDM_ABOUT);

	return 0;
}

LRESULT XGuiZip::OnCreateStatusbar(WPARAM wParam, LPARAM lParam)
{
	CStatusbar* pStatusbar = CastAnyPtr(CStatusbar, lParam);

	pStatusbar->add_item(CStatusbar::TStatusStyleStatusMessages, 200, _T("Bereit"));
	pStatusbar->add_item(CStatusbar::TStatusStyleKeyboardStatus, 100, _T("NUM CAPS"));
	pStatusbar->add_item(CStatusbar::TStatusStyleClock, 100, _T("00.00.00 00:00:00"));
	return 0;
}

LRESULT XGuiZip::OnCreateMenubar(WPARAM wParam, LPARAM lParam)
{
	CMenubar* pMenubar = CastAnyPtr(CMenubar, lParam);

	CImageList lNormal((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_NORM)), IMAGE_BITMAP);
	CImageList lDisabled((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_DIS)), IMAGE_BITMAP);
	CImageList lHot((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_HOT)), IMAGE_BITMAP);

	pMenubar->set_ImagesNormal(lNormal);
	pMenubar->set_ImagesDisabled(lDisabled);
	pMenubar->set_ImagesHot(lHot);

	pMenubar->CreateFromHMENU((HMENU)(theGuiApp->get_ResourceManager()->get_resource(_T("CFRAMEWND"), CResourceManager::WindowMenu)));
	pMenubar->get_Menu(0)->set_menutype(CPopupMenu::TMenuTypeFile);
	pMenubar->get_Menu(1)->set_menutype(CPopupMenu::TMenuTypeEdit);
	pMenubar->get_Menu(2)->set_menutype(CPopupMenu::TMenuTypeView);

	int ix = 0;

	pMenubar->set_Image(ix++, IDM_NEW);
	pMenubar->set_Image(ix++, IDM_OPEN);
	pMenubar->set_Image(ix++, IDM_SAVE);
	pMenubar->set_Image(ix++, IDM_CUT);
	pMenubar->set_Image(ix++, IDM_COPY);
	pMenubar->set_Image(ix++, IDM_PASTE);
	pMenubar->set_Image(ix++, IDM_PRINT);
	pMenubar->set_Image(ix++, IDM_ABOUT);
	return 0;
}

bool XGuiZip::_CreateTextContextMenu()
{
	if ( !m_pEditorContextMenu )
	{
		CImageList lNormal((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_NORM)), IMAGE_BITMAP);
		CImageList lDisabled((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_DIS)), IMAGE_BITMAP);
		CImageList lHot((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_HOT)), IMAGE_BITMAP);
		HMENU hMenu = ::LoadMenu(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDC_XGUITEST_EDIT));

		if ( !hMenu )
			return false;

		m_pEditorContextMenu = OK_NEW_OPERATOR CPopupMenuEx(_T("XGuiZip.TextContextMenu"));

		m_pEditorContextMenu->set_CreateVisible(FALSE);
		m_pEditorContextMenu->set_ClientBorder(TRUE);
		m_pEditorContextMenu->set_ImagesNormal(lNormal);
		m_pEditorContextMenu->set_ImagesDisabled(lDisabled);
		m_pEditorContextMenu->set_ImagesHot(lHot);
		m_pEditorContextMenu->Create();
		m_pEditorContextMenu->CreateFromHMENU(hMenu);

		int ix = 3;

		m_pEditorContextMenu->set_Image(ix++, IDM_CUT);
		m_pEditorContextMenu->set_Image(ix++, IDM_COPY);
		m_pEditorContextMenu->set_Image(ix++, IDM_PASTE);

		::DestroyMenu(hMenu);
	}
	return true;
}

LRESULT XGuiZip::OnEditorContextMenu(WPARAM wParam, LPARAM lParam)
{
	Ptr(NotifyMessage) pHdr = CastAnyPtr(NotifyMessage, lParam);
	Ptr(TNotifyMessageEditorContextMenu) param = CastAnyPtr(TNotifyMessageEditorContextMenu, pHdr->param);

	if ( !_CreateTextContextMenu() )
		return -1;

	UINT codes[] = { IDM_UNDO, IDM_REDO, IDM_CUT, IDM_COPY, IDM_PASTE, IDM_DELETE };

	for ( UINT ix = 0; ix < (sizeof(codes) / sizeof(UINT)); ++ix )
		m_pEditorContextMenu->set_Enabled(codes[ix], (param->pEditor->SendMessage(WM_COMMAND, MAKEWPARAM(codes[ix], 2), 0))?true:false);
	m_pEditorContextMenu->Track(param->pt, param->pEditor);
	return 0;
}

LRESULT XGuiZip::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer name(get_name());

	name.AppendString(_T(".TabControl"));
	m_pTabControl = OK_NEW_OPERATOR CTabControl(name);

	m_pTabControl->set_SizeSpecX(CControl::TSizeSpecifierPercentSize);
	m_pTabControl->set_LayoutSizeX(100);
	m_pTabControl->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	m_pTabControl->set_LayoutSizeY(100);
	m_pTabControl->Create((HWND)wParam, 1000);

	name.SetString(__FILE__LINE__ _T("TabControl"));
	CDialog::AddDialogControl(OK_NEW_OPERATOR CTabControl(name));

	return 0;
}

LRESULT XGuiZip::OnAboutCommand(WPARAM wParam, LPARAM lParam)
{
	DialogBox(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), m_hwnd, About);
	return 0;
}

LRESULT XGuiZip::OnNewCommand(WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	TCHAR pFile[1024] = { 0 };

	SecureZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFilter = _T("ZipFiles\0*.zip\0All Files\0*.*\0");
	ofn.lpstrFile = pFile;
	ofn.nMaxFile = 1024;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = _T("zip");

	if ( ::GetSaveFileName(&ofn) )
	{
		CStringBuffer name;

		name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

		CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

		pPage->set_Text(ofn.lpstrFile);
		pPage->set_CreateVisible(FALSE);
		pPage->set_HScrollEnabled(TRUE);
		pPage->set_VScrollEnabled(TRUE);
		pPage->set_border(TRUE);
		pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
		m_pTabControl->set_CurrentTabPage(pPage);
		++m_PageCnt;
	}
	return 0;
}

LRESULT XGuiZip::OnOpenCommand(WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	TCHAR pFile[1024] = { 0 };

	SecureZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFilter = _T("ZipFiles\0*.zip\0All Files\0*.*\0");
	ofn.lpstrFile = pFile;
	ofn.nMaxFile = 1024;
	ofn.lpstrDefExt = _T("zip");
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if ( ::GetOpenFileName(&ofn) )
	{
		CStringBuffer name;

		name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

		CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

		pPage->set_Text(ofn.lpstrFile);
		pPage->set_CreateVisible(FALSE);
		pPage->set_HScrollEnabled(TRUE);
		pPage->set_VScrollEnabled(TRUE);
		pPage->set_border(TRUE);
		pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);

		name += _T(".TreeView");

		CTreeView* pTreeView = OK_NEW_OPERATOR CTreeView(name);

		pTreeView->set_MultiSelect(true);
		pTreeView->Create(pPage->get_handle(), 1000);

		XGuiZipViewFiles(ofn.lpstrFile, pTreeView);

		m_pTabControl->set_CurrentTabPage(pPage);
		++m_PageCnt;
	}
	return 0;
}

static LPOPENFILENAME OFNHookProc_ofn = nullptr;
static UINT_PTR CALLBACK OFNHookProc_AddFilesExtract(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hParent = ::GetParent(hdlg);

	switch ( uiMsg )
	{
	case WM_INITDIALOG:
		CommDlg_OpenSave_HideControl(hParent, IDOK);
		CommDlg_OpenSave_HideControl(hParent, IDCANCEL);
		OFNHookProc_ofn = CastAnyPtr(OPENFILENAME, lParam);
		break;
	case WM_NOTIFY:
        switch ( ((LPNMHDR)lParam)->code )
        {
		case CDN_FILEOK:
			::SetWindowLong(hdlg, DWLP_MSGRESULT, 1);
			return TRUE;
		default:
			break;
		}
		break;
	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_ADDFILES:
			case IDC_EXTRACT:
				if ( OFNHookProc_ofn )
				{
					LPTSTR pStr = OFNHookProc_ofn->lpstrFile;
					CPointer vSplit[64];
					dword nSplit;
					int iStrSize = 1024;
					int iStrLen;

					if ( (iStrLen = CommDlg_OpenSave_GetFolderPath(hParent, pStr, iStrSize)) < 0 )
						goto _ReturnEmpty;
					pStr += iStrLen;
					iStrSize -= iStrLen;
					if ( (iStrLen = CommDlg_OpenSave_GetSpec(hParent, pStr, iStrSize)) < 0 )
						goto _ReturnEmpty;
					if ( iStrLen == 0 )
					{
						s_strcpy(pStr, iStrSize, _T("*.*"));
						iStrLen = 4;
					}
					else
					{
						s_strsplitquoted(pStr, _T("\""), _T(" "), vSplit, 64, &nSplit);
						if ( nSplit == 0 )
							goto _ReturnEmpty;
					}
					pStr += iStrLen;
					iStrSize -= iStrLen;
					if ( iStrSize > 0 )
						*pStr = _T('\0');
					else
					{
						pStr -= 2;
						*pStr++ = _T('\0');
						*pStr = _T('\0');
					}
				}
				::PostMessage(hParent, WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
				break;
			case IDC_ADDCANCEL:
			case IDC_EXTRCANCEL:
_ReturnEmpty:
				if ( OFNHookProc_ofn )
					*(OFNHookProc_ofn->lpstrFile) = _T('\0');
				::PostMessage(hParent, WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
				break;
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT XGuiZip::OnAddFilesCommand(WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	TCHAR pFile[1024] = { 0 };

	SecureZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hInstance = theGuiApp->get_GuiInstance();
	ofn.lpTemplateName = MAKEINTRESOURCE(IDD_ADDFILESDIALOGEX);
	ofn.lpfnHook = OFNHookProc_AddFilesExtract;
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrTitle = _T("Hinzufuegen");
	ofn.lpstrFilter = _T("All Files\0*.*\0");
	ofn.lpstrFile = pFile;
	ofn.nMaxFile = 1024;
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;

	::GetOpenFileName(&ofn);

	if ( NotStrEmpty(pFile) )
	{
		CPointer p = pFile;
		CFilePath dir(__FILE__LINE__ p);
		TMBCharList filespecs(__FILE__LINE__ 16, 16);
		dword sz;

		sz = s_strlen(p,1024);
		p += sz + 1;
		while ( NotStrEmpty(p) )
		{
			filespecs.Append(p);
			sz = s_strlen(p,1024);
			p += sz + 1;
		}

		CDirectoryIterator::SetCurrentDirectory(dir);

		CTabPage* pPage = m_pTabControl->get_CurrentTabPage();
		CStringBuffer zipfile(pPage->get_Text());

		XZipAddFiles(zipfile, true, filespecs);

		CTreeView* pTreeView = CastDynamicPtr(CTreeView, pPage->get_childbyID(1000));
		CStringBuffer name(pTreeView->get_name());

		pTreeView->DestroyWindow();
		pTreeView = OK_NEW_OPERATOR CTreeView(name);
		pTreeView->set_MultiSelect(true);
		pTreeView->Create(pPage->get_handle(), 1000);

		XGuiZipViewFiles(zipfile, pTreeView);
	}

	return 0;
}

LRESULT XGuiZip::OnExtractCommand(WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	TCHAR pFile[1024] = { 0 };

	SecureZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hInstance = theGuiApp->get_GuiInstance();
	ofn.lpTemplateName = MAKEINTRESOURCE(IDD_EXTRACTDIALOGEX);
	ofn.lpfnHook = OFNHookProc_AddFilesExtract;
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrTitle = _T("Extrahieren");
	ofn.lpstrFilter = _T("All Files\0*.*\0");
	ofn.lpstrFile = pFile;
	ofn.nMaxFile = 1024;
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;

	::GetOpenFileName(&ofn);

	if ( NotStrEmpty(pFile) )
	{
		CPointer p = pFile;
		CFilePath dir(__FILE__LINE__ p);
		TMBCharList filespecs(__FILE__LINE__ 16, 16);
		CDataVectorT<CStringBuffer> messages(__FILE__LINE__ 16, 16);
		dword sz;
		dword sl = 1024;

		sz = s_strlen(p,sl);
		p += sz + 1;
		sl -= sz + 1;
		while ( NotStrEmpty(p) )
		{
			filespecs.Append(p);
			sz = s_strlen(p,sl);
			p += sz + 1;
			sl -= sz + 1;
		}

		CDirectoryIterator::SetCurrentDirectory(dir);

		CTabPage* pPage = m_pTabControl->get_CurrentTabPage();
		CStringBuffer zipfile(pPage->get_Text());

		XGuiZipExtractFiles(zipfile, true, filespecs, messages);
		if ( messages.Count() > 0 )
			::MessageBox(m_hwnd, messages.Join(_T("\r\n")), _T("Messages"), MB_OK);
	}
	return 0;
}

LRESULT XGuiZip::OnExitCommand(WPARAM wParam, LPARAM lParam)
{
	::DestroyWindow(m_hwnd);
	return 0;
}

LRESULT XGuiZip::OnConfigDialogCommand(WPARAM wParam, LPARAM lParam)
{
	CConfigDialog* pDialog = OK_NEW_OPERATOR CConfigDialog(_T("ConfigDialog"), MAKEINTRESOURCE(IDD_CONFIGDIALOG), this);

	pDialog->DoModal();
	return 0;
}

