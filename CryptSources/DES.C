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
@file des.c
LTC_DES code submitted by Dobes Vandermeer
*/
#include "CY_PCH.H"
#include "CIPHER.H"
#include "DES.H"

#ifdef OK_COMP_MSC
#define get_byte(x, n) ((byte)((x) >> (8 * (n))))
#else
#define get_byte(x, n) (((x) >> (8 * (n))) & 255)
#endif 

#define LOAD32H(x, y)                            \
	 { x = ((dword)((y)[0] & 255) << 24) | \
	 ((dword)((y)[1] & 255) << 16) | \
	 ((dword)((y)[2] & 255) << 8) | \
	 ((dword)((y)[3] & 255)); }

#define STORE32H(x, y)                                                                     \
	 { (y)[0] = (byte)(((x) >> 24) & 255); (y)[1] = (byte)(((x) >> 16) & 255);   \
	 (y)[2] = (byte)(((x) >> 8) & 255); (y)[3] = (byte)((x)& 255); }

#ifdef OK_COMP_MSC
#pragma intrinsic(_lrotr)
#define RORc(x,n) _lrotr(x,n)
#else
#define RORc(x, y) ( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif

static void cookey(DWConstPointer raw1, DWPointer keyout);

static void deskey(BConstPointer key, short edf, DWPointer keyout)
{
	dword i;
	dword j;
	dword l;
	dword m;
	dword n;
	dword kn[32];
	byte pc1m[56];
	byte pcr[56];

	for (j = 0; j < 56; j++) 
	{
		l = (dword)pc1[j];
		m = l & 7;
		pc1m[j] = (byte)((key[l >> 3U] & bytebit[m]) == bytebit[m] ? 1 : 0);
	}
	for (i = 0; i < 16; i++) 
	{
		m = (edf == DE1) ? ((15 - i) << 1) : (i << 1);
		n = m + 1;
		kn[m] = kn[n] = 0L;
		for (j = 0; j < 28; j++) 
		{
			l = j + (dword)totrot[i];
			pcr[j] = (l < 28) ? (pc1m[l]) : (pc1m[l - 28]);
		}
		for (; j < 56; j++) 
		{
			l = j + (dword)totrot[i];
			pcr[j] = (l < 56) ? (pc1m[l]) : (pc1m[l - 28]);
		}
		for (j = 0; j < 24; j++)  
		{
			if ((int)pcr[(int)pc2[j]] != 0)
				kn[m] |= bigbyte[j];
			if ((int)pcr[(int)pc2[j + 24]] != 0)
				kn[n] |= bigbyte[j];
		}
	}
	cookey(kn, keyout);
}

static void cookey(DWConstPointer raw1, DWPointer keyout)
{
	DWPointer cook;
	DWConstPointer raw0;
	dword dough[32];
	int i;

	cook = dough;
	for (i = 0; i < 16; i++, raw1++)
	{
		raw0 = raw1++;
		*cook = (*raw0 & 0x00fc0000L) << 6;
		*cook |= (*raw0 & 0x00000fc0L) << 10;
		*cook |= (*raw1 & 0x00fc0000L) >> 10;
		*cook++ |= (*raw1 & 0x00000fc0L) >> 6;
		*cook = (*raw0 & 0x0003f000L) << 12;
		*cook |= (*raw0 & 0x0000003fL) << 16;
		*cook |= (*raw1 & 0x0003f000L) >> 4;
		*cook++ |= (*raw1 & 0x0000003fL);
	}

	s_memcpy(keyout, dough, 32 * 4);
}

static void desfunc(DWPointer block, DWConstPointer keys)
{
	dword work;
	dword right;
	dword leftt;
	int cur_round;
	qword tmp;

	leftt = block[0];
	right = block[1];

	tmp = des_ip[0][get_byte(leftt, 0)] ^
		des_ip[1][get_byte(leftt, 1)] ^
		des_ip[2][get_byte(leftt, 2)] ^
		des_ip[3][get_byte(leftt, 3)] ^
		des_ip[4][get_byte(right, 0)] ^
		des_ip[5][get_byte(right, 1)] ^
		des_ip[6][get_byte(right, 2)] ^
		des_ip[7][get_byte(right, 3)];
	leftt = (dword)(tmp >> 32);
	right = (dword)(tmp & 0xFFFFFFFFUL);

	for (cur_round = 0; cur_round < 8; cur_round++)
	{
		work = RORc(right, 4) ^ *keys++;
		leftt ^= SP7[work & 0x3fL]
			^ SP5[(work >> 8) & 0x3fL]
			^ SP3[(work >> 16) & 0x3fL]
			^ SP1[(work >> 24) & 0x3fL];
		work = right ^ *keys++;
		leftt ^= SP8[work & 0x3fL]
			^ SP6[(work >> 8) & 0x3fL]
			^ SP4[(work >> 16) & 0x3fL]
			^ SP2[(work >> 24) & 0x3fL];

		work = RORc(leftt, 4) ^ *keys++;
		right ^= SP7[work & 0x3fL]
			^ SP5[(work >> 8) & 0x3fL]
			^ SP3[(work >> 16) & 0x3fL]
			^ SP1[(work >> 24) & 0x3fL];
		work = leftt ^ *keys++;
		right ^= SP8[work & 0x3fL]
			^ SP6[(work >> 8) & 0x3fL]
			^ SP4[(work >> 16) & 0x3fL]
			^ SP2[(work >> 24) & 0x3fL];
	}

	tmp = des_fp[0][get_byte(leftt, 0)] ^
		des_fp[1][get_byte(leftt, 1)] ^
		des_fp[2][get_byte(leftt, 2)] ^
		des_fp[3][get_byte(leftt, 3)] ^
		des_fp[4][get_byte(right, 0)] ^
		des_fp[5][get_byte(right, 1)] ^
		des_fp[6][get_byte(right, 2)] ^
		des_fp[7][get_byte(right, 3)];
	leftt = (dword)(tmp >> 32);
	right = (dword)(tmp & 0xFFFFFFFFUL);

	block[0] = right;
	block[1] = leftt;
}

void DESSetup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey)
{
	assert(NotPtrCheck(key));
	assert(NotPtrCheck(skey));
	assert((num_rounds == 0) || (num_rounds == 16));
	assert(keylen == 8);

	deskey(key, EN0, skey->deskey.ek);
	deskey(key, DE1, skey->deskey.dk);
}

void DES3Setup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey)
{
	assert(NotPtrCheck(key));
	assert(NotPtrCheck(skey));
	assert((num_rounds == 0) || (num_rounds == 16));
	assert(keylen == 24);

	deskey(key, EN0, skey->des3key.ek[0]);
	deskey(key + 8, DE1, skey->des3key.ek[1]);
	deskey(key + 16, EN0, skey->des3key.ek[2]);

	deskey(key, DE1, skey->des3key.dk[2]);
	deskey(key + 8, EN0, skey->des3key.dk[1]);
	deskey(key + 16, DE1, skey->des3key.dk[0]);
}

void DESEncrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey)
{
	dword work[2];

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));

	LOAD32H(work[0], pt + 0);
	LOAD32H(work[1], pt + 4);
	desfunc(work, skey->deskey.ek);
	STORE32H(work[0], ct + 0);
	STORE32H(work[1], ct + 4);
}

void DESDecrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey)
{
	dword work[2];

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));

	LOAD32H(work[0], ct + 0);
	LOAD32H(work[1], ct + 4);
	desfunc(work, skey->deskey.dk);
	STORE32H(work[0], pt + 0);
	STORE32H(work[1], pt + 4);
}

void DES3Encrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey)
{
	dword work[2];

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));

	LOAD32H(work[0], pt + 0);
	LOAD32H(work[1], pt + 4);
	desfunc(work, skey->des3key.ek[0]);
	desfunc(work, skey->des3key.ek[1]);
	desfunc(work, skey->des3key.ek[2]);
	STORE32H(work[0], ct + 0);
	STORE32H(work[1], ct + 4);
}

void DES3Decrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey)
{
	dword work[2];

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));

	LOAD32H(work[0], ct + 0);
	LOAD32H(work[1], ct + 4);
	desfunc(work, skey->des3key.dk[0]);
	desfunc(work, skey->des3key.dk[1]);
	desfunc(work, skey->des3key.dk[2]);
	STORE32H(work[0], pt + 0);
	STORE32H(work[1], pt + 4);
}

void DESFinish(SYMMETRIC_KEY *skey)
{
}

void DES3Finish(SYMMETRIC_KEY *skey)
{
}

/* $Source: /cvs/libtom/libtomcrypt/src/ciphers/des.c,v $ */
/* $Revision: 1.15 $ */
/* $Date: 2007/05/12 14:20:27 $ */
