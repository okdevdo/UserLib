/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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
#include "CPPS_PCH.H"
#include "CryptFilter.h"
#include "CIPHER.H"

IMPL_EXCEPTION(CCryptException, CBaseException)

CDataVectorT<CStringBuffer> CCryptFilter::_algos(__FILE__LINE__ 16, 16);

CCryptFilter::CCryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
CFilter(input, output),
_algo(0),
_key(),
_iv()
{
	ConstRef(CDataVectorT<CStringBuffer>) buf(get_Algorithms());
	CDataVectorT<CStringBuffer>::Iterator it = buf.Begin();

	if (!it)
		throw OK_NEW_OPERATOR CCryptException(__FILE__LINE__ _T("No algorithms defined in %s"), _T("CCryptFilter::CCryptFilter"));
}

CCryptFilter::~CCryptFilter()
{
}

void CCryptFilter::set_Algorithm(word ix)
{
	CDataVectorT<CStringBuffer>::Iterator it = _algos.Index(ix);

	if (!it)
		throw OK_NEW_OPERATOR CCryptException(__FILE__LINE__ _T("Index out of range in %s"), _T("CCryptFilter::set_Algorithm"));
	_algo = ix;
}

ConstRef(CDataVectorT<CStringBuffer>) CCryptFilter::get_Algorithms()
{
	if (0 == _algos.Count())
	{
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm DES - Mode ECB (DES-ECB)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm DES - Mode CBC (DES-CBC)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm DES - Mode CFB (DES-CFB)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm DES - Mode OFB (DES-OFB)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm DES3 - Mode ECB (DES3-ECB)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm AES - Mode ECB (AES-ECB)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm AES - Mode CBC (AES-CBC)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm AES - Mode CFB (AES-CFB)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm AES - Mode OFB (AES-OFB)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm AES - Mode CTR (AES-CTR)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm AES - Mode XTS (AES-XTS)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm Blowfish - Mode ECB (Blowfish-ECB)")));
		_algos.Append(CStringBuffer(__FILE__LINE__ _T("Algorithm Twofish - Mode ECB (Twofish-ECB)")));
	}
	return _algos;
}

word CCryptFilter::get_AlgorithmCount()
{
	ConstRef(CDataVectorT<CStringBuffer>) buf(get_Algorithms());

	return Castword(buf.Count());
}

CStringBuffer CCryptFilter::get_Algorithm(word ix)
{
	ConstRef(CDataVectorT<CStringBuffer>) buf(get_Algorithms());
	CDataVectorT<CStringBuffer>::Iterator it = buf.Index(ix);

	if (it)
		return *it;
	return CStringBuffer::null();
}

sword CCryptFilter::find_Algorithm(CConstPointer name, sword start)
{
	ConstRef(CDataVectorT<CStringBuffer>) buf(get_Algorithms());
	CDataVectorT<CStringBuffer>::Iterator it = buf.Begin();
	CStringBuffer sname(__FILE__LINE__ name);
	sword ix = 0;

	sname.ToUpperCase();
	while (it)
	{
		if (ix > start)
		{
			CStringBuffer tmp(*it);

			tmp.ToUpperCase();

			CStringConstIterator itL(tmp);

			itL.Find(sname);
			if (!(itL.IsEnd()))
				return ix;
		}
		++it;
		++ix;
	}
	return -1;
}

dword CCryptFilter::do_filter()
{
	return 0;
}

CCryptEncryptFilter::CCryptEncryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
CCryptFilter(input, output)
{
}

CCryptEncryptFilter::~CCryptEncryptFilter()
{
}

dword CCryptEncryptFilter::_algo_DES_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 8);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	DESSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	DESEncrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	DESFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_DES_CBC()
{
	SYMMETRIC_KEY_CBC key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CBC));
	CBCSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &DESSetup, &DESEncrypt, &DESDecrypt, &DESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CBCEncrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CBCFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_DES_CFB()
{
	SYMMETRIC_KEY_CFB key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CFB));
	CFBSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &DESSetup, &DESEncrypt, &DESDecrypt, &DESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CFBEncrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CFBFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_DES_OFB()
{
	SYMMETRIC_KEY_OFB key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_OFB));
	OFBSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &DESSetup, &DESEncrypt, &DESDecrypt, &DESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	OFBEncrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	OFBFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_DES3_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 8);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	DES3Setup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	DES3Encrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	DES3Finish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_AES_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 16);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	AESSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	AESEncrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	AESFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_AES_CBC()
{
	SYMMETRIC_KEY_CBC key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CBC));
	CBCSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CBCEncrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CBCFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_AES_CFB()
{
	SYMMETRIC_KEY_CFB key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CFB));
	CFBSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CFBEncrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CFBFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_AES_OFB()
{
	SYMMETRIC_KEY_OFB key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_OFB));
	OFBSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	OFBEncrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	OFBFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_AES_CTR()
{
	SYMMETRIC_KEY_CTR key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CTR));
	CTRSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, CTR_COUNTER_LITTLE_ENDIAN, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CTREncrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CTRFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_AES_XTS()
{
	SYMMETRIC_KEY_XTS key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_XTS));
	XTSSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	XTSEncrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), _iv.get_Buffer(), &key);
	write(out);
	XTSFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_Blowfish_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 8);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	BlowfishSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	BlowfishEncrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	BlowfishFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::_algo_Twofish_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 16);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	TwofishSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	TwofishEncrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	TwofishFinish(&key);
	return 0;
}

dword CCryptEncryptFilter::do_filter()
{
	typedef dword(CCryptEncryptFilter::*filter_algos_t)(void);
	static filter_algos_t _func[] = {
		&CCryptEncryptFilter::_algo_DES_ECB, 
		&CCryptEncryptFilter::_algo_DES_CBC,
		&CCryptEncryptFilter::_algo_DES_CFB,
		&CCryptEncryptFilter::_algo_DES_OFB,
		&CCryptEncryptFilter::_algo_DES3_ECB,
		&CCryptEncryptFilter::_algo_AES_ECB, 
		&CCryptEncryptFilter::_algo_AES_CBC,
		&CCryptEncryptFilter::_algo_AES_CFB,
		&CCryptEncryptFilter::_algo_AES_OFB,
		&CCryptEncryptFilter::_algo_AES_CTR,
		&CCryptEncryptFilter::_algo_AES_XTS,
		&CCryptEncryptFilter::_algo_Blowfish_ECB,
		&CCryptEncryptFilter::_algo_Twofish_ECB
	};

	if (_algo >= (sizeof(_func) / sizeof(filter_algos_t)))
		throw OK_NEW_OPERATOR CCryptException(__FILE__LINE__ _T("Index out of range in %s"), _T("CCryptEncryptFilter::do_filter"));

	filter_algos_t f = _func[_algo];

	return (this->*f)();
}

CCryptDecryptFilter::CCryptDecryptFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
CCryptFilter(input, output)
{
}

CCryptDecryptFilter::~CCryptDecryptFilter()
{
}

dword CCryptDecryptFilter::_algo_DES_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 8);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	DESSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	DESDecrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	DESFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_DES_CBC()
{
	SYMMETRIC_KEY_CBC key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CBC));
	CBCSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &DESSetup, &DESEncrypt, &DESDecrypt, &DESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CBCDecrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CBCFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_DES_CFB()
{
	SYMMETRIC_KEY_CFB key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CFB));
	CFBSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &DESSetup, &DESEncrypt, &DESDecrypt, &DESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CFBDecrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CFBFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_DES_OFB()
{
	SYMMETRIC_KEY_OFB key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_OFB));
	OFBSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &DESSetup, &DESEncrypt, &DESDecrypt, &DESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	OFBDecrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	OFBFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_DES3_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 8);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	DES3Setup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	DES3Decrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	DES3Finish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_AES_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 16);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	AESSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	AESDecrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	AESFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_AES_CBC()
{
	SYMMETRIC_KEY_CBC key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CBC));
	CBCSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CBCDecrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CBCFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_AES_CFB()
{
	SYMMETRIC_KEY_CFB key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CFB));
	CFBSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CFBDecrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CFBFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_AES_OFB()
{
	SYMMETRIC_KEY_OFB key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_OFB));
	OFBSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	OFBDecrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	OFBFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_AES_CTR()
{
	SYMMETRIC_KEY_CTR key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_CTR));
	CTRSetup(_iv.get_Buffer(), _iv.get_BufferSize(), _key.get_Buffer(), _key.get_BufferSize(), 0, CTR_COUNTER_LITTLE_ENDIAN, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	CTRDecrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), &key);
	write(out);
	CTRFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_AES_XTS()
{
	SYMMETRIC_KEY_XTS key;
	CByteBuffer in;
	CByteBuffer out;

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY_XTS));
	XTSSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &AESSetup, &AESEncrypt, &AESDecrypt, &AESFinish, &key);
	read(in);
	out.set_BufferSize(__FILE__LINE__ in.get_BufferSize());
	XTSDecrypt(in.get_Buffer(), out.get_Buffer(), in.get_BufferSize(), _iv.get_Buffer(), &key);
	write(out);
	XTSFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_Blowfish_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 8);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	BlowfishSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	BlowfishDecrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	BlowfishFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::_algo_Twofish_ECB()
{
	SYMMETRIC_KEY key;
	CByteBuffer in;
	CByteBuffer out(__FILE__LINE__ 16);

	s_memset(&key, 0, sizeof(SYMMETRIC_KEY));
	TwofishSetup(_key.get_Buffer(), _key.get_BufferSize(), 0, &key);
	read(in);
	TwofishDecrypt(in.get_Buffer(), out.get_Buffer(), &key);
	write(out);
	TwofishFinish(&key);
	return 0;
}

dword CCryptDecryptFilter::do_filter()
{
	typedef dword(CCryptDecryptFilter::*filter_algos_t)(void);
	static filter_algos_t _func[] = {
		&CCryptDecryptFilter::_algo_DES_ECB,
		&CCryptDecryptFilter::_algo_DES_CBC,
		&CCryptDecryptFilter::_algo_DES_CFB,
		&CCryptDecryptFilter::_algo_DES_OFB,
		&CCryptDecryptFilter::_algo_DES3_ECB,
		&CCryptDecryptFilter::_algo_AES_ECB,
		&CCryptDecryptFilter::_algo_AES_CBC,
		&CCryptDecryptFilter::_algo_AES_CFB,
		&CCryptDecryptFilter::_algo_AES_OFB,
		&CCryptDecryptFilter::_algo_AES_CTR,
		&CCryptDecryptFilter::_algo_AES_XTS,
		&CCryptDecryptFilter::_algo_Blowfish_ECB,
		&CCryptDecryptFilter::_algo_Twofish_ECB
	};

	if (_algo >= (sizeof(_func) / sizeof(filter_algos_t)))
		throw OK_NEW_OPERATOR CCryptException(__FILE__LINE__ _T("Index out of range in %s"), _T("CCryptDecryptFilter::do_filter"));

	filter_algos_t f = _func[_algo];

	return (this->*f)();
}

