/******************************************************************************
    
	This file is part of WinSources, which is part of UserLib.

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
#include "WS_PCH.H"
#include "WinCryptFilter.h"
#include "FindVersion.h"
#include <Wincrypt.h>
#include <bcrypt.h>

#define KEYLENGTH  0x00800000
#define ENCRYPT_ALGORITHM CALG_RC4 
#define ENCRYPT_BLOCK_SIZE 8 

static const BYTE rgbIV[] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

static void HandleError(BOOL cmd, DECL_FILE_LINE CConstPointer innerfunc, CConstPointer outerfunc)
{
	if (!cmd)
		throw OK_NEW_OPERATOR CWinCryptException(ARGS_FILE_LINE _T("[%s] in %s failed"), innerfunc, outerfunc, CWinException::WinExtError);
}

static void HandleError(NTSTATUS cmd, DECL_FILE_LINE CConstPointer innerfunc, CConstPointer outerfunc)
{
	if (!BCRYPT_SUCCESS(cmd))
		throw OK_NEW_OPERATOR CWinCryptException(ARGS_FILE_LINE _T("[%s] in %s failed"), innerfunc, outerfunc, CWinException::BCryptStatus, cmd);
}

IMPL_WINEXCEPTION(CWinCryptException, CWinException)

CDataVectorT<CStringBuffer> CWinCryptEncryptFilter::_algos(__FILE__LINE__ 16, 16);

CWinCryptEncryptFilter::CWinCryptEncryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
CFilter(input, output),
_passwd(),
_algo()
{
	ConstRef(CDataVectorT<CStringBuffer>) buf(get_Algorithms());
	CDataVectorT<CStringBuffer>::Iterator it = buf.Begin();

	if (!it)
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("No algorithms defined in %s"), _T("CWinCryptEncryptFilter::CWinCryptEncryptFilter"));
	_algo = *it;
}

CWinCryptEncryptFilter::~CWinCryptEncryptFilter()
{
}

void CWinCryptEncryptFilter::set_Algorithm(int ix)
{
	CDataVectorT<CStringBuffer>::Iterator it = _algos.Index(ix);

	if (!it)
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Index out of range in %s"), _T("CWinCryptEncryptFilter::set_SelectedAlgorithm"), CWinException::WinExtError);
	_algo = *it;
}

ConstRef(CDataVectorT<CStringBuffer>) CWinCryptEncryptFilter::get_Algorithms()
{
	if (0 == _algos.Count())
	{
		OSVERSIONINFOEX ver;

		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm RC4, KeyLength=128")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm RSA_KEYX, KeyLength=2048")));
		if (GetVersionEx2(&ver))
		{
			if (ver.dwMajorVersion >= 6)
			{
				_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm DES, KeyLength=64")));
				_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm AES-CBC, KeyLength=256")));
			}
		}
	}
	return _algos;
}

dword CWinCryptEncryptFilter::_algo1()
{
	CByteBuffer buf;
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL;
	HCRYPTKEY hXchgKey = NULL;
	HCRYPTHASH hHash = NULL;

	PBYTE pbKeyBlob = NULL;
	DWORD dwKeyBlobLen = 0;

	PBYTE pbBuffer = NULL;
	DWORD dwBlockLen = 0;
	DWORD dwBufferLen = 0;
	DWORD dwCount = 0;

	bool fEOF = false;

	HandleError(CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0),
		__FILE__LINE__ _T("CryptAcquireContext"), _T("CWinCryptEncryptFilter::do_filter"));

	if (_passwd.IsEmpty())
	{
		HandleError(CryptGenKey(hCryptProv, ENCRYPT_ALGORITHM, KEYLENGTH | CRYPT_EXPORTABLE, &hKey),
			__FILE__LINE__ _T("CryptGenKey"), _T("CWinCryptEncryptFilter::do_filter"));
		//DWORD datalen = sizeof(DWORD);
		//DWORD keylen = 0;
		//HandleError(CryptGetKeyParam(hKey, KP_KEYLEN, (BYTE*)(&keylen), &datalen, 0),
		//	__FILE__LINE__ _T("CryptGetKeyParam"), _T("CWinCryptEncryptFilter::do_filter"));
		//_tprintf(_T("CryptGetKeyParam hKey keylen=%d\n"), keylen);
		if (!CryptGetUserKey(hCryptProv, AT_KEYEXCHANGE, &hXchgKey))
		{
			if (NTE_NO_KEY != GetLastError())
				HandleError(false, __FILE__LINE__ _T("CryptGetUserKey"), _T("CWinCryptEncryptFilter::do_filter"));
			HandleError(CryptGenKey(hCryptProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hXchgKey),
				__FILE__LINE__ _T("CryptGenKey"), _T("CWinCryptEncryptFilter::do_filter"));
		}
		HandleError(CryptExportKey(hKey, hXchgKey, SIMPLEBLOB, 0, NULL, &dwKeyBlobLen),
			__FILE__LINE__ _T("CryptExportKey"), _T("CWinCryptEncryptFilter::do_filter"));
		//datalen = sizeof(DWORD);
		//keylen = 0;
		//HandleError(CryptGetKeyParam(hXchgKey, KP_KEYLEN, (BYTE*)(&keylen), &datalen, 0),
		//	__FILE__LINE__ _T("CryptGetKeyParam"), _T("CWinCryptEncryptFilter::do_filter"));
		//_tprintf(_T("CryptGetKeyParam hXchgKey keylen=%d\n"), keylen);

		buf.set_BufferSize(__FILE__LINE__ dwKeyBlobLen + sizeof(DWORD)+8);
		memcpy(buf.get_Buffer(), "OKCRYPT1", 8);
		DerefAnyPtr(DWORD, buf.get_Buffer() + 8) = dwKeyBlobLen;
		pbKeyBlob = buf.get_Buffer() + sizeof(DWORD)+8;

		HandleError(CryptExportKey(hKey, hXchgKey, SIMPLEBLOB, 0, pbKeyBlob, &dwKeyBlobLen),
			__FILE__LINE__ _T("CryptExportKey"), _T("CWinCryptEncryptFilter::do_filter"));

		write(buf);

		if (hXchgKey)
		{
			HandleError(CryptDestroyKey(hXchgKey),
				__FILE__LINE__ _T("CryptDestroyKey"), _T("CWinCryptEncryptFilter::do_filter"));
			hXchgKey = 0;
		}
	}
	else
	{
		HandleError(CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash),
			__FILE__LINE__ _T("CryptCreateHash"), _T("CWinCryptEncryptFilter::do_filter"));
		HandleError(CryptHashData(hHash, CastAnyPtr(BYTE, CastMutable(CPointer, _passwd.GetString())), _passwd.GetLength() * szchar, 0),
			__FILE__LINE__ _T("CryptHashData"), _T("CWinCryptEncryptFilter::do_filter"));
		HandleError(CryptDeriveKey(hCryptProv, ENCRYPT_ALGORITHM, hHash, KEYLENGTH, &hKey),
			__FILE__LINE__ _T("CryptDeriveKey"), _T("CWinCryptEncryptFilter::do_filter"));
		//DWORD datalen = sizeof(DWORD);
		//DWORD blocklen = 0;
		//HandleError(CryptGetKeyParam(hKey, KP_KEYLEN, (BYTE*)(&blocklen), &datalen, 0),
		//	__FILE__LINE__ _T("CryptGetKeyParam"), _T("CWinCryptEncryptFilter::do_filter"));
		//_tprintf(_T("CryptGetKeyParam hKey keylen=%d\n"), blocklen);

		buf.set_BufferSize(__FILE__LINE__ 8);
		memcpy(buf.get_Buffer(), "OKCRYPT2", 8);
		write(buf);
	}

	dwBufferLen = dwBlockLen = 1000 - (1000 % ENCRYPT_BLOCK_SIZE);
	if (ENCRYPT_BLOCK_SIZE > 1)
		dwBufferLen += ENCRYPT_BLOCK_SIZE;

	fEOF = FALSE;

	do
	{
		buf.set_BufferSize(__FILE__LINE__ dwBlockLen);
		read(buf);

		dwCount = buf.get_BufferSize();
		if (dwCount < dwBlockLen)
			fEOF = TRUE;

		buf.set_BufferSize(__FILE__LINE__ dwBufferLen);
		HandleError(CryptEncrypt(hKey, NULL, fEOF, 0, buf.get_Buffer(), &dwCount, dwBufferLen),
			__FILE__LINE__ _T("CryptEncrypt"), _T("CWinCryptEncryptFilter::do_filter"));
		buf.set_BufferSize(__FILE__LINE__ dwCount);
		write(buf);
	} while (!fEOF);

	if (hHash)
	{
		HandleError(CryptDestroyHash(hHash),
			__FILE__LINE__ _T("CryptDestroyHash"), _T("CWinCryptEncryptFilter::do_filter"));
		hHash = NULL;
	}
	if (hKey)
	{
		HandleError(CryptDestroyKey(hKey),
			__FILE__LINE__ _T("CryptDestroyKey"), _T("CWinCryptEncryptFilter::do_filter"));
		hKey = NULL;
	}
	if (hCryptProv)
	{
		HandleError(CryptReleaseContext(hCryptProv, 0),
			__FILE__LINE__ _T("CryptReleaseContext"), _T("CWinCryptEncryptFilter::do_filter"));
		hCryptProv = NULL;
	}
	return 0;
}

dword CWinCryptEncryptFilter::_algo2()
{
	CByteBuffer buf;
	CByteBuffer buf1;
	HCRYPTPROV hProv;
	HCRYPTKEY hXchgKey;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;
	DWORD datalen;
	DWORD datalen1;

	if (_passwd.IsEmpty())
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Password cannot be empty in %s"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0),
		__FILE__LINE__ _T("CryptAcquireContext"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash),
		__FILE__LINE__ _T("CryptCreateHash"), _T("CWinCryptEncryptFilter::do_filter"));
	_passwd.convertToByteBuffer(buf1);
	HandleError(CryptHashData(hHash, buf1.get_Buffer(), buf1.get_BufferSize(), 0),
		__FILE__LINE__ _T("CryptHashData"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey),
		__FILE__LINE__ _T("CryptDeriveKey"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(CryptGenKey(hProv, AT_KEYEXCHANGE, 0x08000000 | CRYPT_EXPORTABLE, &hXchgKey),
		__FILE__LINE__ _T("CryptGenKey"), _T("CWinCryptEncryptFilter::do_filter"));
	datalen = 0;
	HandleError(CryptExportKey(hXchgKey, 0, PRIVATEKEYBLOB, 0, NULL, &datalen),
		__FILE__LINE__ _T("CryptExportKey"), _T("CWinCryptEncryptFilter::do_filter"));
	buf1.set_BufferSize(__FILE__LINE__ datalen);
	HandleError(CryptExportKey(hXchgKey, 0, PRIVATEKEYBLOB, 0, buf1.get_Buffer(), &datalen),
		__FILE__LINE__ _T("CryptExportKey"), _T("CWinCryptEncryptFilter::do_filter"));
	datalen = datalen1 = buf1.get_BufferSize();
	HandleError(CryptEncrypt(hKey, NULL, TRUE, 0, NULL, &datalen, buf1.get_BufferSize()),
		__FILE__LINE__ _T("CryptEncrypt"), _T("CWinCryptEncryptFilter::do_filter"));
	buf1.set_BufferSize(__FILE__LINE__ datalen);
	HandleError(CryptEncrypt(hKey, NULL, TRUE, 0, buf1.get_Buffer(), &datalen1, buf1.get_BufferSize()),
		__FILE__LINE__ _T("CryptEncrypt"), _T("CWinCryptEncryptFilter::do_filter"));
	buf1.set_BufferSize(__FILE__LINE__ datalen);

	buf.set_BufferSize(__FILE__LINE__ sizeof(DWORD)+8);
	memcpy(buf.get_Buffer(), "OKCRYPT3", 8);
	DerefAnyPtr(DWORD, buf.get_Buffer() + 8) = datalen;
	write(buf);
	write(buf1);

	while (true)
	{
		buf.set_BufferSize(__FILE__LINE__ 240);
		read(buf);

		datalen = buf.get_BufferSize();
		if (0 == datalen)
			break;

		buf.set_BufferSize(__FILE__LINE__ 256);
		HandleError(CryptEncrypt(hXchgKey, NULL, FALSE, 0, buf.get_Buffer(), &datalen, 256),
			__FILE__LINE__ _T("CryptEncrypt"), _T("CWinCryptEncryptFilter::do_filter"));
		write(buf);
	}

	HandleError(CryptDestroyKey(hKey),
		__FILE__LINE__ _T("CryptDestroyKey"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(CryptDestroyHash(hHash),
		__FILE__LINE__ _T("CryptDestroyHash"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(CryptDestroyKey(hXchgKey),
		__FILE__LINE__ _T("CryptDestroyKey"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(CryptReleaseContext(hProv, 0),
		__FILE__LINE__ _T("CryptReleaseContext"), _T("CWinCryptEncryptFilter::do_filter"));
	return 0;
}

dword CWinCryptEncryptFilter::_algo3()
{
	CByteBuffer buf;
	CByteBuffer buf1;
	CByteBuffer buf2;
	BCRYPT_ALG_HANDLE algH;
	BCRYPT_ALG_HANDLE algHHash;
	BCRYPT_KEY_HANDLE keyH;
	BCRYPT_HASH_HANDLE hashH;
	ULONG cbResult;
	ULONG cbData;
	DWORD cbBlockLen;
	DWORD cbHashLength;
	CByteBuffer vIV;
	CByteBuffer vKeyObject;
	CByteBuffer vHashObject;
	DWORD cbKeyObject;
	DWORD cbHashObject;

	if (_passwd.IsEmpty())
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Password cannot be empty in %s"), _T("CWinCryptEncryptFilter::do_filter"));

	HandleError(BCryptOpenAlgorithmProvider(&algHHash, BCRYPT_SHA512_ALGORITHM, NULL, 0),
		__FILE__LINE__ _T("BCryptOpenAlgorithmProvider"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algHHash, BCRYPT_OBJECT_LENGTH, CastAnyPtr(BYTE, &cbHashObject), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptEncryptFilter::do_filter"));
	vHashObject.set_BufferSize(__FILE__LINE__ cbHashObject);
	HandleError(BCryptCreateHash(algHHash, &hashH, vHashObject.get_Buffer(), cbHashObject, NULL, 0, 0),
		__FILE__LINE__ _T("BCryptCreateHash"), _T("CWinCryptEncryptFilter::do_filter"));
	_passwd.convertToByteBuffer(buf);
	HandleError(BCryptHashData(hashH, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptHashData"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algHHash, BCRYPT_HASH_LENGTH, CastAnyPtr(BYTE, &cbHashLength), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptEncryptFilter::do_filter"));
	buf.set_BufferSize(__FILE__LINE__ cbHashLength);
	HandleError(BCryptFinishHash(hashH, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptFinishHash"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptDestroyHash(hashH),
		__FILE__LINE__ _T("BCryptDestroyHash"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptCloseAlgorithmProvider(algHHash, 0),
		__FILE__LINE__ _T("BCryptCloseAlgorithmProvider"), _T("CWinCryptEncryptFilter::do_filter"));

	HandleError(BCryptOpenAlgorithmProvider(&algH, BCRYPT_DES_ALGORITHM, NULL, 0),
		__FILE__LINE__ _T("BCryptOpenAlgorithmProvider"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algH, BCRYPT_OBJECT_LENGTH, CastAnyPtr(BYTE, &cbKeyObject), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptEncryptFilter::do_filter"));
	vKeyObject.set_BufferSize(__FILE__LINE__ cbKeyObject);
	HandleError(BCryptGetProperty(algH, BCRYPT_BLOCK_LENGTH, CastAnyPtr(BYTE, &cbBlockLen), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptEncryptFilter::do_filter"));
	if (cbBlockLen > sizeof (rgbIV))
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Block length larger than IV in %s"), _T("CWinCryptEncryptFilter::do_filter"));
	vIV.set_Buffer(__FILE__LINE__ rgbIV, cbBlockLen);

	//BCRYPT_KEY_LENGTHS_STRUCT vKL;

	//HandleError(BCryptGetProperty(algH, BCRYPT_KEY_LENGTHS, CastAnyPtr(BYTE, &vKL), sizeof(BCRYPT_KEY_LENGTHS_STRUCT), &cbData, 0),
	//	__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	//_tprintf(_T("DES Min=%d, Max=%d, Incr=%d\n"), vKL.dwMinLength, vKL.dwMaxLength, vKL.dwIncrement);

	HandleError(BCryptGenerateSymmetricKey(algH, &keyH, vKeyObject.get_Buffer(), vKeyObject.get_BufferSize(), buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptGenerateSymmetricKey"), _T("CWinCryptEncryptFilter::do_filter"));

	//DWORD cbKeyLength;

	//HandleError(BCryptGetProperty(keyH, BCRYPT_KEY_LENGTH, CastAnyPtr(BYTE, &cbKeyLength), sizeof(DWORD), &cbData, 0),
	//	__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	//_tprintf(_T("DES KeyLength=%d\n"), cbKeyLength);

	buf.set_BufferSize(__FILE__LINE__ 8);
	memcpy(buf.get_Buffer(), "OKCRYPT4", 8);
	write(buf);

	cbBlockLen *= 64;
	while (true)
	{
		buf.set_BufferSize(__FILE__LINE__ cbBlockLen);
		read(buf);
		if (0 == buf.get_BufferSize())
			break;

		HandleError(BCryptEncrypt(keyH, buf.get_Buffer(), buf.get_BufferSize(), NULL, vIV.get_Buffer(), vIV.get_BufferSize(), NULL, 0, &cbResult, BCRYPT_BLOCK_PADDING),
			__FILE__LINE__ _T("BCryptEncrypt"), _T("CWinCryptEncryptFilter::do_filter"));
		buf2.set_BufferSize(__FILE__LINE__ sizeof(DWORD));
		DerefAnyPtr(DWORD, buf2.get_Buffer()) = cbResult;
		write(buf2);
		buf1.set_BufferSize(__FILE__LINE__ cbResult);
		HandleError(BCryptEncrypt(keyH, buf.get_Buffer(), buf.get_BufferSize(), NULL, vIV.get_Buffer(), vIV.get_BufferSize(), buf1.get_Buffer(), buf1.get_BufferSize(), &cbResult, BCRYPT_BLOCK_PADDING),
			__FILE__LINE__ _T("BCryptEncrypt"), _T("CWinCryptEncryptFilter::do_filter"));
		write(buf1);
	}
	HandleError(BCryptDestroyKey(keyH),
		__FILE__LINE__ _T("BCryptDestroyKey"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptCloseAlgorithmProvider(algH, 0),
		__FILE__LINE__ _T("BCryptCloseAlgorithmProvider"), _T("CWinCryptEncryptFilter::do_filter"));
	return 0;
}

dword CWinCryptEncryptFilter::_algo4()
{
	CByteBuffer buf;
	CByteBuffer buf1;
	CByteBuffer buf2;
	BCRYPT_ALG_HANDLE algH;
	BCRYPT_ALG_HANDLE algHHash;
	BCRYPT_KEY_HANDLE keyH;
	BCRYPT_HASH_HANDLE hashH;
	ULONG cbResult;
	ULONG cbData;
	DWORD cbBlockLen;
	DWORD cbHashLength;
	CByteBuffer vIV;
	CByteBuffer vKeyObject;
	CByteBuffer vHashObject;
	DWORD cbKeyObject;
	DWORD cbHashObject;

	if (_passwd.IsEmpty())
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Password cannot be empty in %s"), _T("CWinCryptEncryptFilter::do_filter"));

	HandleError(BCryptOpenAlgorithmProvider(&algHHash, BCRYPT_SHA512_ALGORITHM, NULL, 0),
		__FILE__LINE__ _T("BCryptOpenAlgorithmProvider"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algHHash, BCRYPT_OBJECT_LENGTH, CastAnyPtr(BYTE, &cbHashObject), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptEncryptFilter::do_filter"));
	vHashObject.set_BufferSize(__FILE__LINE__ cbHashObject);
	HandleError(BCryptCreateHash(algHHash, &hashH, vHashObject.get_Buffer(), cbHashObject, NULL, 0, 0),
		__FILE__LINE__ _T("BCryptCreateHash"), _T("CWinCryptEncryptFilter::do_filter"));
	_passwd.convertToByteBuffer(buf);
	HandleError(BCryptHashData(hashH, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptHashData"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algHHash, BCRYPT_HASH_LENGTH, CastAnyPtr(BYTE, &cbHashLength), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptEncryptFilter::do_filter"));
	buf.set_BufferSize(__FILE__LINE__ cbHashLength);
	HandleError(BCryptFinishHash(hashH, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptFinishHash"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptDestroyHash(hashH),
		__FILE__LINE__ _T("BCryptDestroyHash"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptCloseAlgorithmProvider(algHHash, 0),
		__FILE__LINE__ _T("BCryptCloseAlgorithmProvider"), _T("CWinCryptEncryptFilter::do_filter"));

	HandleError(BCryptOpenAlgorithmProvider(&algH, BCRYPT_AES_ALGORITHM, NULL, 0),
		__FILE__LINE__ _T("BCryptOpenAlgorithmProvider"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algH, BCRYPT_OBJECT_LENGTH, CastAnyPtr(BYTE, &cbKeyObject), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptEncryptFilter::do_filter"));
	vKeyObject.set_BufferSize(__FILE__LINE__ cbKeyObject);
	HandleError(BCryptGetProperty(algH, BCRYPT_BLOCK_LENGTH, CastAnyPtr(BYTE, &cbBlockLen), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptEncryptFilter::do_filter"));
	if (cbBlockLen > sizeof (rgbIV))
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Block length larger than IV in %s"), _T("CWinCryptEncryptFilter::do_filter"));
	vIV.set_Buffer(__FILE__LINE__ rgbIV, cbBlockLen);
	HandleError(BCryptSetProperty(algH, BCRYPT_CHAINING_MODE, CastAny(PBYTE, BCRYPT_CHAIN_MODE_CBC), sizeof(BCRYPT_CHAIN_MODE_CBC), 0),
		__FILE__LINE__ _T("BCryptSetProperty"), _T("CWinCryptEncryptFilter::do_filter"));

	//BCRYPT_KEY_LENGTHS_STRUCT vKL;

	//HandleError(BCryptGetProperty(algH, BCRYPT_KEY_LENGTHS, CastAnyPtr(BYTE, &vKL), sizeof(BCRYPT_KEY_LENGTHS_STRUCT), &cbData, 0),
	//	__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	//_tprintf(_T("AES Min=%d, Max=%d, Incr=%d\n"), vKL.dwMinLength, vKL.dwMaxLength, vKL.dwIncrement);

	HandleError(BCryptGenerateSymmetricKey(algH, &keyH, vKeyObject.get_Buffer(), cbKeyObject, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptGenerateSymmetricKey"), _T("CWinCryptEncryptFilter::do_filter"));

	//HandleError(BCryptGetProperty(keyH, BCRYPT_KEY_LENGTH, CastAnyPtr(BYTE, &cbKeyLength), sizeof(DWORD), &cbData, 0),
	//	__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	//_tprintf(_T("AES KeyLength=%d\n"), cbKeyLength);

	buf.set_BufferSize(__FILE__LINE__ 8);
	memcpy(buf.get_Buffer(), "OKCRYPT5", 8);
	write(buf);

	cbBlockLen *= 64;
	while (true)
	{
		buf.set_BufferSize(__FILE__LINE__ cbBlockLen);
		read(buf);
		if (0 == buf.get_BufferSize())
			break;

		HandleError(BCryptEncrypt(keyH, buf.get_Buffer(), buf.get_BufferSize(), NULL, vIV.get_Buffer(), vIV.get_BufferSize(), NULL, 0, &cbResult, BCRYPT_BLOCK_PADDING),
			__FILE__LINE__ _T("BCryptEncrypt"), _T("CWinCryptEncryptFilter::do_filter"));
		buf2.set_BufferSize(__FILE__LINE__ sizeof(DWORD));
		DerefAnyPtr(DWORD, buf2.get_Buffer()) = cbResult;
		write(buf2);
		buf1.set_BufferSize(__FILE__LINE__ cbResult);
		HandleError(BCryptEncrypt(keyH, buf.get_Buffer(), buf.get_BufferSize(), NULL, vIV.get_Buffer(), vIV.get_BufferSize(), buf1.get_Buffer(), buf1.get_BufferSize(), &cbResult, BCRYPT_BLOCK_PADDING),
			__FILE__LINE__ _T("BCryptEncrypt"), _T("CWinCryptEncryptFilter::do_filter"));
		write(buf1);
	}
	HandleError(BCryptDestroyKey(keyH),
		__FILE__LINE__ _T("BCryptDestroyKey"), _T("CWinCryptEncryptFilter::do_filter"));
	HandleError(BCryptCloseAlgorithmProvider(algH, 0),
		__FILE__LINE__ _T("BCryptCloseAlgorithmProvider"), _T("CWinCryptEncryptFilter::do_filter"));
	return 0;
}

dword CWinCryptEncryptFilter::do_filter()
{
	CDataVectorT<CStringBuffer>::Iterator it = _algos.Begin();
	dword ix = 0;

	while (it)
	{
		if (_algo.Compare(*it, 0, CStringLiteral::cIgnoreCase) == 0)
		{
			switch (ix)
			{
			case 0:
				return _algo1();
			case 1:
				return _algo2();
			case 2:
				return _algo3();
			case 3:
				return _algo4();
			default:
				return 0;
			}
		}
		++it;
		++ix;
	}
	return 0;
}

CDataVectorT<CStringBuffer> CWinCryptDecryptFilter::_algos(__FILE__LINE__ 16, 16);

CWinCryptDecryptFilter::CWinCryptDecryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
CFilter(input, output),
_passwd(),
_algo()
{
	ConstRef(CDataVectorT<CStringBuffer>) buf(get_Algorithms());
	CDataVectorT<CStringBuffer>::Iterator it = buf.Begin();

	if (!it)
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("No algorithms defined in %s"), _T("CWinCryptDecryptFilter::CWinCryptDecryptFilter"));
	_algo = *it;
}

CWinCryptDecryptFilter::~CWinCryptDecryptFilter()
{
}

void CWinCryptDecryptFilter::set_Algorithm(int ix)
{
	CDataVectorT<CStringBuffer>::Iterator it = _algos.Index(ix);

	if (!it)
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Index out of range in %s"), _T("CWinCryptEncryptFilter::set_SelectedAlgorithm"), CWinException::WinExtError);
	_algo = *it;
}

ConstRef(CDataVectorT<CStringBuffer>) CWinCryptDecryptFilter::get_Algorithms()
{
	if (0 == _algos.Count())
	{
		OSVERSIONINFOEX ver;

		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm RC4, KeyLength=128")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm RSA_KEYX, KeyLength=2048")));
		if (GetVersionEx2(&ver))
		{
			if (ver.dwMajorVersion >= 6)
			{
				_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm AES-CBC, KeyLength=256")));
				_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm DES, KeyLength=64")));
			}
		}
	}
	return _algos;
}

dword CWinCryptDecryptFilter::_algo1()
{
	CByteBuffer buf;
	HCRYPTKEY hKey = NULL;
	HCRYPTHASH hHash = NULL;

	HCRYPTPROV hCryptProv = NULL;

	DWORD dwCount;
	PBYTE pbBuffer = NULL;
	DWORD dwBlockLen;
	DWORD dwBufferLen;
	DWORD dwKeyBlobLen;

	bool fEOF;

	HandleError(CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0),
		__FILE__LINE__ _T("CryptAcquireContext"), _T("CWinCryptDecryptFilter::do_filter"));

	if (_passwd.IsEmpty())
	{
		buf.set_BufferSize(__FILE__LINE__ sizeof(DWORD) + 8);
		read(buf);
		if (memcmp(buf.get_Buffer(), "OKCRYPT1", 8) != 0)
			throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("File magic illegal in %s"), _T("CWinCryptDecryptFilter::do_filter"));
		dwKeyBlobLen = DerefAnyPtr(DWORD, buf.get_Buffer() + 8);
		buf.set_BufferSize(__FILE__LINE__ dwKeyBlobLen);
		read(buf);

		HandleError(CryptImportKey(hCryptProv, buf.get_Buffer(), dwKeyBlobLen, 0, 0, &hKey),
			__FILE__LINE__ _T("CryptImportKey"), _T("CWinCryptDecryptFilter::do_filter"));
	}
	else
	{
		buf.set_BufferSize(__FILE__LINE__ 8);
		read(buf);
		if (memcmp(buf.get_Buffer(), "OKCRYPT2", 8) != 0)
			throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("File magic illegal in %s"), _T("CWinCryptDecryptFilter::do_filter"));

		HandleError(CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash),
			__FILE__LINE__ _T("CryptCreateHash"), _T("CWinCryptDecryptFilter::do_filter"));
		HandleError(CryptHashData(hHash, CastAnyPtr(BYTE, CastMutable(CPointer, _passwd.GetString())), _passwd.GetLength() * szchar, 0),
			__FILE__LINE__ _T("CryptHashData"), _T("CWinCryptDecryptFilter::do_filter"));
		HandleError(CryptDeriveKey(hCryptProv, ENCRYPT_ALGORITHM, hHash, KEYLENGTH, &hKey),
			__FILE__LINE__ _T("CryptDeriveKey"), _T("CWinCryptDecryptFilter::do_filter"));
	}

	dwBlockLen = 1000 - (1000 % ENCRYPT_BLOCK_SIZE);
	dwBufferLen = dwBlockLen;

	fEOF = false;

	do
	{
		buf.set_BufferSize(__FILE__LINE__ dwBlockLen);
		read(buf);

		dwCount = buf.get_BufferSize();
		if (dwCount < dwBlockLen)
			fEOF = true;

		HandleError(CryptDecrypt(hKey, 0, fEOF, 0, buf.get_Buffer(), &dwCount),
			__FILE__LINE__ _T("CryptDecrypt"), _T("CWinCryptDecryptFilter::do_filter"));
		buf.set_BufferSize(__FILE__LINE__ dwCount);
		write(buf);
	} while (!fEOF);

	if (hHash)
	{
		HandleError(CryptDestroyHash(hHash),
			__FILE__LINE__ _T("CryptDestroyHash"), _T("CWinCryptDecryptFilter::do_filter"));
		hHash = NULL;
	}
	if (hKey)
	{
		HandleError(CryptDestroyKey(hKey),
			__FILE__LINE__ _T("CryptDestroyKey"), _T("CWinCryptDecryptFilter::do_filter"));
		hKey = NULL;
	}
	if (hCryptProv)
	{
		HandleError(CryptReleaseContext(hCryptProv, 0),
			__FILE__LINE__ _T("CryptReleaseContext"), _T("CWinCryptDecryptFilter::do_filter"));
		hCryptProv = NULL;
	}
	return 0;
}

dword CWinCryptDecryptFilter::_algo2()
{
	CByteBuffer buf;
	HCRYPTPROV hProv = 0;
	HCRYPTKEY hXchgKey = 0;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;
	DWORD datalen = 0;

	if (_passwd.IsEmpty())
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Password cannot be empty in %s"), _T("CWinCryptDecryptFilter::do_filter"));
	buf.set_BufferSize(__FILE__LINE__ sizeof(DWORD)+8);
	read(buf);
	if (memcmp(buf.get_Buffer(), "OKCRYPT3", 8) != 0)
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("File magic illegal in %s"), _T("CWinCryptDecryptFilter::do_filter"));
	datalen = DerefAnyPtr(DWORD, buf.get_Buffer() + 8);

	HandleError(CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0),
		__FILE__LINE__ _T("CryptAcquireContext"), _T("CWinCryptDecryptFilter::do_filter)"));
	HandleError(CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash),
		__FILE__LINE__ _T("CryptCreateHash"), _T("CWinCryptDecryptFilter::do_filter)"));
	_passwd.convertToByteBuffer(buf);
	HandleError(CryptHashData(hHash, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("CryptHashData"), _T("CWinCryptDecryptFilter::do_filter)"));
	HandleError(CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey),
		__FILE__LINE__ _T("CryptDeriveKey"), _T("CWinCryptDecryptFilter::do_filter)"));

	buf.set_BufferSize(__FILE__LINE__ datalen);
	read(buf);

	HandleError(CryptDecrypt(hKey, NULL, TRUE, 0, buf.get_Buffer(), &datalen),
		__FILE__LINE__ _T("CryptDecrypt"), _T("CWinCryptDecryptFilter::do_filter)"));
	buf.set_BufferSize(__FILE__LINE__ datalen);
	HandleError(CryptImportKey(hProv, buf.get_Buffer(), datalen, 0, 0, &hXchgKey),
		__FILE__LINE__ _T("CryptImportKey"), _T("CWinCryptDecryptFilter::do_filter)"));
	//datalen = sizeof(DWORD);
	//DWORD blocklen = 0;
	//HandleError(CryptGetKeyParam(hKey, KP_KEYLEN, (BYTE*)(&blocklen), &datalen, 0),
	//	__FILE__LINE__ _T("CryptGetKeyParam"), _T("CWinCryptDecryptFilter::do_filter)"));
	//_tprintf(_T("CryptGetKeyParam hKey keylen=%d\n"), blocklen);
	//datalen = sizeof(DWORD);
	//DWORD blocklen = 0;
	//HandleError(CryptGetKeyParam(hXchgKey, KP_BLOCKLEN, (BYTE*)(&blocklen), &datalen, 0),
	//	__FILE__LINE__ _T("CryptGetKeyParam"), _T("CWinCryptDecryptFilter::do_filter)"));
	//_tprintf(_T("CryptGetKeyParam hXchgKey blocklen=%d\n"), blocklen);
	//datalen = sizeof(DWORD);
	//DWORD algid = 0;
	//HandleError(CryptGetKeyParam(hXchgKey, KP_ALGID, (BYTE*)(&algid), &datalen, 0),
	//	__FILE__LINE__ _T("CryptGetKeyParam"), _T("CWinCryptDecryptFilter::do_filter)"));
	//_tprintf(_T("CryptGetKeyParam hXchgKey algid=0x%08x\n"), algid);

	while (true)
	{
		buf.set_BufferSize(__FILE__LINE__ 256);
		read(buf);

		if (0 == buf.get_BufferSize())
			break;

		datalen = buf.get_BufferSize();
		HandleError(CryptDecrypt(hXchgKey, NULL, FALSE, 0, buf.get_Buffer(), &datalen),
			__FILE__LINE__ _T("CryptDecrypt"), _T("CWinCryptDecryptFilter::do_filter)"));
		buf.set_BufferSize(__FILE__LINE__ datalen);
		write(buf);
	}

	HandleError(CryptDestroyKey(hXchgKey),
		__FILE__LINE__ _T("CryptDestroyKey"), _T("CWinCryptDecryptFilter::do_filter)"));
	HandleError(CryptDestroyKey(hKey),
		__FILE__LINE__ _T("CryptDestroyKey"), _T("CWinCryptDecryptFilter::do_filter)"));
	HandleError(CryptDestroyHash(hHash),
		__FILE__LINE__ _T("CryptDestroyHash"), _T("CWinCryptDecryptFilter::do_filter)"));
	HandleError(CryptReleaseContext(hProv, 0),
		__FILE__LINE__ _T("CryptReleaseContext"), _T("CWinCryptDecryptFilter::do_filter)"));
	return 0;
}

dword CWinCryptDecryptFilter::_algo3()
{
	CByteBuffer buf;
	CByteBuffer buf1;
	CByteBuffer buf2;
	BCRYPT_ALG_HANDLE algH;
	BCRYPT_ALG_HANDLE algHHash;
	BCRYPT_KEY_HANDLE keyH;
	BCRYPT_HASH_HANDLE hashH;
	ULONG cbResult;
	ULONG cbData;
	DWORD cbBlockLen;
	DWORD cbHashLength;
	CByteBuffer vIV;
	CByteBuffer vKeyObject;
	CByteBuffer vHashObject;
	DWORD cbKeyObject;
	DWORD cbHashObject;

	buf.set_BufferSize(__FILE__LINE__ 8);
	read(buf);
	if (memcmp(buf.get_Buffer(), "OKCRYPT4", 8) != 0)
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("File magic illegal in %s"), _T("CWinCryptDecryptFilter::do_filter"));

	if (_passwd.IsEmpty())
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Password cannot be empty in %s"), _T("CWinCryptDecryptFilter::do_filter"));

	HandleError(BCryptOpenAlgorithmProvider(&algHHash, BCRYPT_SHA512_ALGORITHM, NULL, 0),
		__FILE__LINE__ _T("BCryptOpenAlgorithmProvider"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algHHash, BCRYPT_OBJECT_LENGTH, CastAnyPtr(BYTE, &cbHashObject), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	vHashObject.set_BufferSize(__FILE__LINE__ cbHashObject);
	HandleError(BCryptCreateHash(algHHash, &hashH, vHashObject.get_Buffer(), cbHashObject, NULL, 0, 0),
		__FILE__LINE__ _T("BCryptCreateHash"), _T("CWinCryptDecryptFilter::do_filter"));
	_passwd.convertToByteBuffer(buf);
	HandleError(BCryptHashData(hashH, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptHashData"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algHHash, BCRYPT_HASH_LENGTH, CastAnyPtr(BYTE, &cbHashLength), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	buf.set_BufferSize(__FILE__LINE__ cbHashLength);
	HandleError(BCryptFinishHash(hashH, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptFinishHash"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptDestroyHash(hashH),
		__FILE__LINE__ _T("BCryptDestroyHash"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptCloseAlgorithmProvider(algHHash, 0),
		__FILE__LINE__ _T("BCryptCloseAlgorithmProvider"), _T("CWinCryptDecryptFilter::do_filter"));

	HandleError(BCryptOpenAlgorithmProvider(&algH, BCRYPT_DES_ALGORITHM, NULL, 0),
		__FILE__LINE__ _T("BCryptOpenAlgorithmProvider"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algH, BCRYPT_OBJECT_LENGTH, CastAnyPtr(BYTE, &cbKeyObject), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	vKeyObject.set_BufferSize(__FILE__LINE__ cbKeyObject);
	HandleError(BCryptGetProperty(algH, BCRYPT_BLOCK_LENGTH, CastAnyPtr(BYTE, &cbBlockLen), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	if (cbBlockLen > sizeof (rgbIV))
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Block length larger than IV in %s"), _T("CWinCryptEncryptFilter::do_filter"));
	vIV.set_Buffer(__FILE__LINE__ rgbIV, cbBlockLen);

	//BCRYPT_KEY_LENGTHS_STRUCT vKL;

	//HandleError(BCryptGetProperty(algH, BCRYPT_KEY_LENGTHS, CastAnyPtr(BYTE, &vKL), sizeof(BCRYPT_KEY_LENGTHS_STRUCT), &cbData, 0),
	//	__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	//_tprintf(_T("AES Min=%d, Max=%d, Incr=%d\n"), vKL.dwMinLength, vKL.dwMaxLength, vKL.dwIncrement);

	HandleError(BCryptGenerateSymmetricKey(algH, &keyH, vKeyObject.get_Buffer(), cbKeyObject, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptGenerateSymmetricKey"), _T("CWinCryptDecryptFilter::do_filter"));

	//HandleError(BCryptGetProperty(keyH, BCRYPT_KEY_LENGTH, CastAnyPtr(BYTE, &cbKeyLength), sizeof(DWORD), &cbData, 0),
	//	__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	//_tprintf(_T("AES KeyLength=%d\n"), cbKeyLength);

	while (true)
	{
		buf2.set_BufferSize(__FILE__LINE__ sizeof(DWORD));
		read(buf2);
		if (0 == buf2.get_BufferSize())
			break;
		cbBlockLen = DerefAnyPtr(DWORD, buf2.get_Buffer());
		buf.set_BufferSize(__FILE__LINE__ cbBlockLen);
		read(buf);
		if (0 == buf.get_BufferSize())
			break;

		HandleError(BCryptDecrypt(keyH, buf.get_Buffer(), buf.get_BufferSize(), NULL, vIV.get_Buffer(), vIV.get_BufferSize(), NULL, 0, &cbResult, BCRYPT_BLOCK_PADDING),
			__FILE__LINE__ _T("BCryptEncrypt"), _T("CWinCryptDecryptFilter::do_filter"));
		buf1.set_BufferSize(__FILE__LINE__ cbResult);
		HandleError(BCryptDecrypt(keyH, buf.get_Buffer(), buf.get_BufferSize(), NULL, vIV.get_Buffer(), vIV.get_BufferSize(), buf1.get_Buffer(), cbResult, &cbResult, BCRYPT_BLOCK_PADDING),
			__FILE__LINE__ _T("BCryptEncrypt"), _T("CWinCryptDecryptFilter::do_filter"));
		buf1.set_BufferSize(__FILE__LINE__ cbResult);
		write(buf1);
	}
	HandleError(BCryptDestroyKey(keyH),
		__FILE__LINE__ _T("BCryptDestroyKey"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptCloseAlgorithmProvider(algH, 0),
		__FILE__LINE__ _T("BCryptCloseAlgorithmProvider"), _T("CWinCryptDecryptFilter::do_filter"));
	return 0;
}

dword CWinCryptDecryptFilter::_algo4()
{
	CByteBuffer buf;
	CByteBuffer buf1;
	CByteBuffer buf2;
	BCRYPT_ALG_HANDLE algH;
	BCRYPT_ALG_HANDLE algHHash;
	BCRYPT_KEY_HANDLE keyH;
	BCRYPT_HASH_HANDLE hashH;
	ULONG cbResult;
	ULONG cbData;
	DWORD cbBlockLen;
	DWORD cbHashLength;
	CByteBuffer vIV;
	CByteBuffer vKeyObject;
	CByteBuffer vHashObject;
	DWORD cbKeyObject;
	DWORD cbHashObject;

	buf.set_BufferSize(__FILE__LINE__ 8);
	read(buf);
	if (memcmp(buf.get_Buffer(), "OKCRYPT5", 8) != 0)
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("File magic illegal in %s"), _T("CWinCryptDecryptFilter::do_filter"));

	if (_passwd.IsEmpty())
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Password cannot be empty in %s"), _T("CWinCryptDecryptFilter::do_filter"));

	HandleError(BCryptOpenAlgorithmProvider(&algHHash, BCRYPT_SHA512_ALGORITHM, NULL, 0),
		__FILE__LINE__ _T("BCryptOpenAlgorithmProvider"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algHHash, BCRYPT_OBJECT_LENGTH, CastAnyPtr(BYTE, &cbHashObject), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	vHashObject.set_BufferSize(__FILE__LINE__ cbHashObject);
	HandleError(BCryptCreateHash(algHHash, &hashH, vHashObject.get_Buffer(), cbHashObject, NULL, 0, 0),
		__FILE__LINE__ _T("BCryptCreateHash"), _T("CWinCryptDecryptFilter::do_filter"));
	_passwd.convertToByteBuffer(buf);
	HandleError(BCryptHashData(hashH, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptHashData"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algHHash, BCRYPT_HASH_LENGTH, CastAnyPtr(BYTE, &cbHashLength), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	buf.set_BufferSize(__FILE__LINE__ cbHashLength);
	HandleError(BCryptFinishHash(hashH, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptFinishHash"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptDestroyHash(hashH),
		__FILE__LINE__ _T("BCryptDestroyHash"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptCloseAlgorithmProvider(algHHash, 0),
		__FILE__LINE__ _T("BCryptCloseAlgorithmProvider"), _T("CWinCryptDecryptFilter::do_filter"));

	HandleError(BCryptOpenAlgorithmProvider(&algH, BCRYPT_AES_ALGORITHM, NULL, 0),
		__FILE__LINE__ _T("BCryptOpenAlgorithmProvider"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptGetProperty(algH, BCRYPT_OBJECT_LENGTH, CastAnyPtr(BYTE, &cbKeyObject), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	vKeyObject.set_BufferSize(__FILE__LINE__ cbKeyObject);
	HandleError(BCryptGetProperty(algH, BCRYPT_BLOCK_LENGTH, CastAnyPtr(BYTE, &cbBlockLen), sizeof(DWORD), &cbData, 0),
		__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	if (cbBlockLen > sizeof (rgbIV))
		throw OK_NEW_OPERATOR CWinCryptException(__FILE__LINE__ _T("Block length larger than IV in %s"), _T("CWinCryptEncryptFilter::do_filter"));
	vIV.set_Buffer(__FILE__LINE__ rgbIV, cbBlockLen);
	HandleError(BCryptSetProperty(algH, BCRYPT_CHAINING_MODE, CastAny(PBYTE, BCRYPT_CHAIN_MODE_CBC), sizeof(BCRYPT_CHAIN_MODE_CBC), 0),
		__FILE__LINE__ _T("BCryptSetProperty"), _T("CWinCryptDecryptFilter::do_filter"));

	//BCRYPT_KEY_LENGTHS_STRUCT vKL;

	//HandleError(BCryptGetProperty(algH, BCRYPT_KEY_LENGTHS, CastAnyPtr(BYTE, &vKL), sizeof(BCRYPT_KEY_LENGTHS_STRUCT), &cbData, 0),
	//	__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	//_tprintf(_T("AES Min=%d, Max=%d, Incr=%d\n"), vKL.dwMinLength, vKL.dwMaxLength, vKL.dwIncrement);

	HandleError(BCryptGenerateSymmetricKey(algH, &keyH, vKeyObject.get_Buffer(), cbKeyObject, buf.get_Buffer(), buf.get_BufferSize(), 0),
		__FILE__LINE__ _T("BCryptGenerateSymmetricKey"), _T("CWinCryptDecryptFilter::do_filter"));

	//DWORD cbKeyLength;

	//HandleError(BCryptGetProperty(keyH, BCRYPT_KEY_LENGTH, CastAnyPtr(BYTE, &cbKeyLength), sizeof(DWORD), &cbData, 0),
	//	__FILE__LINE__ _T("BCryptGetProperty"), _T("CWinCryptDecryptFilter::do_filter"));
	//_tprintf(_T("AES KeyLength=%d\n"), cbKeyLength);

	while (true)
	{
		buf2.set_BufferSize(__FILE__LINE__ sizeof(DWORD));
		read(buf2);
		if (0 == buf2.get_BufferSize())
			break;
		cbBlockLen = DerefAnyPtr(DWORD, buf2.get_Buffer());
		buf.set_BufferSize(__FILE__LINE__ cbBlockLen);
		read(buf);
		if (0 == buf.get_BufferSize())
			break;

		HandleError(BCryptDecrypt(keyH, buf.get_Buffer(), buf.get_BufferSize(), NULL, vIV.get_Buffer(), vIV.get_BufferSize(), NULL, 0, &cbResult, BCRYPT_BLOCK_PADDING),
			__FILE__LINE__ _T("BCryptEncrypt"), _T("CWinCryptDecryptFilter::do_filter"));
		buf1.set_BufferSize(__FILE__LINE__ cbResult);
		HandleError(BCryptDecrypt(keyH, buf.get_Buffer(), buf.get_BufferSize(), NULL, vIV.get_Buffer(), vIV.get_BufferSize(), buf1.get_Buffer(), cbResult, &cbResult, BCRYPT_BLOCK_PADDING),
			__FILE__LINE__ _T("BCryptEncrypt"), _T("CWinCryptDecryptFilter::do_filter"));
		buf1.set_BufferSize(__FILE__LINE__ cbResult);
		write(buf1);
	}
	HandleError(BCryptDestroyKey(keyH),
		__FILE__LINE__ _T("BCryptDestroyKey"), _T("CWinCryptDecryptFilter::do_filter"));
	HandleError(BCryptCloseAlgorithmProvider(algH, 0),
		__FILE__LINE__ _T("BCryptCloseAlgorithmProvider"), _T("CWinCryptDecryptFilter::do_filter"));
	return 0;
}

dword CWinCryptDecryptFilter::do_filter()
{
	CDataVectorT<CStringBuffer>::Iterator it = _algos.Begin();
	dword ix = 0;

	while (it)
	{
		if (_algo.Compare(*it, 0, CStringLiteral::cIgnoreCase) == 0)
		{
			switch (ix)
			{
			case 0:
				return _algo1();
			case 1:
				return _algo2();
			case 2:
				return _algo3();
			case 3:
				return _algo4();
			default:
				return 0;
			}
		}
		++it;
		++ix;
	}
	return 0;
}

