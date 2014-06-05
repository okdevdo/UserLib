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
@file md5.c
LTC_MD5 hash function by Tom St Denis
*/
#include "cy_pch.h"
#include "hash.h"

#ifdef OK_COMP_MSC
#pragma intrinsic(_lrotl)
#define ROLc(x, n) _lrotl(x,n)
#else
#define ROLc(x, y) ((((dword)(x)<<(dword)((y)&31)) | (((dword)(x)&0xFFFFFFFFUL)>>(dword)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif

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

#define F(x,y,z)  (z ^ (x & (y ^ z)))
#define G(x,y,z)  (y ^ (z & (y ^ x)))
#define H(x,y,z)  (x^y^z)
#define I(x,y,z)  (y^(x|(~z)))

#define FF(a,b,c,d,M,s,t) \
	a = (a + F(b, c, d) + M + t); a = ROLc(a, s) + b;

#define GG(a,b,c,d,M,s,t) \
	a = (a + G(b, c, d) + M + t); a = ROLc(a, s) + b;

#define HH(a,b,c,d,M,s,t) \
	a = (a + H(b, c, d) + M + t); a = ROLc(a, s) + b;

#define II(a,b,c,d,M,s,t) \
	a = (a + I(b, c, d) + M + t); a = ROLc(a, s) + b;

static void MD5Transform(Ptr(MD5_CTX) ctx, BConstPointer buf)
{
	dword i;
	dword W[16];
	dword a;
	dword b;
	dword c;
	dword d;

	for (i = 0; i < 16; i++)
		LOAD32L(W[i], buf + (4 * i));
	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];

	FF(a, b, c, d, W[0], 7, 0xd76aa478UL)
	FF(d, a, b, c, W[1], 12, 0xe8c7b756UL)
	FF(c, d, a, b, W[2], 17, 0x242070dbUL)
	FF(b, c, d, a, W[3], 22, 0xc1bdceeeUL)
	FF(a, b, c, d, W[4], 7, 0xf57c0fafUL)
	FF(d, a, b, c, W[5], 12, 0x4787c62aUL)
	FF(c, d, a, b, W[6], 17, 0xa8304613UL)
	FF(b, c, d, a, W[7], 22, 0xfd469501UL)
	FF(a, b, c, d, W[8], 7, 0x698098d8UL)
	FF(d, a, b, c, W[9], 12, 0x8b44f7afUL)
	FF(c, d, a, b, W[10], 17, 0xffff5bb1UL)
	FF(b, c, d, a, W[11], 22, 0x895cd7beUL)
	FF(a, b, c, d, W[12], 7, 0x6b901122UL)
	FF(d, a, b, c, W[13], 12, 0xfd987193UL)
	FF(c, d, a, b, W[14], 17, 0xa679438eUL)
	FF(b, c, d, a, W[15], 22, 0x49b40821UL)
	GG(a, b, c, d, W[1], 5, 0xf61e2562UL)
	GG(d, a, b, c, W[6], 9, 0xc040b340UL)
	GG(c, d, a, b, W[11], 14, 0x265e5a51UL)
	GG(b, c, d, a, W[0], 20, 0xe9b6c7aaUL)
	GG(a, b, c, d, W[5], 5, 0xd62f105dUL)
	GG(d, a, b, c, W[10], 9, 0x02441453UL)
	GG(c, d, a, b, W[15], 14, 0xd8a1e681UL)
	GG(b, c, d, a, W[4], 20, 0xe7d3fbc8UL)
	GG(a, b, c, d, W[9], 5, 0x21e1cde6UL)
	GG(d, a, b, c, W[14], 9, 0xc33707d6UL)
	GG(c, d, a, b, W[3], 14, 0xf4d50d87UL)
	GG(b, c, d, a, W[8], 20, 0x455a14edUL)
	GG(a, b, c, d, W[13], 5, 0xa9e3e905UL)
	GG(d, a, b, c, W[2], 9, 0xfcefa3f8UL)
	GG(c, d, a, b, W[7], 14, 0x676f02d9UL)
	GG(b, c, d, a, W[12], 20, 0x8d2a4c8aUL)
	HH(a, b, c, d, W[5], 4, 0xfffa3942UL)
	HH(d, a, b, c, W[8], 11, 0x8771f681UL)
	HH(c, d, a, b, W[11], 16, 0x6d9d6122UL)
	HH(b, c, d, a, W[14], 23, 0xfde5380cUL)
	HH(a, b, c, d, W[1], 4, 0xa4beea44UL)
	HH(d, a, b, c, W[4], 11, 0x4bdecfa9UL)
	HH(c, d, a, b, W[7], 16, 0xf6bb4b60UL)
	HH(b, c, d, a, W[10], 23, 0xbebfbc70UL)
	HH(a, b, c, d, W[13], 4, 0x289b7ec6UL)
	HH(d, a, b, c, W[0], 11, 0xeaa127faUL)
	HH(c, d, a, b, W[3], 16, 0xd4ef3085UL)
	HH(b, c, d, a, W[6], 23, 0x04881d05UL)
	HH(a, b, c, d, W[9], 4, 0xd9d4d039UL)
	HH(d, a, b, c, W[12], 11, 0xe6db99e5UL)
	HH(c, d, a, b, W[15], 16, 0x1fa27cf8UL)
	HH(b, c, d, a, W[2], 23, 0xc4ac5665UL)
	II(a, b, c, d, W[0], 6, 0xf4292244UL)
	II(d, a, b, c, W[7], 10, 0x432aff97UL)
	II(c, d, a, b, W[14], 15, 0xab9423a7UL)
	II(b, c, d, a, W[5], 21, 0xfc93a039UL)
	II(a, b, c, d, W[12], 6, 0x655b59c3UL)
	II(d, a, b, c, W[3], 10, 0x8f0ccc92UL)
	II(c, d, a, b, W[10], 15, 0xffeff47dUL)
	II(b, c, d, a, W[1], 21, 0x85845dd1UL)
	II(a, b, c, d, W[8], 6, 0x6fa87e4fUL)
	II(d, a, b, c, W[15], 10, 0xfe2ce6e0UL)
	II(c, d, a, b, W[6], 15, 0xa3014314UL)
	II(b, c, d, a, W[13], 21, 0x4e0811a1UL)
	II(a, b, c, d, W[4], 6, 0xf7537e82UL)
	II(d, a, b, c, W[11], 10, 0xbd3af235UL)
	II(c, d, a, b, W[2], 15, 0x2ad7d2bbUL)
	II(b, c, d, a, W[9], 21, 0xeb86d391UL)

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
}

void MD5Init(Ptr(MD5_CTX) ctx)
{
	assert(NotPtrCheck(ctx));
	ctx->state[0] = 0x67452301UL;
	ctx->state[1] = 0xefcdab89UL;
	ctx->state[2] = 0x98badcfeUL;
	ctx->state[3] = 0x10325476UL;
	ctx->curlen = 0;
	ctx->length = 0;
}

void MD5Update(Ptr(MD5_CTX) ctx, BConstPointer data, dword datalen)
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
			MD5Transform(ctx, data);
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
				MD5Transform(ctx, ctx->buf);
				ctx->length += 8 * block_size;
				ctx->curlen = 0;
			}
		}
	}
}

void MD5Finish(Ptr(MD5_CTX) ctx, byte data[16])
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
		MD5Transform(ctx, ctx->buf);
		ctx->curlen = 0;
	}
	while (ctx->curlen < 56)
		ctx->buf[ctx->curlen++] = (byte)0;
	STORE64L(ctx->length, ctx->buf + 56);
	MD5Transform(ctx, ctx->buf);
	for (i = 0; i < 4; i++)
		STORE32L(ctx->state[i], data + (4 * i));
	s_memset(ctx, 0, sizeof(MD5_CTX));
}

/* $Source: /cvs/libtom/libtomcrypt/src/hashes/md5.c,v $ */
/* $Revision: 1.10 $ */
/* $Date: 2007/05/12 14:25:28 $ */
