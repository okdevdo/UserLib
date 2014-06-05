/******************************************************************************
    
	This file is part of ConSources, which is part of UserLib.

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
#include "CON_PCH.H"
#include "ConsoleFileDialog.h"
#include "Console.h"
#include "ConsoleDirListControl.h"
#include "ConsoleFileListControl.h"
#include "ConsoleButtonControl.h"
#include "ConsoleComboControl.h"
#include "ConsoleLabelControl.h"
#include "ConsoleTextControl.h"
#include "ConsoleLayout.h"
#include "Application.h"
#include "Configuration.h"
#ifdef OK_SYS_WINDOWS
#ifndef MAX_PATH
#define MAX_PATH _MAX_PATH
#endif
#endif
#ifdef OK_SYS_UNIX
#define MAX_PATH 260
#endif

CConsoleFileDialog::CConsoleFileDialog(CConstPointer name, CConsole* pConsole):
    CConsoleDialog(name, pConsole),
	m_FileDialogMode(FileDialogModeOpenFile),
	m_Path()
{
}

CConsoleFileDialog::CConsoleFileDialog(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleDialog(name, title, pConsole),
	m_FileDialogMode(FileDialogModeOpenFile),
	m_Path()
{
}

CConsoleFileDialog::CConsoleFileDialog(FileDialogMode mode, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleDialog(name, title, pConsole),
	m_FileDialogMode(mode),
	m_Path()
{
}

CConsoleFileDialog::~CConsoleFileDialog(void)
{
	SaveConfiguration();
}

void CConsoleFileDialog::Initialize(COORD _size)
{
#ifdef OK_SYS_WINDOWS
	CConstPointer startDirPattern = _T("C:\\*");
#endif
#ifdef OK_SYS_UNIX
	CConstPointer startDirPattern = _T("/*");
#endif
	_size.X -= 2;
	_size.Y -= 2;

	COORD pos;
	COORD size;

	pos.X = 1;
	pos.Y = 1;
	size.X = _size.X - 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleLabelControl(
		_T("PathLabel"),
		startDirPattern,
		m_Console), pos, size);

	pos.X = 1;
	pos.Y = 3;
	size.X = (_size.X - 4) / 2;
	size.Y = _size.Y - 11;

	AddControl(OK_NEW_OPERATOR CConsoleDirListControl(
		0,
		startDirPattern,
		OK_NEW_OPERATOR CConsoleControlCallback<CConsoleFileDialog>(this, &CConsoleFileDialog::DirListCallback),
		_T("DirList"),
		_T("Directories"),
		m_Console), pos, size);

	pos.X = ((_size.X - 4) / 2) + 3;
	pos.Y = 3;
	size.X = (_size.X - 4) / 2;
	size.Y = _size.Y - 11;

	AddControl(OK_NEW_OPERATOR CConsoleFileListControl(
		1,
		startDirPattern,
		OK_NEW_OPERATOR CConsoleControlCallback<CConsoleFileDialog>(this, &CConsoleFileDialog::FileListCallback),
		_T("FileList"),
		_T("Files"),
		m_Console), pos, size);

	pos.X = 1;
	pos.Y = _size.Y - 7;
	size.X = (_size.X - 4) / 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleLabelControl(
		_T("WildCardLabel"),
		_T("Suchmuster"),
		m_Console), pos, size);

	pos.X = ((_size.X - 4) / 2) + 3;
	pos.Y = _size.Y - 7;
	size.X = (_size.X - 4) / 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleComboControl(
		2,
		OK_NEW_OPERATOR CConsoleControlCallback<CConsoleFileDialog>(this, &CConsoleFileDialog::WildCardCallback),
		_T("WildCardText"),
		_T(""),
		m_Console), pos, size);

	pos.X = 1;
	pos.Y = _size.Y - 5;
	size.X = (_size.X - 4) / 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleLabelControl(
		_T("FileNameLabel"),
		_T("Dateiname"),
		m_Console), pos, size);

	pos.X = ((_size.X - 4) / 2) + 3;
	pos.Y = _size.Y - 5;
	size.X = (_size.X - 4) / 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleTextControl(
		MAX_PATH,
		false,
		3,
		OK_NEW_OPERATOR CConsoleControlCallback<CConsoleFileDialog>(this, 
		    &CConsoleFileDialog::FileNameCallback),
		_T("FileNameText"),
		_T(""),
		m_Console), pos, size);

	pos.X = _size.X - 27;
	pos.Y = _size.Y - 3;
	size.X = 12;
	size.Y = 3;

	m_DefaultButton = OK_NEW_OPERATOR CConsoleButtonControl(
		4,
		nullptr,
		_T("OKButton"),
		_T("OK"),
		m_Console);
	AddControl(m_DefaultButton, pos, size);

	pos.X = _size.X - 14;
	pos.Y = _size.Y - 3;
	size.X = 13;
	size.Y = 3;

	m_CancelButton = OK_NEW_OPERATOR CConsoleButtonControl(
		5,
		nullptr,
		_T("CancelButton"),
		_T("Abbrechen"),
		m_Console);
	AddControl(m_CancelButton, pos, size);
}

void CConsoleFileDialog::Initialize2(COORD _size)
{
#ifdef OK_SYS_WINDOWS
	CConstPointer startDirPattern = _T("C:\\*");
#endif
#ifdef OK_SYS_UNIX
	CConstPointer startDirPattern = _T("/*");
#endif
	_size.X -= 2;
	_size.Y -= 2;

	SMALL_RECT margins;
	COORD spacing;

	margins.Left = 1;
	margins.Right = 1;
	margins.Top = 1;
	margins.Bottom = 1;
	spacing.X = 1;
	spacing.Y = 1;

	Ptr(CConsoleVBoxLayout) pVBoxLayout = OK_NEW_OPERATOR CConsoleVBoxLayout(margins, spacing);
	Ptr(CConsoleHBoxLayout) pHBoxLayout;

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
		_T("PathLabel"),
		startDirPattern,
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
	pVBoxLayout->addControl(GetControl(_T("PathLabel")), maxSize, minSize, prefSize, sizeCon, align);

	pHBoxLayout = OK_NEW_OPERATOR CConsoleHBoxLayout(margins, spacing);

	pos.X = 1;
	pos.Y = 3;
	size.X = (_size.X - 4) / 2;
	size.Y = _size.Y - 11;

	AddControl(OK_NEW_OPERATOR CConsoleDirListControl(
		0,
		startDirPattern,
		OK_NEW_OPERATOR CConsoleControlCallback<CConsoleFileDialog>(this, &CConsoleFileDialog::DirListCallback),
		_T("DirList"),
		_T("Directories"),
		m_Console), pos, size);

	maxSize.X = SHRT_MAX;
	maxSize.Y = SHRT_MAX;
	minSize.X = 10;
	minSize.Y = 10;
	prefSize.X = (_size.X - 4) / 2;
	prefSize.Y = _size.Y - 11;
	sizeCon.X = CConsoleLayoutItem::FillConstraint;
	sizeCon.Y = CConsoleLayoutItem::FillConstraint;
	align.X = CConsoleLayoutItem::LeftAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	pHBoxLayout->addControl(GetControl(_T("DirList")), maxSize, minSize, prefSize, sizeCon, align);

	pos.X = ((_size.X - 4) / 2) + 3;
	pos.Y = 3;
	size.X = (_size.X - 4) / 2;
	size.Y = _size.Y - 11;

	AddControl(OK_NEW_OPERATOR CConsoleFileListControl(
		1,
		startDirPattern,
		OK_NEW_OPERATOR CConsoleControlCallback<CConsoleFileDialog>(this, &CConsoleFileDialog::FileListCallback),
		_T("FileList"),
		_T("Files"),
		m_Console), pos, size);

	maxSize.X = SHRT_MAX;
	maxSize.Y = SHRT_MAX;
	minSize.X = 10;
	minSize.Y = 10;
	prefSize.X = (_size.X - 4) / 2;
	prefSize.Y = _size.Y - 11;
	sizeCon.X = CConsoleLayoutItem::FillConstraint;
	sizeCon.Y = CConsoleLayoutItem::FillConstraint;
	align.X = CConsoleLayoutItem::LeftAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	pHBoxLayout->addControl(GetControl(_T("FileList")), maxSize, minSize, prefSize, sizeCon, align);

	pVBoxLayout->addLayout(pHBoxLayout);
	pHBoxLayout = OK_NEW_OPERATOR CConsoleHBoxLayout(margins, spacing);

	pos.X = 1;
	pos.Y = _size.Y - 7;
	size.X = (_size.X - 4) / 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleLabelControl(
		_T("WildCardLabel"),
		_T("Suchmuster"),
		m_Console), pos, size);

	maxSize.X = SHRT_MAX;
	maxSize.Y = 1;
	minSize.X = 10;
	minSize.Y = 1;
	prefSize.X = (_size.X - 4) / 2;
	prefSize.Y = 1;
	sizeCon.X = CConsoleLayoutItem::FillConstraint;
	sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
	align.X = CConsoleLayoutItem::LeftAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	pHBoxLayout->addControl(GetControl(_T("WildCardLabel")), maxSize, minSize, prefSize, sizeCon, align);

	pos.X = ((_size.X - 4) / 2) + 3;
	pos.Y = _size.Y - 7;
	size.X = (_size.X - 4) / 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleComboControl(
		2,
		OK_NEW_OPERATOR CConsoleControlCallback<CConsoleFileDialog>(this, &CConsoleFileDialog::WildCardCallback),
		_T("WildCardText"),
		_T(""),
		m_Console), pos, size);

	maxSize.X = SHRT_MAX;
	maxSize.Y = 1;
	minSize.X = 10;
	minSize.Y = 1;
	prefSize.X = (_size.X - 4) / 2;
	prefSize.Y = 1;
	sizeCon.X = CConsoleLayoutItem::FillConstraint;
	sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
	align.X = CConsoleLayoutItem::LeftAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	pHBoxLayout->addControl(GetControl(_T("WildCardText")), maxSize, minSize, prefSize, sizeCon, align);

	pVBoxLayout->addLayout(pHBoxLayout);
	pHBoxLayout = OK_NEW_OPERATOR CConsoleHBoxLayout(margins, spacing);

	pos.X = 1;
	pos.Y = _size.Y - 5;
	size.X = (_size.X - 4) / 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleLabelControl(
		_T("FileNameLabel"),
		_T("Dateiname"),
		m_Console), pos, size);

	maxSize.X = SHRT_MAX;
	maxSize.Y = 1;
	minSize.X = 10;
	minSize.Y = 1;
	prefSize.X = (_size.X - 4) / 2;
	prefSize.Y = 1;
	sizeCon.X = CConsoleLayoutItem::FillConstraint;
	sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
	align.X = CConsoleLayoutItem::LeftAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	pHBoxLayout->addControl(GetControl(_T("FileNameLabel")), maxSize, minSize, prefSize, sizeCon, align);

	pos.X = ((_size.X - 4) / 2) + 3;
	pos.Y = _size.Y - 5;
	size.X = (_size.X - 4) / 2;
	size.Y = 1;

	AddControl(OK_NEW_OPERATOR CConsoleTextControl(
		MAX_PATH,
		false,
		3,
		OK_NEW_OPERATOR CConsoleControlCallback<CConsoleFileDialog>(this, 
		    &CConsoleFileDialog::FileNameCallback),
		_T("FileNameText"),
		_T(""),
		m_Console), pos, size);

	maxSize.X = SHRT_MAX;
	maxSize.Y = 1;
	minSize.X = 10;
	minSize.Y = 1;
	prefSize.X = (_size.X - 4) / 2;
	prefSize.Y = 1;
	sizeCon.X = CConsoleLayoutItem::FillConstraint;
	sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
	align.X = CConsoleLayoutItem::LeftAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	pHBoxLayout->addControl(GetControl(_T("FileNameText")), maxSize, minSize, prefSize, sizeCon, align);

	pVBoxLayout->addLayout(pHBoxLayout);
	pHBoxLayout = OK_NEW_OPERATOR CConsoleHBoxLayout(margins, spacing);

	pos.X = _size.X - 27;
	pos.Y = _size.Y - 3;
	size.X = 12;
	size.Y = 3;

	m_DefaultButton = OK_NEW_OPERATOR CConsoleButtonControl(
		4,
		nullptr,
		_T("OKButton"),
		_T("OK"),
		m_Console);
	AddControl(m_DefaultButton, pos, size);

	maxSize.X = 12;
	maxSize.Y = 3;
	minSize.X = 12;
	minSize.Y = 3;
	prefSize.X = 12;
	prefSize.Y = 3;
	sizeCon.X = CConsoleLayoutItem::FixedConstraint;
	sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
	align.X = CConsoleLayoutItem::RightAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	pHBoxLayout->addControl(GetControl(_T("OKButton")), maxSize, minSize, prefSize, sizeCon, align);

	pos.X = _size.X - 14;
	pos.Y = _size.Y - 3;
	size.X = 13;
	size.Y = 3;

	m_CancelButton = OK_NEW_OPERATOR CConsoleButtonControl(
		5,
		nullptr,
		_T("CancelButton"),
		_T("Abbrechen"),
		m_Console);
	AddControl(m_CancelButton, pos, size);

	maxSize.X = 13;
	maxSize.Y = 3;
	minSize.X = 13;
	minSize.Y = 3;
	prefSize.X = 13;
	prefSize.Y = 3;
	sizeCon.X = CConsoleLayoutItem::FixedConstraint;
	sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
	align.X = CConsoleLayoutItem::RightAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	pHBoxLayout->addControl(GetControl(_T("CancelButton")), maxSize, minSize, prefSize, sizeCon, align);

	pVBoxLayout->addLayout(pHBoxLayout);

	m_pLayout = pVBoxLayout;
}

void CConsoleFileDialog::DrawFileDialog(bool highlightFirst)
{
	CConsoleDirListControl* pList = 
		CastDynamicPtr(CConsoleDirListControl, GetControl(_T("DirList")));

	if ( pList )
	{
		CFilePath dirPath(m_Path);

		dirPath.set_Filename(_T("*"));
		pList->SetPath(dirPath);
		pList->ClearListe();
		pList->CreateListe();
		pList->DrawListControl(highlightFirst);
	}

	CConsoleLabelControl* pLabel = 
		CastDynamicPtr(CConsoleLabelControl, GetControl(_T("PathLabel")));

	if ( pLabel )
	{
		CStringBuffer title(m_Path.get_Path());
		SMALL_RECT rect = pLabel->GetClientArea();
		WULong sizeX = rect.Right - rect.Left + 1;

		if ( title.GetLength() > sizeX )
		{
			dword len = title.GetLength() - sizeX + 3;
			dword pos = (title.GetLength() - len) / 2;

			title.DeleteString(pos, len);
			title.InsertString(pos, _T("..."));
		}
		pLabel->SetTitle(title);
		pLabel->DrawLabel();
	}

	CConsoleFileListControl* pFileList = 
		CastDynamicPtr(CConsoleFileListControl, GetControl(_T("FileList")));

	if ( pFileList )
	{
		pFileList->SetPath(m_Path);
		pFileList->ClearListe();
		pFileList->CreateListe();
		pFileList->DrawListControl(false);
	}
	if ( m_FileDialogMode == FileDialogModeOpenFile )
	{
		CConsoleTextControl* pFileNameControl = 
			CastDynamicPtr(CConsoleTextControl, GetControl(_T("FileNameText")));

		if ( pFileNameControl && pFileList )
		{
			CStringBuffer item = pFileList->GetCurrentListItem();

			pFileNameControl->SetText(item);
		}
	}
}

void CConsoleFileDialog::DirListCallback(CConsoleControl* pControl, DWORD command)
{
	CConsoleDirListControl* pList = CastDynamicPtr(CConsoleDirListControl, pControl);

	if ( pList )
	{
		switch ( command )
		{
		case CConsoleControl::ListItemSelected:
			break;
		case CConsoleControl::ListItemClicked:
			{
				CStringBuffer dir(m_Path.get_Directory());

				dir.AppendString(pList->GetCurrentListItem());
				dir.AppendString(CDirectoryIterator::DefaultPathSeparatorString());
				m_Path.set_Directory(dir);
				m_Path.Normalize();

				DrawFileDialog();
				PostPaintEvent();
			}
			break;
		default:
			break;
		}
	}
}

void CConsoleFileDialog::FileListCallback(CConsoleControl* pControl, DWORD command)
{
	CConsoleFileListControl* pList = CastDynamicPtr(CConsoleFileListControl, pControl);

	if ( pList )
	{
		switch ( command )
		{
		case CConsoleControl::ListItemSelected:
			if ( m_FileDialogMode == FileDialogModeOpenFile )
			{
				CConsoleTextControl* pFileNameControl = 
					CastDynamicPtr(CConsoleTextControl, GetControl(_T("FileNameText")));

				if ( pFileNameControl )
				{
					CStringBuffer item = pList->GetCurrentListItem();

					pFileNameControl->SetText(item);
					pFileNameControl->PostPaintEvent();
				}
			}
			break;
		case CConsoleControl::ListItemClicked:
			if ( m_FileDialogMode == FileDialogModeOpenFile )
			{
				if ( m_DefaultButton )
					m_DefaultButton->InvokeControlCommand(CConsoleControl::ButtonClicked);
			}
			break;
		default:
			break;
		}
	}
}

void CConsoleFileDialog::FileNameCallback(CConsoleControl* pControl, DWORD command)
{
}

void CConsoleFileDialog::WildCardCallback(CConsoleControl* pControl, DWORD command)
{
	CConsoleComboControl* pComboControl = CastDynamicPtr(CConsoleComboControl, pControl);

	if ( !pComboControl )
		return;

	switch ( command )
	{
	case CConsoleControl::ComboItemSelected:
		{
			CStringBuffer currentItem = pComboControl->GetCurrentComboItem();

			if ( currentItem.IsEmpty() )
				currentItem.SetString(__FILE__LINE__ _T("*"));
			m_Path.set_Filename(currentItem);

			DrawFileDialog(false);
			PostPaintEvent();
		}
		break;
	default:
		break;
	}
}

void CConsoleFileDialog::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	m_hasBorder = true;
	m_BorderStyle = doubleborderstyle;
	m_hasTitle = true;
	m_TitleStyle = dialogtitlebarstyle;
	m_Resizeable = false;
	CConsoleDialog::Create(pos, size);

	LoadConfiguration();
	DrawFileDialog();
	HideCursor();
}

void CConsoleFileDialog::LoadConfiguration()
{
	CStringBuffer cfgKey;
	CStringBuffer cfgValue;

	cfgKey = theApp->config()->GetValue(_T("Application.Name"));
	cfgKey += _T("User.");
	switch ( m_FileDialogMode )
	{
	case FileDialogModeOpenFile:
		cfgKey += _T("FileDialogOpen.");
		break;
	case FileDialogModeCreateFile:
		cfgKey += _T("FileDialogCreate.");
		break;
	default:
		return;
	}
	cfgKey += m_Name;
	cfgKey += _T(".Path");

#ifdef OK_SYS_WINDOWS
	CStringBuffer path(__FILE__LINE__ _T("C:\\"));
#endif
#ifdef OK_SYS_UNIX
	CStringBuffer path(__FILE__LINE__ _T("/"));
#endif
	CConsoleComboControl* pControl = CastDynamicPtr(CConsoleComboControl, 
    	GetControl(_T("WildCardText")));

	if ( (pControl != nullptr) && (!(pControl->GetCurrentComboItem().IsEmpty())) )
		path.AppendString(pControl->GetCurrentComboItem());
	else
		path.AppendString(_T("*"));
	cfgValue = theApp->config()->GetValue(cfgKey);
	if ( !(cfgValue.IsEmpty()) )
		path = cfgValue;

	CFilePath fpath(path);
	CStringBuffer wildcard(fpath.get_Filename());

	if ( pControl )
	{
		pControl->SetCurrentComboItem(wildcard);
		pControl->DrawComboControl();
	}
	m_Path = path;
}

void CConsoleFileDialog::SaveConfiguration()
{
	CStringBuffer cfgKey;
	CStringBuffer cfgValue;

	cfgKey = theApp->config()->GetValue(_T("Application.Name"));
	cfgKey += _T("User.");
	switch ( m_FileDialogMode )
	{
	case FileDialogModeOpenFile:
		cfgKey += _T("FileDialogOpen.");
		break;
	case FileDialogModeCreateFile:
		cfgKey += _T("FileDialogCreate.");
		break;
	default:
		return;
	}
	cfgKey += m_Name;
	cfgKey += _T(".Path");

	theApp->config()->SetValue(cfgKey, m_Path.get_Path());
}
