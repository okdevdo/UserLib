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

#include "ByteLinkedBuffer.h"
#include "DataHashtable.h"
#include "DataVector.h"

#include "WinSources.h"
#include "URL.h"
#include "SyncTCPSocketClasses.h"

class WINSOURCES_API CHttpClient: public CCppObject
{
public:
	enum LoadOptions
	{
		OPEN_TRANSIENT,
		OPEN_PERSISTENT,
		STAY_PERSISTENT,
		CLOSE_PERSISTENT
	};

	enum LoadCommand
	{
		GET_COMMAND,
		POST_COMMAND,
		HEAD_COMMAND
	};

	struct ResponseDataItem
	{
		CStringBuffer Key;
		CStringBuffer Value;

		ConstRef(CStringBuffer) key() const
		{
			return Key;
		}

		ResponseDataItem(void):
		    Key(), Value()
		{
		}
		ResponseDataItem(ConstRef(CStringBuffer) k, ConstRef(CStringBuffer) v):
		    Key(k), Value(v)
		{
		}
		ResponseDataItem(ConstRef(ResponseDataItem) copy):
		    Key(copy.Key), Value(copy.Value)
		{
		}
	};

	typedef CHashLinkedListT<ResponseDataItem, CStringBuffer, HashFunctorString> ResponseDataList;

public:
	CHttpClient(void);
	CHttpClient(ConstRef(CUrl) url);
	virtual ~CHttpClient(void);

	__inline ConstRef(CStringBuffer) get_ServerName() const { return _serverName; }
	__inline void set_ServerName(ConstRef(CStringBuffer) value) { _serverName = value; }
	__inline ConstRef(CStringBuffer) get_ServerBackupName() const { return _serverBackupName; }
	__inline void set_ServerBackupName(ConstRef(CStringBuffer) value) { _serverBackupName = value; }
	__inline ConstRef(CStringBuffer) get_ResourceString() const { return _resourceString; }
	__inline void set_ResourceString(ConstRef(CStringBuffer) value) { _resourceString = value; }
	__inline ConstRef(CStringBuffer) get_ResourceBackupString() const { return _resourceBackupString; }
	__inline void set_ResourceBackupString(ConstRef(CStringBuffer) value) { _resourceBackupString = value; }
	__inline bool get_DefaultURL() const { return _defaultURL; }
	__inline int get_BackupReason() const { return _backupReason; }
	__inline void set_BackupReason(int value) { _backupReason = value; }

	__inline dword get_RequestDataCount() const { return _requestData.count(); }
	__inline ConstRef(CStringBuffer) get_RequestData(ConstRef(CStringBuffer) name) const { return _requestData.search(name).Value; }
	__inline void set_RequestData(ConstRef(CStringBuffer) name, ConstRef(CStringBuffer) value) { ResponseDataItem item(name, value); _requestData.insert(item); }
	__inline void remove_RequestData(ConstRef(CStringBuffer) name) { _requestData.remove(name); }
	__inline ResponseDataList::iterator get_RequestDataBegin() { return _requestData.begin(); }

	__inline ConstRef(CStringBuffer) get_ResponseVersion() const { return _responseVersion; }
	__inline ConstRef(CStringBuffer) get_ResponseTypeNum() const { return _responseTypeNum; }
	__inline ConstRef(CStringBuffer) get_ResponseTypeText() const { return _responseTypeText; }
	__inline ConstRef(CByteLinkedBuffer) get_ResponseContent() const { return _responseContent; }

	__inline dword get_ResponseDataCount() const { return _responseData.count(); }
	__inline ConstRef(CStringBuffer) get_ResponseData(ConstRef(CStringBuffer) name) const { return _responseData.search(name).Value; }
	__inline ResponseDataList::iterator get_ResponseDataBegin() { return _responseData.begin(); }

	bool InitRequest(ConstRef(CStringBuffer) serverName, ConstRef(CStringBuffer) resourceString);
	bool InitRequest(ConstRef(CUrl) url);
	bool Load(LoadOptions options = OPEN_TRANSIENT, LoadCommand command = GET_COMMAND);
	void ClearResponse();
	void ClearAll();

	__inline bool CanLog() const { return _canlog; }
	__inline void SetLog(bool v) { _canlog = v; }
	__inline word GetErrCnt() const { return _errcnt; }
	__inline void SetErrCnt(word errCnt) { _errcnt = errCnt; }
	__inline ConstRef(CDataVectorT<CStringBuffer>) GetLog() const { return _log; }
	void ResetLog();

protected:
	bool Parse(Ref(CByteLinkedBuffer::Iterator) pos, int *state);
	void _InitRequest();
	void MakeCommand();
	void Log(ConstRef(CStringBuffer) text);

	// request data
	CStringBuffer _serverName;
	CStringBuffer _serverBackupName;
	CStringBuffer _resourceString;
	CStringBuffer _resourceBackupString;
	int _backupReason;
	bool _defaultURL;
	ResponseDataList _requestData;
	LoadCommand _command;
	CByteBuffer _getBuffer;
	// response data
	CStringBuffer _responseVersion;
	CStringBuffer _responseTypeNum;
	CStringBuffer _responseTypeText;
	ResponseDataList _responseData;
	CByteLinkedBuffer _responseContent;
    dword _totalsize;
	// tcp client
	CTcpClient _tcpClient;
	bool _open;
	// error cnt and logging
	word _errcnt;
	bool _canlog;
	CDataVectorT<CStringBuffer> _log;
};

