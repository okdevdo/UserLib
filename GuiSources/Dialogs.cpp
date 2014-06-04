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
#include "Dialogs.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "Configuration.h"
#include "PropertySheet.h"
#include "ComboBox.h"
#include "TabControl.h"

BEGIN_MESSAGE_MAP(CDialog, CConfigDialog)
	ON_WM_ERASEBKGND()
	ON_WM_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_WM_COMMMAND(IDOK, OnOKCommand)
	ON_WM_COMMMAND(IDCANCEL, OnCancelCommand)
END_MESSAGE_MAP()

CConfigDialog::CConfigDialog(LPCTSTR name, LPCTSTR resID, CWin* pParent):
	CDialog(name, resID, pParent),
	m_pSheetFarben(nullptr),
	m_pSheetSchriftarten(nullptr)
{
}

CConfigDialog::CConfigDialog(ConstRef(CStringBuffer) name, LPCTSTR resID, CWin* pParent):
	CDialog(name, resID, pParent),
	m_pSheetFarben(nullptr),
	m_pSheetSchriftarten(nullptr)
{
}

CConfigDialog::~CConfigDialog()
{
}

LRESULT CConfigDialog::OnEraseBackGround(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CConfigDialog::FillFarben()
{
	CWin* pFrame = get_frame();
	CDataVectorT<CStringBuffer> keys(__FILE__LINE__ 1024, 256);

	pFrame->get_BrushKeys(keys);

	m_pSheetFarben->BeginUpdate();

	CPropertySheetTitleNode* pTitleNode = OK_NEW_OPERATOR CPropertySheetTitleNode(m_pSheetFarben, _T("Farben"));
	CDataVectorT<CStringBuffer>::Iterator it = keys.Begin();
	CStringBuffer vAppName = theGuiApp->config()->GetValue(_T("Application.Name"));

	while ( it )
	{
		CStringBuffer vKey(*it);
		CStringBuffer vConfigKey;

		vConfigKey.FormatString(__FILE__LINE__ _T("%sUser.%s"), vAppName.GetString(), vKey.GetString());

		CStringBuffer vValue = theGuiApp->config()->GetValue(vConfigKey);

		if ( vValue.IsEmpty() )
		{
			vConfigKey.FormatString(__FILE__LINE__ _T("%s.%s"), vAppName.GetString(), vKey.GetString());
			vValue = theGuiApp->config()->GetValue(vConfigKey);
		}

		CPropertySheetNode* pNode = OK_NEW_OPERATOR CPropertySheetNode(m_pSheetFarben, vKey, vValue);

		++it;
	}
	m_pSheetFarben->EndUpdate(TRUE);

	CStringBuffer name(m_pSheetFarben->get_name());

	name.AppendString(_T(".Editor"));

	CComboBox* pEditor = OK_NEW_OPERATOR CComboBox(name);

	pEditor->set_CreateVisible(FALSE);
	pEditor->set_Editable(true);
	pEditor->Create(m_pSheetFarben->get_handle(), 1000);

	CDataVectorT<CStringBuffer> colorN(__FILE__LINE__ 256, 256);
	CStringBuffer vNullItem(__FILE__LINE__ _T("<nullptr>"));

	theGuiApp->get_BrushManager()->enumerate_Colors(colorN);
	pEditor->add_Text(vNullItem);
	it = colorN.Begin();
	while ( it )
	{
		pEditor->add_Text(*it);
		++it;
	}
	m_pSheetFarben->set_Editor(pEditor);
}

void CConfigDialog::SaveFarben()
{
	for ( dword ix = 0; ix < (m_pSheetFarben->get_NodeCount()); ++ix )
	{
		CPropertySheetAbstractNode* pNode = m_pSheetFarben->get_Node(ix);

		if ( pNode->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
		{
			CPropertySheetNode* pNode1 = CastDynamicPtr(CPropertySheetNode, pNode);

			if ( pNode1 && (pNode1->is_Modified()) )
			{
				CStringBuffer vValue(pNode1->get_ValueString());

				if ( vValue.Compare(_T("<nullptr>")) == 0 )
					vValue.Clear();
				theGuiApp->get_BrushManager()->set_Brush(pNode1->get_KeyString(), vValue);
			}
		}
	}
}

void CConfigDialog::FillFonts()
{
	CWin* pFrame = get_frame();
	CDataVectorT<CStringBuffer> keys(__FILE__LINE__ 1024, 256);

	pFrame->get_FontKeys(keys);

	m_pSheetSchriftarten->BeginUpdate();

	CPropertySheetTitleNode* pTitleNode = OK_NEW_OPERATOR CPropertySheetTitleNode(m_pSheetSchriftarten, _T("Schriftarten"));
	CDataVectorT<CStringBuffer>::Iterator it = keys.Begin();
	CStringBuffer vAppName = theGuiApp->config()->GetValue(_T("Application.Name"));

	while ( it )
	{
		CStringBuffer vKey(*it);
		CStringBuffer vConfigKey;

		vConfigKey.FormatString(__FILE__LINE__ _T("%sUser.%s"), vAppName.GetString(), vKey.GetString());

		CStringBuffer vValue = theGuiApp->config()->GetValue(vConfigKey);

		if ( vValue.IsEmpty() )
		{
			vConfigKey.FormatString(__FILE__LINE__ _T("%s.%s"), vAppName.GetString(), vKey.GetString());
			vValue = theGuiApp->config()->GetValue(vConfigKey);
		}

		CPropertySheetNode* pNode = OK_NEW_OPERATOR CPropertySheetNode(m_pSheetSchriftarten, vKey, vValue);

		++it;
	}
	m_pSheetSchriftarten->EndUpdate(TRUE);
}

void CConfigDialog::SaveFonts()
{
	for ( dword ix = 0; ix < (m_pSheetSchriftarten->get_NodeCount()); ++ix )
	{
		CPropertySheetAbstractNode* pNode = m_pSheetSchriftarten->get_Node(ix);

		if ( pNode->get_Discriminator() == CPropertySheetAbstractNode::TDiscriminatorNode )
		{
			CPropertySheetNode* pNode1 = CastDynamicPtr(CPropertySheetNode, pNode);

			if ( pNode1 && (pNode1->is_Modified()) )
				theGuiApp->get_FontManager()->set_Font(pNode1->get_KeyString(), pNode1->get_ValueString());
		}
	}
}

LRESULT CConfigDialog::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	CTabControl* pTabControl = CastDynamicPtr(CTabControl, get_childbyID(IDC_TABPAGES));

	if ( pTabControl )
	{
		CStringBuffer name;
		CStringBuffer name1;

		name.FormatString(__FILE__LINE__ _T("%s.TabControl"), get_name().GetString());
		pTabControl->set_name(name);

		name1 = name;
		name1.AppendString(_T(".TabPageFarben"));

		CTabPage* pTabPage = OK_NEW_OPERATOR CTabPage(name1);
		pTabPage->set_Text(_T("Farben"));
		pTabPage->set_CreateVisible(FALSE);
		pTabPage->set_border(TRUE);
		pTabPage->Create(pTabControl->get_handle(), 1000);

		name1.AppendString(_T(".PropertySheet"));

		m_pSheetFarben = OK_NEW_OPERATOR CPropertySheet(name1);
		m_pSheetFarben->set_Editable(true);
		m_pSheetFarben->set_HScrollEnabled(TRUE);
		m_pSheetFarben->set_VScrollEnabled(TRUE);
		m_pSheetFarben->set_SizeSpecX(CControl::TSizeSpecifierPercentSize);
		m_pSheetFarben->set_LayoutSizeX(100);
		m_pSheetFarben->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
		m_pSheetFarben->set_LayoutSizeY(100);
		m_pSheetFarben->set_MinimumValueWidth(100);
		m_pSheetFarben->Create(pTabPage->get_handle(), 1000);

		FillFarben();

		name1 = name;
		name1.AppendString(_T(".TabPageSchriftarten"));

		pTabPage = OK_NEW_OPERATOR CTabPage(name1);

		pTabPage->set_Text(_T("Schriftarten"));
		pTabPage->set_CreateVisible(FALSE);
		pTabPage->set_border(TRUE);
		pTabPage->Create(pTabControl->get_handle(), 1001);

		name1.AppendString(_T(".PropertySheet"));

		m_pSheetSchriftarten = OK_NEW_OPERATOR CPropertySheet(name1);
		m_pSheetSchriftarten->set_Editable(true);
		m_pSheetSchriftarten->set_HScrollEnabled(TRUE);
		m_pSheetSchriftarten->set_VScrollEnabled(TRUE);
		m_pSheetSchriftarten->set_SizeSpecX(CControl::TSizeSpecifierPercentSize);
		m_pSheetSchriftarten->set_LayoutSizeX(100);
		m_pSheetSchriftarten->set_SizeSpecY(CControl::TSizeSpecifierPercentSize);
		m_pSheetSchriftarten->set_LayoutSizeY(100);
		m_pSheetSchriftarten->set_MinimumValueWidth(100);
		m_pSheetSchriftarten->Create(pTabPage->get_handle(), 1000);

		FillFonts();
	}
	CenterWindow();
	return FALSE;
}

LRESULT CConfigDialog::OnOKCommand(WPARAM wParam, LPARAM lParam)
{
	SaveFarben();
	SaveFonts();
	get_frame()->Refresh();
	::EndDialog(m_hwnd, 0);
	return 0;
}

LRESULT CConfigDialog::OnCancelCommand(WPARAM wParam, LPARAM lParam)
{
	::EndDialog(m_hwnd, 0);
	return 0;
}


