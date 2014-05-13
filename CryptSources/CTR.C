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

void CTRSetup(BConstPointer IV, int blocklen, BConstPointer key, int keylen, int num_rounds, int ctr_mode,
	void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey),
	void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
	void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
	void(*Finish)(SYMMETRIC_KEY *skey),
	SYMMETRIC_KEY_CTR *skey)
{
	int x;

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

	skey->ctrlen = (ctr_mode & 255) ? (ctr_mode & 255) : blocklen;
	assert(skey->ctrlen <= blocklen);

	if ((ctr_mode & CTR_COUNTER_BIG_ENDIAN) == CTR_COUNTER_BIG_ENDIAN)
		skey->ctrlen = blocklen - skey->ctrlen;

	Setup(key, keylen, num_rounds, &(skey->key));

	skey->padlen = 0;
	skey->mode = ctr_mode & 0x1000;
	for (x = 0; x < skey->blocklen; x++) {
		skey->ctr[x] = IV[x];
	}

	if (ctr_mode & CTR_COUNTER_RFC3686)
	{
		if (skey->mode == CTR_COUNTER_LITTLE_ENDIAN) 
		{
			for (x = 0; x < skey->ctrlen; x++) 
			{
				skey->ctr[x] = (skey->ctr[x] + (byte)1) & (byte)255;
				if (skey->ctr[x] != (byte)0)
					break;
			}
		}
		else 
		{
			for (x = skey->blocklen - 1; x >= skey->ctrlen; x--) 
			{
				skey->ctr[x] = (skey->ctr[x] + (byte)1) & (byte)255;
				if (skey->ctr[x] != (byte)0)
					break;
			}
		}
	}
	Encrypt(skey->ctr, skey->pad, &(skey->key));
}

void CTRSetIV(BConstPointer IV, int blocklen, SYMMETRIC_KEY_CTR *skey)
{
	assert(NotPtrCheck(IV));
	assert(NotPtrCheck(skey));
	assert(blocklen == skey->blocklen);

	s_memcpy(skey->ctr, IV, blocklen);
	skey->padlen = 0;
	skey->Encrypt(IV, skey->pad, &(skey->key));
}

void CTRGetIV(BPointer IV, int blocklen, SYMMETRIC_KEY_CTR *skey)
{
	assert(NotPtrCheck(IV));
	assert(NotPtrCheck(skey));
	assert(blocklen == skey->blocklen);

	s_memcpy(IV, skey->ctr, blocklen);
}

void CTREncrypt(BConstPointer pt, BPointer ct, dword len, SYMMETRIC_KEY_CTR *skey)
{
	int x;

	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));
	assert((len % skey->blocklen) == 0);

	while (len)
	{
		if (skey->padlen == skey->blocklen)
		{
			if (skey->mode == CTR_COUNTER_LITTLE_ENDIAN) 
			{
				for (x = 0; x < skey->ctrlen; x++)
				{
					skey->ctr[x] = (skey->ctr[x] + (unsigned char)1) & (unsigned char)255;
					if (skey->ctr[x] != (unsigned char)0)
						break;
				}
			}
			else 
			{
				for (x = skey->blocklen - 1; x >= skey->ctrlen; x--) 
				{
					skey->ctr[x] = (skey->ctr[x] + (unsigned char)1) & (unsigned char)255;
					if (skey->ctr[x] != (unsigned char)0)
						break;
				}
			}
			skey->Encrypt(skey->ctr, skey->pad, &(skey->key));
			skey->padlen = 0;
		}
		*ct++ = *pt++ ^ skey->pad[(skey->padlen)++];
		--len;
	}
}

void CTRDecrypt(BConstPointer ct, BPointer pt, dword len, SYMMETRIC_KEY_CTR *skey)
{
	assert(NotPtrCheck(pt));
	assert(NotPtrCheck(ct));
	assert(NotPtrCheck(skey));
	assert((len % skey->blocklen) == 0);

	CTREncrypt(ct, pt, len, skey);
}

void CTRFinish(SYMMETRIC_KEY_CTR *skey)
{
	assert(NotPtrCheck(skey));

	skey->Finish(&(skey->key));
}

