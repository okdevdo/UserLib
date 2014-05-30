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
#ifndef __SLISTE_H__
#define __SLISTE_H__

#include "CSOURCES.H"
#include "LISTE.H"

#ifdef __cplusplus
extern "C" {
#endif

	CSOURCES_API Pointer __stdcall SDoubleLinkedListOpen(DECL_FILE_LINE dword datasize);
	CSOURCES_API TListCnt __stdcall SDoubleLinkedListCount(Pointer liste);
	CSOURCES_API void __stdcall SDoubleLinkedListClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListPrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListLast(Pointer liste);
	CSOURCES_API bool __stdcall SDoubleLinkedListForEach(Pointer liste, TForEachFunc func, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API void __stdcall SDoubleLinkedListSort(Pointer liste, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListAppend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListPrepend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListInsertBefore(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListInsertAfter(LSearchResultType node, ConstPointer data);
	CSOURCES_API void __stdcall SDoubleLinkedListRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SDoubleLinkedListInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API bool __stdcall SDoubleLinkedListRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API Pointer __stdcall SDoubleLinkedListGetData(LSearchResultType node);
	CSOURCES_API void __stdcall SDoubleLinkedListSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall SArrayOpen(DECL_FILE_LINE TListCnt max, dword datasize);
	CSOURCES_API TListCnt __stdcall SArrayCount(Pointer liste);
	CSOURCES_API void __stdcall SArrayClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SArrayBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall SArrayNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SArrayPrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SArrayLast(Pointer liste);
	CSOURCES_API bool __stdcall SArrayForEach(Pointer liste, TForEachFunc func, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SArrayFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc);
	CSOURCES_API LSearchResultType __stdcall SArrayFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SArrayUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SArrayLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API void __stdcall SArraySort(Pointer liste, TSearchAndSortFunc sortFunc, word mode);
	CSOURCES_API LSearchResultType __stdcall SArrayAppend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SArrayPrepend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SArrayInsertBefore(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SArrayInsertAfter(LSearchResultType node, ConstPointer data);
	CSOURCES_API void __stdcall SArrayRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SArrayInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API bool __stdcall SArrayRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API Pointer __stdcall SArrayGetData(LSearchResultType node);
	CSOURCES_API void __stdcall SArraySetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall SVectorOpen(DECL_FILE_LINE TListCnt max, TListCnt exp, dword datasize);
	CSOURCES_API TListCnt __stdcall SVectorCount(Pointer liste);
	CSOURCES_API void __stdcall SVectorReserve(Pointer liste);
	CSOURCES_API void __stdcall SVectorClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SVectorIndex(Pointer liste, TListIndex index);
	CSOURCES_API LSearchResultType __stdcall SVectorBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall SVectorNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SVectorPrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SVectorLast(Pointer liste);
	CSOURCES_API bool __stdcall SVectorForEach(Pointer liste, TForEachFunc func, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SVectorFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc);
	CSOURCES_API LSearchResultType __stdcall SVectorFindUser(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SVectorFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SVectorUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SVectorLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API void __stdcall SVectorSort(Pointer liste, TSearchAndSortFunc sortFunc, word mode);
	CSOURCES_API void __stdcall SVectorSortUser(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context, word mode);
	CSOURCES_API LSearchResultType __stdcall SVectorAppend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SVectorPrepend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SVectorInsertBefore(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SVectorInsertAfter(LSearchResultType node, ConstPointer data);
	CSOURCES_API void __stdcall SVectorRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SVectorInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API bool __stdcall SVectorRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API Pointer __stdcall SVectorGetData(LSearchResultType node);
	CSOURCES_API void __stdcall SVectorSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall SAVLBinaryTreeOpen(DECL_FILE_LINE dword datasize);
	CSOURCES_API TListCnt __stdcall SAVLBinaryTreeCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall SAVLBinaryTreeHeight(Pointer liste);
	CSOURCES_API void __stdcall SAVLBinaryTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreeBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreeNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreePrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreeLast(Pointer liste);
	CSOURCES_API bool __stdcall SAVLBinaryTreeForEach(Pointer liste, TForEachFunc func, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SAVLBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API bool __stdcall SAVLBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API Pointer __stdcall SAVLBinaryTreeGetData(LSearchResultType node);
	CSOURCES_API void __stdcall SAVLBinaryTreeSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall SRBBinaryTreeOpen(DECL_FILE_LINE dword datasize);
	CSOURCES_API TListCnt __stdcall SRBBinaryTreeCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall SRBBinaryTreeHeight(Pointer liste);
	CSOURCES_API void __stdcall SRBBinaryTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreeBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreeNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreePrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreeLast(Pointer liste);
	CSOURCES_API bool __stdcall SRBBinaryTreeForEach(Pointer liste, TForEachFunc func, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SRBBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API bool __stdcall SRBBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API Pointer __stdcall SRBBinaryTreeGetData(LSearchResultType node);
	CSOURCES_API void __stdcall SRBBinaryTreeSetData(LSearchResultType node, ConstPointer data);

	CSOURCES_API Pointer __stdcall SBTreeOpen(DECL_FILE_LINE TListCnt maxEntriesPerNode, dword datasize);
	CSOURCES_API TListCnt __stdcall SBTreeCount(Pointer liste);
	CSOURCES_API TListCnt __stdcall SBTreeHeight(Pointer liste);
	CSOURCES_API void __stdcall SBTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SBTreeIndex(Pointer liste, TListIndex index);
	CSOURCES_API LSearchResultType __stdcall SBTreeBegin(Pointer liste);
	CSOURCES_API LSearchResultType __stdcall SBTreeNext(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SBTreePrev(LSearchResultType node);
	CSOURCES_API LSearchResultType __stdcall SBTreeLast(Pointer liste);
	CSOURCES_API bool __stdcall SBTreeForEach(Pointer liste, TForEachFunc func, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SBTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc);
	CSOURCES_API LSearchResultType __stdcall SBTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SBTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SBTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API LSearchResultType __stdcall SBTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc);
	CSOURCES_API bool __stdcall SBTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API LSearchResultType __stdcall SBTreeAppend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SBTreePrepend(Pointer liste, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SBTreeInsertBefore(LSearchResultType node, ConstPointer data);
	CSOURCES_API LSearchResultType __stdcall SBTreeInsertAfter(LSearchResultType node, ConstPointer data);
	CSOURCES_API void __stdcall SBTreeRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context);
	CSOURCES_API Pointer __stdcall SBTreeGetData(LSearchResultType node);
	CSOURCES_API void __stdcall SBTreeSetData(LSearchResultType node, ConstPointer data);

#ifdef __cplusplus
}
#endif

#endif
