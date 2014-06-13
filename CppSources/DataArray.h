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

template <class Item, class Lesser = CCppObjectLessFunctor<Item>, class Deleter = CCppObjectReleaseFunctor<Item> >
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

		operator bool() { return NotPtrCheck(_Lnode(_result)) && (_Loffset(_result) >= 0) && NotPtrCheck(ArrayGetData(_result)); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataArrayT(DECL_FILE_LINE TListCnt cnt, RefRef(Lesser) rLesser = Lesser(), RefRef(Deleter) rDeleter = Deleter()) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE cnt);
	}
	CDataArrayT(DECL_FILE_LINE TListCnt cnt, ConstRef(Lesser) rLesser, ConstRef(Deleter) rDeleter) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE cnt);
	}
	CDataArrayT(ConstRef(CDataArrayT) copy) :
		_liste(NULL), _deleter(copy._deleter), _lesser(copy._lesser)
	{
		Copy(copy);
	}
	CDataArrayT(RefRef(CDataArrayT) _move) :
		_liste(_move._liste), _deleter(_move._deleter), _lesser(_move._lesser)
	{
		_move._liste = NULL;
	}
	virtual ~CDataArrayT(void)
	{
		Close();
	}

	ConstRef(CDataArrayT) operator = (ConstRef(CDataArrayT) copy)
	{
		if (this != &copy)
		{
			Close();
			Copy(copy);
		}
		return *this;
	}
	ConstRef(CDataArrayT) operator = (RefRef(CDataArrayT) _move)
	{
		if (this != &_move)
		{
			_liste = _move._liste;
			_move._liste = NULL;
		}
		return *this;
	}

	bool Open(DECL_FILE_LINE TListCnt cnt) 
	{ 
		_liste = ArrayOpen(ARGS_FILE_LINE cnt); 
		return _liste != NULL; 
	}
	TListCnt Count() const 
	{ 
		return ArrayCount(_liste); 
	}
	TListCnt Maximum() const
	{ 
		return ArrayMaximum(_liste); 
	}
	void Copy(ConstRef(CDataArrayT) copy)
	{
		Iterator it = copy.Begin();
		Open(__FILE__LINE__ copy.Count());

		while (it)
		{
			Ptr(Item) p = *it;

			p->addRef();
			Append(p);
			++it;
		}
	}
	void Clear()
	{
		ArrayClear(_liste, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Clear(RefRef(D) rD = D())
	{
		ArrayClear(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
	}
	void Close()
	{
		if (!_liste)
			return;
		ArrayClose(_liste, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(RefRef(D) rD = D())
	{
		if (!_liste)
			return;
		ArrayClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const
	{ 
		Iterator it = ArrayIndex(_liste, index); 
		return it; 
	}
	Iterator Begin() const 
	{ 
		Iterator it = ArrayBegin(_liste); 
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
		Iterator it = ArrayLast(_liste); 
		return it; 
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(RefRef(D) rD = D(), bool bbackward = false) const
	{
		return ArrayForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD, bbackward);
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it = ArrayFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator FindSorted(ConstPtr(Item) data)
	{
		Iterator it;

		it = ArrayFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator FindSorted(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it;

		it = ArrayFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data)
	{
		Iterator it;

		it = ArrayUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator UpperBound(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it;

		it = ArrayUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data)
	{
		Iterator it;

		it = ArrayLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator LowerBound(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it;

		it = ArrayLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	void Sort(TSortMode mode = HeapSortMode)
	{
		ArraySort(_liste, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser, Castword(mode));
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	void Sort(RefRef(D) rD = D(), TSortMode mode = HeapSortMode)
	{
		ArraySort(_liste, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, Castword(mode));
	}
	Iterator Append(ConstPtr(Item) data) const
	{ 
		Iterator it = ArrayAppend(_liste, data); 
		return it; 
	}
	Iterator Prepend(ConstPtr(Item) data) const 
	{ 
		Iterator it = ArrayPrepend(_liste, data); 
		return it; 
	}
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const 
	{ 
		Iterator it = ArrayInsertBefore(node, data); 
		return it; 
	}
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const 
	{ 
		Iterator it = ArrayInsertAfter(node, data); 
		return it; 
	}
	void Remove(Iterator node)
	{
		ArrayRemove(node, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Remove(Iterator node, RefRef(D) rD = D())
	{
		ArrayRemove(node, &TCppObjectReleaseFunc<Item, D>, &rD);
	}
	Iterator InsertSorted(ConstPtr(Item) data)
	{
		return ArrayInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator InsertSorted(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		return ArrayInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	bool RemoveSorted(ConstPtr(Item) data)
	{
		return ArrayRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D, typename E> // CCppObjectLessFunctor<Item>, CCppObjectReleaseFunctor<Item>
	bool RemoveSorted(ConstPtr(Item) data, RefRef(D) rD = D(), RefRef(E) rE = E())
	{
		return ArrayRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, &TCppObjectReleaseFunc<Item, E>, &rE);
	}
	Ptr(Item) GetData(Iterator node) const
	{ 
		return CastAnyPtr(Item, ArrayGetData(node)); 
	}
	void SetData(Iterator node, Ptr(Item) data)
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			_deleter(CastMutablePtr(Item, p));
			ArraySetData(node, data);
		}
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void SetData(Iterator node, Ptr(Item) data, RefRef(D) rD = D())
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			rD(CastMutablePtr(Item, p));
			ArraySetData(node, data);
		}
	}

protected:
	Pointer _liste;
	Deleter _deleter;
	Lesser _lesser;

private:
	CDataArrayT(void);
};

