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
#include "DataVector.h"
#include "okDateTime.h"
#include "WinException.h"
#include <winevt.h>

DECL_WINEXCEPTION(WINSOURCES_API, CEventLogEventException, CWinException)

class WINSOURCES_API CEventLogProviders;
class WINSOURCES_API CEventLogEvent : public CCppObject
{
public:
	CEventLogEvent();
	virtual ~CEventLogEvent();

	BOOLEAN Load(EVT_HANDLE hEvent, Ptr(CEventLogProviders) pProviders);

	__inline ConstRef(CStringBuffer) get_providerName() const { return _providerName; }
	__inline ConstRef(CStringBuffer) get_providerGuid() const { return _providerGuid; }
	__inline DWORD32 get_eventID() const { return _eventID; }
	__inline DWORD32 get_qualifiers() const { return _qualifiers; }
	__inline DWORD32 get_version() const { return _version; }
	__inline DWORD32 get_level() const { return _level; }
	__inline ConstRef(CStringBuffer) get_levelName() const { return _levelName; }
	__inline ConstRef(CStringBuffer) get_levelMessage() const { return _levelMessage; }
	__inline DWORD32 get_task() const { return _task; }
	__inline ConstRef(CStringBuffer) get_taskName() const { return _taskName; }
	__inline ConstRef(CStringBuffer) get_taskMessage() const { return _taskMessage; }
	__inline ConstRef(CStringBuffer) get_taskEventGuid() const { return _taskEventGuid; }
	__inline DWORD32 get_opCode() const { return _opCode; }
	__inline ConstRef(CStringBuffer) get_opCodeName() const { return _opCodeName; }
	__inline ConstRef(CStringBuffer) get_opCodeMessage() const { return _opCodeMessage; }
	__inline DWORD64 get_keyWords() const { return _keyWords; }
	__inline ConstRef(CStringBuffer) get_keyWordsName() const { return _keyWordsName; }
	__inline ConstRef(CStringBuffer) get_keyWordsMessage() const { return _keyWordsMessage; }
	__inline ConstRef(CDateTime) get_creationTime() const { return _creationTime; }
	__inline DWORD64 get_eventRecordID() const { return _eventRecordID; }
	__inline ConstRef(CStringBuffer) get_activityID() const { return _activityID; }
	__inline ConstRef(CStringBuffer) get_relatedActivityID() const { return _relatedActivityID; }
	__inline DWORD32 get_processID() const { return _processID; }
	__inline DWORD32 get_threadID() const { return _threadID; }
	__inline ConstRef(CStringBuffer) get_channel() const { return _channel; }
	__inline ConstRef(CStringBuffer) get_computer() const { return _computer; }
	__inline ConstRef(CStringBuffer) get_userSID() const { return _userSID; }
	__inline ConstRef(CStringBuffer) get_userAccount() const { return _userAccount; }
	__inline ConstRef(CStringBuffer) get_userDomain() const { return _userDomain; }
	__inline SID_NAME_USE get_userUse() const { return _userUse; }
	CStringBuffer get_userUseString() const;
	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline DWORD32 get_messageID() const { return _messageID; }
	__inline DWORD32 get_userDataCount() const { return _userDataCount; }
	__inline PEVT_VARIANT get_userData() const { return _userData; }
	__inline ConstRef(CStringBuffer) get_template() const { return _template; }

	__inline void set_eventRecordID(DWORD64 v) { _eventRecordID = v; }

protected:
	CStringBuffer _providerName;
	CStringBuffer _providerGuid;
	DWORD32 _eventID;
	DWORD32 _qualifiers;
	DWORD32 _version;
	DWORD32 _level;
	CStringBuffer _levelName;
	CStringBuffer _levelMessage;
	DWORD32 _task;
	CStringBuffer _taskName;
	CStringBuffer _taskMessage;
	CStringBuffer _taskEventGuid;
	DWORD32 _opCode;
	CStringBuffer _opCodeName;
	CStringBuffer _opCodeMessage;
	DWORD64 _keyWords;
	CStringBuffer _keyWordsName;
	CStringBuffer _keyWordsMessage;
	CDateTime _creationTime;
	DWORD64 _eventRecordID;
	CStringBuffer _activityID;
	CStringBuffer _relatedActivityID;
	DWORD32 _processID;
	DWORD32 _threadID;
	CStringBuffer _channel;
	CStringBuffer _computer;
	CStringBuffer _userSID;
	CStringBuffer _userAccount;
	CStringBuffer _userDomain;
	SID_NAME_USE _userUse;
	CStringBuffer _message;
	DWORD32 _messageID;
	DWORD32 _userDataCount;
	PEVT_VARIANT _userData;
	CStringBuffer _template;
};

class WINSOURCES_API CEventLogEvents : public CDataVectorT<CEventLogEvent>
{
	typedef CDataVectorT<CEventLogEvent> super;

public:

	CEventLogEvents(DECL_FILE_LINE0);
	~CEventLogEvents();

	BOOLEAN Load(ConstRef(CStringBuffer) channelPath, Ptr(CEventLogProviders) pProviders);
	BOOLEAN ForEach(TForEachFunc func, Pointer context, bool bReverse = false) const;
	Ptr(CEventLogEvent) FindSorted(DWORD64 eventRecordID) const;
};
