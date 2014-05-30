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
#include "CPPS_PCH.H"
#include "Option.h"

IMPL_EXCEPTION(COptionException, CBaseException)
IMPL_EXCEPTION(COptionMissingOptionException, COptionException)
IMPL_EXCEPTION(COptionMissingArgumentException, COptionException)
IMPL_EXCEPTION(COptionUnexpectedArgumentException, COptionException)
IMPL_EXCEPTION(COptionIllegalArgumentException, COptionException)
IMPL_EXCEPTION(COptionUnknownException, COptionException)
IMPL_EXCEPTION(COptionAmbiguityException, COptionException)
IMPL_EXCEPTION(COptionNotRepeatableException, COptionException)
IMPL_EXCEPTION(COptionRequiredMissingException, COptionException)
IMPL_EXCEPTION(COptionGroupException, COptionException)

CAbstractOptionCallback::CAbstractOptionCallback()
{
}

CAbstractOptionCallback::CAbstractOptionCallback(const CAbstractOptionCallback&)
{
}

CAbstractOptionCallback::~CAbstractOptionCallback()
{
}

COption::COption(void):
	m_fullname(),
	m_shortname(),
	m_category(),
	m_description(),
	m_required(false),
	m_repeatable(false),
	m_argname(),
	m_argrequired(false),
	m_argposition(0),
	m_group(),
	m_pCallback(0)
{
}

COption::COption(CStringLiteral _fullname):
	m_fullname(__FILE__LINE__ _fullname),
	m_shortname(),
	m_category(),
	m_description(),
	m_required(false),
	m_repeatable(false),
	m_argname(),
	m_argrequired(false),
	m_argposition(0),
	m_group(),
	m_pCallback(0)
{
}

COption::COption(CStringLiteral _fullname, CStringLiteral _shortname):
	m_fullname(__FILE__LINE__ _fullname),
	m_shortname(__FILE__LINE__ _shortname),
	m_category(),
	m_description(),
	m_required(false),
	m_repeatable(false),
	m_argname(),
	m_argrequired(false),
	m_argposition(0),
	m_group(),
	m_pCallback(0)
{
}

COption::COption(CStringLiteral _fullname, CStringLiteral _shortname, CStringLiteral _category, CStringLiteral _description, WBool _required):
	m_fullname(__FILE__LINE__ _fullname),
	m_shortname(__FILE__LINE__ _shortname),
	m_category(__FILE__LINE__ _category),
	m_description(__FILE__LINE__ _description),
	m_required(_required),
	m_repeatable(false),
	m_argname(),
	m_argrequired(false),
	m_argposition(0),
	m_group(),
	m_pCallback(0)
{
}

COption::COption(const Ref(COption) _copy):
    m_fullname(_copy.m_fullname),
	m_shortname(_copy.m_shortname),
	m_category(_copy.m_category),
	m_description(_copy.m_description),
	m_required(_copy.m_required),
	m_repeatable(_copy.m_repeatable),
	m_argname(_copy.m_argname),
	m_argrequired(_copy.m_argrequired),
	m_argposition(_copy.m_argposition),
	m_group(_copy.m_group),
	m_pCallback((0 == _copy.m_pCallback)?0:(_copy.m_pCallback->clone()))
{
}

COption::~COption(void)
{
	if ( m_pCallback )
		delete m_pCallback;
}

Ref(COption) COption::shortName(CStringLiteral _shortname)
{
	m_shortname.SetString(__FILE__LINE__ _shortname);
	return *this;
}

Ref(COption) COption::category(CStringLiteral _category)
{
	m_category.SetString(__FILE__LINE__ _category);
	return *this;
}

Ref(COption) COption::description(CStringLiteral _description)
{
	m_description.SetString(__FILE__LINE__ _description);
	return *this;
}

Ref(COption) COption::required(WBool _required)
{
	m_required = _required;
	return *this;
}

Ref(COption) COption::repeatable(WBool _repeatable)
{
	m_repeatable = _repeatable;
	return *this;
}

Ref(COption) COption::argument(CStringLiteral _name, WBool _required, WInt _position)
{
	m_argname.SetString(__FILE__LINE__ _name);
	m_argrequired = _required;
	m_argposition = _position;
	return *this;
}

Ref(COption) COption::noArgument()
{
	m_argname.Clear();
	m_argrequired = false;
	m_argposition = 0;
	return *this;
}

Ref(COption) COption::group(CStringLiteral _group)
{
	m_group.SetString(__FILE__LINE__ _group);
	return *this;
}

Ref(COption) COption::callBack(CAbstractOptionCallback& _callback)
{
	if ( m_pCallback )
		delete m_pCallback;
	m_pCallback = _callback.clone();
	return *this;
}

ConstRef(CStringBuffer) COption::fullname() const
{
	return m_fullname;
}

ConstRef(CStringBuffer) COption::get_Name() const
{
	return m_fullname;
}

ConstRef(CStringBuffer) COption::shortName() const
{
	return m_shortname;
}

ConstRef(CStringBuffer) COption::category() const
{
	return m_category;
}

ConstRef(CStringBuffer) COption::description() const
{
	return m_description;
}

WBool COption::required() const
{
	return m_required;
}

WBool COption::repeatable() const
{
	return m_repeatable;
}

ConstRef(CStringBuffer) COption::argument() const
{
	return m_argname;
}

WBool COption::hasArgument() const
{
	return !(m_argname.IsEmpty());
}

WBool COption::argumentRequired() const
{
	return m_argrequired;
}

WInt COption::argumentPosition() const
{
	return m_argposition;
}

ConstRef(CStringBuffer) COption::group() const
{
	return m_group;
}

CAbstractOptionCallback const* COption::callBack() const
{
	return m_pCallback;
}

WBool COption::matchesShort(CStringLiteral option)
{
	dword len = m_shortname.GetLength();
	dword len1;
	dword len2 = option.GetLength();
	CStringConstIterator it(option.Begin());

#ifdef OK_SYS_WINDOWS
	it.FirstOf(_T(":="));
#endif
#ifdef OK_SYS_UNIX
	it.Find(_T('='));
#endif
	len1 = it.GetLength();
	if ( len1 < len2 )
		len2 -= len1;
	if ( len2 != len )
		return false;
	return (!(m_shortname.IsEmpty())) && (m_shortname.Compare(option, len, CStringLiteral::cLimited) == 0);
}

WBool COption::matchesFull(CStringLiteral option)
{
	dword len = m_fullname.GetLength();

	return (!(m_fullname.IsEmpty())) && (m_fullname.Compare(option, len, CStringLiteral::cLimited | CStringLiteral::cIgnoreCase) == 0);
}

WBool COption::matchesPartial(CStringLiteral option)
{
	dword len = option.GetLength();
	dword len1;
	CStringConstIterator it(option.Begin());

#ifdef OK_SYS_WINDOWS
	it.FirstOf(_T(":="));
#endif
#ifdef OK_SYS_UNIX
	it.Find(_T('='));
#endif
	len1 = it.GetLength();
	if ( len1 < len )
		len -= len1;
	return (!(m_fullname.IsEmpty())) && (m_fullname.Compare(option, len, CStringLiteral::cLimited | CStringLiteral::cIgnoreCase) == 0);
}

COption* COption::clone()
{
	return OK_NEW_OPERATOR COption(*this);
}
