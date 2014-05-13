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
#pragma once

#include "OpenSSL.h"

class OPENSSL_API COpenSSLClass : public CCppObject
{
public:
	COpenSSLClass(ConstPointer raw = NULL);
	virtual ~COpenSSLClass();

	__inline ConstPointer get_raw() const { return _raw; }
	__inline qword key() const { return CastAny(qword, _raw); }

	static Ptr(COpenSSLClass) find_obj(ConstPointer raw);

	void insert_obj();
	void remove_obj();

protected:
	Pointer _raw;

private:
	static CDataVectorT<COpenSSLClass> _objs;
};

