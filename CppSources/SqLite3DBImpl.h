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
#include "DataVector.h"

#include "SqLite3.h"
#include "SqLite3DB.h"

class CSqLite3ConnectionImpl;
class CPPSOURCES_LOCAL CSqLite3EnvironmentImpl : public CCppObject
{
public:
	CSqLite3EnvironmentImpl(void);
	virtual ~CSqLite3EnvironmentImpl(void);

	__inline CSqLite3Exception* get_LastError() { return _lastError; }
	void set_LastError(CSqLite3Exception* pLastError);

	CSqLite3ConnectionImpl* create_Connection();

	void Open();
	void Close();

	void HandleError(DECL_FILE_LINE int err, CConstPointer funcName);

protected:
	CSqLite3Exception* _lastError;
};


class CSqLite3StatementImpl;
class CPPSOURCES_LOCAL CSqLite3ConnectionImpl : public CCppObject
{
public:
	CSqLite3ConnectionImpl(Ptr(CSqLite3EnvironmentImpl) lpEnv = NULL);
	virtual ~CSqLite3ConnectionImpl(void);

	__inline Ptr(CSqLite3EnvironmentImpl) get_Environment() { return _lpEnv; }
	__inline sqlite3* get_Handle() { return _lpDBHandle; }
	__inline ConstRef(CSqLite3Connection::create_function_infoclass_list) get_FuncList() const { return _funcList; }

	Ptr(CSqLite3StatementImpl) create_Statement();
	void free_Statement(Ptr(CSqLite3StatementImpl) pStmt);

	void Open(CConstPointer _pszConnStr = NULL);
	void Close();

	void create_function(Ptr(CSqLite3Connection::create_function_infoclass) pInfoClass);

	void HandleError(DECL_FILE_LINE int err, CConstPointer funcName);

protected:
	Ptr(CSqLite3EnvironmentImpl) _lpEnv;
	sqlite3* _lpDBHandle;
	bool _bConnected;
	word _numStmts;
	word _cntStmts;
	CSqLite3Connection::create_function_infoclass_list _funcList;
};

class CPPSOURCES_LOCAL CSqLite3StatementImpl : public CCppObject
{
public:
	CSqLite3StatementImpl(CSqLite3EnvironmentImpl* lpEnv = NULL, CSqLite3ConnectionImpl* lpConn = NULL);
	virtual ~CSqLite3StatementImpl(void);

	__inline CSqLite3EnvironmentImpl* get_Environment() { return _lpEnv; }
	__inline sqlite3_stmt* get_Handle() { return _lpStmt; }

	__inline sword get_NumResultColumns() { return _siNumResultColumns; }
	__inline sqword get_NumResultRows() { return _siNumResultRows; }

	__inline ConstRef(CStringBuffer) get_Command() const { return _command; }

	void Prepare(CConstPointer pCommand);
	void BindParameter(word no, sqword value);
	void BindParameter(word no, double value);
	void BindParameter(word no, CConstPointer value, int valueSize);
	void Execute(CConstPointer pCommand = NULL);
	void BindColumns(Ref(CSqLite3Statement::CSqLite3Columns) cols);
	bool Fetch(Ref(CSqLite3Statement::CSqLite3Columns) cols);
	void Close();
	void Free();

	void HandleError(DECL_FILE_LINE int err, CConstPointer funcName);

protected:
	CSqLite3EnvironmentImpl* _lpEnv;
	CSqLite3ConnectionImpl* _lpConn;
	CStringBuffer _command;
	sqlite3_stmt* _lpStmt;
	sword _siNumResultColumns;
	sqword _siNumResultRows;
	word _siNumParams;
};

