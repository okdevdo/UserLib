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

class OPENSSL_API CRSA_METHOD : public COpenSSLClass
{
public:
	CRSA_METHOD(ConstPointer pMethod = NULL);
	virtual ~CRSA_METHOD();

	__inline ConstPointer get_method() const { return _raw; }

	static Ptr(CRSA_METHOD) create_PKCS1_SSLeay();
	static Ptr(CRSA_METHOD) create_null_method();

	enum TFlags
	{
		FlagNoCheck = 0x0001,
		FlagCachePublic = 0x0002,
		FlagCachePrivate = 0x0004,
		FlagBlinding = 0x0008,
		FlagThreadSafe = 0x0010,
		FlagExtPKey = 0x0020,
		FlagSignVerify = 0x0040,
		FlagNoBlinding = 0x0080,
		FlagNoConstTime = 0x0100
	};

	int get_flags();
	void set_flags(int);
};

class CBIO;
class CEVP_MD;
class OPENSSL_API CRSA_KEY : public COpenSSLClass
{
public:
	typedef int(*TCallback1)(char *buf, int len, const char *prompt, int verify);

	CRSA_KEY(ConstPointer pkey = NULL);
	virtual ~CRSA_KEY();

	__inline ConstPointer get_key() const { return _raw; }

	static Ptr(CRSA_KEY) create();
	static Ptr(CRSA_KEY) create_method(Ptr(CENGINE) engine);

	void free();

	enum TFlags
	{
		FlagNoCheck = 0x0001,
		FlagCachePublic = 0x0002,
		FlagCachePrivate = 0x0004,
		FlagBlinding = 0x0008,
		FlagThreadSafe = 0x0010,
		FlagExtPKey = 0x0020,
		FlagSignVerify = 0x0040,
		FlagNoBlinding = 0x0080,
		FlagNoConstTime = 0x0100
	};

	int get_flags();
	void set_flags(int);

	void set_app_data(void* arg);
	void *get_app_data(void);

	int size();

	int	generate_key_ex(int bits, CBIGNUM *e, CBN_GENCB *cb);

	int	check_key();

	enum TPadding
	{
		PKCS1_PADDING = 1,
		SSLV23_PADDING,
		NO_PADDING,
		PKCS1_OAEP_PADDING,
		X931_PADDING,
		PKCS1_PSS_PADDING
	};

	int	public_encrypt(int flen, const unsigned char *from, unsigned char *to, TPadding padding);
	int	private_encrypt(int flen, const unsigned char *from, unsigned char *to, TPadding padding);
	int	public_decrypt(int flen, const unsigned char *from, unsigned char *to, TPadding padding);
	int	private_decrypt(int flen, const unsigned char *from, unsigned char *to, TPadding padding);

	static void set_default_method(const CRSA_METHOD *meth);
	static const CRSA_METHOD *get_default_method(void);
	const CRSA_METHOD *get_method();
	int set_method(const CRSA_METHOD *meth);

	static Ptr(CRSA_KEY) d2i_PublicKey(const unsigned char **in, long len);
	int i2d_PublicKey(unsigned char **out);
	static Ptr(CRSA_KEY) d2i_PrivateKey(const unsigned char **in, long len);
	int i2d_PrivateKey(unsigned char **out);

	int	print_fp(FILE *fp, int offset);
	int	print(CBIO *bp, int offset);
	int i2d_NET(unsigned char **pp, TCallback1 cb, int sgckey);
	static Ptr(CRSA_KEY) d2i_NET(const unsigned char **pp, long length, TCallback1 cb, int sgckey);
	int i2d_Netscape(unsigned char **pp, TCallback1 cb);
	static Ptr(CRSA_KEY) d2i_Netscape(const unsigned char **pp, long length, TCallback1 cb);

	int sign(int type, const unsigned char *m, unsigned int m_length, unsigned char *sigret, unsigned int *siglen);
	int verify(int type, const unsigned char *m, unsigned int m_length, const unsigned char *sigbuf, unsigned int siglen);
	int sign_ASN1_OCTET_STRING(int type, const unsigned char *m, unsigned int m_length, unsigned char *sigret, unsigned int *siglen);
	int verify_ASN1_OCTET_STRING(int type, const unsigned char *m, unsigned int m_length, unsigned char *sigbuf, unsigned int siglen);

	int blinding_on(CBN_CTX *ctx);
	void blinding_off();
	CBN_BLINDING *setup_blinding(CBN_CTX *ctx);

	static int padding_add_PKCS1_type_1(unsigned char *to, int tlen, const unsigned char *f, int fl);
	static int padding_check_PKCS1_type_1(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len);
	static int padding_add_PKCS1_type_2(unsigned char *to, int tlen, const unsigned char *f, int fl);
	static int padding_check_PKCS1_type_2(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len);
	static int padding_add_PKCS1_OAEP(unsigned char *to, int tlen, const unsigned char *f, int fl, const unsigned char *p, int pl);
	static int padding_check_PKCS1_OAEP(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len, const unsigned char *p, int pl);
	static int padding_add_SSLv23(unsigned char *to, int tlen, const unsigned char *f, int fl);
	static int padding_check_SSLv23(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len);
	static int padding_add_none(unsigned char *to, int tlen, const unsigned char *f, int fl);
	static int padding_check_none(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len);
	static int padding_add_X931(unsigned char *to, int tlen, const unsigned char *f, int fl);
	static int padding_check_X931(unsigned char *to, int tlen, const unsigned char *f, int fl, int rsa_len);

	int X931_hash_id(int nid);
	int verify_PKCS1_PSS(const unsigned char *mHash, const CEVP_MD *Hash, const unsigned char *EM, int sLen); 
	int padding_add_PKCS1_PSS(unsigned char *EM, const unsigned char *mHash, const CEVP_MD *Hash, int sLen);
	int verify_PKCS1_PSS_mgf1(const unsigned char *mHash, const CEVP_MD *Hash, const CEVP_MD *mgf1Hash, const unsigned char *EM, int sLen);
	int padding_add_PKCS1_PSS_mgf1(unsigned char *EM, const unsigned char *mHash, const CEVP_MD *Hash, const CEVP_MD *mgf1Hash, int sLen);

	static int get_ex_new_index(long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func);
	int set_ex_data(int idx, void *arg);
	void *get_ex_data(int idx);

	Ptr(CRSA_KEY) PublicKey_dup();
	Ptr(CRSA_KEY) PrivateKey_dup();
};

class OPENSSL_API CDSA_KEY : public COpenSSLClass
{
public:
	CDSA_KEY(ConstPointer pkey = NULL);
	virtual ~CDSA_KEY();

	__inline ConstPointer get_key() const { return _raw; }

	static Ptr(CDSA_KEY) create();

	void free();
};

class OPENSSL_API CDH_KEY : public COpenSSLClass
{
public:
	CDH_KEY(ConstPointer pkey = NULL);
	virtual ~CDH_KEY();

	__inline ConstPointer get_key() const { return _raw; }

	static Ptr(CDH_KEY) create();

	void free();
};

class OPENSSL_API CEC_KEY : public COpenSSLClass
{
public:
	CEC_KEY(ConstPointer pkey = NULL);
	virtual ~CEC_KEY();

	__inline ConstPointer get_key() const { return _raw; }

	static Ptr(CEC_KEY) create();

	void free();
};

class OPENSSL_API CEVP_PKEY : public COpenSSLClass
{
public:
	CEVP_PKEY(ConstPointer pkey = NULL);
	virtual ~CEVP_PKEY();

	__inline ConstPointer get_key() const { return _raw; }

	static Ptr(CEVP_PKEY) create();

	void free();

	int type();
	int size();

	int set_RSA(Ptr(CRSA_KEY) key);
	Ptr(CRSA_KEY) get_RSA();
	int set_DSA(Ptr(CDSA_KEY) key);
	Ptr(CDSA_KEY) get_DSA();
	int set_DH(Ptr(CDH_KEY) key);
	Ptr(CDH_KEY) get_DH();
	int set_EC_KEY(Ptr(CEC_KEY) key);
	Ptr(CEC_KEY) get_EC_KEY();
};

class CEVP_MD;
class OPENSSL_API CEVP_PKEY_CTX : public COpenSSLClass
{
public:
	CEVP_PKEY_CTX(ConstPointer pcontext = NULL);
	virtual ~CEVP_PKEY_CTX();

	__inline ConstPointer get_context() const { return _raw; }

	static Ptr(CEVP_PKEY_CTX) create(Ptr(CEVP_PKEY) pkey, Ptr(CENGINE) e);
	static Ptr(CEVP_PKEY_CTX) create(int id, Ptr(CENGINE) e);

	Ptr(CEVP_PKEY_CTX) dup();
	void free();

	// keygen
	int keygen_init();
	int keygen(CEVP_PKEY **ppkey);
	int paramgen_init();
	int paramgen(CEVP_PKEY **ppkey);

	typedef int (*TCallback)(CEVP_PKEY_CTX *ctx);

	void set_cb(TCallback cb);
	TCallback get_cb();

	int get_keygen_info(int idx);

	void set_app_data(void *data);
	void *get_app_data();

	// decrypt using a public key algorithm
	int decrypt_init();
	int decrypt(unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen);

	// encrypt using a public key algorithm
	int encrypt_init();
	int encrypt(unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen);

	// signing using a public key algorithm
	int sign_init();
	int sign(unsigned char *sig, size_t *siglen, const unsigned char *tbs, size_t tbslen);

	// signature verification using a public key algorithm 
	int verify_init();
	int verify(const unsigned char *sig, size_t siglen, const unsigned char *tbs, size_t tbslen);

	enum TPadding
	{
		PKCS1_Padding = 1,
		SSLV23_Padding,
		NoPadding,
		PKCS1_OAEP_Padding,
		X931_Padding,
		PKCS1_PSS_Padding
	};

	int set_rsa_padding(TPadding);
	int get_rsa_padding(TPadding*);

	int set_rsa_pss_saltlen(int);
	int get_rsa_pss_saltlen(int*);

	int set_rsa_keygen_bits(int);
	int set_rsa_keygen_pubexp(Ptr(CBIGNUM));

	int set_rsa_mgf1_md(Ptr(CEVP_MD));
	int get_rsa_mgf1_md(PtrPtr(CEVP_MD));
};
