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
#include "ConsoleApplication.h"
#include "WinException.h"

CConsoleApplication::CConsoleApplication() :
CApplication(),
_stdout_init(false),
_stdout_redirected(false),
_stderr_init(false),
_stderr_redirected(false)
{
}

CConsoleApplication::CConsoleApplication(ConstRef(CStringBuffer) _defaultAppName):
CApplication(_defaultAppName),
_stdout_init(false),
_stdout_redirected(false),
_stderr_init(false),
_stderr_redirected(false)
{
}

CConsoleApplication::~CConsoleApplication()
{
}

bool CConsoleApplication::is_stdout_redirected()
{
	if (_stdout_init)
		return _stdout_redirected;

#ifdef OK_SYS_WINDOWS
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	_stdout_redirected = true;
	if (h == INVALID_HANDLE_VALUE)
		throw OK_NEW_OPERATOR CWinException(__FILE__LINE__ _T("Cannot retrieve handle to stdout."), _T("CConsoleApplication::is_stdout_redirected"), CWinException::WinExtError);
	switch (GetFileType(h))
	{
	case FILE_TYPE_UNKNOWN:
		if (GetLastError())
			throw OK_NEW_OPERATOR CBaseException(__FILE__LINE__ _T("Cannot retrieve handle to stdout."), _T("CConsoleApplication::is_stdout_redirected"), CWinException::WinExtError);
		break;
	case FILE_TYPE_DISK:
		break;
	case FILE_TYPE_CHAR:
		_stdout_redirected = false;
		break;
	case FILE_TYPE_PIPE:
		break;
	}
#endif
#ifdef OK_SYS_UNIX
	_stdout_redirected = !(isatty(STDOUT_FILENO));
#endif
	_stdout_init = true;
	return _stdout_redirected;
}

bool CConsoleApplication::is_stderr_redirected()
{
	if (_stderr_init)
		return _stderr_redirected;

#ifdef OK_SYS_WINDOWS
	HANDLE h = GetStdHandle(STD_ERROR_HANDLE);

	_stderr_redirected = true;
	if (h == INVALID_HANDLE_VALUE)
		throw OK_NEW_OPERATOR CWinException(__FILE__LINE__ _T("Cannot retrieve handle to stderr."), _T("CConsoleApplication::is_stderr_redirected"), CWinException::WinExtError);
	switch (GetFileType(h))
	{
	case FILE_TYPE_UNKNOWN:
		if (GetLastError())
			throw OK_NEW_OPERATOR CBaseException(__FILE__LINE__ _T("Cannot retrieve handle to stderr."), _T("CConsoleApplication::is_stderr_redirected"), CWinException::WinExtError);
		break;
	case FILE_TYPE_DISK:
		break;
	case FILE_TYPE_CHAR:
		_stderr_redirected = false;
		break;
	case FILE_TYPE_PIPE:
		break;
	}
#endif
#ifdef OK_SYS_UNIX
	_stderr_redirected = !(isatty(STDERR_FILENO));
#endif
	_stderr_init = true;
	return _stderr_redirected;
}

