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
#include "STRUTIL.H"

static const char* strtoll_cnvt = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static long long wcstoll_test(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix)
{
	long long result = 0;
	int state = 0;
	int factor = 1;
	int value;
	const char *p;
	long long vmax;
	int vrem;

	if (PtrCheck(_Str) || ((_Radix != 0) && ((_Radix < 2) || (_Radix > 36))))
	{
		errno = EINVAL;
		return 0;
	}
	while (*_Str)
	{
		switch (state)
		{
		case 0:
			if (s_isspace(*_Str))
			{
				++_Str;
				break;
			}
			if (*_Str == L'+')
			{
				factor = 1;
				++_Str;
				++state;
				break;
			}
			if (*_Str == L'-')
			{
				factor = -1;
				++_Str;
				++state;
				break;
			}
			++state;
		case 1:
			if (*_Str == L'0')
			{
				++_Str;
				if (m_toupper(*_Str) == L'X')
				{
					if (_Radix == 0)
						_Radix = 16;
					++_Str;
					++state;
					break;
				}
				if (_Radix == 0)
					_Radix = 8;
			}
			else if ((*_Str >= L'1') && (*_Str <= L'9'))
			{
				if (_Radix == 0)
					_Radix = 10;
			}
			else if (_Radix == 0)
			{
				errno = EINVAL;
				return 0;
			}
			++state;
		case 2:
			vmax = LLONG_MAX / _Radix;
			vrem = LLONG_MAX % _Radix;
			++state;
		case 3:
			p = strchr(strtoll_cnvt, m_toupper(*_Str));
			if (PtrCheck(p))
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(wchar_t, _Str);
				return result * factor;
			}
			value = Cast(int, p - strtoll_cnvt);
			if (value >= _Radix)
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(wchar_t, _Str);
				return result * factor;
			}
			if ((result > vmax) || ((result == vmax) && (value > vrem)))
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(wchar_t, _Str);
				errno = ERANGE;
				if (factor > 0)
					return LLONG_MAX;
				return LLONG_MIN;
			}
			result = (result * _Radix) + value;
			++_Str;
			break;
		default:
			break;
		}
	}
	if (_EndPtr)
		*_EndPtr = CastMutablePtr(wchar_t, _Str);
	return result * factor;
}

static long long strtoll_test(const char *_Str, char **_EndPtr, int _Radix)
{
	long long result = 0;
	int state = 0;
	int factor = 1;
	int value;
	const char *p;
	long long vmax;
	int vrem;

	if (PtrCheck(_Str) || ((_Radix != 0) && ((_Radix < 2) || (_Radix > 36))))
	{
		errno = EINVAL;
		return 0;
	}
	while (*_Str)
	{
		switch (state)
		{
		case 0:
			if (s_isspace(*_Str))
			{
				++_Str;
				break;
			}
			if (*_Str == '+')
			{
				factor = 1;
				++_Str;
				++state;
				break;
			}
			if (*_Str == '-')
			{
				factor = -1;
				++_Str;
				++state;
				break;
			}
			++state;
		case 1:
			if (*_Str == '0')
			{
				++_Str;
				if (m_toupper(*_Str) == 'X')
				{
					if (_Radix == 0)
						_Radix = 16;
					++_Str;
					++state;
					break;
				}
				if (_Radix == 0)
					_Radix = 8;
			}
			else if ((*_Str >= '1') && (*_Str <= '9'))
			{
				if (_Radix == 0)
					_Radix = 10;
			}
			else if (_Radix == 0)
			{
				errno = EINVAL;
				return 0;
			}
			++state;
		case 2:
			vmax = LLONG_MAX / _Radix;
			vrem = LLONG_MAX % _Radix;
			++state;
		case 3:
			p = strchr(strtoll_cnvt, m_toupper(*_Str));
			if (PtrCheck(p))
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(char, _Str);
				return result * factor;
			}
			value = Cast(int, p - strtoll_cnvt);
			if (value >= _Radix)
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(char, _Str);
				return result * factor;
			}
			if ((result > vmax) || ((result == vmax) && (value > vrem)))
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(char, _Str);
				errno = ERANGE;
				if (factor > 0)
					return LLONG_MAX;
				return LLONG_MIN;
			}
			result = (result * _Radix) + value;
			++_Str;
			break;
		default:
			break;
		}
	}
	if (_EndPtr)
		*_EndPtr = CastMutablePtr(char, _Str);
	return result * factor;
}

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
		vTestZahl = strtoll_test(TestZahlA[i].sZahl, &sZahlEndA, TestZahlA[i].vRadix);
		if (vTestZahl != TestZahlA[i].vZahl)
			WriteErrorTestFile(1, _T("vTestZahl=%lld != TestZahlA[i].vZahl=%lld"), vTestZahl, TestZahlA[i].vZahl);
	}
	for (dword i = 0; i < sizeof(TestZahlU) / sizeof(struct __tagTestZahlU); ++i)
	{
		vTestZahl = wcstoll_test(TestZahlU[i].sZahl, &sZahlEndU, TestZahlU[i].vRadix);
		if (vTestZahl != TestZahlU[i].vZahl)
			WriteErrorTestFile(1, _T("vTestZahl=%lld != TestZahlU[i].vZahl=%lld"), vTestZahl, TestZahlU[i].vZahl);
	}
	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestStrStr()
{
	OpenTestFile(_T("TestStrStr"));

	CPointer p = s_strnistr2(_T("CONSOURCES"), _T("ConSources"), 10);

	if ((p == nullptr) || (s_strcmp(p, _T("CONSOURCES"))))
		WriteErrorTestFile(1, _T("p=%s != CONSOURCES"), p);

	p = s_strnistr2(_T("CONSOURCESabcdef"), _T("ConSources"), 15);

	if ((p == nullptr) || (s_strcmp(p, _T("CONSOURCESabcdef"))))
		WriteErrorTestFile(1, _T("p=%s != CONSOURCESabcdef"), p);

	p = s_strnistr2(_T("abcdefCONSOURCES"), _T("ConSources"), 15);

	if ((p == nullptr) || (s_strcmp(p, _T("CONSOURCES"))))
		WriteErrorTestFile(1, _T("p=%s != CONSOURCES"), p);

	p = s_strnistr2(_T("abcdefCONSOURCESghijk"), _T("ConSources"), 20);

	if ((p == nullptr) || (s_strcmp(p, _T("CONSOURCESghijk"))))
		WriteErrorTestFile(1, _T("p=%s != CONSOURCESghijk"), p);

	WriteSuccessTestFile(1);
	CloseTestFile();
	return;
}

void TestCSources()
{
	COUT << _T("******************** TestStrUtil *********************\n") << endl;
	TestStrUtil();
	COUT << _T("******************** TestStrStr ****************************\n") << endl;
	TestStrStr();
}
