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
#pragma once

#include "BaseException.h"
#include "WinSources.h"

class WINSOURCES_API CWinException: public CBaseException
{
public:

	enum TErrorType 
	{
		CRunTimeError,
#ifdef OK_SYS_WINDOWS
		WinExtError,
		WSAExtError,
		BCryptStatus
#endif
#ifdef OK_SYS_UNIX
		GAIExtError
#endif
	};

	CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format);
	CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer sArg);
	CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer sArg1, CConstPointer sArg2);
	CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, int sArg);
	CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, TErrorType errType, long errCode = 0);
	CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer sArg, TErrorType errType, long errCode = 0);
	CWinException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer sArg1, CConstPointer sArg2, TErrorType errType, long errCode = 0);
	~CWinException(void);

protected:
	CWinException(void);

	CStringBuffer GetMessagePostfix(TErrorType errType, long errCode = 0);
};

#define DECL_WINEXCEPTION(mexport, name, base) \
class mexport name: public base \
{ \
public: \
	name(DECL_FILE_LINE ConstRef(CStringLiteral) format); \
	name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer sArg); \
	name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer sArg1, CConstPointer sArg2); \
	name(DECL_FILE_LINE ConstRef(CStringLiteral) format, int sArg); \
	name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, TErrorType errType, long errCode = 0); \
	name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer sArg, TErrorType errType, long errCode = 0); \
	name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer sArg1, CConstPointer sArg2, TErrorType errType, long errCode = 0); \
	~name(void); \
private: \
	name(void); \
};

#define IMPL_WINEXCEPTION(name, base) \
	name::name() {} \
	name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format) : base(ARGS_FILE_LINE format) {} \
	name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer sArg) : base(ARGS_FILE_LINE format, sArg) {} \
	name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer sArg1, CConstPointer sArg2) : base(ARGS_FILE_LINE format, sArg1, sArg2) {} \
	name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, int sArg) : base(ARGS_FILE_LINE format, sArg) {} \
	name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, base::TErrorType errType, long errCode) : \
		base(ARGS_FILE_LINE format, func, errType, errCode) {} \
	name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer sArg, base::TErrorType errType, long errCode): \
		base(ARGS_FILE_LINE format, func, sArg, errType, errCode) {} \
	name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer sArg1, CConstPointer sArg2, base::TErrorType errType, long errCode): \
		base(ARGS_FILE_LINE format, func, sArg1, sArg2, errType, errCode) {} \
	name::~name(void) {}
