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

#include "DataDoubleLinkedList.h"

#include "ConSources.h"
#include "ConsoleWindow.h"
#include "ConsoleMenuItem.h"

class CConsolePopupMenu;
class CONSOURCES_API CAbstractConsoleMenuNavigationCallback: public CCppObject
{
public:
	enum TNavigation {
		NavigationRight,
		NavigationLeft
	};

public:
	virtual ~CAbstractConsoleMenuNavigationCallback();

	virtual void invoke(CConsolePopupMenu* pMenu, TNavigation command) const = 0;	

protected:
	CAbstractConsoleMenuNavigationCallback();
	CAbstractConsoleMenuNavigationCallback(const CAbstractConsoleMenuNavigationCallback&);
};

template <class C>
class CConsoleMenuNavigationCallback: public CAbstractConsoleMenuNavigationCallback
{
public:

	typedef void (C::*Callback)(CConsolePopupMenu* pMenu, TNavigation command);

	CConsoleMenuNavigationCallback(C* pObject, Callback method):
		_pObject(pObject),
		_method(method)
	{
	}
	
	CConsoleMenuNavigationCallback(const CConsoleMenuNavigationCallback& cb):
		CAbstractConsoleMenuNavigationCallback(cb),
		_pObject(cb._pObject),
		_method(cb._method)
	{
	}
	
	virtual ~CConsoleMenuNavigationCallback()
	{
	}
	
	CConsoleMenuNavigationCallback& operator = (const CConsoleMenuNavigationCallback& cb)
	{
		if (&cb != this)
		{
			this->_pObject = cb._pObject;
			this->_method  = cb._method;
		}
		return *this;
	}
	
	virtual void invoke(CConsolePopupMenu* pMenu, TNavigation command) const
	{
		(_pObject->*_method)(pMenu, command);
	}
	
private:
	CConsoleMenuNavigationCallback();
	
	C* _pObject;
	Callback _method;
};

class CConsole;
class CConsoleMenuItem;
class CConsoleMainMenu;
class CONSOURCES_API CConsolePopupMenu: public CConsoleWindow
{
public:
	CConsolePopupMenu(CConstPointer name, CConsole* pConsole);
	CConsolePopupMenu(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsolePopupMenu(CConsoleMainMenu* mainMenu, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsolePopupMenu(void);

	void ClearMenus();
	void AddMenuItem(CConsoleMenuItem* pMenuItem);

	bool TestKeyCode(WORD vkey, DWORD kmod);

	void DrawMenuItemList();

	virtual void Create(COORD pos, COORD size);
	virtual void Paint(COORD bufferSize, PCHAR_INFO buffer);

	virtual bool KeyDown(WORD virtualKeyCode, DWORD controlKeyState);
	virtual bool KeyPress(TCHAR key, DWORD controlKeyState);
	virtual bool KeyUp(WORD virtualKeyCode, DWORD controlKeyState);

	virtual bool LeftMouseDown(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDown(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseDownMove(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDownMove(COORD mousePos, DWORD controlKeyState);

protected:
	typedef CDataDoubleLinkedListT<CConsoleMenuItem> CConsoleMenuItemList;

	CConsoleMenuItemList m_MenuItemList;
	CConsoleMenuItemList::Iterator m_MenuCurrent;
	CAbstractConsoleMenuNavigationCallback* m_Callback;

	void DoMenuItem(CConsoleMenuItem* pMenuItem);

private:
	CConsolePopupMenu();
	CConsolePopupMenu(ConstRef(CConsolePopupMenu));
};

