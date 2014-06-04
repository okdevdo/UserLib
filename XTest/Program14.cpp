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
#include "SyncTCPSocketClasses.h"
#include "DirectoryIterator.h"
#include "File.h"
#include "Filter.h"
#include "FTPClient.h"
#include "HTTPDownload.h"
#include "SecurityContext.h"

#include <time.h>

#ifdef OK_SYS_UNIX
#include <unistd.h>
#define Sleep(t) sleep(t / 1000)
#endif

#define MAX_BUFFER		512
#define FTP_SERVER      _T("linux.rz.rub.de")

// drwxrwsr-x 596 root     root        20480 Jul 26 23:34 release
// -rw-r--r--   1 root     root      1690563 Jul 25 22:10 setup.ini
// drwxrwsr-x   2 root     root         4096 Mar 26  2011 w32api
class CTestFileInfo: public CCppObject
{
public:
	CTestFileInfo():
	  	faccess(),
		fcnt(0),
		fuser(),
		fgroup(),
		fsize(0L),
		fname(),
		fLoadedAndCurrent(false)
	{
	}

	void Parse(Ref(CStringConstIterator) it )
	{
		word mon, day, year, hour = 0, min = 0;
		CPointer pValue;
		WULong sValue;
		WLong nValue;

		it.EatWord(pValue, sValue);
		faccess.SetString(__FILE__LINE__ pValue, sValue);
		it.EatWhite();
		it.EatDecimal(nValue);
		fcnt = Cast(word, nValue);
		it.EatWhite();
		it.EatWord(pValue, sValue);
		fuser.SetString(__FILE__LINE__ pValue, sValue);
		it.EatWhite();
		it.EatWord(pValue, sValue);
		fgroup.SetString(__FILE__LINE__ pValue, sValue);
		it.EatWhite();
		it.EatDecimal(nValue);
		fsize = nValue;
		it.EatWhite();
		it.EatMonth3(nValue);
		mon = Cast(word, nValue) + 1;
		it.EatWhite();
		it.EatDecimal(nValue);
		day = Cast(word, nValue);
		it.EatWhite();
		it.EatDecimal(nValue);
		year = Cast(word, nValue);
		if ( year < 24 )
		{
			CDateTime cur;
			WChar cValue;

			hour = year;
			year = cur.GetYears();
			it.EatCharacter(cValue);
			it.EatDecimal(nValue);
			min = Cast(word, nValue);
			if ( mon > cur.GetMonths() )
				--year;
		}
		ftime.SetTime(year, mon, day, hour, min, 0);
		it.EatWhite();
		it.EatWord(pValue, sValue);
		fname.SetString(__FILE__LINE__ pValue, sValue);
	}

	bool IsDirectory()
	{
		CStringConstIterator it = faccess.GetString();

		return (it[0] == _T('d'));
	}

	bool CompareLessOrEqual(ConstRef(CDateTime) fileTime)
	{
		if ( fileTime.GetYears() > ftime.GetYears() )
			return true;
		if ( fileTime.GetYears() == ftime.GetYears() )
		{
			if ( fileTime.GetMonths() > ftime.GetMonths() )
				return true;
			if ( fileTime.GetMonths() == ftime.GetMonths() )
			{
				if ( fileTime.GetDays() >= ftime.GetDays() )
					return true;
			}
		}
		return false;
	}

	__inline ConstRef(CStringBuffer) get_Name() const { return fname; }

public:
	CStringBuffer faccess;
	word fcnt;
	CStringBuffer fuser;
	CStringBuffer fgroup;
	dword fsize;
	CDateTime ftime;
	CStringBuffer fname;
	bool fLoadedAndCurrent;
};

class CTestFileInfoVector;
class CTestFileInfoVectorFilterOutput: public CFilterOutput
{
public:
	CTestFileInfoVectorFilterOutput(Ref(CTestFileInfoVector) vector):
	    _vector(vector)
	{
	}
	virtual ~CTestFileInfoVectorFilterOutput()
	{
	}

	virtual void open()
	{
	}
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close()
	{
	}

private:
	Ref(CTestFileInfoVector) _vector;
};

class CTestFileInfoVector: public CDataVectorT<CTestFileInfo, CStringByNameLessFunctor<CTestFileInfo> >
{
	typedef CDataVectorT<CTestFileInfo, CStringByNameLessFunctor<CTestFileInfo> > super;

public:
	CTestFileInfoVector(TListCnt cnt, TListCnt exp):
		super(__FILE__LINE__ cnt, exp)
	{
	}
	virtual ~CTestFileInfoVector()
	{
	}

	void LoadListFile(ConstRef(CFilePath) fpath)
	{
		CCppObjectPtr<CDiskFile> pInputFile = OK_NEW_OPERATOR CDiskFile(fpath, true, false, CFile::BinaryFile_NoEncoding);
		CCppObjectPtr<CFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(pInputFile);
		CCppObjectPtr<CFilterOutput> pOutput = OK_NEW_OPERATOR CTestFileInfoVectorFilterOutput(*this);
		CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput);

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();
	}

	bool CompareFileInfo(ConstRef(CFilePath) fpath)
	{
		CDirectoryIterator fit;

		fit.Open(fpath);
		if ( fit )
		{
			CTestFileInfo finfo;

			finfo.fname = fit.get_Name();
			Iterator it = FindSorted(&finfo);

			if (MatchSorted(it, &finfo))
			{
				CTestFileInfo* pfinfo = *it;
				CDateTime pFTime = fit.get_LastWriteTime();

				if ((pfinfo->CompareLessOrEqual(pFTime)) && (pfinfo->fsize == Castdword(fit.get_FileSize())))
					return true;
			}
		}
		return false;
	}

	void FilterFileInfo()
	{
		CDirectoryIterator fit;
		CTestFileInfo finfo;
		Iterator it;

		fit.Open(CFilePath(__FILE__LINE__ _T("*")));
		while ( fit )
		{
			if ( !(fit.is_SubDir()) )
			{
				finfo.fname = fit.get_Name();
				it = FindSorted(&finfo);
				if (MatchSorted(it, &finfo))
				{
					CTestFileInfo* pfinfo = *it;
					CDateTime pFTime = fit.get_LastWriteTime();

					pfinfo->fLoadedAndCurrent = pfinfo->CompareLessOrEqual(pFTime);
				}
			}
			++fit;
		}
	}

private:
	CTestFileInfoVector();
};

void CTestFileInfoVectorFilterOutput::write(Ref(CByteBuffer) outputbuf)
{
	CStringBuffer sBuf;
	CStringConstIterator it;
	CTestFileInfo* pInfo = OK_NEW_OPERATOR CTestFileInfo();

	sBuf.convertFromByteBuffer(outputbuf);
	it = sBuf;
	pInfo->Parse(it);
	_vector.InsertSorted(pInfo);
}

class CTestFTPClient
{
public:
	CTestFTPClient(): currentPath(__FILE__LINE__ CDirectoryIterator::UnixPathSeparatorString(), -1, CDirectoryIterator::UnixPathSeparatorString()) {}

	void SendData(const CStringBuffer& buffer)
	{
		CByteBuffer bBuffer;
		dword numBytesSend;

		buffer.convertToByteBuffer(bBuffer);
		tcpClient.SendData(bBuffer.get_Buffer(), bBuffer.get_BufferSize(), &numBytesSend);
		assert(numBytesSend == bBuffer.get_BufferSize());
	}

	void ReceiveData(CStringBuffer& buffer, WLong& msgNo)
	{
		CStringConstIterator it;
		CByteBuffer responseBuffer;
		dword numDataReceived = 0;
		dword totalDataReceived = 0;
		bool bContinuationLine = false;
		WLong cmpNo;

		for ( ;; )
		{
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
			tcpClient.ReceiveData(responseBuffer.get_Buffer() + totalDataReceived, MAX_BUFFER, &numDataReceived);
			totalDataReceived += numDataReceived;
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
			buffer.convertFromByteBuffer(responseBuffer);
			if ( buffer.IsEmpty() )
			{
				msgNo = 0;
				return;
			}
			it = buffer;
			it.EatDecimal(msgNo);
			bContinuationLine = (it[0] == _T('-'));
			it.Find(_T("\r\n"));
			if ( (!(it.IsEnd())) && (!bContinuationLine) )
				return;
			while ( !(it.IsEnd()) && bContinuationLine )
			{
				it += 2;
				it.EatDecimal(cmpNo);
				bContinuationLine = (it[0] == _T('-'));
				it.Find(_T("\r\n"));
				if ( (!(it.IsEnd())) && (!bContinuationLine) )
					return;
			}
		}
	}

	void ReceiveFileData(CFilePath& fpath)
	{
		CByteBuffer responseBuffer;
		dword numDataReceived = 0;
		CDiskFile file;
		bool bOk = true;

		try
		{
			COUT << _T("Receiving '") << fpath.get_Path() << _T("': '");
			file.Create(fpath, false, CFile::BinaryFile_NoEncoding);
			do
			{
				responseBuffer.set_BufferSize(__FILE__LINE__ MAX_BUFFER);
				tcpClient2.ReceiveData(responseBuffer.get_Buffer(), MAX_BUFFER, &numDataReceived);
				COUT << _T(".");
				responseBuffer.set_BufferSize(__FILE__LINE__ numDataReceived);
				file.Write(responseBuffer);
			}
			while ( numDataReceived > 0 );
			file.Close();
			COUT << endl;
		}
		catch ( CTcpClientException* ex )
		{
			bOk = false;
			COUT << ex->GetExceptionMessage() << endl;
		}
		catch ( CFileException* ex )
		{
			bOk = false;
			COUT << ex->GetExceptionMessage() << endl;
		}
		if ( !bOk )
		{
			try
			{
				tcpClient2.CloseConnection();
				file.Close();
				CDirectoryIterator::RemoveFile(fpath);
			}
			catch ( CDirectoryIteratorException* ex )
			{
				COUT << ex->GetExceptionMessage() << endl;
			}
		}
	}

	void OpenConnection()
	{
		WLong msgNo;
		CStringBuffer sBuffer;
		CStringBuffer sBuffer0(__FILE__LINE__ _T("USER anonymous\r\n"));
		CStringBuffer sBuffer1(__FILE__LINE__ _T("PASS okinfo@arcor.de\r\n"));
		CStringBuffer sBuffer2(__FILE__LINE__ _T("SYST\r\n"));
		CStringBuffer sBuffer3(__FILE__LINE__ _T("PWD\r\n"));
		int cnt = 10;

		while ( !(tcpClient.IsConnected()) && (cnt-- > 0) )
		{
			if ( cnt == 5 )
				Sleep(5000);
			try
			{
				tcpClient.OpenConnection(FTP_SERVER, _T("ftp"));

				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				SendData(sBuffer0);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				SendData(sBuffer1);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				SendData(sBuffer2);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				SendData(sBuffer3);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;
			}
			catch ( CTcpClientException* ex )
			{
				COUT << ex->GetExceptionMessage() << endl;
				tcpClient.CloseConnection();
			}
		}
	}

	void ReOpenConnection()
	{
		WLong msgNo;
		CStringBuffer sBuffer;
		CStringBuffer sBuffer0(__FILE__LINE__ _T("USER anonymous\r\n"));
		CStringBuffer sBuffer1(__FILE__LINE__ _T("PASS okinfo@arcor.de\r\n"));
		CStringBuffer sBuffer2;
		int cnt = 10;

		sBuffer2.FormatString(__FILE__LINE__ _T("CWD %s\r\n"), currentPath.GetString());
		while ( !(tcpClient.IsConnected()) && (cnt-- > 0) )
		{
			if ( cnt == 5 )
				Sleep(5000);
			try
			{
				tcpClient.OpenConnection(FTP_SERVER, _T("ftp"));

				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				SendData(sBuffer0);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				SendData(sBuffer1);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				COUT << sBuffer2 << endl;
				SendData(sBuffer2);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;
			}
			catch ( CTcpClientException* ex )
			{
				COUT << ex->GetExceptionMessage() << endl;
				tcpClient.CloseConnection();
			}
		}
	}

	void ChangeToParentDir()
	{
		if ( !(tcpClient.IsConnected()) )
			return;

		WLong msgNo;
		CStringBuffer sBuffer;
		CStringBuffer sBuffer0(__FILE__LINE__ _T("CDUP\r\n"));
		CFilePath fpath;
		bool bFirst = true;
		bool bOk = true;

		try
		{
			CDirectoryIterator::GetCurrentDirectory(fpath);
			fpath.set_Directory(_T(""), -1);
			CDirectoryIterator::SetCurrentDirectory(fpath);
		}
		catch ( CDirectoryIteratorException* ex )
		{
			COUT << ex->GetExceptionMessage() << endl;
			tcpClient.CloseConnection();
			return;
		}
		currentPath.set_Directory(_T(""), -1);
		do
		{
			try
			{
				if ( bFirst )
				{
					SendData(sBuffer0);
					ReceiveData(sBuffer, msgNo);
					COUT << sBuffer << endl;
					bFirst = false;
				}
				else
					ReOpenConnection();
				bOk = false;
			}
			catch ( CTcpClientException* ex )
			{
				COUT << ex->GetExceptionMessage() << endl;
				tcpClient.CloseConnection();
			}
		}
		while ( bOk );
	}


	void ChangeWorkingDir(CStringLiteral dir)
	{
		if ( !(tcpClient.IsConnected()) )
			return;

		WLong msgNo;
		CStringBuffer sBuffer;
		CStringBuffer sBuffer0;
		CStringBuffer sBuffer1(__FILE__LINE__ _T("TYPE A\r\n"));
		CStringBuffer sBuffer2(__FILE__LINE__ _T("PASV\r\n"));
		CStringBuffer sBuffer3(__FILE__LINE__ _T("LIST\r\n"));
		CFilePath fpath(__FILE__LINE__ dir);
		CFilePath fpath1(__FILE__LINE__ _T(".listing"));
		bool bFirst = true;
		bool bOk = true;

		sBuffer0.FormatString(__FILE__LINE__ _T("CWD %s\r\n"), dir.GetString());

		try
		{
			CDirectoryIterator::MakeDirectory(fpath);
			CDirectoryIterator::SetCurrentDirectory(fpath);
		}
		catch ( CDirectoryIteratorException* ex )
		{
			COUT << ex->GetExceptionMessage() << endl;
			tcpClient.CloseConnection();
			return;
		}
		sBuffer.SetString(__FILE__LINE__ dir);
		sBuffer.AppendString(CDirectoryIterator::UnixPathSeparatorString());
		currentPath.set_Filename(sBuffer);
		do 
		{
			try
			{
				if ( bFirst )
				{
					COUT << sBuffer0 << endl;
					SendData(sBuffer0);
					ReceiveData(sBuffer, msgNo);
					COUT << sBuffer << endl;
					bFirst = false;
				}
				else
					ReOpenConnection();

				SendData(sBuffer1);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				SendData(sBuffer2);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				if ( msgNo == 227 )
				{
					CStringConstIterator it = sBuffer.GetString();

					it += 3;
					it.FirstOf(_T("0123456789"));
					if ( !it.IsEnd() )
					{
						CStringBuffer ip;
						CStringBuffer port;
						WLong ip0, ip1, ip2, ip3, port0, port1;
						WChar c;

						it.EatDecimal(ip0);
						it.EatCharacter(c);
						it.EatDecimal(ip1);
						it.EatCharacter(c);
						it.EatDecimal(ip2);
						it.EatCharacter(c);
						it.EatDecimal(ip3);
						it.EatCharacter(c);
						it.EatDecimal(port0);
						it.EatCharacter(c);
						it.EatDecimal(port1);

						ip.FormatString(__FILE__LINE__ _T("%d.%d.%d.%d"), ip0, ip1, ip2, ip3);
						port.FormatString(__FILE__LINE__ _T("%d"), (port0 * 256) + port1);

						tcpClient2.OpenConnection(ip, port);
					}
				}

				SendData(sBuffer3);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				if ( tcpClient2.IsConnected() )
				{
					ReceiveFileData(fpath1);
					tcpClient2.CloseConnection();
				}

				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;
				bOk = (!(CDirectoryIterator::FileExists(fpath1)));
			}
			catch ( CTcpClientException* ex )
			{
				COUT << ex->GetExceptionMessage() << endl;
				tcpClient.CloseConnection();
				tcpClient2.CloseConnection();
			}
			catch ( CDirectoryIteratorException* ex )
			{
				COUT << ex->GetExceptionMessage() << endl;
				tcpClient.CloseConnection();
				tcpClient2.CloseConnection();
				bOk = false;
			}
		}
		while ( bOk );
	}

	void RetrieveFile(CStringLiteral filename)
	{
		if ( !(tcpClient.IsConnected()) )
			return;

		WLong msgNo;
		CStringBuffer sBuffer;
		CStringBuffer sBuffer1(__FILE__LINE__ _T("TYPE I\r\n"));
		CStringBuffer sBuffer2(__FILE__LINE__ _T("PASV\r\n"));
		CStringBuffer sBuffer3;
		CFilePath fpath(__FILE__LINE__ filename);
		bool bFirst = true;
		bool bOk = true;

		sBuffer3.FormatString(__FILE__LINE__ _T("RETR %s\r\n"), filename.GetString());

		do
		{
			try
			{
				if ( bFirst )
					bFirst = false;
				else
					ReOpenConnection();

				SendData(sBuffer1);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				SendData(sBuffer2);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				if ( msgNo == 227 )
				{
					CStringConstIterator it = sBuffer.GetString();

					it += 3;
					it.FirstOf(_T("0123456789"));
					if ( !it.IsEnd() )
					{
						CStringBuffer ip;
						CStringBuffer port;
						WLong ip0, ip1, ip2, ip3, port0, port1;
						WChar c;

						it.EatDecimal(ip0);
						it.EatCharacter(c);
						it.EatDecimal(ip1);
						it.EatCharacter(c);
						it.EatDecimal(ip2);
						it.EatCharacter(c);
						it.EatDecimal(ip3);
						it.EatCharacter(c);
						it.EatDecimal(port0);
						it.EatCharacter(c);
						it.EatDecimal(port1);

						ip.FormatString(__FILE__LINE__ _T("%d.%d.%d.%d"), ip0, ip1, ip2, ip3);
						port.FormatString(__FILE__LINE__ _T("%d"), (port0 * 256) + port1);

						tcpClient2.OpenConnection(ip, port);
					}
				}

				SendData(sBuffer3);
				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;

				if ( tcpClient2.IsConnected() )
				{
					ReceiveFileData(fpath);
					tcpClient2.CloseConnection();
				}

				ReceiveData(sBuffer, msgNo);
				COUT << sBuffer << endl;
				bOk = (!(CDirectoryIterator::FileExists(fpath)));
			}
			catch ( CTcpClientException* ex )
			{
				COUT << ex->GetExceptionMessage() << endl;
				tcpClient.CloseConnection();
				tcpClient2.CloseConnection();
			}
			catch ( CDirectoryIteratorException* ex )
			{
				COUT << ex->GetExceptionMessage() << endl;
				tcpClient.CloseConnection();
				tcpClient2.CloseConnection();
				bOk = false;
			}
		}
		while ( bOk );
	}

	void CloseConnection()
	{
		if ( !(tcpClient.IsConnected()) )
			return;

		WLong msgNo;
		CStringBuffer sBuffer;
		CStringBuffer sBuffer0(__FILE__LINE__ _T("QUIT\r\n"));

		try
		{
			SendData(sBuffer0);
			ReceiveData(sBuffer, msgNo);
			COUT << sBuffer << endl;

			tcpClient.CloseConnection();
		}
		catch ( CTcpClientException* ex )
		{
			COUT << ex->GetExceptionMessage() << endl;
			tcpClient.CloseConnection();
		}
	}

	bool IsConnected()
	{
		return tcpClient.IsConnected();
	}

private:
	CTcpClient tcpClient;
	CTcpClient tcpClient2;
	CFilePath currentPath;
};

static void TestFTPClient1(Ref(CTestFTPClient) client)
{
	client.ChangeWorkingDir(_T("/cygwin/x86/"));
	if (client.IsConnected())
	{
		CTestFileInfoVector fiv(16, 16);

		fiv.LoadListFile(CFilePath(__FILE__LINE__ _T(".listing")));
		if ( !(fiv.CompareFileInfo(CFilePath(__FILE__LINE__ _T("setup.ini")))) )
			client.RetrieveFile(_T("setup.ini"));
	}
}

static void TestFTPClient2(Ref(CTestFTPClient) client, CStringLiteral dir)
{
	client.ChangeWorkingDir(dir);
	if ( client.IsConnected() )
	{
		CTestFileInfoVector fiv(16, 16);
		CTestFileInfoVector::Iterator it;
		CTestFileInfo* pInfo;

		fiv.LoadListFile(CFilePath(__FILE__LINE__ _T(".listing")));
		fiv.FilterFileInfo();
		it = fiv.Begin();

		while ( it && client.IsConnected() )
		{
			pInfo = *it;
			if ( !(pInfo->fLoadedAndCurrent) )
			{
				if ( pInfo->IsDirectory() )
					TestFTPClient2(client, pInfo->fname);
				else
					client.RetrieveFile(pInfo->fname);
			}
			++it;
		}
		if ( client.IsConnected() )
			client.ChangeToParentDir();
	}
}

void TestFTPClient()
{
//	CFTPClient client;
//	CUrl serverandroot(_T("ftp://ftp-stud.fht-esslingen.de/pub/Mirrors/sourceware.org/cygwinports/"));
//#if defined(__MINGW32_MAJOR_VERSION) || defined(_MSC_VER)
//	CFilePath path(_T("C:\\Users\\Oliver\\Downloads\\ftptest\\ftp%3a%2f%2fftp-stud.fht-esslingen.de%2fpub%2fMirrors%2fsourceware.org%2fcygwinports%2f\\"));
//#else
//	CFilePath path(_T("/home/Oliver/ftp-stud.fht-esslingen.de/"));
//#endif
//	CDateTime modTime;
//
//	CDirectoryIterator::MakeDirectory(path);
//	client.SetLog(true);
//	client.OpenConnection(_T("anonymous"), _T("okinfo10@arcor.de"), serverandroot, path);
//	client.RetrieveFileTime(_T("x86/setup.ini"), modTime);
//	client.RetrieveFile(_T("x86/setup.ini"), modTime);
//	client.RetrieveFileTime(_T("x86/release/AClock.app/AClock.app-0.4.0-1.tar.bz2"), modTime);
//	client.RetrieveFile(_T("x86/release/AClock.app/AClock.app-0.4.0-1.tar.bz2"), modTime);
//	client.CloseConnection();
//	COUT << _T("CFTPClient: errcnt = ") << client.GetErrCnt() << endl;
//	COUT << client.GetLog().Join(_T("\r\n")) << endl;
//
//	CHttpDownload client1;
//	CUrl serverandroot1(_T("http://linux.rz.ruhr-uni-bochum.de/download/cygwin/"));
//#if defined(__MINGW32_MAJOR_VERSION) || defined(_MSC_VER)
//	CFilePath path1(_T("C:\\Users\\Oliver\\Downloads\\ftptest\\http%3a%2f%2flinux.rz.ruhr-uni-bochum.de%2fdownload%2fcygwin%2f\\"));
//#else
//	CFilePath path1(_T("/home/Oliver/linux.rz.ruhr-uni-bochum.de/"));
//#endif
//	CDateTime modTime1;
//
//	CDirectoryIterator::MakeDirectory(path1);
//	client1.SetLog(true);
//	client1.OpenConnection(_T("anonymous"), _T("okinfo10@arcor.de"), serverandroot1, path1);
//	client1.RetrieveFileTime(_T("x86/setup.ini"), modTime1);
//	client1.RetrieveFile(_T("x86/setup.ini"), modTime1);
//	client1.RetrieveFileTime(_T("x86/release/CUnit/CUnit-2.1.2-1.tar.bz2"), modTime1);
//	client1.RetrieveFile(_T("x86/release/CUnit/CUnit-2.1.2-1.tar.bz2"), modTime1);
//	client1.CloseConnection();
//	COUT << _T("CHttpDownload: errcnt = ") << client1.GetErrCnt() << endl;
//	COUT << client1.GetLog().Join(_T("\r\n")) << endl;

	CTestFTPClient client0;

	client0.OpenConnection();
	TestFTPClient1(client0);
	TestFTPClient2(client0, _T("release"));
	client0.CloseConnection();

	CSecurityContext_FreeInstance
}

