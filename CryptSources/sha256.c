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
@file sha256.c
LTC_SHA256 by Tom St Denis
*/
#include "cy_pch.h"
#include "hash.h"

#ifdef OK_COMP_MSC
#pragma intrinsic(_lrotr)
#define RORc(x, n) _lrotr(x,n)
#else
#define RORc(x, y) (((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif

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
   (y)[2] = (unsigned char)(((x) >> 40) & 255); (y)[3] = (unsigned char)(((x) >> 32) & 255);     \
   (y)[4] = (unsigned char)(((x) >> 24) & 255); (y)[5] = (unsigned char)(((x) >> 16) & 255);     \
   (y)[6] = (unsigned char)(((x) >> 8) & 255); (y)[7] = (unsigned char)((x)& 255); }

#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y)) 
#define S(x, n)         RORc((x),(n))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))
#define Sigma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))

static void SHA256Transform(Ptr(SHA256_CTX) ctx, BPointer buf)
{
	dword S[8];
	dword W[64];
	dword t0;
	dword t1;
	int i;

	for (i = 0; i < 8; i++)
		S[i] = ctx->state[i];
	for (i = 0; i < 16; i++)
		LOAD32H(W[i], buf + (4 * i));
	for (i = 16; i < 64; i++)
		W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];

#define RND(a,b,c,d,e,f,g,h,i,ki)                    \
	t0 = h + Sigma1(e) + Ch(e, f, g) + ki + W[i];   \
	t1 = Sigma0(a) + Maj(a, b, c);                  \
	d += t0;                                        \
	h = t0 + t1;

	RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 0, 0x428a2f98);
	RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 1, 0x71374491);
	RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 2, 0xb5c0fbcf);
	RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 3, 0xe9b5dba5);
	RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 4, 0x3956c25b);
	RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 5, 0x59f111f1);
	RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 6, 0x923f82a4);
	RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 7, 0xab1c5ed5);
	RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 8, 0xd807aa98);
	RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 9, 0x12835b01);
	RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 10, 0x243185be);
	RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 11, 0x550c7dc3);
	RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 12, 0x72be5d74);
	RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 13, 0x80deb1fe);
	RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 14, 0x9bdc06a7);
	RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 15, 0xc19bf174);
	RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 16, 0xe49b69c1);
	RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 17, 0xefbe4786);
	RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 18, 0x0fc19dc6);
	RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 19, 0x240ca1cc);
	RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 20, 0x2de92c6f);
	RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 21, 0x4a7484aa);
	RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 22, 0x5cb0a9dc);
	RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 23, 0x76f988da);
	RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 24, 0x983e5152);
	RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 25, 0xa831c66d);
	RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 26, 0xb00327c8);
	RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 27, 0xbf597fc7);
	RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 28, 0xc6e00bf3);
	RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 29, 0xd5a79147);
	RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 30, 0x06ca6351);
	RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 31, 0x14292967);
	RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 32, 0x27b70a85);
	RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 33, 0x2e1b2138);
	RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 34, 0x4d2c6dfc);
	RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 35, 0x53380d13);
	RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 36, 0x650a7354);
	RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 37, 0x766a0abb);
	RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 38, 0x81c2c92e);
	RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 39, 0x92722c85);
	RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 40, 0xa2bfe8a1);
	RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 41, 0xa81a664b);
	RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 42, 0xc24b8b70);
	RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 43, 0xc76c51a3);
	RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 44, 0xd192e819);
	RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 45, 0xd6990624);
	RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 46, 0xf40e3585);
	RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 47, 0x106aa070);
	RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 48, 0x19a4c116);
	RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 49, 0x1e376c08);
	RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 50, 0x2748774c);
	RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 51, 0x34b0bcb5);
	RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 52, 0x391c0cb3);
	RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 53, 0x4ed8aa4a);
	RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 54, 0x5b9cca4f);
	RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 55, 0x682e6ff3);
	RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 56, 0x748f82ee);
	RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 57, 0x78a5636f);
	RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 58, 0x84c87814);
	RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 59, 0x8cc70208);
	RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 60, 0x90befffa);
	RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 61, 0xa4506ceb);
	RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 62, 0xbef9a3f7);
	RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 63, 0xc67178f2);

#undef RND     

	for (i = 0; i < 8; i++)
		ctx->state[i] += S[i];
}

void SHA256Init(Ptr(SHA256_CTX) ctx)
{
	assert(NotPtrCheck(ctx));
	ctx->curlen = 0;
	ctx->length = 0;
	ctx->state[0] = 0x6A09E667UL;
	ctx->state[1] = 0xBB67AE85UL;
	ctx->state[2] = 0x3C6EF372UL;
	ctx->state[3] = 0xA54FF53AUL;
	ctx->state[4] = 0x510E527FUL;
	ctx->state[5] = 0x9B05688CUL;
	ctx->state[6] = 0x1F83D9ABUL;
	ctx->state[7] = 0x5BE0CD19UL;
}

void SHA256Update(Ptr(SHA256_CTX) ctx, BConstPointer data, dword datalen)
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
			SHA256Transform(ctx, CastMutable(BPointer, data));
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
				SHA256Transform(ctx, ctx->buf);
				ctx->length += 8 * block_size;
				ctx->curlen = 0;
			}
		}
	}
}

void SHA256Finish(Ptr(SHA256_CTX) ctx, byte data[32])
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
		SHA256Transform(ctx, ctx->buf);
		ctx->curlen = 0;
	}
	while (ctx->curlen < 56)
		ctx->buf[ctx->curlen++] = (byte)0;
	STORE64H(ctx->length, ctx->buf + 56);
	SHA256Transform(ctx, ctx->buf);
	for (i = 0; i < 8; i++)
		STORE32H(ctx->state[i], data + (4 * i));
	s_memset(ctx, 0, sizeof(SHA256_CTX));
}

void SHA224Init(Ptr(SHA224_CTX) ctx)
{
	assert(NotPtrCheck(ctx));
	ctx->curlen = 0;
	ctx->length = 0;
	ctx->state[0] = 0xc1059ed8UL;
	ctx->state[1] = 0x367cd507UL;
	ctx->state[2] = 0x3070dd17UL;
	ctx->state[3] = 0xf70e5939UL;
	ctx->state[4] = 0xffc00b31UL;
	ctx->state[5] = 0x68581511UL;
	ctx->state[6] = 0x64f98fa7UL;
	ctx->state[7] = 0xbefa4fa4UL;
}

void SHA224Update(Ptr(SHA224_CTX) ctx, BConstPointer data, dword datalen)
{
	SHA256Update(CastAnyPtr(SHA256_CTX, ctx), data, datalen);
}

void SHA224Finish(Ptr(SHA224_CTX) ctx, byte data[28])
{
	unsigned char buf[32];

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	SHA256Finish(CastAnyPtr(SHA256_CTX, ctx), buf);
	s_memcpy(data, buf, 28);
	s_memset(buf, 0, 32);
}

/* $Source: /cvs/libtom/libtomcrypt/src/hashes/sha2/sha256.c,v $ */
/* $Revision: 1.11 $ */
/* $Date: 2007/05/12 14:25:28 $ */
