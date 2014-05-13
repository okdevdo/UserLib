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
#include "EVP_CIPHER.h"
#include "EVP_PKEY.h"
#include <openssl/evp.h>

CEVP_CIPHER::CEVP_CIPHER(ConstPointer cipher) : COpenSSLClass(cipher) {}
CEVP_CIPHER::~CEVP_CIPHER() {}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_enc_null(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_enc_null();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ecb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ecb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_cfb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_cfb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ofb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ofb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ede_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ede_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ede(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ede();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ede_ofb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ede_ofb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ede_cfb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ede_cfb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ede3_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ede3_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ede3(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ede3();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ede3_ofb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ede3_ofb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_des_ede3_cfb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_des_ede3_cfb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_desx_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_desx_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_rc4(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_rc4();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_rc4_40(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_rc4_40();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_idea_cbc(void)
{
#ifndef OPENSSL_NO_IDEA
	ConstPtr(EVP_CIPHER) pCipher = EVP_idea_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
#else
	return NULL;
#endif
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_idea_ecb(void)
{
#ifndef OPENSSL_NO_IDEA
	ConstPtr(EVP_CIPHER) pCipher = EVP_idea_ecb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
#else
	return NULL;
#endif
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_idea_cfb(void)
{
#ifndef OPENSSL_NO_IDEA
	ConstPtr(EVP_CIPHER) pCipher = EVP_idea_cfb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
#else
	return NULL;
#endif
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_idea_ofb(void)
{
#ifndef OPENSSL_NO_IDEA
	ConstPtr(EVP_CIPHER) pCipher = EVP_idea_ofb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
#else
	return NULL;
#endif
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_rc2_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_rc2_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_rc2_ecb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_rc2_ecb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_rc2_cfb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_rc2_cfb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_rc2_ofb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_rc2_ofb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_rc2_40_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_rc2_40_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_rc2_64_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_rc2_64_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_bf_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_bf_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_bf_ecb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_bf_ecb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_bf_cfb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_bf_cfb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_bf_ofb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_bf_ofb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_cast5_cbc(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_cast5_cbc();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_cast5_ecb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_cast5_ecb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_cast5_cfb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_cast5_cfb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_cast5_ofb(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_cast5_ofb();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_aes_128_gcm(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_aes_128_gcm();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_aes_192_gcm(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_aes_192_gcm();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_aes_256_gcm(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_aes_256_gcm();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_aes_128_ccm(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_aes_128_ccm();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_aes_192_ccm(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_aes_192_ccm();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

Ptr(CEVP_CIPHER) CEVP_CIPHER::create_aes_256_ccm(void)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_aes_256_ccm();

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_CIPHER) CEVP_CIPHER::get_cipherbyname(const char *name)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_get_cipherbyname(name);

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_CIPHER) CEVP_CIPHER::get_cipherbynid(int nid)
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_get_cipherbynid(nid);

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);

	p->insert_obj();
	return p;
}

int CEVP_CIPHER::nid()
{
	return EVP_CIPHER_nid(CastAnyPtr(EVP_CIPHER, _raw));
}

int CEVP_CIPHER::block_size()
{
	return EVP_CIPHER_block_size(CastAnyPtr(EVP_CIPHER, _raw));
}

int CEVP_CIPHER::key_length()
{
	return EVP_CIPHER_key_length(CastAnyPtr(EVP_CIPHER, _raw));
}

int CEVP_CIPHER::iv_length()
{
	return EVP_CIPHER_iv_length(CastAnyPtr(EVP_CIPHER, _raw));
}

unsigned long CEVP_CIPHER::flags()
{
	return EVP_CIPHER_flags(CastAnyPtr(EVP_CIPHER, _raw));
}

unsigned long CEVP_CIPHER::mode()
{
	return EVP_CIPHER_mode(CastAnyPtr(EVP_CIPHER, _raw));
}

int CEVP_CIPHER::type()
{
	return EVP_CIPHER_type(CastAnyPtr(EVP_CIPHER, _raw));
}

CEVP_CIPHER_CTX::CEVP_CIPHER_CTX(ConstPointer cipherCtx) : COpenSSLClass(cipherCtx) {}
CEVP_CIPHER_CTX::~CEVP_CIPHER_CTX() { free(); }

Ptr(CEVP_CIPHER_CTX) CEVP_CIPHER_CTX::create()
{
	Ptr(EVP_CIPHER_CTX) pCipherCtx = EVP_CIPHER_CTX_new();

	if (!pCipherCtx)
		return NULL;

	Ptr(CEVP_CIPHER_CTX) p = OK_NEW_OPERATOR CEVP_CIPHER_CTX(pCipherCtx);

	p->insert_obj();
	return p;
}

void CEVP_CIPHER_CTX::free()
{
	if (_raw)
		EVP_CIPHER_CTX_free(CastAnyPtr(EVP_CIPHER_CTX, _raw));
	_raw = NULL;
}

void CEVP_CIPHER_CTX::init()
{
	EVP_CIPHER_CTX_init(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

int CEVP_CIPHER_CTX::EncryptInit(const CEVP_CIPHER *type, unsigned char *key, unsigned char *iv)
{
	return EVP_EncryptInit(CastAnyPtr(EVP_CIPHER_CTX, _raw), CastAnyConstPtr(EVP_CIPHER, type->get_cipher()), key, iv);
}

int CEVP_CIPHER_CTX::EncryptInit_ex(const CEVP_CIPHER *type, CENGINE *impl, unsigned char *key, unsigned char *iv)
{
	return EVP_EncryptInit_ex(CastAnyPtr(EVP_CIPHER_CTX, _raw),
		CastAnyConstPtr(EVP_CIPHER, type->get_cipher()),
		CastAnyPtr(ENGINE, CastMutable(Pointer, impl->get_impl())), key, iv);
}

int CEVP_CIPHER_CTX::EncryptUpdate(unsigned char *out, int *outl, unsigned char *in, int inl)
{
	return EVP_EncryptUpdate(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl, in, inl);
}

int CEVP_CIPHER_CTX::EncryptFinal(unsigned char *out, int *outl)
{
	return EVP_EncryptFinal(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl);
}

int CEVP_CIPHER_CTX::EncryptFinal_ex(unsigned char *out, int *outl)
{
	return EVP_EncryptFinal_ex(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl);
}

int CEVP_CIPHER_CTX::DecryptInit(const CEVP_CIPHER *type, unsigned char *key, unsigned char *iv)
{
	return EVP_DecryptInit(CastAnyPtr(EVP_CIPHER_CTX, _raw), CastAnyConstPtr(EVP_CIPHER, type->get_cipher()), key, iv);
}

int CEVP_CIPHER_CTX::DecryptInit_ex(const CEVP_CIPHER *type, CENGINE *impl, unsigned char *key, unsigned char *iv)
{
	return EVP_DecryptInit_ex(CastAnyPtr(EVP_CIPHER_CTX, _raw),
		CastAnyConstPtr(EVP_CIPHER, type->get_cipher()),
		CastAnyPtr(ENGINE, CastMutable(Pointer, impl->get_impl())), key, iv);
}

int CEVP_CIPHER_CTX::DecryptUpdate(unsigned char *out, int *outl, unsigned char *in, int inl)
{
	return EVP_DecryptUpdate(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl, in, inl);
}

int CEVP_CIPHER_CTX::DecryptFinal(unsigned char *outm, int *outl)
{
	return EVP_DecryptFinal(CastAnyPtr(EVP_CIPHER_CTX, _raw), outm, outl);
}

int CEVP_CIPHER_CTX::DecryptFinal_ex(unsigned char *outm, int *outl)
{
	return EVP_DecryptFinal_ex(CastAnyPtr(EVP_CIPHER_CTX, _raw), outm, outl);
}

int CEVP_CIPHER_CTX::CipherInit(const CEVP_CIPHER *type, unsigned char *key, unsigned char *iv, int enc)
{
	return EVP_CipherInit(CastAnyPtr(EVP_CIPHER_CTX, _raw), CastAnyConstPtr(EVP_CIPHER, type->get_cipher()), key, iv, enc);
}

int CEVP_CIPHER_CTX::CipherInit_ex(const CEVP_CIPHER *type, CENGINE *impl, unsigned char *key, unsigned char *iv, int enc)
{
	return EVP_CipherInit_ex(CastAnyPtr(EVP_CIPHER_CTX, _raw), 
		CastAnyConstPtr(EVP_CIPHER, type->get_cipher()), 
		CastAnyPtr(ENGINE, CastMutable(Pointer, impl->get_impl())), key, iv, enc);
}

int CEVP_CIPHER_CTX::CipherUpdate(unsigned char *out, int *outl, unsigned char *in, int inl)
{
	return EVP_CipherUpdate(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl, in, inl);
}

int CEVP_CIPHER_CTX::CipherFinal(unsigned char *outm, int *outl)
{
	return EVP_CipherFinal(CastAnyPtr(EVP_CIPHER_CTX, _raw), outm, outl);
}

int CEVP_CIPHER_CTX::CipherFinal_ex(unsigned char *outm, int *outl)
{
	return EVP_CipherFinal_ex(CastAnyPtr(EVP_CIPHER_CTX, _raw), outm, outl);
}

int CEVP_CIPHER_CTX::OpenInit(CEVP_CIPHER *type, unsigned char *ek, int ekl, unsigned char *iv, CEVP_PKEY *priv)
{
	return EVP_OpenInit(CastAnyPtr(EVP_CIPHER_CTX, _raw), CastAnyPtr(EVP_CIPHER, CastMutable(Pointer, type->get_cipher())), ek, ekl, iv, CastAnyPtr(EVP_PKEY, CastMutable(Pointer, priv->get_key())));
}

int CEVP_CIPHER_CTX::OpenUpdate(unsigned char *out, int *outl, unsigned char *in, int inl)
{
	return EVP_OpenUpdate(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl, in, inl);
}

int CEVP_CIPHER_CTX::OpenFinal(unsigned char *out, int *outl)
{
	return EVP_OpenFinal(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl);
}

int CEVP_CIPHER_CTX::SealInit(const CEVP_CIPHER *type, unsigned char **ek, int *ekl, unsigned char *iv, CEVP_PKEY **pubk, int npubk)
{
	Ptr(Ptr(EVP_PKEY)) pKeys = CastAnyPtr(Ptr(EVP_PKEY), TFalloc(sizeof(Ptr(EVP_PKEY)) * npubk));
	int ret;

	for (int i = 0; i < npubk; ++i)
		pKeys[i] = CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pubk[i]->get_key()));
	ret = EVP_SealInit(CastAnyPtr(EVP_CIPHER_CTX, _raw), CastAnyPtr(EVP_CIPHER, CastMutable(Pointer, type->get_cipher())), ek, ekl, iv, pKeys, npubk);
	TFfree(pKeys);
	return ret;
}

int CEVP_CIPHER_CTX::SealUpdate(unsigned char *out, int *outl, unsigned char *in, int inl)
{
	return EVP_SealUpdate(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl, in, inl);
}

int CEVP_CIPHER_CTX::SealFinal(unsigned char *out, int *outl)
{
	return EVP_SealFinal(CastAnyPtr(EVP_CIPHER_CTX, _raw), out, outl);
}

int CEVP_CIPHER_CTX::set_padding(int padding)
{
	return EVP_CIPHER_CTX_set_padding(CastAnyPtr(EVP_CIPHER_CTX, _raw), padding);
}

int CEVP_CIPHER_CTX::set_key_length(int keylen)
{
	return EVP_CIPHER_CTX_set_key_length(CastAnyPtr(EVP_CIPHER_CTX, _raw), keylen);
}

int CEVP_CIPHER_CTX::ctrl(int type, int arg, void *ptr)
{
	return EVP_CIPHER_CTX_ctrl(CastAnyPtr(EVP_CIPHER_CTX, _raw), type, arg, ptr);
}

int CEVP_CIPHER_CTX::cleanup()
{
	return EVP_CIPHER_CTX_cleanup(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

ConstPtr(CEVP_CIPHER) CEVP_CIPHER_CTX::cipher()
{
	ConstPtr(EVP_CIPHER) pCipher = EVP_CIPHER_CTX_cipher(CastAnyPtr(EVP_CIPHER_CTX, _raw));

	if (!pCipher)
		return NULL;

	Ptr(CEVP_CIPHER) p = CastDynamicPtr(CEVP_CIPHER, find_obj(pCipher));

	if (!p)
	{
		p = OK_NEW_OPERATOR CEVP_CIPHER(pCipher);
		p->insert_obj();
	}
	return p;
}

int CEVP_CIPHER_CTX::nid()
{
	return EVP_CIPHER_CTX_nid(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

int CEVP_CIPHER_CTX::block_size()
{
	return EVP_CIPHER_CTX_block_size(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

int CEVP_CIPHER_CTX::key_length()
{
	return EVP_CIPHER_CTX_key_length(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

int CEVP_CIPHER_CTX::iv_length()
{
	return EVP_CIPHER_CTX_iv_length(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

void* CEVP_CIPHER_CTX::get_app_data()
{
	return EVP_CIPHER_CTX_get_app_data(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

void CEVP_CIPHER_CTX::set_app_data(void *d)
{
	EVP_CIPHER_CTX_set_app_data(CastAnyPtr(EVP_CIPHER_CTX, _raw), d);
}

int CEVP_CIPHER_CTX::type()
{
	return EVP_CIPHER_CTX_type(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

unsigned long CEVP_CIPHER_CTX::flags()
{
	return EVP_CIPHER_CTX_flags(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}

unsigned long CEVP_CIPHER_CTX::mode()
{
	return EVP_CIPHER_CTX_mode(CastAnyPtr(EVP_CIPHER_CTX, _raw));
}
