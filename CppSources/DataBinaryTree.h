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
 *  \file DataBinaryTree.h
 *  \brief AVL and RB binary tree data structure.
 */
#pragma once

#include "LISTE.H"

#include "CppSources.h"

class CPPSOURCES_API CDataAVLBinaryTree
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = AVLBinaryTreeNext(_result); return *this; }
		Iterator& operator--() { _result = AVLBinaryTreePrev(_result); return *this; }
		Pointer operator*() { return AVLBinaryTreeGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataAVLBinaryTree(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataAVLBinaryTree(ConstRef(CDataAVLBinaryTree) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataAVLBinaryTree(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				AVLBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataAVLBinaryTree) operator = (ConstRef(CDataAVLBinaryTree) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = AVLBinaryTreeOpen(ARGS_FILE_LINE0); return _liste != NULL; }
	TListCnt Count() const { return AVLBinaryTreeCount(_liste); }
	TListCnt Height() const { return AVLBinaryTreeHeight(_liste); }
	void Copy(ConstRef(CDataAVLBinaryTree) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			AVLBinaryTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			AVLBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			AVLBinaryTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = AVLBinaryTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = AVLBinaryTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return AVLBinaryTreeForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = AVLBinaryTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = AVLBinaryTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = AVLBinaryTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = AVLBinaryTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = AVLBinaryTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = AVLBinaryTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = AVLBinaryTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return AVLBinaryTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return AVLBinaryTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return AVLBinaryTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return AVLBinaryTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return AVLBinaryTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return AVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return AVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return AVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return AVLBinaryTreeGetData(node); }
	void SetData(Iterator node, Pointer data) const { AVLBinaryTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};

template <class Item>
class CDataAVLBinaryTreeT
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = AVLBinaryTreeNext(_result); return *this; }
		Iterator& operator--() { _result = AVLBinaryTreePrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, AVLBinaryTreeGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataAVLBinaryTreeT(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataAVLBinaryTreeT(ConstRef(CDataAVLBinaryTreeT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataAVLBinaryTreeT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				AVLBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataAVLBinaryTreeT) operator = (ConstRef(CDataAVLBinaryTreeT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = AVLBinaryTreeOpen(ARGS_FILE_LINE0); return _liste != NULL; }
	TListCnt Count() const { return AVLBinaryTreeCount(_liste); }
	TListCnt Height() const { return AVLBinaryTreeHeight(_liste); }
	void Copy(ConstRef(CDataAVLBinaryTreeT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			AVLBinaryTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			AVLBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			AVLBinaryTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = AVLBinaryTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = AVLBinaryTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return AVLBinaryTreeForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = AVLBinaryTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = AVLBinaryTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = AVLBinaryTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = AVLBinaryTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = AVLBinaryTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = AVLBinaryTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = AVLBinaryTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return AVLBinaryTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return AVLBinaryTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return AVLBinaryTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return AVLBinaryTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return AVLBinaryTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return AVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return AVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return AVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, AVLBinaryTreeGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { AVLBinaryTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};

class CPPSOURCES_API CDataRBBinaryTree
{
public:
	class Iterator
	{
	public:
		Iterator(void) : _result(_LNULL) {}
		Iterator(LSearchResultType result) : _result(result) {}

		Iterator& operator++() { _result = RBBinaryTreeNext(_result); return *this; }
		Iterator& operator--() { _result = RBBinaryTreePrev(_result); return *this; }
		Pointer operator*() { return RBBinaryTreeGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataRBBinaryTree(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataRBBinaryTree(ConstRef(CDataRBBinaryTree) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataRBBinaryTree(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				RBBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataRBBinaryTree) operator = (ConstRef(CDataRBBinaryTree) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = RBBinaryTreeOpen(ARGS_FILE_LINE0); return _liste != NULL; }
	TListCnt Count() const { return RBBinaryTreeCount(_liste); }
	TListCnt Height() const { return RBBinaryTreeHeight(_liste); }
	void Copy(ConstRef(CDataRBBinaryTree) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			RBBinaryTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			RBBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			RBBinaryTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = RBBinaryTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if (node) ++node; return node; }
	Iterator Prev(Iterator node) const { if (node) --node; return node; }
	Iterator Last() const { Iterator it = RBBinaryTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return RBBinaryTreeForEach(_liste, func, context); }
	Iterator Find(ConstPointer data, TSearchAndSortFunc findFunc) const { Iterator it = RBBinaryTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = RBBinaryTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = RBBinaryTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = RBBinaryTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = RBBinaryTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = RBBinaryTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = RBBinaryTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return RBBinaryTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return RBBinaryTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return RBBinaryTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return RBBinaryTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return RBBinaryTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return RBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return RBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return RBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return RBBinaryTreeGetData(node); }
	void SetData(Iterator node, Pointer data) const { RBBinaryTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};

template <class Item>
class CDataRBBinaryTreeT
{
public:
	class Iterator
	{
	public:
		Iterator(void) : _result(_LNULL) {}
		Iterator(LSearchResultType result) : _result(result) {}

		Iterator& operator++() { _result = RBBinaryTreeNext(_result); return *this; }
		Iterator& operator--() { _result = RBBinaryTreePrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, RBBinaryTreeGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataRBBinaryTreeT(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataRBBinaryTreeT(ConstRef(CDataRBBinaryTreeT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataRBBinaryTreeT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				RBBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataRBBinaryTreeT) operator = (ConstRef(CDataRBBinaryTreeT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = RBBinaryTreeOpen(ARGS_FILE_LINE0); return _liste != NULL; }
	TListCnt Count() const { return RBBinaryTreeCount(_liste); }
	TListCnt Height() const { return RBBinaryTreeHeight(_liste); }
	void Copy(ConstRef(CDataRBBinaryTreeT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			RBBinaryTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			RBBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			RBBinaryTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = RBBinaryTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if (node) ++node; return node; }
	Iterator Prev(Iterator node) const { if (node) --node; return node; }
	Iterator Last() const { Iterator it = RBBinaryTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return RBBinaryTreeForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = RBBinaryTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = RBBinaryTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = RBBinaryTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = RBBinaryTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = RBBinaryTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = RBBinaryTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = RBBinaryTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return RBBinaryTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return RBBinaryTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return RBBinaryTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return RBBinaryTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return RBBinaryTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return RBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return RBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return RBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, RBBinaryTreeGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { RBBinaryTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};

#include "SLISTE.H"

class CPPSOURCES_API CDataSAVLBinaryTree
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = SAVLBinaryTreeNext(_result); return *this; }
		Iterator& operator--() { _result = SAVLBinaryTreePrev(_result); return *this; }
		Pointer operator*() { return SAVLBinaryTreeGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSAVLBinaryTree(DECL_FILE_LINE dword datasize, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE datasize);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSAVLBinaryTree(ConstRef(CDataSAVLBinaryTree) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSAVLBinaryTree(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SAVLBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSAVLBinaryTree) operator = (ConstRef(CDataSAVLBinaryTree) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE dword datasize) { _liste = SAVLBinaryTreeOpen(ARGS_FILE_LINE datasize); return _liste != NULL; }
	TListCnt Count() const { return SAVLBinaryTreeCount(_liste); }
	TListCnt Height() const { return SAVLBinaryTreeHeight(_liste); }
	void Copy(ConstRef(CDataSAVLBinaryTree) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SAVLBinaryTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SAVLBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			SAVLBinaryTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = SAVLBinaryTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SAVLBinaryTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SAVLBinaryTreeForEach(_liste, func, context); }
	Iterator Find(Pointer data, TSearchAndSortFunc findFunc) const { Iterator it = SAVLBinaryTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SAVLBinaryTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SAVLBinaryTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SAVLBinaryTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SAVLBinaryTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SAVLBinaryTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SAVLBinaryTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SAVLBinaryTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SAVLBinaryTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SAVLBinaryTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SAVLBinaryTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SAVLBinaryTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SAVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SAVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SAVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return SAVLBinaryTreeGetData(node); }
	void SetData(Iterator node, Pointer data) const { SAVLBinaryTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

	CDataSAVLBinaryTree(void);
};

template <class Item>
class CDataSAVLBinaryTreeT
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = SAVLBinaryTreeNext(_result); return *this; }
		Iterator& operator--() { _result = SAVLBinaryTreePrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, SAVLBinaryTreeGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSAVLBinaryTreeT(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSAVLBinaryTreeT(ConstRef(CDataSAVLBinaryTreeT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSAVLBinaryTreeT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SAVLBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSAVLBinaryTreeT) operator = (ConstRef(CDataSAVLBinaryTreeT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = SAVLBinaryTreeOpen(ARGS_FILE_LINE sizeof(Item)); return _liste != NULL; }
	TListCnt Count() const { return SAVLBinaryTreeCount(_liste); }
	TListCnt Height() const { return SAVLBinaryTreeHeight(_liste); }
	void Copy(ConstRef(CDataSAVLBinaryTreeT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SAVLBinaryTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SAVLBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			SAVLBinaryTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = SAVLBinaryTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = SAVLBinaryTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SAVLBinaryTreeForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = SAVLBinaryTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SAVLBinaryTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SAVLBinaryTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SAVLBinaryTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SAVLBinaryTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SAVLBinaryTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SAVLBinaryTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SAVLBinaryTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SAVLBinaryTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SAVLBinaryTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SAVLBinaryTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SAVLBinaryTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SAVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SAVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SAVLBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, SAVLBinaryTreeGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { SAVLBinaryTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};

class CPPSOURCES_API CDataSRBBinaryTree
{
public:
	class Iterator
	{
	public:
		Iterator(void) : _result(_LNULL) {}
		Iterator(LSearchResultType result) : _result(result) {}

		Iterator& operator++() { _result = SRBBinaryTreeNext(_result); return *this; }
		Iterator& operator--() { _result = SRBBinaryTreePrev(_result); return *this; }
		Pointer operator*() { return SRBBinaryTreeGetData(_result); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSRBBinaryTree(DECL_FILE_LINE dword datasize, TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE datasize);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSRBBinaryTree(ConstRef(CDataSRBBinaryTree) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSRBBinaryTree(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SRBBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSRBBinaryTree) operator = (ConstRef(CDataSRBBinaryTree) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE dword datasize) { _liste = SRBBinaryTreeOpen(ARGS_FILE_LINE datasize); return _liste != NULL; }
	TListCnt Count() const { return SRBBinaryTreeCount(_liste); }
	TListCnt Height() const { return SRBBinaryTreeHeight(_liste); }
	void Copy(ConstRef(CDataSRBBinaryTree) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SRBBinaryTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SRBBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			SRBBinaryTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = SRBBinaryTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if (node) ++node; return node; }
	Iterator Prev(Iterator node) const { if (node) --node; return node; }
	Iterator Last() const { Iterator it = SRBBinaryTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SRBBinaryTreeForEach(_liste, func, context); }
	Iterator Find(Pointer data, TSearchAndSortFunc findFunc) const { Iterator it = SRBBinaryTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SRBBinaryTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SRBBinaryTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SRBBinaryTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SRBBinaryTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPointer data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SRBBinaryTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SRBBinaryTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator InsertSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SRBBinaryTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SRBBinaryTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPointer data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SRBBinaryTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SRBBinaryTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SRBBinaryTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SRBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SRBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SRBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Pointer GetData(Iterator node) const { return SRBBinaryTreeGetData(node); }
	void SetData(Iterator node, Pointer data) const { SRBBinaryTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;

	CDataSRBBinaryTree(void);
};

template <class Item>
class CDataSRBBinaryTreeT
{
public:
	class Iterator
	{
	public:
		Iterator(void) : _result(_LNULL) {}
		Iterator(LSearchResultType result) : _result(result) {}

		Iterator& operator++() { _result = SRBBinaryTreeNext(_result); return *this; }
		Iterator& operator--() { _result = SRBBinaryTreePrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, SRBBinaryTreeGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataSRBBinaryTreeT(DECL_FILE_LINE TDeleteFunc pDeleteFunc = NULL, Pointer pDeleteContext = NULL, TSearchAndSortFunc pSearchAndSortFunc = NULL)
	{
		Open(ARGS_FILE_LINE0);
		_deleteFunc = pDeleteFunc;
		_deleteContext = pDeleteContext;
		_searchAndSortFunc = pSearchAndSortFunc;
	}
	CDataSRBBinaryTreeT(ConstRef(CDataSRBBinaryTreeT) copy)
	{
		_liste = copy._liste;
		_deleteFunc = copy._deleteFunc;
		_deleteContext = copy._deleteContext;
		_searchAndSortFunc = copy._searchAndSortFunc;
		AddRef();
	}
	~CDataSRBBinaryTreeT(void)
	{
		if (_deleteFunc)
		{
			if (!Release())
				SRBBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		}
	}

	ConstRef(CDataSRBBinaryTreeT) operator = (ConstRef(CDataSRBBinaryTreeT) copy)
	{
		Copy(copy);
		return *this;
	}

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE0) { _liste = SRBBinaryTreeOpen(ARGS_FILE_LINE sizeof(Item)); return _liste != NULL; }
	TListCnt Count() const { return SRBBinaryTreeCount(_liste); }
	TListCnt Height() const { return SRBBinaryTreeHeight(_liste); }
	void Copy(ConstRef(CDataSRBBinaryTreeT) copy, TDeleteFunc freeFunc = NULL, Pointer context = NULL)
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
			SRBBinaryTreeClose(_liste, freeFunc, context);
		else if (_deleteFunc)
			SRBBinaryTreeClose(_liste, _deleteFunc, _deleteContext);
		else
			SRBBinaryTreeClose(_liste, NULL, NULL);
		_liste = NULL;
	}
	Iterator Begin() const { Iterator it = SRBBinaryTreeBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if (node) ++node; return node; }
	Iterator Prev(Iterator node) const { if (node) --node; return node; }
	Iterator Last() const { Iterator it = SRBBinaryTreeLast(_liste); return it; }
	bool ForEach(TForEachFunc func, Pointer context = NULL) const { return SRBBinaryTreeForEach(_liste, func, context); }
	Iterator Find(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const { Iterator it = SRBBinaryTreeFind(_liste, data, findFunc); return it; }
	Iterator FindSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		Iterator it;

		if (findFunc)
			it = SRBBinaryTreeFindSorted(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SRBBinaryTreeFindSorted(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SRBBinaryTreeUpperBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SRBBinaryTreeUpperBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data, TSearchAndSortFunc findFunc) const
	{
		Iterator it;

		if (findFunc)
			it = SRBBinaryTreeLowerBound(_liste, data, findFunc);
		else if (_searchAndSortFunc)
			it = SRBBinaryTreeLowerBound(_liste, data, _searchAndSortFunc);
		return it;
	}
	Iterator InsertSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL) const
	{
		if (findFunc)
			return SRBBinaryTreeInsertSorted(_liste, data, findFunc);
		if (_searchAndSortFunc)
			return SRBBinaryTreeInsertSorted(_liste, data, _searchAndSortFunc);
		return _LNULL;
	}
	bool RemoveSorted(ConstPtr(Item) data, TSearchAndSortFunc findFunc = NULL, TDeleteFunc freeFunc = NULL, Pointer context = NULL) const
	{
		if (findFunc)
		{
			if (freeFunc)
				return SRBBinaryTreeRemoveSorted(_liste, data, findFunc, freeFunc, context);
			if (_deleteFunc)
				return SRBBinaryTreeRemoveSorted(_liste, data, findFunc, _deleteFunc, _deleteContext);
			return SRBBinaryTreeRemoveSorted(_liste, data, findFunc, NULL, NULL);
		}
		if (_searchAndSortFunc)
		{
			if (freeFunc)
				return SRBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, freeFunc, context);
			if (_deleteFunc)
				return SRBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, _deleteFunc, _deleteContext);
			return SRBBinaryTreeRemoveSorted(_liste, data, _searchAndSortFunc, NULL, NULL);
		}
		return true;
	}
	Ptr(Item) GetData(Iterator node) const { return CastAnyPtr(Item, SRBBinaryTreeGetData(node)); }
	void SetData(Iterator node, ConstPtr(Item) data) const { SRBBinaryTreeSetData(node, data); }

protected:
	Pointer _liste;
	TDeleteFunc _deleteFunc;
	Pointer _deleteContext;
	TSearchAndSortFunc _searchAndSortFunc;
};

