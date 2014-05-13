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
#include "SSL.h"
#include "BIO.h"
#include "EVP_PKEY.h"
#include "X509.h"
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/stack.h>
#include <openssl/safestack.h>
#include <openssl/lhash.h>
#undef X509_NAME

CSSL_METHOD::CSSL_METHOD(ConstPointer sslMeth) : COpenSSLClass(sslMeth) {}
CSSL_METHOD::~CSSL_METHOD() {}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv2_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv2_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv2_server_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv2_server_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv2_client_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv2_client_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv3_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv3_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv3_server_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv3_server_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv3_client_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv3_client_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv23_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv23_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv23_server_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv23_server_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::SSLv23_client_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::SSLv23_client_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_server_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_server_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_client_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_client_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_1_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_1_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_1_server_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_1_server_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_1_client_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_1_client_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_2_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_2_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_2_server_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_2_server_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::TLSv1_2_client_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::TLSv1_2_client_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::DTLSv1_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::DTLSv1_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::DTLSv1_server_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::DTLSv1_server_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

ConstPtr(CSSL_METHOD) CSSL_METHOD::DTLSv1_client_method(void)
{
	ConstPtr(SSL_METHOD) pMeth = ::DTLSv1_client_method();

	return OK_NEW_OPERATOR CSSL_METHOD(pMeth);
}

CSSL_CIPHER::CSSL_CIPHER(ConstPointer sslCipher) : COpenSSLClass(sslCipher) {}
CSSL_CIPHER::~CSSL_CIPHER() {}

int	CSSL_CIPHER::get_bits(int *alg_bits)
{
	return SSL_CIPHER_get_bits(CastAnyPtr(SSL_CIPHER, _raw), alg_bits);
}

char *CSSL_CIPHER::get_version()
{
	return SSL_CIPHER_get_version(CastAnyPtr(SSL_CIPHER, _raw));
}

const char *CSSL_CIPHER::get_name()
{
	return SSL_CIPHER_get_name(CastAnyPtr(SSL_CIPHER, _raw));
}

unsigned long CSSL_CIPHER::get_id()
{
	return SSL_CIPHER_get_id(CastAnyPtr(SSL_CIPHER, _raw));
}

char *CSSL_CIPHER::description(char *buf, int size)
{
	return SSL_CIPHER_description(CastAnyPtr(SSL_CIPHER, _raw), buf, size);
}

class OPENSSL_LOCAL TSSLCTXCallbackData : public CCppObject
{
public:
	TSSLCTXCallbackData() : cbMsg(NULL), cbNewSession(NULL), cbRemoveSession(NULL), cbGetSession(NULL), cbInfo(NULL), 
		cbClientCert(NULL), cbGenerateCookie(NULL), cbVerifyCookie(NULL), cbNextProtosAdvertised(NULL), 
		cbNextProtoSelect(NULL), cbPSKClient(NULL), cbPSKServer(NULL), cbGenerateSessionId(NULL), cbCertVerify(NULL),
		certVerifyArg(NULL), cbSRPClientPwd(NULL), cbSRPVerifyParam(NULL), cbSRPUsername(NULL) {}
	virtual ~TSSLCTXCallbackData() {}

	CSSL_CTX::TCallbackMsg cbMsg;
	CSSL_CTX::TCallbackNewSession cbNewSession;
	CSSL_CTX::TCallbackRemoveSession cbRemoveSession;
	CSSL_CTX::TCallbackGetSession cbGetSession;
	CSSL_CTX::TCallbackInfo cbInfo;
	CSSL_CTX::TCallbackClientCert cbClientCert;
	CSSL_CTX::TCallbackGenerateCookie cbGenerateCookie;
	CSSL_CTX::TCallbackVerifyCookie cbVerifyCookie;
	CSSL_CTX::TCallbackNextProtosAdvertised cbNextProtosAdvertised;
	CSSL_CTX::TCallbackNextProtoSelect cbNextProtoSelect;
	CSSL_CTX::TCallbackPSKClient cbPSKClient;
	CSSL_CTX::TCallbackPSKServer cbPSKServer;
	CSSL_CTX::TCallbackGenerateSessionId cbGenerateSessionId;
	CSSL_CTX::TCallbackCertVerify cbCertVerify;
	void* certVerifyArg;
	CSSL_CTX::TCallbackSRPClientPwd cbSRPClientPwd;
	CSSL_CTX::TCallbackSRPVerifyParam cbSRPVerifyParam;
	CSSL_CTX::TCallbackSRPUsername cbSRPUsername;
	CSSL_CTX::TCallbackTmpRSA cbTmpRSA;
	CSSL_CTX::TCallbackTmpDH cbTmpDH;
	CSSL_CTX::TCallbackTmpECDH cbTmpECDH;
};

typedef struct __tagSSLCTXCallbackAssoc
{
	ConstPointer raw;
	Ptr(TSSLCTXCallbackData) cdata;
} TSSLCTXCallbackAssoc;

static void __stdcall SSLCTXCallbackAssocListDeleteFunc(ConstPointer data, Pointer context)
{
	TSSLCTXCallbackAssoc* pInfo = CastAnyPtr(TSSLCTXCallbackAssoc, CastMutable(Pointer, data));

	pInfo->cdata->release();
}

static sword __stdcall SSLCTXCallbackAssocListSearchAndSortFunc(ConstPointer pa, ConstPointer pb)
{
	TSSLCTXCallbackAssoc* ppa = CastAnyPtr(TSSLCTXCallbackAssoc, CastMutable(Pointer, pa));
	TSSLCTXCallbackAssoc* ppb = CastAnyPtr(TSSLCTXCallbackAssoc, CastMutable(Pointer, pb));

	if (ppa->raw < ppb->raw)
		return -1;
	if (ppa->raw == ppb->raw)
		return 0;
	return 1;
}

static sword __stdcall SSLCTXCallbackAssocListSearchAndSortFunc1(ConstPointer pa, ConstPointer pb)
{
	TSSLCTXCallbackAssoc* ppa = CastAnyPtr(TSSLCTXCallbackAssoc, CastMutable(Pointer, pa));

	if (ppa->raw < pb)
		return -1;
	if (ppa->raw == pb)
		return 0;
	return 1;
}

class OPENSSL_LOCAL TSSLCTXCallbackAssocList : public CDataSVectorT<TSSLCTXCallbackAssoc>
{
public:
	typedef CDataSVectorT<TSSLCTXCallbackAssoc> super;

	TSSLCTXCallbackAssocList(DECL_FILE_LINE TListCnt cnt, TListCnt exp, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL):
		super(ARGS_FILE_LINE cnt, exp, pDeleteFunc, pDeleteContext, pSearchAndSortFunc) {}
	~TSSLCTXCallbackAssocList() {}

	Ptr(TSSLCTXCallbackData) Find(ConstPointer raw)
	{
		super::Iterator it = super::FindSorted(CastAnyConstPtr(TSSLCTXCallbackAssoc, raw), SSLCTXCallbackAssocListSearchAndSortFunc1);

		if (it && *it && (SSLCTXCallbackAssocListSearchAndSortFunc1(*it, raw) == 0))
			return (*it)->cdata;
		return NULL;
	}

	void Insert(ConstPointer raw, Ptr(TSSLCTXCallbackData) cdata)
	{
		if (Find(raw))
			return;

		TSSLCTXCallbackAssoc assoc;

		assoc.raw = raw;
		assoc.cdata = cdata;

		super::InsertSorted(&assoc);
	}

	void Remove(ConstPointer raw)
	{
		if (!Find(raw))
			return;
		super::RemoveSorted(CastAnyConstPtr(TSSLCTXCallbackAssoc, raw), SSLCTXCallbackAssocListSearchAndSortFunc1);
	}

private:
	TSSLCTXCallbackAssocList();

};

static TSSLCTXCallbackAssocList _sslctxcallbackassoclist(__FILE__LINE__ 16, 16, SSLCTXCallbackAssocListDeleteFunc, NULL, SSLCTXCallbackAssocListSearchAndSortFunc);

CSSL_CTX::CSSL_CTX(ConstPointer sslCtx) : COpenSSLClass(sslCtx) {}
CSSL_CTX::~CSSL_CTX() { free(); }

Ptr(CSSL_CTX) CSSL_CTX::create(ConstPtr(CSSL_METHOD) meth)
{
	Ptr(SSL_CTX) ret = SSL_CTX_new(CastAnyConstPtr(SSL_METHOD, meth->get_sslMethod()));

	if (!ret)
		return NULL;

	Ptr(CSSL_CTX) p = OK_NEW_OPERATOR CSSL_CTX(ret);

	p->insert_obj();
	return p;
}

void CSSL_CTX::free()
{
	if (_raw)
	{
		_sslctxcallbackassoclist.Remove(_raw);

		SSL_CTX_free(CastAnyPtr(SSL_CTX, _raw));
		_raw = NULL;
	}
}

void CSSL_CTX::set_options(unsigned long op)
{
	SSL_CTX_set_options(CastAnyPtr(SSL_CTX, _raw), op);
}

void CSSL_CTX::clear_options(unsigned long op)
{
	SSL_CTX_clear_options(CastAnyPtr(SSL_CTX, _raw), op);
}

unsigned long CSSL_CTX::get_options()
{
	return SSL_CTX_get_options(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::set_mode(unsigned long op)
{
	SSL_CTX_set_mode(CastAnyPtr(SSL_CTX, _raw), op);
}

void CSSL_CTX::clear_mode(unsigned long op)
{
	SSL_CTX_clear_mode(CastAnyPtr(SSL_CTX, _raw), op);
}

unsigned long CSSL_CTX::get_mode()
{
	return SSL_CTX_get_mode(CastAnyPtr(SSL_CTX, _raw));
}

static void MsgCallback(int write_p, int version, int content_type, const void *buf, size_t len, SSL *ssl, void *arg)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}

	(pCallbackData->cbMsg)(write_p, version, content_type, buf, len, pSSL, arg);
}

void CSSL_CTX::set_msg_callback(TCallbackMsg cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbMsg = cb;

		SSL_CTX_set_msg_callback(CastAnyPtr(SSL_CTX, _raw), MsgCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbMsg = NULL;

		SSL_CTX_set_msg_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

void CSSL_CTX::set_msg_callback_arg(void *arg)
{
	SSL_CTX_set_msg_callback_arg(CastAnyPtr(SSL_CTX, _raw), arg);
}

static void sessions_doall_arg(void *item, void *context)
{
	Ptr(CHashList<CSSL_SESSION>) pHashList = CastAnyPtr(CHashList<CSSL_SESSION>, context);
	Ptr(CSSL_SESSION) pSession = OK_NEW_OPERATOR CSSL_SESSION(item);

	pHashList->insert(pSession);
}

CHashList<CSSL_SESSION> CSSL_CTX::sessions()
{
	CHashList<CSSL_SESSION> ret;
	Ptr(LHASH_OF(SSL_SESSION)) pHashList = SSL_CTX_sessions(CastAnyPtr(SSL_CTX, _raw));

	LHM_lh_doall_arg(SSL_SESSION, pHashList, sessions_doall_arg, CHashList<CSSL_SESSION>, &ret);
	return ret;
}

int CSSL_CTX::sess_number()
{
	return SSL_CTX_sess_number(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_connect()
{
	return SSL_CTX_sess_connect(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_connect_good()
{
	return SSL_CTX_sess_connect_good(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_connect_renegotiate()
{
	return SSL_CTX_sess_connect_renegotiate(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_accept()
{
	return SSL_CTX_sess_accept(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_accept_renegotiate()
{
	return SSL_CTX_sess_accept_renegotiate(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_accept_good()
{
	return SSL_CTX_sess_accept_good(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_hits()
{
	return SSL_CTX_sess_hits(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_cb_hits()
{
	return SSL_CTX_sess_cb_hits(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_misses()
{
	return SSL_CTX_sess_misses(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_timeouts()
{
	return SSL_CTX_sess_timeouts(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::sess_cache_full()
{
	return SSL_CTX_sess_cache_full(CastAnyPtr(SSL_CTX, _raw));
}

static int NewSessionCallback(SSL *ssl, SSL_SESSION *sess)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	CSSL_SESSION* pSSL_SESSION = CastDynamicPtr(CSSL_SESSION, COpenSSLClass::find_obj(sess));
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	if (!pSSL_SESSION)
	{
		pSSL_SESSION = OK_NEW_OPERATOR CSSL_SESSION(sess);
		pSSL_SESSION->insert_obj();
	}
	ret = (pCallbackData->cbNewSession)(pSSL, pSSL_SESSION);
	return ret;
}

void CSSL_CTX::sess_set_new_cb(TCallbackNewSession cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbNewSession = cb;

		SSL_CTX_sess_set_new_cb(CastAnyPtr(SSL_CTX, _raw), NewSessionCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbNewSession = NULL;

		SSL_CTX_sess_set_new_cb(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

CSSL_CTX::TCallbackNewSession CSSL_CTX::sess_get_new_cb()
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (pCallbackData)
		return pCallbackData->cbNewSession;
	return NULL;
}

static void RemoveSessionCallback(SSL_CTX *ctx, SSL_SESSION *sess)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(ctx);
	CSSL_CTX* pSSL_CTX = CastDynamicPtr(CSSL_CTX, COpenSSLClass::find_obj(ctx));
	CSSL_SESSION* pSSL_SESSION = CastDynamicPtr(CSSL_SESSION, COpenSSLClass::find_obj(sess));

	if (!pSSL_CTX)
	{
		pSSL_CTX = OK_NEW_OPERATOR CSSL_CTX(ctx);
		pSSL_CTX->insert_obj();
	}
	if (!pSSL_SESSION)
	{
		pSSL_SESSION = OK_NEW_OPERATOR CSSL_SESSION(sess);
		pSSL_SESSION->insert_obj();
	}
	(pCallbackData->cbRemoveSession)(pSSL_CTX, pSSL_SESSION);
}

void CSSL_CTX::sess_set_remove_cb(TCallbackRemoveSession cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbRemoveSession = cb;

		SSL_CTX_sess_set_remove_cb(CastAnyPtr(SSL_CTX, _raw), RemoveSessionCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbRemoveSession = NULL;

		SSL_CTX_sess_set_remove_cb(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

CSSL_CTX::TCallbackRemoveSession CSSL_CTX::sess_get_remove_cb()
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (pCallbackData)
		return pCallbackData->cbRemoveSession;
	return NULL;
}

SSL_SESSION *GetSessionCallback(SSL *ssl, unsigned char *data, int len, int *copy)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	CSSL_SESSION* pSSL_SESSION = NULL;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	pSSL_SESSION = (pCallbackData->cbGetSession)(pSSL, data, len, copy);
	return CastAnyPtr(SSL_SESSION, CastMutable(Pointer, pSSL_SESSION->get_sslsession()));
}

void CSSL_CTX::sess_set_get_cb(TCallbackGetSession cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbGetSession = cb;

		SSL_CTX_sess_set_get_cb(CastAnyPtr(SSL_CTX, _raw), GetSessionCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbGetSession = NULL;

		SSL_CTX_sess_set_get_cb(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

CSSL_CTX::TCallbackGetSession CSSL_CTX::sess_get_get_cb()
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (pCallbackData)
		return pCallbackData->cbGetSession;
	return NULL;
}

static void InfoCallback(const SSL *ssl, int where, int ret)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	(pCallbackData->cbInfo)(pSSL, where, ret);
}

void CSSL_CTX::set_info_callback(TCallbackInfo cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbInfo = cb;

		SSL_CTX_set_info_callback(CastAnyPtr(SSL_CTX, _raw), InfoCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbInfo = NULL;

		SSL_CTX_set_info_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

CSSL_CTX::TCallbackInfo CSSL_CTX::get_info_callback()
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (pCallbackData)
		return pCallbackData->cbInfo;
	return NULL;
}

static int ClientCertCallback(SSL *ssl, X509 **x509, EVP_PKEY **pkey)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	CX509* pX509 = NULL;
	CEVP_PKEY* ppKey = NULL;
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbClientCert)(pSSL, &pX509, &ppKey);
	*x509 = CastAnyPtr(X509, CastMutable(Pointer, pX509->get_X509()));
	*pkey = CastAnyPtr(EVP_PKEY, CastMutable(Pointer, ppKey->get_key()));
	return ret;
}

void CSSL_CTX::set_client_cert_cb(TCallbackClientCert cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbClientCert = cb;

		SSL_CTX_set_client_cert_cb(CastAnyPtr(SSL_CTX, _raw), ClientCertCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbClientCert = NULL;

		SSL_CTX_set_client_cert_cb(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

CSSL_CTX::TCallbackClientCert CSSL_CTX::get_client_cert_cb()
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (pCallbackData)
		return pCallbackData->cbClientCert;
	return NULL;
}

int CSSL_CTX::set_client_cert_engine(CENGINE *e)
{
	return SSL_CTX_set_client_cert_engine(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(ENGINE, CastMutable(Pointer, e->get_impl())));
}

static int GenerateCookieCallback(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbGenerateCookie)(pSSL, cookie, cookie_len);
	return ret;
}

void CSSL_CTX::set_cookie_generate_cb(TCallbackGenerateCookie cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbGenerateCookie = cb;

		SSL_CTX_set_cookie_generate_cb(CastAnyPtr(SSL_CTX, _raw), GenerateCookieCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbGenerateCookie = NULL;

		SSL_CTX_set_cookie_generate_cb(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

static int VerifyCookieCallback(SSL *ssl, unsigned char *cookie, unsigned int cookie_len)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbVerifyCookie)(pSSL, cookie, cookie_len);
	return ret;
}

void CSSL_CTX::set_cookie_verify_cb(TCallbackVerifyCookie cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbVerifyCookie = cb;

		SSL_CTX_set_cookie_verify_cb(CastAnyPtr(SSL_CTX, _raw), VerifyCookieCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbVerifyCookie = NULL;

		SSL_CTX_set_cookie_verify_cb(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

static int NextProtosAdvertisedCallback(SSL *ssl, const unsigned char **out, unsigned int *outlen, void *arg)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbNextProtosAdvertised)(pSSL, out, outlen, arg);
	return ret;
}

void CSSL_CTX::set_next_protos_advertised_cb(TCallbackNextProtosAdvertised cb, void *arg)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbNextProtosAdvertised = cb;

		SSL_CTX_set_next_protos_advertised_cb(CastAnyPtr(SSL_CTX, _raw), NextProtosAdvertisedCallback, arg);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbNextProtosAdvertised = NULL;

		SSL_CTX_set_next_protos_advertised_cb(CastAnyPtr(SSL_CTX, _raw), NULL, NULL);
	}
}

static int NextProtoSelectCallback(SSL *ssl, unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void *arg)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbNextProtoSelect)(pSSL, out, outlen, in, inlen, arg);
	return ret;
}

void CSSL_CTX::set_next_proto_select_cb(TCallbackNextProtoSelect cb, void *arg)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbNextProtoSelect = cb;

		SSL_CTX_set_next_proto_select_cb(CastAnyPtr(SSL_CTX, _raw), NextProtoSelectCallback, arg);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbNextProtoSelect = NULL;

		SSL_CTX_set_next_proto_select_cb(CastAnyPtr(SSL_CTX, _raw), NULL, NULL);
	}
}

static unsigned int PSKClientCallback(SSL *ssl, const char *hint, char *identity, unsigned int max_identity_len, unsigned char *psk, unsigned int max_psk_len)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	unsigned int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbPSKClient)(pSSL, hint, identity, max_identity_len, psk, max_psk_len);
	return ret;
}

void CSSL_CTX::set_psk_client_callback(TCallbackPSKClient cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbPSKClient = cb;

		SSL_CTX_set_psk_client_callback(CastAnyPtr(SSL_CTX, _raw), PSKClientCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbPSKClient = NULL;

		SSL_CTX_set_psk_client_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

static unsigned int PSKServerCallback(SSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	unsigned int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbPSKServer)(pSSL, identity, psk, max_psk_len);
	return ret;
}

void CSSL_CTX::set_psk_server_callback(TCallbackPSKServer cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbPSKServer = cb;

		SSL_CTX_set_psk_server_callback(CastAnyPtr(SSL_CTX, _raw), PSKServerCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbPSKServer = NULL;

		SSL_CTX_set_psk_server_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

int CSSL_CTX::use_psk_identity_hint(const char *identity_hint)
{
	return SSL_CTX_use_psk_identity_hint(CastAnyPtr(SSL_CTX, _raw), identity_hint);
}

void *CSSL_CTX::get_app_data()
{
	return SSL_CTX_get_app_data(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::set_app_data(void *arg)
{
	SSL_CTX_set_app_data(CastAnyPtr(SSL_CTX, _raw), arg);
}

long CSSL_CTX::need_tmp_RSA()
{
	return SSL_CTX_need_tmp_RSA(CastAnyPtr(SSL_CTX, _raw));
}

long CSSL_CTX::set_tmp_rsa(CRSA_KEY *rsa)
{
	return SSL_CTX_set_tmp_rsa(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(RSA, CastMutable(Pointer, rsa->get_key())));
}

long CSSL_CTX::set_tmp_dh(CDH_KEY *dh)
{
	return SSL_CTX_set_tmp_dh(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(DH, CastMutable(Pointer, dh->get_key())));
}

long CSSL_CTX::set_tmp_ecdh(CEC_KEY* ecdh)
{
	return SSL_CTX_set_tmp_ecdh(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(EC_KEY, CastMutable(Pointer, ecdh->get_key())));
}

long CSSL_CTX::add_extra_chain_cert(CX509 *x509)
{
	return SSL_CTX_add_extra_chain_cert(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(X509, CastMutable(Pointer, x509->get_X509())));
}

long CSSL_CTX::get_extra_chain_certs(CX509 **px509)
{
	X509* pX509 = NULL;
	long ret;

	ret = SSL_CTX_get_extra_chain_certs(CastAnyPtr(SSL_CTX, _raw), &pX509);
	if (!pX509)
		return ret;
	*px509 = CastDynamicPtr(CX509, find_obj(pX509));
	if (!(*px509))
	{
		*px509 = OK_NEW_OPERATOR CX509(pX509);
		(*px509)->insert_obj();
	}
	return ret;
}

long CSSL_CTX::clear_extra_chain_certs()
{
	return SSL_CTX_clear_extra_chain_certs(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::set_cipher_list(const char *str)
{
	return SSL_CTX_set_cipher_list(CastAnyPtr(SSL_CTX, _raw), str);
}

void CSSL_CTX::set_timeout(long t)
{
	SSL_CTX_set_timeout(CastAnyPtr(SSL_CTX, _raw), t);
}

long CSSL_CTX::get_timeout()
{
	return SSL_CTX_get_timeout(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::set_cert_store(CX509_STORE *cs)
{
	SSL_CTX_set_cert_store(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(X509_STORE, CastMutable(Pointer, cs->get_X509Store())));
}

CX509_STORE *CSSL_CTX::get_cert_store()
{
	Ptr(X509_STORE) ret = SSL_CTX_get_cert_store(CastAnyPtr(SSL_CTX, _raw));

	if (!ret)
		return NULL;

	Ptr(CX509_STORE) p = CastDynamicPtr(CX509_STORE, find_obj(ret));

	if (!p)
	{
		p = OK_NEW_OPERATOR CX509_STORE(ret);
		p->insert_obj();
	}
	return p;
}

void CSSL_CTX::flush_sessions(long t)
{
	SSL_CTX_flush_sessions(CastAnyPtr(SSL_CTX, _raw), t);
}

int CSSL_CTX::use_RSAPrivateKey_file(char *file, TFileType type)
{
	return SSL_CTX_use_RSAPrivateKey_file(CastAnyPtr(SSL_CTX, _raw), file, type);
}

int CSSL_CTX::use_PrivateKey_file(char *file, TFileType type)
{
	return SSL_CTX_use_PrivateKey_file(CastAnyPtr(SSL_CTX, _raw), file, type);
}

int CSSL_CTX::use_certificate_file(char *file, TFileType type)
{
	return SSL_CTX_use_certificate_file(CastAnyPtr(SSL_CTX, _raw), file, type);
}

int	CSSL_CTX::use_certificate_chain_file(const char *file)
{
	return SSL_CTX_use_certificate_chain_file(CastAnyPtr(SSL_CTX, _raw), file); /* PEM type */
}

int CSSL_CTX::add_session(CSSL_SESSION *c)
{
	return SSL_CTX_add_session(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(SSL_SESSION, CastMutable(Pointer, c->get_sslsession())));
}

int CSSL_CTX::remove_session(CSSL_SESSION *c)
{
	return SSL_CTX_remove_session(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(SSL_SESSION, CastMutable(Pointer, c->get_sslsession())));
}

static int GenerateSessionIdCallback(const SSL *ssl, unsigned char *id, unsigned int *id_len)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbGenerateSessionId)(pSSL, id, id_len);
	return ret;
}

void CSSL_CTX::set_generate_session_id(TCallbackGenerateSessionId cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbGenerateSessionId = cb;

		SSL_CTX_set_generate_session_id(CastAnyPtr(SSL_CTX, _raw), GenerateSessionIdCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbGenerateSessionId = NULL;

		SSL_CTX_set_generate_session_id(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

int CSSL_CTX::get_verify_depth()
{
	return SSL_CTX_get_verify_depth(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::set_verify_depth(int depth)
{
	SSL_CTX_set_verify_depth(CastAnyPtr(SSL_CTX, _raw), depth);
}

static int CertVerifyCallback(X509_STORE_CTX *ctx, void *arg)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = CastAnyPtr(TSSLCTXCallbackData, arg);
	Ptr(CX509_STORE_CTX) pCtx = CastDynamicPtr(CX509_STORE_CTX, COpenSSLClass::find_obj(ctx));
	int ret;

	if (!pCtx)
	{
		pCtx = OK_NEW_OPERATOR CX509_STORE_CTX(ctx);
		pCtx->insert_obj();
	}
	ret = (pCallbackData->cbCertVerify)(pCtx, pCallbackData->certVerifyArg);
	return ret;
}

void CSSL_CTX::set_cert_verify_callback(TCallbackCertVerify cb, void *arg)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}

		pCallbackData->cbCertVerify = cb;
		pCallbackData->certVerifyArg = arg;

		SSL_CTX_set_cert_verify_callback(CastAnyPtr(SSL_CTX, _raw), CertVerifyCallback, pCallbackData);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbCertVerify = NULL;
		pCallbackData->certVerifyArg = NULL;

		SSL_CTX_set_cert_verify_callback(CastAnyPtr(SSL_CTX, _raw), NULL, NULL);
	}
}

int CSSL_CTX::use_RSAPrivateKey(CRSA_KEY *rsa)
{
	return SSL_CTX_use_RSAPrivateKey(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(RSA, CastMutable(Pointer, rsa->get_key())));
}

int CSSL_CTX::use_RSAPrivateKey_ASN1(unsigned char *d, long len)
{
	return SSL_CTX_use_RSAPrivateKey_ASN1(CastAnyPtr(SSL_CTX, _raw), d, len);
}

int CSSL_CTX::use_PrivateKey(CEVP_PKEY *pkey)
{
	return SSL_CTX_use_PrivateKey(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pkey->get_key())));
}

int CSSL_CTX::use_PrivateKey_ASN1(int pk, unsigned char *d, long len)
{
	return SSL_CTX_use_PrivateKey_ASN1(pk, CastAnyPtr(SSL_CTX, _raw), d, len);
}

int CSSL_CTX::use_certificate(CX509 *x)
{
	return SSL_CTX_use_certificate(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(X509, CastMutable(Pointer, x->get_X509())));
}

int CSSL_CTX::use_certificate_ASN1(int len, unsigned char *d)
{
	return SSL_CTX_use_certificate_ASN1(CastAnyPtr(SSL_CTX, _raw), len, d);
}

void CSSL_CTX::set_default_passwd_cb(TCallbackDefaultPasswd cb)
{
	SSL_CTX_set_default_passwd_cb(CastAnyPtr(SSL_CTX, _raw), cb);
}

void CSSL_CTX::set_default_passwd_cb_userdata(void *u)
{
	SSL_CTX_set_default_passwd_cb_userdata(CastAnyPtr(SSL_CTX, _raw), u);
}

int CSSL_CTX::check_private_key()
{
	return SSL_CTX_check_private_key(CastAnyPtr(SSL_CTX, _raw));
}

int	CSSL_CTX::set_session_id_context(const unsigned char *sid_ctx, unsigned int sid_ctx_len)
{
	return SSL_CTX_set_session_id_context(CastAnyPtr(SSL_CTX, _raw), sid_ctx, sid_ctx_len);
}

int CSSL_CTX::set1_param(CX509_VERIFY_PARAM *vpm)
{
	return SSL_CTX_set1_param(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(X509_VERIFY_PARAM, CastMutable(Pointer, vpm->get_X509VerifyParam())));
}

int CSSL_CTX::set_srp_username(char *name)
{
	return SSL_CTX_set_srp_username(CastAnyPtr(SSL_CTX, _raw), name);
}

int CSSL_CTX::set_srp_password(char *password)
{
	return SSL_CTX_set_srp_password(CastAnyPtr(SSL_CTX, _raw), password);
}

int CSSL_CTX::set_srp_strength(int strength)
{
	return SSL_CTX_set_srp_strength(CastAnyPtr(SSL_CTX, _raw), strength);
}

static char *SRPClientPwdCallback(SSL *ssl, void *u)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	char* ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbSRPClientPwd)(pSSL, u);
	return ret;
}

void CSSL_CTX::set_srp_client_pwd_callback(TCallbackSRPClientPwd cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}
		pCallbackData->cbSRPClientPwd = cb;
		SSL_CTX_set_srp_client_pwd_callback(CastAnyPtr(SSL_CTX, _raw), SRPClientPwdCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbSRPClientPwd = NULL;
		SSL_CTX_set_srp_client_pwd_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

static int SRPVerifyParamCallback(SSL *ssl, void *u)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbSRPVerifyParam)(pSSL, u);
	return ret;
}

void CSSL_CTX::set_srp_verify_param_callback(TCallbackSRPVerifyParam cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}
		pCallbackData->cbSRPVerifyParam = cb;
		SSL_CTX_set_srp_verify_param_callback(CastAnyPtr(SSL_CTX, _raw), SRPVerifyParamCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbSRPVerifyParam = NULL;
		SSL_CTX_set_srp_verify_param_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

static int SRPUsernameCallback(SSL *ssl, int *i, void *u)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbSRPUsername)(pSSL, i, u);
	return ret;
}

void CSSL_CTX::set_srp_username_callback(TCallbackSRPUsername cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}
		pCallbackData->cbSRPUsername = cb;
		SSL_CTX_set_srp_username_callback(CastAnyPtr(SSL_CTX, _raw), SRPUsernameCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbSRPUsername = NULL;
		SSL_CTX_set_srp_username_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

int CSSL_CTX::set_srp_cb_arg(void *arg)
{
	return SSL_CTX_set_srp_cb_arg(CastAnyPtr(SSL_CTX, _raw), arg);
}

int CSSL_CTX::set_ssl_version(const CSSL_METHOD *meth)
{
	return SSL_CTX_set_ssl_version(CastAnyPtr(SSL_CTX, _raw), CastAnyConstPtr(SSL_METHOD, meth->get_sslMethod()));
}

void CSSL_CTX::set_client_CA_list(CStack<CX509_NAME>& list)
{
	_STACK *pStack = sk_new_null();
	Ptr(CX509_NAME) pName;

	while ((pName = list.Pop()) !=  NULL)
	{
		sk_push(pStack, CastMutable(Pointer, pName->get_X509Name()));
	}
	SSL_CTX_set_client_CA_list(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(stack_st_X509_NAME, pStack));
}

CStack<CX509_NAME> CSSL_CTX::get_client_CA_list()
{
	CStack<CX509_NAME> result __FILE__LINE__0P;
	Ptr(X509_NAME) value;
	_STACK *ret = CastAnyPtr(_STACK, SSL_CTX_get_client_CA_list(CastAnyPtr(SSL_CTX, _raw)));

	while ((value = CastAnyPtr(X509_NAME, sk_pop(ret))) != NULL)
	{
		CX509_NAME* item = OK_NEW_OPERATOR CX509_NAME(value);

		result.Push(item);
	}
	return result;
}

int CSSL_CTX::add_client_CA(CX509 *x)
{
	return SSL_CTX_add_client_CA(CastAnyPtr(SSL_CTX, _raw), CastAnyPtr(X509, CastMutable(Pointer, x->get_X509())));
}

int CSSL_CTX::get_quiet_shutdown()
{
	return SSL_CTX_get_quiet_shutdown(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::set_quiet_shutdown(int mode)
{
	SSL_CTX_set_quiet_shutdown(CastAnyPtr(SSL_CTX, _raw), mode);
}

int CSSL_CTX::set_default_verify_paths()
{
	return SSL_CTX_set_default_verify_paths(CastAnyPtr(SSL_CTX, _raw));
}

int CSSL_CTX::load_verify_locations(char *CAfile, char *CApath)
{
	return SSL_CTX_load_verify_locations(CastAnyPtr(SSL_CTX, _raw), CAfile, CApath);
}

void *CSSL_CTX::get_ex_data(int idx)
{
	return SSL_CTX_get_ex_data(CastAnyPtr(SSL_CTX, _raw), idx);
}

int CSSL_CTX::set_ex_data(int idx, char *arg)
{
	return SSL_CTX_set_ex_data(CastAnyPtr(SSL_CTX, _raw), idx, arg);
}

int CSSL_CTX::get_ex_new_index(long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func)
{
	return SSL_CTX_get_ex_new_index(argl, argp, new_func, dup_func, free_func);
}

int CSSL_CTX::sess_get_cache_size()
{
	return SSL_CTX_sess_get_cache_size(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::sess_set_cache_size(long t)
{
	SSL_CTX_sess_set_cache_size(CastAnyPtr(SSL_CTX, _raw), t);
}

int CSSL_CTX::get_session_cache_mode()
{
	return SSL_CTX_get_session_cache_mode(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::set_session_cache_mode(int mode)
{
	SSL_CTX_set_session_cache_mode(CastAnyPtr(SSL_CTX, _raw), mode);
}

int CSSL_CTX::get_read_ahead()
{
	return SSL_CTX_get_read_ahead(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::set_read_ahead(int m)
{
	SSL_CTX_set_read_ahead(CastAnyPtr(SSL_CTX, _raw), m);
}

int CSSL_CTX::get_max_cert_list()
{
	return SSL_CTX_get_max_cert_list(CastAnyPtr(SSL_CTX, _raw));
}

void CSSL_CTX::set_max_cert_list(int m)
{
	SSL_CTX_set_max_cert_list(CastAnyPtr(SSL_CTX, _raw), m);
}

void CSSL_CTX::set_max_send_fragment(int m)
{
	SSL_CTX_set_max_send_fragment(CastAnyPtr(SSL_CTX, _raw), m);
}

static RSA* TmpRSACallback(SSL *ssl, int is_export, int keylength)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	Ptr(CRSA_KEY) ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbTmpRSA)(pSSL, is_export, keylength);
	return CastAnyPtr(RSA, CastMutable(Pointer, ret->get_key()));
}

void CSSL_CTX::set_tmp_rsa_callback(TCallbackTmpRSA cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}
		pCallbackData->cbTmpRSA = cb;
		SSL_CTX_set_tmp_rsa_callback(CastAnyPtr(SSL_CTX, _raw), TmpRSACallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbTmpRSA = NULL;
		SSL_CTX_set_tmp_rsa_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

static DH* TmpDHCallback(SSL *ssl, int is_export, int keylength)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	Ptr(CDH_KEY) ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbTmpDH)(pSSL, is_export, keylength);
	return CastAnyPtr(DH, CastMutable(Pointer, ret->get_key()));
}

void CSSL_CTX::set_tmp_dh_callback(TCallbackTmpDH cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}
		pCallbackData->cbTmpDH = cb;
		SSL_CTX_set_tmp_dh_callback(CastAnyPtr(SSL_CTX, _raw), TmpDHCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbTmpDH = NULL;
		SSL_CTX_set_tmp_dh_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

static EC_KEY* TmpECDHCallback(SSL *ssl, int is_export, int keylength)
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(ssl);
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(pCtx);
	CSSL* pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	Ptr(CEC_KEY) ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	ret = (pCallbackData->cbTmpECDH)(pSSL, is_export, keylength);
	return CastAnyPtr(EC_KEY, CastMutable(Pointer, ret->get_key()));
}

void CSSL_CTX::set_tmp_ecdh_callback(TCallbackTmpECDH cb)
{
	Ptr(TSSLCTXCallbackData) pCallbackData = _sslctxcallbackassoclist.Find(_raw);

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCTXCallbackData;
			_sslctxcallbackassoclist.Insert(_raw, pCallbackData);
		}
		pCallbackData->cbTmpECDH = cb;
		SSL_CTX_set_tmp_ecdh_callback(CastAnyPtr(SSL_CTX, _raw), TmpECDHCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbTmpECDH = NULL;
		SSL_CTX_set_tmp_ecdh_callback(CastAnyPtr(SSL_CTX, _raw), NULL);
	}
}

CSSL_SESSION::CSSL_SESSION(ConstPointer sslSession) : COpenSSLClass(sslSession) {}
CSSL_SESSION::~CSSL_SESSION() { free(); }

Ptr(CSSL_SESSION) CSSL_SESSION::create()
{
	Ptr(SSL_SESSION) pSession = SSL_SESSION_new();

	if (!pSession)
		return NULL;

	Ptr(CSSL_SESSION) p = OK_NEW_OPERATOR CSSL_SESSION(pSession);

	p->insert_obj();
	return p;
}

void CSSL_SESSION::free()
{
	if (_raw)
		SSL_SESSION_free(CastAnyPtr(SSL_SESSION, _raw));
	_raw = NULL;
}

long CSSL_SESSION::get_time()
{
	return SSL_SESSION_get_time(CastAnyPtr(SSL_SESSION, _raw));
}

long CSSL_SESSION::set_time(long t)
{
	return SSL_SESSION_set_time(CastAnyPtr(SSL_SESSION, _raw), t);
}

long CSSL_SESSION::get_timeout()
{
	return SSL_SESSION_get_timeout(CastAnyPtr(SSL_SESSION, _raw));
}

long CSSL_SESSION::set_timeout(long t)
{
	return SSL_SESSION_set_timeout(CastAnyPtr(SSL_SESSION, _raw), t);
}

CX509 *CSSL_SESSION::get0_peer()
{
	Ptr(X509) pX509 = SSL_SESSION_get0_peer(CastAnyPtr(SSL_SESSION, _raw));

	if (!pX509)
		return NULL;

	Ptr(CX509) p = CastDynamicPtr(CX509, find_obj(pX509));

	if (!p)
	{
		p = OK_NEW_OPERATOR CX509(pX509);
		p->insert_obj();
	}
	return p;
}

int CSSL_SESSION::set1_id_context(const unsigned char *sid_ctx, unsigned int sid_ctx_len)
{
	return SSL_SESSION_set1_id_context(CastAnyPtr(SSL_SESSION, _raw), sid_ctx, sid_ctx_len);
}

const unsigned char *CSSL_SESSION::get_id(unsigned int *len)
{
	return SSL_SESSION_get_id(CastAnyPtr(SSL_SESSION, _raw), len);
}

unsigned int CSSL_SESSION::get_compress_id()
{
	return SSL_SESSION_get_compress_id(CastAnyPtr(SSL_SESSION, _raw));
}

int	CSSL_SESSION::print_fp(FILE *fp)
{
	return SSL_SESSION_print_fp(fp, CastAnyPtr(SSL_SESSION, _raw));
}

int	CSSL_SESSION::print(CBIO *fp)
{
	return SSL_SESSION_print(CastAnyPtr(BIO, CastMutable(Pointer, fp->get_bio())), CastAnyPtr(SSL_SESSION, _raw));
}

int	CSSL_SESSION::i2d(unsigned char **pp)
{
	return i2d_SSL_SESSION(CastAnyPtr(SSL_SESSION, _raw), pp);
}

CSSL_SESSION *CSSL_SESSION::d2i(CSSL_SESSION **a, const unsigned char **pp, long length)
{
	Ptr(SSL_SESSION) pRet = NULL;
	Ptr(SSL_SESSION) pSession = d2i_SSL_SESSION(&pRet, pp, length);

	if (!pRet)
		return NULL;
	if (!pSession)
		return NULL;

	Ptr(CSSL_SESSION) p = CastDynamicPtr(CSSL_SESSION, find_obj(pRet));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pRet);
		p->insert_obj();
	}
	*a = p;

	p = CastDynamicPtr(CSSL_SESSION, find_obj(pSession));
	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pSession);
		p->insert_obj();
	}
	return p;
}

CSSL_SESSION *CSSL_SESSION::d2i_bio(CBIO* bp, CSSL_SESSION **s_id)
{
	Ptr(SSL_SESSION) pRet = NULL;
	Ptr(SSL_SESSION) pSession = d2i_SSL_SESSION_bio(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), &pRet);

	if (!pRet)
		return NULL;
	if (!pSession)
		return NULL;

	Ptr(CSSL_SESSION) p = CastDynamicPtr(CSSL_SESSION, find_obj(pRet));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pRet);
		p->insert_obj();
	}
	*s_id = p;

	p = CastDynamicPtr(CSSL_SESSION, find_obj(pSession));
	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pSession);
		p->insert_obj();
	}
	return p;
}

int CSSL_SESSION::i2d_bio(CBIO* bp)
{
	return i2d_SSL_SESSION_bio(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), CastAnyPtr(SSL_SESSION, _raw));
}

CSSL_SESSION *CSSL_SESSION::PEM_read_bio(CBIO *bp, CSSL_SESSION **x, TCallbackPEMPassword cb, void *u)
{
	Ptr(SSL_SESSION) pRet = NULL;
	Ptr(SSL_SESSION) pSession = PEM_read_bio_SSL_SESSION(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), &pRet, cb, u);

	if (!pRet)
		return NULL;
	if (!pSession)
		return NULL;

	Ptr(CSSL_SESSION) p = CastDynamicPtr(CSSL_SESSION, find_obj(pRet));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pRet);
		p->insert_obj();
	}
	*x = p;

	p = CastDynamicPtr(CSSL_SESSION, find_obj(pSession));
	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pSession);
		p->insert_obj();
	}
	return p;
}

CSSL_SESSION *CSSL_SESSION::PEM_read(FILE *fp, CSSL_SESSION **x, TCallbackPEMPassword cb, void *u)
{
	Ptr(SSL_SESSION) pRet = NULL;
	Ptr(SSL_SESSION) pSession = PEM_read_SSL_SESSION(fp, &pRet, cb, u);

	if (!pRet)
		return NULL;
	if (!pSession)
		return NULL;

	Ptr(CSSL_SESSION) p = CastDynamicPtr(CSSL_SESSION, find_obj(pRet));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pRet);
		p->insert_obj();
	}
	*x = p;

	p = CastDynamicPtr(CSSL_SESSION, find_obj(pSession));
	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pSession);
		p->insert_obj();
	}
	return p;
}

int CSSL_SESSION::PEM_write_bio(CBIO *bp)
{
	return PEM_write_bio_SSL_SESSION(CastAnyPtr(BIO, CastMutable(Pointer, bp->get_bio())), CastAnyPtr(SSL_SESSION, _raw));
}

int CSSL_SESSION::PEM_write(FILE *fp)
{
	return PEM_write_SSL_SESSION(fp, CastAnyPtr(SSL_SESSION, _raw));
}

int CSSL_SESSION::set_ex_data(int idx, void *data)
{
	return SSL_SESSION_set_ex_data(CastAnyPtr(SSL_SESSION, _raw), idx, data);
}

void *CSSL_SESSION::get_ex_data(int idx)
{
	return SSL_SESSION_get_ex_data(CastAnyPtr(SSL_SESSION, _raw), idx);
}

int CSSL_SESSION::get_ex_new_index(long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func)
{
	return SSL_SESSION_get_ex_new_index(argl, argp, new_func, dup_func, free_func);
}

class OPENSSL_LOCAL TSSLCallbackData : public CCppObject
{
public:
	TSSLCallbackData() : arg(NULL), cbMsg(NULL), cbPSKClient(NULL), cbPSKServer(NULL), cbVerify(NULL), cbGenSession(NULL), cbInfo(NULL),
		cbTmpRSA(NULL), cbTmpDH(NULL), cbTmpECDH(NULL), cbTLSSessionTicketExt(NULL), cbTLSSessionSecret(NULL) {}
	virtual ~TSSLCallbackData() {}

	void *arg;
	CSSL::TCallbackMsg cbMsg;
	CSSL::TCallbackPSKClient cbPSKClient;
	CSSL::TCallbackPSKServer cbPSKServer;
	CSSL::TCallbackVerify cbVerify;
	CSSL::TCallbackGenSession cbGenSession;
	CSSL::TCallbackInfo cbInfo;
	CSSL::TCallbackTmpRSA cbTmpRSA;
	CSSL::TCallbackTmpDH cbTmpDH;
	CSSL::TCallbackTmpECDH cbTmpECDH;
	CSSL::TCallbackTLSSessionTicketExt cbTLSSessionTicketExt;
	CSSL::TCallbackTLSSessionSecret cbTLSSessionSecret;
};

CSSL::CSSL(ConstPointer ssl) : COpenSSLClass(ssl) {}
CSSL::~CSSL() { free(); }

Ptr(CSSL) CSSL::create(Ptr(CSSL_CTX) pCtx)
{
	Ptr(SSL) pSSl = SSL_new(CastAnyPtr(SSL_CTX, CastMutable(Pointer, pCtx->get_sslContext())));

	if (!pSSl)
		return NULL;

	Ptr(CSSL) p = OK_NEW_OPERATOR CSSL(pSSl);

	p->insert_obj();
	return p;
}

Ptr(CSSL) CSSL::dup()
{
	Ptr(SSL) pSSl = SSL_dup(CastAnyPtr(SSL, _raw));

	if (!pSSl)
		return NULL;

	Ptr(CSSL) p = OK_NEW_OPERATOR CSSL(pSSl);

	p->insert_obj();
	return p;
}

int	CSSL::clear()
{
	return SSL_clear(CastAnyPtr(SSL, _raw));
}

void CSSL::free()
{
	if (_raw)
		SSL_free(CastAnyPtr(SSL, _raw));
	_raw = NULL;
}

long CSSL::set_options(long op)
{
	return SSL_set_options(CastAnyPtr(SSL, _raw), op);
}

long CSSL::clear_options(long op)
{
	return SSL_clear_options(CastAnyPtr(SSL, _raw), op);
}

long CSSL::get_options()
{
	return SSL_get_options(CastAnyPtr(SSL, _raw));
}

long CSSL::clear_mode(long op)
{
	return SSL_clear_mode(CastAnyPtr(SSL, _raw), op);
}

long CSSL::set_mode(long op)
{
	return SSL_set_mode(CastAnyPtr(SSL, _raw), op);
}

long CSSL::get_mode()
{
	return SSL_get_mode(CastAnyPtr(SSL, _raw));
}

int CSSL::set_mtu(long mtu)
{
	return SSL_set_mtu(CastAnyPtr(SSL, _raw), mtu);
}

int CSSL::get_secure_renegotiation_support()
{
	return SSL_get_secure_renegotiation_support(CastAnyPtr(SSL, _raw));
}

int CSSL::heartbeat()
{
	return SSL_heartbeat(CastAnyPtr(SSL, _raw));
}

static void SSLMsgCallback(int write_p, int version, int content_type, const void *buf, size_t len, SSL *ssl, void *arg)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	pCallbackData->cbMsg(write_p, version, content_type, buf, len, pSSL, arg);
}

void CSSL::set_msg_callback(TCallbackMsg cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbMsg = cb;
		SSL_set_msg_callback(CastAnyPtr(SSL, _raw), SSLMsgCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbMsg = NULL;
		SSL_set_msg_callback(CastAnyPtr(SSL, _raw), NULL);
	}
}

void CSSL::set_msg_callback_arg(void *arg)
{
	SSL_set_msg_callback_arg(CastAnyPtr(SSL, _raw), arg);
}

int CSSL::SRP_CTX_init()
{
	return SSL_SRP_CTX_init(CastAnyPtr(SSL, _raw));
}

int CSSL::SRP_CTX_free()
{
	return SSL_SRP_CTX_free(CastAnyPtr(SSL, _raw));
}

int CSSL::SRP_server_param_with_username(int *ad)
{
	return SSL_srp_server_param_with_username(CastAnyPtr(SSL, _raw), ad);
}

int CSSL::SRP_gen_server_master_secret(unsigned char *master_key)
{
	return SRP_generate_server_master_secret(CastAnyPtr(SSL, _raw), master_key);
}

int CSSL::SRP_calc_param()
{
	return SRP_Calc_A_param(CastAnyPtr(SSL, _raw));
}

int CSSL::SRP_gen_client_master_secret(unsigned char *master_key)
{
	return SRP_generate_client_master_secret(CastAnyPtr(SSL, _raw), master_key);
}

static unsigned int SSLPSKClient(SSL *ssl, const char *hint, char *identity, unsigned int max_identity_len, unsigned char *psk, unsigned int max_psk_len)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	return pCallbackData->cbPSKClient(pSSL, hint, identity, max_identity_len, psk, max_psk_len);
}

void CSSL::set_psk_client_callback(TCallbackPSKClient cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbPSKClient = cb;
		SSL_set_psk_client_callback(CastAnyPtr(SSL, _raw), SSLPSKClient);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbPSKClient = NULL;
		SSL_set_psk_client_callback(CastAnyPtr(SSL, _raw), NULL);
	}
}

static unsigned int SSLPSKServer(SSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	return pCallbackData->cbPSKServer(pSSL, identity, psk, max_psk_len);
}

void CSSL::set_psk_server_callback(TCallbackPSKServer cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbPSKServer = cb;
		SSL_set_psk_server_callback(CastAnyPtr(SSL, _raw), SSLPSKServer);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbPSKServer = NULL;
		SSL_set_psk_server_callback(CastAnyPtr(SSL, _raw), NULL);
	}
}

int CSSL::use_psk_identity_hint(const char *identity_hint)
{
	return SSL_use_psk_identity_hint(CastAnyPtr(SSL, _raw), identity_hint);
}

const char *CSSL::get_psk_identity_hint()
{
	return SSL_get_psk_identity_hint(CastAnyPtr(SSL, _raw));
}

const char *CSSL::get_psk_identity()
{
	return SSL_get_psk_identity(CastAnyPtr(SSL, _raw));
}

void CSSL::set_app_data(void *arg)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (arg)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->arg = arg;
	}
	else if (pCallbackData)
		pCallbackData->arg = NULL;
}

void *CSSL::get_app_data()
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (pCallbackData)
		return pCallbackData->arg;
	return NULL;
}

int CSSL::get_state()
{
	return SSL_get_state(CastAnyPtr(SSL, _raw));
}

int CSSL::is_init_finished()
{
	return SSL_is_init_finished(CastAnyPtr(SSL, _raw));
}

int CSSL::in_init()
{
	return SSL_in_init(CastAnyPtr(SSL, _raw));
}

int CSSL::in_before()
{
	return SSL_in_before(CastAnyPtr(SSL, _raw));
}

int CSSL::in_connect_init()
{
	return SSL_in_connect_init(CastAnyPtr(SSL, _raw));
}

int CSSL::in_accept_init()
{
	return SSL_in_accept_init(CastAnyPtr(SSL, _raw));
}

size_t CSSL::get_finished(void *buf, size_t count)
{
	return SSL_get_finished(CastAnyPtr(SSL, _raw), buf, count);
}

size_t CSSL::get_peer_finished(void *buf, size_t count)
{
	return SSL_get_peer_finished(CastAnyPtr(SSL, _raw), buf, count);
}

int CSSL::DTLS1_get_timeout(struct timeval* arg)
{
	return DTLSv1_get_timeout(CastAnyPtr(SSL, _raw), arg);
}

int CSSL::DTLS1_handle_timeout()
{
	return DTLSv1_handle_timeout(CastAnyPtr(SSL, _raw));
}

int CSSL::DTLS1_listen(struct sockaddr *peer)
{
	return DTLSv1_listen(CastAnyPtr(SSL, _raw), peer);
}

int CSSL::session_reused()
{
	return SSL_session_reused(CastAnyPtr(SSL, _raw));
}

int CSSL::num_renegotiations()
{
	return SSL_num_renegotiations(CastAnyPtr(SSL, _raw));
}

int CSSL::clear_num_renegotiations()
{
	return SSL_clear_num_renegotiations(CastAnyPtr(SSL, _raw));
}

int CSSL::total_renegotiations()
{
	return SSL_total_renegotiations(CastAnyPtr(SSL, _raw));
}

int CSSL::need_tmp_RSA()
{
	return SSL_need_tmp_RSA(CastAnyPtr(SSL, _raw));
}

int CSSL::set_tmp_rsa(CRSA_KEY* rsa)
{
	return SSL_set_tmp_rsa(CastAnyPtr(SSL, _raw), CastAnyPtr(RSA, CastMutable(Pointer, rsa->get_key())));
}

int CSSL::set_tmp_dh(CDH_KEY* dh)
{
	return SSL_set_tmp_dh(CastAnyPtr(SSL, _raw), CastAnyPtr(DH, CastMutable(Pointer, dh->get_key())));
}

int CSSL::set_tmp_ecdh(CEC_KEY *ecdh)
{
	return SSL_set_tmp_ecdh(CastAnyPtr(SSL, _raw), CastAnyPtr(EC_KEY, CastMutable(Pointer, ecdh->get_key())));
}

int CSSL::want()
{
	return SSL_want(CastAnyPtr(SSL, _raw));
}

ConstPtr(CSSL_CIPHER) CSSL::get_current_cipher()
{
	ConstPtr(SSL_CIPHER) pCipher = SSL_get_current_cipher(CastAnyPtr(SSL, _raw));

	if (!pCipher)
		return NULL;

	Ptr(CSSL_CIPHER) p = CastDynamicPtr(CSSL_CIPHER, find_obj(pCipher));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_CIPHER(pCipher);
		p->insert_obj();
	}
	return p;
}

int	CSSL::get_fd()
{
	return SSL_get_fd(CastAnyPtr(SSL, _raw));
}

int	CSSL::get_rfd()
{
	return SSL_get_rfd(CastAnyPtr(SSL, _raw));
}

int	CSSL::get_wfd()
{
	return SSL_get_wfd(CastAnyPtr(SSL, _raw));
}

const char *CSSL::get_cipher_list(int n)
{
	return SSL_get_cipher_list(CastAnyPtr(SSL, _raw), n);
}

char *CSSL::get_shared_ciphers(char *buf, int len)
{
	return SSL_get_shared_ciphers(CastAnyPtr(SSL, _raw), buf, len);
}

int	CSSL::get_read_ahead()
{
	return SSL_get_read_ahead(CastAnyPtr(SSL, _raw));
}

int	CSSL::pending()
{
	return SSL_pending(CastAnyPtr(SSL, _raw));
}

int	CSSL::set_fd(int fd)
{
	return SSL_set_fd(CastAnyPtr(SSL, _raw), fd);
}

int	CSSL::set_rfd(int fd)
{
	return SSL_set_rfd(CastAnyPtr(SSL, _raw), fd);
}

int	CSSL::set_wfd(int fd)
{
	return SSL_set_wfd(CastAnyPtr(SSL, _raw), fd);
}

void CSSL::set_bio(CBIO *rbio, CBIO *wbio)
{
	SSL_set_bio(CastAnyPtr(SSL, _raw), CastAnyPtr(BIO, CastMutable(Pointer, rbio->get_bio())), CastAnyPtr(BIO, CastMutable(Pointer, wbio->get_bio())));
}

CBIO * CSSL::get_rbio()
{
	Ptr(BIO) pBio = SSL_get_rbio(CastAnyPtr(SSL, _raw));

	if (!pBio)
		return NULL;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}
	return p;
}

CBIO * CSSL::get_wbio()
{
	Ptr(BIO) pBio = SSL_get_wbio(CastAnyPtr(SSL, _raw));

	if (!pBio)
		return NULL;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}
	return p;
}

int	CSSL::set_cipher_list(const char *str)
{
	return SSL_set_cipher_list(CastAnyPtr(SSL, _raw), str);
}

void CSSL::set_read_ahead(int yes)
{
	SSL_set_read_ahead(CastAnyPtr(SSL, _raw), yes);
}

int	CSSL::get_verify_mode()
{
	return SSL_get_verify_mode(CastAnyPtr(SSL, _raw));
}

int	CSSL::get_verify_depth()
{
	return SSL_get_verify_depth(CastAnyPtr(SSL, _raw));
}

CSSL::TCallbackVerify CSSL::get_verify_callback()
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (pCallbackData)
		return pCallbackData->cbVerify;
	return NULL;
}

static int SSLVerifyCallback(int ok, X509_STORE_CTX *ctx)
{
	Ptr(SSL) ssl = CastAnyPtr(SSL, X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx()));
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CX509_STORE_CTX) pCtx = CastDynamicPtr(CX509_STORE_CTX, COpenSSLClass::find_obj(ctx));

	if (!pCtx)
	{
		pCtx = OK_NEW_OPERATOR CX509_STORE_CTX(ctx);
		pCtx->insert_obj();
	}
	return pCallbackData->cbVerify(ok, pCtx);
}

void CSSL::set_verify(int mode, TCallbackVerify cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbVerify = cb;
		SSL_set_verify(CastAnyPtr(SSL, _raw), mode, SSLVerifyCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbVerify = NULL;
		SSL_set_verify(CastAnyPtr(SSL, _raw), mode, NULL);
	}
}

void CSSL::set_verify_depth(int depth)
{
	SSL_set_verify_depth(CastAnyPtr(SSL, _raw), depth);
}

int	CSSL::use_RSAPrivateKey(CRSA_KEY *rsa)
{
	return SSL_use_RSAPrivateKey(CastAnyPtr(SSL, _raw), CastAnyPtr(RSA, CastMutable(Pointer, rsa->get_key())));
}

int	CSSL::use_RSAPrivateKey_ASN1(unsigned char *d, long len)
{
	return SSL_use_RSAPrivateKey_ASN1(CastAnyPtr(SSL, _raw), d, len);
}

int	CSSL::use_PrivateKey(CEVP_PKEY *pkey)
{
	return SSL_use_PrivateKey(CastAnyPtr(SSL, _raw), CastAnyPtr(EVP_PKEY, CastMutable(Pointer, pkey->get_key())));
}

int	CSSL::use_PrivateKey_ASN1(int pk, const unsigned char *d, long len)
{
	return SSL_use_PrivateKey_ASN1(pk, CastAnyPtr(SSL, _raw), d, len);
}

int	CSSL::use_certificate(CX509 *x)
{
	return SSL_use_certificate(CastAnyPtr(SSL, _raw), CastAnyPtr(X509, CastMutable(Pointer, x->get_X509())));
}

int	CSSL::use_certificate_ASN1(const unsigned char *d, int len)
{
	return SSL_use_certificate_ASN1(CastAnyPtr(SSL, _raw), d, len);
}

int	CSSL::use_RSAPrivateKey_file(const char *file, TFileType type)
{
	return SSL_use_RSAPrivateKey_file(CastAnyPtr(SSL, _raw), file, type);
}

int	CSSL::use_PrivateKey_file(const char *file, TFileType type)
{
	return SSL_use_PrivateKey_file(CastAnyPtr(SSL, _raw), file, type);
}

int	CSSL::use_certificate_file(const char *file, TFileType type)
{
	return SSL_use_certificate_file(CastAnyPtr(SSL, _raw), file, type);
}

const char *CSSL::state_string()
{
	return SSL_state_string(CastAnyPtr(SSL, _raw));
}

const char *CSSL::rstate_string()
{
	return SSL_rstate_string(CastAnyPtr(SSL, _raw));
}

const char *CSSL::state_string_long()
{
	return SSL_state_string_long(CastAnyPtr(SSL, _raw));
}

const char *CSSL::rstate_string_long()
{
	return SSL_rstate_string_long(CastAnyPtr(SSL, _raw));
}

void CSSL::copy_session_id(CSSL *to)
{
	SSL_copy_session_id(CastAnyPtr(SSL, _raw), CastAnyPtr(SSL, CastMutable(Pointer, to->get_ssl())));
}

int	CSSL::set_session(CSSL_SESSION *session)
{
	return SSL_set_session(CastAnyPtr(SSL, _raw), CastAnyPtr(SSL_SESSION, CastMutable(Pointer, session->get_sslsession())));
}

static int SSLGenSessionCallback(const SSL *ssl, unsigned char *id, unsigned int *id_len)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyConstPtr(SSL, ssl)));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	return pCallbackData->cbGenSession(pSSL, id, id_len);
}

void CSSL::set_generate_session_id(TCallbackGenSession cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbGenSession = cb;
		SSL_set_generate_session_id(CastAnyPtr(SSL, _raw), SSLGenSessionCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbGenSession = NULL;
		SSL_set_generate_session_id(CastAnyPtr(SSL, _raw), NULL);
	}
}

int CSSL::has_matching_session_id(const unsigned char *id, unsigned int id_len)
{
	return SSL_has_matching_session_id(CastAnyPtr(SSL, _raw), id, id_len);
}

CX509 *CSSL::get_peer_certificate()
{
	Ptr(X509) pX509 = SSL_get_peer_certificate(CastAnyPtr(SSL, _raw));

	if (!pX509)
		return NULL;

	Ptr(CX509) p = CastDynamicPtr(CX509, find_obj(pX509));

	if (!p)
	{
		p = OK_NEW_OPERATOR CX509(pX509);
		p->insert_obj();
	}
	return p;
}

CStack<CX509> CSSL::get_peer_cert_chain()
{
	STACK_OF(X509) * pStack = SSL_get_peer_cert_chain(CastAnyPtr(SSL, _raw));
	CStack<CX509> ret __FILE__LINE__0P;

	for (int i = 0; i < sk_X509_num(pStack); ++i)
	{
		Ptr(X509) pX509 = sk_X509_value(pStack, i);
		Ptr(CX509) p = CastDynamicPtr(CX509, find_obj(pX509));

		if (!p)
		{
			p = OK_NEW_OPERATOR CX509(pX509);
			p->insert_obj();
		}
		ret.Push(p);
	}
	return ret;
}

int CSSL::check_private_key()
{
	return SSL_check_private_key(CastAnyPtr(SSL, _raw));
}

int	CSSL::set_session_id_context(const unsigned char *sid_ctx, unsigned int sid_ctx_len)
{
	return SSL_set_session_id_context(CastAnyPtr(SSL, _raw), sid_ctx, sid_ctx_len);
}

int CSSL::set_purpose(int purpose)
{
	return SSL_set_purpose(CastAnyPtr(SSL, _raw), purpose);
}

int CSSL::set_trust(int trust)
{
	return SSL_set_trust(CastAnyPtr(SSL, _raw), trust);
}

int CSSL::set1_param(CX509_VERIFY_PARAM *vpm)
{
	return SSL_set1_param(CastAnyPtr(SSL, _raw), CastAnyPtr(X509_VERIFY_PARAM, CastMutable(Pointer, vpm->get_X509VerifyParam())));
}

int CSSL::accept()
{
	return SSL_accept(CastAnyPtr(SSL, _raw));
}

int CSSL::connect()
{
	return SSL_connect(CastAnyPtr(SSL, _raw));
}

int CSSL::read(void *buf, int num)
{
	return SSL_read(CastAnyPtr(SSL, _raw), buf, num);
}

int CSSL::peek(void *buf, int num)
{
	return SSL_peek(CastAnyPtr(SSL, _raw), buf, num);
}

int CSSL::write(const void *buf, int num)
{
	return SSL_write(CastAnyPtr(SSL, _raw), buf, num);
}

int	CSSL::get_error(int ret_code)
{
	return SSL_get_error(CastAnyPtr(SSL, _raw), ret_code);
}

const char *CSSL::get_version()
{
	return SSL_get_version(CastAnyPtr(SSL, _raw));
}

CStack<CSSL_CIPHER> CSSL::get_ciphers()
{
	STACK_OF(SSL_CIPHER) * pStack = SSL_get_ciphers(CastAnyPtr(SSL, _raw));
	CStack<CSSL_CIPHER> ret __FILE__LINE__0P;

	for (int i = 0; i < sk_SSL_CIPHER_num(pStack); ++i)
	{
		Ptr(SSL_CIPHER) pCipher = sk_SSL_CIPHER_value(pStack, i);
		Ptr(CSSL_CIPHER) p = CastDynamicPtr(CSSL_CIPHER, find_obj(pCipher));

		if (!p)
		{
			p = OK_NEW_OPERATOR CSSL_CIPHER(pCipher);
			p->insert_obj();
		}
		ret.Push(p);
	}
	return ret;
}

int CSSL::do_handshake()
{
	return SSL_do_handshake(CastAnyPtr(SSL, _raw));
}

int CSSL::renegotiate()
{
	return SSL_renegotiate(CastAnyPtr(SSL, _raw));
}

int CSSL::renegotiate_abbreviated()
{
	return SSL_renegotiate_abbreviated(CastAnyPtr(SSL, _raw));
}

int CSSL::renegotiate_pending()
{
	return SSL_renegotiate_pending(CastAnyPtr(SSL, _raw));
}

int CSSL::shutdown()
{
	return SSL_shutdown(CastAnyPtr(SSL, _raw));
}

ConstPtr(CSSL_METHOD) CSSL::get_method()
{
	ConstPtr(SSL_METHOD) pMethod = SSL_get_ssl_method(CastAnyPtr(SSL, _raw));

	if (!pMethod)
		return NULL;

	Ptr(CSSL_METHOD) p = CastDynamicPtr(CSSL_METHOD, find_obj(pMethod));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_METHOD(pMethod);
		p->insert_obj();
	}
	return p;
}

int CSSL::set_method(ConstPtr(CSSL_METHOD) method)
{
	return SSL_set_ssl_method(CastAnyPtr(SSL, _raw), CastAnyPtr(SSL_METHOD, CastMutable(Pointer, method->get_sslMethod())));
}

void CSSL::set_client_CA_list(ConstRef(CStack<CX509_NAME>) name_list)
{
	STACK_OF(X509_NAME) *pStack = sk_X509_NAME_new_null();

	for (TListCnt i = 0; i < name_list.Count(); ++i)
	{
		ConstPtr(CX509_NAME) pValue = name_list.Value(i);

		sk_X509_NAME_push(pStack, CastAnyPtr(X509_NAME, CastMutable(Pointer, pValue->get_X509Name())));
	}
	SSL_set_client_CA_list(CastAnyPtr(SSL, _raw), pStack);
}

CStack<CX509_NAME> CSSL::get_client_CA_list()
{
	STACK_OF(X509_NAME) * pStack = SSL_get_client_CA_list(CastAnyPtr(SSL, _raw));
	CStack<CX509_NAME> ret __FILE__LINE__0P;

	for (int i = 0; i < sk_X509_NAME_num(pStack); ++i)
	{
		Ptr(X509_NAME) pX509Name = sk_X509_NAME_value(pStack, i);
		Ptr(CX509_NAME) p = CastDynamicPtr(CX509_NAME, find_obj(pX509Name));

		if (!p)
		{
			p = OK_NEW_OPERATOR CX509_NAME(pX509Name);
			p->insert_obj();
		}
		ret.Push(p);
	}
	return ret;
}

int CSSL::add_client_CA(CX509 *x)
{
	return SSL_add_client_CA(CastAnyPtr(SSL, _raw), CastAnyPtr(X509, CastMutable(Pointer, x->get_X509())));
}

void CSSL::set_connect_state()
{
	SSL_set_connect_state(CastAnyPtr(SSL, _raw));
}

void CSSL::set_accept_state()
{
	SSL_set_accept_state(CastAnyPtr(SSL, _raw));
}

long CSSL::get_default_timeout()
{
	return SSL_get_default_timeout(CastAnyPtr(SSL, _raw));
}

void CSSL::set_quiet_shutdown(int mode)
{
	SSL_set_quiet_shutdown(CastAnyPtr(SSL, _raw), mode);
}

int CSSL::get_quiet_shutdown()
{
	return SSL_get_quiet_shutdown(CastAnyPtr(SSL, _raw));
}

void CSSL::set_shutdown(int mode)
{
	SSL_set_shutdown(CastAnyPtr(SSL, _raw), mode);
}

int CSSL::get_shutdown()
{
	return SSL_get_shutdown(CastAnyPtr(SSL, _raw));
}

int CSSL::version()
{
	return SSL_version(CastAnyPtr(SSL, _raw));
}

CSSL_SESSION *CSSL::get0_session() /* just peek at pointer */
{
	Ptr(SSL_SESSION) pSession = SSL_get0_session(CastAnyPtr(SSL, _raw));

	if (!pSession)
		return NULL;

	Ptr(CSSL_SESSION) p = CastDynamicPtr(CSSL_SESSION, find_obj(pSession));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pSession);
		p->insert_obj();
	}
	return p;
}

CSSL_SESSION *CSSL::get1_session() /* obtain a reference count */
{
	Ptr(SSL_SESSION) pSession = SSL_get1_session(CastAnyPtr(SSL, _raw));

	if (!pSession)
		return NULL;

	Ptr(CSSL_SESSION) p = CastDynamicPtr(CSSL_SESSION, find_obj(pSession));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_SESSION(pSession);
		p->insert_obj();
	}
	return p;
}

CSSL_CTX *CSSL::get_CTX()
{
	Ptr(SSL_CTX) pCtx = SSL_get_SSL_CTX(CastAnyPtr(SSL, _raw));

	if (!pCtx)
		return NULL;

	Ptr(CSSL_CTX) p = CastDynamicPtr(CSSL_CTX, find_obj(pCtx));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_CTX(pCtx);
		p->insert_obj();
	}
	return p;
}

CSSL_CTX *CSSL::set_CTX(CSSL_CTX* ctx)
{
	Ptr(SSL_CTX) pCtx = SSL_set_SSL_CTX(CastAnyPtr(SSL, _raw), CastAnyPtr(SSL_CTX, CastMutable(Pointer, ctx->get_sslContext())));

	if (!pCtx)
		return NULL;

	Ptr(CSSL_CTX) p = CastDynamicPtr(CSSL_CTX, find_obj(pCtx));

	if (!p)
	{
		p = OK_NEW_OPERATOR CSSL_CTX(pCtx);
		p->insert_obj();
	}
	return p;
}

static void SSLInfoCallback(const SSL *ssl, int type, int val)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	return pCallbackData->cbInfo(pSSL, type, val);
}

void CSSL::set_info_callback(TCallbackInfo cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbInfo = cb;
		SSL_set_info_callback(CastAnyPtr(SSL, _raw), SSLInfoCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbInfo = NULL;
		SSL_set_info_callback(CastAnyPtr(SSL, _raw), NULL);
	}
}

CSSL::TCallbackInfo CSSL::get_info_callback()
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (pCallbackData)
		return pCallbackData->cbInfo;
	return NULL;
}

int CSSL::state()
{
	return SSL_state(CastAnyPtr(SSL, _raw));
}

void CSSL::set_state(int state)
{
	SSL_set_state(CastAnyPtr(SSL, _raw), state);
}

void CSSL::set_verify_result(long v)
{
	SSL_set_verify_result(CastAnyPtr(SSL, _raw), v);
}

long CSSL::get_verify_result()
{
	return SSL_get_verify_result(CastAnyPtr(SSL, _raw));
}

int CSSL::set_ex_data(int idx, void *data)
{
	return SSL_set_ex_data(CastAnyPtr(SSL, _raw), idx, data);
}

void *CSSL::get_ex_data(int idx)
{
	return SSL_get_ex_data(CastAnyPtr(SSL, _raw), idx);
}

int CSSL::get_ex_new_index(long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func)
{
	return SSL_get_ex_new_index(argl, argp, new_func, dup_func, free_func);
}

int CSSL::get_ex_data_X509_STORE_CTX_idx(void)
{
	return SSL_get_ex_data_X509_STORE_CTX_idx();
}

int CSSL::get_max_cert_list()
{
	return SSL_get_max_cert_list(CastAnyPtr(SSL, _raw));
}

void CSSL::set_max_cert_list(int m)
{
	SSL_set_max_cert_list(CastAnyPtr(SSL, _raw), m);
}

void CSSL::set_max_send_fragment(int m)
{
	SSL_set_max_send_fragment(CastAnyPtr(SSL, _raw), m);
}

static RSA *SSLTmpRSACallback(SSL *ssl, int is_export, int keylength)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	Ptr(CRSA_KEY) pKey = NULL;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	pKey = pCallbackData->cbTmpRSA(pSSL, is_export, keylength);

	return CastAnyPtr(RSA, CastMutable(Pointer, pKey->get_key()));
}

void CSSL::set_tmp_rsa_callback(TCallbackTmpRSA cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbTmpRSA = cb;
		SSL_set_tmp_rsa_callback(CastAnyPtr(SSL, _raw), SSLTmpRSACallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbTmpRSA = NULL;
		SSL_set_tmp_rsa_callback(CastAnyPtr(SSL, _raw), NULL);
	}
}

static DH *SSLTmpDHCallback(SSL *ssl, int is_export, int keylength)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	Ptr(CDH_KEY) pKey = NULL;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	pKey = pCallbackData->cbTmpDH(pSSL, is_export, keylength);

	return CastAnyPtr(DH, CastMutable(Pointer, pKey->get_key()));
}

void CSSL::set_tmp_dh_callback(TCallbackTmpDH cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbTmpDH = cb;
		SSL_set_tmp_dh_callback(CastAnyPtr(SSL, _raw), SSLTmpDHCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbTmpDH = NULL;
		SSL_set_tmp_dh_callback(CastAnyPtr(SSL, _raw), NULL);
	}
}

static EC_KEY *SSLTmpECDHCallback(SSL *ssl, int is_export, int keylength)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	Ptr(CEC_KEY) pKey = NULL;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	pKey = pCallbackData->cbTmpECDH(pSSL, is_export, keylength);

	return CastAnyPtr(EC_KEY, CastMutable(Pointer, pKey->get_key()));
}

void CSSL::set_tmp_ecdh_callback(TCallbackTmpECDH cb)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbTmpECDH = cb;
		SSL_set_tmp_ecdh_callback(CastAnyPtr(SSL, _raw), SSLTmpECDHCallback);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbTmpECDH = NULL;
		SSL_set_tmp_ecdh_callback(CastAnyPtr(SSL, _raw), NULL);
	}
}

int CSSL::set_session_ticket_ext(void *ext_data, int ext_len)
{
	return SSL_set_session_ticket_ext(CastAnyPtr(SSL, _raw), ext_data, ext_len);
}

static int SSLTLSSessionTicketExtCallback(SSL *ssl, const unsigned char *data, int len, void *arg)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	return pCallbackData->cbTLSSessionTicketExt(pSSL, data, len, arg);
}

void CSSL::set_session_ticket_ext_cb(TCallbackTLSSessionTicketExt cb, void *arg)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbTLSSessionTicketExt = cb;
		SSL_set_session_ticket_ext_cb(CastAnyPtr(SSL, _raw), SSLTLSSessionTicketExtCallback, arg);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbTLSSessionTicketExt = NULL;
		SSL_set_session_ticket_ext_cb(CastAnyPtr(SSL, _raw), NULL, NULL);
	}
}

static int SSLTLSSessionSecretCallback(SSL *ssl, void *secret, int *secret_len, STACK_OF(SSL_CIPHER) *peer_ciphers, SSL_CIPHER **cipher, void *arg)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(ssl));
	Ptr(CSSL) pSSL = CastDynamicPtr(CSSL, COpenSSLClass::find_obj(ssl));
	CStack<CSSL_CIPHER> stack __FILE__LINE__0P;
	Ptr(CSSL_CIPHER) pCipher = NULL;
	int ret;

	if (!pSSL)
	{
		pSSL = OK_NEW_OPERATOR CSSL(ssl);
		pSSL->insert_obj();
	}
	for (int i = 0; i < sk_SSL_CIPHER_num(peer_ciphers); ++i)
	{
		Ptr(SSL_CIPHER) pCipher = sk_SSL_CIPHER_value(peer_ciphers, i);
		Ptr(CSSL_CIPHER) p = CastDynamicPtr(CSSL_CIPHER, COpenSSLClass::find_obj(pCipher));

		if (!p)
		{
			p = OK_NEW_OPERATOR CSSL_CIPHER(pCipher);
			p->insert_obj();
		}
		stack.Push(p);
	}
	ret =  pCallbackData->cbTLSSessionSecret(pSSL, secret, secret_len, stack, &pCipher, arg);
	*cipher = CastAnyPtr(SSL_CIPHER, CastMutable(Pointer, pCipher->get_sslcipher()));
	return ret;
}

void CSSL::set_session_secret_cb(TCallbackTLSSessionSecret cb, void *arg)
{
	Ptr(TSSLCallbackData) pCallbackData = CastAnyPtr(TSSLCallbackData, SSL_get_app_data(CastAnyPtr(SSL, _raw)));

	if (cb)
	{
		if (!pCallbackData)
		{
			pCallbackData = OK_NEW_OPERATOR TSSLCallbackData;
			SSL_set_app_data(CastAnyPtr(SSL, _raw), pCallbackData);
		}
		pCallbackData->cbTLSSessionSecret = cb;
		SSL_set_session_secret_cb(CastAnyPtr(SSL, _raw), SSLTLSSessionSecretCallback, arg);
	}
	else if (pCallbackData)
	{
		pCallbackData->cbTLSSessionSecret = NULL;
		SSL_set_session_secret_cb(CastAnyPtr(SSL, _raw), NULL, NULL);
	}
}

void CSSL::set_debug(int debug)
{
	SSL_set_debug(CastAnyPtr(SSL, _raw), debug);
}

int CSSL::cache_hit()
{
	return SSL_cache_hit(CastAnyPtr(SSL, _raw));
}

