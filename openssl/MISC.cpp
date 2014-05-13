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
#include "MISC.H"
#include <openssl/buffer.h>
#include <openssl/asn1.h>
#include <openssl/bn.h>

CENGINE::CENGINE(ConstPointer impl) : COpenSSLClass(impl) {}
CENGINE::~CENGINE() {}

CBUF_MEM::CBUF_MEM(ConstPointer bufmem) : COpenSSLClass(bufmem) {}
CBUF_MEM::~CBUF_MEM() { free(); }

Ptr(CBUF_MEM) CBUF_MEM::create()
{
	Ptr(BUF_MEM) pMem = BUF_MEM_new();

	if (!pMem)
		return NULL;

	Ptr(CBUF_MEM) p = OK_NEW_OPERATOR CBUF_MEM(pMem);

	p->insert_obj();
	return p;
}

void CBUF_MEM::free()
{
	if (_raw)
		BUF_MEM_free(CastAnyPtr(BUF_MEM, _raw));
	_raw = NULL;
}

int CBUF_MEM::grow(size_t len)
{
	return BUF_MEM_grow(CastAnyPtr(BUF_MEM, _raw), len);
}

int CBUF_MEM::grow_clean(size_t len)
{
	return BUF_MEM_grow_clean(CastAnyPtr(BUF_MEM, _raw), len);
}

CASN1_OBJECT::CASN1_OBJECT(ConstPointer pObject) : COpenSSLClass(pObject) {}
CASN1_OBJECT::~CASN1_OBJECT() { free(); }

Ptr(CASN1_OBJECT) CASN1_OBJECT::create()
{
	Ptr(ASN1_OBJECT) pObject = ASN1_OBJECT_new();

	if (!pObject)
		return NULL;

	Ptr(CASN1_OBJECT) p = OK_NEW_OPERATOR CASN1_OBJECT(pObject);

	p->insert_obj();
	return p;
}

void CASN1_OBJECT::free()
{
	if (_raw)
		ASN1_OBJECT_free(CastAnyPtr(ASN1_OBJECT, _raw));
	_raw = NULL;
}

int	CASN1_OBJECT::i2d(unsigned char **pp)
{
	return i2d_ASN1_OBJECT(CastAnyPtr(ASN1_OBJECT, _raw), pp);
}

CASN1_OBJECT *CASN1_OBJECT::c2i(CASN1_OBJECT **a, const unsigned char **pp, long length)
{
	Ptr(ASN1_OBJECT) pObject = c2i_ASN1_OBJECT(NULL, pp, length);

	if (pObject)
		return OK_NEW_OPERATOR CASN1_OBJECT(pObject);
	return NULL;
}

CASN1_OBJECT *CASN1_OBJECT::d2i(CASN1_OBJECT **a, const unsigned char **pp, long length)
{
	Ptr(ASN1_OBJECT) pObject = d2i_ASN1_OBJECT(NULL, pp, length);

	if (pObject)
		return OK_NEW_OPERATOR CASN1_OBJECT(pObject);
	return NULL;
}

CASN1_INTEGER::CASN1_INTEGER(ConstPointer pInteger) : COpenSSLClass(pInteger) {}
CASN1_INTEGER::~CASN1_INTEGER() { free(); }

Ptr(CASN1_INTEGER) CASN1_INTEGER::create()
{
	Ptr(ASN1_INTEGER) pObject = M_ASN1_INTEGER_new();

	if (!pObject)
		return NULL;

	Ptr(CASN1_INTEGER) p = OK_NEW_OPERATOR CASN1_INTEGER(pObject);

	p->insert_obj();
	return p;
}

void CASN1_INTEGER::free()
{
	if (_raw)
		M_ASN1_INTEGER_free(CastAnyPtr(ASN1_INTEGER, _raw));
	_raw = NULL;
}

Ptr(CASN1_INTEGER) CASN1_INTEGER::dup()
{
	Ptr(ASN1_INTEGER) pObject = M_ASN1_INTEGER_dup(CastAnyPtr(ASN1_INTEGER, _raw));

	if (!pObject)
		return NULL;

	Ptr(CASN1_INTEGER) p = OK_NEW_OPERATOR CASN1_INTEGER(pObject);

	p->insert_obj();
	return p;
}

int	CASN1_INTEGER::i2c(unsigned char **pp)
{
	return i2c_ASN1_INTEGER(CastAnyPtr(ASN1_INTEGER, _raw), pp);
}

CASN1_INTEGER *CASN1_INTEGER::c2i(CASN1_INTEGER **a, const unsigned char **pp, long length)
{
	Ptr(ASN1_INTEGER) pObject = c2i_ASN1_INTEGER(NULL, pp, length);

	if (pObject)
		return OK_NEW_OPERATOR CASN1_INTEGER(pObject);
	return NULL;
}

CASN1_TIME::CASN1_TIME(ConstPointer pTime) : COpenSSLClass(pTime) {}
CASN1_TIME::~CASN1_TIME() { free(); }

Ptr(CASN1_TIME) CASN1_TIME::create()
{
	Ptr(ASN1_TIME) pObject = M_ASN1_TIME_new();

	if (!pObject)
		return NULL;

	Ptr(CASN1_TIME) p = OK_NEW_OPERATOR CASN1_TIME(pObject);

	p->insert_obj();
	return p;
}

void CASN1_TIME::free()
{
	if (_raw)
		M_ASN1_TIME_free(CastAnyPtr(ASN1_TIME, _raw));
	_raw = NULL;
}

CASN1_BIT_STRING::CASN1_BIT_STRING(ConstPointer pBitString) : COpenSSLClass(pBitString) {}
CASN1_BIT_STRING::~CASN1_BIT_STRING() { free(); }

Ptr(CASN1_BIT_STRING) CASN1_BIT_STRING::create()
{
	Ptr(ASN1_BIT_STRING) pObject = M_ASN1_BIT_STRING_new();

	if (!pObject)
		return NULL;

	Ptr(CASN1_BIT_STRING) p = OK_NEW_OPERATOR CASN1_BIT_STRING(pObject);

	p->insert_obj();
	return p;
}

void CASN1_BIT_STRING::free()
{
	if (_raw)
		M_ASN1_BIT_STRING_free(CastAnyPtr(ASN1_BIT_STRING, _raw));
	_raw = NULL;
}

CBIGNUM::CBIGNUM(ConstPointer pBigNum) : COpenSSLClass(pBigNum) {}
CBIGNUM::~CBIGNUM() { free(); }

Ptr(CBIGNUM) CBIGNUM::create()
{
	Ptr(BIGNUM) pObject = BN_new();

	if (!pObject)
		return NULL;

	Ptr(CBIGNUM) p = OK_NEW_OPERATOR CBIGNUM(pObject);

	p->insert_obj();
	return p;
}

void CBIGNUM::free()
{
	if (_raw)
		BN_clear_free(CastAnyPtr(BIGNUM, _raw));
	_raw = NULL;
}

CBN_CTX::CBN_CTX(ConstPointer pBigNumContext) : COpenSSLClass(pBigNumContext) {}
CBN_CTX::~CBN_CTX() { free(); }

Ptr(CBN_CTX) CBN_CTX::create()
{
	Ptr(BN_CTX) pObject = BN_CTX_new();

	if (!pObject)
		return NULL;

	Ptr(CBN_CTX) p = OK_NEW_OPERATOR CBN_CTX(pObject);

	p->insert_obj();
	return p;
}

void CBN_CTX::free()
{
	if (_raw)
		BN_CTX_free(CastAnyPtr(BN_CTX, _raw));
	_raw = NULL;
}

CBN_BLINDING::CBN_BLINDING(ConstPointer pBigNumBlinding) : COpenSSLClass(pBigNumBlinding) {}
CBN_BLINDING::~CBN_BLINDING() { free(); }

Ptr(CBN_BLINDING) CBN_BLINDING::create(ConstPtr(CBIGNUM) A, ConstPtr(CBIGNUM) Ai, Ptr(CBIGNUM) mod)
{
	Ptr(BN_BLINDING) pObject = BN_BLINDING_new(CastAnyConstPtr(BIGNUM, A->get_bignum()), CastAnyConstPtr(BIGNUM, Ai->get_bignum()), CastAnyPtr(BIGNUM, CastMutable(Pointer, mod->get_bignum())));

	if (!pObject)
		return NULL;

	Ptr(CBN_BLINDING) p = OK_NEW_OPERATOR CBN_BLINDING(pObject);

	p->insert_obj();
	return p;
}

void CBN_BLINDING::free()
{
	if (_raw)
		BN_BLINDING_free(CastAnyPtr(BN_BLINDING, _raw));
	_raw = NULL;
}

class OPENSSL_LOCAL TBN_GENCBCallbackData : public CCppObject
{
public:
	TBN_GENCBCallbackData(): cb(NULL), arg(NULL) {}
	virtual ~TBN_GENCBCallbackData() {}

	CBN_GENCB* self;
	CBN_GENCB::cb_2 cb;
	void *arg;
};

CBN_GENCB::CBN_GENCB(ConstPointer pBigNum) : COpenSSLClass(pBigNum) {}
CBN_GENCB::~CBN_GENCB() { free(); }

Ptr(CBN_GENCB) CBN_GENCB::create()
{
	Ptr(BN_GENCB) pObject = CastAnyPtr(BN_GENCB, TFalloc(sizeof(BN_GENCB)));

	if (!pObject)
		return NULL;

	Ptr(CBN_GENCB) p = OK_NEW_OPERATOR CBN_GENCB(pObject);

	p->insert_obj();
	return p;
}

Ptr(CBN_GENCB) CBN_GENCB::create_1(cb_1 cb, void* arg)
{
	Ptr(BN_GENCB) pObject = CastAnyPtr(BN_GENCB, TFalloc(sizeof(BN_GENCB)));

	if (!pObject)
		return NULL;

	Ptr(CBN_GENCB) p = OK_NEW_OPERATOR CBN_GENCB(pObject);

	p->set_1(cb, arg);
	p->insert_obj();
	return p;
}

Ptr(CBN_GENCB) CBN_GENCB::create_2(cb_2 cb, void* arg)
{
	Ptr(BN_GENCB) pObject = CastAnyPtr(BN_GENCB, TFalloc(sizeof(BN_GENCB)));

	if (!pObject)
		return NULL;

	Ptr(CBN_GENCB) p = OK_NEW_OPERATOR CBN_GENCB(pObject);

	p->set_2(cb, arg);
	p->insert_obj();
	return p;
}

void CBN_GENCB::free()
{
	if (_raw)
	{
		Ptr(BN_GENCB) pObject = CastAnyPtr(BN_GENCB, _raw);

		if (pObject->ver == 2)
		{
			Ptr(TBN_GENCBCallbackData) data = CastAnyPtr(TBN_GENCBCallbackData, pObject->arg);

			data->release();
		}
		TFfree(_raw);
	}
	_raw = NULL;
}

void CBN_GENCB::set_1(cb_1 cb, void* arg)
{
	Ptr(BN_GENCB) pObject = CastAnyPtr(BN_GENCB, _raw);

	if (pObject->ver == 2)
	{
		Ptr(TBN_GENCBCallbackData) data = CastAnyPtr(TBN_GENCBCallbackData, pObject->arg);

		data->release();
	}
	BN_GENCB_set_old(CastAnyPtr(BN_GENCB, _raw), cb, arg);
}

static int CBN_GENCB_cb_2(int a, int b, BN_GENCB *arg)
{
	Ptr(TBN_GENCBCallbackData) data = CastAnyPtr(TBN_GENCBCallbackData, arg->arg);
	int ret;

	BN_GENCB_set(CastAnyPtr(BN_GENCB, CastMutable(Pointer, data->self->get_gencb())), CBN_GENCB_cb_2, data->arg);
	ret = (data->cb)(a, b, data->self);
	data->arg = data->self->get_app_data();
	BN_GENCB_set(CastAnyPtr(BN_GENCB, CastMutable(Pointer, data->self->get_gencb())), CBN_GENCB_cb_2, data);
	return ret;
}

void CBN_GENCB::set_2(cb_2 cb, void* arg)
{
	Ptr(BN_GENCB) pObject = CastAnyPtr(BN_GENCB, _raw);
	Ptr(TBN_GENCBCallbackData) data = NULL;

	if (pObject->ver == 2)
		data = CastAnyPtr(TBN_GENCBCallbackData, pObject->arg);
	else
		data = OK_NEW_OPERATOR TBN_GENCBCallbackData;
	data->self = this;
	data->cb = cb;
	data->arg = arg;
	BN_GENCB_set(CastAnyPtr(BN_GENCB, _raw), CBN_GENCB_cb_2, data);
}

void CBN_GENCB::set_app_data(void* arg)
{
	Ptr(BN_GENCB) pObject = CastAnyPtr(BN_GENCB, _raw);

	switch (pObject->ver)
	{
	case 2:
		{
			Ptr(TBN_GENCBCallbackData) data = CastAnyPtr(TBN_GENCBCallbackData, pObject->arg);

			data->arg = arg;
		}
		break;
	case 1:
		pObject->arg = arg;
	default:
		break;
	}
}

void *CBN_GENCB::get_app_data(void)
{
	Ptr(BN_GENCB) pObject = CastAnyPtr(BN_GENCB, _raw);

	switch (pObject->ver)
	{
	case 2:
		{
			Ptr(TBN_GENCBCallbackData) data = CastAnyPtr(TBN_GENCBCallbackData, pObject->arg);

			return data->arg;
		}
		break;
	case 1:
		return pObject->arg;
	default:
		break;
	}
	return NULL;
}
