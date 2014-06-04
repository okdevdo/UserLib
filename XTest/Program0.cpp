/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#include "Program.h"
#include "Application.h"
#include "Configuration.h"
#include "DirectoryIterator.h"
#include "File.h"
#include "CryptFilter.h"
#include "okDateTime.h"
#include "HashBuffer.h"
#include "SHA1Buffer.h"

static void TestFile()
{
	OpenTestFile(_T("TestFile"));

	Ptr(CFile) f = OK_NEW_OPERATOR CStreamFile;
	CFile::TFileSize fsz;
	CFilePath fname(__FILE__LINE__ _T("Test.dat"));
	CFilePath fname1(__FILE__LINE__ _T("Test1.dat"));
	CFilePath fname2(__FILE__LINE__ _T("Test2.dat"));
	CStringBuffer s(__FILE__LINE__ _T("Testdaten"));

	try
	{
		f->Create(fname, false, CFile::UTF_8_Encoding);
#ifdef OK_SYS_WINDOWS
		f->Write(_T("\x016b\n"));
#endif
#ifdef OK_SYS_UNIX
		f->Write(_T("ü\n"));
#endif
		f->Write(_T("%s\n"), s.GetString());
		fsz = f->GetSize();
		f->Close();
		WriteTestFile(1, _T("In File '%s' wurden %lld bytes geschrieben."), fname.get_Path().GetString(), fsz);
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		WriteErrorTestFile(1, _T("%s"), ex->GetExceptionMessage().GetString());
	}
	WriteSuccessTestFile(1);

	try
	{
		CDirectoryIterator it;

		it.Open(fname);
		if (it)
		{
			WriteTestFile(2, _T("Directory-Eintrag fuer '%s' existiert."), fname.get_Path().GetString(), fsz);
			WriteTestFile(2, _T("Directory-Eintrag fuer '%s' weist %lld bytes aus."), it.get_Name().GetString(), it.get_FileSize());
			if (Castsqword(fsz) != it.get_FileSize())
				WriteErrorTestFile(2, _T("Sizes differ: FileSize=%lld, DirEntry=%lld"), fsz, it.get_FileSize());
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		WriteErrorTestFile(2, _T("%s"), ex->GetExceptionMessage().GetString());
	}
	WriteSuccessTestFile(2);

	CFile::TFileSize sz = s.GetSize();
	CPointer fbuf = CastAny(CPointer, TFalloc(Cast(dword, sz)));
	mbchar c;

	try
	{
		f->Open(fname, true, false, CFile::ScanFor_Encoding);
		f->Read(_T("%c\n"), &c, 1);
		f->Read(_T("%s\n"), fbuf, sz);
		f->Close();
		WriteTestFile(3, _T("Aus File '%s' wurden %lld bytes gelesen. Buffer=%s"), fname.get_Path().GetString(), sz, fbuf);

		if (
#ifdef OK_SYS_WINDOWS
			(c != 0x016b)
#endif
#ifdef OK_SYS_UNIX
			(c != _T('ü'))
#endif
			|| (s.Compare(CStringLiteral(fbuf)) != 0))
			WriteErrorTestFile(3, _T("Content differs."));
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		WriteErrorTestFile(3, _T("%s"), ex->GetExceptionMessage().GetString());
	}
	TFfree(fbuf);
	f->release();
	WriteSuccessTestFile(3);

	Ptr(CFile) d = OK_NEW_OPERATOR CDiskFile;
	CStringBuffer sbuf(__FILE__LINE__ _T("Test"));

	try
	{
		d->Create(fname2);
		d->Write(sbuf);
		fsz = d->GetSize();
		WriteTestFile(4, _T("In File '%s' wurden %lld bytes geschrieben."), fname2.get_Path().GetString(), fsz);
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		WriteErrorTestFile(4, _T("%s"), ex->GetExceptionMessage().GetString());
	}
	WriteSuccessTestFile(4);

	Ptr(CFile) d1 = OK_NEW_OPERATOR CDiskFile(*CastDynamicPtr(CDiskFile, d));

	try
	{
		sbuf.SetString(__FILE__LINE__ _T("Test1"));
		d1->Write(sbuf);
		fsz = d1->GetSize();
		d1->Close();
		WriteTestFile(5, _T("In File '%s' wurden %lld bytes geschrieben."), d1->GetPath().GetString(), fsz);
		d->Close();

		d->Open(fname2);
		d->Read(sbuf, 9);
		d->Close();
		WriteTestFile(5, _T("Aus File '%s' wurden %ld bytes gelesen. Buffer=%s"), fname2.get_Path().GetString(), sbuf.GetLength(), sbuf.GetString());

		if (sbuf.Compare(CStringLiteral(_T("TestTest1"))) != 0)
			WriteErrorTestFile(5, _T("Content differs."));
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		WriteErrorTestFile(5, _T("%s"), ex->GetExceptionMessage().GetString());
	}
	d1->release();
	d->release();
	WriteSuccessTestFile(5);

	CloseTestFile();
}

static void TestStringBuffer()
{
	OpenTestFile(_T("TestStringBuffer"));

	CStringBuffer testScanString(__FILE__LINE__ _T("TestString 123456 0xA67C chchch 1.2345"));
	mbchar vTestString[11];
	int vTestZahl;
	long long vTestZahl2;
	double vTestZahl3;
	mbchar vChars[6];
	sdword result;

	result = testScanString.ScanString(_T("%10s %d %llx %6c %lf"), vTestString, 11, &vTestZahl, &vTestZahl2, vChars, 6, &vTestZahl3);
	if (result != Cast(sdword, testScanString.GetLength()))
		WriteErrorTestFile(1, _T("result=%ld != testScanString.GetLength()=%ld"), result, testScanString.GetLength());
	if (s_strcmp(vTestString, _T("TestString")) != 0)
		WriteErrorTestFile(1, _T("vTestString=%s!=TestString"), vTestString);
	if (vTestZahl != 123456)
		WriteErrorTestFile(1, _T("vTestZahl=%d!=123456"), vTestZahl);
	if (vTestZahl2 != 0xA67C)
		WriteErrorTestFile(1, _T("vTestZahl2=%lld!=0xA67C"), vTestZahl2);
	if (s_strncmp(vChars, _T("chchch"), 6) != 0)
		WriteErrorTestFile(1, _T("Chars=%c%c%c%c%c%c!=chchch"), (int)(vChars[0]), (int)(vChars[1]), (int)(vChars[2]), (int)(vChars[3]), (int)(vChars[4]), (int)(vChars[5]));
	if (vTestZahl3 != 1.2345)
		WriteErrorTestFile(1, _T("vTestZahl3=%f!=1.2345"), vTestZahl3);
	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestConfiguration()
{
	OpenTestFile(_T("TestConfiguration"));

	CAbstractConfiguration::Values values(__FILE__LINE__ 16, 16);
	TListCnt cnt;

	cnt = theApp->config()->GetUserValues(_T("HttpServerService.AddUrls"), _T("XTest"), values);
	if (cnt != 2)
		WriteErrorTestFile(1, _T("cnt != 2"));

	cnt = theApp->config()->GetUserValues(_T("HttpServerService.LoggingFields"), _T("XTest"), values);
	if (cnt != 27)
		WriteErrorTestFile(1, _T("cnt != 27"));
	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestDateTime()
{
	OpenTestFile(_T("TestDateTime"));

	CStringBuffer sBuf0;
	CStringBuffer sBuf1;
	CStringBuffer sBuf2;
	CStringBuffer sBuf3;
	CDateTime t0;
	CDateTime t1;
	time_t tt0;
	time_t tt1;
	int dst;

	t0.GetTime(tt0);
	t0.GetTime(tt1, CDateTime::LocalTime, &dst);
	t1.SetTime(tt1, dst);

	if (((t1.GetDST() > 0) && ((tt0 + 7200) != tt1)) || ((t1.GetDST() == 0) && ((tt0 + 3600) != tt1)))
	{
		t0.GetTimeString(sBuf0);
		t1.GetTimeString(sBuf1);
		WriteErrorTestFile(1, _T("%s != %s"), sBuf0.GetString(), sBuf1.GetString());
	}

	t0.Now(CDateTime::LocalTime);
	t0.GetTime(tt0, CDateTime::LocalTime);
	t0.GetTime(tt1);
	t1.SetTime(tt1);

	if (((t0.GetDST() > 0) && ((tt0 - 7200) != tt1)) || ((t0.GetDST() == 0) && ((tt0 - 3600) != tt1)))
	{
		t0.GetTimeString(sBuf0);
		t1.GetTimeString(sBuf1);
		WriteErrorTestFile(1, _T("%s != %s"), sBuf0.GetString(), sBuf1.GetString());
	}

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static const struct {
	const char *msg;
	CConstPointer digest;
} tests_md2[] = {
	{ "", _T("8350e5a3e24c153df2275c9f80692773") },
	{ "a", _T("32ec01ec4a6dac72c0ab96fb34c0b5d1") },
	{ "message digest", _T("ab4f496bfb2a530b219ff33031fe06b0") },
	{ "abcdefghijklmnopqrstuvwxyz", _T("4e8ddff3650292ab5a4108c3aa47940b") },
	{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", _T("da33def2a42df13975352846c30338cd") },
	{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890", _T("d5976f79d83d3a0dc9806c3c66f3efd8") }
};

static const struct {
	const char *msg;
	CConstPointer digest;
} tests_md4[] = {
	{ "", _T("31d6cfe0d16ae931b73c59d7e0c089c0") },
	{ "a", _T("bde52cb31de33e46245e05fbdbd6fb24") },
	{ "abc", _T("a448017aaf21d8525fc10ae87aa6729d") },
	{ "message digest", _T("d9130a8164549fe818874806e1c7014b") },
	{ "abcdefghijklmnopqrstuvwxyz", _T("d79e1c308aa5bbcdeea8ed63df412da9") },
	{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", _T("043f8582f241db351ce627e153e7f0e4") },
	{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890", _T("e33b4ddc9c38f2199c3e7b164fcc0536") }
};

static const struct {
	const char *msg;
	CConstPointer digest;
} tests_md5[] = {
	{ "", _T("d41d8cd98f00b204e9800998ecf8427e") },
	{ "a", _T("0cc175b9c0f1b6a831c399e269772661") },
	{ "abc", _T("900150983cd24fb0d6963f7d28e17f72") },
	{ "message digest", _T("f96b697d7cb7938d525a2f31aaf161d0") },
	{ "abcdefghijklmnopqrstuvwxyz", _T("c3fcd3d76192e4007dfb496cca67e13b") },
	{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", _T("d174ab98d277d9f5a5611c2c9f419d9f") },
	{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890", _T("57edf4a22be3c955ac49da2e2107b67a") }
};

static const struct {
	const char *msg;
	CConstPointer digest;
} tests_sha1[] = {
	{ "", _T("da39a3ee5e6b4b0d3255bfef95601890afd80709") },
	{ "abc", _T("A9993E364706816ABA3E25717850C26C9CD0D89D") },
	{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", _T("84983E441C3BD26EBAAE4AA1F95129E5E54670F1") },
	{ "a*1000000", _T("34AA973CD4C4DAA4F61EEB2BDBAD27316534016F") }
};

static const struct {
	const char *msg;
	CConstPointer digest;
} tests_sha224[] = {
	{ "", _T("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f") },
	{ "abc", _T("23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7") },
	{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", _T("75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525") }
};

static const struct {
	const char *msg;
	CConstPointer digest;
} tests_sha256[] = {
	{ "", _T("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") },
	{ "abc", _T("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") },
	{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", _T("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1") }
};

static const struct {
	const char *msg;
	CConstPointer digest;
} tests_sha384[] = {
	{ "", _T("38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b") },
	{ "abc", _T("cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7") },
	{ "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", _T("09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039") }
};

static const struct {
	const char *msg;
	CConstPointer digest;
} tests_sha512[] = {
	{ "", _T("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e") },
	{ "abc", _T("ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f") },
	{ "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", _T("8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909") } 
};

#ifdef OK_COMP_MSC
#pragma warning(push)
#pragma warning(disable: 4267)
#endif
static void TestHashBuffer()
{
	OpenTestFile(_T("TestHashBuffer"));

	dword i;
	dword j;
	CHashBuffer buf;
	CStringBuffer tmp;

	for (i = 0; i < (dword)(sizeof(tests_md2) / sizeof(tests_md2[0])); i++) 
	{
		buf.Init();
		buf.Append(CastAny(BConstPointer, tests_md2[i].msg), strlen(tests_md2[i].msg));
		buf.Finish();
		tmp.SetString(__FILE__LINE__ tests_md2[i].digest);
		if (buf != tmp)
			WriteErrorTestFile(1, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	}
	WriteSuccessTestFile(1);

	for (i = 0; i < (dword)(sizeof(tests_md4) / sizeof(tests_md4[0])); i++)
	{
		buf.Init(1);
		buf.Append(CastAny(BConstPointer, tests_md4[i].msg), strlen(tests_md4[i].msg));
		buf.Finish();
		tmp.SetString(__FILE__LINE__ tests_md4[i].digest);
		if (buf != tmp)
			WriteErrorTestFile(2, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	}
	WriteSuccessTestFile(2);

	for (i = 0; i < (dword)(sizeof(tests_md5) / sizeof(tests_md5[0])); i++)
	{
		buf.Init(2);
		buf.Append(CastAny(BConstPointer, tests_md5[i].msg), strlen(tests_md5[i].msg));
		buf.Finish();
		tmp.SetString(__FILE__LINE__ tests_md5[i].digest);
		if (buf != tmp)
			WriteErrorTestFile(3, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	}
	WriteSuccessTestFile(3);

	for (i = 0; i < (dword)((sizeof(tests_sha1) / sizeof(tests_sha1[0])) - 1); i++)
	{
		buf.Init(3);
		buf.Append(CastAny(BConstPointer, tests_sha1[i].msg), strlen(tests_sha1[i].msg));
		buf.Finish();
		tmp.SetString(__FILE__LINE__ tests_sha1[i].digest);
		if (buf != tmp)
			WriteErrorTestFile(4, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	}
	buf.Init(3);
	for (j = 0; j < 1000000; ++j)
		buf.Append(CastAny(BConstPointer, "a"), 1);
	buf.Finish();
	tmp.SetString(__FILE__LINE__ tests_sha1[i].digest);
	if (buf != tmp)
		WriteErrorTestFile(4, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	WriteSuccessTestFile(4);

	{
		CSHA1Buffer buf1;

		for (i = 0; i < (dword)((sizeof(tests_sha1) / sizeof(tests_sha1[0])) - 1); i++)
		{
			CByteBuffer buf2(__FILE__LINE__ CastAny(BConstPointer, tests_sha1[i].msg), strlen(tests_sha1[i].msg));

			buf1.Init();
			buf1.Append(buf2);
			buf1.Finish();
			tmp.SetString(__FILE__LINE__ tests_sha1[i].digest);
			if (buf1 != tmp)
				WriteErrorTestFile(5, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
		}
		{
			CByteBuffer buf2(__FILE__LINE__ CastAny(BConstPointer, "a"), 1);

			buf1.Init();
			for (j = 0; j < 1000000; ++j)
				buf1.Append(buf2);
			buf1.Finish();
			tmp.SetString(__FILE__LINE__ tests_sha1[i].digest);
			if (buf1 != tmp)
				WriteErrorTestFile(5, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
		}
		WriteSuccessTestFile(5);
	}

	for (i = 0; i < (dword)(sizeof(tests_sha224) / sizeof(tests_sha224[0])); i++)
	{
		buf.Init(4);
		buf.Append(CastAny(BConstPointer, tests_sha224[i].msg), strlen(tests_sha224[i].msg));
		buf.Finish();
		tmp.SetString(__FILE__LINE__ tests_sha224[i].digest);
		if (buf != tmp)
			WriteErrorTestFile(6, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	}
	WriteSuccessTestFile(6);

	for (i = 0; i < (dword)(sizeof(tests_sha256) / sizeof(tests_sha256[0])); i++)
	{
		buf.Init(5);
		buf.Append(CastAny(BConstPointer, tests_sha256[i].msg), strlen(tests_sha256[i].msg));
		buf.Finish();
		tmp.SetString(__FILE__LINE__ tests_sha256[i].digest);
		if (buf != tmp)
			WriteErrorTestFile(7, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	}
	WriteSuccessTestFile(7);

	for (i = 0; i < (dword)(sizeof(tests_sha384) / sizeof(tests_sha384[0])); i++)
	{
		buf.Init(6);
		buf.Append(CastAny(BConstPointer, tests_sha384[i].msg), strlen(tests_sha384[i].msg));
		buf.Finish();
		tmp.SetString(__FILE__LINE__ tests_sha384[i].digest);
		if (buf != tmp)
			WriteErrorTestFile(8, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	}
	WriteSuccessTestFile(8);

	for (i = 0; i < (dword)(sizeof(tests_sha512) / sizeof(tests_sha512[0])); i++)
	{
		buf.Init(7);
		buf.Append(CastAny(BConstPointer, tests_sha512[i].msg), strlen(tests_sha512[i].msg));
		buf.Finish();
		tmp.SetString(__FILE__LINE__ tests_sha512[i].digest);
		if (buf != tmp)
			WriteErrorTestFile(9, _T("Digest Mismatch: got %s, expected %s"), buf.GetHexBuffer().GetString(), tmp.GetString());
	}
	WriteSuccessTestFile(9);

	CloseTestFile();
}
#ifdef OK_COMP_MSC
#pragma warning(pop)
#endif

class CByteFilterInput : public CFilterInput
{
public:
	CByteFilterInput(ConstRef(CByteBuffer) data): _data(data) {}
	virtual ~CByteFilterInput() {}

	virtual void open() {}
	virtual CFile::TFileSize size() { return _data.get_BufferSize(); }
	virtual void read(Ref(CByteBuffer) inputbuf) 
	{
		inputbuf = _data;
	}
	virtual void close() {}

private:
	CByteBuffer _data;
};

class CByteFilterOutput : public CFilterOutput
{
public:
	CByteFilterOutput() {}
	virtual ~CByteFilterOutput() {}

	__inline ConstRef(CByteBuffer) get_data() const { return _data; }

	virtual void open() {}
	virtual void write(Ref(CByteBuffer) outputbuf) 
	{
		_data = outputbuf;
	}
	virtual void close() {}

private:
	CByteBuffer _data;
};

typedef struct {
	int keylen;
	unsigned char key[32], pt[16], ct[16], iv[16];
} AES_TEST_STRUCT;

typedef struct {
	int keylen;
	int datalen;
	byte key[64], pt[48], ct[48], iv[16];
} AES_TEST_STRUCT1;

typedef struct {
	byte key[8], pt[8], ct[8], iv[8];
} Crypt8_TEST_STRUCT;

typedef struct {
	int keylen;
	byte key[32], pt[16], ct[16];
} Twofish_TEST_STRUCT;

static void TestAesCryptFilter(int algorithm, int testcase, ConstPtr(AES_TEST_STRUCT) test)
{
	CByteBuffer key(__FILE__LINE__ test->key, test->keylen);
	CByteBuffer iv(__FILE__LINE__ test->iv, 16);
	CByteBuffer data(__FILE__LINE__ test->pt, 16);
	CByteBuffer eout(__FILE__LINE__ test->ct, 16);

	CCppObjectPtr<CByteFilterInput> pInput1 = OK_NEW_OPERATOR CByteFilterInput(data);
	CCppObjectPtr<CByteFilterOutput> pOutput1 = OK_NEW_OPERATOR CByteFilterOutput();
	CCppObjectPtr<CCryptEncryptFilter> pEncryptFilter = OK_NEW_OPERATOR CCryptEncryptFilter(pInput1, pOutput1);

	pEncryptFilter->set_Algorithm(algorithm);
	pEncryptFilter->set_key(key);
	pEncryptFilter->set_iv(iv);
	pEncryptFilter->open();
	pEncryptFilter->do_filter();
	pEncryptFilter->close();

	if (eout != pOutput1->get_data())
	{
		CStringBuffer seout;
		CStringBuffer sdata;

		seout.convertToHex(eout);
		sdata.convertToHex(pOutput1->get_data());
		WriteErrorTestFile(testcase, _T("Cipher Mismatch: got %s, expected %s"), sdata.GetString(), seout.GetString());
	}

	CCppObjectPtr<CByteFilterInput> pInput2 = OK_NEW_OPERATOR CByteFilterInput(eout);
	CCppObjectPtr<CByteFilterOutput> pOutput2 = OK_NEW_OPERATOR CByteFilterOutput();
	CCppObjectPtr<CCryptDecryptFilter> pDecryptFilter = OK_NEW_OPERATOR CCryptDecryptFilter(pInput2, pOutput2);

	pDecryptFilter->set_Algorithm(algorithm);
	pDecryptFilter->set_key(key);
	pDecryptFilter->set_iv(iv);
	pDecryptFilter->open();
	pDecryptFilter->do_filter();
	pDecryptFilter->close();

	if (data != pOutput2->get_data())
	{
		CStringBuffer sdata1;
		CStringBuffer sdata2;

		sdata1.convertToHex(data);
		sdata2.convertToHex(pOutput2->get_data());
		WriteErrorTestFile(testcase, _T("Cipher Mismatch: got %s, expected %s"), sdata2.GetString(), sdata1.GetString());
	}
	WriteSuccessTestFile(testcase);
}

static void TestAesCryptFilter1(int algorithm, int testcase, ConstPtr(AES_TEST_STRUCT1) test)
{
	CByteBuffer key(__FILE__LINE__ test->key, test->keylen);
	CByteBuffer iv(__FILE__LINE__ test->iv, 16);
	CByteBuffer data(__FILE__LINE__ test->pt, test->datalen);
	CByteBuffer eout(__FILE__LINE__ test->ct, test->datalen);

	CCppObjectPtr<CByteFilterInput> pInput1 = OK_NEW_OPERATOR CByteFilterInput(data);
	CCppObjectPtr<CByteFilterOutput> pOutput1 = OK_NEW_OPERATOR CByteFilterOutput();
	CCppObjectPtr<CCryptEncryptFilter> pEncryptFilter = OK_NEW_OPERATOR CCryptEncryptFilter(pInput1, pOutput1);

	pEncryptFilter->set_Algorithm(algorithm);
	pEncryptFilter->set_key(key);
	pEncryptFilter->set_iv(iv);
	pEncryptFilter->open();
	pEncryptFilter->do_filter();
	pEncryptFilter->close();

	if (eout != pOutput1->get_data())
	{
		CStringBuffer seout;
		CStringBuffer sdata;

		seout.convertToHex(eout);
		sdata.convertToHex(pOutput1->get_data());
		WriteErrorTestFile(testcase, _T("Cipher Mismatch: got %s, expected %s"), sdata.GetString(), seout.GetString());
	}

	CCppObjectPtr<CByteFilterInput> pInput2 = OK_NEW_OPERATOR CByteFilterInput(eout);
	CCppObjectPtr<CByteFilterOutput> pOutput2 = OK_NEW_OPERATOR CByteFilterOutput();
	CCppObjectPtr<CCryptDecryptFilter> pDecryptFilter = OK_NEW_OPERATOR CCryptDecryptFilter(pInput2, pOutput2);

	pDecryptFilter->set_Algorithm(algorithm);
	pDecryptFilter->set_key(key);
	pDecryptFilter->set_iv(iv);
	pDecryptFilter->open();
	pDecryptFilter->do_filter();
	pDecryptFilter->close();

	if (data != pOutput2->get_data())
	{
		CStringBuffer sdata1;
		CStringBuffer sdata2;

		sdata1.convertToHex(eout);
		sdata2.convertToHex(pOutput2->get_data());
		WriteErrorTestFile(testcase, _T("Cipher Mismatch: got %s, expected %s"), sdata2.GetString(), sdata1.GetString());
	}
	WriteSuccessTestFile(testcase);
}

static void TestCryptFilter8(int algorithm, int testcase, ConstPtr(Crypt8_TEST_STRUCT) test)
{
	CByteBuffer key(__FILE__LINE__ test->key, 8);
	CByteBuffer data(__FILE__LINE__ test->pt, 8);
	CByteBuffer eout(__FILE__LINE__ test->ct, 8);
	CByteBuffer iv(__FILE__LINE__ test->iv, 8);

	CCppObjectPtr<CByteFilterInput> pInput1 = OK_NEW_OPERATOR CByteFilterInput(data);
	CCppObjectPtr<CByteFilterOutput> pOutput1 = OK_NEW_OPERATOR CByteFilterOutput();
	CCppObjectPtr<CCryptEncryptFilter> pEncryptFilter = OK_NEW_OPERATOR CCryptEncryptFilter(pInput1, pOutput1);

	pEncryptFilter->set_Algorithm(algorithm);
	pEncryptFilter->set_key(key);
	pEncryptFilter->set_iv(iv);
	pEncryptFilter->open();
	pEncryptFilter->do_filter();
	pEncryptFilter->close();

	if (eout != pOutput1->get_data())
	{
		CStringBuffer seout;
		CStringBuffer sdata;

		seout.convertToHex(eout);
		sdata.convertToHex(pOutput1->get_data());
		WriteErrorTestFile(testcase, _T("Cipher Mismatch: got %s, expected %s"), sdata.GetString(), seout.GetString());
	}

	CCppObjectPtr<CByteFilterInput> pInput2 = OK_NEW_OPERATOR CByteFilterInput(eout);
	CCppObjectPtr<CByteFilterOutput> pOutput2 = OK_NEW_OPERATOR CByteFilterOutput();
	CCppObjectPtr<CCryptDecryptFilter> pDecryptFilter = OK_NEW_OPERATOR CCryptDecryptFilter(pInput2, pOutput2);

	pDecryptFilter->set_Algorithm(algorithm);
	pDecryptFilter->set_key(key);
	pDecryptFilter->set_iv(iv);
	pDecryptFilter->open();
	pDecryptFilter->do_filter();
	pDecryptFilter->close();

	if (data != pOutput2->get_data())
	{
		CStringBuffer sdata1;
		CStringBuffer sdata2;

		sdata1.convertToHex(data);
		sdata2.convertToHex(pOutput2->get_data());
		WriteErrorTestFile(testcase, _T("Cipher Mismatch: got %s, expected %s"), sdata2.GetString(), sdata1.GetString());
	}
	WriteSuccessTestFile(testcase);
}

static void TestTwofishCryptFilter(int algorithm, int testcase, ConstPtr(Twofish_TEST_STRUCT) test)
{
	CByteBuffer key(__FILE__LINE__ test->key, test->keylen);
	CByteBuffer data(__FILE__LINE__ test->pt, 16);
	CByteBuffer eout(__FILE__LINE__ test->ct, 16);

	CCppObjectPtr<CByteFilterInput> pInput1 = OK_NEW_OPERATOR CByteFilterInput(data);
	CCppObjectPtr<CByteFilterOutput> pOutput1 = OK_NEW_OPERATOR CByteFilterOutput();
	CCppObjectPtr<CCryptEncryptFilter> pEncryptFilter = OK_NEW_OPERATOR CCryptEncryptFilter(pInput1, pOutput1);

	pEncryptFilter->set_Algorithm(algorithm);
	pEncryptFilter->set_key(key);
	pEncryptFilter->open();
	pEncryptFilter->do_filter();
	pEncryptFilter->close();

	if (eout != pOutput1->get_data())
	{
		CStringBuffer seout;
		CStringBuffer sdata;

		seout.convertToHex(eout);
		sdata.convertToHex(pOutput1->get_data());
		WriteErrorTestFile(testcase, _T("Cipher Mismatch: got %s, expected %s"), sdata.GetString(), seout.GetString());
	}

	CCppObjectPtr<CByteFilterInput> pInput2 = OK_NEW_OPERATOR CByteFilterInput(eout);
	CCppObjectPtr<CByteFilterOutput> pOutput2 = OK_NEW_OPERATOR CByteFilterOutput();
	CCppObjectPtr<CCryptDecryptFilter> pDecryptFilter = OK_NEW_OPERATOR CCryptDecryptFilter(pInput2, pOutput2);

	pDecryptFilter->set_Algorithm(algorithm);
	pDecryptFilter->set_key(key);
	pDecryptFilter->open();
	pDecryptFilter->do_filter();
	pDecryptFilter->close();

	if (data != pOutput2->get_data())
	{
		CStringBuffer sdata1;
		CStringBuffer sdata2;

		sdata1.convertToHex(data);
		sdata2.convertToHex(pOutput2->get_data());
		WriteErrorTestFile(testcase, _T("Cipher Mismatch: got %s, expected %s"), sdata2.GetString(), sdata1.GetString());
	}
	WriteSuccessTestFile(testcase);
}

static void LoadAesTestData(ConstRef(CFilePath) sname, AES_TEST_STRUCT** pData, DWPointer pDatasize)
{
	CCppObjectPtr<CFilterInput> pInput;
	CCppObjectPtr<CFilterOutput> pOutput;
	CCppObjectPtr<CFilter> pFilter;

	try
	{
		CDataVectorT<CStringBuffer> data(__FILE__LINE__ 256, 64);
		CDataVectorT<CStringBuffer>::Iterator it;

		pInput = OK_NEW_OPERATOR CFileFilterInput(sname);
		pOutput = OK_NEW_OPERATOR CStringVectorFilterOutput(data);
		pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput);

		WLong vcount;
		CPointer vkey;
		WULong vkeyl;
		AES_TEST_STRUCT *vdata = nullptr;

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();

		for (int run = 0; run < 2; ++run)
		{
			it = data.Begin();
			while (it)
			{
				CStringBuffer l(*it);
				CStringConstIterator itL(l);

				if ((itL[0] == _T('#')) || (itL[0] == 0))
				{
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("[ENCRYPT]")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("[DECRYPT]")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						if (NotPtrCheck(vdata))
							++vdata;
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("COUNT")), 5, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 0)
						++(*pDatasize);
					else
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatDecimal(vcount);
							if (vcount != 0)
							{
								if (NotPtrCheck(vdata))
									++vdata;
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("KEY")), 3, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 64)
									vkeyl = 64;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->key);
								vdata->keylen = vkeyl / 2;
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("IV")), 2, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 32)
									vkeyl = 32;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->iv);
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("PLAINTEXT")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 32)
									vkeyl = 32;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->pt);
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("CIPHERTEXT")), 10, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 32)
									vkeyl = 32;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->ct);
							}
						}
					}
					++it;
					continue;
				}

				++it;
			}
			if (run == 0)
			{
				if ((*pDatasize) > 0)
				{
					*pData = CastAnyPtr(AES_TEST_STRUCT, TFalloc(sizeof(AES_TEST_STRUCT)* (*pDatasize)));
					vdata = *pData;
				}
				else
					vdata = nullptr;
			}
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void LoadAesTestData1(ConstRef(CFilePath) sname, AES_TEST_STRUCT1** pData, DWPointer pDatasize)
{
	CCppObjectPtr<CFilterInput> pInput;
	CCppObjectPtr<CFilterOutput> pOutput;
	CCppObjectPtr<CFilter> pFilter;

	try
	{
		CDataVectorT<CStringBuffer> data(__FILE__LINE__ 256, 64);
		CDataVectorT<CStringBuffer>::Iterator it;

		pInput = OK_NEW_OPERATOR CFileFilterInput(sname);
		pOutput = OK_NEW_OPERATOR CStringVectorFilterOutput(data);
		pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput);

		WLong vcount;
		CPointer vkey;
		WULong vkeyl;
		AES_TEST_STRUCT1 *vdata = nullptr;

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();

		for (int run = 0; run < 2; ++run)
		{
			it = data.Begin();
			while (it)
			{
				CStringBuffer l(*it);
				CStringConstIterator itL(l);

				if ((itL[0] == _T('#')) || (itL[0] == 0))
				{
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("[ENCRYPT]")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("[DECRYPT]")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						if (NotPtrCheck(vdata))
							++vdata;
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("COUNT")), 5, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 0)
						++(*pDatasize);
					else
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatDecimal(vcount);
							if (vcount != 1)
							{
								if (NotPtrCheck(vdata))
									++vdata;
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("KEY")), 3, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 128)
									vkeyl = 128;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->key);
								vdata->keylen = vkeyl / 2;
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("I")), 1, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 32)
									vkeyl = 32;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->iv);
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("PT")), 2, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 96)
									vkeyl = 96;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->pt);
								vdata->datalen = vkeyl / 2;
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("CT")), 2, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 96)
									vkeyl = 96;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->ct);
								vdata->datalen = vkeyl / 2;
							}
						}
					}
					++it;
					continue;
				}

				++it;
			}
			if (run == 0)
			{
				if ((*pDatasize) > 0)
				{
					*pData = CastAnyPtr(AES_TEST_STRUCT1, TFalloc(sizeof(AES_TEST_STRUCT1)* (*pDatasize)));
					vdata = *pData;
				}
				else
					vdata = nullptr;
			}
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void LoadTestData8(ConstRef(CFilePath) sname, Crypt8_TEST_STRUCT** pData, DWPointer pDatasize)
{
	CCppObjectPtr<CFilterInput> pInput;
	CCppObjectPtr<CFilterOutput> pOutput;
	CCppObjectPtr<CFilter> pFilter;

	try
	{
		CDataVectorT<CStringBuffer> data(__FILE__LINE__ 256, 64);
		CDataVectorT<CStringBuffer>::Iterator it;

		pInput = OK_NEW_OPERATOR CFileFilterInput(sname);
		pOutput = OK_NEW_OPERATOR CStringVectorFilterOutput(data);
		pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput);

		WLong vcount;
		CPointer vkey;
		WULong vkeyl;
		Crypt8_TEST_STRUCT *vdata = nullptr;

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();

		for (int run = 0; run < 2; ++run)
		{
			it = data.Begin();
			while (it)
			{
				CStringBuffer l(*it);
				CStringConstIterator itL(l);

				if ((itL[0] == _T('#')) || (itL[0] == 0))
				{
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("[ENCRYPT]")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("[DECRYPT]")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						if (NotPtrCheck(vdata))
							++vdata;
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("COUNT")), 5, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 0)
						++(*pDatasize);
					else
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatDecimal(vcount);
							if (vcount != 0)
							{
								if (NotPtrCheck(vdata))
									++vdata;
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("KEY")), 3, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 16)
									vkeyl = 16;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->key);
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("KEYs")), 4, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 16)
									vkeyl = 16;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->key);
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("PLAINTEXT")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 16)
									vkeyl = 16;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->pt);
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("CIPHERTEXT")), 10, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 16)
									vkeyl = 16;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->ct);
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("IV")), 2, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 16)
									vkeyl = 16;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->iv);
							}
						}
					}
					++it;
					continue;
				}

				++it;
			}
			if (run == 0)
			{
				if ((*pDatasize) > 0)
				{
					*pData = CastAnyPtr(Crypt8_TEST_STRUCT, TFalloc(sizeof(Crypt8_TEST_STRUCT) * (*pDatasize)));
					vdata = *pData;
				}
				else
					vdata = nullptr;
			}
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void LoadTwofishTestData(ConstRef(CFilePath) sname, Twofish_TEST_STRUCT** pData, DWPointer pDatasize)
{
	CCppObjectPtr<CFilterInput> pInput;
	CCppObjectPtr<CFilterOutput> pOutput;
	CCppObjectPtr<CFilter> pFilter;

	try
	{
		CDataVectorT<CStringBuffer> data(__FILE__LINE__ 256, 64);
		CDataVectorT<CStringBuffer>::Iterator it;

		pInput = OK_NEW_OPERATOR CFileFilterInput(sname);
		pOutput = OK_NEW_OPERATOR CStringVectorFilterOutput(data);
		pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput);

		WLong vcount;
		CPointer vkey;
		WULong vkeyl;
		Twofish_TEST_STRUCT *vdata = nullptr;

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();

		for (int run = 0; run < 2; ++run)
		{
			it = data.Begin();
			while (it)
			{
				CStringBuffer l(*it);
				CStringConstIterator itL(l);

				if ((itL[0] == _T('#')) || (itL[0] == 0))
				{
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("COUNT")), 5, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 0)
						++(*pDatasize);
					else
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatDecimal(vcount);
							if (vcount != 1)
							{
								if (NotPtrCheck(vdata))
									++vdata;
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("KEY")), 3, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 64)
									vkeyl = 64;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->key);
								vdata->keylen = vkeyl / 2;
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("PLAINTEXT")), 9, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 32)
									vkeyl = 32;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->pt);
							}
						}
					}
					++it;
					continue;
				}
				if (l.Compare(CStringLiteral(_T("CIPHERTEXT")), 10, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0)
				{
					if (run == 1)
					{
						itL.Find(_T('='));
						if (!(itL.IsEnd()))
						{
							++itL;
							itL.EatWhite();
							itL.EatWord(vkey, vkeyl);
							if (NotPtrCheck(vdata))
							{
								if (vkeyl > 32)
									vkeyl = 32;
								s_cvthexstr2byteptr(vkey, vkeyl, vdata->ct);
							}
						}
					}
					++it;
					continue;
				}

				++it;
			}
			if (run == 0)
			{
				if ((*pDatasize) > 0)
				{
					*pData = CastAnyPtr(Twofish_TEST_STRUCT, TFalloc(sizeof(Twofish_TEST_STRUCT) * (*pDatasize)));
					vdata = *pData;
				}
				else
					vdata = nullptr;
			}
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void AesTestVectorData(int algorithm, Ref(int) testcase, CConstPointer tv_file)
{
	if (algorithm < 0)
	{
		CERR << _T("Algorithmus nicht gefunden!") << endl;
		return;
	}

	CFilePath fname(__FILE__LINE__ tv_file);
	CDirectoryIterator it;
	Ptr(AES_TEST_STRUCT) pData = nullptr;
	dword vDataSize = 0;

	try
	{
		it.Open(fname);

		while (it)
		{
			pData = nullptr;
			vDataSize = 0;
			LoadAesTestData(it.get_Path(), &pData, &vDataSize);
			for (dword i = 0; i < vDataSize; ++i)
				TestAesCryptFilter(algorithm, testcase++, pData + i);
			if (pData)
				TFfree(pData);
			++it;
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void AesTestVectorData1(int algorithm, Ref(int) testcase, CConstPointer tv_file)
{
	if (algorithm < 0)
	{
		CERR << _T("Algorithmus nicht gefunden!") << endl;
		return;
	}

	CFilePath fname(__FILE__LINE__ tv_file);
	CDirectoryIterator it;
	Ptr(AES_TEST_STRUCT1) pData = nullptr;
	dword vDataSize = 0;

	try
	{
		it.Open(fname);

		while (it)
		{
			pData = nullptr;
			vDataSize = 0;
			LoadAesTestData1(it.get_Path(), &pData, &vDataSize);
			for (dword i = 0; i < vDataSize; ++i)
				TestAesCryptFilter1(algorithm, testcase++, pData + i);
			if (pData)
				TFfree(pData);
			++it;
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void TestVectorData8(int algorithm, Ref(int) testcase, CConstPointer tv_file)
{
	if (algorithm < 0)
	{
		CERR << _T("Algorithmus nicht gefunden!") << endl;
		return;
	}

	CFilePath fname(__FILE__LINE__ tv_file);
	CDirectoryIterator it;
	Ptr(Crypt8_TEST_STRUCT) pData = nullptr;
	dword vDataSize = 0;

	try
	{
		it.Open(fname);

		while (it)
		{
			pData = nullptr;
			vDataSize = 0;
			LoadTestData8(it.get_Path(), &pData, &vDataSize);
			for (dword i = 0; i < vDataSize; ++i)
				TestCryptFilter8(algorithm, testcase++, pData + i);
			if (pData)
				TFfree(pData);
			++it;
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void TwofishTestVectorData(int algorithm, Ref(int) testcase, CConstPointer tv_file)
{
	if (algorithm < 0)
	{
		CERR << _T("Algorithmus nicht gefunden!") << endl;
		return;
	}

	CFilePath fname(__FILE__LINE__ tv_file);
	CDirectoryIterator it;
	Ptr(Twofish_TEST_STRUCT) pData = nullptr;
	dword vDataSize = 0;

	try
	{
		it.Open(fname);

		while (it)
		{
			pData = nullptr;
			vDataSize = 0;
			LoadTwofishTestData(it.get_Path(), &pData, &vDataSize);
			for (dword i = 0; i < vDataSize; ++i)
				TestTwofishCryptFilter(algorithm, testcase++, pData + i);
			if (pData)
				TFfree(pData);
			++it;
		}
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void TestCryptFilter()
{
	int i = 0;

	OpenTestFile(_T("TestCryptFilter"));

	AesTestVectorData(CCryptFilter::find_Algorithm(_T("AES-ECB")), i, _T("AesTestVector") PATH_SEPDEFARG _T("ECB*.rsp"));
	AesTestVectorData(CCryptFilter::find_Algorithm(_T("AES-CBC")), i, _T("AesTestVector") PATH_SEPDEFARG _T("CBC*.rsp"));
	AesTestVectorData(CCryptFilter::find_Algorithm(_T("AES-CFB")), i, _T("AesTestVector") PATH_SEPDEFARG _T("CFB128*.rsp"));
	AesTestVectorData(CCryptFilter::find_Algorithm(_T("AES-OFB")), i, _T("AesTestVector") PATH_SEPDEFARG _T("OFB*.rsp"));
	AesTestVectorData(CCryptFilter::find_Algorithm(_T("AES-CTR")), i, _T("AesTestVector") PATH_SEPDEFARG _T("CTR*.rsp"));
	AesTestVectorData1(CCryptFilter::find_Algorithm(_T("AES-XTS")), i, _T("AesTestVector") PATH_SEPDEFARG _T("XTS*.rsp"));
	TestVectorData8(CCryptFilter::find_Algorithm(_T("Blowfish-ECB")), i, _T("BlowfishTestVector8.rsp"));
	TwofishTestVectorData(CCryptFilter::find_Algorithm(_T("Twofish-ECB")), i, _T("TwofishTestVector*.rsp"));
	TestVectorData8(CCryptFilter::find_Algorithm(_T("DES-ECB")), i, _T("DESTestVector8.rsp"));
	TestVectorData8(CCryptFilter::find_Algorithm(_T("DES-ECB")), i, _T("DesTestVector") PATH_SEPDEFARG _T("TECB*.rsp"));
	TestVectorData8(CCryptFilter::find_Algorithm(_T("DES-CBC")), i, _T("DesTestVector") PATH_SEPDEFARG _T("TCBC*.rsp"));
	TestVectorData8(CCryptFilter::find_Algorithm(_T("DES-CFB")), i, _T("DesTestVector") PATH_SEPDEFARG _T("TCFB*.rsp"));
	TestVectorData8(CCryptFilter::find_Algorithm(_T("DES-OFB")), i, _T("DesTestVector") PATH_SEPDEFARG _T("TOFB*.rsp"));

	CloseTestFile();
}

void TestCppSources()
{
	COUT << _T("******************** TestFile *****************************\n") << endl;
	TestFile();
	COUT << _T("******************** TestStringBuffer ************************\n") << endl;
	TestStringBuffer();
	COUT << _T("******************** TestConfiguration *********************\n") << endl;
	TestConfiguration();
	COUT << _T("******************** TestDateTime **************************\n") << endl;
	TestDateTime();
	COUT << _T("******************** TestHashBuffer **************************\n") << endl;
	TestHashBuffer();
	COUT << _T("******************** TestCryptFilter **************************\n") << endl;
	TestCryptFilter();
}