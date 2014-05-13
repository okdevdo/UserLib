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
#include "XGuiTest.h"

class XGuiTestApplication: public CGuiApplication
{
public:
	XGuiTestApplication():
	    CGuiApplication(_T("XGuiTest")),
		m_bAnyOption(false),
		m_bHelp(false),
		m_bMDITest(false)
	{
        COptionCallback<XGuiTestApplication> cb(this, &XGuiTestApplication::handleHelp);
                
		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("TestMDI"))
			.shortName(_T("mdi"))
			.category(_T("Test"))
			.description(_T("Test MDI.")));
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral &value)
	{
		m_bHelp = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		if ( name == CStringLiteral(_T("TestMDI")) )
			m_bMDITest = true;
		m_bAnyOption = true;
	}

	virtual int main()
	{
		int result = 0;

		if ( m_bHelp )
		{
			CStringBuffer tmp;

			usage(tmp);
			MessageBox(NULL, tmp.GetString(), _T("Usage"), MB_OK | MB_ICONINFORMATION);
			return result;
		}

		if ( !m_bAnyOption )
		{
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowAccelarator, IDC_XGUITEST);
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowTitle, IDS_APP_TITLE);
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowIcon, IDI_XGUITEST);
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowSmallIcon, IDI_SMALL);
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowMenu, IDC_XGUITEST);

			XGuiTest* win = OK_NEW_OPERATOR XGuiTest();

			win->set_ClientBorder(TRUE);
			win->set_ClientHScrollEnabled(TRUE);
			win->set_ClientVScrollEnabled(TRUE);
			win->set_StatusBarEnabled(TRUE);
			win->set_ToolBarEnabled(TRUE);
			win->set_DockingEnabled(TRUE);
			result = do_messageloop(win);
		}
		if ( m_bMDITest )
		{
			get_ResourceManager()->add_resource(_T("CMDIFRAME"), CResourceManager::WindowAccelarator, IDC_XMDITEST);
			get_ResourceManager()->add_resource(_T("CMDIFRAME"), CResourceManager::WindowTitle, IDS_APP_TITLE);
			get_ResourceManager()->add_resource(_T("CMDIFRAME"), CResourceManager::WindowIcon, IDI_XGUITEST);
			get_ResourceManager()->add_resource(_T("CMDIFRAME"), CResourceManager::WindowSmallIcon, IDI_SMALL);
			get_ResourceManager()->add_resource(_T("CMDIFRAME"), CResourceManager::WindowMenu, IDC_XMDITEST);

			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest1"), CResourceManager::WindowTitle, IDS_DOCKING);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest1"), CResourceManager::WindowIcon, IDI_DOCKING);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest1"), CResourceManager::WindowSmallIcon, IDI_DOCKING);

			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest2"), CResourceManager::WindowTitle, IDS_CLASSVIEW);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest2"), CResourceManager::WindowIcon, IDI_CLASSVIEW);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest2"), CResourceManager::WindowSmallIcon, IDI_CLASSVIEW);

			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest3"), CResourceManager::WindowTitle, IDS_FILEVIEW);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest3"), CResourceManager::WindowIcon, IDI_FILEVIEW);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest3"), CResourceManager::WindowSmallIcon, IDI_FILEVIEW);

			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest4"), CResourceManager::WindowTitle, IDS_RECT);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest4"), CResourceManager::WindowIcon, IDI_RECT);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest4"), CResourceManager::WindowSmallIcon, IDI_RECT);

			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest5"), CResourceManager::WindowTitle, IDS_TEXT);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest5"), CResourceManager::WindowIcon, IDI_TEXT);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest5"), CResourceManager::WindowSmallIcon, IDI_TEXT);

			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest6"), CResourceManager::WindowTitle, IDS_TEXT);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest6"), CResourceManager::WindowIcon, IDI_TEXT);
			get_ResourceManager()->add_resource(_T("XGuiMDIChildTest6"), CResourceManager::WindowSmallIcon, IDI_TEXT);

			XGuiMDITest* win = OK_NEW_OPERATOR XGuiMDITest();

			win->set_StatusBarEnabled(TRUE);
			win->set_ToolBarEnabled(TRUE);
			win->set_DockingEnabled(TRUE);
			result = do_messageloop(win);
		}
		return result;
	}

private:
	WBool m_bAnyOption;
	WBool m_bHelp;
	WBool m_bMDITest;
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	XGuiTestApplication app;

	return app.run(hInstance, nCmdShow);
}