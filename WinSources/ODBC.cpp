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
#include "ODBC.h"
#include "ODBCImpl.h"

CODBCException::CODBCException(CConstPointer pText, int lench) :
_text(), _severity(E_SQL_NORMAL)
{
	SetString(pText, lench);
}

CODBCException::CODBCException(ConstRef(CStringBuffer) pText) :
_text(pText), _severity(E_SQL_NORMAL)
{
}

CODBCException::~CODBCException(void)
{
}

bool CODBCException::FormatString(CConstPointer format, ...)
{
	CStringBuffer tmp;
	va_list argList;

	va_start(argList, format);

	if (!(tmp.FormatString(__FILE__LINE__ format, argList)))
	{
		va_end(argList);
		return false;
	}

	va_end(argList);

	AppendString(tmp);
	return true;
}

CODBCEnvironment::CODBCEnvironment(void):
	_lpImpl(NULL)
{
	_lpImpl = OK_NEW_OPERATOR CODBCEnvironmentImpl();
}

CODBCEnvironment::~CODBCEnvironment(void)
{
	if (_lpImpl)
		_lpImpl->release();
}

CODBCException* CODBCEnvironment::get_LastError() const
{
	if (_lpImpl)
		return _lpImpl->get_LastError();
	return NULL;
}

void CODBCEnvironment::set_LastError(CODBCException* pLastError)
{
	if (_lpImpl)
		_lpImpl->set_LastError(pLastError);
}

CODBCConnection* CODBCEnvironment::create_Connection()
{
	if (_lpImpl)
		return OK_NEW_OPERATOR CODBCConnection(this, _lpImpl->create_Connection());
	return NULL;
}

void CODBCEnvironment::Open()
{
	if (_lpImpl)
		_lpImpl->Open();
}

void CODBCEnvironment::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

CODBCConnection::CODBCConnection(CODBCEnvironment* lpEnv, CODBCConnectionImpl* lpImpl):
	_lpEnv(lpEnv), _lpImpl(lpImpl)
{
	if (_lpEnv)
		_lpEnv->addRef();
}

CODBCConnection::~CODBCConnection(void)
{
	if (_lpEnv)
		_lpEnv->release();
	if (_lpImpl)
		_lpImpl->release();
}

bool CODBCConnection::can_Transact()
{
	if (_lpImpl)
		return _lpImpl->can_Transact();
	return false;
}

void CODBCConnection::close_Transact(bool commit)
{
	if (_lpImpl)
		_lpImpl->close_Transact(commit);
}

void CODBCConnection::set_AutoCommit(bool val)
{
	if (_lpImpl)
		_lpImpl->set_AutoCommit(val);
}

bool CODBCConnection::is_AutoCommit()
{
	if (_lpImpl)
		return _lpImpl->is_AutoCommit();
	return false;
}

bool CODBCConnection::is_Connected()
{
	if (_lpImpl)
		return _lpImpl->is_Connected();
	return false;
}

CODBCStatement* CODBCConnection::create_Statement()
{
	if (_lpImpl)
		return OK_NEW_OPERATOR CODBCStatement(_lpEnv, _lpImpl->create_Statement());
	return NULL;
}

void CODBCConnection::free_Statement(Ref(Ptr(CODBCStatement)) pStmt)
{
	if (_lpImpl)
	{
		_lpImpl->free_Statement(pStmt->get_Implementation());
		pStmt->reset_Implementation();
	}
	if (pStmt)
	{
		pStmt->release();
		pStmt = NULL;
	}
}

void CODBCConnection::Open(CConstPointer _pszConnStr)
{
	if (_lpImpl)
		_lpImpl->Open(_pszConnStr);
}

void CODBCConnection::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

static void __stdcall CODBCColumnsDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CODBCColumn) column = CastAnyPtr(CODBCColumn, CastMutable(Pointer, data));

	column->release();
}

static sword __stdcall CODBCColumnsSearchAndSortFunc(ConstPointer item, ConstPointer data)
{
	Ptr(CODBCColumn) pColumn = CastAnyPtr(CODBCColumn, CastMutable(Pointer, item));
	CStringLiteral pName(CastAny(CPointer, CastMutable(Pointer, data)));

	return pName.Compare(pColumn->get_Name());
}

#ifdef OK_CPU_64BIT
sqword CODBCStatement::NullParameter = SQL_NULL_DATA;
sqword CODBCStatement::NullTerminatedParameterString = SQL_NTS;
#endif
#ifdef OK_CPU_32BIT
sdword CODBCStatement::NullParameter = SQL_NULL_DATA;
sdword CODBCStatement::NullTerminatedParameterString = SQL_NTS;
#endif

CODBCStatement::CODBCStatement(CODBCEnvironment* lpEnv, CODBCStatementImpl* lpStmtImpl) :
_lpEnv(lpEnv), _lpImpl(lpStmtImpl), _columns(__FILE__LINE__ 16, 16, CODBCColumnsDeleteFunc)
{
	if (_lpEnv)
		_lpEnv->addRef();
}

CODBCStatement::~CODBCStatement(void)
{
	if (_lpEnv)
		_lpEnv->release();
	if (_lpImpl)
		_lpImpl->release();
}

word CODBCStatement::get_ColumnCount() const
{
	if (_lpImpl)
		return _lpImpl->get_NumResultColumns();
	return 0;
}

CODBCColumn* CODBCStatement::get_ColumnInfo(word ix) const
{
	CODBCColumns::Iterator it = _columns.Index(ix);

	if (it)
		return *it;
	return NULL;
}

CODBCColumn* CODBCStatement::get_ColumnInfo(LPCTSTR name) const
{
	CODBCColumns::Iterator it = _columns.Find(CastAnyPtr(CODBCColumn, CastMutable(CPointer, name)), CODBCColumnsSearchAndSortFunc);

	if (it)
		return *it;
	return NULL;
}

sqword CODBCStatement::get_RowCount() const
{
	if (_lpImpl)
		return _lpImpl->get_NumResultRows();
	return 0;
}

ConstRef(CStringBuffer) CODBCStatement::get_Command() const
{
	if (_lpImpl)
		return _lpImpl->get_Command();
	throw OK_NEW_OPERATOR CODBCException(_T("illegal call of CODBCStatement::get_Command()"));
}

void CODBCStatement::Prepare(CConstPointer pCommand, dword arraysize)
{
	if (_lpImpl)
		_lpImpl->Prepare(pCommand, arraysize);
}

#ifdef OK_CPU_64BIT
void CODBCStatement::BindParameter(word no, SDWConstPointer value, SQWConstPointer valueSize, TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatement::BindParameter(word no, SDWConstPointer value, SDWConstPointer valueSize, TParamDirection paramdir)
#endif
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value, valueSize, paramdir);
}

#ifdef OK_CPU_64BIT
void CODBCStatement::BindParameter(word no, const double* value, SQWConstPointer valueSize, TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatement::BindParameter(word no, const double* value, SDWConstPointer valueSize, TParamDirection paramdir)
#endif
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value, valueSize, paramdir);
}

#ifdef OK_CPU_64BIT
void CODBCStatement::BindParameter(word no, CConstPointer value, qword maxvaluesize, SQWConstPointer valueSize, TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatement::BindParameter(word no, CConstPointer value, dword maxvaluesize, SDWConstPointer valueSize, TParamDirection paramdir)
#endif
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, value, maxvaluesize, valueSize, paramdir);
}

#ifdef OK_CPU_64BIT
void CODBCStatement::BindParameter(word no, CConstPointer name, SDWConstPointer value, SQWConstPointer valueSize, TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatement::BindParameter(word no, CConstPointer name, SDWConstPointer value, SDWConstPointer valueSize, TParamDirection paramdir)
#endif
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, name, value, valueSize, paramdir);
}

#ifdef OK_CPU_64BIT
void CODBCStatement::BindParameter(word no, CConstPointer name, const double* value, SQWConstPointer valueSize, TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatement::BindParameter(word no, CConstPointer name, const double* value, SDWConstPointer valueSize, TParamDirection paramdir)
#endif
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, name, value, valueSize, paramdir);
}

#ifdef OK_CPU_64BIT
void CODBCStatement::BindParameter(word no, CConstPointer name, CConstPointer value, qword maxvaluesize, SQWConstPointer valueSize, TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatement::BindParameter(word no, CConstPointer name, CConstPointer value, dword maxvaluesize, SDWConstPointer valueSize, TParamDirection paramdir)
#endif
{
	if (_lpImpl)
		_lpImpl->BindParameter(no, name, value, maxvaluesize, valueSize, paramdir);
}

void CODBCStatement::Execute(CConstPointer pCommand)
{
	if (_lpImpl)
		_lpImpl->Execute(pCommand);
}

void CODBCStatement::BindColumns()
{
	while (_columns.Count())
		_columns.Remove(_columns.Begin());
	if (_lpImpl)
		_lpImpl->BindColumns(_columns);
}

bool CODBCStatement::Fetch()
{
	if (_lpImpl)
	{
		BOOL b = _lpImpl->Fetch();

		if (b)
			return true;
		Close();
	}
	return false;
}

void CODBCStatement::FetchSingleValue(SDWPointer value)
{
	*value = 0;
	if (_lpImpl)
	{
		_lpImpl->Execute();
		while (_columns.Count())
			_columns.Remove(_columns.Begin());
		_lpImpl->BindColumns(_columns);
		if (_lpImpl->Fetch())
		{
			CODBCColumns::Iterator it = _columns.Index(0);
			CByteBuffer b((*it)->get_Data());

			*value = DerefDWPointer(b.get_Buffer());
		}
	}
}

void CODBCStatement::FetchSingleValue(float* value)
{
	*value = 0.0;
	if (_lpImpl)
	{
		_lpImpl->Execute();
		while (_columns.Count())
			_columns.Remove(_columns.Begin());
		_lpImpl->BindColumns(_columns);
		if (_lpImpl->Fetch())
		{
			CODBCColumns::Iterator it = _columns.Index(0);
			CByteBuffer b((*it)->get_Data());

			*value = DerefAnyPtr(float, b.get_Buffer());
		}
	}
}

void CODBCStatement::FetchSingleValue(double* value)
{
	*value = 0.0;
	if (_lpImpl)
	{
		_lpImpl->Execute();
		while (_columns.Count())
			_columns.Remove(_columns.Begin());
		_lpImpl->BindColumns(_columns);
		if (_lpImpl->Fetch())
		{
			CODBCColumns::Iterator it = _columns.Index(0);
			CByteBuffer b((*it)->get_Data());

			*value = DerefAnyPtr(double, b.get_Buffer());
		}
	}
}

void CODBCStatement::FetchSingleValue(CPointer value, qword valueSize)
{

}

void CODBCStatement::Reset()
{
	if (_lpImpl)
		_lpImpl->Reset();
}

void CODBCStatement::Close()
{
	if (_lpImpl)
		_lpImpl->Close();
}

void CODBCStatement::Free()
{
	if (_lpImpl)
		_lpImpl->Free();
}

sqword CODBCColumn::NullValue = -1;

static CConstPointer CODBCColumnDataTypeStr[] = {
	_T("SQL_NULL"),
	_T("SQL_CHAR"),
	_T("SQL_VARCHAR"),
	_T("SQL_LONGVARCHAR"),
	_T("SQL_WCHAR"),
	_T("SQL_WVARCHAR"),
	_T("SQL_WLONGVARCHAR"),
	_T("SQL_DECIMAL"),
	_T("SQL_NUMERIC"),
	_T("SQL_SMALLINT"),
	_T("SQL_INTEGER"),
	_T("SQL_REAL"),
	_T("SQL_FLOAT"),
	_T("SQL_DOUBLE"),
	_T("SQL_BIT"),
	_T("SQL_TINYINT"),
	_T("SQL_BIGINT"),
	_T("SQL_BINARY"),
	_T("SQL_VARBINARY"),
	_T("SQL_LONGVARBINARY"),
	_T("SQL_TYPE_DATE"),
	_T("SQL_TYPE_TIME"),
	_T("SQL_TYPE_TIMESTAMP"),
	_T("SQL_INTERVAL_MONTH"),
	_T("SQL_INTERVAL_YEAR"),
	_T("SQL_INTERVAL_YEAR_TO_MONTH"),
	_T("SQL_INTERVAL_DAY"),
	_T("SQL_INTERVAL_HOUR"),
	_T("SQL_INTERVAL_MINUTE"),
	_T("SQL_INTERVAL_SECOND"),
	_T("SQL_INTERVAL_DAY_TO_HOUR"),
	_T("SQL_INTERVAL_DAY_TO_MINUTE"),
	_T("SQL_INTERVAL_DAY_TO_SECOND"),
	_T("SQL_INTERVAL_HOUR_TO_MINUTE"),
	_T("SQL_INTERVAL_HOUR_TO_SECOND"),
	_T("SQL_INTERVAL_MINUTE_TO_SECOND"),
	_T("SQL_GUID")
};

CODBCColumn::CODBCColumn(CConstPointer name, TDataType dataType, word decp, bool isNull, dword dataLen) :
_data(__FILE__LINE__ dataLen),
_dataType(dataType),
_name(__FILE__LINE__ name),
_decimalPoint(decp),
_nullable(isNull),
_ind(0)
{
}

CODBCColumn::CODBCColumn(ConstRef(CStringBuffer) name, TDataType dataType, word decp, bool isNull, dword dataLen) :
_data(__FILE__LINE__ dataLen),
_dataType(dataType),
_name(name),
_decimalPoint(decp),
_nullable(isNull),
_ind(0)
{
}

CODBCColumn::~CODBCColumn()
{
}

CStringBuffer CODBCColumn::get_DataTypeAsStr() const
{
	return CStringBuffer(__FILE__LINE__ CODBCColumnDataTypeStr[_dataType]);
}