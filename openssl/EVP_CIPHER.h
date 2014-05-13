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

class OPENSSL_API CEVP_CIPHER : public COpenSSLClass
{
public:
	CEVP_CIPHER(ConstPointer cipher = NULL);
	virtual ~CEVP_CIPHER();

	__inline ConstPointer get_cipher() const { return _raw; }

	static Ptr(CEVP_CIPHER) create_enc_null(void);
	static Ptr(CEVP_CIPHER) create_des_cbc(void);
	static Ptr(CEVP_CIPHER) create_des_ecb(void);
	static Ptr(CEVP_CIPHER) create_des_cfb(void);
	static Ptr(CEVP_CIPHER) create_des_ofb(void);
	static Ptr(CEVP_CIPHER) create_des_ede_cbc(void);
	static Ptr(CEVP_CIPHER) create_des_ede(void);
	static Ptr(CEVP_CIPHER) create_des_ede_ofb(void);
	static Ptr(CEVP_CIPHER) create_des_ede_cfb(void);
	static Ptr(CEVP_CIPHER) create_des_ede3_cbc(void);
	static Ptr(CEVP_CIPHER) create_des_ede3(void);
	static Ptr(CEVP_CIPHER) create_des_ede3_ofb(void);
	static Ptr(CEVP_CIPHER) create_des_ede3_cfb(void);
	static Ptr(CEVP_CIPHER) create_desx_cbc(void);
	static Ptr(CEVP_CIPHER) create_rc4(void);
	static Ptr(CEVP_CIPHER) create_rc4_40(void);
	static Ptr(CEVP_CIPHER) create_idea_cbc(void); 
	static Ptr(CEVP_CIPHER) create_idea_ecb(void);
	static Ptr(CEVP_CIPHER) create_idea_cfb(void);
	static Ptr(CEVP_CIPHER) create_idea_ofb(void);
	static Ptr(CEVP_CIPHER) create_rc2_cbc(void);
	static Ptr(CEVP_CIPHER) create_rc2_ecb(void);
	static Ptr(CEVP_CIPHER) create_rc2_cfb(void);
	static Ptr(CEVP_CIPHER) create_rc2_ofb(void);
	static Ptr(CEVP_CIPHER) create_rc2_40_cbc(void);
	static Ptr(CEVP_CIPHER) create_rc2_64_cbc(void);
	static Ptr(CEVP_CIPHER) create_bf_cbc(void);
	static Ptr(CEVP_CIPHER) create_bf_ecb(void);
	static Ptr(CEVP_CIPHER) create_bf_cfb(void);
	static Ptr(CEVP_CIPHER) create_bf_ofb(void);
	static Ptr(CEVP_CIPHER) create_cast5_cbc(void);
	static Ptr(CEVP_CIPHER) create_cast5_ecb(void);
	static Ptr(CEVP_CIPHER) create_cast5_cfb(void);
	static Ptr(CEVP_CIPHER) create_cast5_ofb(void);
	static Ptr(CEVP_CIPHER) create_aes_128_gcm(void);
	static Ptr(CEVP_CIPHER) create_aes_192_gcm(void);
	static Ptr(CEVP_CIPHER) create_aes_256_gcm(void);
	static Ptr(CEVP_CIPHER) create_aes_128_ccm(void);
	static Ptr(CEVP_CIPHER) create_aes_192_ccm(void);
	static Ptr(CEVP_CIPHER) create_aes_256_ccm(void);

	static ConstPtr(CEVP_CIPHER) get_cipherbyname(const char *name);
	static ConstPtr(CEVP_CIPHER) get_cipherbynid(int nid);

	int nid();
	int block_size();
	int key_length();
	int iv_length();
	unsigned long flags();
	unsigned long mode();
	int type();
};

class CEVP_PKEY;
class OPENSSL_API CEVP_CIPHER_CTX : public COpenSSLClass
{
public:
	CEVP_CIPHER_CTX(ConstPointer cipherCtx = NULL);
	virtual ~CEVP_CIPHER_CTX();

	__inline ConstPointer get_cipherContext() const { return _raw; }

	static Ptr(CEVP_CIPHER_CTX) create();

	void free();

	void init();

	int EncryptInit(const CEVP_CIPHER *type, unsigned char *key, unsigned char *iv);
	int EncryptInit_ex(const CEVP_CIPHER *type, CENGINE *impl, unsigned char *key, unsigned char *iv);
	int EncryptUpdate(unsigned char *out, int *outl, unsigned char *in, int inl);
	int EncryptFinal(unsigned char *out, int *outl);
	int EncryptFinal_ex(unsigned char *out, int *outl);

	int DecryptInit(const CEVP_CIPHER *type, unsigned char *key, unsigned char *iv);
	int DecryptInit_ex(const CEVP_CIPHER *type, CENGINE *impl, unsigned char *key, unsigned char *iv);
	int DecryptUpdate(unsigned char *out, int *outl, unsigned char *in, int inl);
	int DecryptFinal(unsigned char *outm, int *outl);
	int DecryptFinal_ex(unsigned char *outm, int *outl);

	int CipherInit(const CEVP_CIPHER *type, unsigned char *key, unsigned char *iv, int enc);
	int CipherInit_ex(const CEVP_CIPHER *type, CENGINE *impl, unsigned char *key, unsigned char *iv, int enc);
	int CipherUpdate(unsigned char *out, int *outl, unsigned char *in, int inl);
	int CipherFinal(unsigned char *outm, int *outl);
	int CipherFinal_ex(unsigned char *outm, int *outl);

	int OpenInit(CEVP_CIPHER *type, unsigned char *ek, int ekl, unsigned char *iv, CEVP_PKEY *priv);
	int OpenUpdate(unsigned char *out, int *outl, unsigned char *in, int inl);
	int OpenFinal(unsigned char *out, int *outl);

	int SealInit(const CEVP_CIPHER *type, unsigned char **ek, int *ekl, unsigned char *iv, CEVP_PKEY **pubk, int npubk);
	int SealUpdate(unsigned char *out, int *outl, unsigned char *in, int inl);
	int SealFinal(unsigned char *out, int *outl);

	int set_padding(int padding);
	int set_key_length(int keylen);
	int ctrl(int type, int arg, void *ptr);
	int cleanup();

	ConstPtr(CEVP_CIPHER) cipher();
	int nid();
	int block_size();
	int key_length();
	int iv_length();
	void* get_app_data();
	void set_app_data(void *d);
	int type();
	unsigned long flags();
	unsigned long mode();
};

