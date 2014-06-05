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
 *  \file DataVector.h
 *  \brief Vector (variable size linear buffer) data structure.
 */
#pragma once

#include "LISTE.H"

#include "CppSources.h"

template <class Item, class Lesser = CCppObjectLessFunctor<Item>, class Deleter = CCppObjectReleaseFunctor<Item> >
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

	CDataVectorT(DECL_FILE_LINE TListCnt cnt, TListCnt exp, RefRef(Lesser) rLesser = Lesser(), RefRef(Deleter) rDeleter = Deleter()) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE cnt, exp);
	}
	CDataVectorT(DECL_FILE_LINE TListCnt cnt, TListCnt exp, ConstRef(Lesser) rLesser, ConstRef(Deleter) rDeleter) :
		_liste(NULL), _deleter(rDeleter), _lesser(rLesser)
	{
		Open(ARGS_FILE_LINE cnt, exp);
	}
	CDataVectorT(ConstRef(CDataVectorT) copy) :
		_liste(NULL), _deleter(copy._deleter), _lesser(copy._lesser)
	{
		Copy(copy);
	}
	CDataVectorT(RefRef(CDataVectorT) _move) :
		_liste(_move._liste), _deleter(_move._deleter), _lesser(_move._lesser)
	{
		_move._liste = NULL;
	}
	virtual ~CDataVectorT(void)
	{
		Close();
	}

	ConstRef(CDataVectorT) operator = (ConstRef(CDataVectorT) copy)
	{
		if (this != &copy)
		{
			Close();
			Copy(copy);
		}
		return *this;
	}
	ConstRef(CDataVectorT) operator = (RefRef(CDataVectorT) _move)
	{
		if (this != &_move)
		{
			_liste = _move._liste;
			_move._liste = NULL;
		}
		return *this;
	}

	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp)
	{ 
		_liste = VectorOpen(ARGS_FILE_LINE cnt, exp); 
		return _liste != NULL; 
	}
	TListCnt Count() const 
	{ 
		return (PtrCheck(_liste) ? 0 : VectorCount(_liste)); 
	}
	void Copy(ConstRef(CDataVectorT) copy)
	{
		Iterator it = copy.Begin();
		Open(__FILE__LINE__ copy.Count(), 256);

		while (it)
		{
			Ptr(Item) p = *it;

			p->addRef();
			Append(p);
			++it;
		}
	}
	void Close()
	{
		if (!_liste)
			return;
		VectorClose(_liste, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(RefRef(D) rD = D())
	{
		if (!_liste)
			return;
		VectorClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Close(Ref(D) rD)
	{
		if (!_liste)
			return;
		VectorClose(_liste, &TCppObjectReleaseFunc<Item, D>, &rD);
		_liste = NULL;
	}
	Iterator Index(TListIndex index) const
	{ 
		Iterator it = VectorIndex(_liste, index); 
		return it; 
	}
	Iterator Begin() const 
	{ 
		Iterator it = VectorBegin(_liste); 
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
		Iterator it = VectorLast(_liste); 
		return it; 
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(RefRef(D) rD = D()) const
	{
		return VectorForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectForEachFunctor<Item>
	bool ForEach(Ref(D) rD) const
	{
		return VectorForEach(_liste, TCppObjectForEachFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it = VectorFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectEqualFunctor<Item>
	Iterator Find(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it = VectorFind(_liste, data, &TCppObjectFindUserFunc<Item, D>, &rD);
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
	template <typename D> // CCppObjectLessFunctor<Item>
	bool MatchSorted(Iterator it, ConstPtr(Item) data, RefRef(D) rD = D())
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

		it = VectorFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator FindSorted(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it;

		it = VectorFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator FindSorted(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it;

		it = VectorFindSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator UpperBound(ConstPtr(Item) data)
	{
		Iterator it;

		it = VectorUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator UpperBound(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it;

		it = VectorUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator UpperBound(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it;

		it = VectorUpperBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	Iterator LowerBound(ConstPtr(Item) data)
	{
		Iterator it;

		it = VectorLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator LowerBound(ConstPtr(Item) data, Ref(D) rD)
	{
		Iterator it;

		it = VectorLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator LowerBound(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		Iterator it;

		it = VectorLowerBound(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
		return it;
	}
	void Sort(TSortMode mode = HeapSortMode)
	{
		VectorSort(_liste, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser, Castword(mode));
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	void Sort(Ref(D) rD, TSortMode mode = HeapSortMode)
	{
		VectorSort(_liste, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, Castword(mode));
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	void Sort(RefRef(D) rD, TSortMode mode = HeapSortMode)
	{
		VectorSort(_liste, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, Castword(mode));
	}
	Iterator Append(ConstPtr(Item) data) const
	{ 
		Iterator it = VectorAppend(_liste, data); 
		return it; 
	}
	Iterator Prepend(ConstPtr(Item) data) const
	{ 
		Iterator it = VectorPrepend(_liste, data); 
		return it; 
	}
	Iterator InsertBefore(Iterator node, ConstPtr(Item) data) const 
	{ 
		Iterator it = VectorInsertBefore(node, data); 
		return it; 
	}
	Iterator InsertAfter(Iterator node, ConstPtr(Item) data) const 
	{ 
		Iterator it = VectorInsertAfter(node, data); 
		return it; 
	}
	void Remove(Iterator node)
	{
		VectorRemove(node, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D> // CCppObjectReleaseFunctor<Item>
	void Remove(Iterator node, RefRef(D) rD = D())
	{
		VectorRemove(node, &TCppObjectReleaseFunc<Item, D>, &rD);
	}
	template <typename D>
	void Remove(Iterator node, Ref(D) rD)
	{
		VectorRemove(node, &TCppObjectReleaseFunc<Item, D>, &rD);
	}
	Iterator InsertSorted(ConstPtr(Item) data)
	{
		return VectorInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator InsertSorted(ConstPtr(Item) data, RefRef(D) rD = D())
	{
		return VectorInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	template <typename D> // CCppObjectLessFunctor<Item>
	Iterator InsertSorted(ConstPtr(Item) data, Ref(D) rD)
	{
		return VectorInsertSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD);
	}
	bool RemoveSorted(ConstPtr(Item) data)
	{
		return VectorRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, Lesser>, &_lesser, &TCppObjectReleaseFunc<Item, Deleter>, &_deleter);
	}
	template <typename D, typename E> // CCppObjectLessFunctor<Item>, CCppObjectReleaseFunctor<Item>
	bool RemoveSorted(ConstPtr(Item) data, RefRef(D) rD = D(), RefRef(E) rE = E())
	{
		return VectorRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, &TCppObjectReleaseFunc<Item, E>, &rE);
	}
	template <typename D, typename E> // CCppObjectLessFunctor<Item>, CCppObjectReleaseFunctor<Item>
	bool RemoveSorted(ConstPtr(Item) data, Ref(D) rD, Ref(E) rE)
	{
		return VectorRemoveSorted(_liste, data, &TCppObjectSearchAndSortUserFunc<Item, D>, &rD, &TCppObjectReleaseFunc<Item, E>, &rE);
	}
	ConstPtr(Item) GetData(Iterator node) const
	{ 
		return CastAnyPtr(Item, VectorGetData(node)); 
	}
	void SetData(Iterator node, Ptr(Item) data)
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			_deleter(CastMutablePtr(Item, p));
			VectorSetData(node, data);
		}
	}
	template <typename D>
	void SetData(Iterator node, Ptr(Item) data, RefRef(D) rD = D())
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			rD(CastMutablePtr(Item, p));
			VectorSetData(node, data);
		}
	}
	template <typename D>
	void SetData(Iterator node, Ptr(Item) data, Ref(D) rD)
	{
		ConstPtr(Item) p = GetData(node);

		if (NotPtrCheck(p) && (p != data))
		{
			rD(CastMutablePtr(Item, p));
			VectorSetData(node, data);
		}
	}

protected:
	Pointer _liste;
	Deleter _deleter;
	Lesser _lesser;

private:
	CDataVectorT(void);
};

CPPSOURCES_API void __stdcall CDataVectorT_StringBuffer_DeleteFunc(ConstPointer data, Pointer context);
CPPSOURCES_API sword __stdcall CDataVectorT_StringBuffer_SearchAndSortUserFunc(ConstPointer pa, ConstPointer pb, Pointer context);

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
		Iterator(void) : _result(_LNULL) {}
		Iterator(LSearchResultType result) : _result(result) {}

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

	CDataVectorT(DECL_FILE_LINE TListCnt cnt, TListCnt exp) : _liste(NULL) { Open(ARGS_FILE_LINE cnt, exp); }
	CDataVectorT(ConstRef(CDataVectorT) copy) { _liste = copy._liste; AddRef(); }
	~CDataVectorT(void) { if (!Release()) Close(); }

	ConstRef(CDataVectorT) operator = (ConstRef(CDataVectorT) copy) { Copy(copy); return *this; }

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp) { if (_liste != NULL) return false; _liste = VectorOpen(ARGS_FILE_LINE cnt, exp); return _liste != NULL; }
	void Copy(ConstRef(CDataVectorT) copy) { if (!Release()) Close(); _liste = copy._liste; AddRef(); }
	TListCnt Count() const { return VectorCount(_liste); }
	void Close() { if (_liste == NULL) return; VectorClose(_liste, CDataVectorT_StringBuffer_DeleteFunc, NULL); _liste = NULL; }
	Iterator Index(TListIndex index) const { Iterator it = VectorIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = VectorBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if (node) ++node; return node; }
	Iterator Prev(Iterator node) const { if (node) --node; return node; }
	Iterator Last() const { Iterator it = VectorLast(_liste); return it; }
	Iterator Find(ConstRef(CStringBuffer) data) const { Iterator it = VectorFind(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortUserFunc, NULL); return it; }
	Iterator FindSorted(ConstRef(CStringBuffer) data) const { Iterator it = VectorFindSorted(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortUserFunc, NULL); return it; }
	Iterator UpperBound(ConstRef(CStringBuffer) data) const { Iterator it = VectorUpperBound(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortUserFunc, NULL); return it; }
	Iterator LowerBound(ConstRef(CStringBuffer) data) const { Iterator it = VectorLowerBound(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortUserFunc, NULL); return it; }
	void Sort(TSortMode mode = HeapSortMode) const { VectorSort(_liste, CDataVectorT_StringBuffer_SearchAndSortUserFunc, NULL, Castword(mode)); }
	Iterator Append(ConstRef(CStringBuffer) data) const { Iterator it = VectorAppend(_liste, data.GetString()); data.addRef(); return it; }
	Iterator Prepend(ConstRef(CStringBuffer) data) const { Iterator it = VectorPrepend(_liste, data.GetString()); data.addRef(); return it; }
	Iterator InsertBefore(Iterator node, ConstRef(CStringBuffer) data) const { Iterator it = VectorInsertBefore(node, data.GetString()); data.addRef(); return it; }
	Iterator InsertAfter(Iterator node, ConstRef(CStringBuffer) data) const { Iterator it = VectorInsertAfter(node, data.GetString()); data.addRef(); return it; }
	void Remove(Iterator node) const { VectorRemove(node, CDataVectorT_StringBuffer_DeleteFunc, NULL); }
	Iterator InsertSorted(ConstRef(CStringBuffer) data) const { Iterator it = VectorInsertSorted(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortUserFunc, NULL); data.addRef(); return it; }
	bool RemoveSorted(ConstRef(CStringBuffer) data) const { return VectorRemoveSorted(_liste, data.GetString(), CDataVectorT_StringBuffer_SearchAndSortUserFunc, NULL, CDataVectorT_StringBuffer_DeleteFunc, NULL); }
	CStringBuffer GetData(Iterator node) const { CStringBuffer tmp; tmp.assign(CastAny(CConstPointer, VectorGetData(node))); CStringBuffer tmp1(tmp); tmp.release(); return tmp1; }
	void SetData(Iterator node, ConstRef(CStringBuffer) data) const
	{
		CStringBuffer tmp;

		tmp.assign(CastAny(CConstPointer, VectorGetData(node)));
		if (tmp.GetString() == data.GetString())
		{
			tmp.release();
			return;
		}
		VectorSetData(node, data.GetString());
		data.addRef();
	}
	void Remove(Iterator begin, Iterator end)
	{
		if (!begin)
			return;
		while (begin != end)
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
		if (text.IsEmpty())
			return 0;

		CStringBuffer tmp(text);
		CStringConstIterator it(tmp);
		dword cnt = 1;
		dword schl = sch.GetLength();

		if (schl == 0)
		{
			if (!itInsert)
				Append(tmp);
			else if (bBefore)
			{
				InsertBefore(itInsert, tmp);
				++itInsert;
			}
			else
				itInsert = InsertAfter(itInsert, tmp);
			return 1;
		}

		it.Find(sch.GetString());
		while (!(it.IsEnd()))
		{
			++cnt;
			it += schl;
			it.Find(sch.GetString());
		}

		CArray sar = CastAny(CArray, TFalloc(cnt * szPointer));
		dword max;

		tmp.Split(sch.GetString(), sar, cnt, &max);
		for (dword ix = 0; ix < max; ++ix)
		{
			CStringBuffer tmp1(__FILE__LINE__ sar[ix]);

			if (!itInsert)
				Append(tmp1);
			else if (bBefore)
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

		while (node)
		{
			if (!bFirst)
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

		while (node)
		{
			tmp = GetData(node);
			l = tmp.GetLength();
			if (l > res)
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

		while (node)
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

		while (node)
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

		while (node)
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

CPPSOURCES_API void __stdcall CDataVectorT_dword_DeleteFunc(ConstPointer data, Pointer context);
CPPSOURCES_API sword __stdcall CDataVectorT_dword_SearchAndSortUserFunc(ConstPointer pa, ConstPointer pb, Pointer context);

template <>
class CDataVectorT<dword>
{
public:
	enum TSortMode {
		HeapSortMode = LSORTMODE_HEAPSORT,
		QuickSortMode = LSORTMODE_QUICKSORT
	};

	class Iterator
	{
	public:
		Iterator(void) : _result(_LNULL) {}
		Iterator(LSearchResultType result) : _result(result) {}

		Iterator& operator++() { _result = VectorNext(_result); return *this; }
		Iterator& operator--() { _result = VectorPrev(_result); return *this; }
		dword operator*()
		{
#ifdef OK_CPU_32BIT
			return CastAny(dword, VectorGetData(_result));
#endif
#ifdef OK_CPU_64BIT
			return Castdword(CastAny(qword, VectorGetData(_result)));
#endif
		}
		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataVectorT(DECL_FILE_LINE TListCnt cnt, TListCnt exp) : _liste(NULL) { Open(ARGS_FILE_LINE cnt, exp); }
	CDataVectorT(ConstRef(CDataVectorT) copy) { _liste = copy._liste; AddRef(); }
	~CDataVectorT(void) { if (!Release()) Close(); }

	ConstRef(CDataVectorT) operator = (ConstRef(CDataVectorT) copy) { Copy(copy); return *this; }

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp) { if (_liste != NULL) return false; _liste = VectorOpen(ARGS_FILE_LINE cnt, exp); return _liste != NULL; }
	void Copy(ConstRef(CDataVectorT) copy) { if (!Release()) Close(); _liste = copy._liste; AddRef(); }
	TListCnt Count() const { return VectorCount(_liste); }
	void Close() { if (_liste == NULL) return; VectorClose(_liste, CDataVectorT_dword_DeleteFunc, NULL); _liste = NULL; }
	Iterator Index(TListIndex index) const { Iterator it = VectorIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = VectorBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if (node) ++node; return node; }
	Iterator Prev(Iterator node) const { if (node) --node; return node; }
	Iterator Last() const { Iterator it = VectorLast(_liste); return it; }
	Iterator Find(dword data) const { Iterator it = VectorFind(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	Iterator FindSorted(dword data) const { Iterator it = VectorFindSorted(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	Iterator UpperBound(dword data) const { Iterator it = VectorUpperBound(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	Iterator LowerBound(dword data) const { Iterator it = VectorLowerBound(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	void Sort(TSortMode mode = HeapSortMode) const { VectorSort(_liste, CDataVectorT_dword_SearchAndSortUserFunc, NULL, Castword(mode)); }
	Iterator Append(dword data) const { Iterator it = VectorAppend(_liste, CastAny(Pointer, data)); return it; }
	Iterator Prepend(dword data) const { Iterator it = VectorPrepend(_liste, CastAny(Pointer, data)); return it; }
	Iterator InsertBefore(Iterator node, dword data) const { Iterator it = VectorInsertBefore(node, CastAny(Pointer, data)); return it; }
	Iterator InsertAfter(Iterator node, dword data) const { Iterator it = VectorInsertAfter(node, CastAny(Pointer, data)); return it; }
	void Remove(Iterator node) const { VectorRemove(node, CDataVectorT_dword_DeleteFunc, NULL); }
	Iterator InsertSorted(dword data) const { Iterator it = VectorInsertSorted(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	bool RemoveSorted(dword data) const { return VectorRemoveSorted(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL, CDataVectorT_dword_DeleteFunc, NULL); }
	dword GetData(Iterator node) const {
#ifdef OK_CPU_32BIT
		return CastAny(dword, VectorGetData(node));
#endif
#ifdef OK_CPU_64BIT
		return Castdword(CastAny(qword, VectorGetData(node)));
#endif
	}
	void SetData(Iterator node, dword data) const
	{
		VectorSetData(node, CastAny(Pointer, data));
	}

protected:
	Pointer _liste;

private:
	CDataVectorT(void);
};

template <>
class CDataVectorT<sdword>
{
public:
	enum TSortMode {
		HeapSortMode = LSORTMODE_HEAPSORT,
		QuickSortMode = LSORTMODE_QUICKSORT
	};

	class Iterator
	{
	public:
		Iterator(void) : _result(_LNULL) {}
		Iterator(LSearchResultType result) : _result(result) {}

		Iterator& operator++() { _result = VectorNext(_result); return *this; }
		Iterator& operator--() { _result = VectorPrev(_result); return *this; }
		sdword operator*() 
		{ 
#ifdef OK_CPU_32BIT
		return CastAny(sdword, VectorGetData(_result));
#endif
#ifdef OK_CPU_64BIT
		return Castsdword(CastAny(sqword, VectorGetData(_result)));
#endif
		}
		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataVectorT(DECL_FILE_LINE TListCnt cnt, TListCnt exp) : _liste(NULL) { Open(ARGS_FILE_LINE cnt, exp); }
	CDataVectorT(ConstRef(CDataVectorT) copy) { _liste = copy._liste; AddRef(); }
	~CDataVectorT(void) { if (!Release()) Close(); }

	ConstRef(CDataVectorT) operator = (ConstRef(CDataVectorT) copy) { Copy(copy); return *this; }

	TListCnt AddRef() { return TFincrefcnt(_liste); }
	TListCnt RefCount() { return TFrefcnt(_liste); }
	TListCnt Release() { return TFdecrefcnt(_liste); }
	bool Open(DECL_FILE_LINE TListCnt cnt, TListCnt exp) { if (_liste != NULL) return false; _liste = VectorOpen(ARGS_FILE_LINE cnt, exp); return _liste != NULL; }
	void Copy(ConstRef(CDataVectorT) copy) { if (!Release()) Close(); _liste = copy._liste; AddRef(); }
	TListCnt Count() const { return VectorCount(_liste); }
	void Close() { if (_liste == NULL) return; VectorClose(_liste, CDataVectorT_dword_DeleteFunc, NULL); _liste = NULL; }
	Iterator Index(TListIndex index) const { Iterator it = VectorIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = VectorBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if (node) ++node; return node; }
	Iterator Prev(Iterator node) const { if (node) --node; return node; }
	Iterator Last() const { Iterator it = VectorLast(_liste); return it; }
	Iterator Find(sdword data) const { Iterator it = VectorFind(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	Iterator FindSorted(sdword data) const { Iterator it = VectorFindSorted(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	Iterator UpperBound(sdword data) const { Iterator it = VectorUpperBound(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	Iterator LowerBound(sdword data) const { Iterator it = VectorLowerBound(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	void Sort(TSortMode mode = HeapSortMode) const { VectorSort(_liste, CDataVectorT_dword_SearchAndSortUserFunc, NULL, Castword(mode)); }
	Iterator Append(sdword data) const { Iterator it = VectorAppend(_liste, CastAny(Pointer, data)); return it; }
	Iterator Prepend(sdword data) const { Iterator it = VectorPrepend(_liste, CastAny(Pointer, data)); return it; }
	Iterator InsertBefore(Iterator node, sdword data) const { Iterator it = VectorInsertBefore(node, CastAny(Pointer, data)); return it; }
	Iterator InsertAfter(Iterator node, sdword data) const { Iterator it = VectorInsertAfter(node, CastAny(Pointer, data)); return it; }
	void Remove(Iterator node) const { VectorRemove(node, CDataVectorT_dword_DeleteFunc, NULL); }
	Iterator InsertSorted(sdword data) const { Iterator it = VectorInsertSorted(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL); return it; }
	bool RemoveSorted(sdword data) const { return VectorRemoveSorted(_liste, CastAny(Pointer, data), CDataVectorT_dword_SearchAndSortUserFunc, NULL, CDataVectorT_dword_DeleteFunc, NULL); }
	sdword GetData(Iterator node) const 
	{ 
#ifdef OK_CPU_32BIT
		return CastAny(sdword, VectorGetData(node));
#endif
#ifdef OK_CPU_64BIT
		return Castsdword(CastAny(sqword, VectorGetData(node)));
#endif
	}
	void SetData(Iterator node, sdword data) const
	{
		VectorSetData(node, CastAny(Pointer, data));
	}

protected:
	Pointer _liste;

private:
	CDataVectorT(void);
};

typedef CDataVectorT<mbchar, CCppObjectLessFunctor<mbchar>, CCppObjectNullFunctor<mbchar> > TMBCharList;

