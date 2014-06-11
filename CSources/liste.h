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
#ifndef __LISTE_H__
#define __LISTE_H__

#include "csources.h"
#include "utlptr.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef dword TListCnt;
	typedef sdword TListIndex;

	typedef struct _tag_lsearchresulttype {
		Pointer node;
		TListIndex offset;
	} LSearchResultType;
	
	extern const CSOURCES_API LSearchResultType _lnull;
	#define _LNULL _lnull
	#define _Lvalue(p) DerefAnyPtr(LSearchResultType,(p))
	#define _Lnode(v) v.node
	#define _LPnode(p) CastAnyPtr(LSearchResultType,(p))->node
	#define _Loffset(v) v.offset
	#define _LPoffset(p) CastAnyPtr(LSearchResultType,(p))->offset

	#define LPtrCheck(v) (PtrCheck(_Lnode(v)) && (_Loffset(v) == 0))
	#define LPPtrCheck(p) (PtrCheck(_LPnode(p)) && (_LPoffset(p) == 0))
	#define LCompareNotEqual(va,vb) ((v_ptrdiff( _Lnode(va), _Lnode(vb) ) != 0) || ((_Loffset(va) - _Loffset(vb)) != 0))
	#define LCompareEqual(va,vb) ((v_ptrdiff( _Lnode(va), _Lnode(vb) ) == 0) && ((_Loffset(va) - _Loffset(vb)) == 0))

	typedef void (__stdcall *TDeleteFunc)( ConstPointer data, Pointer context );
	typedef bool (__stdcall *TForEachFunc)( ConstPointer data, Pointer context );
	typedef sdword (__stdcall *THashFunc)( ConstPointer data, Pointer context );

	#define LSORTMODE_HEAPSORT 0
	#define LSORTMODE_QUICKSORT 1

	CSOURCES_API Pointer __stdcall DoubleLinkedListOpen(DECL_FILE_LINE0);
	CSOURCES_API TListCnt __stdcall DoubleLinkedListCount(Pointer liste);
	CSOURCES_API void __stdcall DoubleLinkedListClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API void __stdcall DoubleLinkedListClear(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListIndex(Pointer liste, TListIndex index);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListPrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListLast(Pointer liste);
	CSOURCES_API bool __stdcall DoubleLinkedListForEach(Pointer liste, TForEachFunc func, Pointer context, bool bbackward);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API void __stdcall DoubleLinkedListSort(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListAppend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListPrepend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListInsertBefore(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListInsertAfter(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall DoubleLinkedListInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API bool __stdcall DoubleLinkedListRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext);
	CSOURCES_API Pointer __stdcall DoubleLinkedListGetData(LSearchResultType node);
	CSOURCES_API void __stdcall DoubleLinkedListSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall ArrayOpen(DECL_FILE_LINE TListCnt max);
	CSOURCES_API TListCnt __stdcall ArrayCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall ArrayMaximum(Pointer liste);
	CSOURCES_API void __stdcall ArrayClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API void __stdcall ArrayClear(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall ArrayIndex(Pointer liste, TListIndex index);
	CSOURCES_API LSearchResultType __stdcall ArrayBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall ArrayNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall ArrayPrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall ArrayLast(Pointer liste);
	CSOURCES_API bool __stdcall ArrayForEach(Pointer liste, TForEachFunc func, Pointer context, bool bbackward);
	CSOURCES_API LSearchResultType __stdcall ArrayFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall ArrayFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall ArrayUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall ArrayLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API void __stdcall ArraySort(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context, word mode);
	CSOURCES_API LSearchResultType __stdcall ArrayAppend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall ArrayPrepend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall ArrayInsertBefore(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall ArrayInsertAfter(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall ArrayRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall ArrayInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API bool __stdcall ArrayRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext);
	CSOURCES_API Pointer __stdcall ArrayGetData(LSearchResultType node);
	CSOURCES_API void __stdcall ArraySetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall VectorOpen(DECL_FILE_LINE TListCnt max, TListCnt exp);
	CSOURCES_API TListCnt __stdcall VectorCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall VectorMaximum(Pointer liste);
	CSOURCES_API void __stdcall VectorReserve(Pointer liste);
	CSOURCES_API void __stdcall VectorClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API void __stdcall VectorClear(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall VectorIndex(Pointer liste, TListIndex index);
	CSOURCES_API LSearchResultType __stdcall VectorBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall VectorNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall VectorPrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall VectorLast(Pointer liste);
	CSOURCES_API bool __stdcall VectorForEach(Pointer liste, TForEachFunc func, Pointer context, bool bbackward);
	CSOURCES_API LSearchResultType __stdcall VectorFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context );
	CSOURCES_API LSearchResultType __stdcall VectorFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall VectorUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall VectorLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API void __stdcall VectorSort(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context, word mode);
	CSOURCES_API LSearchResultType __stdcall VectorAppend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall VectorPrepend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall VectorInsertBefore(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall VectorInsertAfter(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall VectorRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall VectorInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API bool __stdcall VectorRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext);
	CSOURCES_API Pointer __stdcall VectorGetData(LSearchResultType node);
	CSOURCES_API void __stdcall VectorSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall AVLBinaryTreeOpen(DECL_FILE_LINE0);
	CSOURCES_API TListCnt __stdcall AVLBinaryTreeCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall AVLBinaryTreeHeight(Pointer liste);
	CSOURCES_API void __stdcall AVLBinaryTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API void __stdcall AVLBinaryTreeClear(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall AVLBinaryTreeBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall AVLBinaryTreeNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall AVLBinaryTreePrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall AVLBinaryTreeLast(Pointer liste);
	CSOURCES_API bool __stdcall AVLBinaryTreeForEach(Pointer liste, TForEachFunc func, Pointer context, bool bbackward);
	CSOURCES_API LSearchResultType __stdcall AVLBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall AVLBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall AVLBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API bool __stdcall AVLBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext);
	CSOURCES_API Pointer __stdcall AVLBinaryTreeGetData(LSearchResultType node);
	CSOURCES_API void __stdcall AVLBinaryTreeSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall RBBinaryTreeOpen(DECL_FILE_LINE0);
	CSOURCES_API TListCnt __stdcall RBBinaryTreeCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall RBBinaryTreeHeight(Pointer liste);
	CSOURCES_API void __stdcall RBBinaryTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API void __stdcall RBBinaryTreeClear(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall RBBinaryTreeBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall RBBinaryTreeNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall RBBinaryTreePrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall RBBinaryTreeLast(Pointer liste);
	CSOURCES_API bool __stdcall RBBinaryTreeForEach(Pointer liste, TForEachFunc func, Pointer context, bool bbackward);
	CSOURCES_API LSearchResultType __stdcall RBBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall RBBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall RBBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API bool __stdcall RBBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext);
	CSOURCES_API Pointer __stdcall RBBinaryTreeGetData(LSearchResultType node);
	CSOURCES_API void __stdcall RBBinaryTreeSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall BTreeOpen(DECL_FILE_LINE TListCnt maxEntriesPerNode);
	CSOURCES_API TListCnt __stdcall BTreeCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall BTreeHeight(Pointer liste);
	CSOURCES_API void __stdcall BTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API void __stdcall BTreeClear(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall BTreeIndex(Pointer liste, TListIndex index);
	CSOURCES_API LSearchResultType __stdcall BTreeBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall BTreeNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall BTreePrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall BTreeLast(Pointer liste);
	CSOURCES_API bool __stdcall BTreeForEach(Pointer liste, TForEachFunc func, Pointer context, bool bbackward);
	CSOURCES_API LSearchResultType __stdcall BTreeFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall BTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall BTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall BTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API void __stdcall BTreeSort(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall BTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API bool __stdcall BTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext);
	CSOURCES_API LSearchResultType __stdcall BTreeAppend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall BTreePrepend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall BTreeInsertBefore(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall BTreeInsertAfter(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall BTreeRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API Pointer __stdcall BTreeGetData(LSearchResultType node);
	CSOURCES_API void __stdcall BTreeSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall HashLinkedListOpen(DECL_FILE_LINE TListCnt max, THashFunc hfunc, Pointer hContext);
	CSOURCES_API TListCnt __stdcall HashLinkedListCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall HashLinkedListMaximum(Pointer liste);
	CSOURCES_API void __stdcall HashLinkedListClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API void __stdcall HashLinkedListClear(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall HashLinkedListIndex(Pointer liste, TListIndex index);
	CSOURCES_API LSearchResultType __stdcall HashLinkedListBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall HashLinkedListNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall HashLinkedListPrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall HashLinkedListLast(Pointer liste);
	CSOURCES_API bool __stdcall HashLinkedListForEach(Pointer liste, TForEachFunc func, Pointer context, bool bbackward);
	CSOURCES_API LSearchResultType __stdcall HashLinkedListFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall HashLinkedListFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall HashLinkedListInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context);
	CSOURCES_API bool __stdcall HashLinkedListRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext);
	CSOURCES_API Pointer __stdcall HashLinkedListGetData(LSearchResultType node);
	CSOURCES_API void __stdcall HashLinkedListSetData(LSearchResultType node, ConstPointer data);

#ifdef __cplusplus
}
#endif

#endif
