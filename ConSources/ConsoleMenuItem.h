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
#include "ConsoleWindow.h"

class CConsoleMenuItem;
class CONSOURCES_API CAbstractConsoleMenuItemCallback: public CCppObject
{
public:
	virtual ~CAbstractConsoleMenuItemCallback();

	virtual void invoke(CConsoleMenuItem* pMenuItem) const = 0;	

protected:
	CAbstractConsoleMenuItemCallback();
	CAbstractConsoleMenuItemCallback(const CAbstractConsoleMenuItemCallback&);
};


template <class C>
class CConsoleMenuItemCallback: public CAbstractConsoleMenuItemCallback
{
public:
	typedef void (C::*Callback)(CConsoleMenuItem* pMenuItem);

	CConsoleMenuItemCallback(C* pObject, Callback method):
		_pObject(pObject),
		_method(method)
	{
	}
	
	CConsoleMenuItemCallback(const CConsoleMenuItemCallback& cb):
		CAbstractConsoleMenuItemCallback(cb),
		_pObject(cb._pObject),
		_method(cb._method)
	{
	}
	
	virtual ~CConsoleMenuItemCallback()
	{
	}
	
	CConsoleMenuItemCallback& operator = (const CConsoleMenuItemCallback& cb)
	{
		if (&cb != this)
		{
			this->_pObject = cb._pObject;
			this->_method  = cb._method;
		}
		return *this;
	}
	
	virtual void invoke(CConsoleMenuItem* pMenuItem) const
	{
		(_pObject->*_method)(pMenuItem);
	}
	
private:
	CConsoleMenuItemCallback();
	
	C* _pObject;
	Callback _method;
};

class CConsole;
class CConsolePopupMenu;
class CONSOURCES_API CConsoleMenuItem: public CConsoleWindow
{
public:
	CConsoleMenuItem(CConstPointer name, CConsole* pConsole);
	CConsoleMenuItem(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleMenuItem(CConsolePopupMenu* subMenu, CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleMenuItem(CAbstractConsoleMenuItemCallback* pCallback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleMenuItem(WORD vkey, DWORD kmod, CAbstractConsoleMenuItemCallback* pCallback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleMenuItem(void);

	void InvokeMenuItemCommand();
	bool TestKeyCode(WORD vkey, DWORD kmod);
	void DrawMenuItem(bool bHighlight = false);
	dword GetTitleSize() const;

	__inline bool HasSubMenu() { return m_SubMenu != NULL; }
	__inline CConsolePopupMenu* GetSubMenu() { return m_SubMenu; }

	virtual void Create(COORD pos, COORD size);
	virtual void Resize();

protected:
	CAbstractConsoleMenuItemCallback* m_Callback;
	WORD m_VirtualKeyCode;
	DWORD m_KeyModifiers;
	CConsolePopupMenu* m_SubMenu;

	CStringBuffer CreateMenuTitle() const;

private:
	CConsoleMenuItem();
	CConsoleMenuItem(ConstRef(CConsoleMenuItem));

};

