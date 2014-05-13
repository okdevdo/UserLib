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

class CConsoleControl;
class CONSOURCES_API CAbstractConsoleControlCallback: public CCppObject
{
public:
	virtual ~CAbstractConsoleControlCallback();

	virtual void invoke(CConsoleControl* pControl, DWORD command) const = 0;	

protected:
	CAbstractConsoleControlCallback();
	CAbstractConsoleControlCallback(const CAbstractConsoleControlCallback&);
};


template <class C>
class CConsoleControlCallback: public CAbstractConsoleControlCallback
{
public:
	typedef void (C::*Callback)(CConsoleControl* pControl, DWORD command);

	CConsoleControlCallback(C* pObject, Callback method):
		_pObject(pObject),
		_method(method)
	{
	}
	
	CConsoleControlCallback(const CConsoleControlCallback& cb):
		CAbstractConsoleControlCallback(cb),
		_pObject(cb._pObject),
		_method(cb._method)
	{
	}
	
	virtual ~CConsoleControlCallback()
	{
	}
	
	CConsoleControlCallback& operator = (const CConsoleControlCallback& cb)
	{
		if (&cb != this)
		{
			this->_pObject = cb._pObject;
			this->_method  = cb._method;
		}
		return *this;
	}
	
	virtual void invoke(CConsoleControl* pControl, DWORD command) const
	{
		(_pObject->*_method)(pControl, command);
	}
	
private:
	CConsoleControlCallback();
	
	C* _pObject;
	Callback _method;
};

class CONSOURCES_API CConsoleControl: public CConsoleWindow
{
public:
	CConsoleControl(CConstPointer name, CConsole* pConsole);
	CConsoleControl(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleControl(void);

	inline CAbstractConsoleControlCallback* GetCallback() { return m_Callback; }
	inline void SetCallback(CAbstractConsoleControlCallback* pCallback) { m_Callback = pCallback; }

	inline bool IsTabStop() { return m_TabStop; }
	inline word GetTabOrder() { return m_TabOrder; }

	enum ControlCommand
	{
		ButtonClicked,
		ListItemClicked,
		ListItemSelected,
		ComboItemSelected
	};

	void InvokeControlCommand(DWORD command);

	virtual void Create(COORD pos, COORD size);

protected:
	CAbstractConsoleControlCallback* m_Callback;
	bool m_TabStop;
	word m_TabOrder;

private:
	CConsoleControl();
	CConsoleControl(ConstRef(CConsoleControl));

};

