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
#include "X509.h"
#include "BIO.h"
#include "EVP_PKEY.h"
#include "EVP_MD.h"
#include <openssl/X509.h>

CX509_VERIFY_PARAM::CX509_VERIFY_PARAM(ConstPointer pX509VerifyParam) : COpenSSLClass(pX509VerifyParam)
{
}

CX509_VERIFY_PARAM::~CX509_VERIFY_PARAM()
{
	free();
}

Ptr(CX509_VERIFY_PARAM) CX509_VERIFY_PARAM::create()
{
	Ptr(X509_VERIFY_PARAM) pParam = X509_VERIFY_PARAM_new();

	if (!pParam)
		return NULL;

	Ptr(CX509_VERIFY_PARAM) p = OK_NEW_OPERATOR CX509_VERIFY_PARAM(pParam);

	p->insert_obj();
	return p;
}

void CX509_VERIFY_PARAM::free()
{
	if (_raw)
		X509_VERIFY_PARAM_free(CastAnyPtr(X509_VERIFY_PARAM, _raw));
	_raw = NULL;
}

CX509_NAME::CX509_NAME(ConstPointer pX509Name) : COpenSSLClass(pX509Name)
{
}

CX509_NAME::~CX509_NAME()
{
	free();
}

Ptr(CX509_NAME) CX509_NAME::create()
{
	Ptr(X509_NAME) pName = X509_NAME_new();

	if (!pName)
		return NULL;

	Ptr(CX509_NAME) p = OK_NEW_OPERATOR CX509_NAME(pName);

	p->insert_obj();
	return p;
}

void CX509_NAME::free()
{
	if (_raw)
		X509_NAME_free(CastAnyPtr(X509_NAME, _raw));
	_raw = NULL;
}

CX509_EXTENSION::CX509_EXTENSION(ConstPointer pX509Extension) : COpenSSLClass(pX509Extension)
{
}

CX509_EXTENSION::~CX509_EXTENSION()
{
	free();
}

Ptr(CX509_EXTENSION) CX509_EXTENSION::create()
{
	Ptr(X509_EXTENSION) pExt = X509_EXTENSION_new();

	if (!pExt)
		return NULL;

	Ptr(CX509_EXTENSION) p = OK_NEW_OPERATOR CX509_EXTENSION(pExt);

	p->insert_obj();
	return p;
}

void CX509_EXTENSION::free()
{
	if (_raw)
		X509_EXTENSION_free(CastAnyPtr(X509_EXTENSION, _raw));
	_raw = NULL;
}


CX509::CX509(ConstPointer pX509) : COpenSSLClass(pX509)
{
}

CX509::~CX509()
{
	free();
}

Ptr(CX509) CX509::create()
{
	Ptr(X509) pX509 = X509_new();

	if (!pX509)
		return NULL;

	Ptr(CX509) p = OK_NEW_OPERATOR CX509(pX509);

	p->insert_obj();
	return p;
}

void CX509::free()
{
	if (_raw)
		X509_free(CastAnyPtr(X509, _raw));
	_raw = NULL;
}

int CX509::verify(CEVP_PKEY *r)
{
	return X509_verify(CastAnyPtr(X509, _raw), CastAnyPtr(EVP_PKEY, CastMutable(Pointer, r->get_key())));
}

int CX509::sign(CEVP_PKEY *pkey, const CEVP_MD *md)
{
	return X509_sign(CastAnyPtr(X509, _raw), CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pkey->get_key())), CastAnyConstPtr(EVP_MD, md->get_md()));
}

int CX509::sign_ctx(CEVP_MD_CTX *ctx)
{
	return X509_sign_ctx(CastAnyPtr(X509, _raw), CastAnyPtr(EVP_MD_CTX, CastMutable(Pointer, ctx->get_mdContext())));
}

int CX509::pubkey_digest(const CEVP_MD *type, unsigned char *md, unsigned int *len)
{
	return X509_pubkey_digest(CastAnyPtr(X509, _raw), CastAnyConstPtr(EVP_MD, type->get_md()), md, len);
}

int CX509::digest(const CEVP_MD *type, unsigned char *md, unsigned int *len)
{
	return X509_digest(CastAnyPtr(X509, _raw), CastAnyConstPtr(EVP_MD, type->get_md()), md, len);
}

CX509 *CX509::d2i_fp(FILE *fp, CX509 **x509)
{
	Ptr(X509) ret = NULL;

	if (NULL == d2i_X509_fp(fp, &ret))
		return NULL;

	Ptr(CX509) ret2 = OK_NEW_OPERATOR CX509(ret);

	if ((x509 != NULL) && (*x509 == NULL))
		*x509 = ret2;
	return ret2;
}

int CX509::i2d_fp(FILE *fp)
{
	return i2d_X509_fp(fp, CastAnyPtr(X509, _raw));
}

CX509 *CX509::d2i_bio(CBIO *bp, CX509 **x509)
{
	Ptr(X509) ret = NULL;

	if (NULL == d2i_X509_bio(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), &ret))
		return NULL;

	Ptr(CX509) ret2 = OK_NEW_OPERATOR CX509(ret);

	if ((x509 != NULL) && (*x509 == NULL))
		*x509 = ret2;
	return ret2;
}

int CX509::i2d_bio(CBIO *bp)
{
	return i2d_X509_bio(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), CastAnyPtr(X509, _raw));
}

CX509 *CX509::dup()
{
	return OK_NEW_OPERATOR CX509(X509_dup(CastAnyPtr(X509, _raw)));
}

int CX509::set_ex_data(int idx, void *arg)
{
	return X509_set_ex_data(CastAnyPtr(X509, _raw), idx, arg);
}

void *CX509::get_ex_data(int idx)
{
	return X509_get_ex_data(CastAnyPtr(X509, _raw), idx);
}

int CX509::i2d_AUX(unsigned char **pp)
{
	return i2d_X509_AUX(CastAnyPtr(X509, _raw), pp);
}

CX509 *CX509::d2i_AUX(CX509 **x509, const unsigned char **pp, long length)
{
	Ptr(X509) ret = NULL;

	if (NULL == d2i_X509_AUX(&ret, pp, length))
		return NULL;

	Ptr(CX509) ret2 = OK_NEW_OPERATOR CX509(ret);

	if ((x509 != NULL) && (*x509 == NULL))
		*x509 = ret2;
	return ret2;
}

int CX509::alias_set1(unsigned char *name, int len)
{
	return X509_alias_set1(CastAnyPtr(X509, _raw), name, len);
}

int CX509::keyid_set1(unsigned char *id, int len)
{
	return X509_keyid_set1(CastAnyPtr(X509, _raw), id, len);
}

unsigned char * CX509::alias_get0(int *len)
{
	return X509_alias_get0(CastAnyPtr(X509, _raw), len);
}

unsigned char * CX509::keyid_get0(int *len)
{
	return X509_keyid_get0(CastAnyPtr(X509, _raw), len);
}

int CX509::add1_trust_object(CASN1_OBJECT *obj)
{
	return X509_add1_trust_object(CastAnyPtr(X509, _raw), CastAnyPtr(ASN1_OBJECT, CastMutable(Pointer, obj->get_object())));
}

int CX509::add1_reject_object(CASN1_OBJECT *obj)
{
	return X509_add1_reject_object(CastAnyPtr(X509, _raw), CastAnyPtr(ASN1_OBJECT, CastMutable(Pointer, obj->get_object())));
}

void CX509::trust_clear()
{
	X509_trust_clear(CastAnyPtr(X509, _raw));
}

void CX509::reject_clear()
{
	X509_reject_clear(CastAnyPtr(X509, _raw));
}

int CX509::set_version(long version)
{
	return X509_set_version(CastAnyPtr(X509, _raw), version);
}

int CX509::set_serialNumber(CASN1_INTEGER *serial)
{
	return X509_set_serialNumber(CastAnyPtr(X509, _raw), CastAnyPtr(ASN1_INTEGER, CastMutable(Pointer, serial->get_integer())));
}

CASN1_INTEGER *CX509::get_serialNumber()
{
	Ptr(ASN1_INTEGER) ret = X509_get_serialNumber(CastAnyPtr(X509, _raw));

	if (ret)
		return OK_NEW_OPERATOR CASN1_INTEGER(ret);
	return NULL;
}

int CX509::set_issuer_name(CX509_NAME *name)
{
	return X509_set_issuer_name(CastAnyPtr(X509, _raw), CastAnyPtr(X509_NAME, CastMutable(Pointer, name->get_X509Name())));
}

CX509_NAME *CX509::get_issuer_name()
{
	Ptr(X509_NAME) ret = X509_get_issuer_name(CastAnyPtr(X509, _raw));

	if (ret)
		return OK_NEW_OPERATOR CX509_NAME(ret);
	return NULL;
}

int CX509::set_subject_name(CX509_NAME *name)
{
	return X509_set_subject_name(CastAnyPtr(X509, _raw), CastAnyPtr(X509_NAME, CastMutable(Pointer, name->get_X509Name())));
}

CX509_NAME *CX509::get_subject_name()
{
	Ptr(X509_NAME) ret = X509_get_subject_name(CastAnyPtr(X509, _raw));

	if (ret)
		return OK_NEW_OPERATOR CX509_NAME(ret);
	return NULL;
}

int CX509::set_notBefore(const CASN1_TIME *tm)
{
	return X509_set_notBefore(CastAnyPtr(X509, _raw), CastAnyConstPtr(ASN1_TIME, tm->get_time()));
}

int CX509::set_notAfter(const CASN1_TIME *tm)
{
	return X509_set_notAfter(CastAnyPtr(X509, _raw), CastAnyConstPtr(ASN1_TIME, tm->get_time()));
}

int CX509::set_pubkey(CEVP_PKEY *pkey)
{
	return X509_set_pubkey(CastAnyPtr(X509, _raw), CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pkey->get_key())));
}

CEVP_PKEY *CX509::get_pubkey()
{
	Ptr(EVP_PKEY) ret = X509_get_pubkey(CastAnyPtr(X509, _raw));

	if (ret)
		return OK_NEW_OPERATOR CEVP_PKEY(ret);
	return NULL;
}

CASN1_BIT_STRING* CX509::get0_pubkey_bitstr()
{
	Ptr(ASN1_BIT_STRING) ret = X509_get0_pubkey_bitstr(CastAnyPtr(X509, _raw));

	if (ret)
		return OK_NEW_OPERATOR CASN1_BIT_STRING(ret);
	return NULL;
}

int	CX509::certificate_type(CEVP_PKEY *pubkey /* optional */)
{
	return X509_certificate_type(CastAnyPtr(X509, _raw), CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pubkey->get_key())));
}

int	CX509::check_private_key(CEVP_PKEY *pkey)
{
	return X509_check_private_key(CastAnyPtr(X509, _raw), CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pkey->get_key())));
}

int	CX509::issuer_and_serial_cmp(const CX509 *other)
{
	return X509_issuer_and_serial_cmp(CastAnyPtr(X509, _raw), CastAnyConstPtr(X509, other->get_X509()));
}

unsigned long CX509::issuer_and_serial_hash()
{
	return X509_issuer_and_serial_hash(CastAnyPtr(X509, _raw));
}

int	CX509::issuer_name_cmp(const CX509 *other)
{
	return	X509_issuer_name_cmp(CastAnyPtr(X509, _raw), CastAnyConstPtr(X509, other->get_X509()));
}

unsigned long CX509::issuer_name_hash()
{
	return	X509_issuer_name_hash(CastAnyPtr(X509, _raw));
}

int CX509::subject_name_cmp(const CX509 *other)
{
	return	X509_subject_name_cmp(CastAnyPtr(X509, _raw), CastAnyConstPtr(X509, other->get_X509()));
}

unsigned long CX509::subject_name_hash()
{
	return	X509_subject_name_hash(CastAnyPtr(X509, _raw));
}

int	CX509::cmp(const CX509 *other)
{
	return	X509_cmp(CastAnyPtr(X509, _raw), CastAnyConstPtr(X509, other->get_X509()));
}

int	CX509::print_ex_fp(FILE *bp, unsigned long nmflag, unsigned long cflag)
{
	return	X509_print_ex_fp(bp, CastAnyPtr(X509, _raw), nmflag, cflag);
}

int	CX509::print_fp(FILE *bp)
{
	return	X509_print_fp(bp, CastAnyPtr(X509, _raw));
}

int	CX509::print_ex(CBIO *bp, unsigned long nmflag, unsigned long cflag)
{
	return	X509_print_ex(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), CastAnyPtr(X509, _raw), nmflag, cflag);
}

int	CX509::print(CBIO *bp)
{
	return	X509_print(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), CastAnyPtr(X509, _raw));
}

int	CX509::ocspid_print(CBIO *bp)
{
	return	X509_ocspid_print(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), CastAnyPtr(X509, _raw));
}

int	CX509::get_ext_count()
{
	return	X509_get_ext_count(CastAnyPtr(X509, _raw));
}

int	CX509::get_ext_by_NID(int nid, int lastpos)
{
	return	X509_get_ext_by_NID(CastAnyPtr(X509, _raw), nid, lastpos);
}

int	CX509::get_ext_by_OBJ(CASN1_OBJECT *obj, int lastpos)
{
	return	X509_get_ext_by_OBJ(CastAnyPtr(X509, _raw), CastAnyPtr(ASN1_OBJECT, CastMutable(Pointer, obj->get_object())), lastpos);
}

int	CX509::get_ext_by_critical(int crit, int lastpos)
{
	return	X509_get_ext_by_critical(CastAnyPtr(X509, _raw), crit, lastpos);
}

CX509_EXTENSION *CX509::get_ext(int loc)
{
	Ptr(X509_EXTENSION) ret = X509_get_ext(CastAnyPtr(X509, _raw), loc);

	if (ret)
		return OK_NEW_OPERATOR CX509_EXTENSION(ret);
	return NULL;
}

CX509_EXTENSION *CX509::delete_ext(int loc)
{
	Ptr(X509_EXTENSION) ret = X509_delete_ext(CastAnyPtr(X509, _raw), loc);

	if (ret)
		return OK_NEW_OPERATOR CX509_EXTENSION(ret);
	return NULL;
}

int	CX509::add_ext(CX509_EXTENSION *ex, int loc)
{
	return X509_add_ext(CastAnyPtr(X509, _raw), CastAnyPtr(X509_EXTENSION, CastMutable(Pointer, ex->get_X509Extension())), loc);
}

void* CX509::get_ext_d2i(int nid, int *crit, int *idx)
{
	return X509_get_ext_d2i(CastAnyPtr(X509, _raw), nid, crit, idx);
}

int	CX509::add1_ext_i2d(int nid, void *value, int crit, unsigned long flags)
{
	return	X509_add1_ext_i2d(CastAnyPtr(X509, _raw), nid, value, crit, flags);
}

int CX509::check_trust(int id, int flags)
{
	return X509_check_trust(CastAnyPtr(X509, _raw), id, flags);
}

CX509_STORE::CX509_STORE(ConstPointer pX509Store) : COpenSSLClass(pX509Store)
{
}

CX509_STORE::~CX509_STORE()
{
	free();
}

Ptr(CX509_STORE) CX509_STORE::create()
{
	Ptr(X509_STORE) pStore = X509_STORE_new();

	if (!pStore)
		return NULL;

	Ptr(CX509_STORE) p = OK_NEW_OPERATOR CX509_STORE(pStore);

	p->insert_obj();
	return p;
}

void CX509_STORE::free()
{
	if (_raw)
		X509_STORE_free(CastAnyPtr(X509_STORE, _raw));
	_raw = NULL;
}

CX509_STORE_CTX::CX509_STORE_CTX(ConstPointer pX509Name) : COpenSSLClass(pX509Name)
{
}

CX509_STORE_CTX::~CX509_STORE_CTX()
{
	free();
}

Ptr(CX509_STORE_CTX) CX509_STORE_CTX::create()
{
	Ptr(X509_STORE_CTX) pCtx = X509_STORE_CTX_new();

	if (!pCtx)
		return NULL;

	Ptr(CX509_STORE_CTX) p = OK_NEW_OPERATOR CX509_STORE_CTX(pCtx);

	p->insert_obj();
	return p;
}

void CX509_STORE_CTX::free()
{
	if (_raw)
		X509_STORE_CTX_free(CastAnyPtr(X509_STORE_CTX, _raw));
	_raw = NULL;
}

