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

class CBIO_METHOD;
class CEVP_CIPHER;
class CEVP_CIPHER_CTX;
class CEVP_MD;
class CEVP_MD_CTX;
class CSSL;
class CSSL_CTX;
class OPENSSL_API CBIO : public COpenSSLClass
{
public:
	enum
	{
		BIO_TYPE_NONE    =      0,
		BIO_TYPE_MEM     =      (1|0x0400),
		BIO_TYPE_FILE    =      (2|0x0400),

		BIO_TYPE_FD      =		(4|0x0400|0x0100),
		BIO_TYPE_SOCKET  =      (5|0x0400|0x0100),
		BIO_TYPE_NULL    =		(6|0x0400),
		BIO_TYPE_SSL     =		(7|0x0200),
		BIO_TYPE_MD      =		(8|0x0200),
		BIO_TYPE_BUFFER  =      (9|0x0200),
		BIO_TYPE_CIPHER  =      (10|0x0200),
		BIO_TYPE_BASE64  =      (11|0x0200),
		BIO_TYPE_CONNECT =      (12|0x0400|0x0100),
		BIO_TYPE_ACCEPT  =      (13|0x0400|0x0100),
		BIO_TYPE_PROXY_CLIENT = (14|0x0200),
		BIO_TYPE_PROXY_SERVER = (15|0x0200),
		BIO_TYPE_NBIO_TEST    = (16|0x0200),
		BIO_TYPE_NULL_FILTER  = (17|0x0200),
		BIO_TYPE_BER          = (18|0x0200),
		BIO_TYPE_BIO          = (19|0x0400),
		BIO_TYPE_LINEBUFFER	  = (20|0x0200),
		BIO_TYPE_DGRAM		  = (21|0x0400|0x0100),
		BIO_TYPE_ASN1 		  = (22|0x0200),
		BIO_TYPE_COMP 		  = (23|0x0200),
		BIO_TYPE_DGRAM_SCTP	  = (24|0x0400|0x0100),

		BIO_TYPE_DESCRIPTOR   = 0x0100,
		BIO_TYPE_FILTER       = 0x0200,
		BIO_TYPE_SOURCE_SINK  = 0x0400
	};

	enum 
	{
		BIO_FLAGS_READ =        0x01,
		BIO_FLAGS_WRITE =       0x02,
		BIO_FLAGS_IO_SPECIAL =   0x04,
		BIO_FLAGS_RWS = (BIO_FLAGS_READ|BIO_FLAGS_WRITE|BIO_FLAGS_IO_SPECIAL),
		BIO_FLAGS_SHOULD_RETRY = 0x08
	};

	typedef long(*TCallback)(CBIO *b, int oper, const char *argp,
		int argi, long argl, long retvalue);

	CBIO(ConstPointer bio = NULL);
	virtual ~CBIO();

	static Ptr(CBIO) create(Ptr(CBIO_METHOD) type);

	int set_method(Ptr(CBIO_METHOD) type);
	void free();

	CBIO *  find_type(int bio_type);
	CBIO *  next();
	int method_type();

	__inline ConstPointer get_bio() const { return _raw; }

	int reset();
	int seek(int ofs);
	int tell();
	int flush();
	int eof();

	int set_close(long flag);
	int get_close();

	int pending();
	size_t ctrl_pending();
	int wpending();
	size_t ctrl_wpending();

	int read(void *buf, int len);
	int gets(char *buf, int size);
	int write(const void *buf, int len);
	int puts(const char *buf);
	int printf(const char *format, ...);
	int vprintf(const char *format, va_list args);

	CBIO *  push(CBIO *append);
	CBIO *  pop();

	void set_callback(TCallback cb);
	TCallback get_callback();
	void set_callback_arg(void *arg);
	void* get_callback_arg();

	static long debug_callback(CBIO *bio, int cmd, const char *argp, int argi,
		long argl, long ret);

	int should_read();
	int should_write();
	int should_io_special();
	int retry_type();
	int should_retry();

	CBIO* get_retry_BIO(int *reason);
	int get_retry_reason();

	// buffer method
	int get_buffer_num_lines();
	int set_read_buffer_size(int size);
	int set_write_buffer_size(int size);
	int set_buffer_size(int size);
	int set_buffer_read_data(const char *buf, int num);

	// cipher method
	void set_cipher(const CEVP_CIPHER *cipher,
		unsigned char *key, unsigned char *iv, int enc);
	int get_cipher_status();
	int get_cipher_ctx(CEVP_CIPHER_CTX **pctx);

	// md method
	int set_md(CEVP_MD *md);
	int get_md(CEVP_MD **mdp);
	int get_md_ctx(CEVP_MD_CTX **mdcp);

	// ssl method
	int set_ssl(CSSL* ssl, int c);
	int get_ssl(CSSL**sslp);
	int set_ssl_mode(int client);
	int set_ssl_renegotiate_bytes(int num);
	int set_ssl_renegotiate_timeout(int seconds);
	int get_num_renegotiates();

	static Ptr(CBIO) create_ssl(Ptr(CSSL_CTX) ctx, int client);
	static Ptr(CBIO) create_ssl_connect(Ptr(CSSL_CTX) ctx);
	static Ptr(CBIO) create_buffer_ssl_connect(Ptr(CSSL_CTX) ctx);

	static int ssl_copy_session_id(CBIO *to, CBIO *from);

	void ssl_shutdown();
	int do_handshake();

	// bio pair method
	int make_bio_pair(CBIO* b2);
	int destroy_bio_pair();

	int shutdown_wr();

	int set_write_buf_size(int size);
	size_t get_write_buf_size(int size);

	static int create_bio_pair(CBIO **bio1, size_t writebuf1, CBIO **bio2, size_t writebuf2);

	int get_write_guarantee();
	size_t ctrl_get_write_guarantee();

	int get_read_request();
	size_t ctrl_get_read_request();

	int ctrl_reset_read_request();

	// accept method
	enum
	{
		BIO_BIND_NORMAL,
		BIO_BIND_REUSEADDR_IF_UNUSED,
		BIO_BIND_REUSEADDR
	};

	long set_accept_port(char *name);
	char *get_accept_port();

	static CBIO *create_accept(char *host_port);

	long set_nbio_accept(int n);
	long set_accept_bios(CBIO *bio);

	long set_bind_mode(long mode);
	long get_bind_mode(long dummy);

	int do_accept();

	// connect method
	static CBIO *create_connect(char *name);

	long set_conn_hostname(char *name);
	long set_conn_port(char *port);
	long set_conn_ip(char *ip);
	long set_conn_int_port(int port);
	char *get_conn_hostname();
	char *get_conn_port();
	char *get_conn_ip();
	long get_conn_int_port();

	long set_nbio(long n);

	int do_connect();

	// fd method
	int set_fd(int fd, int c);
	int get_fd(int *c = NULL);

	static CBIO *create_fd(int fd, int close_flag);

	// file method
	enum
	{
		BIO_NOCLOSE,
		BIO_CLOSE,
		BIO_FP_TEXT = 0x10
	};

	static CBIO *create_file(const char *filename, const char *mode);
	static CBIO *create_fp(FILE *stream, int flags);

	int set_fp(FILE *fp, int flags);
	int get_fp(FILE **fpp);

	int read_filename(char *name);
	int write_filename(char *name);
	int append_filename(char *name);
	int rw_filename(char *name);

	// mem method
	int set_mem_eof_return(int v);
	long get_mem_data(char **pp);
	int set_mem_buf(CBUF_MEM *bm, int c);
	int get_mem_ptr(CBUF_MEM **pp);

	static CBIO *create_mem_buf(void *buf, int len);

	// socket method

	//long set_fd(int fd, long close_flag);
	//long get_fd(int *c);

	static CBIO *create_socket(int sock, int close_flag);
};

