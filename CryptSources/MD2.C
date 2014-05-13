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
/* LibTomCrypt, modular cryptographic library -- Tom St Denis
*
* LibTomCrypt is a library that provides various cryptographic
* algorithms in a highly modular and flexible manner.
*
* The library is free for all purposes without any express
* guarantee it works.
*
* Tom St Denis, tomstdenis@gmail.com, http://libtom.org
*/
/**
@param md2.c
LTC_MD2 (RFC 1319) hash function implementation by Tom St Denis
*/
#include "CY_PCH.H"
#include "HASH.H"

static const byte PI_SUBST[256] = {
	41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
	19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
	76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
	138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
	245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
	148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
	39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
	181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
	150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
	112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
	96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
	85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
	234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
	129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
	8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
	203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
	166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
	31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

/* adds 16 bytes to the checksum */
static void MD2UpdateChecksum(Ptr(MD2_CTX) ctx)
{
	dword j;
	byte L = ctx->checksum[15];

	for (j = 0; j < 16; j++) 
		L = (ctx->checksum[j] ^= PI_SUBST[(int)(ctx->buffer[j] ^ L)] & 255);
}

static void MD2Transform(Ptr(MD2_CTX) ctx)
{
	dword j;
	dword k;
	byte t;

	for (j = 0; j < 16; j++) 
	{
		ctx->X[16 + j] = ctx->buffer[j];
		ctx->X[32 + j] = ctx->X[j] ^ ctx->X[16 + j];
	}
	t = (byte)0;
	for (j = 0; j < 18; j++) 
	{
		for (k = 0; k < 48; k++)
			t = (ctx->X[k] ^= PI_SUBST[(int)(t & 255)]);
		t = (t + (byte)j) & 255;
	}
}

void MD2Init(Ptr(MD2_CTX) ctx)
{
	assert(NotPtrCheck(ctx));

	s_memset(ctx->X, 0, 48);
	s_memset(ctx->checksum, 0, 16);
	s_memset(ctx->buffer, 0, 16);
	ctx->count = 0;
}

void MD2Update(Ptr(MD2_CTX) ctx, BConstPointer data, dword datalen)
{
	dword n;

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	assert(ctx->count <= 16);
	while (datalen > 0)
	{
		n = Min(datalen, (16 - ctx->count));
		s_memcpy(ctx->buffer + ctx->count, data, n);
		ctx->count += n;
		data += n;
		datalen -= n;

		if (ctx->count >= 16)
		{
			MD2Transform(ctx);
			MD2UpdateChecksum(ctx);
			ctx->count = 0;
		}
	}
}

void MD2Finish(Ptr(MD2_CTX) ctx, byte data[16])
{
	dword i;
	dword k;

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	assert(ctx->count <= 16);
	k = 16 - ctx->count;
	for (i = ctx->count; i < 16; i++)
		ctx->buffer[i] = (byte)k;
	MD2Transform(ctx);
	MD2UpdateChecksum(ctx);
	s_memcpy(ctx->buffer, ctx->checksum, 16);
	MD2Transform(ctx);
	s_memcpy(data, ctx->X, 16);
	s_memset(ctx, 0, sizeof(MD2_CTX));
}

/* $Source: /cvs/libtom/libtomcrypt/src/hashes/md2.c,v $ */
/* $Revision: 1.10 $ */
/* $Date: 2007/05/12 14:25:28 $ */
