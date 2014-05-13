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
#include "LISTE.H"
#include "STRUTIL.H"

const LSearchResultType _lnull = {NULL, 0};

/*					*/
/* DoubleLinkedList */
/*					*/

struct _tagDoubleLinkedListHead;
typedef struct _tagDoubleLinkedListNode
{
	Ptr(struct _tagDoubleLinkedListHead) head;
	Ptr(struct _tagDoubleLinkedListNode) prev;
	Ptr(struct _tagDoubleLinkedListNode) next;
	Pointer data;
} _DoubleLinkedListNode, *_pDoubleLinkedListNode;
#define szDoubleLinkedListNode sizeof(_DoubleLinkedListNode)

typedef struct _tagDoubleLinkedListHead
{
	Ptr(struct _tagDoubleLinkedListNode) first;
	Ptr(struct _tagDoubleLinkedListNode) last;
	TListCnt nodeCount;
} _DoubleLinkedListHead, *_pDoubleLinkedListHead;
#define szDoubleLinkedListHead sizeof(_DoubleLinkedListHead)

Pointer __stdcall 
DoubleLinkedListOpen(DECL_FILE_LINE0)
{
#ifdef __DEBUG__
	Pointer p = TFallocDbg(szDoubleLinkedListHead, file, line);
#else
	Pointer p = TFalloc(szDoubleLinkedListHead);
#endif

	if ( PtrCheck(p) )
		return NULL;
	return p;
}

TListCnt __stdcall 
DoubleLinkedListCount(Pointer liste)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);

	assert(liste != NULL);
	return head->nodeCount;
}

void __stdcall 
DoubleLinkedListClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->first;
	_pDoubleLinkedListNode node1;

	assert(liste != NULL);
	while ( NotPtrCheck(node) )
	{
		node1 = node->next;
		if ( PtrCheck(freeFunc) )
			TFfree(node->data);
		else
			freeFunc(node->data, context);
		TFfree(node);
		node = node1;
	}
	TFfree(head);
}

LSearchResultType __stdcall 
DoubleLinkedListIndex(Pointer liste, TListIndex index)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	if ( (index < 0) || (index >= Cast(TListIndex, head->nodeCount)) )
		return result;
	while ( NotPtrCheck(node) && (index > 0) )
	{
		node = node->next;
		--index;
	}
	_Lnode(result) = node;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListBegin(Pointer liste)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	_Lnode(result) = head->first;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListNext(LSearchResultType node)
{
	_pDoubleLinkedListNode node1 = CastAny(_pDoubleLinkedListNode,_Lnode(node));
	LSearchResultType result = _LNULL;

	if ( PtrCheck(node1) )
		return result;
	_Lnode(result) = node1->next;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListPrev(LSearchResultType node)
{
	_pDoubleLinkedListNode node1 = CastAny(_pDoubleLinkedListNode,_Lnode(node));
	LSearchResultType result = _LNULL;

	if ( PtrCheck(node1) )
		return result;
	_Lnode(result) = node1->prev;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListLast(Pointer liste)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	_Lnode(result) = head->last;
	return result;
}

bool __stdcall 
DoubleLinkedListForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->first;
	bool result = true;

	assert(liste != NULL);
	assert(func != NULL);
	while ( NotPtrCheck(node) )
	{
		if (0 == func(node->data, context))
		{
			result = false;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		if ( 0 == findFunc(node->data, data) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		if ( 0 <= sortFunc(node->data, data) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		if ( 0 < sortFunc(node->data, data) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->first;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	while ( NotPtrCheck(node) )
	{
		if ( 0 <= sortFunc(node->data, data) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

void __stdcall 
DoubleLinkedListSort(Pointer liste, TSearchAndSortFunc sortFunc)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode nodeIn = head->first;
	_pDoubleLinkedListNode nodeIn1 = NULL;
	_DoubleLinkedListNode nodeOutHead;
	_pDoubleLinkedListNode nodeOut = NULL;
	_pDoubleLinkedListNode nodeOut1 = NULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	s_memset(&nodeOutHead, 0, szDoubleLinkedListNode);
	nodeOut = &nodeOutHead;
	for ( ; NotPtrCheck(nodeIn); nodeIn = nodeIn1 )
	{
		nodeIn1 = nodeIn->next;
		for ( nodeOut1 = nodeOut; NotPtrCheck(nodeOut1->next); nodeOut1 = nodeOut1->next )
			if ( 0 < sortFunc(nodeOut1->next->data, nodeIn->data) )
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
DoubleLinkedListAppend(Pointer liste, ConstPointer data)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	node2 = CastAny(_pDoubleLinkedListNode,TFalloc(szDoubleLinkedListNode));
	if ( PtrCheck(node2) )
		return result;

	node2->data = CastMutable(Pointer, data);
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
DoubleLinkedListPrepend(Pointer liste, ConstPointer data)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	node2 = CastAny(_pDoubleLinkedListNode,TFalloc(szDoubleLinkedListNode));
	if ( PtrCheck(node2) )
		return result;

	node2->data = CastMutable(Pointer, data);
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
DoubleLinkedListInsertBefore(LSearchResultType node, ConstPointer data)
{
	_pDoubleLinkedListNode node1 = CastAny(_pDoubleLinkedListNode,_Lnode(node));
	_pDoubleLinkedListHead head = node1->head;
	_pDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(node1 != NULL);
	node2 = CastAny(_pDoubleLinkedListNode,TFalloc(szDoubleLinkedListNode));
	if ( PtrCheck(node2) )
		return result;

	node2->data = CastMutable(Pointer, data);
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
DoubleLinkedListInsertAfter(LSearchResultType node, ConstPointer data)
{
	_pDoubleLinkedListNode node1 = CastAny(_pDoubleLinkedListNode,_Lnode(node));
	_pDoubleLinkedListHead head = node1->head;
	_pDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(node1 != NULL);
	node2 = CastAny(_pDoubleLinkedListNode,TFalloc(szDoubleLinkedListNode));
	if ( PtrCheck(node2) )
		return result;

	node2->data = CastMutable(Pointer, data);
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
DoubleLinkedListRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pDoubleLinkedListNode node1 = CastAny(_pDoubleLinkedListNode,_Lnode(node));
	_pDoubleLinkedListHead head = node1->head;

	assert( NotPtrCheck(node1) );
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
	if ( PtrCheck(freeFunc) )
		TFfree(node1->data);
	else
		freeFunc(node1->data, context);
	TFfree(node1);
	(head->nodeCount)--;
}

LSearchResultType __stdcall 
DoubleLinkedListInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	LSearchResultType result = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	result = DoubleLinkedListFindSorted(liste, data, sortFunc);
	if ( LPtrCheck(result) )
		return DoubleLinkedListAppend(liste, data);
	cmp = sortFunc(DoubleLinkedListGetData(result), data);
	while ( cmp == 0 )
	{
		result = DoubleLinkedListNext(result);
		if ( LPtrCheck(result) )
			break;
		cmp = sortFunc(DoubleLinkedListGetData(result), data);
	}
	if ( LPtrCheck(result) )
		return DoubleLinkedListAppend(liste, data);
	return DoubleLinkedListInsertBefore(result, data);
}

bool __stdcall 
DoubleLinkedListRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	LSearchResultType result = _LNULL;
	LSearchResultType result1 = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	result = DoubleLinkedListFindSorted(liste, data, sortFunc);
	if ( LPtrCheck(result) )
		return false;
	cmp = sortFunc(DoubleLinkedListGetData(result), data);
	while ( cmp == 0 )
	{
		result1 = DoubleLinkedListNext(result);
		DoubleLinkedListRemove(result, freeFunc, context);
		if ( LPtrCheck(result1) )
			break;
		result = result1;
		cmp = sortFunc(DoubleLinkedListGetData(result), data);
	}
	return true;
}

Pointer __stdcall 
DoubleLinkedListGetData(LSearchResultType node)
{
	_pDoubleLinkedListNode node1 = CastAny(_pDoubleLinkedListNode,_Lnode(node));

	if ( PtrCheck(node1) )
		return NULL;
	return node1->data;
}

void __stdcall 
DoubleLinkedListSetData(LSearchResultType node, ConstPointer data)
{
	_pDoubleLinkedListNode node1 = CastAny(_pDoubleLinkedListNode,_Lnode(node));

	assert( NotPtrCheck(node1) );
	node1->data = CastMutable(Pointer, data);
}

/*		 */
/* Array */
/*		 */
typedef struct _tagArrayHead
{
	TListCnt max;
	TListCnt cnt;
} _ArrayHead, *_pArrayHead;
#define szArrayHead sizeof(_ArrayHead)

Pointer __stdcall 
ArrayOpen(DECL_FILE_LINE TListCnt max)
{
#ifdef __DEBUG__
	Pointer p = TFallocDbg(szArrayHead + (max * szPointer), file, line);
#else
	Pointer p = TFalloc(szArrayHead + (max * szPointer));
#endif
	_pArrayHead head;

	if ( PtrCheck(p) )
		return NULL;
	head = CastAny(_pArrayHead,p);
	assert(max > 0);
	head->max = max;
	return p;
}

TListCnt __stdcall 
ArrayCount(Pointer liste)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);

	assert(liste != NULL);
	return head->cnt;
}

void __stdcall 
ArrayClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array data = CastAny(Array,_l_ptradd(head,szArrayHead));
	Pointer d;
	TListCnt ix;

	assert(liste != NULL);
	for ( ix = 0; ix < head->cnt; ++ix, data = CastAny(Array,_l_ptradd(data,szPointer)) )
	{
		d = DerefPtr(Pointer,data);
		if ( PtrCheck(freeFunc) )
			TFfree(d);
		else
			freeFunc(d, context);
	}
	TFfree(head);
}

LSearchResultType __stdcall 
ArrayIndex(Pointer liste, TListIndex index)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (index < 0) || (index >= Cast(TListIndex,head->cnt)) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = index;
	return result;
}

LSearchResultType __stdcall 
ArrayBegin(Pointer liste)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
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
ArrayNext(LSearchResultType node)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
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
ArrayPrev(LSearchResultType node)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
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
ArrayLast(Pointer liste)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
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
ArrayForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead) );
	Array pt = dataArray;
	bool result = true;
	TListCnt ix;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(func != NULL);
	for (ix = 0; ix < head->cnt; ++ix, pt = CastAny(Array, _l_ptradd(pt, szPointer)))
	{
		if (0 == func(*pt, context))
		{
			result = false;
			break;
		}
	}
	return result;
}

LSearchResultType __stdcall 
ArrayFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(findFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_lsearch( dataArray, data, head->cnt, findFunc, UTLPTR_MATCHMODE );
	if ( _Loffset(result) < 0 )
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
ArrayFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataArray, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return result;
}

LSearchResultType __stdcall 
ArrayUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataArray, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return result;
}

LSearchResultType __stdcall 
ArrayLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataArray, data, head->cnt, sortFunc, UTLPTR_SEARCHMODE);
	return result;
}

void __stdcall 
ArraySort(Pointer liste, TSearchAndSortFunc sortFunc, word mode)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead) );

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return;
	switch ( mode )
	{
	case LSORTMODE_HEAPSORT:
		_lv_heapsort(dataArray, head->cnt, sortFunc);
		break;
	case LSORTMODE_QUICKSORT:
		_lv_quicksort(dataArray, head->cnt, sortFunc);
		break;
	}
}

LSearchResultType __stdcall 
ArrayAppend(Pointer liste, ConstPointer data)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(head->cnt < head->max);
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataArray, Cast(sdword,head->cnt) - 1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
ArrayPrepend(Pointer liste, ConstPointer data)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(head->cnt < head->max);
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataArray, -1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
ArrayInsertBefore(LSearchResultType node, ConstPointer data)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	assert(head->cnt < head->max);
	if ( (_Loffset(node) < 0) || ((head->cnt > 0) && (_Loffset(node) >= Cast(TListIndex, head->cnt))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataArray, _Loffset(node) - 1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
ArrayInsertAfter(LSearchResultType node, ConstPointer data)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	assert(head->cnt < head->max);
	if ( (_Loffset(node) < -1) || ((head->cnt > 0) && (_Loffset(node) >= Cast(TListIndex, (head->cnt - 1)))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataArray, _Loffset(node), data, &(head->cnt) );
	return result;
}

void __stdcall 
ArrayRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );
	Pointer data;

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return;
	data = DerefPtr(Pointer,_l_ptradd(dataArray, _Loffset(node) * szPointer));
	if ( PtrCheck(freeFunc) )
		TFfree( data );
	else
		freeFunc( data, context );
	_lv_delete( dataArray, _Loffset(node), &(head->cnt) );
}

LSearchResultType __stdcall 
ArrayInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return ArrayAppend(liste, data);
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataArray, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return ArrayInsertAfter(result, data);
}

bool __stdcall 
ArrayRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );
	LSearchResultType result = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return false;
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataArray, data, head->cnt, sortFunc, UTLPTR_SEARCHMODE);
	if ( _Loffset(result) < 0 )
		return false;
	cmp = sortFunc(ArrayGetData(result), data);
	while ( cmp == 0 )
	{
		ArrayRemove(result, freeFunc, context);
		if ( _Loffset(result) >= Cast(TListIndex, head->cnt) )
			break;
		cmp = sortFunc(ArrayGetData(result), data);
	}
	return true;
}

Pointer __stdcall 
ArrayGetData(LSearchResultType node)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || LPtrCheck(node) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return NULL;
	return DerefPtr(Pointer,_l_ptradd(dataArray, _Loffset(node) * szPointer));
}

void __stdcall 
ArraySetData(LSearchResultType node, ConstPointer data)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return;
	DerefPtr(Pointer,_l_ptradd(dataArray, _Loffset(node) * szPointer)) = CastMutable(Pointer, data);
}

/*		 */
/* Vector */
/*		 */
typedef struct _tagVectorHead
{
	TListCnt max;
	TListCnt exp;
	TListCnt cnt;
	Array data;
} _VectorHead, *_pVectorHead;
#define szVectorHead sizeof(_VectorHead)

Pointer __stdcall 
VectorOpen(DECL_FILE_LINE TListCnt max, TListCnt exp)
{
#ifdef __DEBUG__
	Pointer p1 = TFallocDbg(szVectorHead, file, line);
	Pointer p2 = TFallocDbg(max * szPointer, file, line);
#else
	Pointer p1 = TFalloc(szVectorHead);
	Pointer p2 = TFalloc(max * szPointer);
#endif
	_pVectorHead head;

	if ( PtrCheck(p1) || PtrCheck(p2) )
	{
		if ( NotPtrCheck(p1) )
			TFfree(p1);
		if ( NotPtrCheck(p2) )
			TFfree(p2);
		return NULL;
	}

	head = CastAny(_pVectorHead,p1);

	assert(max > 0);
	head->max = max;
	assert(exp > 0);
	head->exp = exp;
	head->data = CastAny(Array, p2);
	return p1;
}

TListCnt __stdcall 
VectorCount(Pointer liste)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);

	assert(liste != NULL);
	return head->cnt;
}

void __stdcall 
VectorReserve(Pointer liste)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Pointer p;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(head->exp > 0);
	p = TFrealloc(head->data, ((head->max + head->exp) * szPointer));
	if ( PtrCheck(p) )
		return;
	head->max += head->exp;
	head->data = CastAny(Array, p);
}

void __stdcall 
VectorClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Pointer d;

	assert(liste != NULL);
	while (head->cnt > 0)
	{
		--(head->cnt);
		d = DerefPtr(Pointer, _l_ptradd(head->data, (szPointer * head->cnt)));
		if ( PtrCheck(freeFunc) )
			TFfree(d);
		else
			freeFunc(d, context);
	}
	TFfree(head->data);
	TFfree(head);
}

LSearchResultType __stdcall 
VectorIndex(Pointer liste, TListIndex index)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (index < 0) || (index >= Cast(TListIndex,head->cnt)) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = index;
	return result;
}

LSearchResultType __stdcall 
VectorBegin(Pointer liste)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
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
VectorNext(LSearchResultType node)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	LSearchResultType result = _LNULL;

	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) < -1) || (_Loffset(node) >= Cast(TListIndex, (head->cnt - 1))) )
		return result;
	assert(head->max > 0);
	_Lnode(result) = head;
	_Loffset(result) = head->cnt - 1;
	if ( _Loffset(node) < Cast(TListIndex,(head->cnt - 1)) )
		_Loffset(result) = _Loffset(node) + 1;
	return result;
}

LSearchResultType __stdcall 
VectorPrev(LSearchResultType node)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	LSearchResultType result = _LNULL;

	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) <= 0) || (_Loffset(node) > Cast(TListIndex, (head->cnt - 1))) )
		return result;
	assert(head->max > 0);
	_Lnode(result) = head;
	_Loffset(result) = 0;
	if ( _Loffset(node) > 0 )
		_Loffset(result) = _Loffset(node) - 1;
	return result;
}

LSearchResultType __stdcall 
VectorLast(Pointer liste)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
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
VectorForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead, liste);
	Array dataVector = head->data;
	Array pt = dataVector;
	bool result = true;
	TListCnt ix;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(func != NULL);
	for (ix = 0; ix < head->cnt; ++ix, pt = CastAny(Array, _l_ptradd(pt, szPointer)))
	{
		if (0 == func(*pt, context))
		{
			result = false;
			break;
		}
	}
	return result;
}

LSearchResultType __stdcall
VectorFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(findFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_lsearch( dataVector, data, head->cnt, findFunc, UTLPTR_MATCHMODE );
	if ( _Loffset(result) < 0 )
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
VectorFindUser(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, ConstPointer context)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(findFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_ulsearch( dataVector, data, head->cnt, findFunc, context, UTLPTR_MATCHMODE );
	if ( _Loffset(result) < 0 )
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
VectorFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataVector, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return result;
}

LSearchResultType __stdcall 
VectorUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataVector, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return result;
}

LSearchResultType __stdcall 
VectorLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataVector, data, head->cnt, sortFunc, UTLPTR_SEARCHMODE);
	return result;
}

void __stdcall 
VectorSort(Pointer liste, TSearchAndSortFunc sortFunc, word mode)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return;
	switch ( mode )
	{
	case LSORTMODE_HEAPSORT:
		_lv_heapsort(dataVector, head->cnt, sortFunc);
		break;
	case LSORTMODE_QUICKSORT:
		_lv_quicksort(dataVector, head->cnt, sortFunc);
		break;
	}
}

void __stdcall 
VectorSortUser(Pointer liste, TSearchAndSortUserFunc sortFunc, ConstPointer context, word mode)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return;
	switch ( mode )
	{
	case LSORTMODE_HEAPSORT:
		_lv_uheapsort(dataVector, head->cnt, sortFunc, context);
		break;
	case LSORTMODE_QUICKSORT:
		_lv_uquicksort(dataVector, head->cnt, sortFunc, context);
		break;
	}
}

LSearchResultType __stdcall 
VectorAppend(Pointer liste, ConstPointer data)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( head->cnt >= head->max )
	{
		VectorReserve(liste);
		if ( head->cnt >= head->max )
			return result;
		dataVector = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataVector, Cast(sdword,head->cnt) - 1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
VectorPrepend(Pointer liste, ConstPointer data)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( head->cnt >= head->max )
	{
		VectorReserve(liste);
		if ( head->cnt >= head->max )
			return result;
		dataVector = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataVector, -1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
VectorInsertBefore(LSearchResultType node, ConstPointer data)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	if ( _Loffset(node) < 0 )
		return result;
	if ( head->cnt >= head->max )
	{
		VectorReserve(head);
		if ( head->cnt >= head->max )
			return result;
		dataVector = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataVector, _Loffset(node) - 1, data, &(head->cnt) );
	return result;
}

LSearchResultType __stdcall 
VectorInsertAfter(LSearchResultType node, ConstPointer data)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	if ( _Loffset(node) < -1 )
		return result;
	if ( head->cnt >= head->max )
	{
		VectorReserve(head);
		if ( head->cnt >= head->max )
			return result;
		dataVector = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataVector, _Loffset(node), data, &(head->cnt) );
	return result;
}

void __stdcall 
VectorRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	Array dataVector = head->data;
	Pointer data;

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return;
	data = DerefPtr(Pointer,_l_ptradd(dataVector, _Loffset(node) * szPointer));
	_lv_delete( dataVector, _Loffset(node), &(head->cnt) );
	if ( PtrCheck(freeFunc) )
		TFfree( data );
	else
		freeFunc( data, context );
}

LSearchResultType __stdcall 
VectorInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return VectorAppend(liste, data);
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataVector, data, head->cnt, sortFunc, UTLPTR_INSERTMODE);
	return VectorInsertAfter(result, data);
}

bool __stdcall 
VectorRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	_Lnode(result) = head;
	_Loffset(result) = _lv_bsearch( dataVector, data, head->cnt, sortFunc, UTLPTR_SEARCHMODE);
	if ( _Loffset(result) < 0 )
		return false;
	cmp = sortFunc(VectorGetData(result), data);
	while ( cmp == 0 )
	{
		VectorRemove(result, freeFunc, context);
		if ( _Loffset(result) >= Cast(TListIndex, head->cnt) )
			break;
		cmp = sortFunc(VectorGetData(result), data);
	}
	return true;
}

Pointer __stdcall 
VectorGetData(LSearchResultType node)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	Array dataVector = head->data;

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || LPtrCheck(node) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return NULL;
	return DerefPtr(Pointer,_l_ptradd(dataVector, _Loffset(node) * szPointer));
}

void __stdcall 
VectorSetData(LSearchResultType node, ConstPointer data)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	Array dataVector = head->data;

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return;
	DerefPtr(Pointer,_l_ptradd(dataVector, _Loffset(node) * szPointer)) = CastMutable(Pointer, data);
}

/*			  */
/* AVLBinaryTree */
/*			  */
struct _tagAVLBinaryTreeHead;
typedef struct _tagAVLBinaryTreeNode
{
	Ptr(struct _tagAVLBinaryTreeHead) head;
	Ptr(struct _tagAVLBinaryTreeNode) parent;
	Ptr(struct _tagAVLBinaryTreeNode) left;
	Ptr(struct _tagAVLBinaryTreeNode) right;
	TListCnt height;
	Pointer data;
} _AVLBinaryTreeNode, *_pAVLBinaryTreeNode;
#define szAVLBinaryTreeNode sizeof(_AVLBinaryTreeNode)

typedef struct _tagAVLBinaryTreeHead
{
	Ptr(struct _tagAVLBinaryTreeNode) root;
	TListCnt nodeCount;
} _AVLBinaryTreeHead, *_pAVLBinaryTreeHead;
#define szAVLBinaryTreeHead sizeof(_AVLBinaryTreeHead)

Pointer __stdcall
AVLBinaryTreeOpen(DECL_FILE_LINE0)
{
#ifdef __DEBUG__
	_pAVLBinaryTreeHead p = CastAnyPtr(_AVLBinaryTreeHead, TFallocDbg(szAVLBinaryTreeHead, file, line));
#else
	_pAVLBinaryTreeHead p = CastAnyPtr(_AVLBinaryTreeHead, TFalloc(szAVLBinaryTreeHead));
#endif
	return p;
}

TListCnt __stdcall 
AVLBinaryTreeCount(Pointer liste)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);

	assert(liste != NULL);
	return head->nodeCount;
}

static TListCnt __stdcall
AVLBinaryTreeHeightHelper(_pAVLBinaryTreeNode node)
{
	if ( PtrCheck(node) )
		return 0;
	return node->height;
}

TListCnt __stdcall 
AVLBinaryTreeHeight(Pointer liste)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);

	assert(liste != NULL);
	return AVLBinaryTreeHeightHelper(head->root);
}

static void __stdcall 
AVLBinaryTreeCloseHelper(_pAVLBinaryTreeNode node)
{
	_pAVLBinaryTreeNode nodeLeft = node->left;
	_pAVLBinaryTreeNode nodeRight = node->right;

	if ( NotPtrCheck(nodeLeft) )
	{
		AVLBinaryTreeCloseHelper(nodeLeft);
		TFfree(nodeLeft);
	}
	if ( NotPtrCheck(nodeRight) )
	{
		AVLBinaryTreeCloseHelper(nodeRight);
		TFfree(nodeRight);
	}
}

void __stdcall 
AVLBinaryTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = AVLBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	while ( !LPtrCheck(result) )
	{
		d = AVLBinaryTreeGetData(result);
		if ( PtrCheck(freeFunc) )
			TFfree(d);
		else
			freeFunc(d, context);
		result = AVLBinaryTreeNext(result);
	}
	if ( PtrCheck(liste) )
		return;
	if ( NotPtrCheck(head->root) )
	{
		AVLBinaryTreeCloseHelper(head->root);
		TFfree(head->root);
	}
	TFfree(head);
}

LSearchResultType __stdcall 
AVLBinaryTreeBegin(Pointer liste)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;
	_pAVLBinaryTreeNode node;

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
AVLBinaryTreeNext(LSearchResultType node)
{
	_pAVLBinaryTreeNode node1 = CastAnyPtr(_AVLBinaryTreeNode, _Lnode(node));
	_pAVLBinaryTreeNode node2 = NULL;
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
AVLBinaryTreePrev(LSearchResultType node)
{
	_pAVLBinaryTreeNode node1 = CastAnyPtr(_AVLBinaryTreeNode, _Lnode(node));
	_pAVLBinaryTreeNode node2 = NULL;
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
AVLBinaryTreeLast(Pointer liste)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;
	_pAVLBinaryTreeNode node;

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
AVLBinaryTreeForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	LSearchResultType it = AVLBinaryTreeBegin(liste);
	bool result = true;
	Pointer d;

	assert(liste != NULL);
	assert(func != NULL);
	while (!LPtrCheck(it))
	{
		d = AVLBinaryTreeGetData(it);
		if (0 == func(d, context))
		{
			result = false;
			break;
		}
		it = AVLBinaryTreeNext(it);
	}
	return result;
}

LSearchResultType __stdcall 
AVLBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	LSearchResultType result = AVLBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while ( !LPtrCheck(result) )
	{
		d = AVLBinaryTreeGetData(result);
		if ( 0 == findFunc(d, data) )
			return result;
		result = AVLBinaryTreeNext(result);
	}
	return _LNULL;
}

static _pAVLBinaryTreeNode __stdcall
AVLBinaryTreeFindSortedHelper(_pAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(node->data, data);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return AVLBinaryTreeFindSortedHelper(node->left, data, sortFunc);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return AVLBinaryTreeFindSortedHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall 
AVLBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = AVLBinaryTreeFindSortedHelper(head->root, data, sortFunc);
	return result;
}

static _pAVLBinaryTreeNode __stdcall
AVLBinaryTreeUpperBoundHelper(_pAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(node->data, data);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return AVLBinaryTreeUpperBoundHelper(node->left, data, sortFunc);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return AVLBinaryTreeUpperBoundHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall 
AVLBinaryTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = AVLBinaryTreeUpperBoundHelper(head->root, data, sortFunc);
	return result;
}

static _pAVLBinaryTreeNode __stdcall
AVLBinaryTreeLowerBoundHelper(_pAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(node->data, data);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return AVLBinaryTreeLowerBoundHelper(node->left, data, sortFunc);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return AVLBinaryTreeLowerBoundHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall 
AVLBinaryTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = AVLBinaryTreeLowerBoundHelper(head->root, data, sortFunc);
	return result;
}

static _pAVLBinaryTreeNode __stdcall 
AVLBinaryTreeRotateLeft(_pAVLBinaryTreeNode node)
{
	_pAVLBinaryTreeNode right = node->right;

	if ( NotPtrCheck(right->left) )
	{
		node->right = right->left;
		node->right->parent = node;
	}
	else
		node->right = NULL;
	right->left = node;
	right->parent = node->parent;
	node->parent = right;

	node->height = Max(AVLBinaryTreeHeightHelper(node->left), AVLBinaryTreeHeightHelper(node->right)) + 1;
	right->height = Max(AVLBinaryTreeHeightHelper(right->left), AVLBinaryTreeHeightHelper(right->right)) + 1;

	return right;
}

static _pAVLBinaryTreeNode __stdcall 
AVLBinaryTreeRotateRight(_pAVLBinaryTreeNode node)
{
	_pAVLBinaryTreeNode left = node->left;

	if ( NotPtrCheck(left->right) )
	{
		node->left = left->right;
		node->left->parent = node;
	}
	else
		node->left = NULL;
	left->right = node;
	left->parent = node->parent;
	node->parent = left;

	node->height = Max(AVLBinaryTreeHeightHelper(node->left), AVLBinaryTreeHeightHelper(node->right)) + 1;
	left->height = Max(AVLBinaryTreeHeightHelper(left->left), AVLBinaryTreeHeightHelper(left->right)) + 1;

	return left;
}

static sdword __stdcall 
AVLBinaryTreeBalance(_pAVLBinaryTreeNode node)
{
	if (PtrCheck(node))
		return 0;
	return AVLBinaryTreeHeightHelper(node->left) - AVLBinaryTreeHeightHelper(node->right);
}

static _pAVLBinaryTreeNode __stdcall 
AVLBinaryTreeInsertSortedHelper(_pAVLBinaryTreeHead head, _pAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc, _pAVLBinaryTreeNode* result)
{
	sword cmp;
	sdword balance;

	if (PtrCheck(node))
	{
		node = CastAnyPtr(_AVLBinaryTreeNode, TFalloc(szAVLBinaryTreeNode));
		if (PtrCheck(node))
			return NULL;
		node->data = CastMutable(Pointer, data);
		node->head = head;
		node->height = 1;
		++(head->nodeCount);
		if (NotPtrCheck(result))
			*result = node;
		return node;
	}
	cmp = sortFunc(node->data, data);
	if (cmp == 0)
		return node;
	if (cmp > 0)
	{
		node->left = AVLBinaryTreeInsertSortedHelper(head, node->left, data, sortFunc, result);
		if (NotPtrCheck(node->left))
			node->left->parent = node;
	}
	else
	{
		node->right = AVLBinaryTreeInsertSortedHelper(head, node->right, data, sortFunc, result);
		if (NotPtrCheck(node->right))
			node->right->parent = node;
	}
	node->height = Max(AVLBinaryTreeHeightHelper(node->left), AVLBinaryTreeHeightHelper(node->right)) + 1;
	balance = AVLBinaryTreeBalance(node);
	if ((balance > 1) && (sortFunc(node->left->data, data) > 0))
		return AVLBinaryTreeRotateRight(node);
	if ((balance < -1) && (sortFunc(node->right->data, data) < 0))
		return AVLBinaryTreeRotateLeft(node);
	if ((balance > 1) && (sortFunc(node->left->data, data) < 0))
	{
		node->left = AVLBinaryTreeRotateLeft(node->left);
		return AVLBinaryTreeRotateRight(node);
	}
	if ((balance < -1) && (sortFunc(node->right->data, data) > 0))
	{
		node->right = AVLBinaryTreeRotateRight(node->right);
		return AVLBinaryTreeRotateLeft(node);
	}
	return node;
}

LSearchResultType __stdcall 
AVLBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	_pAVLBinaryTreeNode node1 = head->root;
	_pAVLBinaryTreeNode node2 = NULL;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(node1) )
	{
		node2 = CastAnyPtr(_AVLBinaryTreeNode, TFalloc(szAVLBinaryTreeNode));
		if ( PtrCheck(node2) )
			return result;
		node2->data = CastMutable(Pointer, data);
		node2->head = head;
		node2->height = 1;
		head->root = node2;
		++(head->nodeCount);
		_Lnode(result) = node2;
		return result;
	}
	node2 = NULL;
	head->root = AVLBinaryTreeInsertSortedHelper(head, node1, data, sortFunc, &node2);
	_Lnode(result) = node2;
	return result;
}

static _pAVLBinaryTreeNode __stdcall
AVLBinaryTreeRemoveSortedHelper(_pAVLBinaryTreeHead head, _pAVLBinaryTreeNode root, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context, bool freedata, bool *result)
{
	_pAVLBinaryTreeNode temp;
	sword cmp;
	sdword balance;

	if (root == NULL)
		return root;

	cmp = sortFunc(root->data, data);
	if (cmp > 0)
	{
		root->left = AVLBinaryTreeRemoveSortedHelper(head, root->left, data, sortFunc, freeFunc, context, freedata, result);
		if (NotPtrCheck(root->left))
			root->left->parent = root;
	}
	else if (cmp < 0)
	{
		root->right = AVLBinaryTreeRemoveSortedHelper(head, root->right, data, sortFunc, freeFunc, context, freedata, result);
		if (NotPtrCheck(root->right))
			root->right->parent = root;
	}
	else
	{
		if (freedata)
		{
			if (PtrCheck(freeFunc))
				TFfree(root->data);
			else
				freeFunc(root->data, context);
		}
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
				root->data = temp->data;
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
			root->data = temp->data;
			root->right = AVLBinaryTreeRemoveSortedHelper(head, root->right, temp->data, sortFunc, freeFunc, context, false, result);
			if (NotPtrCheck(root->right))
				root->right->parent = root;
		}
	}
	if (root == NULL)
		return root;
	root->height = Max(AVLBinaryTreeHeightHelper(root->left), AVLBinaryTreeHeightHelper(root->right)) + 1;
	balance = AVLBinaryTreeBalance(root);
	if ((balance > 1) && (AVLBinaryTreeBalance(root->left) >= 0))
		return AVLBinaryTreeRotateRight(root);
	if ((balance > 1) && (AVLBinaryTreeBalance(root->left) < 0))
	{
		root->left = AVLBinaryTreeRotateLeft(root->left);
		return AVLBinaryTreeRotateRight(root);
	}
	if ((balance < -1) && (AVLBinaryTreeBalance(root->right) <= 0))
		return AVLBinaryTreeRotateLeft(root);
	if ((balance < -1) && (AVLBinaryTreeBalance(root->right) > 0))
	{
		root->right = AVLBinaryTreeRotateRight(root->right);
		return AVLBinaryTreeRotateLeft(root);
	}
	return root;
}

bool __stdcall 
AVLBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	_pAVLBinaryTreeNode node = head->root;
	bool result = false;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(node) )
		return false;
	head->root = AVLBinaryTreeRemoveSortedHelper(head, node, data, sortFunc, freeFunc, context, true, &result);
	return result;
}

Pointer __stdcall 
AVLBinaryTreeGetData(LSearchResultType node)
{
	_pAVLBinaryTreeNode node1 = CastAnyPtr(_AVLBinaryTreeNode, _Lnode(node));

	if ( PtrCheck(node1) )
		return NULL;
	return node1->data;
}

void __stdcall 
AVLBinaryTreeSetData(LSearchResultType node, ConstPointer data)
{
	_pAVLBinaryTreeNode node1 = CastAnyPtr(_AVLBinaryTreeNode, _Lnode(node));

	assert(node1 != NULL);
	node1->data = CastMutable(Pointer, data);
}

/*			    */
/* RBBinaryTree */
/*			    */
enum _RBBinaryTreeNodeColor { RED, BLACK };
struct _tagRBBinaryTreeHead;
typedef struct _tagRBBinaryTreeNode
{
	Ptr(struct _tagRBBinaryTreeHead) head;
	Ptr(struct _tagRBBinaryTreeNode) parent;
	Ptr(struct _tagRBBinaryTreeNode) left;
	Ptr(struct _tagRBBinaryTreeNode) right;
	enum _RBBinaryTreeNodeColor color;
	Pointer data;
} _RBBinaryTreeNode, *_pRBBinaryTreeNode;
#define szRBBinaryTreeNode sizeof(_RBBinaryTreeNode)

typedef struct _tagRBBinaryTreeHead
{
	Ptr(struct _tagRBBinaryTreeNode) root;
	TListCnt nodeCount;
} _RBBinaryTreeHead, *_pRBBinaryTreeHead;
#define szRBBinaryTreeHead sizeof(_RBBinaryTreeHead)

static enum _RBBinaryTreeNodeColor __stdcall
RBBinaryTreeNodeColor(_pRBBinaryTreeNode n) {
	return PtrCheck(n) ? BLACK : n->color;
}

static void __stdcall
RBBinaryTreeVerifyProperties1(_pRBBinaryTreeNode n)
{
	assert(RBBinaryTreeNodeColor(n) == RED || RBBinaryTreeNodeColor(n) == BLACK);
	if (n == NULL) return;
	RBBinaryTreeVerifyProperties1(n->left);
	RBBinaryTreeVerifyProperties1(n->right);
}

static void __stdcall
RBBinaryTreeVerifyProperties2(_pRBBinaryTreeNode root)
{
	assert(RBBinaryTreeNodeColor(root) == BLACK);
}

static void __stdcall
RBBinaryTreeVerifyProperties4(_pRBBinaryTreeNode n)
{
	if (RBBinaryTreeNodeColor(n) == RED) {
		assert(RBBinaryTreeNodeColor(n->left) == BLACK);
		assert(RBBinaryTreeNodeColor(n->right) == BLACK);
		assert(RBBinaryTreeNodeColor(n->parent) == BLACK);
	}
	if (n == NULL) return;
	RBBinaryTreeVerifyProperties4(n->left);
	RBBinaryTreeVerifyProperties4(n->right);
}

static void __stdcall
RBBinaryTreeVerifyProperties5Helper(_pRBBinaryTreeNode n, int black_count, int* path_black_count)
{
	if (RBBinaryTreeNodeColor(n) == BLACK) {
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
	RBBinaryTreeVerifyProperties5Helper(n->left, black_count, path_black_count);
	RBBinaryTreeVerifyProperties5Helper(n->right, black_count, path_black_count);
}

static void __stdcall
RBBinaryTreeVerifyProperties5(_pRBBinaryTreeNode root)
{
	int black_count_path = -1;
	RBBinaryTreeVerifyProperties5Helper(root, 0, &black_count_path);
}

static void __stdcall
RBBinaryTreeVerifyProperties(_pRBBinaryTreeHead t)
{
#ifdef VERIFY_RBTREE
	RBBinaryTreeVerifyProperties1(t->root);
	RBBinaryTreeVerifyProperties2(t->root);
	RBBinaryTreeVerifyProperties4(t->root);
	RBBinaryTreeVerifyProperties5(t->root);
#endif
}

Pointer __stdcall
RBBinaryTreeOpen(DECL_FILE_LINE0)
{
#ifdef __DEBUG__
	_pRBBinaryTreeHead p = CastAnyPtr(_RBBinaryTreeHead, TFallocDbg(szRBBinaryTreeHead, file, line));
#else
	_pRBBinaryTreeHead p = CastAnyPtr(_RBBinaryTreeHead, TFalloc(szRBBinaryTreeHead));
#endif
	RBBinaryTreeVerifyProperties(p);
	return p;
}

TListCnt __stdcall
RBBinaryTreeCount(Pointer liste)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);

	assert(liste != NULL);
	return head->nodeCount;
}

static TListCnt __stdcall
RBBinaryTreeHeightHelper(_pRBBinaryTreeNode node)
{
	TListCnt l;
	TListCnt r;

	if (PtrCheck(node))
		return 0;
	l = RBBinaryTreeHeightHelper(node->left);
	r = RBBinaryTreeHeightHelper(node->right);
	return Max(l, r) + 1;
}

TListCnt __stdcall
RBBinaryTreeHeight(Pointer liste)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);

	assert(liste != NULL);
	return RBBinaryTreeHeightHelper(head->root);
}

static void __stdcall
RBBinaryTreeCloseHelper(_pRBBinaryTreeNode node)
{
	_pRBBinaryTreeNode nodeLeft = node->left;
	_pRBBinaryTreeNode nodeRight = node->right;

	if (NotPtrCheck(nodeLeft))
	{
		RBBinaryTreeCloseHelper(nodeLeft);
		TFfree(nodeLeft);
	}
	if (NotPtrCheck(nodeRight))
	{
		RBBinaryTreeCloseHelper(nodeRight);
		TFfree(nodeRight);
	}
}

void __stdcall
RBBinaryTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = RBBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	while (!LPtrCheck(result))
	{
		d = RBBinaryTreeGetData(result);
		if (PtrCheck(freeFunc))
			TFfree(d);
		else
			freeFunc(d, context);
		result = RBBinaryTreeNext(result);
	}
	if (PtrCheck(liste))
		return;
	if (NotPtrCheck(head->root))
	{
		RBBinaryTreeCloseHelper(head->root);
		TFfree(head->root);
	}
	TFfree(head);
}

LSearchResultType __stdcall
RBBinaryTreeBegin(Pointer liste)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;
	_pRBBinaryTreeNode node;

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
RBBinaryTreeNext(LSearchResultType node)
{
	_pRBBinaryTreeNode node1 = CastAnyPtr(_RBBinaryTreeNode, _Lnode(node));
	_pRBBinaryTreeNode node2 = NULL;
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
RBBinaryTreePrev(LSearchResultType node)
{
	_pRBBinaryTreeNode node1 = CastAnyPtr(_RBBinaryTreeNode, _Lnode(node));
	_pRBBinaryTreeNode node2 = NULL;
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
RBBinaryTreeLast(Pointer liste)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;
	_pRBBinaryTreeNode node;

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
RBBinaryTreeForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	LSearchResultType it = RBBinaryTreeBegin(liste);
	bool result = true;
	Pointer d;

	assert(liste != NULL);
	assert(func != NULL);
	while (!LPtrCheck(it))
	{
		d = RBBinaryTreeGetData(it);
		if (0 == func(d, context))
		{
			result = false;
			break;
		}
		it = RBBinaryTreeNext(it);
	}
	return result;
}

LSearchResultType __stdcall
RBBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	LSearchResultType result = RBBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while (!LPtrCheck(result))
	{
		d = RBBinaryTreeGetData(result);
		if (0 == findFunc(d, data))
			return result;
		result = RBBinaryTreeNext(result);
	}
	return _LNULL;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeFindSortedHelper(_pRBBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(node->data, data);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return RBBinaryTreeFindSortedHelper(node->left, data, sortFunc);
	}
	if (PtrCheck(node->right))
		return NULL;
	return RBBinaryTreeFindSortedHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall
RBBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = RBBinaryTreeFindSortedHelper(head->root, data, sortFunc);
	return result;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeUpperBoundHelper(_pRBBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(node->data, data);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return RBBinaryTreeUpperBoundHelper(node->left, data, sortFunc);
	}
	if (PtrCheck(node->right))
		return NULL;
	return RBBinaryTreeUpperBoundHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall
RBBinaryTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = RBBinaryTreeUpperBoundHelper(head->root, data, sortFunc);
	return result;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeLowerBoundHelper(_pRBBinaryTreeNode node, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	sword funcResult = sortFunc(node->data, data);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return RBBinaryTreeLowerBoundHelper(node->left, data, sortFunc);
	}
	if (PtrCheck(node->right))
		return NULL;
	return RBBinaryTreeLowerBoundHelper(node->right, data, sortFunc);
}

LSearchResultType __stdcall
RBBinaryTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = RBBinaryTreeLowerBoundHelper(head->root, data, sortFunc);
	return result;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeGrandParent(_pRBBinaryTreeNode node)
{
	assert(NotPtrCheck(node));
	assert(NotPtrCheck(node->parent));
	assert(NotPtrCheck(node->parent->parent));
	return node->parent->parent;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeSibling(_pRBBinaryTreeNode node)
{
	assert(NotPtrCheck(node));
	assert(NotPtrCheck(node->parent));
	if (node == node->parent->left)
		return node->parent->right;
	return node->parent->left;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeUncle(_pRBBinaryTreeNode node)
{
	assert(NotPtrCheck(node));
	assert(NotPtrCheck(node->parent));
	assert(NotPtrCheck(node->parent->parent));
	return RBBinaryTreeSibling(node->parent);
}

static void __stdcall
RBBinaryTreeReplaceNode(_pRBBinaryTreeHead t, _pRBBinaryTreeNode oldn, _pRBBinaryTreeNode newn)
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
RBBinaryTreeRotateLeft(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	_pRBBinaryTreeNode r = n->right;

	RBBinaryTreeReplaceNode(t, n, r);
	n->right = r->left;
	if (NotPtrCheck(r->left))
		r->left->parent = n;
	r->left = n;
	n->parent = r;
}

static void __stdcall
RBBinaryTreeRotateRight(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	_pRBBinaryTreeNode l = n->left;

	RBBinaryTreeReplaceNode(t, n, l);
	n->left = l->right;
	if (NotPtrCheck(l->right))
		l->right->parent = n;
	l->right = n;
	n->parent = l;
}

static void __stdcall
RBBinaryTreeInsertCase1(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n);

static void __stdcall
RBBinaryTreeInsertCase5(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n) 
{
	n->parent->color = BLACK;
	RBBinaryTreeGrandParent(n)->color = RED;
	if ((n == n->parent->left) && (n->parent == RBBinaryTreeGrandParent(n)->left))
		RBBinaryTreeRotateRight(t, RBBinaryTreeGrandParent(n));
	else
	{
		assert((n == n->parent->right) && (n->parent == RBBinaryTreeGrandParent(n)->right));
		RBBinaryTreeRotateLeft(t, RBBinaryTreeGrandParent(n));
	}
}

static void __stdcall
RBBinaryTreeInsertCase4(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	if ((n == n->parent->right) && (n->parent == RBBinaryTreeGrandParent(n)->left))
	{
		RBBinaryTreeRotateLeft(t, n->parent);
		n = n->left;
	}
	else if ((n == n->parent->left) && (n->parent == RBBinaryTreeGrandParent(n)->right))
	{
		RBBinaryTreeRotateRight(t, n->parent);
		n = n->right;
	}
	RBBinaryTreeInsertCase5(t, n);
}

static void __stdcall
RBBinaryTreeInsertCase3(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n) 
{
	if (RBBinaryTreeNodeColor(RBBinaryTreeUncle(n)) == RED) {
		n->parent->color = BLACK;
		RBBinaryTreeUncle(n)->color = BLACK;
		RBBinaryTreeGrandParent(n)->color = RED;
		RBBinaryTreeInsertCase1(t, RBBinaryTreeGrandParent(n));
	}
	else
		RBBinaryTreeInsertCase4(t, n);
}

static void __stdcall
RBBinaryTreeInsertCase2(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n) 
{
	if (RBBinaryTreeNodeColor(n->parent) == BLACK)
		return;
	RBBinaryTreeInsertCase3(t, n);
}

static void __stdcall
RBBinaryTreeInsertCase1(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	if (PtrCheck(n->parent))
		n->color = BLACK;
	else
		RBBinaryTreeInsertCase2(t, n);
}

LSearchResultType __stdcall
RBBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	_pRBBinaryTreeNode inserted_node = CastAnyPtr(_RBBinaryTreeNode, TFalloc(szRBBinaryTreeNode));
	LSearchResultType result = _LNULL;

	if (PtrCheck(inserted_node))
		return result;
	inserted_node->data = CastMutable(Pointer, data);
	inserted_node->head = head;
	inserted_node->color = RED;
	++(head->nodeCount);

	if (head->root == NULL) {
		head->root = inserted_node;
	}
	else {
		_pRBBinaryTreeNode n = head->root;

		while (true) 
		{
			sword cmp = sortFunc(n->data, data);

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
	RBBinaryTreeInsertCase1(head, inserted_node);
	RBBinaryTreeVerifyProperties(head);

	_Lnode(result) = inserted_node;
	return result;
}

static void __stdcall
RBBinaryTreeDeleteCase1(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n);

static void __stdcall
RBBinaryTreeDeleteCase6(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	RBBinaryTreeSibling(n)->color = RBBinaryTreeNodeColor(n->parent);
	n->parent->color = BLACK;
	if (n == n->parent->left) {
		assert(RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->right) == RED);
		RBBinaryTreeSibling(n)->right->color = BLACK;
		RBBinaryTreeRotateLeft(t, n->parent);
	}
	else
	{
		assert(RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->left) == RED);
		RBBinaryTreeSibling(n)->left->color = BLACK;
		RBBinaryTreeRotateRight(t, n->parent);
	}
}

static void __stdcall
RBBinaryTreeDeleteCase5(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	if (n == n->parent->left &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)) == BLACK &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->left) == RED &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->right) == BLACK)
	{
		RBBinaryTreeSibling(n)->color = RED;
		RBBinaryTreeSibling(n)->left->color = BLACK;
		RBBinaryTreeRotateRight(t, RBBinaryTreeSibling(n));
	}
	else if (n == n->parent->right &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)) == BLACK &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->right) == RED &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->left) == BLACK)
	{
		RBBinaryTreeSibling(n)->color = RED;
		RBBinaryTreeSibling(n)->right->color = BLACK;
		RBBinaryTreeRotateLeft(t, RBBinaryTreeSibling(n));
	}
	RBBinaryTreeDeleteCase6(t, n);
}

static void __stdcall
RBBinaryTreeDeleteCase4(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	if (RBBinaryTreeNodeColor(n->parent) == RED &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)) == BLACK &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->left) == BLACK &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->right) == BLACK)
	{
		RBBinaryTreeSibling(n)->color = RED;
		n->parent->color = BLACK;
	}
	else
		RBBinaryTreeDeleteCase5(t, n);
}

static void __stdcall
RBBinaryTreeDeleteCase3(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	if (RBBinaryTreeNodeColor(n->parent) == BLACK &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)) == BLACK &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->left) == BLACK &&
		RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)->right) == BLACK)
	{
		RBBinaryTreeSibling(n)->color = RED;
		RBBinaryTreeDeleteCase1(t, n->parent);
	}
	else
		RBBinaryTreeDeleteCase4(t, n);
}

static void __stdcall
RBBinaryTreeDeleteCase2(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	if (RBBinaryTreeNodeColor(RBBinaryTreeSibling(n)) == RED)
	{
		n->parent->color = RED;
		RBBinaryTreeSibling(n)->color = BLACK;
		if (n == n->parent->left)
			RBBinaryTreeRotateLeft(t, n->parent);
		else
			RBBinaryTreeRotateRight(t, n->parent);
	}
	RBBinaryTreeDeleteCase3(t, n);
}

static void __stdcall
RBBinaryTreeDeleteCase1(_pRBBinaryTreeHead t, _pRBBinaryTreeNode n)
{
	if (n->parent == NULL)
		return;
	RBBinaryTreeDeleteCase2(t, n);
}

bool __stdcall
RBBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	_pRBBinaryTreeNode child;
	_pRBBinaryTreeNode n = head->root;

	if (PtrCheck(n)) 
		return false;
	n = RBBinaryTreeFindSortedHelper(n, data, sortFunc);
	if (PtrCheck(n)) 
		return false;
	if (NotPtrCheck(n->left) && NotPtrCheck(n->right))
	{
		_pRBBinaryTreeNode pred = n->left;
		Pointer vData;

		while (pred->right != NULL) 
			pred = pred->right;
		vData = n->data;
		n->data = pred->data;
		pred->data = vData;
		n = pred;
	}

	assert(PtrCheck(n->left) || PtrCheck(n->right));
	child = PtrCheck(n->right) ? n->left : n->right;
	if (RBBinaryTreeNodeColor(n) == BLACK) {
		n->color = RBBinaryTreeNodeColor(child);
		RBBinaryTreeDeleteCase1(head, n);
	}
	RBBinaryTreeReplaceNode(head, n, child);
	if (PtrCheck(n->parent) && NotPtrCheck(child))
		child->color = BLACK;

	if (PtrCheck(freeFunc))
		TFfree(n->data);
	else
		freeFunc(n->data, context);
	TFfree(n);
	--(head->nodeCount);

	RBBinaryTreeVerifyProperties(head);
	return true;
}

Pointer __stdcall
RBBinaryTreeGetData(LSearchResultType node)
{
	_pRBBinaryTreeNode node1 = CastAnyPtr(_RBBinaryTreeNode, _Lnode(node));

	if (PtrCheck(node1))
		return NULL;
	return node1->data;
}

void __stdcall
RBBinaryTreeSetData(LSearchResultType node, ConstPointer data)
{
	_pRBBinaryTreeNode node1 = CastAnyPtr(_RBBinaryTreeNode, _Lnode(node));

	assert(node1 != NULL);
	node1->data = CastMutable(Pointer, data);
}

/*			 */
/* BTree */
/*			 */
struct _tagBTreeHead;
typedef struct _tagBTreeNode
{
	struct _tagBTreeHead* head;
	struct _tagBTreeNode* parent;
	TListCnt cnt;
	Pointer key;
	dword isData;
} _BTreeNode, *_pBTreeNode;
#define szBTreeNode sizeof(_BTreeNode)

typedef struct _tagBTreeHead
{
	TListCnt nodeCount;
	TListCnt maxEntriesPerNode;
	_pBTreeNode root;
} _BTreeHead, *_pBTreeHead;
#define szBTreeHead sizeof(_BTreeHead)

Pointer __stdcall 
BTreeOpen(DECL_FILE_LINE TListCnt maxEntriesPerNode)
{
#ifdef __DEBUG__
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, TFallocDbg(szBTreeHead, file, line));
#else
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, TFalloc(szBTreeHead));
#endif
	_pBTreeNode pNode;

	if ( PtrCheck(pHead) )
		return NULL;
	assert(maxEntriesPerNode > 0);
#ifdef __DEBUG__
	pNode = CastAnyPtr(_BTreeNode, TFallocDbg(szBTreeNode + (maxEntriesPerNode * szPointer), file, line));
#else
	pNode = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (maxEntriesPerNode * szPointer)));
#endif
	if ( PtrCheck(pNode) )
	{
		TFfree(pHead);
		return NULL;
	}

	pHead->maxEntriesPerNode = maxEntriesPerNode;
	pHead->root = pNode;

	pNode->head = pHead;
	pNode->isData = TRUE;

	return pHead;
}

TListCnt __stdcall 
BTreeCount(Pointer liste)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);

	assert(liste != NULL);
	return pHead->nodeCount;
}

TListCnt __stdcall 
BTreeHeight(Pointer liste)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	Array p;
	TListCnt height = 1;

	assert(liste != NULL);
	assert(pNode != NULL);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
		pNode = DerefAnyPtr(_pBTreeNode, p);
		++height;
	}
	return height;
}

static void __stdcall
BTreeCloseHelper(_pBTreeNode node)
{
	Array p;
	TListCnt ix;

	if ( !(node->isData) )
	{
		for ( ix = 0, p = CastAny(Array, _l_ptradd(node, szBTreeNode)); 
			ix < node->cnt; 
			++ix, p = CastAny(Array, _l_ptradd(p, szPointer)) 
		)
			BTreeCloseHelper(DerefAnyPtr(_pBTreeNode, p));
	}
	TFfree(node);
}

void __stdcall 
BTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	LSearchResultType result = BTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	while ( !LPtrCheck(result) )
	{
		d = BTreeGetData(result);
		if ( PtrCheck(freeFunc) )
			TFfree(d);
		else
			freeFunc(d, context);
		result = BTreeNext(result);
	}
	if ( PtrCheck(liste) )
		return;
	BTreeCloseHelper(pHead->root);
	TFfree(pHead);
}

static LSearchResultType __stdcall
BTreeIndexHelper(_pBTreeNode node, Ptr(TListIndex) index)
{
	Array p;
	TListCnt ix;
	LSearchResultType result = _LNULL;

	if ( !(node->isData) )
	{
		for ( ix = 0, p = CastAny(Array, _l_ptradd(node, szBTreeNode)); 
			ix < node->cnt; 
			++ix, p = CastAny(Array, _l_ptradd(p, szPointer)) 
		)
		{
			result = BTreeIndexHelper(DerefAnyPtr(_pBTreeNode, p), index);
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

CSOURCES_API LSearchResultType __stdcall 
BTreeIndex(Pointer liste, TListIndex index)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	if ( (0 == pHead->nodeCount) || (index < 0) || (index >= Cast(TListIndex,pHead->nodeCount)) )
		return _LNULL;
	return BTreeIndexHelper(pNode, &index);
}

LSearchResultType __stdcall 
BTreeBegin(Pointer liste)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	Array p;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
		pNode = DerefAnyPtr(_pBTreeNode, p);
	}
	if ( 0 == pNode->cnt )
		return result;
	_Lnode(result) = pNode;
	_Loffset(result) = 0;
	return result;
}

LSearchResultType __stdcall 
BTreeNext(LSearchResultType node)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	_pBTreeNode pNode1;
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
		p = CastAny(Array, _l_ptradd(pNode1, szBTreeNode));
		pos = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(pos >= 0);
		if ( ++pos < Cast(sdword,pNode1->cnt) )
			break;
		pNode = pNode1;
	}
	if ( PtrCheck(pNode->parent) )
		return result;
	p = CastAny(Array, _l_ptradd(p, pos * szPointer));
	pNode = DerefAnyPtr(_pBTreeNode, p);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
		pNode = DerefAnyPtr(_pBTreeNode, p);
	}
	_Lnode(result) = pNode;
	_Loffset(result) = 0;
	return result;
}

LSearchResultType __stdcall 
BTreePrev(LSearchResultType node)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	_pBTreeNode pNode1;
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
		p = CastAny(Array, _l_ptradd(pNode1, szBTreeNode));
		pos = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(pos >= 0);
		if ( --pos >= 0 )
			break;
		pNode = pNode1;
	}
	if ( PtrCheck(pNode->parent) )
		return result;
	p = CastAny(Array, _l_ptradd(p, pos * szPointer));
	pNode = DerefAnyPtr(_pBTreeNode, p);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szBTreeNode + ((pNode->cnt - 1) * szPointer)));
		pNode = DerefAnyPtr(_pBTreeNode, p);
	}
	_Lnode(result) = pNode;
	_Loffset(result) = pNode->cnt - 1;
	return result;
}

LSearchResultType __stdcall 
BTreeLast(Pointer liste)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	Array p;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAny(Array, _l_ptradd(pNode, szBTreeNode + ((pNode->cnt - 1) * szPointer)));
		pNode = DerefAnyPtr(_pBTreeNode, p);
	}
	if ( 0 == pNode->cnt )
		return result;
	_Lnode(result) = pNode;
	_Loffset(result) = pNode->cnt - 1;
	return result;
}

bool __stdcall 
BTreeForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	LSearchResultType it = _LNULL;
	Pointer d;
	bool result = true;

	assert(liste != NULL);
	assert(func != NULL);
	it = BTreeBegin(liste);
	while ( !LPtrCheck(it) )
	{
		d = BTreeGetData(it);
		if (0 == func(d, context))
		{
			result = false;
			break;
		}
		it = BTreeNext(it);
	}
	return result;
}

LSearchResultType __stdcall 
BTreeFind(Pointer liste, ConstPointer data, TSearchAndSortFunc findFunc)
{
	LSearchResultType result = _LNULL;
	Pointer d;

	assert(liste != NULL);
	assert(findFunc != NULL);
	result = BTreeBegin(liste);
	while ( !LPtrCheck(result) )
	{
		d = BTreeGetData(result);
		if ( 0 == findFunc(d, data) )
			return result;
		result = BTreeNext(result);
	}
	return result;
}

static LSearchResultType __stdcall 
BTreeFindSortedHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeNode pNode1;
	Array p;
	Array p1;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
        sdword ix = _lv_bsearch( p, data, pNode->cnt, sortFunc, UTLPTR_MATCHMODE );

		if ( ix < 0 )
			return result;
		_Lnode(result) = pNode;
		_Loffset(result) = ix;
		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
			return BTreeFindSortedHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc);
	}
	return result;
}

LSearchResultType __stdcall 
BTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return BTreeFindSortedHelper(pNode, data, sortFunc);
}

static LSearchResultType __stdcall 
BTreeUpperBoundHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeNode pNode1;
	Array p;
	Array p1;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
        sdword ix = _lv_bsearch( p, data, pNode->cnt, sortFunc, UTLPTR_INSERTMODE );
		if ( ix < 0 )
			return result;
		_Lnode(result) = pNode;
		_Loffset(result) = ix;
		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
			return BTreeUpperBoundHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc);
	}
	return result;
}

LSearchResultType __stdcall 
BTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return BTreeUpperBoundHelper(pNode, data, sortFunc);
}

static LSearchResultType __stdcall 
BTreeLowerBoundHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeNode pNode1;
	Array p;
	Array p1;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
        sdword ix = _lv_bsearch( p, data, pNode->cnt, sortFunc, UTLPTR_SEARCHMODE );
		if ( ix < 0 )
			return result;
		_Lnode(result) = pNode;
		_Loffset(result) = ix;
		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
			return BTreeLowerBoundHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc);
	}
	return result;
}

LSearchResultType __stdcall 
BTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return BTreeLowerBoundHelper(pNode, data, sortFunc);
}

static sword __stdcall 
BTreeTestSortFunc( ConstPointer pa, ConstPointer pb )
{
	if ( pa < pb )
		return -1;
	if ( pa > pb )
		return 1;
	return 0;
}

static void __stdcall
BTreeSortedTester1(_pBTreeNode pNode, TSearchAndSortFunc sortFunc)
{
	_pBTreeNode pNode1;
	Pointer d1;
	Pointer d2;
	Array p;
	Array p1;
	sdword ix;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
		if ( pNode->cnt > 0 )
		{
			d1 = DerefAnyPtr(Pointer, p);
			d2 = pNode->key;
			assert(0 == sortFunc(d1, d2));
		}
	}
	else
	{
		assert(pNode->cnt > 0);
		for ( ix = 0, p1 = p; ix < Cast(sdword,pNode->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
			BTreeSortedTester1(DerefAnyPtr(_pBTreeNode, p1), sortFunc);
		pNode1 = DerefAnyPtr(_pBTreeNode, p);
		d1 = pNode1->key;
		for ( ix = 1, p1 = CastAny(Array, _l_ptradd(p, szPointer)); ix < Cast(sdword,pNode->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
		{
			pNode1 = DerefAnyPtr(_pBTreeNode, p1);
			d2 = pNode1->key;
			assert(0 > sortFunc(d1, d2));
			d1 = d2;
		}
	}
}

static void __stdcall
BTreeSortedTester(Pointer liste, TSearchAndSortFunc sortFunc)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	LSearchResultType result = _LNULL;
	Pointer d1;
	Pointer d2;

	BTreeSortedTester1(pNode, sortFunc);
	result = BTreeBegin(liste);
	if ( !LPtrCheck(result) )
	{
		d1 = BTreeGetData(result);
		result = BTreeNext(result);
	}
	while ( !LPtrCheck(result) )
	{
		d2 = BTreeGetData(result);
		assert( 0 > sortFunc(d1, d2) );
		result = BTreeNext(result);
		d1 = d2;
	}
}

static _pBTreeNode __stdcall
BTreeInsertSortedHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc, Ptr(LSearchResultType) pResult)
{
	_pBTreeNode pNode1 = NULL;
	_pBTreeNode pNode2;
	Array p;
	Array p1;
    TListCnt ix;
	dword cnt;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
        sdword ix = _lv_binsert(p, data, &(pNode->cnt), sortFunc);

		_LPnode(pResult) = pNode;
		_LPoffset(pResult) = ix;
		if ( ix == 0 )
		{
			pNode1 = pNode;
			pNode1->key = DerefPtr(Pointer, _l_ptradd(pNode1, szBTreeNode));
			while ( NotPtrCheck(pNode1->parent) )
			{
				pNode2 = pNode1->parent;
				ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
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
			pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
			pNode2->head = pNode->head;
			p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			cnt = pNode->cnt / 2;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode2->key = DerefAnyPtr(Pointer,p1);
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
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
        pNode1 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
		{
			pNode1 = BTreeInsertSortedHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc, pResult);
			if ( PtrCheck(pNode1) )
				return NULL;
			pNode1->parent = pNode;
			break;
		}
	}
	_lv_insert(p, ix, pNode1, &(pNode->cnt));
	if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
	{
		pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
		pNode2->head = pNode->head;
		p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
		cnt = pNode->cnt / 2;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode1 = DerefAnyPtr(_pBTreeNode,p1);
		pNode2->key = pNode1->key;
        for ( ix = 0; ix < pNode2->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
		{
			pNode1 = DerefAnyPtr(_pBTreeNode,p1);
			pNode1->parent = pNode2;
		}
		return pNode2;
	}
	return NULL;
}

LSearchResultType __stdcall 
BTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	_pBTreeNode pNode2;
	Array p1;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = BTreeInsertSortedHelper(pNode, data, sortFunc, &result);
	if ( PtrCheck(pNode) )
		return result;
	pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
	pNode2->head = pHead;
	pNode2->cnt = 2;
	p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
	DerefAnyPtr(_pBTreeNode, p1) = pHead->root;
	pHead->root->parent = pNode2;
	pNode2->key = pHead->root->key;
	p1 = CastAny(Array, _l_ptradd(p1, szPointer));
	DerefAnyPtr(_pBTreeNode, p1) = pNode;
	pNode->parent = pNode2;
	pHead->root = pNode2;
	BTreeSortedTester(liste, sortFunc);
	return result;
}

static void __stdcall
BTreeRemoveJoinHelper(_pBTreeNode pNodeDest, _pBTreeNode pNodeSrc)
{
	Array pDest = CastAny(Array, _l_ptradd(pNodeDest, szBTreeNode));
	Array pSrc = CastAny(Array, _l_ptradd(pNodeSrc, szBTreeNode));
	_pBTreeNode pNode;
	sdword ix;

	pDest = CastAny(Array, _l_ptradd(pDest, pNodeDest->cnt * szPointer));
	s_memcpy_s(pDest, (pNodeDest->head->maxEntriesPerNode - pNodeDest->cnt) * szPointer,
		pSrc, pNodeSrc->cnt * szPointer);
	pNodeDest->cnt += pNodeSrc->cnt;
	if ( !(pNodeDest->isData) )
	{
		for ( ix = 0; ix < Cast(sdword, pNodeSrc->cnt); ++ix, pDest = CastAny(Array, _l_ptradd(pDest, szPointer)) )
		{
			pNode = DerefAnyPtr(_pBTreeNode, pDest);
			pNode->parent = pNodeDest;
		}
	}
	TFfree(pNodeSrc);
}

static void __stdcall
BTreeRemoveKeyUpdateHelper(_pBTreeNode pNode)
{
	_pBTreeNode pNode1;
	_pBTreeNode pNode2;
	sdword ix;

	pNode1 = pNode;
	if ( pNode1->isData )
		pNode1->key = DerefPtr(Pointer, _l_ptradd(pNode1, szBTreeNode));
	else
	{
		pNode2 = DerefPtr(_pBTreeNode, _l_ptradd(pNode1, szBTreeNode));
		pNode1->key = pNode2->key;
	}
	while ( NotPtrCheck(pNode1->parent) )
	{
		pNode2 = pNode1->parent;
		ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
		assert(ix >= 0);
		if ( ix != 0 )
			break;
		pNode2->key = pNode1->key;
		pNode1 = pNode2;
	}
}

static _pBTreeNode __stdcall
BTreeRemoveSortedHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context, Ptr(bool) pResult)
{
	_pBTreeNode pNode1;
	_pBTreeNode pNode2;
	Array p;
	Array p1;
	Pointer data1;
    TListCnt ix;
	bool b;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	assert(pNode->cnt > 0);
	if ( pNode->isData )
	{
        sdword ix;

		DerefPtr(bool,pResult) = true;
		ix = _lv_bsearch( p, data, pNode->cnt, sortFunc, UTLPTR_MATCHMODE );
		if ( ix < 0 )
		{
			DerefPtr(bool,pResult) = false;
			return NULL;
		}
		data1 = DerefPtr(Pointer, _l_ptradd(p, ix * szPointer));
		if ( freeFunc )
			freeFunc(data1, context);
		else
			TFfree(data1);
		_lv_delete( p, ix, &(pNode->cnt) );
		if ( (ix == 0) && (pNode->cnt > 0) )
			BTreeRemoveKeyUpdateHelper(pNode);
		--(pNode->head->nodeCount);
		if ( pNode->cnt < (pNode->head->maxEntriesPerNode / 2) )
			return pNode;
		return NULL;
	}
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data)) )
		{
			pNode1 = BTreeRemoveSortedHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc, freeFunc, context, pResult);
			if ( PtrCheck(pNode1) )
				return NULL;
			assert(pNode1 == DerefAnyPtr(_pBTreeNode, p1));
			if ( (pNode->cnt < 2) && (pNode1->cnt == 0) )
			{
				TFfree(pNode1);
				_lv_delete( p, ix, &(pNode->cnt) );
				return pNode;
			}
			pNode2 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
            if ( (ix < (pNode->cnt - 1)) && ((pNode2->cnt + pNode1->cnt) < pNode->head->maxEntriesPerNode) )
			{
				b = ((ix == 0) && (pNode1->cnt == 0));
				BTreeRemoveJoinHelper(pNode1, pNode2);
				if ( b )
					BTreeRemoveKeyUpdateHelper(pNode1);
				++ix;
				break;
			}
			pNode2 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, -Cast(sdword,szPointer)));
			if ( (ix > 0) && ((pNode2->cnt + pNode1->cnt) < pNode->head->maxEntriesPerNode) )
			{
				b = ((ix == 1) && (pNode2->cnt == 0));
				BTreeRemoveJoinHelper(pNode2, pNode1);
				if ( b )
					BTreeRemoveKeyUpdateHelper(pNode2);
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
BTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc, TDeleteFunc freeFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	bool result = false;
	
	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = BTreeRemoveSortedHelper(pNode, data, sortFunc, freeFunc, context, &result);
	if ( PtrCheck(pNode) )
		return result;
	assert(pNode == pHead->root);
	while ( (!(pNode->isData)) && (pNode->cnt == 1) )
	{
		pHead->root = DerefPtr(_pBTreeNode, _l_ptradd(pNode, szBTreeNode));
		pHead->root->parent = NULL;
		TFfree(pNode);
		pNode = pHead->root;
	}
	BTreeSortedTester(liste, sortFunc);
	return result;
}

static _pBTreeNode __stdcall
BTreeAppendHelper(_pBTreeNode pNode, ConstPointer data, Ptr(LSearchResultType) pResult)
{
	_pBTreeNode pNode1 = NULL;
	_pBTreeNode pNode2;
	Array p;
	Array p1;
    TListCnt ix;
	dword cnt;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
		_LPnode(pResult) = pNode;
		_LPoffset(pResult) = _lv_insert( p, Cast(sdword,pNode->cnt) - 1, data, &(pNode->cnt) );

		if ( _LPoffset(pResult) == 0 )
		{
			pNode1 = pNode;
			pNode1->key = DerefPtr(Pointer, _l_ptradd(pNode1, szBTreeNode));
			while ( NotPtrCheck(pNode1->parent) )
			{
				pNode2 = pNode1->parent;
				ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
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
			pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
			pNode2->head = pNode->head;
			p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			cnt = 1;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode2->key = DerefAnyPtr(Pointer,p1);
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
	pNode1 = BTreeAppendHelper(DerefAnyPtr(_pBTreeNode, p1), data, pResult);
	if ( PtrCheck(pNode1) )
		return NULL;
	pNode1->parent = pNode;
	_lv_insert(p, ix, pNode1, &(pNode->cnt));
	if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
	{
		pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
		pNode2->head = pNode->head;
		p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
		cnt = 1;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode1 = DerefAnyPtr(_pBTreeNode,p1);
		pNode2->key = pNode1->key;
        for ( ix = 0; ix < pNode2->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
		{
			pNode1 = DerefAnyPtr(_pBTreeNode,p1);
			pNode1->parent = pNode2;
		}
		return pNode2;
	}
	return NULL;
}

CSOURCES_API LSearchResultType __stdcall 
BTreeAppend(Pointer liste, ConstPointer data)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	_pBTreeNode pNode2;
	Array p1;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = BTreeAppendHelper(pNode, data, &result);
	if ( PtrCheck(pNode) )
		return result;
	pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
	pNode2->head = pHead;
	pNode2->cnt = 2;
	p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
	DerefAnyPtr(_pBTreeNode, p1) = pHead->root;
	pHead->root->parent = pNode2;
	pNode2->key = pHead->root->key;
	p1 = CastAny(Array, _l_ptradd(p1, szPointer));
	DerefAnyPtr(_pBTreeNode, p1) = pNode;
	pNode->parent = pNode2;
	pHead->root = pNode2;
	return result;
}

static _pBTreeNode __stdcall
BTreePrependHelper(_pBTreeNode pNode, ConstPointer data, Ptr(LSearchResultType) pResult)
{
	_pBTreeNode pNode1 = NULL;
	_pBTreeNode pNode2;
	Array p;
	Array p1;
    TListCnt ix;
	dword cnt;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
		_LPnode(pResult) = pNode;
		_LPoffset(pResult) = _lv_insert( p, -1, data, &(pNode->cnt) );

		if ( _LPoffset(pResult) == 0 )
		{
			pNode1 = pNode;
			pNode1->key = DerefPtr(Pointer, _l_ptradd(pNode1, szBTreeNode));
			while ( NotPtrCheck(pNode1->parent) )
			{
				pNode2 = pNode1->parent;
				ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
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
			pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
			pNode2->head = pNode->head;
			p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			cnt = pNode->cnt - 1;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode2->key = DerefAnyPtr(Pointer,p1);
			pNode2->isData = TRUE;
			return pNode2;
		}
		return NULL;
	}
	assert(pNode->cnt > 0);
	ix = 0;
	p1 = p;
	pNode1 = BTreePrependHelper(DerefAnyPtr(_pBTreeNode, p1), data, pResult);
	if ( PtrCheck(pNode1) )
		return NULL;
	pNode1->parent = pNode;
	_lv_insert(p, ix, pNode1, &(pNode->cnt));
	if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
	{
		pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pNode->head->maxEntriesPerNode * szPointer)));
		pNode2->head = pNode->head;
		p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
		cnt = pNode->cnt - 1;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode1 = DerefAnyPtr(_pBTreeNode,p1);
		pNode2->key = pNode1->key;
        for ( ix = 0; ix < pNode2->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
		{
			pNode1 = DerefAnyPtr(_pBTreeNode,p1);
			pNode1->parent = pNode2;
		}
		return pNode2;
	}
	return NULL;
}

CSOURCES_API LSearchResultType __stdcall 
BTreePrepend(Pointer liste, ConstPointer data)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	_pBTreeNode pNode2;
	Array p1;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = BTreePrependHelper(pNode, data, &result);
	if ( PtrCheck(pNode) )
		return result;
	pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
	pNode2->head = pHead;
	pNode2->cnt = 2;
	p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
	DerefAnyPtr(_pBTreeNode, p1) = pHead->root;
	pHead->root->parent = pNode2;
	pNode2->key = pHead->root->key;
	p1 = CastAny(Array, _l_ptradd(p1, szPointer));
	DerefAnyPtr(_pBTreeNode, p1) = pNode;
	pNode->parent = pNode2;
	pHead->root = pNode2;
	return result;
}

CSOURCES_API LSearchResultType __stdcall 
BTreeInsertBefore(LSearchResultType node, ConstPointer data)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	_pBTreeHead pHead = pNode->head;
	_pBTreeNode pNode1;
	_pBTreeNode pNode2;
	Array p;
	Array p1;
	sdword ix = _Loffset(node);
	TListCnt cnt;

	assert(pNode->isData);
	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	ix = _lv_insert( p, ix - 1, data, &(pNode->cnt) );
	if ( ix == 0 )
	{
		pNode1 = pNode;
		pNode1->key = DerefPtr(Pointer, _l_ptradd(pNode1, szBTreeNode));
		while ( NotPtrCheck(pNode1->parent) )
		{
			pNode2 = pNode1->parent;
			ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
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
		pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
		pNode2->head = pHead;
		pNode2->parent = pNode->parent;
		p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
		cnt = pNode->cnt / 2;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode2->key = DerefAnyPtr(Pointer,p1);
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
		p = CastAny(Array, _l_ptradd(pNode1, szBTreeNode));
		assert(pNode1->cnt > 0);
		ix = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(ix >= 0);
		_lv_insert(p, ix, pNode2, &(pNode1->cnt));
		pNode = pNode1;
		pNode2 = NULL;
		if ( pNode->cnt >= pHead->maxEntriesPerNode )
		{
			pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
			pNode2->head = pHead;
			pNode2->parent = pNode->parent;
			p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			cnt = pNode->cnt / 2;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode1 = DerefAnyPtr(_pBTreeNode,p1);
			pNode2->key = pNode1->key;
			for ( ix = 0; ix < Cast(sdword, pNode2->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
			{
				pNode1 = DerefAnyPtr(_pBTreeNode,p1);
				pNode1->parent = pNode2;
			}
		}
	}
	if ( NotPtrCheck(pNode2) )
	{
		pNode1 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
		pNode1->head = pHead;
		pNode1->cnt = 2;
		p1 = CastAny(Array, _l_ptradd(pNode1, szBTreeNode));
		DerefAnyPtr(_pBTreeNode, p1) = pHead->root;
		pHead->root->parent = pNode1;
		pNode1->key = pHead->root->key;
		p1 = CastAny(Array, _l_ptradd(p1, szPointer));
		DerefAnyPtr(_pBTreeNode, p1) = pNode2;
		pNode2->parent = pNode1;
		pHead->root = pNode1;
	}
	return node;
}

CSOURCES_API LSearchResultType __stdcall 
BTreeInsertAfter(LSearchResultType node, ConstPointer data)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	_pBTreeHead pHead = pNode->head;
	_pBTreeNode pNode1;
	_pBTreeNode pNode2;
	Array p;
	Array p1;
	sdword ix = _Loffset(node);
	TListCnt cnt;

	assert(pNode->isData);
	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	ix = _lv_insert( p, ix, data, &(pNode->cnt) );
	if ( ix == 0 )
	{
		pNode1 = pNode;
		pNode1->key = DerefPtr(Pointer, _l_ptradd(pNode1, szBTreeNode));
		while ( NotPtrCheck(pNode1->parent) )
		{
			pNode2 = pNode1->parent;
			ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
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
		pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
		pNode2->head = pHead;
		pNode2->parent = pNode->parent;
		p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
		cnt = pNode->cnt / 2;
		s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
		pNode2->key = DerefAnyPtr(Pointer,p1);
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
		p = CastAny(Array, _l_ptradd(pNode1, szBTreeNode));
		assert(pNode1->cnt > 0);
		ix = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(ix >= 0);
		_lv_insert(p, ix, pNode2, &(pNode1->cnt));
		pNode = pNode1;
		pNode2 = NULL;
		if ( pNode->cnt >= pHead->maxEntriesPerNode )
		{
			pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
			pNode2->head = pHead;
			pNode2->parent = pNode->parent;
			p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			cnt = pNode->cnt / 2;
			s_memcpy(p1, _l_ptradd(p, (pNode->cnt - cnt) * szPointer), cnt * szPointer);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode1 = DerefAnyPtr(_pBTreeNode,p1);
			pNode2->key = pNode1->key;
			for ( ix = 0; ix < Cast(sdword, pNode2->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
			{
				pNode1 = DerefAnyPtr(_pBTreeNode,p1);
				pNode1->parent = pNode2;
			}
		}
	}
	if ( NotPtrCheck(pNode2) )
	{
		pNode1 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + (pHead->maxEntriesPerNode * szPointer)));
		pNode1->head = pHead;
		pNode1->cnt = 2;
		p1 = CastAny(Array, _l_ptradd(pNode1, szBTreeNode));
		DerefAnyPtr(_pBTreeNode, p1) = pHead->root;
		pHead->root->parent = pNode1;
		pNode1->key = pHead->root->key;
		p1 = CastAny(Array, _l_ptradd(p1, szPointer));
		DerefAnyPtr(_pBTreeNode, p1) = pNode2;
		pNode2->parent = pNode1;
		pHead->root = pNode1;
	}
	return node;
}

void __stdcall 
BTreeRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	_pBTreeHead pHead = pNode->head;
	_pBTreeNode pNode1;
	_pBTreeNode pNode2;
	_pBTreeNode pNode3;
	_pBTreeNode pNode4;
	Array p;
	Pointer data1;
	sdword ix = _Loffset(node);
	bool b;

	assert(pNode->cnt > 0);
	assert(pNode->isData);
	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	data1 = DerefPtr(Pointer, _l_ptradd(p, ix * szPointer));
	if ( freeFunc )
		freeFunc(data1, context);
	else
		TFfree(data1);
	_lv_delete( p, ix, &(pNode->cnt) );
	if ( (ix == 0) && (pNode->cnt > 0) )
		BTreeRemoveKeyUpdateHelper(pNode);
	--(pHead->nodeCount);
	if ( pNode->cnt < (pHead->maxEntriesPerNode / 2) )
	{
		pNode1 = pNode;
		while ( NotPtrCheck(pNode1->parent) )
		{
			pNode2 = pNode1->parent;
			p = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			assert(pNode2->cnt > 0);
			ix = _lv_searchptr(p, pNode1, pNode2->cnt);
			assert(ix >= 0);
			pNode3 = DerefAnyPtr(_pBTreeNode,_l_ptradd(p, ix * szPointer));
			if ( (pNode2->cnt < 2) && (pNode3->cnt == 0) )
			{
				TFfree(pNode3);
				_lv_delete(p, ix, &(pNode2->cnt) );
			}
			else
			{
				pNode4 = DerefAnyPtr(_pBTreeNode,_l_ptradd(p, (ix + 1) * szPointer));
				if ( (ix < Cast(sdword, (pNode2->cnt - 1))) && ((pNode3->cnt + pNode4->cnt) < pHead->maxEntriesPerNode) )
				{
					b = ((ix == 0) && (pNode3->cnt == 0));
					BTreeRemoveJoinHelper(pNode3, pNode4);
					_lv_delete(p, ix + 1, &(pNode2->cnt) );
					if ( b )
						BTreeRemoveKeyUpdateHelper(pNode3);
				}
				else
				{
					pNode4 = DerefAnyPtr(_pBTreeNode,_l_ptradd(p, (ix - 1) * szPointer));
					if ( (ix > 0) && ((pNode3->cnt + pNode4->cnt) < pHead->maxEntriesPerNode) )
					{
						b = ((ix == 1) && (pNode4->cnt == 0));
						BTreeRemoveJoinHelper(pNode4, pNode3);
						_lv_delete(p, ix, &(pNode2->cnt) );
						if ( b )
							BTreeRemoveKeyUpdateHelper(pNode4);
					}
				}
			}
			pNode1 = pNode2;
		}
		assert(pNode1 == pHead->root);
		while ( (!(pNode1->isData)) && (pNode1->cnt == 1) )
		{
			pHead->root = DerefPtr(_pBTreeNode, _l_ptradd(pNode1, szBTreeNode));
			pHead->root->parent = NULL;
			TFfree(pNode1);
			pNode1 = pHead->root;
		}
	}
}

Pointer __stdcall 
BTreeGetData(LSearchResultType node)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	Array p;

	if ( PtrCheck(pNode) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, pNode->cnt)) )
		return NULL;
	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode + (_Loffset(node) * szPointer)));
	return DerefPtr(Pointer, p);
}

void __stdcall 
BTreeSetData(LSearchResultType node, ConstPointer data)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	Array p;

	assert(pNode != NULL);
	if ( (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, pNode->cnt)) )
		return;
	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode + (_Loffset(node) * szPointer)));
	DerefPtr(Pointer, p) = CastMutable(Pointer, data);
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

	/* DoubleLinkedList */
	liste = DoubleLinkedListOpen();

	result = DoubleLinkedListAppend(liste, result, CastAny(Pointer,0x2));
	_ASSERTE(LCompareEqual(result, DoubleLinkedListBegin(liste)));
	_ASSERTE(LCompareEqual(result, DoubleLinkedListLast(liste)));

	result = DoubleLinkedListAppend(liste, result, CastAny(Pointer,0x3));
	_ASSERTE(LCompareEqual(result, DoubleLinkedListNext(DoubleLinkedListBegin(liste))));
	_ASSERTE(LCompareEqual(result, DoubleLinkedListLast(liste)));

	result = DoubleLinkedListBegin(liste);
	result = DoubleLinkedListInsert(liste, result, CastAny(Pointer,0x1));
	_ASSERTE(LCompareEqual(result, DoubleLinkedListBegin(liste)));
	_ASSERTE(LCompareEqual(result, DoubleLinkedListPrev(DoubleLinkedListPrev(DoubleLinkedListLast(liste)))));

	result = DoubleLinkedListBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(DoubleLinkedListGetData(result) == CastAny(Pointer,++cnt));
		result = DoubleLinkedListNext(result);
	}

	result = DoubleLinkedListBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(DoubleLinkedListGetData(result) == CastAny(Pointer,++cnt));
		DoubleLinkedListRemove(result, FreeListData);
		result = DoubleLinkedListBegin(liste);
	}

	DoubleLinkedListClose(liste, FreeListData);

	/* Array */
	liste = ArrayOpen(10);

	result = ArrayAppend(liste, result, CastAny(Pointer,0x2));
	_ASSERTE(LCompareEqual(result, ArrayBegin(liste)));
	_ASSERTE(LCompareEqual(result, ArrayLast(liste)));

	result = ArrayAppend(liste, result, CastAny(Pointer,0x3));
	_ASSERTE(LCompareEqual(result, ArrayNext(ArrayBegin(liste))));
	_ASSERTE(LCompareEqual(result, ArrayLast(liste)));

	result = ArrayBegin(liste);
	result = ArrayInsert(liste, result, CastAny(Pointer,0x1));
	_ASSERTE(LCompareEqual(result, ArrayBegin(liste)));
	_ASSERTE(LCompareEqual(result, ArrayPrev(ArrayPrev(ArrayLast(liste)))));

	result = ArrayBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(ArrayGetData(result) == CastAny(Pointer,++cnt));
		result = ArrayNext(result);
	}

	result = ArrayBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(ArrayGetData(result) == CastAny(Pointer,++cnt));
		ArrayRemove(result, FreeListData);
		result = ArrayBegin(liste);
	}

	ArrayClose(liste, FreeListData);

	/* Vector */
	liste = VectorOpen(10, 10);

	result = VectorAppend(liste, result, CastAny(Pointer,0x2));
	_ASSERTE(LCompareEqual(result, VectorBegin(liste)));
	_ASSERTE(LCompareEqual(result, VectorLast(liste)));

	result = VectorAppend(liste, result, CastAny(Pointer,0x3));
	_ASSERTE(LCompareEqual(result, VectorNext(VectorBegin(liste))));
	_ASSERTE(LCompareEqual(result, VectorLast(liste)));

	result = VectorBegin(liste);
	result = VectorInsert(liste, result, CastAny(Pointer,0x1));
	_ASSERTE(LCompareEqual(result, VectorBegin(liste)));
	_ASSERTE(LCompareEqual(result, VectorPrev(VectorPrev(VectorLast(liste)))));

	result = VectorBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(VectorGetData(result) == CastAny(Pointer,++cnt));
		result = VectorNext(result);
	}

	result = VectorBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERT(VectorGetData(result) == CastAny(Pointer,++cnt));
		VectorRemove(result, FreeListData);
		result = VectorBegin(liste);
	}

	VectorClose(liste, FreeListData);

	/* AVLBinaryTree */
	liste = AVLBinaryTreeOpen();

	result = AVLBinaryTreeAppend(liste, result, CastAny(Pointer,0x2));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreeBegin(liste)));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreeLast(liste)));

	result = AVLBinaryTreeAppend(liste, result, CastAny(Pointer,0x3));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreeNext(AVLBinaryTreeBegin(liste))));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreeLast(liste)));

	result = AVLBinaryTreeAppend(liste, result, CastAny(Pointer,0x4));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreeNext(AVLBinaryTreeNext(AVLBinaryTreeBegin(liste)))));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreeLast(liste)));

	result = AVLBinaryTreeBegin(liste);
	result = AVLBinaryTreeInsert(liste, result, CastAny(Pointer,0x1));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreeBegin(liste)));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreePrev(AVLBinaryTreePrev(AVLBinaryTreePrev(AVLBinaryTreeLast(liste))))));

	result = AVLBinaryTreeInsert(liste, result, CastAny(Pointer,0x0));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreeBegin(liste)));
	_ASSERTE(LCompareEqual(result, AVLBinaryTreePrev(AVLBinaryTreePrev(AVLBinaryTreePrev(AVLBinaryTreePrev(AVLBinaryTreeLast(liste)))))));

	result = AVLBinaryTreeBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERTE(AVLBinaryTreeGetData(result) == CastAny(Pointer,cnt++));
		result = AVLBinaryTreeNext(result);
	}

	result = AVLBinaryTreeBegin(liste);
	cnt = 0;
	while ( !LPtrCheck(result) )
	{
		_ASSERTE(AVLBinaryTreeGetData(result) == CastAny(Pointer,cnt++));
		AVLBinaryTreeRemove(result, FreeListData);
		result = AVLBinaryTreeBegin(liste);
	}

	AVLBinaryTreeClose(liste, FreeListData);

	_CrtMemDumpAllObjectsSince(&memState);

	_ASSERTE( _CrtCheckMemory( ) );
}

#endif
