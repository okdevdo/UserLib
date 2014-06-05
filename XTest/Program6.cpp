/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

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
#include "Program.h"
#include "Console.h"
#include "ConsoleTextViewer.h"
#include "ConsoleTableViewer.h"
#include "ConsoleMainMenu.h"
#include "ConsoleMenuItem.h"
#include "ConsolePopupMenu.h"
#include "ConsoleEditMenu.h"
#include "ConsoleWindowMenu.h"
#include "ConsoleLayout.h"
#include "ConsoleFileListControl.h"
#include "ConsoleButtonControl.h"
#include "ConsoleLabelControl.h"
#include "ConsoleTextControl.h"
#include "ConsoleComboControl.h"
#include "ConsoleFileDialog.h"
#include "ConsoleTabControl.h"
#include "ConsoleTabPageControl.h"
#include "FilePath.h"
#include "Debug.h"

#ifdef OK_SYS_WINDOWS
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{ 
  switch( fdwCtrlType ) 
  { 
    // Handle the CTRL-C signal. 
    case CTRL_C_EVENT: 
      COUT << _T("Ctrl-C event") << endl << endl;
      Beep( 750, 300 ); 
      return( TRUE );
 
    // CTRL-CLOSE: confirm that the user wants to exit. 
    case CTRL_CLOSE_EVENT: 
      COUT << _T("Ctrl-Close event") << endl << endl;
      Beep( 600, 200 ); 
      return( TRUE ); 
 
    // Pass other signals to the next handler. 
    case CTRL_BREAK_EVENT: 
      Beep( 900, 200 ); 
      COUT << _T("Ctrl-Break event") << endl << endl;
      return FALSE; 
 
    case CTRL_LOGOFF_EVENT: 
      Beep( 1000, 200 ); 
      COUT << _T("Ctrl-Logoff event") << endl << endl;
      return FALSE; 
 
    case CTRL_SHUTDOWN_EVENT: 
      Beep( 750, 500 ); 
      COUT << _T("Ctrl-Shutdown event") << endl << endl;
      return FALSE; 
 
    default: 
      return FALSE; 
  } 
}
#endif

class CTestTabPageControl1: public CConsoleTabPageControl
{
public:
	CTestTabPageControl1(CConstPointer name, CConsole* pConsole): 
	    CConsoleTabPageControl(name, pConsole) {}
	CTestTabPageControl1(CConstPointer name, CConstPointer title, CConsole* pConsole): 
		CConsoleTabPageControl(name, title, pConsole) {}
	CTestTabPageControl1(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole): 
		CConsoleTabPageControl(callback, name, title, pConsole) {}
	CTestTabPageControl1(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole): 
		CConsoleTabPageControl(taborder, callback, name, title, pConsole) {}
	virtual ~CTestTabPageControl1(void) {}

	void Initialize(COORD _size)
	{
		SMALL_RECT margins;
		COORD spacing;

		margins.Left = 1;
		margins.Right = 1;
		margins.Top = 1;
		margins.Bottom = 1;
		spacing.X = 1;
		spacing.Y = 1;

		Ptr(CConsoleVBoxLayout) pVBoxLayout = OK_NEW_OPERATOR CConsoleVBoxLayout(margins, spacing);

		margins.Left = 0;
		margins.Right = 0;
		margins.Top = 0;
		margins.Bottom = 0;

		COORD maxSize;
		COORD minSize;
		COORD prefSize;
		COORD sizeCon;
		COORD align;

		maxSize.X = 0;
		maxSize.Y = 0;
		minSize.X = 0;
		minSize.Y = 0;
		prefSize.X = 0;
		prefSize.Y = 0;
		sizeCon.X = CConsoleLayoutItem::NoConstraint;
		sizeCon.Y = CConsoleLayoutItem::NoConstraint;
		align.X = CConsoleLayoutItem::LeftAlignment;
		align.Y = CConsoleLayoutItem::TopAlignment;

		COORD pos;
		COORD size;

		pos.X = 1;
		pos.Y = 1;
		size.X = _size.X - 2;
		size.Y = 1;

		AddControl(OK_NEW_OPERATOR CConsoleLabelControl(
			_T("TestLabel1"),
			_T("TestLabel1"),
			m_Console), pos, size);

		maxSize.X = SHRT_MAX;
		maxSize.Y = 1;
		minSize.X = 10;
		minSize.Y = 1;
		prefSize.X = _size.X - 2;
		prefSize.Y = 1;
		sizeCon.X = CConsoleLayoutItem::FillConstraint;
		sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
		align.X = CConsoleLayoutItem::LeftAlignment;
		align.Y = CConsoleLayoutItem::TopAlignment;
		pVBoxLayout->addControl(GetControl(_T("TestLabel1")), maxSize, minSize, prefSize, sizeCon, align);

		m_pLayout = pVBoxLayout;
	}

	void Create(COORD pos, COORD size)
	{
		m_Color = m_Console->GetDefaultColor();
		m_HighLightColor = m_Console->GetDefaultHighlightedColor();
		m_hasBorder = true;
		m_BorderStyle = singleborderstyle;
		m_Resizeable = false;
		CConsoleTabPageControl::Create(pos, size);
	}
};

class CTestTabPageControl2: public CConsoleTabPageControl
{
public:
	CTestTabPageControl2(CConstPointer name, CConsole* pConsole): 
	    CConsoleTabPageControl(name, pConsole) {}
	CTestTabPageControl2(CConstPointer name, CConstPointer title, CConsole* pConsole): 
		CConsoleTabPageControl(name, title, pConsole) {}
	CTestTabPageControl2(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole): 
		CConsoleTabPageControl(callback, name, title, pConsole) {}
	CTestTabPageControl2(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole): 
		CConsoleTabPageControl(taborder, callback, name, title, pConsole) {}
	virtual ~CTestTabPageControl2(void) {}

	void Initialize(COORD _size)
	{
		SMALL_RECT margins;
		COORD spacing;

		margins.Left = 1;
		margins.Right = 1;
		margins.Top = 1;
		margins.Bottom = 1;
		spacing.X = 1;
		spacing.Y = 1;

		Ptr(CConsoleVBoxLayout) pVBoxLayout = OK_NEW_OPERATOR CConsoleVBoxLayout(margins, spacing);

		margins.Left = 0;
		margins.Right = 0;
		margins.Top = 0;
		margins.Bottom = 0;

		COORD maxSize;
		COORD minSize;
		COORD prefSize;
		COORD sizeCon;
		COORD align;

		maxSize.X = 0;
		maxSize.Y = 0;
		minSize.X = 0;
		minSize.Y = 0;
		prefSize.X = 0;
		prefSize.Y = 0;
		sizeCon.X = CConsoleLayoutItem::NoConstraint;
		sizeCon.Y = CConsoleLayoutItem::NoConstraint;
		align.X = CConsoleLayoutItem::LeftAlignment;
		align.Y = CConsoleLayoutItem::TopAlignment;

		COORD pos;
		COORD size;

		pos.X = 1;
		pos.Y = 1;
		size.X = _size.X - 2;
		size.Y = 1;

		AddControl(OK_NEW_OPERATOR CConsoleLabelControl(
			_T("TestLabel2"),
			_T("TestLabel2"),
			m_Console), pos, size);

		maxSize.X = SHRT_MAX;
		maxSize.Y = 1;
		minSize.X = 10;
		minSize.Y = 1;
		prefSize.X = _size.X - 2;
		prefSize.Y = 1;
		sizeCon.X = CConsoleLayoutItem::FillConstraint;
		sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
		align.X = CConsoleLayoutItem::LeftAlignment;
		align.Y = CConsoleLayoutItem::TopAlignment;
		pVBoxLayout->addControl(GetControl(_T("TestLabel2")), maxSize, minSize, prefSize, sizeCon, align);

		m_pLayout = pVBoxLayout;
	}

	void Create(COORD pos, COORD size)
	{
		m_Color = m_Console->GetDefaultColor();
		m_HighLightColor = m_Console->GetDefaultHighlightedColor();
		m_hasBorder = true;
		m_BorderStyle = singleborderstyle;
		m_Resizeable = false;
		CConsoleTabPageControl::Create(pos, size);
	}
};

class CTestTabControl: public CConsoleTabControl
{
public:
	CTestTabControl(CConstPointer name, CConsole* pConsole): 
	    CConsoleTabControl(name, pConsole) {}
	CTestTabControl(CConstPointer name, CConstPointer title, CConsole* pConsole): 
		CConsoleTabControl(name, title, pConsole) {}
	CTestTabControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole): 
		CConsoleTabControl(callback, name, title, pConsole) {}
	CTestTabControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole): 
	    CConsoleTabControl(taborder, callback, name, title, pConsole) {}
		virtual ~CTestTabControl(void) {}

	void Initialize(COORD _size)
	{
		_size.X -= 4;
		_size.Y -= 4;

		Ptr(CTestTabPageControl1) pTabPage1 = OK_NEW_OPERATOR CTestTabPageControl1(0, nullptr, _T("TestTabPage1"), _T("TestTabPage1"), m_Console);

		pTabPage1->Initialize(_size);
		AddTabPage(pTabPage1);

		Ptr(CTestTabPageControl2) pTabPage2 = OK_NEW_OPERATOR CTestTabPageControl2(0, nullptr, _T("TestTabPage2"), _T("TestTabPage2"), m_Console);

		pTabPage2->Initialize(_size);
		AddTabPage(pTabPage2);
	}

	void Create(COORD pos, COORD size)
	{
		m_Color = m_Console->GetDefaultColor();
		m_HighLightColor = m_Console->GetDefaultHighlightedColor();
		m_hasBorder = true;
		m_BorderStyle = singleborderstyle;
		m_Resizeable = false;
		CConsoleTabControl::Create(pos, size);
	}
};

class CTestDialog: public CConsoleDialog
{
public:
	CTestDialog(CConstPointer name, CConsole* pConsole): CConsoleDialog(name, pConsole) {}
	CTestDialog(CConstPointer name, CConstPointer title, CConsole* pConsole): CConsoleDialog(name, title, pConsole) {}
	virtual ~CTestDialog() {}

	void Initialize(COORD _size)
	{
		_size.X -= 2;
		_size.Y -= 2;

		COORD pos;

		pos.X = 0;
		pos.Y = 0;

		Ptr(CTestTabControl)  pTab = OK_NEW_OPERATOR CTestTabControl(_T("TabControl"), _T("TabControl"), m_Console);

		pTab->Initialize(_size);

		AddControl(pTab, pos, _size);
	}

	void Create(COORD pos, COORD size)
	{
		m_Color = m_Console->GetDefaultColor();
		m_HighLightColor = m_Console->GetDefaultHighlightedColor();
		m_hasBorder = true;
		m_BorderStyle = doubleborderstyle;
		m_hasTitle = true;
		m_TitleStyle = dialogtitlebarstyle;
		m_Resizeable = false;
		CConsoleDialog::Create(pos, size);

		HideCursor();
	}
};

class CTestConsole: public CConsole
{
public:
	CTestConsole(): m_NumWindows(0), m_pDialog(nullptr) {}

	void TestDialogOeffnenMenuItem(CConsoleMenuItem* pMenu)
	{
		Ptr(CTestDialog) pDialog = OK_NEW_OPERATOR CTestDialog(_T("TestDialog"), _T("TestDialog"), this);
		COORD size;

		size.X = 60;
		if ( size.X > m_ScreenBufferSize.X )
			size.X = m_ScreenBufferSize.X;
		size.Y = 30;
		if ( size.Y > (m_ScreenBufferSize.Y - 1) )
			size.Y = m_ScreenBufferSize.Y - 1;

		pDialog->Initialize(size);

		CreateConsoleModalDialog(pDialog, size);
	}

	void TabelleDateiOeffnenMenuItem(CConsoleMenuItem* pMenu)
	{
#ifdef OK_SYS_WINDOWS
		CreateConsoleWindow(
			OK_NEW_OPERATOR CConsoleTableViewer(CFilePath(__FILE__LINE__ _T("C:\\Users\\okreis\\Documents\\HumanResourcesEmployee.dbf")), _T("Test"), _T("Test"), this));
#endif
#ifdef OK_SYS_UNIX
		CreateConsoleWindow(
			OK_NEW_OPERATOR CConsoleTableViewer(CFilePath(__FILE__LINE__ _T("/home/Oliver/HumanResourcesEmployee.dbf")), _T("Test"), _T("Test"), this));
#endif
	}
	void TextDateiOeffnenMenuItem(CConsoleMenuItem* pMenu)
	{
		if ( NotPtrCheck(m_pDialog) && TFisalloc(m_pDialog) )
			return;

		m_pDialog = 
			OK_NEW_OPERATOR CConsoleFileDialog(
				CConsoleFileDialog::FileDialogModeOpenFile, 
				_T("TextDateiOeffnen"), 
#ifdef OK_SYS_WINDOWS
				_T("Datei \xF6") _T("ffnen"),
#endif
#ifdef OK_SYS_UNIX
				_T("Datei oeffnen"),
#endif
				this);
		COORD size;

		size.X = 60;
		if ( size.X > m_ScreenBufferSize.X )
			size.X = m_ScreenBufferSize.X;
		size.Y = 30;
		if ( size.Y > (m_ScreenBufferSize.Y - 1) )
			size.Y = m_ScreenBufferSize.Y - 1;

		m_pDialog->Initialize2(size);

		CConsoleComboControl* pComboControl = 
			CastDynamicPtr(CConsoleComboControl, m_pDialog->GetControl(_T("WildCardText")));
		if ( pComboControl )
		{
			pComboControl->AddComboItem(CStringBuffer(__FILE__LINE__ _T("*.*")));
			pComboControl->AddComboItem(CStringBuffer(__FILE__LINE__ _T("*.c")));
			pComboControl->AddComboItem(CStringBuffer(__FILE__LINE__ _T("*.cpp")));
			pComboControl->AddComboItem(CStringBuffer(__FILE__LINE__ _T("*.h")));
			pComboControl->AddComboItem(CStringBuffer(__FILE__LINE__ _T("*.hpp")));
		}

		CConsoleButtonControl* pButtonControl = 
			CastDynamicPtr(CConsoleButtonControl, m_pDialog->GetControl(_T("OKButton")));
		if ( pButtonControl )
			pButtonControl->SetCallback(
			    OK_NEW_OPERATOR CConsoleControlCallback<CTestConsole>(this, &CTestConsole::TextDateiOeffnenOKButton));

		pButtonControl = 
			CastDynamicPtr(CConsoleButtonControl, m_pDialog->GetControl(_T("CancelButton")));
		if ( pButtonControl )
			pButtonControl->SetCallback(
			    OK_NEW_OPERATOR CConsoleControlCallback<CTestConsole>(this, &CTestConsole::TextDateiOeffnenCancelButton));

		CreateConsoleModalDialog(m_pDialog, size);
	}
	void TextDateiOeffnenOKButton(CConsoleControl* pControl, DWORD command)
	{
		if ( NotPtrCheck(m_pDialog) )
		{
			CFilePath path;

			CConsoleFileListControl* pFileListControl = 
				CastDynamicPtr(CConsoleFileListControl, m_pDialog->GetControl(_T("FileList")));

			if ( pFileListControl )
				path = pFileListControl->GetPath();

			CConsoleTextControl* pFileText = 
				CastDynamicPtr(CConsoleTextControl,  m_pDialog->GetControl(_T("FileNameText")));

			if ( pFileText )
				path.set_Filename(pFileText->GetText());

			CStringBuffer name;

			name.FormatString(__FILE__LINE__ _T("TextViewer%d"), m_NumWindows++);

			CreateConsoleWindow(
				OK_NEW_OPERATOR CConsoleTextViewer(path, name, path.get_Path(), this));
			CloseConsoleModalDialog(m_pDialog);
			m_pDialog = nullptr;
		}
	}
	void TextDateiOeffnenCancelButton(CConsoleControl* pControl, DWORD command)
	{
		if ( NotPtrCheck(m_pDialog) )
			CloseConsoleModalDialog(m_pDialog);
		m_pDialog = nullptr;
	}
	void DateiBeendenMenuItem(CConsoleMenuItem* pMenu)
	{
		m_Continue = false;
		return;
	}

protected:
	int m_NumWindows;
	CConsoleFileDialog* m_pDialog;
};

void TestConsole()
{
#ifdef OK_SYS_WINDOWS
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#endif

	CTestConsole testCon;
	CConsoleMainMenu* pMainMenu = OK_NEW_OPERATOR CConsoleMainMenu(_T("MainMenu"), &testCon);
	CConsolePopupMenu* pPopupMenu = OK_NEW_OPERATOR CConsolePopupMenu(
		pMainMenu, _T("DateiPopupMenu"), _T(""), &testCon);

	pPopupMenu->AddMenuItem(
		OK_NEW_OPERATOR CConsoleMenuItem(
			OK_NEW_OPERATOR CConsoleMenuItemCallback<CTestConsole>(&testCon, 
				&CTestConsole::TextDateiOeffnenMenuItem),
			_T("MenuItemOpenText"), 
#ifdef OK_SYS_WINDOWS
			_T("\xD6") _T("ffnen"),
#endif
#ifdef OK_SYS_UNIX
			_T("Oeffnen"),
#endif
			&testCon));
	pPopupMenu->AddMenuItem(
		OK_NEW_OPERATOR CConsoleMenuItem(
			OK_NEW_OPERATOR CConsoleMenuItemCallback<CTestConsole>(&testCon, 
				&CTestConsole::TabelleDateiOeffnenMenuItem),
			_T("MenuItemOpenTable"), 
			_T("Tabelle oeffnen"), 
			&testCon));
	pPopupMenu->AddMenuItem(
		OK_NEW_OPERATOR CConsoleMenuItem(
			OK_NEW_OPERATOR CConsoleMenuItemCallback<CTestConsole>(&testCon, 
			    &CTestConsole::TestDialogOeffnenMenuItem),
			_T("MenuItemOpenTestDialog"), 
			_T("Testdialog oeffnen"), 
			&testCon));
	pPopupMenu->AddMenuItem(
		OK_NEW_OPERATOR CConsoleMenuItem(
			OK_NEW_OPERATOR CConsoleMenuItemCallback<CTestConsole>(&testCon, 
				&CTestConsole::DateiBeendenMenuItem),
			_T("MenuItemExit"), 
			_T("Beenden"), 
			&testCon));

	pMainMenu->AddMenuItem(
		OK_NEW_OPERATOR CConsoleMenuItem(
		    pPopupMenu,
		    _T("DateiMenu"), 
		    _T("Datei"), 
			&testCon));

	pMainMenu->AddMenuItem(
		OK_NEW_OPERATOR CConsoleMenuItem(
		    OK_NEW_OPERATOR CConsoleEditMenu(pMainMenu, _T("EditMenuPopup"), _T(""), &testCon),
		    _T("EditMenu"), 
     		_T("Bearbeiten"), 
	    	&testCon));

	pMainMenu->AddMenuItem(
		OK_NEW_OPERATOR CConsoleMenuItem(
		    OK_NEW_OPERATOR CConsoleWindowMenu(pMainMenu, _T("WindowMenuPopup"), _T(""), &testCon),
		    _T("WindowMenu"), 
     		_T("Fenster"), 
	    	&testCon));

	try
	{
		testCon.Start();
		testCon.SetConsoleMainMenu(pMainMenu);
		testCon.Run();
		testCon.Exit();
	}
	catch ( CConsoleException* ex )
	{
		testCon.Exit();
		COUT << ex->GetExceptionMessage() << endl;
	}
	catch ( CBaseException* ex )
	{
		testCon.Exit();
		COUT << ex->GetExceptionMessage() << endl;
	}
}
