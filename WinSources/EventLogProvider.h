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
#include "FilePath.h"
#include "EventLogChannel.h"
#include "DataDoubleLinkedList.h"
#include "WinException.h"
#include <winevt.h>

DECL_WINEXCEPTION(WINSOURCES_API, CEventLogProviderException, CWinException)

class WINSOURCES_API CEventLogProviderChannel : public CCppObject
{
public:
	CEventLogProviderChannel();
	virtual ~CEventLogProviderChannel();

	BOOLEAN Load(EVT_HANDLE hMetadata, EVT_HANDLE hChannels, UINT32 dwIndex);

	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline UINT32 get_messageID() const { return _messageID; }
	__inline ConstRef(CStringBuffer) get_path() const { return _path; }
	__inline UINT32 get_index() const { return _index; }
	__inline UINT32 get_id() const { return _id; }
	__inline BOOLEAN get_imported() const { return _imported; }

protected:
	CStringBuffer _message;
	UINT32 _messageID;
	CStringBuffer _path;
	UINT32 _index;
	UINT32 _id;
	BOOLEAN _imported;
};

class WINSOURCES_API CEventLogProviderLevel : public CCppObject
{
public:
	CEventLogProviderLevel();
	virtual ~CEventLogProviderLevel();

	BOOLEAN Load(EVT_HANDLE hMetadata, EVT_HANDLE hLevels, UINT32 dwIndex);

	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline UINT32 get_messageID() const { return _messageID; }
	__inline ConstRef(CStringBuffer) get_name() const { return _name; }
	__inline UINT32 get_value() const { return _value; }

protected:
	CStringBuffer _message;
	UINT32 _messageID;
	CStringBuffer _name;
	UINT32 _value;
};

class WINSOURCES_API CEventLogProviderTask : public CCppObject
{
public:
	CEventLogProviderTask();
	virtual ~CEventLogProviderTask();

	BOOLEAN Load(EVT_HANDLE hMetadata, EVT_HANDLE hTasks, UINT32 dwIndex);

	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline UINT32 get_messageID() const { return _messageID; }
	__inline ConstRef(CStringBuffer) get_name() const { return _name; }
	__inline ConstRef(CStringBuffer) get_eventGuid() const { return _eventGuid; }
	__inline UINT32 get_value() const { return _value; }

protected:
	CStringBuffer _message;
	UINT32 _messageID;
	CStringBuffer _name;
	CStringBuffer _eventGuid;
	UINT32 _value;
};

class WINSOURCES_API CEventLogProviderOpCode : public CCppObject
{
public:
	CEventLogProviderOpCode();
	virtual ~CEventLogProviderOpCode();

	BOOLEAN Load(EVT_HANDLE hMetadata, EVT_HANDLE hOpCodes, UINT32 dwIndex);

	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline UINT32 get_messageID() const { return _messageID; }
	__inline ConstRef(CStringBuffer) get_name() const { return _name; }
	__inline UINT32 get_value() const { return _value; }

protected:
	CStringBuffer _message;
	UINT32 _messageID;
	CStringBuffer _name;
	UINT32 _value;
};

class WINSOURCES_API CEventLogProviderKeyWord : public CCppObject
{
public:
	CEventLogProviderKeyWord();
	virtual ~CEventLogProviderKeyWord();

	BOOLEAN Load(EVT_HANDLE hMetadata, EVT_HANDLE hKeyWords, UINT32 dwIndex);

	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline UINT32 get_messageID() const { return _messageID; }
	__inline ConstRef(CStringBuffer) get_name() const { return _name; }
	__inline UINT64 get_value() const { return _value; }

protected:
	CStringBuffer _message;
	UINT32 _messageID;
	CStringBuffer _name;
	UINT64 _value;
};

class WINSOURCES_API CEventLogProviderEvent : public CCppObject
{
public:
	CEventLogProviderEvent();
	virtual ~CEventLogProviderEvent();

	__inline UINT32 get_id() const { return _id; }
	__inline void set_id(UINT32 v) { _id = v; }
	__inline UINT32 get_version() const { return _version; }
	__inline void set_version(UINT32 v) { _version = v; }
	__inline UINT32 get_channelValue() const { return _channelValue; }
	__inline void set_channelValue(UINT32 v) { _channelValue = v; }
	__inline ConstPtr(CEventLogProviderChannel) get_channel() const { return _channel; }
	__inline void set_channel(Ptr(CEventLogProviderChannel) p) { _channel = p; }
	__inline UINT32 get_levelValue() const { return _levelValue; }
	__inline void set_levelValue(UINT32 v) { _levelValue = v; }
	__inline ConstPtr(CEventLogProviderLevel) get_level() const { return _level; }
	__inline void set_level(Ptr(CEventLogProviderLevel) p) { _level = p; }
	__inline UINT32 get_opCodeValue() const { return _opCodeValue; }
	__inline void set_opCodeValue(UINT32 v) { _opCodeValue = v; }
	__inline ConstPtr(CEventLogProviderOpCode) get_opCode() const { return _opCode; }
	__inline void set_opCode(Ptr(CEventLogProviderOpCode) p) { _opCode = p; }
	__inline UINT32 get_taskValue() const { return _taskValue; }
	__inline void set_taskValue(UINT32 v) { _taskValue = v; }
	__inline ConstPtr(CEventLogProviderTask) get_task() const { return _task; }
	__inline void set_task(Ptr(CEventLogProviderTask) p) { _task = p; }
	__inline UINT64 get_keyWordValue() const { return _keyWordValue; }
	__inline void set_keyWordValue(UINT64 v) { _keyWordValue = v; }
	__inline ConstRef(CDataDoubleLinkedListT<CEventLogProviderKeyWord>) get_keyWordList() const { return _keyWordList; }
	__inline void set_keyWordList(ConstRef(CDataDoubleLinkedListT<CEventLogProviderKeyWord>) p) 
	{ 
		_keyWordList = p; 
		//COUT << _T("set_keyWordList: ") << _keyWordList.RefCount() << endl;
	}
	__inline UINT32 get_messageID() const { return _messageID; }
	__inline void set_messageID(UINT32 v) { _messageID = v; }
	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline void set_message(ConstRef(CStringBuffer) s) { _message = s; }
	__inline ConstRef(CStringBuffer) get_template() const { return _template; }
	__inline void set_template(ConstRef(CStringBuffer) s) { _template = s; }

protected:
	UINT32 _id;
	UINT32 _version;
	UINT32 _channelValue;
	Ptr(CEventLogProviderChannel) _channel;
	UINT32 _levelValue;
	Ptr(CEventLogProviderLevel) _level;
	UINT32 _opCodeValue;
	Ptr(CEventLogProviderOpCode) _opCode;
	UINT32 _taskValue;
	Ptr(CEventLogProviderTask) _task;
	UINT64 _keyWordValue;
	CDataDoubleLinkedListT<CEventLogProviderKeyWord> _keyWordList;
	UINT32 _messageID;
	CStringBuffer _message;
	CStringBuffer _template;
};

class WINSOURCES_API CEventLogProvider : public CCppObject
{
public:
	CEventLogProvider();
	virtual ~CEventLogProvider();

	BOOLEAN Load(CConstPointer pName);
	BOOLEAN Load(ConstRef(CStringBuffer) name);

	__inline ConstRef(CStringBuffer) get_name() const { return _name; }
	__inline ConstRef(CStringBuffer) get_guid() const { return _guid; }
	__inline ConstRef(CFilePath) get_resourceFile() const { return _resourceFile; }
	__inline ConstRef(CFilePath) get_parameterFile() const { return _parameterFile; }
	__inline ConstRef(CFilePath) get_messageFile() const { return _messageFile; }
	__inline ConstRef(CStringBuffer) get_helpLink() const { return _helpLink; }
	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline ConstRef(CDataDoubleLinkedListT<CEventLogProviderChannel>) get_channelList() const { return _channelList; }
	__inline ConstRef(CDataDoubleLinkedListT<CEventLogProviderLevel>) get_levelList() const { return _levelList; }
	__inline ConstRef(CDataDoubleLinkedListT<CEventLogProviderTask>) get_taskList() const { return _taskList; }
	__inline ConstRef(CDataDoubleLinkedListT<CEventLogProviderOpCode>) get_opCodeList() const { return _opCodeList; }
	__inline ConstRef(CDataDoubleLinkedListT<CEventLogProviderKeyWord>) get_keyWordList() const { return _keyWordList; }
	__inline ConstRef(CDataDoubleLinkedListT<CEventLogProviderEvent>) get_eventList() const { return _eventList; }

	__inline void set_name(ConstRef(CStringBuffer) name) { _name = name; }
	__inline void set_name(CConstPointer pName) { _name.SetString(__FILE__LINE__ pName); }

	Ptr(CEventLogProviderChannel) get_channel(UINT32 v) const;
	Ptr(CEventLogProviderLevel) get_level(UINT32 v) const;
	Ptr(CEventLogProviderTask) get_task(UINT32 v) const;
	Ptr(CEventLogProviderOpCode) get_opCode(UINT32 vOpCode, UINT32 vTask) const;
	CDataDoubleLinkedListT<CEventLogProviderKeyWord> get_keyWords(UINT64 v) const;
	Ptr(CEventLogProviderEvent) get_event(UINT32 eventID) const;

protected:
	CStringBuffer _name;
	CStringBuffer _guid;
	CFilePath _resourceFile;
	CFilePath _parameterFile;
	CFilePath _messageFile;
	CStringBuffer _helpLink;
	CStringBuffer _message;
	CDataDoubleLinkedListT<CEventLogProviderChannel> _channelList;
	CDataDoubleLinkedListT<CEventLogProviderLevel> _levelList;
	CDataDoubleLinkedListT<CEventLogProviderTask> _taskList;
	CDataDoubleLinkedListT<CEventLogProviderOpCode> _opCodeList;
	CDataDoubleLinkedListT<CEventLogProviderKeyWord> _keyWordList;
	CDataDoubleLinkedListT<CEventLogProviderEvent> _eventList;

private:

	void _load();
};

class WINSOURCES_API CEventLogProviders : public CDataVectorT<CEventLogProvider>
{
	typedef CDataVectorT<CEventLogProvider> super;

public:
	CEventLogProviders(DECL_FILE_LINE0);
	virtual ~CEventLogProviders();

	BOOLEAN Load();
	BOOLEAN ForEach(TForEachFunc func, Pointer context) const;
	Ptr(CEventLogProvider) FindSorted(ConstRef(CStringBuffer) name);
};

