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
#include "TFMEM.H"
#include "LISTE.H"
#include "STRUTIL.H"
#include "UTLPTR.H"

#ifdef __DEBUG__
#include <stdio.h>
#endif

#ifdef __MT__
#ifdef OK_SYS_UNIX
#include <pthread.h>
#endif

static
#ifdef OK_SYS_WINDOWS
	CRITICAL_SECTION
#endif
#ifdef OK_SYS_UNIX
	pthread_mutex_t 
#endif
	g_Mutex;

#ifdef OK_SYS_WINDOWS
#define TFLOCK    EnterCriticalSection(&g_Mutex);
#define TFUNLOCK  LeaveCriticalSection(&g_Mutex);
#endif
#ifdef OK_SYS_UNIX
#define TFLOCK    pthread_mutex_lock(&g_Mutex);
#define TFUNLOCK  pthread_mutex_unlock(&g_Mutex);
#endif

#else /* __MT__ */
#define TFLOCK    
#define TFUNLOCK  
#endif /* __MT__ */

#define MAX_DEFAULTPOOL_ENTRIESPERNODE 256
#define MAX_MEMITEMS 32

#define MEMITEM_MODE_SMALLBLOCK 0x00
#define MEMITEM_MODE_LARGEBLOCK 0x01
#define MEMITEM_MODE_ALLOCBLOCK 0x02

typedef struct _tagVectorHead
{
	TListCnt max;
	TListCnt exp;
	TListCnt cnt;
	Array data;
} _VectorHead, *_pVectorHead;
#define szVectorHead sizeof(_VectorHead)

static Pointer __stdcall 
_TFVectorOpen(TListCnt max, TListCnt exp)
{
	Pointer p1 = malloc(szVectorHead);
	Pointer p2 = malloc(max * szPointer);
	_pVectorHead head;

	if ( PtrCheck(p1) || PtrCheck(p2) )
	{
		if ( NotPtrCheck(p1) )
			free(p1);
		if ( NotPtrCheck(p2) )
			free(p2);
		return NULL;
	}
	s_memset(p1, 0, szVectorHead);
	s_memset(p2, 0, (max * szPointer));

	head = CastAny(_pVectorHead,p1);

	assert(max > 0);
	head->max = max;
	assert(exp > 0);
	head->exp = exp;
	head->data = CastAny(Array, p2);
	return p1;
}

static void __stdcall 
_TFVectorReserve(Pointer liste)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Pointer p;

	assert(liste != NULL);
	assert(head->max > 0);
	assert(head->exp > 0);
	p = realloc(head->data, ((head->max + head->exp) * szPointer));
	if ( PtrCheck(p) )
		return;
	s_memset(_l_ptradd(p, (head->max * szPointer)), 0, (head->exp * szPointer));
	head->max += head->exp;
	head->data = CastAny(Array, p);
}

static void __stdcall 
_TFVectorClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataArray = head->data;
	Pointer d;
	TListCnt ix;

	assert(liste != NULL);
	for ( ix = 0; ix < head->cnt; ++ix, dataArray = CastAny(Array,_l_ptradd(dataArray,szPointer)) )
	{
		d = DerefPtr(Pointer,dataArray);
		if ( PtrCheck(freeFunc) )
			free(d);
		else
			freeFunc(d, context);
	}
	free(head->data);
	free(head);
}

static LSearchResultType __stdcall 
_TFVectorAppend(Pointer liste, ConstPointer data)
{
	_pVectorHead head = CastAny(_pVectorHead,liste);
	Array dataVector = head->data;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(head->max > 0);
	if ( head->cnt >= head->max )
	{
		_TFVectorReserve(liste);
		if ( head->cnt >= head->max )
			return result;
		dataVector = head->data;
	}
	_Lnode(result) = head;
	_Loffset(result) = _lv_insert( dataVector, Cast(sdword,head->cnt) - 1, data, &(head->cnt) );
	return result;
}

struct _tagBTreeHead;
typedef struct _tagBTreeNode
{
	struct _tagBTreeHead* head;
	struct _tagBTreeNode* parent;
	Pointer key;
	TListCnt cnt;
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

static Pointer __stdcall 
_TFBTreeGetData(LSearchResultType node);

static Pointer __stdcall 
_TFBTreeOpen(TListCnt maxEntriesPerNode)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, malloc(szBTreeHead));
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, malloc(szBTreeNode + (maxEntriesPerNode * szPointer)));

	if ( PtrCheck(pHead) || PtrCheck(pNode) )
	{
		if ( NotPtrCheck(pHead) )
			free(pHead);
		if ( NotPtrCheck(pNode) )
			free(pNode);
		return NULL;
	}
	s_memset(pHead, 0, szBTreeHead);
	s_memset(pNode, 0, szBTreeNode + (maxEntriesPerNode * szPointer));

	pHead->maxEntriesPerNode = maxEntriesPerNode;
	pHead->root = pNode;

	pNode->head = pHead;
	pNode->isData = TRUE;

	return pHead;
}

static TListCnt __stdcall 
_TFBTreeCount(Pointer liste)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);

	assert(liste != NULL);
	return pHead->nodeCount;
}

static LSearchResultType __stdcall 
_TFBTreeBegin(Pointer liste)
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

static LSearchResultType __stdcall 
_TFBTreeNext(LSearchResultType node)
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

static void __stdcall
_TFBTreeCloseHelper(_pBTreeNode node)
{
	Array p;
	TListCnt ix;

	if ( !(node->isData) )
	{
		for ( ix = 0, p = CastAny(Array, _l_ptradd(node, szBTreeNode)); 
			ix < node->cnt; 
			++ix, p = CastAny(Array, _l_ptradd(p, szPointer)) 
		)
			_TFBTreeCloseHelper(DerefAnyPtr(_pBTreeNode, p));
	}
	free(node);
}

static void __stdcall 
_TFBTreeClose(Pointer liste, TDeleteFunc freeFunc, Pointer context)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	LSearchResultType result;
	Pointer d;

	assert(liste != NULL);
    result = _TFBTreeBegin(liste);
	while ( !LPtrCheck(result) )
	{
		d = _TFBTreeGetData(result);
		if ( PtrCheck(freeFunc) )
			free(d);
		else
			freeFunc(d, context);
		result = _TFBTreeNext(result);
	}
	_TFBTreeCloseHelper(pHead->root);
	free(pHead);
}

#ifdef __DEBUG__
static void __stdcall 
_TFBTreeForEach(Pointer liste, TForEachFunc forEachFunc, Pointer context)
{
	LSearchResultType result;

	assert(liste != NULL);
    assert(forEachFunc != NULL);
    result = _TFBTreeBegin(liste);
	while ( !LPtrCheck(result) )
	{
		if (!forEachFunc(_TFBTreeGetData(result), context))
			break;
		result = _TFBTreeNext(result);
	}
}
#endif

static LSearchResultType __stdcall 
_TFBTreeFindSortedHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc)
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
			return _TFBTreeFindSortedHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc);
	}
	return result;
}

static LSearchResultType __stdcall 
_TFBTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return _TFBTreeFindSortedHelper(pNode, data, sortFunc);
}

static LSearchResultType __stdcall 
_TFBTreeLowerBoundHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc)
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
			return _TFBTreeLowerBoundHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc);
	}
	return result;
}

static LSearchResultType __stdcall 
_TFBTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;

	assert(liste != NULL);
	assert(pNode != NULL);
	assert(sortFunc != NULL);
	return _TFBTreeLowerBoundHelper(pNode, data, sortFunc);
}

static _pBTreeNode __stdcall
_TFBTreeInsertSortedHelper(_pBTreeNode pNode, ConstPointer data, TSearchAndSortFunc sortFunc, Ptr(LSearchResultType) pResult)
{
	_pBTreeNode pNode1 = NULL;
	_pBTreeNode pNode2;
	Array p;
	Array p1;
	dword sz;
	dword cnt;
    TListCnt ix;

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
			sz = szBTreeNode + (pNode->head->maxEntriesPerNode * szPointer);
			pNode2 = CastAnyPtr(_BTreeNode, malloc(sz));
            assert(NotPtrCheck(pNode2));
			s_memset(pNode2, 0, sz);
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
			pNode1 = _TFBTreeInsertSortedHelper(DerefAnyPtr(_pBTreeNode, p1), data, sortFunc, pResult);
			if ( PtrCheck(pNode1) )
				return NULL;
			pNode1->parent = pNode;
			break;
		}
	}
	_lv_insert(p, ix, pNode1, &(pNode->cnt));
	if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
	{
		sz = szBTreeNode + (pNode->head->maxEntriesPerNode * szPointer);
        pNode2 = CastAnyPtr(_BTreeNode, malloc(sz));
        assert(NotPtrCheck(pNode2));
		s_memset(pNode2, 0, sz);
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

static LSearchResultType __stdcall 
_TFBTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortFunc sortFunc)
{
	_pBTreeHead pHead = CastAnyPtr(_BTreeHead, liste);
	_pBTreeNode pNode = pHead->root;
	_pBTreeNode pNode2;
	Array p1;
	dword sz;
	LSearchResultType result = _LNULL;

	assert(liste != NULL);
	assert(pNode != NULL);
	pNode = _TFBTreeInsertSortedHelper(pNode, data, sortFunc, &result);
	if ( PtrCheck(pNode) )
		return result;
	sz = szBTreeNode + (pHead->maxEntriesPerNode * szPointer);
	pNode2 = CastAnyPtr(_BTreeNode, malloc(sz));
    assert(NotPtrCheck(pNode2));
	s_memset(pNode2, 0, sz);
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

static void __stdcall
_TFBTreeRemoveJoinHelper(_pBTreeNode pNodeDest, _pBTreeNode pNodeSrc)
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
	free(pNodeSrc);
}

static void __stdcall
_TFBTreeRemoveKeyUpdateHelper(_pBTreeNode pNode)
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

static void __stdcall 
_TFBTreeRemove(LSearchResultType node, TDeleteFunc freeFunc, Pointer context)
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
		free(data1);
	_lv_delete( p, ix, &(pNode->cnt) );
	if ( (ix == 0) && (pNode->cnt > 0) )
		_TFBTreeRemoveKeyUpdateHelper(pNode);
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
				free(pNode3);
				_lv_delete(p, ix, &(pNode2->cnt) );
			}
			else
			{
				pNode4 = DerefAnyPtr(_pBTreeNode,_l_ptradd(p, (ix + 1) * szPointer));
				if ( (ix < Cast(sdword, (pNode2->cnt - 1))) && ((pNode3->cnt + pNode4->cnt) < pHead->maxEntriesPerNode) )
				{
					b = ((ix == 0) && (pNode3->cnt == 0));
					_TFBTreeRemoveJoinHelper(pNode3, pNode4);
					_lv_delete(p, ix + 1, &(pNode2->cnt) );
					if ( b )
						_TFBTreeRemoveKeyUpdateHelper(pNode3);
				}
				else
				{
					pNode4 = DerefAnyPtr(_pBTreeNode,_l_ptradd(p, (ix - 1) * szPointer));
					if ( (ix > 0) && ((pNode3->cnt + pNode4->cnt) < pHead->maxEntriesPerNode) )
					{
						b = ((ix == 1) && (pNode4->cnt == 0));
						_TFBTreeRemoveJoinHelper(pNode4, pNode3);
						_lv_delete(p, ix, &(pNode2->cnt) );
						if ( b )
							_TFBTreeRemoveKeyUpdateHelper(pNode4);
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
			free(pNode1);
			pNode1 = pHead->root;
		}
	}
}

static Pointer __stdcall 
_TFBTreeGetData(LSearchResultType node)
{
	_pBTreeNode pNode = CastAnyPtr(_BTreeNode, _Lnode(node));
	Array p;

	if ( PtrCheck(pNode) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, pNode->cnt)) )
		return NULL;
	p = CastAny(Array, _l_ptradd(pNode, szBTreeNode + (_Loffset(node) * szPointer)));
	return DerefPtr(Pointer, p);
}

typedef struct _MEMITEM
{
	size_t size;
	dword refCnt;
	dword mode;
#ifdef __DEBUG__
	const char* file;
	int line;
#endif
} MEMITEM, *PMEMITEM;
#define szMEMITEM sizeof(MEMITEM)

static Pointer _defaultpoola = NULL;
static Pointer _defaultpoolf = NULL;
#ifdef __DEBUG__
#define MAX_DBGLISTE 199
static PMEMITEM _dbgListe[MAX_DBGLISTE + 1];
static int _dbgListeNum = -1;

static dword _cntTFalloc = 0;
static size_t _allocatedBufferSize = 0;
static size_t _freeBufferSize = 0;
#endif

static sword __stdcall 
_TFsortpointers(ConstPointer pa, ConstPointer pb)
{
	ptrdiff_t cmp = _fc_ptrdiff(pa, pb);

	if ( cmp < 0 )
		return -1;
	if ( cmp > 0 )
		return 1;
	return 0;
}

#ifdef OK_CPU_64BIT
static sword __stdcall
_TFsortsizes(ConstPointer pa, ConstPointer pb)
{
	PMEMITEM pa1 = CastAnyPtr(MEMITEM, CastMutable(Pointer, pa));
	PMEMITEM pb1 = CastAnyPtr(MEMITEM, CastMutable(Pointer, pb));
	sqword cmp = Castsqword(pa1->size) - Castsqword(pb1->size);

	if (cmp < 0LL)
		return -1;
	if (cmp > 0LL)
		return 1;
	return 0;
}
#endif
#ifdef OK_CPU_32BIT
static sword __stdcall
_TFsortsizes(ConstPointer pa, ConstPointer pb)
{
	PMEMITEM pa1 = CastAnyPtr(MEMITEM, CastMutable(Pointer, pa));
	PMEMITEM pb1 = CastAnyPtr(MEMITEM, CastMutable(Pointer, pb));
	sdword cmp = Castsdword(pa1->size) - Castsdword(pb1->size);

	if (cmp < 0)
		return -1;
	if (cmp > 0)
		return 1;
	return 0;
}
#endif

static void __stdcall
_TFremovenull( ConstPointer data, Pointer context )
{
}

static void __stdcall 
_TFremoveonexita( ConstPointer data, Pointer context )
{
	PMEMITEM pmi = CastAnyPtr(MEMITEM, CastMutable(Pointer, data));

	--pmi;
	switch ( pmi->mode )
	{
	case MEMITEM_MODE_LARGEBLOCK:
	case MEMITEM_MODE_ALLOCBLOCK:
		_TFVectorAppend(context, pmi);
		break;
	default:
		break;
	}
}

static void __stdcall 
_TFremoveonexitf( ConstPointer data, Pointer context )
{
	PMEMITEM pmi = CastAnyPtr(MEMITEM, CastMutable(Pointer, data));

	switch ( pmi->mode )
	{
	case MEMITEM_MODE_LARGEBLOCK:
	case MEMITEM_MODE_ALLOCBLOCK:
		_TFVectorAppend(context, pmi);
		break;
	default:
		break;
	}
}

#ifdef __DEBUG__
static bool __stdcall 
_TFscanonalloca( ConstPointer data, Pointer context )
{
	PMEMITEM pmi = CastAnyPtr(MEMITEM, CastMutable(Pointer, data));

	--pmi;
	assert(pmi->refCnt > 0);
	switch ( pmi->mode )
	{
	case MEMITEM_MODE_LARGEBLOCK:
		_allocatedBufferSize += pmi->size;
		break;
	case MEMITEM_MODE_ALLOCBLOCK:
		_allocatedBufferSize += pmi->size * MAX_MEMITEMS;
		break;
	default:
		break;
	}
	return TRUE;
}

static bool __stdcall 
_TFscanonallocf( ConstPointer data, Pointer context )
{
	PMEMITEM pmi = CastAnyPtr(MEMITEM, CastMutable(Pointer, data));

	assert(pmi->refCnt == 1);
	switch ( pmi->mode )
	{
	case MEMITEM_MODE_LARGEBLOCK:
		_freeBufferSize += pmi->size;
		break;
	case MEMITEM_MODE_ALLOCBLOCK:
		_freeBufferSize += pmi->size * MAX_MEMITEMS;
		break;
	default:
		break;
	}
	return TRUE;
}

static bool __stdcall 
_TFscanonexita( ConstPointer data, Pointer context )
{
	PMEMITEM pmi = CastAnyPtr(MEMITEM, CastMutable(Pointer, data));
	const char* bp = NULL;

	--pmi;
	switch ( pmi->mode )
	{
    case MEMITEM_MODE_SMALLBLOCK:
        bp = "SMALLBLOCK";
        break;
	case MEMITEM_MODE_LARGEBLOCK:
        bp = "LARGEBLOCK";
        break;
	case MEMITEM_MODE_ALLOCBLOCK:
        bp = "ALLOCBLOCK";
        break;
	}
#ifdef OK_COMP_MSC
#ifdef OK_CPU_64BIT
	_RPT5(_CRT_WARN, "%hs(%d): %hs (%016llx, %05d)\n", pmi->file, pmi->line, bp, (ULongPointer)pmi, pmi->refCnt);
#endif
#ifdef OK_CPU_32BIT
	_RPT5(_CRT_WARN, " %hs(%d): %hs (%08lx, %05d):\n", pmi->file, pmi->line, bp, (ULongPointer)pmi, pmi->refCnt);
#endif
#endif
#ifdef OK_COMP_GNUC
#ifdef OK_CPU_64BIT
	printf("%s(%d): %s(%016llx, %05d)\n", pmi->file, pmi->line, bp, (ULongPointer)pmi, pmi->refCnt);
#endif
#ifdef OK_CPU_32BIT
	printf("%s(%d): %s(%08lx, %05d)\n", pmi->file, pmi->line, bp, (ULongPointer)pmi, pmi->refCnt);
#endif
#endif
	return TRUE;
}

#ifdef OK_COMP_GNUC
sdword _TFHashTabSize = 2048;
sdword _TFHashTabCount = 0;
char* _TFHashTab[2049] = { NULL };

Pointer _TFHashTabAlloc(size_t sz);

sdword _TFHashTabFunctor(const char* v)
{
	sdword h = 0, a = 31415, b = 27183;

	for (; *v != 0; ++v, a = (a*b) % (_TFHashTabSize - 1))
		h = ((a*h) + (*v)) % _TFHashTabSize;
	return (h < 0) ? (h + _TFHashTabSize) : h;
}

char* _TFHashTabInsert(const char* v)
{
	sdword i = _TFHashTabFunctor(v);
	char* p;
	size_t psz;

	while (_TFHashTab[i] != NULL) i = (i + 1) % _TFHashTabSize;
	psz = strlen(v);
	p = CastAnyPtr(char, _TFHashTabAlloc(psz + 1));
	strcpy(p, v);
	_TFHashTab[i] = p; 
	++_TFHashTabCount;
	return p;
}

char* _TFHashTabSearch(const char* v)
{
	sdword i = _TFHashTabFunctor(v);

	while (_TFHashTab[i] != NULL)
	{
		if (strcmp(_TFHashTab[i], v) == 0)
			return _TFHashTab[i];
		i = (i + 1) % _TFHashTabSize;
	}
	return NULL;
}

Pointer _TFHashTabAlloc(size_t sz)
{
	size_t bufSize = 0;
	LSearchResultType result;
	PMEMITEM pmi;
	PMEMITEM pmi1;
	MEMITEM mi;
	dword ix;

	if (sz == 0)
		return NULL;
	TFLOCK
	if (sz < 256)
		bufSize = ((sz + 15) / 16) * 16;
	else if (sz < 1024)
		bufSize = ((sz + 31) / 32) * 32;
	else
	{
		bufSize = ((sz + 3) / 4) * 4;
		s_memset(&mi, 0, szMEMITEM);
		mi.size = bufSize;
		result = _TFBTreeLowerBound(_defaultpoolf, &mi, _TFsortsizes);
		if (LPtrCheck(result) || (0 > _TFsortsizes(_TFBTreeGetData(result), &mi)))
		{
			pmi = (PMEMITEM)malloc(bufSize + sizeof(MEMITEM));

			if (PtrCheck(pmi))
			{
				TFUNLOCK
				return NULL;
			}
			s_memset(pmi, 0, bufSize + sizeof(MEMITEM));
			pmi->mode = MEMITEM_MODE_LARGEBLOCK;
		}
		else
		{
			pmi = CastAnyPtr(MEMITEM, _TFBTreeGetData(result));
			_TFBTreeRemove(result, _TFremovenull, NULL);
		}
		pmi->size = bufSize;
		pmi->refCnt = 1;
		pmi->file = NULL;
		pmi->line = 0;
		++pmi;
		_TFBTreeInsertSorted(_defaultpoola, pmi, _TFsortpointers);
		TFUNLOCK
		return pmi;
	}
	s_memset(&mi, 0, szMEMITEM);
	mi.size = bufSize;
	result = _TFBTreeLowerBound(_defaultpoolf, &mi, _TFsortsizes);
	if (LPtrCheck(result) || (0 != _TFsortsizes(_TFBTreeGetData(result), &mi)))
	{
		mi.size = (bufSize + sizeof(MEMITEM)) * MAX_MEMITEMS;
		result = _TFBTreeLowerBound(_defaultpoolf, &mi, _TFsortsizes);
		while (!(LPtrCheck(result) || (0 > _TFsortsizes(_TFBTreeGetData(result), &mi))))
		{
			pmi = CastAnyPtr(MEMITEM, _TFBTreeGetData(result));
			if (pmi->mode != MEMITEM_MODE_SMALLBLOCK)
			{
				_TFBTreeRemove(result, _TFremovenull, NULL);
				break;
			}
			result = _TFBTreeNext(result);
		}
		if (LPtrCheck(result) || (0 > _TFsortsizes(_TFBTreeGetData(result), &mi)))
		{
			pmi = (PMEMITEM)malloc((bufSize + sizeof(MEMITEM)) * MAX_MEMITEMS);
			if (PtrCheck(pmi))
			{
				TFUNLOCK
				return NULL;
			}
			s_memset(pmi, 0, (bufSize + sizeof(MEMITEM)) * MAX_MEMITEMS);
			pmi->mode = MEMITEM_MODE_ALLOCBLOCK;
		}
		pmi1 = pmi;
		++pmi;
		pmi = CastAnyPtr(MEMITEM, _l_ptradd(pmi, bufSize));
		for (ix = 1; ix < MAX_MEMITEMS; ++ix)
		{
			pmi->mode = MEMITEM_MODE_SMALLBLOCK;
			pmi->size = bufSize;
			pmi->refCnt = 1;
			_TFBTreeInsertSorted(_defaultpoolf, pmi, _TFsortsizes);
			++pmi;
			pmi = CastAnyPtr(MEMITEM, _l_ptradd(pmi, bufSize));
		}
		pmi = pmi1;
	}
	else
	{
		pmi = CastAnyPtr(MEMITEM, _TFBTreeGetData(result));
		_TFBTreeRemove(result, _TFremovenull, NULL);
	}
	pmi->size = bufSize;
	pmi->refCnt = 1;
	pmi->file = NULL;
	pmi->line = 0;
	++pmi;
	_TFBTreeInsertSorted(_defaultpoola, pmi, _TFsortpointers);
	TFUNLOCK
	return pmi;
}
#endif /* OK_COMP_GNUC */
#endif /* __DEBUG__ */

#if defined(__DEBUG__) && defined(OK_COMP_MSC)
static _CrtMemState memState;
#endif

static void __cdecl
_TFexit(void)
{
	dword sz;
	Pointer pVector;

	if ( PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf) )
		return;
#ifdef __DEBUG__
    if ( _TFBTreeCount(_defaultpoola) > 0 )
        _TFBTreeForEach(_defaultpoola, _TFscanonexita, NULL);
#endif
	sz = _TFBTreeCount(_defaultpoola) + _TFBTreeCount(_defaultpoolf);
	sz = ((sz + 256) / 256) + 255;
	pVector = _TFVectorOpen(sz, 256);
	if ( PtrCheck(pVector) )
		return;

	_TFBTreeClose(_defaultpoola, _TFremoveonexita, pVector);
	_TFBTreeClose(_defaultpoolf, _TFremoveonexitf, pVector);
	_TFVectorClose(pVector, NULL, NULL);

#ifdef __MT__
#ifdef OK_SYS_WINDOWS
	DeleteCriticalSection(&g_Mutex);
#endif
#ifdef OK_SYS_UNIX
	pthread_mutex_destroy(&g_Mutex);
#endif
#endif

#if defined(__DEBUG__) && defined(OK_COMP_MSC)
	_CrtMemDumpAllObjectsSince(&memState);
	_ASSERTE( _CrtCheckMemory( ) );
#endif
}

static void __stdcall 
_TFinit()
{
#if defined(__DEBUG__) && defined(OK_COMP_MSC)
	_CrtMemCheckpoint(&memState);
#endif
#ifdef __MT__
#ifdef OK_SYS_WINDOWS
	InitializeCriticalSection(&g_Mutex);
#endif
#ifdef OK_SYS_UNIX
	pthread_mutex_init(&g_Mutex, NULL);
#endif
#endif

	_defaultpoola = _TFBTreeOpen(MAX_DEFAULTPOOL_ENTRIESPERNODE);
	_defaultpoolf = _TFBTreeOpen(MAX_DEFAULTPOOL_ENTRIESPERNODE);

	assert(NotPtrCheck(_defaultpoola));
	assert(NotPtrCheck(_defaultpoolf));
	atexit(_TFexit);
}

#ifdef __DEBUG__
void* TFallocDbg(size_t sz, const char* file, int line)
#else
void* TFalloc(size_t sz)
#endif
{
	size_t bufSize = 0;
	LSearchResultType result;
	PMEMITEM pmi;
	PMEMITEM pmi1;
	MEMITEM mi;
	dword ix;

	if (sz == 0)
		return NULL;
	if (PtrCheck(_defaultpoola))
		_TFinit();
	if (PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf))
		return NULL;
	TFLOCK
#ifdef __DEBUG__
		++_cntTFalloc;
	if ((_cntTFalloc % 1000) == 0)
	{
		_allocatedBufferSize = 0;
		_TFBTreeForEach(_defaultpoola, _TFscanonalloca, NULL);
		_freeBufferSize = 0;
		_TFBTreeForEach(_defaultpoolf, _TFscanonallocf, NULL);
	}
#endif
	if (sz < 256)
		bufSize = ((sz + 15) / 16) * 16;
	else if (sz < 1024)
		bufSize = ((sz + 31) / 32) * 32;
	else
	{
		bufSize = ((sz + 3) / 4) * 4;
		s_memset(&mi, 0, szMEMITEM);
		mi.size = bufSize;
		result = _TFBTreeLowerBound(_defaultpoolf, &mi, _TFsortsizes);
		if (LPtrCheck(result) || (0 > _TFsortsizes(_TFBTreeGetData(result), &mi)))
		{
			pmi = (PMEMITEM)malloc(bufSize + sizeof(MEMITEM));
			if (PtrCheck(pmi))
			{
				TFUNLOCK
					return NULL;
			}
			s_memset(pmi, 0, bufSize + sizeof(MEMITEM));
			pmi->mode = MEMITEM_MODE_LARGEBLOCK;
		}
		else
		{
			pmi = CastAnyPtr(MEMITEM, _TFBTreeGetData(result));
			_TFBTreeRemove(result, _TFremovenull, NULL);
		}
		pmi->size = bufSize;
		pmi->refCnt = 1;
#ifdef __DEBUG__
#ifdef OK_COMP_GNUC
		if (PtrCheck(file))
			pmi->file = NULL;
		else
		{
			pmi->file = _TFHashTabSearch(file);
			if (PtrCheck(pmi->file))
			{
				TFUNLOCK
				pmi->file = _TFHashTabInsert(file);
				TFLOCK
			}
		}
#endif
#ifdef OK_COMP_MSC
		pmi->file = file;
#endif
		pmi->line = line;
		if ( _dbgListeNum < MAX_DBGLISTE ) {
			++_dbgListeNum;
			_dbgListe[_dbgListeNum] = pmi;
		}
#endif
		++pmi;
		_TFBTreeInsertSorted(_defaultpoola, pmi, _TFsortpointers);
		TFUNLOCK
		return pmi;
	}
	s_memset(&mi, 0, szMEMITEM);
	mi.size = bufSize;
	result = _TFBTreeLowerBound(_defaultpoolf, &mi, _TFsortsizes);
	if ( LPtrCheck(result) || (0 != _TFsortsizes(_TFBTreeGetData(result), &mi)) )
	{
		mi.size = (bufSize + sizeof(MEMITEM)) * MAX_MEMITEMS;
		result = _TFBTreeLowerBound(_defaultpoolf, &mi, _TFsortsizes);
		while (!(LPtrCheck(result) || (0 > _TFsortsizes(_TFBTreeGetData(result), &mi))))
		{
			pmi = CastAnyPtr(MEMITEM, _TFBTreeGetData(result));
			if (pmi->mode != MEMITEM_MODE_SMALLBLOCK)
				break;
			result = _TFBTreeNext(result);
		}
		if (LPtrCheck(result) || (0 > _TFsortsizes(_TFBTreeGetData(result), &mi)))
		{
			pmi = (PMEMITEM)malloc((bufSize + sizeof(MEMITEM)) * MAX_MEMITEMS);
			if (PtrCheck(pmi))
			{
				TFUNLOCK
				return NULL;
			}
			s_memset(pmi, 0, (bufSize + sizeof(MEMITEM)) * MAX_MEMITEMS);
		}
		else
			_TFBTreeRemove(result, _TFremovenull, NULL);
		pmi->mode = MEMITEM_MODE_ALLOCBLOCK;
        pmi1 = pmi;
        ++pmi;
		pmi = CastAnyPtr(MEMITEM, _l_ptradd(pmi, bufSize));
		for ( ix = 1; ix < MAX_MEMITEMS; ++ix)
		{
			pmi->mode = MEMITEM_MODE_SMALLBLOCK;
			pmi->size = bufSize;
			pmi->refCnt = 1;
			_TFBTreeInsertSorted(_defaultpoolf, pmi, _TFsortsizes);
			++pmi;
			pmi = CastAnyPtr(MEMITEM, _l_ptradd(pmi, bufSize));
		}
		pmi = pmi1;
	}
	else
	{
		pmi = CastAnyPtr(MEMITEM, _TFBTreeGetData(result));
		_TFBTreeRemove(result, _TFremovenull, NULL);
	}
	pmi->size = bufSize;
	pmi->refCnt = 1;
#ifdef __DEBUG__
#ifdef OK_COMP_GNUC
	if (PtrCheck(file))
		pmi->file = NULL;
	else
	{
		pmi->file = _TFHashTabSearch(file);
		if (PtrCheck(pmi->file))
		{
			TFUNLOCK
			pmi->file = _TFHashTabInsert(file);
			TFLOCK
		}
	}
#endif
#ifdef OK_COMP_MSC
	pmi->file = file;
#endif
	pmi->line = line;
	if ( _dbgListeNum < MAX_DBGLISTE ) {
		++_dbgListeNum;
		_dbgListe[_dbgListeNum] = pmi;
	}
#endif
	++pmi;
	_TFBTreeInsertSorted(_defaultpoola, pmi, _TFsortpointers);
	TFUNLOCK
	return pmi;
}

#ifdef __DEBUG__
void* TFreallocDbg(void* p, size_t newsz, const char* file, int line)
#else
void* TFrealloc(void* p, size_t newsz)
#endif
{
	Pointer p1;
	size_t oldsz;
	size_t sz;
	PMEMITEM pmi;
	PMEMITEM pmi1;

	if ( PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf) || PtrCheck(p) )
		return NULL;
	if ( newsz == 0 )
	{
		TFfree(p);
		return NULL;
	}
	TFLOCK
	pmi = CastAnyPtr(MEMITEM, p);
	--pmi;
	oldsz = pmi->size;
	if (((oldsz - newsz) >= 0) && ((oldsz - newsz) <= 16))
	{
#ifdef __DEBUG__
#ifdef OK_COMP_GNUC
		pmi->file = _TFHashTabSearch(file);
		if (pmi->file == NULL)
		{
			TFUNLOCK
				pmi->file = _TFHashTabInsert(file);
			TFLOCK
		}
#endif
#ifdef OK_COMP_MSC
		pmi->file = file;
#endif
		pmi->line = line;
#endif
		TFUNLOCK
		return p;
	}	
	TFUNLOCK
#ifdef __DEBUG__
	p1 = TFallocDbg(newsz, file, line);
#else
	p1 = TFalloc(newsz);
#endif
	if ( PtrCheck(p1) )
		return NULL;
	TFLOCK
	pmi = CastAnyPtr(MEMITEM, p);
	--pmi;
	pmi1 = CastAnyPtr(MEMITEM, p1);
	--pmi1;
	pmi1->refCnt = pmi->refCnt;
	sz = Min(oldsz, newsz);
	s_memcpy_s(p1, sz, p, sz);
	if ( newsz > oldsz )
		s_memset(_l_ptradd(p1, oldsz), 0, newsz - oldsz);
	TFUNLOCK
	TFfree(p);
	return p1;
}

void TFfree(void* p)
{
	LSearchResultType result;
	PMEMITEM pmi;

	if ( PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf) || PtrCheck(p) )
		return;
	TFLOCK
	result = _TFBTreeFindSorted(_defaultpoola, p, _TFsortpointers);
	if (LPtrCheck(result) || (_TFBTreeGetData(result) != p))
	{
		TFUNLOCK
		return;
	}
	_TFBTreeRemove(result, _TFremovenull, NULL);
	pmi = CastAnyPtr(MEMITEM, p);
	--pmi;
	s_memset(p, 0, pmi->size);
	pmi->refCnt = 1;
#ifdef __DEBUG__
	pmi->file = NULL;
	pmi->line = 0;
#endif
	_TFBTreeInsertSorted(_defaultpoolf, pmi, _TFsortsizes);
	TFUNLOCK
}

bool TFisalloc(void* p)
{
	LSearchResultType result;

	if ( PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf) || PtrCheck(p) )
		return false;
	TFLOCK
	result = _TFBTreeFindSorted(_defaultpoola, p, _TFsortpointers);
	if ( (!LPtrCheck(result)) && (_TFBTreeGetData(result) == p) )
	{
		TFUNLOCK
		return true;
	}
	TFUNLOCK
	return false;
}

size_t TFsize(void* p)
{
	PMEMITEM pmi;
	size_t result;

	if ( PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf) || PtrCheck(p) )
		return 0;
	TFLOCK
	pmi = CastAnyPtr(MEMITEM, p);
	--pmi;
	result = pmi->size;
	TFUNLOCK
	return result;
}

dword TFrefcnt(void* p)
{
	PMEMITEM pmi;
	dword result;

	if ( PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf) || PtrCheck(p) )
		return 0;
	TFLOCK
	pmi = CastAnyPtr(MEMITEM, p);
	--pmi;
	result = pmi->refCnt;
	TFUNLOCK
	return result;
}

dword TFincrefcnt(void* p)
{
	PMEMITEM pmi;
	dword result;

	if ( PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf) || PtrCheck(p) )
		return 0;
	TFLOCK
	pmi = CastAnyPtr(MEMITEM, p);
	--pmi;
	++(pmi->refCnt);
	result = pmi->refCnt;
	TFUNLOCK
	return result;
}

dword TFdecrefcnt(void* p)
{
	LSearchResultType sresult;
	PMEMITEM pmi;
	dword result = 0;
	dword result1 = 0;

	if ( PtrCheck(_defaultpoola) || PtrCheck(_defaultpoolf) || PtrCheck(p) )
		return 0;
	TFLOCK
	pmi = CastAnyPtr(MEMITEM, p);
	sresult = _TFBTreeFindSorted(_defaultpoola, p, _TFsortpointers);
	if ((!LPtrCheck(sresult)) && (_TFBTreeGetData(sresult) == p))
	{
		--pmi;
		result1 = pmi->refCnt;
		--(pmi->refCnt);
		result = pmi->refCnt;
	}
	TFUNLOCK
	assert(result1 > 0);
	return result;
}
