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
#include "CS_PCH.H"
#include "SLISTE.H"
#include "STRUTIL.H"

/*					*/
/* SDoubleLinkedList */
/*					*/

struct _tagSDoubleLinkedListHead;
typedef struct _tagSDoubleLinkedListNode
{
	Ptr(struct _tagSDoubleLinkedListHead) head;
	Ptr(struct _tagSDoubleLinkedListNode) prev;
	Ptr(struct _tagSDoubleLinkedListNode) next;
} _SDoubleLinkedListNode, *_pSDoubleLinkedListNode;
#define szSDoubleLinkedListNode sizeof(_SDoubleLinkedListNode)

typedef struct _tagSDoubleLinkedListHead
{
	Ptr(struct _tagSDoubleLinkedListNode) first;
	Ptr(struct _tagSDoubleLinkedListNode) last;
	TListCnt nodeCount;
	dword dataSize;
} _SDoubleLinkedListHead, *_pSDoubleLinkedListHead;
#define szSDoubleLinkedListHead sizeof(_SDoubleLinkedListHead)

Pointer __stdcall 
SDoubleLinkedListOpen(DECL_FILE_LINE dword datasize)
{
#ifdef __DEBUG__
	_pSDoubleLinkedListHead head = CastAnyPtr(_SDoubleLinkedListHead, TFallocDbg(szSDoubleLinkedListHead, file, line));
#else
	_pSDoubleLinkedListHead head = CastAnyPtr(_SDoubleLinkedListHead, TFalloc(szSDoubleLinkedListHead));
#endif

	if ( PtrCheck(head) )
		return NULL;
	assert(datasize > 0);
	head->dataSize = datasize;
	return head;
}

TListCnt __stdcall 
SDoubleLinkedListCount(Pointer liste)
{
	_pSDoubleLinkedListHead head = CastAnyPtr(_SDoubleLinkedListHead,liste);

	assert(head != NULL);
	return head->nodeCount;
}

void __stdcall 
SDoubleLinkedListClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pSDoubleLinkedListHead head = CastAnyPtr(_SDoubleLinkedListHead,liste);
	_pSDoubleLinkedListNode node = head->first;
	_pSDoubleLinkedListNode node1;

	assert(liste != NULL);
	assert(freeFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		node1 = node->next;
		freeFunc(_l_ptradd(node, szSDoubleLinkedListNode), context);
		TFfree(node);
		node = node1;
	}
	TFfree(head);
}

LSearchResultType __stdcall 
SDoubleLinkedListBegin(Pointer liste)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	_Lnode(result) = head->first;
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListNext(LSearchResultType node)
{
	_pSDoubleLinkedListNode node1 = CastAny(_pSDoubleLinkedListNode,_Lnode(node));
	LSearchResultType result = _LNULL;

	if ( PtrCheck(node1) )
		return result;
	_Lnode(result) = node1->next;
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListPrev(LSearchResultType node)
{
	_pSDoubleLinkedListNode node1 = CastAny(_pSDoubleLinkedListNode,_Lnode(node));
	LSearchResultType result = _LNULL;

	if ( PtrCheck(node1) )
		return result;
	_Lnode(result) = node1->prev;
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListLast(Pointer liste)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	_Lnode(result) = head->last;
	return result;
}

bool __stdcall
SDoubleLinkedListForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead, liste);
	_pSDoubleLinkedListNode node = head->first;
	bool result = true;

	assert(liste != NULL);
	assert(func != NULL);
	while (NotPtrCheck(node))
	{
		if (0 == func(_l_ptradd(node, szSDoubleLinkedListNode), context))
		{
			result = false;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall
SDoubleLinkedListFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	_pSDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		if ( 0 == findFunc(_l_ptradd(node, szSDoubleLinkedListNode), data) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	_pSDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		if ( 0 <= sortFunc(_l_ptradd(node, szSDoubleLinkedListNode), data) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	_pSDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		if ( 0 < sortFunc(_l_ptradd(node, szSDoubleLinkedListNode), data) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	_pSDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		if ( 0 <= sortFunc(_l_ptradd(node, szSDoubleLinkedListNode), data) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

void __stdcall 
SDoubleLinkedListSort(Pointer liste, TSearchAndSortFunc sortFunc)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	_pSDoubleLinkedListNode nodeIn = head->first;
	_pSDoubleLinkedListNode nodeIn1 = NULL;
	_SDoubleLinkedListNode nodeOutHead;
	_pSDoubleLinkedListNode nodeOut = NULL;
	_pSDoubleLinkedListNode nodeOut1 = NULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	s_memset(&nodeOutHead, 0, szSDoubleLinkedListNode);
	nodeOut = &nodeOutHead;
	for ( ; NotPtrCheck(nodeIn); nodeIn = nodeIn1 )
	{
		nodeIn1 = nodeIn->next;
		for ( nodeOut1 = nodeOut; NotPtrCheck(nodeOut1->next); nodeOut1 = nodeOut1->next )
			if ( 0 < sortFunc(_l_ptradd(nodeOut1->next, szSDoubleLinkedListNode), _l_ptradd(nodeIn, szSDoubleLinkedListNode)) )
				break;
		nodeIn->next = nodeOut1->next;
		nodeOut1->next = nodeIn;
	}
	head->first = nodeOut->next;
	nodeIn = head->first;
	nodeIn1 = NULL;
	while ( NotPtrCheck(nodeIn) )
	{
		nodeIn->prev = nodeIn1;
		nodeIn1 = nodeIn;
		if ( PtrCheck(nodeIn->next) )
			head->last = nodeIn;
		nodeIn = nodeIn->next;
	}
}

LSearchResultType __stdcall 
SDoubleLinkedListAppend(Pointer liste, ConstPointer data)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	_pSDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	node2 = CastAnyPtr(_SDoubleLinkedListNode,TFalloc(szSDoubleLinkedListNode + head->dataSize));
	if ( PtrCheck(node2) )
		return result;

	s_memcpy_s(_l_ptradd(node2, szSDoubleLinkedListNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
	node2->head = head;
	if ( NotPtrCheck(head->last) )
	{
		node2->prev = head->last;
		node2->prev->next = node2;
	}
	else
		head->first = node2;
	head->last = node2;
	(head->nodeCount)++;
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListPrepend(Pointer liste, ConstPointer data)
{
	_pSDoubleLinkedListHead head = CastAny(_pSDoubleLinkedListHead,liste);
	_pSDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	node2 = CastAny(_pSDoubleLinkedListNode,TFalloc(szSDoubleLinkedListNode + head->dataSize));
	if ( PtrCheck(node2) )
		return result;

	s_memcpy_s(_l_ptradd(node2, szSDoubleLinkedListNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
	node2->head = head;
	if ( NotPtrCheck(head->first) )
	{
		node2->next = head->first;
		node2->next->prev = node2;
	}
	else
		head->last = node2;
	head->first = node2;
	(head->nodeCount)++;
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListInsertBefore(LSearchResultType node, ConstPointer data)
{
	_pSDoubleLinkedListNode node1 = CastAny(_pSDoubleLinkedListNode,_Lnode(node));
	_pSDoubleLinkedListHead head = node1->head;
	_pSDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(node1 != NULL);
	node2 = CastAny(_pSDoubleLinkedListNode,TFalloc(szSDoubleLinkedListNode + head->dataSize));
	if ( PtrCheck(node2) )
		return result;

	s_memcpy_s(_l_ptradd(node2, szSDoubleLinkedListNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
	node2->head = head;
	if ( (head->first == head->last) || (node1 == head->first) )
	{
		assert( NotPtrCheck(head->first) );
		node2->next = head->first;
		node2->next->prev = node2;
		head->first = node2;
	}
	else
	{
		node2->next = node1;
		node2->prev = node1->prev;
		node2->prev->next = node2;
		node2->next->prev = node2;
	}
	(head->nodeCount)++;
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall 
SDoubleLinkedListInsertAfter(LSearchResultType node, ConstPointer data)
{
	_pSDoubleLinkedListNode node1 = CastAny(_pSDoubleLinkedListNode,_Lnode(node));
	_pSDoubleLinkedListHead head = node1->head;
	_pSDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(node1 != NULL);
	node2 = CastAny(_pSDoubleLinkedListNode,TFalloc(szSDoubleLinkedListNode + head->dataSize));
	if ( PtrCheck(node2) )
		return result;

	s_memcpy_s(_l_ptradd(node2, szSDoubleLinkedListNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
	node2->head = head;
	if ( (head->first == head->last) || (node1 == head->last) )
	{
		assert( NotPtrCheck(head->last) );
		node2->prev = head->last;
		node2->prev->next = node2;
		head->last = node2;
	}
	else
	{
		node2->prev = node1;
		node2->next = node1->next;
		node2->prev->next = node2;
		node2->next->prev = node2;
	}
	(head->nodeCount)++;
	_Lnode(result) = node2;
	return result;
}

void __stdcall 
SDoubleLinkedListRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pSDoubleLinkedListNode node1 = CastAny(_pSDoubleLinkedListNode,_Lnode(node));
	_pSDoubleLinkedListHead head = node1->head;

	assert( node1 != NULL );
	assert( freeFunc != NULL );
	if ( node1 == head->first )
	{
		if ( head->first == head->last )
		{
			head->first = NULL;
			head->last = NULL;
		}
		else
		{
			head->first = head->first->next;
			head->first->prev = NULL;
		}
	}
	else if ( node1 == head->last )
	{
		head->last = head->last->prev;
		head->last->next = NULL;
	}
	else
	{
		node1->next->prev = node1->prev;
		node1->prev->next = node1->next;
	}
	freeFunc(_l_ptradd(node1, szSDoubleLinkedListNode), context);
	TFfree(node1);
	(head->nodeCount)--;
}

LSearchResultType __stdcall 
SDoubleLinkedListInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	LSearchResultType result = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	result = SDoubleLinkedListFindSorted(liste, data, sortFunc);
	if ( LPtrCheck(result) )
		return SDoubleLinkedListAppend(liste, data);
	cmp = sortFunc(SDoubleLinkedListGetData(result), data);
	while ( cmp == 0 )
	{
		result = SDoubleLinkedListNext(result);
		if ( LPtrCheck(result) )
			break;
		cmp = sortFunc(SDoubleLinkedListGetData(result), data);
	}
	if ( LPtrCheck(result) )
		return SDoubleLinkedListAppend(liste, data);
	return SDoubleLinkedListInsertBefore(result, data);
}

bool __stdcall 
SDoubleLinkedListRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	LSearchResultType result = _LNULL;
	LSearchResultType result1 = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	result = SDoubleLinkedListFindSorted(liste, data, sortFunc);
	if ( LPtrCheck(result) )
		return false;
	cmp = sortFunc(SDoubleLinkedListGetData(result), data);
	while ( cmp == 0 )
	{
		result1 = SDoubleLinkedListNext(result);
		SDoubleLinkedListRemove(result, freeFunc, context);
		if ( LPtrCheck(result1) )
			break;
		result = result1;
		cmp = sortFunc(SDoubleLinkedListGetData(result), data);
	}
	return true;
}

Pointer __stdcall 
SDoubleLinkedListGetData(LSearchResultType node)
{
	_pSDoubleLinkedListNode node1 = CastAny(_pSDoubleLinkedListNode,_Lnode(node));

	if ( PtrCheck(node1) )
		return NULL;
	return _l_ptradd(node1, szSDoubleLinkedListNode);
}

void __stdcall 
SDoubleLinkedListSetData(LSearchResultType node, ConstPointer data)
{
	_pSDoubleLinkedListNode node1 = CastAny(_pSDoubleLinkedListNode,_Lnode(node));
	_pSDoubleLinkedListHead head = node1->head;

	assert( node1 != NULL );
	s_memcpy_s(_l_ptradd(node1, szSDoubleLinkedListNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
}

/*		 */
/* Array */
/*		 */
typedef struct _tagSArrayHead
{
	TListCnt max;
	TListCnt cnt;
	dword dataSize;
} _SArrayHead, *_pSArrayHead;
#define szSArrayHead sizeof(_SArrayHead)

Pointer __stdcall 
SArrayOpen(DECL_FILE_LINE TListCnt max, dword datasize)
{
	Pointer p;
	_pSArrayHead head;

	assert(datasize > 0);
	assert(max > 0);
#ifdef __DEBUG__
	p = TFallocDbg(szSArrayHead + (max * datasize), file, line);
#else
	p = TFalloc(szSArrayHead + (max * datasize));
#endif
	if ( PtrCheck(p) )
		return NULL;
	head = CastAny(_pSArrayHead,p);
	head->max = max;
	head->dataSize = datasize;
	return p;
}

TListCnt __stdcall 
SArrayCount(Pointer liste)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);

	assert(liste != NULL);
	return head->cnt;
}

void __stdcall 
SArrayClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer data = _l_ptradd(head,szSArrayHead);
	TListCnt ix;

	assert(liste != NULL);
	assert(freeFunc != NULL);
	for ( ix = 0; ix < head->cnt; ++ix, data = _l_ptradd(data,head->dataSize) )
		freeFunc(data, context);
	TFfree(head);
}

LSearchResultType __stdcall 
SArrayBegin(Pointer liste)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = 0;
	return result;
}

LSearchResultType __stdcall 
SArrayNext(LSearchResultType node)
{
	_pSArrayHead head = CastAny(_pSArrayHead,_Lnode(node));
	LSearchResultType result = _LNULL;

	assert(head->max > 0);
	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) < -1) || (_Loffset(node) >= Cast(TListIndex, (head->cnt - 1))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = head->cnt - 1;
	if ( _Loffset(node) < Cast(TListIndex,(head->cnt - 1)) )
		_Loffset(result) = _Loffset(node) + 1;
	return result;
}

LSearchResultType __stdcall 
SArrayPrev(LSearchResultType node)
{
	_pSArrayHead head = CastAny(_pSArrayHead,_Lnode(node));
	LSearchResultType result = _LNULL;

	assert(head->max > 0);
	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) <= 0) || (_Loffset(node) > Cast(TListIndex, (head->cnt - 1))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = 0;
	if ( _Loffset(node) > 0 )
		_Loffset(result) = _Loffset(node) - 1;
	return result;
}

LSearchResultType __stdcall 
SArrayLast(Pointer liste)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = head->cnt - 1;
	return result;
}

bool __stdcall
SArrayForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pSArrayHead head = CastAny(_pSArrayHead, liste);
	Pointer arraydata = _l_ptradd(liste, szSArrayHead);
	Pointer pt = arraydata;
	bool result = true;
	TListCnt ix;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(func != NULL);
	for (ix = 0; ix < head->cnt; ++ix, pt = _fl_ptradd(pt, head->dataSize))
	{
		if (0 == func(pt, context))
		{
			result = false;
			break;
		}
	}
	return result;
}

LSearchResultType __stdcall
SArrayFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(liste, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(findFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_lsearch( arraydata, head->dataSize, data, head->cnt, findFunc, UTLPTR_MATCHMODE );
	if ( _Loffset(result) < 0 )
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
SArrayFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(liste, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( arraydata, head->dataSize, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return result;
}

LSearchResultType __stdcall 
SArrayUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(liste, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( arraydata, head->dataSize, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return result;
}

LSearchResultType __stdcall 
SArrayLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(liste, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( arraydata, head->dataSize, data, head->cnt, sortFunc, UTLPTR_SEARCHMODE);
	return result;
}

void __stdcall 
SArraySort(Pointer liste, TSearchAndSortFunc sortFunc, word mode)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(liste, szSArrayHead);

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return;
	switch ( mode )
	{
	case LSORTMODE_HEAPSORT:
		_ls_heapsort(arraydata, head->dataSize, head->cnt, sortFunc);
		break;
	case LSORTMODE_QUICKSORT:
		_ls_quicksort(arraydata, head->dataSize, head->cnt, sortFunc);
		break;
	}
}

LSearchResultType __stdcall 
SArrayAppend(Pointer liste, ConstPointer data)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(liste, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(head->cnt < head->max);
	_Lnode(result) = head;
	_Loffset(result) = _ls_insert( arraydata, head->dataSize, Cast(sdword,head->cnt) - 1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
SArrayPrepend(Pointer liste, ConstPointer data)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(liste, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(head->cnt < head->max);
	_Lnode(result) = head;
	_Loffset(result) = _ls_insert( arraydata, head->dataSize, -1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
SArrayInsertBefore(LSearchResultType node, ConstPointer data)
{
	_pSArrayHead head = CastAny(_pSArrayHead,_Lnode(node));
	Pointer arraydata = _l_ptradd(head, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	assert(head->cnt < head->max);
	if ( (_Loffset(node) < 0) || ((head->cnt > 0) && (_Loffset(node) >= Cast(TListIndex, head->cnt))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_insert( arraydata, head->dataSize, _Loffset(node) - 1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
SArrayInsertAfter(LSearchResultType node, ConstPointer data)
{
	_pSArrayHead head = CastAny(_pSArrayHead,_Lnode(node));
	Pointer arraydata = _l_ptradd(head, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	assert(head->cnt < head->max);
	if ( (_Loffset(node) < -1) || ((head->cnt > 0) && (_Loffset(node) >= Cast(TListIndex, (head->cnt - 1)))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_insert( arraydata, head->dataSize, _Loffset(node), data, &(head->cnt) );
	return result;
}

void __stdcall 
SArrayRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pSArrayHead head = CastAny(_pSArrayHead,_Lnode(node));
	Pointer arraydata = _l_ptradd(head, szSArrayHead);
	Pointer data;

	assert(freeFunc != NULL);
	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return;
	data = _l_ptradd(arraydata, _Loffset(node) * head->dataSize);
	freeFunc( data, context );
	_ls_delete( arraydata, head->dataSize, _Loffset(node), &(head->cnt) );
}

LSearchResultType __stdcall 
SArrayInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(head, szSArrayHead);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return SArrayAppend(liste, data);
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( arraydata, head->dataSize, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return SArrayInsertAfter(result, data);
}

bool __stdcall 
SArrayRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pSArrayHead head = CastAny(_pSArrayHead,liste);
	Pointer arraydata = _l_ptradd(head, szSArrayHead);
	LSearchResultType result = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return false;
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( arraydata, head->dataSize, data, head->cnt, sortFunc, UTLPTR_SEARCHMODE);
	if ( _Loffset(result) < 0 )
		return false;
	cmp = sortFunc(SArrayGetData(result), data);
	while ( cmp == 0 )
	{
		SArrayRemove(result, freeFunc, context);
		if ( _Loffset(result) >= Cast(TListIndex, head->cnt) )
			break;
		cmp = sortFunc(SArrayGetData(result), data);
	}
	return true;
}

Pointer __stdcall 
SArrayGetData(LSearchResultType node)
{
	_pSArrayHead head = CastAny(_pSArrayHead,_Lnode(node));
	Pointer arraydata = _l_ptradd(head, szSArrayHead);

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || LPtrCheck(node) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return NULL;
	return _l_ptradd(arraydata, _Loffset(node) * head->dataSize);
}

void __stdcall 
SArraySetData(LSearchResultType node, ConstPointer data)
{
	_pSArrayHead head = CastAny(_pSArrayHead,_Lnode(node));
	Pointer arraydata = _l_ptradd(head, szSArrayHead);

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return;
	s_memcpy_s(_l_ptradd(arraydata, _Loffset(node) * head->dataSize), head->dataSize, CastMutable(Pointer, data), head->dataSize);
}

/*		 */
/* SVector */
/*		 */
typedef struct _tagSVectorHead
{
	TListCnt max;
	TListCnt exp;
	TListCnt cnt;
	dword dataSize;
	Pointer data;
} _SVectorHead, *_pSVectorHead;
#define szSVectorHead sizeof(_SVectorHead)

Pointer __stdcall 
SVectorOpen(DECL_FILE_LINE TListCnt max, TListCnt exp, dword datasize)
{
#ifdef __DEBUG__
	Pointer p1 = TFallocDbg(szSVectorHead, file, line);
#else
	Pointer p1 = TFalloc(szSVectorHead);
#endif
	Pointer p2;
	_pSVectorHead head;

	if ( PtrCheck(p1) )
		return NULL;
	assert(max > 0);
	assert(datasize > 0);
#ifdef __DEBUG__
	p2 = TFallocDbg(max * datasize, file, line);
#else
	p2 = TFalloc(max * datasize);
#endif
	if ( PtrCheck(p2) )
	{
		TFfree(p1);
		return NULL;
	}

	head = CastAny(_pSVectorHead,p1);

	assert(exp > 0);
	head->max = max;
	head->exp = exp;
	head->dataSize = datasize;
	head->data = p2;
	return p1;
}

TListCnt __stdcall 
SVectorCount(Pointer liste)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);

	assert(liste != NULL);
	return head->cnt;
}

void __stdcall 
SVectorReserve(Pointer liste)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer p;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(head->exp > 0);
	p = TFrealloc(head->data, ((head->max + head->exp) * head->dataSize));
	if ( PtrCheck(p) )
		return;
	head->max += head->exp;
	head->data = p;
}

void __stdcall 
SVectorClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	TListCnt ix;

	assert(liste != NULL);
	assert(freeFunc != NULL);
	for ( ix = 0; ix < head->cnt; ++ix, dataPointer = _l_ptradd(dataPointer,head->dataSize) )
		freeFunc(dataPointer, context);
	TFfree(head->data);
	TFfree(head);
}

LSearchResultType __stdcall 
SVectorIndex(Pointer liste, TListIndex index)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	if ( index <= 0 )
		_Loffset(result) = 0;
	else if ( index >= Cast(TListIndex,head->cnt) )
		_Loffset(result) = head->cnt - 1;
	else
		_Loffset(result) = index;
	return result;
}

LSearchResultType __stdcall 
SVectorBegin(Pointer liste)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = 0;
	return result;
}

LSearchResultType __stdcall 
SVectorNext(LSearchResultType node)
{
	_pSVectorHead head = CastAny(_pSVectorHead,_Lnode(node));
	LSearchResultType result = _LNULL;

	assert(head->max > 0);
	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) < -1) || (_Loffset(node) >= Cast(TListIndex, (head->cnt - 1))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = head->cnt - 1;
	if ( _Loffset(node) < Cast(TListIndex,(head->cnt - 1)) )
		_Loffset(result) = _Loffset(node) + 1;
	return result;
}

LSearchResultType __stdcall 
SVectorPrev(LSearchResultType node)
{
	_pSVectorHead head = CastAny(_pSVectorHead,_Lnode(node));
	LSearchResultType result = _LNULL;

	assert(head->max > 0);
	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) <= 0) || (_Loffset(node) > Cast(TListIndex, (head->cnt - 1))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = 0;
	if ( _Loffset(node) > 0 )
		_Loffset(result) = _Loffset(node) - 1;
	return result;
}

LSearchResultType __stdcall 
SVectorLast(Pointer liste)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = head->cnt - 1;
	return result;
}

bool __stdcall
SVectorForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pSVectorHead head = CastAny(_pSVectorHead, liste);
	Pointer dataPointer = head->data;
	Pointer pt = dataPointer;
	bool result = true;
	TListCnt ix;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(func != NULL);
	for (ix = 0; ix < head->cnt; ++ix, pt = _fl_ptradd(pt, head->dataSize))
	{
		if (0 == func(pt, context))
		{
			result = false;
			break;
		}
	}
	return result;
}

LSearchResultType __stdcall
SVectorFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(findFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_lsearch( dataPointer, head->dataSize, data, head->cnt, findFunc, UTLPTR_MATCHMODE );
	if ( _Loffset(result) < 0 )
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
SVectorFindUser(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(findFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_ulsearch( dataPointer, head->dataSize, data, head->cnt, findFunc, context, UTLPTR_MATCHMODE );
	if ( _Loffset(result) < 0 )
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
SVectorFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( dataPointer, head->dataSize, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return result;
}

LSearchResultType __stdcall 
SVectorUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( dataPointer, head->dataSize, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return result;
}

LSearchResultType __stdcall 
SVectorLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( dataPointer, head->dataSize, data, head->cnt, sortFunc, UTLPTR_SEARCHMODE);
	return result;
}

void __stdcall 
SVectorSort(Pointer liste, TSearchAndSortFunc sortFunc, word mode)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return;
	switch ( mode )
	{
	case LSORTMODE_HEAPSORT:
		_ls_heapsort(dataPointer, head->dataSize, head->cnt, sortFunc);
		break;
	case LSORTMODE_QUICKSORT:
		_ls_quicksort(dataPointer, head->dataSize, head->cnt, sortFunc);
		break;
	}
}

void __stdcall 
SVectorSortUser(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context, word mode)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return;
	switch ( mode )
	{
	case LSORTMODE_HEAPSORT:
		_ls_uheapsort(dataPointer, head->dataSize, head->cnt, sortFunc, context);
		break;
	case LSORTMODE_QUICKSORT:
		_ls_uquicksort(dataPointer, head->dataSize, head->cnt, sortFunc, context);
		break;
	}
}

LSearchResultType __stdcall 
SVectorAppend(Pointer liste, ConstPointer data)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( head->cnt >= head->max )
	{
		SVectorReserve(liste);
		if ( head->cnt >= head->max )
			return result;
		dataPointer = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _ls_insert( dataPointer, head->dataSize, Cast(sdword,head->cnt) - 1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
SVectorPrepend(Pointer liste, ConstPointer data)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( head->cnt >= head->max )
	{
		SVectorReserve(liste);
		if ( head->cnt >= head->max )
			return result;
		dataPointer = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _ls_insert( dataPointer, head->dataSize, -1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
SVectorInsertBefore(LSearchResultType node, ConstPointer data)
{
	_pSVectorHead head = CastAny(_pSVectorHead,_Lnode(node));
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	if ( _Loffset(node) < 0 )
		return result;
	if ( head->cnt >= head->max )
	{
		SVectorReserve(head);
		if ( head->cnt >= head->max )
			return result;
		dataPointer = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _ls_insert( dataPointer, head->dataSize, _Loffset(node) - 1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
SVectorInsertAfter(LSearchResultType node, ConstPointer data)
{
	_pSVectorHead head = CastAny(_pSVectorHead,_Lnode(node));
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	if ( _Loffset(node) < -1 )
		return result;
	if ( head->cnt >= head->max )
	{
		SVectorReserve(head);
		if ( head->cnt >= head->max )
			return result;
		dataPointer = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _ls_insert( dataPointer, head->dataSize, _Loffset(node), data, &(head->cnt) );
	return result;
}

void __stdcall 
SVectorRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pSVectorHead head = CastAny(_pSVectorHead,_Lnode(node));
	Pointer dataPointer = head->data;

	assert(freeFunc != NULL);
	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return;
	freeFunc( _l_ptradd(dataPointer, _Loffset(node) * head->dataSize), context );
	_ls_delete( dataPointer, head->dataSize, _Loffset(node), &(head->cnt) );
}

LSearchResultType __stdcall 
SVectorInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return SVectorAppend(liste, data);
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( dataPointer, head->dataSize, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return SVectorInsertAfter(result, data);
}

bool __stdcall 
SVectorRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pSVectorHead head = CastAny(_pSVectorHead,liste);
	Pointer dataPointer = head->data;
	LSearchResultType result = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	_Lnode(result) = head;
	_Loffset(result) = _ls_bsearch( dataPointer, head->dataSize, data, head->cnt, sortFunc, UTLPTR_SEARCHMODE);
	if ( _Loffset(result) < 0 )
		return false;
	cmp = sortFunc(SVectorGetData(result), data);
	while ( cmp == 0 )
	{
		SVectorRemove(result, freeFunc, context);
		if ( _Loffset(result) >= Cast(TListIndex, head->cnt) )
			break;
		cmp = sortFunc(SVectorGetData(result), data);
	}
	return true;
}

Pointer __stdcall 
SVectorGetData(LSearchResultType node)
{
	_pSVectorHead head = CastAny(_pSVectorHead,_Lnode(node));
	Pointer dataPointer = head->data;

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || LPtrCheck(node) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return NULL;
	return _l_ptradd(dataPointer, _Loffset(node) * head->dataSize);
}

void __stdcall 
SVectorSetData(LSearchResultType node, ConstPointer data)
{
	_pSVectorHead head = CastAny(_pSVectorHead,_Lnode(node));
	Pointer dataPointer = head->data;

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return;
	s_memcpy_s(_l_ptradd(dataPointer, _Loffset(node) * head->dataSize), head->dataSize, CastMutable(Pointer, data), head->dataSize);
}

/*			  */
/* SAVLBinaryTree */
/*			  */
struct _tagSAVLBinaryTreeHead;
typedef struct _tagSAVLBinaryTreeNode
{
	Ptr(struct _tagSAVLBinaryTreeHead) head;
	Ptr(struct _tagSAVLBinaryTreeNode) parent;
	Ptr(struct _tagSAVLBinaryTreeNode) left;
	Ptr(struct _tagSAVLBinaryTreeNode) right;
	TListCnt height;
} _SAVLBinaryTreeNode, *_pSAVLBinaryTreeNode;
#define szSAVLBinaryTreeNode sizeof(_SAVLBinaryTreeNode)

typedef struct _tagSAVLBinaryTreeHead
{
	Ptr(struct _tagSAVLBinaryTreeNode) root;
	TListCnt nodeCount;
	dword dataSize;
} _SAVLBinaryTreeHead, *_pSAVLBinaryTreeHead;
#define szSAVLBinaryTreeHead sizeof(_SAVLBinaryTreeHead)

Pointer __stdcall 
SAVLBinaryTreeOpen(DECL_FILE_LINE dword datasize)
{
#ifdef __DEBUG__
	_pSAVLBinaryTreeHead p = CastAnyPtr(_SAVLBinaryTreeHead, TFallocDbg(szSAVLBinaryTreeHead, file, line));
#else
	_pSAVLBinaryTreeHead p = CastAnyPtr(_SAVLBinaryTreeHead, TFalloc(szSAVLBinaryTreeHead));
#endif

	assert(datasize > 0);
	p->dataSize = datasize;
	return p;
}

TListCnt __stdcall 
SAVLBinaryTreeCount(Pointer liste)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);

	assert(liste != NULL);
	return head->nodeCount;
}

static TListCnt __stdcall
SAVLBinaryTreeHeightHelper(_pSAVLBinaryTreeNode node)
{
	if (PtrCheck(node))
		return 0;
	return node->height;
}

TListCnt __stdcall 
SAVLBinaryTreeHeight(Pointer liste)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);

	assert(liste != NULL);
	return SAVLBinaryTreeHeightHelper(head->root);
}

static void __stdcall 
SAVLBinaryTreeCloseHelper(_pSAVLBinaryTreeNode node)
{
	_pSAVLBinaryTreeNode nodeLeft = node->left;
	_pSAVLBinaryTreeNode nodeRight = node->right;

	if ( NotPtrCheck(nodeLeft) )
	{
		SAVLBinaryTreeCloseHelper(nodeLeft);
		TFfree(nodeLeft);
	}
	if ( NotPtrCheck(nodeRight) )
	{
		SAVLBinaryTreeCloseHelper(nodeRight);
		TFfree(nodeRight);
	}
}

void __stdcall 
SAVLBinaryTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);
	LSearchResultType result = SAVLBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(freeFunc != NULL);
	while ( !LPtrCheck(result) )
	{
		d = SAVLBinaryTreeGetData(result);
		freeFunc(d, context);
		result = SAVLBinaryTreeNext(result);
	}
	if ( PtrCheck(liste) )
		return;
	if ( NotPtrCheck(head->root) )
	{
		SAVLBinaryTreeCloseHelper(head->root);
		TFfree(head->root);
	}
	TFfree(head);
}

LSearchResultType __stdcall 
SAVLBinaryTreeBegin(Pointer liste)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;
	_pSAVLBinaryTreeNode node;

	assert(liste != NULL);
	if ( PtrCheck(head->root) )
		return result;
	node = head->root;
	while ( NotPtrCheck(node->left) )
		node = node->left;
	_Lnode(result) = node;
	return result;
}

LSearchResultType __stdcall 
SAVLBinaryTreeNext(LSearchResultType node)
{
	_pSAVLBinaryTreeNode node1 = CastAnyPtr(_SAVLBinaryTreeNode, _Lnode(node));
	_pSAVLBinaryTreeNode node2 = NULL;
	LSearchResultType result = _LNULL;

	if ( PtrCheck(node1) )
		return result;
	if ( NotPtrCheck(node1->right) )
	{
		node2 = node1->right;
		while ( NotPtrCheck(node2->left) )
			node2 = node2->left;
	}
	else
	{
		node2 = node1->parent;
		while ( NotPtrCheck(node2) )
		{
			if ( node2->left == node1 )
				break;
			node1 = node2;
			node2 = node2->parent;
		}
	}
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall 
SAVLBinaryTreePrev(LSearchResultType node)
{
	_pSAVLBinaryTreeNode node1 = CastAnyPtr(_SAVLBinaryTreeNode, _Lnode(node));
	_pSAVLBinaryTreeNode node2 = NULL;
	LSearchResultType result = _LNULL;

	if ( PtrCheck(node1) )
		return result;
	if ( NotPtrCheck(node1->left) )
	{
		node2 = node1->left;
		while ( NotPtrCheck(node2->right) )
			node2 = node2->right;
	}
	else
	{
		node2 = node1->parent;
		while ( NotPtrCheck(node2) )
		{
			if ( node2->right == node1 )
				break;
			node1 = node2;
			node2 = node2->parent;
		}
	}
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall 
SAVLBinaryTreeLast(Pointer liste)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;
	_pSAVLBinaryTreeNode node;

	assert(liste != NULL);
	if ( PtrCheck(head->root) )
		return result;
	node = head->root;
	while ( NotPtrCheck(node->right) )
		node = node->right;
	_Lnode(result) = node;
	return result;
}

bool __stdcall
SAVLBinaryTreeForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	LSearchResultType it = _LNULL;
	bool result = true;
	Pointer d;

	assert(liste != NULL);
	assert(func != NULL);
	it = SAVLBinaryTreeBegin(liste);
	while (!LPtrCheck(it))
	{
		d = SAVLBinaryTreeGetData(it);
		if (0 == func(d, context))
		{
			result = false;
			break;
		}
		it = SAVLBinaryTreeNext(it);
	}
	return result;
}

LSearchResultType __stdcall
SAVLBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	LSearchResultType result = SAVLBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while ( !LPtrCheck(result) )
	{
		d = SAVLBinaryTreeGetData(result);
		if ( 0 == findFunc(d, data) )
			return result;
		result = SAVLBinaryTreeNext(result);
	}
	return _LNULL;
}

static _pSAVLBinaryTreeNode __stdcall
SAVLBinaryTreeFindSortedHelper(_pSAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(_l_ptradd(node, szSAVLBinaryTreeNode), data);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return SAVLBinaryTreeFindSortedHelper(node->left, data, sortFunc);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return SAVLBinaryTreeFindSortedHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall 
SAVLBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = SAVLBinaryTreeFindSortedHelper(head->root, data, sortFunc);
	return result;
}

static _pSAVLBinaryTreeNode __stdcall
SAVLBinaryTreeUpperBoundHelper(_pSAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(_l_ptradd(node, szSAVLBinaryTreeNode), data);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return SAVLBinaryTreeUpperBoundHelper(node->left, data, sortFunc);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return SAVLBinaryTreeUpperBoundHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall 
SAVLBinaryTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = SAVLBinaryTreeUpperBoundHelper(head->root, data, sortFunc);
	return result;
}

static _pSAVLBinaryTreeNode __stdcall
SAVLBinaryTreeLowerBoundHelper(_pSAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(_l_ptradd(node, szSAVLBinaryTreeNode), data);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return SAVLBinaryTreeLowerBoundHelper(node->left, data, sortFunc);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return SAVLBinaryTreeLowerBoundHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall 
SAVLBinaryTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = SAVLBinaryTreeLowerBoundHelper(head->root, data, sortFunc);
	return result;
}

static _pSAVLBinaryTreeNode __stdcall
SAVLBinaryTreeRotateLeft(_pSAVLBinaryTreeNode node)
{
	_pSAVLBinaryTreeNode right = node->right;

	if (NotPtrCheck(right->left))
	{
		node->right = right->left;
		node->right->parent = node;
	}
	else
		node->right = NULL;
	right->left = node;
	right->parent = node->parent;
	node->parent = right;

	node->height = Max(SAVLBinaryTreeHeightHelper(node->left), SAVLBinaryTreeHeightHelper(node->right)) + 1;
	right->height = Max(SAVLBinaryTreeHeightHelper(right->left), SAVLBinaryTreeHeightHelper(right->right)) + 1;

	return right;
}

static _pSAVLBinaryTreeNode __stdcall
SAVLBinaryTreeRotateRight(_pSAVLBinaryTreeNode node)
{
	_pSAVLBinaryTreeNode left = node->left;

	if (NotPtrCheck(left->right))
	{
		node->left = left->right;
		node->left->parent = node;
	}
	else
		node->left = NULL;
	left->right = node;
	left->parent = node->parent;
	node->parent = left;

	node->height = Max(SAVLBinaryTreeHeightHelper(node->left), SAVLBinaryTreeHeightHelper(node->right)) + 1;
	left->height = Max(SAVLBinaryTreeHeightHelper(left->left), SAVLBinaryTreeHeightHelper(left->right)) + 1;

	return left;
}

static sdword __stdcall
SAVLBinaryTreeBalance(_pSAVLBinaryTreeNode node)
{
	if (PtrCheck(node))
		return 0;
	return SAVLBinaryTreeHeightHelper(node->left) - SAVLBinaryTreeHeightHelper(node->right);
}

static _pSAVLBinaryTreeNode __stdcall
SAVLBinaryTreeInsertSortedHelper(_pSAVLBinaryTreeHead head, _pSAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc, _pSAVLBinaryTreeNode* result)
{
	sword cmp;
	sdword balance;

	if (PtrCheck(node))
	{
		node = CastAnyPtr(_SAVLBinaryTreeNode, TFalloc(szSAVLBinaryTreeNode + head->dataSize));
		if (PtrCheck(node))
			return NULL;
		s_memcpy_s(_l_ptradd(node, szSAVLBinaryTreeNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
		node->head = head;
		node->height = 1;
		++(head->nodeCount);
		if (NotPtrCheck(result))
			*result = node;
		return node;
	}
	cmp = sortFunc(_l_ptradd(node, szSAVLBinaryTreeNode), data);
	if (cmp == 0)
		return node;
	if (cmp > 0)
	{
		node->left = SAVLBinaryTreeInsertSortedHelper(head, node->left, data, sortFunc, result);
		if (NotPtrCheck(node->left))
			node->left->parent = node;
	}
	else
	{
		node->right = SAVLBinaryTreeInsertSortedHelper(head, node->right, data, sortFunc, result);
		if (NotPtrCheck(node->right))
			node->right->parent = node;
	}
	node->height = Max(SAVLBinaryTreeHeightHelper(node->left), SAVLBinaryTreeHeightHelper(node->right)) + 1;
	balance = SAVLBinaryTreeBalance(node);
	if ((balance > 1) && (sortFunc(_l_ptradd(node->left, szSAVLBinaryTreeNode), data) > 0))
		return SAVLBinaryTreeRotateRight(node);
	if ((balance < -1) && (sortFunc(_l_ptradd(node->right, szSAVLBinaryTreeNode), data) < 0))
		return SAVLBinaryTreeRotateLeft(node);
	if ((balance > 1) && (sortFunc(_l_ptradd(node->left, szSAVLBinaryTreeNode), data) < 0))
	{
		node->left = SAVLBinaryTreeRotateLeft(node->left);
		return SAVLBinaryTreeRotateRight(node);
	}
	if ((balance < -1) && (sortFunc(_l_ptradd(node->right, szSAVLBinaryTreeNode), data) > 0))
	{
		node->right = SAVLBinaryTreeRotateRight(node->right);
		return SAVLBinaryTreeRotateLeft(node);
	}
	return node;
}

LSearchResultType __stdcall
SAVLBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);
	_pSAVLBinaryTreeNode node1 = head->root;
	_pSAVLBinaryTreeNode node2 = NULL;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(node1))
	{
		node2 = CastAnyPtr(_SAVLBinaryTreeNode, TFalloc(szSAVLBinaryTreeNode + head->dataSize));
		if (PtrCheck(node2))
			return result;
		s_memcpy_s(_l_ptradd(node2, szSAVLBinaryTreeNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
		node2->head = head;
		node2->height = 1;
		head->root = node2;
		++(head->nodeCount);
		_Lnode(result) = node2;
		return result;
	}
	node2 = NULL;
	head->root = SAVLBinaryTreeInsertSortedHelper(head, node1, data, sortFunc, &node2);
	_Lnode(result) = node2;
	return result;
}

static _pSAVLBinaryTreeNode __stdcall
SAVLBinaryTreeRemoveSortedHelper(_pSAVLBinaryTreeHead head, _pSAVLBinaryTreeNode root, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context, bool freedata, bool *result)
{
	_pSAVLBinaryTreeNode temp;
	sword cmp;
	sdword balance;

	if (root == NULL)
		return root;

	cmp = sortFunc(_l_ptradd(root, szSAVLBinaryTreeNode), data);
	if (cmp > 0)
	{
		root->left = SAVLBinaryTreeRemoveSortedHelper(head, root->left, data, sortFunc, freeFunc, context, freedata, result);
		if (NotPtrCheck(root->left))
			root->left->parent = root;
	}
	else if (cmp < 0)
	{
		root->right = SAVLBinaryTreeRemoveSortedHelper(head, root->right, data, sortFunc, freeFunc, context, freedata, result);
		if (NotPtrCheck(root->right))
			root->right->parent = root;
	}
	else
	{
		if (freedata)
			freeFunc(_l_ptradd(root, szSAVLBinaryTreeNode), context);
		if (PtrCheck(root->left) || PtrCheck(root->right))
		{
			temp = NotPtrCheck(root->left) ? root->left : root->right;
			if (PtrCheck(temp))
			{
				temp = root;
				root = NULL;
			}
			else
			{
				s_memcpy_s(_l_ptradd(root, szSAVLBinaryTreeNode), head->dataSize, _l_ptradd(temp, szSAVLBinaryTreeNode), head->dataSize);
				root->left = temp->left;
				if (NotPtrCheck(root->left))
					root->left->parent = root;
				root->right = temp->right;
				if (NotPtrCheck(root->right))
					root->right->parent = root;
			}
			--(head->nodeCount);
			TFfree(temp);
			if (NotPtrCheck(result))
				*result = true;
		}
		else
		{
			temp = root->right;
			while (temp->left != NULL)
				temp = temp->left;
			s_memcpy_s(_l_ptradd(root, szSAVLBinaryTreeNode), head->dataSize, _l_ptradd(temp, szSAVLBinaryTreeNode), head->dataSize);
			root->right = SAVLBinaryTreeRemoveSortedHelper(head, root->right, _l_ptradd(temp, szSAVLBinaryTreeNode), sortFunc, freeFunc, context, false, result);
			if (NotPtrCheck(root->right))
				root->right->parent = root;
		}
	}
	if (root == NULL)
		return root;
	root->height = Max(SAVLBinaryTreeHeightHelper(root->left), SAVLBinaryTreeHeightHelper(root->right)) + 1;
	balance = SAVLBinaryTreeBalance(root);
	if ((balance > 1) && (SAVLBinaryTreeBalance(root->left) >= 0))
		return SAVLBinaryTreeRotateRight(root);
	if ((balance > 1) && (SAVLBinaryTreeBalance(root->left) < 0))
	{
		root->left = SAVLBinaryTreeRotateLeft(root->left);
		return SAVLBinaryTreeRotateRight(root);
	}
	if ((balance < -1) && (SAVLBinaryTreeBalance(root->right) <= 0))
		return SAVLBinaryTreeRotateLeft(root);
	if ((balance < -1) && (SAVLBinaryTreeBalance(root->right) > 0))
	{
		root->right = SAVLBinaryTreeRotateRight(root->right);
		return SAVLBinaryTreeRotateLeft(root);
	}
	return root;
}

bool __stdcall
SAVLBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pSAVLBinaryTreeHead head = CastAnyPtr(_SAVLBinaryTreeHead, liste);
	_pSAVLBinaryTreeNode node = head->root;
	bool result = false;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	assert(freeFunc != NULL);
	if (PtrCheck(node))
		return false;
	if (head->nodeCount == 1)
	{
		if (sortFunc(_l_ptradd(node, szSAVLBinaryTreeNode), data) == 0)
		{
			freeFunc(_l_ptradd(node, szSAVLBinaryTreeNode), context);
			--(head->nodeCount);
			head->root = NULL;
			TFfree(node);
			return true;
		}
		return false;
	}
	head->root = SAVLBinaryTreeRemoveSortedHelper(head, node, data, sortFunc, freeFunc, context, true, &result);
	return result;
}

Pointer __stdcall 
SAVLBinaryTreeGetData(LSearchResultType node)
{
	_pSAVLBinaryTreeNode node1 = CastAnyPtr(_SAVLBinaryTreeNode, _Lnode(node));

	if ( PtrCheck(node1) )
		return NULL;
	return _l_ptradd(node1, szSAVLBinaryTreeNode);
}

void __stdcall 
SAVLBinaryTreeSetData(LSearchResultType node, ConstPointer data)
{
	_pSAVLBinaryTreeNode node1 = CastAnyPtr(_SAVLBinaryTreeNode, _Lnode(node));
	_pSAVLBinaryTreeHead head = node1->head;

	assert(node1 != NULL);
	s_memcpy_s(_l_ptradd(node1, szSAVLBinaryTreeNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
}

/*			     */
/* SRBBinaryTree */
/*			     */
enum _SRBBinaryTreeNodeColor { RED, BLACK };
struct _tagSRBBinaryTreeHead;
typedef struct _tagSRBBinaryTreeNode
{
	Ptr(struct _tagSRBBinaryTreeHead) head;
	Ptr(struct _tagSRBBinaryTreeNode) parent;
	Ptr(struct _tagSRBBinaryTreeNode) left;
	Ptr(struct _tagSRBBinaryTreeNode) right;
	enum _SRBBinaryTreeNodeColor color;
} _SRBBinaryTreeNode, *_pSRBBinaryTreeNode;
#define szSRBBinaryTreeNode sizeof(_SRBBinaryTreeNode)

typedef struct _tagSRBBinaryTreeHead
{
	Ptr(struct _tagSRBBinaryTreeNode) root;
	TListCnt nodeCount;
	dword dataSize;
} _SRBBinaryTreeHead, *_pSRBBinaryTreeHead;
#define szSRBBinaryTreeHead sizeof(_SRBBinaryTreeHead)

static enum _SRBBinaryTreeNodeColor __stdcall
SRBBinaryTreeNodeColor(_pSRBBinaryTreeNode n) {
	return PtrCheck(n) ? BLACK : n->color;
}

static void __stdcall
SRBBinaryTreeVerifyProperties1(_pSRBBinaryTreeNode n)
{
	assert(SRBBinaryTreeNodeColor(n) == RED || SRBBinaryTreeNodeColor(n) == BLACK);
	if (n == NULL) return;
	SRBBinaryTreeVerifyProperties1(n->left);
	SRBBinaryTreeVerifyProperties1(n->right);
}

static void __stdcall
SRBBinaryTreeVerifyProperties2(_pSRBBinaryTreeNode root)
{
	assert(SRBBinaryTreeNodeColor(root) == BLACK);
}

static void __stdcall
SRBBinaryTreeVerifyProperties4(_pSRBBinaryTreeNode n)
{
	if (SRBBinaryTreeNodeColor(n) == RED) {
		assert(SRBBinaryTreeNodeColor(n->left) == BLACK);
		assert(SRBBinaryTreeNodeColor(n->right) == BLACK);
		assert(SRBBinaryTreeNodeColor(n->parent) == BLACK);
	}
	if (n == NULL) return;
	SRBBinaryTreeVerifyProperties4(n->left);
	SRBBinaryTreeVerifyProperties4(n->right);
}

static void __stdcall
SRBBinaryTreeVerifyProperties5Helper(_pSRBBinaryTreeNode n, int black_count, int* path_black_count)
{
	if (SRBBinaryTreeNodeColor(n) == BLACK) {
		black_count++;
	}
	if (n == NULL) {
		if (*path_black_count == -1) {
			*path_black_count = black_count;
		}
		else {
			assert(black_count == *path_black_count);
		}
		return;
	}
	SRBBinaryTreeVerifyProperties5Helper(n->left, black_count, path_black_count);
	SRBBinaryTreeVerifyProperties5Helper(n->right, black_count, path_black_count);
}

static void __stdcall
SRBBinaryTreeVerifyProperties5(_pSRBBinaryTreeNode root)
{
	int black_count_path = -1;
	SRBBinaryTreeVerifyProperties5Helper(root, 0, &black_count_path);
}

static void __stdcall
SRBBinaryTreeVerifyProperties(_pSRBBinaryTreeHead t)
{
#ifdef VERIFY_RBTREE
	SRBBinaryTreeVerifyProperties1(t->root);
	SRBBinaryTreeVerifyProperties2(t->root);
	SRBBinaryTreeVerifyProperties4(t->root);
	SRBBinaryTreeVerifyProperties5(t->root);
#endif
}

Pointer __stdcall
SRBBinaryTreeOpen(DECL_FILE_LINE  dword datasize)
{
#ifdef __DEBUG__
	_pSRBBinaryTreeHead p = CastAnyPtr(_SRBBinaryTreeHead, TFallocDbg(szSRBBinaryTreeHead, file, line));
#else
	_pSRBBinaryTreeHead p = CastAnyPtr(_SRBBinaryTreeHead, TFalloc(szSRBBinaryTreeHead));
#endif
	p->dataSize = datasize;
	SRBBinaryTreeVerifyProperties(p);
	return p;
}

TListCnt __stdcall
SRBBinaryTreeCount(Pointer liste)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);

	assert(liste != NULL);
	return head->nodeCount;
}

static TListCnt __stdcall
SRBBinaryTreeHeightHelper(_pSRBBinaryTreeNode node)
{
	TListCnt l;
	TListCnt r;

	if (PtrCheck(node))
		return 0;
	l = SRBBinaryTreeHeightHelper(node->left);
	r = SRBBinaryTreeHeightHelper(node->right);
	return Max(l, r) + 1;
}

TListCnt __stdcall
SRBBinaryTreeHeight(Pointer liste)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);

	assert(liste != NULL);
	return SRBBinaryTreeHeightHelper(head->root);
}

static void __stdcall
SRBBinaryTreeCloseHelper(_pSRBBinaryTreeNode node)
{
	_pSRBBinaryTreeNode nodeLeft = node->left;
	_pSRBBinaryTreeNode nodeRight = node->right;

	if (NotPtrCheck(nodeLeft))
	{
		SRBBinaryTreeCloseHelper(nodeLeft);
		TFfree(nodeLeft);
	}
	if (NotPtrCheck(nodeRight))
	{
		SRBBinaryTreeCloseHelper(nodeRight);
		TFfree(nodeRight);
	}
}

void __stdcall
SRBBinaryTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);
	LSearchResultType result = SRBBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(freeFunc != NULL);
	while (!LPtrCheck(result))
	{
		d = SRBBinaryTreeGetData(result);
		freeFunc(d, context);
		result = SRBBinaryTreeNext(result);
	}
	if (PtrCheck(liste))
		return;
	if (NotPtrCheck(head->root))
	{
		SRBBinaryTreeCloseHelper(head->root);
		TFfree(head->root);
	}
	TFfree(head);
}

LSearchResultType __stdcall
SRBBinaryTreeBegin(Pointer liste)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;
	_pSRBBinaryTreeNode node;

	assert(liste != NULL);
	if (PtrCheck(head->root))
		return result;
	node = head->root;
	while (NotPtrCheck(node->left))
		node = node->left;
	_Lnode(result) = node;
	return result;
}

LSearchResultType __stdcall
SRBBinaryTreeNext(LSearchResultType node)
{
	_pSRBBinaryTreeNode node1 = CastAnyPtr(_SRBBinaryTreeNode, _Lnode(node));
	_pSRBBinaryTreeNode node2 = NULL;
	LSearchResultType result = _LNULL;

	if (PtrCheck(node1))
		return result;
	if (NotPtrCheck(node1->right))
	{
		node2 = node1->right;
		while (NotPtrCheck(node2->left))
			node2 = node2->left;
	}
	else
	{
		node2 = node1->parent;
		while (NotPtrCheck(node2))
		{
			if (node2->left == node1)
				break;
			node1 = node2;
			node2 = node2->parent;
		}
	}
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall
SRBBinaryTreePrev(LSearchResultType node)
{
	_pSRBBinaryTreeNode node1 = CastAnyPtr(_SRBBinaryTreeNode, _Lnode(node));
	_pSRBBinaryTreeNode node2 = NULL;
	LSearchResultType result = _LNULL;

	if (PtrCheck(node1))
		return result;
	if (NotPtrCheck(node1->left))
	{
		node2 = node1->left;
		while (NotPtrCheck(node2->right))
			node2 = node2->right;
	}
	else
	{
		node2 = node1->parent;
		while (NotPtrCheck(node2))
		{
			if (node2->right == node1)
				break;
			node1 = node2;
			node2 = node2->parent;
		}
	}
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall
SRBBinaryTreeLast(Pointer liste)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;
	_pSRBBinaryTreeNode node;

	assert(liste != NULL);
	if (PtrCheck(head->root))
		return result;
	node = head->root;
	while (NotPtrCheck(node->right))
		node = node->right;
	_Lnode(result) = node;
	return result;
}

bool __stdcall
SRBBinaryTreeForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	LSearchResultType it = SRBBinaryTreeBegin(liste);
	bool result = true;
	Pointer d;

	assert(liste != NULL);
	assert(func != NULL);
	while (!LPtrCheck(it))
	{
		d = SRBBinaryTreeGetData(it);
		if (0 == func(d, context))
		{
			result = false;
			break;
		}
		it = SRBBinaryTreeNext(it);
	}
	return result;
}

LSearchResultType __stdcall
SRBBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	LSearchResultType result = SRBBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while (!LPtrCheck(result))
	{
		d = SRBBinaryTreeGetData(result);
		if (0 == findFunc(d, data))
			return result;
		result = SRBBinaryTreeNext(result);
	}
	return _LNULL;
}

static _pSRBBinaryTreeNode __stdcall
SRBBinaryTreeFindSortedHelper(_pSRBBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(_l_ptradd(node, szSRBBinaryTreeNode), data);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return SRBBinaryTreeFindSortedHelper(node->left, data, sortFunc);
	}
	if (PtrCheck(node->right))
		return NULL;
	return SRBBinaryTreeFindSortedHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall
SRBBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = SRBBinaryTreeFindSortedHelper(head->root, data, sortFunc);
	return result;
}

static _pSRBBinaryTreeNode __stdcall
SRBBinaryTreeUpperBoundHelper(_pSRBBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(_l_ptradd(node, szSRBBinaryTreeNode), data);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return SRBBinaryTreeUpperBoundHelper(node->left, data, sortFunc);
	}
	if (PtrCheck(node->right))
		return NULL;
	return SRBBinaryTreeUpperBoundHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall
SRBBinaryTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = SRBBinaryTreeUpperBoundHelper(head->root, data, sortFunc);
	return result;
}

static _pSRBBinaryTreeNode __stdcall
SRBBinaryTreeLowerBoundHelper(_pSRBBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(_l_ptradd(node, szSRBBinaryTreeNode), data);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return SRBBinaryTreeLowerBoundHelper(node->left, data, sortFunc);
	}
	if (PtrCheck(node->right))
		return NULL;
	return SRBBinaryTreeLowerBoundHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall
SRBBinaryTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = SRBBinaryTreeLowerBoundHelper(head->root, data, sortFunc);
	return result;
}

static _pSRBBinaryTreeNode __stdcall
SRBBinaryTreeGrandParent(_pSRBBinaryTreeNode node)
{
	assert(NotPtrCheck(node));
	assert(NotPtrCheck(node->parent));
	assert(NotPtrCheck(node->parent->parent));
	return node->parent->parent;
}

static _pSRBBinaryTreeNode __stdcall
SRBBinaryTreeSibling(_pSRBBinaryTreeNode node)
{
	assert(NotPtrCheck(node));
	assert(NotPtrCheck(node->parent));
	if (node == node->parent->left)
		return node->parent->right;
	return node->parent->left;
}

static _pSRBBinaryTreeNode __stdcall
SRBBinaryTreeUncle(_pSRBBinaryTreeNode node)
{
	assert(NotPtrCheck(node));
	assert(NotPtrCheck(node->parent));
	assert(NotPtrCheck(node->parent->parent));
	return SRBBinaryTreeSibling(node->parent);
}

static void __stdcall
SRBBinaryTreeReplaceNode(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode oldn, _pSRBBinaryTreeNode newn)
{
	if (PtrCheck(oldn->parent))
		t->root = newn;
	else if (oldn == oldn->parent->left)
		oldn->parent->left = newn;
	else
		oldn->parent->right = newn;
	if (NotPtrCheck(newn))
		newn->parent = oldn->parent;
}

static void __stdcall
SRBBinaryTreeRotateLeft(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	_pSRBBinaryTreeNode r = n->right;

	SRBBinaryTreeReplaceNode(t, n, r);
	n->right = r->left;
	if (NotPtrCheck(r->left))
		r->left->parent = n;
	r->left = n;
	n->parent = r;
}

static void __stdcall
SRBBinaryTreeRotateRight(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	_pSRBBinaryTreeNode l = n->left;

	SRBBinaryTreeReplaceNode(t, n, l);
	n->left = l->right;
	if (NotPtrCheck(l->right))
		l->right->parent = n;
	l->right = n;
	n->parent = l;
}

static void __stdcall
SRBBinaryTreeInsertCase1(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n);

static void __stdcall
SRBBinaryTreeInsertCase5(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	n->parent->color = BLACK;
	SRBBinaryTreeGrandParent(n)->color = RED;
	if ((n == n->parent->left) && (n->parent == SRBBinaryTreeGrandParent(n)->left))
		SRBBinaryTreeRotateRight(t, SRBBinaryTreeGrandParent(n));
	else
	{
		assert((n == n->parent->right) && (n->parent == SRBBinaryTreeGrandParent(n)->right));
		SRBBinaryTreeRotateLeft(t, SRBBinaryTreeGrandParent(n));
	}
}

static void __stdcall
SRBBinaryTreeInsertCase4(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if ((n == n->parent->right) && (n->parent == SRBBinaryTreeGrandParent(n)->left))
	{
		SRBBinaryTreeRotateLeft(t, n->parent);
		n = n->left;
	}
	else if ((n == n->parent->left) && (n->parent == SRBBinaryTreeGrandParent(n)->right))
	{
		SRBBinaryTreeRotateRight(t, n->parent);
		n = n->right;
	}
	SRBBinaryTreeInsertCase5(t, n);
}

static void __stdcall
SRBBinaryTreeInsertCase3(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if (SRBBinaryTreeNodeColor(SRBBinaryTreeUncle(n)) == RED) {
		n->parent->color = BLACK;
		SRBBinaryTreeUncle(n)->color = BLACK;
		SRBBinaryTreeGrandParent(n)->color = RED;
		SRBBinaryTreeInsertCase1(t, SRBBinaryTreeGrandParent(n));
	}
	else
		SRBBinaryTreeInsertCase4(t, n);
}

static void __stdcall
SRBBinaryTreeInsertCase2(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if (SRBBinaryTreeNodeColor(n->parent) == BLACK)
		return;
	SRBBinaryTreeInsertCase3(t, n);
}

static void __stdcall
SRBBinaryTreeInsertCase1(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if (PtrCheck(n->parent))
		n->color = BLACK;
	else
		SRBBinaryTreeInsertCase2(t, n);
}

LSearchResultType __stdcall
SRBBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);
	_pSRBBinaryTreeNode inserted_node = CastAnyPtr(_SRBBinaryTreeNode, TFalloc(szSRBBinaryTreeNode + head->dataSize));
	LSearchResultType result = _LNULL;

	if (PtrCheck(inserted_node))
		return result;
	s_memcpy_s(_l_ptradd(inserted_node, szSRBBinaryTreeNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
	inserted_node->head = head;
	inserted_node->color = RED;
	++(head->nodeCount);

	if (head->root == NULL) {
		head->root = inserted_node;
	}
	else {
		_pSRBBinaryTreeNode n = head->root;

		while (true)
		{
			sword cmp = sortFunc(_l_ptradd(n, szSRBBinaryTreeNode), data);

			if (cmp == 0)
				return result;
			if (cmp > 0) {
				if (n->left == NULL) {
					n->left = inserted_node;
					break;
				}
				else {
					n = n->left;
				}
			}
			else {
				assert(cmp < 0);
				if (n->right == NULL) {
					n->right = inserted_node;
					break;
				}
				else {
					n = n->right;
				}
			}
		}
		inserted_node->parent = n;
	}
	SRBBinaryTreeInsertCase1(head, inserted_node);
	SRBBinaryTreeVerifyProperties(head);

	_Lnode(result) = inserted_node;
	return result;
}

static void __stdcall
SRBBinaryTreeDeleteCase1(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n);

static void __stdcall
SRBBinaryTreeDeleteCase6(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	SRBBinaryTreeSibling(n)->color = SRBBinaryTreeNodeColor(n->parent);
	n->parent->color = BLACK;
	if (n == n->parent->left) {
		assert(SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->right) == RED);
		SRBBinaryTreeSibling(n)->right->color = BLACK;
		SRBBinaryTreeRotateLeft(t, n->parent);
	}
	else
	{
		assert(SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->left) == RED);
		SRBBinaryTreeSibling(n)->left->color = BLACK;
		SRBBinaryTreeRotateRight(t, n->parent);
	}
}

static void __stdcall
SRBBinaryTreeDeleteCase5(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if (n == n->parent->left &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)) == BLACK &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->left) == RED &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->right) == BLACK)
	{
		SRBBinaryTreeSibling(n)->color = RED;
		SRBBinaryTreeSibling(n)->left->color = BLACK;
		SRBBinaryTreeRotateRight(t, SRBBinaryTreeSibling(n));
	}
	else if (n == n->parent->right &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)) == BLACK &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->right) == RED &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->left) == BLACK)
	{
		SRBBinaryTreeSibling(n)->color = RED;
		SRBBinaryTreeSibling(n)->right->color = BLACK;
		SRBBinaryTreeRotateLeft(t, SRBBinaryTreeSibling(n));
	}
	SRBBinaryTreeDeleteCase6(t, n);
}

static void __stdcall
SRBBinaryTreeDeleteCase4(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if (SRBBinaryTreeNodeColor(n->parent) == RED &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)) == BLACK &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->left) == BLACK &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->right) == BLACK)
	{
		SRBBinaryTreeSibling(n)->color = RED;
		n->parent->color = BLACK;
	}
	else
		SRBBinaryTreeDeleteCase5(t, n);
}

static void __stdcall
SRBBinaryTreeDeleteCase3(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if (SRBBinaryTreeNodeColor(n->parent) == BLACK &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)) == BLACK &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->left) == BLACK &&
		SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)->right) == BLACK)
	{
		SRBBinaryTreeSibling(n)->color = RED;
		SRBBinaryTreeDeleteCase1(t, n->parent);
	}
	else
		SRBBinaryTreeDeleteCase4(t, n);
}

static void __stdcall
SRBBinaryTreeDeleteCase2(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if (SRBBinaryTreeNodeColor(SRBBinaryTreeSibling(n)) == RED)
	{
		n->parent->color = RED;
		SRBBinaryTreeSibling(n)->color = BLACK;
		if (n == n->parent->left)
			SRBBinaryTreeRotateLeft(t, n->parent);
		else
			SRBBinaryTreeRotateRight(t, n->parent);
	}
	SRBBinaryTreeDeleteCase3(t, n);
}

static void __stdcall
SRBBinaryTreeDeleteCase1(_pSRBBinaryTreeHead t, _pSRBBinaryTreeNode n)
{
	if (n->parent == NULL)
		return;
	SRBBinaryTreeDeleteCase2(t, n);
}

bool __stdcall
SRBBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pSRBBinaryTreeHead head = CastAnyPtr(_SRBBinaryTreeHead, liste);
	_pSRBBinaryTreeNode child;
	_pSRBBinaryTreeNode n = head->root;

	if (PtrCheck(n))
		return false;
	n = SRBBinaryTreeFindSortedHelper(n, data, sortFunc);
	if (PtrCheck(n))
		return false;
	if (NotPtrCheck(n->left) && NotPtrCheck(n->right))
	{
		_pSRBBinaryTreeNode pred = n->left;
		Pointer vData = TFalloc(head->dataSize);

		while (pred->right != NULL)
			pred = pred->right;
		s_memcpy_s(vData, head->dataSize, _l_ptradd(n, szSRBBinaryTreeNode), head->dataSize);
		s_memcpy_s(_l_ptradd(n, szSRBBinaryTreeNode), head->dataSize, _l_ptradd(pred, szSRBBinaryTreeNode), head->dataSize);
		s_memcpy_s(_l_ptradd(pred, szSRBBinaryTreeNode), head->dataSize, vData, head->dataSize);
		TFfree(vData);
		n = pred;
	}

	assert(PtrCheck(n->left) || PtrCheck(n->right));
	child = PtrCheck(n->right) ? n->left : n->right;
	if (SRBBinaryTreeNodeColor(n) == BLACK) {
		n->color = SRBBinaryTreeNodeColor(child);
		SRBBinaryTreeDeleteCase1(head, n);
	}
	SRBBinaryTreeReplaceNode(head, n, child);
	if (PtrCheck(n->parent) && NotPtrCheck(child))
		child->color = BLACK;

	freeFunc(_l_ptradd(n, szSRBBinaryTreeNode), context);
	TFfree(n);
	--(head->nodeCount);

	SRBBinaryTreeVerifyProperties(head);
	return true;
}

Pointer __stdcall
SRBBinaryTreeGetData(LSearchResultType node)
{
	_pSRBBinaryTreeNode node1 = CastAnyPtr(_SRBBinaryTreeNode, _Lnode(node));

	if (PtrCheck(node1))
		return NULL;
	return _l_ptradd(node1, szSRBBinaryTreeNode);
}

void __stdcall
SRBBinaryTreeSetData(LSearchResultType node, ConstPointer data)
{
	_pSRBBinaryTreeNode node1 = CastAnyPtr(_SRBBinaryTreeNode, _Lnode(node));
	_pSRBBinaryTreeHead head = node1->head;

	assert(node1 != NULL);
	s_memcpy_s(_l_ptradd(node1, szSRBBinaryTreeNode), head->dataSize, CastMutable(Pointer, data), head->dataSize);
}

/*			 */
/* SBTree */
/*			 */
struct _tagSBTreeHead;
typedef struct _tagSBTreeNode
{
	struct _tagSBTreeHead* head;
	struct _tagSBTreeNode* parent;
	TListCnt cnt;
	Pointer key;
	dword isData;
} _SBTreeNode, *_pSBTreeNode;
#define szSBTreeNode sizeof(_SBTreeNode)

typedef struct _tagSBTreeHead
{
	TListCnt nodeCount;
	TListCnt maxEntriesPerNode;
	_pSBTreeNode root;
	dword dataSize;
} _SBTreeHead, *_pSBTreeHead;
#define szSBTreeHead sizeof(_SBTreeHead)

Pointer __stdcall 
SBTreeOpen(DECL_FILE_LINE TListCnt maxEntriesPerNode, dword datasize)
{
#ifdef __DEBUG__
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, TFallocDbg(szSBTreeHead, file, line));
#else
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, TFalloc(szSBTreeHead));
#endif
	_pSBTreeNode pNode;

	if ( PtrCheck(pHead) )
		return NULL;
	assert(maxEntriesPerNode > 0);
	assert(datasize > 0);
#ifdef __DEBUG__
	pNode = CastAnyPtr(_SBTreeNode, TFallocDbg(szSBTreeNode + (maxEntriesPerNode * datasize), file, line));
#else
	pNode = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (maxEntriesPerNode * datasize)));
#endif
	if ( PtrCheck(pNode) )
	{
		TFfree(pHead);
		return NULL;
	}

	pHead->maxEntriesPerNode = maxEntriesPerNode;
	pHead->root = pNode;
	pHead->dataSize = datasize;

	pNode->head = pHead;
	pNode->isData = TRUE;

	return pHead;
}

TListCnt __stdcall 
SBTreeCount(Pointer liste)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);

	assert(liste != NULL);
	return pHead->nodeCount;
}

TListCnt __stdcall 
SBTreeHeight(Pointer liste)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;
	Array p;
	TListCnt height = 1;

	assert(liste != NULL);
	assert(pNode != NULL);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
		pNode = DerefAnyPtr(_pSBTreeNode, p);
		++height;
	}
	return height;
}

static void __stdcall
SBTreeCloseHelper(_pSBTreeNode node)
{
	Array p;
	TListCnt ix;

	if ( !(node->isData) )
	{
		for ( ix = 0, p = CastAny(Array, _l_ptradd(node, szSBTreeNode)); 
			ix < node->cnt; 
			++ix, p = CastAny(Array, _l_ptradd(p, szPointer)) 
		)
			SBTreeCloseHelper(DerefAnyPtr(_pSBTreeNode, p));
	}
	TFfree(node);
}

void __stdcall 
SBTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	LSearchResultType result = SBTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(freeFunc != NULL);
	while ( !LPtrCheck(result) )
	{
		d = SBTreeGetData(result);
		freeFunc(d, context);
		result = SBTreeNext(result);
	}
	if ( PtrCheck(liste) )
		return;
	SBTreeCloseHelper(pHead->root);
	TFfree(pHead);
}

static LSearchResultType __stdcall
SBTreeIndexHelper(_pSBTreeNode node, Ptr(TListIndex) index)
{
	Array p;
	TListCnt ix;
	LSearchResultType result = _LNULL;

	if ( !(node->isData) )
	{
		for ( ix = 0, p = CastAny(Array, _l_ptradd(node, szSBTreeNode)); 
			ix < node->cnt; 
			++ix, p = CastAny(Array, _l_ptradd(p, szPointer)) 
		)
		{
			result = SBTreeIndexHelper(DerefAnyPtr(_pSBTreeNode, p), index);
			if ( !LPtrCheck(result) )
				break;
		}
	}
	else if ( *index < Cast(TListIndex, node->cnt) )
	{
		_Lnode(result) = node;
		_Loffset(result) = *index;
	}
	else
		*index -= node->cnt;
	return result;
}

LSearchResultType __stdcall 
SBTreeIndex(Pointer liste, TListIndex index)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	if ( (0 == pHead->nodeCount) || (index < 0) || (index >= Cast(TListIndex,pHead->nodeCount)) )
		return _LNULL;
	return SBTreeIndexHelper(pNode, &index);
}

LSearchResultType __stdcall 
SBTreeBegin(Pointer liste)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;
	Array p;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
		pNode = DerefAnyPtr(_pSBTreeNode, p);
	}
	if ( 0 == pNode->cnt )
		return result;
	_Lnode(result) = pNode;
	_Loffset(result) = 0;
	return result;
}

LSearchResultType __stdcall 
SBTreeNext(LSearchResultType node)
{
	_pSBTreeNode pNode = CastAnyPtr(_SBTreeNode, _Lnode(node));
	_pSBTreeNode pNode1;
	Array p;
	sdword pos;
	LSearchResultType result = _LNULL;

	if ( PtrCheck(pNode) )
		return result;
	if ( _Loffset(node) < (Cast(TListIndex,pNode->cnt) - 1) )
	{
		_Lnode(result) = _Lnode(node);
		_Loffset(result) = _Loffset(node) + 1;
		return result;
	}
	if ( PtrCheck(pNode->parent) )
		return result;
	while ( NotPtrCheck(pNode->parent) )
	{
		pNode1 = pNode->parent;
		p = CastAny(Array, _l_ptradd(pNode1, szSBTreeNode));
		pos = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(pos >= 0);
		if ( ++pos < Cast(sdword,pNode1->cnt) )
			break;
		pNode = pNode1;
	}
	if ( PtrCheck(pNode->parent) )
		return result;
	p = CastAny(Array, _l_ptradd(p, pos * szPointer));
	pNode = DerefAnyPtr(_pSBTreeNode, p);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
		pNode = DerefAnyPtr(_pSBTreeNode, p);
	}
	_Lnode(result) = pNode;
	_Loffset(result) = 0;
	return result;
}

LSearchResultType __stdcall 
SBTreePrev(LSearchResultType node)
{
	_pSBTreeNode pNode = CastAnyPtr(_SBTreeNode, _Lnode(node));
	_pSBTreeNode pNode1;
	Array p;
	sdword pos;
	LSearchResultType result = _LNULL;

	if ( PtrCheck(pNode) )
		return result;
	if ( _Loffset(node) > 0 )
	{
		_Lnode(result) = _Lnode(node);
		_Loffset(result) = _Loffset(node) - 1;
		return result;
	}
	if ( PtrCheck(pNode->parent) )
		return result;
	while ( NotPtrCheck(pNode->parent) )
	{
		pNode1 = pNode->parent;
		p = CastAny(Array, _l_ptradd(pNode1, szSBTreeNode));
		pos = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(pos >= 0);
		if ( --pos >= 0 )
			break;
		pNode = pNode1;
	}
	if ( PtrCheck(pNode->parent) )
		return result;
	p = CastAny(Array, _l_ptradd(p, pos * szPointer));
	pNode = DerefAnyPtr(_pSBTreeNode, p);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode + ((pNode->cnt - 1) * szPointer)));
		pNode = DerefAnyPtr(_pSBTreeNode, p);
	}
	_Lnode(result) = pNode;
	_Loffset(result) = pNode->cnt - 1;
	return result;
}

LSearchResultType __stdcall 
SBTreeLast(Pointer liste)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;
	Array p;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode + ((pNode->cnt - 1) * szPointer)));
		pNode = DerefAnyPtr(_pSBTreeNode, p);
	}
	if ( 0 == pNode->cnt )
		return result;
	_Lnode(result) = pNode;
	_Loffset(result) = pNode->cnt - 1;
	return result;
}

bool __stdcall
SBTreeForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	LSearchResultType it = _LNULL;
	Pointer d;
	bool result = true;

	assert(liste != NULL);
	assert(func != NULL);
	it = SBTreeBegin(liste);
	while (!LPtrCheck(it))
	{
		d = SBTreeGetData(it);
		if (0 == func(d, context))
		{
			result = false;
			break;
		}
		it = SBTreeNext(it);
	}
	return result;
}

LSearchResultType __stdcall
SBTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	LSearchResultType result = _LNULL;
	Pointer d;

	assert(liste != NULL);
	assert(findFunc != NULL);
	result = SBTreeBegin(liste);
	while ( !LPtrCheck(result) )
	{
		d = SBTreeGetData(result);
		if ( 0 == findFunc(d, data) )
			return result;
		result = SBTreeNext(result);
	}
	return result;
}

static LSearchResultType __stdcall 
SBTreeFindSortedHelper(_pSBTreeNode pNode, ConstPointer data, dword datasize, TSearchAndSortFunc sortFunc)
{
	_pSBTreeNode pNode1;
	Array p;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	if ( pNode->isData )
	{
        sdword ix = _ls_bsearch( _l_ptradd(pNode, szSBTreeNode), datasize, data, pNode->cnt, sortFunc, UTLPTR_MATCHMODE );
		if ( ix < 0 )
			return result;
		_Lnode(result) = pNode;
		_Loffset(result) = ix;
		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode)); ix < pNode->cnt; ++ix, p = CastAny(Array, _l_ptradd(p, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pSBTreeNode, _l_ptradd(p, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
			return SBTreeFindSortedHelper(DerefAnyPtr(_pSBTreeNode, p), data, datasize, sortFunc);
	}
	return result;
}

LSearchResultType __stdcall 
SBTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return SBTreeFindSortedHelper(pNode, data, pHead->dataSize, sortFunc);
}

static LSearchResultType __stdcall 
SBTreeUpperBoundHelper(_pSBTreeNode pNode, ConstPointer data, dword datasize, TSearchAndSortFunc sortFunc)
{
	_pSBTreeNode pNode1;
	Array p;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	if ( pNode->isData )
	{
        sdword ix = _ls_bsearch( _l_ptradd(pNode, szSBTreeNode), datasize, data, pNode->cnt, sortFunc, UTLPTR_INSERTMODE );
		if ( ix < 0 )
			return result;
		_Lnode(result) = pNode;
		_Loffset(result) = ix;
		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode)); ix < pNode->cnt; ++ix, p = CastAny(Array, _l_ptradd(p, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pSBTreeNode, _l_ptradd(p, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
			return SBTreeUpperBoundHelper(DerefAnyPtr(_pSBTreeNode, p), data, datasize, sortFunc);
	}
	return result;
}

LSearchResultType __stdcall 
SBTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return SBTreeUpperBoundHelper(pNode, data, pHead->dataSize, sortFunc);
}

static LSearchResultType __stdcall 
SBTreeLowerBoundHelper(_pSBTreeNode pNode, ConstPointer data, dword datasize, TSearchAndSortFunc sortFunc)
{
	_pSBTreeNode pNode1;
	Array p;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	if ( pNode->isData )
	{
        sdword ix = _ls_bsearch( _l_ptradd(pNode, szSBTreeNode), datasize, data, pNode->cnt, sortFunc, UTLPTR_SEARCHMODE );
		if ( ix < 0 )
			return result;
		_Lnode(result) = pNode;
		_Loffset(result) = ix;
		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode)); ix < pNode->cnt; ++ix, p = CastAny(Array, _l_ptradd(p, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pSBTreeNode, _l_ptradd(p, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
			return SBTreeLowerBoundHelper(DerefAnyPtr(_pSBTreeNode, p), data, datasize, sortFunc);
	}
	return result;
}

LSearchResultType __stdcall 
SBTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return SBTreeLowerBoundHelper(pNode, data, pHead->dataSize, sortFunc);
}

static sword __stdcall 
SBTreeTestSortFunc( ConstPointer pa, ConstPointer pb )
{
	unsigned int* ppa = CastAnyPtr(unsigned int, CastMutable(Pointer, pa));
	unsigned int* ppb = CastAnyPtr(unsigned int, CastMutable(Pointer, pb));

	if ( *ppa < *ppb )
		return -1;
	if ( *ppa > *ppb )
		return 1;
	return 0;
}

static void __stdcall
SBTreeSortedTester1(_pSBTreeNode pNode, TSearchAndSortFunc sortFunc)
{
	_pSBTreeNode pNode1;
	Pointer d1;
	Pointer d2;
	Array p;
	Array p1;
	sdword ix;

	if ( pNode->isData )
	{
		if ( pNode->cnt > 0 )
		{
			d1 = _l_ptradd(pNode, szSBTreeNode);
			d2 = pNode->key;
			assert(0 == sortFunc(d1, d2));
		}
	}
	else
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
		for ( ix = 0, p1 = p; ix < Cast(sdword,pNode->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
			SBTreeSortedTester1(DerefAnyPtr(_pSBTreeNode, p1), sortFunc);
		pNode1 = DerefAnyPtr(_pSBTreeNode, p);
		d1 = pNode1->key;
		for ( ix = 1, p1 = CastAny(Array, _l_ptradd(p, szPointer)); ix < Cast(sdword,pNode->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
		{
			pNode1 = DerefAnyPtr(_pSBTreeNode, p1);
			d2 = pNode1->key;
			assert(0 > sortFunc(d1, d2));
			d1 = d2;
		}
	}
}

static void __stdcall
SBTreeSortedTester(Pointer liste, TSearchAndSortFunc sortFunc)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;
	LSearchResultType result = _LNULL;
	Pointer d1;
	Pointer d2;

	SBTreeSortedTester1(pNode, sortFunc);
	result = SBTreeBegin(liste);
	if ( !LPtrCheck(result) )
	{
		d1 = SBTreeGetData(result);
		result = SBTreeNext(result);
	}
	while ( !LPtrCheck(result) )
	{
		d2 = SBTreeGetData(result);
		assert( 0 > sortFunc(d1, d2) );
		result = SBTreeNext(result);
		d1 = d2;
	}
}

static _pSBTreeNode __stdcall
SBTreeInsertSortedHelper(_pSBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc, Ptr(LSearchResultType) pResult)
{
	_pSBTreeNode pNode1 = NULL;
	_pSBTreeNode pNode2;
	Array p;
	Array p1;
    TListCnt ix;
	dword cnt;

	if ( pNode->isData )
	{
		Pointer p =  _l_ptradd(pNode, szSBTreeNode);
		Pointer p1;
        sdword ix = _ls_binsert(p, pNode->head->dataSize, data, &(pNode->cnt), sortFunc);

		_LPnode(pResult) = pNode;
		_LPoffset(pResult) = ix;
		if ( ix == 0 )
		{
			pNode1 = pNode;
			pNode1->key = p;
			while ( NotPtrCheck(pNode1->parent) )
			{
				pNode2 = pNode1->parent;
				ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szSBTreeNode)), pNode1, pNode2->cnt);
				assert(ix >= 0);
				if ( ix != 0 )
					break;
				pNode2->key = pNode1->key;
				pNode1 = pNode1->parent;
			}
		}
		++(pNode->head->nodeCount);
		if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
		{
			pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pNode->head->maxEntriesPerNode * pNode->head->dataSize)));
			pNode2->head = pNode->head;
			p1 = _l_ptradd(pNode2, szSBTreeNode);
			cnt = pNode->cnt / 2;
			s_memcpy_s(p1, cnt * pNode->head->dataSize, _l_ptradd(p, (pNode->cnt - cnt) * pNode->head->dataSize), cnt * pNode->head->dataSize);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode2->key = p1;
			pNode2->isData = TRUE;
			if ( _LPoffset(pResult) >= Cast(TListIndex, pNode->cnt) )
			{
				_LPnode(pResult) = pNode2;
				_LPoffset(pResult) -= pNode->cnt;
			}
			return pNode2;
		}
		return NULL;
	}
	assert(pNode->cnt > 0);
	p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pSBTreeNode, _l_ptradd(p1, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
		{
			pNode1 = SBTreeInsertSortedHelper(DerefAnyPtr(_pSBTreeNode, p1), data, sortFunc, pResult);
			if ( PtrCheck(pNode1) )
				return NULL;
			pNode1->parent = pNode;
			break;
		}
	}
	_lv_insert(p, ix, pNode1, &(pNode->cnt));
	if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
	{
		pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
		pNode2->head = pNode->head;
		p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
		cnt = pNode->cnt / 2;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
		pNode2->key = pNode1->key;
        for ( ix = 0; ix < pNode2->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
		{
			pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
			pNode1->parent = pNode2;
		}
		return pNode2;
	}
	return NULL;
}

LSearchResultType __stdcall 
SBTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;
	_pSBTreeNode pNode2;
	Array p1;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = SBTreeInsertSortedHelper(pNode, data, sortFunc, &result);
	if ( PtrCheck(pNode) )
		return result;
	pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
	pNode2->head = pHead;
	pNode2->cnt = 2;
	p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
	DerefAnyPtr(_pSBTreeNode, p1) = pHead->root;
	pHead->root->parent = pNode2;
	pNode2->key = pHead->root->key;
	p1 = CastAny(Array, _l_ptradd(p1, szPointer));
	DerefAnyPtr(_pSBTreeNode, p1) = pNode;
	pNode->parent = pNode2;
	pHead->root = pNode2;
	SBTreeSortedTester(liste, sortFunc);
	return result;
}

static void __stdcall
SBTreeRemoveJoinHelper(_pSBTreeNode pNodeDest, _pSBTreeNode pNodeSrc)
{
	if ( pNodeDest->isData )
	{
		Pointer pDest = _l_ptradd(pNodeDest, szSBTreeNode + (pNodeDest->cnt * pNodeDest->head->dataSize) );
		Pointer pSrc = _l_ptradd(pNodeSrc, szSBTreeNode);

		s_memcpy_s(pDest, (pNodeDest->head->maxEntriesPerNode - pNodeDest->cnt) * pNodeDest->head->dataSize,
			pSrc, pNodeSrc->cnt * pNodeDest->head->dataSize);
		pNodeDest->cnt += pNodeSrc->cnt;
	}
	else
	{
		Array pDest = CastAny(Array, _l_ptradd(pNodeDest, szSBTreeNode + (pNodeDest->cnt * szPointer)));
		Array pSrc = CastAny(Array, _l_ptradd(pNodeSrc, szSBTreeNode));
		_pSBTreeNode pNode;
		sdword ix;

		s_memcpy_s(pDest, (pNodeDest->head->maxEntriesPerNode - pNodeDest->cnt) * szPointer,
			pSrc, pNodeSrc->cnt * szPointer);
		pNodeDest->cnt += pNodeSrc->cnt;
		for ( ix = 0; ix < Cast(sdword, pNodeSrc->cnt); ++ix, pDest = CastAny(Array, _l_ptradd(pDest, szPointer)) )
		{
			pNode = DerefAnyPtr(_pSBTreeNode, pDest);
			pNode->parent = pNodeDest;
		}
	}
	TFfree(pNodeSrc);
}

static void __stdcall
SBTreeRemoveKeyUpdateHelper(_pSBTreeNode pNode)
{
	_pSBTreeNode pNode1;
	_pSBTreeNode pNode2;
	sdword ix;

	pNode1 = pNode;
	if ( pNode1->isData )
		pNode1->key = _l_ptradd(pNode1, szSBTreeNode);
	else
	{
		pNode2 = DerefPtr(_pSBTreeNode, _l_ptradd(pNode1, szSBTreeNode));
		pNode1->key = pNode2->key;
	}
	while ( NotPtrCheck(pNode1->parent) )
	{
		pNode2 = pNode1->parent;
		ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szSBTreeNode)), pNode1, pNode2->cnt);
		assert(ix >= 0);
		if ( ix != 0 )
			break;
		pNode2->key = pNode1->key;
		pNode1 = pNode2;
	}
}

static _pSBTreeNode __stdcall
SBTreeRemoveSortedHelper(_pSBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context, Ptr(bool) pResult)
{
	_pSBTreeNode pNode1;
	_pSBTreeNode pNode2;
	Array p;
	Array p1;
	Pointer data1;
    TListCnt ix;
	bool b;

	assert(pNode->cnt > 0);
	if ( pNode->isData )
	{
		Pointer p = _l_ptradd(pNode, szSBTreeNode);
        sdword ix;

		DerefPtr(bool,pResult) = true;
		ix = _ls_bsearch( p, pNode->head->dataSize, data, pNode->cnt, sortFunc, UTLPTR_MATCHMODE );
		if ( ix < 0 )
		{
			DerefPtr(bool,pResult) = false;
			return NULL;
		}
		data1 = _l_ptradd(p, ix * pNode->head->dataSize);
		freeFunc(data1, context);
		_ls_delete( p, pNode->head->dataSize, ix, &(pNode->cnt) );
		if ( (ix == 0) && (pNode->cnt > 0) )
			SBTreeRemoveKeyUpdateHelper(pNode);
		--(pNode->head->nodeCount);
		if ( pNode->cnt < (pNode->head->maxEntriesPerNode / 2) )
			return pNode;
		return NULL;
	}
	p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pSBTreeNode, _l_ptradd(p1, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
		{
			pNode1 = SBTreeRemoveSortedHelper(DerefAnyPtr(_pSBTreeNode, p1), data, sortFunc, freeFunc, context, pResult);
			if ( PtrCheck(pNode1) )
				return NULL;
			assert(pNode1 == DerefAnyPtr(_pSBTreeNode, p1));
			if ( (pNode->cnt < 2) && (pNode1->cnt == 0) )
			{
				TFfree(pNode1);
				_lv_delete( p, ix, &(pNode->cnt) );
				return pNode;
			}
			pNode2 = DerefAnyPtr(_pSBTreeNode, _l_ptradd(p1, szPointer));
            if ( (ix < (pNode->cnt - 1)) && ((pNode2->cnt + pNode1->cnt) < pNode->head->maxEntriesPerNode) )
			{
                b = ((ix == 0) && (pNode1->cnt == 0));
				SBTreeRemoveJoinHelper(pNode1, pNode2);
				if ( b )
					SBTreeRemoveKeyUpdateHelper(pNode1);
				++ix;
				break;
			}
			pNode2 = DerefAnyPtr(_pSBTreeNode, _l_ptradd(p1, -Cast(sdword,szPointer)));
			if ( (ix > 0) && ((pNode2->cnt + pNode1->cnt) < pNode->head->maxEntriesPerNode) )
			{
				b = ((ix == 1) && (pNode2->cnt == 0));
				SBTreeRemoveJoinHelper(pNode2, pNode1);
				if ( b )
					SBTreeRemoveKeyUpdateHelper(pNode2);
				break;
			}
			return NULL;
		}
	}
    if ( ix < pNode->cnt )
		_lv_delete( p, ix, &(pNode->cnt) );
	if ( pNode->cnt < (pNode->head->maxEntriesPerNode / 2) )
		return pNode;
	return NULL;
}

bool __stdcall 
SBTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;
	bool result = false;
	
	assert(liste != NULL);
	assert(sortFunc != NULL);
	assert(freeFunc != NULL);
	assert(pNode != NULL);
	pNode = SBTreeRemoveSortedHelper(pNode, data, sortFunc, freeFunc, context, &result);
	if ( PtrCheck(pNode) )
		return result;
	assert(pNode == pHead->root);
	while ( (!(pNode->isData)) && (pNode->cnt == 1) )
	{
		pHead->root = DerefPtr(_pSBTreeNode, _l_ptradd(pNode, szSBTreeNode));
		pHead->root->parent = NULL;
		TFfree(pNode);
		pNode = pHead->root;
	}
	SBTreeSortedTester(liste, sortFunc);
	return result;
}

static _pSBTreeNode __stdcall
SBTreeAppendHelper(_pSBTreeNode pNode, ConstPointer data, Ptr(LSearchResultType) pResult)
{
	_pSBTreeNode pNode1 = NULL;
	_pSBTreeNode pNode2;
	Array p;
	Array p1;
    TListCnt ix;
	dword cnt;

	p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
	if ( pNode->isData )
	{
		_LPnode(pResult) = pNode;
		_LPoffset(pResult) = _ls_insert( p, pNode->head->dataSize, Cast(sdword,pNode->cnt) - 1, data, &(pNode->cnt) );

		if ( _LPoffset(pResult) == 0 )
		{
			pNode1 = pNode;
			pNode1->key = p;
			while ( NotPtrCheck(pNode1->parent) )
			{
				pNode2 = pNode1->parent;
				ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szSBTreeNode)), pNode1, pNode2->cnt);
				assert(ix >= 0);
				if ( ix != 0 )
					break;
				pNode2->key = pNode1->key;
				pNode1 = pNode1->parent;
			}
		}
		++(pNode->head->nodeCount);
		if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
		{
			pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pNode->head->maxEntriesPerNode * pNode->head->dataSize)));
			pNode2->head = pNode->head;
			p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
			cnt = 1;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * pNode->head->dataSize), cnt * pNode->head->dataSize);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode2->key = p1;
			pNode2->isData = TRUE;
			if ( _LPoffset(pResult) >= Cast(TListIndex, pNode->cnt) )
			{
				_LPnode(pResult) = pNode2;
				_LPoffset(pResult) -= pNode->cnt;
			}
			return pNode2;
		}
		return NULL;
	}
	assert(pNode->cnt > 0);
	ix = pNode->cnt - 1;
	p1 = CastAny(Array, _l_ptradd(p, ix * szPointer));
	pNode1 = SBTreeAppendHelper(DerefAnyPtr(_pSBTreeNode, p1), data, pResult);
	if ( PtrCheck(pNode1) )
		return NULL;
	pNode1->parent = pNode;
	_lv_insert(p, ix, pNode1, &(pNode->cnt));
	if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
	{
		pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
		pNode2->head = pNode->head;
		p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
		cnt = 1;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
		pNode2->key = pNode1->key;
        for ( ix = 0; ix < pNode2->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
		{
			pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
			pNode1->parent = pNode2;
		}
		return pNode2;
	}
	return NULL;
}

LSearchResultType __stdcall 
SBTreeAppend(Pointer liste, ConstPointer data)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;
	_pSBTreeNode pNode2;
	Array p1;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = SBTreeAppendHelper(pNode, data, &result);
	if ( PtrCheck(pNode) )
		return result;
	pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
	pNode2->head = pHead;
	pNode2->cnt = 2;
	p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
	DerefAnyPtr(_pSBTreeNode, p1) = pHead->root;
	pHead->root->parent = pNode2;
	pNode2->key = pHead->root->key;
	p1 = CastAny(Array, _l_ptradd(p1, szPointer));
	DerefAnyPtr(_pSBTreeNode, p1) = pNode;
	pNode->parent = pNode2;
	pHead->root = pNode2;
	return result;
}

static _pSBTreeNode __stdcall
SBTreePrependHelper(_pSBTreeNode pNode, ConstPointer data, Ptr(LSearchResultType) pResult)
{
	_pSBTreeNode pNode1 = NULL;
	_pSBTreeNode pNode2;
	Array p;
	Array p1;
    TListCnt ix;
	dword cnt;

	p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
	if ( pNode->isData )
	{
		_LPnode(pResult) = pNode;
		_LPoffset(pResult) = _ls_insert( p, pNode->head->dataSize, -1, data, &(pNode->cnt) );

		if ( _LPoffset(pResult) == 0 )
		{
			pNode1 = pNode;
			pNode1->key = p;
			while ( NotPtrCheck(pNode1->parent) )
			{
				pNode2 = pNode1->parent;
				ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szSBTreeNode)), pNode1, pNode2->cnt);
				assert(ix >= 0);
				if ( ix != 0 )
					break;
				pNode2->key = pNode1->key;
				pNode1 = pNode1->parent;
			}
		}
		++(pNode->head->nodeCount);
		if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
		{
			pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pNode->head->maxEntriesPerNode * pNode->head->dataSize)));
			pNode2->head = pNode->head;
			p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
			cnt = pNode->cnt - 1;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * pNode->head->dataSize), cnt * pNode->head->dataSize);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode2->key = p1;
			pNode2->isData = TRUE;
			return pNode2;
		}
		return NULL;
	}
	assert(pNode->cnt > 0);
	ix = 0;
	p1 = p;
	pNode1 = SBTreePrependHelper(DerefAnyPtr(_pSBTreeNode, p1), data, pResult);
	if ( PtrCheck(pNode1) )
		return NULL;
	pNode1->parent = pNode;
	_lv_insert(p, ix, pNode1, &(pNode->cnt));
	if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
	{
		pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
		pNode2->head = pNode->head;
		p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
		cnt = pNode->cnt - 1;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
		pNode2->key = pNode1->key;
        for ( ix = 0; ix < pNode2->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
		{
			pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
			pNode1->parent = pNode2;
		}
		return pNode2;
	}
	return NULL;
}

LSearchResultType __stdcall 
SBTreePrepend(Pointer liste, ConstPointer data)
{
	_pSBTreeHead pHead = CastAnyPtr(_SBTreeHead, liste);
	_pSBTreeNode pNode = pHead->root;
	_pSBTreeNode pNode2;
	Array p1;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = SBTreePrependHelper(pNode, data, &result);
	if ( PtrCheck(pNode) )
		return result;
	pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
	pNode2->head = pHead;
	pNode2->cnt = 2;
	p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
	DerefAnyPtr(_pSBTreeNode, p1) = pHead->root;
	pHead->root->parent = pNode2;
	pNode2->key = pHead->root->key;
	p1 = CastAny(Array, _l_ptradd(p1, szPointer));
	DerefAnyPtr(_pSBTreeNode, p1) = pNode;
	pNode->parent = pNode2;
	pHead->root = pNode2;
	return result;
}

LSearchResultType __stdcall 
SBTreeInsertBefore(LSearchResultType node, ConstPointer data)
{
	_pSBTreeNode pNode = CastAnyPtr(_SBTreeNode, _Lnode(node));
	_pSBTreeHead pHead = pNode->head;
	_pSBTreeNode pNode1;
	_pSBTreeNode pNode2;
	Array p;
	Array p1;
	sdword ix = _Loffset(node);
	TListCnt cnt;

	assert(pNode->isData);
	p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
	ix = _ls_insert( p, pHead->dataSize, ix - 1, data, &(pNode->cnt) );
	if ( ix == 0 )
	{
		pNode1 = pNode;
		pNode1->key = p;
		while ( NotPtrCheck(pNode1->parent) )
		{
			pNode2 = pNode1->parent;
			ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szSBTreeNode)), pNode1, pNode2->cnt);
			assert(ix >= 0);
			if ( ix != 0 )
				break;
			pNode2->key = pNode1->key;
			pNode1 = pNode1->parent;
		}
	}
	++(pHead->nodeCount);
	pNode2 = NULL;
	if ( pNode->cnt >= pHead->maxEntriesPerNode )
	{
		pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * pHead->dataSize)));
		pNode2->head = pHead;
		pNode2->parent = pNode->parent;
		p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
		cnt = pNode->cnt / 2;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * pHead->dataSize), cnt * pHead->dataSize);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode2->key = p1;
		pNode2->isData = TRUE;
		if ( _Loffset(node) >= Cast(TListIndex, pNode->cnt) )
		{
			_Lnode(node) = pNode2;
			_Loffset(node) -= pNode->cnt;
		}
	}
	while ( NotPtrCheck(pNode2) && NotPtrCheck(pNode2->parent) )
	{
		pNode1 = pNode2->parent;
		p = CastAny(Array, _l_ptradd(pNode1, szSBTreeNode));
		assert(pNode1->cnt > 0);
		ix = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(ix >= 0);
		_lv_insert(p, ix, pNode2, &(pNode1->cnt));
		pNode = pNode1;
		pNode2 = NULL;
		if ( pNode->cnt >= pHead->maxEntriesPerNode )
		{
			pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
			pNode2->head = pHead;
			pNode2->parent = pNode->parent;
			p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
			cnt = pNode->cnt / 2;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
			pNode2->key = pNode1->key;
			for ( ix = 0; ix < Cast(sdword, pNode2->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
			{
				pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
				pNode1->parent = pNode2;
			}
		}
	}
	if ( NotPtrCheck(pNode2) )
	{
		pNode1 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
		pNode1->head = pHead;
		pNode1->cnt = 2;
		p1 = CastAny(Array, _l_ptradd(pNode1, szSBTreeNode));
		DerefAnyPtr(_pSBTreeNode, p1) = pHead->root;
		pHead->root->parent = pNode1;
		pNode1->key = pHead->root->key;
		p1 = CastAny(Array, _l_ptradd(p1, szPointer));
		DerefAnyPtr(_pSBTreeNode, p1) = pNode2;
		pNode2->parent = pNode1;
		pHead->root = pNode1;
	}
	return node;
}

LSearchResultType __stdcall 
SBTreeInsertAfter(LSearchResultType node, ConstPointer data)
{
	_pSBTreeNode pNode = CastAnyPtr(_SBTreeNode, _Lnode(node));
	_pSBTreeHead pHead = pNode->head;
	_pSBTreeNode pNode1;
	_pSBTreeNode pNode2;
	Array p;
	Array p1;
	sdword ix = _Loffset(node);
	TListCnt cnt;

	assert(pNode->isData);
	p = CastAny(Array, _l_ptradd(pNode, szSBTreeNode));
	ix = _ls_insert( p, pHead->dataSize, ix, data, &(pNode->cnt) );
	if ( ix == 0 )
	{
		pNode1 = pNode;
		pNode1->key = p;
		while ( NotPtrCheck(pNode1->parent) )
		{
			pNode2 = pNode1->parent;
			ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szSBTreeNode)), pNode1, pNode2->cnt);
			assert(ix >= 0);
			if ( ix != 0 )
				break;
			pNode2->key = pNode1->key;
			pNode1 = pNode1->parent;
		}
	}
	++(_Loffset(node));
	++(pHead->nodeCount);
	pNode2 = NULL;
	if ( pNode->cnt >= pHead->maxEntriesPerNode )
	{
		pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * pHead->dataSize)));
		pNode2->head = pHead;
		pNode2->parent = pNode->parent;
		p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
		cnt = pNode->cnt / 2;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * pHead->dataSize), cnt * pHead->dataSize);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode2->key = p1;
		pNode2->isData = TRUE;
		if ( _Loffset(node) >= Cast(TListIndex, pNode->cnt) )
		{
			_Lnode(node) = pNode2;
			_Loffset(node) -= pNode->cnt;
		}
	}
	while ( NotPtrCheck(pNode2) && NotPtrCheck(pNode2->parent) )
	{
		pNode1 = pNode2->parent;
		p = CastAny(Array, _l_ptradd(pNode1, szSBTreeNode));
		assert(pNode1->cnt > 0);
		ix = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(ix >= 0);
		_lv_insert(p, ix, pNode2, &(pNode1->cnt));
		pNode = pNode1;
		pNode2 = NULL;
		if ( pNode->cnt >= pHead->maxEntriesPerNode )
		{
			pNode2 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
			pNode2->head = pHead;
			pNode2->parent = pNode->parent;
			p1 = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
			cnt = pNode->cnt / 2;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
			pNode2->key = pNode1->key;
			for ( ix = 0; ix < Cast(sdword, pNode2->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
			{
				pNode1 = DerefAnyPtr(_pSBTreeNode,p1);
				pNode1->parent = pNode2;
			}
		}
	}
	if ( NotPtrCheck(pNode2) )
	{
		pNode1 = CastAnyPtr(_SBTreeNode, TFalloc(szSBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
		pNode1->head = pHead;
		pNode1->cnt = 2;
		p1 = CastAny(Array, _l_ptradd(pNode1, szSBTreeNode));
		DerefAnyPtr(_pSBTreeNode, p1) = pHead->root;
		pHead->root->parent = pNode1;
		pNode1->key = pHead->root->key;
		p1 = CastAny(Array, _l_ptradd(p1, szPointer));
		DerefAnyPtr(_pSBTreeNode, p1) = pNode2;
		pNode2->parent = pNode1;
		pHead->root = pNode1;
	}
	return node;
}

void __stdcall 
SBTreeRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pSBTreeNode pNode = CastAnyPtr(_SBTreeNode, _Lnode(node));
	_pSBTreeHead pHead = pNode->head;
	_pSBTreeNode pNode1;
	_pSBTreeNode pNode2;
	_pSBTreeNode pNode3;
	_pSBTreeNode pNode4;
	Array p;
	Pointer pp;
	Pointer data1;
	sdword ix = _Loffset(node);
	bool b;

	assert(pNode->cnt > 0);
	assert(pNode->isData);
	pp = _l_ptradd(pNode, szSBTreeNode);
	data1 = _l_ptradd(pp, ix * pHead->dataSize);
	freeFunc(data1, context);
	_ls_delete( pp, pHead->dataSize, ix, &(pNode->cnt) );
	if ( (ix == 0) && (pNode->cnt > 0) )
		SBTreeRemoveKeyUpdateHelper(pNode);
	--(pHead->nodeCount);
	if ( pNode->cnt < (pHead->maxEntriesPerNode / 2) )
	{
		pNode1 = pNode;
		while ( NotPtrCheck(pNode1->parent) )
		{
			pNode2 = pNode1->parent;
			p = CastAny(Array, _l_ptradd(pNode2, szSBTreeNode));
			assert(pNode2->cnt > 0);
			ix = _lv_searchptr(p, pNode1, pNode2->cnt);
			assert(ix >= 0);
			pNode3 = DerefAnyPtr(_pSBTreeNode,_l_ptradd(p, ix * szPointer));
			if ( (pNode2->cnt < 2) && (pNode3->cnt == 0) )
			{
				TFfree(pNode3);
				_lv_delete(p, ix, &(pNode2->cnt) );
			}
			else
			{
				pNode4 = DerefAnyPtr(_pSBTreeNode,_l_ptradd(p, (ix + 1) * szPointer));
				if ( (ix < Cast(sdword, (pNode2->cnt - 1))) && ((pNode3->cnt + pNode4->cnt) < pHead->maxEntriesPerNode) )
				{
					b = ((ix == 0) && (pNode3->cnt == 0));
					SBTreeRemoveJoinHelper(pNode3, pNode4);
					_lv_delete(p, ix + 1, &(pNode2->cnt) );
					if ( b )
						SBTreeRemoveKeyUpdateHelper(pNode3);
				}
				else
				{
					pNode4 = DerefAnyPtr(_pSBTreeNode,_l_ptradd(p, (ix - 1) * szPointer));
					if ( (ix > 0) && ((pNode3->cnt + pNode4->cnt) < pHead->maxEntriesPerNode) )
					{
						b = ((ix == 1) && (pNode4->cnt == 0));
						SBTreeRemoveJoinHelper(pNode4, pNode3);
						_lv_delete(p, ix, &(pNode2->cnt) );
						if ( b )
							SBTreeRemoveKeyUpdateHelper(pNode4);
					}
				}
			}
			pNode1 = pNode2;
		}
		assert(pNode1 == pHead->root);
		while ( (!(pNode1->isData)) && (pNode1->cnt == 1) )
		{
			pHead->root = DerefPtr(_pSBTreeNode, _l_ptradd(pNode1, szSBTreeNode));
			pHead->root->parent = NULL;
			TFfree(pNode1);
			pNode1 = pHead->root;
		}
	}
}

Pointer __stdcall 
SBTreeGetData(LSearchResultType node)
{
	_pSBTreeNode pNode = CastAnyPtr(_SBTreeNode, _Lnode(node));
	_pSBTreeHead pHead;

	if ( PtrCheck(pNode) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, pNode->cnt)) || (!(pNode->isData)) )
		return NULL;
	pHead = pNode->head;
	return _l_ptradd(pNode, szSBTreeNode + (_Loffset(node) * pHead->dataSize));
}

void __stdcall 
SBTreeSetData(LSearchResultType node, ConstPointer data)
{
	_pSBTreeNode pNode = CastAnyPtr(_SBTreeNode, _Lnode(node));
	_pSBTreeHead pHead;

	if ( PtrCheck(pNode) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, pNode->cnt)) || (!(pNode->isData)) )
		return;
	pHead = pNode->head;
	s_memcpy_s(_l_ptradd(pNode, szSBTreeNode + (_Loffset(node) * pHead->dataSize)), pNode->head->dataSize, CastMutable(Pointer, data), pNode->head->dataSize);
}

#ifdef __DBGTEST_LISTE__
void FreeListData( ConstPointer data )
{
}

int 
_tmain( int argc, CPointer *argv )
{
	Pointer liste = NULL;
	LSearchResultType result = _LNULL;
	dword cnt;
	_CrtMemState memState;

	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );

	_CrtMemCheckpoint(&memState);

	/* SDoubleLinkedList */
	liste = SDoubleLinkedListOpen();

	result = SDoubleLinkedListAppend(liste, result, CastAny(Pointer,0x2));
	_ASSERTE(LCompareEqual(result, SDoubleLinkedListBegin(liste)));
	_ASSERTE(LCompareEqual(result, SDoubleLinkedListLast(liste)));

	result = SDoubleLinkedListAppend(liste, result, CastAny(Pointer,0x3));
	_ASSERTE(LCompareEqual(result, SDoubleLinkedListNext(SDoubleLinkedListBegin(liste))));
	_ASSERTE(LCompareEqual(result, SDoubleLinkedListLast(liste)));

	result = SDoubleLinkedListBegin(liste);
	result = SDoubleLinkedListInsert(liste, result, CastAny(Pointer,0x1));
	_ASSERTE(LCompareEqual(result, SDoubleLinkedListBegin(liste)));
	_ASSERTE(LCompareEqual(result, SDoubleLinkedListPrev(SDoubleLinkedListPrev(SDoubleLinkedListLast(liste)))));

	result = SDoubleLinkedListBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(SDoubleLinkedListGetData(result) == CastAny(Pointer,++cnt));
		result = SDoubleLinkedListNext(result);
	}

	result = SDoubleLinkedListBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(SDoubleLinkedListGetData(result) == CastAny(Pointer,++cnt));
		SDoubleLinkedListRemove(result, FreeListData);
		result = SDoubleLinkedListBegin(liste);
	}

	SDoubleLinkedListClose(liste, FreeListData);

	/* Array */
	liste = SArrayOpen(10);

	result = SArrayAppend(liste, result, CastAny(Pointer,0x2));
	_ASSERTE(LCompareEqual(result, SArrayBegin(liste)));
	_ASSERTE(LCompareEqual(result, SArrayLast(liste)));

	result = SArrayAppend(liste, result, CastAny(Pointer,0x3));
	_ASSERTE(LCompareEqual(result, SArrayNext(SArrayBegin(liste))));
	_ASSERTE(LCompareEqual(result, SArrayLast(liste)));

	result = SArrayBegin(liste);
	result = SArrayInsert(liste, result, CastAny(Pointer,0x1));
	_ASSERTE(LCompareEqual(result, SArrayBegin(liste)));
	_ASSERTE(LCompareEqual(result, SArrayPrev(SArrayPrev(SArrayLast(liste)))));

	result = SArrayBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(SArrayGetData(result) == CastAny(Pointer,++cnt));
		result = SArrayNext(result);
	}

	result = SArrayBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(SArrayGetData(result) == CastAny(Pointer,++cnt));
		SArrayRemove(result, FreeListData);
		result = SArrayBegin(liste);
	}

	SArrayClose(liste, FreeListData);

	/* SVector */
	liste = SVectorOpen(10, 10);

	result = SVectorAppend(liste, result, CastAny(Pointer,0x2));
	_ASSERTE(LCompareEqual(result, SVectorBegin(liste)));
	_ASSERTE(LCompareEqual(result, SVectorLast(liste)));

	result = SVectorAppend(liste, result, CastAny(Pointer,0x3));
	_ASSERTE(LCompareEqual(result, SVectorNext(SVectorBegin(liste))));
	_ASSERTE(LCompareEqual(result, SVectorLast(liste)));

	result = SVectorBegin(liste);
	result = SVectorInsert(liste, result, CastAny(Pointer,0x1));
	_ASSERTE(LCompareEqual(result, SVectorBegin(liste)));
	_ASSERTE(LCompareEqual(result, SVectorPrev(SVectorPrev(SVectorLast(liste)))));

	result = SVectorBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(SVectorGetData(result) == CastAny(Pointer,++cnt));
		result = SVectorNext(result);
	}

	result = SVectorBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(SVectorGetData(result) == CastAny(Pointer,++cnt));
		SVectorRemove(result, FreeListData);
		result = SVectorBegin(liste);
	}

	SVectorClose(liste, FreeListData);

	/* SAVLBinaryTree */
	liste = SAVLBinaryTreeOpen();

	result = SAVLBinaryTreeAppend(liste, result, CastAny(Pointer,0x2));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreeBegin(liste)));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreeLast(liste)));

	result = SAVLBinaryTreeAppend(liste, result, CastAny(Pointer,0x3));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreeNext(SAVLBinaryTreeBegin(liste))));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreeLast(liste)));

	result = SAVLBinaryTreeAppend(liste, result, CastAny(Pointer,0x4));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreeNext(SAVLBinaryTreeNext(SAVLBinaryTreeBegin(liste)))));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreeLast(liste)));

	result = SAVLBinaryTreeBegin(liste);
	result = SAVLBinaryTreeInsert(liste, result, CastAny(Pointer,0x1));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreeBegin(liste)));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreePrev(SAVLBinaryTreePrev(SAVLBinaryTreePrev(SAVLBinaryTreeLast(liste))))));

	result = SAVLBinaryTreeInsert(liste, result, CastAny(Pointer,0x0));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreeBegin(liste)));
	_ASSERTE(LCompareEqual(result, SAVLBinaryTreePrev(SAVLBinaryTreePrev(SAVLBinaryTreePrev(SAVLBinaryTreePrev(SAVLBinaryTreeLast(liste)))))));

	result = SAVLBinaryTreeBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERTE(SAVLBinaryTreeGetData(result) == CastAny(Pointer,cnt++));
		result = SAVLBinaryTreeNext(result);
	}

	result = SAVLBinaryTreeBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERTE(SAVLBinaryTreeGetData(result) == CastAny(Pointer,cnt++));
		SAVLBinaryTreeRemove(result, FreeListData);
		result = SAVLBinaryTreeBegin(liste);
	}

	SAVLBinaryTreeClose(liste, FreeListData);

	_CrtMemDumpAllObjectsSince(&memState);

	_ASSERTE( _CrtCheckMemory( ) );
}

#endif
