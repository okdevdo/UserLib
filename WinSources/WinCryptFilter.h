/******************************************************************************
    
	This file is part of WinSources, which is part of UserLib.

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

#include "WinSources.h"
#include "WinException.h"
#include "Filter.h"
#include "DataVector.h"

class WINSOURCES_API CWinCryptEncryptFilter : public CFilter
{
public:
	CWinCryptEncryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CWinCryptEncryptFilter(void);

	__inline ConstRef(CStringBuffer) get_Password() const { return _passwd;  }
	__inline void set_Password(ConstRef(CStringBuffer) p) { _passwd = p; }
	__inline void set_Password(CConstPointer p) { _passwd.SetString(__FILE__LINE__ p); }

	__inline ConstRef(CStringBuffer) get_Algorithm() const { return _algo; }
	void set_Algorithm(int ix);

	static ConstRef(CDataVectorT<CStringBuffer>) get_Algorithms();

	virtual dword do_filter();

protected:
	CStringBuffer _passwd;
	CStringBuffer _algo;
	static CDataVectorT<CStringBuffer> _algos;

private:
	CWinCryptEncryptFilter(void);

	dword _algo1();
	dword _algo2();
	dword _algo3();
	dword _algo4();
};

class WINSOURCES_API CWinCryptDecryptFilter : public CFilter
{
public:
	CWinCryptDecryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CWinCryptDecryptFilter(void);

	__inline ConstRef(CStringBuffer) get_Password() const { return _passwd;  }
	__inline void set_Password(ConstRef(CStringBuffer) p) { _passwd = p; }
	__inline void set_Password(CConstPointer p) { _passwd.SetString(__FILE__LINE__ p); }

	__inline ConstRef(CStringBuffer) get_Algorithm() const { return _algo; }
	void set_Algorithm(int ix);

	static ConstRef(CDataVectorT<CStringBuffer>) get_Algorithms();

	virtual dword do_filter();

protected:
	CStringBuffer _passwd;
	CStringBuffer _algo;
	static CDataVectorT<CStringBuffer> _algos;

private:
	CWinCryptDecryptFilter(void);

	dword _algo1();
	dword _algo2();
	dword _algo3();
	dword _algo4();
};

DECL_WINEXCEPTION(WINSOURCES_API, CWinCryptException, CWinException)
