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
#include "StdAfx.h"
#include "afxwin.h"
#include "GuiApplication.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "Configuration.h"

CGuiApplication::CGuiApplication(LPCTSTR _defaultAppName):
CApplication(CStringBuffer(__FILE__LINE__ CastAny(CConstPointer, _defaultAppName))),
	m_guiinstance(0),
	m_mainwnd(NULL),
	m_cmdshow(SW_SHOWDEFAULT),
	m_rsrcman(NULL),
	m_fontman(NULL),
	m_brushman(NULL)
#ifdef __DEBUG__
	, m_isDebug(false)
	, m_debugID(0)
#endif
{
}

CGuiApplication::CGuiApplication(ConstRef(CStringBuffer) _defaultAppName):
    CApplication(_defaultAppName),
	m_guiinstance(0),
	m_mainwnd(NULL),
	m_cmdshow(SW_SHOWDEFAULT),
	m_rsrcman(NULL),
	m_fontman(NULL),
	m_brushman(NULL)
#ifdef __DEBUG__
	, m_isDebug(false)
	, m_debugID(0)
#endif
{
}

CGuiApplication::~CGuiApplication(void)
{
}

#ifdef __DEBUG__
bool CGuiApplication::DebugString(const char* format, ...)
{
	if ( !m_isDebug )
		return true;
	if ( !format )
		return false;

	char buffer[MAX_FORMATTEXT];
    va_list argList;

	s_memset(buffer, 0, MAX_FORMATTEXT);
    va_start(argList, format);
#ifdef OK_COMP_MSC
	if ( 0 > vsprintf_s(buffer, MAX_FORMATTEXT, format, argList) )
#else
	if ( 0 > vsprintf(buffer, format, argList) )
#endif
	{
		va_end(argList);
		return false;
	}

	va_end(argList);

	_RPT0(_CRT_WARN, buffer);
	return true;
}
#endif

int CGuiApplication::run(HINSTANCE hGuiInstance, int nCmdShow)
{
	int result = 0;
#if (GDIPVER >= 0x0110)
    Gdiplus::GdiplusStartupInputEx gdiplusStartupInput;
#else
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
#endif
    ULONG_PTR gdiplusToken;

    if ( Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok )
		return -4;

	m_guiinstance = hGuiInstance;
	m_cmdshow = nCmdShow;

	LPTSTR lpCmdLine = GetCommandLine();
	CStringBuffer sCmdLine(__FILE__LINE__ lpCmdLine);
	CStringBuffer appName;
	CPointer argv[64];
	dword argc = 0;

	sCmdLine.SplitQuoted(_T("\""), _T(" "), argv, 64, &argc);
	try
	{
		m_rsrcman = OK_NEW_OPERATOR CResourceManager();
		loadConfiguration(argv[0]);
		appName = config()->GetValue(_T("Application.Name"), m_defaultAppName);
		m_fontman = OK_NEW_OPERATOR CFontManager(appName);
		m_brushman = OK_NEW_OPERATOR CBrushManager(appName);
		if ( processOptions(argc, argv) )
			result = main();
	}
	catch ( COptionException* ex )
	{
		MessageBox(NULL, ex->GetExceptionMessage().GetString(), _T("Option Exception"), MB_OK | MB_ICONERROR);
		result = -4;
	}
	catch ( CBaseException* ex )
	{
		MessageBox(NULL, ex->GetExceptionMessage().GetString(), _T("Exception"), MB_OK | MB_ICONERROR);
		result = -4;
	}
	catch ( ... )
	{
		MessageBox(NULL, _T("FATAL error"), _T("Exception"), MB_OK | MB_ICONERROR);
		result = -4;
	}
	if ( m_brushman )
		delete m_brushman;
	if ( m_fontman )
		delete m_fontman;
	if ( m_rsrcman )
		delete m_rsrcman;

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return result;
}

int CGuiApplication::do_messageloop(CFrameWnd* win)
{
	MSG msg;
	HACCEL hAccelTable = (HACCEL)(m_rsrcman->get_resource(_T("CFRAMEWND"), CResourceManager::WindowAccelarator));

	m_mainwnd = win;
#ifdef __DEBUG__
	theGuiApp->set_Debug(true);
#endif
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls Create %d\n", m_mainwnd->get_name().GetString(), debugID);
#endif
	if ( !(m_mainwnd->Create()) )
		return -4;
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls Create %d\n", m_mainwnd->get_name().GetString(), debugID);
#endif

	BOOL bRet;

	//m_mainwnd->PostMessage(WM_COMMAND, IDM_NEW, 0);
	while ( (bRet = GetMessage(&msg, NULL, 0, 0)) != 0 )
	{
		if ( bRet == -1 )
			return -4;
		if (!TranslateAccelerator(m_mainwnd->get_handle(), hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

int CGuiApplication::do_messageloop(CMDIFrame* win)
{
	MSG msg;
	HACCEL hAccelTable = (HACCEL)(m_rsrcman->get_resource(_T("CMDIFRAME"), CResourceManager::WindowAccelarator));

	m_mainwnd = win;
#ifdef __DEBUG__
	theGuiApp->set_Debug(true);
#endif
#ifdef __DEBUG1__
	int debugID = theGuiApp->get_DebugID();
	theGuiApp->DebugString("Before %ls Create %d\n", m_mainwnd->get_name().GetString(), debugID);
#endif
	if ( !(m_mainwnd->Create()) )
		return -4;
#ifdef __DEBUG1__
	theGuiApp->DebugString("After %ls Create %d\n", m_mainwnd->get_name().GetString(), debugID);
#endif

	BOOL bRet;

	m_mainwnd->PostMessage(WM_COMMAND, IDM_NEW, 0);
	while ( (bRet = GetMessage(&msg, NULL, 0, 0)) != 0 )
	{
		if ( bRet == -1 )
			return -4;
		if (!TranslateMDISysAccel(win->get_clienthandle(), &msg) && 
			!TranslateAccelerator(m_mainwnd->get_handle(), hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}
