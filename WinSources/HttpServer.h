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

#include "FilePath.h"
#include "DataHashtable.h"
#include "ByteLinkedBuffer.h"

#include "WinSources.h"
#include "WinException.h"
#include "URL.h"

class WINSOURCES_API CHttpServer : public CCppObject
{
public:
	CHttpServer(bool bWorker = false);
	virtual ~CHttpServer(void);

	void Initialize(bool bWorker = false);
	void Uninitialize();

	void ShutdownRequestQueue();
	void CloseRequestQueue();

	// Server
	void CreateRequestQueue(CConstPointer name);
	void AddUrl(ConstRef(CStringBuffer) url, qword context);
	void RegisterWorker(ConstRef(CFilePath) path, CConstPointer args);
	void SetupLogging(CConstPointer logConfigPathPrefix, CConstPointer _defaultappname);
	void RunServer();
	void StopServer();

	// Worker
	class ResponseDataItem: public CCppObject
	{
	public:
		CStringBuffer Key;
		CStringBuffer Value;

		ResponseDataItem(void) :
			Key(), Value()
		{
		}
		ResponseDataItem(ConstRef(CStringBuffer) k) :
			Key(k), Value()
		{
		}
		ResponseDataItem(ConstRef(CStringBuffer) k, ConstRef(CStringBuffer) v) :
			Key(k), Value(v)
		{
		}
		ResponseDataItem(ConstRef(ResponseDataItem) copy) :
			Key(copy.Key), Value(copy.Value)
		{
		}
	};

	class TResponseDataItemHashFunctor
	{
	public:
		TResponseDataItemHashFunctor(sdword cnt) : hs(cnt) {}

		sdword operator()(ConstPtr(ResponseDataItem) p) const
		{
			return hs(p->Key);
		}

	protected:
		HashFunctorString hs;
	};

	class TResponseDataItemLessFunctor
	{
	public:
		bool operator()(ConstPtr(ResponseDataItem) p1, ConstPtr(ResponseDataItem) p2) const
		{
			return p1->Key.LT(p2->Key);
		}
	};

	typedef CDataHashLinkedListT<ResponseDataItem, TResponseDataItemHashFunctor, TResponseDataItemLessFunctor> TResponseDataItems;

	class RequestDataItem: public CCppObject
	{
	public:
		CStringBuffer Key;
		CStringBuffer Value;

		RequestDataItem(void) :
			Key(), Value()
		{
		}
		RequestDataItem(ConstRef(CStringBuffer) k) :
			Key(k), Value()
		{
		}
		RequestDataItem(ConstRef(CStringBuffer) k, ConstRef(CStringBuffer) v) :
			Key(k), Value(v)
		{
		}
		RequestDataItem(ConstRef(RequestDataItem) copy) :
			Key(copy.Key), Value(copy.Value)
		{
		}
	};

	class TRequestDataItemHashFunctor
	{
	public:
		TRequestDataItemHashFunctor(sdword cnt) : hs(cnt) {}

		sdword operator()(ConstPtr(RequestDataItem) p) const
		{
			return hs(p->Key);
		}

	protected:
		HashFunctorString hs;
	};

	class TRequestDataItemLessFunctor
	{
	public:
		bool operator()(ConstPtr(RequestDataItem) p1, ConstPtr(RequestDataItem) p2) const
		{
			return p1->Key.LT(p2->Key);
		}
	};

	typedef CDataHashLinkedListT<RequestDataItem, TRequestDataItemHashFunctor, TRequestDataItemLessFunctor> TRequestDataItems;

	class StatusCode: public CCppObject
	{
	public:
		long Status;
		CStringBuffer Reason;

		StatusCode(void) :
			Status(0), Reason()
		{
		}
		StatusCode(long k) :
			Status(k), Reason()
		{
		}
		StatusCode(long k, CConstPointer v) :
			Status(k), Reason(__FILE__LINE__ v)
		{
		}
		StatusCode(long k, ConstRef(CStringBuffer) v) :
			Status(k), Reason(v)
		{
		}
		StatusCode(ConstRef(StatusCode) copy) :
			Status(copy.Status), Reason(copy.Reason)
		{
		}
	};

	class TStatusCodeHashFunctor
	{
	public:
		TStatusCodeHashFunctor(sdword cnt) : hd(cnt) {}

		sdword operator()(ConstPtr(StatusCode) p) const
		{
			return hd(p->Status);
		}

	protected:
		HashFunctorDigit hd;
	};

	class TStatusCodeLessFunctor
	{
	public:
		bool operator()(ConstPtr(StatusCode) p1, ConstPtr(StatusCode) p2) const
		{
			return p1->Status < p2->Status;
		}
	};

	typedef CDataHashLinkedListT<StatusCode, TStatusCodeHashFunctor, TStatusCodeLessFunctor> TStatusCodes;

	void OpenRequestQueue(CConstPointer name);
	void ReceiveRequestHeader(Ref(TRequestDataItems) header, DWORD timeout = INFINITE);
	void ReceiveRequestBody(Ref(CByteLinkedBuffer) body);

	ConstPtr(StatusCode) get_StatusCode(long statuscode) const;

	void SendResponse(ConstPtr(StatusCode) statuscode, ConstRef(TResponseDataItems) header, ConstRef(CByteLinkedBuffer) body, bool bLogging);

protected:
	bool _worker;
	Pointer _data;
};

DECL_WINEXCEPTION(WINSOURCES_API, CHttpServerException, CWinException)
