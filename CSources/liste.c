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
#include "liste.h"
#include "strutil.h"

const LSearchResultType _lnull = {NULL, 0};

/*					*/
/* DoubleLinkedList */
/*					*/

struct _tagDoubleLinkedListHead;
typedef struct _tagDoubleLinkedListNode
{
	struct _tagDoubleLinkedListHead *head;
	struct _tagDoubleLinkedListNode *prev;
	struct _tagDoubleLinkedListNode *next;
	Pointer data;
} _DoubleLinkedListNode, *_pDoubleLinkedListNode;
#define szDoubleLinkedListNode sizeof(_DoubleLinkedListNode)

typedef struct _tagDoubleLinkedListHead
{
	struct _tagDoubleLinkedListNode *head;
	TListCnt nodeCount;
} _DoubleLinkedListHead, *_pDoubleLinkedListHead;
#define szDoubleLinkedListHead sizeof(_DoubleLinkedListHead)

Pointer __stdcall 
DoubleLinkedListOpen(DECL_FILE_LINE0)
{
#ifdef __DEBUG__
	Pointer p = TFallocDbg(szDoubleLinkedListHead, file, line);
	_pDoubleLinkedListNode pNode = CastAnyPtr(_DoubleLinkedListNode, TFallocDbg(szDoubleLinkedListNode, file, line));
#else
	Pointer p = TFalloc(szDoubleLinkedListHead);
	_pDoubleLinkedListNode pNode = CastAnyPtr(_DoubleLinkedListNode, TFalloc(szDoubleLinkedListNode));
#endif
	if ( PtrCheck(p) )
		return NULL;
	CastAnyPtr(_DoubleLinkedListHead, p)->head = pNode;
	pNode->head = CastAnyPtr(_DoubleLinkedListHead, p);
	pNode->prev = pNode;
	pNode->next = pNode;
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
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead, liste);
	_pDoubleLinkedListNode node = head->head->next;
	_pDoubleLinkedListNode node1;

	assert(liste != NULL);
	while (node != head->head)
	{
		node1 = node->next;
		if (PtrCheck(freeFunc))
			TFfree(node->data);
		else
			freeFunc(node->data, context);
		TFfree(node);
		node = node1;
	}
	TFfree(head->head);
	TFfree(head);
}

void __stdcall
DoubleLinkedListClear(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead, liste);
	_pDoubleLinkedListNode node = head->head->next;
	_pDoubleLinkedListNode node1;

	assert(liste != NULL);
	while (node != head->head)
	{
		node1 = node->next;
		if (PtrCheck(freeFunc))
			TFfree(node->data);
		else
			freeFunc(node->data, context);
		TFfree(node);
		node = node1;
	}
	head->head->next = head->head;
	head->head->prev = head->head;
}

LSearchResultType __stdcall
DoubleLinkedListIndex(Pointer liste, TListIndex index)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->head->next;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	if ( (index < 0) || (index > Cast(TListIndex, head->nodeCount)) )
		return result;
	while ((node != head->head) && (index > 0))
	{
		--index;
		node = node->next;
	}
	_Lnode(result) = node;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListBegin(Pointer liste)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->head->next;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	if (node == head->head)
		return result;
	_Lnode(result) = node;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListNext(LSearchResultType _node)
{
	_pDoubleLinkedListNode node = CastAny(_pDoubleLinkedListNode,_Lnode(_node));
	_pDoubleLinkedListHead head = node->head;
	LSearchResultType result = _LNULL;

	assert(node != NULL);
	if (node == head->head)
		return result;
	node = node->next;
	_Lnode(result) = node;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListPrev(LSearchResultType _node)
{
	_pDoubleLinkedListNode node = CastAny(_pDoubleLinkedListNode,_Lnode(_node));
	_pDoubleLinkedListHead head = node->head;
	LSearchResultType result = _LNULL;

	assert(node != NULL);
	node = node->prev;
	if (node == head->head)
		return result;
	_Lnode(result) = node;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListLast(Pointer liste)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead, liste);
	_pDoubleLinkedListNode node = head->head->prev;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	if (node == head->head)
		return result;
	_Lnode(result) = node;
	return result;
}

bool __stdcall 
DoubleLinkedListForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->head->next;
	bool result = true;

	assert(liste != NULL);
	assert(func != NULL);
	while (node != head->head)
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
DoubleLinkedListFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->head->next;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while (node != head->head)
	{
		if ( 0 == findFunc(node->data, data, context) )
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->head->next;
	LSearchResultType result = _LNULL;
	sword cmp;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	while (node != head->head)
	{
		cmp = sortFunc(node->data, data, context);
		if (0 <= cmp)
		{
			if (0 == cmp)
				_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->head->next;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (head->nodeCount == 0)
		return result;
	while (node != head->head)
	{
		if (0 < sortFunc(node->data, data, context))
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	if (node == head->head)
		_Lnode(result) = head->head;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode node = head->head->next;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (head->nodeCount == 0)
		return result;
	while (node != head->head)
	{
		if (0 <= sortFunc(node->data, data, context))
		{
			_Lnode(result) = node;
			break;
		}
		node = node->next;
	}
	if (node == head->head)
		_Lnode(result) = head->head;
	return result;
}

void __stdcall 
DoubleLinkedListSort(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead,liste);
	_pDoubleLinkedListNode nodeIn = NULL;
	_pDoubleLinkedListNode nodeIn1 = NULL;
	_DoubleLinkedListNode nodeOutHead;
	_pDoubleLinkedListNode nodeOut = NULL;
	_pDoubleLinkedListNode nodeOut1 = NULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (head->head->next == head->head)
		return;
	head->head->prev->next = NULL;
	nodeIn = head->head->next;
	s_memset(&nodeOutHead, 0, szDoubleLinkedListNode);
	nodeOut = &nodeOutHead;
	for ( ; NotPtrCheck(nodeIn); nodeIn = nodeIn1 )
	{
		nodeIn1 = nodeIn->next;
		for ( nodeOut1 = nodeOut; NotPtrCheck(nodeOut1->next); nodeOut1 = nodeOut1->next )
			if (0 < sortFunc(nodeOut1->next->data, nodeIn->data, context))
				break;
		nodeIn->next = nodeOut1->next;
		nodeOut1->next = nodeIn;
	}
	head->head->next = nodeOut->next;
	nodeIn = head->head->next;
	nodeIn1 = head->head;
	while ( NotPtrCheck(nodeIn) )
	{
		nodeIn->prev = nodeIn1;
		nodeIn1 = nodeIn;
		if (PtrCheck(nodeIn->next))
		{
			head->head->prev = nodeIn;
			nodeIn->next = head->head;
			break;
		}
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
	node2->prev = head->head->prev;
	node2->prev->next = node2;
	node2->next = head->head;
	head->head->prev = node2;
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
	node2->next = head->head->next;
	node2->next->prev = node2;
	node2->prev = head->head;
	head->head->next = node2;
	(head->nodeCount)++;
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListInsertBefore(LSearchResultType _node, ConstPointer data)
{
	_pDoubleLinkedListNode node = CastAny(_pDoubleLinkedListNode,_Lnode(_node));
	_pDoubleLinkedListHead head = node->head;
	_pDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(node != NULL);
	node2 = CastAny(_pDoubleLinkedListNode,TFalloc(szDoubleLinkedListNode));
	if ( PtrCheck(node2) )
		return result;

	node2->data = CastMutable(Pointer, data);
	node2->head = head;
	node2->next = node;
	node2->prev = node->prev;
	node2->prev->next = node2;
	node2->next->prev = node2;
	(head->nodeCount)++;
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListInsertAfter(LSearchResultType _node, ConstPointer data)
{
	_pDoubleLinkedListNode node = CastAny(_pDoubleLinkedListNode, _Lnode(_node));
	_pDoubleLinkedListHead head = node->head;
	_pDoubleLinkedListNode node2;
	LSearchResultType result = _LNULL;

	assert(node != NULL);
	node2 = CastAny(_pDoubleLinkedListNode,TFalloc(szDoubleLinkedListNode));
	if ( PtrCheck(node2) )
		return result;

	node2->data = CastMutable(Pointer, data);
	node2->head = head;
	node2->prev = node;
	node2->next = node->next;
	node2->prev->next = node2;
	node2->next->prev = node2;
	(head->nodeCount)++;
	_Lnode(result) = node2;
	return result;
}

LSearchResultType __stdcall
DoubleLinkedListRemove(LSearchResultType _node, TDeleteFunc freeFunc, Pointer context)
{
	_pDoubleLinkedListNode node = CastAny(_pDoubleLinkedListNode,_Lnode(_node));
	_pDoubleLinkedListHead head = node->head;
	LSearchResultType result = _LNULL;

	assert(NotPtrCheck(node));
	if (node == head->head)
		return result;
	if (node->next == head->head)
		_Lnode(result) = node->prev;
	else
		_Lnode(result) = node->next;
	node->next->prev = node->prev;
	node->prev->next = node->next;
	if ( PtrCheck(freeFunc) )
		TFfree(node->data);
	else
		freeFunc(node->data, context);
	TFfree(node);
	(head->nodeCount)--;
	return result;
}

LSearchResultType __stdcall 
DoubleLinkedListInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pDoubleLinkedListHead head = CastAny(_pDoubleLinkedListHead, liste);

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (head->nodeCount == 0)
		return DoubleLinkedListAppend(liste, data);
	return DoubleLinkedListInsertBefore(DoubleLinkedListUpperBound(liste, data, sortFunc, context), data);
}

bool __stdcall 
DoubleLinkedListRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext)
{
	LSearchResultType result = _LNULL;
	Pointer key;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	result = DoubleLinkedListFindSorted(liste, data, sortFunc, sortContext);
	if (LPtrCheck(result))
		return false;
	key = DoubleLinkedListGetData(result);
	while (NotPtrCheck(key) && (sortFunc(key, data, sortContext) == 0))
	{
		result = DoubleLinkedListRemove(result, freeFunc, freeContext);
		key = DoubleLinkedListGetData(result);
	}
	return true;
}

Pointer __stdcall 
DoubleLinkedListGetData(LSearchResultType _node)
{
	_pDoubleLinkedListNode node = CastAny(_pDoubleLinkedListNode,_Lnode(_node));
	_pDoubleLinkedListHead head = node->head;

	if (PtrCheck(node) || (node == head->head))
		return NULL;
	return node->data;
}

void __stdcall 
DoubleLinkedListSetData(LSearchResultType _node, ConstPointer data)
{
	_pDoubleLinkedListNode node = CastAny(_pDoubleLinkedListNode, _Lnode(_node));
	_pDoubleLinkedListHead head = node->head;

	if (PtrCheck(node) || (node == head->head))
		return;
	node->data = CastMutable(Pointer, data);
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
	Pointer p = TFallocDbg(szArrayHead + ((max + 1) * szPointer), file, line);
#else
	Pointer p = TFalloc(szArrayHead + ((max + 1) * szPointer));
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
	_pArrayHead head = CastAny(_pArrayHead, liste);

	assert(liste != NULL);
	return head->cnt;
}

TListCnt __stdcall
ArrayMaximum(Pointer liste)
{
	_pArrayHead head = CastAny(_pArrayHead, liste);

	assert(liste != NULL);
	return head->max;
}

void __stdcall
ArrayClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead, liste);
	Array data = CastAny(Array, _l_ptradd(head, szArrayHead));
	Pointer d;
	TListCnt ix;

	assert(liste != NULL);
	for (ix = 0; ix < head->cnt; ++ix, data = CastAny(Array, _l_ptradd(data, szPointer)))
	{
		d = DerefPtr(Pointer, data);
		if (PtrCheck(freeFunc))
			TFfree(d);
		else
			freeFunc(d, context);
	}
	TFfree(head);
}

void __stdcall
ArrayClear(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead, liste);
	Array data = CastAny(Array, _l_ptradd(head, szArrayHead));
	Pointer d;
	TListCnt ix;

	assert(liste != NULL);
	for (ix = 0; ix < head->cnt; ++ix, data = CastAny(Array, _l_ptradd(data, szPointer)))
	{
		d = DerefPtr(Pointer, data);
		if (PtrCheck(freeFunc))
			TFfree(d);
		else
			freeFunc(d, context);
		d = NULL;
	}
	head->cnt = 0;
}

LSearchResultType __stdcall
ArrayIndex(Pointer liste, TListIndex index)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (index < 0) || (index > Cast(TListIndex,head->cnt)) )
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
	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) < -1) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = head->cnt;
	if ( _Loffset(node) < Cast(TListIndex,head->cnt) )
		_Loffset(result) = _Loffset(node) + 1;
	return result;
}

LSearchResultType __stdcall 
ArrayPrev(LSearchResultType node)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
	LSearchResultType result = _LNULL;

	assert(head->max > 0);
	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) <= 0) || (_Loffset(node) > Cast(TListIndex, head->cnt)) )
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
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead));
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
ArrayFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead));
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(findFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_ulsearch( dataArray, data, head->cnt, findFunc, context, UTLPTR_MATCHMODE );
	if ( _Loffset(result) < 0 )
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
ArrayFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead));
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return _LNULL;
	_Lnode(result) = head;
	_Loffset(result) = _lv_ubsearch(dataArray, data, head->cnt, sortFunc, context, UTLPTR_MATCHMODE);
	if (_Loffset(result) < 0)
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
ArrayUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead));
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_ubsearch(dataArray, data, head->cnt, sortFunc, context, UTLPTR_INSERTMODE) + 1;
	return result;
}

LSearchResultType __stdcall 
ArrayLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(liste, szArrayHead));
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if ( 0 == head->cnt )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_ubsearch(dataArray, data, head->cnt, sortFunc, context, UTLPTR_SEARCHMODE);
	return result;
}

void __stdcall 
ArraySort(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context, word mode)
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
		_lv_uheapsort(dataArray, head->cnt, sortFunc, context);
		break;
	case LSORTMODE_QUICKSORT:
		_lv_uquicksort(dataArray, head->cnt, sortFunc, context);
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
	if (head->cnt >= head->max)
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataArray, Cast(sdword,head->cnt) - 1, data, &(head->cnt) );
	DerefPtr(Pointer, _fl_ptradd(dataArray, head->cnt * szPointer)) = NULL;
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
	DerefPtr(Pointer, _fl_ptradd(dataArray, head->cnt * szPointer)) = NULL;
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
	if ( (_Loffset(node) < 0) || ((head->cnt > 0) && (_Loffset(node) > Cast(TListIndex, head->cnt))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataArray, _Loffset(node) - 1, data, &(head->cnt) );
	DerefPtr(Pointer, _fl_ptradd(dataArray, head->cnt * szPointer)) = NULL;
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
	if ( (_Loffset(node) < -1) || ((head->cnt > 0) && (_Loffset(node) >= Cast(TListIndex, head->cnt))) )
		return result;
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataArray, _Loffset(node), data, &(head->cnt) );
	DerefPtr(Pointer, _fl_ptradd(dataArray, head->cnt * szPointer)) = NULL;
	return result;
}

LSearchResultType __stdcall
ArrayRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,_Lnode(node));
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );
	Pointer data;
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return result;
	data = DerefPtr(Pointer,_l_ptradd(dataArray, _Loffset(node) * szPointer));
	if ( PtrCheck(freeFunc) )
		TFfree( data );
	else
		freeFunc( data, context );
	_Lnode(result) = head;
	_Loffset(result) = _lv_delete(dataArray, _Loffset(node), &(head->cnt));
	DerefPtr(Pointer, _fl_ptradd(dataArray, head->cnt * szPointer)) = NULL;
	return result;
}

LSearchResultType __stdcall 
ArrayInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if (0 == head->cnt)
		return ArrayAppend(liste, data);
	return ArrayInsertBefore(ArrayUpperBound(liste, data, sortFunc, context), data);
}

bool __stdcall 
ArrayRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext)
{
	_pArrayHead head = CastAny(_pArrayHead,liste);
	Array dataArray = CastAny(Array, _l_ptradd(head, szArrayHead) );
	LSearchResultType result = _LNULL;
	Pointer key;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	result = ArrayFindSorted(liste, data, sortFunc, sortContext);
	if (LPtrCheck(result))
		return false;
	key = ArrayGetData(result);
	while (NotPtrCheck(key) && (sortFunc(key, data, sortContext) == 0))
	{
		result = ArrayRemove(result, freeFunc, freeContext);
		key = ArrayGetData(result);
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
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
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
	_pVectorHead head;
#ifdef __DEBUG__
	Pointer p1 = TFallocDbg(szVectorHead, file, line);
	Pointer p2 = TFallocDbg((max + 1) * szPointer, file, line);
#else
	Pointer p1 = TFalloc(szVectorHead);
	Pointer p2 = TFalloc((max + 1) * szPointer);
#endif

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
	_pVectorHead head = CastAny(_pVectorHead, liste);

	assert(liste != NULL);
	return head->cnt;
}

TListCnt __stdcall
VectorMaximum(Pointer liste)
{
	_pVectorHead head = CastAny(_pVectorHead, liste);

	assert(liste != NULL);
	return head->max;
}

void __stdcall
VectorReserve(Pointer liste)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Pointer p;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(head->exp > 0);
	p = TFrealloc(head->data, ((head->max + head->exp + 1) * szPointer));
	if ( PtrCheck(p) )
		return;
	head->max += head->exp;
	head->data = CastAny(Array, p);
}

void __stdcall
VectorClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead, liste);
	Pointer d;

	assert(liste != NULL);
	while (head->cnt > 0)
	{
		--(head->cnt);
		d = DerefPtr(Pointer, _l_ptradd(head->data, (szPointer * head->cnt)));
		if (PtrCheck(freeFunc))
			TFfree(d);
		else
			freeFunc(d, context);
	}
	TFfree(head->data);
	TFfree(head);
}

void __stdcall
VectorClear(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead, liste);
	Pointer d;

	assert(liste != NULL);
	while (head->cnt > 0)
	{
		--(head->cnt);
		d = DerefPtr(Pointer, _l_ptradd(head->data, (szPointer * head->cnt)));
		if (PtrCheck(freeFunc))
			TFfree(d);
		else
			freeFunc(d, context);
		d = NULL;
	}
}

LSearchResultType __stdcall
VectorIndex(Pointer liste, TListIndex index)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (index < 0) || (index > Cast(TListIndex,head->cnt)) )
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

	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) < -1) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return result;
	assert(head->max > 0);
	_Lnode(result) = head;
	_Loffset(result) = head->cnt;
	if ( _Loffset(node) < Cast(TListIndex,head->cnt) )
		_Loffset(result) = _Loffset(node) + 1;
	return result;
}

LSearchResultType __stdcall 
VectorPrev(LSearchResultType node)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	LSearchResultType result = _LNULL;

	if ( LPtrCheck(node) || (0 == head->cnt) || (_Loffset(node) <= 0) || (_Loffset(node) > Cast(TListIndex, head->cnt)) )
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
		if (!(func(*pt, context)))
		{
			result = false;
			break;
		}
	}
	return result;
}

LSearchResultType __stdcall 
VectorFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
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
VectorFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
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
	_Loffset(result) = _lv_ubsearch(dataVector, data, head->cnt, sortFunc, context, UTLPTR_MATCHMODE);
	if ( _Loffset(result) < 0 )
		return _LNULL;
	return result;
}

LSearchResultType __stdcall 
VectorUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
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
	_Loffset(result) = _lv_ubsearch( dataVector, data, head->cnt, sortFunc, context, UTLPTR_INSERTMODE) + 1;
	return result;
}

LSearchResultType __stdcall 
VectorLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
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
	_Loffset(result) = _lv_ubsearch( dataVector, data, head->cnt, sortFunc, context, UTLPTR_SEARCHMODE);
	return result;
}

void __stdcall 
VectorSort(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context, word mode)
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
	DerefPtr(Pointer, _fl_ptradd(dataVector, head->cnt * szPointer)) = NULL;
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
	DerefPtr(Pointer, _fl_ptradd(dataVector, head->cnt * szPointer)) = NULL;
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
	DerefPtr(Pointer, _fl_ptradd(dataVector, head->cnt * szPointer)) = NULL;
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
	DerefPtr(Pointer, _fl_ptradd(dataVector, head->cnt * szPointer)) = NULL;
	return result;
}

LSearchResultType __stdcall
VectorRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead,_Lnode(node));
	Array dataVector = head->data;
	Pointer data;
	LSearchResultType result = _LNULL;

	assert(head != NULL);
	assert(head->max > 0);
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
		return result;
	data = DerefPtr(Pointer,_l_ptradd(dataVector, _Loffset(node) * szPointer));
	if ( PtrCheck(freeFunc) )
		TFfree( data );
	else
		freeFunc( data, context );
	_Lnode(result) = head;
	_Loffset(result) = _lv_delete( dataVector, _Loffset(node), &(head->cnt) );
	DerefPtr(Pointer, _fl_ptradd(dataVector, head->cnt * szPointer)) = NULL;
	return result;
}

LSearchResultType __stdcall 
VectorInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	if (0 == head->cnt)
		return VectorAppend(liste, data);
	return VectorInsertBefore(VectorUpperBound(liste, data, sortFunc, context), data);
}

bool __stdcall 
VectorRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;
	Pointer key;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(sortFunc != NULL);
	result = VectorFindSorted(liste, data, sortFunc, sortContext);
	if (LPtrCheck(result))
		return false;
	key = VectorGetData(result);
	while (NotPtrCheck(key) && (sortFunc(key, data, sortContext) == 0))
	{
		result = VectorRemove(result, freeFunc, freeContext);
		key = VectorGetData(result);
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
	if ( (0 == head->cnt) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, head->cnt)) )
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
	struct _tagAVLBinaryTreeHead *head;
	struct _tagAVLBinaryTreeNode *parent;
	struct _tagAVLBinaryTreeNode *left;
	struct _tagAVLBinaryTreeNode *right;
	TListCnt height;
	Pointer data;
} _AVLBinaryTreeNode, *_pAVLBinaryTreeNode;
#define szAVLBinaryTreeNode sizeof(_AVLBinaryTreeNode)

typedef struct _tagAVLBinaryTreeHead
{
	struct _tagAVLBinaryTreeNode *root;
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
	while (!LPtrCheck(result))
	{
		d = AVLBinaryTreeGetData(result);
		if (PtrCheck(freeFunc))
			TFfree(d);
		else
			freeFunc(d, context);
		result = AVLBinaryTreeNext(result);
	}
	if (PtrCheck(liste))
		return;
	if (NotPtrCheck(head->root))
	{
		AVLBinaryTreeCloseHelper(head->root);
		TFfree(head->root);
	}
	TFfree(head);
}

void __stdcall
AVLBinaryTreeClear(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = AVLBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	while (!LPtrCheck(result))
	{
		d = AVLBinaryTreeGetData(result);
		if (PtrCheck(freeFunc))
			TFfree(d);
		else
			freeFunc(d, context);
		result = AVLBinaryTreeNext(result);
	}
	if (PtrCheck(liste))
		return;
	if (NotPtrCheck(head->root))
	{
		AVLBinaryTreeCloseHelper(head->root);
		TFfree(head->root);
	}
	head->nodeCount = 0;
	head->root = NULL;
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
AVLBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
{
	LSearchResultType result = AVLBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while ( !LPtrCheck(result) )
	{
		d = AVLBinaryTreeGetData(result);
		if ( 0 == findFunc(d, data, context) )
			return result;
		result = AVLBinaryTreeNext(result);
	}
	return _LNULL;
}

static _pAVLBinaryTreeNode __stdcall
AVLBinaryTreeFindSortedHelper(_pAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	sword funcResult = sortFunc(node->data, data, context);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return AVLBinaryTreeFindSortedHelper(node->left, data, sortFunc, context);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return AVLBinaryTreeFindSortedHelper(node->right, data, sortFunc, context);
}

LSearchResultType __stdcall 
AVLBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = AVLBinaryTreeFindSortedHelper(head->root, data, sortFunc, context);
	return result;
}

static _pAVLBinaryTreeNode __stdcall
AVLBinaryTreeUpperBoundHelper(_pAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	sword funcResult = sortFunc(node->data, data, context);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return AVLBinaryTreeUpperBoundHelper(node->left, data, sortFunc, context);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return AVLBinaryTreeUpperBoundHelper(node->right, data, sortFunc, context);
}

LSearchResultType __stdcall 
AVLBinaryTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = AVLBinaryTreeUpperBoundHelper(head->root, data, sortFunc, context);
	return result;
}

static _pAVLBinaryTreeNode __stdcall
AVLBinaryTreeLowerBoundHelper(_pAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	sword funcResult = sortFunc(node->data, data, context);

	if ( 0 == funcResult )
		return node;
	if ( 0 < funcResult )
	{
		if ( PtrCheck(node->left) )
			return NULL;
		return AVLBinaryTreeLowerBoundHelper(node->left, data, sortFunc, context);
	}
	if ( PtrCheck(node->right) )
		return NULL;
	return AVLBinaryTreeLowerBoundHelper(node->right, data, sortFunc, context);
}

LSearchResultType __stdcall 
AVLBinaryTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(head->root) )
		return result;
	_Lnode(result) = AVLBinaryTreeLowerBoundHelper(head->root, data, sortFunc, context);
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
AVLBinaryTreeInsertSortedHelper(_pAVLBinaryTreeHead head, _pAVLBinaryTreeNode node, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, _pAVLBinaryTreeNode* result)
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
	cmp = sortFunc(node->data, data, context);
	if (cmp == 0)
		return node;
	if (cmp > 0)
	{
		node->left = AVLBinaryTreeInsertSortedHelper(head, node->left, data, sortFunc, context, result);
		if (NotPtrCheck(node->left))
			node->left->parent = node;
	}
	else
	{
		node->right = AVLBinaryTreeInsertSortedHelper(head, node->right, data, sortFunc, context, result);
		if (NotPtrCheck(node->right))
			node->right->parent = node;
	}
	node->height = Max(AVLBinaryTreeHeightHelper(node->left), AVLBinaryTreeHeightHelper(node->right)) + 1;
	balance = AVLBinaryTreeBalance(node);
	if ((balance > 1) && (sortFunc(node->left->data, data, context) > 0))
		return AVLBinaryTreeRotateRight(node);
	if ((balance < -1) && (sortFunc(node->right->data, data, context) < 0))
		return AVLBinaryTreeRotateLeft(node);
	if ((balance > 1) && (sortFunc(node->left->data, data, context) < 0))
	{
		node->left = AVLBinaryTreeRotateLeft(node->left);
		return AVLBinaryTreeRotateRight(node);
	}
	if ((balance < -1) && (sortFunc(node->right->data, data, context) > 0))
	{
		node->right = AVLBinaryTreeRotateRight(node->right);
		return AVLBinaryTreeRotateLeft(node);
	}
	return node;
}

LSearchResultType __stdcall 
AVLBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
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
	head->root = AVLBinaryTreeInsertSortedHelper(head, node1, data, sortFunc, context, &node2);
	_Lnode(result) = node2;
	return result;
}

static _pAVLBinaryTreeNode __stdcall
AVLBinaryTreeRemoveSortedHelper(_pAVLBinaryTreeHead head, _pAVLBinaryTreeNode root, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext, bool freedata, bool *result)
{
	_pAVLBinaryTreeNode temp;
	sword cmp;
	sdword balance;

	if (root == NULL)
		return root;

	cmp = sortFunc(root->data, data, sortContext);
	if (cmp > 0)
	{
		root->left = AVLBinaryTreeRemoveSortedHelper(head, root->left, data, sortFunc, sortContext, freeFunc, freeContext, freedata, result);
		if (NotPtrCheck(root->left))
			root->left->parent = root;
	}
	else if (cmp < 0)
	{
		root->right = AVLBinaryTreeRemoveSortedHelper(head, root->right, data, sortFunc, sortContext, freeFunc, freeContext, freedata, result);
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
				freeFunc(root->data, freeContext);
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
			root->right = AVLBinaryTreeRemoveSortedHelper(head, root->right, temp->data, sortFunc, sortContext, freeFunc, freeContext, false, result);
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
AVLBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext)
{
	_pAVLBinaryTreeHead head = CastAnyPtr(_AVLBinaryTreeHead, liste);
	_pAVLBinaryTreeNode node = head->root;
	bool result = false;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if ( PtrCheck(node) )
		return false;
	head->root = AVLBinaryTreeRemoveSortedHelper(head, node, data, sortFunc, sortContext, freeFunc, freeContext, true, &result);
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
	struct _tagRBBinaryTreeHead *head;
	struct _tagRBBinaryTreeNode *parent;
	struct _tagRBBinaryTreeNode *left;
	struct _tagRBBinaryTreeNode *right;
	enum _RBBinaryTreeNodeColor color;
	Pointer data;
} _RBBinaryTreeNode, *_pRBBinaryTreeNode;
#define szRBBinaryTreeNode sizeof(_RBBinaryTreeNode)

typedef struct _tagRBBinaryTreeHead
{
	struct _tagRBBinaryTreeNode *root;
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

void __stdcall
RBBinaryTreeClear(Pointer liste, TDeleteFunc freeFunc, Pointer context)
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
	head->nodeCount = 0;
	head->root = 0;
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
RBBinaryTreeFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
{
	LSearchResultType result = RBBinaryTreeBegin(liste);
	Pointer d;

	assert(liste != NULL);
	assert(findFunc != NULL);
	while (!LPtrCheck(result))
	{
		d = RBBinaryTreeGetData(result);
		if (0 == findFunc(d, data, context))
			return result;
		result = RBBinaryTreeNext(result);
	}
	return _LNULL;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeFindSortedHelper(_pRBBinaryTreeNode node, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	sword funcResult = sortFunc(node->data, data, context);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return RBBinaryTreeFindSortedHelper(node->left, data, sortFunc, context);
	}
	if (PtrCheck(node->right))
		return NULL;
	return RBBinaryTreeFindSortedHelper(node->right, data, sortFunc, context);
}

LSearchResultType __stdcall
RBBinaryTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = RBBinaryTreeFindSortedHelper(head->root, data, sortFunc, context);
	return result;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeUpperBoundHelper(_pRBBinaryTreeNode node, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	sword funcResult = sortFunc(node->data, data, context);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return RBBinaryTreeUpperBoundHelper(node->left, data, sortFunc, context);
	}
	if (PtrCheck(node->right))
		return NULL;
	return RBBinaryTreeUpperBoundHelper(node->right, data, sortFunc, context);
}

LSearchResultType __stdcall
RBBinaryTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = RBBinaryTreeUpperBoundHelper(head->root, data, sortFunc, context);
	return result;
}

static _pRBBinaryTreeNode __stdcall
RBBinaryTreeLowerBoundHelper(_pRBBinaryTreeNode node, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	sword funcResult = sortFunc(node->data, data, context);

	if (0 == funcResult)
		return node;
	if (0 < funcResult)
	{
		if (PtrCheck(node->left))
			return NULL;
		return RBBinaryTreeLowerBoundHelper(node->left, data, sortFunc, context);
	}
	if (PtrCheck(node->right))
		return NULL;
	return RBBinaryTreeLowerBoundHelper(node->right, data, sortFunc, context);
}

LSearchResultType __stdcall
RBBinaryTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (PtrCheck(head->root))
		return result;
	_Lnode(result) = RBBinaryTreeLowerBoundHelper(head->root, data, sortFunc, context);
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
RBBinaryTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
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
			sword cmp = sortFunc(n->data, data, context);

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
RBBinaryTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext)
{
	_pRBBinaryTreeHead head = CastAnyPtr(_RBBinaryTreeHead, liste);
	_pRBBinaryTreeNode child;
	_pRBBinaryTreeNode n = head->root;

	if (PtrCheck(n)) 
		return false;
	n = RBBinaryTreeFindSortedHelper(n, data, sortFunc, sortContext);
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
		freeFunc(n->data, freeContext);
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
	struct _tagBTreeHead *head;
	struct _tagBTreeNode *parent;
	Pointer key;
	TListCnt cnt;
	word isData;
	word isLastDataBlock;
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
	pNode = CastAnyPtr(_BTreeNode, TFallocDbg(szBTreeNode + ((maxEntriesPerNode + 1) * szPointer), file, line));
#else
	pNode = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + ((maxEntriesPerNode + 1) * szPointer)));
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
	pNode->isLastDataBlock = TRUE;

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
	TListCnt height = 1;

	assert(liste != NULL);
	assert(pNode != NULL);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		pNode = DerefAnyPtr(_pBTreeNode, _l_ptradd(pNode, szBTreeNode));
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

static void __stdcall
BTreeFreeHelper(_pBTreeNode pNode, TDeleteFunc freeFunc, Pointer context)
{
	Array p;
	Array p1;
	Pointer d;
	TListCnt ix;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if (pNode->isData)
	{
		for (ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
		{
			d = DerefPtr(Pointer, p1);
			if (PtrCheck(freeFunc))
				TFfree(d);
			else
				freeFunc(d, context);
		}
	}
	else
	{
		assert(pNode->cnt > 0);
		for (ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
			BTreeFreeHelper(DerefAnyPtr(_pBTreeNode, p1), freeFunc, context);
	}
}

void __stdcall
BTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);

	assert(liste != NULL);
	BTreeFreeHelper(pHead->root, freeFunc, context);
	BTreeCloseHelper(pHead->root);
	TFfree(pHead);
}

void __stdcall
BTreeClear(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode;

	assert(liste != NULL);
	BTreeFreeHelper(pHead->root, freeFunc, context);
	BTreeCloseHelper(pHead->root);
	pHead->nodeCount = 0;
	pHead->root = NULL;
	pNode = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + ((pHead->maxEntriesPerNode + 1) * szPointer)));
	pHead->root = pNode;
	pNode->head = pHead;
	pNode->isData = TRUE;
	pNode->isLastDataBlock = TRUE;
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
	else if (node->isLastDataBlock)
	{
		_Lnode(result) = node;
		_Loffset(result) = *index;
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
	if ( (0 == pHead->nodeCount) || (index < 0) || (index > Cast(TListIndex,pHead->nodeCount)) )
		return _LNULL;
	return BTreeIndexHelper(pNode, &index);
}

LSearchResultType __stdcall 
BTreeBegin(Pointer liste)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		pNode = DerefAnyPtr(_pBTreeNode, _l_ptradd(pNode, szBTreeNode));
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
	if (pNode->isLastDataBlock)
	{
		if ( _Loffset(node) < Cast(TListIndex,pNode->cnt) )
		{
			_Lnode(result) = _Lnode(node);
			_Loffset(result) = _Loffset(node) + 1;
		}
		return result;
	}
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

static bool __stdcall
BTreeForEachHelper(_pBTreeNode pNode, TForEachFunc func, Pointer context)
{
	Array p;
	Array p1;
	Pointer d;
	TListCnt ix;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if (pNode->isData)
	{
		for (ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
		{
			d = DerefPtr(Pointer, p1);
			if (0 == func(d, context))
				return false;
		}
	}
	else
	{
		assert(pNode->cnt > 0);
		for (ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
			if (!BTreeForEachHelper(DerefAnyPtr(_pBTreeNode, p1), func, context))
				return false;
	}
	return true;
}

bool __stdcall
BTreeForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(func != NULL);
	return BTreeForEachHelper(pNode, func, context);
}

static LSearchResultType __stdcall
BTreeFindHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
{
	LSearchResultType result = _LNULL;
	Array p;
	Array p1;
	Pointer d;
	TListCnt ix;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if (pNode->isData)
	{
		for (ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
		{
			d = DerefPtr(Pointer, p1);
			if (0 == findFunc(d, data, context))
			{
				_Lnode(result) = pNode;
				_Loffset(result) = ix;
				return result;
			}
		}
	}
	else
	{
		assert(pNode->cnt > 0);
		for (ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
		{
			result = BTreeFindHelper(DerefAnyPtr(_pBTreeNode, p1), data, findFunc, context);
			if (!LPtrCheck(result))
				return result;
		}
	}
	return result;
}

LSearchResultType __stdcall
BTreeFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(findFunc != NULL);
	return BTreeFindHelper(pNode, data, findFunc, context);
}

static LSearchResultType __stdcall 
BTreeFindSortedHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeNode pNode1;
	Array p;
	Array p1;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
		sdword ix = _lv_ubsearch(p, data, pNode->cnt, sortFunc, context, UTLPTR_MATCHMODE);

		if (ix < 0)
		{
			ix = _lv_ubsearch(p, data, pNode->cnt, sortFunc, context, UTLPTR_SEARCHMODE);
			if ((!(pNode->isLastDataBlock)) && (ix >= Cast(TListIndex, pNode->cnt)))
			{
				Pointer d;

				_Lnode(result) = pNode;
				_Loffset(result) = ix;
				result = BTreeNext(result);
				d = BTreeGetData(result);
				if (NotPtrCheck(d) && (sortFunc(d, data, context) == 0))
					return result;
				result = _LNULL;
			}
			return result;
		}
		_Lnode(result) = pNode;
		_Loffset(result) = ix;
		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
		if (((ix + 1) == pNode->cnt) || (0 <= sortFunc(pNode1->key, data, context)))
			return BTreeFindSortedHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc, context);
	}
	return result;
}

LSearchResultType __stdcall 
BTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return BTreeFindSortedHelper(pNode, data, sortFunc, context);
}

static LSearchResultType __stdcall 
BTreeUpperBoundHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeNode pNode1;
	Array p;
	Array p1;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
		if (pNode->cnt == 0)
			return result;
		_Lnode(result) = pNode;
		_Loffset(result) = _lv_ubsearch(p, data, pNode->cnt, sortFunc, context, UTLPTR_INSERTMODE) + 1;

		if ((!(pNode->isLastDataBlock)) && (_Loffset(result) >= Cast(TListIndex, pNode->cnt)))
			return BTreeNext(result);

		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
		if (((ix + 1) == pNode->cnt) || (0 < sortFunc(pNode1->key, data, context)))
			return BTreeUpperBoundHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc, context);
	}
	return result;
}

LSearchResultType __stdcall 
BTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return BTreeUpperBoundHelper(pNode, data, sortFunc, context);
}

static LSearchResultType __stdcall 
BTreeLowerBoundHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeNode pNode1;
	Array p;
	Array p1;
    TListCnt ix;
	LSearchResultType result = _LNULL;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if ( pNode->isData )
	{
		sdword ix = _lv_ubsearch(p, data, pNode->cnt, sortFunc, context, UTLPTR_SEARCHMODE);

		if ( ix < 0 )
			return result;
		_Lnode(result) = pNode;
		_Loffset(result) = ix;

		if ((!(pNode->isLastDataBlock)) && (ix >= Cast(TListIndex, pNode->cnt)))
			return BTreeNext(result);

		return result;
	}
	assert(pNode->cnt > 0);
    for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)) )
	{
		pNode1 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p1, szPointer));
		if (((ix + 1) == pNode->cnt) || (0 <= sortFunc(pNode1->key, data, context)))
			return BTreeLowerBoundHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc, context);
	}
	return result;
}

LSearchResultType __stdcall 
BTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return BTreeLowerBoundHelper(pNode, data, sortFunc, context);
}

static void __stdcall
BTreeSortHelper(_pBTreeNode node, _pBTreeHead pHead)
{
	Array p;
	TListCnt ix;

	node->head = pHead;
	if (!(node->isData))
		for (ix = 0, p = CastAny(Array, _l_ptradd(node, szBTreeNode)); ix < node->cnt; ++ix, p = CastAny(Array, _l_ptradd(p, szPointer)))
			BTreeSortHelper(DerefAnyPtr(_pBTreeNode, p), pHead);
}

void __stdcall
BTreeSort(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	Pointer liste2 = NULL;
	Pointer data;
	LSearchResultType it = _LNULL;
	
	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);

	liste2 = BTreeOpen(__FILE__LINE__ pHead->maxEntriesPerNode);
	assert(liste2 != NULL);
	it = BTreeBegin(liste);
	data = BTreeGetData(it);
	while (NotPtrCheck(data))
	{
		BTreeInsertSorted(liste2, data, sortFunc, context);
		it = BTreeNext(it);
		data = BTreeGetData(it);
	}
	BTreeCloseHelper(pHead->root);
	pHead->root = CastAnyPtr(_BTreeHead, liste2)->root;
	BTreeSortHelper(pHead->root, pHead);
	TFfree(liste2);
}

static _pBTreeNode __stdcall
BTreeAppendPrependHelper(_pBTreeNode pNode, ConstPointer data, bool bAppend, Ptr(LSearchResultType) pResult)
{
	_pBTreeNode pNode1 = NULL;
	_pBTreeNode pNode2;
	Array p;
	Array p1;
	TListCnt ix;
	dword cnt;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if (pNode->isData)
	{
		sdword ix = _lv_insert(p, bAppend ? (Cast(sdword, pNode->cnt) - 1) : -1, data, &(pNode->cnt));

		DerefPtr(Pointer, _fl_ptradd(p, pNode->cnt * szPointer)) = NULL;

		_LPnode(pResult) = pNode;
		_LPoffset(pResult) = ix;
		if (ix == 0)
		{
			pNode1 = pNode;
			pNode1->key = DerefPtr(Pointer, _l_ptradd(pNode1, szBTreeNode));
			while (NotPtrCheck(pNode1->parent))
			{
				pNode2 = pNode1->parent;
				ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
				assert(ix >= 0);
				if (ix != 0)
					break;
				pNode2->key = pNode1->key;
				pNode1 = pNode1->parent;
			}
		}
		++(pNode->head->nodeCount);
		if (pNode->cnt >= pNode->head->maxEntriesPerNode)
		{
			pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + ((pNode->head->maxEntriesPerNode + 1) * szPointer)));
			pNode2->head = pNode->head;
			p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			cnt = bAppend ? 1 : (pNode->cnt - 1);
			pNode->cnt -= cnt;
			s_memcpy(p1, _l_ptradd(p, pNode->cnt * szPointer), cnt * szPointer);
			pNode2->cnt += cnt;
			pNode2->key = DerefAnyPtr(Pointer, p1);
			pNode2->isData = TRUE;
			if (pNode->isLastDataBlock)
			{
				pNode->isLastDataBlock = FALSE;
				pNode2->isLastDataBlock = TRUE;
			}
			if (_LPoffset(pResult) >= Cast(TListIndex, pNode->cnt))
			{
				_LPnode(pResult) = pNode2;
				_LPoffset(pResult) -= pNode->cnt;
			}
			return pNode2;
		}
		return NULL;
	}
	assert(pNode->cnt > 0);
	ix = bAppend ? (pNode->cnt - 1) : 0;
	p1 = CastAny(Array, _l_ptradd(p, ix * szPointer));
	pNode1 = BTreeAppendPrependHelper(DerefAnyPtr(_pBTreeNode, p1), data, bAppend, pResult);
	if (PtrCheck(pNode1))
		return NULL;
	pNode1->parent = pNode;
	_lv_insert(p, ix, pNode1, &(pNode->cnt));
	if (pNode->cnt >= pNode->head->maxEntriesPerNode)
	{
		pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + ((pNode->head->maxEntriesPerNode + 1) * szPointer)));
		pNode2->head = pNode->head;
		p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
		cnt = bAppend ? 1 : (pNode->cnt - 1);
		pNode->cnt -= cnt;
		s_memcpy(p1, _l_ptradd(p, pNode->cnt * szPointer), cnt * szPointer);
		pNode2->cnt += cnt;
		pNode1 = DerefAnyPtr(_pBTreeNode, p1);
		pNode2->key = pNode1->key;
		for (ix = 0; ix < pNode2->cnt; ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
		{
			pNode1 = DerefAnyPtr(_pBTreeNode, p1);
			pNode1->parent = pNode2;
		}
		return pNode2;
	}
	return NULL;
}

static LSearchResultType __stdcall
BTreeAppendPrependHelper2(Pointer liste, ConstPointer data, bool bAppend)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	_pBTreeNode pNode2;
	Array p1;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = BTreeAppendPrependHelper(pNode, data, bAppend, &result);
	if (PtrCheck(pNode))
		return result;
	pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + ((pHead->maxEntriesPerNode + 1) * szPointer)));
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

LSearchResultType __stdcall
BTreeAppend(Pointer liste, ConstPointer data)
{
	return BTreeAppendPrependHelper2(liste, data, true);
}

LSearchResultType __stdcall
BTreePrepend(Pointer liste, ConstPointer data)
{
	return BTreeAppendPrependHelper2(liste, data, false);
}

static LSearchResultType __stdcall
BTreeInsertHelper(LSearchResultType node, ConstPointer data, bool bbefore)
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
	ix = _lv_insert(p, bbefore ? (ix - 1) : ix, data, &(pNode->cnt));
	DerefPtr(Pointer, _fl_ptradd(p, pNode->cnt * szPointer)) = NULL;
	if (ix == 0)
	{
		pNode1 = pNode;
		pNode1->key = DerefPtr(Pointer, p);
		while (NotPtrCheck(pNode1->parent))
		{
			pNode2 = pNode1->parent;
			ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
			assert(ix >= 0);
			if (ix != 0)
				break;
			pNode2->key = pNode1->key;
			pNode1 = pNode1->parent;
		}
	}
	if (!bbefore)
		++(_Loffset(node));
	++(pHead->nodeCount);
	pNode2 = NULL;
	if (pNode->cnt >= pHead->maxEntriesPerNode)
	{
		pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + ((pHead->maxEntriesPerNode + 1) * szPointer)));
		pNode2->head = pHead;
		pNode2->parent = pNode->parent;
		p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
		cnt = pNode->cnt / 2;
		pNode->cnt -= cnt;
		s_memcpy(p1, _l_ptradd(p, pNode->cnt * szPointer), cnt * szPointer);
		pNode2->cnt += cnt;
		pNode2->key = DerefAnyPtr(Pointer, p1);
		pNode2->isData = TRUE;
		if (pNode->isLastDataBlock)
		{
			pNode->isLastDataBlock = FALSE;
			pNode2->isLastDataBlock = TRUE;
		}
		if (_Loffset(node) >= Cast(TListIndex, pNode->cnt))
		{
			_Lnode(node) = pNode2;
			_Loffset(node) -= pNode->cnt;
		}
	}
	while (NotPtrCheck(pNode2) && NotPtrCheck(pNode2->parent))
	{
		pNode1 = pNode2->parent;
		p = CastAny(Array, _l_ptradd(pNode1, szBTreeNode));
		assert(pNode1->cnt > 0);
		ix = _lv_searchptr(p, pNode, pNode1->cnt);
		assert(ix >= 0);
		_lv_insert(p, ix, pNode2, &(pNode1->cnt));
		pNode = pNode1;
		pNode2 = NULL;
		if (pNode->cnt >= pHead->maxEntriesPerNode)
		{
			pNode2 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + ((pHead->maxEntriesPerNode + 1) * szPointer)));
			pNode2->head = pHead;
			pNode2->parent = pNode->parent;
			p1 = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			cnt = pNode->cnt / 2;
			pNode->cnt -= cnt;
			s_memcpy(p1, _l_ptradd(p, pNode->cnt * szPointer), cnt * szPointer);
			pNode2->cnt += cnt;
			pNode1 = DerefAnyPtr(_pBTreeNode, p1);
			pNode2->key = pNode1->key;
			for (ix = 0; ix < Cast(sdword, pNode2->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
			{
				pNode1 = DerefAnyPtr(_pBTreeNode, p1);
				pNode1->parent = pNode2;
			}
		}
	}
	if (NotPtrCheck(pNode2))
	{
		pNode1 = CastAnyPtr(_BTreeNode, TFalloc(szBTreeNode + ((pHead->maxEntriesPerNode + 1) * szPointer)));
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

LSearchResultType __stdcall
BTreeInsertBefore(LSearchResultType node, ConstPointer data)
{
	return BTreeInsertHelper(node, data, true);
}

LSearchResultType __stdcall
BTreeInsertAfter(LSearchResultType node, ConstPointer data)
{
	return BTreeInsertHelper(node, data, false);
}

static void __stdcall
BTreeRemoveKeyUpdateHelper(_pBTreeNode pNode)
{
	_pBTreeNode pNode1;
	_pBTreeNode pNode2;
	sdword ix;

	pNode1 = pNode;
	if (pNode1->isData)
		pNode1->key = DerefPtr(Pointer, _l_ptradd(pNode1, szBTreeNode));
	else
	{
		pNode2 = DerefPtr(_pBTreeNode, _l_ptradd(pNode1, szBTreeNode));
		pNode1->key = pNode2->key;
	}
	while (NotPtrCheck(pNode1->parent))
	{
		pNode2 = pNode1->parent;
		ix = _lv_searchptr(CastAny(Array, _l_ptradd(pNode2, szBTreeNode)), pNode1, pNode2->cnt);
		assert(ix >= 0);
		if (ix != 0)
			break;
		pNode2->key = pNode1->key;
		pNode1 = pNode2;
	}
}

static void __stdcall
BTreeRemoveJoinHelper(_pBTreeNode pNodeDest, _pBTreeNode pNodeSrc)
{
	Array pDest = CastAny(Array, _l_ptradd(pNodeDest, szBTreeNode));
	Array pSrc = CastAny(Array, _l_ptradd(pNodeSrc, szBTreeNode));
	_pBTreeNode pNode;
	sdword ix;

	pDest = CastAny(Array, _l_ptradd(pDest, pNodeDest->cnt * szPointer));
	s_memcpy_s(pDest, (pNodeDest->head->maxEntriesPerNode - pNodeDest->cnt) * szPointer, pSrc, pNodeSrc->cnt * szPointer);
	if (!(pNodeDest->isData))
	{
		for (ix = 0; ix < Castsdword(pNodeSrc->cnt); ++ix, pDest = CastAny(Array, _l_ptradd(pDest, szPointer)))
		{
			pNode = DerefAnyPtr(_pBTreeNode, pDest);
			pNode->parent = pNodeDest;
		}
	}
	else if (pNodeSrc->isLastDataBlock)
		pNodeDest->isLastDataBlock = TRUE;
	if (pNodeDest->cnt == 0)
		BTreeRemoveKeyUpdateHelper(pNodeDest);
	pNodeDest->cnt += pNodeSrc->cnt;
	TFfree(pNodeSrc);
}

static sword __stdcall
BTreeTestSortFunc(ConstPointer pa, ConstPointer pb)
{
	if (pa < pb)
		return -1;
	if (pa > pb)
		return 1;
	return 0;
}

static void __stdcall
BTreeSortedTester1(_pBTreeNode pNode, TSearchAndSortUserFunc sortFunc, Pointer context, bool bLast)
{
	_pBTreeNode pNode1;
	Pointer d1;
	Pointer d2;
	Array p;
	Array p1;
	sdword ix;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	if (pNode->isData)
	{
		if (bLast)
			assert(pNode->isLastDataBlock);
		else
			assert(!(pNode->isLastDataBlock));
		if (pNode->cnt > 0)
		{
			d1 = DerefAnyPtr(Pointer, p);
			d2 = pNode->key;
			assert(0 == sortFunc(d1, d2, context));
			for (ix = 1; ix < Cast(sdword, pNode->cnt); ++ix)
			{
				p = CastAny(Array, _l_ptradd(p, szPointer));
				d2 = DerefAnyPtr(Pointer, p);
				assert(0 >= sortFunc(d1, d2, context));
				d1 = d2;
			}
		}
	}
	else
	{
		assert(pNode->cnt > 0);
		for (ix = 0, p1 = p; ix < Castsdword(pNode->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
			BTreeSortedTester1(DerefAnyPtr(_pBTreeNode, p1), sortFunc, context, bLast && (ix == Castsdword(pNode->cnt - 1)));
		pNode1 = DerefAnyPtr(_pBTreeNode, p);
		d1 = pNode1->key;
		for (ix = 1, p1 = CastAny(Array, _l_ptradd(p, szPointer)); ix < Cast(sdword, pNode->cnt); ++ix, p1 = CastAny(Array, _l_ptradd(p1, szPointer)))
		{
			pNode1 = DerefAnyPtr(_pBTreeNode, p1);
			d2 = pNode1->key;
			assert(0 >= sortFunc(d1, d2, context));
			d1 = d2;
		}
	}
}

static void __stdcall
BTreeSortedTester(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	BTreeSortedTester1(pNode, sortFunc, context, true);
}

LSearchResultType __stdcall
BTreeRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	_pBTreeHead pHead;
	_pBTreeNode pNode1;
	_pBTreeNode pNode2;
	_pBTreeNode pNode3;
	_pBTreeNode pNode4;
	Array p;
	Pointer data1;
	sdword ix = _Loffset(node);
	LSearchResultType result = node;

	assert(NotPtrCheck(pNode));
	assert(pNode->cnt > 0);
	assert(ix < Castsdword(pNode->cnt));
	assert(pNode->isData);
	pHead = pNode->head;

	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode));
	data1 = DerefPtr(Pointer, _l_ptradd(p, ix * szPointer));
	if (freeFunc)
		freeFunc(data1, context);
	else
		TFfree(data1);
	_lv_delete(p, ix, &(pNode->cnt));
	DerefPtr(Pointer, _fl_ptradd(p, pNode->cnt * szPointer)) = NULL;
	if (pNode->cnt == 0)
	{
		if (pNode->isLastDataBlock)
			result = BTreePrev(result);
		else
			result = BTreeNext(result);
	}
	else if (ix == pNode->cnt)
	{
		if (pNode->isLastDataBlock)
			_Loffset(result) = ix - 1;
		else
			result = BTreeNext(result);
	}
	if ((ix == 0) && (pNode->cnt > 0))
		BTreeRemoveKeyUpdateHelper(pNode);
	--(pHead->nodeCount);
	if (pNode->cnt < (pHead->maxEntriesPerNode / 2))
	{
		pNode1 = pNode;
		while (NotPtrCheck(pNode1->parent))
		{
			pNode2 = pNode1->parent;
			p = CastAny(Array, _l_ptradd(pNode2, szBTreeNode));
			assert(pNode2->cnt > 0);
			ix = _lv_searchptr(p, pNode1, pNode2->cnt);
			assert(ix >= 0);
			pNode3 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p, ix * szPointer));
			assert(pNode1 == pNode3);
			if ((pNode2->cnt < 2) && (pNode3->cnt == 0))
			{
				if (pNode3->isLastDataBlock)
				{
					pNode4 = CastAnyPtr(_BTreeNode, _Lnode(result));
					pNode4->isLastDataBlock = TRUE;
				}
				TFfree(pNode3);
				_lv_delete(p, ix, &(pNode2->cnt));
			}
			else
			{
				pNode4 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p, (ix + 1) * szPointer));
				if ((ix < Castsdword((pNode2->cnt - 1))) && ((pNode3->cnt + pNode4->cnt) < pHead->maxEntriesPerNode))
				{
					if (_Lnode(result) == pNode4)
					{
						_Lnode(result) = pNode3;
						_Loffset(result) += pNode3->cnt;
					}
					BTreeRemoveJoinHelper(pNode3, pNode4);
					_lv_delete(p, ix + 1, &(pNode2->cnt));
				}
				else
				{
					pNode4 = DerefAnyPtr(_pBTreeNode, _l_ptradd(p, (ix - 1) * szPointer));
					if ((ix > 0) && ((pNode3->cnt + pNode4->cnt) < pHead->maxEntriesPerNode))
					{
						if (_Lnode(result) == pNode3)
						{
							_Lnode(result) = pNode4;
							_Loffset(result) += pNode4->cnt;
						}
						BTreeRemoveJoinHelper(pNode4, pNode3);
						_lv_delete(p, ix, &(pNode2->cnt));
					}
				}
			}
			pNode1 = pNode2;
		}
		assert(pNode1 == pHead->root);
		while ((!(pNode1->isData)) && (pNode1->cnt == 1))
		{
			pHead->root = DerefPtr(_pBTreeNode, _l_ptradd(pNode1, szBTreeNode));
			pHead->root->parent = NULL;
			TFfree(pNode1);
			pNode1 = pHead->root;
		}
	}
	return result;
}

LSearchResultType __stdcall
BTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	if (0 == pHead->nodeCount)
		result = BTreeAppend(liste, data);
	else
		result = BTreeInsertBefore(BTreeUpperBound(liste, data, sortFunc, context), data);
	//BTreeSortedTester(liste, sortFunc, context);
	return result;
}

bool __stdcall 
BTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext)
{
	LSearchResultType result = _LNULL;
	Pointer key;

	assert(liste != NULL);
	assert(sortFunc != NULL);
	result = BTreeFindSorted(liste, data, sortFunc, sortContext);
	if (LPtrCheck(result))
		return false;
	key = BTreeGetData(result);
	while (NotPtrCheck(key) && (sortFunc(key, data, sortContext) == 0))
	{
		result = BTreeRemove(result, freeFunc, freeContext);
		key = BTreeGetData(result);
	}
	//BTreeSortedTester(liste, sortFunc, sortContext);
	return true;
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

/*					*/
/* HashLinkedList */
/*					*/
struct _tagHashLinkedListHead;
struct _tagHashLinkedListBucketNode;
typedef struct _tagHashLinkedListNode
{
	struct _tagHashLinkedListBucketNode *bucket;
	struct _tagHashLinkedListNode *next;
	struct _tagHashLinkedListNode *prev;
	Pointer data;
} _HashLinkedListNode, *_pHashLinkedListNode;
#define szHashLinkedListNode sizeof(_HashLinkedListNode)

typedef struct _tagHashLinkedListBucketNode
{
	struct _tagHashLinkedListHead *head;
	struct _tagHashLinkedListNode *list;
	struct _tagHashLinkedListNode *lend;
	TListCnt listCount;
	sdword bucket;
} _HashLinkedListBucketNode, *_pHashLinkedListBucketNode;
#define szHashLinkedListBucketNode sizeof(_HashLinkedListBucketNode)

typedef struct _tagHashLinkedListHead
{
	struct _tagHashLinkedListBucketNode *buckets;
	THashFunc hfunc;
	Pointer hContext;
	TListCnt nodeCount;
	TListCnt nodeMax;
} _HashLinkedListHead, *_pHashLinkedListHead;
#define szHashLinkedListHead sizeof(_HashLinkedListHead)

Pointer __stdcall
HashLinkedListOpen(DECL_FILE_LINE TListCnt max, THashFunc hfunc, Pointer hContext)
{
#ifdef __DEBUG__
	Pointer p1 = TFallocDbg(szHashLinkedListHead, file, line);
	Pointer p2 = TFallocDbg(max * szHashLinkedListBucketNode, file, line);
#else
	Pointer p1 = TFalloc(szHashLinkedListHead);
	Pointer p2 = TFalloc(max * szHashLinkedListBucketNode);
#endif
	_pHashLinkedListHead head;

	if (PtrCheck(p1) || PtrCheck(p2))
	{
		if (NotPtrCheck(p1))
			TFfree(p1);
		if (NotPtrCheck(p2))
			TFfree(p2);
		return NULL;
	}

	head = CastAny(_pHashLinkedListHead, p1);

	assert(max > 0);
	head->nodeMax = max;
	head->hfunc = hfunc;
	head->hContext = hContext;
	head->buckets = CastAnyPtr(_HashLinkedListBucketNode, p2);
	return p1;
}

TListCnt __stdcall
HashLinkedListCount(Pointer liste)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);

	assert(NotPtrCheck(liste));
	return head->nodeCount;
}

TListCnt __stdcall
HashLinkedListMaximum(Pointer liste)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);

	assert(NotPtrCheck(liste));
	return head->nodeMax;
}

void __stdcall
HashLinkedListClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	_pHashLinkedListBucketNode node;
	_pHashLinkedListNode node1;
	_pHashLinkedListNode node2;
	TListCnt i;

	assert(NotPtrCheck(liste));
	for (i = 0, node = head->buckets; i < head->nodeMax; ++i, ++node)
	{
		node1 = node->list;
		while (NotPtrCheck(node1))
		{
			node2 = node1->next;
			if (PtrCheck(freeFunc))
				TFfree(node1->data);
			else
				freeFunc(node1->data, context);
			TFfree(node1);
			node1 = node2;
		}
	}
	TFfree(head->buckets);
	TFfree(head);
}

void __stdcall
HashLinkedListClear(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	_pHashLinkedListBucketNode node;
	_pHashLinkedListNode node1;
	_pHashLinkedListNode node2;
	TListCnt i;

	assert(NotPtrCheck(liste));
	for (i = 0, node = head->buckets; i < head->nodeMax; ++i, ++node)
	{
		node1 = node->list;
		while (NotPtrCheck(node1))
		{
			node2 = node1->next;
			if (PtrCheck(freeFunc))
				TFfree(node1->data);
			else
				freeFunc(node1->data, context);
			TFfree(node1);
			node1 = node2;
		}
		node->bucket = 0;
		node->head = head;
		node->list = NULL;
		node->lend = NULL;
		node->listCount = 0;
	}
	head->nodeCount = 0;
}

LSearchResultType __stdcall
HashLinkedListIndex(Pointer liste, TListIndex index)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	LSearchResultType result = _LNULL;
	_pHashLinkedListBucketNode node;
	_pHashLinkedListNode node1;
	TListCnt i;
	TListIndex ix = index;

	assert(NotPtrCheck(liste));
	for (i = 0, node = head->buckets; i < head->nodeMax; ++i, ++node)
	{
		if (node->listCount == 0)
			continue;
		ix -= node->listCount;
		if (ix < 0)
		{
			node1 = node->lend;
			++ix;
			while (ix < 0)
			{
				node1 = node1->prev;
				++ix;
			}
			_Lnode(result) = node1;
			return result;
		}
	}
	return result;
}

LSearchResultType __stdcall
HashLinkedListBegin(Pointer liste)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	LSearchResultType result = _LNULL;
	_pHashLinkedListBucketNode node;
	TListCnt i;

	assert(NotPtrCheck(liste));
	for (i = 0, node = head->buckets; i < head->nodeMax; ++i, ++node)
	{
		if (node->listCount == 0)
			continue;
		_Lnode(result) = node->list;
		break;
	}
	return result;
}

LSearchResultType __stdcall
HashLinkedListNext(LSearchResultType _node)
{
	_pHashLinkedListNode node1 = _Lnode(_node);
	LSearchResultType result = _LNULL;
	_pHashLinkedListBucketNode node;
	_pHashLinkedListHead head;
	sdword i;

	assert(NotPtrCheck(node1));
	if (NotPtrCheck(node1->next))
	{
		_Lnode(result) = node1->next;
		return result;
	}
	node = node1->bucket;
	head = node->head;
	i = node->bucket;
	for (++i, ++node; i < Castsdword(head->nodeMax); ++i, ++node)
	{
		if (node->listCount == 0)
			continue;
		_Lnode(result) = node->list;
		break;
	}
	return result;
}

LSearchResultType __stdcall
HashLinkedListPrev(LSearchResultType _node)
{
	_pHashLinkedListNode node1 = _Lnode(_node);
	LSearchResultType result = _LNULL;
	_pHashLinkedListBucketNode node;
	_pHashLinkedListHead head;
	sdword i;

	assert(NotPtrCheck(node1));
	if (NotPtrCheck(node1->prev))
	{
		_Lnode(result) = node1->prev;
		return result;
	}
	node = node1->bucket;
	head = node->head;
	i = node->bucket;
	for (--i, --node; i >= 0; --i, --node)
	{
		if (node->listCount == 0)
			continue;
		_Lnode(result) = node->lend;
		break;
	}
	return result;
}

LSearchResultType __stdcall
HashLinkedListLast(Pointer liste)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	LSearchResultType result = _LNULL;
	_pHashLinkedListBucketNode node;
	TListCnt i;

	assert(NotPtrCheck(liste));
	for (i = head->nodeMax, node = head->buckets + (i - 1); i > 0; --i, --node)
	{
		if (node->listCount == 0)
			continue;
		_Lnode(result) = node->lend;
		break;
	}
	return result;
}

bool __stdcall
HashLinkedListForEach(Pointer liste, TForEachFunc func, Pointer context)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	_pHashLinkedListBucketNode node;
	_pHashLinkedListNode node1;
	TListCnt i;

	assert(NotPtrCheck(liste));
	assert(NotPtrCheck(func));
	for (i = 0, node = head->buckets; i < head->nodeMax; ++i, ++node)
	{
		node1 = node->list;
		while (NotPtrCheck(node1))
		{
			if (!(func(node1->data, context)))
				return false;
			node1 = node1->next;
		}
	}
	return true;
}

LSearchResultType __stdcall
HashLinkedListFind(Pointer liste, ConstPointer data, TSearchAndSortUserFunc findFunc, Pointer context)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	LSearchResultType result = _LNULL;
	_pHashLinkedListBucketNode node;
	_pHashLinkedListNode node1;
	TListCnt i;

	assert(NotPtrCheck(liste));
	assert(NotPtrCheck(findFunc));
	for (i = 0, node = head->buckets; i < head->nodeMax; ++i, ++node)
	{
		node1 = node->list;
		while (NotPtrCheck(node1))
		{
			if (0 == findFunc(node1->data, data, context))
			{
				_Lnode(result) = node1;
				return result;
			}
			node1 = node1->next;
		}
	}
	return result;
}

LSearchResultType __stdcall
HashLinkedListFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	LSearchResultType result = _LNULL;
	_pHashLinkedListBucketNode node;
	_pHashLinkedListNode node1;
	sdword ix;
	sword cmp;

	assert(NotPtrCheck(liste));
	assert(NotPtrCheck(sortFunc));
	ix = head->hfunc(data, head->hContext);
	if (ix > Castsdword(head->nodeMax))
		ix = ix % head->nodeMax;
	node = head->buckets + ix;
	if (PtrCheck(node->list))
		return result;
	node1 = node->list;
	while ( NotPtrCheck(node1) )
	{
		cmp = sortFunc(node1->data, data, context);
		if ( 0 <= cmp )
		{
			if (0 == cmp)
				_Lnode(result) = node1;
			break;
		}
		node1 = node1->next;
	}
	return result;
}

LSearchResultType __stdcall
HashLinkedListInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	LSearchResultType result = _LNULL;
	_pHashLinkedListBucketNode node;
	_pHashLinkedListNode node1;
	_pHashLinkedListNode node2;
	sdword ix;

	assert(NotPtrCheck(liste));
	assert(NotPtrCheck(sortFunc));
	ix = head->hfunc(data, head->hContext);
	if (ix > Castsdword(head->nodeMax))
		ix = ix % head->nodeMax;
	node = head->buckets + ix;
	if (PtrCheck(node->list))
	{
		node->bucket = ix;
		node->head = head;
		node2 = CastAnyPtr(_HashLinkedListNode, TFalloc(szHashLinkedListNode));
		node->list = node2;
		node->lend = node->list;
		node2->bucket = node;
		node2->data = CastMutable(Pointer, data);
		++(node->listCount);
		++(head->nodeCount);
		_Lnode(result) = node2;
		return result;
	}
	node1 = node->list;
	while (NotPtrCheck(node1))
	{
		if (0 <= sortFunc(node1->data, data, context))
			break;
		node1 = node1->next;
	}
	if (PtrCheck(node1))
	{
		node2 = CastAnyPtr(_HashLinkedListNode, TFalloc(szHashLinkedListNode));
		node->lend->next = node2;
		node2->prev = node->lend;
		node->lend = node2;
		node2->bucket = node;
		node2->data = CastMutable(Pointer, data);
		++(node->listCount);
		++(head->nodeCount);
		_Lnode(result) = node2;
		return result;
	}
	while (NotPtrCheck(node1))
	{
		if (0 < sortFunc(node1->data, data, context))
			break;
		node1 = node1->next;
	}
	if (PtrCheck(node1))
	{
		node2 = CastAnyPtr(_HashLinkedListNode, TFalloc(szHashLinkedListNode));
		node->lend->next = node2;
		node2->prev = node->lend;
		node->lend = node2;
		node2->bucket = node;
		node2->data = CastMutable(Pointer, data);
		++(node->listCount);
		++(head->nodeCount);
		_Lnode(result) = node2;
		return result;
	}
	node2 = CastAnyPtr(_HashLinkedListNode, TFalloc(szHashLinkedListNode));
	if ((node->list == node->lend) || (node1 == node->list))
	{
		node2->next = node->list;
		node2->next->prev = node2;
		node->list = node2;
	}
	else
	{
		node2->next = node1;
		node2->prev = node1->prev;
		node2->prev->next = node2;
		node2->next->prev = node2;
	}
	node2->bucket = node;
	node2->data = CastMutable(Pointer, data);
	++(node->listCount);
	++(head->nodeCount);
	_Lnode(result) = node2;
	return result;
}

bool __stdcall
HashLinkedListRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer sortContext, TDeleteFunc freeFunc, Pointer freeContext)
{
	_pHashLinkedListHead head = CastAny(_pHashLinkedListHead, liste);
	bool result = false;
	_pHashLinkedListBucketNode node;
	_pHashLinkedListNode node1;
	_pHashLinkedListNode node2;
	sdword ix;

	assert(NotPtrCheck(liste));
	assert(NotPtrCheck(sortFunc));
	ix = head->hfunc(data, head->hContext);
	if (ix > Castsdword(head->nodeMax))
		ix = ix % head->nodeMax;
	node = head->buckets + ix;
	if (PtrCheck(node->list))
		return result;
	node1 = node->list;
	while (NotPtrCheck(node1))
	{
		if (0 <= sortFunc(node1->data, data, sortContext))
			break;
		node1 = node1->next;
	}
	while (NotPtrCheck(node1))
	{
		if (0 == sortFunc(node1->data, data, sortContext))
		{
			node2 = node1;
			node1 = node1->next;
			if (node2 == node->list)
			{
				if (node->list == node->lend)
				{
					node->list = NULL;
					node->lend = NULL;
				}
				else
				{
					node->list = node->list->next;
					node->list->prev = NULL;
				}
			}
			else if (node2 == node->lend)
			{
				node->lend = node->lend->prev;
				node->lend->next = NULL;
			}
			else
			{
				node2->next->prev = node2->prev;
				node2->prev->next = node2->next;
			}
			if ( PtrCheck(freeFunc) )
				TFfree(node2->data);
			else
				freeFunc(node2->data, freeContext);
			TFfree(node2);
			--(node->listCount);
			--(head->nodeCount);
			result = true;
			break;
		}
		else
			break;
	}
	return result;
}

Pointer __stdcall
HashLinkedListGetData(LSearchResultType _node)
{
	_pHashLinkedListNode node1 = _Lnode(_node);

	assert(NotPtrCheck(node1));
	return node1->data;
}

void __stdcall
HashLinkedListSetData(LSearchResultType _node, ConstPointer data)
{
	_pHashLinkedListNode node1 = _Lnode(_node);

	assert(NotPtrCheck(node1));
	node1->data = CastMutable(Pointer, data);
}

