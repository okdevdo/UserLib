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

template <class Item, class Lesser = CCppObjectLessFunctor<Item>, class Deleter = CCppObjectReleaseFunctor<Item> >
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

	CDataDoubleLinkedListT(DECL_FILE_LINE RefRef(Lesser) rLesser = Lesser(), RefRef(Deleter) rDeleter = Deleter()) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE0);
	}
	CDataDoubleLinkedListT(DECL_FILE_LINE ConstRef(Lesser) rLesser, ConstRef(Deleter) rDeleter) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE0);
	}
	CDataDoubleLinkedListT(ConstRef(CDataDoubleLinkedListT) copy) :
		_liste(NULL), _deleter(copy._deleter), _lesser(copy._lesser)
	{
		Copy(copy);
	}
	CDataDoubleLinkedListT(RefRef(CDataDoubleLinkedListT) _move) :
		_liste(_move._liste), _deleter(_move._deleter), _lesser(_move._lesser)
	{
		_move._liste = NULL;
	}
	virtual ~CDataDoubleLinkedListT(void)
	{
		Close();
	}

	ConstRef(CDataDoubleLinkedListT) operator = (ConstRef(CDataDoubleLinkedListT) copy)
	{
		if (this != &copy)
		{
			Close();
			Copy(copy);
		}
		return *this;
	}
	ConstRef(CDataDoubleLinkedListT) operator = (RefRef(CDataDoubleLinkedListT) _move)
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
		_liste = DoubleLinkedListOpen(ARGS_FILE_LINE0); 
		return _liste != NULL; 
	}
	TListCnt Count() const 
	{ 
		return DoubleLinkedListCount(_liste); 
	}
	void Copy(ConstRef(CDataDoubleLinkedListT) copy)
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
		DoubleLinkedListClose(_liste, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(RefRef(D) rD = D())
	{
		if (!_liste)
			return;
		DoubleLinkedListClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(Ref(D) rD)
	{
		if (!_liste)
			return;
		DoubleLinkedListClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const
	{ 
		Iterator it = DoubleLinkedListIndex(_liste, index); 
		return it; 
	}
	Iterator Begin() const 
	{ 
		Iterator it = DoubleLinkedListBegin(_liste); 
		return it; 
	}
	Iterator Next(Iterator node) const 
	{ 
		if ( node ) 
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
		Iterator it = DoubleLinkedListLast(_liste); 
		return it; 
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(RefRef(D) rD = D()) const
	{
		return DoubleLinkedListForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(Ref(D) rD) const
	{
		return DoubleLinkedListForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, RefRef(D) rD = D()) const
	{
		Iterator it = DoubleLinkedListFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it = DoubleLinkedListFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
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

		it =  DoubleLinkedListFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator FindSorted(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it;

		it =  DoubleLinkedListFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data) const
	{
		Iterator it;

		it =  DoubleLinkedListUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator UpperBound(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it;

		it =  DoubleLinkedListUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data) const
	{
		Iterator it;

		it =  DoubleLinkedListLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator LowerBound(ConstPtr(Item) data, Ref(D) rD) const
	{
		Iterator it;

		it =  DoubleLinkedListLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	void Sort()
	{
		 DoubleLinkedListSort(_liste, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	void Sort(Ref(D) rD) const
	{
		 DoubleLinkedListSort(_liste, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	Iterator Append(ConstPtr(Item) data) const 
	{ 
		Iterator it = DoubleLinkedListAppend(_liste, data); 
		return it; 
	}
	Iterator Prepend(ConstPtr(Item) data) const 
	{ 
		Iterator it = DoubleLinkedListPrepend(_liste, data); 
		return it; 
	}
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const 
	{ 
		Iterator it = DoubleLinkedListInsertBefore(node, data); 
		return it; 
	}
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const 
	{ 
		Iterator it = DoubleLinkedListInsertAfter(node, data); 
		return it; 
	}
	void Remove(Iterator node)
	{
		 DoubleLinkedListRemove(node, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Remove(Iterator node, RefRef(D) rD = D())
	{
		DoubleLinkedListRemove(node, &TCppObjectReleaseFunc<Item, D>, &rD);
	}
	Iterator InsertSorted(ConstPtr(Item) data)
	{
		return  DoubleLinkedListInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator InsertSorted(ConstPtr(Item) data, RefRef(D) rD = D()) const
	{
		return  DoubleLinkedListInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	bool RemoveSorted(ConstPtr(Item) data)
	{
		return  DoubleLinkedListRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D, typename E> // CCppObjectLessFunctor<Item>, CCppObjectReleaseFunctor<Item>
	bool RemoveSorted(ConstPtr(Item) data, RefRef(D) rD = D(), RefRef(E) rE = E()) const
	{
		return DoubleLinkedListRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, &TCppObjectReleaseFunc<Item, E>, &rE);
	}
	Ptr(Item) GetData(Iterator node) const
	{ 
		return CastAnyPtr(Item, DoubleLinkedListGetData(node)); 
	}
	void SetData(Iterator node, Ptr(Item) data)
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			_deleter(CastMutablePtr(Item, p));
			DoubleLinkedListSetData(node, data);
		}
	}

protected:
	Pointer _liste;
	Deleter _deleter;
	Lesser _lesser;

private:
	CDataDoubleLinkedListT(void);
};

