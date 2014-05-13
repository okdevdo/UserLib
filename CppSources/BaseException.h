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
#pragma once

#include "CppSources.h"

class CPPSOURCES_API CBaseException: public CCppObject
{
public:
	CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format);
	CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer arg1);
	CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer arg1, CConstPointer arg2);
	CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, int arg1);
	CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, errno_t err);
	CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer path, errno_t err);
	CBaseException(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer path1, CConstPointer path2, errno_t err);
	~CBaseException(void);

	__inline ConstRef(CStringBuffer) GetExceptionMessage() const { return m_Message; }

protected:
	CBaseException(void);
#ifdef __DEBUG__
	CStringBuffer GetMessagePrefix(const char* file, int line);
#endif
	CStringBuffer GetMessagePostfix(errno_t err);

	CStringBuffer m_Message;
};

#define DECL_EXCEPTION(mexport, name, base) \
class mexport name: public base \
{ \
public: \
name(DECL_FILE_LINE ConstRef(CStringLiteral) format); \
name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer arg1); \
name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer arg1, CConstPointer arg2); \
name(DECL_FILE_LINE ConstRef(CStringLiteral) format, int arg1); \
name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, errno_t err); \
name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer path, errno_t err); \
name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer path1, CConstPointer path2, errno_t err); \
~name(void); \
protected: \
name(void); \
};

#define IMPL_EXCEPTION(name, base) \
name::name() {} \
name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format): base(ARGS_FILE_LINE format) {} \
name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer arg1): base(ARGS_FILE_LINE format, arg1) {} \
name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer arg1, CConstPointer arg2): base(ARGS_FILE_LINE format, arg1, arg2) {} \
name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, int arg1): base(ARGS_FILE_LINE format, arg1) {} \
name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, errno_t err): base(ARGS_FILE_LINE format, func, err) {} \
name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer path, errno_t err): base(ARGS_FILE_LINE format, func, path, err) {} \
name::name(DECL_FILE_LINE ConstRef(CStringLiteral) format, CConstPointer func, CConstPointer path1, CConstPointer path2, errno_t err): base(ARGS_FILE_LINE format, func, path1, path2, err) {} \
name::~name(void) {}
