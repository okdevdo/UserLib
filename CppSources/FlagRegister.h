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
 *  \file FlagRegister.h
 *  \brief Bit vector (variable size linear buffer) data structure.
 */
#pragma once

#include "CppSources.h"

class CPPSOURCES_API CFlagRegister {

public:
    typedef dword value_type;
    typedef Ptr(value_type) pointer;

    enum 
	{ 
		valuebitcount = CALC_BITS(value_type),
		notfound = value_type(-1)
	};

    static value_type _memsize( value_type n ); 
    static value_type _getnextprime( value_type n0 );

protected:
    value_type _cnt;
    value_type _size;
    pointer _bits;

public:
    CFlagRegister();
    CFlagRegister( pointer p, value_type n );
    ~CFlagRegister();

public:
    // services

    CFlagRegister& set();
    CFlagRegister& reset();
    CFlagRegister& flip();
    value_type countbits() const;
    bool all() const;
    bool any() const;

    __inline bool none() const
	{
        return !any();
    }

    CFlagRegister& f_or( const CFlagRegister& p );
    CFlagRegister& f_and( const CFlagRegister& p );
    CFlagRegister& f_xor( const CFlagRegister& p );
    bool compare( const CFlagRegister& p ) const;
    CFlagRegister& set( value_type n, value_type off = mskGENERIC(value_type) );
    CFlagRegister& reset( value_type n, value_type off = mskGENERIC(value_type) );
    CFlagRegister& flip( value_type n, value_type off = mskGENERIC(value_type) );
    bool test( value_type n ) const;
	dword getnumber( value_type at, value_type len ) const;
	CFlagRegister& setnumber( dword v, value_type at, value_type len );
	value_type findnext( bool onoff = true, value_type start = 0 ) const;

    __inline bool operator []( value_type n ) const
	{
        return test( n );
    }
	__inline dword operator()( value_type at, value_type len ) const 
	{
		return getnumber( at, len );
	}
	__inline CFlagRegister& operator()( dword v, value_type at, value_type len ) 
	{
		return setnumber( v, at, len );
	}
    __inline CFlagRegister& operator |=( const CFlagRegister& rhs ) 
	{
        return f_or( rhs );
    }
    __inline CFlagRegister& operator &=( const CFlagRegister& rhs ) 
	{
        return f_and( rhs );
    }
    __inline CFlagRegister& operator ^=( const CFlagRegister& rhs ) 
	{
        return f_xor( rhs );
    }
    __inline CFlagRegister& operator ~() 
	{
        return flip();
    }
    __inline bool operator ==( const CFlagRegister& rhs ) const 
	{
        return compare( rhs );
    }
    __inline bool operator !=( const CFlagRegister& rhs ) const 
	{
        return !compare( rhs );
    }

public:
    __inline value_type bitcnt() const 
	{
        return _size;
    }
    __inline value_type memsize() const 
	{
        return _cnt;
    }
    __inline pointer buffer() const 
	{
        return _bits;
    }

};
