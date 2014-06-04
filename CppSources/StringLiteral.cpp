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
#include "StringLiteral.h"

CStringConstIterator::CStringConstIterator( CConstPointer _str ): 
	_origin(CastMutable(CPointer,_str)), 
    _current(CastMutable(CPointer,_str))
{
}

CStringConstIterator::CStringConstIterator( CConstPointer _strOrigin, CConstPointer _strCurrent ): 
	_origin(CastMutable(CPointer,_strOrigin)), 
    _current(CastMutable(CPointer,_strCurrent))
{
}

CStringConstIterator::CStringConstIterator( ConstRef(CStringConstIterator) _str ): 
    _origin(_str._origin), 
	_current(_str._current)
{
}

CStringConstIterator::~CStringConstIterator()
{
}

CStringConstIterator& CStringConstIterator::operator = ( CConstPointer _str ) 
{
    _current = CastMutable(CPointer,_str);
    _origin = _current;
    return *this;
}

CStringConstIterator& CStringConstIterator::operator = ( ConstRef(CStringConstIterator) _str ) 
{
	_current = _str._current;
	_origin = _str._origin;
    return *this;
}

CStringConstIterator& CStringConstIterator::operator++() 
{
    if ( !IsEnd() )
        _current++;
    return *this;
}

CStringConstIterator CStringConstIterator::operator++(int) 
{
    CStringConstIterator tmp = *this;
    ++(*this);
    return tmp;
}

CStringConstIterator& CStringConstIterator::operator--() 
{
    if ( !IsBegin() )
        _current--;
    return *this;
}

CStringConstIterator CStringConstIterator::operator--(int)
{
    CStringConstIterator tmp = *this;
    --(*this);
    return tmp;
}

CStringConstIterator& CStringConstIterator::operator += (WLong offset)
{
    add_offset(offset);
	return *this;
}

CStringConstIterator& CStringConstIterator::operator -= (WLong offset)
{
    add_offset(-offset);
	return *this;
}

sword CStringConstIterator::Compare(const CStringConstIterator& _cmp2, dword num, dword mod) const
{
	if (PtrCheck(_current)) {
		if (PtrCheck(_cmp2._current))
			return 0;
		return -1;
	}
	if (PtrCheck(_cmp2._current))
		return 1;

	int cmp = -1;

	switch (mod)
	{
	case cComparePure:
		cmp = s_strcmp(_current, _cmp2._current);
		break;
	case cIgnoreCase:
		cmp = s_stricmp(_current, _cmp2._current);
		break;
	case cLimited:
		cmp = s_strncmp(_current, _cmp2._current, num);
		break;
	case cLimited + cIgnoreCase:
		cmp = s_strnicmp(_current, _cmp2._current, num);
		break;
	default:
		break;
	}
	if (cmp < 0)
		return -1;
	if (cmp > 0)
		return 1;
	return 0;
}

CStringConstIterator& CStringConstIterator::Find(mbchar c, WULong mod)
{
	if ( IsEnd() )
		return *this;
    switch ( mod ) {
    case cComparePure:
        m_strpos(_current,c);
        break;
    case cIgnoreCase:
        m_stripos(_current,c);
        break;
    case cBreakOnWord:
        m_strwpos(_current,c);
        break;
    case (cIgnoreCase+cBreakOnWord):
        m_striwpos(_current,c);
    default:
        break;
    }
    return *this;
}

CStringConstIterator& CStringConstIterator::FindReverse( mbchar c, WULong mod )
{
	if ( IsEnd() )
		return *this;

	CPointer p = nullptr;

    switch ( mod ) {
    case cComparePure:
        p = s_strprpos(_current,c);
        break;
    case cIgnoreCase:
        p = s_strpripos(_current,c);
        break;
    case cBreakOnWord:
        p = s_strprwpos(_current,c);
        break;
    case (cIgnoreCase+cBreakOnWord):
        p = s_strpriwpos(_current,c);
    default:
        break;
    }
	_current = PtrCheck(p)?(_current + GetLength()):p;
    return *this;
}

CStringConstIterator& CStringConstIterator::Find( CConstPointer str, WULong mod ) 
{
	if ( IsEnd() )
		return *this;

	CPointer p = nullptr;

    switch ( mod ) {
    case cComparePure:
		p = s_strstr(_current, str);
        break;
	case cIgnoreCase:
		p = s_strnistr2(_current, str, s_strlen(str, INT_MAX));
        break;
    case cBreakOnWord:
		p = s_strwstr2(_current, str);
        break;
    case (cIgnoreCase+cBreakOnWord):
		p = s_striwstr2(_current, str);
    default:
        break;
    }
	_current = PtrCheck(p)?(_current + GetLength()):p;
    return *this;
}

CStringConstIterator& CStringConstIterator::FirstOf( CConstPointer s2, WULong mod ) 
{
	if ( IsEnd() )
		return *this;

    CPointer p;
	mbchar cc;

    switch ( mod ) {
    case cComparePure:
        m_strpbrk(_current,CastMutable(CPointer,s2),p);
        break;
    case cIgnoreCase:
		while ( NotStrEmpty(_current) )
			{
			p = CastMutable(CPointer,s2);
			cc = m_toupper(*_current);
			m_stripos(p,cc);
			if ( 0 != *p )
				break;
			_current++;
			}
		break;
    case cBreakOnWord:
		while ( NotStrEmpty( _current ) )
			{
			m_streatwhite( _current );
			if ( StrEmpty( _current ) || ( 0 != s_strpos( s2, *_current ) ) )
				break;
			m_strfindwhite( _current );
			}
		break;
    case (cIgnoreCase+cBreakOnWord):
		while ( NotStrEmpty( _current ) )
			{
			m_streatwhite( _current );
			if ( StrEmpty( _current ) || ( 0 != s_stripos( s2, *_current ) ) )
				break;
			m_strfindwhite( _current );
			}
		break;
    default:
        break;
    }
    return *this;
}

CStringConstIterator& CStringConstIterator::FirstNotOf( CConstPointer s2, WULong mod ) 
{
	if ( IsEnd() )
		return *this;

	CPointer p;
	mbchar cc;

    switch ( mod ) {
    case cComparePure:
        m_strpnonbrk(_current,CastMutable(CPointer,s2),p);
        break;
    case cIgnoreCase:
		while ( NotStrEmpty(_current) )
			{
			p = CastMutable(CPointer,s2);
			cc = m_toupper(*_current);
			m_stripos(p,cc);
			if ( 0 == *p )
				break;
			_current++;
			}
		break;
    case cBreakOnWord:
		while ( NotStrEmpty( _current ) )
			{
			m_streatwhite( _current );
			if ( StrEmpty( _current ) || ( 0 == s_strpos( s2, *_current ) ) )
				break;
			m_strfindwhite( _current );
			}
		break;
    case (cIgnoreCase+cBreakOnWord):
		while ( NotStrEmpty( _current ) )
			{
			m_streatwhite( _current );
			if ( StrEmpty( _current ) || ( 0 == s_stripos( s2, *_current ) ) )
				break;
			m_strfindwhite( _current );
			}
		break;
    default:
        break;
    }
    return *this;
}

CStringConstIterator& CStringConstIterator::FirstStrOf(CArray a2, WULong a2Num, WULong mod)
{
	if (IsEnd() || (0 == a2Num) || PtrCheck(a2))
		return *this;

	CPointer s2 = CastAny(CPointer, TFalloc((a2Num + 1) * szchar));
	CPointer p;
	mbchar cc;
	WULong i;

	p = s2;
	for (i = 0; i < a2Num; ++i)
		*p++ = a2[i][0];
	p = nullptr;

	switch (mod) {
	case cComparePure:
		do
		{
			m_strpbrk(_current, s2, p);
			if (IsEnd())
				break;
			i = Castdword(p - s2);
		} while (s_strncmp(_current, a2[i], s_strlen(a2[i], INT_MAX)));
		break;
	case cIgnoreCase:
		do
		{
			while (NotStrEmpty(_current))
			{
				p = CastMutable(CPointer, s2);
				cc = m_toupper(*_current);
				m_stripos(p, cc);
				if (0 != *p)
					break;
				_current++;
			}
			if (IsEnd())
				break;
			i = Castdword(p - s2);
		} while (s_strncmp(_current, a2[i], s_strlen(a2[i], INT_MAX)));
		break;
	case cBreakOnWord:
		do
		{
			while (NotStrEmpty(_current))
			{
				m_streatwhite(_current);
				if (StrEmpty(_current) || (0 != s_strpos(s2, *_current)))
					break;
				m_strfindwhite(_current);
			}
			if (IsEnd())
				break;
			i = Castdword(p - s2);
		} while (s_strncmp(_current, a2[i], s_strlen(a2[i], INT_MAX)));
		break;
	case (cIgnoreCase + cBreakOnWord) :
		do
		{
			while (NotStrEmpty(_current))
			{
				m_streatwhite(_current);
				if (StrEmpty(_current) || (0 != s_stripos(s2, *_current)))
					break;
				m_strfindwhite(_current);
			}
			if (IsEnd())
				break;
			i = Castdword(p - s2);
		} while (s_strncmp(_current, a2[i], s_strlen(a2[i], INT_MAX)));
		break;
	default:
		break;
	}
	TFfree(s2);
	return *this;
}

CStringConstIterator& CStringConstIterator::EatWhite( void ) 
{
	if ( IsEnd() )
		return *this;
    m_streatwhite(_current);
    return *this;
}

CStringConstIterator& CStringConstIterator::EatNumber( Ref(WLong) value, WULong maxdigits )
{
	value = 0;
	if ( IsEnd() )
		return *this;

	int ntype = 0;
	bool bOK = true;

	if ( (*_current) == _T('0') )
	{
		ntype = 1;
		++_current;
		if ( ((*_current) == 0) || ((!(s_isdigit(*_current))) && ((*_current) != _T('x')) && ((*_current) != _T('X'))) )
			return *this;
	}
	if ( ((*_current) == _T('x')) || ((*_current) == _T('X')) )
	{
		ntype = 2;
		++_current;
		if ( ((*_current) == 0) || ((!(s_isdigit(*_current))) && (((*_current) < _T('a')) || ((*_current) > _T('f'))) && (((*_current) < _T('A')) || ((*_current) > _T('F')))) )
			return *this;
	}
	for ( ; bOK && (maxdigits > 0); )
	{
		switch ( ntype )
		{
		case 0:
			switch ( *_current )
			{
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
				value = (value * 10) + ((*_current) - _T('0'));
				break;
			default:
				bOK = false;
				break;
			}
			break;
		case 1:
			switch ( *_current )
			{
			case _T('0'):
			case _T('1'):
			case _T('2'):
			case _T('3'):
			case _T('4'):
			case _T('5'):
			case _T('6'):
			case _T('7'):
				value = (value * 8) + ((*_current) - _T('0'));
				break;
			default:
				bOK = false;
				break;
			}
			break;
		case 2:
			switch ( *_current )
			{
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
				value = (value * 16) + ((*_current) - _T('0'));
				break;
			case _T('A'):
			case _T('B'):
			case _T('C'):
			case _T('D'):
			case _T('E'):
			case _T('F'):
				value = (value * 16) + ((*_current) - _T('A')) + 10;
				break;
			case _T('a'):
			case _T('b'):
			case _T('c'):
			case _T('d'):
			case _T('e'):
			case _T('f'):
				value = (value * 16) + ((*_current) - _T('a')) + 10;
				break;
			default:
				bOK = false;
				break;
			}
			break;
		default:
			bOK = false;
			break;
		}
		if ( bOK )
			++_current;
		--maxdigits;
	}
	return *this;
}

CStringConstIterator& CStringConstIterator::EatDecimal( Ref(WLong) value, WULong maxdigits )
{
	value = 0;
	if ( IsEnd() )
		return *this;

	bool bOK = true;

	for ( ; bOK && (maxdigits > 0); )
	{
		switch ( *_current )
		{
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
			value = (value * 10) + ((*_current) - _T('0'));
			break;
		default:
			bOK = false;
			break;
		}
		if ( bOK )
			++_current;
		--maxdigits;
	}
	return *this;
}

CStringConstIterator& CStringConstIterator::EatOctal( Ref(WLong) value, WULong maxdigits )
{
	value = 0;
	if ( IsEnd() )
		return *this;

	bool bOK = true;

	if ( (*_current) == _T('0') )
	{
		++_current;
		if ( ((*_current) == 0) || (!(s_isdigit(*_current))) )
			return *this;
	}
	for ( ; bOK && (maxdigits > 0); )
	{
		switch ( *_current )
		{
		case _T('0'):
		case _T('1'):
		case _T('2'):
		case _T('3'):
		case _T('4'):
		case _T('5'):
		case _T('6'):
		case _T('7'):
			value = (value * 8) + ((*_current) - _T('0'));
			break;
		default:
			bOK = false;
			break;
		}
		if ( bOK )
			++_current;
		--maxdigits;
	}
	return *this;
}

CStringConstIterator& CStringConstIterator::EatHex( Ref(WLong) value, WULong maxdigits )
{
	value = 0;
	if ( IsEnd() )
		return *this;

	bool bOK = true;

	if ( (*_current) == _T('0') )
	{
		++_current;
		if ( ((*_current) == 0) || (!(s_isdigit(*_current))) )
			return *this;
	}
	if ( ((*_current) == _T('x')) || ((*_current) == _T('X')) )
	{
		++_current;
		if ( ((*_current) == 0) || ((!(s_isdigit(*_current))) && (((*_current) < _T('a')) || ((*_current) > _T('f'))) && (((*_current) < _T('A')) || ((*_current) > _T('F')))) )
			return *this;
	}
	for ( ; bOK && (maxdigits > 0); )
	{
		switch ( *_current )
		{
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
			value = (value * 16) + ((*_current) - _T('0'));
			break;
		case _T('A'):
		case _T('B'):
		case _T('C'):
		case _T('D'):
		case _T('E'):
		case _T('F'):
			value = (value * 16) + ((*_current) - _T('A')) + 10;
			break;
		case _T('a'):
		case _T('b'):
		case _T('c'):
		case _T('d'):
		case _T('e'):
		case _T('f'):
			value = (value * 16) + ((*_current) - _T('a')) + 10;
			break;
		default:
			bOK = false;
			break;
		}
		if ( bOK )
			++_current;
		--maxdigits;
	}
	return *this;
}

CStringConstIterator& CStringConstIterator::EatCharacter( Ref(WChar) value )
{
	value = 0;
	if ( IsEnd() )
		return *this;
	value = *_current++;
	return *this;
}

CStringConstIterator& CStringConstIterator::EatWord( Ref(CPointer) pValue, Ref(WULong) sValue )
{
	pValue = nullptr;
	sValue = 0;
	if ( IsEnd() )
		return *this;
	pValue = _current;
	m_strfindwhite(_current);
	sValue = Castdword(_current - pValue);
	return *this;
}

static CConstPointer month3[] = {
	_T("Jan") , _T("Feb") , _T("Mar") , _T("Apr") , _T("May") , _T("Jun") , _T("Jul") , _T("Aug") , _T("Sep") , _T("Oct") , _T("Nov") , _T("Dec") };

CStringConstIterator& CStringConstIterator::EatMonth3( Ref(WLong) value )
{
	CConstPointer m;
	bool bFound;

	value = -1;
	for ( dword ix = 0; ix < 12; ++ix )
	{
		m = month3[ix];
		bFound = true;
		for ( dword jx = 0; jx < 3; ++jx )
		{
			if ( (*(_current + jx)) == 0 )
				return *this;
			if ( (*(_current + jx)) != m[jx] )
			{
				bFound = false;
				break;
			}
		}
		if ( bFound )
		{
			_current += 3;
			value = ix;
			return *this;
		}
	}
	return *this;
}

void CStringConstIterator::add_offset(WLong offset)
{
	if ( offset == 0 )
		return;
	if ( offset > 0 )
	{
		if ( IsEnd() )
			return;

		WULong len = GetLength();

		if ( Cast(WULong, offset) > len )
			_current += len;
		else
			_current += offset;
	}
	else
	{
		if ( IsBegin() )
			return;

		WULong len = Castdword(_current - _origin);

		if ( Cast(WULong, -offset) > len )
			_current -= len;
		else
			_current += offset;
	}
}

CStringLiteral::CStringLiteral( CConstPointer _str ): 
	_string(CastMutable(CPointer,_str)) 
{
}

CStringLiteral::CStringLiteral( ConstRef(CStringConstIterator) _str ): 
    _string(CastMutable(CPointer, _str.GetCurrent()))
{
}

CStringLiteral::CStringLiteral( ConstRef(CStringLiteral) _str ): 
_string(CastMutable(CPointer,_str.GetString()))
{
}

CStringLiteral::~CStringLiteral() 
{
}

CStringLiteral& CStringLiteral::operator = ( CConstPointer _str )
{
	_string = CastMutable(CPointer,_str);
	return *this;
}

CStringLiteral& CStringLiteral::operator = ( ConstRef(CStringConstIterator) _str )
{
	_string = CastMutable(CPointer,_str.GetCurrent());
	return *this;
}

CStringLiteral& CStringLiteral::operator = ( ConstRef(CStringLiteral) _str )
{
	_string = CastMutable(CPointer,_str.GetString());
	return *this;
}

sword CStringLiteral::Compare( const CStringLiteral& _cmp2, dword num, dword mod) const
{
    if ( PtrCheck(_string) ) {
        if ( PtrCheck(_cmp2._string) )
            return 0;
        return -1;
    }
    if ( PtrCheck(_cmp2._string) )
        return 1;

    int cmp = -1;

    switch ( mod )
    {
    case cComparePure:
        cmp = s_strcmp( _string, _cmp2._string );
        break;
    case cIgnoreCase:
        cmp = s_stricmp( _string, _cmp2._string );
        break;
    case cLimited:
        cmp = s_strncmp( _string, _cmp2._string, num );
        break;
    case cLimited + cIgnoreCase:
        cmp = s_strnicmp( _string, _cmp2._string, num );
        break;
    default:
        break;
    }
	if ( cmp < 0 )
		return -1;
	if ( cmp > 0 )
		return 1;
	return 0;
}

CStringConstIterator CStringLiteral::Begin( void ) const
{
	CStringConstIterator tmp( GetString() );
    return tmp;
}

CStringConstIterator CStringLiteral::End( void ) const
{
    CStringConstIterator tmp( GetString(), GetString() + GetLength() );
    return tmp;
}

