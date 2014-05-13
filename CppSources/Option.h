/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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

#include "CppSources.h"
#include "BaseException.h"

class CPPSOURCES_API CAbstractOptionCallback: public CCppObject
{
public:
	virtual ~CAbstractOptionCallback();

	virtual void invoke(const CStringLiteral& name, const CStringLiteral& value) const = 0;	
	virtual CAbstractOptionCallback* clone() const = 0;

protected:
	CAbstractOptionCallback();
	CAbstractOptionCallback(const CAbstractOptionCallback&);
};


template <class C>
class COptionCallback: public CAbstractOptionCallback
{
public:
	typedef void (C::*Callback)(const CStringLiteral& name, const CStringLiteral& value);

	COptionCallback(C* pObject, Callback method):
		_pObject(pObject),
		_method(method)
	{
	}
	
	COptionCallback(const COptionCallback& cb):
		CAbstractOptionCallback(cb),
		_pObject(cb._pObject),
		_method(cb._method)
	{
	}
	
	virtual ~COptionCallback()
	{
	}
	
	COptionCallback& operator = (const COptionCallback& cb)
	{
		if (&cb != this)
		{
			this->_pObject = cb._pObject;
			this->_method  = cb._method;
		}
		return *this;
	}
	
	void invoke(const CStringLiteral& name, const CStringLiteral& value) const
	{
		(_pObject->*_method)(name, value);
	}
	
	CAbstractOptionCallback* clone() const
	{
		return OK_NEW_OPERATOR COptionCallback(_pObject, _method);
	}
	
private:
	COptionCallback();
	
	C* _pObject;
	Callback _method;
};

class CPPSOURCES_API COption: public CCppObject
{
public:
	COption(CStringLiteral _fullname);
	COption(CStringLiteral _fullname, CStringLiteral _shortname);
	COption(CStringLiteral _fullname, CStringLiteral _shortname, CStringLiteral _category, CStringLiteral _description, WBool _required = false);
	COption(ConstRef(COption) _copy);
	~COption(void);

	Ref(COption) shortName(CStringLiteral _shortname);
	Ref(COption) category(CStringLiteral _category);
	Ref(COption) description(CStringLiteral _description);
	Ref(COption) required(WBool _required);
	Ref(COption) repeatable(WBool _repeatable);
	Ref(COption) argument(CStringLiteral _name, WBool _required = true, WInt _position = 0);
	Ref(COption) noArgument();
	Ref(COption) group(CStringLiteral _group);
	Ref(COption) callBack(CAbstractOptionCallback& _callback);

	ConstRef(CStringBuffer) fullname() const;
	ConstRef(CStringBuffer) shortName() const;
	ConstRef(CStringBuffer) category() const;
	ConstRef(CStringBuffer) description() const;
	WBool required() const;
	WBool repeatable() const;
	ConstRef(CStringBuffer) argument() const;
	WBool hasArgument() const;
	WBool argumentRequired() const;
	WInt argumentPosition() const;
	ConstRef(CStringBuffer) group() const;
	CAbstractOptionCallback const* callBack() const;

	WBool matchesShort(CStringLiteral option);
	WBool matchesFull(CStringLiteral option);
	WBool matchesPartial(CStringLiteral option);

	COption* clone();

private:
	COption(void);

	CStringBuffer m_fullname;
	CStringBuffer m_shortname;
	CStringBuffer m_category;
	CStringBuffer m_description;
	WBool m_required;
	WBool m_repeatable;
	CStringBuffer m_argname;
	WBool m_argrequired;
	WInt m_argposition;
	CStringBuffer m_group;
	CAbstractOptionCallback* m_pCallback;
};

DECL_EXCEPTION(CPPSOURCES_API, COptionException, CBaseException)
DECL_EXCEPTION(CPPSOURCES_API, COptionMissingOptionException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionMissingArgumentException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionUnexpectedArgumentException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionIllegalArgumentException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionUnknownException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionAmbiguityException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionNotRepeatableException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionRequiredMissingException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionGroupException, COptionException)

