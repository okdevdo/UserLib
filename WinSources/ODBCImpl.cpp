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
#include "ODBCImpl.h"

#define STR_LEN 128 + 1
#define REM_LEN 254 + 1
#define MAX_COLUMNSIZE 65536

#define	TRYODBC1(h,ht,x) {	RETCODE	iRet = x; \
	\
if (iRet != SQL_SUCCESS) \
							{ \
							h->HandleError(ht, iRet); \
							} \
if (iRet == SQL_ERROR) \
							{ \
if (h->get_LastError()) \
	h->get_LastError()->AppendString(TEXT("\r\nError in ") TEXT(#x), -1); \
	goto _exit;	\
							}  \
}

#define	TRYODBC(h,ht,x) {	RETCODE	iRet = x; \
	\
if (iRet != SQL_SUCCESS) \
							{ \
							h->HandleError(ht, iRet); \
							} \
if (iRet == SQL_ERROR) \
							{ \
if ((h->get_Environment()) && (h->get_Environment()->get_LastError())) \
	h->get_Environment()->get_LastError()->AppendString(TEXT("\r\nError in ") TEXT(#x), -1); \
	goto _exit;	\
							}  \
}

CODBCStatementImpl::CODBCStatementImpl(CODBCEnvironmentImpl* lpEnv, SQLHSTMT lpStmt)
{
	_lpEnv = lpEnv;
	_lpStmt = lpStmt;
	_siNumResultColumns = 0;
	_siNumResultRows = 0;
	_siNumParams = 0;
	_pParamArrayResults = nullptr;
	_siParamArrayCnt = 0;

}

CODBCStatementImpl::~CODBCStatementImpl(void)
{
	Free();
}

void CODBCStatementImpl::Prepare(LPCTSTR pCommand, dword arraysize)
{
	if ((!_lpEnv) || (!_lpStmt) || PtrCheck(pCommand))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::Prepare] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLRETURN RetCode = SQL_SUCCESS;

	Close();
	RetCode = SQLPrepare(_lpStmt, (SQLTCHAR*)pCommand, SQL_NTS);

	switch (RetCode)
	{
	case SQL_SUCCESS_WITH_INFO:
		HandleError(SQL_HANDLE_STMT, RetCode);
	case SQL_SUCCESS:
		{
			TRYODBC(this,
				SQL_HANDLE_STMT,
				SQLNumParams(_lpStmt, &_siNumParams));

			_command.SetString(__FILE__LINE__ pCommand);
		}
		break;
	case SQL_ERROR:
		HandleError(SQL_HANDLE_STMT, RetCode);
		goto _exit;
	default:
		{
			Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException();

			pEx->FormatString(_T("Unexpected return code %d!"), RetCode);
			_lpEnv->set_LastError(pEx);
			goto _exit;
		}
		break;
	}
	TRYODBC(this,
		SQL_HANDLE_STMT, SQLSetStmtAttr(_lpStmt, SQL_ATTR_PARAMSET_SIZE, CastAny(SQLPOINTER, arraysize), 0));
	if (arraysize > 1)
	{
		_pParamArrayResults = CastAnyPtr(SQLUSMALLINT, TFalloc(arraysize * sizeof(SQLUSMALLINT)));
		_siParamArrayCnt = 0;

		TRYODBC(this,
			SQL_HANDLE_STMT, SQLSetStmtAttr(_lpStmt, SQL_ATTR_PARAM_BIND_TYPE, SQL_PARAM_BIND_BY_COLUMN, 0));
		TRYODBC(this,
			SQL_HANDLE_STMT, SQLSetStmtAttr(_lpStmt, SQL_ATTR_PARAM_STATUS_PTR, _pParamArrayResults, 0));
		TRYODBC(this,
			SQL_HANDLE_STMT, SQLSetStmtAttr(_lpStmt, SQL_ATTR_PARAMS_PROCESSED_PTR, &_siParamArrayCnt, 0));
	}
	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

#ifdef OK_CPU_64BIT
void CODBCStatementImpl::BindParameter(word no, SDWConstPointer value, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatementImpl::BindParameter(word no, SDWConstPointer value, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
{
	if ((!_lpEnv) || (!_lpStmt) || (no > _siNumParams))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLSMALLINT vParamDir;
	SQLSMALLINT vDataType;
	SQLULEN vParameterSize;
	SQLSMALLINT vDecimalDigits;
	SQLSMALLINT vNullable;

	switch (paramdir)
	{
	case CODBCStatement::eParamInput:
		vParamDir = SQL_PARAM_INPUT;
		break;
	case CODBCStatement::eParamOutput:
		vParamDir = SQL_PARAM_OUTPUT;
		break;
	case CODBCStatement::eParamInOutput:
		vParamDir = SQL_PARAM_INPUT_OUTPUT;
		break;
	}
	
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLDescribeParam(_lpStmt, no, &vDataType, &vParameterSize, &vDecimalDigits, &vNullable));

#ifdef OK_CPU_64BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_LONG, vDataType, vParameterSize, vDecimalDigits, 
			CastAny(SQLPOINTER, CastMutable(SDWPointer, value)), szsdword, 
			CastAnyPtr(SQLLEN, CastMutable(SQWPointer, valueSize))));
#endif
#ifdef OK_CPU_32BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_LONG, vDataType, vParameterSize, vDecimalDigits, 
			CastAny(SQLPOINTER, CastMutable(SDWPointer, value)), szsdword, 
			CastAnyPtr(SQLLEN, CastMutable(SDWPointer, valueSize))));
#endif

	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

#ifdef OK_CPU_64BIT
void CODBCStatementImpl::BindParameter(word no, const double* value, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatementImpl::BindParameter(word no, const double* value, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
{
	if ((!_lpEnv) || (!_lpStmt) || (no > _siNumParams))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLSMALLINT vParamDir;
	SQLSMALLINT vDataType;
	SQLULEN vParameterSize;
	SQLSMALLINT vDecimalDigits;
	SQLSMALLINT vNullable;

	switch (paramdir)
	{
	case CODBCStatement::eParamInput:
		vParamDir = SQL_PARAM_INPUT;
		break;
	case CODBCStatement::eParamOutput:
		vParamDir = SQL_PARAM_OUTPUT;
		break;
	case CODBCStatement::eParamInOutput:
		vParamDir = SQL_PARAM_INPUT_OUTPUT;
		break;
	}

	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLDescribeParam(_lpStmt, no, &vDataType, &vParameterSize, &vDecimalDigits, &vNullable));

#ifdef OK_CPU_64BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_DOUBLE, vDataType, vParameterSize, vDecimalDigits, 
			CastAny(SQLPOINTER, CastMutablePtr(double, value)), sizeof(double), 
			CastAnyPtr(SQLLEN, CastMutable(SQWPointer, valueSize))));
#endif
#ifdef OK_CPU_32BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_DOUBLE, vDataType, vParameterSize, vDecimalDigits, 
			CastAny(SQLPOINTER, CastMutablePtr(double, value)), sizeof(double), 
			CastAnyPtr(SQLLEN, CastMutable(SDWPointer, valueSize))));
#endif

	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

#ifdef OK_CPU_64BIT
void CODBCStatementImpl::BindParameter(word no, CConstPointer value, qword maxvaluesize, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatementImpl::BindParameter(word no, CConstPointer value, dword maxvaluesize, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
{
	if ((!_lpEnv) || (!_lpStmt) || (no > _siNumParams))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLSMALLINT vParamDir;
	SQLSMALLINT vDataType;
	SQLULEN vParameterSize;
	SQLSMALLINT vDecimalDigits;
	SQLSMALLINT vNullable;

	switch (paramdir)
	{
	case CODBCStatement::eParamInput:
		vParamDir = SQL_PARAM_INPUT;
		break;
	case CODBCStatement::eParamOutput:
		vParamDir = SQL_PARAM_OUTPUT;
		break;
	case CODBCStatement::eParamInOutput:
		vParamDir = SQL_PARAM_INPUT_OUTPUT;
		break;
	}

	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLDescribeParam(_lpStmt, no, &vDataType, &vParameterSize, &vDecimalDigits, &vNullable));

#ifdef OK_CPU_64BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_TCHAR, vDataType, vParameterSize, vDecimalDigits,
			CastAny(SQLPOINTER, CastMutable(CPointer, value)), maxvaluesize, 
			CastAnyPtr(SQLLEN, CastMutable(SQWPointer, valueSize))));
#endif
#ifdef OK_CPU_32BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_TCHAR, vDataType, vParameterSize, vDecimalDigits,
			CastAny(SQLPOINTER, CastMutable(CPointer, value)), maxvaluesize, 
			CastAnyPtr(SQLLEN, CastMutable(SDWPointer, valueSize))));
#endif
	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

#ifdef OK_CPU_64BIT
void CODBCStatementImpl::BindParameter(word no, CConstPointer name, SDWConstPointer value, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatementImpl::BindParameter(word no, CConstPointer name, SDWConstPointer value, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
{
	if ((!_lpEnv) || (!_lpStmt) || (no > _siNumParams))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLSMALLINT vParamDir;
	SQLSMALLINT vDataType;
	SQLULEN vParameterSize;
	SQLSMALLINT vDecimalDigits;
	SQLSMALLINT vNullable;
	SQLHDESC hIpd = nullptr;

	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLDescribeParam(_lpStmt, no, &vDataType, &vParameterSize, &vDecimalDigits, &vNullable));

	switch (paramdir)
	{
	case CODBCStatement::eParamInput:
		vParamDir = SQL_PARAM_INPUT;
		break;
	case CODBCStatement::eParamOutput:
		vParamDir = SQL_PARAM_OUTPUT;
		break;
	case CODBCStatement::eParamInOutput:
		vParamDir = SQL_PARAM_INPUT_OUTPUT;
		break;
	}

#ifdef OK_CPU_64BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_LONG, vDataType, vParameterSize, vDecimalDigits, 
			CastAny(SQLPOINTER, CastMutable(SDWPointer, value)), szsdword, 
			CastAnyPtr(SQLLEN, CastMutable(SQWPointer, valueSize))));
#endif
#ifdef OK_CPU_32BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_LONG, vDataType, vParameterSize, vDecimalDigits, 
			CastAny(SQLPOINTER, CastMutable(SDWPointer, value)), szsdword, 
			CastAnyPtr(SQLLEN, CastMutable(SDWPointer, valueSize))));
#endif

	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLGetStmtAttr(_lpStmt, SQL_ATTR_IMP_PARAM_DESC, &hIpd, 0, 0));

	TRYODBC(this,
		SQL_HANDLE_DESC,
		SQLSetDescField(hIpd, no, SQL_DESC_NAME, CastAny(SQLPOINTER, CastMutable(CPointer, name)), SQL_NTS));

	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

#ifdef OK_CPU_64BIT
void CODBCStatementImpl::BindParameter(word no, CConstPointer name, const double* value, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatementImpl::BindParameter(word no, CConstPointer name, const double* value, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
{
	if ((!_lpEnv) || (!_lpStmt) || (no > _siNumParams))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLSMALLINT vParamDir;
	SQLSMALLINT vDataType;
	SQLULEN vParameterSize;
	SQLSMALLINT vDecimalDigits;
	SQLSMALLINT vNullable;
	SQLHDESC hIpd = nullptr;

	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLDescribeParam(_lpStmt, no, &vDataType, &vParameterSize, &vDecimalDigits, &vNullable));

	switch (paramdir)
	{
	case CODBCStatement::eParamInput:
		vParamDir = SQL_PARAM_INPUT;
		break;
	case CODBCStatement::eParamOutput:
		vParamDir = SQL_PARAM_OUTPUT;
		break;
	case CODBCStatement::eParamInOutput:
		vParamDir = SQL_PARAM_INPUT_OUTPUT;
		break;
	}

#ifdef OK_CPU_64BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_DOUBLE, vDataType, vParameterSize, vDecimalDigits, 
			CastAny(SQLPOINTER, CastMutablePtr(double, value)), sizeof(double), 
			CastAnyPtr(SQLLEN, CastMutable(SQWPointer, valueSize))));
#endif
#ifdef OK_CPU_32BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_DOUBLE, vDataType, vParameterSize, vDecimalDigits, 
			CastAny(SQLPOINTER, CastMutablePtr(double, value)), sizeof(double), 
			CastAnyPtr(SQLLEN, CastMutable(SDWPointer, valueSize))));
#endif

	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLGetStmtAttr(_lpStmt, SQL_ATTR_IMP_PARAM_DESC, &hIpd, 0, 0));

	TRYODBC(this,
		SQL_HANDLE_DESC,
		SQLSetDescField(hIpd, no, SQL_DESC_NAME, CastAny(SQLPOINTER, CastMutable(CPointer, name)), SQL_NTS));

	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

#ifdef OK_CPU_64BIT
void CODBCStatementImpl::BindParameter(word no, CConstPointer name, CConstPointer value, qword maxvaluesize, SQWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
#ifdef OK_CPU_32BIT
void CODBCStatementImpl::BindParameter(word no, CConstPointer name, CConstPointer value, dword maxvaluesize, SDWConstPointer valueSize, CODBCStatement::TParamDirection paramdir)
#endif
{
	if ((!_lpEnv) || (!_lpStmt) || (no > _siNumParams))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLSMALLINT vParamDir;
	SQLSMALLINT vDataType;
	SQLULEN vParameterSize;
	SQLSMALLINT vDecimalDigits;
	SQLSMALLINT vNullable;
	SQLHDESC hIpd = nullptr;

	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLDescribeParam(_lpStmt, no, &vDataType, &vParameterSize, &vDecimalDigits, &vNullable));

	switch (paramdir)
	{
	case CODBCStatement::eParamInput:
		vParamDir = SQL_PARAM_INPUT;
		break;
	case CODBCStatement::eParamOutput:
		vParamDir = SQL_PARAM_OUTPUT;
		break;
	case CODBCStatement::eParamInOutput:
		vParamDir = SQL_PARAM_INPUT_OUTPUT;
		break;
	}

#ifdef OK_CPU_64BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_TCHAR, vDataType, vParameterSize, vDecimalDigits,
			CastAny(SQLPOINTER, CastMutable(CPointer, value)), maxvaluesize, 
			CastAnyPtr(SQLLEN, CastMutable(SQWPointer, valueSize))));
#endif
#ifdef OK_CPU_32BIT
	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLBindParameter(_lpStmt, no, vParamDir, SQL_C_TCHAR, vDataType, vParameterSize, vDecimalDigits,
			CastAny(SQLPOINTER, CastMutable(CPointer, value)), maxvaluesize, 
			CastAnyPtr(SQLLEN, CastMutable(SDWPointer, valueSize))));
#endif

	TRYODBC(this,
		SQL_HANDLE_STMT,
		SQLGetStmtAttr(_lpStmt, SQL_ATTR_IMP_PARAM_DESC, &hIpd, 0, 0));

	TRYODBC(this,
		SQL_HANDLE_DESC,
		SQLSetDescField(hIpd, no, SQL_DESC_NAME, CastAny(SQLPOINTER, CastMutable(CPointer, name)), SQL_NTS));

	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

void CODBCStatementImpl::Execute(LPCTSTR pCommand)
{
	if ((!_lpEnv) || (!_lpStmt))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::Execute] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLRETURN RetCode = SQL_SUCCESS;

	if (PtrCheck(pCommand))
		RetCode = SQLExecute(_lpStmt);
	else
	{
		Reset();
		RetCode = SQLExecDirect(_lpStmt, (SQLTCHAR*)pCommand, SQL_NTS);
	}

	_siNumResultColumns = 0;
	_siNumResultRows = 0;
	switch (RetCode)
	{
	case SQL_SUCCESS_WITH_INFO:
		HandleError(SQL_HANDLE_STMT, RetCode);
	case SQL_SUCCESS:
		{
			TRYODBC(this,
				SQL_HANDLE_STMT,
				SQLNumResultCols(_lpStmt, &_siNumResultColumns));

			if (_siNumResultColumns <= 0)
			{
				TRYODBC(this,
					SQL_HANDLE_STMT,
					SQLRowCount(_lpStmt, &_siNumResultRows));
			}
			if (NotPtrCheck(pCommand))
				_command.SetString(__FILE__LINE__ pCommand);
		}
		break;
	case SQL_ERROR:
		HandleError(SQL_HANDLE_STMT, RetCode);
		goto _exit;
	default:
		{
			Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException;

			pEx->FormatString(_T("Unexpected return code %d!"), RetCode);
			_lpEnv->set_LastError(pEx);
			goto _exit;
		}
		break;
	}
	if (_pParamArrayResults)
	{
		Ptr(CODBCException) pEx = nullptr;

		for (SQLULEN i = 0; i < _siParamArrayCnt; i++)
		{
			switch (_pParamArrayResults[i]) {
			case SQL_PARAM_SUCCESS:
			case SQL_PARAM_SUCCESS_WITH_INFO:
				break;

			case SQL_PARAM_ERROR:
				if (!pEx)
					pEx = OK_NEW_OPERATOR CODBCException;
				pEx->FormatString(_T("[%lld] Error\n"), i);
				break;

			case SQL_PARAM_UNUSED:
				if (!pEx)
					pEx = OK_NEW_OPERATOR CODBCException;
				pEx->FormatString(_T("[%lld] Unused\n"), i);
				break;

			case SQL_PARAM_DIAG_UNAVAILABLE:
				if (!pEx)
					pEx = OK_NEW_OPERATOR CODBCException;
				pEx->FormatString(_T("[%lld] Unknown\n"), i);
				break;

			}
		}
		if (pEx)
		{
			_lpEnv->set_LastError(pEx);
			goto _exit;
		}
	}
	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

void CODBCStatementImpl::BindColumns(Ref(CODBCStatement::CODBCColumns) cols)
{
	if ((!_lpEnv) || (!_lpStmt) || (_siNumResultColumns <= 0))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::BindColumns] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLSMALLINT	iCol;

	for (iCol = 1; iCol <= _siNumResultColumns; ++iCol)
	{
		SQLSMALLINT vNameLength;
		SQLSMALLINT vDataType;
		SQLULEN vColumnSize;
		SQLSMALLINT vDecimalDigits;
		SQLSMALLINT vNullable;
		CStringBuffer vTmp;

		TRYODBC(this,
			SQL_HANDLE_STMT,
			SQLDescribeCol(_lpStmt,
			iCol,
			nullptr,
			0,
			&vNameLength,
			&vDataType,
			&vColumnSize,
			&vDecimalDigits,
			&vNullable));

		if (vNameLength > 0)
		{
			vTmp.SetSize(__FILE__LINE__ vNameLength + 1);

			TRYODBC(this,
				SQL_HANDLE_STMT,
				SQLDescribeCol(_lpStmt,
				iCol,
				CastAnyPtr(SQLTCHAR, CastMutable(CPointer, vTmp.GetString())),
				vNameLength + 1,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr));
		}
		
		CODBCColumn::TDataType vDataTypeEnum = CODBCColumn::eSQL_NULL;
		ULongPointer vCBufferSize = 0;
		SQLSMALLINT vCDataType;

		switch (vDataType)
		{
		case SQL_CHAR:  // CHAR(n), Character string of fixed string length n.
			vDataTypeEnum = CODBCColumn::eSQL_CHAR; 
			vCDataType = SQL_C_TCHAR;
			if (vColumnSize)
				vCBufferSize = (vColumnSize + 1) * sizeof(TCHAR);
			break;

		case SQL_VARCHAR: // VARCHAR(n), Variable - length character string with a maximum string length n.
			vDataTypeEnum = CODBCColumn::eSQL_VARCHAR;
			vCDataType = SQL_C_TCHAR;
			if (vColumnSize)
				vCBufferSize = (vColumnSize + 1) * sizeof(TCHAR);
			break;
			
		case SQL_LONGVARCHAR: // LONG VARCHAR, Variable length character data. Maximum length is data source–dependent.[9]
			vDataTypeEnum = CODBCColumn::eSQL_LONGVARCHAR;
			vCDataType = SQL_C_TCHAR;
			if (vColumnSize)
				vCBufferSize = (vColumnSize + 1) * sizeof(TCHAR);
			break;
			
		case SQL_WCHAR: // WCHAR(n), Unicode character string of fixed string length n
			vDataTypeEnum = CODBCColumn::eSQL_WCHAR;
			vCDataType = SQL_C_TCHAR;
			if (vColumnSize)
				vCBufferSize = (vColumnSize + 1) * sizeof(TCHAR);
			break;
			
		case SQL_WVARCHAR: // VARWCHAR(n), Unicode variable - length character string with a maximum string length n
			vDataTypeEnum = CODBCColumn::eSQL_WVARCHAR;
			vCDataType = SQL_C_TCHAR;
			if (vColumnSize)
				vCBufferSize = (vColumnSize + 1) * sizeof(TCHAR);
			break;
			
		case SQL_WLONGVARCHAR: // LONGWVARCHAR, Unicode variable - length character data.Maximum length is data source–dependent
			vDataTypeEnum = CODBCColumn::eSQL_WLONGVARCHAR;
			vCDataType = SQL_C_TCHAR;
			if (vColumnSize)
				vCBufferSize = (vColumnSize + 1) * sizeof(TCHAR);
			break;
			
		case SQL_DECIMAL: // DECIMAL(p, s), Signed, exact, numeric value with a precision of at least p and scale s. (The maximum precision is driver - defined.) (1 <= p <= 15; s <= p).[4]
			vDataTypeEnum = CODBCColumn::eSQL_DECIMAL;
			vCDataType = SQL_C_TCHAR;
			if (vColumnSize)
				vCBufferSize = (vColumnSize + 1) * sizeof(TCHAR);
			break;
			
		case SQL_NUMERIC: // NUMERIC(p, s), Signed, exact, numeric value with a precision p and scale s(1 <= p <= 15; s <= p).[4]
			vDataTypeEnum = CODBCColumn::eSQL_NUMERIC;
			vCDataType = SQL_C_TCHAR;
			if (vColumnSize)
				vCBufferSize = (vColumnSize + 1) * sizeof(TCHAR);
			break;
			
		case SQL_SMALLINT: // SMALLINT, Exact numeric value with precision 5 and scale 0 (signed: –32, 768 <= n <= 32, 767, unsigned : 0 <= n <= 65, 535)[3].
			vDataTypeEnum = CODBCColumn::eSQL_SMALLINT;
			vCDataType = SQL_C_SHORT;
			vCBufferSize = sizeof(short);
			break;
			
		case SQL_INTEGER: // INTEGER, Exact numeric value with precision 10 and scale 0 (signed: –2[31] <= n <= 2[31] – 1, unsigned : 0 <= n <= 2[32] – 1)[3].
			vDataTypeEnum = CODBCColumn::eSQL_INTEGER;
			vCDataType = SQL_C_LONG;
			vCBufferSize = sizeof(long);
			break;
			
		case SQL_REAL: // REAL, Signed, approximate, numeric value with a binary precision 24 (zero or absolute value 10[–38] to 10[38]).
			vDataTypeEnum = CODBCColumn::eSQL_REAL;
			vCDataType = SQL_C_FLOAT;
			vCBufferSize = sizeof(float);
			break;
			
		case SQL_FLOAT: // FLOAT(p), Signed, approximate, numeric value with a binary precision of at least p. (The maximum precision is driver - defined.)[5]
			vDataTypeEnum = CODBCColumn::eSQL_FLOAT;
			vCDataType = SQL_C_DOUBLE;
			vCBufferSize = sizeof(double);
			break;
			
		case SQL_DOUBLE: // DOUBLE PRECISION, Signed, approximate, numeric value with a binary precision 53 (zero or absolute value 10[–308] to 10[308]).
			vDataTypeEnum = CODBCColumn::eSQL_DOUBLE;
			vCDataType = SQL_C_DOUBLE;
			vCBufferSize = sizeof(double);
			break;
			
		case SQL_BIT: // BIT, Single bit binary data.[8]
			vDataTypeEnum = CODBCColumn::eSQL_BIT;
			vCDataType = SQL_C_BIT;
			vCBufferSize = sizeof(char);
			break;
			
		case SQL_TINYINT: // TINYINT, Exact numeric value with precision 3 and scale 0 (signed: –128 <= n <= 127, unsigned : 0 <= n <= 255)[3].
			vDataTypeEnum = CODBCColumn::eSQL_TINYINT;
			vCDataType = SQL_C_TINYINT;
			vCBufferSize = sizeof(char);
			break;
			
		case SQL_BIGINT: // BIGINT, Exact numeric value with precision 19 (if signed) or 20 (if unsigned) and scale 0 (signed: –2[63] <= n <= 2[63] – 1, unsigned : 0 <= n <= 2[64] – 1)[3], [9].
			vDataTypeEnum = CODBCColumn::eSQL_BIGINT;
			vCDataType = SQL_C_SBIGINT;
			vCBufferSize = sizeof(sqword);
			break;
			
		case SQL_BINARY: // BINARY(n), Binary data of fixed length n.[9]
			vDataTypeEnum = CODBCColumn::eSQL_BINARY;
			vCDataType = SQL_C_BINARY;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_VARBINARY: // VARBINARY(n), Variable length binary data of maximum length n.The maximum is set by the user.[9]
			vDataTypeEnum = CODBCColumn::eSQL_VARBINARY;
			vCDataType = SQL_C_BINARY;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_LONGVARBINARY: // LONG VARBINARY, Variable length binary data.Maximum length is data source–dependent.[9]
			vDataTypeEnum = CODBCColumn::eSQL_LONGVARBINARY;
			vCDataType = SQL_C_BINARY;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_TYPE_DATE: // DATE, Year, month, and day fields, conforming to the rules of the Gregorian calendar. (See Constraints of the Gregorian Calendar, later in this appendix.)
			vDataTypeEnum = CODBCColumn::eSQL_TYPE_DATE;
			vCDataType = SQL_C_DATE;
			vCBufferSize = 10;
			break;
			
		case SQL_TYPE_TIME: // TIME(p), Hour, minute, and second fields, with valid values for hours of 00 to 23, valid values for minutes of 00 to 59, and valid values for seconds of 00 to 61. Precision p indicates the seconds precision.
			vDataTypeEnum = CODBCColumn::eSQL_TYPE_TIME;
			vCDataType = SQL_C_TIME;
			vCBufferSize = 8;
			break;
			
		case SQL_TYPE_TIMESTAMP: // TIMESTAMP(p), Year, month, day, hour, minute, and second fields, with valid values as defined for the DATE and TIME data types.
			vDataTypeEnum = CODBCColumn::eSQL_TYPE_TIMESTAMP;
			vCDataType = SQL_C_TIMESTAMP;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_MONTH: // INTERVAL MONTH(p), Number of months between two dates; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_MONTH;
			vCDataType = SQL_C_INTERVAL_MONTH;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_YEAR: // INTERVAL YEAR(p), Number of years between two dates; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_YEAR;
			vCDataType = SQL_C_INTERVAL_YEAR;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_YEAR_TO_MONTH: // INTERVAL YEAR(p) TO MONTH, Number of years and months between two dates; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_YEAR_TO_MONTH;
			vCDataType = SQL_C_INTERVAL_YEAR_TO_MONTH;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_DAY: // INTERVAL DAY(p), Number of days between two dates; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_DAY;
			vCDataType = SQL_C_INTERVAL_DAY;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_HOUR: // INTERVAL HOUR(p), Number of hours between two date / times; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_HOUR;
			vCDataType = SQL_C_INTERVAL_HOUR;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_MINUTE: // INTERVAL MINUTE(p), Number of minutes between two date / times; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_MINUTE;
			vCDataType = SQL_C_INTERVAL_MINUTE;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_SECOND: // INTERVAL SECOND(p, q), Number of seconds between two date / times; p is the interval leading precision and q is the interval seconds precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_SECOND;
			vCDataType = SQL_C_INTERVAL_SECOND;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_DAY_TO_HOUR: // INTERVAL DAY(p) TO HOUR, Number of days / hours between two date / times; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_DAY_TO_HOUR;
			vCDataType = SQL_C_INTERVAL_DAY_TO_HOUR;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_DAY_TO_MINUTE: // INTERVAL DAY(p) TO MINUTE, Number of days / hours / minutes between two date / times; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_DAY_TO_MINUTE;
			vCDataType = SQL_C_INTERVAL_DAY_TO_MINUTE;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_DAY_TO_SECOND: // INTERVAL DAY(p) TO SECOND(q), Number of days / hours / minutes / seconds between two date / times; p is the interval leading precision and q is the interval seconds precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_DAY_TO_SECOND;
			vCDataType = SQL_C_INTERVAL_DAY_TO_SECOND;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_HOUR_TO_MINUTE: // INTERVAL HOUR(p) TO MINUTE, Number of hours / minutes between two date / times; p is the interval leading precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_HOUR_TO_MINUTE;
			vCDataType = SQL_C_INTERVAL_HOUR_TO_MINUTE;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_HOUR_TO_SECOND: // INTERVAL HOUR(p) TO SECOND(q), Number of hours / minutes / seconds between two date / times; p is the interval leading precision and q is the interval seconds precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_HOUR_TO_SECOND;
			vCDataType = SQL_C_INTERVAL_HOUR_TO_SECOND;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_INTERVAL_MINUTE_TO_SECOND: // INTERVAL MINUTE(p) TO SECOND(q), Number of minutes / seconds between two date / times; p is the interval leading precision and q is the interval seconds precision.
			vDataTypeEnum = CODBCColumn::eSQL_INTERVAL_MINUTE_TO_SECOND;
			vCDataType = SQL_C_INTERVAL_MINUTE_TO_SECOND;
			vCBufferSize = vColumnSize;
			break;
			
		case SQL_GUID: // GUID, Fixed length GUID.
			vDataTypeEnum = CODBCColumn::eSQL_GUID;
			vCDataType = SQL_C_GUID;
			vCBufferSize = 36;
			break;
			
		default:
			break;
		}

		Ptr(CODBCColumn) pColumn = OK_NEW_OPERATOR CODBCColumn(vTmp, vDataTypeEnum, vDecimalDigits, vNullable == SQL_NULLABLE, Cast(dword, vCBufferSize));

		if (vCBufferSize > 0)
		{
			TRYODBC(this,
				SQL_HANDLE_STMT,
				SQLBindCol(_lpStmt,
				iCol,
				vCDataType,
				CastAny(SQLPOINTER, pColumn->get_Data().get_Buffer()),
				vCBufferSize,
				pColumn->get_IndPtr()));
		}

		cols.Append(pColumn);
	}
	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

BOOL CODBCStatementImpl::Fetch()
{
	if ((!_lpEnv) || (!_lpStmt) || (_siNumResultColumns <= 0))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::Fetch] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	RETCODE RetCode;

	TRYODBC(this, SQL_HANDLE_STMT, RetCode = SQLFetch(_lpStmt));

	if (RetCode == SQL_NO_DATA_FOUND)
		return FALSE;
	return TRUE;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
	return FALSE;
}

void CODBCStatementImpl::Reset()
{
	if (!_lpEnv)
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::Close] Invalid arguments or programming sequence error"));

		throw pEx;
	}
	if (_pParamArrayResults)
	{
		TFfree(_pParamArrayResults);
		_pParamArrayResults = nullptr;
	}
	_siParamArrayCnt = 0;
	if (_lpStmt)
	{
		SQLFreeStmt(_lpStmt, SQL_UNBIND);
		SQLFreeStmt(_lpStmt, SQL_RESET_PARAMS);
	}
	_siNumResultColumns = 0;
	_siNumResultRows = 0;
	_siNumParams = 0;
}

void CODBCStatementImpl::Close()
{
	if (!_lpEnv)
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::Close] Invalid arguments or programming sequence error"));

		throw pEx;
	}
	Reset();
	if (_lpStmt)
		SQLFreeStmt(_lpStmt, SQL_CLOSE);
}

void CODBCStatementImpl::Free()
{
	Close();
	if (_lpStmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, _lpStmt);
		_lpStmt = nullptr;
	}
}

void CODBCStatementImpl::HandleError(SQLSMALLINT hType, RETCODE	RetCode)
{
	if ((!_lpEnv) || (!_lpStmt))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCStatementImpl::HandleError] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	if (RetCode == SQL_NO_DATA_FOUND)
		return;

	SQLHANDLE	hHandle = get_Handle();
	SQLSMALLINT	iRec = 0;
	SQLINTEGER	iError;
	TCHAR		szMessage[SQL_MAX_MESSAGE_LENGTH];
	TCHAR		szState[SQL_SQLSTATE_SIZE + 1];

	if (RetCode == SQL_INVALID_HANDLE)
	{
		_lpEnv->set_LastError(OK_NEW_OPERATOR CODBCException(TEXT("[CODBCStatementImpl] Invalid handle")));
		_lpEnv->get_LastError()->set_Severity(E_SQL_FATALERROR);

		throw _lpEnv->get_LastError();
	}

	_lpEnv->set_LastError(OK_NEW_OPERATOR CODBCException());
	if (_command)
		_lpEnv->get_LastError()->AppendString(_command.GetString(), _command.GetLength());
	while (SQLGetDiagRec(hType,
		hHandle,
		++iRec,
		(SQLTCHAR*)szState,
		&iError,
		(SQLTCHAR*)szMessage,
		(SQLSMALLINT)(sizeof(szMessage) / sizeof(TCHAR)),
		(SQLSMALLINT *)nullptr) == SQL_SUCCESS)
	{

		// Hide data truncated..
		if (s_strncmp(szState, _T("01004"), 5))
			_lpEnv->get_LastError()->FormatString(_T("\r\n[%5.5s] %s (%d)"), szState, szMessage, iError);
	}
}

CODBCConnectionImpl::CODBCConnectionImpl(CODBCEnvironmentImpl* lpEnv, SQLHDBC lpDbc)
{
	_lpEnv = lpEnv;
	_lpDbc = lpDbc;
	_numStmts = 0;
	_cntStmts = 0;
}

CODBCConnectionImpl::~CODBCConnectionImpl(void)
{
	Close();
}

bool CODBCConnectionImpl::can_Transact()
{
	SQLUSMALLINT ret;

	TRYODBC(this,
		SQL_HANDLE_DBC, SQLGetInfo(_lpDbc, SQL_TXN_CAPABLE, &ret, 0, 0));

	return (SQL_TC_NONE != ret);
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
	return false;
}

void CODBCConnectionImpl::close_Transact(bool commit)
{
	if (is_AutoCommit())
		return;
	TRYODBC(this,
		SQL_HANDLE_DBC, SQLEndTran(SQL_HANDLE_DBC, _lpDbc, commit ? SQL_COMMIT : SQL_ROLLBACK));
	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

void CODBCConnectionImpl::set_AutoCommit(bool val)
{
	TRYODBC(this,
		SQL_HANDLE_DBC, SQLSetConnectAttr(_lpDbc, SQL_ATTR_AUTOCOMMIT, CastAny(SQLPOINTER, val ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF), 0));
	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

bool CODBCConnectionImpl::is_AutoCommit()
{
	dword value = 0;

	TRYODBC(this,
		SQL_HANDLE_DBC, SQLGetConnectAttr(_lpDbc, SQL_ATTR_AUTOCOMMIT, &value, 0, 0));

	return (0 != value);
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
	return false;
}

bool CODBCConnectionImpl::is_Connected()
{
	dword value = 0;

	TRYODBC(this,
		SQL_HANDLE_DBC, SQLGetConnectAttr(_lpDbc, SQL_ATTR_CONNECTION_DEAD, &value, 0, 0));

	return (0 == value);
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
	return false;
}

void CODBCConnectionImpl::Open(LPCTSTR _pszConnStr)
{
	if ((!_lpEnv) || (!_lpDbc))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCConnectionImpl::Open] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

#ifdef __DEBUG__
	TRYODBC(this,
		SQL_HANDLE_DBC,
		SQLSetConnectAttr(_lpDbc,
		SQL_ATTR_LOGIN_TIMEOUT,
		(SQLPOINTER)30,
		0));
#else
	TRYODBC(this,
		SQL_HANDLE_DBC,
		SQLSetConnectAttr(_lpDbc,
		SQL_ATTR_LOGIN_TIMEOUT,
		(SQLPOINTER)5,
		0));
#endif
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS32
	TRYODBC(this,
		SQL_HANDLE_DBC,
		SQLDriverConnect(_lpDbc,
		GetDesktopWindow(),
		(SQLTCHAR*)_pszConnStr,
		SQL_NTS,
		nullptr,
		0,
		nullptr,
		SQL_DRIVER_COMPLETE));
#endif
#ifdef OK_SYS_WINDOWS64
	TRYODBC(this,
		SQL_HANDLE_DBC,
		SQLDriverConnect(_lpDbc,
		nullptr,
		(SQLTCHAR*)_pszConnStr,
		SQL_NTS,
		nullptr,
		0,
		nullptr,
		SQL_DRIVER_COMPLETE));
#endif
#endif
#ifdef OK_COMP_MSC
	TRYODBC(this,
		SQL_HANDLE_DBC,
		SQLDriverConnect(_lpDbc,
		GetDesktopWindow(),
		(SQLTCHAR*)_pszConnStr,
		SQL_NTS,
		nullptr,
		0,
		nullptr,
		SQL_DRIVER_COMPLETE));
#endif

	//TRYODBC(this,
	//	SQL_HANDLE_DBC,
	//	SQLGetInfo(_lpDbc,
	//					SQL_ACTIVE_STATEMENTS,
	//					&_numStmts,
	//					0,
	//					nullptr));

	return;
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
}

CODBCStatementImpl* CODBCConnectionImpl::create_Statement()
{
	if ((!_lpEnv) || (!_lpDbc) || (!is_Connected()) || ((_numStmts > 0) && (_cntStmts >= _numStmts)))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCConnectionImpl::create_Statement] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLHSTMT lpStmt = nullptr;

	TRYODBC(this,
		SQL_HANDLE_DBC,
		SQLAllocHandle(SQL_HANDLE_STMT, _lpDbc, &lpStmt));

	++_cntStmts;
	return OK_NEW_OPERATOR CODBCStatementImpl(_lpEnv, lpStmt);
_exit:
	if (_lpEnv->get_LastError())
		throw _lpEnv->get_LastError();
	return nullptr;
}

void CODBCConnectionImpl::free_Statement(CODBCStatementImpl* pStmt)
{
	if ((!_lpEnv) || (!_lpDbc))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCConnectionImpl::free_Statement] Invalid arguments or programming sequence error"));

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

void CODBCConnectionImpl::Close()
{
	if (!_lpEnv)
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCConnectionImpl::Close] Invalid arguments or programming sequence error"));

		throw pEx;
	}
	if (is_Connected())
		SQLDisconnect(_lpDbc);
	if (_lpDbc)
	{
		SQLFreeConnect(_lpDbc);
		_lpDbc = nullptr;
	}
}

void CODBCConnectionImpl::HandleError(SQLSMALLINT hType, RETCODE RetCode)
{
	if ((!_lpEnv) || (!_lpDbc))
	{
		Ptr(CODBCException) pEx = OK_NEW_OPERATOR CODBCException(_T("[CODBCConnectionImpl::HandleError] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	SQLHANDLE	hHandle = get_Handle();
	SQLSMALLINT	iRec = 0;
	SQLINTEGER	iError;
	TCHAR		szMessage[SQL_MAX_MESSAGE_LENGTH];
	TCHAR		szState[SQL_SQLSTATE_SIZE + 1];


	if (RetCode == SQL_INVALID_HANDLE)
	{
		_lpEnv->set_LastError(OK_NEW_OPERATOR CODBCException(TEXT("[CODBCConnectionImpl] Invalid handle")));
		_lpEnv->get_LastError()->set_Severity(E_SQL_FATALERROR);

		throw _lpEnv->get_LastError();
	}

	_lpEnv->set_LastError(OK_NEW_OPERATOR CODBCException());
	while (SQLGetDiagRec(hType,
		hHandle,
		++iRec,
		(SQLTCHAR*)szState,
		&iError,
		(SQLTCHAR*)szMessage,
		(SQLSMALLINT)(sizeof(szMessage) / sizeof(TCHAR)),
		(SQLSMALLINT *)nullptr) == SQL_SUCCESS)
	{

		// Hide data truncated..
		if (s_strncmp(szState, _T("01004"), 5))
			_lpEnv->get_LastError()->FormatString(_T("[%5.5s] %s (%d)\r\n"), szState, szMessage, iError);
	}
}

CODBCEnvironmentImpl::CODBCEnvironmentImpl(void)
{
	_lpEnv = nullptr;
	_lastError = nullptr;
}


CODBCEnvironmentImpl::~CODBCEnvironmentImpl(void)
{
	Close();
	if (_lastError)
		delete _lastError;
	_lastError = nullptr;
}

void CODBCEnvironmentImpl::set_LastError(CODBCException* pLastError)
{
	if (_lastError)
		delete _lastError;
	_lastError = pLastError;
}

CODBCConnectionImpl* CODBCEnvironmentImpl::create_Connection()
{
	if (!_lpEnv)
		return nullptr;

	SQLHDBC lpDbc = nullptr;

	TRYODBC1(this,
		SQL_HANDLE_ENV,
		SQLAllocHandle(SQL_HANDLE_DBC, _lpEnv, &lpDbc));

	return OK_NEW_OPERATOR CODBCConnectionImpl(this, lpDbc);
_exit:
	if (_lastError)
		throw _lastError;
	return nullptr;
}

void CODBCEnvironmentImpl::Open()
{
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_lpEnv) == SQL_ERROR)
	{
		_lastError = OK_NEW_OPERATOR CODBCException(TEXT("Unable to allocate an environment handle"));
		_lastError->set_Severity(E_SQL_FATALERROR);
		goto _exit;
	}
	TRYODBC1(this,
		SQL_HANDLE_ENV, SQLSetEnvAttr(_lpEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER));
	return;
_exit:
	if (_lastError)
		throw _lastError;
}

void CODBCEnvironmentImpl::Close()
{
	if (_lpEnv)
	{
		SQLFreeEnv(_lpEnv);
		_lpEnv = nullptr;
	}
}

void CODBCEnvironmentImpl::HandleError(SQLSMALLINT hType, RETCODE RetCode)
{
	SQLHANDLE	hHandle = get_Handle();
	SQLSMALLINT	iRec = 0;
	SQLINTEGER	iError;
	TCHAR		szMessage[SQL_MAX_MESSAGE_LENGTH];
	TCHAR		szState[SQL_SQLSTATE_SIZE + 1];


	if (RetCode == SQL_INVALID_HANDLE)
	{
		set_LastError(OK_NEW_OPERATOR CODBCException(TEXT("[CODBCEnvironmentImpl] Invalid handle")));
		get_LastError()->set_Severity(E_SQL_FATALERROR);

		throw get_LastError();
	}

	set_LastError(OK_NEW_OPERATOR CODBCException());
	while (SQLGetDiagRec(hType,
		hHandle,
		++iRec,
		(SQLTCHAR*)szState,
		&iError,
		(SQLTCHAR*)szMessage,
		(SQLSMALLINT)(sizeof(szMessage) / sizeof(TCHAR)),
		(SQLSMALLINT *)nullptr) == SQL_SUCCESS)
	{

		// Hide data truncated..
		if (s_strncmp(szState, _T("01004"), 5))
			get_LastError()->FormatString(_T("[%5.5s] %s (%d)\r\n"), szState, szMessage, iError);
	}
}
