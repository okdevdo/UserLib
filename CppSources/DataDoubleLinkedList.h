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
/**
 *  \file DataDoubleLinkedList.h
 *  \brief Double linked list data structure.
 */
#pragma once

#include "LISTE.H"

#include "CppSources.h"

class CPPSOURCES_API CDataDoubleLinkedList
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = DoubleLinkedListNext(_result); return *this; }
		Iterator& operator--() { _result = DoubleLinkedListPrev(_result); return *this; }
		Pointer operator*() { return DoubleLinkedListGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataDoubleLinkedList(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataDoubleLinkedList(ConstRef(CDataDoubleLinkedList) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataDoubleLinkedList(void) 
	{
		if (_deleteFunc)
		{
			if (!Release())
				DoubleLinkedListClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataDoubleLinkedList) operator = (ConstRef(CDataDoubleLinkedList) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = DoubleLinkedListOpen(ARGS_FILE_LINE0); return _liste != NULL; }
	TListCnt Count() const { return DoubleLinkedListCount(_liste); }
	void Copy(ConstRef(CDataDoubleLinkedList) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			DoubleLinkedListClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			DoubleLinkedListClose(_liste, _deleteFunc, _deleteContext);
		else
			DoubleLinkedListClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = DoubleLinkedListIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = DoubleLinkedListBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = DoubleLinkedListLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return DoubleLinkedListForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = DoubleLinkedListFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = DoubleLinkedListFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = DoubleLinkedListFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = DoubleLinkedListUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = DoubleLinkedListUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = DoubleLinkedListLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = DoubleLinkedListLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL) const
	{
		if (sortFunc)
			DoubleLinkedListSort(_liste, sortFunc);
		else if (_searchAndSortFunc)
			DoubleLinkedListSort(_liste, _searchAndSortFunc);
	}
	Iterator Append(ConstPointer data) const { Iterator it = DoubleLinkedListAppend(_liste, data); return it; }
	Iterator Prepend(ConstPointer data) const { Iterator it = DoubleLinkedListPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPointer data) const { Iterator it = DoubleLinkedListInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPointer data) const { Iterator it = DoubleLinkedListInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			DoubleLinkedListRemove(node, freeFunc, context);
		else if (_deleteFunc)
			DoubleLinkedListRemove(node, _deleteFunc, _deleteContext);
		else
			DoubleLinkedListRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return DoubleLinkedListInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return DoubleLinkedListInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return DoubleLinkedListRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return DoubleLinkedListRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return DoubleLinkedListRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return DoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return DoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return DoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return DoubleLinkedListGetData(node); }
	void SetData(Iterator node, ConstPointer data) const { DoubleLinkedListSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};

template <class Item>
class CDataDoubleLinkedListT
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = DoubleLinkedListNext(_result); return *this; }
		Iterator& operator--() { _result = DoubleLinkedListPrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, DoubleLinkedListGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataDoubleLinkedListT(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataDoubleLinkedListT(ConstRef(CDataDoubleLinkedListT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataDoubleLinkedListT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				DoubleLinkedListClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataDoubleLinkedListT) operator = (ConstRef(CDataDoubleLinkedListT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() const { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = DoubleLinkedListOpen(ARGS_FILE_LINE0); return _liste != NULL; }
	TListCnt Count() const { return DoubleLinkedListCount(_liste); }
	void Copy(ConstRef(CDataDoubleLinkedListT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			DoubleLinkedListClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			DoubleLinkedListClose(_liste, _deleteFunc, _deleteContext);
		else
			DoubleLinkedListClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = DoubleLinkedListIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = DoubleLinkedListBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = DoubleLinkedListLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return DoubleLinkedListForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = DoubleLinkedListFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = DoubleLinkedListFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = DoubleLinkedListFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = DoubleLinkedListUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = DoubleLinkedListUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = DoubleLinkedListLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = DoubleLinkedListLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL) const
	{
		if (sortFunc)
			DoubleLinkedListSort(_liste, sortFunc);
		else if (_searchAndSortFunc)
			DoubleLinkedListSort(_liste, _searchAndSortFunc);
	}
	Iterator Append(ConstPtr(Item) data) const { Iterator it = DoubleLinkedListAppend(_liste, data); return it; }
	Iterator Prepend(ConstPtr(Item) data) const { Iterator it = DoubleLinkedListPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const { Iterator it = DoubleLinkedListInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const { Iterator it = DoubleLinkedListInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			DoubleLinkedListRemove(node, freeFunc, context);
		else if (_deleteFunc)
			DoubleLinkedListRemove(node, _deleteFunc, _deleteContext);
		else
			DoubleLinkedListRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return DoubleLinkedListInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return DoubleLinkedListInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return DoubleLinkedListRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return DoubleLinkedListRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return DoubleLinkedListRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return DoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return DoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return DoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, DoubleLinkedListGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { DoubleLinkedListSetData(node, data); }

public:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};

#include "SLISTE.H"

class CPPSOURCES_API CDataSDoubleLinkedList
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = SDoubleLinkedListNext(_result); return *this; }
		Iterator& operator--() { _result = SDoubleLinkedListPrev(_result); return *this; }
		Pointer operator*() { return SDoubleLinkedListGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSDoubleLinkedList(DECL_FILE_LINE dword datasize, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE datasize);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSDoubleLinkedList(ConstRef(CDataSDoubleLinkedList) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSDoubleLinkedList(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SDoubleLinkedListClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSDoubleLinkedList) operator = (ConstRef(CDataSDoubleLinkedList) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE dword datasize) { _liste = SDoubleLinkedListOpen(ARGS_FILE_LINE datasize); return _liste != NULL; }
	TListCnt Count() const { return SDoubleLinkedListCount(_liste); }
	void Copy(ConstRef(CDataSDoubleLinkedList) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SDoubleLinkedListClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SDoubleLinkedListClose(_liste, _deleteFunc, _deleteContext);
		else
			SDoubleLinkedListClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = SDoubleLinkedListBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SDoubleLinkedListLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SDoubleLinkedListForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = SDoubleLinkedListFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SDoubleLinkedListFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SDoubleLinkedListFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SDoubleLinkedListUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SDoubleLinkedListUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SDoubleLinkedListLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SDoubleLinkedListLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL) const
	{
		if (sortFunc)
			SDoubleLinkedListSort(_liste, sortFunc);
		else if (_searchAndSortFunc)
			SDoubleLinkedListSort(_liste, _searchAndSortFunc);
	}
	Iterator Append(ConstPointer data) const { Iterator it = SDoubleLinkedListAppend(_liste, data); return it; }
	Iterator Prepend(ConstPointer data) const { Iterator it = SDoubleLinkedListPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPointer data) const { Iterator it = SDoubleLinkedListInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPointer data) const { Iterator it = SDoubleLinkedListInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			SDoubleLinkedListRemove(node, freeFunc, context);
		else if (_deleteFunc)
			SDoubleLinkedListRemove(node, _deleteFunc, _deleteContext);
		else
			SDoubleLinkedListRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SDoubleLinkedListInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SDoubleLinkedListInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SDoubleLinkedListRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SDoubleLinkedListRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SDoubleLinkedListRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SDoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SDoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SDoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return SDoubleLinkedListGetData(node); }
	void SetData(Iterator node, ConstPointer data) const { SDoubleLinkedListSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

	CDataSDoubleLinkedList(void);
};

template <class Item>
class CDataSDoubleLinkedListT
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = SDoubleLinkedListNext(_result); return *this; }
		Iterator& operator--() { _result = SDoubleLinkedListPrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, SDoubleLinkedListGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSDoubleLinkedListT(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSDoubleLinkedListT(ConstRef(CDataSDoubleLinkedListT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSDoubleLinkedListT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SDoubleLinkedListClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSDoubleLinkedListT) operator = (ConstRef(CDataSDoubleLinkedListT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = SDoubleLinkedListOpen(ARGS_FILE_LINE sizeof(Item)); return _liste != NULL; }
	TListCnt Count() const { return SDoubleLinkedListCount(_liste); }
	void Copy(ConstRef(CDataSDoubleLinkedListT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SDoubleLinkedListClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SDoubleLinkedListClose(_liste, _deleteFunc, _deleteContext);
		else
			SDoubleLinkedListClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = SDoubleLinkedListBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SDoubleLinkedListLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SDoubleLinkedListForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = SDoubleLinkedListFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SDoubleLinkedListFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SDoubleLinkedListFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SDoubleLinkedListUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SDoubleLinkedListUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SDoubleLinkedListLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SDoubleLinkedListLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL) const
	{
		if (sortFunc)
			SDoubleLinkedListSort(_liste, sortFunc);
		else if (_searchAndSortFunc)
			SDoubleLinkedListSort(_liste, _searchAndSortFunc);
	}
	Iterator Append(ConstPtr(Item) data) const { Iterator it = SDoubleLinkedListAppend(_liste, data); return it; }
	Iterator Prepend(ConstPtr(Item) data) const { Iterator it = SDoubleLinkedListPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const { Iterator it = SDoubleLinkedListInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const { Iterator it = SDoubleLinkedListInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			SDoubleLinkedListRemove(node, freeFunc, context);
		else if (_deleteFunc)
			SDoubleLinkedListRemove(node, _deleteFunc, _deleteContext);
		else
			SDoubleLinkedListRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SDoubleLinkedListInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SDoubleLinkedListInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SDoubleLinkedListRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SDoubleLinkedListRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SDoubleLinkedListRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SDoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SDoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SDoubleLinkedListRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, SDoubleLinkedListGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { SDoubleLinkedListSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};
