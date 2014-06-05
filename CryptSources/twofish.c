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
@file twofish.c
Implementation of Twofish by Tom St Denis
*/
#include "cy_pch.h"
#include "cipher.h"

#ifdef OK_COMP_MSC
#define get_byte(x, n) ((byte)((x) >> (8 * (n))))
#else
#define get_byte(x, n) (((x) >> (8 * (n))) & 255)
#endif   

#define STORE32L(x, y)                                                                     \
	 { (y)[3] = (unsigned char)(((x)>>24)&255); (y)[2] = (unsigned char)(((x)>>16)&255);   \
	 (y)[1] = (unsigned char)(((x)>>8)&255); (y)[0] = (unsigned char)((x)&255); }
#define LOAD32L(x, y)                            \
	 { x = ((unsigned long)((y)[3] & 255)<<24) | \
	 ((unsigned long)((y)[2] & 255)<<16) | \
	 ((unsigned long)((y)[1] & 255)<<8)  | \
	 ((unsigned long)((y)[0] & 255)); }

#ifdef OK_COMP_MSC
#pragma intrinsic(_lrotl)
#define ROLc(x, n) _lrotl(x,n)
#else
#define ROLc(x, y) ((((dword)(x)<<(dword)((y)&31)) | (((dword)(x)&0xFFFFFFFFUL)>>(dword)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif
#ifdef OK_COMP_MSC
#pragma intrinsic(_lrotr)
#define RORc(x,n) _lrotr(x,n)
#else
#define RORc(x, y) ( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif

/* the two polynomials */
#define MDS_POLY          0x169
#define RS_POLY           0x14D

/* The 4x4 MDS Linear Transform */
static const byte MDS[4][4] = {
	{ 0x01, 0xEF, 0x5B, 0x5B },
	{ 0x5B, 0xEF, 0xEF, 0x01 },
	{ 0xEF, 0x5B, 0x01, 0xEF },
	{ 0xEF, 0x01, 0xEF, 0x5B }
};

/* The 4x8 RS Linear Transform */
static const byte RS[4][8] = {
	{ 0x01, 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E },
	{ 0xA4, 0x56, 0x82, 0xF3, 0X1E, 0XC6, 0X68, 0XE5 },
	{ 0X02, 0XA1, 0XFC, 0XC1, 0X47, 0XAE, 0X3D, 0X19 },
	{ 0XA4, 0X55, 0X87, 0X5A, 0X58, 0XDB, 0X9E, 0X03 }
};

/* sbox usage orderings */
static const byte qord[4][5] = {
	{ 1, 1, 0, 0, 1 },
	{ 0, 1, 1, 0, 0 },
	{ 0, 0, 0, 1, 1 },
	{ 1, 0, 1, 1, 0 }
};

#include "TWOFISH.H"

#define sbox(i, x) ((dword)SBOX[i][(x)&255])

/* computes ab mod p */
static dword gf_mult(dword a, dword b, dword p)
{
	dword result;
	dword B[2];
	dword P[2];

	P[1] = p;
	B[1] = b;
	result = P[0] = B[0] = 0;

	/* unrolled branchless GF multiplier */
	result ^= B[a & 1]; a >>= 1;  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
	result ^= B[a & 1]; a >>= 1;  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
	result ^= B[a & 1]; a >>= 1;  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
	result ^= B[a & 1]; a >>= 1;  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
	result ^= B[a & 1]; a >>= 1;  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
	result ^= B[a & 1]; a >>= 1;  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
	result ^= B[a & 1]; a >>= 1;  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
	result ^= B[a & 1];

	return result;
}

/* computes [y0 y1 y2 y3] = MDS . [x0] */
#define mds_column_mult(x, i) mds_tab[i][x]

/* Computes [y0 y1 y2 y3] = MDS . [x0 x1 x2 x3] */
static void mds_mult(BConstPointer in, BPointer out)
{
	int x;
	dword tmp;

	for (tmp = x = 0; x < 4; x++)
		tmp ^= mds_column_mult(in[x], x);
	STORE32L(tmp, out);
}

/* computes [y0 y1 y2 y3] = RS . [x0 x1 x2 x3 x4 x5 x6 x7] */
static void rs_mult(BConstPointer in, BPointer out)
{
	dword tmp;

	tmp = rs_tab0[in[0]] ^ rs_tab1[in[1]] ^ rs_tab2[in[2]] ^ rs_tab3[in[3]] ^
		rs_tab4[in[4]] ^ rs_tab5[in[5]] ^ rs_tab6[in[6]] ^ rs_tab7[in[7]];
	STORE32L(tmp, out);
}

/* computes h(x) */
static void h_func(BConstPointer in, BPointer out, BPointer M, int k, int offset)
{
	int x;
	byte y[4];

	for (x = 0; x < 4; x++)
		y[x] = in[x];
	switch (k) 
	{
	case 4:
		y[0] = (byte)(sbox(1, (dword)y[0]) ^ M[4 * (6 + offset) + 0]);
		y[1] = (byte)(sbox(0, (dword)y[1]) ^ M[4 * (6 + offset) + 1]);
		y[2] = (byte)(sbox(0, (dword)y[2]) ^ M[4 * (6 + offset) + 2]);
		y[3] = (byte)(sbox(1, (dword)y[3]) ^ M[4 * (6 + offset) + 3]);
	case 3:
		y[0] = (byte)(sbox(1, (dword)y[0]) ^ M[4 * (4 + offset) + 0]);
		y[1] = (byte)(sbox(1, (dword)y[1]) ^ M[4 * (4 + offset) + 1]);
		y[2] = (byte)(sbox(0, (dword)y[2]) ^ M[4 * (4 + offset) + 2]);
		y[3] = (byte)(sbox(0, (dword)y[3]) ^ M[4 * (4 + offset) + 3]);
	case 2:
		y[0] = (byte)(sbox(1, sbox(0, sbox(0, (dword)y[0]) ^ M[4 * (2 + offset) + 0]) ^ M[4 * (0 + offset) + 0]));
		y[1] = (byte)(sbox(0, sbox(0, sbox(1, (dword)y[1]) ^ M[4 * (2 + offset) + 1]) ^ M[4 * (0 + offset) + 1]));
		y[2] = (byte)(sbox(1, sbox(1, sbox(0, (dword)y[2]) ^ M[4 * (2 + offset) + 2]) ^ M[4 * (0 + offset) + 2]));
		y[3] = (byte)(sbox(0, sbox(1, sbox(1, (dword)y[3]) ^ M[4 * (2 + offset) + 3]) ^ M[4 * (0 + offset) + 3]));
	}
	mds_mult(y, out);
}

/* for GCC we don't use pointer aliases */
#if defined(__GNUC__)
#define S1 skey->twofishkey.S[0]
#define S2 skey->twofishkey.S[1]
#define S3 skey->twofishkey.S[2]
#define S4 skey->twofishkey.S[3]
#endif

/* the G function */
#define g_func(x, dum)  (S1[get_byte(x,0)] ^ S2[get_byte(x,1)] ^ S3[get_byte(x,2)] ^ S4[get_byte(x,3)])
#define g1_func(x, dum) (S2[get_byte(x,0)] ^ S3[get_byte(x,1)] ^ S4[get_byte(x,2)] ^ S1[get_byte(x,3)])

void TwofishSetup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey)
{
	byte S[4 * 4];
	byte tmpx0;
	byte tmpx1;
	int k;
	int x;
	int y;
	byte tmp[4];
	byte tmp2[4];
	byte M[8 * 4];
	dword A;
	dword B;

	assert(NotPtrCheck(key));
	assert(NotPtrCheck(skey));
	assert((keylen == 16) || (keylen == 24) || (keylen == 32));
	assert((num_rounds == 0) || (num_rounds == 16));

	k = keylen / 8;
	for (x = 0; x < keylen; x++)
		M[x] = key[x] & 255;
	for (x = 0; x < k; x++)
		rs_mult(M + (x * 8), S + (x * 4));
	for (x = 0; x < 20; x++) 
	{
		for (y = 0; y < 4; y++)
			tmp[y] = x + x;
		h_func(tmp, tmp2, M, k, 0);
		LOAD32L(A, tmp2);
		for (y = 0; y < 4; y++)
			tmp[y] = (byte)(x + x + 1);
		h_func(tmp, tmp2, M, k, 1);
		LOAD32L(B, tmp2);
		B = ROLc(B, 8);
		skey->twofishkey.K[x + x] = (A + B) & 0xFFFFFFFFUL;
		skey->twofishkey.K[x + x + 1] = ROLc(B + B + A, 9);
	}
	if (k == 2) 
	{
		for (x = 0; x < 256; x++) 
		{
			tmpx0 = (byte)sbox(0, x);
			tmpx1 = (byte)sbox(1, x);
			skey->twofishkey.S[0][x] = mds_column_mult(sbox(1, (sbox(0, tmpx0 ^ S[0]) ^ S[4])), 0);
			skey->twofishkey.S[1][x] = mds_column_mult(sbox(0, (sbox(0, tmpx1 ^ S[1]) ^ S[5])), 1);
			skey->twofishkey.S[2][x] = mds_column_mult(sbox(1, (sbox(1, tmpx0 ^ S[2]) ^ S[6])), 2);
			skey->twofishkey.S[3][x] = mds_column_mult(sbox(0, (sbox(1, tmpx1 ^ S[3]) ^ S[7])), 3);
		}
	}
	else if (k == 3) 
	{
		for (x = 0; x < 256; x++) 
		{
			tmpx0 = (byte)sbox(0, x);
			tmpx1 = (byte)sbox(1, x);
			skey->twofishkey.S[0][x] = mds_column_mult(sbox(1, (sbox(0, sbox(0, tmpx1 ^ S[0]) ^ S[4]) ^ S[8])), 0);
			skey->twofishkey.S[1][x] = mds_column_mult(sbox(0, (sbox(0, sbox(1, tmpx1 ^ S[1]) ^ S[5]) ^ S[9])), 1);
			skey->twofishkey.S[2][x] = mds_column_mult(sbox(1, (sbox(1, sbox(0, tmpx0 ^ S[2]) ^ S[6]) ^ S[10])), 2);
			skey->twofishkey.S[3][x] = mds_column_mult(sbox(0, (sbox(1, sbox(1, tmpx0 ^ S[3]) ^ S[7]) ^ S[11])), 3);
		}
	}
	else 
	{
		for (x = 0; x < 256; x++) 
		{
			tmpx0 = (byte)sbox(0, x);
			tmpx1 = (byte)sbox(1, x);
			skey->twofishkey.S[0][x] = mds_column_mult(sbox(1, (sbox(0, sbox(0, sbox(1, tmpx1 ^ S[0]) ^ S[4]) ^ S[8]) ^ S[12])), 0);
			skey->twofishkey.S[1][x] = mds_column_mult(sbox(0, (sbox(0, sbox(1, sbox(1, tmpx0 ^ S[1]) ^ S[5]) ^ S[9]) ^ S[13])), 1);
			skey->twofishkey.S[2][x] = mds_column_mult(sbox(1, (sbox(1, sbox(0, sbox(0, tmpx0 ^ S[2]) ^ S[6]) ^ S[10]) ^ S[14])), 2);
			skey->twofishkey.S[3][x] = mds_column_mult(sbox(0, (sbox(1, sbox(1, sbox(0, tmpx1 ^ S[3]) ^ S[7]) ^ S[11]) ^ S[15])), 3);
		}
	}
}

void TwofishEncrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey)
{
	dword a;
	dword b;
	dword c;
	dword d;
	dword ta;
	dword tb;
	dword tc;
	dword td;
	dword t1;
	dword t2;
	DWPointer k;
	int r;
#ifndef __GNUC__
	DWPointer S1;
	DWPointer S2;
	DWPointer S3;
	DWPointer S4;
#endif    

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));

#ifndef __GNUC__
	S1 = skey->twofishkey.S[0];
	S2 = skey->twofishkey.S[1];
	S3 = skey->twofishkey.S[2];
	S4 = skey->twofishkey.S[3];
#endif    

	LOAD32L(a, &pt[0]); LOAD32L(b, &pt[4]);
	LOAD32L(c, &pt[8]); LOAD32L(d, &pt[12]);
	a ^= skey->twofishkey.K[0];
	b ^= skey->twofishkey.K[1];
	c ^= skey->twofishkey.K[2];
	d ^= skey->twofishkey.K[3];

	k = skey->twofishkey.K + 8;
	for (r = 8; r != 0; --r)
	{
		t2 = g1_func(b, skey);
		t1 = g_func(a, skey) + t2;
		c = RORc(c ^ (t1 + k[0]), 1);
		d = ROLc(d, 1) ^ (t2 + t1 + k[1]);

		t2 = g1_func(d, skey);
		t1 = g_func(c, skey) + t2;
		a = RORc(a ^ (t1 + k[2]), 1);
		b = ROLc(b, 1) ^ (t2 + t1 + k[3]);
		k += 4;
	}
	ta = c ^ skey->twofishkey.K[4];
	tb = d ^ skey->twofishkey.K[5];
	tc = a ^ skey->twofishkey.K[6];
	td = b ^ skey->twofishkey.K[7];
	STORE32L(ta, &ct[0]); STORE32L(tb, &ct[4]);
	STORE32L(tc, &ct[8]); STORE32L(td, &ct[12]);
}

void TwofishDecrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey)
{
	dword a;
	dword b;
	dword c;
	dword d;
	dword ta;
	dword tb;
	dword tc;
	dword td;
	dword t1;
	dword t2;
	DWPointer k;
	int r;
#ifndef __GNUC__
	DWPointer S1;
	DWPointer S2;
	DWPointer S3;
	DWPointer S4;
#endif    

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));

#ifndef __GNUC__
	S1 = skey->twofishkey.S[0];
	S2 = skey->twofishkey.S[1];
	S3 = skey->twofishkey.S[2];
	S4 = skey->twofishkey.S[3];
#endif    

	LOAD32L(ta, &ct[0]); LOAD32L(tb, &ct[4]);
	LOAD32L(tc, &ct[8]); LOAD32L(td, &ct[12]);
	a = tc ^ skey->twofishkey.K[6];
	b = td ^ skey->twofishkey.K[7];
	c = ta ^ skey->twofishkey.K[4];
	d = tb ^ skey->twofishkey.K[5];
	k = skey->twofishkey.K + 36;
	for (r = 8; r != 0; --r) 
	{
		t2 = g1_func(d, skey);
		t1 = g_func(c, skey) + t2;
		a = ROLc(a, 1) ^ (t1 + k[2]);
		b = RORc(b ^ (t2 + t1 + k[3]), 1);

		t2 = g1_func(b, skey);
		t1 = g_func(a, skey) + t2;
		c = ROLc(c, 1) ^ (t1 + k[0]);
		d = RORc(d ^ (t2 + t1 + k[1]), 1);
		k -= 4;
	}
	a ^= skey->twofishkey.K[0];
	b ^= skey->twofishkey.K[1];
	c ^= skey->twofishkey.K[2];
	d ^= skey->twofishkey.K[3];
	STORE32L(a, &pt[0]); STORE32L(b, &pt[4]);
	STORE32L(c, &pt[8]); STORE32L(d, &pt[12]);
}

void TwofishFinish(SYMMETRIC_KEY *skey)
{
}

/* $Source: /cvs/libtom/libtomcrypt/src/ciphers/twofish/twofish.c,v $ */
/* $Revision: 1.16 $ */
/* $Date: 2007/05/12 14:32:35 $ */
