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

/* AES implementation by Tom St Denis
*
* Derived from the Public Domain source code by

---
* rijndael-alg-fst.c
*
* @version 3.0 (December 2000)
*
* Optimised ANSI C code for the Rijndael cipher (now AES)
*
* @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
* @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
* @author Paulo Barreto <paulo.barreto@terra.com.br>
---
*/
/**
@file aes.c
Implementation of AES
*/
#include "cy_pch.h"
#include "cipher.h"
#include "aestab.h"

#ifdef OK_COMP_MSC
#define get_byte(x, n) ((byte)((x) >> (8 * (n))))
#else
#define get_byte(x, n) (((x) >> (8 * (n))) & 255)
#endif   

#ifdef OK_COMP_MSC
#pragma intrinsic(_lrotr)
#define RORc(x,n) _lrotr(x,n)
#else
#define RORc(x, y) ( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif

#define LOAD32H(x, y)                            \
	 { x = ((unsigned long)((y)[0] & 255) << 24) | \
	 ((unsigned long)((y)[1] & 255) << 16) | \
	 ((unsigned long)((y)[2] & 255) << 8) | \
	 ((unsigned long)((y)[3] & 255)); }

#define STORE32H(x, y)                                                                     \
	 { (y)[0] = (unsigned char)(((x) >> 24) & 255); (y)[1] = (unsigned char)(((x) >> 16) & 255);   \
	 (y)[2] = (unsigned char)(((x) >> 8) & 255); (y)[3] = (unsigned char)((x)& 255); }

static dword setup_mix(dword temp)
{
	return (Te4_3[get_byte(temp, 2)]) ^
		(Te4_2[get_byte(temp, 1)]) ^
		(Te4_1[get_byte(temp, 0)]) ^
		(Te4_0[get_byte(temp, 3)]);
}

void AESSetup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey)
{
	int i, j;
	dword temp;
	DWPointer rk;
	DWPointer rrk;

	assert(NotPtrCheck(key));
	assert(NotPtrCheck(skey));
	assert((keylen == 16) || (keylen == 24) || (keylen == 32));
	assert((num_rounds == 0) || (num_rounds == (10 + ((keylen / 8) - 2) * 2)));

	skey->aeskey.Nr = 10 + ((keylen / 8) - 2) * 2;

	/* setup the forward key */
	i = 0;
	rk = skey->aeskey.eK;
	LOAD32H(rk[0], key);
	LOAD32H(rk[1], key + 4);
	LOAD32H(rk[2], key + 8);
	LOAD32H(rk[3], key + 12);
	if (keylen == 16) {
		j = 44;
		for (;;) {
			temp = rk[3];
			rk[4] = rk[0] ^ setup_mix(temp) ^ rcon[i];
			rk[5] = rk[1] ^ rk[4];
			rk[6] = rk[2] ^ rk[5];
			rk[7] = rk[3] ^ rk[6];
			if (++i == 10) {
				break;
			}
			rk += 4;
		}
	}
	else if (keylen == 24) {
		j = 52;
		LOAD32H(rk[4], key + 16);
		LOAD32H(rk[5], key + 20);
		for (;;) {
#ifdef OK_COMP_MSC
			temp = skey->aeskey.eK[rk - skey->aeskey.eK + 5];
#else
			temp = rk[5];
#endif
			rk[6] = rk[0] ^ setup_mix(temp) ^ rcon[i];
			rk[7] = rk[1] ^ rk[6];
			rk[8] = rk[2] ^ rk[7];
			rk[9] = rk[3] ^ rk[8];
			if (++i == 8) {
				break;
			}
			rk[10] = rk[4] ^ rk[9];
			rk[11] = rk[5] ^ rk[10];
			rk += 6;
		}
	}
	else if (keylen == 32) {
		j = 60;
		LOAD32H(rk[4], key + 16);
		LOAD32H(rk[5], key + 20);
		LOAD32H(rk[6], key + 24);
		LOAD32H(rk[7], key + 28);
		for (;;) {
#ifdef OK_COMP_MSC
			temp = skey->aeskey.eK[rk - skey->aeskey.eK + 7];
#else
			temp = rk[7];
#endif
			rk[8] = rk[0] ^ setup_mix(temp) ^ rcon[i];
			rk[9] = rk[1] ^ rk[8];
			rk[10] = rk[2] ^ rk[9];
			rk[11] = rk[3] ^ rk[10];
			if (++i == 7) {
				break;
			}
			temp = rk[11];
			rk[12] = rk[4] ^ setup_mix(RORc(temp, 8));
			rk[13] = rk[5] ^ rk[12];
			rk[14] = rk[6] ^ rk[13];
			rk[15] = rk[7] ^ rk[14];
			rk += 8;
		}
	}

	/* setup the inverse key now */
	rk = skey->aeskey.dK;
	rrk = skey->aeskey.eK + j - 4;

	/* apply the inverse MixColumn transform to all round keys but the first and the last: */
	/* copy first */
	*rk++ = *rrk++;
	*rk++ = *rrk++;
	*rk++ = *rrk++;
	*rk = *rrk;
	rk -= 3; rrk -= 3;

	for (i = 1; i < skey->aeskey.Nr; i++) {
		rrk -= 4;
		rk += 4;
		temp = rrk[0];
		rk[0] =
			Tks0[get_byte(temp, 3)] ^
			Tks1[get_byte(temp, 2)] ^
			Tks2[get_byte(temp, 1)] ^
			Tks3[get_byte(temp, 0)];
		temp = rrk[1];
		rk[1] =
			Tks0[get_byte(temp, 3)] ^
			Tks1[get_byte(temp, 2)] ^
			Tks2[get_byte(temp, 1)] ^
			Tks3[get_byte(temp, 0)];
		temp = rrk[2];
		rk[2] =
			Tks0[get_byte(temp, 3)] ^
			Tks1[get_byte(temp, 2)] ^
			Tks2[get_byte(temp, 1)] ^
			Tks3[get_byte(temp, 0)];
		temp = rrk[3];
		rk[3] =
			Tks0[get_byte(temp, 3)] ^
			Tks1[get_byte(temp, 2)] ^
			Tks2[get_byte(temp, 1)] ^
			Tks3[get_byte(temp, 0)];
	}

	/* copy last */
	rrk -= 4;
	rk += 4;
	*rk++ = *rrk++;
	*rk++ = *rrk++;
	*rk++ = *rrk++;
	*rk = *rrk;
}

/**
Encrypts a block of text with AES
@param pt The input plaintext (16 bytes)
@param ct The output ciphertext (16 bytes)
@param skey The key as scheduled
@return CRYPT_OK if successful
*/
void AESEncrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey)
{
	dword s0;
	dword s1;
	dword s2;
	dword s3;
	dword t0;
	dword t1;
	dword t2;
	dword t3;
	DWPointer rk;
	int Nr;
	int r;

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));

	Nr = skey->aeskey.Nr;
	rk = skey->aeskey.eK;

	/*
	* map get_byte array block to cipher state
	* and add initial round key:
	*/
	LOAD32H(s0, pt); s0 ^= rk[0];
	LOAD32H(s1, pt + 4); s1 ^= rk[1];
	LOAD32H(s2, pt + 8); s2 ^= rk[2];
	LOAD32H(s3, pt + 12); s3 ^= rk[3];

	/*
	* Nr - 1 full rounds:
	*/
	r = Nr >> 1;
	for (;;) {
		t0 =
			Te0(get_byte(s0, 3)) ^
			Te1(get_byte(s1, 2)) ^
			Te2(get_byte(s2, 1)) ^
			Te3(get_byte(s3, 0)) ^
			rk[4];
		t1 =
			Te0(get_byte(s1, 3)) ^
			Te1(get_byte(s2, 2)) ^
			Te2(get_byte(s3, 1)) ^
			Te3(get_byte(s0, 0)) ^
			rk[5];
		t2 =
			Te0(get_byte(s2, 3)) ^
			Te1(get_byte(s3, 2)) ^
			Te2(get_byte(s0, 1)) ^
			Te3(get_byte(s1, 0)) ^
			rk[6];
		t3 =
			Te0(get_byte(s3, 3)) ^
			Te1(get_byte(s0, 2)) ^
			Te2(get_byte(s1, 1)) ^
			Te3(get_byte(s2, 0)) ^
			rk[7];

		rk += 8;
		if (--r == 0) {
			break;
		}

		s0 =
			Te0(get_byte(t0, 3)) ^
			Te1(get_byte(t1, 2)) ^
			Te2(get_byte(t2, 1)) ^
			Te3(get_byte(t3, 0)) ^
			rk[0];
		s1 =
			Te0(get_byte(t1, 3)) ^
			Te1(get_byte(t2, 2)) ^
			Te2(get_byte(t3, 1)) ^
			Te3(get_byte(t0, 0)) ^
			rk[1];
		s2 =
			Te0(get_byte(t2, 3)) ^
			Te1(get_byte(t3, 2)) ^
			Te2(get_byte(t0, 1)) ^
			Te3(get_byte(t1, 0)) ^
			rk[2];
		s3 =
			Te0(get_byte(t3, 3)) ^
			Te1(get_byte(t0, 2)) ^
			Te2(get_byte(t1, 1)) ^
			Te3(get_byte(t2, 0)) ^
			rk[3];
	}

	/*
	* apply last round and
	* map cipher state to get_byte array block:
	*/
	s0 =
		(Te4_3[get_byte(t0, 3)]) ^
		(Te4_2[get_byte(t1, 2)]) ^
		(Te4_1[get_byte(t2, 1)]) ^
		(Te4_0[get_byte(t3, 0)]) ^
		rk[0];
	STORE32H(s0, ct);
	s1 =
		(Te4_3[get_byte(t1, 3)]) ^
		(Te4_2[get_byte(t2, 2)]) ^
		(Te4_1[get_byte(t3, 1)]) ^
		(Te4_0[get_byte(t0, 0)]) ^
		rk[1];
	STORE32H(s1, ct + 4);
	s2 =
		(Te4_3[get_byte(t2, 3)]) ^
		(Te4_2[get_byte(t3, 2)]) ^
		(Te4_1[get_byte(t0, 1)]) ^
		(Te4_0[get_byte(t1, 0)]) ^
		rk[2];
	STORE32H(s2, ct + 8);
	s3 =
		(Te4_3[get_byte(t3, 3)]) ^
		(Te4_2[get_byte(t0, 2)]) ^
		(Te4_1[get_byte(t1, 1)]) ^
		(Te4_0[get_byte(t2, 0)]) ^
		rk[3];
	STORE32H(s3, ct + 12);
}

/**
Decrypts a block of text with AES
@param ct The input ciphertext (16 bytes)
@param pt The output plaintext (16 bytes)
@param skey The key as scheduled
@return CRYPT_OK if successful
*/
void AESDecrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey)
{
	dword s0;
	dword s1;
	dword s2;
	dword s3;
	dword t0;
	dword t1;
	dword t2;
	dword t3;
	DWPointer rk;
	int Nr;
	int r;

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));

	Nr = skey->aeskey.Nr;
	rk = skey->aeskey.dK;

	/*
	* map get_byte array block to cipher state
	* and add initial round key:
	*/
	LOAD32H(s0, ct); s0 ^= rk[0];
	LOAD32H(s1, ct + 4); s1 ^= rk[1];
	LOAD32H(s2, ct + 8); s2 ^= rk[2];
	LOAD32H(s3, ct + 12); s3 ^= rk[3];

	/*
	* Nr - 1 full rounds:
	*/
	r = Nr >> 1;
	for (;;) {

		t0 =
			Td0(get_byte(s0, 3)) ^
			Td1(get_byte(s3, 2)) ^
			Td2(get_byte(s2, 1)) ^
			Td3(get_byte(s1, 0)) ^
			rk[4];
		t1 =
			Td0(get_byte(s1, 3)) ^
			Td1(get_byte(s0, 2)) ^
			Td2(get_byte(s3, 1)) ^
			Td3(get_byte(s2, 0)) ^
			rk[5];
		t2 =
			Td0(get_byte(s2, 3)) ^
			Td1(get_byte(s1, 2)) ^
			Td2(get_byte(s0, 1)) ^
			Td3(get_byte(s3, 0)) ^
			rk[6];
		t3 =
			Td0(get_byte(s3, 3)) ^
			Td1(get_byte(s2, 2)) ^
			Td2(get_byte(s1, 1)) ^
			Td3(get_byte(s0, 0)) ^
			rk[7];

		rk += 8;
		if (--r == 0) {
			break;
		}


		s0 =
			Td0(get_byte(t0, 3)) ^
			Td1(get_byte(t3, 2)) ^
			Td2(get_byte(t2, 1)) ^
			Td3(get_byte(t1, 0)) ^
			rk[0];
		s1 =
			Td0(get_byte(t1, 3)) ^
			Td1(get_byte(t0, 2)) ^
			Td2(get_byte(t3, 1)) ^
			Td3(get_byte(t2, 0)) ^
			rk[1];
		s2 =
			Td0(get_byte(t2, 3)) ^
			Td1(get_byte(t1, 2)) ^
			Td2(get_byte(t0, 1)) ^
			Td3(get_byte(t3, 0)) ^
			rk[2];
		s3 =
			Td0(get_byte(t3, 3)) ^
			Td1(get_byte(t2, 2)) ^
			Td2(get_byte(t1, 1)) ^
			Td3(get_byte(t0, 0)) ^
			rk[3];
	}
	/*
	* apply last round and
	* map cipher state to get_byte array block:
	*/
	s0 =
		(Td4[get_byte(t0, 3)] & 0xff000000) ^
		(Td4[get_byte(t3, 2)] & 0x00ff0000) ^
		(Td4[get_byte(t2, 1)] & 0x0000ff00) ^
		(Td4[get_byte(t1, 0)] & 0x000000ff) ^
		rk[0];
	STORE32H(s0, pt);
	s1 =
		(Td4[get_byte(t1, 3)] & 0xff000000) ^
		(Td4[get_byte(t0, 2)] & 0x00ff0000) ^
		(Td4[get_byte(t3, 1)] & 0x0000ff00) ^
		(Td4[get_byte(t2, 0)] & 0x000000ff) ^
		rk[1];
	STORE32H(s1, pt + 4);
	s2 =
		(Td4[get_byte(t2, 3)] & 0xff000000) ^
		(Td4[get_byte(t1, 2)] & 0x00ff0000) ^
		(Td4[get_byte(t0, 1)] & 0x0000ff00) ^
		(Td4[get_byte(t3, 0)] & 0x000000ff) ^
		rk[2];
	STORE32H(s2, pt + 8);
	s3 =
		(Td4[get_byte(t3, 3)] & 0xff000000) ^
		(Td4[get_byte(t2, 2)] & 0x00ff0000) ^
		(Td4[get_byte(t1, 1)] & 0x0000ff00) ^
		(Td4[get_byte(t0, 0)] & 0x000000ff) ^
		rk[3];
	STORE32H(s3, pt + 12);
}

/** Terminate the context
@param skey    The scheduled key
*/
void AESFinish(SYMMETRIC_KEY *skey)
{
}

/* $Source: /cvs/libtom/libtomcrypt/src/ciphers/aes/aes.c,v $ */
/* $Revision: 1.16 $ */
/* $Date: 2007/05/12 14:13:00 $ */
