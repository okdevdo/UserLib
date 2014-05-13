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
#include "PostgresDBImpl.h"

CPostgresStatementImpl::CPostgresStatementImpl(CPostgresEnvironmentImpl* lpEnv, CPostgresConnectionImpl* lpConn)
{
	_lpEnv = lpEnv;
	_lpConn = lpConn;
	_lpResultset = NULL;
	_boundParams = NULL;
	_siNumResultColumns = 0;
	_siNumResultRows = 0;
	_siNumCurrentRow = 0;
	_siNumParams = 0;
}

CPostgresStatementImpl::~CPostgresStatementImpl(void)
{
	Free();
}

void CPostgresStatementImpl::Prepare(CConstPointer pCommand)
{
	if ((!_lpEnv) || (!_lpConn))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresStatementImpl::Prepare] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	CStringBuffer sBuf(__FILE__LINE__ pCommand);
	CByteBuffer bBuf;

	sBuf.convertToByteBuffer(bBuf);
	bBuf.concat_Buffer(CastAny(BConstPointer, "\0"), 1);
	_siNumParams = 0;

	Free();

	_lpResultset = PQprepare(_lpConn->get_Handle(), "", CastAnyPtr(char, bBuf.get_Buffer()), 0, NULL);

	switch (PQresultStatus(_lpResultset))
	{
	case PGRES_SINGLE_TUPLE:
	case PGRES_TUPLES_OK:
	case PGRES_COMMAND_OK:
	case PGRES_COPY_OUT:
	case PGRES_COPY_IN:
	case PGRES_COPY_BOTH:
		PQclear(_lpResultset);
		break;
	case PGRES_EMPTY_QUERY:
	case PGRES_BAD_RESPONSE:
	case PGRES_NONFATAL_ERROR:
	case PGRES_FATAL_ERROR:
		HandleError(__FILE__LINE__ _T("PQprepare"));
		break;
	}
	
	_lpResultset = PQdescribePrepared(_lpConn->get_Handle(), "");

	switch (PQresultStatus(_lpResultset))
	{
	case PGRES_SINGLE_TUPLE:
	case PGRES_TUPLES_OK:
	case PGRES_COMMAND_OK:
	case PGRES_COPY_OUT:
	case PGRES_COPY_IN:
	case PGRES_COPY_BOTH:
		_siNumParams = PQnparams(_lpResultset);
		if (_siNumParams > 0)
			_boundParams = CastAny(const char**, TFalloc(_siNumParams * sizeof(char*)));
		PQclear(_lpResultset);
		break;
	case PGRES_EMPTY_QUERY:
	case PGRES_BAD_RESPONSE:
	case PGRES_NONFATAL_ERROR:
	case PGRES_FATAL_ERROR:
		HandleError(__FILE__LINE__ _T("PQdescribePrepared"));
		break;
	}
}

void CPostgresStatementImpl::BindParameter(word no, const char * value)
{
	if ((!_lpEnv) || (!_lpConn) || (!_boundParams) || (no == 0) || (no > _siNumParams))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	_boundParams[no - 1] = value;
}

void CPostgresStatementImpl::Execute(CConstPointer pCommand)
{
	if ((!_lpEnv) || (!_lpConn))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresStatementImpl::Execute] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	if (PtrCheck(pCommand))
	{
		_siNumResultColumns = 0;
		_siNumResultRows = 0;
		_siNumCurrentRow = 0;

		if (!(_lpResultset = PQexecPrepared(_lpConn->get_Handle(), "", _siNumParams, _boundParams, NULL, NULL, 0)))
			_lpConn->HandleError(__FILE__LINE__ _T("PQexecPrepared"));

		switch (PQresultStatus(_lpResultset))
		{
		case PGRES_SINGLE_TUPLE:
		case PGRES_TUPLES_OK:
			_siNumResultColumns = PQnfields(_lpResultset);
			_siNumResultRows = PQntuples(_lpResultset);
			return;
		case PGRES_COMMAND_OK:
		case PGRES_COPY_OUT:
		case PGRES_COPY_IN:
		case PGRES_COPY_BOTH:
			return;
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			HandleError(__FILE__LINE__ _T("PQexecPrepared"));
			break;
		}
	}
	else
	{
		CStringBuffer sBuf(__FILE__LINE__ pCommand);
		CByteBuffer bBuf;

		_siNumResultColumns = 0;
		_siNumResultRows = 0;
		_siNumCurrentRow = 0;

		sBuf.convertToByteBuffer(bBuf);
		bBuf.concat_Buffer(CastAny(BConstPointer, "\0"), 1);
		if (!(_lpResultset = PQexec(_lpConn->get_Handle(), CastAnyPtr(char, bBuf.get_Buffer()))))
			_lpConn->HandleError(__FILE__LINE__ _T("PQexec"));

		switch (PQresultStatus(_lpResultset))
		{
		case PGRES_SINGLE_TUPLE:
		case PGRES_TUPLES_OK:
			_siNumResultColumns = PQnfields(_lpResultset);
			_siNumResultRows = PQntuples(_lpResultset);
			return;
		case PGRES_COMMAND_OK:
		case PGRES_COPY_OUT:
		case PGRES_COPY_IN:
		case PGRES_COPY_BOTH:
			return;
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			HandleError(__FILE__LINE__ _T("PQexec"));
			break;
		}
	}
}

static char *findOid(PGresult* typeTable, Oid value, Ptr(int) pLen)
{
	char vBuf[20];

	sprintf(vBuf, "%d", value);
	for (int ix = 0; ix < PQntuples(typeTable); ++ix)
	{
		CByteBuffer bBuf(__FILE__LINE__ CastAny(BPointer, PQgetvalue(typeTable, ix, 0)), PQgetlength(typeTable, ix, 0));

		if (memcmp(vBuf, bBuf.get_Buffer(), bBuf.get_BufferSize()) == 0)
		{
			if (pLen)
				*pLen = PQgetlength(typeTable, ix, 1);
			return PQgetvalue(typeTable, ix, 1);
		}
	}
	if (pLen)
		*pLen = 0;
	return NULL;
}

void CPostgresStatementImpl::BindColumns(Ref(CPostgresStatement::CPostgresColumns) cols)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpResultset) || (_siNumResultColumns <= 0))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresStatementImpl::BindColumns] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	sword iCol;
	PGresult* typeTable = PQexec(_lpConn->get_Handle(), "select oid, typname from pg_type");

	for (iCol = 0; iCol < _siNumResultColumns; ++iCol)
	{
		char* fname = PQfname(_lpResultset, iCol);
		CByteBuffer bBuf(__FILE__LINE__ CastAny(BPointer, fname), Cast(dword, strlen(fname)));
		CStringBuffer vTmp;

		vTmp.convertFromByteBuffer(bBuf);

		Oid vDataType = PQftype(_lpResultset, iCol);
		int vDataTypeStrLen = 0;
		char* vDataTypeStr = findOid(typeTable, vDataType, &vDataTypeStrLen);
		CByteBuffer bBuf2(__FILE__LINE__ CastAny(BPointer, vDataTypeStr), Cast(dword, vDataTypeStrLen));
		CStringBuffer vTmp2;

		vTmp2.convertFromByteBuffer(bBuf2);

		Ptr(CPostgresColumn) pColumn = OK_NEW_OPERATOR CPostgresColumn(vTmp, vTmp2);

		cols.Append(pColumn);
	}
	PQclear(typeTable);
}

bool CPostgresStatementImpl::Fetch(Ref(CPostgresStatement::CPostgresColumns) cols)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpResultset) || (_siNumResultColumns <= 0))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresStatementImpl::Fetch] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	if (_siNumCurrentRow < _siNumResultRows)
	{
		sword iCol;

		for (iCol = 0; iCol < _siNumResultColumns; ++iCol)
		{
			CPostgresStatement::CPostgresColumns::Iterator it = cols.Index(iCol);

			if (it)
			{
				Ptr(CPostgresColumn) pColumn = *it;
				CByteBuffer bBuf(__FILE__LINE__ CastAny(BPointer, PQgetvalue(_lpResultset, _siNumCurrentRow, iCol)), PQgetlength(_lpResultset, _siNumCurrentRow, iCol));
				int vIsNull = PQgetisnull(_lpResultset, _siNumCurrentRow, iCol);

				pColumn->set_Data(bBuf);
				pColumn->set_isnull(vIsNull?TRUE:FALSE);
			}
		}
		++_siNumCurrentRow;
		return TRUE;
	}
	return FALSE;
}

void CPostgresStatementImpl::Close()
{
	if ((!_lpEnv) || (!_lpConn))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresStatementImpl::Close] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	if (_lpResultset)
	{
		PQclear(_lpResultset);
		_lpResultset = NULL;
	}
	if (_boundParams)
	{
		TFfree(_boundParams);
		_boundParams = NULL;
	}
}

void CPostgresStatementImpl::Free()
{
	Close();
}

void CPostgresStatementImpl::HandleError(DECL_FILE_LINE CConstPointer funcN)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpResultset))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresStatementImpl::HandleError] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	const char * msg = PQresultErrorMessage(_lpResultset);
	CByteBuffer bBuf(__FILE__LINE__ CastAny(BConstPointer, msg), Cast(dword, strlen(msg)));
	CStringBuffer sBuf;

	sBuf.convertFromUTF8(bBuf, false);
	_lpEnv->set_LastError(OK_NEW_OPERATOR CPostgresException(ARGS_FILE_LINE _T("%s failed (%s)"), funcN, sBuf.GetString()));
	throw _lpEnv->get_LastError();
}

CPostgresConnectionImpl::CPostgresConnectionImpl(CPostgresEnvironmentImpl* lpEnv)
{
	_lpEnv = lpEnv;
	_lpDBHandle = NULL;
	_bConnected = FALSE;
	_numStmts = 0;
	_cntStmts = 0;
}

CPostgresConnectionImpl::~CPostgresConnectionImpl(void)
{
	Close();
}

void CPostgresConnectionImpl::Open(CConstPointer _pszConnStr)
{
	if ((!_lpEnv) || _lpDBHandle || _bConnected)
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresConnectionImpl::Open] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	CStringBuffer sBuf(__FILE__LINE__ _pszConnStr);
	CByteBuffer vConnStr;

	sBuf.convertToByteBuffer(vConnStr);
	vConnStr.concat_Buffer(CastAny(BConstPointer, "\0"), 1);
	if (!(_lpDBHandle = PQconnectdb(CastAnyPtr(char, vConnStr.get_Buffer()))))
	{
		_lpEnv->set_LastError(OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("PQconnectdb failed")));
		throw _lpEnv->get_LastError();
	}
	if (PQstatus(_lpDBHandle) != CONNECTION_OK)
		HandleError(__FILE__LINE__ _T("PQconnectdb"));
	_bConnected = TRUE;
}

CPostgresStatementImpl* CPostgresConnectionImpl::create_Statement()
{
	if ((!_lpEnv) || (!_lpDBHandle) || (!_bConnected) || ((_numStmts > 0) && (_cntStmts >= _numStmts)))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresConnectionImpl::create_Statement] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	++_cntStmts;
	return OK_NEW_OPERATOR CPostgresStatementImpl(_lpEnv, this);
}

void CPostgresConnectionImpl::free_Statement(CPostgresStatementImpl* pStmt)
{
	if ((!_lpEnv) || (!_lpDBHandle) || (!_bConnected) || ((_numStmts > 0) && (_cntStmts >= _numStmts)))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresConnectionImpl::free_Statement] Invalid arguments or programming sequence error"));

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

void CPostgresConnectionImpl::Close()
{
	if ((!_lpEnv) || (!_lpDBHandle) || (!_bConnected))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresConnectionImpl::Close] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	PQfinish(_lpDBHandle);
	_lpDBHandle = NULL;
	_bConnected = FALSE;
}

void CPostgresConnectionImpl::HandleError(DECL_FILE_LINE CConstPointer funcN)
{
	if ((!_lpEnv) || (!_lpDBHandle) || (!_bConnected))
	{
		Ptr(CPostgresException) pEx = OK_NEW_OPERATOR CPostgresException(__FILE__LINE__ _T("[CPostgresConnectionImpl::HandleError] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	const char * msg = PQerrorMessage(_lpDBHandle);
	CByteBuffer bBuf(__FILE__LINE__ CastAny(BConstPointer, msg), Cast(dword, strlen(msg)));
	CStringBuffer sBuf;

	sBuf.convertFromUTF8(bBuf, false);
	_lpEnv->set_LastError(OK_NEW_OPERATOR CPostgresException(ARGS_FILE_LINE _T("%s failed (%s)"), funcN, sBuf.GetString()));
}

CPostgresEnvironmentImpl::CPostgresEnvironmentImpl(void)
{
	_lastError = NULL;
}


CPostgresEnvironmentImpl::~CPostgresEnvironmentImpl(void)
{
	Close();
	if (_lastError)
		delete _lastError;
	_lastError = NULL;
}

void CPostgresEnvironmentImpl::set_LastError(CPostgresException* pLastError)
{
	if (_lastError)
		delete _lastError;
	_lastError = pLastError;
}

CPostgresConnectionImpl* CPostgresEnvironmentImpl::create_Connection()
{
	return OK_NEW_OPERATOR CPostgresConnectionImpl(this);
}

void CPostgresEnvironmentImpl::Open()
{
}

void CPostgresEnvironmentImpl::Close()
{
}

