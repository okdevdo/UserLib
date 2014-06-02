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

class OPENSSL_API CSSL_METHOD : public COpenSSLClass
{
public:
	CSSL_METHOD(ConstPointer sslMeth = NULL);
	virtual ~CSSL_METHOD();

	__inline ConstPointer get_sslMethod() const { return _raw; }

	static ConstPtr(CSSL_METHOD) SSLv2_method(void);		/* SSLv2 */
	static ConstPtr(CSSL_METHOD) SSLv2_server_method(void);	/* SSLv2 */
	static ConstPtr(CSSL_METHOD) SSLv2_client_method(void);	/* SSLv2 */

	static ConstPtr(CSSL_METHOD) SSLv3_method(void);		/* SSLv3 */
	static ConstPtr(CSSL_METHOD) SSLv3_server_method(void);	/* SSLv3 */
	static ConstPtr(CSSL_METHOD) SSLv3_client_method(void);	/* SSLv3 */

	static ConstPtr(CSSL_METHOD) SSLv23_method(void);	/* SSLv3 but can rollback to v2 */
	static ConstPtr(CSSL_METHOD) SSLv23_server_method(void);	/* SSLv3 but can rollback to v2 */
	static ConstPtr(CSSL_METHOD) SSLv23_client_method(void);	/* SSLv3 but can rollback to v2 */

	static ConstPtr(CSSL_METHOD) TLSv1_method(void);		/* TLSv1.0 */
	static ConstPtr(CSSL_METHOD) TLSv1_server_method(void);	/* TLSv1.0 */
	static ConstPtr(CSSL_METHOD) TLSv1_client_method(void);	/* TLSv1.0 */

	static ConstPtr(CSSL_METHOD) TLSv1_1_method(void);		/* TLSv1.1 */
	static ConstPtr(CSSL_METHOD) TLSv1_1_server_method(void);	/* TLSv1.1 */
	static ConstPtr(CSSL_METHOD) TLSv1_1_client_method(void);	/* TLSv1.1 */

	static ConstPtr(CSSL_METHOD) TLSv1_2_method(void);		/* TLSv1.2 */
	static ConstPtr(CSSL_METHOD) TLSv1_2_server_method(void);	/* TLSv1.2 */
	static ConstPtr(CSSL_METHOD) TLSv1_2_client_method(void);	/* TLSv1.2 */

	static ConstPtr(CSSL_METHOD) DTLSv1_method(void);		/* DTLSv1.0 */
	static ConstPtr(CSSL_METHOD) DTLSv1_server_method(void);	/* DTLSv1.0 */
	static ConstPtr(CSSL_METHOD) DTLSv1_client_method(void);	/* DTLSv1.0 */
};

class OPENSSL_API CSSL_CIPHER : public COpenSSLClass
{
public:
	CSSL_CIPHER(ConstPointer sslCipher = NULL);
	virtual ~CSSL_CIPHER();

	__inline ConstPointer get_sslcipher() const { return _raw; }

	int	get_bits(int *alg_bits);
	char *get_version();
	const char *get_name();
	unsigned long get_id();
	char *description(char *buf, int size);
};

class CSSL_SESSION;
class CSSL;
class CX509;
class CX509_NAME;
class CX509_STORE;
class CX509_STORE_CTX;
class CX509_VERIFY_PARAM;
class CEVP_PKEY;
class CRSA_KEY;
class CDH_KEY;
class CEC_KEY;
class OPENSSL_API CSSL_CTX : public COpenSSLClass
{
public:
	typedef void(*TCallbackMsg)(int write_p, int version, int content_type, const void *buf, size_t len, CSSL *ssl, void *arg);
	typedef int(*TCallbackNewSession)(CSSL *ssl, CSSL_SESSION *sess);
	typedef void(*TCallbackRemoveSession)(CSSL_CTX *ctx, CSSL_SESSION *sess);
	typedef CSSL_SESSION *(*TCallbackGetSession)(CSSL *ssl, unsigned char *data, int len, int *copy);
	typedef void(*TCallbackInfo)(CSSL *ssl, int type, int val);
	typedef int(*TCallbackClientCert)(CSSL *ssl, CX509 **x509, CEVP_PKEY **pkey);
	typedef int(*TCallbackGenerateCookie)(CSSL *ssl, unsigned char *cookie, unsigned int *cookie_len);
	typedef int(*TCallbackVerifyCookie)(CSSL *ssl, unsigned char *cookie, unsigned int cookie_len);
	typedef int(*TCallbackNextProtosAdvertised)(CSSL *ssl, const unsigned char **out, unsigned int *outlen, void *arg);
	typedef int(*TCallbackNextProtoSelect)(CSSL *ssl, unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void *arg);
	typedef unsigned int(*TCallbackPSKClient)(CSSL *ssl, const char *hint, char *identity, unsigned int max_identity_len, unsigned char *psk, unsigned int max_psk_len);
	typedef unsigned int(*TCallbackPSKServer)(CSSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len);
	typedef int(*TCallbackGenerateSessionId)(const CSSL *ssl, unsigned char *id, unsigned int *id_len);
	typedef int(*TCallbackCertVerify)(CX509_STORE_CTX *ctx, void *arg);
	typedef int (*TCallbackDefaultPasswd)(char *buf, int size, int rwflag, void *userdata);
	typedef char *(*TCallbackSRPClientPwd)(CSSL *, void *);
	typedef int(*TCallbackSRPVerifyParam)(CSSL *, void *);
	typedef int(*TCallbackSRPUsername)(CSSL *, int *, void *);
	typedef CRSA_KEY *(*TCallbackTmpRSA)(CSSL *ssl, int is_export, int keylength);
	typedef CDH_KEY *(*TCallbackTmpDH)(CSSL *ssl, int is_export, int keylength);
	typedef CEC_KEY *(*TCallbackTmpECDH)(CSSL *ssl, int is_export, int keylength);

	CSSL_CTX(ConstPointer sslCtx = NULL);
	virtual ~CSSL_CTX();

	__inline ConstPointer get_sslContext() const { return _raw; }

	static Ptr(CSSL_CTX) create(ConstPtr(CSSL_METHOD) meth);

	void free();

	void set_options(unsigned long op);
	void clear_options(unsigned long op);
	unsigned long get_options();

	enum TMode
	{
		/* Allow SSL_write(..., n) to return r with 0 < r < n (i.e. report success
		* when just a single record has been written): */
		eModeEnablePartialWrite = 0x00000001L,
		/* Make it possible to retry SSL_write() with changed buffer location
		* (buffer contents must stay the same!); this is not the default to avoid
		* the misconception that non-blocking SSL_write() behaves like
		* non-blocking write(): */
		eModeAcceptMovingWriteBuffer = 0x00000002L,
		/* Never bother the application with retries if the transport
		* is blocking: */
		eModeAutoRetry = 0x00000004L,
		/* Don't attempt to automatically build certificate chain */
		eModeNoAutoChain = 0x00000008L,
		/* Save RAM by releasing read and write buffers when they're empty. (SSL3 and
		* TLS only.)  "Released" buffers are put onto a free-list in the context
		* or just freed (depending on the context's setting for freelist_max_len). */
		eModeReleaseBuffers = 0x00000010L,
		/* Send the current time in the Random fields of the ClientHello and
		* ServerHello records for compatibility with hypothetical implementations
		* that require it.
		*/
		eModeSendClientHelloTime = 0x00000020L,
		eModeSendServerHelloTime = 0x00000040L
	};

	void set_mode(unsigned long op);
	void clear_mode(unsigned long op);
	unsigned long get_mode();

	void set_msg_callback(TCallbackMsg cb);
	void set_msg_callback_arg(void *arg);

	CHashList<CSSL_SESSION> sessions();
	int sess_number();
	int sess_connect();
	int sess_connect_good();
	int sess_connect_renegotiate();
	int sess_accept();
	int sess_accept_renegotiate();
	int sess_accept_good();
	int sess_hits();
	int sess_cb_hits();
	int sess_misses();
	int sess_timeouts();
	int sess_cache_full();

	void sess_set_new_cb(TCallbackNewSession cb);
	TCallbackNewSession sess_get_new_cb();
	void sess_set_remove_cb(TCallbackRemoveSession cb);
	TCallbackRemoveSession sess_get_remove_cb();
	void sess_set_get_cb(TCallbackGetSession cb);
	TCallbackGetSession sess_get_get_cb();
	void set_info_callback(TCallbackInfo cb);
	TCallbackInfo get_info_callback();
	void set_client_cert_cb(TCallbackClientCert cb);
	TCallbackClientCert get_client_cert_cb();

	int set_client_cert_engine(CENGINE *e);
	void set_cookie_generate_cb(TCallbackGenerateCookie cb);
	void set_cookie_verify_cb(TCallbackVerifyCookie cb);

	void set_next_protos_advertised_cb(TCallbackNextProtosAdvertised cb, void *arg);
	void set_next_proto_select_cb(TCallbackNextProtoSelect cb, void *arg);

	void set_psk_client_callback(TCallbackPSKClient cb);
	void set_psk_server_callback(TCallbackPSKServer cb);
	int use_psk_identity_hint(const char *identity_hint);

	void *get_app_data();
	void set_app_data(void *arg);

	long need_tmp_RSA();
	long set_tmp_rsa(CRSA_KEY *rsa);
	long set_tmp_dh(CDH_KEY *dh);
	long set_tmp_ecdh(CEC_KEY* ecdh);

	long add_extra_chain_cert(CX509 *x509);
	long get_extra_chain_certs(CX509 **px509);
	long clear_extra_chain_certs();

	int set_cipher_list(const char *str);
	void set_timeout(long t);
	long get_timeout();
	void set_cert_store(CX509_STORE *cs);
	CX509_STORE *get_cert_store();
	void flush_sessions(long t);

	enum TFileType
	{
		eFileTypePEM = 1,
		eFileTypeASN1 = 2
	};

	int use_RSAPrivateKey_file(char *file, TFileType type);
	int use_PrivateKey_file(char *file, TFileType type);
	int use_certificate_file(char *file, TFileType type);
	int	use_certificate_chain_file(const char *file); /* PEM type */

	int add_session(CSSL_SESSION *c);
	int remove_session(CSSL_SESSION *c);
	void set_generate_session_id(TCallbackGenerateSessionId cb);

	int get_verify_depth();
	void set_verify_depth(int depth);
	void set_cert_verify_callback(TCallbackCertVerify cb, void *arg);

	int use_RSAPrivateKey(CRSA_KEY *rsa);
	int use_RSAPrivateKey_ASN1(unsigned char *d, long len);
	int use_PrivateKey(CEVP_PKEY *pkey);
	int use_PrivateKey_ASN1(int pk, unsigned char *d, long len);
	int use_certificate(CX509 *x);
	int use_certificate_ASN1(int len, unsigned char *d);
	void set_default_passwd_cb(TCallbackDefaultPasswd cb);
	void set_default_passwd_cb_userdata(void *u);
	int check_private_key();
	int	set_session_id_context(const unsigned char *sid_ctx, unsigned int sid_ctx_len);
	int set1_param(CX509_VERIFY_PARAM *vpm);

	int set_srp_username(char *name);
	int set_srp_password(char *password);
	int set_srp_strength(int strength);
	void set_srp_client_pwd_callback(TCallbackSRPClientPwd cb);
	void set_srp_verify_param_callback(TCallbackSRPVerifyParam cb);
	void set_srp_username_callback(TCallbackSRPUsername cb);
	int set_srp_cb_arg(void *arg);

	int set_ssl_version(const CSSL_METHOD *meth);

	void set_client_CA_list(CStack<CX509_NAME>& list);
	void get_client_CA_list(CStack<CX509_NAME>& list);
	int add_client_CA(CX509 *x);

	int get_quiet_shutdown();
	void set_quiet_shutdown(int mode);

	int set_default_verify_paths();
	int load_verify_locations(char *CAfile, char *CApath);

	void *get_ex_data(int idx);
	int set_ex_data(int idx, char *arg);
	static int get_ex_new_index(long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func);

	int sess_get_cache_size();
	void sess_set_cache_size(long t);
	int get_session_cache_mode();
	void set_session_cache_mode(int mode);

	int get_read_ahead();
	void set_read_ahead(int m);
	int get_max_cert_list();
	void set_max_cert_list(int m);
	void set_max_send_fragment(int m);

	void set_tmp_rsa_callback(TCallbackTmpRSA cb);
	void set_tmp_dh_callback(TCallbackTmpDH cb);
	void set_tmp_ecdh_callback(TCallbackTmpECDH cb);
};

class CBIO;
class OPENSSL_API CSSL_SESSION : public COpenSSLClass
{
public:
	typedef int (*TCallbackPEMPassword)(char *buf, int size, int rwflag, void *userdata);

	CSSL_SESSION(ConstPointer sslSession = NULL);
	virtual ~CSSL_SESSION();

	__inline ConstPointer get_sslsession() const { return _raw; }

	static Ptr(CSSL_SESSION) create();

	void free();

	long get_time();
	long set_time(long t);
	long get_timeout();
	long set_timeout(long t);

	CX509 *get0_peer();
	int set1_id_context(const unsigned char *sid_ctx, unsigned int sid_ctx_len);

	const unsigned char *get_id(unsigned int *len);
	unsigned int get_compress_id();

	int	print_fp(FILE *fp);
	int	print(CBIO *fp);

	int	i2d(unsigned char **pp);
	static CSSL_SESSION *d2i(CSSL_SESSION **a, const unsigned char **pp, long length);
	static CSSL_SESSION *d2i_bio(CBIO* bp, CSSL_SESSION **s_id);
	int i2d_bio(CBIO* bp);
	static CSSL_SESSION *PEM_read_bio(CBIO *bp, CSSL_SESSION **x, TCallbackPEMPassword cb, void *u);
	static CSSL_SESSION *PEM_read(FILE *fp, CSSL_SESSION **x, TCallbackPEMPassword cb, void *u);
	int PEM_write_bio(CBIO *bp);
	int PEM_write(FILE *fp);

	int set_ex_data(int idx, void *data);
	void *get_ex_data(int idx);
    static int get_ex_new_index(long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func);
};

class CEVP_PKEY;
class OPENSSL_API CSSL : public COpenSSLClass
{
public:
	typedef void(*TCallbackMsg)(int write_p, int version, int content_type, const void *buf, size_t len, CSSL *ssl, void *arg);
	typedef unsigned int(*TCallbackPSKClient)(CSSL *ssl, const char *hint, char *identity, unsigned int max_identity_len, unsigned char *psk, unsigned int max_psk_len);
	typedef unsigned int(*TCallbackPSKServer)(CSSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len);
	typedef int(*TCallbackVerify)(int ok, CX509_STORE_CTX *ctx);
	typedef int(*TCallbackGenSession)(CSSL* ssl, unsigned char *id, unsigned int *id_len);
	typedef void(*TCallbackInfo)(CSSL *ssl, int type, int val);
	typedef CRSA_KEY *(*TCallbackTmpRSA)(CSSL* ssl, int is_export, int keylength);
	typedef CDH_KEY *(*TCallbackTmpDH)(CSSL* ssl, int is_export, int keylength);
	typedef CEC_KEY *(*TCallbackTmpECDH)(CSSL* ssl, int is_export, int keylength);
	typedef int(*TCallbackTLSSessionTicketExt)(CSSL* ssl, const unsigned char *data, int len, void *arg);
	typedef int(*TCallbackTLSSessionSecret)(CSSL* ssl, void *secret, int *secret_len, ConstRef(CStack<CSSL_CIPHER>) peer_ciphers, CSSL_CIPHER **cipher, void *arg);

	CSSL(ConstPointer ssl = NULL);
	virtual ~CSSL();

	__inline ConstPointer get_ssl() const { return _raw; }

	static Ptr(CSSL) create(Ptr(CSSL_CTX) pCtx);

	Ptr(CSSL) dup();
	int	clear();
	void free();

	long set_options(long op);
	long clear_options(long op);
	long get_options();

	enum TMode
	{
		/* Allow SSL_write(..., n) to return r with 0 < r < n (i.e. report success
		* when just a single record has been written): */
		eModeEnablePartialWrite = 0x00000001L,
		/* Make it possible to retry SSL_write() with changed buffer location
		* (buffer contents must stay the same!); this is not the default to avoid
		* the misconception that non-blocking SSL_write() behaves like
		* non-blocking write(): */
		eModeAcceptMovingWriteBuffer = 0x00000002L,
		/* Never bother the application with retries if the transport
		* is blocking: */
		eModeAutoRetry = 0x00000004L,
		/* Don't attempt to automatically build certificate chain */
		eModeNoAutoChain = 0x00000008L,
		/* Save RAM by releasing read and write buffers when they're empty. (SSL3 and
		* TLS only.)  "Released" buffers are put onto a free-list in the context
		* or just freed (depending on the context's setting for freelist_max_len). */
		eModeReleaseBuffers = 0x00000010L,
		/* Send the current time in the Random fields of the ClientHello and
		* ServerHello records for compatibility with hypothetical implementations
		* that require it.
		*/
		eModeSendClientHelloTime = 0x00000020L,
		eModeSendServerHelloTime = 0x00000040L
	};

	long clear_mode(long op);
	long set_mode(long op);
	long get_mode();

	int set_mtu(long mtu);
	int get_secure_renegotiation_support();
	int heartbeat();

	void set_msg_callback(TCallbackMsg cb);
	void set_msg_callback_arg(void *arg);

	int SRP_CTX_init();
	int SRP_CTX_free();
	int SRP_server_param_with_username(int *ad);
	int SRP_gen_server_master_secret(unsigned char *master_key);
	int SRP_calc_param();
	int SRP_gen_client_master_secret(unsigned char *master_key);

	void set_psk_client_callback(TCallbackPSKClient cb);
	void set_psk_server_callback(TCallbackPSKServer cb);
	int use_psk_identity_hint(const char *identity_hint);
	const char *get_psk_identity_hint();
	const char *get_psk_identity();

	void set_app_data(void *arg);
	void *get_app_data();

	int get_state();
	int is_init_finished();
	int in_init();
	int in_before();
	int in_connect_init();
	int in_accept_init();

	size_t get_finished(void *buf, size_t count);
	size_t get_peer_finished(void *buf, size_t count);

	int DTLS1_get_timeout(struct timeval* arg);
	int DTLS1_handle_timeout();
	int DTLS1_listen(struct sockaddr *peer);

	int session_reused();
	int num_renegotiations();
	int clear_num_renegotiations();
	int total_renegotiations();

	int need_tmp_RSA();
	int set_tmp_rsa(CRSA_KEY* rsa);
	int set_tmp_dh(CDH_KEY* dh);
	int set_tmp_ecdh(CEC_KEY *ecdh);

	int want();
	ConstPtr(CSSL_CIPHER) get_current_cipher();
	int	get_fd();
	int	get_rfd();
	int	get_wfd();
	const char *get_cipher_list(int n);
	char *get_shared_ciphers(char *buf, int len);
	int	get_read_ahead();
	int	pending();
	int	set_fd(int fd);
	int	set_rfd(int fd);
	int	set_wfd(int fd);
	void set_bio(CBIO *rbio, CBIO *wbio);
	CBIO * get_rbio();
	CBIO * get_wbio();
	int	set_cipher_list(const char *str);
	void set_read_ahead(int yes);

	int	get_verify_mode();
	int	get_verify_depth();
	TCallbackVerify get_verify_callback();
	void set_verify(int mode, TCallbackVerify cb);
	void set_verify_depth(int depth);

	int	use_RSAPrivateKey(CRSA_KEY *rsa);
	int	use_RSAPrivateKey_ASN1(unsigned char *d, long len);
	int	use_PrivateKey(CEVP_PKEY *pkey);
	int	use_PrivateKey_ASN1(int pk, const unsigned char *d, long len);
	int	use_certificate(CX509 *x);
	int	use_certificate_ASN1(const unsigned char *d, int len);

	enum TFileType
	{
		eFileTypePEM = 1,
		eFileTypeASN1 = 2
	};

	int	use_RSAPrivateKey_file(const char *file, TFileType type);
	int	use_PrivateKey_file(const char *file, TFileType type);
	int	use_certificate_file(const char *file, TFileType type);

	const char *state_string();
	const char *rstate_string();
	const char *state_string_long();
	const char *rstate_string_long();

	void copy_session_id(CSSL *to);
	int	set_session(CSSL_SESSION *session);
	void set_generate_session_id(TCallbackGenSession cb);
	int has_matching_session_id(const unsigned char *id, unsigned int id_len);

	CX509 *get_peer_certificate();
	void get_peer_cert_chain(Ref(CStack<CX509>) ret);
	int check_private_key();

	int	set_session_id_context(const unsigned char *sid_ctx, unsigned int sid_ctx_len);
	int set_purpose(int purpose);
	int set_trust(int trust);
	int set1_param(CX509_VERIFY_PARAM *vpm);

	int accept();
	int connect();
	int read(void *buf, int num);
	int peek(void *buf, int num);
	int write(const void *buf, int num);

	int	get_error( int ret_code);
	const char *get_version();

	void get_ciphers(Ref(CStack<CSSL_CIPHER>));

	int do_handshake();
	int renegotiate();
	int renegotiate_abbreviated();
	int renegotiate_pending();
	int shutdown();

	ConstPtr(CSSL_METHOD) get_method();
	int set_method(ConstPtr(CSSL_METHOD) method);

	void set_client_CA_list(ConstRef(CStack<CX509_NAME>) name_list);
	void get_client_CA_list(Ref(CStack<CX509_NAME>));
	int add_client_CA(CX509 *x);

	void set_connect_state();
	void set_accept_state();

	long get_default_timeout();
	void set_quiet_shutdown(int mode);
	int get_quiet_shutdown();
	void set_shutdown(int mode);
	int get_shutdown();
	int version();

	CSSL_SESSION *get0_session(); /* just peek at pointer */
	CSSL_SESSION *get1_session(); /* obtain a reference count */
	CSSL_CTX *get_CTX();
	CSSL_CTX *set_CTX(CSSL_CTX* ctx);

	void set_info_callback(TCallbackInfo cb);
	TCallbackInfo get_info_callback();

	int state();
	void set_state(int state);

	void set_verify_result(long v);
	long get_verify_result();

	int set_ex_data(int idx, void *data);
	void *get_ex_data(int idx);
	static int get_ex_new_index(long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func);
	static int get_ex_data_X509_STORE_CTX_idx(void);

	int get_max_cert_list();
	void set_max_cert_list(int m);
	void set_max_send_fragment(int m);

	void set_tmp_rsa_callback(TCallbackTmpRSA cb);
	void set_tmp_dh_callback(TCallbackTmpDH cb);
	void set_tmp_ecdh_callback(TCallbackTmpECDH cb);

	int set_session_ticket_ext(void *ext_data, int ext_len);
	void set_session_ticket_ext_cb(TCallbackTLSSessionTicketExt cb, void *arg);
	void set_session_secret_cb(TCallbackTLSSessionSecret cb, void *arg);

	void set_debug(int debug);
	int cache_hit();
};

