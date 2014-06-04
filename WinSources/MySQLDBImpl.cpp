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
#include "MySQLDBImpl.h"

CMySQLStatementImpl::CMySQLStatementImpl(CMySQLEnvironmentImpl* lpEnv, CMySQLConnectionImpl* lpConn)
{
	_lpEnv = lpEnv;
	_lpConn = lpConn;
	_lpResultset = nullptr;
	_lpStmt = nullptr;
	_lpBindInputParam = nullptr;
	_lpBindInputParamBound = FALSE;
	_lpBindOutputCol = nullptr;
	_siNumResultColumns = 0;
	_siNumResultRows = 0;
	_siNumParams = 0;
}

CMySQLStatementImpl::~CMySQLStatementImpl(void)
{
	Free();
}

void CMySQLStatementImpl::Prepare(CConstPointer pCommand)
{
	if ((!_lpEnv) || (!_lpConn) || (!pCommand))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::Prepare] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	CStringBuffer sBuf(__FILE__LINE__ pCommand);
	CByteBuffer bBuf;

	Free();
	_siNumResultColumns = 0;
	_siNumResultRows = 0;
	_siNumParams = 0;

	sBuf.convertToByteBuffer(bBuf);
	_lpStmt = mysql_stmt_init(_lpConn->get_Handle());
	if (!_lpStmt)
		_lpConn->HandleError(__FILE__LINE__ _T("mysql_stmt_init"));

	if (mysql_stmt_prepare(_lpStmt, CastAnyPtr(char, bBuf.get_Buffer()), bBuf.get_BufferSize()))
		HandleError(__FILE__LINE__ _T("mysql_stmt_prepare"));

	/* Get the parameter count from the statement */
	_siNumParams = mysql_stmt_param_count(_lpStmt);
	if (_siNumParams > 0)
		_lpBindInputParam = CastAnyPtr(MYSQL_BIND, TFalloc(_siNumParams * sizeof(MYSQL_BIND)));
	return;
}

void CMySQLStatementImpl::BindParameter(word no, SDWConstPointer value)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpBindInputParam) || (no == 0) || (no > _siNumParams))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	MYSQL_BIND* p = _lpBindInputParam + (no - 1);

	p->buffer = CastAnyPtr(void, CastMutable(SDWPointer, value));
	p->buffer_type = MYSQL_TYPE_LONG;
}

void CMySQLStatementImpl::BindParameter(word no, const double* value)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpBindInputParam) || (no == 0) || (no > _siNumParams))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	MYSQL_BIND* p = _lpBindInputParam + (no - 1);

	p->buffer = CastAnyPtr(void, CastMutablePtr(double, value));
	p->buffer_type = MYSQL_TYPE_DOUBLE;
}

void CMySQLStatementImpl::BindParameter(word no, BConstPointer value, ConstPtr(TUnixULong) valueSize)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpBindInputParam) || (no == 0) || (no > _siNumParams))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	MYSQL_BIND* p = _lpBindInputParam + (no - 1);

	p->buffer = CastAnyPtr(void, CastMutable(BPointer, value));
	p->buffer_type = MYSQL_TYPE_VARCHAR;
	p->length = CastMutablePtr(TUnixULong, valueSize);
}

void CMySQLStatementImpl::Execute(CConstPointer pCommand)
{
	if ((!_lpEnv) || (!_lpConn))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::Execute] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	if (NotPtrCheck(pCommand))
	{
		CStringBuffer sBuf(__FILE__LINE__ pCommand);
		CByteBuffer bBuf;

		Free();
		_siNumResultColumns = 0;
		_siNumResultRows = 0;

		sBuf.convertToByteBuffer(bBuf);
		if (mysql_real_query(_lpConn->get_Handle(), CastAnyPtr(char, bBuf.get_Buffer()), bBuf.get_BufferSize()))
			_lpConn->HandleError(__FILE__LINE__ _T("mysql_real_query"));

		_lpResultset = mysql_store_result(_lpConn->get_Handle());
		if (_lpResultset)  // there are rows
		{
			_siNumResultColumns = mysql_num_fields(_lpResultset);
			_siNumResultRows = mysql_num_rows(_lpResultset);
			return;
		}

		if (mysql_field_count(_lpConn->get_Handle()) == 0)
			_siNumResultRows = mysql_affected_rows(_lpConn->get_Handle());
		else
			_lpConn->HandleError(__FILE__LINE__ _T("mysql_store_result"));
	}
	else
	{
		_siNumResultColumns = 0;
		_siNumResultRows = 0;

		if (!_lpStmt)
		{
			_lpEnv->set_LastError(OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::Execute] programming sequence error")));
			throw _lpEnv->get_LastError();
		}
		if (_lpBindInputParam && (!_lpBindInputParamBound))
		{
			if (mysql_stmt_bind_param(_lpStmt, _lpBindInputParam))
				HandleError(__FILE__LINE__ _T("mysql_stmt_bind_param"));
			_lpBindInputParamBound = TRUE;
		}
		if (mysql_stmt_execute(_lpStmt))
				HandleError(__FILE__LINE__ _T("mysql_stmt_execute"));
		_siNumResultColumns = mysql_stmt_field_count(_lpStmt);
		if (_siNumResultColumns == 0)
			_siNumResultRows = mysql_stmt_affected_rows(_lpStmt);
		else
			_siNumResultRows = mysql_stmt_num_rows(_lpStmt);
	}
}

bool CMySQLStatementImpl::NextResult()
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpStmt))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::NextResult] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	int status = mysql_stmt_next_result(_lpStmt);

	if (status < 0)
		return FALSE;
	if (status > 0)
		HandleError(__FILE__LINE__ _T("mysql_stmt_next_result"));

	_siNumResultColumns = mysql_stmt_field_count(_lpStmt);
	if (_siNumResultColumns == 0)
		_siNumResultRows = mysql_stmt_affected_rows(_lpStmt);
	else
		_siNumResultRows = mysql_stmt_num_rows(_lpStmt);

	if (_lpBindOutputCol)
	{
		TFfree(_lpBindOutputCol);
		_lpBindOutputCol = nullptr;
	}

	return (_siNumResultColumns > 0);
}

void CMySQLStatementImpl::BindColumns(Ref(CMySQLStatement::CMySQLColumns) cols)
{
	if ((!_lpEnv) || (!_lpConn) || (_siNumResultColumns <= 0))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::BindColumns] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	sword iCol;
	MYSQL_RES* pResultset = nullptr;

	if (_lpStmt)
	{
		pResultset = mysql_stmt_result_metadata(_lpStmt);
		if (!pResultset)
			HandleError(__FILE__LINE__ _T("mysql_stmt_result_metadata"));
		assert(mysql_num_fields(pResultset) == Castdword(_siNumResultColumns));
	}
	else if (_lpResultset)
		pResultset = _lpResultset;
	else
	{
		_lpEnv->set_LastError(OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::BindColumns] programming sequence error")));
		throw _lpEnv->get_LastError();
	}

	for (iCol = 0; iCol < _siNumResultColumns; ++iCol)
	{
		MYSQL_FIELD *field = mysql_fetch_field_direct(pResultset, iCol);

		if (!field)
		{
			Ptr(CMySQLColumn) pColumn = OK_NEW_OPERATOR CMySQLColumn;

			cols.Append(pColumn);
			continue;
		}

		enum_field_types vDataType = field->type;
		CByteBuffer bBuf(__FILE__LINE__ CastAny(BPointer, field->name), field->name_length);
		CStringBuffer vTmp;

		vTmp.convertFromByteBuffer(bBuf);

		CMySQLColumn::TDataType vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_NULL;

		switch (vDataType)
		{
		case MYSQL_TYPE_DECIMAL: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_DECIMAL; break;
		case MYSQL_TYPE_TINY: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_TINY; break;
		case MYSQL_TYPE_SHORT: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_SHORT; break;
		case MYSQL_TYPE_LONG: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_LONG; break;
		case MYSQL_TYPE_FLOAT: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_FLOAT; break;
		case MYSQL_TYPE_DOUBLE: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_DOUBLE; break;
		case MYSQL_TYPE_NULL: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_NULL; break;
		case MYSQL_TYPE_TIMESTAMP: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_TIMESTAMP; break;
		case MYSQL_TYPE_LONGLONG: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_LONGLONG; break;
		case MYSQL_TYPE_INT24: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_INT24; break;
		case MYSQL_TYPE_DATE: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_DATE; break;
		case MYSQL_TYPE_TIME: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_TIME; break;
		case MYSQL_TYPE_DATETIME: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_DATETIME; break;
		case MYSQL_TYPE_YEAR: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_YEAR; break;
		case MYSQL_TYPE_NEWDATE: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_NEWDATE; break;
		case MYSQL_TYPE_VARCHAR: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_VARCHAR; break;
		case MYSQL_TYPE_BIT: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_BIT; break;
		case MYSQL_TYPE_TIMESTAMP2: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_TIMESTAMP2; break;
		case MYSQL_TYPE_DATETIME2: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_DATETIME2; break;
		case MYSQL_TYPE_TIME2: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_TIME2; break;
		case MYSQL_TYPE_NEWDECIMAL: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_NEWDECIMAL; break;
		case MYSQL_TYPE_ENUM: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_ENUM; break;
		case MYSQL_TYPE_SET: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_SET; break;
		case MYSQL_TYPE_TINY_BLOB: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_TINY_BLOB; break;
		case MYSQL_TYPE_MEDIUM_BLOB: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_MEDIUM_BLOB; break;
		case MYSQL_TYPE_LONG_BLOB: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_LONG_BLOB; break;
		case MYSQL_TYPE_BLOB: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_BLOB; break;
		case MYSQL_TYPE_VAR_STRING: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_VAR_STRING; break;
		case MYSQL_TYPE_STRING: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_STRING; break;
		case MYSQL_TYPE_GEOMETRY: vDataTypeEnum = CMySQLColumn::eMYSQL_TYPE_GEOMETRY; break;
		default:
			break;
		}

		Ptr(CMySQLColumn) pColumn = OK_NEW_OPERATOR CMySQLColumn(vTmp, vDataTypeEnum, field->length);

		cols.Append(pColumn);
	}
	if (_lpStmt)
	{
		mysql_free_result(pResultset);

		_lpBindOutputCol = CastAnyPtr(MYSQL_BIND, TFalloc(_siNumResultColumns * sizeof(MYSQL_BIND)));

		CMySQLStatement::CMySQLColumns::Iterator it = cols.Begin();
		dword ix = 0;

		while (it)
		{
			Ptr(CMySQLColumn) pColumn = *it;
			Ptr(MYSQL_BIND) pBind = _lpBindOutputCol + ix;
			enum_field_types vDataType = MYSQL_TYPE_NULL;

			pBind->buffer = pColumn->get_Data().get_Buffer();
			pBind->buffer_length = pColumn->get_Data().get_BufferSize();

			switch (pColumn->get_DataType())
			{
			case CMySQLColumn::eMYSQL_TYPE_DECIMAL: vDataType = MYSQL_TYPE_DECIMAL; break;
			case CMySQLColumn::eMYSQL_TYPE_TINY: vDataType = MYSQL_TYPE_TINY; break;
			case CMySQLColumn::eMYSQL_TYPE_SHORT: vDataType = MYSQL_TYPE_SHORT; break;
			case CMySQLColumn::eMYSQL_TYPE_LONG: vDataType = MYSQL_TYPE_LONG; break;
			case CMySQLColumn::eMYSQL_TYPE_FLOAT: vDataType = MYSQL_TYPE_FLOAT; break;
			case CMySQLColumn::eMYSQL_TYPE_DOUBLE: vDataType = MYSQL_TYPE_DOUBLE; break;
			case CMySQLColumn::eMYSQL_TYPE_NULL: vDataType = MYSQL_TYPE_NULL; break;
			case CMySQLColumn::eMYSQL_TYPE_TIMESTAMP: vDataType = MYSQL_TYPE_TIMESTAMP; break;
			case CMySQLColumn::eMYSQL_TYPE_LONGLONG: vDataType = MYSQL_TYPE_LONGLONG; break;
			case CMySQLColumn::eMYSQL_TYPE_INT24: vDataType = MYSQL_TYPE_INT24; break;
			case CMySQLColumn::eMYSQL_TYPE_DATE: vDataType = MYSQL_TYPE_DATE; break;
			case CMySQLColumn::eMYSQL_TYPE_TIME: vDataType = MYSQL_TYPE_TIME; break;
			case CMySQLColumn::eMYSQL_TYPE_DATETIME: vDataType = MYSQL_TYPE_DATETIME; break;
			case CMySQLColumn::eMYSQL_TYPE_YEAR: vDataType = MYSQL_TYPE_YEAR; break;
			case CMySQLColumn::eMYSQL_TYPE_NEWDATE: vDataType = MYSQL_TYPE_NEWDATE; break;
			case CMySQLColumn::eMYSQL_TYPE_VARCHAR: vDataType = MYSQL_TYPE_VARCHAR; break;
			case CMySQLColumn::eMYSQL_TYPE_BIT: vDataType = MYSQL_TYPE_BIT; break;
			case CMySQLColumn::eMYSQL_TYPE_TIMESTAMP2: vDataType = MYSQL_TYPE_TIMESTAMP2; break;
			case CMySQLColumn::eMYSQL_TYPE_DATETIME2: vDataType = MYSQL_TYPE_DATETIME2; break;
			case CMySQLColumn::eMYSQL_TYPE_TIME2: vDataType = MYSQL_TYPE_TIME2; break;
			case CMySQLColumn::eMYSQL_TYPE_NEWDECIMAL: vDataType = MYSQL_TYPE_NEWDECIMAL; break;
			case CMySQLColumn::eMYSQL_TYPE_ENUM: vDataType = MYSQL_TYPE_ENUM; break;
			case CMySQLColumn::eMYSQL_TYPE_SET: vDataType = MYSQL_TYPE_SET; break;
			case CMySQLColumn::eMYSQL_TYPE_TINY_BLOB: vDataType = MYSQL_TYPE_TINY_BLOB; break;
			case CMySQLColumn::eMYSQL_TYPE_MEDIUM_BLOB: vDataType = MYSQL_TYPE_MEDIUM_BLOB; break;
			case CMySQLColumn::eMYSQL_TYPE_LONG_BLOB: vDataType = MYSQL_TYPE_LONG_BLOB; break;
			case CMySQLColumn::eMYSQL_TYPE_BLOB: vDataType = MYSQL_TYPE_BLOB; break;
			case CMySQLColumn::eMYSQL_TYPE_VAR_STRING: vDataType = MYSQL_TYPE_VAR_STRING; break;
			case CMySQLColumn::eMYSQL_TYPE_STRING: vDataType = MYSQL_TYPE_STRING; break;
			case CMySQLColumn::eMYSQL_TYPE_GEOMETRY: vDataType = MYSQL_TYPE_GEOMETRY; break;
			default:
				break;
			}
			pBind->buffer_type = vDataType;

			++ix;
			++it;
		}
		if (mysql_stmt_bind_result(_lpStmt, _lpBindOutputCol))
			HandleError(__FILE__LINE__ _T("mysql_stmt_bind_result"));
	}
}

bool CMySQLStatementImpl::Fetch(Ref(CMySQLStatement::CMySQLColumns) cols)
{
	if ((!_lpEnv) || (!_lpConn) || (_siNumResultColumns <= 0))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::Fetch] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	if (_lpStmt)
	{
		switch (mysql_stmt_fetch(_lpStmt))
		{
		case 0:
			return TRUE;
		case 1:
			HandleError(__FILE__LINE__ _T("mysql_stmt_fetch"));
			break;
		case MYSQL_NO_DATA:
			break;
		case MYSQL_DATA_TRUNCATED:
			return TRUE;
		}
	}
	else if (_lpResultset)
	{
		MYSQL_ROW row;

		row = mysql_fetch_row(_lpResultset);
		if (row)
		{
			unsigned long *lengths = mysql_fetch_lengths(_lpResultset);
			sword iCol;

			for (iCol = 0; iCol < _siNumResultColumns; ++iCol)
			{
				CMySQLStatement::CMySQLColumns::Iterator it = cols.Index(iCol);

				if (it)
				{
					Ptr(CMySQLColumn) pColumn = *it;
					CByteBuffer bBuf(__FILE__LINE__ CastAny(BPointer, row[iCol]), lengths[iCol]);

					pColumn->set_Data(bBuf);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

void CMySQLStatementImpl::Close()
{
	if ((!_lpEnv) || (!_lpConn))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::Close] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	if (_lpStmt)
	{
		if (mysql_stmt_close(_lpStmt))
			_lpConn->HandleError(__FILE__LINE__ _T("mysql_stmt_close"));
		_lpStmt = nullptr;
	}
	if (_lpBindOutputCol)
	{
		TFfree(_lpBindOutputCol);
		_lpBindOutputCol = nullptr;
	}
	if (_lpBindInputParam)
	{
		TFfree(_lpBindInputParam);
		_lpBindInputParam = nullptr;
		_lpBindInputParamBound = FALSE;
	}
	if (_lpResultset)
	{
		mysql_free_result(_lpResultset);
		_lpResultset = nullptr;
	}
}

void CMySQLStatementImpl::Free()
{
	Close();
}

void CMySQLStatementImpl::HandleError(DECL_FILE_LINE CConstPointer funcN)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpStmt))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLStatementImpl::HandleError] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	const char * msg = mysql_stmt_error(_lpStmt);
	CByteBuffer bBuf(__FILE__LINE__ CastAny(BConstPointer, msg), Cast(dword, strlen(msg)));
	CStringBuffer sBuf;

	sBuf.convertFromUTF8(bBuf, false);
	_lpEnv->set_LastError(OK_NEW_OPERATOR CMySQLException(ARGS_FILE_LINE _T("%s failed (%s)"), funcN, sBuf.GetString()));
	throw _lpEnv->get_LastError();
}

CMySQLConnectionImpl::CMySQLConnectionImpl(CMySQLEnvironmentImpl* lpEnv)
{
	_lpEnv = lpEnv;
	_lpDBHandle = nullptr;
	_bAutoCommit = false;
	_numStmts = 0;
	_cntStmts = 0;
}

CMySQLConnectionImpl::~CMySQLConnectionImpl(void)
{
	Close();
}

void CMySQLConnectionImpl::close_Transact(bool commit)
{
	if ((!_lpEnv) || (!_lpDBHandle))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::Open] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	if (commit)
	{
		if (mysql_commit(_lpDBHandle))
			HandleError(__FILE__LINE__ _T("mysql_commit"));
	}
	else
	{
		if (mysql_rollback(_lpDBHandle))
			HandleError(__FILE__LINE__ _T("mysql_rollback"));
	}
}

void CMySQLConnectionImpl::set_AutoCommit(bool val)
{
	if ((!_lpEnv) || (!_lpDBHandle))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::Open] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	if (mysql_autocommit(_lpDBHandle, val))
		HandleError(__FILE__LINE__ _T("mysql_autocommit"));
	_bAutoCommit = val;
}

bool CMySQLConnectionImpl::is_AutoCommit()
{
	if ((!_lpEnv) || (!_lpDBHandle))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::Open] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	return _bAutoCommit;
}

bool CMySQLConnectionImpl::is_Connected()
{
	if ((!_lpEnv) || (!_lpDBHandle))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::Open] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	if (mysql_ping(_lpDBHandle))
		return false;
	return true;
}

void CMySQLConnectionImpl::Open(CConstPointer _pszConnStr)
{
	if (!_lpEnv)
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::Open] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	CStringBuffer sBuf(__FILE__LINE__ _pszConnStr);
	CByteBuffer vHost;
	CByteBuffer vUser;
	CByteBuffer vPasswd;
	CByteBuffer vDb;

	_lpDBHandle = mysql_init(nullptr);
	if (!_lpDBHandle)
	{
		_lpEnv->set_LastError(OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("mysql_init failed")));
		throw _lpEnv->get_LastError();
	}

	CPointer sArray[64];
	dword sSize = 0;

	sBuf.Split(_T(";"), sArray, 64, &sSize);

	for (dword ix = 0; ix < sSize; ++ix)
	{
		CStringBuffer sTmp(__FILE__LINE__ sArray[ix]);
		CPointer sTmpArray[3];
		dword sTmpSize = 0;

		sTmp.Split(_T("="), sTmpArray, 3, &sTmpSize);
		if (sTmpSize != 2)
		{
			_lpEnv->set_LastError(OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::Open] Argument error")));
			throw _lpEnv->get_LastError();
		}
		if (s_stricmp(sTmpArray[0], _T("host")) == 0)
		{
			CStringBuffer sHost(__FILE__LINE__ sTmpArray[1]);

			sHost.convertToByteBuffer(vHost);
			vHost.concat_Buffer(CastAny(BConstPointer, "\0"), 1);
		}
		else if (s_stricmp(sTmpArray[0], _T("user")) == 0)
		{
			CStringBuffer sUser(__FILE__LINE__ sTmpArray[1]);

			sUser.convertToByteBuffer(vUser);
			vUser.concat_Buffer(CastAny(BConstPointer, "\0"), 1);
		}
		else if (s_stricmp(sTmpArray[0], _T("passwd")) == 0)
		{
			CStringBuffer sPasswd(__FILE__LINE__ sTmpArray[1]);

			sPasswd.convertToByteBuffer(vPasswd);
			vPasswd.concat_Buffer(CastAny(BConstPointer, "\0"), 1);
		}
		else if (s_stricmp(sTmpArray[0], _T("db")) == 0)
		{
			CStringBuffer sDb(__FILE__LINE__ sTmpArray[1]);

			sDb.convertToByteBuffer(vDb);
			vDb.concat_Buffer(CastAny(BConstPointer, "\0"), 1);
		}
	}

	if (!mysql_real_connect(_lpDBHandle, CastAnyPtr(char, vHost.get_Buffer()), CastAnyPtr(char, vUser.get_Buffer()), CastAnyPtr(char, vPasswd.get_Buffer()), CastAnyPtr(char, vDb.get_Buffer()), 0, nullptr, 0))
		HandleError(__FILE__LINE__ _T("mysql_real_connect"));
}

CMySQLStatementImpl* CMySQLConnectionImpl::create_Statement()
{
	if ((!_lpEnv) || (!_lpDBHandle) || (!is_Connected()) || ((_numStmts > 0) && (_cntStmts >= _numStmts)))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::create_Statement] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	++_cntStmts;
	return OK_NEW_OPERATOR CMySQLStatementImpl(_lpEnv, this);
}

void CMySQLConnectionImpl::free_Statement(CMySQLStatementImpl* pStmt)
{
	if ((!_lpEnv) || (!_lpDBHandle))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::free_Statement] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	if (pStmt)
	{
		pStmt->release();
		--_cntStmts;
	}
}

void CMySQLConnectionImpl::Close()
{
	if (!_lpEnv)
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::Close] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	if (_lpDBHandle)
	{
		mysql_close(_lpDBHandle);
		_lpDBHandle = nullptr;
	}
}

void CMySQLConnectionImpl::HandleError(DECL_FILE_LINE CConstPointer funcN)
{
	if ((!_lpEnv) || (!_lpDBHandle))
	{
		Ptr(CMySQLException) pEx = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("[CMySQLConnectionImpl::HandleError] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	const char * msg = mysql_error(_lpDBHandle);
	CByteBuffer bBuf(__FILE__LINE__ CastAny(BConstPointer, msg), Cast(dword, strlen(msg)));
	CStringBuffer sBuf;

	sBuf.convertFromUTF8(bBuf, false);
	_lpEnv->set_LastError(OK_NEW_OPERATOR CMySQLException(ARGS_FILE_LINE _T("%s failed (%s)"), funcN, sBuf.GetString()));
	throw _lpEnv->get_LastError();
}

CMySQLEnvironmentImpl::CMySQLEnvironmentImpl(void)
{
	_lastError = nullptr;
}


CMySQLEnvironmentImpl::~CMySQLEnvironmentImpl(void)
{
	Close();
	if (_lastError)
		delete _lastError;
	_lastError = nullptr;
}

void CMySQLEnvironmentImpl::set_LastError(CMySQLException* pLastError)
{
	if (_lastError)
		delete _lastError;
	_lastError = pLastError;
}

CMySQLConnectionImpl* CMySQLEnvironmentImpl::create_Connection()
{
	return OK_NEW_OPERATOR CMySQLConnectionImpl(this);
}

void CMySQLEnvironmentImpl::Open()
{
	if (mysql_library_init(0, nullptr, nullptr)) 
	{
		_lastError = OK_NEW_OPERATOR CMySQLException(__FILE__LINE__ _T("mysql_library_init failed"));
		throw _lastError;
	}
}

void CMySQLEnvironmentImpl::Close()
{
	mysql_library_end();
}

