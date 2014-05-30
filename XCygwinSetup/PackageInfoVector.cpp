/******************************************************************************
    
	This file is part of XCygwinSetup, which is part of UserLib.

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
#include "ConsoleApplication.h"
#include "PackageInfoVector.h"
#include "DirectoryIterator.h"
#include "BZip2Filter.h"
#include "ZLibFilter.h"
#include "GZipFilter.h"
#include "XZFilter.h"
#include "TarArchive.h"
#include "Script.h"
#include "SecurityFile.h"
#include "MD5Buffer.h"
#include "URL.h"
#include "FTPClient.h"
#include "HTTPDownload.h"
#include "WinDirectoryIterator.h"

CPackageInfoVector::CPackageInfoVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp):
    super(ARGS_FILE_LINE cnt, exp)
{
}

CPackageInfoVector::~CPackageInfoVector()
{
}

bool CPackageInfoVector::InsertSortedUnique(Ptr(CPackageInfo) pInfo)
{
	Iterator itPI = FindSorted(pInfo);

	if (!(MatchSorted(itPI, pInfo)))
	{
		pInfo->addRef();
		InsertSorted(pInfo);
		return true;
	}
	return false;
}

bool CPackageInfoVector::AppendUnique(Ptr(CPackageInfo) pInfo)
{
	Iterator itPI = Find<CStringByNameLessFunctor<CPackageInfo>>(pInfo);

	if (!(MatchSorted(itPI, pInfo)))
	{
		pInfo->addRef();
		Append(pInfo);
		return true;
	}
	return false;
}

class CUninstallScriptFilterOutput: public CFilterOutput
{
public:
	CUninstallScriptFilterOutput(CStringLiteral pInstallDir, Ref(CBashScript) rBScript, Ref(CCmdScript) rCScript, WBool bPrintOnly):
	    _sInstallDir(pInstallDir), _bPrintOnly(bPrintOnly), _bscript(rBScript), _cscript(rCScript)
	{
	}

	virtual ~CUninstallScriptFilterOutput()
	{
	}

	virtual void open()
	{
	}

	virtual void write(Ref(CByteBuffer) outputbuf)
	{
		CStringBuffer tmp;

		tmp.convertFromByteBuffer(outputbuf);

		CStringConstIterator it(tmp);

		it.Find(_T("etc/preremove/"));
		if ( !(it.IsEnd()) )
		{
			CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());

			fpath.Normalize(_sInstallDir);
			if ( fpath.is_File() && CWinDirectoryIterator::FileExists(fpath) )
			{
				fpath.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());
				fpath.set_Root(CDirectoryIterator::UnixPathSeparatorString());

				COUT << _T("Uninstall: Do preremove script: '") << fpath.get_Path() << _T("'") << endl;
				if ( !_bPrintOnly )
				{
					CStringBuffer ext(fpath.get_Extension());

					if ( ext.Compare(_T("sh"), 0, CStringLiteral::cIgnoreCase) == 0 )
						_bscript.run(fpath);
					else if ( ext.Compare(_T("bat"), 0, CStringLiteral::cIgnoreCase) == 0 )
					{
						fpath.Normalize(_sInstallDir);
						_cscript.run(fpath);
					}
					else if ( ext.Compare(_T("cmd"), 0, CStringLiteral::cIgnoreCase) == 0 )
					{
						fpath.Normalize(_sInstallDir);
						_cscript.run(fpath);
					}
				}
			}
		}
	}

	virtual void close()
	{
	}

private:
	CStringLiteral _sInstallDir;
	WBool _bPrintOnly;
	Ref(CBashScript) _bscript;
	Ref(CCmdScript) _cscript;

	CUninstallScriptFilterOutput();
};

class CUninstallFilterOutput: public CFilterOutput
{
public:
	CUninstallFilterOutput(CStringLiteral pInstallDir, WBool bPrintOnly):
		_sInstallDir(pInstallDir), _bPrintOnly(bPrintOnly), _stack(__FILE__LINE__ 64, 64)
	{
	}

	virtual ~CUninstallFilterOutput()
	{
	}

	virtual void open()
	{
	}

	virtual void write(Ref(CByteBuffer) outputbuf)
	{
		CStringBuffer tmp;

		tmp.convertFromByteBuffer(outputbuf);

		CStringConstIterator it(tmp);
		CConstPointer p = _T("etc/postinstall/");

		it.Find(p);
		if ( !(it.IsEnd()) )
		{
			it += s_strlen(CastMutable(CPointer,p), 20);
			if ( !(it.IsEnd()) )
				tmp += _T(".done");
		}
		else
		{
			it = tmp;
			it.Find(_T("usr/bin"));
			if ( it.IsBegin() )
			{
				tmp.DeleteString(0, 7);
				tmp.InsertString(0, _T("bin"));
			}
			it = tmp;
			it.Find(_T("usr/lib"));
			if ( it.IsBegin() )
			{
				tmp.DeleteString(0, 7);
				tmp.InsertString(0, _T("lib"));
			}
		}
		_stack.Append(tmp);
	}

	virtual void close()
	{
		CDataVectorT<CStringBuffer>::Iterator it = _stack.Last();
		int dcnt;

		while ( it )
		{
			CFilePath fpath(*it, CDirectoryIterator::UnixPathSeparatorString());

			fpath.Normalize(_sInstallDir);
			try
			{
				if (fpath.is_File() && CWinDirectoryIterator::FileExists(fpath))
				{
					COUT << _T("Uninstall: Delete file '") << fpath.get_Path() << _T("'") << endl;
					if (!_bPrintOnly)
						CWinDirectoryIterator::UnlinkFile(fpath);
				}
				else if ((dcnt = CWinDirectoryIterator::DirectoryExists(fpath)) >= 0)
				{
					COUT << _T("Uninstall: Check directory '") << fpath.get_Path() << _T("' for delete (") << dcnt << _T(" entries)") << endl;
					if ((!_bPrintOnly) && (dcnt == 0))
						CWinDirectoryIterator::UnlinkDirectory(fpath);
				}
				else
				{
					CERR << _T("Uninstall: File/Directory '") << fpath.get_Path() << _T("' does not exist") << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << _T("Uninstall: File/Directory '") << fpath.get_Path() << _T("' does not exist") << endl;
				}
			}
			catch (CBaseException* ex)
			{
				CERR << _T("Uninstall: ") << ex->GetExceptionMessage() << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << _T("Uninstall: ") << ex->GetExceptionMessage() << endl;
			}
			--it;
		}
	}

private:
	CStringLiteral _sInstallDir;
	WBool _bPrintOnly;
	CDataVectorT<CStringBuffer> _stack;

	CUninstallFilterOutput();
};

void CPackageInfoVector::Uninstall(CStringLiteral pInstallDir, WBool bPrintOnly, WInt iVerbose)
{
	Iterator it;
	CStringBuffer tmp;
	Ptr(CPackageInfo) pInfo = NULL;
	CBashScript bashscript(pInstallDir);
	CCmdScript cmdscript(pInstallDir);
	CFilePath fpath1(__FILE__LINE__ CYGWIN_SETUP_DIR, -1, CDirectoryIterator::UnixPathSeparatorString());
	CFilePath fpath11(__FILE__LINE__ CYGWIN_TMP_DIR, -1, CDirectoryIterator::UnixPathSeparatorString());
	bool bExist;

	fpath1.Normalize(pInstallDir);
	fpath11.Normalize(pInstallDir);
	it = Begin();
	while ( it )
	{
		pInfo = *it;
		tmp = pInfo->GetPackageName();
		tmp += _T(".lst.gz");
		fpath1.set_Filename(tmp);
		try 
		{
			bExist = CWinDirectoryIterator::FileExists(fpath1);
		}
		catch ( CBaseException* ex ) 
		{
			CERR << ex->GetExceptionMessage() << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << ex->GetExceptionMessage() << endl;
			bExist = false;
		}
		if ( bExist )
		{
			tmp = pInfo->GetPackageName();
			tmp += _T(".lst");
			fpath11.set_Filename(tmp);

			CSecurityFile* pInputFile = NULL;
			CSecurityFile* pOutputFile = NULL;
			CFilterInput* pInput = NULL;
			CFilterOutput* pOutput = NULL;
			CFilter* pFilter = NULL;

			try
			{
				pInputFile = OK_NEW_OPERATOR CSecurityFile();
				pInputFile->Open(fpath1);
				pInput = OK_NEW_OPERATOR CFileFilterInput(pInputFile);

				CWinDirectoryIterator::UnlinkFile(fpath11);
				pOutputFile = OK_NEW_OPERATOR CSecurityFile();
				pOutputFile->Create(fpath11, true, CFile::ISO_8859_1_Encoding, 0644);
				pOutput = OK_NEW_OPERATOR CFileFilterOutput(pOutputFile);
				pFilter = OK_NEW_OPERATOR CGZipDeCompressFilter(pInput, pOutput);

				pFilter->open();
				pFilter->do_filter();
				pFilter->close();

				pFilter->release(); pFilter = NULL;
				pInput->release(); pInput = NULL;
				pOutput->release(); pOutput = NULL;
				pInputFile->release(); pInputFile = NULL;
				pOutputFile->release(); pOutputFile = NULL;

				pInputFile = OK_NEW_OPERATOR CSecurityFile();
				pInputFile->Open(fpath11);
				pInput = OK_NEW_OPERATOR CFileFilterInput(pInputFile);
				pOutput = OK_NEW_OPERATOR CUninstallScriptFilterOutput(pInstallDir, bashscript, cmdscript, bPrintOnly);
				pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput, CLineReadFilter::UnixLineEnd);

				pFilter->open();
				pFilter->do_filter();
				pFilter->close();

				pFilter->release(); pFilter = NULL;
				pInput->release(); pInput = NULL;
				pOutput->release(); pOutput = NULL;
				pInputFile->release(); pInputFile = NULL;
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				CERR <<  _T("Uninstall: Doing list file: '") << fpath1.get_Path() << _T("' failed.") << endl;
				if (theConsoleApp->is_stdout_redirected())
				{
					COUT << ex->GetExceptionMessage() << endl;
					COUT << _T("Uninstall: Doing list file: '") << fpath1.get_Path() << _T("' failed.") << endl;
				}
				if ( NotPtrCheck(pInputFile) )
					pInputFile->release();
				if ( NotPtrCheck(pInput) )
					pInput->release();
				if ( NotPtrCheck(pOutputFile) )
					pOutputFile->release();
				if ( NotPtrCheck(pOutput) )
					pOutput->release();
				if ( NotPtrCheck(pFilter) )
					pFilter->release();
			}
		}
		else
		{
			CERR <<  _T("Uninstall: List file: '") << fpath1.get_Path() << _T("' does not exist.") << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << _T("Uninstall: List file: '") << fpath1.get_Path() << _T("' does not exist.") << endl;
			if ( NotPtrCheck(pInfo->GetInstallInfo()) )
				pInfo->GetInstallInfo()->SetIsInstalled(false);
		}
		++it;
	}
	it = Begin();
	while ( it )
	{
		pInfo = *it;
		tmp = pInfo->GetPackageName();
		tmp += _T(".lst");
		fpath11.set_Filename(tmp);
		try 
		{
			bExist = CWinDirectoryIterator::FileExists(fpath11);
		}
		catch ( CBaseException* ex ) 
		{
			CERR << ex->GetExceptionMessage() << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << ex->GetExceptionMessage() << endl;
			bExist = false;
		}
		if ( bExist )
		{
			tmp = pInfo->GetPackageName();
			tmp += _T(".lst.gz");
			fpath1.set_Filename(tmp);

			COUT << _T("Uninstall: Do list file: '") << fpath1.get_Path() << _T("'") << endl;

			CSecurityFile* pInputFile = NULL;
			CFilterInput* pInput = NULL;
			CFilterOutput* pOutput = NULL;
			CFilter* pFilter = NULL;

			try
			{
				pInputFile = OK_NEW_OPERATOR CSecurityFile();
				pInputFile->Open(fpath11);
				pInput = OK_NEW_OPERATOR CFileFilterInput(pInputFile);
				pOutput = OK_NEW_OPERATOR CUninstallFilterOutput(pInstallDir, bPrintOnly);
				pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput, CLineReadFilter::UnixLineEnd);

				pFilter->open();
				pFilter->do_filter();
				pFilter->close();

				pFilter->release(); pFilter = NULL;
				pInput->release(); pInput = NULL;
				pOutput->release(); pOutput = NULL;
				pInputFile->release(); pInputFile = NULL;

				CWinDirectoryIterator::UnlinkFile(fpath11);
				if ( !bPrintOnly )
					CWinDirectoryIterator::UnlinkFile(fpath1);

				if ( NotPtrCheck(pInfo->GetInstallInfo()) )
					pInfo->GetInstallInfo()->SetIsInstalled(false);
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				CERR <<  _T("Uninstall: Doing list file: '") << fpath1.get_Path() << _T("' failed.") << endl;
				if (theConsoleApp->is_stdout_redirected())
				{
					COUT << ex->GetExceptionMessage() << endl;
					COUT << _T("Uninstall: Doing list file: '") << fpath1.get_Path() << _T("' failed.") << endl;
				}
				if ( NotPtrCheck(pInputFile) )
					pInputFile->release();
				if ( NotPtrCheck(pInput) )
					pInput->release();
				if ( NotPtrCheck(pOutput) )
					pOutput->release();
				if ( NotPtrCheck(pFilter) )
					pFilter->release();
			}
		}
		++it;
	}
}

void CPackageInfoVector::Install(CStringLiteral pInstallDir, Ref(CInstallPackageInfoVector) packInstalled, WBool bPrintOnly, WInt iVerbose)
{
	Iterator it;
	CInstallPackageInfoVector::Iterator itIP;
	CInstallPackageInfo ipInfo;
	Ptr(CInstallPackageInfo) pIPInfo = NULL;
	Ptr(CPackageInfo) pInfo = NULL;
	Ptr(CPackageInstallInfo) pPIInfo = NULL;
	ConstPtr(CInstallPackageInfo) pCIPInfo = NULL;
	Ptr(CSecurityFile) pFile = NULL;
	Ptr(CSecurityFile) pFile1 = NULL;
	Ptr(CSecurityFile) pFile2 = NULL;
	CFilePath fpath;
	CFilePath fpath1;
	CFilePath fpath2;
	CFilePath fpath3;
	CStringBuffer tmp;
	CDataVectorT<CStringBuffer> postInstallScripts(__FILE__LINE__ 16, 16);
	CDataVectorT<CStringBuffer>::Iterator itScript;
	CBashScript bashscript(pInstallDir);
	CCmdScript cmdscript(pInstallDir);
	CFTPClient ftpclient;
	CHttpDownload httpclient;
	CUrl url;

	it = Begin();
	while ( it )
	{
		pInfo = *it;
		COUT << _T("Install: Determine version of '") << pInfo->GetPackageName() << _T("' to be installed.") << endl;
		pPIInfo = pInfo->FindToBeInstalled();
		if ( NotPtrCheck(pPIInfo) )
		{
			pInfo->SetToBeInstalled(pPIInfo);
			COUT << _T("Install: Version is '") << pPIInfo->GetVendorVersion().GetVersion() << _T("'.") << endl;
		}
		else
			COUT << _T("Install: No valid version found, will not install.") << endl;
		++it;
	}
	it = Begin();
	while ( it )
	{
		pInfo = *it;
		pPIInfo = pInfo->GetToBeInstalled();
		if ( NotPtrCheck(pPIInfo) )
		{
			bool bExist = false;
			CSystemTime vCreateTime;
			CSystemTime vLastAccessTime;
			CSystemTime vLastWriteTime;

			pCIPInfo = pPIInfo->GetBinaryPackage();
			fpath.set_Path(pCIPInfo->GetRelativePath().get_Path(), CDirectoryIterator::UnixPathSeparatorString());
			fpath.Normalize(pPIInfo->GetRootPath().GetString());

			COUT << _T("Install: Download of '") << pInfo->GetPackageName() << _T("'.") << endl;
			try
			{
				bExist = CWinDirectoryIterator::FileExists(fpath);
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << ex->GetExceptionMessage() << endl;
				bExist = false;
			}
			if ( bExist )
			{
				try
				{
					if ( PtrCheck(pFile) )
						pFile = OK_NEW_OPERATOR CSecurityFile();

					pFile->Open(fpath);
					if ( 0 == pFile->GetSize() )
					{
						pFile->Close();
						CWinDirectoryIterator::UnlinkFile(fpath);
						bExist = false;
					}
					else
						pFile->Close();
					if ( bExist )
						CWinDirectoryIterator::ReadFileTimes(fpath, vCreateTime, vLastAccessTime, vLastWriteTime);
				}
				catch ( CBaseException* ex )
				{
					CERR << ex->GetExceptionMessage() << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << ex->GetExceptionMessage() << endl;
					bExist = false;
				}
			}
			fpath1 = fpath;
			fpath1.set_Filename(_T(""));
			try
			{
				CWinDirectoryIterator::MakeDirectory(fpath1);
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << ex->GetExceptionMessage() << endl;
			}

			tmp = fpath.get_Extension();
			tmp += _T(".bak");

			fpath1 = fpath;
			fpath1.set_Extension(tmp);

			fpath2 = pPIInfo->GetRootPath();
			fpath3.set_Path(pCIPInfo->GetRelativePath().get_Path(), CDirectoryIterator::UnixPathSeparatorString());

			try
			{
				if ( bExist && CWinDirectoryIterator::FileExists(fpath1) )
					CWinDirectoryIterator::UnlinkFile(fpath1);
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << ex->GetExceptionMessage() << endl;
			}
			try
			{
				if ( bExist )
					CWinDirectoryIterator::Rename(fpath, fpath1);
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << ex->GetExceptionMessage() << endl;
			}
			
			tmp = fpath2.get_Directory(-1);
			url.set_Encoded(tmp);

			if ( 0 == (url.get_Protocol().Compare(_T("ftp"), 0, CStringLiteral::cIgnoreCase)) )
			{
				CDateTime modTime;

				if ( !(ftpclient.IsConnected()) )
					ftpclient.OpenConnection(_T("anonymous"), _T("okinfo10@arcor.de"), url, fpath2);
				else if ( 0 != (url.get_Url().Compare(ftpclient.GetServerAndRoot().get_Url())) )
				{
					ftpclient.CloseConnection();
					ftpclient.OpenConnection(_T("anonymous"), _T("okinfo10@arcor.de"), url, fpath2);
				}

				ftpclient.RetrieveFileTime(fpath3, modTime);
				if ( (!bExist) || (vLastWriteTime < modTime) )
					ftpclient.RetrieveFile(fpath3, modTime);
				
				if ( 0 < ftpclient.GetErrCnt() )
				{
					CERR << ftpclient.GetLog().Join(_T("\r\n")) << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << ftpclient.GetLog().Join(_T("\r\n")) << endl;
					ftpclient.ResetLog();
					ftpclient.SetErrCnt(0);
					try
					{
						CWinDirectoryIterator::UnlinkFile(fpath);
					}
					catch ( CBaseException* ex )
					{
						CERR << ex->GetExceptionMessage() << endl;
						if (theConsoleApp->is_stdout_redirected())
							COUT << ex->GetExceptionMessage() << endl;
					}
				}
			}
			else if ( 0 == (url.get_Protocol().Compare(_T("http"), 0, CStringLiteral::cIgnoreCase)) )
			{
				CDateTime modTime;

				httpclient.OpenConnection(_T("anonymous"), _T("okinfo10@arcor.de"), url, fpath2);
				httpclient.RetrieveFileTime(fpath3, modTime);
				if ( (!bExist) || (vLastWriteTime < modTime) )
					httpclient.RetrieveFile(fpath3, modTime);
				httpclient.CloseConnection();
				if ( 0 < httpclient.GetErrCnt() )
				{
					CERR << httpclient.GetLog().Join(_T("\r\n")) << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << httpclient.GetLog().Join(_T("\r\n")) << endl;
					httpclient.ResetLog();
					httpclient.SetErrCnt(0);
					try
					{
						CWinDirectoryIterator::UnlinkFile(fpath);
					}
					catch ( CBaseException* ex )
					{
						CERR << ex->GetExceptionMessage() << endl;
						if (theConsoleApp->is_stdout_redirected())
							COUT << ex->GetExceptionMessage() << endl;
					}
				}
			}
			try
			{
				if ( !(CWinDirectoryIterator::FileExists(fpath)) && (CWinDirectoryIterator::FileExists(fpath1)) )
					CWinDirectoryIterator::Rename(fpath1, fpath);
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << ex->GetExceptionMessage() << endl;
			}
			COUT << _T("Install: Checking MD5 sum and filesize of '") << pInfo->GetPackageName() << _T("'.") << endl;
			try
			{
				if ( CWinDirectoryIterator::FileExists(fpath) )
				{
					if ( PtrCheck(pFile) )
						pFile = OK_NEW_OPERATOR CSecurityFile();

					pFile->Open(fpath);
					if ( pCIPInfo->GetFileSize() != pFile->GetSize() )
					{
						COUT << _T("Install: File Sizes differ: ") << pCIPInfo->GetFileSize() << _T(" and ") << pFile->GetSize() << _T(", will not install.") << endl;
						pInfo->SetToBeInstalled(NULL);
					}
					else
					{
						CMD5Buffer md5Buffer(pFile);

						if ( md5Buffer != pCIPInfo->GetMD5Buffer() )
						{
							COUT << _T("Install: MD5 sum differ: ") << md5Buffer.GetDigest() << _T(" and ") << pCIPInfo->GetMD5Buffer().GetDigest() << _T(", will not install.") << endl;
							pInfo->SetToBeInstalled(NULL);
						}
					}
					pFile->Close();
				}
				else
				{
					COUT << _T("Install: File '") << fpath.get_Path() << _T("' does not exist, will not install.") << endl;
					pInfo->SetToBeInstalled(NULL);
				}
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << ex->GetExceptionMessage() << endl;
				pInfo->SetToBeInstalled(NULL);
				if ( NotPtrCheck(pFile) )
					pFile->Close();
			}
		}
		++it;
	}
	ftpclient.CloseConnection();
	it = Begin();
	while ( it )
	{
		pInfo = *it;
		pPIInfo = pInfo->GetToBeInstalled();
		if ( NotPtrCheck(pPIInfo) )
		{
			bool bExist = false;

			COUT << _T("Install: Installing files of '") << pInfo->GetPackageName() << _T("'.") << endl;
			pCIPInfo = pPIInfo->GetBinaryPackage();
			fpath.set_Path(pCIPInfo->GetRelativePath().get_Path(), CDirectoryIterator::UnixPathSeparatorString());
			fpath.Normalize(pPIInfo->GetRootPath().get_Path());

			try
			{
				bExist = CWinDirectoryIterator::FileExists(fpath);
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << ex->GetExceptionMessage() << endl;
				bExist = false;
			}
			if ( bExist )
			{
				CFilterInput* pInput = NULL;
				CFilterOutput* pOutput = NULL;
				CFilter* pFilter = NULL;
				bool bCopied = false;
				bool bContinue = true;

				if ( PtrCheck(pFile) )
					pFile = OK_NEW_OPERATOR CSecurityFile();
				if ( PtrCheck(pFile1) )
					pFile1 = OK_NEW_OPERATOR CSecurityFile();
				if ( PtrCheck(pFile2) )
					pFile2 = OK_NEW_OPERATOR CSecurityFile();

				fpath1.set_Path(__FILE__LINE__ CYGWIN_TMP_DIR, CDirectoryIterator::UnixPathSeparatorString());
				fpath1.Normalize(pInstallDir);
				fpath1.set_Filename(fpath.get_Filename());
				tmp = fpath1.get_Extension();
				fpath1.set_Extension(_T(""));

				try
				{
					if ( tmp.Compare(_T("bz2"), 0, CStringLiteral::cIgnoreCase) == 0 )
					{
						pFile->Open(fpath);
						pInput = OK_NEW_OPERATOR CFileFilterInput(pFile);

						CWinDirectoryIterator::UnlinkFile(fpath1);
						pFile1->Create(fpath1, false, CFile::BinaryFile_NoEncoding, 0644);
						pOutput = OK_NEW_OPERATOR CFileFilterOutput(pFile1);

						pFilter = OK_NEW_OPERATOR CBZip2DeCompressFilter(pInput, pOutput);
					}
					else if ( tmp.Compare(_T("gz"), 0, CStringLiteral::cIgnoreCase) == 0 )
					{
						pFile->Open(fpath);
						pInput = OK_NEW_OPERATOR CFileFilterInput(pFile);

						CWinDirectoryIterator::UnlinkFile(fpath1);
						pFile1->Create(fpath1, false, CFile::BinaryFile_NoEncoding, 0644);
						pOutput = OK_NEW_OPERATOR CFileFilterOutput(pFile1);

						pFilter = OK_NEW_OPERATOR CGZipDeCompressFilter(pInput, pOutput);
					}
					else if ( tmp.Compare(_T("xz"), 0, CStringLiteral::cIgnoreCase) == 0 )
					{
						pFile->Open(fpath);
						pInput = OK_NEW_OPERATOR CFileFilterInput(pFile);

						CWinDirectoryIterator::UnlinkFile(fpath1);
						pFile1->Create(fpath1, false, CFile::BinaryFile_NoEncoding, 0644);
						pOutput = OK_NEW_OPERATOR CFileFilterOutput(pFile1);

						pFilter = OK_NEW_OPERATOR CXZDeCompressFilter(pInput, pOutput);
					}

					if ( NotPtrCheck(pFilter) )
					{
						pFilter->open();
						pFilter->do_filter();
						pFilter->close();

						pFilter->release(); pFilter = NULL;
						pInput->release(); pInput = NULL;
						pOutput->release(); pOutput = NULL;

						pFile->Close();
						pFile1->Close();

						bCopied = true;
					}
					if ( tmp.Compare(_T("tar"), 0, CStringLiteral::cIgnoreCase) == 0 )
						pFile->Open(fpath);
					else if ( bCopied )
						pFile->Open(fpath1);
					else
					{
						COUT << _T("Install: Unknown file format: '") << fpath.get_Path() << _T("'.") << endl;
						pInfo->SetToBeInstalled(NULL);
						bContinue = false;
					}
				}
				catch ( CBaseException* ex )
				{
					CERR << ex->GetExceptionMessage() << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << ex->GetExceptionMessage() << endl;
					pInfo->SetToBeInstalled(NULL);
					if ( NotPtrCheck(pInput) )
						pInput->release();
					pInput = NULL;
					if ( NotPtrCheck(pOutput) )
						pOutput->release();
					pOutput = NULL;
					if ( NotPtrCheck(pFilter) )
						pFilter->release();
					pFilter = NULL;
					pFile->Close();
					pFile1->Close();
					bContinue = false;
				}
				try
				{
					if ( bContinue )
					{
						CDataVectorT<CStringBuffer> archiveLstOutput(__FILE__LINE__ 16, 16);
						CTarArchive tarArchive(pFile);
						CArchiveIterator *tarIt = tarArchive.begin();

						while ( tarIt->Next() )
						{
							switch ( tarIt->GetType() )
							{
							case CArchiveIterator::ARCHIVE_FILE_REGULAR:
								{
									CStringConstIterator it;
									CArchiveFile* afile = NULL;

									tmp = tarIt->GetFileName();

									COUT << _T("Install: ARCHIVE_FILE_REGULAR: ") << tmp << endl;

									sqword vfiletime;
									sqword vfilemode;
									bool isNull;

									tarIt->GetProperty(_T("FILETIME"), vfiletime, isNull);
									tarIt->GetProperty(_T("FILEMODE"), vfilemode, isNull);

									archiveLstOutput.Append(tmp);

									it = tmp;
									it.Find(_T("etc/postinstall"));
									if ( !(it.IsEnd()) )
										postInstallScripts.Append(tmp);

									try
									{
										if ( !bPrintOnly )
										{
											it = tmp;
											it.Find(_T("usr/bin"));
											if ( it.IsBegin() )
											{
												tmp.DeleteString(0, 7);
												tmp.InsertString(0, _T("bin"));
											}
											it = tmp;
											it.Find(_T("usr/lib"));
											if ( it.IsBegin() )
											{
												tmp.DeleteString(0, 7);
												tmp.InsertString(0, _T("lib"));
											}
											fpath.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());
											fpath.Normalize(pInstallDir);

											tmp = fpath.get_Root();
											tmp += fpath.get_Directory();
											CWinDirectoryIterator::MakeDirectory(tmp, 0755);
											CWinDirectoryIterator::UnlinkFile(fpath);
											pFile1->Create(fpath, true, CFile::ISO_8859_1_Encoding, Cast(mode_t, vfilemode));
										}

										afile = tarIt->GetFile();

										pInput = OK_NEW_OPERATOR CFileFilterInput(afile);
										if ( bPrintOnly )
											pOutput = OK_NEW_OPERATOR CNullFilterOutput();
										else
											pOutput = OK_NEW_OPERATOR CFileFilterOutput(pFile1);
										pFilter = OK_NEW_OPERATOR CCopyFilter(pInput, pOutput);

										pFilter->open();
										pFilter->do_filter();
										pFilter->close();

										pFilter->release(); pFilter = NULL;
										pInput->release(); pInput = NULL;
										pOutput->release(); pOutput = NULL;
										afile->release(); afile = NULL;

										if ( !bPrintOnly )
										{
											pFile1->Close();

											CDateTime ftime(Cast(time_t, vfiletime));

											CWinDirectoryIterator::WriteFileTimes(fpath, ftime, ftime, ftime);
										}
									}
									catch ( CBaseException* ex )
									{
										CERR << ex->GetExceptionMessage() << endl;
										if (theConsoleApp->is_stdout_redirected())
											COUT << ex->GetExceptionMessage() << endl;
										if ( NotPtrCheck(pInput) )
											pInput->release();
										pInput = NULL;
										if ( NotPtrCheck(pOutput) )
											pOutput->release();
										pOutput = NULL;
										if ( NotPtrCheck(pFilter) )
											pFilter->release();
										pFilter = NULL;
										if ( NotPtrCheck(afile) )
											afile->release();
										afile = NULL;
										pFile1->Close();
									}
								}
								break;
							case CArchiveIterator::ARCHIVE_FILE_DIRECTORY:
								{
									CStringConstIterator it;

									tmp = tarIt->GetFileName();

									COUT << _T("Install: ARCHIVE_FILE_DIRECTORY: ") << tmp << endl;

									sqword vfiletime;
									sqword vfilemode;
									bool isNull;

									tarIt->GetProperty(_T("FILETIME"), vfiletime, isNull);
									tarIt->GetProperty(_T("FILEMODE"), vfilemode, isNull);

									CDateTime ftime(Cast(time_t, vfiletime));

									if ( !bPrintOnly )
									{
										archiveLstOutput.Append(tmp);

										it = tmp;
										it.Find(_T("usr/bin"));
										if ( it.IsBegin() )
										{
											tmp.DeleteString(0, 7);
											tmp.InsertString(0, _T("bin"));
										}
										it = tmp;
										it.Find(_T("usr/lib"));
										if ( it.IsBegin() )
										{
											tmp.DeleteString(0, 7);
											tmp.InsertString(0, _T("lib"));
										}

										fpath.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());
										fpath.Normalize(pInstallDir);

										try
										{
											CWinDirectoryIterator::MakeDirectory(fpath, Cast(mode_t, vfilemode));

											CWinDirectoryIterator::WriteFileTimes(fpath, ftime, ftime, ftime);
										}
										catch ( CBaseException* ex )
										{
											CERR << ex->GetExceptionMessage() << endl;
											if (theConsoleApp->is_stdout_redirected())
												COUT << ex->GetExceptionMessage() << endl;
										}
									}
								}
								break;
							case CArchiveIterator::ARCHIVE_FILE_HARDLINK:
								{
									CStringConstIterator it;

									tmp = tarIt->GetFileName(); // has to be created

									COUT << _T("Install: ARCHIVE_FILE_HARDLINK: ") << tmp;

									archiveLstOutput.Append(tmp);

									it = tmp;
									it.Find(_T("usr/bin"));
									if ( it.IsBegin() )
									{
										tmp.DeleteString(0, 7);
										tmp.InsertString(0, _T("bin"));
									}
									it = tmp;
									it.Find(_T("usr/lib"));
									if ( it.IsBegin() )
									{
										tmp.DeleteString(0, 7);
										tmp.InsertString(0, _T("lib"));
									}

									fpath.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());
									fpath.Normalize(pInstallDir);

									tmp = tarIt->GetLinkName(); // file that exists, may be relative to tmp

									COUT << _T(":") << tmp << endl;

									if ( !bPrintOnly )
									{
										it = tmp;
										it.Find(_T("usr/bin"));
										if ( it.IsBegin() )
										{
											tmp.DeleteString(0, 7);
											tmp.InsertString(0, _T("bin"));
										}
										it = tmp;
										it.Find(_T("usr/lib"));
										if ( it.IsBegin() )
										{
											tmp.DeleteString(0, 7);
											tmp.InsertString(0, _T("lib"));
										}

										fpath1.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());
										fpath1.Normalize(pInstallDir);

										try
										{
											if (CWinDirectoryIterator::FileExists(fpath))
												CWinDirectoryIterator::UnlinkFile(fpath);
											CWinDirectoryIterator::LinkFile(fpath1, fpath);
										}
										catch ( CBaseException* ex )
										{
											CERR << ex->GetExceptionMessage() << endl;
											if (theConsoleApp->is_stdout_redirected())
												COUT << ex->GetExceptionMessage() << endl;
										}
									}
								}
								break;
							case CArchiveIterator::ARCHIVE_FILE_SYMLINK:
								{
									CByteBuffer buf;
									CStringConstIterator it;

									tmp = tarIt->GetFileName(); // has to be created

									COUT << _T("Install: ARCHIVE_FILE_SYMLINK: ") << tmp;

									archiveLstOutput.Append(tmp);

									it = tmp;
									it.Find(_T("usr/bin"));
									if ( it.IsBegin() )
									{
										tmp.DeleteString(0, 7);
										tmp.InsertString(0, _T("bin"));
									}
									it = tmp;
									it.Find(_T("usr/lib"));
									if ( it.IsBegin() )
									{
										tmp.DeleteString(0, 7);
										tmp.InsertString(0, _T("lib"));
									}

									fpath.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());
									fpath.Normalize(pInstallDir);

									fpath1.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());

									tmp = tarIt->GetLinkName(); // file that exists, may be relative to tmp

									COUT << _T(":") << tmp << endl;

									it = tmp;
									if ( it[0] != CDirectoryIterator::UnixPathSeparatorChar() )
									{
										tmp.PrependString(fpath1.get_Directory());
										tmp.PrependString(CDirectoryIterator::UnixPathSeparatorString());

										COUT << _T("Install: ARCHIVE_FILE_SYMLINK: Made Target absolute = ") << tmp << endl;
									}

									if ( !bPrintOnly )
									{
										try
										{
											if ( CWinDirectoryIterator::FileExists(fpath) )
												CWinDirectoryIterator::UnlinkFile(fpath);

											pFile1->Create(fpath, false, CFile::BinaryFile_NoEncoding, 0644);
											tmp.PrependString(_T("!<symlink>"));
											tmp.convertToByteBuffer(buf);
											buf.concat_Buffer(CastAnyPtr(byte, "\0"), 1);
											pFile1->Write(buf);
											pFile1->Close();

											dword attrs = CWinDirectoryIterator::ReadFileAttributes(fpath);
											attrs |= FILE_ATTRIBUTE_SYSTEM;
											CWinDirectoryIterator::WriteFileAttributes(fpath, attrs);
										}
										catch ( CBaseException* ex )
										{
											CERR << ex->GetExceptionMessage() << endl;
											if (theConsoleApp->is_stdout_redirected())
												COUT << ex->GetExceptionMessage() << endl;
										}
									}
								}
								break;
							default:
								COUT << _T("Unknown type") << endl;
								break;
							}
						}
						tarIt->release();
						pFile->Close();
						if ( bCopied )
							CWinDirectoryIterator::UnlinkFile(pFile->GetPath());

						if ( !bPrintOnly )
						{
							pInput = OK_NEW_OPERATOR CStringVectorFilterInput(archiveLstOutput);

							fpath.set_Path(__FILE__LINE__ CYGWIN_TMP_DIR, CDirectoryIterator::UnixPathSeparatorString());
							fpath.Normalize(pInstallDir);
							tmp = pInfo->GetPackageName();
							tmp += _T(".lst");
							fpath.set_Filename(tmp);

							CWinDirectoryIterator::UnlinkFile(fpath);
							pFile2->Create(fpath, true, CFile::ISO_8859_1_Encoding, 0644);

							pOutput = OK_NEW_OPERATOR CFileFilterOutput(pFile2);
							pFilter = OK_NEW_OPERATOR CLineWriteFilter(pInput, pOutput, CLineWriteFilter::UnixLineEnd);

							pFilter->open();
							pFilter->do_filter();
							pFilter->close();

							pInput->release(); pInput = NULL;
							pOutput->release(); pOutput = NULL;
							pFilter->release(); pFilter = NULL;

							pFile2->Close();

							fpath1.set_Path(__FILE__LINE__ CYGWIN_SETUP_DIR, CDirectoryIterator::UnixPathSeparatorString());
							fpath1.Normalize(pInstallDir);
							tmp = pInfo->GetPackageName();
							tmp += _T(".lst.gz");
							fpath1.set_Filename(tmp);

							pFile1->Open(fpath);
							pInput = OK_NEW_OPERATOR CFileFilterInput(pFile1);

							CWinDirectoryIterator::UnlinkFile(fpath1);
							pFile2->Create(fpath1, false, CFile::BinaryFile_NoEncoding, 0644);
							pOutput = OK_NEW_OPERATOR CFileFilterOutput(pFile2);

							pFilter = OK_NEW_OPERATOR CGZipCompressFilter(pInput, pOutput);

							pFilter->open();
							pFilter->do_filter();
							pFilter->close();

							pInput->release(); pInput = NULL;
							pOutput->release(); pOutput = NULL;
							pFilter->release(); pFilter = NULL;

							pFile1->Close();
							pFile2->Close();

							CWinDirectoryIterator::UnlinkFile(fpath);
						}
					}
				}
				catch ( CBaseException* ex )
				{
					CERR << ex->GetExceptionMessage() << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << ex->GetExceptionMessage() << endl;
					pInfo->SetToBeInstalled(NULL);
					if ( NotPtrCheck(pInput) )
						pInput->release();
					if ( NotPtrCheck(pOutput) )
						pOutput->release();
					if ( NotPtrCheck(pFilter) )
						pFilter->release();
					pFile->Close();
					pFile1->Close();
					pFile2->Close();
				}
			}
			else
			{
				COUT << _T("Install: File '") << fpath.get_Path() << _T("' does not exist, will not install.") << endl;
				pInfo->SetToBeInstalled(NULL);
			}
		}
		++it;
	}
	itScript = postInstallScripts.Begin();
	while ( itScript )
	{
		tmp = *itScript;
		COUT << _T("Install: Doing postinstall '") << tmp << _T("'.") << endl;
		if ( !bPrintOnly )
		{
			fpath.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());
			fpath.set_Root(CDirectoryIterator::UnixPathSeparatorString());

			CStringBuffer ext(fpath.get_Extension());

			if ( ext.Compare(_T("sh"), 0, CStringLiteral::cIgnoreCase) == 0 )
			{
				bashscript.run(fpath);
				fpath.Normalize(pInstallDir);
			}
			else if ( ext.Compare(_T("bat"), 0, CStringLiteral::cIgnoreCase) == 0 )
			{
				fpath.Normalize(pInstallDir);
				cmdscript.run(fpath);
			}
			else if ( ext.Compare(_T("cmd"), 0, CStringLiteral::cIgnoreCase) == 0 )
			{
				fpath.Normalize(pInstallDir);
				cmdscript.run(fpath);
			}
			else
				fpath.Normalize(pInstallDir);
			tmp += _T(".done");
			fpath1.set_Path(tmp, CDirectoryIterator::UnixPathSeparatorString());
			fpath1.Normalize(pInstallDir);
			CWinDirectoryIterator::Rename(fpath, fpath1);
		}
		++itScript;
	}
	it = Begin();
	while ( it )
	{
		pInfo = *it;
		pPIInfo = pInfo->GetToBeInstalled();
		if ( NotPtrCheck(pPIInfo) )
		{
			tmp = pInfo->GetPackageName();
			tmp += _T("-");
			tmp += pPIInfo->GetVendorVersion().GetVersion();
			tmp += _T(".tar.bz2");

			ipInfo.SetName(pInfo->GetPackageName());
			itIP = packInstalled.FindSorted(&ipInfo, InstallPackageInfoSearchAndSortFunc);
			if ( (itIP && (*itIP) && (InstallPackageInfoSearchAndSortFunc(&ipInfo, *itIP) == 0)) )
			{
				pIPInfo = *itIP;
				pIPInfo->SetIsInstalled(true);
				pIPInfo->SetVersion(pPIInfo->GetVendorVersion());
				pIPInfo->SetRelativePath(tmp);
			}
			else
			{
				pIPInfo = OK_NEW_OPERATOR CInstallPackageInfo(pInfo->GetPackageName(), tmp, true);
				packInstalled.InsertSorted(pIPInfo, InstallPackageInfoSearchAndSortFunc);
			}
			packInstalled.SetModified(true);
		}
		++it;
	}
	if ( NotPtrCheck(pFile) )
		pFile->release();
	if ( NotPtrCheck(pFile1) )
		pFile1->release();
	if ( NotPtrCheck(pFile2) )
		pFile2->release();
}
