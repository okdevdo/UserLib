/******************************************************************************
    
	This file is part of XGuiLog, which is part of UserLib.

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
#include "Dialogs.h"
#include "XGuiLog.h"
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
#include "EventLogChannel.h"
#include "EventLogEvent.h"
#include "EventLogProvider.h"
#include "EventLogger.h"

CEventLogChannels eventLogChannels __FILE__LINE__0P;
CEventLogProviders eventLogProviders __FILE__LINE__0P;
CEventLogEvents eventLogEvents __FILE__LINE__0P;

bool __stdcall TEventLogChannelForEachFunc(ConstPointer data, Pointer context)
{
	Ptr(CEventLogChannel) pChannel = CastAnyPtr(CEventLogChannel, CastMutable(Pointer, data));
	Ptr(CListView) pListView = CastAnyPtr(CListView, context);

	OK_NEW_OPERATOR CListViewNode(pListView, pChannel->get_name());
	return true;
}

class TEventLogChannelForEachFunctor
{
public:
	bool operator()(Ptr(CEventLogChannel) pChannel)
	{
		OK_NEW_OPERATOR CListViewNode(pListView, pChannel->get_name());
		return true;
	}

	Ptr(CListView) pListView;
};

class TEventLogEventForEachFunctor
{
public:
	TEventLogEventForEachFunctor(): pGridView(nullptr), lRowCnt(0) {}

	bool operator()(Ptr(CEventLogEvent) pEvent)
	{
		Ptr(CGridViewRow) pRow = nullptr;
		CStringBuffer sText;

		sText.FormatString(__FILE__LINE__ _T("Row%d"), lRowCnt);
		pRow = OK_NEW_OPERATOR CGridViewRow(pGridView, sText);
		sText.FormatString(__FILE__LINE__ _T("%lld"), pEvent->get_eventRecordID());
		OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn[0], sText);
		sText.FormatString(__FILE__LINE__ _T("%02d.%02d.%04d %02d:%02d:%02d"), pEvent->get_creationTime().GetDays(), pEvent->get_creationTime().GetMonths(), pEvent->get_creationTime().GetYears(),
			pEvent->get_creationTime().GetHours(), pEvent->get_creationTime().GetMinutes(), pEvent->get_creationTime().GetSeconds());
		OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn[1], sText);
		sText.FormatString(__FILE__LINE__ _T("%d"), pEvent->get_eventID());
		OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn[2], sText);
		OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn[3], pEvent->get_channel());
		OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn[4], pEvent->get_computer());
		OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn[5], pEvent->get_userSID());
		++lRowCnt;
		return true;
	}

	CGridView* pGridView;
	CGridViewColumn* pColumn[6];
	dword lRowCnt;
};



class CEventLogEventForEachFunc
{
public:
	CEventLogEventForEachFunc() : pGridView(nullptr), lRowCnt(0) {}
	~CEventLogEventForEachFunc() {}

	CGridView* pGridView;
	CGridViewColumn* pColumn[6];
	dword lRowCnt;
};

bool __stdcall TEventLogEventForEachFunc(ConstPointer data, Pointer context)
{
	Ptr(CEventLogEvent) pEvent = CastAnyPtr(CEventLogEvent, CastMutable(Pointer, data));
	Ptr(CEventLogEventForEachFunc) pContext = CastAnyPtr(CEventLogEventForEachFunc, context);
	Ptr(CGridViewRow) pRow = nullptr;
	CStringBuffer sText;

	sText.FormatString(__FILE__LINE__ _T("Row%d"), pContext->lRowCnt);
	pRow = OK_NEW_OPERATOR CGridViewRow(pContext->pGridView, sText);
	sText.FormatString(__FILE__LINE__ _T("%lld"), pEvent->get_eventRecordID());
	OK_NEW_OPERATOR CGridViewCell(pContext->pGridView, pRow, pContext->pColumn[0], sText);
	sText.FormatString(__FILE__LINE__ _T("%02d.%02d.%04d %02d:%02d:%02d"), pEvent->get_creationTime().GetDays(), pEvent->get_creationTime().GetMonths(), pEvent->get_creationTime().GetYears(),
		pEvent->get_creationTime().GetHours(), pEvent->get_creationTime().GetMinutes(), pEvent->get_creationTime().GetSeconds());
	OK_NEW_OPERATOR CGridViewCell(pContext->pGridView, pRow, pContext->pColumn[1], sText);
	sText.FormatString(__FILE__LINE__ _T("%d"), pEvent->get_eventID());
	OK_NEW_OPERATOR CGridViewCell(pContext->pGridView, pRow, pContext->pColumn[2], sText);
	OK_NEW_OPERATOR CGridViewCell(pContext->pGridView, pRow, pContext->pColumn[3], pEvent->get_channel());
	OK_NEW_OPERATOR CGridViewCell(pContext->pGridView, pRow, pContext->pColumn[4], pEvent->get_computer());
	OK_NEW_OPERATOR CGridViewCell(pContext->pGridView, pRow, pContext->pColumn[5], pEvent->get_userSID());
	++(pContext->lRowCnt);
	return true;
}

BEGIN_MESSAGE_MAP(CDialog, COpenDialog)
	ON_WM_ERASEBKGND()
	ON_WM_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_WM_COMMMAND(IDOK, OnOKCommand)
	ON_WM_COMMMAND(IDCANCEL, OnCancelCommand)
	ON_WM_NOTIFY(NM_LISTVIEWNODEENTERED,OnListViewNodeEntered)
END_MESSAGE_MAP()

COpenDialog::COpenDialog(LPCTSTR name, LPCTSTR resID, CWin* pParent):
	CDialog(name, resID, pParent)
{
}

COpenDialog::COpenDialog(ConstRef(CStringBuffer) name, LPCTSTR resID, CWin* pParent):
	CDialog(name, resID, pParent)
{
}

COpenDialog::~COpenDialog()
{
}

LRESULT COpenDialog::OnEraseBackGround(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT COpenDialog::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	CListView* pListView = CastDynamicPtr(CListView, get_childbyID(IDC_MENULIST));

	if ( pListView )
	{
		CStringBuffer name;

		name.FormatString(__FILE__LINE__ _T("%s.ListView"), get_name().GetString());
		pListView->set_name(name);
		name.AppendString(_T(".Color.Workspace"));
		if ( !(theGuiApp->get_BrushManager()->has_Brush(name)) )
			theGuiApp->get_BrushManager()->set_Brush(name, pListView->get_Brush(_T(".BackgroundColor.Normal"), _T("ListView"), Gdiplus::Color::White));

		if (eventLogChannels.Count() == 0)
			eventLogChannels.Load();

		TEventLogChannelForEachFunctor arg;

		arg.pListView = pListView;
		eventLogChannels.ForEach<TEventLogChannelForEachFunctor>(arg);

		pListView->set_CurrentNode(pListView->get_Node((dword)0));
		pListView->set_VScrollEnabled(TRUE);
		pListView->set_UpdateRects(TRUE);
	}
	return FALSE;
}

LRESULT COpenDialog::OnOKCommand(WPARAM wParam, LPARAM lParam)
{
	CListView* pListView = CastDynamicPtr(CListView, get_childbyID(IDC_MENULIST));

	if ( pListView )
	{
		CListViewNode* pNode = pListView->get_CurrentNode();
		int ix = pListView->inx_Node(pNode);

		if ( ix >= 0 )
		{
			::EndDialog(m_hwnd, ix + 1);
			return 0;
		}
	}
	::EndDialog(m_hwnd, 0);
	return 0;
}

LRESULT COpenDialog::OnCancelCommand(WPARAM wParam, LPARAM lParam)
{
	::EndDialog(m_hwnd, 0);
	return 0;
}

LRESULT COpenDialog::OnListViewNodeEntered(WPARAM wParam, LPARAM lParam)
{
	Ptr(NotifyMessage) pHdr = CastAnyPtr(NotifyMessage, lParam);
	CListView* pListView = CastDynamicPtr(CListView, get_childbyID(IDC_MENULIST));

	if ( pListView )
	{
		int ix = pListView->inx_Node(CastAnyPtr(CListViewNode, pHdr->param));

		if ( ix >= 0 )
		{
			::EndDialog(m_hwnd, ix + 1);
			return 0;
		}
	}
	::EndDialog(m_hwnd, 0);
	return 0;

}

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

static INT_PTR CALLBACK Open(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT cursel;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			HWND hListBox = GetDlgItem(hDlg, IDC_MENULIST);

			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test Picturebox")); 
			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test Static")); 
			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test Textbox und ComboBox")); 
			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test ListView")); 
			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test TreeView")); 
			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test PropertySheet")); 
			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test Splitter")); 
			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test GridView")); 
			::SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)_T("Test TextView")); 

            ::SendMessage(hListBox, LB_SETCURSEL, 0, 0);
			::SetFocus(hListBox);
		}
		return (INT_PTR)FALSE;

	case WM_COMMAND:
        switch (LOWORD(wParam)) 
        { 
		case IDOK: 
			cursel = ::SendMessage(GetDlgItem(hDlg, IDC_MENULIST), LB_GETCURSEL, 0, 0);
			::EndDialog(hDlg, cursel + 1); 
            return (INT_PTR)TRUE;
        case IDCANCEL: 
            ::EndDialog(hDlg, 0); 
            return (INT_PTR)TRUE;
		case IDC_MENULIST:
			if ( HIWORD(wParam) == LBN_DBLCLK )
			{
				cursel = ::SendMessage(GetDlgItem(hDlg, IDC_MENULIST), LB_GETCURSEL, 0, 0);
				::EndDialog(hDlg, cursel + 1); 
				return (INT_PTR)TRUE;
			}
            break;
		}
		break;

	case WM_DESTROY:
		return (INT_PTR)TRUE;

	default:
		break;
	}
	return (INT_PTR)FALSE;
}

BEGIN_MESSAGE_MAP(CFrameWnd, XGuiLog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_MESSAGE(WM_CREATETOOLBAR, OnCreateToolbar)
	ON_WM_MESSAGE(WM_CREATESTATUSBAR, OnCreateStatusbar)
	ON_WM_MESSAGE(WM_CREATEMENUBAR, OnCreateMenubar)
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
	ON_WM_MESSAGE(WM_CREATEDOCKINFO, OnCreateDockInfo)
	ON_WM_COMMMAND(IDM_ABOUT, OnAboutCommand)
	ON_WM_COMMMAND(IDM_NEW, OnNewCommand)
	ON_WM_COMMMAND(IDM_EXIT, OnExitCommand)
	ON_WM_COMMMAND(IDM_CREATEDOCKWINDOW,OnCreateDockWindowCommand)
	ON_WM_COMMMAND(IDM_CONFIGDIALOG,OnConfigDialogCommand)
	ON_WM_NOTIFY(NM_EDITORCONTEXTMENU, OnEditorContextMenu)
END_MESSAGE_MAP()

XGuiLog::XGuiLog(): 
	CFrameWnd(_T("XGuiLog")),
	m_pTabControl(nullptr),
	m_PageCnt(0),
	m_pEditorContextMenu(nullptr)
{
}

XGuiLog::~XGuiLog()
{
}

LRESULT XGuiLog::OnTimer(WPARAM wParam, LPARAM lParam)
{
	CFrameWnd::OnTimer(wParam, lParam);
	if ( ((LONG)wParam) == 1001 )
	{
		PostMessage(WM_COMMAND, IDM_NEW, 0);
		::KillTimer(m_hwnd, 1001);
	}
	return 0;
}

LRESULT XGuiLog::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	CFrameWnd::OnDestroy(wParam, lParam);

	if ( m_pEditorContextMenu )
		m_pEditorContextMenu->DestroyWindow();

	CDockInfo* pDockInfo = get_dockinfo();

	pDockInfo->SaveStatus();

	CDialog::RemoveDialogControls();

	return 0;
}

LRESULT XGuiLog::OnCreateToolbar(WPARAM wParam, LPARAM lParam)
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

LRESULT XGuiLog::OnCreateStatusbar(WPARAM wParam, LPARAM lParam)
{
	CStatusbar* pStatusbar = CastAnyPtr(CStatusbar, lParam);

	pStatusbar->add_item(CStatusbar::TStatusStyleStatusMessages, 200, _T("Bereit"));
	pStatusbar->add_item(CStatusbar::TStatusStyleKeyboardStatus, 100, _T("NUM CAPS"));
	pStatusbar->add_item(CStatusbar::TStatusStyleClock, 100, _T("00.00.00 00:00:00"));
	return 0;
}

LRESULT XGuiLog::OnCreateMenubar(WPARAM wParam, LPARAM lParam)
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

bool XGuiLog::_CreateTextContextMenu()
{
	if ( !m_pEditorContextMenu )
	{
		CImageList lNormal((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_NORM)), IMAGE_BITMAP);
		CImageList lDisabled((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_DIS)), IMAGE_BITMAP);
		CImageList lHot((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_HOT)), IMAGE_BITMAP);
		HMENU hMenu = ::LoadMenu(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDC_XGUILOG_EDIT));

		if ( !hMenu )
			return false;

		m_pEditorContextMenu = OK_NEW_OPERATOR CPopupMenuEx(_T("XGuiLog.TextContextMenu"));

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

LRESULT XGuiLog::OnEditorContextMenu(WPARAM wParam, LPARAM lParam)
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

LRESULT XGuiLog::OnCreateDockInfo(WPARAM wParam, LPARAM lParam)
{
	CDockInfo* pDockInfo = CastAnyPtr(CDockInfo, lParam);

	pDockInfo->LoadStatus();
	return 0;
}

LRESULT XGuiLog::OnCreateDockWindowCommand(WPARAM wParam, LPARAM lParam)
{
	CDockInfo* pDockInfo = get_dockinfo();
	CDockWindow* pdockwindow = pDockInfo->CreateFloating();
	CTabControl* pTabControl = CastDynamicPtr(CTabControl, pdockwindow->get_Client());
	CStringBuffer name = pTabControl->get_name();

	name.AppendString(_T(".TabPage1000"));

	CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

	pPage->set_Text(name);
	pPage->set_CreateVisible(FALSE);
	pPage->set_HScrollEnabled(TRUE);
	pPage->set_VScrollEnabled(TRUE);
	pPage->set_border(TRUE);
	pPage->Create(pTabControl->get_handle(), 1000);
	pTabControl->set_CurrentTabPage(pPage);
	pdockwindow->SendMessage(WM_SIZE, 0, 0);

#ifdef __DEBUG1__
	CListView* pListView = OK_NEW_OPERATOR CListView(_T("ListView"));

	pListView->Create(pPage->get_handle(), 1000);
#endif
	return 0;
}

LRESULT XGuiLog::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer name(get_name());

	name.AppendString(_T(".TabControl"));
	m_pTabControl = OK_NEW_OPERATOR CTabControl(name);

	m_pTabControl->set_SizeSpecX(CControl::TSizeSpecifierPercentSize);
	m_pTabControl->set_LayoutSizeX(100);
	m_pTabControl->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	m_pTabControl->set_LayoutSizeY(100);
	m_pTabControl->Create((HWND)wParam, 1000);

	name.SetString(__FILE__LINE__ _T("ListView"));
	CDialog::AddDialogControl(OK_NEW_OPERATOR CListView(name));
	name.SetString(__FILE__LINE__ _T("TabControl"));
	CDialog::AddDialogControl(OK_NEW_OPERATOR CTabControl(name));

	::SetTimer(m_hwnd, 1001, 500, nullptr);
	return 0;
}

LRESULT XGuiLog::OnAboutCommand(WPARAM wParam, LPARAM lParam)
{
	//MessageBox(m_hwnd, _T("Test"), _T("Test"), MB_ICONEXCLAMATION | MB_YESNO);
	DialogBox(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), m_hwnd, About);
	return 0;
}

LRESULT XGuiLog::OnNewCommand(WPARAM wParam, LPARAM lParam)
{
	COpenDialog* pDialog = OK_NEW_OPERATOR COpenDialog(_T("OpenDialog"), MAKEINTRESOURCE(IDD_OPENDIALOG1), this);

	//int cursel = DialogBox(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDD_OPENDIALOG), m_hwnd, Open);
	INT_PTR cursel = pDialog->DoModal();
	Ptr(CEventLogChannel) pChannel = nullptr;

	switch ( cursel )
	{
	case 0:
		::DestroyWindow(m_hwnd);
		break;
	default:
		pChannel = CastAnyPtr(CEventLogChannel, CastMutablePtr(CEventLogChannel, eventLogChannels.GetData(eventLogChannels.Index((TListIndex)(cursel - 1)))));
		get_StatusBar()->set_text(0, _T("Lädt...."));
		if (eventLogProviders.Count() == 0)
			eventLogProviders.Load();
		eventLogEvents.Close();
		eventLogEvents.Open(__FILE__LINE__ 256, 1024);
		eventLogEvents.Load(pChannel->get_name(), &eventLogProviders);

		CGridView* pGridView = OK_NEW_OPERATOR CGridView(_T("CGridView"));

		CStringBuffer name;

		name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

		CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

		pPage->set_Text(name);
		pPage->set_CreateVisible(FALSE);
		pPage->set_HScrollEnabled(TRUE);
		pPage->set_VScrollEnabled(TRUE);
		pPage->set_border(TRUE);
		pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);

		RECT margins = { 2, 2, 2, 2 };

		pGridView->set_border(TRUE);
		//pGridView->set_Editable(true);
		//pGridView->set_HScrollEnabled(TRUE);
		//pGridView->set_VScrollEnabled(TRUE);
		//pGridView->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
		//pGridView->set_LayoutSizeX(100);
		//pGridView->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
		//pGridView->set_LayoutSizeY(100);
		//pGridView->set_LayoutX(0);
		pGridView->set_Margins(margins);
		pGridView->set_MultiSelect(true);
		pGridView->Create(pPage->get_handle());

		pGridView->BeginUpdate();

		TEventLogEventForEachFunctor arg;

		arg.pGridView = pGridView;
		arg.pColumn[0] = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("EventRecordID"));
		arg.pColumn[1] = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("CreationTime"));
		arg.pColumn[2] = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("EventID"));
		arg.pColumn[3] = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("Channel"));
		arg.pColumn[4] = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("Computer"));
		arg.pColumn[5] = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("UserID"));

		eventLogEvents.ForEach(arg, true);

		pGridView->EndUpdate(TRUE);
		m_pTabControl->set_CurrentTabPage(pPage);
		get_StatusBar()->set_text(0, _T(""));
		++m_PageCnt;
		break;
	}
	return 0;
}

LRESULT XGuiLog::OnExitCommand(WPARAM wParam, LPARAM lParam)
{
	::DestroyWindow(m_hwnd);
	return 0;
}

LRESULT XGuiLog::OnConfigDialogCommand(WPARAM wParam, LPARAM lParam)
{
	CConfigDialog* pDialog = OK_NEW_OPERATOR CConfigDialog(_T("ConfigDialog"), MAKEINTRESOURCE(IDD_CONFIGDIALOG), this);

	pDialog->DoModal();
	return 0;
}

BEGIN_MESSAGE_MAP(CMDIFrame, XGuiMDITest)
	ON_WM_DESTROY()
	ON_WM_MESSAGE(WM_CREATETOOLBAR, OnCreateToolbar)
	ON_WM_MESSAGE(WM_CREATESTATUSBAR, OnCreateStatusbar)
	ON_WM_MESSAGE(WM_CREATEMENUBAR, OnCreateMenubar)
	ON_WM_MESSAGE(WM_CREATEDOCKINFO, OnCreateDockInfo)
	ON_WM_COMMMAND(IDM_ABOUT, OnAboutCommand)
	ON_WM_COMMMAND(IDM_NEW, OnNewCommand)
	ON_WM_COMMMAND(IDM_EXIT, OnExitCommand)
	ON_WM_COMMMAND(IDM_CREATEDOCKWINDOW,OnCreateDockWindowCommand)
END_MESSAGE_MAP()

XGuiMDITest::XGuiMDITest(): 
	CMDIFrame(_T("XGuiLog"))
{
}

XGuiMDITest::~XGuiMDITest()
{
}

LRESULT XGuiMDITest::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	CMDIFrame::OnDestroy(wParam, lParam);

	CDockInfo* pDockInfo = get_dockinfo();

	pDockInfo->SaveStatus();
	return 0;
}

LRESULT XGuiMDITest::OnCreateToolbar(WPARAM wParam, LPARAM lParam)
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

LRESULT XGuiMDITest::OnCreateStatusbar(WPARAM wParam, LPARAM lParam)
{
	CStatusbar* pStatusbar = CastAnyPtr(CStatusbar, lParam);

	pStatusbar->add_item(CStatusbar::TStatusStyleStatusMessages, 200, _T("Bereit"));
	pStatusbar->add_item(CStatusbar::TStatusStyleKeyboardStatus, 100, _T("NUM CAPS"));
	pStatusbar->add_item(CStatusbar::TStatusStyleClock, 100, _T("00.00.00 00:00:00"));
	return 0;
}

LRESULT XGuiMDITest::OnCreateMenubar(WPARAM wParam, LPARAM lParam)
{
	CMenubar* pMenubar = CastAnyPtr(CMenubar, lParam);

	CImageList lNormal((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_NORM)), IMAGE_BITMAP);
	CImageList lDisabled((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_DIS)), IMAGE_BITMAP);
	CImageList lHot((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_HOT)), IMAGE_BITMAP);
	int ix = 0;

	pMenubar->set_ImagesNormal(lNormal);
	pMenubar->set_ImagesDisabled(lDisabled);
	pMenubar->set_ImagesHot(lHot);

	pMenubar->CreateFromHMENU((HMENU)(theGuiApp->get_ResourceManager()->get_resource(_T("CMDIFRAME"), CResourceManager::WindowMenu)));
	pMenubar->get_Menu(0)->set_menutype(CPopupMenu::TMenuTypeFile);
	pMenubar->get_Menu(1)->set_menutype(CPopupMenu::TMenuTypeEdit);
	pMenubar->get_Menu(2)->set_menutype(CPopupMenu::TMenuTypeView);
	pMenubar->get_Menu(3)->set_menutype(CPopupMenu::TMenuTypeWindow);

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

LRESULT XGuiMDITest::OnCreateDockInfo(WPARAM wParam, LPARAM lParam)
{
	CDockInfo* pDockInfo = CastAnyPtr(CDockInfo, lParam);

	pDockInfo->LoadStatus();
	return 0;
}

LRESULT XGuiMDITest::OnCreateDockWindowCommand(WPARAM wParam, LPARAM lParam)
{
	CDockInfo* pDockInfo = get_dockinfo();
	CDockWindow* pdockwindow = pDockInfo->CreateFloating();
	CTabControl* pTabControl = CastDynamicPtr(CTabControl, pdockwindow->get_Client());
	CStringBuffer name = pTabControl->get_name();

	name.AppendString(_T(".TabPage1000"));

	CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

	pPage->set_Text(name);
	pPage->set_CreateVisible(FALSE);
	pPage->set_HScrollEnabled(TRUE);
	pPage->set_VScrollEnabled(TRUE);
	pPage->set_border(TRUE);
	pPage->Create(pTabControl->get_handle(), 1000);
	pTabControl->set_CurrentTabPage(pPage);
	pdockwindow->SendMessage(WM_SIZE, 0, 0);

#ifdef __DEBUG1__
	CListView* pListView = OK_NEW_OPERATOR CListView(_T("ListView"));

	pListView->Create(pPage->get_handle(), 1000);
#endif
	return 0;
}

LRESULT XGuiMDITest::OnAboutCommand(WPARAM wParam, LPARAM lParam)
{
	//MessageBox(m_hwnd, _T("Test"), _T("Test"), MB_ICONEXCLAMATION | MB_YESNO);
	DialogBox(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), m_hwnd, About);
	return 0;
}

LRESULT XGuiMDITest::OnNewCommand(WPARAM wParam, LPARAM lParam)
{
	INT_PTR cursel = DialogBox(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDD_OPENDIALOG), m_hwnd, Open);

	switch ( cursel )
	{
	case 0:
		break;
	case 1:
		{
			XGuiMDIChildTest1* win = OK_NEW_OPERATOR XGuiMDIChildTest1();

			win->set_ClientBorder(TRUE);
			win->set_ClientHScrollEnabled(TRUE);
			win->set_ClientVScrollEnabled(TRUE);
			win->Create(m_clientHWnd);
		}
		break;
	case 2:
		{
			XGuiMDIChildTest2* win = OK_NEW_OPERATOR XGuiMDIChildTest2();

			win->set_ClientBorder(TRUE);
			win->set_ClientHScrollEnabled(TRUE);
			win->set_ClientVScrollEnabled(TRUE);
			win->Create(m_clientHWnd);
		}
		break;
	case 3:
		{
			XGuiMDIChildTest3* win = OK_NEW_OPERATOR XGuiMDIChildTest3();

			win->set_ClientBorder(TRUE);
			win->set_ClientHScrollEnabled(TRUE);
			win->set_ClientVScrollEnabled(TRUE);
			win->Create(m_clientHWnd);
		}
		break;
	case 4:
		{
			XGuiMDIChildTest4* win = OK_NEW_OPERATOR XGuiMDIChildTest4();

			win->set_ClientBorder(TRUE);
			win->set_ClientHScrollEnabled(TRUE);
			win->set_ClientVScrollEnabled(TRUE);
			win->Create(m_clientHWnd);
		}
		break;
	case 5:
		{
			XGuiMDIChildTest5* win = OK_NEW_OPERATOR XGuiMDIChildTest5();

			win->set_ClientBorder(TRUE);
			win->set_ClientHScrollEnabled(TRUE);
			win->set_ClientVScrollEnabled(TRUE);
			win->Create(m_clientHWnd);
		}
		break;
	case 6:
		{
			XGuiMDIChildTest6* win = OK_NEW_OPERATOR XGuiMDIChildTest6();

			win->set_ClientBorder(TRUE);
			win->set_ClientHScrollEnabled(TRUE);
			win->set_ClientVScrollEnabled(TRUE);
			win->Create(m_clientHWnd);
		}
		break;
	}
	return 0;
}

LRESULT XGuiMDITest::OnExitCommand(WPARAM wParam, LPARAM lParam)
{
	DestroyWindow();
	return 0;
}

BEGIN_MESSAGE_MAP(CMDIChild, XGuiMDIChildTest1)
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
END_MESSAGE_MAP()

XGuiMDIChildTest1::XGuiMDIChildTest1(): 
	CMDIChild(_T("XGuiMDIChildTest1"))
{
}

XGuiMDIChildTest1::~XGuiMDIChildTest1()
{}

BOOL XGuiMDIChildTest1::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("XGuiMDIChildTest1");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest1"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest1"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL XGuiMDIChildTest1::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest1"), CResourceManager::WindowTitle));
	return TRUE;
}

LRESULT XGuiMDIChildTest1::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BEGIN_MESSAGE_MAP(CMDIChild, XGuiMDIChildTest2)
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
END_MESSAGE_MAP()

XGuiMDIChildTest2::XGuiMDIChildTest2(): 
	CMDIChild(_T("XGuiMDIChildTest2"))
{
}

XGuiMDIChildTest2::~XGuiMDIChildTest2()
{}

BOOL XGuiMDIChildTest2::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("XGuiMDIChildTest2");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest2"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest2"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL XGuiMDIChildTest2::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest2"), CResourceManager::WindowTitle));
	return TRUE;
}

LRESULT XGuiMDIChildTest2::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BEGIN_MESSAGE_MAP(CMDIChild, XGuiMDIChildTest3)
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
END_MESSAGE_MAP()

XGuiMDIChildTest3::XGuiMDIChildTest3(): 
	CMDIChild(_T("XGuiMDIChildTest3"))
{
}

XGuiMDIChildTest3::~XGuiMDIChildTest3()
{}

BOOL XGuiMDIChildTest3::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("XGuiMDIChildTest3");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest3"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest3"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL XGuiMDIChildTest3::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest3"), CResourceManager::WindowTitle));
	return TRUE;
}

LRESULT XGuiMDIChildTest3::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BEGIN_MESSAGE_MAP(CMDIChild, XGuiMDIChildTest4)
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
END_MESSAGE_MAP()

XGuiMDIChildTest4::XGuiMDIChildTest4(): 
	CMDIChild(_T("XGuiMDIChildTest4"))
{
}

XGuiMDIChildTest4::~XGuiMDIChildTest4()
{}

BOOL XGuiMDIChildTest4::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("XGuiMDIChildTest4");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest4"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest4"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL XGuiMDIChildTest4::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest4"), CResourceManager::WindowTitle));
	return TRUE;
}

LRESULT XGuiMDIChildTest4::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BEGIN_MESSAGE_MAP(CMDIChild, XGuiMDIChildTest5)
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
END_MESSAGE_MAP()

XGuiMDIChildTest5::XGuiMDIChildTest5(): 
	CMDIChild(_T("XGuiMDIChildTest5"))
{
}

XGuiMDIChildTest5::~XGuiMDIChildTest5()
{}

BOOL XGuiMDIChildTest5::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("XGuiMDIChildTest5");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest5"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest5"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL XGuiMDIChildTest5::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest5"), CResourceManager::WindowTitle));
	return TRUE;
}

LRESULT XGuiMDIChildTest5::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BEGIN_MESSAGE_MAP(CMDIChild, XGuiMDIChildTest6)
	ON_WM_MESSAGE(WM_CREATECHILDREN, OnCreateChildren)
END_MESSAGE_MAP()

XGuiMDIChildTest6::XGuiMDIChildTest6(): 
	CMDIChild(_T("XGuiMDIChildTest6"))
{
}

XGuiMDIChildTest6::~XGuiMDIChildTest6()
{}

BOOL XGuiMDIChildTest6::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.lpszClassName	= _T("XGuiMDIChildTest6");
	cls.hIcon			= (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest6"), CResourceManager::WindowIcon));
	cls.hIconSm		    = (HICON)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest6"), CResourceManager::WindowSmallIcon));
	return TRUE;
}

BOOL XGuiMDIChildTest6::PreCreate(CREATESTRUCT& cs)
{
	cs.lpszName = (LPTSTR)(theGuiApp->get_ResourceManager()->get_resource(_T("XGuiMDIChildTest6"), CResourceManager::WindowTitle));
	return TRUE;
}

LRESULT XGuiMDIChildTest6::OnCreateChildren(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

