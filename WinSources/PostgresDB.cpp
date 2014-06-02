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
#include "PostgresDB.h"
#include "PostgresDBImpl.h"

IMPL_WINEXCEPTION(CPostgresException, CWinException)

CPostgresEnvironment::CPostgresEnvironment(void) :
_lpImpl(NULL)
{
	_lpImpl = OK_NEW_OPERATOR CPostgresEnvironmentImpl();
}

CPostgresEnvironment::~CPostgresEnvironment(void)
{
	if (_lpImpl)
		_lpImpl->release();
}

CPostgresException* CPostgresEnvironment::get_LastError() const
{
	if (_lpImpl)
		return _lpImpl->get_LastError();
	return NULL;
}

void CPostgresEnvironment::set_LastError(CPostgresException* pLastError)
{
	if (_lpImpl)
		_lpImpl->set_LastError(pLastError);
}

CPostgresConnection* CPostgresEnvironment::create_Connection()
{
	if (_lpImpl)
		return OK_NEW_OPERATOR CPostgresConnection(this, _lpImpl->create_Connection());
	return NULL;
}

void CPostgresEnvironment::Open()
{
	if (_lpImpl)
		_lpImpl->Open();
}

void CPostgresEnvironment::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

CPostgresConnection::CPostgresConnection(CPostgresEnvironment* lpEnv, CPostgresConnectionImpl* lpImpl) :
_lpEnv(lpEnv), _lpImpl(lpImpl)
{
	if (_lpEnv)
		_lpEnv->addRef();
}

CPostgresConnection::~CPostgresConnection(void)
{
	if (_lpEnv)
		_lpEnv->release();
	if (_lpImpl)
		_lpImpl->release();
}

CPostgresStatement* CPostgresConnection::create_Statement()
{
	if (_lpImpl)
		return OK_NEW_OPERATOR CPostgresStatement(_lpEnv, _lpImpl->create_Statement());
	return NULL;
}

void CPostgresConnection::free_Statement(CPostgresStatement* pStmt)
{
	if (_lpImpl)
		_lpImpl->free_Statement(pStmt->get_Implementation());
	if (pStmt)
		pStmt->release();
}

void CPostgresConnection::Open(CConstPointer _pszConnStr)
{
	if (_lpImpl)
		_lpImpl->Open(_pszConnStr);
}

void CPostgresConnection::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

static void __stdcall CPostgresColumnsDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CPostgresColumn) column = CastAnyPtr(CPostgresColumn, CastMutable(Pointer, data));

	column->release();
}

static sword __stdcall CPostgresColumnsSearchAndSortFunc(ConstPointer item, ConstPointer data)
{
	Ptr(CPostgresColumn) pColumn = CastAnyPtr(CPostgresColumn, CastMutable(Pointer, item));
	CStringLiteral pName(CastAny(CPointer, CastMutable(Pointer, data)));

	return pName.Compare(pColumn->get_Name());
}

CPostgresStatement::CPostgresStatement(CPostgresEnvironment* lpEnv, CPostgresStatementImpl* lpStmtImpl) :
_lpEnv(lpEnv), _lpImpl(lpStmtImpl), _columns(__FILE__LINE__ 16, 16)
{
	if (_lpEnv)
		_lpEnv->addRef();
}

CPostgresStatement::~CPostgresStatement(void)
{
	if (_lpEnv)
		_lpEnv->release();
	if (_lpImpl)
		_lpImpl->release();
}

word CPostgresStatement::get_ColumnCount() const
{
	if (_lpImpl)
		return _lpImpl->get_NumResultColumns();
	return 0;
}

CPostgresColumn* CPostgresStatement::get_ColumnInfo(word ix) const
{
	CPostgresColumns::Iterator it = _columns.Index(ix);

	if (it)
		return *it;
	return NULL;
}

CPostgresColumn* CPostgresStatement::get_ColumnInfo(CConstPointer name)
{
	CPostgresColumn cmp(name);
	CPostgresColumns::Iterator it = _columns.Find<CStringByNameEqualFunctor<CPostgresColumn>>(&cmp);

	if (it)
		return *it;
	return NULL;
}

sqword CPostgresStatement::get_RowCount() const
{
	if (_lpImpl)
		return _lpImpl->get_NumResultRows();
	return 0;
}

ConstRef(CStringBuffer) CPostgresStatement::get_Command() const
{
	if (_lpImpl)
		return _lpImpl->get_Command();
	throw OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("illegal call of CPostgresStatement::get_Command()"));
}

void CPostgresStatement::Prepare(CConstPointer pCommand)
{
	if (_lpImpl)
		_lpImpl->Prepare(pCommand);
}

void CPostgresStatement::BindParameter(word no, const char * value)
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value);
}

void CPostgresStatement::Execute(CConstPointer pCommand)
{
	if (_lpImpl)
		_lpImpl->Execute(pCommand);
}

void CPostgresStatement::BindColumns()
{
	while (_columns.Count())
		_columns.Remove(_columns.Begin());
	if (_lpImpl)
		_lpImpl->BindColumns(_columns);
}

bool CPostgresStatement::Fetch()
{
	if (_lpImpl)
		return _lpImpl->Fetch(_columns);
	return FALSE;
}

void CPostgresStatement::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

void CPostgresStatement::Free()
{
	if (_lpImpl)
		_lpImpl->Free();
}

CPostgresColumn::CPostgresColumn(CConstPointer name, CConstPointer dataType) :
_dataType(__FILE__LINE__ dataType),
_data(),
_name(__FILE__LINE__ name)
{
}

CPostgresColumn::CPostgresColumn(ConstRef(CStringBuffer) name, ConstRef(CStringBuffer) dataType) :
_dataType(dataType),
_data(),
_name(name)
{
}

CPostgresColumn::~CPostgresColumn()
{
}
