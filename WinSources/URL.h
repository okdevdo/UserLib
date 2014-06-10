/******************************************************************************
    
	This file is part of WinSources, which is part of UserLib.

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

#include "WinSources.h"
#include "DataHashtable.h"

class WINSOURCES_API CUrl: public CCppObject
{
public:
	CUrl(CConstPointer url = NULL, int chLen = -1);
	CUrl(ConstRef(CStringBuffer) url);
	virtual ~CUrl(void);

	class QueryDataItem: public CCppObject
	{
	public:
		CStringBuffer Key;
		CStringBuffer Value;

		QueryDataItem(void) :
			Key(), Value()
		{
		}
		QueryDataItem(ConstRef(CStringBuffer) a) :
			Key(), Value()
		{
			CStringConstIterator it(a);

			it.Find(_T('='));
			if (it.IsEnd())
				Key = a;
			else
			{
				a.SubString(0, it.GetDistance(), Key);
				a.SubString(it.GetDistance() + 1, it.GetLength() - 1, Value);
			}
		}
		QueryDataItem(ConstRef(CStringBuffer) k, ConstRef(CStringBuffer) v) :
			Key(k), Value(v)
		{
		}
		QueryDataItem(ConstRef(QueryDataItem) copy) :
			Key(copy.Key), Value(copy.Value)
		{
		}
		virtual ~QueryDataItem() {}
	};

	class TQueryDataItemHashFunctor
	{
	public:
		TQueryDataItemHashFunctor(sdword cnt) : hs(cnt) {}

		sdword operator()(ConstPtr(QueryDataItem) p) const
		{
			return hs(p->Key);
		}

	protected:
		HashFunctorString hs;
	};

	class TQueryDataItemLessFunctor
	{
	public:
		bool operator()(ConstPtr(QueryDataItem) p1, ConstPtr(QueryDataItem) p2) const
		{
			return p1->Key.LT(p2->Key);
		}
	};

	typedef CDataHashLinkedListT<QueryDataItem, TQueryDataItemHashFunctor, TQueryDataItemLessFunctor> TQueryDataItems;

	class QueryDataList : public TQueryDataItems
	{
		typedef TQueryDataItems super;

	public:
		QueryDataList(): super(__FILE__LINE__ 250, TQueryDataItemHashFunctor(250)) {}
		virtual ~QueryDataList() {}

		void split(ConstRef(CStringBuffer) a)
		{
			CStringBuffer tmp(a);
			CPointer p[64];
			dword cnt = 0;

			tmp.SplitAny(_T("&;"), p, 64, &cnt);
			for (dword i = 0; i < cnt; ++i)
				InsertSorted(OK_NEW_OPERATOR QueryDataItem(CStringBuffer(__FILE__LINE__ p[i])));
		}
	};

	CStringBuffer get_Url() const;
	bool set_Url(CConstPointer url, int chLen = -1);
	bool set_Url(ConstRef(CStringBuffer) url);

	__inline ConstRef(CStringBuffer) get_Protocol() const { return _protocol; }
	__inline ConstRef(CStringBuffer) get_Server() const { return _server; }
	__inline ConstRef(CStringBuffer) get_Resource() const { return _resource; }
	__inline ConstRef(QueryDataList) get_Queries() const { return _queries; }
	__inline ConstRef(CStringBuffer) get_Fragment() const { return _fragment; }

	__inline void set_Protocol(ConstRef(CStringBuffer) v) { _protocol = v; }
	__inline void set_Server(ConstRef(CStringBuffer) v) { _server = v; }
	__inline void set_Resource(ConstRef(CStringBuffer) v) { _resource = v; }
	__inline void set_Fragment(ConstRef(CStringBuffer) v) { _fragment = v; }
	void add_Query(ConstRef(CStringBuffer) k, ConstRef(CStringBuffer) v);

	void clear();

	enum EncodeType
	{
		EncodeTypeStandard = 0,
		EncodeTypeUnescaped = -1,
		EncodeTypePart = 1
	};

	CStringBuffer get_Encoded(EncodeType encodetype);
	void set_Encoded(CConstPointer encodedstr);

private:
	bool ParseUrl(CConstPointer url = NULL, int chLen = -1);
	bool ParseUrl(ConstRef(CStringBuffer) url);
	CStringBuffer _protocol;
	CStringBuffer _server;
	CStringBuffer _resource;
	QueryDataList _queries;
	CStringBuffer _fragment;
};

