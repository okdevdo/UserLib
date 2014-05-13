/******************************************************************************
    
	This file is part of CSources, which is part of UserLib.

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
#include "CS_PCH.h"
#include "STRUTIL.H"

#ifdef OK_SYS_UNIX
static const char* lltoa_cnvt = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

CSOURCES_API char *lltoa(long long value, char *str, int radix)
{
	char *p;
	char *p1;
	char c;
	unsigned long long quot;
	unsigned long long v;
	bool bMinus;

	if ( PtrCheck(str) )
		return str;
	*str = 0;
	if ( (2 > radix) || (radix > 36) )
		return str;
	p = str;
	if ( value == 0 )
	{
		*p++ = lltoa_cnvt[value];
		*p = 0;
		return str;
	}
	bMinus = ((radix == 10) && (value < 0));
	if ( bMinus )
		v = Cast(unsigned long long, -value);
	else
		v = Cast(unsigned long long, value);
	while ( v > 0 )
	{
		quot = v % radix;
		*p++ = lltoa_cnvt[quot];
		v = v / radix;
	}
	if ( bMinus )
		*p++ = '-';
	*p = 0;
	p1 = str;
	--p;
	while ( p1 < p )
	{
		c = *p1;
		*p1 = *p;
		*p = c;
		++p1;
		--p;
	}
	return str;
}

static const char* ltoa_cnvt = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char *ltoa(
   long value,
   char *str,
   int radix 
)
{
	char *p;
	char *p1;
	char c;
	unsigned long quot;
	unsigned long v;
	bool bMinus;

	if ( PtrCheck(str) )
		return str;
	*str = 0;
	if ( (2 > radix) || (radix > 36) )
		return str;
	p = str;
	if ( value == 0 )
	{
		*p++ = ltoa_cnvt[value];
		*p = 0;
		return str;
	}
	bMinus = ((radix == 10) && (value < 0));
	if ( bMinus )
		v = Cast(unsigned long, -value);
	else
		v = Cast(unsigned long, value);
	while ( v > 0 )
	{
		quot = v % radix;
		*p++ = ltoa_cnvt[quot];
		v = v / radix;
	}
	if ( bMinus )
		*p++ = '-';
	*p = 0;
	p1 = str;
	--p;
	while ( p1 < p )
	{
		c = *p1;
		*p1 = *p;
		*p = c;
		++p1;
		--p;
	}
	return str;
}
#endif

#if _MSC_VER <= 1700
static const char* strtoll_cnvt = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

long long wcstoll(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix)
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
			value = (int)(p - strtoll_cnvt);
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

long long strtoll(const char *_Str, char **_EndPtr, int _Radix)
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
			value = (int)(p - strtoll_cnvt);
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

unsigned long long wcstoull(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix)
{
	unsigned long long result = 0;
	int state = 0;
	int factor = 1;
	int value;
	const char *p;
	unsigned long long vmax;
	unsigned int vrem;

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
			vmax = ULLONG_MAX / _Radix;
			vrem = ULLONG_MAX % _Radix;
			++state;
		case 3:
			p = strchr(strtoll_cnvt, m_toupper(*_Str));
			if (PtrCheck(p))
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(wchar_t, _Str);
				if (factor < 0)
					return ~result;
				return result;
			}
			value = (int)(p - strtoll_cnvt);
			if (value >= _Radix)
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(wchar_t, _Str);
				if (factor < 0)
					return ~result;
				return result;
			}
			if ((result > vmax) || ((result == vmax) && (value > (int)vrem)))
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(wchar_t, _Str);
				errno = ERANGE;
				if (factor < 0)
					return ~ULLONG_MAX;
				return ULLONG_MAX;
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
	if (factor < 0)
		return ~result;
	return result;
}

unsigned long long strtoull(const char *_Str, char **_EndPtr, int _Radix)
{
	unsigned long long result = 0;
	int state = 0;
	int factor = 1;
	int value;
	const char *p;
	unsigned long long vmax;
	unsigned int vrem;

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
			vmax = ULLONG_MAX / _Radix;
			vrem = ULLONG_MAX % _Radix;
			++state;
		case 3:
			p = strchr(strtoll_cnvt, m_toupper(*_Str));
			if (PtrCheck(p))
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(char, _Str);
				if (factor < 0)
					return ~result;
				return result;
			}
			value = (int)(p - strtoll_cnvt);
			if (value >= _Radix)
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(char, _Str);
				if (factor < 0)
					return ~result;
				return result;
			}
			if ((result > vmax) || ((result == vmax) && (value > (int)vrem)))
			{
				if (_EndPtr)
					*_EndPtr = CastMutablePtr(char, _Str);
				errno = ERANGE;
				if (factor < 0)
					return ~ULLONG_MAX;
				return ULLONG_MAX;
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
	if (factor < 0)
		return ~result;
	return result;
}
#endif

void 
s_cvthexstr2byteptr(CConstPointer str, dword strlen, BPointer bptr)
{
	dword ix;

	assert(NotPtrCheck(str));
	assert(NotPtrCheck(bptr));

	for (ix = 0; ix < strlen; ix += 2)
	{
		mbchar buf3[3];
		long cnv;

		buf3[0] = *str++;
		buf3[1] = *str++;
		buf3[2] = 0;

		cnv = s_strtol(buf3, NULL, 16);
		*bptr++ = Castbyte(cnv);
	}
}

void 
s_cvtbyteptr2hexstr(BConstPointer bptr, dword blen, CPointer str)
{
	dword ix;

	assert(NotPtrCheck(str));
	assert(NotPtrCheck(bptr));

	for (ix = 0; ix < blen; ++ix)
	{
		mbchar buf[33];

		s_ltoa(*bptr++, buf, 16);
		switch (s_strlen(buf, 33))
		{
		case 1:
			*str++ = _T('0');
			*str++ = buf[0];
			break;
		case 2:
			*str++ = buf[0];
			*str++ = buf[1];
			break;
		default:
			break;
		}
	}
	*str = 0;
}

bool
s_iscword( mbchar c )
    {
    if ( s_isascii( c ) )
        {
        if ( s_iscsym( c ) )
            return true;
        }
    return false;
    }

bool
s_iscfword( mbchar c )
    {
    if ( s_isascii( c ) )
        {
        if ( s_iscsymf( c ) )
            return true;
        }
    return false;
    }

sword
s_strcnt( CConstPointer str, mbchar ch )
    {
    word cnt = 0;
	if ( PtrCheck(str) )
		return 0;
    while ( *str != 0 )
        {
        if ( ch == *str++ )
            cnt++;
        }
    return cnt;
    }

/*               */
/* strpos series */
/*               */

dword
s_strpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
	if ( PtrCheck(p) )
		return 0;
    m_strpos(p,ch);
	return StrEmpty(p)?0:(Castdword(p-str)+1);
    }

dword
s_strwpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
	if ( PtrCheck(p) )
		return 0;
    m_strwpos(p,ch);
    return StrEmpty(p)?0:(Castdword(p-str)+1);
    }

dword
s_stripos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
	if ( PtrCheck(p) )
		return 0;
	ch = m_toupper(ch); 
    m_stripos(p,ch);
    return StrEmpty(p)?0:(Castdword(p-str)+1);
    }

dword
s_striwpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
	if ( PtrCheck(p) )
		return 0;
	ch = m_toupper(ch);
    m_striwpos(p,ch);
    return StrEmpty(p)?0:(Castdword(p-str)+1);
    }

CPointer
s_strppos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
	if ( PtrCheck(p) )
		return NULL;
    m_strpos(p,ch);
    return StrEmpty(p)?NULL:p;
    }

CPointer
s_strpwpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
	if ( PtrCheck(p) )
		return NULL;
    m_strwpos(p,ch);
    return StrEmpty(p)?NULL:p;
    }

CPointer
s_strpipos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
	if ( PtrCheck(p) )
		return NULL;
	ch = m_toupper(ch); 
    m_stripos(p,ch);
    return StrEmpty(p)?NULL:p;
    }

CPointer
s_strpiwpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
	if ( PtrCheck(p) )
		return NULL;
	ch = m_toupper(ch);
    m_striwpos(p,ch);
    return StrEmpty(p)?NULL:p;
    }

/*                       */
/* reverse strpos series */
/*                       */

dword
s_strrpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
    CPointer l = NULL;
	if ( PtrCheck(p) )
		return 0;
    for ( ;; ) {
        m_strpos(p,ch);
        if ( 0 == *p )
            break;
        l = p++;
    }
    return PtrCheck(l)?0:(Castdword(l-str)+1);
    }

dword
s_strrwpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
    CPointer l = NULL;
	if ( PtrCheck(p) )
		return 0;
    for ( ;; ) {
        m_strwpos(p,ch);
        if ( 0 == *p )
            break;
        l = p++;
        m_strfindwhite(p);
        if ( 0 == *p )
            break;
    }
    return PtrCheck(l)?0:(Castdword(l-str)+1);
    }

dword
s_strripos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
    CPointer l = NULL;
	if ( PtrCheck(p) )
		return 0;
	ch = m_toupper(ch);
    for ( ;; ) {
        m_stripos(p,ch);
        if ( 0 == *p )
            break;
        l = p++;
    }
    return PtrCheck(l)?0:(Castdword(l-str)+1);
    }

dword
s_strriwpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
    CPointer l = NULL;
	if ( PtrCheck(p) )
		return 0;
	ch = m_toupper(ch);
    for ( ;; ) {
        m_striwpos(p,ch);
        if ( 0 == *p )
            break;
        l = p++;
        m_strfindwhite(p);
        if ( 0 == *p )
            break;
    }
    return PtrCheck(l)?0:(Castdword(l-str)+1);
    }

CPointer
s_strprpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
    CPointer l = NULL;
	if ( PtrCheck(p) )
		return NULL;
    for ( ;; ) {
        m_strpos(p,ch);
        if ( 0 == *p )
            break;
        l = p++;
    }
    return l;
    }

CPointer
s_strprwpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
    CPointer l = NULL;
	if ( PtrCheck(p) )
		return NULL;
    for ( ;; ) {
        m_strwpos(p,ch);
        if ( 0 == *p )
            break;
        l = p++;
        m_strfindwhite(p);
        if ( 0 == *p )
            break;
    }
    return l;
    }

CPointer
s_strpripos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
    CPointer l = NULL;
	if ( PtrCheck(p) )
		return NULL;
    ch = toupper(ch);
    for ( ;; ) {
        m_stripos(p,ch);
        if ( 0 == *p )
            break;
        l = p++;
    }
    return l;
    }

CPointer
s_strpriwpos( CConstPointer str, mbchar ch )
    {
    CPointer p = CastMutable(CPointer,str);
    CPointer l = NULL;
	if ( PtrCheck(p) )
		return NULL;
    ch = toupper(ch);
    for ( ;; ) {
        m_striwpos(p,ch);
        if ( 0 == *p )
            break;
        l = p++;
        m_strfindwhite(p);
        if ( 0 == *p )
            break;
    }
    return l;
    }

/*				  */
/* strpbrk series */
/*				  */

CPointer
s_strpbrk( CConstPointer s1, CConstPointer s2 )
    {
    CPointer p;
	CPointer p1 = CastMutable(CPointer,s1);
	CPointer p2 = CastMutable(CPointer,s2);
	if ( PtrCheck(p1) )
		return NULL;
	if ( PtrCheck(p2) )
		return NULL;
    m_strpbrk(p1,p2,p);
    return StrEmpty(p1)?NULL:p1;
    }

CPointer
s_strpnonbrk( CConstPointer s1, CConstPointer s2 )
    {
    CPointer p;
	CPointer p1 = CastMutable(CPointer,s1);
	CPointer p2 = CastMutable(CPointer,s2);
	if ( PtrCheck(p1) )
		return NULL;
	if ( PtrCheck(p2) )
		return NULL;
    m_strpnonbrk(p1,p2,p);
    return StrEmpty(p1)?NULL:p1;
    }

dword
s_strpospbrk( CConstPointer s1, CConstPointer s2 )
    {
    CPointer p;
	CPointer p1 = CastMutable(CPointer,s1);
	CPointer p2 = CastMutable(CPointer,s2);
	if ( PtrCheck(p1) )
		return 0;
	if ( PtrCheck(p2) )
		return 0;
    m_strpbrk(p1,p2,p);
    return StrEmpty(p1)?0:(Castdword(p1-s1)+1);
    }

CPointer
s_strptran( CPointer s1, CConstPointer s2, CConstPointer s3 )
    {
    dword pos;
    CPointer rs = s1;
	if ( PtrCheck(s1) )
		return NULL;
	if ( PtrCheck(CastMutable(CPointer,s2)) )
		return NULL;
	if ( PtrCheck(CastMutable(CPointer,s3)) )
		return NULL;
	if ( s_strlen(CastMutable(CPointer,s2), 65536L) != s_strlen(CastMutable(CPointer,s3), 65536L) )
		return NULL;
    while ( NotStrEmpty(s1) )
        {
        if ( ( pos = s_strpos( s2, *s1 ) ) != 0 )
            *s1 = *( s3 + ( pos - 1 ) );
        s1++;
        }
    return rs;
    }

CPointer
s_stripbrk( CConstPointer s1, CConstPointer s2 )
    {
    CPointer p;
    mbchar cc;
	CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
	if ( PtrCheck(CastMutable(CPointer,s2)) )
		return NULL;
    while ( NotStrEmpty(p1) )
        {
        p = CastMutable(CPointer,s2);
        cc = m_toupper(*p1);
        m_stripos(p,cc);
        if ( 0 != *p )
            break;
        p1++;
        }
    return StrEmpty(p1)?NULL:p1;
    }

CPointer
s_stripnonbrk( CConstPointer s1, CConstPointer s2 )
    {
    CPointer p;
    mbchar cc;
	CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
	if ( PtrCheck(CastMutable(CPointer,s2)) )
		return NULL;
    while ( NotStrEmpty(p1) )
        {
        p = CastMutable(CPointer,s2);
        cc = m_toupper(*p1);
        m_stripos(p,cc);
        if ( 0 == *p )
            break;
        p1++;
        }
	return StrEmpty(p1)?NULL:p1;
    }

dword
s_stripospbrk( CConstPointer s1, CConstPointer s2 )
    {
    CPointer p;
    mbchar cc;
	CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return 0;
	if ( PtrCheck(CastMutable(CPointer,s2)) )
		return 0;
    while ( NotStrEmpty(p1) )
        {
        p = CastMutable(CPointer,s2);
        cc = m_toupper(*p1);
        m_stripos(p,cc);
        if ( 0 != *p )
            break;
        p1++;
        }
    return StrEmpty(p1)?0:(Castdword(p1-s1)+1);
    }

CPointer
s_strwpbrk( CConstPointer s1, CConstPointer s2 )
    {
 	CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
	if ( PtrCheck(CastMutable(CPointer,s2)) )
		return NULL;
    while ( NotStrEmpty( p1 ) )
        {
        p1 = s_strpwhite( p1 );
        if ( StrEmpty( p1 ) || ( 0 != s_strpos( s2, *p1 ) ) )
            break;
        p1 = s_strpnonwhite( p1 );
        }
	return StrEmpty(p1)?NULL:p1;
    }

CPointer
s_striwpbrk( CConstPointer s1, CConstPointer s2 )
    {
 	CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
	if ( PtrCheck(CastMutable(CPointer,s2)) )
		return NULL;
    while ( NotStrEmpty( p1 ) )
        {
        p1 = s_strpwhite( p1 );
        if ( StrEmpty( p1 ) || ( 0 != s_stripos( s2, *p1 ) ) )
            return p1;
        p1 = s_strpnonwhite( p1 );
        }
    return p1;
    }

CPointer
s_strpskip( CConstPointer s1, CConstPointer s2 )
    {
    CPointer p;
	CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
	if ( PtrCheck(CastMutable(CPointer,s2)) )
		return NULL;
    while ( NotStrEmpty(p1) )
        {
        p = CastMutable(CPointer,s2);
        m_strpos(p,*p1);
        if ( 0 == *p )
            break;
        p1++;
        }
	return StrEmpty(p1)?NULL:p1;
    }

/*					  */
/* strposwhite series */
/*					  */

CPointer
s_strpwhite( CConstPointer s1 )
    {
	CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
    m_streatwhite(p1);
    return StrEmpty(p1)?NULL:p1;
    }

dword
s_strposwhite( CConstPointer s1 )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return 0;
    m_streatwhite(p1);
    return StrEmpty(p1)?0:(Castdword(p1-s1)+1);
    }

CPointer
s_strpnwhite( CConstPointer s1, dword cnt )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
    while ( (cnt > 0) && ( NotStrEmpty(p1) ) && ( isspace( *p1 ) ) ) {
        p1++;
        cnt--;
    }
	return StrEmpty(p1)?NULL:p1;
    }

dword
s_strnposwhite( CConstPointer s1, dword cnt )
    {
    CPointer p1 = CastMutable(CPointer,s1);
    dword max = cnt;
	if ( PtrCheck(p1) )
		return 0;
    while ( (cnt > 0) && ( NotStrEmpty(p1) ) && ( isspace( *p1 ) ) ) {
        p1++;
        cnt--;
    }
    return max - cnt;
    }

CPointer
s_strpnonwhite( CConstPointer s1 )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
    m_strfindwhite(p1);
    return StrEmpty(p1)?NULL:p1;
    }

CPointer
s_strpword( CConstPointer s1 )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
    while ( ( NotStrEmpty(p1) ) && ( !s_isspace( *p1 ) ) ) p1++;
    return StrEmpty(p1)?NULL:p1;
    }

CPointer
s_strpnonword( CConstPointer s1 )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
    while ( ( NotStrEmpty(p1) ) && ( s_isspace( *p1 ) ) ) p1++;
    return StrEmpty(p1)?NULL:p1;
    }

/*					   */
/* strrposwhite series */
/*					   */

CPointer
s_strrnpwhite( CConstPointer s1, sdword len )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
    if ( len > 0 )
        p1 += ( len - 1 );
    else
        len = -len;
    while ( (len > 0) && ( isspace( *p1 ) ) ) {
        p1--;
        len--;
        }
    return p1;
    }

dword
s_strrnposwhite( CConstPointer s1, sdword len )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return 0;
    if ( len > 0 )
        p1 += ( len - 1 );
    else
        len = -len;
    while ( ( len > 0 ) && ( isspace( *p1 ) ) )
        {
        p1--;
        len--;
        }
    return len;
    }

CPointer
s_strrnpnonwhite( CConstPointer s1, sdword len )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return NULL;
    if ( len > 0 )
        p1 += ( len - 1 );
    else
        len = -len;
    while ( ( len > 0 ) && ( !isspace( *p1 ) ) )
        {
        p1--;
        len--;
        }
    return p1;
    }

dword
s_strrnposnonwhite( CConstPointer s1, sdword len )
    {
    CPointer p1 = CastMutable(CPointer,s1);
	if ( PtrCheck(p1) )
		return 0;
    if ( len > 0 )
        p1 += ( len - 1 );
    else
        len = -len;
    while ( ( len > 0 ) && ( !isspace( *p1 ) ) )
        {
        p1--;
        len--;
        }
    return len;
    }

/*			     */
/* strstr series */
/*				 */

CPointer
s_strstr (CConstPointer phaystack, CConstPointer pneedle)
{
	CPointer haystack;
	CPointer needle;
	mbchar b;
	CPointer rneedle;

	if ( ConstStrEmpty(phaystack) || ConstStrEmpty(pneedle) )
		return NULL;
	haystack = CastMutable(CPointer,phaystack);
	if ((b = *(needle = CastMutable(CPointer, pneedle))))
    {
      mbchar c;

      haystack--;               /* possible ANSI violation */
      {
        mbchar a;

        do
		{
          if (!(a = *++haystack))
            goto ret0;
		}
        while (a != b);
      }

      if (!(c = *++needle))
        goto foundneedle;
      ++needle;
      goto jin;

      for (;;)
        {
          {
            mbchar a;
            if (0)
            jin:{
                if ((a = *++haystack) == c)
                  goto crest;
              }
            else
              a = *++haystack;
            do
              {
                for (; a != b; a = *++haystack)
                  {
                    if (!a)
                      goto ret0;
                    if ((a = *++haystack) == b)
                      break;
                    if (!a)
                      goto ret0;
                  }
              }
            while ((a = *++haystack) != c);
          }
        crest:
          {
            mbchar a;
            {
              CPointer rhaystack;
              if (*(rhaystack = haystack-- + 1) == (a = *(rneedle = needle)))
                do
                  {
                    if (!a)
                      goto foundneedle;
                    if (*++rhaystack != (a = *++needle))
                      break;
                    if (!a)
                      goto foundneedle;
                  }
                while (*++rhaystack == (a = *++needle));
              needle = rneedle; /* took the register-poor aproach */
            }
            if (!a)
              break;
          }
        }
    }
foundneedle:
  return haystack;
ret0:
  return 0;
}

CPointer
s_strnstr2( CConstPointer s1, CConstPointer s2, dword minsize )
    {
    mbchar cc1;
    mbchar cc2;
    CPointer ps1 = CastMutable(CPointer,s1);
    CPointer ps2 = CastMutable(CPointer,s2);
    dword ls;
    dword lc = 0;
    if ( StrEmpty( ps1 ) || StrEmpty( ps2 ) )
        return NULL;
    ls = s_strpos( ps2 + 1, *ps2 );
    lc = s_strpos( ps1, *ps2 );
    if ( lc == 0 )
        return NULL;
    ps1 += lc - 1;
    s1 = ps1;
    minsize -= Min( lc, minsize );
    lc = 0;
    while ( ( minsize > 0 ) && ( ( cc1 = *s1++ ) != 0 ) )
        {
        if ( ( cc2 = *s2++ ) == cc1 )
            lc++;
        else if ( cc2 == 0 )
            return ps1;
        else
            {
            if ( lc == 0 )
                {
                lc = s_strpos( ps1, *ps2 );
                if ( lc == 0 )
                    return NULL;
                ps1 += lc - 1;
                minsize -= Min( lc, minsize );
                lc = 1;
            } else
                {
                if ( ls )
                    {
                    minsize += lc;
                    lc = Min( lc, ls );
                    minsize -= lc;
                    }                  /* endif */
                ps1 += lc;
                lc = 0;
                }                      /* endif */
            s1 = ps1 + lc;
            s2 = ps2 + lc;
            continue;
            }                          /* endif */
        minsize--;
        }                              /* endwhile */
    if ( *s2 == 0 )
        return ps1;
    return NULL;
    }

CPointer
s_strwstr2( CConstPointer s1, CConstPointer s2 )
    {
    mbchar cc1;
    mbchar cc2;
    dword ls;
    dword lc;
    CPointer ps1 = CastMutable(CPointer,s1);
    CPointer ps2 = CastMutable(CPointer,s2);
    if ( StrEmpty( ps1 ) || StrEmpty( ps2 ) )
        return NULL;
    ls = s_strpos( ps2 + 1, *ps2 );
    lc = s_strpos( ps2, 32 );
    if ( lc )
        ls = Min( ls, lc - 1 );
    lc = s_strwpos( ps1, *ps2 );
    if ( lc == 0 )
        return NULL;
    ps1 += lc - 1;
    s1 = ps1;
    lc = 0;
    while ( ( cc1 = *s1++ ) != 0 )
        {
        if ( ( cc2 = *s2++ ) == cc1 )
            lc++;
        else if ( cc2 == 0 )
            return ps1;
        else
            {
            if ( lc > 0 )
                {
                if ( ls )
                    lc = Min( lc, ls );
                ps1 += lc;
                lc = 0;
                }                      /* endif */
            if ( lc == 0 )
                {
                lc = s_strwpos( ps1, *ps2 );
                if ( lc == 0 )
                    return NULL;
                ps1 += lc - 1;
                lc = 1;
                }                      /* endif */
            s1 = ps1 + lc;
            s2 = ps2 + lc;
            }                          /* endif */
        }                              /* endwhile */
    if ( *s2 == 0 )
        return ps1;
    return NULL;
    }

CPointer
s_strnistr2( CConstPointer s1, CConstPointer s2, dword minsize )
    {
    mbchar cc1;
    mbchar cc2;
    dword ls;
    dword lc;
    CPointer ps1 = CastMutable(CPointer,s1);
    CPointer ps2 = CastMutable(CPointer,s2);
    if ( StrEmpty( ps1 ) || StrEmpty( ps2 ) )
        return NULL;
    ls = s_strpos( ps2 + 1, *ps2 );
    lc = s_stripos( ps1, *ps2 );
    if ( lc == 0 )
        return NULL;
    ps1 += lc - 1;
    s1 = ps1;
    minsize -= Min( lc - 1, minsize );
    lc = 0;
    while ( ( minsize > 0 ) && ( ( cc1 = toupper( *s1++ ) ) != 0 ) )
        {
        if ( ( cc2 = toupper( *s2++ ) ) == cc1 )
            lc++;
        else if ( cc2 == 0 )
            return ps1;
        else
            {
            if ( lc == 0 )
                {
                lc = s_stripos( ps1, *ps2 );
                if ( lc == 0 )
                    return NULL;
                ps1 += lc - 1;
                minsize -= Min( lc - 1, minsize );
                lc = 1;
            } else
                {
                if ( ls )
                    {
                    minsize += lc;
                    lc = Min( lc, ls );
                    minsize -= lc;
                    }                  /* endif */
                ps1 += lc;
                lc = 0;
                }                      /* endif */
            s1 = ps1 + lc;
            s2 = ps2 + lc;
            continue;
            }                          /* endif */
        minsize--;
        }                              /* endwhile */
    if ( *s2 == 0 )
        return ps1;
    return NULL;
    }

CPointer
s_striwstr2( CConstPointer s1, CConstPointer s2 )
    {
    mbchar cc1;
    mbchar cc2;
    dword ls;
    dword lc;
    CPointer ps1 = CastMutable(CPointer,s1);
    CPointer ps2 = CastMutable(CPointer,s2);
    if ( StrEmpty( ps1 ) || StrEmpty( ps2 ) )
        return NULL;
    ls = s_strpos( ps2 + 1, *ps2 );
    lc = s_strpos( ps2, 32 );
    if ( lc )
        ls = Min( ls, lc - 1 );
    lc = s_striwpos( ps1, *ps2 );
    if ( lc == 0 )
        return NULL;
    ps1 += lc - 1;
    s1 = ps1;
    lc = 0;
    while ( ( cc1 = toupper( *s1++ ) ) != 0 )
        {
        if ( ( cc2 = toupper( *s2++ ) ) == cc1 )
            lc++;
        else if ( cc2 == 0 )
            return ps1;
        else
            {
            if ( lc > 0 )
                {
                if ( ls )
                    lc = Min( lc, ls );
                ps1 += lc;
                lc = 0;
                }                      /* endif */
            if ( lc == 0 )
                {
                lc = s_striwpos( ps1, *ps2 );
                if ( lc == 0 )
                    return NULL;
                ps1 += lc - 1;
                lc = 1;
                }                      /* endif */
            s1 = ps1 + lc;
            s2 = ps2 + lc;
            }                          /* endif */
        }                              /* endwhile */
    if ( *s2 == 0 )
        return ps1;
    return NULL;
    }

/*			       */
/* strsplit series */
/*				   */
void 
s_strsplitany(CPointer p, CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize)
{
	CPointer p1;
	dword i;

	if ( ConstStrEmpty(delimiter) || (0 == output) || (maxoutputsize == 0) || StrEmpty(p) )
	{
		if ( outputsize )
			*outputsize = 0;
        return;
    }

	p1 = s_strpbrk(p, delimiter);
	i = 0;

	while ( NotPtrCheck(p1) && (i < maxoutputsize) )
	{
		output[i++] = p; *p1 = 0; p = p1 + 1;
		p1 = s_strpbrk(p, delimiter);
	}
	if ( i < maxoutputsize )
		output[i++] = p;
	if ( outputsize )
		*outputsize = i;
}

void 
s_strsplit(CPointer p, CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize)
{
	CPointer p1;
	dword i;
	dword dlen;

    if ( ConstStrEmpty(delimiter) || (0 == output) || (maxoutputsize == 0) || StrEmpty(p) )
	{
		if ( outputsize )
			*outputsize = 0;
		return;
	}

	p1 = s_strstr(p, delimiter);
	i = 0;
	dlen = s_strlen(CastMutable(CPointer, delimiter), INT_MAX);

	while ( NotPtrCheck(p1) && (i < maxoutputsize) )
	{
		output[i++] = p; *p1 = 0; p = p1 + dlen;
		p1 = s_strstr(p, delimiter);
	}
	if ( i < maxoutputsize )
		output[i++] = p;
	if ( outputsize )
		*outputsize = i;
}

void 
s_strsplitquoted(CPointer p, CConstPointer quote, CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize)
{
	CPointer p1;
	dword i;
	dword dlen;
	dword qlen;
	dword state;

    if ( ConstStrEmpty(delimiter) || ConstStrEmpty(quote) || (0 == output) || (maxoutputsize == 0) || StrEmpty(p) )
    {
        if ( outputsize )
            *outputsize = 0;
        return;
    }

	i = 0;
	dlen = s_strlen(CastMutable(CPointer, delimiter), INT_MAX);
	qlen = s_strlen(CastMutable(CPointer, quote), INT_MAX);
	p1 = p;
	state = 2;

	while ( (*p1 != 0) && (i < maxoutputsize) )
	{
		switch ( state )
		{
		case 0:
			if ( s_strncmp(p1, delimiter, dlen) == 0 )
			{
				*p1 = 0; output[i++] = p;
				p1 += dlen; p = p1;
				state = 2;
				break;
			}
			if ( s_strncmp(p1, quote, qlen) == 0 )
				goto __error;
			++p1;
			break;
		case 1:
			if ( s_strncmp(p1, quote, qlen) == 0 )
			{
				if ( s_strncmp(p1 + qlen, quote, qlen) == 0 )
				{
					Pointer pp1 = p1 + qlen;
					Pointer pp2 = p1 + qlen + qlen;
					size_t sz = s_strlen(pp2, INT_MAX) * szchar;

					s_memmove_s(pp1, sz + (qlen * szchar), pp2, sz);
					p1 += qlen;
					break;
				}
				if ( s_strncmp(p1 + qlen, delimiter, dlen) != 0 )
					goto __error;
				*p1 = 0; output[i++] = p;
				p1 += qlen + dlen; p = p1;
				state = 2;
				break;
			}
			++p1;
			break;
		case 2:
			if ( s_strncmp(p, quote, qlen) == 0 )
			{
				p += qlen;
				state = 1;
			}
			else
				state = 0;
			p1 = p;
			break;
		}
	}
	if ( i < maxoutputsize )
	{
		switch ( state )
		{
		case 0:
		case 2:
			output[i++] = p;
			break;
		default:
			goto __error;
		}
	}
	if ( outputsize )
		*outputsize = i;
	return;
__error:
	if ( outputsize )
		*outputsize = 0;
}

void 
s_strsplitquoted2(CPointer p, CConstPointer recorddelimiter, CConstPointer fieldquote, CConstPointer fielddelimiter, TSplitFunc output, Pointer outputcontext)
{
	CPointer p1;
	CPointer p2;
	dword i;
	dword j;
	dword k;
	dword rdlen;
	dword fdlen;
	dword fqlen;
	dword state;

	if (ConstStrEmpty(recorddelimiter) || ConstStrEmpty(fieldquote) || ConstStrEmpty(fielddelimiter) || (NULL == output) || StrEmpty(p))
        return;

	i = 0;
	j = 0;
	rdlen = s_strlen(CastMutable(CPointer, recorddelimiter), INT_MAX);
	fdlen = s_strlen(CastMutable(CPointer, fielddelimiter), INT_MAX);
	fqlen = s_strlen(CastMutable(CPointer, fieldquote), INT_MAX);
	p1 = p;
	p2 = NULL;
	state = 2;

	while ( *p1 != 0 )
	{
		switch ( state )
		{
		case 0:
			if (s_strncmp(p1, fielddelimiter, fdlen) == 0)
			{
				*p1 = 0; output(i, j, p, outputcontext); ++j; p1 += fdlen; p = p1;
				state = 2;
				break;
			}
			if (s_strncmp(p1, recorddelimiter, rdlen) == 0)
			{
				*p1 = 0; output(i, j, p, outputcontext); j = 0; ++i; p1 += rdlen; p = p1;
				state = 2;
				break;
			}
			if (s_strncmp(p1, fieldquote, fqlen) == 0)
				goto __error;
			++p1;
			break;
		case 1:
			if (s_strncmp(p2, fieldquote, fqlen) == 0)
			{
				if (s_strncmp(p2 + fqlen, fieldquote, fqlen) == 0)
				{
					if (p1 != p2)
					{
						for (k = 0; k < fqlen; ++k)
							*p1++ = *p2++;
						p2 += fqlen;
					}
					else
					{
						p1 += fqlen;
						p2 = p1 + fqlen;
					}
					break;
				}
				if (s_strncmp(p2 + fqlen, fielddelimiter, fdlen) == 0)
				{
					*p1 = 0; output(i, j, p, outputcontext); ++j; p1 = p2 + fqlen + fdlen; p = p1;
					state = 2;
					break;
				}
				if (s_strncmp(p2 + fqlen, recorddelimiter, rdlen) == 0)
				{
					*p1 = 0; output(i, j, p, outputcontext); j = 0; ++i; p1 = p2 + fqlen + rdlen; p = p1;
					state = 2;
					break;
				}
				goto __error;
			}
			if (p2 != p1)
				*p1 = *p2;
			++p1;
			++p2;
			break;
		case 2:
			if (s_strncmp(p, fieldquote, fqlen) == 0)
			{
				p += fqlen;
				p2 = p;
				state = 1;
			}
			else
				state = 0;
			p1 = p;
			break;
		}
	}
__error:
	return;
}

/*		  	      */
/* strxcmp series */
/*				  */
bool 
s_strxcmp(CConstPointer fn, CConstPointer mask)
{
    CPointer fn1 = CastMutable(CPointer,fn);
    CPointer mx1 = CastMutable(CPointer,mask);
	CPointer fn2;
	CPointer fn3;
	CPointer mx2;
	CPointer mx3;
	bool bCmp;

    if ( StrEmpty( fn1 ) || StrEmpty( mx1 ) )
        return false;
	while ( *fn1 && *mx1 )
	{
		switch ( *mx1 )
		{
		case '*':
			do
				++mx1;
			while ( *mx1 == '*' );
			if ( 0 == *mx1 )
				return true;
			mx2 = mx1;
			while ( *mx2 && (*mx2 != '*') )
				++mx2;
			if ( 0 == *mx2 )
			{
				fn2 = fn1;
				while ( *fn2 ) ++fn2;
				fn1 = fn2 - mx2 + mx1;
				break;
			}
			fn2 = fn1;
			while ( *fn2 ) ++fn2;
			if ( (fn2 - fn1) < (mx2 - mx1) )
				return false;
			fn2 = fn2 - mx2 + mx1;
			fn3 = fn1;
			mx3 = mx1;
			bCmp = false;
			while ( fn1 != fn2 )
			{
				if ( mx1 == mx2 )
				{
					bCmp = true;
					break;
				}
#ifdef OK_SYS_WINDOWS
				else if ((*mx1 == '?') || (m_toupper(*mx1) == m_toupper(*fn1)))
#endif
#ifdef OK_SYS_UNIX
				else if ((*mx1 == '?') || (*mx1 == *fn1))
#endif
				{
					++fn1;
					++mx1;
				}
				else
				{
					fn1 = ++fn3;
					mx1 = mx3;
				}
			}
			if ( bCmp )
				break;
			mx1 = mx3;
			fn1 = fn3;
			while ( mx1 != mx2 )
			{
#ifdef OK_SYS_WINDOWS
				if ((*mx1 == '?') || (m_toupper(*mx1) == m_toupper(*fn1)))
#endif
#ifdef OK_SYS_UNIX
				if ((*mx1 == '?') || (*mx1 == *fn1))
#endif
				{
					++fn1;
					++mx1;
				}
				else
					return false;
			}
			break;
		case '?':
			++fn1;
			++mx1;
			break;
		default:
#ifdef OK_SYS_WINDOWS
			if (m_toupper(*mx1) != m_toupper(*fn1))
#endif
#ifdef OK_SYS_UNIX
			if (*mx1 != *fn1)
#endif
				return false;
			++fn1;
			++mx1;
			break;
		}
	}
	return ((0 == *fn1) && (0 == *mx1));
}

#ifdef __DBGTEST_STRUTIL__
CPointer test[10] = {
	__T(" aaaabbbbccc"),
	__T("aaabbbbccccd"),
	__T("abbbbccccddd"),
	__T("bbbccccdddde"),
	__T("bccccddddeee"),
	__T("cccddddeeeef"),
	__T("cddddeeeefff"),
	__T("dddeeeeffffg"),
	__T("deeeeffffggg"),
	__T("eeeffffggggh")
	};

CPointer TestStr = __T(" aaaabbbbccccddddeeeeffffgggghhhh");
CPointer TestStr2 = __T(" aaaa bbbb cccc dddd eeee ffff gggg hhhh");
CPointer TestStr3 = __T("cde");

int 
_tmain( int argc, CPointer *argv )
{
	CPointer presult;
	dword dwresult;

	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );

	dwresult = s_strpos(TestStr, __T('a'));
	_ASSERTE(dwresult == 2);
	dwresult = s_strpos(TestStr, __T('b'));
	_ASSERTE(dwresult == 6);

	dwresult = s_strwpos(TestStr2, __T('a'));
	_ASSERTE(dwresult == 2);
	dwresult = s_strwpos(TestStr2, __T('b'));
	_ASSERTE(dwresult == 7);

	dwresult = s_stripos(TestStr, __T('A'));
	_ASSERTE(dwresult == 2);
	dwresult = s_stripos(TestStr, __T('B'));
	_ASSERTE(dwresult == 6);

	dwresult = s_striwpos(TestStr2, __T('A'));
	_ASSERTE(dwresult == 2);
	dwresult = s_striwpos(TestStr2, __T('B'));
	_ASSERTE(dwresult == 7);

	presult = s_strppos(TestStr, __T('a'));
	_ASSERTE(presult == (TestStr + 1));
	presult = s_strppos(TestStr, __T('b'));
	_ASSERTE(presult == (TestStr + 5));

	presult = s_strpwpos(TestStr2, __T('a'));
	_ASSERTE(presult == (TestStr2 + 1));
	presult = s_strpwpos(TestStr2, __T('b'));
	_ASSERTE(presult == (TestStr2 + 6));

	presult = s_strpipos(TestStr, __T('a'));
	_ASSERTE(presult == (TestStr + 1));
	presult = s_strpipos(TestStr, __T('b'));
	_ASSERTE(presult == (TestStr + 5));

	presult = s_strpiwpos(TestStr2, __T('A'));
	_ASSERTE(presult == (TestStr2 + 1));
	presult = s_strpiwpos(TestStr2, __T('B'));
	_ASSERTE(presult == (TestStr2 + 6));

	dwresult = s_strrpos(TestStr, __T('a'));
	_ASSERTE(dwresult == 5);
	dwresult = s_strrpos(TestStr, __T('b'));
	_ASSERTE(dwresult == 9);

	dwresult = s_strrwpos(TestStr2, __T('a'));
	_ASSERTE(dwresult == 2);
	dwresult = s_strrwpos(TestStr2, __T('b'));
	_ASSERTE(dwresult == 7);

	dwresult = s_strripos(TestStr, __T('A'));
	_ASSERTE(dwresult == 5);
	dwresult = s_strripos(TestStr, __T('B'));
	_ASSERTE(dwresult == 9);

	dwresult = s_strriwpos(TestStr2, __T('A'));
	_ASSERTE(dwresult == 2);
	dwresult = s_strriwpos(TestStr2, __T('B'));
	_ASSERTE(dwresult == 7);

	presult = s_strprpos(TestStr, __T('a'));
	_ASSERTE(presult == (TestStr + 4));
	presult = s_strprpos(TestStr, __T('b'));
	_ASSERTE(presult == (TestStr + 8));

	presult = s_strprwpos(TestStr2, __T('a'));
	_ASSERTE(presult == (TestStr2 + 1));
	presult = s_strprwpos(TestStr2, __T('b'));
	_ASSERTE(presult == (TestStr2 + 6));

	presult = s_strpripos(TestStr, __T('A'));
	_ASSERTE(presult == (TestStr + 4));
	presult = s_strpripos(TestStr, __T('B'));
	_ASSERTE(presult == (TestStr + 8));

	presult = s_strpriwpos(TestStr2, __T('A'));
	_ASSERTE(presult == (TestStr2 + 1));
	presult = s_strpriwpos(TestStr2, __T('B'));
	_ASSERTE(presult == (TestStr2 + 6));

	presult = s_strpbrk(TestStr, TestStr3);
	_ASSERTE(presult == (TestStr + 9));
	presult = s_strpbrk(TestStr2, TestStr3);
	_ASSERTE(presult == (TestStr2 + 11));

	presult = s_strpnonbrk(TestStr, TestStr3);
	_ASSERTE(presult == TestStr);
	presult = s_strpnonbrk(TestStr2, TestStr3);
	_ASSERTE(presult == TestStr2);

	dwresult = s_strpospbrk(TestStr, TestStr3);
	_ASSERTE(dwresult == 10);
	dwresult = s_strpospbrk(TestStr2, TestStr3);
	_ASSERTE(dwresult == 12);

	presult = s_strstr(TestStr, test[0]);
	_ASSERTE(presult == TestStr);
	presult = s_strstr(TestStr, test[1]);
	_ASSERTE(presult == (TestStr + 2));

	getchar();
}
#endif
/* strutil.c */
