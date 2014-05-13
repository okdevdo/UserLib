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
#include "ConsoleEditMenu.h"
#include "Console.h"
#include "ConsoleMenuItem.h"
#include "ConsoleDialog.h"

CConsoleEditMenu::CConsoleEditMenu(CConstPointer name, CConsole* pConsole):
    CConsolePopupMenu(name, pConsole)
{
}

CConsoleEditMenu::CConsoleEditMenu(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsolePopupMenu(name, title, pConsole)
{
}

CConsoleEditMenu::CConsoleEditMenu(CConsoleMainMenu* mainMenu, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsolePopupMenu(mainMenu, name, title, pConsole)
{
}

CConsoleEditMenu::~CConsoleEditMenu(void)
{
	ClearMenus();
}

CConsoleWindow* CConsoleEditMenu::GetCurrentWindow()
{
	if ( !m_Console )
		return NULL;

	CConsoleWindow* result = NULL;

	result = m_Console->GetConsolePopupControl();
	if ( NotPtrCheck(result) )
		return result;
	result = CastDynamicPtr(CConsoleWindow, m_Console->GetCurrentConsoleModalDialog());
	if ( NotPtrCheck(result) )
		return result;
	result = m_Console->GetTopMostWindow();
	if ( NotPtrCheck(result) )
		return result;
	return NULL;
}

void CConsoleEditMenu::EditUndoMenuCallback(CConsoleMenuItem* pMenu)
{
	CConsoleWindow* pW = GetCurrentWindow();

	if ( NotPtrCheck(pW) )
		pW->Undo();
}

void CConsoleEditMenu::EditRedoMenuCallback(CConsoleMenuItem* pMenu)
{
	CConsoleWindow* pW = GetCurrentWindow();

	if ( NotPtrCheck(pW) )
		pW->Redo();
}

void CConsoleEditMenu::EditCutMenuCallback(CConsoleMenuItem* pMenu)
{
	CConsoleWindow* pW = GetCurrentWindow();

	if ( NotPtrCheck(pW) )
		pW->Cut();
}

void CConsoleEditMenu::EditCopyMenuCallback(CConsoleMenuItem* pMenu)
{
	CConsoleWindow* pW = GetCurrentWindow();

	if ( NotPtrCheck(pW) )
		pW->Copy();
}

void CConsoleEditMenu::EditPasteMenuCallback(CConsoleMenuItem* pMenu)
{
	CConsoleWindow* pW = GetCurrentWindow();

	if ( NotPtrCheck(pW) )
		pW->Paste();
}

void CConsoleEditMenu::EditDeleteMenuCallback(CConsoleMenuItem* pMenu)
{
	CConsoleWindow* pW = GetCurrentWindow();

	if ( NotPtrCheck(pW) )
		pW->Delete();
}

void CConsoleEditMenu::Initialize()
{
	if ( !m_Console )
		return;

	AddMenuItem(OK_NEW_OPERATOR CConsoleMenuItem(
#ifdef OK_SYS_WINDOWS
		0x5A, // CTRL + Z
#endif
#ifdef OK_SYS_UNIX
		0x55, // CTRL + U
#endif
		KeyStateCtrlMask,
		OK_NEW_OPERATOR CConsoleMenuItemCallback<CConsoleEditMenu>(this, &CConsoleEditMenu::EditUndoMenuCallback),
		_T("EditMenuUndo"),
#ifdef OK_SYS_WINDOWS
		_T("R\xFC") _T("ckg\xE4ngig"),
#endif
#ifdef OK_SYS_UNIX
		_T("Rueckgaengig"),
#endif
		m_Console));

	AddMenuItem(OK_NEW_OPERATOR CConsoleMenuItem(
		0x59, // CTRL + Y
		KeyStateCtrlMask,
		OK_NEW_OPERATOR CConsoleMenuItemCallback<CConsoleEditMenu>(this, &CConsoleEditMenu::EditRedoMenuCallback),
		_T("EditMenuRedo"),
		_T("Wiederholen"),
		m_Console));

	AddMenuItem(OK_NEW_OPERATOR CConsoleMenuItem(
		0x58,
		KeyStateCtrlMask,
		OK_NEW_OPERATOR CConsoleMenuItemCallback<CConsoleEditMenu>(this, &CConsoleEditMenu::EditCutMenuCallback),
		_T("EditMenuCut"),
		_T("Ausschneiden"),
		m_Console));

	AddMenuItem(OK_NEW_OPERATOR CConsoleMenuItem(
		0x43, // CTRL + C
		KeyStateCtrlMask,
		OK_NEW_OPERATOR CConsoleMenuItemCallback<CConsoleEditMenu>(this, &CConsoleEditMenu::EditCopyMenuCallback),
		_T("EditMenuCopy"),
		_T("Kopieren"),
		m_Console));

	AddMenuItem(OK_NEW_OPERATOR CConsoleMenuItem(
		0x56, // CTRL + V
		KeyStateCtrlMask,
		OK_NEW_OPERATOR CConsoleMenuItemCallback<CConsoleEditMenu>(this, &CConsoleEditMenu::EditPasteMenuCallback),
		_T("EditMenuPaste"),
#ifdef OK_SYS_WINDOWS
		_T("Einf\xFCgen"),
#endif
#ifdef OK_SYS_UNIX
		_T("Einfuegen"),
#endif
		m_Console));

	AddMenuItem(OK_NEW_OPERATOR CConsoleMenuItem(
		OK_NEW_OPERATOR CConsoleMenuItemCallback<CConsoleEditMenu>(this, &CConsoleEditMenu::EditDeleteMenuCallback),
		_T("EditMenuDelete"),
#ifdef OK_SYS_WINDOWS
		_T("L\xF6schen"),
#endif
#ifdef OK_SYS_UNIX
		_T("Loeschen"),
#endif
		m_Console));
}

void CConsoleEditMenu::Create(COORD pos, COORD size)
{
	Initialize();
	CConsolePopupMenu::Create(pos, size);
}
