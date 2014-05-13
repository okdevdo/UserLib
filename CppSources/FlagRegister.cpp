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
#include "FlagRegister.h"

CFlagRegister::value_type CFlagRegister::_memsize( value_type n ) 
{
	return CALC_MEMSIZE(n,valuebitcount);
}

CFlagRegister::value_type CFlagRegister::_getnextprime( CFlagRegister::value_type n0 ) 
{
	if ( (mskGENERIC(value_type) >> 1) < n0 )
		return 0;
	value_type n = n0 << 1;
	pointer bp = (pointer)TFalloc(_memsize( n ));
	if ( !bp )
		return 0;
	// sieve of Eratosthenes 
		
	// a[1] := 0
	// for i := 2 to n do a[i] := 1
	// p := 2
	// while (p*p) <= n do {
	//   j := 2p
	//   while (j < n) do {             
	//     a[j] := 0
	//     j := j+p
	//   }
	//   repeat p := p+1 until a[p] = 1   
	// }
	// return(a)
	CFlagRegister _sieve( bp, n );
	value_type p = 2;
	value_type j;
	_sieve.set();
	_sieve.reset( 0 );
	_sieve.reset( 1 );
		
	while ( (p < n) && ((p*p) <= n) ) {
		j = p << 1;
		_sieve.reset( j, p );
		p = _sieve.findnext( TRUE, p+1 );
	}
	p = _sieve.findnext( TRUE, n0 );
	TFfree(bp);
		
	return p;
}

CFlagRegister::CFlagRegister(): _cnt(0), _size(0), _bits(0) 
{
}

CFlagRegister::CFlagRegister( CFlagRegister::pointer p, CFlagRegister::value_type n ): 
    _cnt(_memsize(n)), _size(n), _bits(p) 
{
    if ( _bits ){
		reset();
        return;
	}
    _cnt = 0;
    _size = 0;
}

CFlagRegister::~CFlagRegister() 
{
    _cnt = 0;
    _size = 0;
    _bits = 0;
}

CFlagRegister& CFlagRegister::set() 
{
	pointer fp = _bits;
	value_type ix = _cnt;
    value_type mod = _size % valuebitcount;
	value_type mask = _mskGENERIC(valuebitcount);

	if ( mod ) 
		--ix;
    for ( ; ix > 0; ix--, ++fp ) 
		*fp = mask;
	if ( mod ) 
	{ 
		mask = _mskGENERIC(mod); 
		*fp = mask; 
	}
    return *this;
}

CFlagRegister& CFlagRegister::reset() 
{
	pointer fp = _bits;
	value_type ix = _cnt;

    for ( ; ix > 0; ix--, ++fp )
		*fp = 0;
    return *this;
}

CFlagRegister& CFlagRegister::flip() 
{
	pointer fp = _bits;
	value_type ix = _cnt;
    value_type mod = _size % valuebitcount;
	value_type mask = _mskGENERIC(valuebitcount);

	if ( mod ) 
		--ix;
    for ( ; ix > 0; ix--, ++fp ) 
		*fp = GetBit(BitNot(*fp),mask); 
	if ( mod ) 
	{ 
		mask = _mskGENERIC(mod); 
		*fp = GetBit(BitNot(*fp),mask); 
	}
    return *this;
}

CFlagRegister::value_type CFlagRegister::countbits() const 
{
    value_type x;
    value_type b = 0;
	pointer fp = _bits;
	value_type ix = _cnt;
    value_type mod = _size % valuebitcount;
	value_type mask = _mskGENERIC(valuebitcount);

	if ( mod )
		--ix;
    for ( ; ix > 0; ix--, ++fp ) 
	{ 
		for ( x = GetBit(*fp,mask); x != 0; GETBIT(x,x-1) ) 
			++b; 
	}
	if ( mod ) 
	{ 
		mask = _mskGENERIC(mod); 
		for ( x = GetBit(*fp,mask); x != 0; GETBIT(x,x-1) ) 
			++b; 
	}
    return b;
}

bool CFlagRegister::all() const 
{
	pointer fp = _bits;
	value_type ix = _cnt;
    value_type mod = _size % valuebitcount;
	value_type mask = _mskGENERIC(valuebitcount);

	if ( mod ) 
		--ix;
    for ( ; ix > 0; ix--, ++fp ) 
		if ( !TestBit(*fp,mask) ) 
			return FALSE;
	if ( mod ) 
	{ 
		mask = _mskGENERIC(mod); 
		if ( !TestBit(*fp,mask) ) 
			return FALSE; 
	}
    return TRUE;
}

bool CFlagRegister::any() const 
{
	pointer fp = _bits;
	value_type ix = _cnt;
    value_type mod = _size % valuebitcount;
	value_type mask = _mskGENERIC(valuebitcount);

	if ( mod ) 
		--ix;
    for ( ; ix > 0; ix--, ++fp ) 
		if ( Test(*fp,mask) ) 
			return TRUE;
	if ( mod ) 
	{ 
		mask = _mskGENERIC(mod); 
		if ( Test(*fp,mask) ) 
			return TRUE; 
	}
    return FALSE;
}

CFlagRegister& CFlagRegister::f_or( const CFlagRegister& p ) 
{
    if ( _size == p._size ) 
	{
		pointer fp1 = _bits;
		pointer fp2 = p._bits;
		value_type ix = _cnt;
		value_type mod = _size % valuebitcount;
		value_type mask = _mskGENERIC(valuebitcount);

		if ( mod ) 
			--ix;
		for ( ; ix > 0; ix--, ++fp1, ++fp2 ) 
			*fp1 = BitOr(GetBit(*fp1,mask),GetBit(*fp2,mask));
		if ( mod ) 
		{ 
			mask = _mskGENERIC(mod); 
			*fp1 = BitOr(GetBit(*fp1,mask),GetBit(*fp2,mask)); 
		}
	}
    return *this;
}

CFlagRegister& CFlagRegister::f_and( const CFlagRegister& p ) 
{
    if ( _size == p._size ) 
	{
		pointer fp1 = _bits;
		pointer fp2 = p._bits;
		value_type ix = _cnt;
		value_type mod = _size % valuebitcount;
		value_type mask = _mskGENERIC(valuebitcount);

		if ( mod ) 
			--ix;
		for ( ; ix > 0; ix--, ++fp1, ++fp2 ) 
			*fp1 = BitAnd(GetBit(*fp1,mask),GetBit(*fp2,mask));
		if ( mod ) 
		{ 
			mask = _mskGENERIC(mod); 
			*fp1 = BitAnd(GetBit(*fp1,mask),GetBit(*fp2,mask)); 
		}
	}
    return *this;
}

CFlagRegister& CFlagRegister::f_xor( const CFlagRegister& p ) 
{
    if ( _size == p._size ) 
	{
		pointer fp1 = _bits;
		pointer fp2 = p._bits;
		value_type ix = _cnt;
		value_type mod = _size % valuebitcount;
		value_type mask = _mskGENERIC(valuebitcount);

		if ( mod ) 
			--ix;
		for ( ; ix > 0; ix--, ++fp1, ++fp2 ) 
			*fp1 = BitXor(GetBit(*fp1,mask),GetBit(*fp2,mask));
		if ( mod ) 
		{ 
			mask = _mskGENERIC(mod); 
			*fp1 = BitXor(GetBit(*fp1,mask),GetBit(*fp2,mask)); 
		}
	}
    return *this;
}

bool CFlagRegister::compare( const CFlagRegister& p ) const 
{
    if ( _size == p._size ) 
	{
		pointer fp1 = _bits;
		pointer fp2 = p._bits;
		value_type ix = _cnt;
		value_type mod = _size % valuebitcount;
		value_type mask = _mskGENERIC(valuebitcount);

		if ( mod ) 
			--ix;
		for ( ; ix > 0; ix--, ++fp1, ++fp2 ) 
			if ( !TestBit(GetBit(*fp1,mask),GetBit(*fp2,mask)) ) 
				return FALSE;
		if ( mod ) 
		{ 
			mask = _mskGENERIC(mod); 
			if ( !TestBit(GetBit(*fp1,mask),GetBit(*fp2,mask)) ) 
				return FALSE; 
		}
	}
    return TRUE;
}

CFlagRegister& CFlagRegister::set( CFlagRegister::value_type n, CFlagRegister::value_type off ) 
{
	value_type mask; 
	pointer fp;

	while ( n < _size ) 
	{
		mask = 1 << (n % valuebitcount);
		fp = _bits + (n / valuebitcount);
		SETBIT( *fp, mask );
		n += Min(off,_size);
	}
    return *this;
}

CFlagRegister& CFlagRegister::reset( CFlagRegister::value_type n, CFlagRegister::value_type off ) 
{
	value_type mask; 
	pointer fp;

	while ( n < _size ) 
	{
		mask = 1 << (n % valuebitcount);
		fp = _bits + (n / valuebitcount);
		DELBIT( *fp, mask );
		n += Min(off,_size);
	}
    return *this;
}

CFlagRegister& CFlagRegister::flip( CFlagRegister::value_type n, CFlagRegister::value_type off ) 
{
	value_type mask; 
	pointer fp;

	while ( n < _size ) 
	{
		mask = 1 << (n % valuebitcount);
		fp = _bits + (n / valuebitcount);
		FLIPBIT( *fp, mask );
		n += Min(off,_size);
	}
    return *this;
}

bool CFlagRegister::test( CFlagRegister::value_type n ) const 
{
	value_type mask = 1 << (n % valuebitcount); 
	pointer fp = _bits + (n / valuebitcount);
    bool res = (Test( *fp, mask )?TRUE:FALSE);

    return res;
}

dword CFlagRegister::getnumber( CFlagRegister::value_type at, CFlagRegister::value_type len ) const 
{
	dword res = 0;
	value_type i = 0;

	if ( 32 < len )
		len = 32;
	value_type m = at+len;
 	value_type mask; 
	pointer fp;

	while ( at < m ) {
		mask = 1 << (at % valuebitcount);
		fp = _bits + (at / valuebitcount);
		res += (Test( *fp, mask )?(1 << i++):(i++,0));
		at += Min(1,_size);
	}
    return res;
}

CFlagRegister& CFlagRegister::setnumber( dword v, CFlagRegister::value_type at, CFlagRegister::value_type len ) 
{
	if ( 32 < len )
		len = 32;
	value_type m = at+len;
	value_type mask; 
	pointer fp;

	while ( at < m ) {
		mask = 1 << (at % valuebitcount);
		fp = _bits + (at / valuebitcount);
		((Test( v, 1 )?(SETBIT( *fp, mask )):(DELBIT( *fp, mask ))),v >>= 1);
		at += Min(1,_size);
	}
    return *this;
}

CFlagRegister::value_type CFlagRegister::findnext( bool onoff, CFlagRegister::value_type start ) const 
{
	value_type ix = start / valuebitcount;
	value_type mx = _cnt;
	pointer fp = _bits + ix;
	value_type jx = start % valuebitcount;
	value_type mask;
	value_type x = 0;

	if ( jx ) 
	{
		mask = _mskGENERIC(jx);
		x = *fp++;
		if ( !onoff )
			BITNOT(x);
		GETBIT(x,BitNot(mask));
		if ( ix == (mx-1) ) 
		{
			jx = _size % valuebitcount;
			mask = _mskGENERIC(jx);
			GETBIT(x,mask);
		}
		GETBIT(x,BitNot(GetBit(x,x-1)));
		if ( (0 != x) || (ix == (mx-1)) )
			goto _r0;
		++ix;
	}
	jx = _size % valuebitcount;
	if ( jx )
		--mx;
	for ( ; (0 == x) && (ix < mx); ++ix ) 
	{
		x = *fp++;
		if ( !onoff )
			BITNOT(x);
		GETBIT(x,BitNot(GetBit(x,x-1)));
	}
	--ix;
	if ( (0 == x) && (0 != jx) ) 
	{
		mask = _mskGENERIC(jx);
		x = *fp;
		if ( !onoff )
			BITNOT(x);
		GETBIT(x,mask);
		GETBIT(x,BitNot(GetBit(x,x-1)));
	}
_r0:
	for ( jx = 0; 0 != x; x >>= 1 ) ++jx;
	return (0 == jx)?notfound:((ix * valuebitcount) + (jx-1));
}
