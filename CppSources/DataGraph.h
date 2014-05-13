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

template <class Item>
void __stdcall CDataGraphNodeT_DeleteFunc( ConstPointer data, Pointer context );
template <class Item>
sword __stdcall CDataGraphNodeT_SearchAndSortFunc( ConstPointer, ConstPointer );

template <class Item>
class CPPSOURCES_API CDataGraphNodeT: public CCppObject
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = DoubleLinkedListNext(_result); return *this; }
		Iterator& operator--() { _result = DoubleLinkedListPrev(_result); return *this; }
		Ptr(CDataGraphNodeT) operator*() { return CastAnyPtr(CDataGraphNodeT, DoubleLinkedListGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataGraphNodeT(DECL_FILE_LINE Ptr(Item) item = 0): _item(item), _color(-1), _stime(0), _ftime(0), _liste(NULL) { Open(ARGS_FILE_LINE0); }
	virtual ~CDataGraphNodeT() { Close(CDataGraphNodeT_DeleteFunc<Item>, NULL); }

	bool Open(DECL_FILE_LINE0) { if ( PtrCheck(_liste) ) _liste = DoubleLinkedListOpen(ARGS_FILE_LINE0); return _liste != NULL; }
	TListCnt Count() const { return DoubleLinkedListCount(_liste); }
	void Close(TDeleteFunc freeFunc, Pointer context) { DoubleLinkedListClose(_liste, freeFunc, context); _liste = NULL; }
	Iterator Index(TListIndex index) const { Iterator it = DoubleLinkedListIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = DoubleLinkedListBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = DoubleLinkedListLast(_liste); return it; }
	Iterator Find(Ptr(CDataGraphNodeT) data, TSearchAndSortFunc findFunc) const { Iterator it = DoubleLinkedListFind(_liste, data, findFunc); return it; }
	Iterator Append(Ptr(CDataGraphNodeT) data) const { Iterator it = DoubleLinkedListAppend(_liste, data); return it; }
	Iterator Prepend(Ptr(CDataGraphNodeT) data) const { Iterator it = DoubleLinkedListPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, Ptr(CDataGraphNodeT) data) const { Iterator it = DoubleLinkedListInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, Ptr(CDataGraphNodeT) data) const { Iterator it = DoubleLinkedListInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc, Pointer context) const { DoubleLinkedListRemove(node, freeFunc, context); }
	Ptr(CDataGraphNodeT) GetData(Iterator node) const { return CastAnyPtr(Item, DoubleLinkedListGetData(node)); }
	void SetData(Iterator node, Ptr(CDataGraphNodeT) data) const { DoubleLinkedListSetData(node, data); }

	Ptr(Item) GetItem() const { return _item; }
	int GetColor() const { return _color; }
	void SetColor(int c) { _color = c; }
	int GetSTime() const { return _stime; }
	void SetSTime(int t) { _stime = t; }
	int GetFTime() const { return _ftime; }
	void SetFTime(int t) { _ftime = t; }

private:
	Ptr(Item) _item;
	int _color;
	int _stime;
	int _ftime;
	Pointer _liste;
};

template <class Item>
void __stdcall CDataGraphNodeT_DeleteFunc( ConstPointer data, Pointer context )
{
}

template <class Item>
sword __stdcall CDataGraphNodeT_SearchAndSortFunc( ConstPointer item, ConstPointer data )
{
	Ptr(CDataGraphNodeT<Item>) p1 = CastAnyPtr(CDataGraphNodeT<Item>, CastMutable(Pointer, item));
	Ptr(CDataGraphNodeT<Item>) p2 = CastAnyPtr(CDataGraphNodeT<Item>, CastMutable(Pointer, data));

	if ( p1->GetItem() == p2->GetItem() )
		return 0;
	return 1;
}

template <class Item>
void __stdcall CDataGraphT_DeleteFunc( ConstPointer data, Pointer context )
{
	Ptr(CDataGraphNodeT<Item>) p = CastAnyPtr(CDataGraphNodeT<Item>, CastMutable(Pointer, data));

	p->release();
}

template <class Item>
sword __stdcall CDataGraphT_SearchAndSortFunc( ConstPointer item, ConstPointer data )
{
	Ptr(CDataGraphNodeT<Item>) p1 = CastAnyPtr(CDataGraphNodeT<Item>, CastMutable(Pointer, item));
	Ptr(CDataGraphNodeT<Item>) p2 = CastAnyPtr(CDataGraphNodeT<Item>, CastMutable(Pointer, data));

	if ( p1->GetItem() == p2->GetItem() )
		return 0;
	return 1;
}

template <class Item>
class CPPSOURCES_API CDataGraphT
{
public:
	class Iterator
	{
	public:
		Iterator(void): _result(_LNULL) {}
		Iterator(LSearchResultType result): _result(result) {}

		Iterator& operator++() { _result = DoubleLinkedListNext(_result); return *this; }
		Iterator& operator--() { _result = DoubleLinkedListPrev(_result); return *this; }
		Ptr(CDataGraphNodeT<Item>) operator*() { return CastAnyPtr(CDataGraphNodeT<Item>, DoubleLinkedListGetData(_result)); }

		operator bool() { return !LPtrCheck(_result); }
		operator LSearchResultType() { return _result; }

		bool operator == (Iterator other) { return LCompareEqual(_result, other._result); }
		bool operator != (Iterator other) { return LCompareNotEqual(_result, other._result); }

	private:
		LSearchResultType _result;
	};

	CDataGraphT(DECL_FILE_LINE0): _liste(NULL), _time(0) { Open(ARGS_FILE_LINE0); }
	~CDataGraphT() { Close(CDataGraphT_DeleteFunc<Item>, NULL); }

	bool Open(DECL_FILE_LINE0) { if ( PtrCheck(_liste) ) _liste = DoubleLinkedListOpen(ARGS_FILE_LINE0); return _liste != NULL; }
	TListCnt Count() const { return DoubleLinkedListCount(_liste); }
	void Close(TDeleteFunc freeFunc, Pointer context) { DoubleLinkedListClose(_liste, freeFunc, context); _liste = NULL; }
	Iterator Index(TListIndex index) const { Iterator it = DoubleLinkedListIndex(_liste, index); return it; }
	Iterator Begin() const { Iterator it = DoubleLinkedListBegin(_liste); return it; }
	Iterator Next(Iterator node) const { if ( node ) ++node; return node; }
	Iterator Prev(Iterator node) const { if ( node ) --node; return node; }
	Iterator Last() const { Iterator it = DoubleLinkedListLast(_liste); return it; }
	Iterator Find(Ptr(CDataGraphNodeT<Item>) data, TSearchAndSortFunc findFunc) const { Iterator it = DoubleLinkedListFind(_liste, data, findFunc); return it; }
	Iterator Append(Ptr(CDataGraphNodeT<Item>) data) const { Iterator it = DoubleLinkedListAppend(_liste, data); return it; }
	Iterator Prepend(Ptr(CDataGraphNodeT<Item>) data) const { Iterator it = DoubleLinkedListPrepend(_liste, data); return it; }
	Iterator InsertBefore(Iterator node, Ptr(CDataGraphNodeT<Item>) data) const { Iterator it = DoubleLinkedListInsertBefore(node, data); return it; }
	Iterator InsertAfter(Iterator node, Ptr(CDataGraphNodeT<Item>) data) const { Iterator it = DoubleLinkedListInsertAfter(node, data); return it; }
	void Remove(Iterator node, TDeleteFunc freeFunc, Pointer context) const { DoubleLinkedListRemove(node, freeFunc, context); }
	Ptr(CDataGraphNodeT<Item>) GetData(Iterator node) const { return CastAnyPtr(Item, DoubleLinkedListGetData(node)); }
	void SetData(Iterator node, Ptr(CDataGraphNodeT<Item>) data) const { DoubleLinkedListSetData(node, data); }

	Iterator AddNode(Ptr(Item) item) { return Append(OK_NEW_OPERATOR CDataGraphNodeT<Item>(item)); }
	void AddLink(Ptr(Item) item1, Ptr(Item) item2) 
	{
		CDataGraphNodeT<Item> node1(item1);
		Iterator it1 = Find(&node1, CDataGraphT_SearchAndSortFunc<Item>);

		if ( !it1 )
			it1 = AddNode(item1);

		CDataGraphNodeT<Item> node2(item2);
		Iterator it2 = Find(&node2, CDataGraphT_SearchAndSortFunc<Item>);

		if ( !it2 )
			it2 = AddNode(item2);
		(*it1)->Append(*it2);
	}
	void DepthFirstSearch()
	{
		Iterator it;
		Ptr(CDataGraphNodeT<Item>) pNode;

		it = Begin();
		while ( it )
		{
			pNode = *it;
			pNode->SetColor(-1);
			++it;
		}
		_time = 0;
		it = Begin();
		while ( it )
		{
			pNode = *it;
			if ( pNode->GetColor() < 0 )
				DFS_visit(pNode);
			++it;
		}
	}

private:
	void DFS_visit(Ptr(CDataGraphNodeT<Item>) pNode)
	{
		typename CDataGraphNodeT<Item>::Iterator it;
		Ptr(CDataGraphNodeT<Item>) pNode1;

		pNode->SetColor(0);
		++_time;
		pNode->SetSTime(_time);
		it = pNode->Begin();
		while ( it )
		{
			pNode1 = *it;
			if ( pNode1->GetColor() < 0 )
				DFS_visit(pNode1);
			++it;
		}
		pNode->SetColor(1);
		++_time;
		pNode->SetFTime(_time);
	}

	Pointer _liste;
	int _time;
};