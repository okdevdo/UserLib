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

#include "CppSources.h"

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

template <class Item, class Key, class HashFunctor>
class CHashLinkedListPT
{
private:
	class node : public CCppObject
	{
	public:
		node() : item(), next(0) {}
		node(Ptr(Item) x, node *t = 0) : item(x), next(t) {}
		virtual ~node() {}
		Item *item;
		node* next;
	};
	typedef node *link;
	link *heads;
	dword N, M;
	HashFunctor hashFunc;
	bool removeR(link& x, ConstRef(Key) v)
	{
		if (x == 0)
			return false;
		if (x->item->key() == v)
		{
			link u = x;
			x = x->next;
			delete u;
			return true;
		}
		link t = x;
		while ((t->next != 0) && (t->next->item->key() != v))
			t = t->next;
		if (t->next == 0)
			return false;
		link u = t->next;
		t->next = u->next;
		delete u;
		return true;
	}
	link searchR(link x, ConstRef(Key) v) const
	{
		while ((x != 0) && (x->item->key() != v))
			x = x->next;
		return x;
	}
public:
	struct iterator
	{
		iterator(link *h, dword maxM) : heads(h), M(maxM), curnode(0), ix(0) { findNext(); }

		ConstRef(iterator) operator++() { findNext(); return *this; }
		link operator *() { return curnode; }

	private:
		void findNext()
		{
			for (;;)
			{
				if (curnode == 0)
				{
					while ((ix < M) && (heads[ix] == 0)) ++ix;
					if (ix == M)
						return;
					curnode = heads[ix];
					return;
				}
				curnode = curnode->next;
				if (curnode == 0)
					++ix;
				else
					return;
			}
		}

		link *heads;
		dword M;
		link curnode;
		dword ix;
	};
	Item* nullItem;
	CHashLinkedListPT(int maxN) : heads(0), N(0), M(maxN / 5), hashFunc(maxN / 5), nullItem(NULL)
	{
		heads = CastAnyPtr(link, TFalloc(M * sizeof(link)));
	}
	~CHashLinkedListPT()
	{
		clear();
		TFfree(heads);
	}
	void clear()
	{
		link t, u;
		for (dword i = 0; i < M; ++i)
		{
			t = heads[i];
			while (t != 0)
			{
				u = t->next;
				delete t;
				t = u;
			}
			heads[i] = 0;
		}
	}
	iterator begin() const
	{
		return iterator(heads, M);
	}
	dword count() const
	{
		return N;
	}
	Ptr(Item) search(ConstRef(Key) v) const
	{
		link t = searchR(heads[hashFunc(v)], v);
		if (t == 0)
			return nullItem;
		return t->item;
	}
	void insert(Ptr(Item) item)
	{
		sdword i = hashFunc(item->key());
		heads[i] = OK_NEW_OPERATOR node(item, heads[i]);
		++N;
	}
	void remove(ConstRef(Key) v)
	{
		if (removeR(heads[hashFunc(v)], v))
			--N;
	}
};

template <class Item, class Key, class HashFunctor>
class CHashLinkedListT
{
private:
	class node : public CCppObject
	{
	public:
		node() : item(), next(0) {}
		node(ConstRef(Item) x, node *t = 0) : item(x), next(t) {}
		Item item;
		node* next;
	};
	typedef node *link;
	link *heads;
	dword N, M;
	HashFunctor hashFunc;
	bool removeR(link& x, ConstRef(Key) v)
	{
		if (x == 0)
			return false;
		if (x->item.key() == v)
		{
			link u = x;
			x = x->next;
			delete u;
			return true;
		}
		link t = x;
		while ((t->next != 0) && (t->next->item.key() != v))
			t = t->next;
		if (t->next == 0)
			return false;
		link u = t->next;
		t->next = u->next;
		delete u;
		return true;
	}
	link searchR(link x, ConstRef(Key) v) const
	{
		while ((x != 0) && (x->item.key() != v))
			x = x->next;
		return x;
	}
public:
	struct iterator
	{
		iterator(link *h, dword maxM) : heads(h), M(maxM), curnode(0), ix(0) { findNext(); }

		ConstRef(iterator) operator++() { findNext(); return *this; }
		link operator *() { return curnode; }

	private:
		void findNext()
		{
			for (;;)
			{
				if (curnode == 0)
				{
					while ((ix < M) && (heads[ix] == 0)) ++ix;
					if (ix == M)
						return;
					curnode = heads[ix];
					return;
				}
				curnode = curnode->next;
				if (curnode == 0)
					++ix;
				else
					return;
			}
		}

		link *heads;
		dword M;
		link curnode;
		dword ix;
	};
	Item nullItem;
	CHashLinkedListT(int maxN) : heads(0), N(0), M(maxN / 5), hashFunc(maxN / 5)
	{
		heads = CastAnyPtr(link, TFalloc(M * sizeof(link)));
	}
	~CHashLinkedListT()
	{
		clear();
		TFfree(heads);
	}
	void clear()
	{
		link t, u;
		for (dword i = 0; i < M; ++i)
		{
			t = heads[i];
			while (t != 0)
			{
				u = t->next;
				delete t;
				t = u;
			}
			heads[i] = 0;
		}
	}
	iterator begin() const
	{
		return iterator(heads, M);
	}
	dword count() const
	{
		return N;
	}
	ConstRef(Item) search(ConstRef(Key) v) const
	{
		link t = searchR(heads[hashFunc(v)], v);
		if (t == 0)
			return nullItem;
		return t->item;
	}
	void insert(ConstRef(Item) item)
	{
		sdword i = hashFunc(item.key());
		heads[i] = OK_NEW_OPERATOR node(item, heads[i]);
		++N;
	}
	void remove(ConstRef(Key) v)
	{
		if (removeR(heads[hashFunc(v)], v))
			--N;
	}
};

template <class Item, class Key, class HashFunctor>
class CHashLinearExploreT
{
private:
	Item *st;
	int N, M;
	HashFunctor hashFunc;
public:
	Item nullItem;
	CHashLinearExploreT(int maxN): st(0), N(0), M(maxN << 1), hashFunc(maxN << 1)
	{
		st = CastAnyPtr(Item, TFalloc(sizeof(Item) * (M + 1)));
		for ( int i = 0; i <= M; ++i )
			st[i] = nullItem;
	}
	~CHashLinearExploreT()
	{
		TFfree(st);
	}
	int count()
	{
		return N;
	}
	void insert(Item item)
	{
		sdword i = hashFunc(item.key());
		while ( !(st[i].null()) ) i = (i + 1) % M;
		st[i] = item; ++N;
	}
	Item search(Key v)
	{
		sdword i = hashFunc(v);
		while ( !(st[i].null()) )
			if ( v == st[i].key() )
				return st[i];
			else
				i = (i + 1) % M;
		return nullItem;
	}
	void remove(Key v)
	{
		sdword i = hashFunc(v);
		sdword j;
		while ( !(st[i].null()) )
			if ( v == st[i].key() )
				break;
			else
				i = (i + 1) % M;
		if ( st[i].null() )
			return;
		st[i] = nullItem; --N;
		for ( j = i + 1; !(st[j].null()); j = (j + 1) % M, --N )
		{
			Item x = st[j];
			st[j] = nullItem;
			insert(x);
		}
	}
};

