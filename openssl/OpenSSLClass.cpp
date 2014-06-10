/******************************************************************************
    
	This file is part of openssl, which is part of UserLib.

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
#include "OS_PCH.H"
#include "OpenSSLClass.h"

COpenSSLClass::COpenSSLClasses COpenSSLClass::_objs(__FILE__LINE__ 16, 16);

COpenSSLClass::COpenSSLClass(ConstPointer raw) : _raw(CastMutable(Pointer, raw))
{
}

COpenSSLClass::~COpenSSLClass()
{
}

Ptr(COpenSSLClass) COpenSSLClass::find_obj(ConstPointer raw)
{
	COpenSSLClass cmp(raw);
	COpenSSLClasses::Iterator it = _objs.FindSorted(&cmp);

	if (it)
		return *it;
	return nullptr;
}

void COpenSSLClass::insert_obj()
{
	COpenSSLClasses::Iterator it = _objs.FindSorted(this);

	if (it)
		return;
	_objs.InsertSorted(this);
}

void COpenSSLClass::remove_obj()
{
	COpenSSLClasses::Iterator it = _objs.FindSorted(this);

	if (it)
		_objs.Remove(it);
}
