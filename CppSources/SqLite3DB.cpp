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
#include "SqLite3DBImpl.h"
#include "SqLite3DB.h"

IMPL_EXCEPTION(CSqLite3Exception, CBaseException)

static void __stdcall CSqLite3ColumnsDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CSqLite3Column) column = CastAnyPtr(CSqLite3Column, CastMutable(Pointer, data));

	column->release();
}

static sword __stdcall CSqLite3ColumnsSearchAndSortFunc(ConstPointer item, ConstPointer data)
{
	Ptr(CSqLite3Column) pColumn = CastAnyPtr(CSqLite3Column, CastMutable(Pointer, item));
	CStringLiteral pName(CastAny(CPointer, CastMutable(Pointer, data)));

	return pName.Compare(pColumn->get_Name());
}

CSqLite3Environment::CSqLite3Environment(void) :
_lpImpl(NULL)
{
	_lpImpl = OK_NEW_OPERATOR CSqLite3EnvironmentImpl();
}

CSqLite3Environment::~CSqLite3Environment(void)
{
	if (_lpImpl)
		_lpImpl->release();
}

CSqLite3Exception* CSqLite3Environment::get_LastError() const
{
	if (_lpImpl)
		return _lpImpl->get_LastError();
	return NULL;
}

void CSqLite3Environment::set_LastError(CSqLite3Exception* pLastError)
{
	if (_lpImpl)
		_lpImpl->set_LastError(pLastError);
}

CSqLite3Connection* CSqLite3Environment::create_Connection()
{
	if (_lpImpl)
		return OK_NEW_OPERATOR CSqLite3Connection(this, _lpImpl->create_Connection());
	return NULL;
}

void CSqLite3Environment::Open()
{
	if (_lpImpl)
		_lpImpl->Open();
}

void CSqLite3Environment::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

CSqLite3Connection::CSqLite3Connection(CSqLite3Environment* lpEnv, CSqLite3ConnectionImpl* lpImpl) :
_lpEnv(lpEnv), _lpImpl(lpImpl)
{
	if (_lpEnv)
		_lpEnv->addRef();
}

CSqLite3Connection::~CSqLite3Connection(void)
{
	if (_lpEnv)
		_lpEnv->release();
	if (_lpImpl)
		_lpImpl->release();
}

CSqLite3Statement* CSqLite3Connection::create_Statement()
{
	if (_lpImpl)
		return OK_NEW_OPERATOR CSqLite3Statement(_lpEnv, _lpImpl->create_Statement());
	return NULL;
}

void CSqLite3Connection::free_Statement(CSqLite3Statement* pStmt)
{
	if (_lpImpl)
		_lpImpl->free_Statement(pStmt->get_Implementation());
	if (pStmt)
		pStmt->release();
}

void CSqLite3Connection::Open(CConstPointer _pszConnStr)
{
	if (_lpImpl)
		_lpImpl->Open(_pszConnStr);
}

void CSqLite3Connection::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

void CSqLite3Connection::create_function(CConstPointer name, create_function_type funcbody, sword nArgs, ...)
{
	if (_lpImpl)
	{
		Ptr(create_function_infoclass) _pFuncInfo = OK_NEW_OPERATOR create_function_infoclass(this, name, funcbody, nArgs);

		va_list argList;

		va_start(argList, nArgs);
		CSqLite3Column::TDataType vDataType = Cast(CSqLite3Column::TDataType, va_arg(argList, int));
		Ptr(CSqLite3Column) pColumn = OK_NEW_OPERATOR CSqLite3Column(name, vDataType);

		_pFuncInfo->Append(pColumn);
		for (sword i = 0; i < nArgs; ++i)
		{
			CSqLite3Column::TDataType vDataType = Cast(CSqLite3Column::TDataType, va_arg(argList, int));
			CConstPointer vName = va_arg(argList, CConstPointer);
			Ptr(CSqLite3Column) pColumn = OK_NEW_OPERATOR CSqLite3Column(vName, vDataType);

			_pFuncInfo->Append(pColumn);
		}
		va_end(argList);

		_lpImpl->create_function(_pFuncInfo);
	}
}

CSqLite3Connection::create_function_infoclass::create_function_infoclass(Ptr(CSqLite3Connection) pConn, CConstPointer name, create_function_type funcbody, sword nArgs) :
_pConn(pConn), _name(__FILE__LINE__ name), _func(funcbody), _nArgs(nArgs), _args(__FILE__LINE__ 16, 16, CSqLite3ColumnsDeleteFunc)
{}

CSqLite3Connection::create_function_infoclass::create_function_infoclass(Ptr(CSqLite3Connection) pConn, ConstRef(CStringBuffer) name, create_function_type funcbody, sword nArgs) :
_pConn(pConn), _name(name), _func(funcbody), _nArgs(nArgs), _args(__FILE__LINE__ 16, 16, CSqLite3ColumnsDeleteFunc)
{}

CSqLite3Connection::create_function_infoclass::~create_function_infoclass() {}

void CSqLite3Connection::create_function_infoclass::Append(Ptr(CSqLite3Column) pArg)
{
	_args.Append(pArg);
}

CSqLite3Statement::CSqLite3Statement(CSqLite3Environment* lpEnv, CSqLite3StatementImpl* lpStmtImpl) :
_lpEnv(lpEnv), _lpImpl(lpStmtImpl), _columns(__FILE__LINE__ 16, 16, CSqLite3ColumnsDeleteFunc)
{
	if (_lpEnv)
		_lpEnv->addRef();
}

CSqLite3Statement::~CSqLite3Statement(void)
{
	if (_lpEnv)
		_lpEnv->release();
	if (_lpImpl)
		_lpImpl->release();
}

word CSqLite3Statement::get_ColumnCount() const
{
	if (_lpImpl)
		return _lpImpl->get_NumResultColumns();
	return 0;
}

CSqLite3Column* CSqLite3Statement::get_ColumnInfo(word ix) const
{
	CSqLite3Columns::Iterator it = _columns.Index(ix);

	if (it)
		return *it;
	return NULL;
}

CSqLite3Column* CSqLite3Statement::get_ColumnInfo(CConstPointer name) const
{
	CSqLite3Columns::Iterator it = _columns.Find(CastAnyPtr(CSqLite3Column, CastMutable(CPointer, name)), CSqLite3ColumnsSearchAndSortFunc);

	if (it)
		return *it;
	return NULL;
}

sqword CSqLite3Statement::get_RowCount() const
{
	if (_lpImpl)
		return _lpImpl->get_NumResultRows();
	return 0;
}

ConstRef(CStringBuffer) CSqLite3Statement::get_Command() const
{
	if (_lpImpl)
		return _lpImpl->get_Command();
	throw OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("illegal call of CSqLite3Statement::get_Command()"));
}

void CSqLite3Statement::Prepare(CConstPointer pCommand)
{
	if (_lpImpl)
		_lpImpl->Prepare(pCommand);
}

void CSqLite3Statement::BindParameter(word no, sqword value)
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value);
}

void CSqLite3Statement::BindParameter(word no, double value)
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value);
}

void CSqLite3Statement::BindParameter(word no, CConstPointer value, int valueSize)
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value, valueSize);
}

void CSqLite3Statement::Execute(CConstPointer pCommand)
{
	if (_lpImpl)
		_lpImpl->Execute(pCommand);
}

void CSqLite3Statement::BindColumns()
{
	while (_columns.Count())
		_columns.Remove(_columns.Begin());
	if (_lpImpl)
		_lpImpl->BindColumns(_columns);
}

void CSqLite3Statement::BindColumns(int nArg, ...)
{
	while (_columns.Count())
		_columns.Remove(_columns.Begin());
	if (_lpImpl)
	{
		va_list argList;

		va_start(argList, nArg);
		for (sword i = 0; i < nArg; ++i)
		{
			CSqLite3Column::TDataType vDataType = Cast(CSqLite3Column::TDataType, va_arg(argList, int));
			Ptr(CSqLite3Column) pColumn = OK_NEW_OPERATOR CSqLite3Column(NULL, vDataType);

			_columns.Append(pColumn);
		}
		va_end(argList);
		_lpImpl->BindColumns(_columns);
	}
}

bool CSqLite3Statement::Fetch()
{
	if (_lpImpl)
		return _lpImpl->Fetch(_columns);
	return FALSE;
}

void CSqLite3Statement::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

void CSqLite3Statement::Free()
{
	if (_lpImpl)
		_lpImpl->Free();
}

CSqLite3Column::CSqLite3Column(CConstPointer name, TDataType dataType) :
_dataType(dataType),
_data(),
_name(__FILE__LINE__ name)
{
}

CSqLite3Column::CSqLite3Column(ConstRef(CStringBuffer) name, TDataType dataType) :
_dataType(dataType),
_data(),
_name(name)
{
}

CSqLite3Column::~CSqLite3Column()
{
}

CStringBuffer CSqLite3Column::get_DataTypeAsStr() const
{
	CStringBuffer vResult;

	switch (_dataType)
	{
	case CSqLite3Column::eSQL_NULL:
		vResult.SetString(__FILE__LINE__ _T("NULL"));
		break;
	case CSqLite3Column::eSQL_BLOB:
		vResult.SetString(__FILE__LINE__ _T("BLOB"));
		break;
	case CSqLite3Column::eSQL_Float:
		vResult.SetString(__FILE__LINE__ _T("FLOAT"));
		break;
	case CSqLite3Column::eSQL_Integer:
		vResult.SetString(__FILE__LINE__ _T("INTEGER"));
		break;
	case CSqLite3Column::eSQL_Text:
		vResult.SetString(__FILE__LINE__ _T("TEXT"));
		break;
	}
	return vResult;
}