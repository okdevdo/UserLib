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
	struct ResponseDataItem
	{
		CStringBuffer Key;
		CStringBuffer Value;

		ConstRef(CStringBuffer) key() const
		{
			return Key;
		}

		ResponseDataItem(void) :
			Key(), Value()
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

	typedef CHashLinkedListT<ResponseDataItem, CStringBuffer, HashFunctorString> ResponseDataList;

	struct RequestDataItem
	{
		CStringBuffer Key;
		CStringBuffer Value;

		ConstRef(CStringBuffer) key() const
		{
			return Key;
		}

		RequestDataItem(void) :
			Key(), Value()
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

	typedef CHashLinkedListT<RequestDataItem, CStringBuffer, HashFunctorString> RequestDataList;

	struct StatusCode
	{
		long Status;
		CStringBuffer Reason;

		long key() const
		{
			return Status;
		}

		StatusCode(void) :
			Status(0), Reason()
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

	typedef CHashLinkedListT<StatusCode, long, HashFunctorDigit> StatusCodeList;

	void OpenRequestQueue(CConstPointer name);
	void ReceiveRequestHeader(Ref(RequestDataList) header, DWORD timeout = INFINITE);
	void ReceiveRequestBody(Ref(CByteLinkedBuffer) body);

	ConstRef(StatusCode) get_StatusCode(long statuscode) const;

	void SendResponse(ConstRef(StatusCode) statuscode, ConstRef(ResponseDataList) header, ConstRef(CByteLinkedBuffer) body, bool bLogging);

protected:
	bool _worker;
	Pointer _data;
};

DECL_WINEXCEPTION(WINSOURCES_API, CHttpServerException, CWinException)
