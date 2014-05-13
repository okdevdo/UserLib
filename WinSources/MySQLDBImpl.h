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

#include "WinSources.h"

#define __LCC__
#include "MySQL.h"
#include "MySQLDB.h"

class CMySQLConnectionImpl;
class WINSOURCES_LOCAL CMySQLEnvironmentImpl : public CCppObject
{
public:
	CMySQLEnvironmentImpl(void);
	virtual ~CMySQLEnvironmentImpl(void);

	__inline CMySQLException* get_LastError() { return _lastError; }
	void set_LastError(CMySQLException* pLastError);

	CMySQLConnectionImpl* create_Connection();

	void Open();
	void Close();

protected:
	CMySQLException* _lastError;
};

class CMySQLStatementImpl;
class WINSOURCES_LOCAL CMySQLConnectionImpl : public CCppObject
{
public:
	CMySQLConnectionImpl(CMySQLEnvironmentImpl* lpEnv = NULL);
	virtual ~CMySQLConnectionImpl(void);

	__inline CMySQLEnvironmentImpl* get_Environment() { return _lpEnv; }
	__inline MYSQL* get_Handle() { return _lpDBHandle; }

	void close_Transact(bool commit);
	void set_AutoCommit(bool val);
	bool is_AutoCommit();
	bool is_Connected();

	CMySQLStatementImpl* create_Statement();
	void free_Statement(CMySQLStatementImpl* pStmt);

	void Open(CConstPointer _pszConnStr = NULL);
	void Close();

	void HandleError(DECL_FILE_LINE CConstPointer funcN);

protected:
	CMySQLEnvironmentImpl* _lpEnv;
	MYSQL* _lpDBHandle;
	bool _bAutoCommit;
	word _numStmts;
	word _cntStmts;
};

class WINSOURCES_LOCAL CMySQLStatementImpl : public CCppObject
{
public:
	CMySQLStatementImpl(CMySQLEnvironmentImpl* lpEnv = NULL, CMySQLConnectionImpl* lpConn = NULL);
	virtual ~CMySQLStatementImpl(void);

	__inline CMySQLEnvironmentImpl* get_Environment() { return _lpEnv; }
	__inline MYSQL_RES* get_Handle() { return _lpResultset; }

	__inline sword get_NumResultColumns() { return _siNumResultColumns; }
	__inline sqword get_NumResultRows() { return _siNumResultRows; }

	__inline ConstRef(CStringBuffer) get_Command() const { return _command; }

	void Prepare(CConstPointer pCommand);
	void BindParameter(word no, SDWConstPointer value);
	void BindParameter(word no, const double* value);
	void BindParameter(word no, BConstPointer value, ConstPtr(TUnixULong) valueSize);
	void Execute(CConstPointer pCommand = NULL);
	bool NextResult();
	void BindColumns(Ref(CMySQLStatement::CMySQLColumns) cols);
	bool Fetch(Ref(CMySQLStatement::CMySQLColumns) cols);
	void Close();
	void Free();

	void HandleError(DECL_FILE_LINE CConstPointer funcN);

protected:
	CMySQLEnvironmentImpl* _lpEnv;
	CMySQLConnectionImpl* _lpConn;
	CStringBuffer _command;
	MYSQL_RES* _lpResultset;
	MYSQL_STMT* _lpStmt;
	MYSQL_BIND* _lpBindInputParam;
	bool _lpBindInputParamBound;
	MYSQL_BIND* _lpBindOutputCol;
	sword _siNumResultColumns;
	sqword _siNumResultRows;
	dword _siNumParams;
};

