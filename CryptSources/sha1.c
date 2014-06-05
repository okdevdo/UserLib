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
@file sha1.c
LTC_SHA1 code by Tom St Denis
*/
#include "cy_pch.h"
#include "hash.h"

#define LOAD32H(x, y)                            \
	 { x = ((unsigned long)((y)[0] & 255) << 24) | \
	 ((unsigned long)((y)[1] & 255) << 16) | \
	 ((unsigned long)((y)[2] & 255) << 8) | \
	 ((unsigned long)((y)[3] & 255)); }

#define STORE32H(x, y)                                                                     \
	 { (y)[0] = (unsigned char)(((x) >> 24) & 255); (y)[1] = (unsigned char)(((x) >> 16) & 255);   \
	 (y)[2] = (unsigned char)(((x) >> 8) & 255); (y)[3] = (unsigned char)((x)& 255); }

#define STORE64H(x, y)                                                                     \
   { (y)[0] = (unsigned char)(((x) >> 56) & 255); (y)[1] = (unsigned char)(((x) >> 48) & 255);     \
   (y)[2] = (unsigned char)(((x)>>40)&255); (y)[3] = (unsigned char)(((x)>>32)&255);     \
   (y)[4] = (unsigned char)(((x)>>24)&255); (y)[5] = (unsigned char)(((x)>>16)&255);     \
   (y)[6] = (unsigned char)(((x) >> 8) & 255); (y)[7] = (unsigned char)((x)& 255); }

#ifdef OK_COMP_MSC
#pragma intrinsic(_lrotl)
#define ROL(x, n) _lrotl(x,n)
#else
#define ROL(x, y) ((((dword)(x)<<(dword)((y)&31)) | (((dword)(x)&0xFFFFFFFFUL)>>(dword)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif
#define ROLc ROL

#define F0(x,y,z)  (z ^ (x & (y ^ z)))
#define F1(x,y,z)  (x ^ y ^ z)
#define F2(x,y,z)  ((x & y) | (z & (x | y)))
#define F3(x,y,z)  (x ^ y ^ z)

static void SHA1Transform(Ptr(SHA1_CTX) ctx, BPointer buf)
{
	dword a;
	dword b;
	dword c;
	dword d;
	dword e;
	dword W[80];
	dword i;

	for (i = 0; i < 16; i++)
		LOAD32H(W[i], buf + (4 * i));

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];

	for (i = 16; i < 80; i++)
		W[i] = ROL(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);

#define FF0(a,b,c,d,e,i) e = (ROLc(a, 5) + F0(b,c,d) + e + W[i] + 0x5a827999UL); b = ROLc(b, 30);
#define FF1(a,b,c,d,e,i) e = (ROLc(a, 5) + F1(b,c,d) + e + W[i] + 0x6ed9eba1UL); b = ROLc(b, 30);
#define FF2(a,b,c,d,e,i) e = (ROLc(a, 5) + F2(b,c,d) + e + W[i] + 0x8f1bbcdcUL); b = ROLc(b, 30);
#define FF3(a,b,c,d,e,i) e = (ROLc(a, 5) + F3(b,c,d) + e + W[i] + 0xca62c1d6UL); b = ROLc(b, 30);

	/* round one */
	for (i = 0; i < 20;) {
		FF0(a, b, c, d, e, i++);
		FF0(e, a, b, c, d, i++);
		FF0(d, e, a, b, c, i++);
		FF0(c, d, e, a, b, i++);
		FF0(b, c, d, e, a, i++);
	}

	/* round two */
	for (; i < 40;)  {
		FF1(a, b, c, d, e, i++);
		FF1(e, a, b, c, d, i++);
		FF1(d, e, a, b, c, i++);
		FF1(c, d, e, a, b, i++);
		FF1(b, c, d, e, a, i++);
	}

	/* round three */
	for (; i < 60;)  {
		FF2(a, b, c, d, e, i++);
		FF2(e, a, b, c, d, i++);
		FF2(d, e, a, b, c, i++);
		FF2(c, d, e, a, b, i++);
		FF2(b, c, d, e, a, i++);
	}

	/* round four */
	for (; i < 80;)  {
		FF3(a, b, c, d, e, i++);
		FF3(e, a, b, c, d, i++);
		FF3(d, e, a, b, c, i++);
		FF3(c, d, e, a, b, i++);
		FF3(b, c, d, e, a, i++);
	}

#undef FF0
#undef FF1
#undef FF2
#undef FF3

	/* store */
	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
}

void SHA1Init(Ptr(SHA1_CTX) ctx)
{
	assert(NotPtrCheck(ctx));
	ctx->state[0] = 0x67452301UL;
	ctx->state[1] = 0xefcdab89UL;
	ctx->state[2] = 0x98badcfeUL;
	ctx->state[3] = 0x10325476UL;
	ctx->state[4] = 0xc3d2e1f0UL;
	ctx->curlen = 0;
	ctx->length = 0;
}

void SHA1Update(Ptr(SHA1_CTX) ctx, BConstPointer data, dword datalen)
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
			SHA1Transform(ctx, CastMutable(BPointer, data));
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
				SHA1Transform(ctx, ctx->buf);
				ctx->length += 8 * block_size;
				ctx->curlen = 0;
			}
		}
	}
}

void SHA1Finish(Ptr(SHA1_CTX) ctx, byte data[20])
{
	int i;

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	assert(ctx->curlen <= 64);

	ctx->length += ctx->curlen * 8;
	ctx->buf[ctx->curlen++] = (byte)0x80;
	if (ctx->curlen > 56)
	{
		while (ctx->curlen < 64)
			ctx->buf[ctx->curlen++] = (byte)0;
		SHA1Transform(ctx, ctx->buf);
		ctx->curlen = 0;
	}
	while (ctx->curlen < 56)
		ctx->buf[ctx->curlen++] = (byte)0;
	STORE64H(ctx->length, ctx->buf + 56);
	SHA1Transform(ctx, ctx->buf);
	for (i = 0; i < 5; i++)
		STORE32H(ctx->state[i], data + (4 * i));
	s_memset(ctx, 0, sizeof(SHA1_CTX));
}

/* $Source: /cvs/libtom/libtomcrypt/src/hashes/sha1.c,v $ */
/* $Revision: 1.10 $ */
/* $Date: 2007/05/12 14:25:28 $ */
