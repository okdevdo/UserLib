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
#include "EVP_MD.h"
#include "EVP_PKEY.h"
#include <openssl/bio.h>
#include <openssl/evp.h>

CEVP_MD::CEVP_MD(ConstPointer md) : COpenSSLClass(md) {}
CEVP_MD::~CEVP_MD() {}

ConstPtr(CEVP_MD) CEVP_MD::create_md_null()
{
	ConstPtr(EVP_MD) pMd = EVP_md_null();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_md4()
{
	ConstPtr(EVP_MD) pMd = EVP_md4();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_md5()
{
	ConstPtr(EVP_MD) pMd = EVP_md5();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_sha()
{
	ConstPtr(EVP_MD) pMd = EVP_sha();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_sha1()
{
	ConstPtr(EVP_MD) pMd = EVP_sha1();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_dss()
{
	ConstPtr(EVP_MD) pMd = EVP_dss();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_dss1()
{
	ConstPtr(EVP_MD) pMd = EVP_dss1();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_mdc2()
{
	ConstPtr(EVP_MD) pMd = EVP_mdc2();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_ripemd160()
{
	ConstPtr(EVP_MD) pMd = EVP_ripemd160();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_sha224()
{
	ConstPtr(EVP_MD) pMd = EVP_sha224();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_sha256()
{
	ConstPtr(EVP_MD) pMd = EVP_sha256();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_sha384()
{
	ConstPtr(EVP_MD) pMd = EVP_sha384();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::create_sha512()
{
	ConstPtr(EVP_MD) pMd = EVP_sha512();

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = OK_NEW_OPERATOR CEVP_MD(pMd);

	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::get_digestbyname(const char *name)
{
	ConstPtr(EVP_MD) pMd = EVP_get_digestbyname(name);

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = CastDynamicPtr(CEVP_MD, find_obj(pMd));

	p = OK_NEW_OPERATOR CEVP_MD(pMd);
	p->insert_obj();
	return p;
}

ConstPtr(CEVP_MD) CEVP_MD::get_digestbynid(int n)
{
	ConstPtr(EVP_MD) pMd = EVP_get_digestbynid(n);

	if (!pMd)
		return nullptr;

	Ptr(CEVP_MD) p = CastDynamicPtr(CEVP_MD, find_obj(pMd));

	p = OK_NEW_OPERATOR CEVP_MD(pMd);
	p->insert_obj();
	return p;
}

int CEVP_MD::type()
{
	return EVP_MD_type(CastAnyPtr(EVP_MD, _raw));
}

int CEVP_MD::pkey_type()
{
	return EVP_MD_pkey_type(CastAnyPtr(EVP_MD, _raw));
}

int CEVP_MD::size()
{
	return EVP_MD_size(CastAnyPtr(EVP_MD, _raw));
}

int CEVP_MD::block_size()
{
	return EVP_MD_block_size(CastAnyPtr(EVP_MD, _raw));
}

CEVP_MD_CTX::CEVP_MD_CTX(ConstPointer mdCtx) : COpenSSLClass(mdCtx) {}
CEVP_MD_CTX::~CEVP_MD_CTX() { destroy(); }

Ptr(CEVP_MD_CTX) CEVP_MD_CTX::create()
{
	Ptr(EVP_MD_CTX) pMdCtx = EVP_MD_CTX_create();

	if (!pMdCtx)
		return nullptr;

	Ptr(CEVP_MD_CTX) p = OK_NEW_OPERATOR CEVP_MD_CTX(pMdCtx);

	p->insert_obj();
	return p;
}

void CEVP_MD_CTX::init()
{
	EVP_MD_CTX_init(CastAnyPtr(EVP_MD_CTX, _raw));
}

int CEVP_MD_CTX::DigestInit(const CEVP_MD *type)
{
	return EVP_DigestInit(CastAnyPtr(EVP_MD_CTX, _raw), CastAnyConstPtr(EVP_MD, type->get_md()));
}

int CEVP_MD_CTX::DigestInit_ex(const CEVP_MD *type, CENGINE *impl)
{
	return EVP_DigestInit_ex(CastAnyPtr(EVP_MD_CTX, _raw), CastAnyConstPtr(EVP_MD, type->get_md()), (PtrCheck(impl)?nullptr:CastAnyPtr(ENGINE, CastMutable(Pointer, impl->get_impl()))));
}

int CEVP_MD_CTX::DigestUpdate(const void *d, size_t cnt)
{
	return EVP_DigestUpdate(CastAnyPtr(EVP_MD_CTX, _raw), d, cnt);
}

int CEVP_MD_CTX::DigestFinal(unsigned char *md, unsigned int *s)
{
	return EVP_DigestFinal(CastAnyPtr(EVP_MD_CTX, _raw), md, s);
}

int CEVP_MD_CTX::DigestFinal_ex(unsigned char *md, unsigned int *s)
{
	return EVP_DigestFinal_ex(CastAnyPtr(EVP_MD_CTX, _raw), md, s);
}

void CEVP_MD_CTX::SignInit(const CEVP_MD *type)
{
	EVP_SignInit(CastAnyPtr(EVP_MD_CTX, _raw), CastAnyConstPtr(EVP_MD, type->get_md()));
}

int CEVP_MD_CTX::SignInit_ex(const CEVP_MD *type, CENGINE *impl)
{
	return EVP_SignInit_ex(CastAnyPtr(EVP_MD_CTX, _raw), CastAnyConstPtr(EVP_MD, type->get_md()), PtrCheck(impl) ? nullptr : CastAnyPtr(ENGINE, CastMutable(Pointer, impl->get_impl())));
}

int CEVP_MD_CTX::SignUpdate(const void *d, unsigned int cnt)
{
	return EVP_SignUpdate(CastAnyPtr(EVP_MD_CTX, _raw), d, cnt);
}

int CEVP_MD_CTX::SignFinal(unsigned char *sig, unsigned int *s, CEVP_PKEY *pkey)
{
	return EVP_SignFinal(CastAnyPtr(EVP_MD_CTX, _raw), sig, s, CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pkey->get_key())));
}

int CEVP_MD_CTX::VerifyInit(const CEVP_MD *type)
{
	return EVP_VerifyInit(CastAnyPtr(EVP_MD_CTX, _raw), CastAnyConstPtr(EVP_MD, type->get_md()));
}

int CEVP_MD_CTX::VerifyInit_ex(const CEVP_MD *type, CENGINE *impl)
{
	return EVP_VerifyInit_ex(CastAnyPtr(EVP_MD_CTX, _raw), CastAnyConstPtr(EVP_MD, type->get_md()), PtrCheck(impl) ? nullptr : CastAnyPtr(ENGINE, CastMutable(Pointer, impl->get_impl())));
}

int CEVP_MD_CTX::VerifyUpdate(const void *d, unsigned int cnt)
{
	return EVP_VerifyUpdate(CastAnyPtr(EVP_MD_CTX, _raw), d, cnt);
}

int CEVP_MD_CTX::VerifyFinal(unsigned char *sigbuf, unsigned int siglen, CEVP_PKEY *pkey)
{
	return EVP_VerifyFinal(CastAnyPtr(EVP_MD_CTX, _raw), sigbuf, siglen, CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pkey->get_key())));
}

int CEVP_MD_CTX::cleanup()
{
	return EVP_MD_CTX_cleanup(CastAnyPtr(EVP_MD_CTX, _raw));
}

void CEVP_MD_CTX::destroy()
{
	if (_raw)
		EVP_MD_CTX_destroy(CastAnyPtr(EVP_MD_CTX, _raw));
	_raw = nullptr;
}

const CEVP_MD *CEVP_MD_CTX::md()
{
	ConstPtr(EVP_MD) pMD = EVP_MD_CTX_md(CastAnyPtr(EVP_MD_CTX, _raw));

	if (!pMD)
		return nullptr;

	Ptr(CEVP_MD) p = CastDynamicPtr(CEVP_MD, find_obj(pMD));

	if (p)
		return p;

	p = OK_NEW_OPERATOR CEVP_MD(pMD);
	p->insert_obj();
	return p;
}

int CEVP_MD_CTX::type()
{
	return EVP_MD_CTX_type(CastAnyPtr(EVP_MD_CTX, _raw));
}

int CEVP_MD_CTX::size()
{
	return EVP_MD_CTX_size(CastAnyPtr(EVP_MD_CTX, _raw));
}

int CEVP_MD_CTX::block_size()
{
	return EVP_MD_CTX_block_size(CastAnyPtr(EVP_MD_CTX, _raw));
}

int CEVP_MD_CTX::copy(CEVP_MD_CTX *out, CEVP_MD_CTX *in)
{
	return EVP_MD_CTX_copy(CastAnyPtr(EVP_MD_CTX, CastMutable(Pointer, out->get_mdContext())), CastAnyPtr(EVP_MD_CTX, CastMutable(Pointer, in->get_mdContext())));
}

int CEVP_MD_CTX::copy_ex(CEVP_MD_CTX *out, const CEVP_MD_CTX *in)
{
	return EVP_MD_CTX_copy_ex(CastAnyPtr(EVP_MD_CTX, CastMutable(Pointer, out->get_mdContext())), CastAnyConstPtr(EVP_MD_CTX, in->get_mdContext()));
}

