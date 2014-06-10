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
#include "HTTPDownload.h"
#include "HttpDate.h"
#include "DirectoryIterator.h"
#include "WinDirectoryIterator.h"
#include "SecurityFile.h"

CHttpDownload::CHttpDownload(void):
    CHttpClient(),
	_username(),
	_password(),
	_serverandroot(),
	_diskrootpath(),
	_unixworkingdir()
{
}

CHttpDownload::CHttpDownload(ConstRef(CStringLiteral) user, ConstRef(CStringLiteral) passwd, ConstRef(CUrl) serverandroot, ConstRef(CFilePath) diskrootpath):
    CHttpClient(),
	_username(),
	_password(),
	_serverandroot(),
	_diskrootpath(),
	_unixworkingdir()
{
	OpenConnection(user, passwd, serverandroot, diskrootpath);
}

CHttpDownload::~CHttpDownload(void)
{
	CloseConnection();
}

void CHttpDownload::OpenConnection(ConstRef(CStringLiteral) user, ConstRef(CStringLiteral) passwd, ConstRef(CUrl) serverandroot, ConstRef(CFilePath) diskrootpath)
{
	_username = user;
	_password = passwd;
	_serverandroot = serverandroot;
	_diskrootpath = diskrootpath;

	if ( _serverandroot.get_Protocol().Compare(_T("http"), 0, CStringLiteral::cIgnoreCase) != 0 )
	{
		CStringBuffer tmp;

		++_errcnt;
		tmp.FormatString(__FILE__LINE__ _T("Protocol error: %s"), serverandroot.get_Url().GetString());
		_log.Append(tmp);
		return;
	}

	_unixworkingdir.set_Path(_serverandroot.get_Resource(), CDirectoryIterator::UnixPathSeparatorString());
}

void CHttpDownload::ChangeWorkingDir(ConstRef(CFilePath) _relativeunixpath, bool reset2root)
{
	if ( _relativeunixpath.is_Absolute() )
	{
		CStringBuffer tmp;

		++_errcnt;
		tmp.FormatString(__FILE__LINE__ _T("ChangeWorkingDir, Path error: %s"), _relativeunixpath.get_Path().GetString());
		_log.Append(tmp);
		return;
	}
	if ( reset2root )
		_unixworkingdir.set_Path(_serverandroot.get_Resource(), CDirectoryIterator::UnixPathSeparatorString());
	_unixworkingdir.set_Filename(_relativeunixpath.get_Path());
}

void CHttpDownload::ChangeToParentDir()
{
	if ( _unixworkingdir.get_Path().Compare(_serverandroot.get_Resource(), 0, CStringLiteral::cIgnoreCase) == 0 )
	{
		CStringBuffer tmp;

		++_errcnt;
		tmp.FormatString(__FILE__LINE__ _T("ChangeToParentDir, Path error: %s"), _unixworkingdir.get_Path().GetString());
		_log.Append(tmp);
		return;
	}
	_unixworkingdir.set_Directory(_T(""), -1);
}

void CHttpDownload::RetrieveFileTime(ConstRef(CFilePath) _relativeunixpath, Ref(CDateTime) modTime)
{
	ClearAll();

	CFilePath fpath(_unixworkingdir);

	fpath.set_Filename(_relativeunixpath.get_Path());

	InitRequest(_serverandroot.get_Server(), fpath.get_Path());
	if ( _open )
		Load(STAY_PERSISTENT, HEAD_COMMAND);
	else
		Load(OPEN_PERSISTENT, HEAD_COMMAND);

	CStringBuffer sBuf(super::get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Last-Modified"))));
	
	if ( sBuf.IsEmpty() )
		return;

	CHttpDate date(sBuf);

	modTime.SetTime(date.GetYears(), date.GetMonths(), date.GetDays(), date.GetHours(), date.GetMinutes(), date.GetSeconds());
}

void CHttpDownload::RetrieveFile(ConstRef(CFilePath) _relativeunixpath, ConstRef(CDateTime) modTime)
{
	ClearAll();

	CFilePath fpath(_unixworkingdir);

	fpath.set_Filename(_relativeunixpath.get_Path());

	InitRequest(_serverandroot.get_Server(), fpath.get_Path());
	if ( _open )
		Load(STAY_PERSISTENT);
	else
		Load(OPEN_PERSISTENT);

	if ( 0 == _responseContent.GetTotalLength() )
	{
		CStringBuffer tmp;

		++_errcnt;
		tmp.FormatString(__FILE__LINE__ _T("RetrieveFile, No Content for file: %s"), fpath.get_Path().GetString());
		_log.Append(tmp);
		return;
	}

	CStringBuffer sBuf(fpath.get_Path());

	sBuf.DeleteString(0, _serverandroot.get_Resource().GetLength());
	fpath.set_Path(sBuf, CDirectoryIterator::UnixPathSeparatorString());
	fpath.Normalize(_diskrootpath.get_Path());

	CSecurityFile file;

	try
	{
		if ( CWinDirectoryIterator::FileExists(fpath) )
			CWinDirectoryIterator::UnlinkFile(fpath);
		else
		{
			CFilePath dirPath(fpath);

			dirPath.set_Filename(_T(""));
			CWinDirectoryIterator::MakeDirectory(dirPath);
		}
		file.Create(fpath);
		file.Write(_responseContent);
		file.Close();
		CWinDirectoryIterator::WriteFileTimes(fpath, modTime, modTime, modTime);
	}
	catch ( CSecurityFileException* ex )
	{
		file.Close();
		++_errcnt;
		_log.Append(ex->GetExceptionMessage());
	}
}

void CHttpDownload::CloseConnection()
{
	_tcpClient.CloseConnection();
	_open = false;
}
