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
#include "EVP_PKEY.h"
#include "EVP_MD.h"
#include "BIO.h"
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/dsa.h>
#include <openssl/dh.h>
#include <openssl/ec.h>

CRSA_METHOD::CRSA_METHOD(ConstPointer pMethod) : COpenSSLClass(pMethod)
{
}

CRSA_METHOD::~CRSA_METHOD()
{
}

Ptr(CRSA_METHOD) CRSA_METHOD::create_PKCS1_SSLeay()
{
	ConstPtr(RSA_METHOD) pRSA_Method = RSA_PKCS1_SSLeay();

	if (!pRSA_Method)
		return NULL;

	Ptr(CRSA_METHOD) p = OK_NEW_OPERATOR CRSA_METHOD(pRSA_Method);

	p->insert_obj();
	return p;
}

Ptr(CRSA_METHOD) CRSA_METHOD::create_null_method()
{
	ConstPtr(RSA_METHOD) pRSA_Method = RSA_null_method();

	if (!pRSA_Method)
		return NULL;

	Ptr(CRSA_METHOD) p = OK_NEW_OPERATOR CRSA_METHOD(pRSA_Method);

	p->insert_obj();
	return p;
}

int CRSA_METHOD::get_flags()
{
	return CastAnyPtr(RSA_METHOD, _raw)->flags;
}

void CRSA_METHOD::set_flags(int flags)
{
	CastAnyPtr(RSA_METHOD, _raw)->flags = flags;
}

CRSA_KEY::CRSA_KEY(ConstPointer pkey) : COpenSSLClass(pkey)
{
}

CRSA_KEY::~CRSA_KEY()
{
	free();
}

Ptr(CRSA_KEY) CRSA_KEY::create()
{
	Ptr(RSA) pRSA = RSA_new();

	if (!pRSA)
		return NULL;

	Ptr(CRSA_KEY) p = OK_NEW_OPERATOR CRSA_KEY(pRSA);

	p->insert_obj();
	return p;
}

Ptr(CRSA_KEY) CRSA_KEY::create_method(Ptr(CENGINE) engine)
{
	Ptr(RSA) pRSA = RSA_new_method(CastAnyPtr(ENGINE, CastMutable(Pointer, engine->get_impl())));

	if (!pRSA)
		return NULL;

	Ptr(CRSA_KEY) p = OK_NEW_OPERATOR CRSA_KEY(pRSA);

	p->insert_obj();
	return p;
}

void CRSA_KEY::free()
{
	if (_raw)
		RSA_free(CastAnyPtr(RSA, _raw));
	_raw = NULL;
}

int CRSA_KEY::get_flags()
{
	return CastAnyPtr(RSA, _raw)->flags;
}

void CRSA_KEY::set_flags(int flags)
{
	CastAnyPtr(RSA, _raw)->flags = flags;
}

void CRSA_KEY::set_app_data(void* arg)
{
	RSA_set_app_data(CastAnyPtr(RSA, _raw), arg);
}

void *CRSA_KEY::get_app_data(void)
{
	return RSA_get_app_data(CastAnyPtr(RSA, _raw));
}

int CRSA_KEY::size()
{
	return RSA_size(CastAnyPtr(RSA, _raw));
}

int	CRSA_KEY::generate_key_ex(int bits, CBIGNUM *e, CBN_GENCB *cb)
{
	return RSA_generate_key_ex(CastAnyPtr(RSA, _raw), bits, CastAnyPtr(BIGNUM, CastMutable(Pointer, e->get_bignum())), CastAnyPtr(BN_GENCB, CastMutable(Pointer, cb->get_gencb())));
}

int	CRSA_KEY::check_key()
{
	return RSA_check_key(CastAnyPtr(RSA, _raw));
}

int	CRSA_KEY::public_encrypt(int flen, const unsigned char *from, unsigned char *to, TPadding padding)
{
	return RSA_public_encrypt(flen, from, to, CastAnyPtr(RSA, _raw), padding);
}

int	CRSA_KEY::private_encrypt(int flen, const unsigned char *from, unsigned char *to, TPadding padding)
{
	return RSA_private_encrypt(flen, from, to, CastAnyPtr(RSA, _raw), padding);
}

int	CRSA_KEY::public_decrypt(int flen, const unsigned char *from, unsigned char *to, TPadding padding)
{
	return RSA_public_decrypt(flen, from, to, CastAnyPtr(RSA, _raw), padding);
}

int	CRSA_KEY::private_decrypt(int flen, const unsigned char *from, unsigned char *to, TPadding padding)
{
	return RSA_private_decrypt(flen, from, to, CastAnyPtr(RSA, _raw), padding);
}

void CRSA_KEY::set_default_method(const CRSA_METHOD *meth)
{
	RSA_set_default_method(CastAnyConstPtr(RSA_METHOD, meth->get_method()));
}

const CRSA_METHOD *CRSA_KEY::get_default_method(void)
{
	ConstPtr(RSA_METHOD) pMeth = RSA_get_default_method();

	if (!pMeth)
		return NULL;

	Ptr(CRSA_METHOD) pObject = CastDynamicPtr(CRSA_METHOD, find_obj(pMeth));

	if (!pObject)
	{
		pObject = OK_NEW_OPERATOR CRSA_METHOD(pMeth);

		pObject->insert_obj();
	}
	return pObject;
}

const CRSA_METHOD *CRSA_KEY::get_method()
{
	ConstPtr(RSA_METHOD) pMeth = RSA_get_method(CastAnyPtr(RSA, _raw));

	if (!pMeth)
		return NULL;

	Ptr(CRSA_METHOD) pObject = CastDynamicPtr(CRSA_METHOD, find_obj(pMeth));

	if (!pObject)
	{
		pObject = OK_NEW_OPERATOR CRSA_METHOD(pMeth);

		pObject->insert_obj();
	}
	return pObject;
}

int CRSA_KEY::set_method(const CRSA_METHOD *meth)
{
	return RSA_set_method(CastAnyPtr(RSA, _raw), CastAnyConstPtr(RSA_METHOD, meth->get_method()));
}

CRSA_KEY* CRSA_KEY::d2i_PublicKey(const unsigned char **in, long len)
{
	Ptr(RSA) pRSA = NULL;

	if (!(d2i_RSAPublicKey(&pRSA, in, len)))
		return NULL;

	Ptr(CRSA_KEY) pObject = CastDynamicPtr(CRSA_KEY, find_obj(pRSA));

	if (!pObject)
	{
		pObject = OK_NEW_OPERATOR CRSA_KEY(pRSA);

		pObject->insert_obj();
	}
	return pObject;
}

int CRSA_KEY::i2d_PublicKey(unsigned char **out)
{
	return i2d_RSAPublicKey(CastAnyPtr(RSA, _raw), out);
}

CRSA_KEY* CRSA_KEY::d2i_PrivateKey(const unsigned char **in, long len)
{
	Ptr(RSA) pRSA = NULL;

	if (!(d2i_RSAPrivateKey(&pRSA, in, len)))
		return NULL;

	Ptr(CRSA_KEY) pObject = CastDynamicPtr(CRSA_KEY, find_obj(pRSA));

	if (!pObject)
	{
		pObject = OK_NEW_OPERATOR CRSA_KEY(pRSA);

		pObject->insert_obj();
	}
	return pObject;
}

int CRSA_KEY::i2d_PrivateKey(unsigned char **out)
{
	return i2d_RSAPrivateKey(CastAnyPtr(RSA, _raw), out);
}

int	CRSA_KEY::print_fp(FILE *fp, int offset)
{
#ifndef OPENSSL_NO_FP_API
	return RSA_print_fp(fp, CastAnyPtr(RSA, _raw), offset);
#else
	return 0;
#endif
}

int	CRSA_KEY::print(CBIO *bp, int offset)
{
#ifndef OPENSSL_NO_BIO
	return RSA_print(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), CastAnyPtr(RSA, _raw), offset);
#else
	return 0;
#endif
}

int CRSA_KEY::i2d_NET(unsigned char **pp, TCallback1 cb, int sgckey)
{
#ifndef OPENSSL_NO_RC4
	return i2d_RSA_NET(CastAnyPtr(RSA, _raw), pp, cb, sgckey);
#else
	return 0;
#endif
}

Ptr(CRSA_KEY) CRSA_KEY::d2i_NET(const unsigned char **pp, long length, TCallback1 cb, int sgckey)
{
#ifndef OPENSSL_NO_RC4
	Ptr(RSA) pRSA = NULL;

	if (!(d2i_RSA_NET(&pRSA, pp, length, cb, sgckey)))
		return NULL;

	Ptr(CRSA_KEY) pObject = CastDynamicPtr(CRSA_KEY, find_obj(pRSA));

	if (!pObject)
	{
		pObject = OK_NEW_OPERATOR CRSA_KEY(pRSA);

		pObject->insert_obj();
	}
	return pObject;
#else
	return NULL;
#endif
}

int CRSA_KEY::i2d_Netscape(unsigned char **pp, TCallback1 cb)
{
#ifndef OPENSSL_NO_RC4
	return i2d_Netscape_RSA(CastAnyPtr(RSA, _raw), pp, cb);
#else
	return 0;
#endif
}

Ptr(CRSA_KEY) CRSA_KEY::d2i_Netscape(const unsigned char **pp, long length, TCallback1 cb)
{
#ifndef OPENSSL_NO_RC4
	Ptr(RSA) pRSA = NULL;

	if (!(d2i_Netscape_RSA(&pRSA, pp, length, cb)))
		return NULL;

	Ptr(CRSA_KEY) pObject = CastDynamicPtr(CRSA_KEY, find_obj(pRSA));

	if (!pObject)
	{
		pObject = OK_NEW_OPERATOR CRSA_KEY(pRSA);

		pObject->insert_obj();
	}
	return pObject;
#else
	return NULL;
#endif
}

int CRSA_KEY::sign(int type, const unsigned char *m, unsigned int m_length, unsigned char *sigret, unsigned int *siglen)
{
	return RSA_sign(type, m, m_length, sigret, siglen, CastAnyPtr(RSA, _raw));
}

int CRSA_KEY::verify(int type, const unsigned char *m, unsigned int m_length, const unsigned char *sigbuf, unsigned int siglen)
{
	return RSA_verify(type, m, m_length, sigbuf, siglen, CastAnyPtr(RSA, _raw));
}

int CRSA_KEY::sign_ASN1_OCTET_STRING(int type, const unsigned char *m, unsigned int m_length, unsigned char *sigret, unsigned int *siglen)
{
	return RSA_sign_ASN1_OCTET_STRING(type, m, m_length, sigret, siglen, CastAnyPtr(RSA, _raw));
}

int CRSA_KEY::verify_ASN1_OCTET_STRING(int type, const unsigned char *m, unsigned int m_length, unsigned char *sigbuf, unsigned int siglen)
{
	return RSA_verify_ASN1_OCTET_STRING(type, m, m_length, sigbuf, siglen, CastAnyPtr(RSA, _raw));
}

int CRSA_KEY::blinding_on(CBN_CTX *ctx)
{
	return RSA_blinding_on(CastAnyPtr(RSA, _raw), CastAnyPtr(BN_CTX, CastMutable(Pointer, ctx->get_bignumcontext())));
}

void CRSA_KEY::blinding_off()
{
	RSA_blinding_off(CastAnyPtr(RSA, _raw));
}

CBN_BLINDING *CRSA_KEY::setup_blinding(CBN_CTX *ctx)
{
	Ptr(BN_BLINDING) pBlinding = RSA_setup_blinding(CastAnyPtr(RSA, _raw), CastAnyPtr(BN_CTX, CastMutable(Pointer, ctx->get_bignumcontext())));

	if (!pBlinding)
		return NULL;

	Ptr(CBN_BLINDING) pObject = CastDynamicPtr(CBN_BLINDING, find_obj(pBlinding));

	if (!pObject)
	{
		pObject = OK_NEW_OPERATOR CBN_BLINDING(pBlinding);

		pObject->insert_obj();
	}
	return pObject;
}

int CRSA_KEY::padding_add_PKCS1_type_1(unsigned char *to, int tlen, const unsigned char *f, int fl)
{
	return RSA_padding_add_PKCS1_type_1(to, tlen, f, fl);
}

int CRSA_KEY::padding_check_PKCS1_type_1(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len)
{
	return RSA_padding_check_PKCS1_type_1(to, tlen, f, fl, rsa_len);
}

int CRSA_KEY::padding_add_PKCS1_type_2(unsigned char *to, int tlen, const unsigned char *f, int fl)
{
	return RSA_padding_add_PKCS1_type_2(to, tlen, f, fl);
}

int CRSA_KEY::padding_check_PKCS1_type_2(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len)
{
	return RSA_padding_check_PKCS1_type_2(to, tlen, f, fl, rsa_len);
}

int CRSA_KEY::padding_add_PKCS1_OAEP(unsigned char *to, int tlen, const unsigned char *f, int fl, const unsigned char *p, int pl)
{
	return RSA_padding_add_PKCS1_OAEP(to, tlen, f, fl, p, pl);
}

int CRSA_KEY::padding_check_PKCS1_OAEP(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len, const unsigned char *p, int pl)
{
	return RSA_padding_check_PKCS1_OAEP(to, tlen, f, fl, rsa_len, p, pl);
}

int CRSA_KEY::padding_add_SSLv23(unsigned char *to, int tlen, const unsigned char *f, int fl)
{
	return RSA_padding_add_SSLv23(to, tlen, f, fl);
}

int CRSA_KEY::padding_check_SSLv23(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len)
{
	return RSA_padding_check_SSLv23(to, tlen, f, fl, rsa_len);
}

int CRSA_KEY::padding_add_none(unsigned char *to, int tlen, const unsigned char *f, int fl)
{
	return RSA_padding_add_none(to, tlen, f, fl);
}

int CRSA_KEY::padding_check_none(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len)
{
	return RSA_padding_check_none(to, tlen, f, fl, rsa_len);
}

int CRSA_KEY::padding_add_X931(unsigned char *to, int tlen, const unsigned char *f, int fl)
{
	return RSA_padding_add_X931(to, tlen, f, fl);
}

int CRSA_KEY::padding_check_X931(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len)
{
	return RSA_padding_check_X931(to, tlen, f, fl, rsa_len);
}

int CRSA_KEY::X931_hash_id(int nid)
{
	return RSA_X931_hash_id(nid);
}

int CRSA_KEY::verify_PKCS1_PSS(const unsigned char *mHash, const CEVP_MD *Hash, const unsigned char *EM, int sLen)
{
	return RSA_verify_PKCS1_PSS(CastAnyPtr(RSA, _raw), mHash, CastAnyConstPtr(EVP_MD, Hash->get_md()), EM, sLen);
}

int CRSA_KEY::padding_add_PKCS1_PSS(unsigned char *EM, const unsigned char *mHash, const CEVP_MD *Hash, int sLen)
{
	return RSA_padding_add_PKCS1_PSS(CastAnyPtr(RSA, _raw), EM, mHash, CastAnyConstPtr(EVP_MD, Hash->get_md()), sLen);
}

int CRSA_KEY::verify_PKCS1_PSS_mgf1(const unsigned char *mHash, const CEVP_MD *Hash, const CEVP_MD *mgf1Hash, const unsigned char *EM, int sLen)
{
	return RSA_verify_PKCS1_PSS_mgf1(CastAnyPtr(RSA, _raw), mHash, CastAnyConstPtr(EVP_MD, Hash->get_md()), CastAnyConstPtr(EVP_MD, mgf1Hash->get_md()), EM, sLen);
}

int CRSA_KEY::padding_add_PKCS1_PSS_mgf1(unsigned char *EM, const unsigned char *mHash, const CEVP_MD *Hash, const CEVP_MD *mgf1Hash, int sLen)
{
	return RSA_padding_add_PKCS1_PSS_mgf1(CastAnyPtr(RSA, _raw), EM, mHash, CastAnyConstPtr(EVP_MD, Hash->get_md()), CastAnyConstPtr(EVP_MD, mgf1Hash->get_md()), sLen);
}

int CRSA_KEY::get_ex_new_index(long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func)
{
	return RSA_get_ex_new_index(argl, argp, new_func, dup_func, free_func);
}

int CRSA_KEY::set_ex_data(int idx, void *arg)
{
	return RSA_set_ex_data(CastAnyPtr(RSA, _raw), idx, arg);
}

void *CRSA_KEY::get_ex_data(int idx)
{
	return RSA_get_ex_data(CastAnyPtr(RSA, _raw), idx);
}

Ptr(CRSA_KEY) CRSA_KEY::PublicKey_dup()
{
	Ptr(RSA) pRSA = RSAPublicKey_dup(CastAnyPtr(RSA, _raw));

	if (!pRSA)
		return NULL;

	Ptr(CRSA_KEY) pObject = OK_NEW_OPERATOR CRSA_KEY(pRSA);

	pObject->insert_obj();
	return pObject;
}

Ptr(CRSA_KEY) CRSA_KEY::PrivateKey_dup()
{
	Ptr(RSA) pRSA = RSAPrivateKey_dup(CastAnyPtr(RSA, _raw));

	if (!pRSA)
		return NULL;

	Ptr(CRSA_KEY) pObject = OK_NEW_OPERATOR CRSA_KEY(pRSA);

	pObject->insert_obj();
	return pObject;
}

CDSA_KEY::CDSA_KEY(ConstPointer pkey) : COpenSSLClass(pkey)
{
}

CDSA_KEY::~CDSA_KEY()
{
	free();
}

Ptr(CDSA_KEY) CDSA_KEY::create()
{
	Ptr(DSA) pDSA = DSA_new();

	if (!pDSA)
		return NULL;

	Ptr(CDSA_KEY) p = OK_NEW_OPERATOR CDSA_KEY(pDSA);

	p->insert_obj();
	return p;
}

void CDSA_KEY::free()
{
	DSA_free(CastAnyPtr(DSA, _raw));
	_raw = NULL;
}

CDH_KEY::CDH_KEY(ConstPointer pkey) : COpenSSLClass(pkey)
{
}

CDH_KEY::~CDH_KEY()
{
	free();
}

Ptr(CDH_KEY) CDH_KEY::create()
{
	Ptr(DH) pDH = DH_new();

	if (!pDH)
		return NULL;

	Ptr(CDH_KEY) p = OK_NEW_OPERATOR CDH_KEY(pDH);

	p->insert_obj();
	return p;
}

void CDH_KEY::free()
{
	DH_free(CastAnyPtr(DH, _raw));
	_raw = NULL;
}

CEC_KEY::CEC_KEY(ConstPointer pkey) : COpenSSLClass(pkey)
{
}

CEC_KEY::~CEC_KEY()
{
}

Ptr(CEC_KEY) CEC_KEY::create()
{
	Ptr(EC_KEY) pEC = EC_KEY_new();

	if (!pEC)
		return NULL;

	Ptr(CEC_KEY) p = OK_NEW_OPERATOR CEC_KEY(pEC);

	p->insert_obj();
	return p;
}

void CEC_KEY::free()
{
	EC_KEY_free(CastAnyPtr(EC_KEY, _raw));
	_raw = NULL;
}

CEVP_PKEY::CEVP_PKEY(ConstPointer pkey) : COpenSSLClass(pkey)
{
}

CEVP_PKEY::~CEVP_PKEY()
{
}

Ptr(CEVP_PKEY) CEVP_PKEY::create()
{
	Ptr(EVP_PKEY) pPKEY = EVP_PKEY_new();

	if (!pPKEY)
		return NULL;

	Ptr(CEVP_PKEY) p = OK_NEW_OPERATOR CEVP_PKEY(pPKEY);

	p->insert_obj();
	return p;
}

void CEVP_PKEY::free()
{
	EVP_PKEY_free(CastAnyPtr(EVP_PKEY, _raw));
	_raw = NULL;
}

int CEVP_PKEY::type()
{
	return EVP_PKEY_type(CastAnyPtr(EVP_PKEY, _raw)->type);
}

int CEVP_PKEY::size()
{
	return EVP_PKEY_size(CastAnyPtr(EVP_PKEY, _raw));
}

int CEVP_PKEY::set_RSA(Ptr(CRSA_KEY) key)
{
	return EVP_PKEY_assign_RSA(CastAnyPtr(EVP_PKEY, _raw), key->get_key());
}

Ptr(CRSA_KEY) CEVP_PKEY::get_RSA()
{
	Ptr(RSA) pRSA = EVP_PKEY_get1_RSA(CastAnyPtr(EVP_PKEY, _raw));

	if (!pRSA)
		return NULL;

	Ptr(CRSA_KEY) p = CastDynamicPtr(CRSA_KEY, find_obj(pRSA));

	if (p)
		return p;

	p = OK_NEW_OPERATOR CRSA_KEY(pRSA);
	p->insert_obj();
	return p;
}

int CEVP_PKEY::set_DSA(Ptr(CDSA_KEY) key)
{
	return EVP_PKEY_assign_DSA(CastAnyPtr(EVP_PKEY, _raw), key->get_key());
}

Ptr(CDSA_KEY) CEVP_PKEY::get_DSA()
{
	Ptr(DSA) pDSA = EVP_PKEY_get1_DSA(CastAnyPtr(EVP_PKEY, _raw));

	if (!pDSA)
		return NULL;

	Ptr(CDSA_KEY) p = CastDynamicPtr(CDSA_KEY, find_obj(pDSA));

	if (p)
		return p;

	p = OK_NEW_OPERATOR CDSA_KEY(pDSA);
	p->insert_obj();
	return p;
}

int CEVP_PKEY::set_DH(Ptr(CDH_KEY) key)
{
	return EVP_PKEY_assign_DH(CastAnyPtr(EVP_PKEY, _raw), key->get_key());
}

Ptr(CDH_KEY) CEVP_PKEY::get_DH()
{
	Ptr(DH) pDH = EVP_PKEY_get1_DH(CastAnyPtr(EVP_PKEY, _raw));

	if (!pDH)
		return NULL;

	Ptr(CDH_KEY) p = CastDynamicPtr(CDH_KEY, find_obj(pDH));

	if (p)
		return p;

	p = OK_NEW_OPERATOR CDH_KEY(pDH);
	p->insert_obj();
	return p;
}

int CEVP_PKEY::set_EC_KEY(Ptr(CEC_KEY) key)
{
	return EVP_PKEY_assign_EC_KEY(CastAnyPtr(EVP_PKEY, _raw), key->get_key());
}

Ptr(CEC_KEY) CEVP_PKEY::get_EC_KEY()
{
	Ptr(EC_KEY) pEC = EVP_PKEY_get1_EC_KEY(CastAnyPtr(EVP_PKEY, _raw));

	if (!pEC)
		return NULL;

	Ptr(CEC_KEY) p = CastDynamicPtr(CEC_KEY, find_obj(pEC));

	if (p)
		return p;

	p = OK_NEW_OPERATOR CEC_KEY(pEC);
	p->insert_obj();
	return p;
}

class OPENSSL_LOCAL TCallbackData : public CCppObject
{
public:
	TCallbackData() {}
	virtual ~TCallbackData() {}

	CEVP_PKEY_CTX::TCallback cb;
	void *arg;
};

CEVP_PKEY_CTX::CEVP_PKEY_CTX(ConstPointer pcontext) : COpenSSLClass(pcontext)
{
}

CEVP_PKEY_CTX::~CEVP_PKEY_CTX()
{
	free();
}

Ptr(CEVP_PKEY_CTX) CEVP_PKEY_CTX::create(Ptr(CEVP_PKEY) pkey, Ptr(CENGINE) e)
{
	Ptr(EVP_PKEY_CTX) pPKEYCtx = EVP_PKEY_CTX_new(CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pkey->get_key())), CastAnyPtr(ENGINE, CastMutable(Pointer, e->get_impl())));

	if (!pPKEYCtx)
		return NULL;

	Ptr(CEVP_PKEY_CTX) p = OK_NEW_OPERATOR CEVP_PKEY_CTX(pPKEYCtx);

	p->insert_obj();
	return p;
}

Ptr(CEVP_PKEY_CTX) CEVP_PKEY_CTX::create(int id, Ptr(CENGINE) e)
{
	Ptr(EVP_PKEY_CTX) pPKEYCtx = EVP_PKEY_CTX_new_id(id, CastAnyPtr(ENGINE, CastMutable(Pointer, e->get_impl())));

	if (!pPKEYCtx)
		return NULL;

	Ptr(CEVP_PKEY_CTX) p = OK_NEW_OPERATOR CEVP_PKEY_CTX(pPKEYCtx);

	p->insert_obj();
	return p;
}

Ptr(CEVP_PKEY_CTX) CEVP_PKEY_CTX::dup()
{
	Ptr(EVP_PKEY_CTX) pPKEYCtx = EVP_PKEY_CTX_dup(CastAnyPtr(EVP_PKEY_CTX, _raw));

	if (!pPKEYCtx)
		return NULL;

	Ptr(CEVP_PKEY_CTX) p = OK_NEW_OPERATOR CEVP_PKEY_CTX(pPKEYCtx);

	p->insert_obj();
	return p;
}

void CEVP_PKEY_CTX::free()
{
	EVP_PKEY_CTX_free(CastAnyPtr(EVP_PKEY_CTX, _raw));
	_raw = NULL;
}

int CEVP_PKEY_CTX::keygen_init()
{
	return EVP_PKEY_keygen_init(CastAnyPtr(EVP_PKEY_CTX, _raw));
}

int CEVP_PKEY_CTX::keygen(CEVP_PKEY **ppkey)
{
	Ptr(EVP_PKEY) pKey;
	int ret;

	ret = EVP_PKEY_keygen(CastAnyPtr(EVP_PKEY_CTX, _raw), &pKey);
	if (!pKey)
		return ret;
	*ppkey = OK_NEW_OPERATOR CEVP_PKEY(pKey);
	(*ppkey)->insert_obj();
	return ret;
}

int CEVP_PKEY_CTX::paramgen_init()
{
	return EVP_PKEY_paramgen_init(CastAnyPtr(EVP_PKEY_CTX, _raw));
}

int CEVP_PKEY_CTX::paramgen(CEVP_PKEY **ppkey)
{
	Ptr(EVP_PKEY) pKey;
	int ret;

	ret = EVP_PKEY_paramgen(CastAnyPtr(EVP_PKEY_CTX, _raw), &pKey);
	if (!pKey)
		return ret;
	*ppkey = OK_NEW_OPERATOR CEVP_PKEY(pKey);
	(*ppkey)->insert_obj();
	return ret;
}

static int stdcallback(EVP_PKEY_CTX *ctx)
{
	TCallbackData* cbdata = CastAnyPtr(TCallbackData, EVP_PKEY_CTX_get_app_data(ctx));
	CEVP_PKEY_CTX* pCtx = CastDynamicPtr(CEVP_PKEY_CTX, COpenSSLClass::find_obj(ctx));
	int ret;

	if (!pCtx)
	{
		pCtx = OK_NEW_OPERATOR CEVP_PKEY_CTX(ctx);
		pCtx->insert_obj();
	}
	EVP_PKEY_CTX_set_app_data(ctx, cbdata->arg);
	ret = (cbdata->cb)(pCtx);
	EVP_PKEY_CTX_set_app_data(ctx, cbdata);
	return ret;
}

void CEVP_PKEY_CTX::set_cb(TCallback cb)
{
	if (cb)
	{
		TCallbackData* cbdata = OK_NEW_OPERATOR TCallbackData;

		cbdata->cb = cb;
		cbdata->arg = NULL;
		EVP_PKEY_CTX_set_app_data(CastAnyPtr(EVP_PKEY_CTX, _raw), cbdata);
		EVP_PKEY_CTX_set_cb(CastAnyPtr(EVP_PKEY_CTX, _raw), stdcallback);
	}
	else
	{
		TCallbackData* cbdata = CastAnyPtr(TCallbackData, EVP_PKEY_CTX_get_app_data(CastAnyPtr(EVP_PKEY_CTX, _raw)));

		cbdata->release();
		EVP_PKEY_CTX_set_app_data(CastAnyPtr(EVP_PKEY_CTX, _raw), NULL);
		EVP_PKEY_CTX_set_cb(CastAnyPtr(EVP_PKEY_CTX, _raw), NULL);
	}
}

CEVP_PKEY_CTX::TCallback CEVP_PKEY_CTX::get_cb()
{
	TCallbackData* cbdata = CastAnyPtr(TCallbackData, EVP_PKEY_CTX_get_app_data(CastAnyPtr(EVP_PKEY_CTX, _raw)));

	if (cbdata)
		return cbdata->cb;
	return NULL;
}

int CEVP_PKEY_CTX::get_keygen_info(int idx)
{
	return EVP_PKEY_CTX_get_keygen_info(CastAnyPtr(EVP_PKEY_CTX, _raw), idx);
}


void CEVP_PKEY_CTX::set_app_data(void *data)
{
	TCallbackData* cbdata = CastAnyPtr(TCallbackData, EVP_PKEY_CTX_get_app_data(CastAnyPtr(EVP_PKEY_CTX, _raw)));

	if (cbdata)
		cbdata->arg = data;
}

void *CEVP_PKEY_CTX::get_app_data()
{
	TCallbackData* cbdata = CastAnyPtr(TCallbackData, EVP_PKEY_CTX_get_app_data(CastAnyPtr(EVP_PKEY_CTX, _raw)));

	if (cbdata)
		return cbdata->arg;
	return NULL;
}

int CEVP_PKEY_CTX::decrypt_init()
{
	return EVP_PKEY_decrypt_init(CastAnyPtr(EVP_PKEY_CTX, _raw));
}

int CEVP_PKEY_CTX::decrypt(unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen)
{
	return EVP_PKEY_decrypt(CastAnyPtr(EVP_PKEY_CTX, _raw), out, outlen, in, inlen);
}

int CEVP_PKEY_CTX::encrypt_init()
{
	return EVP_PKEY_encrypt_init(CastAnyPtr(EVP_PKEY_CTX, _raw));
}

int CEVP_PKEY_CTX::encrypt(unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen)
{
	return EVP_PKEY_encrypt(CastAnyPtr(EVP_PKEY_CTX, _raw), out, outlen, in, inlen);
}

int CEVP_PKEY_CTX::sign_init()
{
	return EVP_PKEY_sign_init(CastAnyPtr(EVP_PKEY_CTX, _raw));
}

int CEVP_PKEY_CTX::sign(unsigned char *sig, size_t *siglen, const unsigned char *tbs, size_t tbslen)
{
	return EVP_PKEY_sign(CastAnyPtr(EVP_PKEY_CTX, _raw), sig, siglen, tbs, tbslen);
}

int CEVP_PKEY_CTX::verify_init()
{
	return EVP_PKEY_verify_init(CastAnyPtr(EVP_PKEY_CTX, _raw));
}

int CEVP_PKEY_CTX::verify(const unsigned char *sig, size_t siglen, const unsigned char *tbs, size_t tbslen)
{
	return EVP_PKEY_verify(CastAnyPtr(EVP_PKEY_CTX, _raw), sig, siglen, tbs, tbslen);
}

enum TPadding
{
	PKCS1_Padding = 1,
	SSLV23_Padding,
	NoPadding,
	PKCS1_OAEP_Padding,
	X931_Padding,
	PKCS1_PSS_Padding
};

int CEVP_PKEY_CTX::set_rsa_padding(TPadding padding)
{
	return EVP_PKEY_CTX_set_rsa_padding(CastAnyPtr(EVP_PKEY_CTX, _raw), padding);
}

int CEVP_PKEY_CTX::get_rsa_padding(TPadding* ppadding)
{
	return EVP_PKEY_CTX_get_rsa_padding(CastAnyPtr(EVP_PKEY_CTX, _raw), ppadding);
}

int CEVP_PKEY_CTX::set_rsa_pss_saltlen(int saltlen)
{
	return EVP_PKEY_CTX_set_rsa_pss_saltlen(CastAnyPtr(EVP_PKEY_CTX, _raw), saltlen);
}

int CEVP_PKEY_CTX::get_rsa_pss_saltlen(int* psaltlen)
{
	return EVP_PKEY_CTX_get_rsa_pss_saltlen(CastAnyPtr(EVP_PKEY_CTX, _raw), psaltlen);
}

int CEVP_PKEY_CTX::set_rsa_keygen_bits(int bits)
{
	return EVP_PKEY_CTX_set_rsa_keygen_bits(CastAnyPtr(EVP_PKEY_CTX, _raw), bits);
}

int CEVP_PKEY_CTX::set_rsa_keygen_pubexp(Ptr(CBIGNUM) pubexp)
{
	return EVP_PKEY_CTX_set_rsa_keygen_pubexp(CastAnyPtr(EVP_PKEY_CTX, _raw), CastMutable(Pointer, pubexp->get_bignum()));
}

int CEVP_PKEY_CTX::set_rsa_mgf1_md(Ptr(CEVP_MD) md)
{
	return EVP_PKEY_CTX_set_rsa_mgf1_md(CastAnyPtr(EVP_PKEY_CTX, _raw), md->get_md());
}

int CEVP_PKEY_CTX::get_rsa_mgf1_md(PtrPtr(CEVP_MD) pmd)
{
	Ptr(EVP_MD) pMD = NULL;
	int ret = EVP_PKEY_CTX_get_rsa_mgf1_md(CastAnyPtr(EVP_PKEY_CTX, _raw), &pMD);

	if (!pMD)
		return ret;

	Ptr(CEVP_MD) p = CastDynamicPtr(CEVP_MD, find_obj(pMD));

	if (!p)
	{
		p = OK_NEW_OPERATOR CEVP_MD(pMD);
		p->insert_obj();
	}
	*pmd = p;
	return ret;
}

