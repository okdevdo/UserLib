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
#include "UTLPTR.H"

static void __stdcall CSqLite3FuncListDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CSqLite3Connection::create_function_infoclass) pInfoClass = CastAnyPtr(CSqLite3Connection::create_function_infoclass, CastMutable(Pointer, data));

	pInfoClass->release();
}

static sword __stdcall CSqLite3FuncListSearchAndSortFunc(ConstPointer item, ConstPointer data)
{
	Ptr(CSqLite3Connection::create_function_infoclass) pItemInfoClass = CastAnyPtr(CSqLite3Connection::create_function_infoclass, CastMutable(Pointer, item));
	Ptr(CSqLite3Connection::create_function_infoclass) pDataInfoClass = CastAnyPtr(CSqLite3Connection::create_function_infoclass, CastMutable(Pointer, data));

	return pItemInfoClass->get_Name().Compare(pDataInfoClass->get_Name());
}

CSqLite3StatementImpl::CSqLite3StatementImpl(CSqLite3EnvironmentImpl* lpEnv, CSqLite3ConnectionImpl* lpConn) :
_lpEnv(lpEnv),
_lpConn(lpConn),
_lpStmt(NULL),
_siNumResultColumns(0),
_siNumResultRows(0),
_siNumParams(0)
{
}

CSqLite3StatementImpl::~CSqLite3StatementImpl(void)
{
	Free();
}

void CSqLite3StatementImpl::Prepare(CConstPointer pCommand)
{
	if ((!_lpEnv) || (!_lpConn) || PtrCheck(pCommand))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::Prepare] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	int res = SQLITE_OK;

#ifdef UNICODE
	ConstPointer pzTail = CastAny(ConstPointer, pCommand);
	ConstPointer pzTail1 = NULL;

	while (NotPtrCheck(pzTail) && DerefCPointer(CastMutable(Pointer, pzTail)))
#else
	CConstPointer pzTail = pCommand;
	CConstPointer pzTail1 = NULL;

	while (NotPtrCheck(pzTail) && DerefCPointer(CastMutable(CPointer, pzTail)))
#endif
	{
		if (_lpStmt)
		{
			res = sqlite3_finalize(_lpStmt);

			if (res != SQLITE_OK)
				HandleError(__FILE__LINE__ res, _T("sqlite3_finalize"));
			_lpStmt = NULL;
		}

		pzTail1 = NULL;
#ifdef UNICODE
		res = sqlite3_prepare16_v2(
			_lpConn->get_Handle(),            /* Database handle */
			pzTail,                           /* SQL statement, UTF-16 encoded */
			-1,                               /* Maximum length of zSql in bytes. */
			&_lpStmt,						  /* OUT: Statement handle */
			&pzTail1                          /* OUT: Pointer to unused portion of zSql */
			);
		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_prepare16_v2"));
#else
		res = sqlite3_prepare_v2(
			_lpConn->get_Handle(),            /* Database handle */
			pzTail,                           /* SQL statement, UTF-8 encoded */
			-1,                               /* Maximum length of zSql in bytes. */
			&_lpStmt,						  /* OUT: Statement handle */
			&pzTail1                          /* OUT: Pointer to unused portion of zSql */
			);
		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_prepare_v2"));
#endif

		if (NotPtrCheck(pzTail1))
			_command.SetString(__FILE__LINE__ CastAny(CConstPointer, pzTail), Castdword(c_ptrdiff(pzTail1, pzTail)));
		else
			_command.SetString(__FILE__LINE__ CastAny(CConstPointer, pzTail));

		_siNumParams = sqlite3_bind_parameter_count(_lpStmt);
		if (_siNumParams > 0)
			return;

		_siNumResultColumns = sqlite3_column_count(_lpStmt);
		if (_siNumResultColumns > 0)
			return;

		res = sqlite3_step(_lpStmt);

		if (res != SQLITE_DONE)
			HandleError(__FILE__LINE__ res, _T("sqlite3_step"));
		pzTail = pzTail1;
	}
}

void CSqLite3StatementImpl::BindParameter(word no, sqword value)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpStmt) || (no == 0) || (no > _siNumParams))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	int res = sqlite3_bind_int64(_lpStmt, no, value);

	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_bind_int"));
}

void CSqLite3StatementImpl::BindParameter(word no, double value)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpStmt) || (no == 0) || (no > _siNumParams))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	int res = sqlite3_bind_double(_lpStmt, no, value);

	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_bind_double"));
}

void CSqLite3StatementImpl::BindParameter(word no, CConstPointer value, int valueSize)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpStmt) || (no == 0) || (no > _siNumParams))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::BindParameter] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	int res;

	if (PtrCheck(value) || (valueSize == 0))
	{
		res = sqlite3_bind_null(_lpStmt, no);

		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_bind_null"));
	}
	else
	{
#ifdef UNICODE
		res = sqlite3_bind_text16(_lpStmt, no, CastAny(ConstPointer, value), valueSize, SQLITE_STATIC);

		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_bind_text16"));
#else
		res = sqlite3_bind_text(_lpStmt, no, value, valueSize, SQLITE_STATIC);

		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_bind_text"));
#endif
	}
}

void CSqLite3StatementImpl::Execute(CConstPointer pCommand)
{
	if ((!_lpEnv) || (!_lpConn))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::Execute] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

#ifdef UNICODE
	ConstPointer pzTail = CastAny(ConstPointer, pCommand);
	ConstPointer pzTail1 = NULL;
#else
	CConstPointer pzTail = pCommand;
	CConstPointer pzTail1 = NULL;
#endif
	int res = SQLITE_OK;

	if (PtrCheck(pzTail))
	{
		if (!_lpStmt)
		{
			_lpEnv->set_LastError(OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("in CSqLite3StatementImpl::Execute: programming sequence error")));
			throw _lpEnv->get_LastError();
		}

		_siNumResultColumns = sqlite3_column_count(_lpStmt);
		if (_siNumResultColumns > 0)
			return;

		res = sqlite3_step(_lpStmt);

		if (res != SQLITE_DONE)
			HandleError(__FILE__LINE__ res, _T("sqlite3_step"));
		return;
	}

	_siNumParams = 0;
#ifdef UNICODE
	while (NotPtrCheck(pzTail) && DerefCPointer(CastMutable(Pointer, pzTail)))
#else
	while (NotPtrCheck(pzTail) && DerefCPointer(CastMutable(CPointer, pzTail)))
#endif
	{
		if (_lpStmt)
		{
			res = sqlite3_finalize(_lpStmt);

			if (res != SQLITE_OK)
				HandleError(__FILE__LINE__ res, _T("sqlite3_finalize"));
			_lpStmt = NULL;
		}

		pzTail1 = NULL;
#ifdef UNICODE
		res = sqlite3_prepare16_v2(
			_lpConn->get_Handle(),            /* Database handle */
			pzTail,                           /* SQL statement, UTF-16 encoded */
			-1,                               /* Maximum length of zSql in bytes. */
			&_lpStmt,						  /* OUT: Statement handle */
			&pzTail1                          /* OUT: Pointer to unused portion of zSql */
			);

		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_prepare16_v2"));
#else
		res = sqlite3_prepare_v2(
			_lpConn->get_Handle(),            /* Database handle */
			pzTail,                           /* SQL statement, UTF-16 encoded */
			-1,                               /* Maximum length of zSql in bytes. */
			&_lpStmt,						  /* OUT: Statement handle */
			&pzTail1                          /* OUT: Pointer to unused portion of zSql */
			);

		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_prepare16_v2"));
#endif

		if (NotPtrCheck(pzTail1))
			_command.SetString(__FILE__LINE__ CastAny(CConstPointer, pzTail), Castdword(c_ptrdiff(pzTail1, pzTail)));
		else
			_command.SetString(__FILE__LINE__ CastAny(CConstPointer, pzTail));

		_siNumResultColumns = sqlite3_column_count(_lpStmt);
		if (_siNumResultColumns > 0)
			return;

		res = sqlite3_step(_lpStmt);

		if (res != SQLITE_DONE)
			HandleError(__FILE__LINE__ res, _T("sqlite3_step"));
		pzTail = pzTail1;
	}
}

void CSqLite3StatementImpl::BindColumns(Ref(CSqLite3Statement::CSqLite3Columns) cols)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpStmt) || (_siNumResultColumns <= 0))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::BindColumns] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	sword iCol;

	for (iCol = 0; iCol < _siNumResultColumns; ++iCol)
	{
#ifdef UNICODE
		CStringBuffer vTmp(__FILE__LINE__ CastAny(CConstPointer, sqlite3_column_name16(_lpStmt, iCol)));
		CStringBuffer vDataType(__FILE__LINE__ CastAny(CConstPointer, sqlite3_column_decltype16(_lpStmt, iCol)));
#else
		CStringBuffer vTmp(__FILE__LINE__ CastAny(CConstPointer, sqlite3_column_name(_lpStmt, iCol)));
		CStringBuffer vDataType(__FILE__LINE__ CastAny(CConstPointer, sqlite3_column_decltype(_lpStmt, iCol)));
#endif

		//if (vDataType.IsEmpty())
		//{
		//	CSqLite3Connection::create_function_infoclass_list vFuncList = _lpConn->get_FuncList();
		//	CStringConstIterator it(vTmp);
		//	CStringBuffer vTmp1;

		//	it.FirstOf(_T("("));
		//	vTmp.SubString(0, it.GetDistance(), vTmp1);

		//	CSqLite3Connection::create_function_infoclass vInfoClass(NULL, vTmp1, NULL, 0);
		//	CSqLite3Connection::create_function_infoclass_list::Iterator it1 = vFuncList.FindSorted(&vInfoClass);

		//	if (it1 && (*it1) && ((*it1)->get_Name().Compare(vTmp1, 0, CStringLiteral::cIgnoreCase) == 0))
		//	{
		//		CSqLite3Connection::CSqLite3Columns::Iterator it2 = (*it1)->get_Args().Index(0);

		//		vDataType = (*it2)->get_DataTypeAsStr();
		//	}
		//}

		CSqLite3Column::TDataType vDataTypeEnum = CSqLite3Column::eSQL_NULL;

		if (vDataType.Compare(_T("INTEGER"), 7, CStringLiteral::cIgnoreCase + CStringLiteral::cLimited) == 0)
			vDataTypeEnum = CSqLite3Column::eSQL_Integer;
		else if (vDataType.Compare(_T("TEXT"), 4, CStringLiteral::cIgnoreCase + CStringLiteral::cLimited) == 0)
			vDataTypeEnum = CSqLite3Column::eSQL_Text;
		else if (vDataType.Compare(_T("FLOAT"), 5, CStringLiteral::cIgnoreCase + CStringLiteral::cLimited) == 0)
			vDataTypeEnum = CSqLite3Column::eSQL_Float;
		else if (vDataType.Compare(_T("BLOB"), 4, CStringLiteral::cIgnoreCase + CStringLiteral::cLimited) == 0)
			vDataTypeEnum = CSqLite3Column::eSQL_BLOB;

		CSqLite3Statement::CSqLite3Columns::Iterator it = cols.Index(iCol);
		Ptr(CSqLite3Column) pColumn = NULL;

		if (it)
		{
			pColumn = *it;
			pColumn->set_Name(vTmp);
		}
		else
		{
			pColumn = OK_NEW_OPERATOR CSqLite3Column(vTmp, vDataTypeEnum);
			cols.Append(pColumn);
		}
	}
	return;
}

bool CSqLite3StatementImpl::Fetch(Ref(CSqLite3Statement::CSqLite3Columns) cols)
{
	if ((!_lpEnv) || (!_lpConn) || (!_lpStmt) || (_siNumResultColumns <= 0))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::Fetch] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	int res = sqlite3_step(_lpStmt);

	if ((res != SQLITE_DONE) && (res != SQLITE_ROW))
		HandleError(__FILE__LINE__ res, _T("sqlite3_step"));

	sword iCol;

	for (iCol = 0; iCol < _siNumResultColumns; ++iCol)
	{
		CSqLite3Statement::CSqLite3Columns::Iterator it = cols.Index(iCol);

		if (it)
		{
			Ptr(CSqLite3Column) pColumn = *it;
			CByteBuffer bBuf;
			double vDoubleValue;
			sqword vIntegerValue;

			switch (pColumn->get_DataType())
			{
			case CSqLite3Column::eSQL_NULL:
				break;
			case CSqLite3Column::eSQL_BLOB:
				bBuf.set_Buffer(__FILE__LINE__ CastAny(BPointer, CastMutable(Pointer, sqlite3_column_blob(_lpStmt, iCol))), sqlite3_column_bytes(_lpStmt, iCol));
				break;
			case CSqLite3Column::eSQL_Float:
				vDoubleValue = sqlite3_column_double(_lpStmt, iCol);
				bBuf.set_Buffer(__FILE__LINE__ CastAny(BPointer, &vDoubleValue), sizeof(double));
				break;
			case CSqLite3Column::eSQL_Integer:
				vIntegerValue = sqlite3_column_int64(_lpStmt, iCol);
				bBuf.set_Buffer(__FILE__LINE__ CastAny(BPointer, &vIntegerValue), szsqword);
				break;
			case CSqLite3Column::eSQL_Text:
#ifdef UNICODE
				bBuf.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, sqlite3_column_text16(_lpStmt, iCol)), sqlite3_column_bytes16(_lpStmt, iCol));
#else
				bBuf.set_Buffer(__FILE__LINE__ sqlite3_column_text(_lpStmt, iCol), sqlite3_column_bytes(_lpStmt, iCol));
#endif
				break;
			}

			pColumn->set_Data(bBuf);
		}
	}

	if (res == SQLITE_DONE)
		return false;
	return true;
}

void CSqLite3StatementImpl::Close()
{
	if ((!_lpEnv) || (!_lpConn))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::Close] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	if (_lpStmt)
	{
		int res = sqlite3_reset(_lpStmt);

		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_reset"));
	}
}

void CSqLite3StatementImpl::Free()
{
	if ((!_lpEnv) || (!_lpConn))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3StatementImpl::Free] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
	Close();
	if (_lpStmt)
	{
		int res = sqlite3_finalize(_lpStmt);

		if (res != SQLITE_OK)
			HandleError(__FILE__LINE__ res, _T("sqlite3_finalize"));
		_lpStmt = NULL;
	}
	_siNumResultColumns = 0;
	_siNumResultRows = 0;
}

void CSqLite3StatementImpl::HandleError(DECL_FILE_LINE int err, CConstPointer funcName)
{
	const char * msg = sqlite3_errstr(err);
	CByteBuffer bBuf(__FILE__LINE__ CastAny(BConstPointer, msg), Cast(dword, strlen(msg)));
	CStringBuffer sBuf;

	sBuf.convertFromUTF8(bBuf, false);
	_lpEnv->set_LastError(OK_NEW_OPERATOR CSqLite3Exception(ARGS_FILE_LINE _T("%s failed (%s)"), funcName, sBuf.GetString()));
	throw _lpEnv->get_LastError();
}

CSqLite3ConnectionImpl::CSqLite3ConnectionImpl(CSqLite3EnvironmentImpl* lpEnv):
_lpEnv(lpEnv),
_lpDBHandle(NULL),
_bConnected(false),
_numStmts(0),
_cntStmts(0),
_funcList(__FILE__LINE__ CSqLite3FuncListDeleteFunc, NULL, CSqLite3FuncListSearchAndSortFunc)
{
}

CSqLite3ConnectionImpl::~CSqLite3ConnectionImpl(void)
{
	Close();
}

void CSqLite3ConnectionImpl::Open(CConstPointer _pszConnStr)
{
	if (!_lpEnv)
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3ConnectionImpl::Open] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

#ifdef UNICODE
	int res = sqlite3_open16(_pszConnStr, &_lpDBHandle);

	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_open16"));
#else
	int res = sqlite3_open(_pszConnStr, &_lpDBHandle);

	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_open"));
#endif

	_bConnected = TRUE;
}

CSqLite3StatementImpl* CSqLite3ConnectionImpl::create_Statement()
{
	if ((!_lpEnv) || (!_lpDBHandle) || (!_bConnected) || ((_numStmts > 0) && (_cntStmts >= _numStmts)))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3ConnectionImpl::create_Statement] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	++_cntStmts;
	return OK_NEW_OPERATOR CSqLite3StatementImpl(_lpEnv, this);
}

void CSqLite3ConnectionImpl::free_Statement(CSqLite3StatementImpl* pStmt)
{
	if (pStmt)
	{
		pStmt->release();
		--_cntStmts;
	}
}

void CSqLite3ConnectionImpl::Close()
{
	if (!_lpEnv)
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3ConnectionImpl::Close] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}

	int res = sqlite3_close(_lpDBHandle);

	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_close"));
	_lpDBHandle = NULL;
	_bConnected = FALSE;
}

static void xFunc_Implementation(sqlite3_context* context, int cnt, sqlite3_value** args)
{
	Ptr(CSqLite3Connection::create_function_infoclass) pImpl = CastAnyPtr(CSqLite3Connection::create_function_infoclass, sqlite3_user_data(context));
	CSqLite3Connection::CSqLite3Columns vColumns(pImpl->get_Args());

	for (int i = 0; i < cnt; ++i)
	{
		CSqLite3Connection::CSqLite3Columns::Iterator it = vColumns.Index(i+1);
		Ptr(CSqLite3Column) pColumn = *it;
		CByteBuffer bBuf;
		double vDouble;
		sqword vInteger;

		switch (pColumn->get_DataType())
		{
		case CSqLite3Column::eSQL_NULL:
			break;
		case CSqLite3Column::eSQL_BLOB:
			bBuf.set_Buffer(__FILE__LINE__ CastAnyPtr(byte, CastMutable(Pointer, sqlite3_value_blob(args[i]))), sqlite3_value_bytes(args[i]));
			break;
		case CSqLite3Column::eSQL_Float:
			vDouble = sqlite3_value_double(args[i]);
			bBuf.set_Buffer(__FILE__LINE__ CastAnyPtr(byte, &vDouble), sizeof(double));
			break;
		case CSqLite3Column::eSQL_Integer:
			vInteger = sqlite3_value_int64(args[i]);
			bBuf.set_Buffer(__FILE__LINE__ CastAnyPtr(byte, &vInteger), szsqword);
			break;
		case CSqLite3Column::eSQL_Text:
#ifdef UNICODE
			bBuf.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, sqlite3_value_text16(args[i])), sqlite3_value_bytes16(args[i]));
#else
			bBuf.set_Buffer(__FILE__LINE__ sqlite3_value_text(args[i]), sqlite3_value_bytes(args[i]));
#endif
			break;
		}
		pColumn->set_Data(bBuf);
	}
	pImpl->get_Func()(pImpl->get_DBConnection(), vColumns);

	CSqLite3Connection::CSqLite3Columns::Iterator it = vColumns.Index(0);
	Ptr(CSqLite3Column) pColumn = *it;
	double vDouble;
	sqword vInteger;

	switch (pColumn->get_DataType())
	{
	case CSqLite3Column::eSQL_NULL:
		sqlite3_result_null(context);
		break;
	case CSqLite3Column::eSQL_BLOB:
		sqlite3_result_blob(context, pColumn->get_Data().get_Buffer(), pColumn->get_Data().get_BufferSize(), SQLITE_STATIC);
		break;
	case CSqLite3Column::eSQL_Float:
		vDouble = DerefAnyPtr(double, pColumn->get_Data().get_Buffer());
		sqlite3_result_double(context, vDouble);
		break;
	case CSqLite3Column::eSQL_Integer:
		vInteger = DerefAnyPtr(sqword, pColumn->get_Data().get_Buffer());
		sqlite3_result_int64(context, vInteger);
		break;
	case CSqLite3Column::eSQL_Text:
#ifdef UNICODE
		sqlite3_result_text16(context, pColumn->get_Data().get_Buffer(), pColumn->get_Data().get_BufferSize(), SQLITE_STATIC);
#else
		sqlite3_result_text(context, CastAny(CConstPointer, pColumn->get_Data().get_Buffer()), pColumn->get_Data().get_BufferSize(), SQLITE_STATIC);
#endif
		break;
	}
}

void CSqLite3ConnectionImpl::create_function(Ptr(CSqLite3Connection::create_function_infoclass) pInfoClass)
{
	if ((!_lpEnv) || (!_lpDBHandle) || (!_bConnected))
	{
		Ptr(CSqLite3Exception) pEx = OK_NEW_OPERATOR CSqLite3Exception(__FILE__LINE__ _T("[CSqLite3ConnectionImpl::create_function] Invalid arguments or programming sequence error"));

		if (_lpEnv)
			_lpEnv->set_LastError(pEx);
		throw pEx;
	}
#ifdef UNICODE
	int res = sqlite3_create_function16(
		_lpDBHandle,
		CastAny(ConstPointer, pInfoClass->get_Name().GetString()),
		pInfoClass->get_NArgs(),
		SQLITE_UTF16,
		pInfoClass,
		xFunc_Implementation,
		NULL,
		NULL
		);
	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_create_function16"));
#else
	int res = sqlite3_create_function(
		_lpDBHandle,
		pInfoClass->get_Name().GetString(),
		pInfoClass->get_NArgs(),
		SQLITE_UTF8,
		pInfoClass,
		xFunc_Implementation,
		NULL,
		NULL
		);
	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_create_function"));
#endif
	_funcList.InsertSorted(pInfoClass);
}

void CSqLite3ConnectionImpl::HandleError(DECL_FILE_LINE int err, CConstPointer funcName)
{
	const char * msg = sqlite3_errstr(err);
	CByteBuffer bBuf(__FILE__LINE__ CastAny(BConstPointer, msg), Cast(dword, strlen(msg)));
	CStringBuffer sBuf;

	sBuf.convertFromUTF8(bBuf, false);
	_lpEnv->set_LastError(OK_NEW_OPERATOR CSqLite3Exception(ARGS_FILE_LINE _T("%s failed (%s)"), funcName, sBuf.GetString()));
	throw _lpEnv->get_LastError();
}

CSqLite3EnvironmentImpl::CSqLite3EnvironmentImpl(void) :
_lastError(NULL)
{
}


CSqLite3EnvironmentImpl::~CSqLite3EnvironmentImpl(void)
{
	Close();
	if (_lastError)
		delete _lastError;
	_lastError = NULL;
}

void CSqLite3EnvironmentImpl::set_LastError(CSqLite3Exception* pLastError)
{
	if (_lastError)
		delete _lastError;
	_lastError = pLastError;
}

CSqLite3ConnectionImpl* CSqLite3EnvironmentImpl::create_Connection()
{
	return OK_NEW_OPERATOR CSqLite3ConnectionImpl(this);
}

void CSqLite3EnvironmentImpl::Open()
{
	int res = SQLITE_OK;

	res = sqlite3_config(SQLITE_CONFIG_URI, 1);

	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_config"));

	res = sqlite3_initialize();

	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_initialize"));
}

void CSqLite3EnvironmentImpl::Close()
{
	int res = sqlite3_shutdown();

	if (res != SQLITE_OK)
		HandleError(__FILE__LINE__ res, _T("sqlite3_shutdown"));
}

void CSqLite3EnvironmentImpl::HandleError(DECL_FILE_LINE int err, CConstPointer funcName)
{
	const char * msg = sqlite3_errstr(err);
	CByteBuffer bBuf(__FILE__LINE__ CastAny(BConstPointer, msg), Cast(dword, strlen(msg)));
	CStringBuffer sBuf;

	sBuf.convertFromUTF8(bBuf, false);
	_lastError = OK_NEW_OPERATOR CSqLite3Exception(ARGS_FILE_LINE  _T("%s failed (%s)"), funcName, sBuf.GetString());
	throw _lastError;
}