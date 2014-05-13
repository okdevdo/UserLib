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
#include "WinException.h"

#ifdef OK_SYS_WINDOWS
#include <winsock2.h>
#ifdef OK_COMP_MSC
#pragma comment (lib, "Ws2_32.lib")
#endif
#endif
#ifdef OK_SYS_UNIX
#include <netdb.h>
#endif

CWinException::CWinException():
	CBaseException()
{
}

CWinException::CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format) :
CBaseException(ARGS_FILE_LINE format)
{
}

CWinException::CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer sArg) :
CBaseException(ARGS_FILE_LINE format, sArg)
{
}

CWinException::CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer sArg1, CConstPointer sArg2) :
CBaseException(ARGS_FILE_LINE format, sArg1, sArg2)
{
}

CWinException::CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, int sArg) :
CBaseException(ARGS_FILE_LINE format, sArg)
{
}

CWinException::CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CWinException::TErrorType errType, long errCode)
{
	m_Message.FormatString(__FILE__LINE__ format, func);
#ifndef NDEBUG
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
	m_Message.AppendString(GetMessagePostfix(errType, errCode));
}

CWinException::CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer sArg, CWinException::TErrorType errType, long errCode)
{
	m_Message.FormatString(__FILE__LINE__ format, func, sArg);
#ifndef NDEBUG
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
	m_Message.AppendString(GetMessagePostfix(errType, errCode));
}

CWinException::CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer sArg1, CConstPointer sArg2, CWinException::TErrorType errType, long errCode)
{
	m_Message.FormatString(__FILE__LINE__ format, func, sArg1, sArg2);
#ifndef NDEBUG
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
	m_Message.AppendString(GetMessagePostfix(errType, errCode));
}

CWinException::~CWinException(void)
{
}

CStringBuffer CWinException::GetMessagePostfix(CWinException::TErrorType errType, long errCode)
{
	CStringBuffer tmp;

	switch ( errType )
	{
	case CRunTimeError:
		tmp = CBaseException::GetMessagePostfix((errCode == 0)?errno:errCode);
		break;
#ifdef OK_SYS_WINDOWS
	case WinExtError:
		{
			LPVOID lpMsgBuf;
			DWORD lastError = (errCode == 0)?GetLastError():errCode;

			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				lastError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
	
			tmp.FormatString(__FILE__LINE__ _T(" (Windows Error: 0x%08x - %s)"), lastError, lpMsgBuf);
			LocalFree( lpMsgBuf );
		}
		break;
	case WSAExtError:
		{
			LPVOID lpMsgBuf;
			DWORD lastError = (errCode == 0)?WSAGetLastError():errCode;

			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				lastError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
	
			tmp.FormatString(__FILE__LINE__ _T(" (Windows Sockets Error: 0x%08x - %s)"), lastError, lpMsgBuf);
			LocalFree( lpMsgBuf );
		}
		break;
#ifdef OK_COMP_MSC
	case BCryptStatus:
		tmp.FormatString(__FILE__LINE__ _T(" (BCrypt status: 0x%08x - "), errCode);
		switch (errCode)
		{
		case STATUS_INVALID_HANDLE:
			tmp.AppendString(_T("Invalid Handle)"));
			break;
		case STATUS_INVALID_PARAMETER:
			tmp.AppendString(_T("Invalid Parameter)"));
			break;
		case STATUS_NO_MEMORY:
			tmp.AppendString(_T("No Memory)"));
			break;
		default:
			tmp.AppendString(_T(")"));
			break;
		}
		break;
#endif
#endif
#ifdef OK_SYS_UNIX
	case GAIExtError:
		{
			errno_t err = (errCode == 0)?errno:errCode;
			const char *buf = gai_strerror(err);
    
			tmp.FormatString(__FILE__LINE__ _T(" (GetAddrInfo Error: %d - %s)"), err, buf);
		}
		break;
#endif
	default:
		break;
	}
	return tmp;
}
