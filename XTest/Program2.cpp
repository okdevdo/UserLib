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
#include "strutil.h"
#include "utlptr.h"

struct __tagTestZahlU {
	const wchar_t*sZahl;
	long long vZahl;
	int vRadix;
} TestZahlU[] = {
	{
		L"  \t \n   123456789",
		123456789LL,
		0
	},
	{
		L"  \t \n   -123456789",
		-123456789LL,
		0
	},
	{
		L"9223372036854775808",
		9223372036854775807LL,
		0
	},
	{
		L"-9223372036854775806",
		-9223372036854775806LL,
		0
	},
	{
		L"0xABCD",
		0xABCD,
		0
	},
	{
		L"04567",
		04567,
		0
	},
	{
		L"01238",
		0123,
		0
	},
	{
		L"ABCD",
		0xABCD,
		16
	},
	{
		L"ZZZZ",
		1679615,
		36
	}
};

struct __tagTestZahlA {
	const char *sZahl;
	long long vZahl;
	int vRadix;
} TestZahlA[] = {
	{
		"  \t \n   123456789",
		123456789LL,
		0
	},
	{
		"  \t \n   -123456789",
		-123456789LL,
		0
	},
	{
		"9223372036854775808",
		9223372036854775807LL,
		0
	},
	{
		"-9223372036854775806",
		-9223372036854775806LL,
		0
	},
	{
		"0xABCD",
		0xABCD,
		0
	},
	{
		"04567",
		04567,
		0
	},
	{
		"01238",
		0123,
		0
	},
	{
		"ABCD",
		0xABCD,
		16
	},
	{
		"ZZZZ",
		1679615,
		36
	}
};

static void TestStrUtil()
{
	OpenTestFile(_T("TestStrUtil"));

	wchar_t *sZahlEndU = nullptr;
	char *sZahlEndA = nullptr;
	long long vTestZahl;

	for (dword i = 0; i < sizeof(TestZahlA) / sizeof(struct __tagTestZahlA); ++i)
	{
		vTestZahl = strtoll(TestZahlA[i].sZahl, &sZahlEndA, TestZahlA[i].vRadix);
		if (vTestZahl != TestZahlA[i].vZahl)
			WriteErrorTestFile(__FILE__, __LINE__, 1, _T("vTestZahl=%lld != TestZahlA[i].vZahl=%lld"), vTestZahl, TestZahlA[i].vZahl);
	}
	for (dword i = 0; i < sizeof(TestZahlU) / sizeof(struct __tagTestZahlU); ++i)
	{
		vTestZahl = wcstoll(TestZahlU[i].sZahl, &sZahlEndU, TestZahlU[i].vRadix);
		if (vTestZahl != TestZahlU[i].vZahl)
			WriteErrorTestFile(__FILE__, __LINE__, 1, _T("vTestZahl=%lld != TestZahlU[i].vZahl=%lld"), vTestZahl, TestZahlU[i].vZahl);
	}
	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestStrStr()
{
	OpenTestFile(_T("TestStrStr"));

	CPointer p = s_strnistr2(_T("CONSOURCES"), _T("ConSources"), 10);

	if ((p == nullptr) || (s_strcmp(p, _T("CONSOURCES"))))
		WriteErrorTestFile(__FILE__, __LINE__, 1, _T("p=%s != CONSOURCES"), p);

	p = s_strnistr2(_T("CONSOURCESabcdef"), _T("ConSources"), 15);

	if ((p == nullptr) || (s_strcmp(p, _T("CONSOURCESabcdef"))))
		WriteErrorTestFile(__FILE__, __LINE__, 1, _T("p=%s != CONSOURCESabcdef"), p);

	p = s_strnistr2(_T("abcdefCONSOURCES"), _T("ConSources"), 15);

	if ((p == nullptr) || (s_strcmp(p, _T("CONSOURCES"))))
		WriteErrorTestFile(__FILE__, __LINE__, 1, _T("p=%s != CONSOURCES"), p);

	p = s_strnistr2(_T("abcdefCONSOURCESghijk"), _T("ConSources"), 20);

	if ((p == nullptr) || (s_strcmp(p, _T("CONSOURCESghijk"))))
		WriteErrorTestFile(__FILE__, __LINE__, 1, _T("p=%s != CONSOURCESghijk"), p);

	WriteSuccessTestFile(1);
	CloseTestFile();
}

static sword __stdcall TestUtlPtr_Func(ConstPointer pA, ConstPointer pB, Pointer context)
{
	dword qa = CastAny(dword, pA);
	dword qb = CastAny(dword, pB);

	if (qa < qb)
		return -1;
	if (qa > qb)
		return 1;
	return 0;
}

static void TestUtlPtr()
{
	OpenTestFile(_T("TestUtlPtr"));

	ULongPointer numbers[64];
	ULongPointer testdata[16] = { 22, 33, 13, 89, 56, 23, 128, 55, 98, 45, 30, 248, 64, 5, 57, 152 };
	ULongPointer testdata1[16] = { 23, 32, 14, 88, 57, 22, 129, 54, 99, 44, 31, 250, 65, 3, 56, 153 };
	dword cnt = 0;
	ULongPointer data;
	sdword result;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			data = testdata[j];
			result = _lv_ubinsert(CastAny(Array, numbers), CastDWPointer(data), &cnt, TestUtlPtr_Func, NULL);

			_tprintf(_T("testvector: "));
			for (dword k = 0; k < cnt; ++k)
				_tprintf(_T("%ld "), numbers[k]);
			_tprintf(_T("\n"));
			_tprintf(_T("searches:\ntestdata insert search match\n"));
			for (int j = 0; j < 16; ++j)
			{
				data = testdata1[j];
				_tprintf(_T("%3ld "), data);
				result = _lv_ubsearch(CastAny(Array, numbers), CastDWPointer(data), cnt, TestUtlPtr_Func, NULL, UTLPTR_INSERTMODE);
				_tprintf(_T("%3ld "), result);
				result = _lv_ubsearch(CastAny(Array, numbers), CastDWPointer(data), cnt, TestUtlPtr_Func, NULL, UTLPTR_SEARCHMODE);
				_tprintf(_T("%3ld "), result);
				result = _lv_ubsearch(CastAny(Array, numbers), CastDWPointer(data), cnt, TestUtlPtr_Func, NULL, UTLPTR_MATCHMODE);
				_tprintf(_T("%3ld"), result);
				_tprintf(_T("\n"));
			}
		}
	}
	WriteSuccessTestFile(1);
	CloseTestFile();
}

void TestCSources()
{
	COUT << _T("******************** TestStrUtil *********************") << endl;
	COUT << _T("Tests 'strtoll' and 'wcstoll'.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestStrUtil();
	COUT << _T("******************** TestStrStr ****************************") << endl;
	COUT << _T("Tests 's_strnistr2'.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestStrStr();
	COUT << _T("******************** TestUtlPtr ****************************") << endl;
	COUT << _T("Tests '_lv_ubsearch'.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestUtlPtr();
}
