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
@param md4.c
Submitted by Dobes Vandermeer  (dobes@smartt.com)
*/
#include "CY_PCH.H"
#include "HASH.H"

#define S11 3
#define S12 7
#define S13 11
#define S14 19
#define S21 3
#define S22 5
#define S23 9
#define S24 13
#define S31 3
#define S32 9
#define S33 11
#define S34 15

#define F(x, y, z) (z ^ (x & (y ^ z)))
#define G(x, y, z) ((x & y) | (z & (x | y)))
#define H(x, y, z) ((x) ^ (y) ^ (z))

#ifdef OK_COMP_MSC
#pragma intrinsic(_lrotl)
#define ROTATE_LEFT(x, n) _lrotl(x,n)
#else
#define ROTATE_LEFT(x, y) ((((dword)(x)<<(dword)((y)&31)) | (((dword)(x)&0xFFFFFFFFUL)>>(dword)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif

#define FF(a, b, c, d, x, s) { \
	(a) += F((b), (c), (d)) + (x); \
	(a) = ROTATE_LEFT((a), (s)); \
}
#define GG(a, b, c, d, x, s) { \
	(a) += G((b), (c), (d)) + (x)+0x5a827999UL; \
	(a) = ROTATE_LEFT((a), (s)); \
	}
#define HH(a, b, c, d, x, s) { \
	(a) += H((b), (c), (d)) + (x)+0x6ed9eba1UL; \
	(a) = ROTATE_LEFT((a), (s)); \
	}

#define LOAD32L(x, y)                            \
	 { x = ((unsigned long)((y)[3] & 255) << 24) | \
	 ((unsigned long)((y)[2] & 255) << 16) | \
	 ((unsigned long)((y)[1] & 255) << 8) | \
	 ((unsigned long)((y)[0] & 255)); }
#define STORE32L(x, y)                                                                     \
	 { (y)[3] = (unsigned char)(((x) >> 24) & 255); (y)[2] = (unsigned char)(((x) >> 16) & 255);   \
	 (y)[1] = (unsigned char)(((x) >> 8) & 255); (y)[0] = (unsigned char)((x)& 255); }
#define STORE64L(x, y)                                                                     \
	 { (y)[7] = (unsigned char)(((x) >> 56) & 255); (y)[6] = (unsigned char)(((x) >> 48) & 255);   \
	 (y)[5] = (unsigned char)(((x) >> 40) & 255); (y)[4] = (unsigned char)(((x) >> 32) & 255);   \
	 (y)[3] = (unsigned char)(((x) >> 24) & 255); (y)[2] = (unsigned char)(((x) >> 16) & 255);   \
	 (y)[1] = (unsigned char)(((x) >> 8) & 255); (y)[0] = (unsigned char)((x)& 255); }

static void MD4Transform(Ptr(MD4_CTX) ctx, BConstPointer buf)
{
	dword x[16];
	dword a;
	dword b;
	dword c;
	dword d;
	dword i;

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];

	for (i = 0; i < 16; i++)
		LOAD32L(x[i], buf + (4 * i));

	/* Round 1 */
	FF(a, b, c, d, x[0], S11); /* 1 */
	FF(d, a, b, c, x[1], S12); /* 2 */
	FF(c, d, a, b, x[2], S13); /* 3 */
	FF(b, c, d, a, x[3], S14); /* 4 */
	FF(a, b, c, d, x[4], S11); /* 5 */
	FF(d, a, b, c, x[5], S12); /* 6 */
	FF(c, d, a, b, x[6], S13); /* 7 */
	FF(b, c, d, a, x[7], S14); /* 8 */
	FF(a, b, c, d, x[8], S11); /* 9 */
	FF(d, a, b, c, x[9], S12); /* 10 */
	FF(c, d, a, b, x[10], S13); /* 11 */
	FF(b, c, d, a, x[11], S14); /* 12 */
	FF(a, b, c, d, x[12], S11); /* 13 */
	FF(d, a, b, c, x[13], S12); /* 14 */
	FF(c, d, a, b, x[14], S13); /* 15 */
	FF(b, c, d, a, x[15], S14); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[0], S21); /* 17 */
	GG(d, a, b, c, x[4], S22); /* 18 */
	GG(c, d, a, b, x[8], S23); /* 19 */
	GG(b, c, d, a, x[12], S24); /* 20 */
	GG(a, b, c, d, x[1], S21); /* 21 */
	GG(d, a, b, c, x[5], S22); /* 22 */
	GG(c, d, a, b, x[9], S23); /* 23 */
	GG(b, c, d, a, x[13], S24); /* 24 */
	GG(a, b, c, d, x[2], S21); /* 25 */
	GG(d, a, b, c, x[6], S22); /* 26 */
	GG(c, d, a, b, x[10], S23); /* 27 */
	GG(b, c, d, a, x[14], S24); /* 28 */
	GG(a, b, c, d, x[3], S21); /* 29 */
	GG(d, a, b, c, x[7], S22); /* 30 */
	GG(c, d, a, b, x[11], S23); /* 31 */
	GG(b, c, d, a, x[15], S24); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[0], S31); /* 33 */
	HH(d, a, b, c, x[8], S32); /* 34 */
	HH(c, d, a, b, x[4], S33); /* 35 */
	HH(b, c, d, a, x[12], S34); /* 36 */
	HH(a, b, c, d, x[2], S31); /* 37 */
	HH(d, a, b, c, x[10], S32); /* 38 */
	HH(c, d, a, b, x[6], S33); /* 39 */
	HH(b, c, d, a, x[14], S34); /* 40 */
	HH(a, b, c, d, x[1], S31); /* 41 */
	HH(d, a, b, c, x[9], S32); /* 42 */
	HH(c, d, a, b, x[5], S33); /* 43 */
	HH(b, c, d, a, x[13], S34); /* 44 */
	HH(a, b, c, d, x[3], S31); /* 45 */
	HH(d, a, b, c, x[11], S32); /* 46 */
	HH(c, d, a, b, x[7], S33); /* 47 */
	HH(b, c, d, a, x[15], S34); /* 48 */

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
}

void MD4Init(Ptr(MD4_CTX) ctx)
{
	assert(NotPtrCheck(ctx));
	ctx->state[0] = 0x67452301UL;
	ctx->state[1] = 0xefcdab89UL;
	ctx->state[2] = 0x98badcfeUL;
	ctx->state[3] = 0x10325476UL;
	ctx->length = 0;
	ctx->curlen = 0;
}

void MD4Update(Ptr(MD4_CTX) ctx, BConstPointer data, dword datalen)
{
	dword block_size = 64;
	dword n;

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	assert(ctx->curlen <= block_size);
	while (datalen > 0) 
	{
		if (ctx->curlen == 0 && datalen >= block_size) 
		{
			MD4Transform(ctx, data);
			ctx->length += block_size * 8;
			data += block_size;
			datalen -= block_size;
		}
		else 
		{
			n = Min(datalen, (block_size - ctx->curlen));
			s_memcpy(ctx->buf + ctx->curlen, data, n);
			ctx->curlen += n;
			data += n;
			datalen -= n;
			if (ctx->curlen == block_size) 
			{
				MD4Transform(ctx, ctx->buf);
				ctx->length += 8 * block_size;
				ctx->curlen = 0;
			}
		}
	}
}

void MD4Finish(Ptr(MD4_CTX) ctx, byte data[16])
{
	dword i;

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	assert(ctx->curlen <= 64);

	ctx->length += ctx->curlen * 8;
	ctx->buf[ctx->curlen++] = (byte)0x80;
	if (ctx->curlen > 56) {
		while (ctx->curlen < 64)
			ctx->buf[ctx->curlen++] = (byte)0;
		MD4Transform(ctx, ctx->buf);
		ctx->curlen = 0;
	}
	while (ctx->curlen < 56)
		ctx->buf[ctx->curlen++] = (byte)0;
	STORE64L(ctx->length, ctx->buf + 56);
	MD4Transform(ctx, ctx->buf);
	for (i = 0; i < 4; i++)
		STORE32L(ctx->state[i], data + (4 * i));
	s_memset(ctx, 0, sizeof(MD4_CTX));
}

/* $Source: /cvs/libtom/libtomcrypt/src/hashes/md4.c,v $ */
/* $Revision: 1.10 $ */
/* $Date: 2007/05/12 14:25:28 $ */
