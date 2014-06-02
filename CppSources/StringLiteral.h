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
/**
 *  \file StringLiteral.h
 *  \brief Contains definitions for a constant char iterator and string literals.
 */
#pragma once

#include <ostream>

#include "CppSources.h"

class CPPSOURCES_API CStringConstIterator 
{
public:
    CStringConstIterator( CConstPointer _str = 0 );
    CStringConstIterator( CConstPointer _strOrigin, CConstPointer _strCurrent );
	CStringConstIterator(ConstRef(CStringConstIterator) _str);
    ~CStringConstIterator();

    CStringConstIterator& operator = (CConstPointer _str);
    CStringConstIterator& operator = (ConstRef(CStringConstIterator) _str);

public:
    enum 
	{ 
		cComparePure = 0x00, 
		cIgnoreCase = 0x01,
		cLimited = 0x02,
		cBreakOnWord = 0x04
	};

	__inline WULong GetSize() const { return (NULL == _current) ? 0 : (GetLength() + 1); }
	__inline WULong GetLength() const { return (NULL == _current) ? 0 : s_strlen( _current, INT_MAX ); }
	__inline WULong GetDistance() const { return Cast(WULong, _current - _origin); }

	__inline CConstPointer GetCurrent() const { return _current; }
	__inline CConstPointer GetOrigin() const { return _origin; }
    __inline WBool IsBegin() const { return (NULL == _current) ? true : (_current == _origin); }
    __inline WBool IsEnd() const { return (NULL == _current) ? true : (*_current == 0); }
	__inline const WChar operator[] (WLong ix) const { return (NULL == _current) ? 0 : *(_current + ix); }

    CStringConstIterator& operator++();
    CStringConstIterator operator++(int);
    CStringConstIterator& operator--();
    CStringConstIterator operator--(int);
    CStringConstIterator& operator += (WLong offset);
    CStringConstIterator& operator -= (WLong offset);

    CStringConstIterator& Find( mbchar c, WULong mod = cComparePure );
    CStringConstIterator& FindReverse( mbchar c, WULong mod = cComparePure );
    CStringConstIterator& Find( CConstPointer c, WULong mod = cComparePure );
    CStringConstIterator& FirstOf( CConstPointer, WULong = cComparePure );
    CStringConstIterator& FirstNotOf( CConstPointer, WULong = cComparePure );
    CStringConstIterator& FirstStrOf( CArray, WULong, WULong = cComparePure );

    CStringConstIterator& EatWhite( void );
    CStringConstIterator& EatNumber( Ref(WLong) value, WULong maxdigits = ULONG_MAX );
    CStringConstIterator& EatDecimal( Ref(WLong) value, WULong maxdigits = ULONG_MAX );
    CStringConstIterator& EatOctal( Ref(WLong) value, WULong maxdigits = ULONG_MAX );
    CStringConstIterator& EatHex( Ref(WLong) value, WULong maxdigits = ULONG_MAX );
    CStringConstIterator& EatCharacter( Ref(WChar) value );
    CStringConstIterator& EatWord( Ref(CPointer) pValue, Ref(WULong) sValue );
    CStringConstIterator& EatMonth3( Ref(WLong) value );

	sword Compare(const CStringConstIterator& _cmp2, dword num = 0, dword mod = cComparePure) const;
	__inline WBool EQ(const CStringConstIterator& _cmp2, dword num = 0, dword mod = cComparePure) const
	{
		return (Compare(_cmp2, num, mod) == 0);
	}
	__inline WBool LT(const CStringConstIterator& _cmp2, dword num = 0, dword mod = cComparePure) const
	{
		return (Compare(_cmp2, num, mod) < 0);
	}

protected:
    CPointer _origin;
    CPointer _current;

    void add_offset(WLong offset);
};

__inline WBool operator == (const CStringConstIterator & x, const CStringConstIterator & y)
{
	return (x.Compare(y) == 0);
}
__inline WBool operator != (const CStringConstIterator & x, const CStringConstIterator & y)
{
	return (x.Compare(y) != 0);
}
__inline WBool operator < (const CStringConstIterator & x, const CStringConstIterator & y)
{
	return (x.Compare(y) < 0);
}
__inline WBool operator <= (const CStringConstIterator & x, const CStringConstIterator & y)
{
	return (x.Compare(y) <= 0);
}
__inline WBool operator > (const CStringConstIterator & x, const CStringConstIterator & y)
{
	return (x.Compare(y) > 0);
}
__inline WBool operator >= (const CStringConstIterator & x, const CStringConstIterator & y)
{
	return (x.Compare(y) >= 0);
}

__inline Ref(std::ostream) operator << (Ref(std::ostream) os, ConstRef(CStringConstIterator) it)
{
    os << it.GetCurrent();
    return os;
}

#ifdef _UNICODE
__inline Ref(std::wostream) operator << (Ref(std::wostream) os, ConstRef(CStringConstIterator) it)
{
    os << it.GetCurrent();
    return os;
}
#endif

class CPPSOURCES_API CStringLiteral {

public:
    CStringLiteral( CConstPointer _str = 0 );
    CStringLiteral( ConstRef(CStringConstIterator) _str );
	CStringLiteral( ConstRef(CStringLiteral) _str );
    ~CStringLiteral();

    CStringLiteral& operator = ( CConstPointer _str );
    CStringLiteral& operator = ( ConstRef(CStringConstIterator) _str );
    CStringLiteral& operator = ( ConstRef(CStringLiteral) _str );

public:
    enum
    {
        cComparePure = 0x00,
        cIgnoreCase = 0x01,
        cLimited = 0x02
    };

	__inline WULong GetLength() const { return (NULL == _string)?0:s_strlen( _string, INT_MAX ); }
	__inline WULong GetSize() const { return (NULL == _string)?0:(GetLength() + 1); }
	__inline WBool IsEmpty() const { return StrEmpty(_string); }

	__inline CConstPointer GetString() const { return _string; }
	__inline operator CConstPointer() const { return _string; }
	__inline const WBool operator !() const { return IsEmpty(); }

public:
    CStringConstIterator Begin( void ) const;
    CStringConstIterator End( void ) const;

	sword Compare( const CStringLiteral& _cmp2, dword num = 0, dword mod = cComparePure ) const;
    __inline WBool EQ( const CStringLiteral& _cmp2, dword num = 0, dword mod = cComparePure ) const
	{
		return (Compare(_cmp2, num, mod) == 0);
	}
    __inline WBool LT( const CStringLiteral& _cmp2, dword num = 0, dword mod = cComparePure ) const
	{
		return (Compare(_cmp2, num, mod) < 0);
	}

protected:
    CPointer _string;

    friend class CStringConstIterator;
};

__inline WBool operator == ( const CStringLiteral & x, const CStringLiteral & y ) 
{
    return (x.Compare( y ) == 0);
}
__inline WBool operator != ( const CStringLiteral & x, const CStringLiteral & y ) 
{
    return (x.Compare( y ) != 0);
}
__inline WBool operator < ( const CStringLiteral & x, const CStringLiteral & y ) 
{
    return (x.Compare( y ) < 0);
}
__inline WBool operator <= ( const CStringLiteral & x, const CStringLiteral & y ) 
{
    return (x.Compare( y ) <= 0);
}
__inline WBool operator > ( const CStringLiteral & x, const CStringLiteral & y ) 
{
    return (x.Compare( y ) > 0);
}
__inline WBool operator >= ( const CStringLiteral & x, const CStringLiteral & y ) 
{
    return (x.Compare( y ) >= 0);
}

__inline Ref(std::ostream) operator << (Ref(std::ostream) os, ConstRef(CStringLiteral) str)
{
	if ( NotPtrCheck(str.GetString()) )
		os << str.GetString();
    return os;
}

#ifdef _UNICODE
__inline Ref(std::wostream) operator << (Ref(std::wostream) os, ConstRef(CStringLiteral) str)
{
	if ( NotPtrCheck(str.GetString()) )
		os << str.GetString();
    return os;
}
#endif

struct CStringLessFunctor 
{
    inline WBool operator(  ) ( const CStringLiteral & x, const CStringLiteral & y ) const 
	{
        return x.LT( y );
    }
};

struct CStringEqualFunctor 
{
    inline WBool operator(  ) ( const CStringLiteral & x, const CStringLiteral & y ) const 
	{
        return x.EQ( y );
    }
};

template<class _Tv>
struct CStringRProjector 
{
	inline ConstRef(CStringLiteral) operator () ( ConstRef(_Tv) d ) const 
	{
		return Cast(ConstRef(CStringLiteral),d);
	}
};

template<class _Pt>
struct CStringPProjector 
{
	inline ConstRef(CStringLiteral) operator () ( ConstRef(_Pt) d ) const 
	{
		return Cast(ConstRef(CStringLiteral),(*d));
	}
};

struct CStringHashStdFunctor {

    CStringHashStdFunctor( WLong p = 1073741827 ): prime(p) {}

    WLong operator() ( const CStringLiteral & c ) const {
        WULong			i;
		WULong			n = c.GetLength();
		CConstPointer	d = c.GetString();
        WLong			h = 0;

        for ( i = 0; i < n; ++i, ++d )
            h = (h << 2) + *d;

        return ((h >= 0) ? (h % prime) : (-h % prime));
    }

    protected:

    WLong            prime;

};

template <typename T>
class CStringByNameLessFunctor
{
public:
	bool operator()(ConstPtr(T) r1, ConstPtr(T) r2) const
	{
		return r1->get_Name().LT(r2->get_Name());
	}
};

template <typename T>
class CStringByNameIgnoreCaseLessFunctor
{
public:
	bool operator()(ConstPtr(T) r1, ConstPtr(T) r2) const
	{
		return r1->get_Name().LT(r2->get_Name(), 0, CStringLiteral::cIgnoreCase);
	}
};

template <typename T>
class CStringByNameEqualFunctor
{
public:
	bool operator()(ConstPtr(T) r1, ConstPtr(T) r2) const
	{
		return r1->get_Name().EQ(r2->get_Name());
	}
};

template <typename T>
class CStringByNameIgnoreCaseEqualFunctor
{
public:
	bool operator()(ConstPtr(T) r1, ConstPtr(T) r2) const
	{
		return r1->get_Name().EQ(r2->get_Name(), 0, CStringLiteral::cIgnoreCase);
	}
};

