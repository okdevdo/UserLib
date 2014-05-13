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
#pragma once

#include "ConSources.h"
#include "ConsolePopupMenu.h"

class CConsole;
class CConsoleMenuItem;
class CConsoleMainMenu;
class CONSOURCES_API CConsoleEditMenu: public CConsolePopupMenu
{
public:
	CConsoleEditMenu(CConstPointer name, CConsole* pConsole);
	CConsoleEditMenu(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleEditMenu(CConsoleMainMenu* mainMenu, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleEditMenu(void);

	void Initialize();

	virtual void Create(COORD pos, COORD size);

protected:
	void EditUndoMenuCallback(CConsoleMenuItem* pMenu);
	void EditRedoMenuCallback(CConsoleMenuItem* pMenu);
	void EditCutMenuCallback(CConsoleMenuItem* pMenu);
	void EditCopyMenuCallback(CConsoleMenuItem* pMenu);
	void EditPasteMenuCallback(CConsoleMenuItem* pMenu);
	void EditDeleteMenuCallback(CConsoleMenuItem* pMenu);

	CConsoleWindow* GetCurrentWindow();
};

