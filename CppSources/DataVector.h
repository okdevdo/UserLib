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

class CPPSOURCES_API CDataVector
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

		Iterator& operator++() { _result = VectorNext(_result); return *this; }
		Iterator& operator--() { _result = VectorPrev(_result); return *this; }
		Pointer operator*() { return VectorGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp,	TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL) 
	{
		Open(ARGS_FILE_LINE cnt, exp);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataVector(ConstRef(CDataVector) copy) 
	{ 
		_liste = copy._liste; 
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef(); 
	}
	~CDataVector(void) 
	{
		if (_deleteFunc)
		{
			if (!Release())
				VectorClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataVector) operator = (ConstRef(CDataVector) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp) { _liste = VectorOpen(ARGS_FILE_LINE cnt, exp); return _liste != NULL; }
	TListCnt Count() const { return (PtrCheck(_liste) ? 0 : VectorCount(_liste)); }
	void Copy(ConstRef(CDataVector) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			VectorClose(_liste, freeFunc, context); 
		else if (_deleteFunc)
			VectorClose(_liste, _deleteFunc, _deleteContext);
		else
			VectorClose(_liste, NULL, NULL);
		_liste = NULL; 
	}
	Iterator Index(TListIndex index) const { Iterator it = VectorIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = VectorBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = VectorLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return VectorForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = VectorFind(_liste, data, findFunc); return it; }
	Iterator FindUser(ConstPointer data, TSearchAndSortUserFunc findFunc, ConstPointer context) const { Iterator it = VectorFindUser(_liste, data, findFunc, context); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const 
	{ 
		Iterator it;

		if (findFunc)
			it = VectorFindSorted(_liste, data, findFunc); 
		else if (_searchAndSortFunc)
			it = VectorFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{ 
		Iterator it;

		if (findFunc)
			it = VectorUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = VectorUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{ 
		Iterator it;

		if (findFunc)
			it = VectorLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = VectorLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL, TSortMode mode = HeapSortMode) const 
	{ 
		if (sortFunc)
			VectorSort(_liste, sortFunc, Castword(mode) );
		else if (_searchAndSortFunc)
			VectorSort(_liste, _searchAndSortFunc, Castword(mode));
	}
	void SortUser(TSearchAndSortUserFunc sortFunc, ConstPointer context, TSortMode mode = HeapSortMode) const { VectorSortUser(_liste, sortFunc, context, Castword(mode) ); }
	Iterator Append(ConstPointer data) const { Iterator it = VectorAppend(_liste, data); return it; }
	Iterator Prepend(ConstPointer data) const { Iterator it = VectorPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPointer data) const { Iterator it = VectorInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPointer data) const { Iterator it = VectorInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const 
	{
		if (freeFunc)
			VectorRemove(node, freeFunc, context);
		else if (_deleteFunc)
			VectorRemove(node, _deleteFunc, _deleteContext);
		else
			VectorRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{ 
		if (findFunc)
			return VectorInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return VectorInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const 
	{ 
		if (findFunc)
		{
			if (freeFunc)
				return VectorRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return VectorRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return VectorRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}		
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return VectorRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return VectorRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return VectorRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	ConstPointer GetData(Iterator node) const { return VectorGetData(node); }
	void SetData(Iterator node, ConstPointer data) const { VectorSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataVector(void);
};

template <class Item>
class CDataVectorT
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

		Iterator& operator++() { _result = VectorNext(_result); return *this; }
		Iterator& operator--() { _result = VectorPrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, VectorGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataVectorT(DECL_FILE_LINE TListCnt cnt, TListCnt exp, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE cnt, exp);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataVectorT(ConstRef(CDataVectorT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataVectorT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				VectorClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataVectorT) operator = (ConstRef(CDataVectorT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp) { _liste = VectorOpen(ARGS_FILE_LINE cnt, exp); return _liste != NULL; }
	TListCnt Count() const { return (PtrCheck(_liste) ? 0 : VectorCount(_liste)); }
	void Copy(ConstRef(CDataVectorT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			VectorClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			VectorClose(_liste, _deleteFunc, _deleteContext);
		else
			VectorClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = VectorIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = VectorBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = VectorLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return VectorForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = VectorFind(_liste, data, findFunc); return it; }
	Iterator FindUser(ConstPtr(Item) data, TSearchAndSortUserFunc findFunc, ConstPointer context) const { Iterator it = VectorFindUser(_liste, data, findFunc, context); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = VectorFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = VectorFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = VectorUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = VectorUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = VectorLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = VectorLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL, TSortMode mode = HeapSortMode) const
	{
		if (sortFunc)
			VectorSort(_liste, sortFunc, Castword(mode));
		else if (_searchAndSortFunc)
			VectorSort(_liste, _searchAndSortFunc, Castword(mode));
	}
	void SortUser(TSearchAndSortUserFunc sortFunc, ConstPointer context, TSortMode mode = HeapSortMode) const { VectorSortUser(_liste, sortFunc, context, Castword(mode) ); }
	Iterator Append(ConstPtr(Item) data) const { Iterator it = VectorAppend(_liste, data); return it; }
	Iterator Prepend(ConstPtr(Item) data) const { Iterator it = VectorPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const { Iterator it = VectorInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const { Iterator it = VectorInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			VectorRemove(node, freeFunc, context);
		else if (_deleteFunc)
			VectorRemove(node, _deleteFunc, _deleteContext);
		else
			VectorRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return VectorInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return VectorInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return VectorRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return VectorRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return VectorRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return VectorRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return VectorRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return VectorRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	ConstPtr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, VectorGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { VectorSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataVectorT(void);
};

#include "SLISTE.H"

class CPPSOURCES_API CDataSVector
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

		Iterator& operator++() { _result = SVectorNext(_result); return *this; }
		Iterator& operator--() { _result = SVectorPrev(_result); return *this; }
		Pointer operator*() { return SVectorGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp, dword datasize, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE cnt, exp, datasize);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSVector(ConstRef(CDataSVector) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSVector(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SVectorClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSVector) operator = (ConstRef(CDataSVector) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp, dword datasize) { _liste = SVectorOpen(ARGS_FILE_LINE cnt, exp, datasize); return _liste != NULL; }
	TListCnt Count() const { return SVectorCount(_liste); }
	void Copy(ConstRef(CDataSVector) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SVectorClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SVectorClose(_liste, _deleteFunc, _deleteContext);
		else
			SVectorClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = SVectorIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = SVectorBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SVectorLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SVectorForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = SVectorFind(_liste, data, findFunc); return it; }
	Iterator FindUser(ConstPointer data, TSearchAndSortUserFunc findFunc, ConstPointer context) const { Iterator it = SVectorFindUser(_liste, data, findFunc, context); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SVectorFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SVectorFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SVectorUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SVectorUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SVectorLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SVectorLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL, TSortMode mode = HeapSortMode) const
	{
		if (sortFunc)
			SVectorSort(_liste, sortFunc, Castword(mode));
		else if (_searchAndSortFunc)
			SVectorSort(_liste, _searchAndSortFunc, Castword(mode));
	}
	void SortUser(TSearchAndSortUserFunc sortFunc, ConstPointer context, TSortMode mode = HeapSortMode) const { SVectorSortUser(_liste, sortFunc, context, Castword(mode)); }
	Iterator Append(ConstPointer data) const { Iterator it = SVectorAppend(_liste, data); return it; }
	Iterator Prepend(ConstPointer data) const { Iterator it = SVectorPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPointer data) const { Iterator it = SVectorInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPointer data) const { Iterator it = SVectorInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			SVectorRemove(node, freeFunc, context);
		else if (_deleteFunc)
			SVectorRemove(node, _deleteFunc, _deleteContext);
		else
			SVectorRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SVectorInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SVectorInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SVectorRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SVectorRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SVectorRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SVectorRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SVectorRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SVectorRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return SVectorGetData(node); }
	void SetData(Iterator node, ConstPointer data) const { SVectorSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataSVector(void);
};

template <class Item>
class CDataSVectorT
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

		Iterator& operator++() { _result = SVectorNext(_result); return *this; }
		Iterator& operator--() { _result = SVectorPrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, SVectorGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSVectorT(DECL_FILE_LINE TListCnt cnt, TListCnt exp, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE cnt, exp);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSVectorT(ConstRef(CDataSVectorT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSVectorT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SVectorClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSVectorT) operator = (ConstRef(CDataSVectorT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp) { _liste = SVectorOpen(ARGS_FILE_LINE cnt, exp, sizeof(Item)); return _liste != NULL; }
	TListCnt Count() const { return SVectorCount(_liste); }
	void Copy(ConstRef(CDataSVectorT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SVectorClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SVectorClose(_liste, _deleteFunc, _deleteContext);
		else
			SVectorClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const { Iterator it = SVectorIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = SVectorBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SVectorLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SVectorForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = SVectorFind(_liste, data, findFunc); return it; }
	Iterator FindUser(ConstPtr(Item) data, TSearchAndSortUserFunc findFunc, ConstPointer context) const { Iterator it = SVectorFindUser(_liste, data, findFunc, context); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SVectorFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SVectorFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SVectorUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SVectorUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SVectorLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SVectorLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	void Sort(TSearchAndSortFunc sortFunc = NULL, TSortMode mode = HeapSortMode) const
	{
		if (sortFunc)
			SVectorSort(_liste, sortFunc, Castword(mode));
		else if (_searchAndSortFunc)
			SVectorSort(_liste, _searchAndSortFunc, Castword(mode));
	}
	void SortUser(TSearchAndSortUserFunc sortFunc, ConstPointer context, TSortMode mode = HeapSortMode) const { SVectorSortUser(_liste, sortFunc, context, Castword(mode)); }
	Iterator Append(ConstPtr(Item) data) const { Iterator it = SVectorAppend(_liste, data); return it; }
	Iterator Prepend(ConstPtr(Item) data) const { Iterator it = SVectorPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const { Iterator it = SVectorInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const { Iterator it = SVectorInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (freeFunc)
			SVectorRemove(node, freeFunc, context);
		else if (_deleteFunc)
			SVectorRemove(node, _deleteFunc, _deleteContext);
		else
			SVectorRemove(node, NULL, NULL);
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SVectorInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SVectorInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SVectorRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SVectorRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SVectorRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SVectorRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SVectorRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SVectorRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, SVectorGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { SVectorSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

private:
	CDataSVectorT(void);
};

CPPSOURCES_API void __stdcall CDataVectorT_StringBuffer_DeleteFunc(ConstPointer data, Pointer context);
CPPSOURCES_API sword __stdcall CDataVectorT_StringBuffer_SearchAndSortFunc(ConstPointer pa, ConstPointer pb);

template <>
class CDataVectorT<CStringBuffer>
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

		Iterator& operator++() { _result = VectorNext(_result); return *this; }
		Iterator& operator--() { _result = VectorPrev(_result); return *this; }
		CStringBuffer operator*() { CStringBuffer tmp; tmp.assign(CastAny(CConstPointer, VectorGetData(_result))); CStringBuffer tmp1(tmp); tmp.release(); return tmp1; }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataVectorT(DECL_FILE_LINE TListCnt cnt, TListCnt exp): _liste(NULL) { Open(ARGS_FILE_LINE cnt, exp); }
	CDataVectorT(ConstRef(CDataVectorT) copy) { _liste = copy._liste; AddRef(); }
	~CDataVectorT(void) { if ( !Release() ) Close(); }

	ConstRef(CDataVectorT) operator = (ConstRef(CDataVectorT) copy) { Copy(copy); return *this; }

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp) { if (_liste != NULL) return false; _liste = VectorOpen(ARGS_FILE_LINE cnt, exp); return _liste != NULL; }
	void Copy(ConstRef(CDataVectorT) copy) { if ( !Release() ) Close(); _liste = copy._liste; AddRef(); }
	TListCnt Count() const { return VectorCount(_liste); }
	void Close() { if (_liste == NULL) return; VectorClose(_liste, CDataVectorT_StringBuffer_DeleteFunc, NULL); _liste = NULL; }
	Iterator Index(TListIndex index) const { Iterator it = VectorIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = VectorBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = VectorLast(_liste); return it; }
	Iterator Find(ConstRef(CStringBuffer) data) const { Iterator it = VectorFind(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortFunc); return it; }
	Iterator FindSorted(ConstRef(CStringBuffer) data) const { Iterator it = VectorFindSorted(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortFunc); return it; }
	Iterator UpperBound(ConstRef(CStringBuffer) data) const { Iterator it = VectorUpperBound(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortFunc); return it; }
	Iterator LowerBound(ConstRef(CStringBuffer) data) const { Iterator it = VectorLowerBound(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortFunc); return it; }
	void Sort(TSortMode mode = HeapSortMode) const { VectorSort(_liste, CDataVectorT_StringBuffer_SearchAndSortFunc, Castword(mode) ); }
	Iterator Append(ConstRef(CStringBuffer) data) const { Iterator it = VectorAppend(_liste, data.GetString()); data.addRef(); return it; }
	Iterator Prepend(ConstRef(CStringBuffer) data) const { Iterator it = VectorPrepend(_liste, data.GetString()); data.addRef(); return it; }
	Iterator InsertBefore(Iterator node, ConstRef(CStringBuffer) data) const { Iterator it = VectorInsertBefore(node, data.GetString()); data.addRef(); return it; }
	Iterator InsertAfter(Iterator node, ConstRef(CStringBuffer) data) const { Iterator it = VectorInsertAfter(node, data.GetString()); data.addRef(); return it; }
	void Remove(Iterator node) const { VectorRemove(node, CDataVectorT_StringBuffer_DeleteFunc, NULL); }
	Iterator InsertSorted(ConstRef(CStringBuffer) data) const { Iterator it = VectorInsertSorted(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortFunc); data.addRef(); return it; }
	bool RemoveSorted(ConstRef(CStringBuffer) data) const { return VectorRemoveSorted(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortFunc, CDataVectorT_StringBuffer_DeleteFunc, NULL); }
	CStringBuffer GetData(Iterator node) const { CStringBuffer tmp; tmp.assign(CastAny(CConstPointer, VectorGetData(node))); CStringBuffer tmp1(tmp); tmp.release(); return tmp1; }
	void SetData(Iterator node, ConstRef(CStringBuffer) data) const 
	{ 
		CStringBuffer tmp; 
		
		tmp.assign(CastAny(CConstPointer, VectorGetData(node)));
		if ( tmp.GetString() == data.GetString() )
		{
			tmp.release();
			return;
		}
		VectorSetData(node, data.GetString()); 
		data.addRef(); 
	}
	void Remove(Iterator begin, Iterator end)
	{
		if ( !begin )
			return;
		while ( begin != end )
		{
			Iterator end2 = end;

			--end2;
			Remove(end);
			end = end2;
		}
		Remove(begin);
	}
	dword Split(ConstRef(CStringBuffer) text, CStringLiteral sch, Iterator itInsert = _LNULL, bool bBefore = false) const
	{
		if ( text.IsEmpty() )
			return 0;

		CStringBuffer tmp(text);
		CStringConstIterator it(tmp);
		dword cnt = 1;
		dword schl = sch.GetLength();

		if ( schl == 0 )
		{
			if ( !itInsert )
				Append(tmp);
			else if ( bBefore )
			{
				InsertBefore(itInsert, tmp);
				++itInsert;
			}
			else
				itInsert = InsertAfter(itInsert, tmp);
			return 1;
		}

		it.Find(sch.GetString());
		while ( !(it.IsEnd()) )
		{
			++cnt;
			it += schl;
			it.Find(sch.GetString());
		}

		CArray sar = CastAny(CArray, TFalloc(cnt * szPointer));
		dword max;
		
		tmp.Split(sch.GetString(), sar, cnt, &max);
		for ( dword ix = 0; ix < max; ++ix )
		{
			CStringBuffer tmp1(__FILE__LINE__ sar[ix]);

			if ( !itInsert )
				Append(tmp1);
			else if ( bBefore )
			{
				InsertBefore(itInsert, tmp1);
				++itInsert;
			}
			else
				itInsert = InsertAfter(itInsert, tmp1);
		}
		TFfree(sar);
		return max;
	}
	CStringBuffer Join(CStringLiteral jch) const
	{
		CStringBuffer tmp;
		Iterator node = Begin();
		bool bFirst = true;

		while ( node )
		{
			if ( !bFirst )
				tmp += jch;
			else
				bFirst = false;
			tmp += GetData(node);
			++node;
		}
		return tmp;
	}
	dword GetMaxLineLength() const
	{
		CStringBuffer tmp;
		Iterator node = Begin();
		dword res = 0;
		dword l;

		while ( node )
		{
			tmp = GetData(node);
			l = tmp.GetLength();
			if ( l > res )
				res = l;
			++node;
		}
		return res;
	}
	dword GetLength() const
	{
		CStringBuffer tmp;
		Iterator node = Begin();
		dword res = 0;

		while ( node )
		{
			tmp = GetData(node);
			res += tmp.GetLength();
			++node;
		}
		return res;
	}
	dword GetSize() const
	{
		CStringBuffer tmp;
		Iterator node = Begin();
		dword res = 0;

		while ( node )
		{
			tmp = GetData(node);
			res += tmp.GetSize();
			++node;
		}
		return res;
	}
	dword GetAllocSize() const
	{
		CStringBuffer tmp;
		Iterator node = Begin();
		dword res = 0;

		while ( node )
		{
			tmp = GetData(node);
			res += tmp.GetAllocSize();
			++node;
		}
		return res;
	}

protected:
	Pointer _liste;

private:
	CDataVectorT(void);
};
