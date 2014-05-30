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
#include "WinException.h"
#include "DataVector.h"

DECL_WINEXCEPTION(WINSOURCES_API, CPostgresException, CWinException)

class CPostgresConnection;
class CPostgresEnvironmentImpl;
class WINSOURCES_API CPostgresEnvironment : public CCppObject
{
public:
	CPostgresEnvironment(void);
	virtual ~CPostgresEnvironment(void);

	__inline CPostgresEnvironmentImpl* get_Implementation() { return _lpImpl; }

	CPostgresException* get_LastError() const;
	void set_LastError(CPostgresException* pLastError);

	CPostgresConnection* create_Connection();

	void Open();
	void Close();

protected:
	CPostgresEnvironmentImpl* _lpImpl;
};

class CPostgresStatement;
class CPostgresConnectionImpl;
class WINSOURCES_API CPostgresConnection : public CCppObject
{
public:
	CPostgresConnection(CPostgresEnvironment* lpEnv = NULL, CPostgresConnectionImpl* lpImpl = NULL);
	virtual ~CPostgresConnection(void);

	__inline CPostgresEnvironment* get_Environment() { return _lpEnv; }
	__inline CPostgresConnectionImpl* get_Implementation() { return _lpImpl; }

	CPostgresStatement* create_Statement();
	void free_Statement(CPostgresStatement* pStmt);

	void Open(CConstPointer _pszConnStr = NULL);
	void Close();

protected:
	CPostgresEnvironment* _lpEnv;
	CPostgresConnectionImpl* _lpImpl;
};

class WINSOURCES_API CPostgresColumn : public CCppObject
{
public:
	CPostgresColumn(CConstPointer name = NULL, CConstPointer dataType = NULL);
	CPostgresColumn(ConstRef(CStringBuffer) name, ConstRef(CStringBuffer) dataType);
	virtual ~CPostgresColumn();

	__inline ConstRef(CStringBuffer) get_DataType() const { return _dataType; }
	__inline ConstRef(CByteBuffer) get_Data() const { return _data; }
	__inline void set_Data(ConstRef(CByteBuffer) buf) { _data = buf; }
	__inline ConstRef(CStringBuffer) get_Name() const { return _name; }
	__inline bool get_isnull() const { return _isnull; }
	__inline void set_isnull(bool b) { _isnull = b; }

protected:
	CStringBuffer _dataType;
	CByteBuffer _data;
	CStringBuffer _name;
	bool _isnull;
};


class CPostgresStatementImpl;
class WINSOURCES_API CPostgresStatement : public CCppObject
{
public:
	typedef CDataVectorT<CPostgresColumn, CStringByNameLessFunctor<CPostgresColumn>> CPostgresColumns;

	CPostgresStatement(CPostgresEnvironment* lpEnv = NULL, CPostgresStatementImpl* lpStmtImpl = NULL);
	virtual ~CPostgresStatement(void);

	__inline CPostgresEnvironment* get_Environment() { return _lpEnv; }
	__inline CPostgresStatementImpl* get_Implementation() { return _lpImpl; }

	word get_ColumnCount() const;
	CPostgresColumn* get_ColumnInfo(word ix) const;
	CPostgresColumn* get_ColumnInfo(CConstPointer name);

	sqword get_RowCount() const;

	ConstRef(CStringBuffer) get_Command() const;

	void Prepare(CConstPointer pCommand);
	void BindParameter(word no, const char * value);
	void Execute(CConstPointer pCommand = NULL);
	void BindColumns();
	bool Fetch();
	void Close();
	void Free();

protected:
	CPostgresEnvironment* _lpEnv;
	CPostgresStatementImpl* _lpImpl;
	CPostgresColumns _columns;
};

