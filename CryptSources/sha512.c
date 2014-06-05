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
@param sha512.c
LTC_SHA512 by Tom St Denis
*/
#include "cy_pch.h"
#include "hash.h"

#ifdef OK_COMP_MSC
#define CONST64(n) n ## ui64
#else
#define CONST64(n) n ## ULL
#endif

#define ROR64c(x, y) \
	(((((x)&CONST64(0xFFFFFFFFFFFFFFFF)) >> ((qword)(y)&CONST64(63))) | \
	((x) << ((qword)(64 - ((y)&CONST64(63)))))) & CONST64(0xFFFFFFFFFFFFFFFF))

#define LOAD64H(x, y) \
   { x = (((qword)((y)[0] & 255)) << 56) | (((qword)((y)[1] & 255)) << 48) | \
(((qword)((y)[2] & 255)) << 40) | (((qword)((y)[3] & 255)) << 32) | \
(((qword)((y)[4] & 255)) << 24) | (((qword)((y)[5] & 255)) << 16) | \
(((qword)((y)[6] & 255)) << 8) | (((qword)((y)[7] & 255)));	}

#define STORE64H(x, y) \
   { (y)[0] = (byte)(((x) >> 56) & 255); (y)[1] = (byte)(((x) >> 48) & 255);     \
   (y)[2] = (byte)(((x) >> 40) & 255); (y)[3] = (byte)(((x) >> 32) & 255);     \
   (y)[4] = (byte)(((x) >> 24) & 255); (y)[5] = (byte)(((x) >> 16) & 255);     \
   (y)[6] = (byte)(((x) >> 8) & 255); (y)[7] = (byte)((x)& 255); }

static const qword K[80] = {
	CONST64(0x428a2f98d728ae22), CONST64(0x7137449123ef65cd),
	CONST64(0xb5c0fbcfec4d3b2f), CONST64(0xe9b5dba58189dbbc),
	CONST64(0x3956c25bf348b538), CONST64(0x59f111f1b605d019),
	CONST64(0x923f82a4af194f9b), CONST64(0xab1c5ed5da6d8118),
	CONST64(0xd807aa98a3030242), CONST64(0x12835b0145706fbe),
	CONST64(0x243185be4ee4b28c), CONST64(0x550c7dc3d5ffb4e2),
	CONST64(0x72be5d74f27b896f), CONST64(0x80deb1fe3b1696b1),
	CONST64(0x9bdc06a725c71235), CONST64(0xc19bf174cf692694),
	CONST64(0xe49b69c19ef14ad2), CONST64(0xefbe4786384f25e3),
	CONST64(0x0fc19dc68b8cd5b5), CONST64(0x240ca1cc77ac9c65),
	CONST64(0x2de92c6f592b0275), CONST64(0x4a7484aa6ea6e483),
	CONST64(0x5cb0a9dcbd41fbd4), CONST64(0x76f988da831153b5),
	CONST64(0x983e5152ee66dfab), CONST64(0xa831c66d2db43210),
	CONST64(0xb00327c898fb213f), CONST64(0xbf597fc7beef0ee4),
	CONST64(0xc6e00bf33da88fc2), CONST64(0xd5a79147930aa725),
	CONST64(0x06ca6351e003826f), CONST64(0x142929670a0e6e70),
	CONST64(0x27b70a8546d22ffc), CONST64(0x2e1b21385c26c926),
	CONST64(0x4d2c6dfc5ac42aed), CONST64(0x53380d139d95b3df),
	CONST64(0x650a73548baf63de), CONST64(0x766a0abb3c77b2a8),
	CONST64(0x81c2c92e47edaee6), CONST64(0x92722c851482353b),
	CONST64(0xa2bfe8a14cf10364), CONST64(0xa81a664bbc423001),
	CONST64(0xc24b8b70d0f89791), CONST64(0xc76c51a30654be30),
	CONST64(0xd192e819d6ef5218), CONST64(0xd69906245565a910),
	CONST64(0xf40e35855771202a), CONST64(0x106aa07032bbd1b8),
	CONST64(0x19a4c116b8d2d0c8), CONST64(0x1e376c085141ab53),
	CONST64(0x2748774cdf8eeb99), CONST64(0x34b0bcb5e19b48a8),
	CONST64(0x391c0cb3c5c95a63), CONST64(0x4ed8aa4ae3418acb),
	CONST64(0x5b9cca4f7763e373), CONST64(0x682e6ff3d6b2b8a3),
	CONST64(0x748f82ee5defb2fc), CONST64(0x78a5636f43172f60),
	CONST64(0x84c87814a1f0ab72), CONST64(0x8cc702081a6439ec),
	CONST64(0x90befffa23631e28), CONST64(0xa4506cebde82bde9),
	CONST64(0xbef9a3f7b2c67915), CONST64(0xc67178f2e372532b),
	CONST64(0xca273eceea26619c), CONST64(0xd186b8c721c0c207),
	CONST64(0xeada7dd6cde0eb1e), CONST64(0xf57d4f7fee6ed178),
	CONST64(0x06f067aa72176fba), CONST64(0x0a637dc5a2c898a6),
	CONST64(0x113f9804bef90dae), CONST64(0x1b710b35131c471b),
	CONST64(0x28db77f523047d84), CONST64(0x32caab7b40c72493),
	CONST64(0x3c9ebe0a15c9bebc), CONST64(0x431d67c49c100d4c),
	CONST64(0x4cc5d4becb3e42b6), CONST64(0x597f299cfc657e2a),
	CONST64(0x5fcb6fab3ad6faec), CONST64(0x6c44198c4a475817)
};

/* Various logical functions */
#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y)) 
#define S(x, n)         ROR64c(x, n)
#define R(x, n)         (((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((qword)n))
#define Sigma0(x)       (S(x, 28) ^ S(x, 34) ^ S(x, 39))
#define Sigma1(x)       (S(x, 14) ^ S(x, 18) ^ S(x, 41))
#define Gamma0(x)       (S(x, 1) ^ S(x, 8) ^ R(x, 7))
#define Gamma1(x)       (S(x, 19) ^ S(x, 61) ^ R(x, 6))

static void SHA512Transform(Ptr(SHA512_CTX) ctx, BPointer buf)
{
	qword S[8];
	qword W[80];
	qword t0;
	qword t1;
	int i;

	for (i = 0; i < 8; i++)
		S[i] = ctx->state[i];
	for (i = 0; i < 16; i++)
		LOAD64H(W[i], buf + (8 * i));
	for (i = 16; i < 80; i++)
		W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];

#define RND(a,b,c,d,e,f,g,h,i)                    \
	t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i];   \
	t1 = Sigma0(a) + Maj(a, b, c);                  \
	d += t0;                                        \
	h = t0 + t1;

	for (i = 0; i < 80; i += 8) {
		RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i + 0);
		RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], i + 1);
		RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], i + 2);
		RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], i + 3);
		RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], i + 4);
		RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], i + 5);
		RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], i + 6);
		RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], i + 7);
	}
	for (i = 0; i < 8; i++)
		ctx->state[i] += S[i];
}

void SHA512Init(Ptr(SHA512_CTX) ctx)
{
	assert(NotPtrCheck(ctx));
	ctx->curlen = 0;
	ctx->length = 0;
	ctx->state[0] = CONST64(0x6a09e667f3bcc908);
	ctx->state[1] = CONST64(0xbb67ae8584caa73b);
	ctx->state[2] = CONST64(0x3c6ef372fe94f82b);
	ctx->state[3] = CONST64(0xa54ff53a5f1d36f1);
	ctx->state[4] = CONST64(0x510e527fade682d1);
	ctx->state[5] = CONST64(0x9b05688c2b3e6c1f);
	ctx->state[6] = CONST64(0x1f83d9abfb41bd6b);
	ctx->state[7] = CONST64(0x5be0cd19137e2179);
}

void SHA512Update(Ptr(SHA512_CTX) ctx, BConstPointer data, dword datalen)
{
	dword block_size = 128;
	dword n;

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	assert(ctx->curlen <= block_size);
	while (datalen > 0)
	{
		if (ctx->curlen == 0 && datalen >= block_size)
		{
			SHA512Transform(ctx, CastMutable(BPointer, data));
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
				SHA512Transform(ctx, ctx->buf);
				ctx->length += 8 * block_size;
				ctx->curlen = 0;
			}
		}
	}
}

void SHA512Finish(Ptr(SHA512_CTX) ctx, byte data[64])
{
	int i;

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	assert(ctx->curlen <= 128);

	ctx->length += ctx->curlen * CONST64(8);
	ctx->buf[ctx->curlen++] = (byte)0x80;
	if (ctx->curlen > 112) 
	{
		while (ctx->curlen < 128)
			ctx->buf[ctx->curlen++] = (byte)0;
		SHA512Transform(ctx, ctx->buf);
		ctx->curlen = 0;
	}
	while (ctx->curlen < 120)
		ctx->buf[ctx->curlen++] = (byte)0;
	STORE64H(ctx->length, ctx->buf + 120);
	SHA512Transform(ctx, ctx->buf);

	/* copy output */
	for (i = 0; i < 8; i++)
		STORE64H(ctx->state[i], data + (8 * i));
	s_memset(ctx, 0, sizeof(SHA512_CTX));
}

void SHA384Init(Ptr(SHA384_CTX) ctx)
{
	assert(NotPtrCheck(ctx));
	ctx->curlen = 0;
	ctx->length = 0;
	ctx->state[0] = CONST64(0xcbbb9d5dc1059ed8);
	ctx->state[1] = CONST64(0x629a292a367cd507);
	ctx->state[2] = CONST64(0x9159015a3070dd17);
	ctx->state[3] = CONST64(0x152fecd8f70e5939);
	ctx->state[4] = CONST64(0x67332667ffc00b31);
	ctx->state[5] = CONST64(0x8eb44a8768581511);
	ctx->state[6] = CONST64(0xdb0c2e0d64f98fa7);
	ctx->state[7] = CONST64(0x47b5481dbefa4fa4);
}

void SHA384Update(Ptr(SHA384_CTX) ctx, BConstPointer data, dword datalen)
{
	SHA512Update(CastAnyPtr(SHA512_CTX, ctx), data, datalen);
}

void SHA384Finish(Ptr(SHA384_CTX) ctx, byte data[48])
{
	unsigned char buf[64];

	assert(NotPtrCheck(ctx));
	assert(NotPtrCheck(data));
	assert(ctx->curlen <= 128);

	SHA512Finish(CastAnyPtr(SHA512_CTX, ctx), buf);
	s_memcpy(data, buf, 48);
	s_memset(buf, 0, 64);
}

/* $Source: /cvs/libtom/libtomcrypt/src/hashes/sha2/sha512.c,v $ */
/* $Revision: 1.10 $ */
/* $Date: 2007/05/12 14:25:28 $ */
