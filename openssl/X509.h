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

class OPENSSL_API CX509_VERIFY_PARAM : public COpenSSLClass
{
public:
	CX509_VERIFY_PARAM(ConstPointer pX509VerifyParam = NULL);
	virtual ~CX509_VERIFY_PARAM();

	ConstPointer get_X509VerifyParam() const { return _raw; }

	static Ptr(CX509_VERIFY_PARAM) create();

	void free();

};

class OPENSSL_API CX509_NAME : public COpenSSLClass
{
public:
	CX509_NAME(ConstPointer pX509Name = NULL);
	virtual ~CX509_NAME();

	ConstPointer get_X509Name() const { return _raw; }

	static Ptr(CX509_NAME) create();

	void free();

};

class OPENSSL_API CX509_EXTENSION : public COpenSSLClass
{
public:
	CX509_EXTENSION(ConstPointer pX509Extension = NULL);
	virtual ~CX509_EXTENSION();

	ConstPointer get_X509Extension() const { return _raw; }

	static Ptr(CX509_EXTENSION) create();

	void free();

};

class CBIO;
class CEVP_PKEY;
class CEVP_MD_CTX;
class CEVP_MD;
class OPENSSL_API CX509 : public COpenSSLClass
{
public:
	CX509(ConstPointer pX509 = NULL);
	virtual ~CX509();

	ConstPointer get_X509() const { return _raw; }
	
	static Ptr(CX509) create();

	void free();

	int verify(CEVP_PKEY *r);
	int sign(CEVP_PKEY *pkey, const CEVP_MD *md);
	int sign_ctx(CEVP_MD_CTX *ctx);
	int pubkey_digest(const CEVP_MD *type, unsigned char *md, unsigned int *len);
	int digest(const CEVP_MD *type, unsigned char *md, unsigned int *len);
	static CX509 *d2i_fp(FILE *fp, CX509 **x509);
	int i2d_fp(FILE *fp);
	static CX509 *d2i_bio(CBIO *bp, CX509 **x509);
	int i2d_bio(CBIO *bp);
	CX509 *dup();
	int set_ex_data(int idx, void *arg);
	void *get_ex_data(int idx);
	int i2d_AUX(unsigned char **pp);
	static CX509 *d2i_AUX(CX509 **a, const unsigned char **pp, long length);
	int alias_set1(unsigned char *name, int len);
	int keyid_set1(unsigned char *id, int len);
	unsigned char * alias_get0(int *len);
	unsigned char * keyid_get0(int *len);
	int add1_trust_object(CASN1_OBJECT *obj);
	int add1_reject_object(CASN1_OBJECT *obj);
	void trust_clear();
	void reject_clear();
	int set_version(long version);
	int set_serialNumber(CASN1_INTEGER *serial);
	CASN1_INTEGER *get_serialNumber();
	int set_issuer_name(CX509_NAME *name);
	CX509_NAME *get_issuer_name();
	int set_subject_name(CX509_NAME *name);
	CX509_NAME *get_subject_name();
	int set_notBefore(const CASN1_TIME *tm);
	int set_notAfter(const CASN1_TIME *tm);
	int set_pubkey(CEVP_PKEY *pkey);
	CEVP_PKEY *get_pubkey();
	CASN1_BIT_STRING* get0_pubkey_bitstr();
	int	certificate_type(CEVP_PKEY *pubkey /* optional */);
	int	check_private_key(CEVP_PKEY *pkey);
	int	issuer_and_serial_cmp(const CX509 *other);
	unsigned long issuer_and_serial_hash();
	int	issuer_name_cmp(const CX509 *other);
	unsigned long issuer_name_hash();
	int subject_name_cmp(const CX509 *other);
	unsigned long subject_name_hash();
	int	cmp(const CX509 *other);
	int	print_ex_fp(FILE *bp, unsigned long nmflag, unsigned long cflag);
	int	print_fp(FILE *bp);
	int	print_ex(CBIO *bp, unsigned long nmflag, unsigned long cflag);
	int	print(CBIO *bp);
	int	ocspid_print(CBIO *bp);
	int	get_ext_count();
	int	get_ext_by_NID(int nid, int lastpos);
	int	get_ext_by_OBJ(CASN1_OBJECT *obj, int lastpos);
	int	get_ext_by_critical(int crit, int lastpos);
	CX509_EXTENSION *get_ext(int loc);
	CX509_EXTENSION *delete_ext(int loc);
	int	add_ext(CX509_EXTENSION *ex, int loc);
	void* get_ext_d2i(int nid, int *crit, int *idx);
	int	add1_ext_i2d(int nid, void *value, int crit, unsigned long flags);
	int check_trust(int id, int flags);
};

class OPENSSL_API CX509_STORE : public COpenSSLClass
{
public:
	CX509_STORE(ConstPointer pX509Store = NULL);
	virtual ~CX509_STORE();

	ConstPointer get_X509Store() const { return _raw; }

	static Ptr(CX509_STORE) create();

	void free();

};

class OPENSSL_API CX509_STORE_CTX : public COpenSSLClass
{
public:
	CX509_STORE_CTX(ConstPointer pX509StoreContext = NULL);
	virtual ~CX509_STORE_CTX();

	ConstPointer get_X509StoreContext() const { return _raw; }

	static Ptr(CX509_STORE_CTX) create();

	void free();

};

