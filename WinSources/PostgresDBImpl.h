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

#include "libpq-fe.h"
#include "PostgresDB.h"

class CPostgresConnectionImpl;
class WINSOURCES_LOCAL CPostgresEnvironmentImpl : public CCppObject
{
public:
	CPostgresEnvironmentImpl(void);
	virtual ~CPostgresEnvironmentImpl(void);

	__inline CPostgresException* get_LastError() { return _lastError; }
	void set_LastError(CPostgresException* pLastError);

	CPostgresConnectionImpl* create_Connection();

	void Open();
	void Close();

protected:
	CPostgresException* _lastError;
};

class CPostgresStatementImpl;
class WINSOURCES_LOCAL CPostgresConnectionImpl : public CCppObject
{
public:
	CPostgresConnectionImpl(CPostgresEnvironmentImpl* lpEnv = NULL);
	virtual ~CPostgresConnectionImpl(void);

	__inline CPostgresEnvironmentImpl* get_Environment() { return _lpEnv; }
	__inline PGconn* get_Handle() { return _lpDBHandle; }

	CPostgresStatementImpl* create_Statement();
	void free_Statement(CPostgresStatementImpl* pStmt);

	void Open(CConstPointer _pszConnStr = NULL);
	void Close();

	void HandleError(DECL_FILE_LINE CConstPointer funcN);

protected:
	CPostgresEnvironmentImpl* _lpEnv;
	PGconn* _lpDBHandle;
	bool _bConnected;
	word _numStmts;
	word _cntStmts;
};

class WINSOURCES_LOCAL CPostgresStatementImpl : public CCppObject
{
public:
	CPostgresStatementImpl(CPostgresEnvironmentImpl* lpEnv = NULL, CPostgresConnectionImpl* lpConn = NULL);
	virtual ~CPostgresStatementImpl(void);

	__inline CPostgresEnvironmentImpl* get_Environment() { return _lpEnv; }
	__inline PGresult* get_Handle() { return _lpResultset; }

	__inline sword get_NumResultColumns() { return _siNumResultColumns; }
	__inline sqword get_NumResultRows() { return _siNumResultRows; }

	__inline ConstRef(CStringBuffer) get_Command() const { return _command; }

	void Prepare(CConstPointer pCommand);
	void BindParameter(word no, const char * value);
	void Execute(CConstPointer pCommand = NULL);
	void BindColumns(Ref(CPostgresStatement::CPostgresColumns) cols);
	bool Fetch(Ref(CPostgresStatement::CPostgresColumns) cols);
	void Close();
	void Free();

	void HandleError(DECL_FILE_LINE CConstPointer funcN);

protected:
	CPostgresEnvironmentImpl* _lpEnv;
	CPostgresConnectionImpl* _lpConn;
	CStringBuffer _command;
	const char** _boundParams;
	PGresult* _lpResultset;
	sword _siNumResultColumns;
	int _siNumResultRows;
	int _siNumCurrentRow;
	word _siNumParams;
};

