/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#include "Program.h"
#include "Application.h"
#include "Configuration.h"
#include "SqLite3DB.h"
#include "MySQLDB.h"
#include "PostgresDB.h"

#ifdef OK_SYS_UNIX
#ifdef _UNICODE
#define _tprintf wprintf
#else
#define _tprintf printf
#endif
#endif

#ifdef OK_SYS_WINDOWS
#include "ODBC.h"

void TestSQL20()
{
	CStringBuffer tmp;
	CODBCEnvironment* env = NULL;
	CODBCConnection* connection = NULL;
	CODBCStatement* stmt = NULL;
	CConstPointer stmt1 = _T("USE [master];")
		_T("SET ANSI_NULLS ON;")
		_T("SET QUOTED_IDENTIFIER ON;")
		_T("DROP TABLE [okTest];")
		_T("CREATE TABLE [okTest](")
		_T("[EmployeeID] INT IDENTITY(1, 1) NOT NULL PRIMARY KEY,")
		_T("[EmployeeName] VARCHAR(100) NOT NULL,")
		_T("[EmployeeSalary] DOUBLE PRECISION NOT NULL,")
		_T("[Comment] VARCHAR (500) NULL")
		_T(");");
	CConstPointer stmt2 = _T("INSERT INTO [okTest] (EmployeeName, EmployeeSalary, Comment) VALUES (?, ?, ?);");
	CConstPointer stmt3 = _T("SELECT * FROM [okTest]");
	CConstPointer stmt41 = _T("IF OBJECT_ID ( 'uspProcTest', 'P' ) IS NOT NULL DROP PROCEDURE uspProcTest;");
	CConstPointer stmt42 = _T("CREATE PROCEDURE uspProcTest @EmplID INT, @EmpSal DOUBLE PRECISION OUTPUT")
		_T(" AS BEGIN; SET NOCOUNT ON; SET @EmpSal = (SELECT T.EmployeeSalary FROM [okTest] AS T WHERE T.EmployeeID = @EmplID); END;");
	CConstPointer stmt43 = _T("EXECUTE uspProcTest @EmplID = ?, @EmpSal = ? OUTPUT;");
	CConstPointer data1[] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	double data2[] = { 100.0, 200.0, 300.0, 400.0 };
	CConstPointer data3[] = { NULL, NULL, _T("TestComment"), NULL };

#if OK_CPU_64BIT
	mbchar data4[4][100] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	sqword data4len[4] = { CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString };
	double data5[4] = { 100.0, 200.0, 300.0, 400.0 };
	sqword data5len[4] = { 0, 0, 0, 0 };
	mbchar data6[4][500] = { _T(""), _T(""), _T("TestComment"), _T("") };
	sqword data6len[4] = { CODBCStatement::NullParameter, CODBCStatement::NullParameter, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullParameter };

	sdword pb_dword;
	sqword pb_dwordsize;
	mbchar pb_string[100];
	sqword pb_stringsize;
	mbchar pb_string2[500];
	sqword pb_string2size;
	double pb_double;
	sqword pb_doublesize;
#endif
#if OK_CPU_32BIT
	mbchar data4[4][100] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	sdword data4len[4] = { CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString };
	double data5[4] = { 100.0, 200.0, 300.0, 400.0 };
	sdword data5len[4] = { 0, 0, 0, 0 };
	mbchar data6[4][500] = { _T(""), _T(""), _T("TestComment"), _T("") };
	sdword data6len[4] = { CODBCStatement::NullParameter, CODBCStatement::NullParameter, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullParameter };

	sdword pb_dword;
	sdword pb_dwordsize;
	mbchar pb_string[100];
	sdword pb_stringsize;
	mbchar pb_string2[500];
	sdword pb_string2size;
	double pb_double;
	sdword pb_doublesize;
#endif
	try
	{
		tmp = theApp->config()->GetUserValue(_T("TestSQL.SQLServerODBC.DSN"), _T("okTest"));
		tmp.PrependString(_T("DSN="));

		env = OK_NEW_OPERATOR CODBCEnvironment();
		env->Open();
		connection = env->create_Connection();
		connection->Open(tmp);
		connection->set_AutoCommit(false);
		stmt = connection->create_Statement();

		stmt->Execute(stmt1);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt1, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());

		stmt->Prepare(stmt2);
		stmt->BindParameter(1, pb_string, 100 * szchar, &pb_stringsize);
		stmt->BindParameter(2, &pb_double);
		stmt->BindParameter(3, pb_string2, 500 * szchar, &pb_string2size);
		for (dword ix = 0; ix < (sizeof(data1) / sizeof(CConstPointer)); ++ix)
		{
			s_strcpy(pb_string, 100, data1[ix]);
			pb_stringsize = s_strlen(data1[ix], 100) * szchar;
			pb_double = data2[ix];
			if (data3[ix])
			{
				s_strcpy(pb_string2, 500, data3[ix]);
				pb_string2size = s_strlen(data3[ix], 500) * szchar;
			}
			else
			{
				*pb_string2 = 0;
				pb_string2size = CODBCStatement::NullParameter;
			}

			stmt->Execute();
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt2, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		connection->close_Transact();
		stmt->Prepare(stmt2, 4);
		stmt->BindParameter(1, CastAnyPtr(mbchar, data4), 100 * szchar, data4len);
		stmt->BindParameter(2, data5, data5len);
		stmt->BindParameter(3, CastAnyPtr(mbchar, data6), 500 * szchar, data6len);
		stmt->Execute();
		connection->close_Transact(false);

		stmt->Execute(stmt3);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt3, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns();

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
		{
			Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);

			_tprintf(_T("%s | "), pColumn->get_Name().GetString());
		}
		_tprintf(_T("\n"));
		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
		{
			Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);

			_tprintf(_T("%s | "), pColumn->get_DataTypeAsStr().GetString());
		}
		_tprintf(_T("\n"));
		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);
				CByteBuffer buf = pColumn->get_Data();

				if (pColumn->get_Ind() == CODBCColumn::NullValue)
				{
					_tprintf(_T("NULL | "));
					continue;
				}
				switch (ix)
				{
				case 0: // EmployeeID
					_tprintf(_T("%ld | "), DerefAnyPtr(sdword, buf.get_Buffer()));
					break;
				case 1: // EmployeeName
					_tprintf(_T("%s | "), CastAnyPtr(mbchar, buf.get_Buffer()));
					break;
				case 2: // EmployeeSalary
					_tprintf(_T("%f | "), DerefAnyPtr(double, buf.get_Buffer()));
					//COUT << pb_double;
					break;
				case 3: // Comment
					_tprintf(_T("%s | "), CastAnyPtr(mbchar, buf.get_Buffer()));
					break;
				default:
					break;
				}
			}
			_tprintf(_T("\n"));
		}

		connection->free_Statement(stmt);
		stmt = connection->create_Statement();

		stmt->Execute(stmt41);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt41, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->Execute(stmt42);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt42, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		connection->close_Transact();
		stmt->Prepare(stmt43);
		stmt->BindParameter(1, _T("@EmplID"), &pb_dword, &pb_dwordsize, CODBCStatement::eParamInput);
		stmt->BindParameter(2, _T("@EmpSal"), &pb_double, &pb_doublesize, CODBCStatement::eParamOutput);
		pb_dwordsize = sizeof(sdword);
		pb_dword = 1;
		pb_doublesize = sizeof(double);
		pb_double = 0.0;
		stmt->Execute();
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt43, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		_tprintf(_T("Output value = %f\n"), pb_double);
		//COUT << _T("Output value = ") << pb_double << endl;

		connection->free_Statement(stmt);
		stmt = NULL;
		connection->release();
		connection = NULL;
		env->Close();
		env->release();
	}
	catch (CODBCException* ex)
	{
		COUT << ex->GetString() << endl;
		if (stmt)
			connection->free_Statement(stmt);
		if (connection)
			connection->release();
		if (env)
		{
			env->Close();
			env->release();
		}
	}
}

void TestSQL21()
{
	CStringBuffer tmp;
	CODBCEnvironment* env = NULL;
	CODBCConnection* connection = NULL;
	CODBCStatement* stmt = NULL;
	CConstPointer stmt1[] = {
		_T("DROP TABLE IF EXISTS test.okTest;"),
		_T("CREATE TABLE test.okTest(") 
			_T("employeeID INT NOT NULL AUTO_INCREMENT PRIMARY KEY, ") 
			_T("employeeName VARCHAR(100) NOT NULL,")
			_T("employeeSalary DOUBLE NOT NULL,")
			_T("Comment VARCHAR(500) NULL);")
	};
	CConstPointer stmt2 = _T("INSERT INTO test.okTest (employeeName, employeeSalary, comment) VALUES (?, ?, ?);");
	CConstPointer stmt3 = _T("SELECT * FROM test.okTest");
	CConstPointer stmt41 = _T("DROP PROCEDURE IF EXISTS uspProcTest;");
	CConstPointer stmt42 = _T("CREATE PROCEDURE uspProcTest(IN EmplID INT, OUT EmpSal DOUBLE)")
		_T(" SELECT EmployeeSalary FROM okTest WHERE EmployeeID = EmplID INTO EmpSal;");
	CConstPointer stmt43 = _T("CALL uspProcTest(?, ?);");
	CConstPointer data1[] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	double data2[] = { 100.0, 200.0, 300.0, 400.0 };
	CConstPointer data3[] = { NULL, NULL, _T("TestComment"), NULL };
#if OK_CPU_64BIT
	mbchar data4[4][100] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	sqword data4len[4] = { CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString };
	double data5[4] = { 100.0, 200.0, 300.0, 400.0 };
	sqword data5len[4] = { 0, 0, 0, 0 };
	mbchar data6[4][500] = { _T(""), _T(""), _T("TestComment"), _T("") };
	sqword data6len[4] = { CODBCStatement::NullParameter, CODBCStatement::NullParameter, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullParameter };

	sdword pb_dword;
	mbchar pb_string[100];
	sqword pb_stringsize;
	mbchar pb_string2[500];
	sqword pb_string2size;
	double pb_double;
#endif
#if OK_CPU_32BIT
	mbchar data4[4][100] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	sdword data4len[4] = { CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString };
	double data5[4] = { 100.0, 200.0, 300.0, 400.0 };
	sdword data5len[4] = { 0, 0, 0, 0 };
	mbchar data6[4][500] = { _T(""), _T(""), _T("TestComment"), _T("") };
	sdword data6len[4] = { CODBCStatement::NullParameter, CODBCStatement::NullParameter, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullParameter };

	sdword pb_dword;
	mbchar pb_string[100];
	sdword pb_stringsize;
	mbchar pb_string2[500];
	sdword pb_string2size;
	double pb_double;
#endif

	try
	{
#if OK_CPU_32BIT
		tmp = theApp->config()->GetUserValue(_T("TestSQL.MySqlODBC.DSN32"), _T("myTest32"));
#endif
#if OK_CPU_64BIT
		tmp = theApp->config()->GetUserValue(_T("TestSQL.MySqlODBC.DSN64"), _T("myTest64"));
#endif
		tmp.PrependString(_T("DSN="));

		env = OK_NEW_OPERATOR CODBCEnvironment();
		env->Open();
		connection = env->create_Connection();
		connection->Open(tmp);
		connection->set_AutoCommit(false);
		stmt = connection->create_Statement();

		for (dword ix = 0; ix < (sizeof(stmt1) / sizeof(CConstPointer)); ++ix)
		{
			stmt->Execute(stmt1[ix]);
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt1[ix], Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		stmt->Close();

		stmt->Prepare(stmt2);
		stmt->BindParameter(1, pb_string, 100 * szchar, &pb_stringsize);
		stmt->BindParameter(2, &pb_double);
		stmt->BindParameter(3, pb_string2, 500 * szchar, &pb_string2size);
		for (dword ix = 0; ix < (sizeof(data1) / sizeof(CConstPointer)); ++ix)
		{
			s_strcpy(pb_string, 100, data1[ix]);
			pb_stringsize = s_strlen(data1[ix], INT_MAX) * szchar;
			pb_double = data2[ix];
			if (data3[ix])
			{
				s_strcpy(pb_string2, 500, data3[ix]);
				pb_string2size = s_strlen(data3[ix], INT_MAX) * szchar;
			}
			else
			{
				*pb_string2 = 0;
				pb_string2size = CODBCStatement::NullParameter;
			}
			stmt->Execute();
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt2, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		connection->close_Transact();
		stmt->Prepare(stmt2, 4);
		stmt->BindParameter(1, CastAnyPtr(mbchar, data4), 100 * szchar, data4len);
		stmt->BindParameter(2, data5, data5len);
		stmt->BindParameter(3, CastAnyPtr(mbchar, data6), 500 * szchar, data6len);
		stmt->Execute();
		connection->close_Transact(false);

		stmt->Execute(stmt3);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt3, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns();

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
		{
			Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);

			_tprintf(_T("%s | "), pColumn->get_Name().GetString());
		}
		_tprintf(_T("\n"));
		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
		{
			Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);

			_tprintf(_T("%s | "), pColumn->get_DataTypeAsStr().GetString());
		}
		_tprintf(_T("\n"));
		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);
				CByteBuffer buf = pColumn->get_Data();

				if (pColumn->get_Ind() == CODBCColumn::NullValue)
				{
					_tprintf(_T("NULL | "));
					continue;
				}
				switch (ix)
				{
				case 0: // EmployeeID
					_tprintf(_T("%ld | "), DerefAnyPtr(sdword, buf.get_Buffer()));
					break;
				case 1: // EmployeeName
					_tprintf(_T("%s | "), CastAnyPtr(mbchar, buf.get_Buffer()));
					break;
				case 2: // EmployeeSalary
					_tprintf(_T("%f | "), DerefAnyPtr(double, buf.get_Buffer()));
					//COUT << pb_double;
					break;
				case 3: // Comment
					_tprintf(_T("%s | "), CastAnyPtr(mbchar, buf.get_Buffer()));
					break;
				default:
					break;
				}
			}
			_tprintf(_T("\n"));
		}

		stmt->Execute(stmt41);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt41, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->Execute(stmt42);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt42, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		connection->close_Transact();
		stmt->Prepare(stmt43);
		stmt->BindParameter(1, &pb_dword);
		stmt->BindParameter(2, &pb_double, NULL, CODBCStatement::eParamOutput);
		pb_dword = 1;
		pb_double = 0.0;
		stmt->Execute();
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt43, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		_tprintf(_T("Output value = %f\n"), pb_double);

		connection->free_Statement(stmt);
		stmt = NULL;
		connection->release();
		connection = NULL;
		env->Close();
		env->release();
	}
	catch (CODBCException* ex)
	{
		COUT << ex->GetString() << endl;
		if (stmt)
			connection->free_Statement(stmt);
		if (connection)
			connection->release();
		if (env)
		{
			env->Close();
			env->release();
		}
	}
}

void TestSQL22()
{
	CStringBuffer tmp;
	CODBCEnvironment* env = NULL;
	CODBCConnection* connection = NULL;
	CODBCStatement* stmt = NULL;
	CConstPointer stmt1[] = {
		_T("DROP TABLE IF EXISTS okTest;"),
		_T("CREATE TABLE okTest(")
			_T("employeeID SERIAL NOT NULL PRIMARY KEY, ") 
			_T("employeeName VARCHAR(100) NOT NULL,")
			_T("employeeSalary DOUBLE PRECISION NOT NULL,")
			_T("Comment VARCHAR(500) NULL);")
	};
	CConstPointer stmt2 = _T("INSERT INTO okTest (employeeName, employeeSalary, Comment) VALUES (?, ?, ?);");
	CConstPointer stmt3 = _T("SELECT * FROM okTest;");
	CConstPointer stmt41 = _T("DROP FUNCTION IF EXISTS uspProcTest(INT);");
	CConstPointer stmt42 = _T("create function uspProcTest(P1 INT) returns double precision as $$ SELECT employeeSalary from okTest where employeeID = $1;$$ LANGUAGE SQL;");
	CConstPointer stmt43 = _T("select uspProcTest(?);");
	CConstPointer data1[] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	double data2[] = { 100.0, 200.0, 300.0, 400.0 };
	CConstPointer data3[] = { NULL, NULL, _T("TestComment"), NULL };
#if OK_CPU_64BIT
	mbchar data4[4][100] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	sqword data4len[4] = { CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString };
	double data5[4] = { 100.0, 200.0, 300.0, 400.0 };
	sqword data5len[4] = { 0, 0, 0, 0 };
	mbchar data6[4][500] = { _T(""), _T(""), _T("TestComment"), _T("") };
	sqword data6len[4] = { CODBCStatement::NullParameter, CODBCStatement::NullParameter, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullParameter };

	sdword pb_dword;
	mbchar pb_string[100];
	sqword pb_stringsize;
	mbchar pb_string2[500];
	sqword pb_string2size;
	double pb_double;
#endif
#if OK_CPU_32BIT
	mbchar data4[4][100] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	sdword data4len[4] = { CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString };
	double data5[4] = { 100.0, 200.0, 300.0, 400.0 };
	sdword data5len[4] = { 0, 0, 0, 0 };
	mbchar data6[4][500] = { _T(""), _T(""), _T("TestComment"), _T("") };
	sdword data6len[4] = { CODBCStatement::NullParameter, CODBCStatement::NullParameter, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullParameter };

	sdword pb_dword;
	mbchar pb_string[100];
	sdword pb_stringsize;
	mbchar pb_string2[500];
	sdword pb_string2size;
	double pb_double;
#endif

	try
	{
#if OK_CPU_32BIT
		tmp = theApp->config()->GetUserValue(_T("TestSQL.PostgreSQLODBC.DSN32"), _T("pqTest32"));
#endif
#if OK_CPU_64BIT
		tmp = theApp->config()->GetUserValue(_T("TestSQL.PostgreSQLODBC.DSN64"), _T("pqTest64"));
#endif
		tmp.PrependString(_T("DSN="));

		env = OK_NEW_OPERATOR CODBCEnvironment();
		env->Open();
		connection = env->create_Connection();
		connection->Open(tmp);
		connection->set_AutoCommit(false);
		stmt = connection->create_Statement();

		for (dword ix = 0; ix < (sizeof(stmt1) / sizeof(CConstPointer)); ++ix)
		{
			stmt->Execute(stmt1[ix]);
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt1[ix], Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}

		stmt->Prepare(stmt2);
		stmt->BindParameter(1, pb_string, 100 * szchar, &pb_stringsize);
		stmt->BindParameter(2, &pb_double);
		stmt->BindParameter(3, pb_string2, 500 * szchar, &pb_string2size);
		for (dword ix = 0; ix < (sizeof(data1) / sizeof(CConstPointer)); ++ix)
		{
			s_strcpy(pb_string, 100, data1[ix]);
			pb_stringsize = s_strlen(data1[ix], INT_MAX) * szchar;
			pb_double = data2[ix];
			if (data3[ix])
			{
				s_strcpy(pb_string2, 500, data3[ix]);
				pb_string2size = s_strlen(data3[ix], INT_MAX) * szchar;
			}
			else
			{
				*pb_string2 = 0;
				pb_string2size = CODBCStatement::NullParameter;
			}

			stmt->Execute();
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt2, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		connection->close_Transact();
		stmt->Prepare(stmt2, 4);
		stmt->BindParameter(1, CastAnyPtr(mbchar, data4), 100 * szchar, data4len);
		stmt->BindParameter(2, data5, data5len);
		stmt->BindParameter(3, CastAnyPtr(mbchar, data6), 500 * szchar, data6len);
		stmt->Execute();
		connection->close_Transact(false);

		stmt->Execute(stmt3);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt3, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns();

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
		{
			Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);

			_tprintf(_T("%s | "), pColumn->get_Name().GetString());
		}
		_tprintf(_T("\n"));
		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
		{
			Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);

			_tprintf(_T("%s | "), pColumn->get_DataTypeAsStr().GetString());
		}
		_tprintf(_T("\n"));
		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);
				CByteBuffer buf = pColumn->get_Data();

				if (pColumn->get_Ind() == CODBCColumn::NullValue)
				{
					_tprintf(_T("NULL | "));
					continue;
				}
				switch (ix)
				{
				case 0: // EmployeeID
					_tprintf(_T("%ld | "), DerefAnyPtr(sdword, buf.get_Buffer()));
					break;
				case 1: // EmployeeName
					_tprintf(_T("%s | "), CastAnyPtr(mbchar, buf.get_Buffer()));
					break;
				case 2: // EmployeeSalary
					_tprintf(_T("%f | "), DerefAnyPtr(double, buf.get_Buffer()));
					//COUT << pb_double;
					break;
				case 3: // Comment
					_tprintf(_T("%s | "), CastAnyPtr(mbchar, buf.get_Buffer()));
					break;
				default:
					break;
				}
			}
			_tprintf(_T("\n"));
		}

		connection->free_Statement(stmt);
		stmt = connection->create_Statement();

		stmt->Execute(stmt41);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt41, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->Execute(stmt42);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt42, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		connection->close_Transact();
		stmt->Prepare(stmt43);
		stmt->BindParameter(1, &pb_dword);
		pb_dword = 3;
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt43, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->FetchSingleValue(&pb_double);
		_tprintf(_T("Output value = %f\n"), pb_double);

		connection->free_Statement(stmt);
		stmt = NULL;
		connection->release();
		connection = NULL;
		env->Close();
		env->release();
	}
	catch (CODBCException* ex)
	{
		COUT << ex->GetString() << endl;
		if (stmt)
			connection->free_Statement(stmt);
		if (connection)
			connection->release();
		if (env)
		{
			env->Close();
			env->release();
		}
	}
}

void TestSQL23()
{
	CStringBuffer tmp;
	CODBCEnvironment* env = NULL;
	CODBCConnection* connection = NULL;
	CODBCStatement* stmt = NULL;
	CConstPointer stmt1 = _T("DROP TABLE IF EXISTS okTest;")
		_T("CREATE TABLE okTest(")
		_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
		_T("EmployeeName TEXT(100) NOT NULL,")
		_T("EmployeeSalary FLOAT NOT NULL,")
		_T("Comment TEXT(500) NULL")
		_T(");");
	CConstPointer stmt2 = _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES (?, ?, ?);");
	CConstPointer stmt3 = _T("SELECT * FROM okTest");
	CConstPointer data1[] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	double data2[] = { 100.0, 200.0, 300.0, 400.0 };
	CConstPointer data3[] = { NULL, NULL, _T("TestComment"), NULL };
#if OK_CPU_64BIT
	mbchar data5[4][100] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	sqword data5len[4] = { CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString };
	double data6[4] = { 100.0, 200.0, 300.0, 400.0 };
	sqword data6len[4] = { 0, 0, 0, 0 };
	mbchar data7[4][500] = { _T(""), _T(""), _T("TestComment"), _T("") };
	sqword data7len[4] = { CODBCStatement::NullParameter, CODBCStatement::NullParameter, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullParameter };

	mbchar pb_string[100];
	sqword pb_stringsize;
	mbchar pb_string2[500];
	sqword pb_string2size;
	double pb_double;
#endif
#if OK_CPU_32BIT
	mbchar data5[4][100] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	sdword data5len[4] = { CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullTerminatedParameterString };
	double data6[4] = { 100.0, 200.0, 300.0, 400.0 };
	sdword data6len[4] = { 0, 0, 0, 0 };
	mbchar data7[4][500] = { _T(""), _T(""), _T("TestComment"), _T("") };
	sdword data7len[4] = { CODBCStatement::NullParameter, CODBCStatement::NullParameter, CODBCStatement::NullTerminatedParameterString, CODBCStatement::NullParameter };

	mbchar pb_string[100];
	sdword pb_stringsize;
	mbchar pb_string2[500];
	sdword pb_string2size;
	double pb_double;
#endif

	try
	{
		tmp = theApp->config()->GetUserValue(_T("TestSQL.SqLite3ODBC.DSN"), _T("sqTest"));
		tmp.PrependString(_T("DSN="));

		env = OK_NEW_OPERATOR CODBCEnvironment();
		env->Open();
		connection = env->create_Connection();
		connection->Open(tmp);
		connection->set_AutoCommit(false);
		stmt = connection->create_Statement();

		stmt->Execute(stmt1);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt1, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());

		stmt->Prepare(stmt2);
		stmt->BindParameter(1, pb_string, 100, &pb_stringsize);
		stmt->BindParameter(2, &pb_double);
		stmt->BindParameter(3, pb_string2, 500, &pb_string2size);
		for (dword ix = 0; ix < (sizeof(data1) / sizeof(CConstPointer)); ++ix)
		{
			s_strcpy(pb_string, 100, data1[ix]);
			pb_stringsize = (s_strlen(data1[ix], 100) + 1) * szchar;
			pb_double = data2[ix];
			if (data3[ix])
			{
				s_strcpy(pb_string2, 500, data3[ix]);
				pb_string2size = (s_strlen(data3[ix], 500) + 1) * szchar;
			}
			else
			{
				*pb_string2 = 0;
				pb_string2size = CODBCStatement::NullParameter;
			}

			stmt->Execute();
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt2, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		connection->close_Transact();
		stmt->Prepare(stmt2, 4);
		stmt->BindParameter(1, CastAnyPtr(mbchar, data5), 100 * szchar, data5len);
		stmt->BindParameter(2, data6, data6len);
		stmt->BindParameter(3, CastAnyPtr(mbchar, data7), 500 * szchar, data7len);
		stmt->Execute();
		connection->close_Transact(false);

		stmt->Execute(stmt3);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt3, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns();

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
		{
			Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);

			_tprintf(_T("%s | "), pColumn->get_Name().GetString());
		}
		_tprintf(_T("\n"));
		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
		{
			Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);

			_tprintf(_T("%s | "), pColumn->get_DataTypeAsStr().GetString());
		}
		_tprintf(_T("\n"));

		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				Ptr(CODBCColumn) pColumn = stmt->get_ColumnInfo(ix);
				CByteBuffer buf = pColumn->get_Data();

				if (pColumn->get_Ind() == CODBCColumn::NullValue)
				{
					_tprintf(_T("NULL | "));
					continue;
				}
				switch (ix)
				{
				case 0: // EmployeeID
					_tprintf(_T("%ld | "), DerefAnyPtr(sdword, buf.get_Buffer()));
					break;
				case 1: // EmployeeName
					_tprintf(_T("%s | "), CastAnyPtr(mbchar, buf.get_Buffer()));
					break;
				case 2: // EmployeeSalary
					_tprintf(_T("%f | "), DerefAnyPtr(double, buf.get_Buffer()));
					//COUT << pb_double;
					break;
				case 3: // Comment
					_tprintf(_T("%s | "), CastAnyPtr(mbchar, buf.get_Buffer()));
					break;
				default:
					break;
				}
			}
			_tprintf(_T("\n"));
		}

		connection->close_Transact();
		connection->free_Statement(stmt);
		stmt = NULL;
		connection->release();
		connection = NULL;
		env->Close();
		env->release();
	}
	catch (CODBCException* ex)
	{
		COUT << ex->GetString() << endl;
		if (stmt)
			connection->free_Statement(stmt);
		if (connection)
			connection->release();
		if (env)
		{
			env->Close();
			env->release();
		}
	}
}
#endif

#if OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR
void TestSQL3()
{
	CStringBuffer tmp;
	CMySQLEnvironment* env = NULL;
	CMySQLConnection* connection = NULL;
	CMySQLStatement* stmt = NULL;
	CConstPointer stmt1[] = {
		_T("DROP TABLE IF EXISTS test.okTest;"),
		_T("CREATE TABLE test.okTest(")
			_T("employeeID INT NOT NULL AUTO_INCREMENT PRIMARY KEY, ")
			_T("employeeName VARCHAR(100) NOT NULL,")
			_T("employeeSalary DOUBLE NOT NULL,")
			_T("Comment VARCHAR(500) NULL);")
	};
	CConstPointer stmt2 = _T("INSERT INTO test.okTest (employeeName, employeeSalary, comment) VALUES (?, ?, ?);");
	CConstPointer stmt3 = _T("SELECT * FROM test.okTest");
	CConstPointer stmt41 = _T("DROP PROCEDURE IF EXISTS uspProcTest;");
	CConstPointer stmt42 = _T("CREATE PROCEDURE uspProcTest(IN EmplID INT, OUT EmpSal DOUBLE)")
		_T(" SELECT EmployeeSalary FROM okTest WHERE EmployeeID = EmplID INTO EmpSal;");
	CConstPointer stmt43 = _T("CALL uspProcTest(?, ?);");
	const char* data1[] = { "okreis", "okreis1", "okreis2", "okreis3" };
	double data2[] = { 100.0, 200.0, 300.0, 400.0 };
	const char* data3[] = { NULL, NULL, "TestComment", NULL };

	sdword pb_dword;
	byte pb_string[100];
	TUnixULong pb_stringsize;
	byte pb_string2[500];
	TUnixULong pb_string2size;
	double pb_double;

	try
	{
		tmp = theApp->config()->GetUserValue(_T("TestSQL.MySqlNative.ConnectionString"), _T("Host=127.0.0.1;User=root;Db=test"));

		env = OK_NEW_OPERATOR CMySQLEnvironment();
		env->Open();
		connection = env->create_Connection();
		connection->Open(tmp);
		connection->set_AutoCommit(false);
		stmt = connection->create_Statement();

		for (dword ix = 0; ix < (sizeof(stmt1) / sizeof(CConstPointer)); ++ix)
		{
			stmt->Execute(stmt1[ix]);
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt1[ix], Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		stmt->Close();
		connection->close_Transact();

		stmt->Prepare(stmt2);
		stmt->BindParameter(1, pb_string, &pb_stringsize);
		stmt->BindParameter(2, &pb_double);
		stmt->BindParameter(3, pb_string2, &pb_string2size);
		for (dword ix = 0; ix < (sizeof(data1) / sizeof(CConstPointer)); ++ix)
		{
			strcpy(CastAnyPtr(char, pb_string), data1[ix]);
			pb_stringsize = Cast(dword, strlen(CastAnyPtr(char, pb_string)));
			pb_double = data2[ix];
			if (data3[ix])
			{
				strcpy(CastAnyPtr(char, pb_string2), data3[ix]);
				pb_string2size = Cast(dword, strlen(CastAnyPtr(char, pb_string2)));
			}
			else
			{
				*pb_string2 = 0;
				pb_string2size = 0;
			}

			stmt->Execute();
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt2, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		stmt->Close();
		connection->close_Transact();

		stmt->Prepare(stmt3);
		stmt->Execute();
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt3, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns();

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_Name().GetString());
		_tprintf(_T("\n"));

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_DataTypeAsStr().GetString());
		_tprintf(_T("\n"));

		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				CByteBuffer buf = stmt->get_ColumnInfo(ix)->get_Data();

				switch (ix)
				{
				case 0:
					_tprintf(_T(" %ld |"), DerefDWPointer(buf.get_Buffer()));
					break;
				case 1:
					printf(" %.*s |", Cast(int, buf.get_BufferSize()), buf.get_Buffer());
					break;
				case 2:
					_tprintf(_T(" %f |"), DerefAnyPtr(double, buf.get_Buffer()));
					break;
				case 3:
					printf(" %.*s |", Cast(int, buf.get_BufferSize()), buf.get_Buffer());
					break;
				}
			}
			_tprintf(_T("\n"));
		}

		stmt->Execute(stmt41);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt41, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->Execute(stmt42);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt42, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->Close();
		connection->close_Transact();

		stmt->Prepare(stmt43);
		stmt->BindParameter(1, &pb_dword);
		stmt->BindParameter(2, &pb_double);
		pb_dword = 1;
		pb_double = 0.0;
		stmt->Execute();
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt43, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		do {
			stmt->BindColumns();

			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
				_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_Name().GetString());
			_tprintf(_T("\n"));

			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
				_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_DataTypeAsStr().GetString());
			_tprintf(_T("\n"));

			while (stmt->Fetch())
			{

				for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
				{
					CByteBuffer buf = stmt->get_ColumnInfo(ix)->get_Data();

					switch (ix)
					{
					case 0:
						_tprintf(_T(" %f |"), *CastAnyPtr(double, buf.get_Buffer()));
						break;
					}
				}
				_tprintf(_T("\n"));
			}
		} while (stmt->NextResult());

		connection->free_Statement(stmt);
		stmt = NULL;
		connection->close_Transact();
		connection->release();
		connection = NULL;
		env->Close();
		env->release();
	}
	catch (CMySQLException* ex)
	{
		COUT << ex->GetExceptionMessage() << endl;
		if (stmt)
			connection->free_Statement(stmt);
		if (connection)
			connection->release();
		if (env)
		{
			env->Close();
			env->release();
		}
	}
}
#endif

static void TestFunc(Ptr(CSqLite3Connection) pConn, ConstRef(CSqLite3Connection::CSqLite3Columns) pArgs)
{
	CSqLite3Connection::CSqLite3Columns::Iterator it0 = pArgs.Index(0);
	Ptr(CSqLite3Column) pResult = *it0;
	sqword vResult;
	CByteBuffer bBuf;
	CSqLite3Connection::CSqLite3Columns::Iterator it1 = pArgs.Index(1);
	Ptr(CSqLite3Column) pA = *it1;
	sqword vpA = DerefAnyPtr(sqword, pA->get_Data().get_Buffer());
	CSqLite3Connection::CSqLite3Columns::Iterator it2 = pArgs.Index(2);
	Ptr(CSqLite3Column) pB = *it2;
	sqword vpB = DerefAnyPtr(sqword, pB->get_Data().get_Buffer());

	vResult = vpA + vpB;
	bBuf.set_Buffer(__FILE__LINE__ CastAnyPtr(byte, &vResult), szsqword);
	pResult->set_Data(bBuf);
}

void TestSQL4()
{
	CStringBuffer tmp;
	CSqLite3Environment* env = NULL;
	CSqLite3Connection* connection = NULL;
	CSqLite3Statement* stmt = NULL;
	CConstPointer stmt1 = _T("DROP TABLE IF EXISTS okTest;")
		_T("CREATE TABLE okTest(")
		_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
		_T("EmployeeName TEXT(100) NOT NULL,")
		_T("EmployeeSalary FLOAT NOT NULL,")
		_T("Comment TEXT(500) NULL")
		_T(");");
	CConstPointer stmt2 = _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES (?, ?, ?);");
	CConstPointer stmt3 = _T("SELECT * FROM okTest;");
	CConstPointer stmt4 = _T("SELECT TestFunc(?,?);");
	CConstPointer stmt5 = _T("SELECT MIN(EmployeeSalary) + MAX(EmployeeSalary) AS SUMME FROM okTest;");
	CConstPointer data1[] = { _T("okreis"), _T("okreis1"), _T("okreis2"), _T("okreis3") };
	double data2[] = { 100.0, 200.0, 300.0, 400.0 };
	CConstPointer data3[] = { NULL, NULL, _T("TestComment"), NULL };
	try
	{
#ifdef OK_SYS_WINDOWS
		tmp = theApp->config()->GetUserValue(_T("TestSQL.SqLite3Native.URI"), _T("file:///C:/Users/Oliver/Documents/oktest.sqlite"));
#endif
#ifdef OK_SYS_UNIX
		tmp = theApp->config()->GetUserValue(_T("TestSQL.SqLite3Native.URI"), _T("file:///home/Oliver/oktest.sqlite"));
#endif

		env = OK_NEW_OPERATOR CSqLite3Environment();
		env->Open();
		connection = env->create_Connection();
		connection->Open(tmp);

		connection->create_function(_T("TestFunc"), TestFunc, 2, CSqLite3Column::eSQL_Integer, CSqLite3Column::eSQL_Integer, _T("pA"), CSqLite3Column::eSQL_Integer, _T("pB"));
		stmt = connection->create_Statement();

		stmt->Execute(stmt1);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt1, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->Close();

		stmt->Prepare(stmt2);
		for (dword ix = 0; ix < (sizeof(data1) / sizeof(CConstPointer)); ++ix)
		{
			stmt->BindParameter(1, data1[ix], s_strlen(data1[ix], 100) * szchar);
			stmt->BindParameter(2, data2[ix]);
			if (data3[ix])
				stmt->BindParameter(3, data3[ix], s_strlen(data3[ix], 500) * szchar);
			else
				stmt->BindParameter(3, NULL, 0);
			stmt->Execute();
			stmt->Close();
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt2, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}

		stmt->Execute(stmt3);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt3, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns();

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_Name().GetString());
		_tprintf(_T("\n"));

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_DataTypeAsStr().GetString());
		_tprintf(_T("\n"));

		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				CByteBuffer buf = stmt->get_ColumnInfo(ix)->get_Data();

				if (buf.get_BufferSize() == 0)
				{
					_tprintf(_T("NULL | "));
					continue;
				}
				switch (ix)
				{
				case 0:
					_tprintf(_T("%lld | "), DerefSQWPointer(buf.get_Buffer()));
					break;
				case 1:
					_tprintf(_T("%.*s | "), Cast(int, buf.get_BufferSize()), buf.get_Buffer());
					break;
				case 2:
					_tprintf(_T("%f | "), DerefAnyPtr(double, buf.get_Buffer()));
					break;
				case 3:
					_tprintf(_T("%.*s | "), Cast(int, buf.get_BufferSize()), buf.get_Buffer());
					break;
				}
			}
			_tprintf(_T("\n"));
		}

		stmt->Prepare(stmt4);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt4, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindParameter(1, Cast(sqword, 123));
		stmt->BindParameter(2, Cast(sqword, 5));
		stmt->Execute();
		stmt->BindColumns(1, CSqLite3Column::eSQL_Integer);

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_Name().GetString());
		_tprintf(_T("\n"));

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_DataTypeAsStr().GetString());
		_tprintf(_T("\n"));

		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				CByteBuffer buf = stmt->get_ColumnInfo(ix)->get_Data();

				if (buf.get_BufferSize() == 0)
				{
					_tprintf(_T("NULL | "));
					continue;
				}
				switch (ix)
				{
				case 0:
					_tprintf(_T("%lld | "), DerefSQWPointer(buf.get_Buffer()));
					break;
				}
			}
			_tprintf(_T("\n"));
		}

		stmt->Execute(stmt5);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt5, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns(1, CSqLite3Column::eSQL_Float);

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_Name().GetString());
		_tprintf(_T("\n"));

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_DataTypeAsStr().GetString());
		_tprintf(_T("\n"));

		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				CByteBuffer buf = stmt->get_ColumnInfo(ix)->get_Data();

				if (buf.get_BufferSize() == 0)
				{
					_tprintf(_T("NULL | "));
					continue;
				}
				switch (ix)
				{
				case 0:
					_tprintf(_T("%f | "), DerefAnyPtr(double, buf.get_Buffer()));
					break;
				}
			}
			_tprintf(_T("\n"));
		}

		connection->free_Statement(stmt);
		stmt = NULL;
		connection->release();
		connection = NULL;
		env->Close();
		env->release();
	}
	catch (CSqLite3Exception* ex)
	{
		COUT << ex->GetExceptionMessage() << endl;
		if (stmt)
			connection->free_Statement(stmt);
		if (connection)
			connection->release();
		if (env)
		{
			env->Close();
			env->release();
		}
	}
}

#if OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR
void TestSQL5()
{
	CStringBuffer tmp;
	CPostgresEnvironment* env = NULL;
	CPostgresConnection* connection = NULL;
	CPostgresStatement* stmt = NULL;
	CConstPointer stmt1[] = {
		_T("DROP TABLE IF EXISTS okTest;"),
		_T("CREATE TABLE okTest(")
			_T("employeeID SERIAL NOT NULL PRIMARY KEY, ") 
			_T("employeeName VARCHAR(100) NOT NULL,")
			_T("employeeSalary DOUBLE PRECISION NOT NULL,")
			_T("Comment VARCHAR(500) NULL);")
	};
	CConstPointer stmt2 = _T("INSERT INTO okTest (employeeName, employeeSalary, Comment) VALUES ($1, $2, $3);");
	CConstPointer stmt3 = _T("SELECT * FROM okTest");
	CConstPointer stmt41 = _T("DROP FUNCTION IF EXISTS uspProcTest(INT);");
	CConstPointer stmt42 = _T("create function uspProcTest(P1 INT) returns DOUBLE PRECISION as $$ SELECT employeeSalary from okTest where employeeID = $1;$$ LANGUAGE SQL;");
	CConstPointer stmt43 = _T("select uspProcTest($1);");
	const char* data2[] = { "okreis", "okreis1", "okreis2", "okreis3" };
	const char* data3[] = { "100.0", "200.0", "300.0", "400.0" };
	const char* data4[] = { NULL, NULL, "TestComment", NULL };
	try
	{
#if OK_CPU_32BIT
		tmp = theApp->config()->GetUserValue(_T("TestSQL.PostgreSQLNative.ConnectionString32"), _T("host=localhost port=5432 user=postgres password=ok14zu dbname=postgres"));
#endif
#if OK_CPU_64BIT
		tmp = theApp->config()->GetUserValue(_T("TestSQL.PostgreSQLNative.ConnectionString64"), _T("host=localhost port=5433 user=postgres password=ok14zu dbname=postgres"));
#endif

		env = OK_NEW_OPERATOR CPostgresEnvironment();
		env->Open();
		connection = env->create_Connection();
		connection->Open(tmp);
		stmt = connection->create_Statement();

		for (dword ix = 0; ix < (sizeof(stmt1) / sizeof(CConstPointer)); ++ix)
		{
			stmt->Execute(stmt1[ix]);
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt1[ix], Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		stmt->Close();

		stmt->Prepare(stmt2);
		for (dword ix = 0; ix < (sizeof(data2) / sizeof(char*)); ++ix)
		{
			stmt->BindParameter(1, data2[ix]);
			stmt->BindParameter(2, data3[ix]);
			stmt->BindParameter(3, data4[ix]);
			stmt->Execute();
			_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt2, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		}
		stmt->Close();

		stmt->Execute(stmt3);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt3, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns();

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_Name().GetString());
		_tprintf(_T("\n"));

		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				Ptr(CPostgresColumn) pColumn = stmt->get_ColumnInfo(ix);
				CByteBuffer buf = pColumn->get_Data();

				if (pColumn->get_isnull())
				{
					_tprintf(_T("NULL | "));
					continue;
				}
				printf("%.*s | ", Cast(int, buf.get_BufferSize()), buf.get_Buffer());
			}
			_tprintf(_T("\n"));
		}
		stmt->Close();
		stmt->Execute(stmt41);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt41, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->Execute(stmt42);
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt42, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->Close();
		stmt->Prepare(stmt43);
		stmt->BindParameter(1, "3");
		stmt->Execute();
		_tprintf(_T("%s, NumColumns=%d, NumRows=%lld\n"), stmt43, Cast(int, stmt->get_ColumnCount()), stmt->get_RowCount());
		stmt->BindColumns();

		for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			_tprintf(_T("%s | "), stmt->get_ColumnInfo(ix)->get_Name().GetString());
		_tprintf(_T("\n"));

		while (stmt->Fetch())
		{
			for (word ix = 0; ix < stmt->get_ColumnCount(); ++ix)
			{
				CByteBuffer buf = stmt->get_ColumnInfo(ix)->get_Data();

				printf(" %.*s |", Cast(int, buf.get_BufferSize()), buf.get_Buffer());
			}
			_tprintf(_T("\n"));
		}

		connection->free_Statement(stmt);
		stmt = NULL;
		connection->release();
		connection = NULL;
		env->Close();
		env->release();
	}
	catch (CPostgresException* ex)
	{
		COUT << ex->GetExceptionMessage() << endl;
		if (stmt)
			connection->free_Statement(stmt);
		if (connection)
			connection->release();
		if (env)
		{
			env->Close();
			env->release();
		}
	}
}
#endif

void TestSQL()
{
#ifdef OK_SYS_WINDOWS
	COUT << _T("TestSQL20: ODBC, SQLServer") << endl;
	TestSQL20();
	COUT << _T("TestSQL21: ODBC, MySQL") << endl;
	TestSQL21();
	COUT << _T("TestSQL22: ODBC, PostGreSQL") << endl;
	TestSQL22();
	COUT << _T("TestSQL23: ODBC, SqLite3") << endl;
	TestSQL23();
#endif
#if OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR
	COUT << _T("TestSQL3: Native, MySQL") << endl;
	TestSQL3();
#endif
	COUT << _T("TestSQL4: Native, SqLite3") << endl;
	TestSQL4();
#if OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR
	COUT << _T("TestSQL5: Native, PostGreSQL") << endl;
	TestSQL5();
#endif
}
