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
#include "cs_pch.h"
#include "utlptr.h"
#include "strutil.h"

static sword __stdcall 
s_lsearch( Pointer table, dword size, ConstPointer ptr, word max, TSearchAndSortFunc func, sword _mode )
	{
	sword ix = max;
	Pointer pt = table;
	sword res;
	sword pix = -1;
	sword pres = 0;
	if ( PtrCheck(table) || PtrCheck(func) || (size == 0) )
		return -1;
	if ( _mode == UTLPTR_INSERTMODE )
		return max - 1;
	for ( ; ix > 0; ix--, pt = l_ptradd( pt, ( long ) size ) )
		{
		res = func( pt, ptr );
		if ( res == 0 )
			return max - ix;
		if ( _mode == UTLPTR_SEARCHMODE )
			{
			if ( ( res > 0 ) && ( ( pix == -1 ) || ( res < pres ) ) )
				{
				pix = max - ix;
				pres = res;
				}					   /* endif */
			}						   /* endif */
		}							   /* endfor */
	if ( _mode == UTLPTR_SEARCHMODE )
		return pix;
	return -1;
	}								   /* end of v_lsearch */

static sword __stdcall
s_delete(Pointer table, dword size, sword ix, WPointer max)
{
	word len;
	Pointer tabP;
	if (PtrCheck(table) || PtrCheck(max) || (size == 0) || (ix < 0) || (ix >= DerefSWPointer(max)))
		return -1;
	tabP = l_ptradd(table, ix * size);
	DerefWPointer(max)--;
	if (0 < (len = DerefWPointer(max) - ix))
		s_memmove_s(tabP, len * size, _fl_ptradd(tabP, size), len * size);
	s_memset(_fl_ptradd(tabP, len * size), 0, size);
	if (ix >= DerefSWPointer(max))
		return ix - 1;
	return ix;
}

sdword __stdcall
_lv_binsert(Array table, ConstPointer ptr, DWPointer max, TSearchAndSortFunc func)
{
	sdword ix = _lv_bsearch(table, ptr, *max, func, UTLPTR_INSERTMODE);
	return _lv_insert(table, ix, ptr, max);
}

sdword __stdcall
_lv_ubinsert(Array table, ConstPointer ptr, DWPointer max, TSearchAndSortUserFunc func, Pointer context)
{
	sdword ix = _lv_ubsearch(table, ptr, *max, func, context, UTLPTR_INSERTMODE);
	return _lv_insert(table, ix, ptr, max);
}

sdword __stdcall 
_lv_linsert( Array table, ConstPointer ptr, DWPointer max, TSearchAndSortFunc func )
	{
	sdword ix = _lv_lsearch( table, ptr, *max, func, UTLPTR_INSERTMODE );
	return _lv_insert( table, ix, ptr, max );
	}

sdword __stdcall 
_lv_insert( Array table, sdword ix, ConstPointer ptr, DWPointer max )
	{
	dword len;
	Handle tabP;
	if ( PtrCheck(table) || PtrCheck(max) || ( ix < -1 ) || ( ix >= DerefSDWPointer( max ) ) )
		return -1L;
	tabP = Cast( Handle, _fl_ptradd( table, ++ix * szPointer ) );
	if ( 0 < (len = DerefDWPointer( max ) - ix) )
		s_memmove_s( _fl_ptradd( tabP, szPointer ), len * szPointer, tabP, len * szPointer );
	DerefDWPointer( max )++;
	DerefPtr(Pointer,tabP) = CastMutable(Pointer,ptr);
	return ix;
	}

sdword __stdcall 
_lv_ninsert( Array table, sdword ix, Array ptr, dword cnt, DWPointer max )
	{
	dword len;
	Handle tabP;
	if ( PtrCheck(table) || PtrCheck(max) || ( cnt == 0 ) || ( ix < -1 ) || ( ix >= DerefSDWPointer( max ) ) )
		return -1L;
	tabP = Cast( Handle, _fl_ptradd( table, ++ix * szPointer ) );
	if ( 0 < (len = DerefDWPointer( max ) - ix) )
		s_memmove_s( _fl_ptradd( tabP, cnt * szPointer ), len * szPointer, tabP, len * szPointer );
	DerefDWPointer( max ) += cnt;
	if ( PtrCheck(ptr) )
		s_memset( tabP, 0, cnt * szPointer );
	else
		s_memmove_s( tabP, cnt * szPointer, ptr, cnt * szPointer );
	return ix + cnt - 1;
	}

sdword __stdcall 
_lv_bdelete( Array table, ConstPointer ptr, DWPointer max, TSearchAndSortFunc func )
	{
	sdword ix = _lv_bsearch( table, ptr, *max, func, UTLPTR_MATCHMODE );
	return _lv_delete( table, ix, max );
	}

sdword __stdcall 
_lv_ldelete( Array table, ConstPointer ptr, DWPointer max, TSearchAndSortFunc func )
	{
	sdword ix = _lv_lsearch( table, ptr, *max, func, UTLPTR_MATCHMODE );
	return _lv_delete( table, ix, max );
	}

sdword __stdcall 
_lv_delete( Array table, sdword ix, DWPointer max )
	{
	dword len;
	Array tabP;
	if ( PtrCheck(table) || PtrCheck(max) || ( ix < 0 ) || ( ix >= DerefSDWPointer( max ) ) )
		return -1;
	tabP = Cast( Handle, _fl_ptradd( table, ix * szPointer ) );
	DerefDWPointer( max )--;
	if ( 0 < (len = ( DerefDWPointer( max ) - ix )) )
		s_memmove_s( tabP, len * szPointer, _fl_ptradd( tabP, szPointer ), len * szPointer );
	DerefPtr(Pointer, _fl_ptradd( tabP, len * szPointer )) = NULL;
	if ( ix >= DerefSDWPointer( max ) )
		return ix - 1;
	return ix;
	}

sdword __stdcall 
_lv_ndelete( Array table, sdword ix, dword cnt, DWPointer max )
	{
	dword len;
	Array tabP;
	if ( PtrCheck(table) || PtrCheck(max) || ( cnt == 0 ) || ( ix < 0 ) || ( ix >= (DerefSDWPointer( max ) - Cast( sdword, ( cnt + 1 ) ) ) ) )
		return -1;
	tabP = Cast( Handle, _fl_ptradd( table, ix * szPointer ) );
	DerefDWPointer( max ) -= cnt;
	if ( 0 < (len = ( DerefDWPointer( max ) - ix )) )
		s_memmove_s( tabP, len * szPointer, _fl_ptradd( tabP, cnt * szPointer ), len * szPointer );
	s_memset( _fl_ptradd( tabP, len * szPointer ), 0, cnt * szPointer );
	if ( ix >= DerefSDWPointer( max ) )
		return ix - cnt;
	return ix;
	}

sdword __stdcall
_lv_bsearch(Array table, ConstPointer ptr, dword max, TSearchAndSortFunc func, sword _mode)
{
	sdword ux = 1;
	sdword ox = max;
	sdword ix = -1;
	sword erg = 0;
	Pointer pt = NULL;

	if (PtrCheck(table) || PtrCheck(func))
		return -1;
	while (ox >= ux)
	{
		ix = ((ox + ux) / 2) - 1;
		erg = func(DerefPtr(Pointer, _fl_ptradd(table, ix * szPointer)), CastMutable(Pointer, ptr));
		if (erg < 0)
			ux = ix + 2;
		else if (erg > 0)
			ox = ix;
		else if (_mode == UTLPTR_INSERTMODE)
		{
			max--;
			if (Cast(dword, ix) < max)
			{
				pt = _fl_ptradd(table, (ix + 1) * szPointer);
				for (; Cast(dword, ix) < max; ix++, pt = _fl_ptradd(pt, szPointer))
					if ((erg = func(DerefPtr(Pointer, pt), CastMutable(Pointer, ptr))) != 0)
						return ix;
			}
			return max;
		}
		else
		{
			if (ix > 0)
			{
				pt = _fl_ptradd(table, (ix - 1) * szPointer);
				for (; ix > 0; ix--, pt = _fl_ptradd(pt, -Cast(sdword, szPointer)))
					if ((erg = func(DerefPtr(Pointer, pt), CastMutable(Pointer, ptr))) != 0)
						return ix;
			}
			return 0;
		}
	}
	if (ix >= 0)
	{
		switch (_mode)
		{
		case UTLPTR_INSERTMODE:
			if (erg > 0)
				--ix;
			break;
		case UTLPTR_SEARCHMODE:
			if (erg < 0)
				++ix;
			break;
		default:
			ix = -1;
			break;
		}
	}
	return ix;
}

sdword __stdcall
_lv_ubsearch(Array table, ConstPointer ptr, dword max, TSearchAndSortUserFunc func, Pointer context, sword _mode)
{
	sdword ux = 1;
	sdword ox = max;
	sdword ix = -1;
	sword erg = 0;
	Pointer pt = NULL;

	if (PtrCheck(table) || PtrCheck(func))
		return -1;
	while (ox >= ux)
	{
		ix = ((ox + ux) / 2) - 1;
		erg = func(DerefPtr(Pointer, _fl_ptradd(table, ix * szPointer)), CastMutable(Pointer, ptr), context);
		if (erg < 0)
			ux = ix + 2;
		else if (erg > 0)
			ox = ix;
		else if (_mode == UTLPTR_INSERTMODE)
		{
			max--;
			if (Cast(dword, ix) < max)
			{
				pt = _fl_ptradd(table, (ix + 1) * szPointer);
				for (; Cast(dword, ix) < max; ix++, pt = _fl_ptradd(pt, szPointer))
					if ((erg = func(DerefPtr(Pointer, pt), CastMutable(Pointer, ptr), context)) != 0)
						return ix;
			}
			return max;
		}
		else
		{
			if (ix > 0)
			{
				pt = _fl_ptradd(table, (ix - 1) * szPointer);
				for (; ix > 0; ix--, pt = _fl_ptradd(pt, -Cast(sdword, szPointer)))
					if ((erg = func(DerefPtr(Pointer, pt), CastMutable(Pointer, ptr), context)) != 0)
						return ix;
			}
			return 0;
		}
	}
	if (ix >= 0)
	{
		switch (_mode)
		{
		case UTLPTR_INSERTMODE:
			if (erg > 0)
				--ix;
			break;
		case UTLPTR_SEARCHMODE:
			if (erg < 0)
				++ix;
			break;
		default:
			ix = -1;
			break;
		}
	}
	return ix;
}

sdword __stdcall 
_lv_lsearch( Array table, ConstPointer ptr, dword max, TSearchAndSortFunc func, sword _mode )
	{
	sdword ix = max;
	Array pt = table;
	sword res;
	sdword pix = -1;
	sdword pres = 0;
	if ( PtrCheck(table) || PtrCheck(func) )
		return -1;
	if ( _mode == UTLPTR_INSERTMODE )
		return max - 1;
	for ( ; ix > 0; ix--, pt = CastAny(Array,_l_ptradd( pt, szPointer ) ) )
		{
		res = func( *pt, ptr );
		if ( res == 0 )
			return max - ix;
		if ( _mode == UTLPTR_SEARCHMODE )
			{
			if ( ( res > 0 ) && ( ( pix == -1 ) || ( res < pres ) ) )
				{
				pix = max - ix;
				pres = res;
				}					   /* endif */
			}						   /* endif */
		}							   /* endfor */
	if ( _mode == UTLPTR_SEARCHMODE )
		return pix;
	return -1;
	}								   /* end of v_lsearch */

sdword __stdcall 
_lv_ulsearch( Array table, ConstPointer ptr, dword max, TSearchAndSortUserFunc func, Pointer user, sword _mode )
	{
	sdword ix = max;
	Array pt = table;
	sword res;
	sdword pix = -1;
	sdword pres = 0;
	if ( PtrCheck(table) || PtrCheck(func) )
		return -1;
	if ( _mode == UTLPTR_INSERTMODE )
		return max - 1;
	for ( ; ix > 0; ix--, pt = CastAny(Array,_l_ptradd( pt, szPointer ) ) )
		{
		res = func( *pt, ptr, user );
		if ( res == 0 )
			return max - ix;
		if ( _mode == UTLPTR_SEARCHMODE )
			{
			if ( ( res > 0 ) && ( ( pix == -1 ) || ( res < pres ) ) )
				{
				pix = max - ix;
				pres = res;
				}					   /* endif */
			}						   /* endif */
		}							   /* endfor */
	if ( _mode == UTLPTR_SEARCHMODE )
		return pix;
	return -1;
	}								   /* end of v_lsearch */

void __stdcall 
_lv_heapsort( Array _heap, dword cnt, TSearchAndSortFunc _cmp )
	{
	dword min;
	dword ix;
	dword i2;
	dword i21;
	dword i;
	dword iix;
	dword sz;
	dword sz1;
	
	for ( iix = cnt / 2; iix > 0; iix-- )
		{
		/* reheap( cnt, iix ); */
		i = iix;
		while ( i <= ( cnt / 2 ) )
			{
			ix = ( i - 1 ) * Castword(szPointer);
			i2 = ( ix << 1 ) + Castword(szPointer);
			min = (_cmp( DerefAnyPtr(Pointer, l_ptradd( _heap, ix )), DerefAnyPtr(Pointer, l_ptradd( _heap, i2 )) ) <= 0) ? ix : i2;
			if ( ( ( i << 1 ) + 1 ) <= cnt )
				{
				i21 = i2 + Castword(szPointer);
				if ( _cmp( DerefAnyPtr(Pointer, l_ptradd( _heap, i21 )), DerefAnyPtr(Pointer, l_ptradd( _heap, min )) ) <= 0 )
					min = i21;
				}
			if ( min == ix )
				break;
			v_swapbytes( l_ptradd( _heap, ix ), l_ptradd( _heap, min ), Castword(szPointer) );
			i = ( min + Castword(szPointer) ) / Castword(szPointer);
			}
		}
	for ( iix = cnt - 1, sz1 = sz = ( cnt - 1 ) * Castword(szPointer); iix > 0; iix--, sz -= Castword(szPointer) )
		{
		v_swapbytes( _heap, l_ptradd( _heap, sz ), Castword(szPointer) );
		/* reheap( iix, 1 ); */
		i = 1;
		while ( i <= ( iix / 2 ) )
			{
			ix = ( i - 1 ) * Castword(szPointer);
			i2 = ( ix << 1 ) + Castword(szPointer);
			min = (_cmp( DerefAnyPtr(Pointer, l_ptradd( _heap, ix )), DerefAnyPtr(Pointer, l_ptradd( _heap, i2 )) ) <= 0) ? ix : i2;
			if ( ( ( i << 1 ) + 1 ) <= iix )
				{
				i21 = i2 + Castword(szPointer);
				if ( _cmp( DerefAnyPtr(Pointer, l_ptradd( _heap, i21 )), DerefAnyPtr(Pointer, l_ptradd( _heap, min )) ) <= 0 )
					min = i21;
				}
			if ( min == ix )
				break;
			v_swapbytes( l_ptradd( _heap, ix ), l_ptradd( _heap, min ), Castword(szPointer) );
			i = ( min + Castword(szPointer) ) / Castword(szPointer);
			}
		}
	for ( iix = cnt / 2, sz = 0 /* , sz1 = (cnt-1) * Castword(szPointer) */ ; iix > 0; iix--, sz += Castword(szPointer), sz1 -= Castword(szPointer) )
		v_swapbytes( l_ptradd( _heap, sz ), l_ptradd( _heap, sz1 ), Castword(szPointer) );
	}

void __stdcall 
_lv_uheapsort( Array _heap, dword cnt, TSearchAndSortUserFunc _cmp, Pointer user )
	{
	dword min;
	dword ix;
	dword i2;
	dword i21;
	dword i;
	dword iix;
	dword sz;
	dword sz1;
	
	for ( iix = cnt / 2; iix > 0; iix-- )
		{
		/* reheap( cnt, iix ); */
		i = iix;
		while ( i <= ( cnt / 2 ) )
			{
			ix = ( i - 1 ) * Castword(szPointer);
			i2 = ( ix << 1 ) + Castword(szPointer);
			min = (_cmp( DerefAnyPtr(Pointer, l_ptradd( _heap, ix )), DerefAnyPtr(Pointer, l_ptradd( _heap, i2 )), user ) <= 0) ? ix : i2;
			if ( ( ( i << 1 ) + 1 ) <= cnt )
				{
				i21 = i2 + Castword(szPointer);
				if ( _cmp( DerefAnyPtr(Pointer, l_ptradd( _heap, i21 )), DerefAnyPtr(Pointer, l_ptradd( _heap, min )), user ) <= 0 )
					min = i21;
				}
			if ( min == ix )
				break;
			v_swapbytes( l_ptradd( _heap, ix ), l_ptradd( _heap, min ), Castword(szPointer) );
			i = ( min + Castword(szPointer) ) / Castword(szPointer);
			}
		}
	for ( iix = cnt - 1, sz1 = sz = ( cnt - 1 ) * Castword(szPointer); iix > 0; iix--, sz -= Castword(szPointer) )
		{
		v_swapbytes( _heap, l_ptradd( _heap, sz ), Castword(szPointer) );
		/* reheap( iix, 1 ); */
		i = 1;
		while ( i <= ( iix / 2 ) )
			{
			ix = ( i - 1 ) * Castword(szPointer);
			i2 = ( ix << 1 ) + Castword(szPointer);
			min = (_cmp( DerefAnyPtr(Pointer, l_ptradd( _heap, ix )), DerefAnyPtr(Pointer, l_ptradd( _heap, i2 )), user ) <= 0) ? ix : i2;
			if ( ( ( i << 1 ) + 1 ) <= iix )
				{
				i21 = i2 + Castword(szPointer);
				if ( _cmp( DerefAnyPtr(Pointer, l_ptradd( _heap, i21 )), DerefAnyPtr(Pointer, l_ptradd( _heap, min )), user ) <= 0 )
					min = i21;
				}
			if ( min == ix )
				break;
			v_swapbytes( l_ptradd( _heap, ix ), l_ptradd( _heap, min ), Castword(szPointer) );
			i = ( min + Castword(szPointer) ) / Castword(szPointer);
			}
		}
	for ( iix = cnt / 2, sz = 0 /* , sz1 = (cnt-1) * Castword(szPointer) */ ; iix > 0; iix--, sz += Castword(szPointer), sz1 -= Castword(szPointer) )
		v_swapbytes( l_ptradd( _heap, sz ), l_ptradd( _heap, sz1 ), Castword(szPointer) );
	}

static void __stdcall
__lv_quicksort(Array _heap, sdword l, sdword r, TSearchAndSortFunc _cmp)
{
	Array tmp = CastAny(Array, _l_ptradd(_heap, r * szPointer));
	Array p1;
	Array p2;
	sdword k;
	sdword i = l - 1;
	sdword j = r;
	sdword p = l - 1;
	sdword q = r;

	if ( r <= l )
		return;
	for ( ;; )
	{
		p1 = CastAny(Array, _l_ptradd(_heap, ++i * szPointer));
		while ( 0 > _cmp(DerefAnyPtr(Pointer,p1), DerefAnyPtr(Pointer,tmp)) )
			p1 = CastAny(Array, _l_ptradd(_heap, ++i * szPointer));
		p1 = CastAny(Array, _l_ptradd(_heap, --j * szPointer));
		while ( 0 < _cmp(DerefAnyPtr(Pointer,p1), DerefAnyPtr(Pointer,tmp)) )
		{
			if ( j == l )
				break;
			p1 = CastAny(Array, _l_ptradd(_heap, --j * szPointer));
		}
		if ( i >= j )
			break;
		p1 = CastAny(Array, _l_ptradd(_heap, i * szPointer));
		p2 = CastAny(Array, _l_ptradd(_heap, j * szPointer));
		v_swapbytes(p1, p2, szPointer);
		if ( 0 == _cmp(DerefAnyPtr(Pointer,p1), DerefAnyPtr(Pointer,tmp)) )
		{
			p++;
			v_swapbytes(CastAny(Array, _l_ptradd(_heap, p * szPointer)), p1, szPointer);
		}
		if ( 0 == _cmp(DerefAnyPtr(Pointer,p2), DerefAnyPtr(Pointer,tmp)) )
		{
			q--;
			v_swapbytes(CastAny(Array, _l_ptradd(_heap, q * szPointer)), p2, szPointer);
		}
	}
	p1 = CastAny(Array, _l_ptradd(_heap, i * szPointer));
	p2 = CastAny(Array, _l_ptradd(_heap, r * szPointer));
	v_swapbytes(p1, p2, szPointer);
	j = i - 1;
	i = i + 1;
	for ( k = l; k <= p; k++, j-- )
	{
		p1 = CastAny(Array, _l_ptradd(_heap, k * szPointer));
		p2 = CastAny(Array, _l_ptradd(_heap, j * szPointer));
		v_swapbytes(p1, p2, szPointer);
	}
	for ( k = r - 1; k >= q; k--, i++ )
	{
		p1 = CastAny(Array, _l_ptradd(_heap, k * szPointer));
		p2 = CastAny(Array, _l_ptradd(_heap, i * szPointer));
		v_swapbytes(p1, p2, szPointer);
	}
	__lv_quicksort(_heap, l, j, _cmp);
	__lv_quicksort(_heap, i, r, _cmp);
}

void __stdcall 
_lv_quicksort( Array _heap, dword cnt, TSearchAndSortFunc _cmp )
{
	__lv_quicksort(_heap, 0, cnt - 1, _cmp);
}

static void __stdcall
__lv_uquicksort(Array _heap, sdword l, sdword r, Pointer user, TSearchAndSortUserFunc _cmp)
{
	Array tmp = CastAny(Array, _l_ptradd(_heap, r * szPointer));
	Array p1;
	Array p2;
	sdword k;
	sdword i = l - 1;
	sdword j = r;
	sdword p = l - 1;
	sdword q = r;

	if ( r <= l )
		return;
	for ( ;; )
	{
		p1 = CastAny(Array, _l_ptradd(_heap, ++i * szPointer));
		while ( 0 > _cmp(DerefAnyPtr(Pointer,p1), DerefAnyPtr(Pointer,tmp), user) )
			p1 = CastAny(Array, _l_ptradd(_heap, ++i * szPointer));
		p1 = CastAny(Array, _l_ptradd(_heap, --j * szPointer));
		while ( 0 < _cmp(DerefAnyPtr(Pointer,p1), DerefAnyPtr(Pointer,tmp), user) )
		{
			if ( j == l )
				break;
			p1 = CastAny(Array, _l_ptradd(_heap, --j * szPointer));
		}
		if ( i >= j )
			break;
		p1 = CastAny(Array, _l_ptradd(_heap, i * szPointer));
		p2 = CastAny(Array, _l_ptradd(_heap, j * szPointer));
		v_swapbytes(p1, p2, szPointer);
		if ( 0 == _cmp(DerefAnyPtr(Pointer,p1), DerefAnyPtr(Pointer,tmp), user) )
		{
			p++;
			v_swapbytes(CastAny(Array, _l_ptradd(_heap, p * szPointer)), p1, szPointer);
		}
		if ( 0 == _cmp(DerefAnyPtr(Pointer,p2), DerefAnyPtr(Pointer,tmp), user) )
		{
			q--;
			v_swapbytes(CastAny(Array, _l_ptradd(_heap, q * szPointer)), p2, szPointer);
		}
	}
	p1 = CastAny(Array, _l_ptradd(_heap, i * szPointer));
	p2 = CastAny(Array, _l_ptradd(_heap, r * szPointer));
	v_swapbytes(p1, p2, szPointer);
	j = i - 1;
	i = i + 1;
	for ( k = l; k <= p; k++, j-- )
	{
		p1 = CastAny(Array, _l_ptradd(_heap, k * szPointer));
		p2 = CastAny(Array, _l_ptradd(_heap, j * szPointer));
		v_swapbytes(p1, p2, szPointer);
	}
	for ( k = r - 1; k >= q; k--, i++ )
	{
		p1 = CastAny(Array, _l_ptradd(_heap, k * szPointer));
		p2 = CastAny(Array, _l_ptradd(_heap, i * szPointer));
		v_swapbytes(p1, p2, szPointer);
	}
	__lv_uquicksort(_heap, l, j, user, _cmp);
	__lv_uquicksort(_heap, i, r, user, _cmp);
}

void __stdcall 
_lv_uquicksort( Array _heap, dword cnt, TSearchAndSortUserFunc _cmp, Pointer user )
{
	__lv_uquicksort(_heap, 0, cnt - 1, user, _cmp);
}

sdword __stdcall 
_ls_binsert( Pointer table, dword size, ConstPointer ptr, DWPointer max, TSearchAndSortFunc func )
	{
	sdword ix = _ls_bsearch( table, size, ptr, *max, func, UTLPTR_INSERTMODE );
	return _ls_insert( table, size, ix, ptr, max );
	}

sdword __stdcall 
_ls_linsert( Pointer table, dword size, ConstPointer ptr, DWPointer max, TSearchAndSortFunc func )
	{
	sdword ix = _ls_lsearch( table, size, ptr, *max, func, UTLPTR_INSERTMODE );
	return _ls_insert( table, size, ix, ptr, max );
	}

sdword __stdcall 
_ls_insert( Pointer table, dword size, sdword ix, ConstPointer ptr, DWPointer max )
	{
	dword len;
	Pointer tabP;
	if ( PtrCheck(table) || PtrCheck(max) || (size == 0) || ( ix < -1L ) || ( ix >= DerefSDWPointer( max ) ) )
		return -1L;
	tabP = _fl_ptradd( table, (++ix) * size );
	if ( 0 < (len = DerefDWPointer( max ) - ix) )
		s_memmove_s( _fl_ptradd( tabP, size ), len * size, tabP, len * size );
	DerefDWPointer( max )++;
	if ( PtrCheck(ptr) )
		s_memset( tabP, 0, size );
	else
		s_memmove_s( tabP, size, CastMutable(Pointer,ptr), size );
	return ix;
	}

sdword __stdcall 
_ls_ninsert( Pointer table, dword size, sdword ix, ConstPointer ptr, dword cnt, DWPointer max )
	{
	dword len;
	Pointer tabP;
	if ( PtrCheck(table) || PtrCheck(max) || (size == 0) || (cnt == 0) || ( ix < -1L ) || ( ix >= DerefSDWPointer( max ) ) )
		return -1L;
	tabP = _fl_ptradd( table, (++ix) * size );
	if ( 0 < (len = DerefDWPointer( max ) - ix) )
		s_memmove_s( _fl_ptradd( tabP, cnt * size ), len * size, tabP, len * size );
	DerefDWPointer( max ) += cnt;
	if ( PtrCheck(ptr) )
		s_memset( tabP, 0, cnt * size );
	else
		s_memmove_s( tabP, cnt * size, CastMutable(Pointer,ptr), cnt * size );
	return ix + cnt - 1;
	}
	
sdword __stdcall 
_ls_bdelete( Pointer table, dword size, ConstPointer ptr, DWPointer max, TSearchAndSortFunc func )
	{
	sdword ix = _ls_bsearch( table, size, ptr, *max, func, UTLPTR_MATCHMODE );
	return _ls_delete( table, size, ix, max );
	}

sdword __stdcall 
_ls_ldelete( Pointer table, dword size, ConstPointer ptr, DWPointer max, TSearchAndSortFunc func )
	{
	sdword ix = _ls_lsearch( table, size, ptr, *max, func, UTLPTR_MATCHMODE );
	return _ls_delete( table, size, ix, max );
	}

sword __stdcall 
s_ldelete( Pointer table, dword size, ConstPointer ptr, WPointer max, TSearchAndSortFunc func )
	{
	sword ix = s_lsearch( table, size, ptr, *max, func, UTLPTR_MATCHMODE );
	return s_delete( table, size, ix, max );
	}

sdword __stdcall 
_ls_delete( Pointer table, dword size, sdword ix, DWPointer max )
	{
	dword len;
	Pointer tabP;
	if ( PtrCheck(table) || PtrCheck(max) || (size == 0) || ( ix < 0 ) || ( ix >= DerefSDWPointer( max ) ) )
		return -1;
	tabP = _fl_ptradd( table, ix * size );
	DerefDWPointer( max )--;
	if ( 0 < (len = DerefDWPointer( max ) - ix) )
		s_memmove_s( tabP, len * size, _fl_ptradd( tabP, size ), len * size );
	s_memset( _fl_ptradd( tabP, len * size ), 0, size );
	if ( ix >= DerefSDWPointer( max ) )
		return ix - 1;
	return ix;
	}

sdword __stdcall 
_ls_ndelete( Pointer table, dword size, sdword ix, dword cnt, DWPointer max )
	{
	dword len;
	Pointer tabP;
	if ( PtrCheck(table) || PtrCheck(max) || (size == 0) || (cnt == 0) || ( ix < 0 ) || ( ix >= (DerefSDWPointer( max ) - Cast( sdword, (cnt + 1) ) ) ) )
		return -1;
	tabP = _fl_ptradd( table, ix * size );
	DerefDWPointer( max ) -= cnt;
	if ( (len = DerefDWPointer( max ) - ix) != 0 )
		s_memmove_s( tabP, len * size, _fl_ptradd( tabP, cnt * size ), len * size );
	s_memset( _fl_ptradd( tabP, len * size ), 0, cnt * size );
	if ( ix >= DerefSDWPointer( max ) )
		return ix - cnt;
	return ix;
	}
	
sdword __stdcall 
_ls_bsearch( Pointer table, dword size, ConstPointer ptr, dword max, TSearchAndSortFunc func, sword _mode )
	{
	sdword ux = 1;
	sdword ox = max;
	sdword ix = -1;
	sword erg;
	Pointer pt;
	if ( PtrCheck(table) || PtrCheck(func) || (size == 0) )
		return -1;
	while ( ox >= ux )
		{
		ix = ( ( ox + ux ) / 2 ) - 1;
		erg = func( _fl_ptradd( table, size * ix ), CastMutable(Pointer, ptr) );
		if ( erg < 0 )
			ux = ix + 2;
		else if ( erg > 0 )
			ox = ix;
		else if ( _mode == UTLPTR_INSERTMODE )
			{
			max--;
			if ( Cast(dword,ix) < max )
				{
				pt = _fl_ptradd( table, size * (ix + 1) );
				for ( ; Cast(dword,ix) < max; ix++, pt = _fl_ptradd( pt, size ) )
					if ( ( erg = func( pt, CastMutable(Pointer, ptr) ) ) != 0 )
						return ix;
				}
			return max;
		} else
			{
			if ( ix > 0 )
				{
				pt = _fl_ptradd( table, size * (ix - 1) );
				for ( ; ix > 0; ix--, pt = _fl_ptradd( pt, -Cast(sdword,size) ) )
					if ( ( erg = func( pt, CastMutable(Pointer, ptr) ) ) != 0 )
						return ix;
				}
			return 0;
			}
		}
	if ( ix >= 0 )
		{
		switch ( _mode )
			{
		case UTLPTR_INSERTMODE:
			if ( func( _fl_ptradd( table, size * ix ), CastMutable(Pointer, ptr) ) > 0 )
				ix--;
			break;
		case UTLPTR_SEARCHMODE:
			if ( func( _fl_ptradd( table, size * ix ), CastMutable(Pointer, ptr) ) >= 0 )
				break;
			if ( Cast(dword,ix) < ( max - 1 ) )
				ix++;
			break;
		default:
			ix = -1;
			break;
			}							   /* endswitch */
		}
	return ix;
	}								   /* end of s_bsearch */

sdword __stdcall 
_ls_ubsearch( Pointer table, dword size, ConstPointer ptr, dword max, TSearchAndSortUserFunc func, Pointer context, sword _mode )
	{
	sdword ux = 1;
	sdword ox = max;
	sdword ix = -1;
	sword erg;
	Pointer pt;
	if ( PtrCheck(table) || PtrCheck(func) || (size == 0) )
		return -1;
	while ( ox >= ux )
		{
		ix = ( ( ox + ux ) / 2 ) - 1;
		erg = func( _fl_ptradd( table, size * ix ), CastMutable(Pointer, ptr), context );
		if ( erg < 0 )
			ux = ix + 2;
		else if ( erg > 0 )
			ox = ix;
		else if ( _mode == UTLPTR_INSERTMODE )
			{
			max--;
			if ( Cast(dword,ix) < max )
				{
				pt = _fl_ptradd( table, size * (ix + 1) );
				for ( ; Cast(dword,ix) < max; ix++, pt = _fl_ptradd( pt, size ) )
					if ( ( erg = func( pt, CastMutable(Pointer, ptr), context ) ) != 0 )
						return ix;
				}
			return max;
		} else
			{
			if ( ix > 0 )
				{
				pt = _fl_ptradd( table, size * (ix - 1) );
				for ( ; ix > 0; ix--, pt = _fl_ptradd( pt, -Cast(sdword,size) ) )
					if ( ( erg = func( pt, CastMutable(Pointer, ptr), context ) ) != 0 )
						return ix;
				}
			return 0;
			}
		}
	if ( ix >= 0 )
		{
		switch ( _mode )
			{
		case UTLPTR_INSERTMODE:
			if ( func( _fl_ptradd( table, size * ix ), CastMutable(Pointer, ptr), context ) > 0 )
				ix--;
			break;
		case UTLPTR_SEARCHMODE:
			if ( func( _fl_ptradd( table, size * ix ), CastMutable(Pointer, ptr), context ) >= 0 )
				break;
			if ( Cast(dword,ix) < ( max - 1 ) )
				ix++;
			break;
		default:
			ix = -1;
			break;
			}							   /* endswitch */
		}
	return ix;
	}								   /* end of s_bsearch */

sdword __stdcall 
_ls_lsearch( Pointer table, dword size, ConstPointer ptr, dword max, TSearchAndSortFunc func, sword _mode )
	{
	sdword ix = max;
	Pointer pt = table;
	sword res;
	sdword pix = -1;
	sdword pres = 0;
	if ( PtrCheck(table) || PtrCheck(func) || (size == 0) )
		return -1;
	if ( _mode == UTLPTR_INSERTMODE )
		return max - 1;
	for ( ; ix > 0; ix--, pt = _fl_ptradd( pt, size ) )
		{
		res = func( pt, ptr );
		if ( res == 0 )
			return max - ix;
		if ( _mode == UTLPTR_SEARCHMODE )
			{
			if ( ( res > 0 ) && ( ( pix == -1 ) || ( res < pres ) ) )
				{
				pix = max - ix;
				pres = res;
				}					   /* endif */
			}						   /* endif */
		}							   /* endfor */
	if ( _mode == UTLPTR_SEARCHMODE )
		return pix;
	return -1;
	}								   /* end of v_lsearch */

sdword __stdcall 
_ls_ulsearch( Pointer table, dword size, ConstPointer ptr, dword max, TSearchAndSortUserFunc func, Pointer user, sword _mode )
	{
	sdword ix = max;
	Pointer pt = table;
	sword res;
	sdword pix = -1;
	sdword pres = 0;
	if ( PtrCheck(table) || PtrCheck(func) || (size == 0) )
		return -1;
	if ( _mode == UTLPTR_INSERTMODE )
		return max - 1;
	for ( ; ix > 0; ix--, pt = _fl_ptradd( pt, size ) )
		{
		res = func( pt, ptr, user );
		if ( res == 0 )
			return max - ix;
		if ( _mode == UTLPTR_SEARCHMODE )
			{
			if ( ( res > 0 ) && ( ( pix == -1 ) || ( res < pres ) ) )
				{
				pix = max - ix;
				pres = res;
				}					   /* endif */
			}						   /* endif */
		}							   /* endfor */
	if ( _mode == UTLPTR_SEARCHMODE )
		return pix;
	return -1;
	}								   /* end of v_lsearch */

void __stdcall
_ls_heapsort( Pointer _heap, dword _size, dword cnt, TSearchAndSortFunc _cmp )
	{
	dword min;
	dword ix;
	dword i2;
	dword i21;
	dword i;
	dword iix;
	dword sz;
	dword sz1;
	
	for ( iix = cnt / 2; iix > 0; iix-- )
		{
		/* reheap( cnt, iix ); */
		i = iix;
		while ( i <= ( cnt / 2 ) )
			{
			ix = ( i - 1 ) * _size;
			i2 = ( ix << 1 ) + _size;
			min = (_cmp( _fl_ptradd( _heap, ix ), _fl_ptradd( _heap, i2 ) ) <= 0) ? ix : i2;
			if ( ( ( i << 1 ) + 1 ) <= cnt )
				{
				i21 = i2 + _size;
				if ( _cmp( _fl_ptradd( _heap, i21 ), _fl_ptradd( _heap, min ) ) <= 0 )
					min = i21;
				}
			if ( min == ix )
				break;
			_lv_swapbytes( _fl_ptradd( _heap, ix ), _fl_ptradd( _heap, min ), _size );
			i = ( min + _size ) / _size;
			}
		}
	for ( iix = cnt - 1, sz1 = sz = ( cnt - 1 ) * _size; iix > 0; iix--, sz -= _size )
		{
		_lv_swapbytes( _heap, _fl_ptradd( _heap, sz ), _size );
		/* reheap( iix, 1 ); */
		i = 1;
		while ( i <= ( iix / 2 ) )
			{
			ix = ( i - 1 ) * _size;
			i2 = ( ix << 1 ) + _size;
			min = (_cmp( _fl_ptradd( _heap, ix ), _fl_ptradd( _heap, i2 ) ) <= 0) ? ix : i2;
			if ( ( ( i << 1 ) + 1 ) <= iix )
				{
				i21 = i2 + _size;
				if ( _cmp( _fl_ptradd( _heap, i21 ), _fl_ptradd( _heap, min ) ) <= 0 )
					min = i21;
				}
			if ( min == ix )
				break;
			_lv_swapbytes( _fl_ptradd( _heap, ix ), _fl_ptradd( _heap, min ), _size );
			i = ( min + _size ) / _size;
			}
		}
	for ( iix = cnt / 2, sz = 0 /* , sz1 = (cnt-1) * Castdword(_size) */ ; iix > 0; iix--, sz += _size, sz1 -= _size )
		_lv_swapbytes( _fl_ptradd( _heap, sz ), _fl_ptradd( _heap, sz1 ), _size );
	}

void __stdcall 
_ls_uheapsort( Pointer _heap, dword _size, dword cnt, TSearchAndSortUserFunc _cmp, Pointer user )
	{
	dword min;
	dword ix;
	dword i2;
	dword i21;
	dword i;
	dword iix;
	dword sz;
	dword sz1;
	
	for ( iix = cnt / 2; iix > 0; iix-- )
		{
		/* reheap( cnt, iix ); */
		i = iix;
		while ( i <= ( cnt / 2 ) )
			{
			ix = ( i - 1 ) * _size;
			i2 = ( ix << 1 ) + _size;
			min = (_cmp( _fl_ptradd( _heap, ix ), _fl_ptradd( _heap, i2 ), user ) <= 0) ? ix : i2;
			if ( ( ( i << 1 ) + 1 ) <= cnt )
				{
				i21 = i2 + _size;
				if ( _cmp( _fl_ptradd( _heap, i21 ), _fl_ptradd( _heap, min ), user ) <= 0 )
					min = i21;
				}
			if ( min == ix )
				break;
			_lv_swapbytes( _fl_ptradd( _heap, ix ), _fl_ptradd( _heap, min ), _size );
			i = ( min + _size ) / _size;
			}
		}
	for ( iix = cnt - 1, sz1 = sz = ( cnt - 1 ) * _size; iix > 0; iix--, sz -= _size )
		{
		_lv_swapbytes( _heap, _fl_ptradd( _heap, sz ), _size );
		/* reheap( iix, 1 ); */
		i = 1;
		while ( i <= ( iix / 2 ) )
			{
			ix = ( i - 1 ) * _size;
			i2 = ( ix << 1 ) + _size;
			min = (_cmp( _fl_ptradd( _heap, ix ), _fl_ptradd( _heap, i2 ), user ) <= 0) ? ix : i2;
			if ( ( ( i << 1 ) + 1 ) <= iix )
				{
				i21 = i2 + _size;
				if ( _cmp( _fl_ptradd( _heap, i21 ), _fl_ptradd( _heap, min ), user ) <= 0 )
					min = i21;
				}
			if ( min == ix )
				break;
			_lv_swapbytes( _fl_ptradd( _heap, ix ), _fl_ptradd( _heap, min ), _size );
			i = ( min + _size ) / _size;
			}
		}
	for ( iix = cnt / 2, sz = 0 /* , sz1 = (cnt-1) * _size */ ; iix > 0; iix--, sz += _size, sz1 -= _size )
		_lv_swapbytes( _fl_ptradd( _heap, sz ), _fl_ptradd( _heap, sz1 ), _size );
	}

static void __stdcall
__ls_quicksort(Pointer _heap, dword datasize, sdword l, sdword r, TSearchAndSortFunc _cmp)
{
	Pointer tmp = _l_ptradd(_heap, r * datasize);
	Pointer p1;
	Pointer p2;
	sdword k;
	sdword i = l - 1;
	sdword j = r;
	sdword p = l - 1;
	sdword q = r;

	if ( r <= l )
		return;
	for ( ;; )
	{
		p1 = _l_ptradd(_heap, ++i * datasize);
		while ( 0 > _cmp(p1, tmp) )
			p1 = _l_ptradd(_heap, ++i * datasize);
		p1 = _l_ptradd(_heap, --j * datasize);
		while ( 0 < _cmp(p1, tmp) )
		{
			if ( j == l )
				break;
			p1 = _l_ptradd(_heap, --j * datasize);
		}
		if ( i >= j )
			break;
		p1 = _l_ptradd(_heap, i * datasize);
		p2 = _l_ptradd(_heap, j * datasize);
		_lv_swapbytes(p1, p2, datasize);
		if ( 0 == _cmp(p1, tmp) )
		{
			p++;
			_lv_swapbytes(_l_ptradd(_heap, p * datasize), p1, datasize);
		}
		if ( 0 == _cmp(p2, tmp) )
		{
			q--;
			_lv_swapbytes(_l_ptradd(_heap, q * datasize), p2, datasize);
		}
	}
	p1 = _l_ptradd(_heap, i * datasize);
	p2 = _l_ptradd(_heap, r * datasize);
	_lv_swapbytes(p1, p2, datasize);
	j = i - 1;
	i = i + 1;
	for ( k = l; k <= p; k++, j-- )
	{
		p1 = _l_ptradd(_heap, k * datasize);
		p2 = _l_ptradd(_heap, j * datasize);
		_lv_swapbytes(p1, p2, datasize);
	}
	for ( k = r - 1; k >= q; k--, i++ )
	{
		p1 = _l_ptradd(_heap, k * datasize);
		p2 = _l_ptradd(_heap, i * datasize);
		_lv_swapbytes(p1, p2, datasize);
	}
	__ls_quicksort(_heap, datasize, l, j, _cmp);
	__ls_quicksort(_heap, datasize, i, r, _cmp);
}

void __stdcall 
_ls_quicksort( Pointer _heap, dword datasize, dword cnt, TSearchAndSortFunc _cmp )
{
	__ls_quicksort(_heap, datasize, 0, cnt - 1, _cmp);
}

static void __stdcall
__ls_uquicksort(Pointer _heap, dword datasize, sdword l, sdword r, TSearchAndSortUserFunc _cmp, Pointer user)
{
	Pointer tmp = _l_ptradd(_heap, r * datasize);
	Pointer p1;
	Pointer p2;
	sdword k;
	sdword i = l - 1;
	sdword j = r;
	sdword p = l - 1;
	sdword q = r;

	if ( r <= l )
		return;
	for ( ;; )
	{
		p1 = _l_ptradd(_heap, ++i * datasize);
		while ( 0 > _cmp(p1, tmp, user) )
			p1 = _l_ptradd(_heap, ++i * datasize);
		p1 = _l_ptradd(_heap, --j * datasize);
		while ( 0 < _cmp(p1, tmp, user) )
		{
			if ( j == l )
				break;
			p1 = _l_ptradd(_heap, --j * datasize);
		}
		if ( i >= j )
			break;
		p1 = _l_ptradd(_heap, i * datasize);
		p2 = _l_ptradd(_heap, j * datasize);
		_lv_swapbytes(p1, p2, datasize);
		if ( 0 == _cmp(p1, tmp, user) )
		{
			p++;
			_lv_swapbytes(_l_ptradd(_heap, p * datasize), p1, datasize);
		}
		if ( 0 == _cmp(p2, tmp, user) )
		{
			q--;
			_lv_swapbytes(_l_ptradd(_heap, q * datasize), p2, datasize);
		}
	}
	p1 = _l_ptradd(_heap, i * datasize);
	p2 = _l_ptradd(_heap, r * datasize);
	_lv_swapbytes(p1, p2, datasize);
	j = i - 1;
	i = i + 1;
	for ( k = l; k <= p; k++, j-- )
	{
		p1 = _l_ptradd(_heap, k * datasize);
		p2 = _l_ptradd(_heap, j * datasize);
		_lv_swapbytes(p1, p2, datasize);
	}
	for ( k = r - 1; k >= q; k--, i++ )
	{
		p1 = _l_ptradd(_heap, k * datasize);
		p2 = _l_ptradd(_heap, i * datasize);
		_lv_swapbytes(p1, p2, datasize);
	}
	__ls_uquicksort(_heap, datasize, l, j, _cmp, user);
	__ls_uquicksort(_heap, datasize, i, r, _cmp, user);
}

void __stdcall 
_ls_uquicksort( Pointer _heap, dword datasize, dword cnt, TSearchAndSortUserFunc _cmp, Pointer user )
{
	__ls_uquicksort(_heap, datasize, 0, cnt - 1, _cmp, user);
}

sdword __stdcall 
_lv_searchptr( Array table, Pointer ptr, dword max )
	{
	sdword ix = max;
	if ( PtrCheck(table) )
		return -1;
	for ( ; ix > 0; ix--, table = CastAny(Array,_fl_ptradd( table, szPointer )) )
		if ( v_ptrdiff( *table, ptr ) == 0 )
			return max - ix;
	return -1L;
	}								   /* end of v_searchptr */
	
int __stdcall 
c_betwptr( CPointer pa, CPointer pb, CPointer px )
	{
	if ( (c_ptrdiff( pa, px ) <= 0) && (c_ptrdiff( px, pb ) <= 0) )
		return TRUE;
	return FALSE;
	}

void __stdcall
v_swapbytes(Pointer p1, Pointer p2, word size)
{
	word ix;
	byte ch[256];
	for (; size > 0; size -= ix)
	{
		ix = (size > 256) ? 256 : size;
		s_memcpy(ch, p1, ix);
		s_memcpy(p1, p2, ix);
		s_memcpy(p2, ch, ix);
		p1 = l_ptradd(p1, ix);
		p2 = l_ptradd(p2, ix);
	}
}

void __stdcall
_lv_swapbytes( Pointer p1, Pointer p2, dword size )
	{
	dword ix;
	byte ch[512];
	for ( ; size > 0; size -= ix )
		{
		ix = (size > 512)?512:size;
		s_memcpy( ch, p1, ix );
		s_memcpy( p1, p2, ix );
		s_memcpy( p2, ch, ix );
		p1 = _fl_ptradd( p1, ix );
		p2 = _fl_ptradd( p2, ix );
		}
	}

