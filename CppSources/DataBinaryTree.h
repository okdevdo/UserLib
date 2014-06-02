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

template <class Item, class Lesser = CCppObjectLessFunctor<Item>, class Deleter = CCppObjectReleaseFunctor<Item> >
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

	CDataAVLBinaryTreeT(DECL_FILE_LINE RefRef(Lesser) rLesser = Lesser(), RefRef(Deleter) rDeleter = Deleter()) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE0);
	}
	CDataAVLBinaryTreeT(DECL_FILE_LINE ConstRef(Lesser) rLesser, ConstRef(Deleter) rDeleter) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE0);
	}
	CDataAVLBinaryTreeT(ConstRef(CDataAVLBinaryTreeT) copy) :
		_liste(NULL), _deleter(copy._deleter), _lesser(copy._lesser)
	{
		Copy(copy);
	}
	CDataAVLBinaryTreeT(RefRef(CDataAVLBinaryTreeT) _move) :
		_liste(_move._liste), _deleter(_move._deleter), _lesser(_move._lesser)
	{
		_move._liste = NULL;
	}
	virtual ~CDataAVLBinaryTreeT(void)
	{
		Close();
	}

	ConstRef(CDataAVLBinaryTreeT) operator = (ConstRef(CDataAVLBinaryTreeT) copy)
	{
		if (this != &copy)
		{
			Close();
			Copy(copy);
		}
		return *this;
	}
	ConstRef(CDataAVLBinaryTreeT) operator = (RefRef(CDataAVLBinaryTreeT) _move)
	{
		if (this != &_move)
		{
			_liste = _move._liste;
			_move._liste = NULL;
		}
		return *this;
	}

	bool Open(DECL_FILE_LINE0)
	{ 
		_liste = AVLBinaryTreeOpen(ARGS_FILE_LINE0); 
		return _liste != NULL; 
	}
	TListCnt Count() const 
	{ 
		return AVLBinaryTreeCount(_liste); 
	}
	TListCnt Height() const 
	{ 
		return AVLBinaryTreeHeight(_liste); 
	}
	void Copy(ConstRef(CDataAVLBinaryTreeT) copy)
	{
		Iterator it = copy.Begin();
		Open(__FILE__LINE__0);

		while (it)
		{
			Ptr(Item) p = *it;

			p->addRef();
			InsertSorted(p);
			++it;
		}
	}
	void Close()
	{
		if (!_liste)
			return;
		AVLBinaryTreeClose(_liste, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(RefRef(D) rD = D())
	{
		if (!_liste)
			return;
		AVLBinaryTreeClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(Ref(D) rD)
	{
		if (!_liste)
			return;
		AVLBinaryTreeClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	Iterator Begin() const
	{
		Iterator it = AVLBinaryTreeBegin(_liste); 
		return it; 
	}
	Iterator Next(Iterator node) const 
	{ 
		if (node)
			++node;
		return node;
	}
	Iterator Prev(Iterator node) const
	{ 
		if ( node ) 
			--node; 
		return node; 
	}
	Iterator Last() const 
	{ 
		Iterator it = AVLBinaryTreeLast(_liste); 
		return it; 
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(RefRef(D) rD = D()) const
	{
		return AVLBinaryTreeForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(Ref(D) rD) const
	{
		return AVLBinaryTreeForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, RefRef(D) rD = D()) const
	{
		Iterator it = AVLBinaryTreeFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it = AVLBinaryTreeFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	bool MatchSorted(Iterator it, ConstPtr(Item) data)
	{
		if (!it)
			return false;
		if (PtrCheck(*it))
			return false;
		if (TCppObjectSearchAndSortUserFunc<Item, Lesser>(*it, data, &_lesser) != 0)
			return false;
		return true;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	bool MatchSorted(Iterator it, ConstPtr(Item) data, Ref(D) rD)
	{
		if (!it)
			return false;
		if (PtrCheck(*it))
			return false;
		if (TCppObjectSearchAndSortUserFunc<Item, D>(*it, data, &rD) != 0)
			return false;
		return true;
	}
	Iterator FindSorted(ConstPtr(Item) data)
	{
		Iterator it;

		it = AVLBinaryTreeFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator FindSorted(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it;

		it = AVLBinaryTreeFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data) const
	{
		Iterator it;

		it = AVLBinaryTreeUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator UpperBound(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it;

		it = AVLBinaryTreeUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data) const
	{
		Iterator it;

		it = AVLBinaryTreeLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator LowerBound(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it;

		it = AVLBinaryTreeLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator InsertSorted(ConstPtr(Item) data)
	{
		return AVLBinaryTreeInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator InsertSorted(ConstPtr(Item) data, RefRef(D) rD = D()) const
	{
		return AVLBinaryTreeInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	bool RemoveSorted(ConstPtr(Item) data)
	{
		return AVLBinaryTreeRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D, typename E> // CCppObjectLessFunctor<Item>, CCppObjectReleaseFunctor<Item>
	bool RemoveSorted(ConstPtr(Item) data, RefRef(D) rD = D(), RefRef(E) rE = E()) const
	{
		return AVLBinaryTreeRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, &TCppObjectReleaseFunc<Item, E>, &rE);
	}
	Ptr(Item) GetData(Iterator node) const
	{ 
		return CastAnyPtr(Item, AVLBinaryTreeGetData(node)); 
	}
	void SetData(Iterator node, Ptr(Item) data)
	{ 
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			_deleter(CastMutablePtr(Item, p));
			AVLBinaryTreeSetData(node, data);
		}
	}

protected:
	Pointer _liste;
	Deleter _deleter;
	Lesser _lesser;

private:
	CDataAVLBinaryTreeT();
};

template <class Item, class Lesser = CCppObjectLessFunctor<Item>, class Deleter = CCppObjectReleaseFunctor<Item> >
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

	CDataRBBinaryTreeT(DECL_FILE_LINE RefRef(Lesser) rLesser = Lesser(), RefRef(Deleter) rDeleter = Deleter()) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE0);
	}
	CDataRBBinaryTreeT(DECL_FILE_LINE ConstRef(Lesser) rLesser, ConstRef(Deleter) rDeleter) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE0);
	}
	CDataRBBinaryTreeT(ConstRef(CDataRBBinaryTreeT) copy) :
		_liste(NULL), _deleter(copy._deleter), _lesser(copy._lesser)
	{
		Copy(copy);
	}
	CDataRBBinaryTreeT(RefRef(CDataRBBinaryTreeT) _move) :
		_liste(_move._liste), _deleter(_move._deleter), _lesser(_move._lesser)
	{
		_move._liste = NULL;
	}
	virtual ~CDataRBBinaryTreeT(void)
	{
		Close();
	}

	ConstRef(CDataRBBinaryTreeT) operator = (ConstRef(CDataRBBinaryTreeT) copy)
	{
		if (this != &copy)
		{
			Close();
			Copy(copy);
		}
		return *this;
	}
	ConstRef(CDataRBBinaryTreeT) operator = (RefRef(CDataRBBinaryTreeT) _move)
	{
		if (this != &_move)
		{
			_liste = _move._liste;
			_move._liste = NULL;
		}
		return *this;
	}

	bool Open(DECL_FILE_LINE0)
	{ 
		_liste = RBBinaryTreeOpen(ARGS_FILE_LINE0); 
		return _liste != NULL; 
	}
	TListCnt Count() const 
	{ 
		return RBBinaryTreeCount(_liste); 
	}
	TListCnt Height() const 
	{ 
		return RBBinaryTreeHeight(_liste); 
	}
	void Copy(ConstRef(CDataRBBinaryTreeT) copy)
	{
		Iterator it = copy.Begin();
		Open(__FILE__LINE__0);

		while (it)
		{
			Ptr(Item) p = *it;

			p->addRef();
			InsertSorted(p);
			++it;
		}
	}
	void Close()
	{
		if (!_liste)
			return;
		RBBinaryTreeClose(_liste, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(RefRef(D) rD = D())
	{
		if (!_liste)
			return;
		RBBinaryTreeClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(Ref(D) rD)
	{
		if (!_liste)
			return;
		RBBinaryTreeClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	Iterator Begin() const
	{ 
		Iterator it = RBBinaryTreeBegin(_liste); 
		return it; 
	}
	Iterator Next(Iterator node) const 
	{ 
		if (node) 
			++node; 
		return node; 
	}
	Iterator Prev(Iterator node) const 
	{ 
		if (node) 
			--node; 
		return node; 
	}
	Iterator Last() const 
	{ 
		Iterator it = RBBinaryTreeLast(_liste); 
		return it; 
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(RefRef(D) rD = D()) const
	{
		return RBBinaryTreeForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(Ref(D) rD) const
	{
		return RBBinaryTreeForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, RefRef(D) rD = D()) const
	{
		Iterator it = RBBinaryTreeFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it = RBBinaryTreeFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	bool MatchSorted(Iterator it, ConstPtr(Item) data)
	{
		if (!it)
			return false;
		if (PtrCheck(*it))
			return false;
		if (TCppObjectSearchAndSortUserFunc<Item, Lesser>(*it, data, &_lesser) != 0)
			return false;
		return true;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	bool MatchSorted(Iterator it, ConstPtr(Item) data, Ref(D) rD)
	{
		if (!it)
			return false;
		if (PtrCheck(*it))
			return false;
		if (TCppObjectSearchAndSortUserFunc<Item, D>(*it, data, &rD) != 0)
			return false;
		return true;
	}
	Iterator FindSorted(ConstPtr(Item) data)
	{
		Iterator it;

		it = RBBinaryTreeFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator FindSorted(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it;

		it = RBBinaryTreeFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data) const
	{
		Iterator it;

		it = RBBinaryTreeUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator UpperBound(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it;

		it = RBBinaryTreeUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data) const
	{
		Iterator it;

		it = RBBinaryTreeLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator LowerBound(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it;

		it = RBBinaryTreeLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator InsertSorted(ConstPtr(Item) data)
	{
		return RBBinaryTreeInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator InsertSorted(ConstPtr(Item) data, RefRef(D) rD = D()) const
	{
		return RBBinaryTreeInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	bool RemoveSorted(ConstPtr(Item) data)
	{
		return RBBinaryTreeRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D, typename E> // CCppObjectLessFunctor<Item>, CCppObjectReleaseFunctor<Item>
	bool RemoveSorted(ConstPtr(Item) data, RefRef(D) rD = D(), RefRef(E) rE = E()) const
	{
		return RBBinaryTreeRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, &TCppObjectReleaseFunc<Item, E>, &rE);
	}
	Ptr(Item) GetData(Iterator node) const
	{ 
		return CastAnyPtr(Item, RBBinaryTreeGetData(node)); 
	}
	void SetData(Iterator node, Ptr(Item) data)
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			_deleter(CastMutablePtr(Item, p));
			RBBinaryTreeSetData(node, data);
		}
	}

protected:
	Pointer _liste;
	Deleter _deleter;
	Lesser _lesser;

private:
	CDataRBBinaryTreeT();
};

