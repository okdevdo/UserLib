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
#include "StringBuffer.h"
#include "UTLPTR.H"

#include <stdio.h>
#include <errno.h>
#include <wchar.h>

#if (defined(OK_SYS_WINDOWS32) && defined(OK_COMP_GNUC)) || defined(OK_SYS_UNIX)
static void _set_errno(int errnum)
{
	errno = errnum;
}
#endif

static const char* _convertBase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#ifdef _UNICODE
static const wchar_t *_whitespace = L"\x0009\x000A\x000B\x000C\x000D\x0020\x0085\x00A0\x1680\x180E\x2000\x2001\x2002\x2003\x2004\x2005\x2006\x2007\x2008\x2009\x200A\x2028\x2029\x202F\x205F\x3000";
#else
static const char *_whitespace = "\x09\x0A\x0B\x0C\x0D\x20";
#endif

static bool IsWhitespace(mbchar pChar)
{
#ifdef _UNICODE
	return (s_strchr(CastMutablePtr(wchar_t,_whitespace), pChar) != NULL);
#else
	return (s_strchr(CastMutablePtr(char,_whitespace), pChar) != NULL);
#endif
}

static int scan_convertBase64(mbchar ch)
{
	char cc = (char)ch;
	const char* p = strchr(_convertBase64, cc);

	if ( p == NULL )
		return -1;
	return Castsdword(p - _convertBase64);
}

CStringBuffer CStringBuffer::_null;

ConstRef(CStringBuffer) CStringBuffer::null()
{
	return _null;
}

CPointer CStringBuffer::_alloc_Buffer(dword size)
{
#ifdef __DEBUG__
	CPointer buffer = (CPointer)(TFallocDbg(size * szchar, _file, _line)); // refcnt == 1
#else
	CPointer buffer = (CPointer)(TFalloc(size * szchar)); // refcnt == 1
#endif

	return buffer;
}

void CStringBuffer::_alloc_Buffer2(dword size, dword lenCh)
{
	CPointer buffer = _alloc_Buffer(size);

	if ( _string )
	{
		s_memcpy(buffer, _string, lenCh * szchar);
		_free_Buffer();
	}
	_string = buffer;
}

void CStringBuffer::_free_Buffer()
{
	if ( _string )
	{
		dword refCnt = TFdecrefcnt(_string);

		if ( !refCnt )
			TFfree(_string);
		_string = NULL;
	}
}

CStringBuffer::CStringBuffer() :
CStringLiteral()
#ifdef __DEBUG__
, _file(NULL), _line(0)
#endif
{
}

CStringBuffer::CStringBuffer(DECL_FILE_LINE dword initialsize) :
CStringLiteral()
#ifdef __DEBUG__
, _file(file), _line(line)
#endif
{
	SetSize(ARGS_FILE_LINE initialsize);
}

CStringBuffer::CStringBuffer(DECL_FILE_LINE mbchar fill, dword initialsize) :
    CStringLiteral()
#ifdef __DEBUG__
	, _file(file), _line(line)
#endif
{
	Fill(fill, initialsize);
}

CStringBuffer::CStringBuffer(DECL_FILE_LINE CConstPointer pText, int lench):
    CStringLiteral()
#ifdef __DEBUG__
	, _file(file), _line(line)
#endif
{
	SetString(ARGS_FILE_LINE pText, lench);
}

CStringBuffer::CStringBuffer(DECL_FILE_LINE CStringConstIterator _str):
    CStringLiteral()
#ifdef __DEBUG__
	, _file(file), _line(line)
#endif
{
	SetString(ARGS_FILE_LINE _str.GetCurrent(), _str.GetLength());
}

CStringBuffer::CStringBuffer(DECL_FILE_LINE CStringLiteral _str):
    CStringLiteral()
#ifdef __DEBUG__
	, _file(file), _line(line)
#endif
{
	SetString(ARGS_FILE_LINE _str.GetString(), _str.GetLength());
}

#ifdef QT_VERSION
CStringBuffer::CStringBuffer( ConstRef(QString) _str):
    CStringLiteral()
#ifdef __DEBUG__
, _file(NULL), _line(0)
#endif
{
    SetString(_str);
}
#endif

CStringBuffer::CStringBuffer(const CStringBuffer& copy):
    CStringLiteral()
#ifdef __DEBUG__
	, _file(copy._file), _line(copy._line)
#endif
{
	_string = copy._string;
	addRef();
}

CStringBuffer::~CStringBuffer(void)
{
	_free_Buffer();
}

void CStringBuffer::operator = (ConstRef(CStringBuffer) copy)
{
	SetString(copy);
}

#ifdef QT_VERSION
void CStringBuffer::operator = (const QString& _str)
{
	SetString(_str);
}
#endif

void CStringBuffer::operator += (mbchar cc)
{
	mbchar buf[2];

	buf[0] = cc;
	buf[1] = 0;
	AppendString(buf, 1);
}

void CStringBuffer::operator+=(CConstPointer pText)
{
	AppendString(pText);
}

void CStringBuffer::operator += ( ConstRef(CStringConstIterator) _str )
{
	AppendString(_str.GetCurrent());
}

void CStringBuffer::operator += ( ConstRef(CStringLiteral) _str )
{
	AppendString(_str.GetString());
}

void CStringBuffer::operator += ( ConstRef(CStringBuffer) copy )
{
	AppendString(copy.GetString());
}

#ifdef QT_VERSION
void CStringBuffer::operator += ( ConstRef(QString) _str)
{
    if ( _str.isEmpty() )
        return;
    AppendString(CastAnyPtr(mbchar, CastMutablePtr(ushort, _str.utf16())), _str.count());
}
#endif

void CStringBuffer::Fill(mbchar fill, dword initialsize)
{
	CPointer text;
#ifdef __DEBUG__
	if (!_file)
		_file = __FILE__;
	if (!_line)
		_line = __LINE__;
	SetSize(_file, _line, initialsize + 1);
#else
	SetSize(initialsize + 1);
#endif
	text = _string;
	for ( ; initialsize > 0; --initialsize, ++text )
		*text = fill;
	*text = 0;
}

void CStringBuffer::SetSize(DECL_FILE_LINE dword reserve)
{
#ifdef __DEBUG__
	_file = file;
	_line = line;
#endif
	if (reserve == 0)
	{
		_free_Buffer();
		return;
	}

	dword len = 0;
	dword refCnt = 0;

	if ( _string )
	{
		len = Castdword(TFsize(_string));
		refCnt = TFrefcnt(_string);
	}
	if ( ((reserve * szchar) < len) && (refCnt < 2) )
		return;

	dword lench = 0;

	if ( _string )
		lench = GetSize();
	_alloc_Buffer2(reserve, Min(reserve - 1, lench));
}

void CStringBuffer::Clear()
{
	_free_Buffer();
}

void CStringBuffer::SetString(DECL_FILE_LINE CConstPointer pText, int lench)
{
#ifdef __DEBUG__
	_file = file;
	_line = line;
#endif
	if (pText && (lench < 0))
		lench = s_strlen(CastMutable(CPointer, pText), INT_MAX);
	if ( (!pText) || (lench == 0) )
	{
		_free_Buffer();
		return;
	}
	++lench;
	if ( _string )
	{
		dword len = Castdword(TFsize(_string));
		dword refCnt = TFrefcnt(_string);

		if ( ((lench * szchar) > len) || (refCnt > 1) )
			_free_Buffer();
	}
	if ( !_string )
		_string = _alloc_Buffer(lench);
	if ( _string )
	{
		s_strncpy(_string, lench, CastMutable(CPointer, pText), lench - 1);
		*(_string + (lench - 1)) = 0;
	}
}

void CStringBuffer::SetString(DECL_FILE_LINE ConstRef(CStringConstIterator) _str)
{
	SetString(ARGS_FILE_LINE _str.GetCurrent(), _str.GetLength());
}

void CStringBuffer::SetString(DECL_FILE_LINE ConstRef(CStringLiteral) _str)
{
	SetString(ARGS_FILE_LINE _str.GetString(), _str.GetLength());
}

void CStringBuffer::SetString(const CStringBuffer& copy)
{
	if (PtrCheck(copy.GetString()))
	{
		_free_Buffer();
		return;
	}
	if ( copy.GetString() != GetString() )
	{
		_free_Buffer();
		_string = CastMutable(CPointer, copy.GetString());
		addRef();
	}
}

#ifdef QT_VERSION
void CStringBuffer::SetString(ConstRef(QString) str)
{
    if ( str.isEmpty() )
    {
        _free_Buffer();
        return;
    }
    SetString(CastAnyPtr(mbchar, CastMutablePtr(ushort, str.utf16())), str.count());
}
#endif

void CStringBuffer::AppendString(CConstPointer pText, int lench)
{
    if ( ConstStrEmpty(pText) || (lench == 0) )
        return;
	if ( lench < 0 )
		lench = s_strlen(CastMutable(CPointer, pText), INT_MAX);
	if ( lench > 0 )
	{
		dword lench1 = GetLength();
		dword len = 0;
		dword refCnt = 0;

		if ( _string )
		{
			len = Castdword(TFsize(_string));
			refCnt = TFrefcnt(_string);
		}
		if ( (((lench1 + lench + 1) * szchar) > len) || (refCnt > 1) )
			_alloc_Buffer2(lench1 + lench + 1, lench1 + 1);
		if ( _string )
		{
			s_strncpy(_string + lench1, lench + 1, CastMutable(CPointer, pText), lench);
			*(_string + (lench1 + lench)) = 0;
		}
	}
}

void CStringBuffer::PrependString(CConstPointer pText, int lench)
{
	InsertString(0, pText, lench);
}

void CStringBuffer::InsertString(dword pos, CConstPointer pText, int lench)
{
    if ( ConstStrEmpty(pText) || (lench == 0) )
        return;

    dword lench1 = GetLength();

	if ( pos >= lench1 )
	{
		AppendString(pText, lench);
		return;
	}
	if ( lench < 0 )
		lench = s_strlen(CastMutable(CPointer, pText), INT_MAX);
	if ( lench > 0 )
	{
		dword len = StrEmpty(_string)?0:(Castdword(TFsize(_string)));
		dword refCnt = TFrefcnt(_string);

		if ( (((lench1 + lench + 1) * szchar) > len) || (refCnt > 1) )
			_alloc_Buffer2(lench1 + lench + 1, lench1 + 1);
		if ( _string )
		{
			s_memmove(_string + pos + lench, _string + pos, (lench1 - pos + 1) * szchar);
			s_memcpy(_string + pos, CastMutable(CPointer, pText), lench * szchar);
		}
	}
}

void CStringBuffer::DeleteString(dword pos, dword len)
{
	if ( StrEmpty(_string) || (len == 0) )
		return;

	dword lench = GetLength();
	dword refCnt = TFrefcnt(_string);

	if ( (pos + len) >= lench )
	{
		if ( pos >= lench )
			return;
		if ( refCnt > 1 )
			_alloc_Buffer2(pos + 1, pos);
		*(_string + pos) = 0;
		return;
	}
	if ( refCnt > 1 )
	{
		CPointer buffer = _alloc_Buffer(lench - len + 1);

		if ( pos )
			s_memcpy(buffer, _string, pos * szchar);
		s_memcpy(buffer + pos, _string + pos + len, (lench - pos - len + 1) * szchar);
		_free_Buffer();
		_string = buffer;
		return;
	}
	s_memcpy(_string + pos, _string + pos + len, (lench - pos - len + 1) * szchar);
}

sdword CStringBuffer::FormatString(DECL_FILE_LINE CConstPointer format, ...)
{
	if (ConstStrEmpty(format))
	{
		_set_errno(EINVAL);
		return -1;
	}

    va_list argList;
	sdword ret;

    va_start(argList, format);
	ret = FormatString(ARGS_FILE_LINE format, argList);
	va_end(argList);
	return ret;
}

sdword CStringBuffer::FormatString(DECL_FILE_LINE CConstPointer format, va_list argList)
{
	if (ConstStrEmpty(format))
	{
		_set_errno(EINVAL);
		return -1;
	}

	mbchar buffer[MAX_FORMATTEXT];
	sdword ret;

	s_memset(buffer, 0, MAX_FORMATTEXT * szchar);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	if (0 > (ret = _vsntprintf(buffer, MAX_FORMATTEXT, CastMutable(CPointer, format), argList)))
#endif
#ifdef OK_SYS_UNIX
	if (0 > (ret = vsnprintf(buffer, MAX_FORMATTEXT, CastMutable(CPointer, format), argList)))
#endif
#endif
#ifdef OK_COMP_MSC
	if (0 > (ret = _vstprintf_s(buffer, MAX_FORMATTEXT, CastMutable(CPointer, format), argList)))
#endif
	{
		SetSize(ARGS_FILE_LINE 0);
		return ret;
	}
	
	SetString(ARGS_FILE_LINE buffer);
	return ret;
}

sdword CStringBuffer::ScanString(CConstPointer format, ...) const
{
	if (StrEmpty(_string))
	{
		_set_errno(EINVAL);
		return -1;
	}

	if (ConstStrEmpty(format))
	{
		_set_errno(EINVAL);
		return -1;
	}

	va_list argList;
	sdword ret;

	va_start(argList, format);
	ret = ScanString(format, argList);
	va_end(argList);

	return ret;
}


sdword CStringBuffer::ScanString(CConstPointer format, va_list argList) const
{
	if ( StrEmpty(_string) )
	{
		_set_errno(EINVAL);
		return -1;
	}

	if (ConstStrEmpty(format))
	{
		_set_errno(EINVAL);
		return -1;
	}

	CStringConstIterator sIt = format;
	CPointer pS = _string;
	CPointer pS1;
	CPointer pSValue;
	dword fldcnt = 0;
	dword chcnt = 0;
	word state = 0;
	long lvalue;
	long long llvalue;
	float fvalue;
	double dvalue;
	word fieldwidth = 0;
	sword fieldwidth2 = 0;
	mbchar svchar;
	bool donotstore = false;
	bool bshortvalue = false;
	bool blongvalue = false;
	bool blonglongvalue = false;

	while (!(sIt.IsEnd()))
	{
		switch (sIt[0])
		{
		case _T('%'):
			if (state != 0)
			{
				_set_errno(EINVAL);
				return -1;
			}
			if (sIt[1] == _T('%'))
			{
				++sIt;
				if (*pS != sIt[0])
				{
					_set_errno(EINVAL);
					return -1;
				}
				pS++;
				++chcnt;
			}
			else
			{
				fieldwidth = 0;
				donotstore = false;
				bshortvalue = false;
				blongvalue = false;
				blonglongvalue = false;
				fldcnt++;
				state = 1;
			}
			break;
		case _T('\\'):
			if (state != 0)
			{
				_set_errno(EINVAL);
				return -1;
			}
			++sIt;
			if (*pS != sIt[0])
				return -1;
			pS++;
			++chcnt;
			break;
		default:
			switch (state)
			{
			case 0:
				if (s_isspace(sIt[0]))
				{
					while (s_isspace(sIt[1])) ++sIt;
					while (s_isspace(*pS)) { ++pS; chcnt++; }
					break;
				}
				if (*pS != sIt[0])
				{
					_set_errno(EINVAL);
					return -1;
				}
				pS++;
				chcnt++;
				break;
			case 1:
				switch (sIt[0])
				{
				case _T('n'):
					*(va_arg(argList, Ptr(int))) = Cast(int, chcnt);
					break;
				case _T('e'):
				case _T('f'):
				case _T('g'):
					if (fieldwidth)
					{
						svchar = *(pS + fieldwidth);
						*(pS + fieldwidth) = 0;
					}
					if (blonglongvalue)
						dvalue = s_strtod(pS, &pSValue);
					else if (blongvalue)
						dvalue = s_strtod(pS, &pSValue);
					else
						fvalue = (float)s_strtod(pS, &pSValue);
					if (fieldwidth)
						*(pS + fieldwidth) = svchar;
					chcnt += Cast(dword, (pSValue - pS));
					pS = pSValue;
					if (!donotstore)
					{
						if (blongvalue)
							*(va_arg(argList, Ptr(double))) = dvalue;
						else if (blonglongvalue)
							*(va_arg(argList, Ptr(long double))) = dvalue;
						else
							*(va_arg(argList, Ptr(float))) = fvalue;
					}
					state = 0;
					break;
				case _T('i'):
					if (fieldwidth)
					{
						svchar = *(pS + fieldwidth);
						*(pS + fieldwidth) = 0;
					}
					if (blonglongvalue)
						llvalue = s_strtoll(pS, &pSValue, 0);
					else
						lvalue = s_strtol(pS, &pSValue, 0);
					if (fieldwidth)
						*(pS + fieldwidth) = svchar;
					chcnt += Cast(dword, (pSValue - pS));
					pS = pSValue;
					if (!donotstore)
					{
						if (bshortvalue)
							*(va_arg(argList, Ptr(short int))) = Cast(short int, lvalue);
						else if (blongvalue)
							*(va_arg(argList, Ptr(long int))) = lvalue;
						else if (blonglongvalue)
							*(va_arg(argList, Ptr(long long int))) = llvalue;
						else
							*(va_arg(argList, Ptr(int))) = lvalue;
					}
					state = 0;
					break;
				case _T('d'):
					if (fieldwidth)
					{
						svchar = *(pS + fieldwidth);
						*(pS + fieldwidth) = 0;
					}
					if (blonglongvalue)
						llvalue = s_strtoll(pS, &pSValue, 10);
					else
						lvalue = s_strtol(pS, &pSValue, 10);
					if (fieldwidth)
						*(pS + fieldwidth) = svchar;
					chcnt += Cast(dword, (pSValue - pS));
					pS = pSValue;
					if (!donotstore)
					{
						if (bshortvalue)
							*(va_arg(argList, Ptr(short int))) = Cast(short int, lvalue);
						else if (blongvalue)
							*(va_arg(argList, Ptr(long int))) = lvalue;
						else if (blonglongvalue)
							*(va_arg(argList, Ptr(long long int))) = llvalue;
						else
							*(va_arg(argList, Ptr(int))) = lvalue;
					}
					state = 0;
					break;
				case _T('u'):
					if (fieldwidth)
					{
						svchar = *(pS + fieldwidth);
						*(pS + fieldwidth) = 0;
					}
					if (blonglongvalue)
						llvalue = s_strtoull(pS, &pSValue, 10);
					else
						lvalue = s_strtoul(pS, &pSValue, 10);
					if (fieldwidth)
						*(pS + fieldwidth) = svchar;
					chcnt += Cast(dword, (pSValue - pS));
					pS = pSValue;
					if (!donotstore)
					{
						if (bshortvalue)
							*(va_arg(argList, Ptr(unsigned short int))) = Cast(short int, lvalue);
						else if (blongvalue)
							*(va_arg(argList, Ptr(unsigned long int))) = lvalue;
						else if (blonglongvalue)
							*(va_arg(argList, Ptr(unsigned long long int))) = llvalue;
						else
							*(va_arg(argList, Ptr(unsigned int))) = lvalue;
					}
					state = 0;
					break;
				case _T('o'):
					if (fieldwidth)
					{
						svchar = *(pS + fieldwidth);
						*(pS + fieldwidth) = 0;
					}
					if (blonglongvalue)
						llvalue = s_strtoll(pS, &pSValue, 8);
					else
						lvalue = s_strtol(pS, &pSValue, 8);
					if (fieldwidth)
						*(pS + fieldwidth) = svchar;
					chcnt += Cast(dword, (pSValue - pS));
					pS = pSValue;
					if (!donotstore)
					{
						if (bshortvalue)
							*(va_arg(argList, Ptr(short int))) = Cast(short int, lvalue);
						else if (blongvalue)
							*(va_arg(argList, Ptr(long int))) = lvalue;
						else if (blonglongvalue)
							*(va_arg(argList, Ptr(long long int))) = llvalue;
						else
							*(va_arg(argList, Ptr(int))) = lvalue;
					}
					state = 0;
					break;
				case _T('x'):
					if (fieldwidth)
					{
						svchar = *(pS + fieldwidth);
						*(pS + fieldwidth) = 0;
					}
					if (blonglongvalue)
						llvalue = s_strtoll(pS, &pSValue,16);
					else
						lvalue = s_strtol(pS, &pSValue, 16);
					if (fieldwidth)
						*(pS + fieldwidth) = svchar;
					chcnt += Cast(dword, (pSValue - pS));
					pS = pSValue;
					if (!donotstore)
					{
						if (bshortvalue)
							*(va_arg(argList, Ptr(short int))) = Cast(short int, lvalue);
						else if (blongvalue)
							*(va_arg(argList, Ptr(long int))) = lvalue;
						else if (blonglongvalue)
							*(va_arg(argList, Ptr(long long int))) = llvalue;
						else
							*(va_arg(argList, Ptr(int))) = lvalue;
					}
					state = 0;
					break;
				case _T('s'):
					pSValue = va_arg(argList, CPointer);
					lvalue = va_arg(argList, unsigned int);
					pS1 = pS;
					fieldwidth2 = fieldwidth;
					if (fieldwidth2)
						while ((*pS1) && (!(s_isspace(*pS1))) && (fieldwidth2--)) ++pS1;
					else
						while ((*pS1) && (!(s_isspace(*pS1)))) ++pS1;
					if (!donotstore)
					{
#ifdef UNICODE
						if (bshortvalue)
						{
							CStringBuffer t(__FILE__LINE__ pS, Cast(dword, pS1 - pS));
							CByteBuffer b;

							t.convertToByteBuffer(b);
							strncpy((char *)pSValue, (const char *)(b.get_Buffer()), Min(Cast(dword, lvalue), b.get_BufferSize()));
							fieldwidth2 = (lvalue <= Cast(sdword, b.get_BufferSize())) ? Castsword(lvalue-1) : Castsword(b.get_BufferSize());
							DerefAnyPtr(char, _l_ptradd(pSValue, fieldwidth2)) = 0;
						}
						else
						{
							fieldwidth2 = (lvalue <= Cast(sdword, pS1 - pS)) ? Castsword(lvalue-1) : Castsword(pS1 - pS);
							s_strncpy(pSValue, lvalue, pS, pS1 - pS);
							DerefAnyPtr(wchar_t, _l_ptradd(pSValue, fieldwidth2 * szchar)) = 0;
						}
#else
						if (blongvalue)
						{
							CStringBuffer t(__FILE__LINE__ pS, Cast(dword, pS1 - pS));
							CByteBuffer b;

							t.convertToUTF16(b, true, false);
							wcsncpy((wchar_t *)pSValue, (const wchar_t *)(b.get_Buffer()), Min(Cast(dword, lvalue), b.get_BufferSize()));
							fieldwidth2 = (lvalue <= Cast(sdword, b.get_BufferSize())) ? Castsword(lvalue-1) : Castsword(b.get_BufferSize());
							DerefAnyPtr(wchar_t, _l_ptradd(pSValue, fieldwidth2 * szchar)) = 0;
						}
						else
						{
							s_strncpy(pSValue, lvalue, pS, pS1 - pS);
							fieldwidth2 = (lvalue <= Cast(sdword, pS1 - pS)) ? Castsword(lvalue-1) : Castsword(pS1 - pS);
							DerefAnyPtr(char, _l_ptradd(pSValue, fieldwidth2)) = 0;
						}
#endif
					}
					if (lvalue < Cast(sdword, pS1 - pS))
					{
						chcnt += lvalue;
						pS += lvalue;
					}
					else
					{
						chcnt += Cast(sdword, pS1 - pS);
						pS = pS1;
					}
					state = 0;
					break;
				case _T('c'):
					pSValue = va_arg(argList, CPointer);
					lvalue = va_arg(argList, unsigned int);
					pS1 = pS;
					fieldwidth2 = fieldwidth;
					if (fieldwidth2)
						while ((*pS1) && (fieldwidth2--)) ++pS1;
					else
						while (*pS1) ++pS1;
					if (!donotstore)
					{
#ifdef UNICODE
						if (bshortvalue)
						{
							CStringBuffer t(__FILE__LINE__ pS, Cast(dword, pS1 - pS));
							CByteBuffer b;

							t.convertToByteBuffer(b);
							memmove((char *)pSValue, (const char *)(b.get_Buffer()), Min(Cast(dword, lvalue), b.get_BufferSize()));
						}
						else
							wmemmove(pSValue, pS, Min(lvalue, pS1 - pS));
#else
						if (blongvalue)
						{
							CStringBuffer t(__FILE__LINE__ pS, Cast(dword, pS1 - pS));
							CByteBuffer b;

							t.convertToUTF16(b, true, false);
							wmemmove((wchar_t *)pSValue, (const wchar_t *)(b.get_Buffer()), Min(Cast(dword, lvalue), b.get_BufferSize()));
						}
						else
							memmove(pSValue, pS, Min(lvalue, pS1 - pS));
#endif
					}
					if (lvalue < Cast(sdword, pS1 - pS))
					{
						chcnt += lvalue;
						pS += lvalue;
					}
					else
					{
						chcnt += Cast(sdword, pS1 - pS);
						pS = pS1;
					}
					state = 0;
					break;
				case _T('0'):
				case _T('1'):
				case _T('2'):
				case _T('3'):
				case _T('4'):
				case _T('5'):
				case _T('6'):
				case _T('7'):
				case _T('8'):
				case _T('9'):
					if (bshortvalue || blongvalue || blonglongvalue)
					{
						_set_errno(EINVAL);
						return -1;
					}
					fieldwidth = (10 * fieldwidth) + (sIt[0] - _T('0'));
					break;
				case _T('*'):
					if ((fieldwidth != 0) || bshortvalue || blongvalue || blonglongvalue)
					{
						_set_errno(EINVAL);
						return -1;
					}
					donotstore = true;
					break;
				case _T('h'):
					bshortvalue = true;
					break;
				case _T('L'):
					blonglongvalue = true;
					break;
				case _T('l'):
					if (blongvalue)
					{
						blonglongvalue = true;
						blongvalue = false;
					}
					else
						blongvalue = true;
					break;
				default:
					_set_errno(EINVAL);
					return -1;
					break;
				}
				break;
			}
			break;
		}
		++sIt;
	}
	return chcnt;
}

void CStringBuffer::ReplaceString(CConstPointer str, CConstPointer pText, int lench)
{
	if ( lench < 0 )
		lench = (pText == NULL)?0:(s_strlen(CastMutable(CPointer, pText), INT_MAX));

	dword lenStr = s_strlen(CastMutable(CPointer, str), INT_MAX);
	CPointer string1 = _string;
	CPointer string2 = s_strstr(string1, str);
	dword pos;

	if ( PtrCheck(string2) )
		return;
	pos = Castdword(string2 - string1);
	while ( pos >= 0 )
	{
		DeleteString(pos, lenStr);
		if ( pText && lench )
		{
			InsertString(pos, pText, lench);
			pos += lench;
		}
		string1 = _string + pos;
		string2 = s_strstr(string1, str);
		if ( PtrCheck(string2) )
			break;
		pos += Castdword(string2 - string1);
	}
}

void CStringBuffer::SubString(dword startpos, dword chLen, CStringBuffer& result) const
{
	if ( StrEmpty(_string) || (chLen == 0) )
	{
		result.Clear();
		return;
	}

	dword lench = GetLength();

	if ( startpos >= lench )
	{
		result.Clear();
		return;
	}
	if ( (startpos + chLen) >= lench )
	{
		result.SetString(__FILE__LINE__ _string + startpos, lench - startpos);
		return;
	}
	result.SetString(__FILE__LINE__ _string + startpos, chLen);
}

void CStringBuffer::Trim()
{
	if ( StrEmpty(_string) )
		return;

	int pos = 0;
	CPointer text = _string;

	while ( (*text) && IsWhitespace(*text) )
	{
		++pos;
		++text;
	}
	DeleteString(0, pos);

	dword lench = GetLength();

	if ( !lench )
		return;

	text = _string + (lench - 1);
	pos = 0;

	while ( (text != _string) && IsWhitespace(*text) )
	{
		++pos;
		--text;
	}
	DeleteString(lench - pos, pos);
}

void CStringBuffer::ToLowerCase()
{
    if ( StrEmpty(_string) )
        return;
    if ( TFrefcnt(_string) > 1 )
    {
        dword len = GetSize();

        _alloc_Buffer2(len, len);
    }

    CPointer text = _string;

	while ( *text )
	{
		if ( s_isupper(*text) )
			*text += 0x0020;
		++text;
	}
}

void CStringBuffer::ToUpperCase()
{
    if ( StrEmpty(_string) )
        return;
    if ( TFrefcnt(_string) > 1 )
    {
        dword len = GetSize();

        _alloc_Buffer2(len, len);
    }

    CPointer text = _string;

	while ( *text )
	{
		if ( s_islower(*text) )
			*text -= 0x0020;
		++text;
	}
}

void CStringBuffer::Split(CConstPointer delimiter, CArray output, dword maxoutputsize, Ptr(dword) outputsize)
{
    if ( ConstStrEmpty(delimiter) || (0 == output) || (maxoutputsize == 0) || StrEmpty(_string) )
	{
		if ( outputsize )
			*outputsize = 0;
		return;
	}
    if ( TFrefcnt(_string) > 1 )
    {
        dword len = GetSize();

        _alloc_Buffer2(len, len);
    }
	s_strsplit(_string, delimiter, output, maxoutputsize, outputsize);
}

void CStringBuffer::SplitAny(CConstPointer delimiter, CArray output, dword maxoutputsize, Ptr(dword) outputsize)
{
    if ( ConstStrEmpty(delimiter) || (0 == output) || (maxoutputsize == 0) || StrEmpty(_string) )
    {
        if ( outputsize )
            *outputsize = 0;
        return;
    }
    if ( TFrefcnt(_string) > 1 )
    {
        dword len = GetSize();

        _alloc_Buffer2(len, len);
    }
	s_strsplitany(_string, delimiter, output, maxoutputsize, outputsize);
}

void CStringBuffer::SplitQuoted(CConstPointer quote, CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize)
{
    if ( ConstStrEmpty(delimiter) || ConstStrEmpty(quote) || (0 == output) || (maxoutputsize == 0) || StrEmpty(_string) )
    {
        if ( outputsize )
            *outputsize = 0;
        return;
    }
    if ( TFrefcnt(_string) > 1 )
    {
        dword len = GetSize();

        _alloc_Buffer2(len, len);
    }
	s_strsplitquoted(_string, quote, delimiter, output, maxoutputsize, outputsize);
}

void CStringBuffer::SplitQuoted(CConstPointer recorddelimiter, CConstPointer fieldquote, CConstPointer fielddelimiter, TSplitFunc output, Pointer outputcontext)
{
	if (ConstStrEmpty(recorddelimiter) || ConstStrEmpty(fieldquote) || ConstStrEmpty(fielddelimiter) || (NULL == output) || StrEmpty(_string))
        return;
    if ( TFrefcnt(_string) > 1 )
    {
        dword len = GetSize();

        _alloc_Buffer2(len, len);
    }
	s_strsplitquoted2(_string, recorddelimiter, fieldquote, fielddelimiter, output, outputcontext);
}

void CStringBuffer::convertToBase64(ConstRef(CByteBuffer) buffer)
{
	dword bufferSize = ((buffer.get_BufferSize() + 2) / 3) * 4;
	dword x;

    SetSize(__FILE__LINE__ bufferSize + 5);

	CPointer textBuffer = (CPointer)_string;
	BPointer byteBuffer = buffer.get_Buffer();

	bufferSize = (buffer.get_BufferSize() + 2) / 3;
	for ( dword ix = 0; ix < bufferSize; ++ix )
	{
		x = 0;

		x = *byteBuffer++;
		x = x << 8;
		x += *byteBuffer++;
		x = x << 8;
		x += *byteBuffer++;

		textBuffer += 3;
		*textBuffer-- = *(_convertBase64 + (x & 0x3F));
		x = x >> 6;
		*textBuffer-- = *(_convertBase64 + (x & 0x3F));
		x = x >> 6;
		*textBuffer-- = *(_convertBase64 + (x & 0x3F));
		x = x >> 6;
		*textBuffer = *(_convertBase64 + (x & 0x3F));
		textBuffer += 4;
	}
	switch ( buffer.get_BufferSize() % 3 )
	{
	case 2:
		x = 0;

		x = *byteBuffer++;
		x = x << 8;
		x += *byteBuffer;
		x = x << 8;

		textBuffer += 3;
		*textBuffer-- = _T('=');
		x = x >> 6;
		*textBuffer-- = *(_convertBase64 + (x & 0x3F));
		x = x >> 6;
		*textBuffer-- = *(_convertBase64 + (x & 0x3F));
		x = x >> 6;
		*textBuffer = *(_convertBase64 + (x & 0x3F));
		textBuffer += 4;
		break;
	case 1:
		x = 0;

		x = *byteBuffer;
		x = x << 8;
		x = x << 8;

		textBuffer += 3;
		*textBuffer-- = _T('=');
		x = x >> 6;
		*textBuffer-- = _T('=');
		x = x >> 6;
		*textBuffer-- = *(_convertBase64 + (x & 0x3F));
		x = x >> 6;
		*textBuffer = *(_convertBase64 + (x & 0x3F));
		textBuffer += 4;
		break;
	case 0:
		break;
	}
	*textBuffer = 0;
}

void CStringBuffer::convertFromBase64(Ref(CByteBuffer) buffer) const
{
	dword lench = GetLength();
	dword bufferSize = ((lench + 3) / 4) * 3;
	dword bufferCnt = 0;

	buffer.set_BufferSize(__FILE__LINE__ bufferSize);

	CConstPointer textBuffer = _string;
	BPointer byteBuffer = buffer.get_Buffer();

	bufferSize = lench / 4;
	for ( dword ix = 0; ix < bufferSize; ++ix )
	{
		dword x = 0;
		int pos = 0;
		int d = 3;

		x = scan_convertBase64(*textBuffer++);
		x = x << 6;
		x += scan_convertBase64(*textBuffer++);
		x = x << 6;
		pos = scan_convertBase64(*textBuffer++);
		if ( pos < 0 )
			--d;
		else
			x += pos;
		x = x << 6;
		pos = scan_convertBase64(*textBuffer++);
		if ( pos < 0 )
			--d;
		else
			x += pos;

		switch ( d )
		{
		case 3:
			byteBuffer += 2;
			*byteBuffer-- = x & 0xFF;
			x = x >> 8;
			*byteBuffer-- = x & 0xFF;
			x = x >> 8;
			*byteBuffer = x & 0xFF;
			byteBuffer += 3;
			bufferCnt += 3;
			break;
		case 2:
			byteBuffer += 1;
			x = x >> 8;
			*byteBuffer-- = x & 0xFF;
			x = x >> 8;
			*byteBuffer = x & 0xFF;
			byteBuffer += 2;
			bufferCnt += 2;
			break;
		case 1:
			x = x >> 8;
			x = x >> 8;
			*byteBuffer++ = x & 0xFF;
			bufferCnt += 1;
			break;
		}
	}
	buffer.set_BufferSize(__FILE__LINE__ bufferCnt);
}

void CStringBuffer::convertToByteBuffer(Ref(CByteBuffer) buffer) const
{
	dword bufferSize = GetLength();

	buffer.set_BufferSize(__FILE__LINE__ bufferSize);

	CConstPointer textbuffer = _string;
	BPointer byteBuffer = buffer.get_Buffer();

	for ( dword ix = 0; ix < bufferSize; ++ix )
		*byteBuffer++ = (byte)(*textbuffer++);
}

void CStringBuffer::convertFromByteBuffer(ConstRef(CByteBuffer) buffer)
{
	dword bufferSize = buffer.get_BufferSize();

	SetSize(__FILE__LINE__ bufferSize + 1);

	CPointer textbuffer = (CPointer)_string;
	BPointer byteBuffer = buffer.get_Buffer();

	for ( dword ix = 0; ix < bufferSize; ++ix )
		*textbuffer++ = (mbchar)(*byteBuffer++);
	*textbuffer = 0;
}

void CStringBuffer::convertToUTF8(Ref(CByteBuffer) buffer, bool hasBOM) const
{
	dword bufferSize = GetLength();
	dword bufferSize2 = hasBOM?3:0;
	CConstPointer textbuffer = _string;
	
	for ( dword ix = 0; ix < bufferSize; ++ix )
	{
		if ( *textbuffer <= 0x007F )
			bufferSize2 += 1;
		else if ( *textbuffer <= 0x07FF )
			bufferSize2 += 2;
		else
			bufferSize2 += 3;
		++textbuffer;
	}
	buffer.set_BufferSize(__FILE__LINE__ bufferSize2);

	BPointer byteBuffer = buffer.get_Buffer();

	if (hasBOM)
	{
		*byteBuffer++ = 0xEF;
		*byteBuffer++ = 0xBB;
		*byteBuffer++ = 0xBF;
	}

	textbuffer = _string;
	for ( dword ix = 0; ix < bufferSize; ++ix )
	{
		if ( *textbuffer <= 0x007F )
			*byteBuffer++ = (byte)(*textbuffer);
		else if ( *textbuffer <= 0x07FF )
		{
			byte loByte = _Lowbyte(*textbuffer);
			byte hiByte = _Highbyte(*textbuffer);

			*byteBuffer++ = ((loByte >> 6) & 0x03) | ((hiByte << 2) & 0x1C) | 0xC0;
			*byteBuffer++ = (loByte & 0x3F) | 0x80;
		}
		else
		{
			byte loByte = _Lowbyte(*textbuffer);
			byte hiByte = _Highbyte(*textbuffer);

			*byteBuffer++ = (hiByte >> 4) | 0xE0;
			*byteBuffer++ = ((loByte >> 6) & 0x03) | ((hiByte << 2) & 0x3C) | 0x80;
			*byteBuffer++ = (loByte & 0x3F) | 0x80;
		}
		textbuffer++;
	}
}

void CStringBuffer::convertFromUTF8(ConstRef(CByteBuffer) buffer, bool detectBOM, BArray mapping)
{
	BPointer byteBuffer = buffer.get_Buffer();
	dword bufferSize = buffer.get_BufferSize();
	dword bufferSize2 = 0;
	bool hasBOM = false;

	if (detectBOM && (bufferSize > 2) && (*byteBuffer == 0xEF) && (*(byteBuffer + 1) == 0xBB) && (*(byteBuffer + 2) == 0xBF))
	{
		byteBuffer += 3;
		bufferSize -= 3;
		hasBOM = true;
	}
	for ( dword ix = 0; ix < bufferSize; ++ix )
	{
		if ( ((*byteBuffer) & 0xE0) == 0xE0 )
		{
			++ix;
			++byteBuffer;
			++ix;
			++byteBuffer;
		}		
		else if ( ((*byteBuffer) & 0xC0) == 0xC0 )
		{
			++ix;
			++byteBuffer;
		}
		++bufferSize2;
		++byteBuffer;
	}
	SetSize(__FILE__LINE__ bufferSize2 + 1);
	if (mapping)
		*mapping = CastAny(BPointer, TFalloc(bufferSize2));

	CPointer textbuffer = (CPointer)_string;
	BPointer mapP = mapping ? (*mapping) : NULL;

	byteBuffer = buffer.get_Buffer();
	if ( hasBOM )
		byteBuffer += 3;
	for ( dword ix = 0; ix < bufferSize2; ++ix )
	{
		mbchar ch = *byteBuffer++;

		if ( (ch & 0xE0) == 0xE0 )
		{
			*textbuffer = (ch & 0x0F) << 12;
			ch = *byteBuffer++;
			*textbuffer |= (ch & 0x3F) << 6;
			ch = *byteBuffer++;
			*textbuffer |= (ch & 0x3F);
			if (mapP)
				*mapP = 3;
		}
		else if ( (ch & 0xC0) == 0xC0 )
		{
			*textbuffer = (ch & 0x1F) << 6;
			ch = *byteBuffer++;
			*textbuffer |= (ch & 0x3F);
			if (mapP)
				*mapP = 2;
		}
		else
		{
			*textbuffer = ch;
			if (mapP)
				*mapP = 1;	
		}
		++textbuffer;
		if (mapP)
			++mapP;
	}
	*textbuffer = 0;
}

void CStringBuffer::convertToUTF16(Ref(CByteBuffer) buffer, bool littleEndian, bool hasBOM) const
{
	dword lench = GetLength();
	dword bufferSize = lench * 2;

	if (hasBOM)
		bufferSize += 2;
	buffer.set_BufferSize(__FILE__LINE__ bufferSize);

	CConstPointer textbuffer = _string;
	BPointer byteBuffer = buffer.get_Buffer();
	word ch;

	if ( littleEndian )
	{
		if (hasBOM)
		{
			*byteBuffer++ = 0xFF;
			*byteBuffer++ = 0xFE;
		}
		for (dword ix = 0; ix < lench; ++ix)
		{
			ch = *textbuffer++;
			*byteBuffer++ = ch & 0x00FF;
			*byteBuffer++ = (ch >> 8) & 0x00FF;
		}
	}
	else
	{
		if (hasBOM)
		{
			*byteBuffer++ = 0xFE;
			*byteBuffer++ = 0xFF;
		}
		for (dword ix = 0; ix < lench; ++ix)
		{
			ch = *textbuffer++;
			*byteBuffer++ = (ch >> 8) & 0x00FF;
			*byteBuffer++ = ch & 0x00FF;
		}
	}
}

void CStringBuffer::convertFromUTF16(ConstRef(CByteBuffer) buffer, Ptr(bool) littleEndian, bool detectBOM)
{
	dword bufferSize = buffer.get_BufferSize();
	BPointer byteBuffer = buffer.get_Buffer();
	bool bLittleEndian = littleEndian?(*littleEndian):true;

	if ( detectBOM && (bufferSize > 1) )
	{
		if ( (byteBuffer[0] == 0xFF) && (byteBuffer[1] == 0xFE) )
		{
			byteBuffer += 2;
			bufferSize -= 2;
			bLittleEndian = true;
		}
		else if ( (byteBuffer[0] == 0xFE) && (byteBuffer[1] == 0xFF) )
		{
			byteBuffer += 2;
			bufferSize -= 2;
			bLittleEndian = false;
		}
		if (littleEndian)
			*littleEndian = bLittleEndian;
	}
	bufferSize /= 2;
	SetSize(__FILE__LINE__ bufferSize + 1);

	CPointer textbuffer = (CPointer)_string;
	mbchar ch;

	if (bLittleEndian)
	{
		for ( dword ix = 0; ix < bufferSize; ++ix )
		{
			ch = Castword(*byteBuffer++) & 0x00FF;
			ch |= (Castword(*byteBuffer++) << 8) & 0xFF00;
			*textbuffer++ = ch;
		}
	}
	else
	{
		for ( dword ix = 0; ix < bufferSize; ++ix )
		{
			ch = (Castword(*byteBuffer++) << 8) & 0xFF00;
			ch |= Castword(*byteBuffer++) & 0x00FF;
			*textbuffer++ = ch;
		}
	}
	*textbuffer = 0;
}

void CStringBuffer::convertToHex(ConstRef(CByteBuffer) buffer)
{
	dword bufferSize = buffer.get_BufferSize();

	SetSize(__FILE__LINE__ (bufferSize * 2) + 1);
	s_cvtbyteptr2hexstr(buffer.get_Buffer(), buffer.get_BufferSize(), _string);
}

void CStringBuffer::convertFromHex(Ref(CByteBuffer) buffer) const
{
	dword bufferSize = GetLength();

	buffer.set_BufferSize(__FILE__LINE__ (bufferSize + 1) / 2);
	s_cvthexstr2byteptr(_string, bufferSize, buffer.get_Buffer());
}

void CStringBuffer::convertToStdWString(Ref(std::wstring) str) const
{
#ifdef _UNICODE
    if ( PtrCheck(_string) || (0 == (*_string)) )
    {
        str.clear();
        return;
    }
    str.assign(_string, GetLength());
#endif
}

void CStringBuffer::convertFromStdWString(ConstRef(std::wstring) str)
{
#ifdef _UNICODE
    if ( str.empty() )
    {
        _free_Buffer();
        return;
    }
    SetString(__FILE__LINE__ str.c_str(), Castdword(str.length()));
#endif
}

void CStringBuffer::convertToStdString(Ref(std::string) str) const
{

}

void CStringBuffer::convertFromStdString(ConstRef(std::string) str)
{

}

#ifdef QT_VERSION
void CStringBuffer::convertToQString(Ref(QString) str) const
{
    if ( PtrCheck(_string) || (0 == (*_string)) )
    {
        str.clear();
        return;
    }
    str.setUtf16(CastAnyPtr(ushort, _string), GetLength());
}

void CStringBuffer::convertFromQString(ConstRef(QString) str)
{
    if ( str.isEmpty() )
    {
        _free_Buffer();
        return;
    }
    SetString(CastAnyPtr(mbchar, CastMutablePtr(ushort, str.utf16())), str.size() + 1);
}

#endif
