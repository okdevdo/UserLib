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
#include "DBASENDX.H"
#include "STRUTIL.H"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#ifdef OK_SYS_WINDOWS
	#include <share.h>
#endif
#ifdef OK_SYS_UNIX
	#include <unistd.h>
	#define _write write
	#define _read read
	#define _close close
#endif
#include <sys/types.h>
#include <sys/stat.h>

#ifdef OK_SYS_UNIX
static errno_t chsize( int handle, long size )
{
	Pointer buf;
	int cnt;
	off_t fpos;
	
	fpos = lseek( handle, 0, SEEK_END );
	if ( 0 > fpos )
		return errno;
	buf = TFalloc(size - fpos);
	if ( PtrCheck(buf) )
		return ENOMEM;
	cnt = write(handle, buf, size - fpos);
	if ( 0 > cnt )
		return errno;
	if ( cnt != (size - fpos) )
		return EIO;
	TFfree(buf);
	return 0;
}
#endif

static errno_t __stdcall
_FBTreeLoadRoot(_pFBTreeFileHead pHead)
{
	_FBTreeFileNode node;
	_pFBTreeFileNodeEntry p1;
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
#endif
	dword sz;
	dword ix;

	if ( pHead->fproot == 0 )
		return EINVAL;
#ifdef OK_SYS_WINDOWS
	fpos = _lseeki64(pHead->fd, pHead->fproot, SEEK_SET);
#endif
#ifdef OK_SYS_UNIX
	fpos = lseek(pHead->fd, pHead->fproot, SEEK_SET);
#endif
	if ( 0 > fpos )
		return errno;
	if ( Cast(qword,fpos) != pHead->fproot )
		return EIO;
	cnt = _read(pHead->fd, &node, szFBTreeFileNode);
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < szFBTreeFileNode )
		return EIO;
	node.head = pHead;
	node.parent = NULL;
	sz = szFBTreeFileNode + (pHead->maxEntriesPerNode * (szFBTreeFileNodeEntry + pHead->keySize));
	pHead->root = CastAnyPtr(_FBTreeFileNode, TFalloc(sz));
	if ( PtrCheck(pHead->root) )
		return ENOMEM;
	s_memcpy_s(pHead->root, szFBTreeFileNode, &node, szFBTreeFileNode);
	if ( node.cnt > 0 )
	{
		p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pHead->root, szFBTreeFileNode));
		sz = node.cnt * (szFBTreeFileNodeEntry + pHead->keySize);
		cnt = _read(pHead->fd, p1, sz);
		if ( cnt < 0 )
			return errno;
		if ( cnt < Cast(int, sz) )
			return EIO;
		for ( ix = 0; ix < node.cnt; ++ix, 
			p1 = CastAnyPtr(_FBTreeFileNodeEntry, 
			_l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize)) )
			p1->next = NULL;
	}
	return 0;
}

static errno_t __stdcall
_FBTreeLoadNext(_pFBTreeFileHead pHead, _pFBTreeFileNode pNode, _pFBTreeFileNodeEntry p)
{
	_FBTreeFileNode node;
	_pFBTreeFileNodeEntry p1;
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
#endif
	dword sz;
	dword ix;

	if ( NotPtrCheck(p->next) )
		return 0;
#ifdef OK_SYS_WINDOWS
	fpos = _lseeki64(pHead->fd, p->fpnext, SEEK_SET);
#endif
#ifdef OK_SYS_UNIX
	fpos = lseek(pHead->fd, p->fpnext, SEEK_SET);
#endif
	if ( 0 > fpos )
		return errno;
	if ( Cast(qword,fpos) != p->fpnext )
		return EIO;
	cnt = _read(pHead->fd, &node, szFBTreeFileNode);
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < szFBTreeFileNode )
		return EIO;
	node.head = pHead;
	node.parent = pNode;
	sz = szFBTreeFileNode + (pHead->maxEntriesPerNode * (szFBTreeFileNodeEntry + pHead->keySize));
	p->next = CastAnyPtr(_FBTreeFileNode, TFalloc(sz));
	if ( PtrCheck(p->next) )
		return ENOMEM;
	s_memcpy_s(p->next, szFBTreeFileNode, &node, szFBTreeFileNode);
	if ( node.cnt > 0 )
	{
		p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p->next, szFBTreeFileNode));
		sz = node.cnt * (szFBTreeFileNodeEntry + pHead->keySize);
		cnt = _read(pHead->fd, p1, sz);
		if ( cnt < 0 )
			return errno;
		if ( cnt < Cast(int, sz) )
			return EIO;
		for ( ix = 0; ix < node.cnt; ++ix, 
			p1 = CastAnyPtr(_FBTreeFileNodeEntry, 
			_l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize)) )
			p1->next = NULL;
	}
	return 0;
}

static errno_t __stdcall
_FBTreeUpdateRoot(_pFBTreeFileHead pHead)
{
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
#endif

	if ( pHead->isBatch )
		return 0;
#ifdef OK_SYS_WINDOWS
	fpos = _lseeki64(pHead->fd, pHead->fphead, SEEK_SET);
#endif
#ifdef OK_SYS_UNIX
	fpos = lseek(pHead->fd, pHead->fphead, SEEK_SET);
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != pHead->fphead )
		return EIO;
	cnt = _write(pHead->fd, pHead, szFBTreeFileHead);
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < szFBTreeFileHead )
		return EIO;
	return 0;
}

static errno_t __stdcall
_FBTreeCreateRoot(_pFBTreeFileHead pHead)
{
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword flength;
	sqword flength1;
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t flength;
	off_t flength1;
	off_t fpos;
#endif
	errno_t res;
	dword sz;

#ifdef OK_SYS_WINDOWS
	flength = _filelengthi64(pHead->fd);
#endif
#ifdef OK_SYS_UNIX
	flength = lseek(pHead->fd, 0, SEEK_END);
#endif
	if ( flength < 0 )
		return errno;
	flength = ((flength + pHead->pagesize - 1LL) / pHead->pagesize) * pHead->pagesize;
	flength1 = flength + pHead->pagesize;
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	res = _chsize( pHead->fd, (long)flength1 );
    if ( res < 0 )
        return errno;
	fpos = _lseeki64( pHead->fd, flength, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
    res = chsize( pHead->fd, flength1 );
    if ( res )
        return res;
	fpos = lseek( pHead->fd, flength, SEEK_SET );
#endif
#endif
#ifdef OK_COMP_MSC
	res = _chsize_s(pHead->fd, flength1);
	if ( res )
		return res;
	fpos = _lseeki64( pHead->fd, flength, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != flength )
		return EIO;
	sz = szFBTreeFileNode + (pHead->maxEntriesPerNode * (szFBTreeFileNodeEntry + pHead->keySize));
	pHead->root = CastAnyPtr(_FBTreeFileNode, TFalloc(sz));
	if ( PtrCheck(pHead->root) )
		return ENOMEM;
	pHead->fproot = Cast(dword, flength);
	pHead->root->fpNode = pHead->fproot;
	pHead->root->head = pHead;
	pHead->root->isData = TRUE;
	cnt = _write(pHead->fd, pHead->root, szFBTreeFileNode);
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < szFBTreeFileNode )
		return EIO;
	res = _FBTreeUpdateRoot(pHead);
	if ( res )
		return res;
	return 0;
}

static errno_t __stdcall
_FBTreeWriteNode(_pFBTreeFileNode pNode)
{
	_pFBTreeFileHead pHead = pNode->head;
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
#endif
	dword sz;

	if ( pHead->isBatch )
	{
		pNode->isModified = TRUE;
		return 0;
	}
#ifdef OK_SYS_WINDOWS
	fpos = _lseeki64(pHead->fd, pNode->fpNode, SEEK_SET);
#endif
#ifdef OK_SYS_UNIX
	fpos = lseek(pHead->fd, pNode->fpNode, SEEK_SET);
#endif	
	if ( 0 > fpos )
		return errno;
	if ( Cast(qword,fpos) != pNode->fpNode )
		return EIO;
	sz = szFBTreeFileNode + (pNode->cnt * (szFBTreeFileNodeEntry + pHead->keySize));
	cnt = _write(pHead->fd, pNode, sz);
	if ( cnt < 0 )
		return errno;
	if ( cnt < Cast(int, sz) )
		return EIO;
	return 0;
}

static errno_t __stdcall
_FBTreeAppendNode(_pFBTreeFileNode pNode)
{
	_pFBTreeFileHead pHead = pNode->head;
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword flength;
	sqword flength1;
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t flength;
	off_t flength1;
	off_t fpos;
#endif
	errno_t res;
	dword sz;

	if ( pHead->isBatch )
	{
		pNode->isToAppend = TRUE;
		return 0;
	}
#ifdef OK_SYS_WINDOWS
	flength = _filelengthi64(pHead->fd);
#endif
#ifdef OK_SYS_UNIX
	flength = lseek(pHead->fd, 0, SEEK_END);
#endif
	if ( flength < 0 )
		return errno;
	flength = ((flength + pHead->pagesize - 1) / pHead->pagesize) * pHead->pagesize;
	flength1 = flength + pHead->pagesize;
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	res = _chsize( pHead->fd, (long)flength1 );
    if ( res < 0 )
        return errno;
	fpos = _lseeki64( pHead->fd, flength, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
    res = chsize( pHead->fd, flength1 );
    if ( res )
        return res;
	fpos = lseek( pHead->fd, flength, SEEK_SET );
#endif
#endif
#ifdef OK_COMP_MSC
	res = _chsize_s(pHead->fd, flength1);
	if ( res )
		return res;
	fpos = _lseeki64( pHead->fd, flength, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != flength )
		return EIO;
	pNode->fpNode = Cast(dword, flength);
	sz = szFBTreeFileNode + (pNode->cnt * (szFBTreeFileNodeEntry + pHead->keySize));
	cnt = _write(pHead->fd, pNode, sz);
	if ( cnt < 0 )
		return errno;
	if ( cnt < Cast(int, sz) )
		return EIO;
	return 0;
}

TListCnt __stdcall
FBTreeCount(Pointer liste)
{
	_pFBTreeFileHead pHead;

	assert(liste != NULL);
	pHead = CastAnyPtr(_FBTreeFileHead, liste);

	return pHead->nodeCount;
}

void __stdcall 
FBTreeBeginBatch(Pointer liste)
{
	_pFBTreeFileHead pHead;

	assert(liste != NULL);
	pHead = CastAnyPtr(_FBTreeFileHead, liste);

	pHead->isBatch = TRUE;
}

static errno_t __stdcall
FBTreeEndBatchHelper(_pFBTreeFileNode node)
{
	_pFBTreeFileNodeEntry p;
	_pFBTreeFileHead pHead;
	TListCnt ix;
	errno_t result;

	if ( PtrCheck(node) )
		return 0;
	if ( !(node->isData) )
	{
		pHead = node->head;
		for ( ix = 0, p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(node, szFBTreeFileNode)); 
			ix < node->cnt; 
			++ix, p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize)) )
		{
		    result = FBTreeEndBatchHelper(p->next);
			if ( result )
				return result;
			if ( NotPtrCheck(p->next) )
			{
				p->fpnext = p->next->fpNode;
				node->isModified = TRUE;
			}
		}
	}
	if ( node->isToAppend )
	{
		node->isToAppend = FALSE;
		node->isModified = FALSE;
		result = _FBTreeAppendNode(node);
		if ( result )
			return result;
	}
	if ( node->isModified )
	{
		node->isModified = FALSE;
		result = _FBTreeWriteNode(node);
		if ( result )
			return result;
	}
	return 0;
}

errno_t __stdcall 
FBTreeEndBatch(Pointer liste)
{
	_pFBTreeFileHead pHead;
	errno_t result;

	assert(liste != NULL);
	pHead = CastAnyPtr(_FBTreeFileHead, liste);

	pHead->isBatch = FALSE;

	result = FBTreeEndBatchHelper(pHead->root);
	if ( result )
		return result;
	if ( NotPtrCheck(pHead->root) )
		pHead->fproot = pHead->root->fpNode;
	result = _FBTreeUpdateRoot(pHead);
	if ( result )
		return result;
	return 0;
}

static void __stdcall
FBTreeCloseHelper(_pFBTreeFileNode node)
{
	_pFBTreeFileNodeEntry p;
	_pFBTreeFileHead pHead;
	TListCnt ix;

	if ( PtrCheck(node) )
		return;
	if ( !(node->isData) )
	{
		pHead = node->head;
		for ( ix = 0, p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(node, szFBTreeFileNode)); 
			ix < node->cnt; 
			++ix, p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize)) )
		    FBTreeCloseHelper(p->next);
	}
	TFfree(node);
}

void __stdcall 
FBTreeClose(Pointer liste)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);

	assert(liste != NULL);
	FBTreeCloseHelper(pHead->root);
	TFfree(pHead);
}

errno_t __stdcall 
FBTreeBegin(Pointer liste, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);
	_pFBTreeFileNode pNode = pHead->root;
	_pFBTreeFileNodeEntry p;
    errno_t err;

	assert(liste != NULL);
    _Lvalue(result) = _LNULL;
	if ( PtrCheck(pNode) )
	{
		if ( pHead->fproot == 0 )
			return EINVAL;
        err = _FBTreeLoadRoot(pHead);
		if ( err )
			return err;
		pNode = pHead->root;
	}
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
        err = _FBTreeLoadNext(pHead, pNode, p);
		if ( err )
			return err;
		pNode = p->next;
	}
	if ( 0 == pNode->cnt )
		return EINVAL;
	_LPnode(result) = pNode;
	_LPoffset(result) = 0;
	return 0;
}

errno_t __stdcall 
FBTreeNext(LSearchResultType node, Ptr(LSearchResultType) result)
{
	_pFBTreeFileNode pNode = CastAnyPtr(_FBTreeFileNode, _Lnode(node));
	_pFBTreeFileNode pNode1;
	_pFBTreeFileHead pHead;
	_pFBTreeFileNodeEntry p;
	TListCnt pos;
    errno_t err;

    _Lvalue(result) = _LNULL;
	if ( PtrCheck(pNode) )
		return EINVAL;
	if ( _Loffset(node) < (Cast(TListIndex,pNode->cnt) - 1) )
	{
		_LPnode(result) = _Lnode(node);
		_LPoffset(result) = _Loffset(node) + 1;
		return 0;
	}
	if ( PtrCheck(pNode->parent) )
		return 0;
	pHead = pNode->head;
	while ( NotPtrCheck(pNode->parent) )
	{
		pNode1 = pNode->parent;
		p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode1, szFBTreeFileNode));
		for ( pos = 0; pos < pNode1->cnt; ++pos )
		{
			if ( p->next == pNode )
				break;
			p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize));
		}
		assert(pos < pNode1->cnt);
		if ( ++pos < pNode1->cnt )
			break;
		pNode = pNode1;
	}
	if ( PtrCheck(pNode->parent) )
		return 0;
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize));
    err = _FBTreeLoadNext(pHead, pNode->parent, p);
	if ( err )
		return err;
	pNode = p->next;
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
        err = _FBTreeLoadNext(pHead, pNode, p);
		if ( err )
			return err;
		pNode = p->next;
	}
	_LPnode(result) = pNode;
	_LPoffset(result) = 0;
	return 0;
}

errno_t __stdcall 
FBTreePrev(LSearchResultType node, Ptr(LSearchResultType) result)
{
	_pFBTreeFileNode pNode = CastAnyPtr(_FBTreeFileNode, _Lnode(node));
	_pFBTreeFileNode pNode1;
	_pFBTreeFileHead pHead;
	_pFBTreeFileNodeEntry p;
	TListCnt pos;
    errno_t err;

    _Lvalue(result) = _LNULL;
	if ( PtrCheck(pNode) )
		return EINVAL;
	if ( _Loffset(node) > 0 )
	{
		_LPnode(result) = _Lnode(node);
		_LPoffset(result) = _Loffset(node) - 1;
		return 0;
	}
	if ( PtrCheck(pNode->parent) )
		return 0;
	pHead = pNode->head;
	while ( NotPtrCheck(pNode->parent) )
	{
		pNode1 = pNode->parent;
		p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode1, szFBTreeFileNode));
		for ( pos = 0; pos < pNode1->cnt; ++pos )
		{
			if ( p->next == pNode )
				break;
			p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize));
		}
		assert(pos < pNode1->cnt);
		if ( pos > 0 )
		{
			--pos;
			break;
		}
		pNode = pNode1;
	}
	if ( PtrCheck(pNode->parent) )
		return 0;
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, -Cast(sdword, (szFBTreeFileNodeEntry + pHead->keySize))));
    err = _FBTreeLoadNext(pHead, pNode->parent, p);
	if ( err )
		return err;
	pNode = p->next;
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAnyPtr(_FBTreeFileNodeEntry, 
			_l_ptradd(pNode, szFBTreeFileNode + ((pNode->cnt - 1) * (szFBTreeFileNodeEntry + pHead->keySize))));
        err = _FBTreeLoadNext(pHead, pNode, p);
		if ( err )
			return err;
		pNode = p->next;
	}
	_LPnode(result) = pNode;
	_LPoffset(result) = pNode->cnt - 1;
	return 0;
}

errno_t __stdcall 
FBTreeLast(Pointer liste, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);
	_pFBTreeFileNode pNode = pHead->root;
	_pFBTreeFileNodeEntry p;
    errno_t err;

	assert(liste != NULL);
    _Lvalue(result) = _LNULL;
	if ( PtrCheck(pNode) )
	{
		if ( pHead->fproot == 0 )
			return EINVAL;
        err = _FBTreeLoadRoot(pHead);
		if ( err )
			return err;
		pNode = pHead->root;
	}
	while ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		p = CastAnyPtr(_FBTreeFileNodeEntry, 
			_l_ptradd(pNode, szFBTreeFileNode + ((pNode->cnt - 1) * (szFBTreeFileNodeEntry + pHead->keySize))));
        err = _FBTreeLoadNext(pHead, pNode, p);
		if ( err )
			return err;
		pNode = p->next;
	}
	if ( 0 == pNode->cnt )
		return EINVAL;
	_LPnode(result) = pNode;
	_LPoffset(result) = pNode->cnt - 1;
	return 0;
}

static errno_t __stdcall 
FBTreeFindSortedHelper(_pFBTreeFileNode pNode, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead;
	_pFBTreeFileNodeEntry p;
	sdword ix;
    errno_t err;

	pHead = pNode->head;
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
	if ( pNode->isData )
	{
		ix = _ls_ubsearch( p, szFBTreeFileNodeEntry + pHead->keySize, data, pNode->cnt, sortFunc, context, UTLPTR_MATCHMODE );
		if ( ix < 0 )
			return 0;
		_LPnode(result) = pNode;
		_LPoffset(result) = ix;
		return 0;
	}
	assert(pNode->cnt > 0);
	for ( ix = 0; ix < Cast(sdword,pNode->cnt); 
		++ix, p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize)) )
	{
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(_l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize), data, context)) )
		{
            err = _FBTreeLoadNext(pHead, pNode, p);
			if ( err )
				return err;
			return FBTreeFindSortedHelper(p->next, data, sortFunc, context, result);
		}
	}
	return 0;
}

errno_t __stdcall 
FBTreeFindSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);
	_pFBTreeFileNode pNode = pHead->root;
    errno_t err;

	assert(liste != NULL);
	assert(sortFunc != NULL);
    _Lvalue(result) = _LNULL;
	if ( PtrCheck(pNode) )
	{
		if ( pHead->fproot == 0 )
			return EINVAL;
        err = _FBTreeLoadRoot(pHead);
		if ( err )
			return err;
		pNode = pHead->root;
	}
	if ( PtrCheck(pNode) )
		return EINVAL;
	return FBTreeFindSortedHelper(pNode, data, sortFunc, context, result);
}

static errno_t __stdcall 
FBTreeUpperBoundHelper(_pFBTreeFileNode pNode, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead;
	_pFBTreeFileNodeEntry p;
	sdword ix;
    errno_t err;

	pHead = pNode->head;
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
	if ( pNode->isData )
	{
		ix = _ls_ubsearch( p, szFBTreeFileNodeEntry + pHead->keySize, data, pNode->cnt, sortFunc, context, UTLPTR_INSERTMODE );
		if ( ix < 0 )
			return 0;
		_LPnode(result) = pNode;
		_LPoffset(result) = ix;
		return 0;
	}
	assert(pNode->cnt > 0);
	for ( ix = 0; ix < Cast(sdword,pNode->cnt); 
		++ix, p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize)) )
	{
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(_l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize), data, context)) )
		{
            err = _FBTreeLoadNext(pHead, pNode, p);
			if ( err )
				return err;
			return FBTreeUpperBoundHelper(p->next, data, sortFunc, context, result);
		}
	}
	return 0;
}

errno_t __stdcall 
FBTreeUpperBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);
	_pFBTreeFileNode pNode = pHead->root;
    errno_t err;
    
	assert(liste != NULL);
	assert(sortFunc != NULL);
    _Lvalue(result) = _LNULL;
	if ( PtrCheck(pNode) )
	{
		if ( pHead->fproot == 0 )
			return EINVAL;
        err = _FBTreeLoadRoot(pHead);
		if ( err )
			return err;
		pNode = pHead->root;
	}
	if ( PtrCheck(pNode) )
		return EINVAL;
	return FBTreeUpperBoundHelper(pNode, data, sortFunc, context, result);
}

static errno_t __stdcall 
FBTreeLowerBoundHelper(_pFBTreeFileNode pNode, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead;
	_pFBTreeFileNodeEntry p;
	sdword ix;
    errno_t err;

    pHead = pNode->head;
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
	if ( pNode->isData )
	{
		ix = _ls_ubsearch( p, szFBTreeFileNodeEntry + pHead->keySize, data, pNode->cnt, sortFunc, context, UTLPTR_SEARCHMODE );
		if ( ix < 0 )
			return 0;
		_LPnode(result) = pNode;
		_LPoffset(result) = ix;
		return 0;
	}
	assert(pNode->cnt > 0);
	for ( ix = 0; ix < Cast(sdword,pNode->cnt); 
		++ix, p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize)) )
	{
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(_l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize), data, context)) )
		{
            err = _FBTreeLoadNext(pHead, pNode, p);
			if ( err )
				return err;
			return FBTreeLowerBoundHelper(p->next, data, sortFunc, context, result);
		}
	}
	return 0;
}

errno_t __stdcall 
FBTreeLowerBound(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);
	_pFBTreeFileNode pNode = pHead->root;
    errno_t err;

	assert(liste != NULL);
	assert(sortFunc != NULL);
    _Lvalue(result) = _LNULL;
	if ( PtrCheck(pNode) )
	{
		if ( pHead->fproot == 0 )
			return EINVAL;
        err = _FBTreeLoadRoot(pHead);
		if ( err )
			return err;
		pNode = pHead->root;
	}
	if ( PtrCheck(pNode) )
		return EINVAL;
	return FBTreeLowerBoundHelper(pNode, data, sortFunc, context, result);
}

static void __stdcall
FBTreeSortedTester1(_pFBTreeFileNode pNode, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pFBTreeFileHead pHead;
	_pFBTreeFileNodeEntry p;
	_pFBTreeFileNodeEntry p1;
	TListCnt ix;

	if ( PtrCheck(pNode) )
		return;
	pHead = pNode->head;
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
	if ( !(pNode->isData) )
	{
		assert(pNode->cnt > 0);
		for ( ix = 0, p1 = p; 
			ix < pNode->cnt; 
			++ix, p1 =  CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize)) )
		{
			assert(_FBTreeLoadNext(pHead, pNode, p1) == 0);
			FBTreeSortedTester1(p1->next, sortFunc, context);
		}
	}
	for ( ix = 1, p1 =  CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize));
		ix < pNode->cnt;
		++ix, p1 =  CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize)) )
	{
#if defined(_MSC_VER) && defined(_DEBUG)
		if ( 0 < sortFunc(p, p1, context) )
			_CrtDbgBreak();
#endif
		assert(0 >= sortFunc(p, p1, context));
		p = p1;
	}
}

void __stdcall
FBTreeSortedTester(Pointer liste, TSearchAndSortUserFunc sortFunc, Pointer context)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);
	_pFBTreeFileNode pNode = pHead->root;
	LSearchResultType result = _LNULL;
	LSearchResultType result1 = _LNULL;
	Pointer d1;
	Pointer d2;
    errno_t err;

	if ( PtrCheck(pNode) )
	{
		if ( pHead->fproot == 0 )
			return;
		assert( _FBTreeLoadRoot(pHead) == 0 );
		pNode = pHead->root;
	}
	FBTreeSortedTester1(pNode, sortFunc, context);
	err = FBTreeBegin(liste, &result);
    assert(err == 0);
	if ( !LPtrCheck(result) )
	{
		d1 = FBTreeGetData(result);
		err = FBTreeNext(result, &result1);
        assert(err == 0);
        result = result1;
	}
	while ( !LPtrCheck(result) )
	{
		d2 = FBTreeGetData(result);
		assert( 0 >= sortFunc(d1, d2, context) );
		err = FBTreeNext(result, & result1);
        assert(err == 0);
        result = result1;
		d1 = d2;
	}
}

static errno_t __stdcall
_FBTreeUpdateKeysHelper(_pFBTreeFileNode pNode)
{
	_pFBTreeFileHead pHead = pNode->head;
	_pFBTreeFileNode pNode1;
	_pFBTreeFileNode pNode2;
	_pFBTreeFileNodeEntry p1;
	TListCnt ix;
	errno_t result = 0;

	pNode1 = pNode;
	while ( NotPtrCheck(pNode1->parent) )
	{
		pNode2 = pNode1->parent;
		p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode2, szFBTreeFileNode));
		for ( ix = 0; ix < pNode2->cnt; ++ix )
		{
			if ( p1->next == pNode1 )
				break;
			p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize));
		}
		assert(ix < pNode2->cnt);
		s_memcpy_s(_l_ptradd(p1, szFBTreeFileNodeEntry), pHead->keySize, 
			_l_ptradd(pNode1, szFBTreeFileNode + szFBTreeFileNodeEntry), pHead->keySize);
		result = _FBTreeWriteNode(pNode2);
		if ( result )
			return result;
		if ( ix != 0 )
			break;
		pNode1 = pNode1->parent;
	}
	return 0;
}

static _pFBTreeFileNode __stdcall
FBTreeInsertSortedHelper(_pFBTreeFileNode pNode, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(LSearchResultType) pResult, Ptr(errno_t) err)
{
	_pFBTreeFileHead pHead;
	_pFBTreeFileNode pNode1 = NULL;
	_pFBTreeFileNode pNode2;
	_pFBTreeFileNodeEntry p;
	_pFBTreeFileNodeEntry p1;
	sword ix;
	word cnt;
	word sz;

	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
	pHead = pNode->head;
	if ( pNode->isData )
	{
		sz = szFBTreeFileNodeEntry + pHead->keySize;
		ix = s_ubsearch(p, sz, data, pNode->cnt, sortFunc, context, UTLPTR_INSERTMODE);
		ix = s_insert( p, sz, ix, data, &(pNode->cnt) );
		_LPnode(pResult) = pNode;
		_LPoffset(pResult) = ix;
		if ( ix == 0 )
		{
            DerefPtr(errno_t, err) = _FBTreeUpdateKeysHelper(pNode);
			if ( DerefPtr(errno_t, err) )
			{
				_Lvalue(pResult) = _LNULL;
				return NULL;
			}
		}
		++(pHead->nodeCount);
        DerefPtr(errno_t, err) = _FBTreeUpdateRoot(pHead);
		if ( DerefPtr(errno_t, err) )
		{
			_Lvalue(pResult) = _LNULL;
			return NULL;
		}
		if ( pNode->cnt >= pHead->maxEntriesPerNode )
		{
			sz = szFBTreeFileNode + (pHead->maxEntriesPerNode * (szFBTreeFileNodeEntry + pHead->keySize));
			pNode2 = CastAnyPtr(_FBTreeFileNode, TFalloc(sz));
			if ( PtrCheck(pNode2) )
			{
                DerefPtr(errno_t, err) = ENOMEM;
				_Lvalue(pResult) = _LNULL;
				return NULL;
			}
			pNode2->head = pNode->head;
			p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode2, szFBTreeFileNode));
			cnt = pNode->cnt / 2;
			sz = cnt * (szFBTreeFileNodeEntry + pHead->keySize);
			s_memcpy_s(p1, sz, _l_ptradd(p, (pNode->cnt - cnt) * (szFBTreeFileNodeEntry + pHead->keySize)), sz);
			pNode->cnt -= cnt;
			pNode2->cnt += cnt;
			pNode2->isData = TRUE;
            DerefPtr(errno_t, err) = _FBTreeWriteNode(pNode);
			if ( DerefPtr(errno_t, err) )
			{
				_Lvalue(pResult) = _LNULL;
				return NULL;
			}
			if ( _LPoffset(pResult) >= Cast(TListIndex, pNode->cnt) )
			{
				_LPnode(pResult) = pNode2;
				_LPoffset(pResult) -= pNode->cnt;
			}
			return pNode2;
		}
        DerefPtr(errno_t, err) = _FBTreeWriteNode(pNode);
		if ( DerefPtr(errno_t, err) )
		{
			_Lvalue(pResult) = _LNULL;
			return NULL;
		}
		return NULL;
	}
	assert(pNode->cnt > 0);
	for ( ix = 0, p1 = p; ix < Cast(sdword,pNode->cnt); ++ix, p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize)) )
	{
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(_l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize), data, context)) )
		{
            DerefPtr(errno_t, err) = _FBTreeLoadNext(pHead, pNode, p1);
			if ( DerefPtr(errno_t, err) )
			{
				_Lvalue(pResult) = _LNULL;
				return NULL;
			}
			pNode1 = FBTreeInsertSortedHelper(p1->next, data, sortFunc, context, pResult, err);
			if ( PtrCheck(pNode1) )
				return NULL;
			pNode1->parent = pNode;
            DerefPtr(errno_t, err) = _FBTreeAppendNode(pNode1);
			if ( DerefPtr(errno_t, err) )
			{
				_Lvalue(pResult) = _LNULL;
				return NULL;
			}
			//++ix; s_insert does already increment ix
			break;
		}
	}
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, TFalloc(szFBTreeFileNodeEntry + pHead->keySize));
	if ( PtrCheck(p1) )
	{
        DerefPtr(errno_t, err) = ENOMEM;
		_Lvalue(pResult) = _LNULL;
		return NULL;
	}
	p1->next = pNode1;
	p1->fpnext = pNode1->fpNode;
	s_memcpy_s(_l_ptradd(p1, szFBTreeFileNodeEntry), pHead->keySize,
		_l_ptradd(pNode1, szFBTreeFileNode + szFBTreeFileNodeEntry), pHead->keySize);
	s_insert(p, szFBTreeFileNodeEntry + pHead->keySize, ix, p1, &(pNode->cnt));
	TFfree(p1);
	if ( pNode->cnt >= pNode->head->maxEntriesPerNode )
	{
		sz = szFBTreeFileNode + (pHead->maxEntriesPerNode * (szFBTreeFileNodeEntry + pHead->keySize));
		pNode2 = CastAnyPtr(_FBTreeFileNode, TFalloc(sz));
		if ( PtrCheck(pNode2) )
		{
            DerefPtr(errno_t, err) = ENOMEM;
			_Lvalue(pResult) = _LNULL;
			return NULL;
		}
		pNode2->head = pNode->head;
		p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode2, szFBTreeFileNode));
		cnt = pNode->cnt / 2;
		sz = cnt * (szFBTreeFileNodeEntry + pHead->keySize);
		s_memcpy_s(p1, sz, _l_ptradd(p, (pNode->cnt - cnt) * (szFBTreeFileNodeEntry + pHead->keySize)), sz);
		pNode->cnt -= cnt;
		pNode2->cnt += cnt;
        DerefPtr(errno_t, err) = _FBTreeWriteNode(pNode);
		if ( DerefPtr(errno_t, err) )
		{
			_Lvalue(pResult) = _LNULL;
			return NULL;
		}
		for ( ix = 0; ix < Cast(sdword, pNode2->cnt); 
			++ix, p1 = CastAnyPtr(_FBTreeFileNodeEntry, 
			_l_ptradd(p1, (szFBTreeFileNodeEntry + pHead->keySize))) )
		{
            DerefPtr(errno_t, err) = _FBTreeLoadNext(pHead, pNode2, p1);
			if ( DerefPtr(errno_t, err) )
			{
				_Lvalue(pResult) = _LNULL;
				return NULL;
			}
			pNode1 = p1->next;
			pNode1->parent = pNode2;
		}
		return pNode2;
	}
    DerefPtr(errno_t, err) = _FBTreeWriteNode(pNode);
	if ( DerefPtr(errno_t, err) )
	{
		_Lvalue(pResult) = _LNULL;
		return NULL;
	}
	return NULL;
}

errno_t __stdcall 
FBTreeInsertSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(LSearchResultType) result)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);
	_pFBTreeFileNode pNode = pHead->root;
	_pFBTreeFileNode pNode2;
	_pFBTreeFileNodeEntry p;
	_pFBTreeFileNodeEntry p1;
    errno_t err;

	assert(liste != NULL);
    _Lvalue(result) = _LNULL;
	if ( PtrCheck(pNode) )
	{
		if ( pHead->fproot == 0 )
		{
            err = _FBTreeCreateRoot(pHead);
			if ( err )
				return err;
		}
		else
		{
            err = _FBTreeLoadRoot(pHead);
			if ( err )
				return err;
		}
		pNode = pHead->root;
	}
	pNode = FBTreeInsertSortedHelper(pNode, data, sortFunc, context, result, &err);
	if ( PtrCheck(pNode) ) {
		FBTreeSortedTester(liste, sortFunc, context);
		return err;
	}
	pNode2 = CastAnyPtr(_FBTreeFileNode, TFalloc(szFBTreeFileNode + (pHead->maxEntriesPerNode * (szFBTreeFileNodeEntry + pHead->keySize))));
	pNode2->head = pHead;
	pNode2->cnt = 2;
	pNode->parent = pNode2;
    err = _FBTreeAppendNode(pNode);
	if ( err )
	{
		_Lvalue(result) = _LNULL;
		return err; 
	}
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode2, szFBTreeFileNode));
	p->next = pHead->root;
	p->fpnext = pHead->root->fpNode;
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pHead->root, szFBTreeFileNode));
	s_memcpy_s(_l_ptradd(p, szFBTreeFileNodeEntry), pHead->keySize,
		_l_ptradd(p1, szFBTreeFileNodeEntry), pHead->keySize);
	pHead->root->parent = pNode2;
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, szFBTreeFileNodeEntry + pHead->keySize));
	p->next = pNode;
	p->fpnext = pNode->fpNode;
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
	s_memcpy_s(_l_ptradd(p, szFBTreeFileNodeEntry), pHead->keySize,
		_l_ptradd(p1, szFBTreeFileNodeEntry), pHead->keySize);
    err = _FBTreeAppendNode(pNode2);
	if ( err )
	{
		_Lvalue(result) = _LNULL;
		return err; 
	}
	pHead->root = pNode2;
	pHead->fproot = pNode2->fpNode;
    err = _FBTreeUpdateRoot(pHead);
	if ( err )
	{
		_Lvalue(result) = _LNULL;
		return err; 
	}
#ifdef OK_SYS_WINDOWS
	if (!(pHead->isBatch))
	{
		if ( 0 > _commit( pHead->fd ) )
		{
			_Lvalue(result) = _LNULL;
			return errno; 
		}
	}
#endif
	FBTreeSortedTester(liste, sortFunc, context);
	return 0;
}

static errno_t __stdcall
FBTreeRemoveJoinHelper(_pFBTreeFileNode pNodeDest, _pFBTreeFileNode pNodeSrc)
{
	_pFBTreeFileHead pHead = pNodeDest->head;
	_pFBTreeFileNodeEntry pDest = CastAnyPtr(_FBTreeFileNodeEntry, 
		_l_ptradd(pNodeDest, szFBTreeFileNode + (pNodeDest->cnt * (szFBTreeFileNodeEntry + pHead->keySize))));
	_pFBTreeFileNodeEntry pSrc = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNodeSrc, szFBTreeFileNode));
	errno_t result;

	s_memcpy_s(pDest, (pHead->maxEntriesPerNode - pNodeDest->cnt) * (szFBTreeFileNodeEntry + pHead->keySize),
		pSrc, pNodeSrc->cnt * (szFBTreeFileNodeEntry + pHead->keySize));
	pNodeDest->cnt += pNodeSrc->cnt;
	if ( !(pNodeDest->isData) )
	{
		_pFBTreeFileNode pNode;
		TListCnt ix;

		for ( ix = 0; ix < pNodeSrc->cnt; ++ix, 
			pDest = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pDest, szFBTreeFileNodeEntry + pHead->keySize)) )
		{
			result = _FBTreeLoadNext(pHead, pNodeDest, pDest);
			if ( result )
				return result;
			pNode = pDest->next;
			pNode->parent = pNodeDest;
		}
	}
	result = _FBTreeWriteNode(pNodeDest);
	if ( result )
		return result;
	TFfree(pNodeSrc);
	return 0;
}

static _pFBTreeFileNode __stdcall
FBTreeRemoveSortedHelper(_pFBTreeFileNode pNode, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(bool) pResult, Ptr(errno_t) pErr)
{
	_pFBTreeFileHead pHead = pNode->head;
	_pFBTreeFileNode pNode1;
	_pFBTreeFileNode pNode2;
	_pFBTreeFileNodeEntry p;
	_pFBTreeFileNodeEntry p1;
	_pFBTreeFileNodeEntry p2;
	sword pos;
	sword pos1;
	word ix;
	bool b;

	assert(pNode->cnt > 0);
	p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
	if ( pNode->isData )
	{
		DerefPtr(bool,pResult) = true;
		pos = s_ubsearch( p, szFBTreeFileNodeEntry + pHead->keySize, data, pNode->cnt, sortFunc, context, UTLPTR_MATCHMODE );
		if ( pos < 0 )
		{
			DerefPtr(bool,pResult) = false;
			return NULL;
		}
		p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, pos * (szFBTreeFileNodeEntry + pHead->keySize)));
		p2 = CastAnyPtr(_FBTreeFileNodeEntry, CastMutable(Pointer, data));
		if ( p1->fpnext != p2->fpnext )
		{
			b = false;
			pos1 = pos - 1;
			while ( pos1 >= 0 )
			{
				p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, pos1 * (szFBTreeFileNodeEntry + pHead->keySize)));
				if ( sortFunc(p1, data, context) != 0 )
					break;
				if ( p1->fpnext == p2->fpnext )
				{
					pos = pos1;
					b = true;
					break;
				}
				--pos1;
			}
			if ( !b )
			{
				pos1 = pos + 1;
				while ( pos1 < pNode->cnt )
				{
					p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p, pos1 * (szFBTreeFileNodeEntry + pHead->keySize)));
					if ( sortFunc(p1, data, context) != 0 )
						break;
					if ( p1->fpnext == p2->fpnext )
					{
						pos = pos1;
						b = true;
						break;
					}
					++pos1;
				}
			}
			if ( !b )
			{
				DerefPtr(bool,pResult) = false;
				return NULL;
			}
		}
		s_delete( p, szFBTreeFileNodeEntry + pHead->keySize, pos, &(pNode->cnt) );
		DerefPtr(errno_t, pErr) = _FBTreeWriteNode(pNode);
		if ( DerefPtr(errno_t, pErr) )
		{
			DerefPtr(bool,pResult) = false;
			return NULL;
		}
		if ( (pos == 0) && (pNode->cnt > 0) )
		{
			DerefPtr(errno_t, pErr) = _FBTreeUpdateKeysHelper(pNode);
			if ( DerefPtr(errno_t, pErr) )
			{
				DerefPtr(bool,pResult) = false;
				return NULL;
			}
		}
		--(pHead->nodeCount);
		DerefPtr(errno_t, pErr) = _FBTreeUpdateRoot(pHead);
		if ( DerefPtr(errno_t, pErr) )
		{
			DerefPtr(bool,pResult) = false;
			return NULL;
		}
		if ( pNode->cnt < (pHead->maxEntriesPerNode / 2) )
			return pNode;
		return NULL;
	}
	for ( ix = 0, p1 = p; ix < pNode->cnt; ++ix, 
		p1 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize)) )
	{
		if ( ((ix + 1) == pNode->cnt) || (0 < sortFunc(_l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize), data, context)) )
		{
			DerefPtr(errno_t, pErr) = _FBTreeLoadNext(pHead, pNode, p1);
			if ( DerefPtr(errno_t, pErr) )
			{
				DerefPtr(bool,pResult) = false;
				return NULL;
			}
			pNode1 = FBTreeRemoveSortedHelper(p1->next, data, sortFunc, context, pResult, pErr);
			if ( PtrCheck(pNode1) )
				return NULL;
			assert(pNode1 == p1->next);
			if ( (pNode->cnt < 2) && (pNode1->cnt == 0) )
			{
				TFfree(pNode1);
				s_delete( p, szFBTreeFileNodeEntry + pHead->keySize, ix, &(pNode->cnt) );
				return pNode;
			}
			if ( ix < (pNode->cnt - 1) )
			{
				p2 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p1, szFBTreeFileNodeEntry + pHead->keySize));
				DerefPtr(errno_t, pErr) = _FBTreeLoadNext(pHead, pNode, p2);
				if ( DerefPtr(errno_t, pErr) )
				{
					DerefPtr(bool,pResult) = false;
					return NULL;
				}
				pNode2 = p2->next;
				if ( (pNode2->cnt + pNode1->cnt) < pHead->maxEntriesPerNode )
				{
					b = ((ix == 0) && (pNode1->cnt == 0));
					DerefPtr(errno_t, pErr) = FBTreeRemoveJoinHelper(pNode1, pNode2);
					if ( DerefPtr(errno_t, pErr) )
					{
						DerefPtr(bool,pResult) = false;
						return NULL;
					}
					if ( b )
					{
						DerefPtr(errno_t, pErr) = _FBTreeUpdateKeysHelper(pNode1);
						if ( DerefPtr(errno_t, pErr) )
						{
							DerefPtr(bool,pResult) = false;
							return NULL;
						}
					}
					++ix;
					break;
				}
			}
			if ( ix > 0 )
			{
				p2 = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(p1, -Cast(sdword, szFBTreeFileNodeEntry + pHead->keySize)));
				DerefPtr(errno_t, pErr) = _FBTreeLoadNext(pHead, pNode, p2);
				if ( DerefPtr(errno_t, pErr) )
				{
					DerefPtr(bool,pResult) = false;
					return NULL;
				}
				pNode2 = p2->next;
				if ( (pNode2->cnt + pNode1->cnt) < pNode->head->maxEntriesPerNode )
				{
					b = ((ix == 1) && (pNode2->cnt == 0));
					DerefPtr(errno_t, pErr) = FBTreeRemoveJoinHelper(pNode2, pNode1);
					if ( DerefPtr(errno_t, pErr) )
					{
						DerefPtr(bool,pResult) = false;
						return NULL;
					}
					if ( b )
					{
						DerefPtr(errno_t, pErr) = _FBTreeUpdateKeysHelper(pNode2);
						if ( DerefPtr(errno_t, pErr) )
						{
							DerefPtr(bool,pResult) = false;
							return NULL;
						}
					}
					break;
				}
			}
			return NULL;
		}
	}
	if ( ix < pNode->cnt )
	{
		s_delete( p, szFBTreeFileNodeEntry + pHead->keySize, ix, &(pNode->cnt) );
		DerefPtr(errno_t, pErr) = _FBTreeWriteNode(pNode);
		if ( DerefPtr(errno_t, pErr) )
		{
			DerefPtr(bool,pResult) = false;
			return NULL;
		}
	}
	if ( pNode->cnt < (pNode->head->maxEntriesPerNode / 2) )
		return pNode;
	return NULL;
}

errno_t __stdcall 
FBTreeRemoveSorted(Pointer liste, ConstPointer data, TSearchAndSortUserFunc sortFunc, Pointer context, Ptr(bool) result)
{
	_pFBTreeFileHead pHead = CastAnyPtr(_FBTreeFileHead, liste);
	_pFBTreeFileNode pNode = pHead->root;
	_pFBTreeFileNodeEntry p;
	errno_t err = 0;
	
	assert(liste != NULL);
	assert(sortFunc != NULL);
	DerefPtr(bool, result) = false;
	if ( PtrCheck(pNode) )
	{
		if ( pHead->fproot == 0 )
			return EINVAL;
		err = _FBTreeLoadRoot(pHead);
		if ( err )
			return err;
		pNode = pHead->root;
	}
	pNode = FBTreeRemoveSortedHelper(pNode, data, sortFunc, context, result, &err);
	if ( PtrCheck(pNode) )
		return err;
	assert(pNode == pHead->root);
	while ( (!(pNode->isData)) && (pNode->cnt == 1) )
	{
		p = CastAnyPtr(_FBTreeFileNodeEntry, _l_ptradd(pNode, szFBTreeFileNode));
		pHead->root = p->next;
		pHead->fproot = p->fpnext;
		pHead->root->parent = NULL;
		TFfree(pNode);
		pNode = pHead->root;
	}
	err = _FBTreeUpdateRoot(pHead);
	if ( err )
		return err;
	return 0;
}

Pointer __stdcall 
FBTreeGetData(LSearchResultType node)
{
	_pFBTreeFileNode pNode = CastAnyPtr(_FBTreeFileNode, _Lnode(node));
	_pFBTreeFileHead pHead;

	if ( PtrCheck(pNode) || (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, pNode->cnt)) )
		return NULL;
	pHead = pNode->head;
	return _l_ptradd(pNode, szFBTreeFileNode + (_Loffset(node) * (szFBTreeFileNodeEntry + pHead->keySize)));
}

void __stdcall 
FBTreeSetData(LSearchResultType node, ConstPointer data)
{
	_pFBTreeFileNode pNode = CastAnyPtr(_FBTreeFileNode, _Lnode(node));
	_pFBTreeFileHead pHead;

	assert(pNode != NULL);
	if ( (_Loffset(node) < 0) || (_Loffset(node) >= Cast(TListIndex, pNode->cnt)) )
		return;
	pHead = pNode->head;
	s_memcpy_s(_l_ptradd(pNode, szFBTreeFileNode + (_Loffset(node) * (szFBTreeFileNodeEntry + pHead->keySize))), szFBTreeFileNodeEntry + pHead->keySize, CastMutable(Pointer, data), szFBTreeFileNodeEntry + pHead->keySize);
}

