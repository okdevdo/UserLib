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
 *  \file CppDataStructures.h
 *  \brief Templated definitions of linked List, binary tree and B*-tree.
 */
#pragma once

#include "CppSources.h"

template <class Item, class Key>
class CLinkedListT
{
protected:
	class node : public CCppObject
	{
	public:
		node() : item(), next(0) {}
		node(Item x, node *t = 0) : item(x), next(t) {}
		virtual ~node() {}
		Item item;
		node* next;
	};
	typedef node *link;
	link head;
	void insertR(link x, link t)
	{
		t->next = x->next;
		x->next = t;
	}
	void removeR(link x)
	{
		link t = x->next;
		x->next = t->next;
		delete t;
	}
	link searchR(link x, Key v)
	{
		while ((x->next != 0) && (x->next->item.key() != v))
			x = x->next;
		if (x->next == 0)
			return 0;
		return x;
	}
public:
	Item nullItem;
	CLinkedListT()
	{
		head = OK_NEW_OPERATOR node;
	}
	~CLinkedListT()
	{
		while (head->next != 0)
			removeR(head);
		delete head;
	}
	bool empty()
	{
		return (0 == head->next);
	}
	void insert(Item x)
	{
		insertR(head, OK_NEW_OPERATOR node(x));
	}
	void remove(Key v)
	{
		link t = searchR(head, v);
		if (t == 0)
			return;
		removeR(t);
	}
	Item search(Key v)
	{
		link t = searchR(head, v);
		if (t == 0)
			return nullItem;
		return t->next->item;
	}
	void traverse(void(*visit)(Item))
	{
		for (link t = head->next; t != 0; t = t->next)
			visit(t->item);
	}
	void reverse()
	{
		link t, y = head->next, r = 0;
		while (0 != y)
		{
			t = y->next;
			y->next = r;
			r = y; y = t;
		}
		head->next = r;
	}
	void sort()
	{
		node headb;
		link a = head->next, u, x, b = &headb, t;
		for (t = a; t != 0; t = u)
		{
			u = t->next;
			for (x = b; x->next != 0; x = x->next)
				if (x->next->item.key() > t->item.key())
					break;
			t->next = x->next; x->next = t;
		}
		head->next = b->next;
	}
};

template <class Item, class Key>
class CBinaryTreeT
{
protected:
	class node : public CCppObject
	{
	public:
		Item item; node *l, *r;
		int N;
		node(Item x)
		{
			item = x; l = 0; r = 0; N = 1;
		}
		virtual ~node() {}
	};
	typedef node* link;
	link head;
	Item searchR(link h, Key v)
	{
		if (h == 0) return nullItem;
		Key t = h->item.key();
		if (v == t) return h->item;
		if (v < t)
			return searchR(h->l, v);
		else
			return searchR(h->r, v);
	}
	void insertR(link& h, Item x)
	{
		if (h == 0) { h = OK_NEW_OPERATOR node(x); return; }
		if (x.key() < h->item.key())
			insertR(h->l, x);
		else
			insertR(h->r, x);
	}
	int heightR(link h)
	{
		if (h == 0) return 0;
		int l = heightR(h->l);
		int r = heightR(h->r);
		return Max(l, r) + 1;
	}
	int indexCounterR(link h)
	{
		if (h == 0) return 0;
		int l = indexCounterR(h->l);
		int r = indexCounterR(h->r);
		h->N = r + l + 1;
		return h->N;
	}
	void deleteR(link h)
	{
		if (h == 0) return;
		deleteR(h->l);
		deleteR(h->r);
		delete h;
	}
	void rotR(link& h)
	{
		link x = h->l;

		h->l = x->r;
		x->r = h;
		h = x;
		indexCounterR(h);
	}
	void rotL(link& h)
	{
		link x = h->r;

		h->r = x->l;
		x->l = h;
		h = x;
		indexCounterR(h);
	}
	void insertT(link& h, Item x)
	{
		if (h == 0) { h = OK_NEW_OPERATOR node(x); return; }
		if (x.key() < h->item.key())
		{
			insertT(h->l, x);
			rotR(h);
		}
		else
		{
			insertT(h->r, x);
			rotL(h);
		}
	}
	void inOrderR(link h, void(*visit)(Item))
	{
		if (h == 0) return;
		inOrderR(h->l, visit);
		visit(h->item);
		inOrderR(h->r, visit);
	}
	void preOrderR(link h, void(*visit)(Item))
	{
		if (h == 0) return;
		visit(h->item);
		preOrderR(h->l, visit);
		preOrderR(h->r, visit);
	}
	void postOrderR(link h, void(*visit)(Item))
	{
		if (h == 0) return;
		postOrderR(h->l, visit);
		postOrderR(h->r, visit);
		visit(h->item);
	}
	Item selectR(link h, int k)
	{
		if (h == 0) return nullItem;
		int t = (h->l == 0) ? 0 : h->l->N;
		if (t > k)
			return selectR(h->l, k);
		if (t < k)
			return selectR(h->r, k - t - 1);
		return h->item;
	}
	void partR(link& h, int k)
	{
		int t = (h->l == 0) ? 0 : h->l->N;
		if (t > k)
		{
			partR(h->l, k);
			rotR(h);
		}
		if (t < k)
		{
			partR(h->r, k - t - 1);
			rotL(h);
		}
	}
	link joinLR(link a, link b)
	{
		if (b == 0) return a;
		partR(b, 0);
		b->l = a;
		indexCounterR(b);
		return b;
	}
	void removeR(link& h, Key v)
	{
		if (h == 0) return;
		Key w = h->item.key();
		if (v < w)
			removeR(h->l, v);
		if (w < v)
			removeR(h->r, v);
		if (v == w)
		{
			link t = h;
			h = joinLR(h->l, h->r);
			delete t;
		}
		indexCounterR(h);
	}
	void balanceR(link& h)
	{
		if ((h == 0) || (h->N == 1)) return;
		partR(h, h->N / 2);
		balanceR(h->l);
		balanceR(h->r);
	}
public:
	Item nullItem;

	CBinaryTreeT(int maxN = 0)
	{
		head = 0;
	}
	~CBinaryTreeT()
	{
		deleteR(head);
	}
	int count()
	{
		if (head == 0)
			return 0;
		return head->N;
	}
	int height()
	{
		return heightR(head);
	}
	Item search(Key v)
	{
		return searchR(head, v);
	}
	void insert(Item x)
	{
		insertT(head, x);
	}
	void inOrder(void(*visit)(Item))
	{
		inOrderR(head, visit);
	}
	void preOrder(void(*visit)(Item))
	{
		preOrderR(head, visit);
	}
	void postOrder(void(*visit)(Item))
	{
		postOrderR(head, visit);
	}
	Item select(int k)
	{
		return selectR(head, k);
	}
	void remove(Key v)
	{
		removeR(head, v);
	}
	void balance()
	{
		balanceR(head);
	}
};

template <class Item, class Key, int M> // M must be even
class CBTreeT
{
protected:
	class node;
	struct entry
	{
		Key key;
		Item item;
		node* next;
		entry() : next(0) {}
	};
	class node : public CCppObject
	{
	public:
		int m;
		entry b[M];
		node() : m(0) {}
		virtual ~node() {}
	};
	typedef node* link;
	link head;
	int HT;
	Item searchR(link h, Key v, int ht)
	{
		int j;
		if (ht == 0)
		for (j = 0; j < h->m; ++j)
		{
			if (v == h->b[j].key)
				return h->b[j].item;
		}
		else
		for (j = 0; j < h->m; ++j)
		if ((j + 1 == h->m) || (v < h->b[j + 1].key))
			return searchR(h->b[j].next, v, ht - 1);
		return nullItem;
	}
	link splitR(link h)
	{
		link t = OK_NEW_OPERATOR node();
		for (int j = 0; j < M / 2; ++j)
			t->b[j] = h->b[M / 2 + j];
		h->m = M / 2; t->m = M / 2;
		return t;
	}
	link insertR(link h, Item x, int ht)
	{
		int i, j; Key v = x.key();
		entry t; t.key = v; t.item = x;
		if (ht == 0)
		for (j = 0; j < h->m; ++j)
		{
			if (v < h->b[j].key)
				break;
		}
		else
		for (j = 0; j < h->m; ++j)
		if ((j + 1 == h->m) || (v < h->b[j + 1].key))
		{
			link u;
			u = insertR(h->b[j].next, x, ht - 1);
			if (j == 0)
				h->b[0].key = h->b[0].next->b[0].key;
			if (u == 0) return 0;
			++j; t.key = u->b[0].key; t.next = u;
			break;
		}
		for (i = h->m; i > j; --i) h->b[i] = h->b[i - 1];
		h->b[j] = t;
		if (++(h->m) < M)
			return 0;
		else
			return splitR(h);
	}
	void joinR(link h, link t)
	{
		int j;

		for (j = 0; j < t->m; ++j)
			h->b[j + (h->m)] = t->b[j];
		h->m += t->m;
		delete t;
	}
	link removeR(link h, Key v, int ht)
	{
		int i, j; bool b;

		if (ht == 0)
		for (j = 0; j < h->m; ++j)
		{
			if (v == h->b[j].key)
				break;
		}
		else
		for (j = 0; j < h->m; ++j)
		if ((j + 1 == h->m) || (v < h->b[j + 1].key))
		{
			link u = removeR(h->b[j].next, v, ht - 1);
			if (u == 0) return 0;
			assert(h->b[j].next == u);
			if ((j == 0) && (u->m > 0))
				h->b[0].key = u->b[0].key;
			if ((h->m < 2) && (u->m == 0))
			{
				delete u;
				break;
			}
			if (j < (h->m - 1) && ((h->b[j + 1].next->m + u->m) < M))
			{
				b = ((j == 0) && (u->m == 0));
				joinR(u, h->b[j + 1].next);
				if (b)
					h->b[0].key = u->b[0].key;
				++j;
				break;
			}
			if ((j > 0) && ((h->b[j - 1].next->m + u->m) < M))
			{
				b = ((j == 1) && (h->b[j - 1].next->m == 0));
				joinR(h->b[j - 1].next, u);
				if (b)
					h->b[0].key = h->b[0].next->b[0].key;
				break;
			}
			return h;
		}
		for (i = j + 1; i < h->m; ++i) h->b[i - 1] = h->b[i];
		if (j < h->m)
		{
			h->b[i - 1].key = 0;
			h->b[i - 1].item = Item();
			h->b[i - 1].next = 0;
			--(h->m);
			return h;
		}
		return 0;
	}
	void deleteR(link h, int ht)
	{
		if (ht == 0)
			return;
		for (int j = 0; j < h->m; ++j)
		{
			deleteR(h->b[j].next, ht - 1);
			delete h->b[j].next;
		}
	}
	int countR(link h, int ht)
	{
		int result = 0;

		if (ht == 0)
			return h->m;
		for (int j = 0; j < h->m; ++j)
			result += countR(h->b[j].next, ht - 1);
		return result;
	}
public:
	Item nullItem;

	CBTreeT(int maxN = 0)
	{
		HT = 0; head = OK_NEW_OPERATOR node();
	}
	~CBTreeT()
	{
		deleteR(head, HT);
		delete head;
	}
	int height()
	{
		return HT;
	}
	int count()
	{
		return countR(head, HT);
	}
	Item search(Key v)
	{
		return searchR(head, v, HT);
	}
	void insert(Item item)
	{
		link u = insertR(head, item, HT);
		if (u == 0)
			return;
		link t = OK_NEW_OPERATOR node(); t->m = 2;
		t->b[0].key = head->b[0].key;
		t->b[1].key = u->b[0].key;
		t->b[0].next = head;
		t->b[1].next = u;
		head = t; ++HT;
	}
	void remove(Key v)
	{
		link u = removeR(head, v, HT);
		if (u == 0)
			return;
		assert(u == head);
		while ((HT > 0) && (u->m == 1))
		{
			head = u->b[0].next;
			delete u;
			--HT;
			u = head;
		}
	}
};

