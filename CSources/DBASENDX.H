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
#ifndef __DBASENDX_H__
#define __DBASENDX_H__

#include "CSOURCES.H"
#include "LISTE.H"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagFBTreeFileNode;
typedef struct _tagFBTreeFileHead
{
	int fd;                          // index filedesriptor
	TListCnt nodeCount;              // tree data node cnt
	word fphead;                     // filepos head
	word pagesize;                   // pagesize
	word maxEntriesPerNode;          // max node cnt
	word keySize;                    // key size
	sqword fproot;                   // filepos root
	struct _tagFBTreeFileNode* root; // pointer to root
	byte isBatch;                    // flag for batch mode
	byte reserve[3];
} _FBTreeFileHead, *_pFBTreeFileHead;
#define szFBTreeFileHead sizeof(_FBTreeFileHead)

typedef struct _tagFBTreeFileNode
{
	struct _tagFBTreeFileHead* head;   // pointer to head
	struct _tagFBTreeFileNode* parent; // pointer to parent
	sqword fpNode;                     // filepos of node
	word cnt;                          // node cnt
	byte isData;                       // indicator, if node is leaf
	byte isToAppend;                   // batch mode: node must be appended
	byte isModified;                   // batch mode: node was modified
	byte reserve[11];
} _FBTreeFileNode, *_pFBTreeFileNode;
#define szFBTreeFileNode sizeof(_FBTreeFileNode)

typedef struct _tagFBTreeFileNodeEntry
{
	sqword fpnext;                      // filepos next entry
	struct _tagFBTreeFileNode* next;    // pointer to next entry
	byte reserve[4];
} _FBTreeFileNodeEntry, *_pFBTreeFileNodeEntry;
#define szFBTreeFileNodeEntry sizeof(_FBTreeFileNodeEntry)

CSOURCES_LOCAL TListCnt __stdcall FBTreeCount(Pointer liste);
CSOURCES_LOCAL void __stdcall FBTreeClose(Pointer liste);
CSOURCES_LOCAL errno_t __stdcall FBTreeBegin(Pointer liste, Ptr(LSearchResultType) result);
CSOURCES_LOCAL errno_t __stdcall FBTreeNext(LSearchResultType node, Ptr(LSearchResultType) result);
CSOURCES_LOCAL errno_t __stdcall FBTreePrev(LSearchResultType node, Ptr(LSearchResultType) result);
CSOURCES_LOCAL errno_t __stdcall FBTreeLast(Pointer liste, Ptr(LSearchResultType) result);
CSOURCES_LOCAL errno_t __stdcall FBTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, ConstPointer context, Ptr(LSearchResultType) result);
CSOURCES_LOCAL errno_t __stdcall FBTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, ConstPointer context, Ptr(LSearchResultType) result);
CSOURCES_LOCAL errno_t __stdcall FBTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, ConstPointer context, Ptr(LSearchResultType) result);
CSOURCES_LOCAL errno_t __stdcall FBTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, ConstPointer context, Ptr(LSearchResultType) result);
CSOURCES_LOCAL errno_t __stdcall FBTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, ConstPointer context, Ptr(bool));
CSOURCES_LOCAL Pointer __stdcall FBTreeGetData(LSearchResultType node);
CSOURCES_LOCAL void __stdcall FBTreeSetData(LSearchResultType node, ConstPointer data);
CSOURCES_LOCAL void __stdcall FBTreeSortedTester(Pointer liste, TSearchAndSortUserFunc sortFunc, ConstPointer context);
CSOURCES_LOCAL void __stdcall FBTreeBeginBatch(Pointer liste);
CSOURCES_LOCAL errno_t __stdcall FBTreeEndBatch(Pointer liste);

#ifdef __cplusplus
}
#endif

#endif
