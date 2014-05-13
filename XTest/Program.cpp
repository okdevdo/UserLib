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
#include "Debug.h"
#include "File.h"
#include "Filter.h"
#include "DirectoryIterator.h"


#if 0
static void TestSpecialBlowfishImport(void)
{
	CFilePath finp(__FILE__LINE__ _T("BlowfishTestVectorRaw.txt"));
	CFilePath foutp(__FILE__LINE__ _T("BlowfishTestVector8.rsp"));
	Ptr(CFile) inp = OK_NEW_OPERATOR CStreamFile(finp, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CFile) outp = OK_NEW_OPERATOR CStreamFile;
	CDataVectorT<CStringBuffer> data(__FILE__LINE__ 256, 128);
	CDataVectorT<CStringBuffer>::Iterator it;

	outp->Create(foutp, false, CFile::BinaryFile_NoEncoding);

	Ptr(CFilterInput) ffinp = OK_NEW_OPERATOR CFileFilterInput(inp);
	Ptr(CFilterOutput) ffoutp = OK_NEW_OPERATOR CStringVectorFilterOutput(data);
	Ptr(CFilter) ffilter = OK_NEW_OPERATOR CLineReadFilter(ffinp, ffoutp);
	bool bFirst = true;
	int r = 0;

	ffilter->open();
	ffilter->do_filter();
	ffilter->close();

	ffilter->release();
	ffoutp->release();
	ffinp->release();
	inp->release();

	it = data.Begin();
	while (it)
	{
		CStringBuffer tmp(*it);
		CStringConstIterator it1(tmp);
		CPointer keyD;
		WULong keyDLen;
		CPointer ptD;
		WULong ptDLen;
		CPointer ctD;
		WULong ctDLen;

		if (bFirst)
		{
			CDateTime now(CDateTime::LocalTime);
			CStringBuffer t;

			bFirst = false;
			++it;
			outp->Write(_T("# TestVector for Blowfish ECB\r\n# Generated "));
			now.GetTimeString(t);
			t.DeleteString(19, t.GetLength() - 19);
			outp->Write(t);
			outp->Write(_T("\r\n"));
			continue;
		}
		it1.EatWord(keyD, keyDLen);
		it1.EatWhite();
		it1.EatWord(ptD, ptDLen);
		it1.EatWhite();
		it1.EatWord(ctD, ctDLen);

		outp->Write(_T("\r\nCOUNT = %d\r\n"), r);
		outp->Write(_T("KEY = %.*s\r\n"), keyDLen, keyD);
		outp->Write(_T("PLAINTEXT = %.*s\r\n"), ptDLen, ptD);
		outp->Write(_T("CIPHERTEXT = %.*s\r\n"), ctDLen, ctD);

		++r;
		++it;
	}
	outp->Close();
	outp->release();
}
#endif

#if 0
static void TestSpecialDESImport(void)
{
	CFilePath finp(__FILE__LINE__ _T("DESTestVectorRaw.txt"));
	CFilePath foutp(__FILE__LINE__ _T("DESTestVector8.rsp"));
	Ptr(CFile) inp = OK_NEW_OPERATOR CStreamFile(finp, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CFile) outp = OK_NEW_OPERATOR CStreamFile;
	CDataVectorT<CStringBuffer> data(__FILE__LINE__ 256, 128);
	CDataVectorT<CStringBuffer>::Iterator it;

	outp->Create(foutp, false, CFile::BinaryFile_NoEncoding);

	Ptr(CFilterInput) ffinp = OK_NEW_OPERATOR CFileFilterInput(inp);
	Ptr(CFilterOutput) ffoutp = OK_NEW_OPERATOR CStringVectorFilterOutput(data);
	Ptr(CFilter) ffilter = OK_NEW_OPERATOR CLineReadFilter(ffinp, ffoutp);
	bool bFirst = true;
	int r = 0;

	ffilter->open();
	ffilter->do_filter();
	ffilter->close();

	ffilter->release();
	ffoutp->release();
	ffinp->release();
	inp->release();

	it = data.Begin();
	while (it)
	{
		CStringBuffer tmp(*it);
		CStringConstIterator it1(tmp);
		CPointer keyD;
		WULong keyDLen;
		CPointer ptD;
		WULong ptDLen;
		CPointer ctD;
		WULong ctDLen;

		if (bFirst)
		{
			CDateTime now(CDateTime::LocalTime);
			CStringBuffer t;

			bFirst = false;
			++it;
			outp->Write(_T("# TestVector for DES\r\n# Generated "));
			now.GetTimeString(t);
			t.DeleteString(19, t.GetLength() - 19);
			outp->Write(t);
			outp->Write(_T("\r\n"));
			continue;
		}
		it1.EatWord(keyD, keyDLen);
		it1.EatWhite();
		it1.EatWord(ptD, ptDLen);
		it1.EatWhite();
		it1.EatWord(ctD, ctDLen);

		outp->Write(_T("\r\nCOUNT = %d\r\n"), r);
		outp->Write(_T("KEY = %.*s\r\n"), keyDLen, keyD);
		outp->Write(_T("PLAINTEXT = %.*s\r\n"), ptDLen, ptD);
		outp->Write(_T("CIPHERTEXT = %.*s\r\n"), ctDLen, ctD);

		++r;
		++it;
	}
	outp->Close();
	outp->release();
}
#endif

static void TestSpecial(void)
{
	CFilePath fname1(__FILE__LINE__ _T("C:\\DEV\\Projects\\UserLib\\"));
	CFilePath fname2(__FILE__LINE__ _T("vs"));

	try
	{
		CDirectoryIterator::SetCurrentDirectory(fname1);
		MetaProjectCreateProjectFilesXML();
		fname2.MakeDirectory();
		if (CDirectoryIterator::DirectoryExists(fname2) < 0)
			CDirectoryIterator::MakeDirectory(fname2);
		CDirectoryIterator::SetCurrentDirectory(fname2);
		CDirectoryIterator::CopyFile(CFilePath(__FILE__LINE__ _T("..\\ProjectFiles.xml")), CFilePath(__FILE__LINE__ _T("ProjectFiles.xml")));
		MetaProjectCreateProjectFilesVS(_T("v100"));
		MetaProjectCreateProjectFilesVS(_T("v110"));
		MetaProjectCreateProjectFilesVS(_T("v120"));
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static Ptr(CFile) _TestFile = NULL;
static WBool _AcceptsTests = false;
static WBool _HasErrors = false;

void OpenTestFile(CConstPointer _TestFunction)
{
	if (NotPtrCheck(_TestFile))
	{
		_TestFile->Close();
		_TestFile->release();
		_TestFile = NULL;
	}

	CFilePath fname(__FILE__LINE__ _T("LogFiles"));

	fname.MakeDirectory();

	try
	{
		if (CDirectoryIterator::DirectoryExists(fname) < 0)
			CDirectoryIterator::MakeDirectory(fname);
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		return;
	}

	CStringBuffer sname(__FILE__LINE__ _TestFunction);

	sname.AppendString(_T(".test.dat"));
	fname.set_Filename(sname);

	try
	{
		if (CDirectoryIterator::FileExists(fname))
			CDirectoryIterator::RemoveFile(fname);
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		return;
	}

	CDateTime now(CDateTime::LocalTime);
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%04d/%02d/%02d-%02d:%02d:%02d"),
		Cast(int, now.GetYears()), Cast(int, now.GetMonths()), Cast(int, now.GetDays()),
		Cast(int, now.GetHours()), Cast(int, now.GetMinutes()), Cast(int, now.GetSeconds()));

	_TestFile = new CStreamFile;
	try
	{
		_TestFile->Create(fname, false, CFile::BinaryFile_NoEncoding);
		_TestFile->Write(_T("**************** %s [%s] *****************\r\n"), _TestFunction, tmp.GetString());
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		_TestFile->release();
		_TestFile = NULL;
		return;
	}
	_HasErrors = false;
}

sdword WriteTestFile(int testcase, CConstPointer format, ...)
{
	if (ConstStrEmpty(format))
	{
		CERR << _T("In WriteTestFile is format empty.") << endl;
		return -1;
	}

	va_list argList;
	sdword ret;
	CStringBuffer tmp;
	CStringBuffer tmp1;

	va_start(argList, format);
	ret = tmp.FormatString(__FILE__LINE__ format, argList);
	va_end(argList);
	tmp1.FormatString(__FILE__LINE__ _T("** TEST%d: "), testcase);
	tmp.PrependString(tmp1);
	tmp.AppendString(_T("\r\n"));
	if (NotPtrCheck(_TestFile))
	{
		try
		{
			_TestFile->Write(tmp);
		}
		catch (CBaseException* ex)
		{
			CERR << ex->GetExceptionMessage() << endl;
			return -1;
		}
	}
	return ret;
}

sdword WriteErrorTestFile(int testcase, CConstPointer format, ...)
{
	_HasErrors = true;
	if (ConstStrEmpty(format))
	{
		CERR << _T("In WriteErrorTestFile is format empty.") << endl;
		return -1;
	}

	va_list argList;
	sdword ret;
	CStringBuffer tmp;
	CStringBuffer tmp1;

	va_start(argList, format);
	ret = tmp.FormatString(__FILE__LINE__ format, argList);
	va_end(argList);
	tmp1.FormatString(__FILE__LINE__ _T("** ERROR TEST%d: "), testcase);
	tmp.PrependString(tmp1);
	tmp.AppendString(_T("\r\n"));
	if (NotPtrCheck(_TestFile))
	{
		try
		{
			_TestFile->Write(tmp);
		}
		catch (CBaseException* ex)
		{
			CERR << ex->GetExceptionMessage() << endl;
			return -1;
		}
	}
	return ret;
}

sdword WriteSuccessTestFile(int testcase)
{
	if (_HasErrors)
	{
		_HasErrors = false;
		return -1;
	}

	sdword ret;
	CStringBuffer tmp;

	ret = tmp.FormatString(__FILE__LINE__ _T("** SUCCESS TEST%d: Alle Tests wurden korrekt durchgefuehrt.\r\n"), testcase);
	if (NotPtrCheck(_TestFile))
	{
		try
		{
			_TestFile->Write(tmp);
		}
		catch (CBaseException* ex)
		{
			CERR << ex->GetExceptionMessage() << endl;
			return -1;
		}
	}
	return ret;
}

void CloseTestFile()
{
	if (PtrCheck(_TestFile))
		return;

	CFilePath fname(_TestFile->GetPath());
	CFilePath fname1;
	CByteBuffer bBuf1;
	CByteBuffer bBuf2;
	Ptr(CFile) vTestFile = NULL;

	try
	{
		_TestFile->Write(_T("**************** End of Test *****************\r\n"));
		_TestFile->Close();
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		goto _exit;
	}

	fname1 = fname;
	fname1.set_FullExtension(_T(".cmp.dat"));

	try
	{
		if (_AcceptsTests)
		{
			if (CDirectoryIterator::FileExists(fname1))
				CDirectoryIterator::RemoveFile(fname1);
			CDirectoryIterator::CopyFile(fname, fname1);
		}
		else if (CDirectoryIterator::FileExists(fname1))
		{
			bool bFirst = true;

			vTestFile = new CStreamFile(fname1, true, false, CFile::BinaryFile_NoEncoding);
			_TestFile->Open(fname, true, false, CFile::BinaryFile_NoEncoding);

			if (vTestFile->GetSize() != _TestFile->GetSize())
			{
				CERR << _T("** ERROR: Files differ in Size: ") << _TestFile->GetPath().get_Path() << _T(", ") << vTestFile->GetPath().get_Path() << endl;
				goto _exit;
			}
			bBuf1.set_BufferSize(__FILE__LINE__ 1024);
			vTestFile->Read(bBuf1);
			bBuf2.set_BufferSize(__FILE__LINE__ 1024);
			_TestFile->Read(bBuf2);
			while ((bBuf1.get_BufferSize() > 0) && (bBuf2.get_BufferSize() > 0))
			{
				if (bBuf1.get_BufferSize() != bBuf2.get_BufferSize())
				{
					CERR << _T("** ERROR: Buffers have different size: ") << _TestFile->GetPath().get_Path() << _T(", ") << vTestFile->GetPath().get_Path() << endl;
					goto _exit;
				}
				if (bFirst)
				{
					int pos1 = bBuf1.find_Char('\r');
					int pos2 = bBuf2.find_Char('\r');

					if ((pos1 < 0) || (pos2 < 0))
					{
						CERR << _T("** ERROR: First line could not be detected: ") << _TestFile->GetPath().get_Path() << _T(", ") << vTestFile->GetPath().get_Path() << endl;
						goto _exit;
					}
					if ((pos1 != pos2) || (memcmp(bBuf1.get_Buffer() + pos1, bBuf2.get_Buffer() + pos1, bBuf1.get_BufferSize() - pos1) != 0))
					{
						CERR << _T("** ERROR: Files differ in Bytes: ") << _TestFile->GetPath().get_Path() << _T(", ") << vTestFile->GetPath().get_Path() << endl;
						goto _exit;
					}
					bFirst = false;
				}
				else if (bBuf1 != bBuf2)
				{
					CERR << _T("** ERROR: Files differ in Bytes: ") << _TestFile->GetPath().get_Path() << _T(", ") << vTestFile->GetPath().get_Path() << endl;
					goto _exit;
				}
				bBuf1.set_BufferSize(__FILE__LINE__ 1024);
				vTestFile->Read(bBuf1);
				bBuf2.set_BufferSize(__FILE__LINE__ 1024);
				_TestFile->Read(bBuf2);
			}
		}
		else
			CERR << _T("** ERROR: ") << fname1.get_Path() << _T(" does not exist.") << endl;
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
_exit:
	if (NotPtrCheck(vTestFile))
	{
		vTestFile->Close();
		vTestFile->release();
	}
	_TestFile->Close();
	_TestFile->release();
	_TestFile = NULL;
}

sword __stdcall TestSortFunc(ConstPointer pa, ConstPointer pb)
{
	if (pa < pb)
		return -1;
	if (pa > pb)
		return 1;
	return 0;
}

sword __stdcall TestSortFuncUInt(ConstPointer pa, ConstPointer pb)
{
	unsigned int* ppa = CastAnyPtr(unsigned int, CastMutable(Pointer, pa));
	unsigned int* ppb = CastAnyPtr(unsigned int, CastMutable(Pointer, pb));

	if (*ppa < *ppb)
		return -1;
	if (*ppa > *ppb)
		return 1;
	return 0;
}

sword __stdcall TestSortFuncULongPointer(ConstPointer pa, ConstPointer pb)
{
	ULongPointer* ppa = CastAnyPtr(ULongPointer, CastMutable(Pointer, pa));
	ULongPointer* ppb = CastAnyPtr(ULongPointer, CastMutable(Pointer, pb));

	if (*ppa < *ppb)
		return -1;
	if (*ppa > *ppb)
		return 1;
	return 0;
}

int __cdecl TestCompareSRand(const void * pA, const void * pB)
{
	unsigned int* piA = (unsigned int*)pA;
	unsigned int* piB = (unsigned int*)pB;

	if (*piA < *piB)
		return -1;
	if (*piA > *piB)
		return 1;
	return 0;
}

int __cdecl TestCompareSRand64(const void * pA, const void * pB)
{
	ULongPointer* piA = (ULongPointer *)pA;
	ULongPointer* piB = (ULongPointer *)pB;

	if (*piA < *piB)
		return -1;
	if (*piA > *piB)
		return 1;
	return 0;
}

void __stdcall TestDeleteFunc(ConstPointer data, Pointer context)
{
}

void __stdcall VectorEmptyDeleteFunc(ConstPointer data, Pointer context)
{
}

void showVisitor(Item v)
{
	v.show(COUT);
}

class TestApplication : public CApplication
{
public:
	TestApplication() :
		CApplication(CStringBuffer(__FILE__LINE__ _T("XTest"))),
		m_AnyOption(false),
		m_AllOption(false),
		m_Output(false),
		m_sOutput(),
		m_help(false),
		m_TestDataStructures(false),
		m_TestCppSources(false),
		m_TestCSources(false),
		m_TestDirectoryIterator(false),
		m_TestLinkedByteBuffer(false),
		m_ScanDirectory(false),
		m_sScanDirectory(__FILE__LINE__ 16, 16, VectorEmptyDeleteFunc),
		m_ScanDirectoryPattern(false),
		m_sScanDirectoryPattern(),
		m_TestDBase(false),
		m_TestThreads(false),
		m_TestTCPServer(false),
		m_TestTCPClient(false),
		m_sTestTCPClient(),
		m_TestTCPServerKeepAlive(false),
		m_TestTCPClientKeepAlive(false),
		m_sTestTCPClientKeepAlive(),
		m_TestConsole(false),
		m_TestSQL(false),
#ifdef OK_SYS_WINDOWS
		m_TestHttp1(false),
		m_sTestHttp1(__FILE__LINE__ 16, 16, VectorEmptyDeleteFunc),
		m_TestHttp2(false),
		m_sTestHttp2(),
#endif
#ifdef OK_COMP_MSC
		m_TestAsyncFile(false),
		m_TestAsyncTCPClient(false),
		m_TestSqLite3ServiceClient(false),
		m_TestAsyncTCPServer(false),
		m_TestEventLog(false),
#endif
		m_TestWinSources(false),
		m_TestFilter(false),
		m_TestFTP(false),
		m_TestSAX(false),
		m_TestSSL(false),
		m_TestSSLServer(false),
		m_DebugServer(false),
		m_StopDebugServer(false),
		m_Debug(false),
		m_Special(false)
	{
		COptionCallback<TestApplication> cb(this, &TestApplication::handleHelp);

		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("All"))
			.shortName(_T("a"))
			.category(_T("Test"))
			.description(_T("Test All Options.")));
		addOption(COption(_T("Output"))
			.shortName(_T("o"))
			.argument(_T("outputfile"), false)
			.category(_T("Util"))
			.description(_T("Write all 'stdout' to 'outputfile'. 'outputfile' defaults to 'XTestYYYYMMDDHHMMSS.log', if not given.")));
		addOption(COption(_T("TestCppSources"))
			.shortName(_T("tcpp"))
			.category(_T("Test"))
			.description(_T("Test CppSources.")));
		addOption(COption(_T("TestCSources"))
			.shortName(_T("tcs"))
			.category(_T("Test"))
			.description(_T("Test CSources.")));
		addOption(COption(_T("TestWinSources"))
			.shortName(_T("twin"))
			.category(_T("Test"))
			.description(_T("Test WinSources.")));
		addOption(COption(_T("TestDataStructures"))
			.shortName(_T("tds"))
			.category(_T("Test"))
			.description(_T("Test Data Structures.")));
		addOption(COption(_T("TestLinkedByteBuffer"))
			.shortName(_T("tlbb"))
			.category(_T("Test"))
			.description(_T("Test LinkedByteBuffer.")));
		addOption(COption(_T("TestDirectoryIterator"))
			.shortName(_T("tdi"))
			.category(_T("Test"))
			.description(_T("Test DirectoryIterator.")));
		addOption(COption(_T("ScanDirectory"))
			.shortName(_T("sdd"))
			.argument(_T("dir"))
			.category(_T("Scan Directory"))
			.description(_T("Scan Directory."))
			.repeatable(true));
		addOption(COption(_T("SDPattern"))
			.shortName(_T("sdp"))
			.argument(_T("pattern"))
			.category(_T("Scan Directory"))
			.description(_T("Sets Pattern for Scan Directory.")));
		addOption(COption(_T("TestDBase"))
			.shortName(_T("tdb"))
			.category(_T("Test"))
			.description(_T("Test DBase.")));
		addOption(COption(_T("TestThreads"))
			.shortName(_T("tth"))
			.category(_T("Test"))
			.description(_T("Test Threads.")));
		addOption(COption(_T("TestTCPServer"))
			.shortName(_T("tcps"))
			.category(_T("Test"))
			.description(_T("Test TCPServer.")));
		addOption(COption(_T("TestTCPClient"))
			.shortName(_T("tcpc"))
			.argument(_T("command"), false)
			.category(_T("Test"))
			.description(_T("Test TCPClient.")));
		addOption(COption(_T("TestKeepAliveTCPServer"))
			.shortName(_T("tcpks"))
			.category(_T("Test"))
			.description(_T("Test TCPServerKeepAlive.")));
		addOption(COption(_T("TestKeepAliveTCPClient"))
			.shortName(_T("tcpkc"))
			.argument(_T("command"), false)
			.category(_T("Test"))
			.description(_T("Test TCPClientKeepAlive.")));
		addOption(COption(_T("TestConsole"))
			.shortName(_T("tcon"))
			.category(_T("Test"))
			.description(_T("Test Console.")));
		addOption(COption(_T("TestSQL"))
			.shortName(_T("tsql"))
			.category(_T("Test"))
			.description(_T("Test SQL.")));
#ifdef OK_SYS_WINDOWS
		addOption(COption(_T("TestHttp1"))
			.shortName(_T("thttp1"))
			.argument(_T("url"), true)
			.category(_T("Test"))
			.description(_T("Test Http1."))
			.repeatable(true));
		addOption(COption(_T("TestHttp2"))
			.shortName(_T("thttp2"))
			.argument(_T("importfile"), true)
			.category(_T("Test"))
			.description(_T("Test Http2. Definition of 'importfile' is one url per line.")));
#endif
#ifdef OK_COMP_MSC
		addOption(COption(_T("TestAsyncFile"))
			.shortName(_T("tasyncf"))
			.category(_T("Test"))
			.description(_T("Test File Async Class.")));
		addOption(COption(_T("TestAsyncTCPClient"))
			.shortName(_T("tasyncc"))
			.category(_T("Test"))
			.description(_T("Test TCPClient Async Class.")));
		addOption(COption(_T("TestSqLite3ServiceClient"))
			.shortName(_T("tssc"))
			.category(_T("Test"))
			.description(_T("Test SqLite3Service Client.")));
		addOption(COption(_T("TestAsyncTCPServer"))
			.shortName(_T("tasyncs"))
			.category(_T("Test"))
			.description(_T("Test TCPServer Async Class.")));
		addOption(COption(_T("TestEventLog"))
			.shortName(_T("tevlog"))
			.category(_T("Test"))
			.description(_T("Test EventLog Class.")));
#endif
		addOption(COption(_T("TestFilter"))
			.shortName(_T("tfl"))
			.category(_T("Test"))
			.description(_T("Test filter.")));
		addOption(COption(_T("Special"))
			.shortName(_T("sp"))
			.category(_T("Util"))
			.description(_T("Test special.")));
		addOption(COption(_T("TestFTP"))
			.shortName(_T("tftp"))
			.category(_T("Test"))
			.description(_T("Test FTP.")));
		addOption(COption(_T("TestSAX"))
			.shortName(_T("tsax"))
			.category(_T("Test"))
			.description(_T("Test SAX.")));
		addOption(COption(_T("TestSSL"))
			.shortName(_T("tssl"))
			.category(_T("Test"))
			.description(_T("Test OpenSSL SSL client.")));
		addOption(COption(_T("TestSSLServer"))
			.shortName(_T("tssls"))
			.category(_T("Test"))
			.description(_T("Test OpenSSL SSL server.")));
		addOption(COption(_T("DebugServer"))
			.shortName(_T("start"))
			.category(_T("Util"))
			.description(_T("Start Debug Server.")));
		addOption(COption(_T("StopDebugServer"))
			.shortName(_T("stop"))
			.category(_T("Util"))
			.description(_T("Stop Debug Server.")));
		addOption(COption(_T("Debug"))
			.shortName(_T("debug"))
			.category(_T("Util"))
			.description(_T("Enable TCP/IP debugging for the client.")));
		addOption(COption(_T("AcceptTest"))
			.shortName(_T("at"))
			.category(_T("Util"))
			.description(_T("Accept Tests of selected options.")));
	}

	~TestApplication()
	{
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral& value)
	{
		m_help = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		COUT << _T("Option = ") << name << _T(", value = ") << value << endl;

		if (name == CStringLiteral(_T("Output")))
		{
			m_Output = true;
			m_sOutput = value;
		}
		if (name == CStringLiteral(_T("AcceptTest")))
			_AcceptsTests = true;
		if (name == CStringLiteral(_T("All")))
			m_AllOption = true;
		if (name == CStringLiteral(_T("TestCppSources")))
			m_TestCppSources = true;
		if (name == CStringLiteral(_T("Special")))
			m_Special = true;
		if (name == CStringLiteral(_T("TestCSources")))
			m_TestCSources = true;
		if (name == CStringLiteral(_T("TestWinSources")))
			m_TestWinSources = true;
		if (name == CStringLiteral(_T("TestDataStructures")))
			m_TestDataStructures = true;
		if (name == CStringLiteral(_T("TestLinkedByteBuffer")))
			m_TestLinkedByteBuffer = true;
		if (name == CStringLiteral(_T("TestDirectoryIterator")))
			m_TestDirectoryIterator = true;
		if (name == CStringLiteral(_T("ScanDirectory")))
		{
			m_ScanDirectory = true;
			if (!(value.IsEmpty()))
				m_sScanDirectory.Append(value);
		}
		if (name == CStringLiteral(_T("SDPattern")))
		{
			m_ScanDirectoryPattern = true;
			m_sScanDirectoryPattern = value;
		}
		if (name == CStringLiteral(_T("TestDBase")))
			m_TestDBase = true;
		if (name == CStringLiteral(_T("TestThreads")))
			m_TestThreads = true;
		if (name == CStringLiteral(_T("TestTCPServer")))
			m_TestTCPServer = true;
		if (name == CStringLiteral(_T("TestTCPClient")))
		{
			m_TestTCPClient = true;
			m_sTestTCPClient = value;
		}
		if (name == CStringLiteral(_T("TestKeepAliveTCPServer")))
			m_TestTCPServerKeepAlive = true;
		if (name == CStringLiteral(_T("TestKeepAliveTCPClient")))
		{
			m_TestTCPClientKeepAlive = true;
			m_sTestTCPClientKeepAlive = value;
		}
		if (name == CStringLiteral(_T("TestConsole")))
			m_TestConsole = true;
		if (name == CStringLiteral(_T("TestSQL")))
			m_TestSQL = true;
#ifdef OK_SYS_WINDOWS
		if (name == CStringLiteral(_T("TestHttp1")))
		{
			m_TestHttp1 = true;
			m_sTestHttp1.Append(value);
		}
		if (name == CStringLiteral(_T("TestHttp2")))
		{
			m_TestHttp2 = true;
			m_sTestHttp2 = value;
		}
#endif
#ifdef OK_COMP_MSC
		if (name == CStringLiteral(_T("TestAsyncFile")))
			m_TestAsyncFile = true;
		if (name == CStringLiteral(_T("TestAsyncTCPClient")))
			m_TestAsyncTCPClient = true;
		if (name == CStringLiteral(_T("TestSqLite3ServiceClient")))
			m_TestSqLite3ServiceClient = true;
		if (name == CStringLiteral(_T("TestAsyncTCPServer")))
			m_TestAsyncTCPServer = true;
		if (name == CStringLiteral(_T("TestEventLog")))
			m_TestEventLog = true;
#endif
		if (name == CStringLiteral(_T("TestFilter")))
			m_TestFilter = true;
		if (name == CStringLiteral(_T("TestFTP")))
			m_TestFTP = true;
		if (name == CStringLiteral(_T("TestSAX")))
			m_TestSAX = true;
		if (name == CStringLiteral(_T("TestSSL")))
			m_TestSSL = true;
		if (name == CStringLiteral(_T("TestSSLServer")))
			m_TestSSLServer = true;
		if (name == CStringLiteral(_T("DebugServer")))
			m_DebugServer = true;
		if (name == CStringLiteral(_T("StopDebugServer")))
			m_StopDebugServer = true;
		if (name == CStringLiteral(_T("Debug")))
			m_Debug = true;
		m_AnyOption = true;
	}

	virtual int main()
	{
		CFilePath curdir(__FILE__LINE__ _T("_TestDir"));
		CStreamFile* pOutFile = NULL;


		if (m_help || (!m_AnyOption))
		{
			CStringBuffer tmp;

			usage(tmp);
			CERR << tmp;
			return 0;
		}

		if (m_StopDebugServer)
		{
			CDebugClient::CreateInstance();
			CDebug() << _T("@stopserver") << eodbg;
			Sleep(2000);
			CDebugClient::FreeInstance();
			return 0;
		}

		if (m_DebugServer)
		{
			CDebugServer srv;

			srv.RunServer();
			return 0;
		}

		if (m_Debug)
		{
			CDebugClient::CreateInstance();
			CDebug() << _T("Starting debug session ...") << eodbg;
		}

		if (m_Special)
		{
			TestSpecial();
			return 0;
		}

		try
		{
			CDirectoryIterator::SetCurrentDirectory(curdir);
		}
		catch (CBaseException *ex)
		{
			CERR << ex->GetExceptionMessage() << endl;
		}

		if (m_Output)
		{
			CFilePath foutf;

			if (m_sOutput.IsEmpty())
			{
				CDateTime now(CDateTime::LocalTime);
				CStringBuffer tmp;

				tmp.FormatString(__FILE__LINE__ _T("XTest%04d%02d%02d%02d%02d%02d.log"),
					Cast(int, now.GetYears()), Cast(int, now.GetMonths()), Cast(int, now.GetDays()),
					Cast(int, now.GetHours()), Cast(int, now.GetMinutes()), Cast(int, now.GetSeconds()));
				foutf.set_Filename(tmp);
			}
			else
				foutf.set_Path(__FILE__LINE__ m_sOutput);
			pOutFile = OK_NEW_OPERATOR CStreamFile;
			pOutFile->ReOpen(foutf, stdout);
		}

		if (m_AllOption)
		{
			m_TestDataStructures = true;
			m_TestCppSources = true;
			m_TestFilter = true;
			m_TestWinSources = true;
			m_TestCSources = true;
		}

		if (m_TestCppSources)
			TestCppSources();
		if (m_TestDataStructures)
			TestDataStructures();
		if (m_TestCSources)
			TestCSources();
		if (m_TestFilter)
			TestFileFilter();
		if (m_TestWinSources)
			TestWinSources();

		if (m_TestLinkedByteBuffer) {
			COUT << _T("TestLinkedByteBuffer") << endl;
			TestLinkedByteBuffer();
		}
		if (m_TestDirectoryIterator) {
			COUT << _T("TestDirectoryIterator") << endl;
			TestDirectoryIterator();
		}
		if (m_ScanDirectory || m_ScanDirectoryPattern)
			ScanDirectory(m_sScanDirectory, m_sScanDirectoryPattern);
		if (m_TestDBase)
			TestDbase();
		if (m_TestThreads)
		{
			COUT << _T("TestThreads") << endl;
			TestThreads();
			COUT << _T("TestThreads1") << endl;
			TestThreads1();
			COUT << _T("TestThreads2") << endl;
			TestThreads2();
		}
		if (m_TestTCPServer)
			TestTCPServer();
		if (m_TestTCPClient)
			TestTCPClient(m_sTestTCPClient);
		if (m_TestTCPServerKeepAlive)
			TestTCPServerKeepAlive();
		if (m_TestTCPClientKeepAlive)
			TestTCPClientKeepAlive(m_sTestTCPClientKeepAlive);
		if (m_TestConsole)
			TestConsole();
		if (m_TestSQL) {
			COUT << _T("TestSQL") << endl;
			TestSQL();
		}
#ifdef OK_SYS_WINDOWS
		if (m_TestHttp1)
			TestHTTP1(m_sTestHttp1);
		if (m_TestHttp2)
			TestHTTP2(m_sTestHttp2);
#endif
#ifdef OK_COMP_MSC
		if (m_TestAsyncFile) {
			COUT << _T("TestAsyncFile") << endl;
			TestAsyncFile();
		}
		if (m_TestAsyncTCPClient) {
			COUT << _T("TestAsyncTCPClient") << endl;
			TestAsyncTCPClient();
		}
		if (m_TestSqLite3ServiceClient) {
			COUT << _T("TestSqLite3ServiceClient") << endl;
			TestSqLite3ServiceClient();
		}
		if (m_TestAsyncTCPServer) {
			COUT << _T("TestAsyncTCPServer") << endl;
			TestAsyncTCPServer();
		}
		if (m_TestEventLog) {
			COUT << _T("TestEventLog") << endl;
			TestEventLog();
		}
#endif
		if (m_TestFTP)
			TestFTPClient();
		if (m_TestSAX)
			TestSAXParser();
		if (m_TestSSL)
			TestSSLFunc();
		if (m_TestSSLServer)
			TestSSLServerFunc();
		if (m_Debug)
			CDebugClient::FreeInstance();
#ifdef OK_COMP_MSC
		if (NotPtrCheck(pOutFile))
		{
			pOutFile->Close();
			pOutFile->release();
		}
#endif
		return 0;
	}
private:
	WBool m_AnyOption;
	WBool m_AllOption;
	WBool m_Output;
	CStringLiteral m_sOutput;
	WBool m_help;
	WBool m_TestDataStructures;
	WBool m_TestCppSources;
	WBool m_TestCSources;
	WBool m_TestDirectoryIterator;
	WBool m_TestLinkedByteBuffer;
	WBool m_ScanDirectory;
	CDataVectorT<mbchar> m_sScanDirectory;
	WBool m_ScanDirectoryPattern;
	CStringLiteral m_sScanDirectoryPattern;
	WBool m_TestDBase;
	WBool m_TestThreads;
	WBool m_TestTCPServer;
	WBool m_TestTCPClient;
	CStringLiteral m_sTestTCPClient;
	WBool m_TestTCPServerKeepAlive;
	WBool m_TestTCPClientKeepAlive;
	CStringLiteral m_sTestTCPClientKeepAlive;
	WBool m_TestConsole;
	WBool m_TestSQL;
#ifdef OK_SYS_WINDOWS
	WBool m_TestHttp1;
	CDataVectorT<mbchar> m_sTestHttp1;
	WBool m_TestHttp2;
	CStringLiteral m_sTestHttp2;
#endif
#ifdef OK_COMP_MSC
	WBool m_TestAsyncFile;
	WBool m_TestAsyncTCPClient;
	WBool m_TestSqLite3ServiceClient;
	WBool m_TestAsyncTCPServer;
	WBool m_TestEventLog;
#endif
	WBool m_TestWinSources;
	WBool m_TestFilter;
	WBool m_TestFTP;
	WBool m_TestSAX;
	WBool m_TestSSL;
	WBool m_TestSSLServer;
	WBool m_DebugServer;
	WBool m_StopDebugServer;
	WBool m_Debug;
	WBool m_Special;
};

int
#ifdef OK_COMP_GNUC
main( int argc, char** argv )
#endif
#ifdef OK_COMP_MSC
_tmain(int argc, CPointer *argv)
#endif

{
	COUT << _T("sizeof(CPointer) = ") << sizeof(CPointer) << endl;

	int res;

#ifdef OK_COMP_MSC
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

	TestApplication testApp;

	res = testApp.run(argc, argv);

#if defined(OK_COMP_MSC) && defined(__DEBUG__)
	_getch();
#endif
	return res;
}
