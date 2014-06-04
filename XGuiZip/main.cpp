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
#include "XGuiZip.h"

class XGuiZipApplication: public CGuiApplication
{
public:
	XGuiZipApplication():
	    CGuiApplication(_T("XGuiZip")),
		m_bAnyOption(false),
		m_bHelp(false)
	{
        COptionCallback<XGuiZipApplication> cb(this, &XGuiZipApplication::handleHelp);
                
		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral &value)
	{
		m_bHelp = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		m_bAnyOption = true;
	}

	virtual int main()
	{
		int result = 0;

		if ( m_bHelp )
		{
			CStringBuffer tmp;

			usage(tmp);
			MessageBox(nullptr, tmp.GetString(), _T("Usage"), MB_OK | MB_ICONINFORMATION);
			return result;
		}

		if ( !m_bAnyOption )
		{
			HWND hwnd = ::FindWindow(_T("XGUIZIP"), nullptr);

			if ( hwnd )
			{
				::SetForegroundWindow(hwnd);
				return 0;
			}

			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowAccelarator, IDC_XGUIZIP);
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowTitle, IDS_APP_TITLE);
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowIcon, IDI_XGUIZIP);
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowSmallIcon, IDI_SMALL);
			get_ResourceManager()->add_resource(_T("CFRAMEWND"), CResourceManager::WindowMenu, IDC_XGUIZIP);

			XGuiZip* win = OK_NEW_OPERATOR XGuiZip();

			win->set_ClientBorder(TRUE);
			win->set_ClientHScrollEnabled(TRUE);
			win->set_ClientVScrollEnabled(TRUE);
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
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	XGuiZipApplication app;

	return app.run(hInstance, nCmdShow);
}