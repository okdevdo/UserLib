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

class OPENSSL_API CEVP_MD : public COpenSSLClass
{
public:
	CEVP_MD(ConstPointer md = NULL);
	virtual ~CEVP_MD();

	__inline ConstPointer get_md() const { return _raw; }

	static ConstPtr(CEVP_MD) create_md_null();
	static ConstPtr(CEVP_MD) create_md4();
	static ConstPtr(CEVP_MD) create_md5();
	static ConstPtr(CEVP_MD) create_sha();
	static ConstPtr(CEVP_MD) create_sha1();
	static ConstPtr(CEVP_MD) create_dss();
	static ConstPtr(CEVP_MD) create_dss1();
	static ConstPtr(CEVP_MD) create_mdc2();
	static ConstPtr(CEVP_MD) create_ripemd160();
	static ConstPtr(CEVP_MD) create_sha224();
	static ConstPtr(CEVP_MD) create_sha256();
	static ConstPtr(CEVP_MD) create_sha384();
	static ConstPtr(CEVP_MD) create_sha512();
	static ConstPtr(CEVP_MD) get_digestbyname(const char *name);
	static ConstPtr(CEVP_MD) get_digestbynid(int n);

	int type();
	int pkey_type();
	int size();
	int block_size();
};

class CEVP_PKEY;
class OPENSSL_API CEVP_MD_CTX : public COpenSSLClass
{
public:
	enum
	{
		EVP_MAX_MD_SIZE = 64
	};

	CEVP_MD_CTX(ConstPointer mdCtx = NULL);
	virtual ~CEVP_MD_CTX();

	__inline ConstPointer get_mdContext() const { return _mdCtx; }

	static Ptr(CEVP_MD_CTX) create();

	void init();

	int DigestInit(const CEVP_MD *type);
	int DigestInit_ex(const CEVP_MD *type, CENGINE *impl);
	int DigestUpdate(const void *d, size_t cnt);
	int DigestFinal(unsigned char *md, unsigned int *s);
	int DigestFinal_ex(unsigned char *md, unsigned int *s);

	void SignInit(const CEVP_MD *type);
	int SignInit_ex(const CEVP_MD *type, CENGINE *impl);
	int SignUpdate(const void *d, unsigned int cnt);
	int SignFinal(unsigned char *sig, unsigned int *s, CEVP_PKEY *pkey);

	int VerifyInit(const CEVP_MD *type);
	int VerifyInit_ex(const CEVP_MD *type, CENGINE *impl);
	int VerifyUpdate(const void *d, unsigned int cnt);
	int VerifyFinal(unsigned char *sigbuf, unsigned int siglen, CEVP_PKEY *pkey);

	int cleanup();
	void destroy();

	const CEVP_MD *md();
	int type();
	int size();
	int block_size();

	static int copy(CEVP_MD_CTX *out, CEVP_MD_CTX *in);
	static int copy_ex(CEVP_MD_CTX *out, const CEVP_MD_CTX *in);

protected:
	Pointer _mdCtx;
};

