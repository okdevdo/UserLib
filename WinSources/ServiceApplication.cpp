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
#include "ServiceApplication.h"
#include "EventLogger.h"

CServiceApplication::CServiceApplication(LPCTSTR appname) :
CApplication(CStringBuffer(__FILE__LINE__ appname)),
_is_service(false),
_is_init(false)
{
}

CServiceApplication::~CServiceApplication()
{
}

bool CServiceApplication::is_service()
{
	if (_is_init)
		return _is_service;

	HWINSTA h;
	DWORD len;
	TCHAR *name;

	h = GetProcessWindowStation();
	if (h == NULL)
	{
		CEventLogger::WriteLog(CEventLogger::Error, _T("GetProcessWindowStation failed"));
		CEventLogger::CleanUp();
		return true;
	}
	if (GetUserObjectInformation(h, UOI_NAME, NULL, 0, &len) || (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
	{
		CEventLogger::WriteLog(CEventLogger::Error, _T("GetProcessWindowStation failed"));
		CEventLogger::CleanUp();
		return true;
	}
	name = (TCHAR *)TFalloc((len + 1) * sizeof(TCHAR));
	if (!GetUserObjectInformation(h, UOI_NAME, name, len, &len))
	{
		CEventLogger::WriteLog(CEventLogger::Error, _T("GetProcessWindowStation failed"));
		CEventLogger::CleanUp();
		return true;
	}
	_is_service = (s_strncmp(name, _T("Service-0x"), 10) == 0);
	_is_init = true;
	TFfree(name);
	return _is_service;
}

int CServiceApplication::run(int argc, CArray argv)
{
	int result = 4;

	try
	{
		loadConfiguration(argv[0]);
		if (processOptions(argc, argv))
			result = main();
	}
	catch (COptionException* ex)
	{
		CStringBuffer outBuf;

		outBuf.AppendString(ex->GetExceptionMessage());
		outBuf.AppendString(_T("\n"));
		usage(outBuf);
		CEventLogger::WriteLog(CEventLogger::Error, outBuf);
		CEventLogger::CleanUp();
		result = -4;
	}
	catch (CBaseException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
		CEventLogger::CleanUp();
		result = -4;
	}
	catch (...)
	{
		CEventLogger::WriteLog(CEventLogger::Error, _T("FATAL error"));
		CEventLogger::CleanUp();
		result = -4;
	}
	return result;
}

