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

DECL_WINEXCEPTION(WINSOURCES_API, CMySQLException, CWinException)

class CMySQLConnection;
class CMySQLEnvironmentImpl;
class WINSOURCES_API CMySQLEnvironment : public CCppObject
{
public:
	CMySQLEnvironment(void);
	virtual ~CMySQLEnvironment(void);

	__inline CMySQLEnvironmentImpl* get_Implementation() { return _lpImpl; }

	CMySQLException* get_LastError() const;
	void set_LastError(CMySQLException* pLastError);

	CMySQLConnection* create_Connection();

	void Open();
	void Close();

protected:
	CMySQLEnvironmentImpl* _lpImpl;
};

class CMySQLStatement;
class CMySQLConnectionImpl;
class WINSOURCES_API CMySQLConnection : public CCppObject
{
public:
	CMySQLConnection(CMySQLEnvironment* lpEnv = NULL, CMySQLConnectionImpl* lpImpl = NULL);
	virtual ~CMySQLConnection(void);

	__inline CMySQLEnvironment* get_Environment() { return _lpEnv; }
	__inline CMySQLConnectionImpl* get_Implementation() { return _lpImpl; }

	void close_Transact(bool commit = true);
	void set_AutoCommit(bool val = true);
	bool is_AutoCommit();
	bool is_Connected();

	CMySQLStatement* create_Statement();
	void free_Statement(CMySQLStatement* pStmt);

	void Open(CConstPointer _pszConnStr = NULL);
	void Close();

protected:
	CMySQLEnvironment* _lpEnv;
	CMySQLConnectionImpl* _lpImpl;
};

class WINSOURCES_API CMySQLColumn : public CCppObject
{
public:
	enum TDataType
	{
		eMYSQL_TYPE_NULL,
		eMYSQL_TYPE_DECIMAL,
		eMYSQL_TYPE_TINY,
		eMYSQL_TYPE_SHORT,
		eMYSQL_TYPE_LONG,
		eMYSQL_TYPE_FLOAT,
		eMYSQL_TYPE_DOUBLE,
		eMYSQL_TYPE_TIMESTAMP,
		eMYSQL_TYPE_LONGLONG,
		eMYSQL_TYPE_INT24,
		eMYSQL_TYPE_DATE,
		eMYSQL_TYPE_TIME,
		eMYSQL_TYPE_DATETIME,
		eMYSQL_TYPE_YEAR,
		eMYSQL_TYPE_NEWDATE,
		eMYSQL_TYPE_VARCHAR,
		eMYSQL_TYPE_BIT,
		eMYSQL_TYPE_TIMESTAMP2,
		eMYSQL_TYPE_DATETIME2,
		eMYSQL_TYPE_TIME2,
		eMYSQL_TYPE_NEWDECIMAL,
		eMYSQL_TYPE_ENUM,
		eMYSQL_TYPE_SET,
		eMYSQL_TYPE_TINY_BLOB,
		eMYSQL_TYPE_MEDIUM_BLOB,
		eMYSQL_TYPE_LONG_BLOB,
		eMYSQL_TYPE_BLOB,
		eMYSQL_TYPE_VAR_STRING,
		eMYSQL_TYPE_STRING,
		eMYSQL_TYPE_GEOMETRY
	};

	CMySQLColumn(CConstPointer name = NULL, TDataType dataType = eMYSQL_TYPE_NULL, dword colLen = 0);
	CMySQLColumn(ConstRef(CStringBuffer) name, TDataType dataType = eMYSQL_TYPE_NULL, dword colLen = 0);
	virtual ~CMySQLColumn();

	__inline TDataType get_DataType() const { return _dataType; }
	CStringBuffer get_DataTypeAsStr() const;
	__inline ConstRef(CByteBuffer) get_Data() const { return _data; }
	__inline void set_Data(ConstRef(CByteBuffer) buf) { _data = buf; }
	__inline ConstRef(CStringBuffer) get_Name() const { return _name; }

protected:
	TDataType _dataType;
	CByteBuffer _data;
	CStringBuffer _name;
};

#include "DataVector.h"

class CMySQLStatementImpl;
class WINSOURCES_API CMySQLStatement : public CCppObject
{
public:
	typedef CDataVectorT<CMySQLColumn, CStringByNameLessFunctor<CMySQLColumn>> CMySQLColumns;

	CMySQLStatement(CMySQLEnvironment* lpEnv = NULL, CMySQLStatementImpl* lpStmtImpl = NULL);
	virtual ~CMySQLStatement(void);

	__inline CMySQLEnvironment* get_Environment() { return _lpEnv; }
	__inline CMySQLStatementImpl* get_Implementation() { return _lpImpl; }

	word get_ColumnCount() const;
	CMySQLColumn* get_ColumnInfo(word ix) const;
	CMySQLColumn* get_ColumnInfo(CConstPointer name);

	sqword get_RowCount() const;

	ConstRef(CStringBuffer) get_Command() const;

	void Prepare(CConstPointer pCommand);
	void BindParameter(word no, SDWConstPointer value);
	void BindParameter(word no, const double* value);
	void BindParameter(word no, BConstPointer value, ConstPtr(TUnixULong) valueSize);
	void Execute(CConstPointer pCommand = NULL);
	bool NextResult();
	void BindColumns();
	bool Fetch();
	void Close();
	void Free();

protected:
	CMySQLEnvironment* _lpEnv;
	CMySQLStatementImpl* _lpImpl;
	CMySQLColumns _columns;
};

