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

#ifdef __CYGWIN__
#define _IODBCUNIX_H
#define WIN32
#endif
#include <sqlucode.h>

#include "WinSources.h"
#include "ODBC.h"

class CODBCConnectionImpl;
class WINSOURCES_LOCAL CODBCEnvironmentImpl : public CCppObject
{
public:
	CODBCEnvironmentImpl(void);
	virtual ~CODBCEnvironmentImpl(void);

	__inline SQLHENV get_Handle() { return _lpEnv; }

	__inline CODBCException* get_LastError() { return _lastError; }
	void set_LastError(CODBCException* pLastError);

	CODBCConnectionImpl* create_Connection();

	void Open();
	void Close();

	void HandleError(SQLSMALLINT hType, RETCODE	RetCode);

protected:
	SQLHENV _lpEnv;
	CODBCException* _lastError;
};

class CODBCStatementImpl;
class WINSOURCES_LOCAL CODBCConnectionImpl : public CCppObject
{
public:
	CODBCConnectionImpl(CODBCEnvironmentImpl* lpEnv = NULL, SQLHDBC lpDbc = NULL);
	virtual ~CODBCConnectionImpl(void);

	__inline CODBCEnvironmentImpl* get_Environment() { return _lpEnv; }
	__inline SQLHDBC get_Handle() { return _lpDbc; }

	bool can_Transact();
	void close_Transact(bool commit);
	void set_AutoCommit(bool val);
	bool is_AutoCommit();
	bool is_Connected();

	CODBCStatementImpl* create_Statement();
	void free_Statement(CODBCStatementImpl* pStmt);

	void Open(LPCTSTR _pszConnStr = NULL);
	void Close();

	void HandleError(SQLSMALLINT hType, RETCODE	RetCode);

protected:
	CODBCEnvironmentImpl* _lpEnv;
	SQLHDBC _lpDbc;
	SQLUSMALLINT _numStmts;
	SQLUSMALLINT _cntStmts;
};

class WINSOURCES_LOCAL CODBCStatementImpl : public CCppObject
{
public:
	CODBCStatementImpl(CODBCEnvironmentImpl* lpEnv = NULL, SQLHSTMT lpStmt = NULL);
	virtual ~CODBCStatementImpl(void);

	__inline CODBCEnvironmentImpl* get_Environment() { return _lpEnv; }
	__inline SQLHSTMT get_Handle() { return _lpStmt; }

	__inline SQLSMALLINT get_NumResultColumns() { return _siNumResultColumns; }
	__inline SQLLEN get_NumResultRows() { return _siNumResultRows; }
	__inline SQLSMALLINT get_NumParameters() { return _siNumParams; }

	__inline ConstRef(CStringBuffer) get_Command() const { return _command; }

	void Prepare(LPCTSTR pCommand, dword arraysize);
#ifdef OK_CPU_64BIT
	void BindParameter(word no, SDWConstPointer value, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, const double* value, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, CConstPointer value, qword maxvaluesize, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, CConstPointer name, SDWConstPointer value, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, CConstPointer name, const double* value, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, CConstPointer name, CConstPointer value, qword maxvaluesize, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
#endif
#ifdef OK_CPU_32BIT
	void BindParameter(word no, SDWConstPointer value, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, const double* value, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, CConstPointer value, dword maxvaluesize, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, CConstPointer name, SDWConstPointer value, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, CConstPointer name, const double* value, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
	void BindParameter(word no, CConstPointer name, CConstPointer value, dword maxvaluesize, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir);
#endif
	void Execute(LPCTSTR pCommand = NULL);
	void BindColumns(Ref(CODBCStatement::CODBCColumns) cols);
	BOOL Fetch();
	void Reset();
	void Close();
	void Free();

	void HandleError(SQLSMALLINT hType, RETCODE	RetCode);

protected:
	Ptr(CODBCEnvironmentImpl) _lpEnv;
	CStringBuffer _command;
	SQLHSTMT _lpStmt;
	SQLSMALLINT	_siNumResultColumns;
	SQLLEN _siNumResultRows;
	SQLSMALLINT _siNumParams;
	Ptr(SQLUSMALLINT) _pParamArrayResults;
	SQLULEN _siParamArrayCnt;
};

