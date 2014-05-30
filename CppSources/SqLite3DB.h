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
/**
 *  \file SqLite3DB.h
 *  \brief Interface classes to access SqLite3 database.
 */
#pragma once

#include "CppSources.h"
#include "BaseException.h"
#include "DataDoubleLinkedList.h"
#include "DataVector.h"

DECL_EXCEPTION(CPPSOURCES_API, CSqLite3Exception, CBaseException)

class CSqLite3Connection;
class CSqLite3EnvironmentImpl;
class CPPSOURCES_API CSqLite3Environment : public CCppObject
{
public:
	CSqLite3Environment(void);
	virtual ~CSqLite3Environment(void);

	__inline CSqLite3EnvironmentImpl* get_Implementation() { return _lpImpl; }

	CSqLite3Exception* get_LastError() const;
	void set_LastError(CSqLite3Exception* pLastError);

	CSqLite3Connection* create_Connection();

	void Open();
	void Close();

protected:
	CSqLite3EnvironmentImpl* _lpImpl;
};

class CPPSOURCES_API CSqLite3Column : public CCppObject
{
public:
	enum TDataType
	{
		eSQL_NULL,
		eSQL_BLOB,
		eSQL_Float,
		eSQL_Integer,
		eSQL_Text
	};

	CSqLite3Column(CConstPointer name = NULL, TDataType dataType = eSQL_NULL);
	CSqLite3Column(ConstRef(CStringBuffer) name, TDataType dataType = eSQL_NULL);
	virtual ~CSqLite3Column();

	__inline TDataType get_DataType() const { return _dataType; }
	CStringBuffer get_DataTypeAsStr() const;
	__inline ConstRef(CByteBuffer) get_Data() const { return _data; }
	__inline void set_Data(ConstRef(CByteBuffer) buf) { _data = buf; }
	__inline ConstRef(CStringBuffer) get_Name() const { return _name; }
	__inline void set_Name(ConstRef(CStringBuffer) s) { _name = s; }

protected:
	TDataType _dataType;
	CByteBuffer _data;
	CStringBuffer _name;
};

class CSqLite3Statement;
class CSqLite3ConnectionImpl;
class CPPSOURCES_API CSqLite3Connection : public CCppObject
{
public:
	typedef CDataVectorT<CSqLite3Column, CStringByNameLessFunctor<CSqLite3Column> > CSqLite3Columns;
	typedef void(*create_function_type)(Ptr(CSqLite3Connection), ConstRef(CSqLite3Columns));

	class create_function_infoclass : public CCppObject
	{
	public:
		create_function_infoclass(Ptr(CSqLite3Connection) pConn = NULL, CConstPointer name = NULL, create_function_type funcbody = NULL, sword nArgs = 0);
		create_function_infoclass(Ptr(CSqLite3Connection) pConn, ConstRef(CStringBuffer) name, create_function_type funcbody, sword nArgs);
		virtual ~create_function_infoclass();

		Ptr(CSqLite3Connection) get_DBConnection() const { return _pConn; }
		ConstRef(CStringBuffer) get_Name() const { return _name; }
		create_function_type get_Func() const{ return _func; }
		sword get_NArgs() const { return _nArgs; }
		ConstRef(CSqLite3Columns) get_Args() const { return _args; }

		void Append(Ptr(CSqLite3Column) pArg);

	protected:
		Ptr(CSqLite3Connection) _pConn;
		CStringBuffer _name;
		create_function_type _func;
		sword _nArgs;
		CSqLite3Columns _args;
	};

	typedef CDataDoubleLinkedListT<create_function_infoclass> create_function_infoclass_list;

	CSqLite3Connection(CSqLite3Environment* lpEnv = NULL, CSqLite3ConnectionImpl* lpImpl = NULL);
	virtual ~CSqLite3Connection(void);

	__inline CSqLite3Environment* get_Environment() { return _lpEnv; }
	__inline CSqLite3ConnectionImpl* get_Implementation() { return _lpImpl; }

	CSqLite3Statement* create_Statement();
	void free_Statement(CSqLite3Statement* pStmt);

	void Open(CConstPointer _pszConnStr = NULL);
	void Close();

	void create_function(CConstPointer name, create_function_type funcbody, sword nArgs, ...);

protected:
	CSqLite3Environment* _lpEnv;
	CSqLite3ConnectionImpl* _lpImpl;
};

class CSqLite3StatementImpl;
class CPPSOURCES_API CSqLite3Statement : public CCppObject
{
public:
	typedef CDataVectorT<CSqLite3Column, CStringByNameLessFunctor<CSqLite3Column> > CSqLite3Columns;

	CSqLite3Statement(CSqLite3Environment* lpEnv = NULL, CSqLite3StatementImpl* lpStmtImpl = NULL);
	virtual ~CSqLite3Statement(void);

	__inline CSqLite3Environment* get_Environment() { return _lpEnv; }
	__inline CSqLite3StatementImpl* get_Implementation() { return _lpImpl; }

	word get_ColumnCount() const;
	CSqLite3Column* get_ColumnInfo(word ix) const;
	CSqLite3Column* get_ColumnInfo(CConstPointer name);

	sqword get_RowCount() const;

	ConstRef(CStringBuffer) get_Command() const;

	void Prepare(CConstPointer pCommand);
	void BindParameter(word no, sqword value);
	void BindParameter(word no, double value);
	void BindParameter(word no, CConstPointer value, int valueSize);
	void Execute(CConstPointer pCommand = NULL);
	void BindColumns();
	void BindColumns(int nArg, ...);
	bool Fetch();
	void Close();
	void Free();

protected:
	CSqLite3Environment* _lpEnv;
	CSqLite3StatementImpl* _lpImpl;
	CSqLite3Columns _columns;
};

