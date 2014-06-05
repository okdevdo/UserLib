/******************************************************************************
    
	This file is part of CryptSources, which is part of UserLib.

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
#ifndef __HASH_H__
#define __HASH_H__

#include "cysources.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		byte checksum[16];
		byte X[48];
		byte buffer[16];
		dword count;
	} MD2_CTX;

	typedef struct {
		qword length;
		dword state[4];
		dword curlen;
		byte buf[64];
	} MD4_CTX;

	typedef struct {
		qword length;
		dword state[4];
		dword curlen;
		byte buf[64];
	} MD5_CTX;

	typedef struct {
		qword length;
		dword state[5];
		dword curlen;
		byte buf[64];
	} SHA1_CTX;

	typedef struct {
		qword length;
		dword state[8];
		dword curlen;
		byte buf[64];
	} SHA224_CTX;

	typedef struct {
		qword length;
		dword state[8];
		dword curlen;
		byte buf[64];
	} SHA256_CTX;

	typedef struct {
		qword length;
		qword state[8];
		dword curlen;
		byte buf[128];
	} SHA384_CTX;

	typedef struct {
		qword length;
		qword state[8];
		dword curlen;
		byte buf[128];
	} SHA512_CTX;

	CYSOURCES_API void MD2Init(Ptr(MD2_CTX) ctx);
	CYSOURCES_API void MD2Update(Ptr(MD2_CTX) ctx, BConstPointer data, dword datalen);
	CYSOURCES_API void MD2Finish(Ptr(MD2_CTX) ctx, byte data[16]);

	CYSOURCES_API void MD4Init(Ptr(MD4_CTX) ctx);
	CYSOURCES_API void MD4Update(Ptr(MD4_CTX) ctx, BConstPointer data, dword datalen);
	CYSOURCES_API void MD4Finish(Ptr(MD4_CTX) ctx, byte data[16]);

	CYSOURCES_API void MD5Init(Ptr(MD5_CTX) ctx);
	CYSOURCES_API void MD5Update(Ptr(MD5_CTX) ctx, BConstPointer data, dword datalen);
	CYSOURCES_API void MD5Finish(Ptr(MD5_CTX) ctx, byte data[16]);

	CYSOURCES_API void SHA1Init(Ptr(SHA1_CTX) ctx);
	CYSOURCES_API void SHA1Update(Ptr(SHA1_CTX) ctx, BConstPointer data, dword datalen);
	CYSOURCES_API void SHA1Finish(Ptr(SHA1_CTX) ctx, byte data[20]);

	CYSOURCES_API void SHA224Init(Ptr(SHA224_CTX) ctx);
	CYSOURCES_API void SHA224Update(Ptr(SHA224_CTX) ctx, BConstPointer data, dword datalen);
	CYSOURCES_API void SHA224Finish(Ptr(SHA224_CTX) ctx, byte data[28]);

	CYSOURCES_API void SHA256Init(Ptr(SHA256_CTX) ctx);
	CYSOURCES_API void SHA256Update(Ptr(SHA256_CTX) ctx, BConstPointer data, dword datalen);
	CYSOURCES_API void SHA256Finish(Ptr(SHA256_CTX) ctx, byte data[32]);

	CYSOURCES_API void SHA384Init(Ptr(SHA384_CTX) ctx);
	CYSOURCES_API void SHA384Update(Ptr(SHA384_CTX) ctx, BConstPointer data, dword datalen);
	CYSOURCES_API void SHA384Finish(Ptr(SHA384_CTX) ctx, byte data[48]);

	CYSOURCES_API void SHA512Init(Ptr(SHA512_CTX) ctx);
	CYSOURCES_API void SHA512Update(Ptr(SHA512_CTX) ctx, BConstPointer data, dword datalen);
	CYSOURCES_API void SHA512Finish(Ptr(SHA512_CTX) ctx, byte data[64]);

#ifdef __cplusplus
}
#endif

#endif
