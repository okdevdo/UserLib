/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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
#pragma once

#include "LISTE.H"

#include "CppSources.h"

class CPPSOURCES_API CDataBTree
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = BTreeNext(_result); return *this; }
		Iterator& operator--() { _result = BTreePrev(_result); return *this; }
		Pointer operator*() { return BTreeGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataBTree(DECL_FILE_LINE TListCnt maxEntriesPerNode, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE maxEntriesPerNode);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataBTree(ConstRef(CDataBTree) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataBTree(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				BTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataBTree) operator = (ConstRef(CDataBTree) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt maxEntriesPerNode) { _liste = BTreeOpen(ARGS_FILE_LINE maxEntriesPerNode); return _liste != NULL; }
	TListCnt Count() const { return BTreeCount(_liste); }
	TListCnt Height() const { return BTreeHeight(_liste); }
	void Copy(ConstRef(CDataBTree) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
	{
		if (!Release())
			Close(freeFunc, context);
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	void Close(TDeleteFunc freeFunc = NULL, Pointer context = NULL)
	{
		if (freeFunc)
			BTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			BTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			BTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = BTreeIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = BTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = BTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return BTreeForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = BTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = BTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = BTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = BTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = BTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = BTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = BTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator Append(ConstPointer data) const { Iterator it = BTreeAppend(_liste, data); return it; }
	Iterator Prepend(ConstPointer data) const { Iterator it = BTreePrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPointer data) const { Iterator it = BTreeInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPointer data) const { Iterator it = BTreeInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			BTreeRemove(node, freeFunc, context);
		else if (_deleteFunc)
			BTreeRemove(node, _deleteFunc, _deleteContext);
		else
			BTreeRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return BTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return BTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return BTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return BTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return BTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return BTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return BTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return BTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return BTreeGetData(node); }
	void SetData(Iterator node, Pointer data) const { BTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataBTree(void);
};

template <class Item>
class CDataBTreeT
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = BTreeNext(_result); return *this; }
		Iterator& operator--() { _result = BTreePrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, BTreeGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataBTreeT(DECL_FILE_LINE TListCnt maxEntriesPerNode, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE maxEntriesPerNode);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataBTreeT(ConstRef(CDataBTreeT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataBTreeT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				BTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataBTreeT) operator = (ConstRef(CDataBTreeT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt maxEntriesPerNode) { _liste = BTreeOpen(ARGS_FILE_LINE maxEntriesPerNode); return _liste != NULL; }
	TListCnt Count() const { return BTreeCount(_liste); }
	TListCnt Height() const { return BTreeHeight(_liste); }
	void Copy(ConstRef(CDataBTreeT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
	{
		if (!Release())
			Close(freeFunc, context);
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	void Close(TDeleteFunc freeFunc = NULL, Pointer context = NULL)
	{
		if (freeFunc)
			BTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			BTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			BTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = BTreeIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = BTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = BTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return BTreeForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = BTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = BTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = BTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = BTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = BTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = BTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = BTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator Append(ConstPtr(Item) data) const { Iterator it = BTreeAppend(_liste, data); return it; }
	Iterator Prepend(ConstPtr(Item) data) const { Iterator it = BTreePrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const { Iterator it = BTreeInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const { Iterator it = BTreeInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			BTreeRemove(node, freeFunc, context);
		else if (_deleteFunc)
			BTreeRemove(node, _deleteFunc, _deleteContext);
		else
			BTreeRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return BTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return BTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return BTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return BTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return BTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return BTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return BTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return BTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, BTreeGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { BTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataBTreeT(void);
};

#include "SLISTE.H"

class CPPSOURCES_API CDataSBTree
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = SBTreeNext(_result); return *this; }
		Iterator& operator--() { _result = SBTreePrev(_result); return *this; }
		Pointer operator*() { return SBTreeGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSBTree(DECL_FILE_LINE TListCnt maxEntriesPerNode, dword datasize, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE maxEntriesPerNode, datasize);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSBTree(ConstRef(CDataSBTree) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSBTree(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SBTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSBTree) operator = (ConstRef(CDataSBTree) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt maxEntriesPerNode, dword datasize) { _liste = SBTreeOpen(ARGS_FILE_LINE maxEntriesPerNode, datasize); return _liste != NULL; }
	TListCnt Count() const { return SBTreeCount(_liste); }
	TListCnt Height() const { return SBTreeHeight(_liste); }
	void Copy(ConstRef(CDataSBTree) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
	{
		if (!Release())
			Close(freeFunc, context);
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	void Close(TDeleteFunc freeFunc = NULL, Pointer context = NULL)
	{
		if (freeFunc)
			SBTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SBTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			SBTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = SBTreeIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = SBTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SBTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SBTreeForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = SBTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SBTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SBTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SBTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SBTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SBTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SBTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator Append(ConstPointer data) const { Iterator it = SBTreeAppend(_liste, data); return it; }
	Iterator Prepend(ConstPointer data) const { Iterator it = SBTreePrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPointer data) const { Iterator it = SBTreeInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPointer data) const { Iterator it = SBTreeInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			SBTreeRemove(node, freeFunc, context);
		else if (_deleteFunc)
			SBTreeRemove(node, _deleteFunc, _deleteContext);
		else
			SBTreeRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SBTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SBTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SBTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SBTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SBTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SBTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SBTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SBTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return SBTreeGetData(node); }
	void SetData(Iterator node, ConstPointer data) const { SBTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataSBTree(void);
};

template <class Item>
class CDataSBTreeT
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = SBTreeNext(_result); return *this; }
		Iterator& operator--() { _result = SBTreePrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, SBTreeGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSBTreeT(DECL_FILE_LINE TListCnt maxEntriesPerNode, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE maxEntriesPerNode);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSBTreeT(ConstRef(CDataSBTreeT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSBTreeT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SBTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSBTreeT) operator = (ConstRef(CDataSBTreeT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt maxEntriesPerNode) { _liste = SBTreeOpen(ARGS_FILE_LINE maxEntriesPerNode, sizeof(Item)); return _liste != NULL; }
	TListCnt Count() const { return SBTreeCount(_liste); }
	TListCnt Height() const { return SBTreeHeight(_liste); }
	void Copy(ConstRef(CDataSBTreeT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
	{
		if (!Release())
			Close(freeFunc, context);
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	void Close(TDeleteFunc freeFunc = NULL, Pointer context = NULL)
	{
		if (freeFunc)
			SBTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SBTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			SBTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = SBTreeIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = SBTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SBTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SBTreeForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = SBTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SBTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SBTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SBTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SBTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SBTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SBTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator Append(ConstPtr(Item) data) const { Iterator it = SBTreeAppend(_liste, data); return it; }
	Iterator Prepend(ConstPtr(Item) data) const { Iterator it = SBTreePrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const { Iterator it = SBTreeInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const { Iterator it = SBTreeInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			SBTreeRemove(node, freeFunc, context);
		else if (_deleteFunc)
			SBTreeRemove(node, _deleteFunc, _deleteContext);
		else
			SBTreeRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SBTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SBTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SBTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SBTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SBTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SBTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SBTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SBTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, SBTreeGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { SBTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataSBTreeT(void);
};


