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

static void __stdcall COpenSSLClassVectorDeleteFunc(ConstPointer data, Pointer context)
{
	COpenSSLClass* pInfo = CastAnyPtr(COpenSSLClass, CastMutable(Pointer, data));

	pInfo->release();
}

static sword __stdcall COpenSSLClassVectorSearchAndSortFunc(ConstPointer pa, ConstPointer pb)
{
	COpenSSLClass* ppa = CastAnyPtr(COpenSSLClass, CastMutable(Pointer, pa));
	COpenSSLClass* ppb = CastAnyPtr(COpenSSLClass, CastMutable(Pointer, pb));

	if (ppa->get_raw() < ppb->get_raw())
		return -1;
	if (ppa->get_raw() == ppb->get_raw())
		return 0;
	return 1;
}

static sword __stdcall COpenSSLClassVectorSearchAndSortFunc1(ConstPointer pa, ConstPointer pb)
{
	COpenSSLClass* ppa = CastAnyPtr(COpenSSLClass, CastMutable(Pointer, pa));

	if (ppa->get_raw() < pb)
		return -1;
	if (ppa->get_raw() == pb)
		return 0;
	return 1;
}

CDataVectorT<COpenSSLClass> COpenSSLClass::_objs(__FILE__LINE__ 16, 16, COpenSSLClassVectorDeleteFunc);

COpenSSLClass::COpenSSLClass(ConstPointer raw) : _raw(CastMutable(Pointer, raw))
{
}

COpenSSLClass::~COpenSSLClass()
{
}

Ptr(COpenSSLClass) COpenSSLClass::find_obj(ConstPointer raw)
{
	CDataVectorT<COpenSSLClass>::Iterator it = _objs.FindSorted(CastAnyConstPtr(COpenSSLClass, raw), COpenSSLClassVectorSearchAndSortFunc1);

	if (it && *it && (COpenSSLClassVectorSearchAndSortFunc1(*it, raw) == 0))
		return *it;
	return NULL;
}

void COpenSSLClass::insert_obj()
{
	CDataVectorT<COpenSSLClass>::Iterator it = _objs.FindSorted(this, COpenSSLClassVectorSearchAndSortFunc);

	if (it && *it && (COpenSSLClassVectorSearchAndSortFunc(*it, this) == 0))
		return;
	_objs.InsertSorted(this, COpenSSLClassVectorSearchAndSortFunc);
}

void COpenSSLClass::remove_obj()
{
	CDataVectorT<COpenSSLClass>::Iterator it = _objs.FindSorted(this, COpenSSLClassVectorSearchAndSortFunc);

	if (it && *it && (COpenSSLClassVectorSearchAndSortFunc(*it, this) == 0))
		_objs.Remove(it, COpenSSLClassVectorDeleteFunc);
}
