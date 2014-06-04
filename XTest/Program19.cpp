/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#include "Program.h"
#include "OpenSSLLib.h"
#include "BIO.h"
#include "BIO_METHOD.h"
#include "EVP_CIPHER.h"
#include "SSL.h"
#if OK_COMP_GNUC
#if OK_SYS_WINDOWS32
#include <openssl/applink.c>
#endif
#endif

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

static void TestFileFunc()
{
	Ptr(CBIO_METHOD) pFileM = CBIO_METHOD::create_s_file();
	Ptr(CBIO) pTmp = CBIO::create(pFileM);

	Ptr(CBIO) pFileBio = CBIO::create_file("Testssl.dat", "w");

	pFileBio->write("Test", 4);
	pFileBio->printf("%d", 1000);
	pFileBio->flush();
}

static void TestCipherEnCryptMethod()
{
	Ptr(CBIO_METHOD) pBase64M = CBIO_METHOD::create_f_base64();
	Ptr(CBIO) pBase64Bio = CBIO::create(pBase64M);
	Ptr(CBIO_METHOD) pCipherM = CBIO_METHOD::create_f_cipher();
	Ptr(CBIO) pCipherBio = CBIO::create(pCipherM);
	Ptr(CEVP_CIPHER) pCipher = CEVP_CIPHER::create_aes_256_ccm();
	unsigned char key[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
	unsigned char iv[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
	char buf[9];

	pCipherBio->set_cipher(pCipher, key, iv, 1);

	Ptr(CBIO) pFileBio = CBIO::create_file("Testssl1.dat", "w");

	pBase64Bio->push(pFileBio);
	pCipherBio->push(pBase64Bio);

	snprintf(buf, 9, "Test%d", 1000);
	pCipherBio->write(buf, 8);
	pCipherBio->flush();
}

static void TestCipherDeCryptMethod()
{
	Ptr(CBIO_METHOD) pBase64M = CBIO_METHOD::create_f_base64();
	Ptr(CBIO) pBase64Bio = CBIO::create(pBase64M);
	Ptr(CBIO_METHOD) pCipherM = CBIO_METHOD::create_f_cipher();
	Ptr(CBIO) pCipherBio = CBIO::create(pCipherM);
	Ptr(CEVP_CIPHER) pCipher = CEVP_CIPHER::create_aes_256_ccm();
	unsigned char key[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
	unsigned char iv[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
	char buf[9] = { 0 };

	pCipherBio->set_cipher(pCipher, key, iv, 1);

	Ptr(CBIO_METHOD) pFileM = CBIO_METHOD::create_s_file();
	Ptr(CBIO) pFileBio = CBIO::create(pFileM);

	pFileBio->read_filename("Testssl1.dat");

	pBase64Bio->push(pFileBio);
	pCipherBio->push(pBase64Bio);

	pCipherBio->read(buf, 8);
	pCipherBio->flush();

	COUT << buf << endl;
}

static void TestClient()
{
	CBIO *sbio, *out;
	int len;
	char tmpbuf[1024];
	CSSL_CTX *ctx;
	CSSL *ssl;

	out = CBIO::create_fp(stdout, CBIO::BIO_NOCLOSE);

	/* We would seed the PRNG here if the platform didn't
	* do it automatically
	*/

	ctx = CSSL_CTX::create(CSSL_METHOD::SSLv23_client_method());

	/* We'd normally set some stuff like the verify paths and
	* mode here because as things stand this will connect to
	* any server whose certificate is signed by any CA.
	*/

	sbio = CBIO::create_ssl_connect(ctx);

	sbio->get_ssl(&ssl);

	if (!ssl) {
		fprintf(stderr, "Can't locate SSL pointer\n");
		/* whatever ... */
	}

	/* Don't want any retries */
	ssl->set_mode(CSSL::eModeAutoRetry);

	/* We might want to do other things with ssl here */
	sbio->set_conn_hostname("localhost:4433");

	if (sbio->do_connect() <= 0) {
		fprintf(stderr, "Error connecting to server\n");
		COpenSSLLib::PrintErrorsFp(stderr);
		/* whatever ... */
	}

	if (sbio->do_handshake() <= 0) {
		fprintf(stderr, "Error establishing SSL connection\n");
		COpenSSLLib::PrintErrorsFp(stderr);
		/* whatever ... */
	}

	/* Could examine ssl here to get connection info */
	sbio->set_callback(CBIO::debug_callback);
	sbio->set_callback_arg(CastMutable(Pointer, out->get_bio()));

	sbio->puts("GET / HTTP/1.0\n\n");
	for (;;) {
		len = sbio->read(tmpbuf, 1024);
		if (len <= 0) break;
		out->write(tmpbuf, len);
	}
	sbio->set_callback(nullptr);
	sbio->set_callback_arg(nullptr);
	sbio->free();
	out->free();
}

void TestSSLFunc()
{
	COpenSSLLib::Initialize();

	//TestFileFunc();
	//TestCipherEnCryptMethod();
	//TestCipherDeCryptMethod();
	TestClient();

	COpenSSLLib::CleanUp();
}

static int TestServerCertVerify(CX509_STORE_CTX *ctx, void *arg)
{
	return 1;
}

CRSA_KEY *TestServerTmpRSA(CSSL *ssl, int is_export, int keylength)
{
	return nullptr;
}

static int TestServer()
{
	CBIO *sbio, *bbio, *acpt, *out;
	int len;
	char tmpbuf[1024];
	CSSL_CTX *ctx;
	CSSL *ssl;

	out = CBIO::create_fp(stdout, CBIO::BIO_NOCLOSE);

	/* Might seed PRNG here */

	ctx = CSSL_CTX::create(CSSL_METHOD::SSLv23_server_method());

	if (!ctx->use_certificate_file("server.pem", CSSL_CTX::eFileTypePEM)
		|| !ctx->use_PrivateKey_file("server.pem", CSSL_CTX::eFileTypePEM)
		|| !ctx->check_private_key()) {

		fprintf(stderr, "Error setting up SSL_CTX\n");
		COpenSSLLib::PrintErrorsFp(stderr);
		return 0;
	}

	/* Might do other things here like setting verify locations and
	* DH and/or RSA temporary key callbacks
	*/
	ctx->set_cert_verify_callback(TestServerCertVerify, nullptr);
	ctx->set_tmp_rsa_callback(TestServerTmpRSA);

	/* New SSL BIO setup as server */
	sbio = CBIO::create_ssl(ctx, 0);

	sbio->get_ssl(&ssl);

	if (!ssl) {
		fprintf(stderr, "Can't locate SSL pointer\n");
		/* whatever ... */
	}

	/* Don't want any retries */
	ssl->set_mode(CSSL::eModeAutoRetry);

	/* Create the buffering BIO */

	bbio = CBIO::create(CBIO_METHOD::create_f_buffer());

	/* Add to chain */
	sbio = bbio->push(sbio);

	acpt = CBIO::create_accept("::4433");

	/* By doing this when a OK_NEW_OPERATOR connection is established
	* we automatically have sbio inserted into it. The
	* BIO chain is now 'swallowed' by the accept BIO and
	* will be freed when the accept BIO is freed.
	*/

	acpt->set_accept_bios(sbio);

	/* Setup accept BIO */
	if (acpt->do_accept() <= 0) {
		fprintf(stderr, "Error setting up accept BIO\n");
		COpenSSLLib::PrintErrorsFp(stderr);
		return 0;
	}

	for (;;)
	{
		/* Now wait for incoming connection */
		if (acpt->do_accept() <= 0) {
			fprintf(stderr, "Error in connection\n");
			COpenSSLLib::PrintErrorsFp(stderr);
			return 0;
		}

		sbio = acpt->pop();

		if (sbio->do_handshake() <= 0) {
			fprintf(stderr, "Error in SSL handshake\n");
			COpenSSLLib::PrintErrorsFp(stderr);
			return 0;
		}

		sbio->puts("HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\n");
		sbio->puts("\r\nConnection Established\r\nRequest headers:\r\n");
		sbio->puts("--------------------------------------------------\r\n");

		for (;;) {
			len = sbio->gets(tmpbuf, 1024);
			if (len <= 0) break;
			sbio->write(tmpbuf, len);
			out->write(tmpbuf, len);
			/* Look for blank line signifying end of headers*/
			if ((tmpbuf[0] == '\r') || (tmpbuf[0] == '\n')) break;
		}

		sbio->puts("--------------------------------------------------\r\n");
		sbio->puts("\r\n");

		/* Since there is a buffering BIO present we had better flush it */
		sbio->flush();
		sbio->ssl_shutdown();
		sbio->free();
	}
	return 1;
}

void TestSSLServerFunc()
{
	COpenSSLLib::Initialize();

	TestServer();

	COpenSSLLib::CleanUp();
}