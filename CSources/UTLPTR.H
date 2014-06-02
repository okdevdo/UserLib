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
#ifndef __UTLPTR_H__
#define __UTLPTR_H__

#include "CSOURCES.H"

#ifdef __cplusplus
extern "C" {
#endif

typedef sword ( __stdcall *TSearchAndSortFunc)( ConstPointer ArrayItem, ConstPointer DataItem );
typedef sword ( __stdcall *TSearchAndSortUserFunc)( ConstPointer ArrayItem, ConstPointer DataItem, Pointer Context );

#define UTLPTR_INSERTMODE 0
#define UTLPTR_SEARCHMODE 1
#define UTLPTR_MATCHMODE 2
CSOURCES_API sdword __stdcall _lv_binsert(Array, ConstPointer, DWPointer, TSearchAndSortFunc);
CSOURCES_API sdword __stdcall _lv_ubinsert(Array, ConstPointer, DWPointer, TSearchAndSortUserFunc, Pointer context);
CSOURCES_API sdword __stdcall _lv_linsert(Array, ConstPointer, DWPointer, TSearchAndSortFunc);
CSOURCES_API sdword __stdcall _lv_insert( Array, sdword, ConstPointer, DWPointer );
CSOURCES_API sdword __stdcall _lv_ninsert( Array, sdword, Array, dword, DWPointer );
CSOURCES_API sdword __stdcall _lv_bdelete( Array, ConstPointer, DWPointer, TSearchAndSortFunc );
CSOURCES_API sdword __stdcall _lv_ldelete( Array, ConstPointer, DWPointer, TSearchAndSortFunc );
CSOURCES_API sdword __stdcall _lv_delete( Array, sdword, DWPointer );
CSOURCES_API sdword __stdcall _lv_ndelete( Array, sdword, dword, DWPointer );
CSOURCES_API sdword __stdcall _lv_bsearch( Array, ConstPointer, dword, TSearchAndSortFunc, sword );
CSOURCES_API sdword __stdcall _lv_ubsearch(Array, ConstPointer, dword, TSearchAndSortUserFunc, Pointer, sword);
CSOURCES_API sdword __stdcall _lv_lsearch( Array, ConstPointer, dword, TSearchAndSortFunc, sword );
CSOURCES_API sdword __stdcall _lv_ulsearch( Array, ConstPointer, dword, TSearchAndSortUserFunc, Pointer, sword );
CSOURCES_API void __stdcall _lv_heapsort( Array _heap, dword cnt, TSearchAndSortFunc _cmp );
CSOURCES_API void __stdcall _lv_uheapsort( Array _heap, dword cnt, TSearchAndSortUserFunc _cmp, Pointer user );
CSOURCES_API void __stdcall _lv_quicksort( Array _heap, dword cnt, TSearchAndSortFunc _cmp );
CSOURCES_API void __stdcall _lv_uquicksort( Array _heap, dword cnt, TSearchAndSortUserFunc _cmp, Pointer user );

CSOURCES_API sdword __stdcall _ls_binsert( Pointer, dword, ConstPointer, DWPointer, TSearchAndSortFunc );
CSOURCES_API sdword __stdcall _ls_linsert( Pointer, dword, ConstPointer, DWPointer, TSearchAndSortFunc );
CSOURCES_API sdword __stdcall _ls_insert( Pointer, dword, sdword, ConstPointer, DWPointer );
CSOURCES_API sdword __stdcall _ls_ninsert( Pointer, dword, sdword, ConstPointer, dword, DWPointer );
CSOURCES_API sdword __stdcall _ls_bdelete( Pointer, dword, ConstPointer, DWPointer, TSearchAndSortFunc );
CSOURCES_API sdword __stdcall _ls_ldelete( Pointer, dword, ConstPointer, DWPointer, TSearchAndSortFunc );
CSOURCES_API sdword __stdcall _ls_delete( Pointer, dword, sdword, DWPointer );
CSOURCES_API sdword __stdcall _ls_ndelete( Pointer, dword, sdword, dword, DWPointer );
CSOURCES_API sdword __stdcall _ls_bsearch( Pointer, dword, ConstPointer, dword, TSearchAndSortFunc, sword );
CSOURCES_API sdword __stdcall _ls_ubsearch( Pointer, dword, ConstPointer, dword, TSearchAndSortUserFunc, Pointer, sword );
CSOURCES_API sdword __stdcall _ls_lsearch( Pointer, dword, ConstPointer, dword, TSearchAndSortFunc, sword );
CSOURCES_API sdword __stdcall _ls_ulsearch( Pointer, dword, ConstPointer, dword, TSearchAndSortUserFunc, Pointer, sword );
CSOURCES_API void __stdcall _ls_heapsort( Pointer _heap, dword _size, dword cnt, TSearchAndSortFunc _cmp );
CSOURCES_API void __stdcall _ls_uheapsort( Pointer _heap, dword _size, dword cnt, TSearchAndSortUserFunc _cmp, Pointer user );
CSOURCES_API void __stdcall _ls_quicksort( Pointer _heap, dword _size, dword cnt, TSearchAndSortFunc _cmp );
CSOURCES_API void __stdcall _ls_uquicksort( Pointer _heap, dword _size, dword cnt, TSearchAndSortUserFunc _cmp, Pointer user );

CSOURCES_API sdword __stdcall _lv_searchptr( Array, Pointer, dword );

#define _l_ptradd(pp,offset) ((Pointer)(((unsigned char *)(pp)) + (offset)))
#define _fl_ptradd _l_ptradd
#define c_ptrdiff(pa,pb) ((ptrdiff_t)(((unsigned char *)(pa)) - ((unsigned char *)(pb))))
#define _fc_ptrdiff c_ptrdiff

#ifdef __DEBUG_TEST_MEMORY__
	#define l_ptradd(p,o)	PtrTest( (p), (o) )
#else
	#define l_ptradd _l_ptradd
#endif

CSOURCES_API int __stdcall c_betwptr( CPointer, CPointer, CPointer );

#define NULDIVEXC(z,n,e)	(n)?((z)/(n)):(e)
#define v_ptrdiff(pa,pb)	c_ptrdiff( (CPointer)(pa), (CPointer)(pb) )
#define v_betwptr(pa,pb,pc)	c_betwptr( (CPointer)(pa), (CPointer)(pb), (CPointer)(pc) )

CSOURCES_API void __stdcall _lv_swapbytes( Pointer p1, Pointer p2, dword size );
CSOURCES_API void __stdcall v_swapbytes( Pointer p1, Pointer p2, word size );

#ifdef __cplusplus
}
#endif

#endif
/* end of UTLPTR.H */
