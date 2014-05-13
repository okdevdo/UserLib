/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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
#include "CPPS_PCH.H"
#include "BaseException.h"

CBaseException::CBaseException(void): 
    m_Message()
{
}

CBaseException::CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format)
{
	m_Message.SetString(__FILE__LINE__ format);
#ifdef __DEBUG__
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
}

CBaseException::CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer arg1)
{
	m_Message.FormatString(__FILE__LINE__ format, arg1);
#ifdef __DEBUG__
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
}

CBaseException::CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer arg1, CConstPointer arg2)
{
	m_Message.FormatString(__FILE__LINE__ format, arg1, arg2);
#ifdef __DEBUG__
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
}

CBaseException::CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, int arg1)
{
	m_Message.FormatString(__FILE__LINE__ format, arg1);
#ifdef __DEBUG__
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
}

CBaseException::CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, errno_t err)
{
	m_Message.FormatString(__FILE__LINE__ format, func);
#ifdef __DEBUG__
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
	m_Message.AppendString(GetMessagePostfix(err));
}

CBaseException::CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer path, errno_t err)
{
	m_Message.FormatString(__FILE__LINE__ format, func, path);
#ifdef __DEBUG__
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
	m_Message.AppendString(GetMessagePostfix(err));
}

CBaseException::CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer path1, CConstPointer path2, errno_t err)
{
	m_Message.FormatString(__FILE__LINE__ format, func, path1, path2);
#ifdef __DEBUG__
	m_Message.PrependString(GetMessagePrefix(file, line));
#endif
	m_Message.AppendString(GetMessagePostfix(err));
}

CBaseException::~CBaseException(void)
{
}

#ifdef __DEBUG__
CStringBuffer CBaseException::GetMessagePrefix(const char* file, int line)
{
	CStringBuffer tmp;

#ifdef OK_SYS_WINDOWS
#ifdef _UNICODE
	tmp.FormatString(__FILE__LINE__ _T("%hs(%d): "), file, line);
#else
	tmp.FormatString(__FILE__LINE__ _T("%s(%d): "), file, line);
#endif
#endif
#ifdef OK_SYS_UNIX
	tmp.FormatString(__FILE__LINE__ _T("%s(%d): "), file, line);
#endif
	return tmp;
}
#endif

CStringBuffer CBaseException::GetMessagePostfix(errno_t err)
{
	CStringBuffer tmp;

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	char buf[128];
    
    *buf = 0;
    if ( (err > 0) && (err < sys_nerr) )
        strncpy(buf, sys_errlist[err], 127);
	tmp.FormatString(__FILE__LINE__ _T(" (CRuntime Error: %d - %hs)"), err, buf);
#endif
#ifdef OK_SYS_UNIX
	char *buf = strerror(err);
    
	tmp.FormatString(__FILE__LINE__ _T(" (CRuntime Error: %d - %s)"), err, buf);
#endif
#endif
#ifdef OK_COMP_MSC
	TCHAR buf[128];
    
	_tcserror_s(buf, err);
    
	tmp.FormatString(__FILE__LINE__ _T(" (CRuntime Error: %d - %s)"), err, buf);
#endif
	return tmp;
}
