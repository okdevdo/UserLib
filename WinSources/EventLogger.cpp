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
#include "EventLogger.h"
#if (WINVER >= _WIN32_WINNT_VISTA)
#include <evntprov.h>
#include "okreis.h"
#endif
#include <Aclapi.h>
#include "provider.h"
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS32
#include <stdio.h>
#endif
#ifdef OK_SYS_WINDOWS64
#include <stdio.h>
#endif
#ifdef OK_SYS_UNIX
#endif
#endif
#ifdef OK_COMP_MSC
#endif

#define PROVIDER_NAME _T("OKreisClassic")

class WINSOURCES_LOCAL CEventLoggerImpl : public CCppObject
{
public:
	CEventLoggerImpl();
	virtual ~CEventLoggerImpl();

	void WriteLog(CEventLogger::InfoLevel level, CConstPointer msgtext);
	void WriteClassicLog(CEventLogger::InfoLevel level, CEventLogger::Category category, CConstPointer msgtext);

protected:
	void InitializeLog();
	void DeInitializeLog();
	void InitializeClassicLog();
	void DeInitializeClassicLog();

#if (WINVER >= _WIN32_WINNT_VISTA)
	REGHANDLE _handle;
#endif
	HANDLE _classicHandle;
};

IMPL_WINEXCEPTION(CEventLoggerException, CWinException)

CEventLoggerImpl::CEventLoggerImpl():
#if (WINVER >= _WIN32_WINNT_VISTA)
_handle(0LL),
#endif
_classicHandle(NULL)
{
	InitializeLog();
	InitializeClassicLog();
}

CEventLoggerImpl::~CEventLoggerImpl()
{
	DeInitializeLog();
	DeInitializeClassicLog();
}

void CEventLoggerImpl::InitializeLog()
{
#if (WINVER >= _WIN32_WINNT_VISTA)
	if (!_handle)
	{
		DWORD status = ERROR_SUCCESS;

		status = EventRegister(
			&OKREIS_PROVIDER_GUID,	// GUID that identifies the provider
			NULL,					// Callback not used
			NULL,					// Context not used
			&_handle				// Used when calling EventWrite and EventUnregister
			);
		if (ERROR_SUCCESS != status)
			throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLoggerImpl::InitializeLog"), CWinException::WinExtError, status);
	}
#endif
}

void CEventLoggerImpl::DeInitializeLog()
{
#if (WINVER >= _WIN32_WINNT_VISTA)
	if (_handle)
	{
		EventUnregister(_handle);
		_handle = NULL;
	}
#endif
}

void CEventLoggerImpl::InitializeClassicLog()
{
	if (!_classicHandle)
	{
		_classicHandle = RegisterEventSource(NULL, PROVIDER_NAME);
		if (!_classicHandle)
			throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLoggerImpl::InitializeClassicLog"), CWinException::WinExtError);
	}
}

void CEventLoggerImpl::DeInitializeClassicLog()
{
	if (_classicHandle)
	{
		DeregisterEventSource(_classicHandle);
		_classicHandle = NULL;
	}
}

void CEventLoggerImpl::WriteLog(CEventLogger::InfoLevel level, CConstPointer msgtext)
{
#if (WINVER >= _WIN32_WINNT_VISTA)
	DWORD status = ERROR_SUCCESS;
	EVENT_DATA_DESCRIPTOR DataDescriptors[1];
	PCEVENT_DESCRIPTOR EventDescriptor = NULL;

	InitializeLog();
	switch (level)
	{
	case CEventLogger::Information:
		EventDescriptor = &OKREIS_EVENT_1;
		break;
	case CEventLogger::Warning:
		EventDescriptor = &OKREIS_EVENT_2;
		break;
	case CEventLogger::Error:
	default:
		EventDescriptor = &OKREIS_EVENT_3;
		break;
	}

	EventDataDescCreate(&DataDescriptors[0], msgtext, (ULONG)(_tcslen(msgtext) + 1) * sizeof(TCHAR));

	status = EventWrite(
		_handle,				// From EventRegister
		EventDescriptor,        // EVENT_DESCRIPTOR generated from the manifest
		1,						// Size of the array of EVENT_DATA_DESCRIPTORs
		DataDescriptors		    // Array of descriptors that contain the event data
		);

	if (status != ERROR_SUCCESS)
		throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLogger::WriteLog"), CWinException::WinExtError, status);
#endif
}

void CEventLoggerImpl::WriteClassicLog(CEventLogger::InfoLevel level, CEventLogger::Category category, CConstPointer msgtext)
{
	HANDLE hToken = NULL;
	DWORD status = ERROR_SUCCESS;
	PSID pOwnerSid = NULL;
	PSECURITY_DESCRIPTOR pSecurityDesc = NULL;
	LPCTSTR pInsertStrings[1] = { msgtext };
	WORD wType;
	WORD wCategory;

	InitializeClassicLog();
	if (!OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
		throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLoggerImpl::WriteClassicLog"), CWinException::WinExtError);

	status = GetSecurityInfo(hToken, SE_KERNEL_OBJECT, OWNER_SECURITY_INFORMATION, &pOwnerSid, NULL, NULL, NULL, &pSecurityDesc);
	if (ERROR_SUCCESS != status)
		throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLogger::WriteLog"), CWinException::WinExtError, status);

	switch (level)
	{
	case CEventLogger::Information:
		wType = EVENTLOG_INFORMATION_TYPE;
		break;
	case CEventLogger::Warning:
		wType = EVENTLOG_WARNING_TYPE;
		break;
	case CEventLogger::Error:
	default:
		wType = EVENTLOG_ERROR_TYPE;
		break;
	}
	switch (category)
	{
	case CEventLogger::NetworkCategory:
		wCategory = NETWORK_CATEGORY;
		break;
	case CEventLogger::DatabaseCategory:
		wCategory = DATABASE_CATEGORY;
		break;
	case CEventLogger::GUICategory:
		wCategory = UI_CATEGORY;
		break;
	}
	if (!ReportEvent(_classicHandle, wType, wCategory, MSG_ERROR_TEXT, pOwnerSid, 1, 0, pInsertStrings, NULL))
		throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLoggerImpl::WriteClassicLog"), CWinException::WinExtError);

	if (pSecurityDesc)
		LocalFree(pSecurityDesc);

	if (hToken != INVALID_HANDLE_VALUE)
		CloseHandle(hToken);
}

Ptr(CEventLoggerImpl) CEventLogger::_impl = NULL;

CEventLogger::CEventLogger()
{
}

CEventLogger::~CEventLogger()
{
}

void CEventLogger::WriteLog(InfoLevel level, CConstPointer msgtext)
{
	if (!_impl)
		_impl = OK_NEW_OPERATOR CEventLoggerImpl;
	if (_impl)
		_impl->WriteLog(level, msgtext);
}

void CEventLogger::WriteFormattedLog(InfoLevel level, CConstPointer _format, ...)
{
	va_list args;
	int cnt = 0;
	LPTSTR buf = NULL;

	va_start(args, _format);
	cnt = _vsctprintf(_format, args);
	va_end(args);
	if (cnt < 0)
		throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLogger::WriteFormattedLog"), CWinException::CRunTimeError);
	buf = CastAny(LPTSTR, TFalloc((cnt + 16) * szchar));
	va_start(args, _format);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS32
#ifdef _UNICODE
	cnt = _vsnwprintf(buf, cnt + 16, _format, args);
#else
	cnt = vsnprintf(buf, cnt + 16, _format, args);
#endif
#endif
#ifdef OK_SYS_WINDOWS64
#ifdef _UNICODE
	cnt = _vsnwprintf(buf, cnt + 16, _format, args);
#else
	cnt = vsnprintf(buf, cnt + 16, _format, args);
#endif
#endif
#ifdef OK_SYS_UNIX
#endif
#endif
#ifdef OK_COMP_MSC
	cnt = _vstprintf_s(buf, cnt + 16, _format, args);
#endif
	va_end(args);
	if (cnt < 0)
		throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLogger::WriteFormattedLog"), CWinException::CRunTimeError);
	try
	{
		if (!_impl)
			_impl = OK_NEW_OPERATOR CEventLoggerImpl;
		if (_impl)
			_impl->WriteLog(level, buf);
		TFfree(buf);
	}
	catch (CEventLoggerException* ex)
	{
		TFfree(buf);
		throw ex;
	}
}

void CEventLogger::WriteClassicLog(InfoLevel level, Category category, CConstPointer msgtext)
{
	if (!_impl)
		_impl = OK_NEW_OPERATOR CEventLoggerImpl;
	if (_impl)
		_impl->WriteClassicLog(level, category, msgtext);
}

void CEventLogger::WriteFormattedClassicLog(InfoLevel level, Category category, CConstPointer _format, ...)
{
	va_list args;
	int cnt = 0;
	LPTSTR buf = NULL;

	va_start(args, _format);
	cnt = _vsctprintf(_format, args);
	va_end(args);
	if (cnt < 0)
		throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLogger::WriteFormattedLog"), CWinException::CRunTimeError);
	buf = CastAny(LPTSTR, TFalloc((cnt + 16) * szchar));
	va_start(args, _format);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS32
#ifdef _UNICODE
	cnt = _vsnwprintf(buf, cnt + 16, _format, args);
#else
	cnt = vsnprintf(buf, cnt + 16, _format, args);
#endif
#endif
#ifdef OK_SYS_WINDOWS64
#ifdef _UNICODE
	cnt = _vsnwprintf(buf, cnt + 16, _format, args);
#else
	cnt = vsnprintf(buf, cnt + 16, _format, args);
#endif
#endif
#ifdef OK_SYS_UNIX
#endif
#endif
#ifdef OK_COMP_MSC
	cnt = _vstprintf_s(buf, cnt + 16, _format, args);
#endif
	va_end(args);
	if (cnt < 0)
		throw OK_NEW_OPERATOR CEventLoggerException(__FILE__LINE__ _T("in %s CEventLoggerException"), _T("CEventLogger::WriteFormattedLog"), CWinException::CRunTimeError);
	try
	{
		if (!_impl)
			_impl = OK_NEW_OPERATOR CEventLoggerImpl;
		if (_impl)
			_impl->WriteClassicLog(level, category, buf);
		TFfree(buf);
	}
	catch (CEventLoggerException* ex)
	{
		TFfree(buf);
		throw ex;
	}
}

void CEventLogger::CleanUp()
{
	if (_impl)
	{
		_impl->release();
		_impl = NULL;
	}
}