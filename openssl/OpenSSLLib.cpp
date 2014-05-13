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
#include "OpenSSLLib.h"
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

void COpenSSLLib::Initialize()
{
#ifdef __DEBUG__
	CRYPTO_set_mem_ex_functions(&TFallocDbg, &TFreallocDbg, &TFfree);
	CRYPTO_set_locked_mem_ex_functions(&TFallocDbg, &TFfree);
#else
	CRYPTO_set_mem_functions(&TFalloc, &TFrealloc, &TFfree);
	CRYPTO_set_locked_mem_functions(&TFalloc, &TFfree);
#endif

	/* Load the human readable error strings for libcrypto */
	ERR_load_crypto_strings();
	SSL_load_error_strings();

	/* Load all digest and cipher algorithms */
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();

	/* init SSL library */
	SSL_library_init();

	/* Load config file, and other important initialisation */
	OPENSSL_config(NULL);
}

void COpenSSLLib::PrintErrorsFp(FILE *fp)
{
	ERR_print_errors_fp(fp);
}

void COpenSSLLib::CleanUp()
{
	/* Removes all digests and ciphers */
	EVP_cleanup();

	/* Remove error strings */
	ERR_free_strings();
}
