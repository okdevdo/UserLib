/******************************************************************************
    
	This file is part of XSetup, which is part of UserLib.

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
#include "SetupIniFile.h"
#include "DirectoryIterator.h"
#include "SecurityFile.h"
#include "URL.h"
#include "FTPClient.h"
#include "HTTPDownload.h"
#include "WinDirectoryIterator.h"

CSetupIniFileVector::CSetupIniFileVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp):
    CDataVectorT<CStringBuffer>(ARGS_FILE_LINE cnt, exp)
{
}

CSetupIniFileVector::~CSetupIniFileVector()
{
}

void CSetupIniFileVector::Fill(CStringLiteral psSetupIniScanDir, CDataVectorT<mbchar>& psSetupIniFiles)
{
	bool bScanDir = false;
	CStringBuffer sScanDir;

	if ( (psSetupIniFiles.Count() == 0) && psSetupIniScanDir.IsEmpty() )
	{
		CFilePath curdir;

		CDirectoryIterator::GetCurrentDirectory(curdir);
		sScanDir = curdir.get_Path();
		if ( sScanDir.IsEmpty() )
			return;
		bScanDir = true;
	}
	else
	{
		if ( !(psSetupIniScanDir.IsEmpty()) )
		{
			bScanDir = (CDirectoryIterator::FileExists(CFilePath(__FILE__LINE__ psSetupIniScanDir)))?
				false:
				(CDirectoryIterator::DirectoryExists(CFilePath(__FILE__LINE__ psSetupIniScanDir)) >= 0);
			sScanDir.SetString(__FILE__LINE__ psSetupIniScanDir);
		}
		if ( psSetupIniFiles.Count() > 0 )
		{
			CDataVectorT<mbchar>::Iterator it = psSetupIniFiles.Begin();

			while ( it )
			{
				CFilePath fpath(__FILE__LINE__ *it);

				if ( CDirectoryIterator::FileExists(fpath) )
					Append(fpath.get_Path());
				++it;
			}
		}
	}
	if ( bScanDir )
		ScanDirectoryHelper(sScanDir.GetString());
}

void CSetupIniFileVector::ScanDirectoryHelper(CStringLiteral dir)
{
	CFilePath path;
	CDataVectorT<CStringBuffer> dirs(__FILE__LINE__ 16, 16);
	CDataVectorT<CStringBuffer>::Iterator itDir;

	path.set_Directory(dir);
	path.set_Basename(_T("*"));

	CDirectoryIterator it(path);
	CStringBuffer buf;
	CStringBuffer setup_ini_file(__FILE__LINE__ _T("setup.ini"));

	while ( it )
	{
		buf = it.get_Name();
		if ( it.is_SubDir() )
		{
			dirs.Append(buf);
		}
		else if ( s_stricmp(buf.GetString(), setup_ini_file.GetString()) == 0 )
		{
			Append(it.get_Path());
			return;
		}
		++it;
	}
	itDir = dirs.Begin();

	while ( itDir )
	{
		path.set_Filename(*itDir);

		ScanDirectoryHelper(path.get_Path().GetString());

		++itDir;
	}
}

void CSetupIniFileVector::Load()
{
	CFTPClient ftpclient;
	CHttpDownload httpclient;
	Iterator it = Begin();

	ftpclient.SetLog(true);
	httpclient.SetLog(true);
	while ( it )
	{
		CFilePath fpath(*it);
		CSystemTime vCreateTime;
		CSystemTime vLastAccessTime;
		CSystemTime vLastWriteTime;

		try
		{
			CWinDirectoryIterator::ReadFileTimes(fpath, vCreateTime, vLastAccessTime, vLastWriteTime);
		}
		catch ( CSecurityFileException* ex )
		{
			CERR << ex->GetExceptionMessage() << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << ex->GetExceptionMessage() << endl;
		}

		CFilePath fpath1(*it);

		fpath1.set_Extension(_T("ini.bak"));

		try
		{
			CWinDirectoryIterator::UnlinkFile(fpath1);
		}
		catch ( CSecurityFileException* ex )
		{
			CERR << ex->GetExceptionMessage() << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << ex->GetExceptionMessage() << endl;
		}
		try
		{
			CWinDirectoryIterator::Rename(fpath, fpath1);
		}
		catch ( CSecurityFileException* ex )
		{
			CERR << ex->GetExceptionMessage() << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << ex->GetExceptionMessage() << endl;
		}
			
		CFilePath fpath2(*it);
		CStringBuffer setup_ini_file;

		setup_ini_file = fpath2.get_Directory(-1);
		fpath2.set_Filename(_T(""));
		fpath2.set_Directory(_T(""), -1);
		setup_ini_file.AppendString(_T("/setup.ini"));

		CStringBuffer sUrl(fpath2.get_Directory(-1));
		CUrl url;

		url.set_Encoded(sUrl);
		if ( 0 == (url.get_Protocol().Compare(_T("ftp"), 0, CStringLiteral::cIgnoreCase)) )
		{
			CDateTime modTime;

			ftpclient.OpenConnection(_T("anonymous"), _T("okinfo10@arcor.de"), url, fpath2);
			ftpclient.RetrieveFileTime(setup_ini_file, modTime);
			if ( vLastWriteTime < modTime )
				ftpclient.RetrieveFile(setup_ini_file, modTime);
			ftpclient.CloseConnection();
			if ( 0 < ftpclient.GetErrCnt() )
			{
				CERR << ftpclient.GetLog().Join(_T("\r\n")) << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << ftpclient.GetLog().Join(_T("\r\n")) << endl;
				try
				{
					CWinDirectoryIterator::UnlinkFile(fpath);
				}
				catch ( CSecurityFileException* ex )
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
			httpclient.RetrieveFileTime(setup_ini_file, modTime);
			if ( vLastWriteTime < modTime )
				httpclient.RetrieveFile(setup_ini_file, modTime);
			httpclient.CloseConnection();
			if ( 0 < httpclient.GetErrCnt() )
			{
				CERR << httpclient.GetLog().Join(_T("\r\n")) << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << httpclient.GetLog().Join(_T("\r\n")) << endl;
				try
				{
					CWinDirectoryIterator::UnlinkFile(fpath);
				}
				catch ( CSecurityFileException* ex )
				{
					CERR << ex->GetExceptionMessage() << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << ex->GetExceptionMessage() << endl;
				}
			}
		}
		try
		{
			if ( !(CWinDirectoryIterator::FileExists(fpath)) )
				CWinDirectoryIterator::Rename(fpath1, fpath);
		}
		catch ( CSecurityFileException* ex )
		{
			CERR << ex->GetExceptionMessage() << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << ex->GetExceptionMessage() << endl;
		}
		++it;
	}
}
