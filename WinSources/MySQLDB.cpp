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
#include "WS_PCH.H"
#include "MySQLDB.h"
#include "MySQLDBImpl.h"

IMPL_WINEXCEPTION(CMySQLException, CWinException)

CMySQLEnvironment::CMySQLEnvironment(void) :
_lpImpl(NULL)
{
	_lpImpl = OK_NEW_OPERATOR CMySQLEnvironmentImpl();
}

CMySQLEnvironment::~CMySQLEnvironment(void)
{
	if (_lpImpl)
		_lpImpl->release();
}

CMySQLException* CMySQLEnvironment::get_LastError() const
{
	if (_lpImpl)
		return _lpImpl->get_LastError();
	return NULL;
}

void CMySQLEnvironment::set_LastError(CMySQLException* pLastError)
{
	if (_lpImpl)
		_lpImpl->set_LastError(pLastError);
}

CMySQLConnection* CMySQLEnvironment::create_Connection()
{
	if (_lpImpl)
		return OK_NEW_OPERATOR CMySQLConnection(this, _lpImpl->create_Connection());
	return NULL;
}

void CMySQLEnvironment::Open()
{
	if (_lpImpl)
		_lpImpl->Open();
}

void CMySQLEnvironment::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

CMySQLConnection::CMySQLConnection(CMySQLEnvironment* lpEnv, CMySQLConnectionImpl* lpImpl) :
_lpEnv(lpEnv), _lpImpl(lpImpl)
{
	if (_lpEnv)
		_lpEnv->addRef();
}

CMySQLConnection::~CMySQLConnection(void)
{
	if (_lpEnv)
		_lpEnv->release();
	if (_lpImpl)
		_lpImpl->release();
}

void CMySQLConnection::close_Transact(bool commit)
{
	if (_lpImpl)
		_lpImpl->close_Transact(commit);
}

void CMySQLConnection::set_AutoCommit(bool val)
{
	if (_lpImpl)
		_lpImpl->set_AutoCommit(val);
}

bool CMySQLConnection::is_AutoCommit()
{
	if (_lpImpl)
		return _lpImpl->is_AutoCommit();
	return false;
}

bool CMySQLConnection::is_Connected()
{
	if (_lpImpl)
		return _lpImpl->is_Connected();
	return false;
}

CMySQLStatement* CMySQLConnection::create_Statement()
{
	if (_lpImpl)
		return OK_NEW_OPERATOR CMySQLStatement(_lpEnv, _lpImpl->create_Statement());
	return NULL;
}

void CMySQLConnection::free_Statement(CMySQLStatement* pStmt)
{
	if (_lpImpl)
		_lpImpl->free_Statement(pStmt->get_Implementation());
	if (pStmt)
		pStmt->release();
}

void CMySQLConnection::Open(CConstPointer _pszConnStr)
{
	if (_lpImpl)
		_lpImpl->Open(_pszConnStr);
}

void CMySQLConnection::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

static void __stdcall CMySQLColumnsDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CMySQLColumn) column = CastAnyPtr(CMySQLColumn, CastMutable(Pointer, data));

	column->release();
}

static sword __stdcall CMySQLColumnsSearchAndSortFunc(ConstPointer item, ConstPointer data)
{
	Ptr(CMySQLColumn) pColumn = CastAnyPtr(CMySQLColumn, CastMutable(Pointer, item));
	CStringLiteral pName(CastAny(CPointer, CastMutable(Pointer, data)));

	return pName.Compare(pColumn->get_Name());
}

CMySQLStatement::CMySQLStatement(CMySQLEnvironment* lpEnv, CMySQLStatementImpl* lpStmtImpl) :
_lpEnv(lpEnv), _lpImpl(lpStmtImpl), _columns(__FILE__LINE__ 16, 16)
{
	if (_lpEnv)
		_lpEnv->addRef();
}

CMySQLStatement::~CMySQLStatement(void)
{
	if (_lpEnv)
		_lpEnv->release();
	if (_lpImpl)
		_lpImpl->release();
}

word CMySQLStatement::get_ColumnCount() const
{
	if (_lpImpl)
		return _lpImpl->get_NumResultColumns();
	return 0;
}

CMySQLColumn* CMySQLStatement::get_ColumnInfo(word ix) const
{
	CMySQLColumns::Iterator it = _columns.Index(ix);

	if (it)
		return *it;
	return NULL;
}

CMySQLColumn* CMySQLStatement::get_ColumnInfo(CConstPointer name)
{
	CMySQLColumn cmp(name);
	CMySQLColumns::Iterator it = _columns.Find<CStringByNameLessFunctor<CMySQLColumn>>(&cmp);

	if (it)
		return *it;
	return NULL;
}

sqword CMySQLStatement::get_RowCount() const
{
	if (_lpImpl)
		return _lpImpl->get_NumResultRows();
	return 0;
}

ConstRef(CStringBuffer) CMySQLStatement::get_Command() const
{
	if (_lpImpl)
		return _lpImpl->get_Command();
	throw OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("illegal call of CMySQLStatement::get_Command()"));
}

void CMySQLStatement::Prepare(CConstPointer pCommand)
{
	if (_lpImpl)
		_lpImpl->Prepare(pCommand);
}

void CMySQLStatement::BindParameter(word no, SDWConstPointer value)
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value);
}

void CMySQLStatement::BindParameter(word no, const double* value)
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value);
}

void CMySQLStatement::BindParameter(word no, BConstPointer value, ConstPtr(TUnixULong) valueSize)
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value, valueSize);
}

void CMySQLStatement::Execute(CConstPointer pCommand)
{
	if (_lpImpl)
		_lpImpl->Execute(pCommand);
}

bool CMySQLStatement::NextResult()
{
	if (_lpImpl)
		return _lpImpl->NextResult();
	return FALSE;
}

void CMySQLStatement::BindColumns()
{
	while (_columns.Count())
		_columns.Remove(_columns.Begin());
	if (_lpImpl)
		_lpImpl->BindColumns(_columns);
}

bool CMySQLStatement::Fetch()
{
	if (_lpImpl)
		return _lpImpl->Fetch(_columns);
	return FALSE;
}

void CMySQLStatement::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

void CMySQLStatement::Free()
{
	if (_lpImpl)
		_lpImpl->Free();
}

static CConstPointer CMySQLColumnTypeStr[] = {
	_T("MYSQL_TYPE_NULL"),
	_T("MYSQL_TYPE_DECIMAL"),
	_T("MYSQL_TYPE_TINY"),
	_T("MYSQL_TYPE_SHORT"),
	_T("MYSQL_TYPE_LONG"),
	_T("MYSQL_TYPE_FLOAT"),
	_T("MYSQL_TYPE_DOUBLE"),
	_T("MYSQL_TYPE_TIMESTAMP"),
	_T("MYSQL_TYPE_LONGLONG"),
	_T("MYSQL_TYPE_INT24"),
	_T("MYSQL_TYPE_DATE"),
	_T("MYSQL_TYPE_TIME"),
	_T("MYSQL_TYPE_DATETIME"),
	_T("MYSQL_TYPE_YEAR"),
	_T("MYSQL_TYPE_NEWDATE"),
	_T("MYSQL_TYPE_VARCHAR"),
	_T("MYSQL_TYPE_BIT"),
	_T("MYSQL_TYPE_TIMESTAMP2"),
	_T("MYSQL_TYPE_DATETIME2"),
	_T("MYSQL_TYPE_TIME2"),
	_T("MYSQL_TYPE_NEWDECIMAL"),
	_T("MYSQL_TYPE_ENUM"),
	_T("MYSQL_TYPE_SET"),
	_T("MYSQL_TYPE_TINY_BLOB"),
	_T("MYSQL_TYPE_MEDIUM_BLOB"),
	_T("MYSQL_TYPE_LONG_BLOB"),
	_T("MYSQL_TYPE_BLOB"),
	_T("MYSQL_TYPE_VAR_STRING"),
	_T("MYSQL_TYPE_STRING"),
	_T("MYSQL_TYPE_GEOMETRY")
};

CMySQLColumn::CMySQLColumn(CConstPointer name, TDataType dataType, dword colLen) :
_dataType(dataType),
_data(__FILE__LINE__ colLen),
_name(__FILE__LINE__ name)
{
}

CMySQLColumn::CMySQLColumn(ConstRef(CStringBuffer) name, TDataType dataType, dword colLen) :
_dataType(dataType),
_data(__FILE__LINE__ colLen),
_name(name)
{
}

CMySQLColumn::~CMySQLColumn()
{
}

CStringBuffer CMySQLColumn::get_DataTypeAsStr() const
{
	return CStringBuffer(__FILE__LINE__ CMySQLColumnTypeStr[_dataType]);
}