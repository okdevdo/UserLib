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
#include "stdafx.h"
#include "afxwin.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "Configuration.h"
#include "Dialogs.h"
#include "XGuiTest.h"
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

		CListViewNode* pNode = OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Picturebox"));

		OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Static")); 
		OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Textbox und ComboBox")); 
		OK_NEW_OPERATOR CListViewNode(pListView, _T("Test ListView")); 
		OK_NEW_OPERATOR CListViewNode(pListView, _T("Test TreeView")); 
		OK_NEW_OPERATOR CListViewNode(pListView, _T("Test PropertySheet")); 
		OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Splitter")); 
		OK_NEW_OPERATOR CListViewNode(pListView, _T("Test GridView")); 
		OK_NEW_OPERATOR CListViewNode(pListView, _T("Test TextView")); 

		pListView->set_CurrentNode(pNode);
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

static void CreatePictureBox(HWND parent)
{
	CPictureBox* picture = OK_NEW_OPERATOR CPictureBox(_T("XGuiTest.CPictureBox"));

	picture->set_picture(_T("C:\\Users\\Public\\Pictures\\best.jpg"));
	picture->Create(parent, 1001);
}

static void CreateStatic(HWND parent)
{
	CBitmap vBitmap((LPCTSTR)(MAKEINTRESOURCE(IDI_XGUITEST)), IMAGE_ICON);
	RECT margins = { 2, 2, 2, 2 };

	CStatic* pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic1"));
	pStatic->set_text(_T("pleft;hleft;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1001);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic2"));
	pStatic->set_text(_T("pleft;hleft;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1002);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic3"));
	pStatic->set_text(_T("pleft;hleft;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1003);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic4"));
	pStatic->set_text(_T("pleft;hcenter;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1004);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic5"));
	pStatic->set_text(_T("pleft;hcenter;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(4);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1005);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic6"));
	pStatic->set_text(_T("pleft;hcenter;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(5);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1006);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic7"));
	pStatic->set_text(_T("pleft;hright;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(6);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1007);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic8"));
	pStatic->set_text(_T("pleft;hright;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(7);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1008);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic9"));
	pStatic->set_text(_T("pleft;hright;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(8);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1009);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic10"));
	pStatic->set_text(_T("pright;hleft;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1010);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic11"));
	pStatic->set_text(_T("pright;hleft;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(1);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1011);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic12"));
	pStatic->set_text(_T("pright;hleft;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(2);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1012);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic13"));
	pStatic->set_text(_T("pright;hcenter;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(3);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1013);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic14"));
	pStatic->set_text(_T("pright;hcenter;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(4);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1014);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic15"));
	pStatic->set_text(_T("pright;hcenter;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(5);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1015);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic16"));
	pStatic->set_text(_T("pright;hright;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(6);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1016);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic17"));
	pStatic->set_text(_T("pright;hright;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(7);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1017);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic18"));
	pStatic->set_text(_T("pright;hright;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pright);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(8);
	pStatic->set_LayoutX(1);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1018);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic19"));
	pStatic->set_text(_T("ptop;hleft;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1019);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic20"));
	pStatic->set_text(_T("ptop;hleft;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(1);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1020);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic21"));
	pStatic->set_text(_T("ptop;hleft;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(2);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1021);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic22"));
	pStatic->set_text(_T("ptop;hcenter;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(3);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1022);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic23"));
	pStatic->set_text(_T("ptop;hcenter;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(4);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1023);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic24"));
	pStatic->set_text(_T("ptop;hcenter;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(5);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1024);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic25"));
	pStatic->set_text(_T("ptop;hright;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(6);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1025);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic26"));
	pStatic->set_text(_T("ptop;hright;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(7);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1026);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic27"));
	pStatic->set_text(_T("ptop;hright;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::ptop);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(8);
	pStatic->set_LayoutX(2);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1027);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic28"));
	pStatic->set_text(_T("pbottom;hleft;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1028);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic29"));
	pStatic->set_text(_T("pbottom;hleft;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(1);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1029);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic30"));
	pStatic->set_text(_T("pbottom;hleft;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hleft);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(2);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1030);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic31"));
	pStatic->set_text(_T("pbottom;hcenter;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(3);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1031);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic32"));
	pStatic->set_text(_T("pbottom;hcenter;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(4);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1032);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic33"));
	pStatic->set_text(_T("pbottom;hcenter;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hcenter);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(5);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1033);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic34"));
	pStatic->set_text(_T("pbottom;hright;vtop"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vtop);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(6);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1034);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic35"));
	pStatic->set_text(_T("pbottom;hright;vcenter"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(7);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1035);

	pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic36"));
	pStatic->set_text(_T("pbottom;hright;vbottom"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pbottom);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vbottom);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_LayoutY(8);
	pStatic->set_LayoutX(3);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1036);
}

static void CreateTextBox(HWND parent)
{
	CBitmap vBitmap((LPCTSTR)(MAKEINTRESOURCE(IDI_XGUITEST)), IMAGE_ICON);
	RECT margins = { 2, 2, 2, 2 };

	CStatic* pStatic = OK_NEW_OPERATOR CStatic(_T("XGuiTest.CStatic1"));
	pStatic->set_text(_T("Test Textbox"));
	pStatic->set_iconnormal(vBitmap);
	pStatic->set_position(CStatic::pleft);
	pStatic->set_halign(CStatic::hright);
	pStatic->set_valign(CStatic::vcenter);
	pStatic->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeX(150);
	pStatic->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pStatic->set_LayoutSizeY(50);
	pStatic->set_Margins(margins);
	pStatic->Create(parent, 1001);

	CTextEditor* pTextbox = OK_NEW_OPERATOR CTextEditor(_T("XGuiTest.CTextEditor1"));
	pTextbox->set_text(CStringBuffer(__FILE__LINE__ _T("Test Text")));
	pTextbox->set_LayoutX(1);
	pTextbox->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pTextbox->set_LayoutSizeX(150);
	pTextbox->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pTextbox->set_LayoutSizeY(50);
	pTextbox->set_Margins(margins);
	pTextbox->Create(parent, 1002);

	CComboBox* pCombobox = OK_NEW_OPERATOR CComboBox(_T("XGuiTest.CComboBox1"));
	pCombobox->set_text(CStringBuffer(__FILE__LINE__ _T("Test Text")));
	pCombobox->set_LayoutX(2);
	pCombobox->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pCombobox->set_LayoutSizeX(150);
	pCombobox->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pCombobox->set_LayoutSizeY(50);
	pCombobox->set_Margins(margins);
	pCombobox->Create(parent, 1003);

	pCombobox->add_Text(_T("Test Text1"));
	pCombobox->add_Text(_T("Test Text2"));
	pCombobox->add_Text(_T("Test Text3"));
	pCombobox->add_Text(_T("Test Text4"));
	pCombobox->add_Text(_T("Test Text5"));

	CTextControl* pTextbox1 = OK_NEW_OPERATOR CTextControl(_T("XGuiTest.CTextControl1"));
	pTextbox1->set_multiLine(true);
	pTextbox1->set_text(CStringBuffer(__FILE__LINE__ _T("Test Text\nTest Text1\nTest Text2")));
	pTextbox1->set_LayoutY(1);
	pTextbox1->set_LayoutX(0);
	pTextbox1->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pTextbox1->set_LayoutSizeX(150);
	pTextbox1->set_SizeSpecY(CControl::TSizeSpecifierFixedSize);
	pTextbox1->set_LayoutSizeY(50);
	pTextbox1->set_Margins(margins);
	pTextbox1->Create(parent, 1004);
}

static void CreateListView(HWND parent)
{
	RECT margins = { 2, 2, 2, 2 };

	CListView* pListView = OK_NEW_OPERATOR CListView(_T("XGuiTest.CListView"));
	pListView->set_Editable(true);
	//pListView->set_HScrollEnabled(TRUE);
	//pListView->set_VScrollEnabled(TRUE);
	//pListView->set_SizeSpecX(CControl::TSizeSpecifierPercentSize);
	//pListView->set_LayoutSizeX(100);
	//pListView->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	//pListView->set_LayoutSizeY(100);
	pListView->set_Margins(margins);
	pListView->set_SelectMode(CListView::TSelectModeMulti);
	pListView->Create(parent, 1001);

	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text1"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text2"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text3"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text4"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text5"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text6"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text7"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text8"));
}

static void CreateTreeView(HWND parent)
{
	RECT margins = { 2, 2, 2, 2 };

	CTreeView* pTreeView = OK_NEW_OPERATOR CTreeView(_T("XGuiTest.CTreeView"));
	pTreeView->set_Editable(true);
	//pTreeView->set_HScrollEnabled(TRUE);
	//pTreeView->set_VScrollEnabled(TRUE);
	//pTreeView->set_SizeSpecX(CControl::TSizeSpecifierPercentSize);
	//pTreeView->set_LayoutSizeX(100);
	//pTreeView->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	//pTreeView->set_LayoutSizeY(100);
	pTreeView->set_Margins(margins);
	pTreeView->set_MultiSelect(true);
	pTreeView->Create(parent, 1001);

	pTreeView->BeginUpdate();

	CTreeViewNode* pTreeNode1 = OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text1"));

	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text1"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text2"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text3"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text4"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text5"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text6"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text7"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text8"));

	pTreeNode1 = OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text2"));

	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text1"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text2"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text3"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text4"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text5"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text6"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text7"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text8"));

	pTreeNode1 = OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text3"));

	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text1"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text2"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text3"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text4"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text5"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text6"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text7"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text8"));

	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text4"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text5"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text6"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text7"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text8"));
	
	pTreeView->EndUpdate(TRUE);
}

static void CreatePropertySheet(HWND parent)
{
	RECT margins = { 2, 2, 2, 2 };

	CPropertySheet* pPropSheet = OK_NEW_OPERATOR CPropertySheet(_T("XGuiTest.CPropertySheet"));
	pPropSheet->set_border(TRUE);
	pPropSheet->set_Editable(true);
	//pListView->set_HScrollEnabled(TRUE);
	//pListView->set_VScrollEnabled(TRUE);
	//pListView->set_SizeSpecX(CControl::TSizeSpecifierPercentSize);
	//pListView->set_LayoutSizeX(100);
	//pListView->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	//pListView->set_LayoutSizeY(100);
	pPropSheet->set_Margins(margins);
	pPropSheet->set_MultiSelect(true);
	pPropSheet->Create(parent, 1001);

	OK_NEW_OPERATOR CPropertySheetTitleNode(pPropSheet, _T("Test PropertySheet"));
	OK_NEW_OPERATOR CPropertySheetNode(pPropSheet, _T("Test Text1"), -1, _T("value"));
	OK_NEW_OPERATOR CPropertySheetNode(pPropSheet, _T("Test Text2"), -1, _T("value"));
	OK_NEW_OPERATOR CPropertySheetNode(pPropSheet, _T("Test Text3"), -1, _T("value"));
	OK_NEW_OPERATOR CPropertySheetNode(pPropSheet, _T("Test Text4"), -1, _T("value"));
	OK_NEW_OPERATOR CPropertySheetNode(pPropSheet, _T("Test Text5"), -1, _T("value"));
	OK_NEW_OPERATOR CPropertySheetNode(pPropSheet, _T("Test Text6"), -1, _T("value"));
	OK_NEW_OPERATOR CPropertySheetNode(pPropSheet, _T("Test Text7"), -1, _T("value"));
}

static void CreateSplitter(HWND parent)
{
	RECT margins = { 2, 2, 2, 2 };

	CTreeView* pTreeView = OK_NEW_OPERATOR CTreeView(_T("XGuiTest.CTreeView"));
	pTreeView->set_Editable(true);
	pTreeView->set_HScrollEnabled(TRUE);
	pTreeView->set_VScrollEnabled(TRUE);
	pTreeView->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pTreeView->set_LayoutSizeX(100);
	pTreeView->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	pTreeView->set_LayoutSizeY(100);
	pTreeView->set_LayoutX(0);
	pTreeView->set_Margins(margins);
	pTreeView->set_MultiSelect(true);
	pTreeView->Create(parent, 1001);

	pTreeView->BeginUpdate();

	CTreeViewNode* pTreeNode1 = OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text1"));

	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text1"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text2"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text3"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text4"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text5"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text6"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text7"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text8"));

	pTreeNode1 = OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text2"));

	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text1"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text2"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text3"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text4"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text5"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text6"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text7"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text8"));

	pTreeNode1 = OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text3"));

	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text1"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text2"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text3"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text4"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text5"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text6"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text7"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeNode1, _T("Test Text8"));

	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text4"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text5"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text6"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text7"));
	OK_NEW_OPERATOR CTreeViewNode(pTreeView, _T("Test Text8"));
	
	pTreeView->EndUpdate(TRUE);

	CListView* pListView = OK_NEW_OPERATOR CListView(_T("XGuiTest.CListView"));
	pListView->set_Editable(true);
	pListView->set_HScrollEnabled(TRUE);
	pListView->set_VScrollEnabled(TRUE);
	pListView->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pListView->set_LayoutSizeX(100);
	pListView->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	pListView->set_LayoutSizeY(100);
	pListView->set_LayoutX(2);
	pListView->set_Margins(margins);
	pListView->set_SelectMode(CListView::TSelectModeMulti);
	pListView->Create(parent, 1003);

	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text1"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text2"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text3"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text4"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text5"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text6"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text7"));
	OK_NEW_OPERATOR CListViewNode(pListView, _T("Test Text8"));

	CSplitter* pSplitter = OK_NEW_OPERATOR CSplitter(_T("XGuiTest.CSplitter"));

	pSplitter->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	pSplitter->set_LayoutSizeX(10);
	pSplitter->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	pSplitter->set_LayoutSizeY(100);
	pSplitter->set_LayoutX(1);
	pSplitter->set_FirstControl(pTreeView);
	pSplitter->set_SecondControl(pListView);
	pSplitter->set_Direction(CSplitter::TDirectionVertical);
	pSplitter->Create(parent, 1002);
}

static void CreateGridView(HWND parent)
{
	RECT margins = { 2, 2, 2, 2 };

	CGridView* pGridView = OK_NEW_OPERATOR CGridView(_T("XGuiTest.GridView"));

	pGridView->set_border(TRUE);
	pGridView->set_Editable(true);
	//pGridView->set_HScrollEnabled(TRUE);
	//pGridView->set_VScrollEnabled(TRUE);
	//pGridView->set_SizeSpecX(CControl::TSizeSpecifierFixedSize);
	//pGridView->set_LayoutSizeX(100);
	//pGridView->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	//pGridView->set_LayoutSizeY(100);
	//pGridView->set_LayoutX(0);
	pGridView->set_Margins(margins);
	pGridView->set_MultiSelect(true);
	pGridView->Create(parent, 1001);

	pGridView->BeginUpdate();

	CGridViewColumn* pColumn0 = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("Column0"));
	CGridViewColumn* pColumn1 = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("Column1"));
	CGridViewColumn* pColumn2 = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("Column2"));
	CGridViewColumn* pColumn3 = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("Column3"));
	CGridViewColumn* pColumn4 = OK_NEW_OPERATOR CGridViewColumn(pGridView, _T("Column4"));

	pColumn0->set_Editable(true);
	pColumn1->set_Editable(true);
	pColumn2->set_Editable(true);
	pColumn3->set_Editable(true);
	pColumn4->set_Editable(true);

	CGridViewRow* pRow = OK_NEW_OPERATOR CGridViewRow(pGridView, _T("Row0"));

	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn0, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn1, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn2, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn3, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn4, _T("Test Data"));

	pRow = OK_NEW_OPERATOR CGridViewRow(pGridView, _T("Row1"));

	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn0, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn1, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn2, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn3, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn4, _T("Test Data"));

	pRow = OK_NEW_OPERATOR CGridViewRow(pGridView, _T("Row2"));

	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn0, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn1, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn2, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn3, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn4, _T("Test Data"));

	pRow = OK_NEW_OPERATOR CGridViewRow(pGridView, _T("Row3"));

	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn0, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn1, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn2, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn3, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn4, _T("Test Data"));

	pRow = OK_NEW_OPERATOR CGridViewRow(pGridView, _T("Row4"));

	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn0, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn1, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn2, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn3, _T("Test Data"));
	OK_NEW_OPERATOR CGridViewCell(pGridView, pRow, pColumn4, _T("Test Data"));

	pGridView->EndUpdate(TRUE);
}

static void CreateTextView(HWND parent)
{
	RECT margins = { 2, 2, 2, 2 };

	CTextView* pTextbox = OK_NEW_OPERATOR CTextView(_T("XGuiTest.CTextView1"));
	pTextbox->set_text(CStringBuffer(__FILE__LINE__ _T("Test Text\r\nTest Text1\r\nTest Text2\r\nTest Text3\r\nTest Text4")));
	pTextbox->set_SizeSpecX(CControl::TSizeSpecifierPercentSize);
	pTextbox->set_LayoutSizeX(100);
	pTextbox->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
	pTextbox->set_LayoutSizeY(100);
	pTextbox->set_Margins(margins);
	pTextbox->set_HScrollEnabled(TRUE);
	pTextbox->set_VScrollEnabled(TRUE);
	pTextbox->Create(parent, 1002);
}

BEGIN_MESSAGE_MAP(CFrameWnd, XGuiTest)
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

XGuiTest::XGuiTest(): 
	CFrameWnd(_T("XGuiTest")),
	m_pTabControl(nullptr),
	m_PageCnt(0),
	m_pEditorContextMenu(nullptr)
{
}

XGuiTest::~XGuiTest()
{
}

LRESULT XGuiTest::OnTimer(WPARAM wParam, LPARAM lParam)
{
	CFrameWnd::OnTimer(wParam, lParam);
	if ( ((LONG)wParam) == 1001 )
	{
		PostMessage(WM_COMMAND, IDM_NEW, 0);
		::KillTimer(m_hwnd, 1001);
	}
	return 0;
}

LRESULT XGuiTest::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	CFrameWnd::OnDestroy(wParam, lParam);

	if ( m_pEditorContextMenu )
		m_pEditorContextMenu->DestroyWindow();

	CDockInfo* pDockInfo = get_dockinfo();

	pDockInfo->SaveStatus();

	CDialog::RemoveDialogControls();

	return 0;
}

LRESULT XGuiTest::OnCreateToolbar(WPARAM wParam, LPARAM lParam)
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

LRESULT XGuiTest::OnCreateStatusbar(WPARAM wParam, LPARAM lParam)
{
	CStatusbar* pStatusbar = CastAnyPtr(CStatusbar, lParam);

	pStatusbar->add_item(CStatusbar::TStatusStyleStatusMessages, 200, _T("Bereit"));
	pStatusbar->add_item(CStatusbar::TStatusStyleKeyboardStatus, 100, _T("NUM CAPS"));
	pStatusbar->add_item(CStatusbar::TStatusStyleClock, 100, _T("00.00.00 00:00:00"));
	return 0;
}

LRESULT XGuiTest::OnCreateMenubar(WPARAM wParam, LPARAM lParam)
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

bool XGuiTest::_CreateTextContextMenu()
{
	if ( !m_pEditorContextMenu )
	{
		CImageList lNormal((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_NORM)), IMAGE_BITMAP);
		CImageList lDisabled((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_DIS)), IMAGE_BITMAP);
		CImageList lHot((LPCTSTR)(MAKEINTRESOURCE(IDB_TOOLBAR_HOT)), IMAGE_BITMAP);
		HMENU hMenu = ::LoadMenu(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDC_XGUITEST_EDIT));

		if ( !hMenu )
			return false;

		m_pEditorContextMenu = OK_NEW_OPERATOR CPopupMenuEx(_T("XGuiTest.TextContextMenu"));

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

LRESULT XGuiTest::OnEditorContextMenu(WPARAM wParam, LPARAM lParam)
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

LRESULT XGuiTest::OnCreateDockInfo(WPARAM wParam, LPARAM lParam)
{
	CDockInfo* pDockInfo = CastAnyPtr(CDockInfo, lParam);

	pDockInfo->LoadStatus();
	return 0;
}

LRESULT XGuiTest::OnCreateDockWindowCommand(WPARAM wParam, LPARAM lParam)
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

LRESULT XGuiTest::OnCreateChildren(WPARAM wParam, LPARAM lParam)
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

LRESULT XGuiTest::OnAboutCommand(WPARAM wParam, LPARAM lParam)
{
	//MessageBox(m_hwnd, _T("Test"), _T("Test"), MB_ICONEXCLAMATION | MB_YESNO);
	DialogBox(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), m_hwnd, About);
	return 0;
}

LRESULT XGuiTest::OnNewCommand(WPARAM wParam, LPARAM lParam)
{
	COpenDialog* pDialog = OK_NEW_OPERATOR COpenDialog(_T("OpenDialog"), MAKEINTRESOURCE(IDD_OPENDIALOG1), this);

	//int cursel = DialogBox(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE(IDD_OPENDIALOG), m_hwnd, Open);
	INT_PTR cursel = pDialog->DoModal();

	switch ( cursel )
	{
	case 0:
		::DestroyWindow(m_hwnd);
		break;

	case 1:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreatePictureBox(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	case 2:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreateStatic(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	case 3:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreateTextBox(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	case 4:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreateListView(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	case 5:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreateTreeView(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	case 6:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreatePropertySheet(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	case 7:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreateSplitter(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	case 8:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreateGridView(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	case 9:
		{
			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("%s.TabPage%d"), m_pTabControl->get_name().GetString(), m_PageCnt);

			CTabPage* pPage = OK_NEW_OPERATOR CTabPage(name);

			pPage->set_Text(name);
			pPage->set_CreateVisible(FALSE);
			pPage->set_HScrollEnabled(TRUE);
			pPage->set_VScrollEnabled(TRUE);
			pPage->set_border(TRUE);
			pPage->Create(m_pTabControl->get_handle(), 1000 + m_PageCnt);
			CreateTextView(pPage->get_handle());
			m_pTabControl->set_CurrentTabPage(pPage);
			++m_PageCnt;
		}
		break;

	}
	return 0;
}

LRESULT XGuiTest::OnExitCommand(WPARAM wParam, LPARAM lParam)
{
	::DestroyWindow(m_hwnd);
	return 0;
}

LRESULT XGuiTest::OnConfigDialogCommand(WPARAM wParam, LPARAM lParam)
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
	CMDIFrame(_T("XGuiTest"))
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
	CreatePictureBox((HWND)wParam);
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
	CreateStatic((HWND)wParam);
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
	CreateTextBox((HWND)wParam);
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
	CreateListView((HWND)wParam);
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
	CreateTreeView((HWND)wParam);
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
	CreatePropertySheet((HWND)wParam);
	return 0;
}

