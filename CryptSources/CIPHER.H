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
#ifndef __CIPHER_H__
#define __CIPHER_H__

#include "CYSOURCES.H"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		dword ek[32];
		dword dk[32];
	} DES_KEY;

	typedef struct {
		dword ek[3][32];
		dword dk[3][32];
	} DES3_KEY;

	typedef struct {
		dword eK[60];
		dword dK[60];
		int Nr;
	} AES_KEY;

	typedef struct {
		dword S[4][256];
		dword K[18];
	} BLOWFISH_KEY;

	typedef struct twofish_key {
		dword S[4][256];
		dword K[40];
	} TWOFISH_KEY;

	typedef union {
		DES_KEY deskey;
		DES3_KEY des3key;
		AES_KEY aeskey;
		BLOWFISH_KEY blowfishkey;
		TWOFISH_KEY twofishkey;
	} SYMMETRIC_KEY;

	void DESSetup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
	void DESEncrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
	void DESDecrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey);
	void DESFinish(SYMMETRIC_KEY *skey);

	void DES3Setup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
	void DES3Encrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
	void DES3Decrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey);
	void DES3Finish(SYMMETRIC_KEY *skey);

	void AESSetup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
	void AESEncrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
	void AESDecrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey);
	void AESFinish(SYMMETRIC_KEY *skey);

	void BlowfishSetup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
	void BlowfishEncrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
	void BlowfishDecrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey);
	void BlowfishFinish(SYMMETRIC_KEY *skey);

	void TwofishSetup(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
	void TwofishEncrypt(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
	void TwofishDecrypt(BConstPointer ct, BPointer pt, SYMMETRIC_KEY *skey);
	void TwofishFinish(SYMMETRIC_KEY *skey);

	typedef struct {
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Finish)(SYMMETRIC_KEY *skey);
		int blocklen;
		byte IV[MAX_BLOCK_SIZE];
		SYMMETRIC_KEY key;
	} SYMMETRIC_KEY_CBC;

	void CBCSetup(BConstPointer IV, int blocklen, BConstPointer key, int keylen, int num_rounds, 
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey),
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Finish)(SYMMETRIC_KEY *skey),
		SYMMETRIC_KEY_CBC *skey);
	void CBCSetIV(BConstPointer IV, int blocklen, SYMMETRIC_KEY_CBC *skey);
	void CBCGetIV(BPointer IV, int blocklen, SYMMETRIC_KEY_CBC *skey);
	void CBCEncrypt(BConstPointer pt, BPointer ct, dword len, SYMMETRIC_KEY_CBC *skey);
	void CBCDecrypt(BConstPointer ct, BPointer pt, dword len, SYMMETRIC_KEY_CBC *skey);
	void CBCFinish(SYMMETRIC_KEY_CBC *skey);

	typedef struct {
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Finish)(SYMMETRIC_KEY *skey);
		int blocklen;
		int padlen;
		byte IV[MAX_BLOCK_SIZE];
		byte pad[MAX_BLOCK_SIZE];
		SYMMETRIC_KEY key;
	} SYMMETRIC_KEY_CFB;

	void CFBSetup(BConstPointer IV, int blocklen, BConstPointer key, int keylen, int num_rounds, 
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey),
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Finish)(SYMMETRIC_KEY *skey),
		SYMMETRIC_KEY_CFB *skey);
	void CFBSetIV(BConstPointer IV, int blocklen, SYMMETRIC_KEY_CFB *skey);
	void CFBGetIV(BPointer IV, int blocklen, SYMMETRIC_KEY_CFB *skey);
	void CFBEncrypt(BConstPointer pt, BPointer ct, dword len, SYMMETRIC_KEY_CFB *skey);
	void CFBDecrypt(BConstPointer ct, BPointer pt, dword len, SYMMETRIC_KEY_CFB *skey);
	void CFBFinish(SYMMETRIC_KEY_CFB *skey);

	typedef struct {
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Finish)(SYMMETRIC_KEY *skey);
		int blocklen;
		int padlen;
		byte IV[MAX_BLOCK_SIZE];
		SYMMETRIC_KEY key;
	} SYMMETRIC_KEY_OFB;

	void OFBSetup(BConstPointer IV, int blocklen, BConstPointer key, int keylen, int num_rounds, 
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey),
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Finish)(SYMMETRIC_KEY *skey),
		SYMMETRIC_KEY_OFB *skey);
	void OFBSetIV(BConstPointer IV, int blocklen, SYMMETRIC_KEY_OFB *skey);
	void OFBGetIV(BPointer IV, int blocklen, SYMMETRIC_KEY_OFB *skey);
	void OFBEncrypt(BConstPointer pt, BPointer ct, dword len, SYMMETRIC_KEY_OFB *skey);
	void OFBDecrypt(BConstPointer ct, BPointer pt, dword len, SYMMETRIC_KEY_OFB *skey);
	void OFBFinish(SYMMETRIC_KEY_OFB *skey);

	typedef struct {
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Finish)(SYMMETRIC_KEY *skey);
		int blocklen;
		int padlen;
		int mode;
		int ctrlen;
		byte ctr[MAX_BLOCK_SIZE];
		byte pad[MAX_BLOCK_SIZE];
		SYMMETRIC_KEY       key;
	} SYMMETRIC_KEY_CTR;

#define CTR_COUNTER_LITTLE_ENDIAN    0x0000
#define CTR_COUNTER_BIG_ENDIAN       0x1000
#define CTR_COUNTER_RFC3686          0x2000

	void CTRSetup(BConstPointer IV, int blocklen, BConstPointer key, int keylen, int num_rounds, int ctr_mode,
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey),
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Finish)(SYMMETRIC_KEY *skey),
		SYMMETRIC_KEY_CTR *skey);
	void CTRSetIV(BConstPointer IV, int blocklen, SYMMETRIC_KEY_CTR *skey);
	void CTRGetIV(BPointer IV, int blocklen, SYMMETRIC_KEY_CTR *skey);
	void CTREncrypt(BConstPointer pt, BPointer ct, dword len, SYMMETRIC_KEY_CTR *skey);
	void CTRDecrypt(BConstPointer ct, BPointer pt, dword len, SYMMETRIC_KEY_CTR *skey);
	void CTRFinish(SYMMETRIC_KEY_CTR *skey);

	typedef struct {
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey);
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey);
		void(*Finish)(SYMMETRIC_KEY *skey);
		SYMMETRIC_KEY key1;
		SYMMETRIC_KEY key2;
	} SYMMETRIC_KEY_XTS;

	void XTSSetup(BConstPointer key, int keylen, int num_rounds,
		void(*Setup)(BConstPointer key, int keylen, int num_rounds, SYMMETRIC_KEY *skey),
		void(*Encrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Decrypt)(BConstPointer pt, BPointer ct, SYMMETRIC_KEY *skey),
		void(*Finish)(SYMMETRIC_KEY *skey),
		SYMMETRIC_KEY_XTS *skey);
	void XTSEncrypt(BConstPointer pt, BPointer ct, dword ptlen, BConstPointer tweak, SYMMETRIC_KEY_XTS *skey);
	void XTSDecrypt(BConstPointer ct, BPointer pt, dword ptlen, BConstPointer tweak, SYMMETRIC_KEY_XTS *skey);
	void XTSFinish(SYMMETRIC_KEY_XTS *skey);

#ifdef __cplusplus
}
#endif

#endif
