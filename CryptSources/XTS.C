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
#include "CY_PCH.H"
#include "CIPHER.H"

static void xts_mult_x(unsigned char *I)
{
	int x;
	byte t;
	byte tt;

	for (x = t = 0; x < 16; x++) 
	{
		tt = I[x] >> 7;
		I[x] = ((I[x] << 1) | t) & 0xFF;
		t = tt;
	}
	if (tt)
		I[0] ^= 0x87;
}

static void tweak_crypt(BConstPointer P, BPointer C, BPointer T, SYMMETRIC_KEY_XTS *xts)
{
	int x;

	for (x = 0; x < 16; x++)
		C[x] = P[x] ^ T[x];
	xts->Encrypt(C, C, &(xts->key1));
	for (x = 0; x < 16; x++)
		C[x] ^= T[x];
	xts_mult_x(T);
}

static void tweak_uncrypt(BConstPointer C, BPointer P, BPointer T, SYMMETRIC_KEY_XTS *xts)
{
	int x;

	for (x = 0; x < 16; x++)
		P[x] = C[x] ^ T[x];
	xts->Decrypt(P, P, &(xts->key1));
	for (x = 0; x < 16; x++)
		P[x] ^= T[x];
	xts_mult_x(T);
}

void XTSSetup(BConstPointer key, int keylen, int num_rounds,
	void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey),
	void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
	void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
	void(*Finish)(SYMMETRIC_KEY *skey),
	SYMMETRIC_KEY_XTS *skey)
{
	assert(NotPtrCheck(Setup));
	assert(NotPtrCheck(Encrypt));
	assert(NotPtrCheck(Decrypt));
	assert(NotPtrCheck(Finish));
	assert(NotPtrCheck(skey));

	skey->Setup = Setup;
	skey->Encrypt = Encrypt;
	skey->Decrypt = Decrypt;
	skey->Finish = Finish;

	Setup(key, keylen / 2, num_rounds, &(skey->key1));
	Setup(key + (keylen / 2), keylen / 2, num_rounds, &(skey->key2));
}

void XTSEncrypt(BConstPointer pt, BPointer ct, dword ptlen, BConstPointer tweak, SYMMETRIC_KEY_XTS *skey)
{
	dword i;
	dword m;
	dword mo;
	dword lim;
	byte PP[16];
	byte CC[16];
	byte T[16];

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(tweak));
	assert(NotPtrCheck(skey));

	m = ptlen >> 4;
	mo = ptlen & 15;

	assert(m > 0);

	skey->Encrypt(tweak, T, &(skey->key2));

	lim = (mo == 0) ? m : (m - 1);
	for (i = 0; i < lim; i++) 
	{
		tweak_crypt(pt, ct, T, skey);
		ct += 16;
		pt += 16;
	}
	if (mo > 0) 
	{
		tweak_crypt(pt, CC, T, skey);
		for (i = 0; i < mo; i++) 
		{
			PP[i] = pt[16 + i];
			ct[16 + i] = CC[i];
		}
		for (; i < 16; i++)
			PP[i] = CC[i];
		tweak_crypt(PP, ct, T, skey);
	}
}

void XTSDecrypt(BConstPointer ct, BPointer pt, dword ptlen, BConstPointer tweak, SYMMETRIC_KEY_XTS *skey)
{
	dword i;
	dword m;
	dword mo;
	dword lim;
	byte PP[16];
	byte CC[16];
	byte T[16];

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(tweak));
	assert(NotPtrCheck(skey));

	m = ptlen >> 4;
	mo = ptlen & 15;

	assert(m > 0);

	skey->Encrypt(tweak, T, &(skey->key2));
	lim = (mo == 0) ? m : (m - 1);
	for (i = 0; i < lim; i++) 
	{
		tweak_uncrypt(ct, pt, T, skey);
		ct += 16;
		pt += 16;
	}
	if (mo > 0) 
	{
		s_memcpy(CC, T, 16);
		xts_mult_x(CC);
		tweak_uncrypt(ct, PP, CC, skey);
		for (i = 0; i < mo; i++) 
		{
			CC[i] = ct[16 + i];
			pt[16 + i] = PP[i];
		}
		for (; i < 16; i++)
			CC[i] = PP[i];
		tweak_uncrypt(CC, pt, T, skey);
	}
}

void XTSFinish(SYMMETRIC_KEY_XTS *skey)
{
	assert(NotPtrCheck(skey));

	skey->Finish(&(skey->key1));
	skey->Finish(&(skey->key2));
}

