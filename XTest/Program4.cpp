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
#include "DataBase.h"
#include "DataVector.h"
#include "DBASE.H"
#include "DirectoryIterator.h"

#define TEST_DBF_FILE _T("C:\\Users\\Oliver\\Documents\\dbase_test.dbf")
#define TEST_DBT_FILE _T("C:\\Users\\Oliver\\Documents\\dbase_test.dbt")
#define TEST_MDX_FILE _T("C:\\Users\\Oliver\\Documents\\dbase_test.mdx")

static void TestDBaseCS0()
{
	dbasefile* _dbase;
	errno_t _result;

	if (CDirectoryIterator::FileExists(CFilePath(__FILE__LINE__ TEST_DBF_FILE)))
	{
		_result = dbasefile_open(TEST_DBF_FILE, &_dbase);
		assert(_result == 0);
	}
	else
	{
		_dbase = dbasefile_init(8 /*word fieldcnt*/, 163 /*word recordlen*/);
		assert(NotPtrCheck(_dbase));
		// 'C' Character, 'M' Memo, 'N' Numeric, 'F' Float, 'I' Index, 'L' Logical, 'D' Date, 'T' Time
		dbasefile_initfield(_dbase, 0, _T("TestIden"), 'I', 10, 0);
		dbasefile_initfield(_dbase, 1, _T("TestNum"), 'N', 10, 0);
		dbasefile_initfield(_dbase, 2, _T("TestFloat"), 'F', 10, 2);
		dbasefile_initfield(_dbase, 3, _T("TestChar"), 'C', 100, 0);
		dbasefile_initfield(_dbase, 4, _T("TestMemo"), 'M', 10, 0);
		dbasefile_initfield(_dbase, 5, _T("TestLogi"), 'L', 1, 0);
		dbasefile_initfield(_dbase, 6, _T("TestDate"), 'D', 8, 0);
		dbasefile_initfield(_dbase, 7, _T("TestTime"), 'T', 14, 0);
		_result = dbasefile_create(TEST_DBF_FILE, _dbase);
		assert(_result == 0);
	}

	sqword reccnt = dbasefile_getrecordcnt(_dbase);
	CPointer p;

	dbasefile_cleardata(_dbase);
	dbasefile_setfielddata_long(_dbase, 1, 1000);
	dbasefile_setfielddata_double(_dbase, 2, 1000.98);
	dbasefile_setfielddata_string(_dbase, 3, _T("Testdaten"), 9);
	dbasefile_setfielddata_long(_dbase, 4, 0);
	dbasefile_setfielddata_bool(_dbase, 5, true);
	dbasefile_setfielddata_string(_dbase, 6, _T("09012014"), 8);
	dbasefile_setfielddata_string(_dbase, 7, _T("09012014153000"), 14);

	_result = dbasefile_append(_dbase);
	assert(_result == 0);

	assert(dbasefile_getrecordcnt(_dbase) == Castdword(reccnt + 1));
	_result = dbasefile_read(_dbase, reccnt);
	assert(_result == 0);

	assert(Castsdword(1000) == dbasefile_getfielddata_long(_dbase, 1));
	assert(Cast(double, 1000.98) == dbasefile_getfielddata_double(_dbase, 2));
	p = dbasefile_getfielddata_string(_dbase, 3);
	assert(0 == s_strncmp(p, _T("Testdaten"), 9));
	TFfree(p);
	assert(Castsdword(0) == dbasefile_getfielddata_long(_dbase, 4));
	assert(true == dbasefile_getfielddata_bool(_dbase, 5));
	p = dbasefile_getfielddata_string(_dbase, 6);
	assert(0 == s_strncmp(p, _T("09012014"), 8));
	TFfree(p);
	p = dbasefile_getfielddata_string(_dbase, 7);
	assert(0 == s_strncmp(p, _T("09012014153000"), 14));
	TFfree(p);

	dbasefile_close(_dbase);
}

static void TestDbase0()
{
	bool bOpen = false;
	CDBase dbfile;
	CFilePath path(__FILE__LINE__ _T("C:\\Users\\Oliver\\Documents\\person.dbf"));
	CStringBuffer value;

	try
	{
		dbfile.Open(path);
		bOpen = true;
	}
	catch ( CDBaseException* ex )
	{
		dbfile.Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
	if ( !bOpen )
	{
		try
		{
			dbfile.Init(3, 90);
			dbfile.InitField(0, _T("Name"), 'C', 30, 0);
			dbfile.InitField(1, _T("Address"), 'C', 50, 0);
			dbfile.InitField(2, _T("Age"), 'N', 10, 0);
			dbfile.Create(path);

			dbfile.ClearData();
			value.SetString(__FILE__LINE__ _T("Bart Simpson"));
			dbfile.SetTextField(0, value);
			value.SetString(__FILE__LINE__ _T("Springfield"));
			dbfile.SetTextField(1, value);
			dbfile.SetNumericField(2, 12.0L);
			dbfile.Append();

			dbfile.ClearData();
			value.SetString(__FILE__LINE__ _T("Lisa Simpson"));
			dbfile.SetTextField(0, value);
			value.SetString(__FILE__LINE__ _T("Springfield"));
			dbfile.SetTextField(1, value);
			dbfile.SetNumericField(2, 10.0L);
			dbfile.Append();

			dbfile.Close();
		}
		catch ( CDBaseException* ex )
		{
			dbfile.Close();
			COUT << ex->GetExceptionMessage() << endl;
		}
	}
	try
	{
		if ( !bOpen )
			dbfile.Open(path);

		dbfile.Read(0);
		value = dbfile.GetTextField(0);
		_ASSERTE(value == CStringLiteral(_T("Bart Simpson")));
		value = dbfile.GetTextField(1);
		_ASSERTE(value == CStringLiteral(_T("Springfield")));

		dbfile.Read(1);
		value = dbfile.GetTextField(0);
		_ASSERTE(value == CStringLiteral(_T("Lisa Simpson")));
		value = dbfile.GetTextField(1);
		_ASSERTE(value == CStringLiteral(_T("Springfield")));

		dbfile.Close();
	}
	catch ( CDBaseException* ex )
	{
		dbfile.Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
}

static void TestDbase1()
{
	bool bOpen = false;
	CDBase dbfile;
	CFilePath path(__FILE__LINE__ _T("C:\\Users\\Oliver\\Documents\\personMemo.dbf"));
	CStringBuffer value;

	try
	{
		dbfile.Open(path);
		bOpen = true;
	}
	catch ( CDBaseException* ex )
	{
		dbfile.Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
	if ( !bOpen )
	{
		try
		{
			dbfile.Init(4, 100);
			dbfile.InitField(0, _T("Name"), 'C', 30, 0);
			dbfile.InitField(1, _T("Address"), 'C', 50, 0);
			dbfile.InitField(2, _T("Age"), 'N', 10, 0);
			dbfile.InitField(3, _T("Descrip"), 'M', 10, 0);
			dbfile.Create(path);

			dbfile.ClearData();
			value.SetString(__FILE__LINE__ _T("Bart Simpson"));
			dbfile.SetTextField(0, value);
			value.SetString(__FILE__LINE__ _T("Springfield"));
			dbfile.SetTextField(1, value);
			dbfile.SetNumericField(2, 12.0L);
			value.SetString(__FILE__LINE__ _T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text."));
			dbfile.SetTextField(3, value);
			dbfile.Append();

			dbfile.ClearData();
			value.SetString(__FILE__LINE__ _T("Lisa Simpson"));
			dbfile.SetTextField(0, value);
			value.SetString(__FILE__LINE__ _T("Springfield"));
			dbfile.SetTextField(1, value);
			dbfile.SetNumericField(2, 10.0L);
			value.SetString(__FILE__LINE__ _T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text."));
			dbfile.SetTextField(3, value);
			dbfile.Append();

			dbfile.Close();
		}
		catch ( CDBaseException* ex )
		{
			dbfile.Close();
			COUT << ex->GetExceptionMessage() << endl;
		}
	}
	try
	{
		if ( !bOpen )
			dbfile.Open(path);

		dbfile.Read(0);
		value = dbfile.GetTextField(0);
		_ASSERTE(value == CStringLiteral(_T("Bart Simpson")));
		value = dbfile.GetTextField(1);
		_ASSERTE(value == CStringLiteral(_T("Springfield")));
		value = dbfile.GetTextField(3);
		_ASSERTE(value == CStringLiteral(_T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text.")));

		dbfile.Read(1);
		value = dbfile.GetTextField(0);
		_ASSERTE(value == CStringLiteral(_T("Lisa Simpson")));
		value = dbfile.GetTextField(1);
		_ASSERTE(value == CStringLiteral(_T("Springfield")));
		value = dbfile.GetTextField(3);
		_ASSERTE(value == CStringLiteral(_T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text.")));

		dbfile.Close();
	}
	catch ( CDBaseException* ex )
	{
		dbfile.Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
}

static void TestDbase2()
{
	bool bOpen = false;
	CDBase dbfile;
	CFilePath path(__FILE__LINE__ _T("C:\\Users\\Oliver\\Documents\\personIndex.dbf"));
	CStringBuffer value;

	try
	{
		dbfile.Open(path);
		bOpen = true;
	}
	catch ( CDBaseException* ex )
	{
		dbfile.Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
	if ( !bOpen )
	{
		try
		{
			dbfile.Init(4, 100);
			dbfile.InitField(0, _T("Name"), 'C', 30, 0);
			dbfile.InitField(1, _T("Address"), 'C', 50, 0);
			dbfile.InitField(2, _T("Age"), 'N', 10, 0);
			dbfile.InitField(3, _T("Descrip"), 'M', 10, 0);
			dbfile.InitIndex(1);
			dbfile.InitIndexFields(0, 2);
			dbfile.InitIndexField(0, 0, 0, 30);
			dbfile.InitIndexField(0, 1, 1, 50);
			dbfile.Create(path);

			dbfile.ClearData();
			value.SetString(__FILE__LINE__ _T("Bart Simpson"));
			dbfile.SetTextField(0, value);
			value.SetString(__FILE__LINE__ _T("Springfield"));
			dbfile.SetTextField(1, value);
			dbfile.SetNumericField(2, 12.0L);
			value.SetString(__FILE__LINE__ _T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text."));
			dbfile.SetTextField(3, value);
			dbfile.Append();

			dbfile.ClearData();
			value.SetString(__FILE__LINE__ _T("Lisa Simpson"));
			dbfile.SetTextField(0, value);
			value.SetString(__FILE__LINE__ _T("Springfield"));
			dbfile.SetTextField(1, value);
			dbfile.SetNumericField(2, 10.0L);
			value.SetString(__FILE__LINE__ _T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text."));
			dbfile.SetTextField(3, value);
			dbfile.Append();

			dbfile.Close();
		}
		catch ( CDBaseException* ex )
		{
			dbfile.Close();
			COUT << ex->GetExceptionMessage() << endl;
		}
	}
	try
	{
		if ( !bOpen )
			dbfile.Open(path);

		dbfile.Read(0);
		value = dbfile.GetTextField(0);
		_ASSERTE(value == CStringLiteral(_T("Bart Simpson")));
		value = dbfile.GetTextField(1);
		_ASSERTE(value == CStringLiteral(_T("Springfield")));
		value = dbfile.GetTextField(3);
		_ASSERTE(value == CStringLiteral(_T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text.")));

		dbfile.Read(1);
		value = dbfile.GetTextField(0);
		_ASSERTE(value == CStringLiteral(_T("Lisa Simpson")));
		value = dbfile.GetTextField(1);
		_ASSERTE(value == CStringLiteral(_T("Springfield")));
		value = dbfile.GetTextField(3);
		_ASSERTE(value == CStringLiteral(_T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text.")));

		dbfile.ClearData();
		value.SetString(__FILE__LINE__ _T("Bart Simpson"));
		dbfile.SetTextField(0, value);
		value.SetString(__FILE__LINE__ _T("Springfield"));
		dbfile.SetTextField(1, value);
		_ASSERTE(dbfile.ReadIndex(0));
		value = dbfile.GetTextField(0);
		_ASSERTE(value == CStringLiteral(_T("Bart Simpson")));
		value = dbfile.GetTextField(1);
		_ASSERTE(value == CStringLiteral(_T("Springfield")));
		value = dbfile.GetTextField(3);
		_ASSERTE(value == CStringLiteral(_T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text.")));
		_ASSERTE(dbfile.NextIndex(0));
		value = dbfile.GetTextField(0);
		_ASSERTE(value == CStringLiteral(_T("Lisa Simpson")));
		value = dbfile.GetTextField(1);
		_ASSERTE(value == CStringLiteral(_T("Springfield")));
		value = dbfile.GetTextField(3);
		_ASSERTE(value == CStringLiteral(_T("Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text. Dies ist ein langer Text.")));
		_ASSERTE(!dbfile.NextIndex(0));

		dbfile.Close();
	}
	catch ( CDBaseException* ex )
	{
		dbfile.Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
}

static void TestDbase3()
{
	bool bOpen = false;
	CDBase dbfile;
#ifdef OK_SYS_WINDOWS
	CFilePath path(__FILE__LINE__ _T("C:\\Users\\Oliver\\Documents\\HumanResourcesEmployee.dbf"));
#endif
#ifdef OK_SYS_UNIX
	CFilePath path(__FILE__LINE__ _T("/home/Oliver/HumanResourcesEmployee.dbf"));
#endif
	CFilePath path2(path);
	CStringBuffer value;
	CStringBuffer value1;
	CDataVectorT<CStringBuffer> StringVector(__FILE__LINE__ 16, 16);
	CDataVectorT<CStringBuffer>::Iterator it;
	dword ix;

	path2.set_Extension(_T("csv"));
	try
	{
		dbfile.Open(path);
		bOpen = true;
	}
	catch ( CDBaseException* ex )
	{
		dbfile.Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
	if ( !bOpen )
	{
		try
		{
			dbfile.Init(16, 428);

			dbfile.InitField(0, _T("EmployeeID"), 'N', 10, 0);        // 10
			dbfile.InitField(1, _T("NationalIDNumber"), 'C', 15, 0);  // 25
			dbfile.InitField(2, _T("ContactID"), 'N', 10, 0);         // 35
			dbfile.InitField(3, _T("LoginID"), 'C', 255, 0);          // 290
			dbfile.InitField(4, _T("ManagerID"), 'N', 10, 0);         // 300
			dbfile.InitField(5, _T("Title"), 'C', 50, 0);             // 350
			dbfile.InitField(6, _T("BirthDate"), 'D', 8, 0);          // 358
			dbfile.InitField(7, _T("MaritalStatus"), 'C', 1, 0);      // 359
			dbfile.InitField(8, _T("Gender"), 'C', 1, 0);             // 360
			dbfile.InitField(9, _T("HireDate"), 'D', 8, 0);           // 368
			dbfile.InitField(10, _T("SalariedFlag"), 'L', 1, 0);      // 369
			dbfile.InitField(11, _T("VacationHours"), 'N', 5, 0);     // 374
			dbfile.InitField(12, _T("SickLeaveHours"), 'N', 5, 0);    // 379
			dbfile.InitField(13, _T("CurrentFlag"), 'L', 1, 0);       // 380
			dbfile.InitField(14, _T("rowguid"), 'C', 40, 0);          // 420
			dbfile.InitField(15, _T("ModifiedDate"), 'D', 8, 0);      // 428

			dbfile.InitIndex(2);
			dbfile.InitIndexFields(0, 1);
			dbfile.InitIndexField(0, 0, 1, 15);
			dbfile.InitIndexFields(1, 1);
			dbfile.InitIndexField(1, 0, 3, 255);
			dbfile.Create(path);

			dbfile.Import(path2, _T(";"), _T("\r\n"), true);

			dbfile.Close();
		}
		catch ( CDBaseException* ex )
		{
			dbfile.Close();
			COUT << ex->GetExceptionMessage() << endl;
		}
	}
	try
	{
		if ( !bOpen )
			dbfile.Open(path);

		for ( ix = 0; ix < dbfile.GetRecordCnt(); ++ix )
		{
			dbfile.Read(ix);
			value = dbfile.GetTextField(1); // NationalIDNumber
			StringVector.InsertSorted(value);
		}

		if ( dbfile.BeginIndex(0) )
		{
			it = StringVector.Begin();
			do
			{
				value = dbfile.GetTextField(1); // NationalIDNumber
				value1 = *it;
				COUT << _T("NationalIDNumber=") << value << _T(", ") << value1 << endl;
				_ASSERTE(value == value1);
				++it;
			} 
			while ( dbfile.NextIndex(0) );
		}

		dbfile.Close();
	}
	catch ( CDBaseException* ex )
	{
		dbfile.Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
}

void TestDbase()
{
	TestDBaseCS0();
	//TestDbase0();
	//TestDbase1();
	//TestDbase2();
	//TestDbase3();
}