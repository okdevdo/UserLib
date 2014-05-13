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

void CFBSetup(BConstPointer IV, int blocklen, BConstPointer key, int keylen, int num_rounds,
	void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey),
	void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
	void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
	void(*Finish)(SYMMETRIC_KEY *skey),
	SYMMETRIC_KEY_CFB *skey)
{
	assert(NotPtrCheck(IV));
	assert(NotPtrCheck(Setup));
	assert(NotPtrCheck(Encrypt));
	assert(NotPtrCheck(Decrypt));
	assert(NotPtrCheck(Finish));
	assert(NotPtrCheck(skey));

	skey->Setup = Setup;
	skey->Encrypt = Encrypt;
	skey->Decrypt = Decrypt;
	skey->Finish = Finish;
	skey->blocklen = blocklen;
	skey->padlen = 0;
	Setup(key, keylen, num_rounds, &(skey->key));
	Encrypt(IV, skey->IV, &(skey->key));
}

void CFBSetIV(BConstPointer IV, int blocklen, SYMMETRIC_KEY_CFB *skey)
{
	assert(NotPtrCheck(IV));
	assert(NotPtrCheck(skey));
	assert(blocklen == skey->blocklen);

	skey->padlen = 0;
	skey->Encrypt(IV, skey->IV, &(skey->key));
}

void CFBGetIV(BPointer IV, int blocklen, SYMMETRIC_KEY_CFB *skey)
{
	assert(NotPtrCheck(IV));
	assert(NotPtrCheck(skey));
	assert(blocklen == skey->blocklen);

	s_memcpy(IV, skey->IV, blocklen);
}

void CFBEncrypt(BConstPointer pt, BPointer ct, dword len, SYMMETRIC_KEY_CFB *skey)
{
	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));
	assert((len % skey->blocklen) == 0);

	while (len-- > 0) 
	{
		if (skey->padlen == skey->blocklen) 
		{
			skey->Encrypt(skey->pad, skey->IV, &(skey->key));
			skey->padlen = 0;
		}
		skey->pad[skey->padlen] = (*ct = *pt ^ skey->IV[skey->padlen]);
		++pt;
		++ct;
		++(skey->padlen);
	}
}

void CFBDecrypt(BConstPointer ct, BPointer pt, dword len, SYMMETRIC_KEY_CFB *skey)
{
	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));
	assert((len % skey->blocklen) == 0);

	while (len-- > 0)
	{
		if (skey->padlen == skey->blocklen)
		{
			skey->Encrypt(skey->pad, skey->IV, &(skey->key));
			skey->padlen = 0;
		}
		skey->pad[skey->padlen] = *ct;
		*pt = *ct ^ skey->IV[skey->padlen];
		++pt;
		++ct;
		++(skey->padlen);
	}

}

void CFBFinish(SYMMETRIC_KEY_CFB *skey)
{
	assert(NotPtrCheck(skey));

	skey->Finish(&(skey->key));
}

