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
 *  \file DataHashtable.h
 *  \brief Hash list data structure.
 */
#pragma once

#include "CppSources.h"
#include "liste.h"

const int bitslong = CALC_BITS(sdword);
const int bitslonglong = CALC_BITS(sqword);
const int bitsbyte = CHAR_BIT;
const int byteslong = bitslong / bitsbyte;
const int byteslonglong = bitslonglong / bitsbyte;
const int R = 1 << bitsbyte;

inline sdword digit64(sqword A, int B)
{
	return (A >> (bitsbyte * (byteslonglong - B - 1))) & (R - 1);
}

struct HashFunctorDigit64
{
	HashFunctorDigit64(sdword tabsz = 256) : M(tabsz) {}

	sdword operator()(sqword v) const
	{
		sdword h = 0, a = 31415, b = 27183;
		int i;
		for (i = 0; i < 8; ++i, a = (a*b) % (M - 1))
			h = ((a*h) + (digit64(v, i))) % M;
		return (h < 0) ? (h + M) : h;
	}

	sdword M;
};

inline sdword digit(sdword A, int B)
{
	return (A >> (bitsbyte * (byteslong - B - 1))) & (R - 1);
}

struct HashFunctorDigit
{
	HashFunctorDigit(sdword tabsz = 256) : M(tabsz) {}

	sdword operator()(sdword v) const
	{
		sdword h = 0, a = 31415, b = 27183;
		int i;
		for (i = 0; i < 4; ++i, a = (a*b) % (M - 1))
			h = ((a*h) + (digit(v, i))) % M;
		return (h < 0) ? (h + M) : h;
	}

	sdword M;
};

struct HashFunctorFloat
{
	HashFunctorFloat(sdword tabsz = 256) : M(tabsz) {}

	sdword operator()(float v) const
	{
		return ((sdword)(.61616 * v)) % M;
	}

	sdword M;
};

struct HashFunctorString
{
	HashFunctorString(sdword tabsz = 256) : M(tabsz) {}

	sdword operator()(CConstPointer v) const
	{
		sdword h = 0, a = 31415, b = 27183;
		for ( ; *v != 0; ++v, a = (a*b) % (M-1))
			h = ((a*h) + (*v)) % M;
		return (h < 0) ? (h+M) : h;
	}

	sdword M;
};

template <typename T1, typename T2>
sdword __stdcall TDataHashFunc(ConstPointer data, Pointer context)
{
	ConstPtr(T1) pData = CastAnyConstPtr(T1, data);
	Ptr(T2) pContext = CastAnyPtr(T2, context);

	return (*pContext)(pData);
}

template <class Item, class Hash, class Lesser = CCppObjectLessFunctor<Item>, class Deleter = CCppObjectReleaseFunctor<Item> >
class CDataHashLinkedListT
{
public:
	class Iterator
	{
	public:
		Iterator(void) : _result(_LNULL) {}
		Iterator(LSearchResultType result) : _result(result) {}

		Iterator& operator++() { _result = HashLinkedListNext(_result); return *this; }
		Iterator& operator--() { _result = HashLinkedListPrev(_result); return *this; }
		Ptr(Item) operator*() { return CastAnyPtr(Item, HashLinkedListGetData(_result)); }

		operator bool() { return NotPtrCheck(_Lnode(_result)) && (_Loffset(_result) >= 0) && NotPtrCheck(HashLinkedListGetData(_result)); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataHashLinkedListT(DECL_FILE_LINE TListCnt max, RefRef(Hash) rHash, RefRef(Lesser) rLesser = Lesser(), RefRef(Deleter) rDeleter = Deleter()) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser), _hash(rHash)
	{
		Open(ARGS_FILE_LINE max);
	}
	CDataHashLinkedListT(DECL_FILE_LINE TListCnt max, ConstRef(Hash) rHash, ConstRef(Lesser) rLesser, ConstRef(Deleter) rDeleter) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser), _hash(rHash)
	{
		Open(ARGS_FILE_LINE max);
	}
	CDataHashLinkedListT(ConstRef(CDataHashLinkedListT) copy) :
		_liste(NULL), _deleter(copy._deleter), _lesser(copy._lesser), _hash(copy._hash)
	{
		Copy(copy);
	}
	CDataHashLinkedListT(RefRef(CDataHashLinkedListT) _move) :
		_liste(_move._liste), _deleter(_move._deleter), _lesser(_move._lesser), _hash(_move._hash)
	{
		_move._liste = NULL;
	}
	virtual ~CDataHashLinkedListT(void)
	{
		Close();
	}

	ConstRef(CDataHashLinkedListT) operator = (ConstRef(CDataHashLinkedListT) copy)
	{
		if (this != &copy)
		{
			Close();
			Copy(copy);
		}
		return *this;
	}
	ConstRef(CDataHashLinkedListT) operator = (RefRef(CDataHashLinkedListT) _move)
	{
		if (this != &_move)
		{
			_liste = _move._liste;
			_move._liste = NULL;
		}
		return *this;
	}

	bool Open(DECL_FILE_LINE TListCnt max)
	{
		_liste = HashLinkedListOpen(ARGS_FILE_LINE max, &TDataHashFunc<Item, Hash>, &_hash);
		return _liste != NULL;
	}
	TListCnt Count() const
	{
		return HashLinkedListCount(_liste);
	}
	TListCnt Maximum() const
	{
		return HashLinkedListMaximum(_liste);
	}
	void Copy(ConstRef(CDataHashLinkedListT) copy)
	{
		Iterator it = copy.Begin();
		Open(__FILE__LINE__ copy.Maximum());

		while (it)
		{
			Ptr(Item) p = *it;

			p->addRef();
			InsertSorted(p);
			++it;
		}
	}
	void Clear()
	{
		HashLinkedListClear(_liste, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Clear(RefRef(D) rD = D())
	{
		HashLinkedListClear(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Clear(Ref(D) rD)
	{
		HashLinkedListClear(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
	}
	void Close()
	{
		if (!_liste)
			return;
		HashLinkedListClose(_liste, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(RefRef(D) rD = D())
	{
		if (!_liste)
			return;
		HashLinkedListClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(Ref(D) rD)
	{
		if (!_liste)
			return;
		HashLinkedListClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const
	{
		Iterator it = HashLinkedListIndex(_liste, index);

		return it;
	}
	Iterator Begin() const
	{
		Iterator it = HashLinkedListBegin(_liste);

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
		Iterator it = HashLinkedListLast(_liste);

		return it;
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(RefRef(D) rD = D(), bool bbackward = false)
	{
		return HashLinkedListForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD, bbackward);
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(Ref(D) rD, bool bbackward = false)
	{
		return HashLinkedListForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD, bbackward);
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it = HashLinkedListFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it = HashLinkedListFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator FindSorted(ConstPtr(Item) data) const
	{
		Iterator it;

		it = HashLinkedListFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, CastMutablePtr(Lesser, &_lesser));
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator FindSorted(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it;

		it = HashLinkedListFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator FindSorted(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it;

		it = HashLinkedListFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator InsertSorted(ConstPtr(Item) data)
	{
		return HashLinkedListInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator InsertSorted(ConstPtr(Item) data, Ref(D) rD)
	{
		return HashLinkedListInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator InsertSorted(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		return HashLinkedListInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	bool RemoveSorted(ConstPtr(Item) data)
	{
		return HashLinkedListRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D, typename E> // CCppObjectLessFunctor<Item>, CCppObjectReleaseFunctor<Item>
	bool RemoveSorted(ConstPtr(Item) data, Ref(D) rD, Ref(E) rE)
	{
		return HashLinkedListRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, &TCppObjectReleaseFunc<Item, E>, &rE);
	}
	template <typename D, typename E> // CCppObjectLessFunctor<Item>, CCppObjectReleaseFunctor<Item>
	bool RemoveSorted(ConstPtr(Item) data, RefRef(D) rD = D(), RefRef(E) rE = E())
	{
		return HashLinkedListRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, &TCppObjectReleaseFunc<Item, E>, &rE);
	}
	Ptr(Item) GetData(Iterator node) const
	{
		return CastAnyPtr(Item, HashLinkedListGetData(node));
	}
	void SetData(Iterator node, Ptr(Item) data)
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			_deleter(CastMutablePtr(Item, p));
			HashLinkedListSetData(node, data);
		}
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void SetData(Iterator node, Ptr(Item) data, Ref(D) rD)
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			rD(CastMutablePtr(Item, p));
			HashLinkedListSetData(node, data);
		}
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void SetData(Iterator node, Ptr(Item) data, RefRef(D) rD = D())
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			rD(CastMutablePtr(Item, p));
			HashLinkedListSetData(node, data);
		}
	}

protected:
	Pointer _liste;
	Deleter _deleter;
	Lesser _lesser;
	Hash _hash;

private:
	CDataHashLinkedListT();
};

