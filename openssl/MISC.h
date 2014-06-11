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

template <typename c>
class OPENSSL_API CStack
{
public:
	CStack(DECL_FILE_LINE0) : _stack(ARGS_FILE_LINE 64, 64) {}
	~CStack() {}

	TListCnt Count() const
	{
		return _stack.Count();
	}
	Ptr(c) Value(TListIndex ix) const
	{
		if (_stack.Count() == 0) 
			return NULL;
		
		typename CDataVectorT<c>::Iterator it = _stack.Index(ix);

		return *it;
	}
	Ptr(c) Pop() 
	{ 
		if (_stack.Count() == 0) 
			return NULL;

		typename CDataVectorT<c>::Iterator it = _stack.Last();
		Ptr(c) ret = *it;

		_stack.Remove(it);
		ret->release();
		return ret;
	}
	void Push(Ptr(c) v)
	{
		v->addRef();
		_stack.Append(v); 
	}

protected:
	CDataVectorT<c> _stack;

private:
	CStack(ConstRef(CStack));
};

template <typename c>
class OPENSSL_API CHashList
{
	typedef CDataHashLinkedListT<c, COpenSSLClassHashFunctor> THashList;
public:
	CHashList() : _list(__FILE__LINE__ 250, COpenSSLClassHashFunctor(250)) {}
	~CHashList() {}

	void insert(Ptr(c) v) 
	{ 
		_list.InsertSorted(v); 
	}
	Ptr(c) search(Ptr(c) k)
	{
		typename THashList::Iterator it = _list.FindSorted(k);

		if (it)
			return (*it);
		return nullptr; 
	}
	void remove(Ptr(c) k)
	{ 
		_list.RemoveSorted(k); 
	}

protected:
	THashList _list;
};

class OPENSSL_API CENGINE : public COpenSSLClass
{
public:
	CENGINE(ConstPointer impl = NULL);
	virtual ~CENGINE();

	__inline ConstPointer get_impl() const { return _raw; }
};

class OPENSSL_API CBUF_MEM : public COpenSSLClass
{
public:
	CBUF_MEM(ConstPointer bufmem = NULL);
	virtual ~CBUF_MEM();

	__inline ConstPointer get_bufmem() const { return _raw; }

	static Ptr(CBUF_MEM) create();

	void free();

	int grow(size_t len);
	int grow_clean(size_t len);
};

class OPENSSL_API CASN1_OBJECT : public COpenSSLClass
{
public:
	CASN1_OBJECT(ConstPointer pObject = NULL);
	virtual ~CASN1_OBJECT();

	__inline ConstPointer get_object() const { return _raw; }

	static Ptr(CASN1_OBJECT) create();

	void free();

	int	i2d(unsigned char **pp);
	static CASN1_OBJECT *c2i(CASN1_OBJECT **a, const unsigned char **pp, long length);
	static CASN1_OBJECT *d2i(CASN1_OBJECT **a, const unsigned char **pp, long length);

};

class OPENSSL_API CASN1_INTEGER : public COpenSSLClass
{
public:
	CASN1_INTEGER(ConstPointer pInteger = NULL);
	virtual ~CASN1_INTEGER();

	__inline ConstPointer get_integer() const { return _raw; }

	static Ptr(CASN1_INTEGER) create();

	void free();

	Ptr(CASN1_INTEGER) dup();

	int	i2c(unsigned char **pp);
	static CASN1_INTEGER *c2i(CASN1_INTEGER **a, const unsigned char **pp, long length);

};

class OPENSSL_API CASN1_TIME : public COpenSSLClass
{
public:
	CASN1_TIME(ConstPointer pTime = NULL);
	virtual ~CASN1_TIME();

	__inline ConstPointer get_time() const { return _raw; }

	static Ptr(CASN1_TIME) create();

	void free();
};

class OPENSSL_API CASN1_BIT_STRING : public COpenSSLClass
{
public:
	CASN1_BIT_STRING(ConstPointer pBitString = NULL);
	virtual ~CASN1_BIT_STRING();

	__inline ConstPointer get_bitstring() const { return _raw; }

	static Ptr(CASN1_BIT_STRING) create();

	void free();
};

class OPENSSL_API CBIGNUM : public COpenSSLClass
{
public:
	CBIGNUM(ConstPointer pBigNum = NULL);
	virtual ~CBIGNUM();

	__inline ConstPointer get_bignum() const { return _raw; }

	static Ptr(CBIGNUM) create();

	void free();
};

class OPENSSL_API CBN_CTX : public COpenSSLClass
{
public:
	CBN_CTX(ConstPointer pBigNumContext = NULL);
	virtual ~CBN_CTX();

	__inline ConstPointer get_bignumcontext() const { return _raw; }

	static Ptr(CBN_CTX) create();

	void free();
};

class OPENSSL_API CBN_BLINDING : public COpenSSLClass
{
public:
	CBN_BLINDING(ConstPointer pBigNumBlinding = NULL);
	virtual ~CBN_BLINDING();

	__inline ConstPointer get_bignumblinding() const { return _raw; }

	static Ptr(CBN_BLINDING) create(ConstPtr(CBIGNUM) A, ConstPtr(CBIGNUM) Ai, Ptr(CBIGNUM) mod);

	void free();
};

class OPENSSL_API CBN_GENCB : public COpenSSLClass
{
public:
	typedef void(*cb_1)(int, int, void *);
	typedef int(*cb_2)(int, int, CBN_GENCB *);

	CBN_GENCB(ConstPointer pGenCb = NULL);
	virtual ~CBN_GENCB();

	__inline ConstPointer get_gencb() const { return _raw; }

	static Ptr(CBN_GENCB) create();
	static Ptr(CBN_GENCB) create_1(cb_1 cb, void* arg);
	static Ptr(CBN_GENCB) create_2(cb_2 cb, void* arg);

	void free();

	void set_1(cb_1 cb, void* arg);
	void set_2(cb_2 cb, void* arg);

	void set_app_data(void* arg);
	void *get_app_data(void);
};

typedef struct crypto_ex_data_st CRYPTO_EX_DATA;
typedef int CRYPTO_EX_new(void *parent, void *ptr, CRYPTO_EX_DATA *ad, int idx, long argl, void *argp);
typedef void CRYPTO_EX_free(void *parent, void *ptr, CRYPTO_EX_DATA *ad, int idx, long argl, void *argp);
typedef int CRYPTO_EX_dup(CRYPTO_EX_DATA *to, CRYPTO_EX_DATA *from, void *from_d, int idx, long argl, void *argp);
