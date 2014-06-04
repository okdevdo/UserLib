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
#include "BIO.h"
#include "BIO_METHOD.h"
#include "EVP_MD.h"
#include "EVP_CIPHER.h"
#include "SSL.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

class OPENSSL_LOCAL TCallbackData : public CCppObject
{
public:
	TCallbackData() {}
	virtual ~TCallbackData() {}

	CBIO::TCallback cb;
	void *arg;
};

CBIO::CBIO(ConstPointer bio) : COpenSSLClass(bio)
{
}

CBIO::~CBIO()
{
	free();
}

Ptr(CBIO) CBIO::create(Ptr(CBIO_METHOD) type)
{
	Ptr(BIO) pBio = BIO_new(CastAnyPtr(BIO_METHOD, CastMutable(Pointer, type->get_method())));

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = OK_NEW_OPERATOR CBIO(pBio);

	p->insert_obj();
	return p;
}

int CBIO::set_method(Ptr(CBIO_METHOD) type)
{
	return BIO_set(CastAnyPtr(BIO, _raw), CastAnyPtr(BIO_METHOD, CastMutable(Pointer, type->get_method())));
}

void CBIO::free()
{
	if (_raw)
	{
		TCallbackData* pCallBack = CastAnyPtr(TCallbackData, BIO_get_callback_arg(CastAnyPtr(BIO, _raw)));

		if (pCallBack)
		{
			pCallBack->release();
			BIO_set_callback_arg(CastAnyPtr(BIO, _raw), nullptr);
		}

		Ptr(BIO) pBio = BIO_pop(CastAnyPtr(BIO, _raw));

		while (pBio)
		{
			Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

			if (p)
				p->remove_obj();

			pBio = BIO_pop(CastAnyPtr(BIO, _raw));
		}
		BIO_free(CastAnyPtr(BIO, _raw));
		_raw = nullptr;
	}
}

CBIO* CBIO::find_type(int bio_type)
{
	BIO* pBio = BIO_find_type(CastAnyPtr(BIO, _raw), bio_type);

	if (pBio)
	{
		Ptr(COpenSSLClass) pClass = find_obj(pBio);

		if (pClass)
			return CastDynamicPtr(CBIO, pClass);

		Ptr(CBIO) p = OK_NEW_OPERATOR CBIO(pBio);

		p->insert_obj();
		return p;
	}
	return nullptr;
}

CBIO* CBIO::next()
{
	BIO* pBio = BIO_next(CastAnyPtr(BIO, _raw));

	if (pBio)
	{
		Ptr(COpenSSLClass) pClass = find_obj(pBio);

		if (pClass)
			return CastDynamicPtr(CBIO, pClass);

		Ptr(CBIO) p = OK_NEW_OPERATOR CBIO(pBio);

		p->insert_obj();
		return p;
	}
	return nullptr;
}

int CBIO::method_type()
{
	return BIO_method_type(CastAnyPtr(BIO, _raw));
}

int CBIO::reset()
{
	return BIO_reset(CastAnyPtr(BIO, _raw));
}

int CBIO::seek(int ofs)
{
	return BIO_seek(CastAnyPtr(BIO, _raw), ofs);
}

int CBIO::tell()
{
	return BIO_tell(CastAnyPtr(BIO, _raw));
}

int CBIO::flush()
{
	return BIO_flush(CastAnyPtr(BIO, _raw));
}

int CBIO::eof()
{
	return BIO_eof(CastAnyPtr(BIO, _raw));
}

int CBIO::set_close(long flag)
{
	return BIO_set_close(CastAnyPtr(BIO, _raw), flag);
}

int CBIO::get_close()
{
	return BIO_get_close(CastAnyPtr(BIO, _raw));
}

int CBIO::pending()
{
	return BIO_pending(CastAnyPtr(BIO, _raw));
}

size_t CBIO::ctrl_pending()
{
	return BIO_ctrl_pending(CastAnyPtr(BIO, _raw));
}

int CBIO::wpending()
{
	return BIO_wpending(CastAnyPtr(BIO, _raw));
}

size_t CBIO::ctrl_wpending()
{
	return BIO_ctrl_wpending(CastAnyPtr(BIO, _raw));
}

int CBIO::read(void *buf, int len)
{
	return BIO_read(CastAnyPtr(BIO, _raw), buf, len);
}

int CBIO::gets(char *buf, int size)
{
	return BIO_gets(CastAnyPtr(BIO, _raw), buf, size);
}

int CBIO::write(const void *buf, int len)
{
	return BIO_write(CastAnyPtr(BIO, _raw), buf, len);
}

int CBIO::puts(const char *buf)
{
	return BIO_puts(CastAnyPtr(BIO, _raw), buf);
}

int CBIO::printf(const char *format, ...)
{
	va_list args;
	int ret;

	va_start(args, format);
	ret = vprintf(format, args);
	va_end(args);
	return ret;
}

int CBIO::vprintf(const char *format, va_list args)
{
	return BIO_vprintf(CastAnyPtr(BIO, _raw), format, args);
}

CBIO* CBIO::push(CBIO *append)
{
	BIO* ret = BIO_push(CastAnyPtr(BIO, _raw), CastAnyPtr(BIO, CastMutable(Pointer, append->get_bio())));

	if (ret == _raw)
		return this;

	Ptr(COpenSSLClass) pClass = find_obj(ret);

	if (pClass)
		return CastDynamicPtr(CBIO, pClass);

	Ptr(CBIO) p = OK_NEW_OPERATOR CBIO(ret);

	p->insert_obj();
	return p;
}

CBIO* CBIO::pop()
{
	BIO* pBio = BIO_pop(CastAnyPtr(BIO, _raw));

	if (pBio)
	{
		Ptr(COpenSSLClass) pClass = find_obj(pBio);

		if (pClass)
			return CastDynamicPtr(CBIO, pClass);

		Ptr(CBIO) p = OK_NEW_OPERATOR CBIO(pBio);

		p->insert_obj();
		return p;
	}
	return nullptr;
}

static long stdcallback(BIO *b, int oper, const char *argp, int argi, long argl, long retvalue)
{
	TCallbackData* cbdata = CastAnyPtr(TCallbackData, BIO_get_callback_arg(b));
	CBIO* pBio = CastDynamicPtr(CBIO, COpenSSLClass::find_obj(b));
	long ret;

	if (!pBio)
	{
		pBio = OK_NEW_OPERATOR CBIO(b);
		pBio->insert_obj();
	}
	BIO_set_callback_arg(b, CastAnyPtr(char, cbdata->arg));
	ret = (cbdata->cb)(pBio, oper, argp, argi, argl, retvalue);
	BIO_set_callback_arg(b, CastAnyPtr(char, cbdata));
	return ret;
}

void CBIO::set_callback(TCallback cb)
{
	if (cb)
	{
		TCallbackData* cbdata = OK_NEW_OPERATOR TCallbackData;

		cbdata->cb = cb;
		cbdata->arg = nullptr;
		BIO_set_callback_arg(CastAnyPtr(BIO, _raw), CastAnyPtr(char, cbdata));
		BIO_set_callback(CastAnyPtr(BIO, _raw), stdcallback);
	}
	else
	{
		TCallbackData* cbdata = CastAnyPtr(TCallbackData, BIO_get_callback_arg(CastAnyPtr(BIO, _raw)));

		cbdata->release();
		BIO_set_callback_arg(CastAnyPtr(BIO, _raw), nullptr);
		BIO_set_callback(CastAnyPtr(BIO, _raw), nullptr);
	}
}

CBIO::TCallback CBIO::get_callback()
{
	TCallbackData* cbdata = CastAnyPtr(TCallbackData, BIO_get_callback_arg(CastAnyPtr(BIO, _raw)));

	if (cbdata)
		return cbdata->cb;
	return nullptr;
}

void CBIO::set_callback_arg(void *arg)
{
	TCallbackData* cbdata = CastAnyPtr(TCallbackData, BIO_get_callback_arg(CastAnyPtr(BIO, _raw)));

	if (cbdata)
		cbdata->arg = arg;
}

void* CBIO::get_callback_arg()
{
	TCallbackData* cbdata = CastAnyPtr(TCallbackData, BIO_get_callback_arg(CastAnyPtr(BIO, _raw)));

	if (cbdata)
		return cbdata->arg;
	return nullptr;
}

long CBIO::debug_callback(CBIO *bio, int cmd, const char *argp, int argi, long argl, long ret)
{
	return BIO_debug_callback(CastAnyPtr(BIO, CastMutable(Pointer, bio->get_bio())), cmd, argp, argi, argl, ret);
}

int CBIO::should_read()
{
	return BIO_should_read(CastAnyPtr(BIO, _raw));
}

int CBIO::should_write()
{
	return BIO_should_write(CastAnyPtr(BIO, _raw));
}

int CBIO::should_io_special()
{
	return BIO_should_io_special(CastAnyPtr(BIO, _raw));
}

int CBIO::retry_type()
{
	return BIO_retry_type(CastAnyPtr(BIO, _raw));
}

int CBIO::should_retry()
{
	return BIO_should_retry(CastAnyPtr(BIO, _raw));
}

CBIO* CBIO::get_retry_BIO(int *reason)
{
	BIO* ret = BIO_get_retry_BIO(CastAnyPtr(BIO, _raw), reason);

	if (!ret)
		return nullptr;

	Ptr(COpenSSLClass) pClass = find_obj(ret);

	if (pClass)
		return CastDynamicPtr(CBIO, pClass);

	Ptr(CBIO) p = OK_NEW_OPERATOR CBIO(ret);

	p->insert_obj();
	return p;
}

int CBIO::get_retry_reason()
{
	return BIO_get_retry_reason(CastAnyPtr(BIO, _raw));
}

int CBIO::get_buffer_num_lines()
{
	return BIO_get_buffer_num_lines(CastAnyPtr(BIO, _raw));
}

int CBIO::set_read_buffer_size(int size)
{
	return BIO_set_read_buffer_size(CastAnyPtr(BIO, _raw), size);
}

int CBIO::set_write_buffer_size(int size)
{
	return BIO_set_write_buffer_size(CastAnyPtr(BIO, _raw), size);
}

int CBIO::set_buffer_size(int size)
{
	return BIO_set_buffer_size(CastAnyPtr(BIO, _raw), size);
}

int CBIO::set_buffer_read_data(const char *buf, int num)
{
	return BIO_set_buffer_read_data(CastAnyPtr(BIO, _raw), CastAny(Pointer, CastMutablePtr(char, buf)), num);
}

void CBIO::set_cipher(const CEVP_CIPHER *cipher, unsigned char *key, unsigned char *iv, int enc)
{
	BIO_set_cipher(CastAnyPtr(BIO, _raw), CastAnyConstPtr(EVP_CIPHER, cipher->get_cipher()), key, iv, enc);
}

int CBIO::get_cipher_status()
{
	return BIO_get_cipher_status(CastAnyPtr(BIO, _raw));
}

int CBIO::get_cipher_ctx(CEVP_CIPHER_CTX **pctx)
{
	EVP_CIPHER_CTX *ctx;
	int ret;

	ret = BIO_get_cipher_ctx(CastAnyPtr(BIO, _raw), &ctx);
	if (!ctx)
		return ret;
	*pctx = CastDynamicPtr(CEVP_CIPHER_CTX, find_obj(ctx));
	if (!(*pctx))
	{
		*pctx = OK_NEW_OPERATOR CEVP_CIPHER_CTX(ctx);
		(*pctx)->insert_obj();
	}
	return ret;
}

int CBIO::set_md(CEVP_MD *md)
{
	return BIO_set_md(CastAnyPtr(BIO, _raw), md->get_md());
}

int CBIO::get_md(CEVP_MD **mdp)
{
	int ret;
	EVP_MD* pMd;

	ret = BIO_get_md(CastAnyPtr(BIO, _raw), &pMd);
	if (!pMd)
		return ret;
	*mdp = CastDynamicPtr(CEVP_MD, find_obj(pMd));
	if (!(*mdp))
	{
		*mdp = OK_NEW_OPERATOR CEVP_MD(pMd);
		(*mdp)->insert_obj();
	}
	return ret;
}

int CBIO::get_md_ctx(CEVP_MD_CTX **mdcp)
{
	int ret;
	EVP_MD_CTX* pMdCtx;

	ret = BIO_get_md_ctx(CastAnyPtr(BIO, _raw), &pMdCtx);
	if (!pMdCtx)
		return ret;
	*mdcp = CastDynamicPtr(CEVP_MD_CTX, find_obj(pMdCtx));
	if (!(*mdcp))
	{
		*mdcp = OK_NEW_OPERATOR CEVP_MD_CTX(pMdCtx);
		(*mdcp)->insert_obj();
	}
	return ret;
}

int CBIO::set_ssl(CSSL* ssl, int c)
{
	return BIO_set_ssl(CastAnyPtr(BIO, _raw), ssl->get_ssl(), c);
}

int CBIO::get_ssl(CSSL**sslp)
{
	SSL* pSsl;
	int ret;
		
	ret = BIO_get_ssl(CastAnyPtr(BIO, _raw), &pSsl);
	if (!pSsl)
		return ret;
	*sslp = CastDynamicPtr(CSSL, find_obj(pSsl));
	if (!(*sslp))
	{
		*sslp = OK_NEW_OPERATOR CSSL(pSsl);
		(*sslp)->insert_obj();
	}
	return ret;
}

int CBIO::set_ssl_mode(int client)
{
	return BIO_set_ssl_mode(CastAnyPtr(BIO, _raw), client);
}

int CBIO::set_ssl_renegotiate_bytes(int num)
{
	return BIO_set_ssl_renegotiate_bytes(CastAnyPtr(BIO, _raw), num);
}

int CBIO::set_ssl_renegotiate_timeout(int seconds)
{
	return BIO_set_ssl_renegotiate_timeout(CastAnyPtr(BIO, _raw), seconds);
}

int CBIO::get_num_renegotiates()
{
	return BIO_get_num_renegotiates(CastAnyPtr(BIO, _raw));
}

Ptr(CBIO) CBIO::create_ssl(Ptr(CSSL_CTX) ctx, int client)
{
	Ptr(BIO) pBio = BIO_new_ssl(CastAnyPtr(SSL_CTX, CastMutable(Pointer, ctx->get_sslContext())), client);

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}
	return p;
}

Ptr(CBIO) CBIO::create_ssl_connect(Ptr(CSSL_CTX) ctx)
{
	Ptr(BIO) pBio = BIO_new_ssl_connect(CastAnyPtr(SSL_CTX, CastMutable(Pointer, ctx->get_sslContext())));

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}
	return p;
}

Ptr(CBIO) CBIO::create_buffer_ssl_connect(Ptr(CSSL_CTX) ctx)
{
	Ptr(BIO) pBio = BIO_new_buffer_ssl_connect(CastAnyPtr(SSL_CTX, CastMutable(Pointer, ctx->get_sslContext())));

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}
	return p;
}

int CBIO::ssl_copy_session_id(CBIO *to, CBIO *from)
{
	return BIO_ssl_copy_session_id(CastAnyPtr(BIO, CastMutable(Pointer, to->get_bio())), CastAnyPtr(BIO, CastMutable(Pointer, from->get_bio())));
}

void CBIO::ssl_shutdown()
{
	BIO_ssl_shutdown(CastAnyPtr(BIO, _raw));
}

int CBIO::do_handshake()
{
	return BIO_do_handshake(CastAnyPtr(BIO, _raw));
}

int CBIO::make_bio_pair(CBIO* b2)
{
	return BIO_make_bio_pair(CastAnyPtr(BIO, _raw), CastMutable(Pointer, b2->get_bio()));
}

int CBIO::destroy_bio_pair()
{
	return BIO_destroy_bio_pair(CastAnyPtr(BIO, _raw));
}

int CBIO::shutdown_wr()
{
	return BIO_shutdown_wr(CastAnyPtr(BIO, _raw));
}

int CBIO::set_write_buf_size(int size)
{
	return BIO_set_write_buf_size(CastAnyPtr(BIO, _raw), size);
}

size_t CBIO::get_write_buf_size(int size)
{
	return BIO_get_write_buf_size(CastAnyPtr(BIO, _raw), size);
}

int CBIO::create_bio_pair(CBIO **bio1, size_t writebuf1, CBIO **bio2, size_t writebuf2)
{
	BIO* pBio1;
	BIO* pBio2;
	int ret;

	ret = BIO_new_bio_pair(&pBio1, writebuf1, &pBio2, writebuf2);
	if ((!pBio1) || (!pBio2))
		return ret;

	*bio1 = CastDynamicPtr(CBIO, find_obj(pBio1));

	if (!(*bio1))
	{
		*bio1 = OK_NEW_OPERATOR CBIO(pBio1);
		(*bio1)->insert_obj();
	}

	*bio2 = CastDynamicPtr(CBIO, find_obj(pBio2));

	if (!(*bio2))
	{
		*bio2= OK_NEW_OPERATOR CBIO(pBio2);
		(*bio2)->insert_obj();
	}
	return ret;
}

int CBIO::get_write_guarantee()
{
	return BIO_get_write_guarantee(CastAnyPtr(BIO, _raw));
}

size_t CBIO::ctrl_get_write_guarantee()
{
	return BIO_ctrl_get_write_guarantee(CastAnyPtr(BIO, _raw));
}

int CBIO::get_read_request()
{
	return BIO_get_read_request(CastAnyPtr(BIO, _raw));
}

size_t CBIO::ctrl_get_read_request()
{
	return BIO_ctrl_get_read_request(CastAnyPtr(BIO, _raw));
}

int CBIO::ctrl_reset_read_request()
{
	return BIO_ctrl_reset_read_request(CastAnyPtr(BIO, _raw));
}

long CBIO::set_accept_port(char *name)
{
	return BIO_set_accept_port(CastAnyPtr(BIO, _raw), name);
}

char *CBIO::get_accept_port()
{
	return BIO_get_accept_port(CastAnyPtr(BIO, _raw));
}

CBIO *CBIO::create_accept(char *host_port)
{
	BIO* pBio = BIO_new_accept(host_port);

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}

	return p;
}

long CBIO::set_nbio_accept(int n)
{
	return BIO_set_nbio_accept(CastAnyPtr(BIO, _raw), n);
}

long CBIO::set_accept_bios(CBIO *bio)
{
	return BIO_set_accept_bios(CastAnyPtr(BIO, _raw), CastAnyPtr(BIO, CastMutable(Pointer, bio->get_bio())));
}

long CBIO::set_bind_mode(long mode)
{
	return BIO_set_bind_mode(CastAnyPtr(BIO, _raw), mode);
}

long CBIO::get_bind_mode(long dummy)
{
	return BIO_get_bind_mode(CastAnyPtr(BIO, _raw), dummy);
}

int CBIO::do_accept()
{
	return BIO_do_accept(CastAnyPtr(BIO, _raw));
}

CBIO *CBIO::create_connect(char *name)
{
	BIO* pBio = BIO_new_connect(name);

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}

	return p;
}

long CBIO::set_conn_hostname(char *name)
{
	return BIO_set_conn_hostname(CastAnyPtr(BIO, _raw), name);
}

long CBIO::set_conn_port(char *port)
{
	return BIO_set_conn_port(CastAnyPtr(BIO, _raw), port);
}

long CBIO::set_conn_ip(char *ip)
{
	return BIO_set_conn_ip(CastAnyPtr(BIO, _raw), ip);
}

long CBIO::set_conn_int_port(int port)
{
	return BIO_set_conn_int_port(CastAnyPtr(BIO, _raw), &port);
}

char *CBIO::get_conn_hostname()
{
	return BIO_get_conn_hostname(CastAnyPtr(BIO, _raw));
}

char *CBIO::get_conn_port()
{
	return BIO_get_conn_port(CastAnyPtr(BIO, _raw));
}

char *CBIO::get_conn_ip()
{
	return BIO_get_conn_ip(CastAnyPtr(BIO, _raw));
}

long CBIO::get_conn_int_port()
{
	return BIO_get_conn_int_port(CastAnyPtr(BIO, _raw));
}

long CBIO::set_nbio(long n)
{
	return BIO_set_nbio(CastAnyPtr(BIO, _raw), n);
}

int CBIO::do_connect()
{
	return BIO_do_connect(CastAnyPtr(BIO, _raw));
}

int CBIO::set_fd(int fd, int c)
{
	return BIO_set_fd(CastAnyPtr(BIO, _raw), fd, c);
}

int CBIO::get_fd(int *c)
{
	return BIO_get_fd(CastAnyPtr(BIO, _raw), c);
}

CBIO *CBIO::create_fd(int fd, int close_flag)
{
	BIO* pBio = BIO_new_fd(fd, close_flag);

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}

	return p;
}

CBIO *CBIO::create_file(const char *filename, const char *mode)
{
	BIO* pBio = BIO_new_file(filename, mode);

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}

	return p;
}

CBIO *CBIO::create_fp(FILE *stream, int flags)
{
	BIO* pBio = BIO_new_fp(stream, flags);

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}

	return p;
}

int CBIO::set_fp(FILE *fp, int flags)
{
	return BIO_set_fp(CastAnyPtr(BIO, _raw), fp, flags);
}

int CBIO::get_fp(FILE **fpp)
{
	return BIO_get_fp(CastAnyPtr(BIO, _raw), fpp);
}

int CBIO::read_filename(char *name)
{
	return BIO_read_filename(CastAnyPtr(BIO, _raw), name);
}

int CBIO::write_filename(char *name)
{
	return BIO_write_filename(CastAnyPtr(BIO, _raw), name);
}

int CBIO::append_filename(char *name)
{
	return BIO_append_filename(CastAnyPtr(BIO, _raw), name);
}

int CBIO::rw_filename(char *name)
{
	return BIO_rw_filename(CastAnyPtr(BIO, _raw), name);
}

int CBIO::set_mem_eof_return(int v)
{
	return BIO_set_mem_eof_return(CastAnyPtr(BIO, _raw), v);
}

long CBIO::get_mem_data(char **pp)
{
	return BIO_get_mem_data(CastAnyPtr(BIO, _raw), pp);
}

int CBIO::set_mem_buf(CBUF_MEM *bm, int c)
{
	return BIO_set_mem_buf(CastAnyPtr(BIO, _raw), bm->get_bufmem(), c);
}

int CBIO::get_mem_ptr(CBUF_MEM **pp)
{
	BUF_MEM* pBufMem;
	int ret;

	ret = BIO_get_mem_ptr(CastAnyPtr(BIO, _raw), &pBufMem);
	if (!pBufMem)
		return ret;
	
	*pp = CastDynamicPtr(CBUF_MEM, find_obj(pBufMem));

	if (!(*pp))
	{
		*pp = OK_NEW_OPERATOR CBUF_MEM(pBufMem);
		(*pp)->insert_obj();
	}
	return ret;
}

CBIO *CBIO::create_mem_buf(void *buf, int len)
{
	BIO* pBio = BIO_new_mem_buf(buf, len);

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}

	return p;
}

CBIO *CBIO::create_socket(int sock, int close_flag)
{
	BIO* pBio = BIO_new_socket(sock, close_flag);

	if (!pBio)
		return nullptr;

	Ptr(CBIO) p = CastDynamicPtr(CBIO, find_obj(pBio));

	if (!p)
	{
		p = OK_NEW_OPERATOR CBIO(pBio);
		p->insert_obj();
	}

	return p;
}