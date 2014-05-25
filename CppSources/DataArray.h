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
 *  \file DataArray.h
 *  \brief Array (fixed size buffer) data structure.
 */
#pragma once

#include "LISTE.H"

#include "CppSources.h"

class CPPSOURCES_API CDataArray
{
public:
	enum TSortMode {
		HeapSortMode = LSORTMODE_HEAPSORT,
		QuickSortMode = LSORTMODE_QUICKSORT
	};

	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = ArrayNext(_result); return *this; }
		Iterator& operator--() { _result = ArrayPrev(_result); return *this; }
		Pointer operator*() { return ArrayGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataArray(DECL_FILE_LINE TListCnt cnt, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE cnt);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataArray(ConstRef(CDataArray) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataArray(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				ArrayClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataArray) operator = (ConstRef(CDataArray) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt) { _liste = ArrayOpen(ARGS_FILE_LINE cnt); return _liste != NULL; }
	TListCnt Count() const { return ArrayCount(_liste); }
	void Copy(ConstRef(CDataArray) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			ArrayClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			ArrayClose(_liste, _deleteFunc, _deleteContext);
		else
			ArrayClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = ArrayIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = ArrayBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = ArrayLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return ArrayForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = ArrayFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = ArrayFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = ArrayFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = ArrayUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = ArrayUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = ArrayLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = ArrayLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL, TSortMode mode = HeapSortMode) const
	{
		if (sortFunc)
			ArraySort(_liste, sortFunc, Castword(mode));
		else if (_searchAndSortFunc)
			ArraySort(_liste, _searchAndSortFunc, Castword(mode));
	}
	Iterator Append(ConstPointer data) const { Iterator it = ArrayAppend(_liste, data); return it; }
	Iterator Prepend(ConstPointer data) const { Iterator it = ArrayPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPointer data) const { Iterator it = ArrayInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPointer data) const { Iterator it = ArrayInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			ArrayRemove(node, freeFunc, context);
		else if (_deleteFunc)
			ArrayRemove(node, _deleteFunc, _deleteContext);
		else
			ArrayRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return ArrayInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return ArrayInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return ArrayRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return ArrayRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return ArrayRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return ArrayRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return ArrayRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return ArrayRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return ArrayGetData(node); }
	void SetData(Iterator node, ConstPointer data) const { ArraySetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataArray(void);
};

template <class Item>
class CDataArrayT
{
public:
	enum TSortMode {
		HeapSortMode = LSORTMODE_HEAPSORT,
		QuickSortMode = LSORTMODE_QUICKSORT
	};

	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = ArrayNext(_result); return *this; }
		Iterator& operator--() { _result = ArrayPrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, ArrayGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataArrayT(DECL_FILE_LINE TListCnt cnt, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE cnt);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataArrayT(ConstRef(CDataArrayT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataArrayT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				ArrayClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataArrayT) operator = (ConstRef(CDataArrayT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt) { _liste = ArrayOpen(ARGS_FILE_LINE cnt); return _liste != NULL; }
	TListCnt Count() const { return ArrayCount(_liste); }
	void Copy(ConstRef(CDataArrayT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			ArrayClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			ArrayClose(_liste, _deleteFunc, _deleteContext);
		else
			ArrayClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = ArrayIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = ArrayBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = ArrayLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return ArrayForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = ArrayFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = ArrayFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = ArrayFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = ArrayUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = ArrayUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = ArrayLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = ArrayLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL, TSortMode mode = HeapSortMode) const
	{
		if (sortFunc)
			ArraySort(_liste, sortFunc, Castword(mode));
		else if (_searchAndSortFunc)
			ArraySort(_liste, _searchAndSortFunc, Castword(mode));
	}
	Iterator Append(ConstPtr(Item) data) const { Iterator it = ArrayAppend(_liste, data); return it; }
	Iterator Prepend(ConstPtr(Item) data) const { Iterator it = ArrayPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const { Iterator it = ArrayInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const { Iterator it = ArrayInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			ArrayRemove(node, freeFunc, context);
		else if (_deleteFunc)
			ArrayRemove(node, _deleteFunc, _deleteContext);
		else
			ArrayRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return ArrayInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return ArrayInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return ArrayRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return ArrayRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return ArrayRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return ArrayRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return ArrayRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return ArrayRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, ArrayGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { ArraySetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataArrayT(void);
};

#include "SLISTE.H"

class CPPSOURCES_API CDataSArray
{
public:
	enum TSortMode {
		HeapSortMode = LSORTMODE_HEAPSORT,
		QuickSortMode = LSORTMODE_QUICKSORT
	};

	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = SArrayNext(_result); return *this; }
		Iterator& operator--() { _result = SArrayPrev(_result); return *this; }
		Pointer operator*() { return SArrayGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSArray(DECL_FILE_LINE TListCnt cnt, dword datasize, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE cnt, datasize);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSArray(ConstRef(CDataSArray) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSArray(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				ArrayClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSArray) operator = (ConstRef(CDataSArray) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, dword datasize) { _liste = SArrayOpen(ARGS_FILE_LINE cnt, datasize); return _liste != NULL; }
	TListCnt Count() const { return SArrayCount(_liste); }
	void Copy(ConstRef(CDataSArray) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SArrayClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SArrayClose(_liste, _deleteFunc, _deleteContext);
		else
			SArrayClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = SArrayBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SArrayLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SArrayForEach(_liste, func, context); }
	Iterator Find(Pointer data, TSearchAndSortFunc findFunc) const { Iterator it = SArrayFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SArrayFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SArrayFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SArrayUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SArrayUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SArrayLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SArrayLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL, TSortMode mode = HeapSortMode) const
	{
		if (sortFunc)
			SArraySort(_liste, sortFunc, Castword(mode));
		else if (_searchAndSortFunc)
			SArraySort(_liste, _searchAndSortFunc, Castword(mode));
	}
	Iterator Append(ConstPointer data) const { Iterator it = SArrayAppend(_liste, data); return it; }
	Iterator Prepend(ConstPointer data) const { Iterator it = SArrayPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPointer data) const { Iterator it = SArrayInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPointer data) const { Iterator it = SArrayInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			SArrayRemove(node, freeFunc, context);
		else if (_deleteFunc)
			SArrayRemove(node, _deleteFunc, _deleteContext);
		else
			SArrayRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SArrayInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SArrayInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SArrayRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SArrayRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SArrayRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SArrayRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SArrayRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SArrayRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return SArrayGetData(node); }
	void SetData(Iterator node, ConstPointer data) const { SArraySetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataSArray(void);
};

template <class Item>
class CDataSArrayT
{
public:
	enum TSortMode {
		HeapSortMode = LSORTMODE_HEAPSORT,
		QuickSortMode = LSORTMODE_QUICKSORT
	};

	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = SArrayNext(_result); return *this; }
		Iterator& operator--() { _result = SArrayPrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, SArrayGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSArrayT(DECL_FILE_LINE TListCnt cnt, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE cnt);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSArrayT(ConstRef(CDataSArrayT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSArrayT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				ArrayClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSArrayT) operator = (ConstRef(CDataSArrayT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt) { _liste = SArrayOpen(ARGS_FILE_LINE cnt, sizeof(Item)); return _liste != NULL; }
	TListCnt Count() const { return SArrayCount(_liste); }
	void Copy(ConstRef(CDataSArrayT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SArrayClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SArrayClose(_liste, _deleteFunc, _deleteContext);
		else
			SArrayClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = SArrayBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SArrayLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SArrayForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = SArrayFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SArrayFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SArrayFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SArrayUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SArrayUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SArrayLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SArrayLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL, TSortMode mode = HeapSortMode) const
	{
		if (sortFunc)
			SArraySort(_liste, sortFunc, Castword(mode));
		else if (_searchAndSortFunc)
			SArraySort(_liste, _searchAndSortFunc, Castword(mode));
	}
	Iterator Append(ConstPtr(Item) data) const { Iterator it = SArrayAppend(_liste, data); return it; }
	Iterator Prepend(ConstPtr(Item) data) const { Iterator it = SArrayPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const { Iterator it = SArrayInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const { Iterator it = SArrayInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			SArrayRemove(node, freeFunc, context);
		else if (_deleteFunc)
			SArrayRemove(node, _deleteFunc, _deleteContext);
		else
			SArrayRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SArrayInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SArrayInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SArrayRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SArrayRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SArrayRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SArrayRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SArrayRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SArrayRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, SArrayGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { SArraySetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataSArrayT(void);
};

