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
/**
 *  \file CryptFilter.h
 *  \brief Encryption and decryption filters.
 */
#pragma once

#include "CppSources.h"
#include "BaseException.h"
#include "Filter.h"
#include "DataVector.h"

class CPPSOURCES_API CCryptFilter : public CFilter
{
public:
	CCryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CCryptFilter(void);

	__inline void set_key(BPointer key, dword keysize) { _key.set_Buffer(__FILE__LINE__ key, keysize); }
	__inline void set_key(ConstRef(CByteBuffer) key) { _key = key; }

	__inline void set_iv(BPointer iv, dword blksize) { _iv.set_Buffer(__FILE__LINE__ iv, blksize); }
	__inline void set_iv(ConstRef(CByteBuffer) iv) { _iv = iv; }

	__inline word get_Algorithm() const { return _algo; }
	void set_Algorithm(word ix);

	static ConstRef(CDataVectorT<CStringBuffer>) get_Algorithms();
	static word get_AlgorithmCount();
	static CStringBuffer get_Algorithm(word ix);
	static sword find_Algorithm(CConstPointer name, sword start = -1);

	virtual dword do_filter();

protected:
	word _algo;
	CByteBuffer _key;
	CByteBuffer _iv;
	static CDataVectorT<CStringBuffer> _algos;
};

class CPPSOURCES_API CCryptEncryptFilter : public CCryptFilter
{
public:
	CCryptEncryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CCryptEncryptFilter(void);

	virtual dword do_filter();

private:
	CCryptEncryptFilter(void);

	dword _algo_DES_ECB();
	dword _algo_DES_CBC();
	dword _algo_DES_CFB();
	dword _algo_DES_OFB();
	dword _algo_DES3_ECB();
	dword _algo_AES_ECB();
	dword _algo_AES_CBC();
	dword _algo_AES_CFB();
	dword _algo_AES_OFB();
	dword _algo_AES_CTR();
	dword _algo_AES_XTS();
	dword _algo_Blowfish_ECB();
	dword _algo_Twofish_ECB();
};

class CPPSOURCES_API CCryptDecryptFilter : public CCryptFilter
{
public:
	CCryptDecryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CCryptDecryptFilter(void);

	virtual dword do_filter();

private:
	CCryptDecryptFilter(void);

	dword _algo_DES_ECB();
	dword _algo_DES_CBC();
	dword _algo_DES_CFB();
	dword _algo_DES_OFB();
	dword _algo_DES3_ECB();
	dword _algo_AES_ECB();
	dword _algo_AES_CBC();
	dword _algo_AES_CFB();
	dword _algo_AES_OFB();
	dword _algo_AES_CTR();
	dword _algo_AES_XTS();
	dword _algo_Blowfish_ECB();
	dword _algo_Twofish_ECB();
};

DECL_EXCEPTION(CPPSOURCES_API, CCryptException, CBaseException)
