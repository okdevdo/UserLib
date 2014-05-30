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
#include "DataVector.h"
#include "WinException.h"
#include "EventLogProvider.h"
#include "EventLogEvent.h"
#include <winevt.h>

DECL_WINEXCEPTION(WINSOURCES_API, CEventLogChannelException, CWinException)

class WINSOURCES_API CEventLogChannel : public CCppObject
{
public:
	CEventLogChannel(CConstPointer pName = NULL);
	virtual ~CEventLogChannel();

	BOOLEAN Load(CConstPointer pName);
	BOOLEAN LoadEvents(Ptr(CEventLogProviders) pProviders);

	__inline ConstRef(CStringBuffer) get_Name() const { return _name; }
	__inline ConstRef(CStringBuffer) get_name() const { return _name; }
	__inline BOOLEAN get_enabled() const { return _enabled; }
	__inline EVT_CHANNEL_ISOLATION_TYPE get_isolation() const { return _isolation; }
	__inline EVT_CHANNEL_TYPE get_type() const { return _type; }
	__inline ConstRef(CStringBuffer) get_owningPublisher() const { return _owningPublisher; }
	__inline BOOLEAN get_classicEventLog() const { return _classicEventLog; }
	__inline ConstRef(CStringBuffer) get_access() const { return _access; }
	__inline BOOLEAN get_retention() const { return _retention; }
	__inline BOOLEAN get_autoBackup() const { return _autoBackup; }
	__inline UINT64 get_maxSize() const { return _maxSize; }
	__inline ConstRef(CFilePath) get_logFilePath() const { return _logFilePath; }
	__inline UINT32 get_level() const { return _level; }
	__inline UINT64 get_keywords() const { return _keywords; }
	__inline ConstRef(CStringBuffer) get_controlGuid() const { return _controlGuid; }
	__inline UINT32 get_bufferSize() const { return _bufferSize; }
	__inline UINT32 get_minBuffers() const { return _minBuffers; }
	__inline UINT32 get_maxBuffers() const { return _maxBuffers; }
	__inline UINT32 get_latency() const { return _latency; }
	__inline EVT_CHANNEL_CLOCK_TYPE get_clockType() const { return _clockType; }
	__inline EVT_CHANNEL_SID_TYPE get_sidType() const { return _sidType; }
	__inline ConstRef(CDataVectorT<CStringBuffer>) get_publisherList() const { return _publisherList; }
	__inline UINT32 get_fileMax() const { return _fileMax; }
	__inline Ref(CEventLogEvents) get_events() { return _events; }

	__inline void set_name(ConstRef(CStringBuffer) name) { _name = name; }
	void set_enabled(BOOLEAN b); // You cannot set this property for the Application, System, and Security channels.
	void set_isolation(EVT_CHANNEL_ISOLATION_TYPE t); // You cannot set this property for the Application, System, and Security channels.
	void set_access(CConstPointer s);
	void set_retention(BOOLEAN b);
	void set_autoBackup(BOOLEAN b);
	void set_maxSize(UINT64 v);
	void set_logFilePath(ConstRef(CFilePath) p);
	void set_level(UINT32 v); // To set this property, you must first disable the debug or analytic channel.
	void set_keywords(UINT64 v); // To set this property, you must first disable the debug or analytic channel.
	void set_fileMax(UINT32 v);

protected:
	CStringBuffer _name;

	// EvtChannelConfigEnabled
	// Identifies the enabled attribute of the channel. The variant type for this property is EvtVarTypeBoolean.
	//
	// You cannot set this property for the Application, System, and Security channels.
	BOOLEAN _enabled;

	// EvtChannelConfigIsolation
	// Identifies the isolation attribute of the channel.The variant type for this property is EvtVarTypeUInt32.For possible isolation values, see the EVT_CHANNEL_ISOLATION_TYPE enumeration.
	//
	// You cannot set this property for the Application, System, and Security channels.
	EVT_CHANNEL_ISOLATION_TYPE _isolation;

	// EvtChannelConfigType
	// Identifies the type attribute of the channel.The variant type for this property is EvtVarTypeUInt32.For possible isolation values, see the EVT_CHANNEL_TYPE enumeration.
	//
	// You cannot set this property.
	EVT_CHANNEL_TYPE _type;

	// EvtChannelConfigOwningPublisher
	// Identifies the name attribute of the provider that defined the channel.The variant type for this property is EvtVarTypeString.
	//
	// You cannot set this property.
	CStringBuffer _owningPublisher;

	// EvtChannelConfigClassicEventlog
	// Identifies the configuration property that indicates whether the channel is a classic event channel(for example the Application or System log).The variant type for this property is EvtVarTypeBoolean.
	//
	// You cannot set this property.
	BOOLEAN _classicEventLog;

	// EvtChannelConfigAccess
	// Identifies the access attribute of the channel.The variant type for this property is EvtVarTypeString.
	CStringBuffer _access;

	// EvtChannelLoggingConfigRetention
	// Identifies the retention logging attribute of the channel.The variant type for this property is EvtVarTypeBoolean.
	BOOLEAN _retention; // TRUE(Sequential), FALSE(Circular)

	// EvtChannelLoggingConfigAutoBackup
	// Identifies the autoBackup logging attribute of the channel.The variant type for this property is EvtVarTypeBoolean.
	BOOLEAN _autoBackup;

	// EvtChannelLoggingConfigMaxSize
	// Identifies the maxSize logging attribute of the channel.The variant type for this property is EvtVarTypeUInt64.
	UINT64 _maxSize;

	// EvtChannelLoggingConfigLogFilePath
	// Identifies the configuration property that contains the path to the file that backs the channel.The variant type for this property is EvtVarTypeString.
	CFilePath _logFilePath;

	// EvtChannelPublishingConfigLevel
	// Identifies the level publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt32.
	//
	// To set this property, you must first disable the debug or analytic channel.
	UINT32 _level;

	// EvtChannelPublishingConfigKeywords
	// Identifies the keywords publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt64.
	//
	// To set this property, you must first disable the debug or analytic channel.
	UINT64 _keywords;

	// EvtChannelPublishingConfigControlGuid
	// Identifies the controlGuid publishing attribute of the channel.The variant type for this property is EvtVarTypeGuid.
	//
	// You cannot set this property.
	CStringBuffer _controlGuid;

	// EvtChannelPublishingConfigBufferSize
	// Identifies the bufferSize publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt32.
	//
	// You cannot set this property.
	UINT32 _bufferSize;

	// EvtChannelPublishingConfigMinBuffers
	// Identifies the minBuffers publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt32.
	//
	// You cannot set this property.
	UINT32 _minBuffers;

	// EvtChannelPublishingConfigMaxBuffers
	// Identifies the maxBuffers publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt32.
	//
	// You cannot set this property.
	UINT32 _maxBuffers;

	// EvtChannelPublishingConfigLatency
	// Identifies the latency publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt32.
	//
	// You cannot set this property.
	UINT32 _latency;

	// EvtChannelPublishingConfigClockType
	// Identifies the clockType publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt32.For possible clock type values, see the EVT_CHANNEL_CLOCK_TYPE enumeration.
	//
	// You cannot set this property.
	EVT_CHANNEL_CLOCK_TYPE _clockType;

	// EvtChannelPublishingConfigSidType
	// Identifies the sidType publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt32.For possible SID type values, see the EVT_CHANNEL_SID_TYPE enumeration.
	//
	// You cannot set this property.
	EVT_CHANNEL_SID_TYPE _sidType;

	// EvtChannelPublisherList
	// Identifies the configuration property that contains the list of providers that import this channel.The variant type for this property is EvtVarTypeString | EVT_VARIANT_TYPE_ARRAY.
	//
	// You cannot set this property.
	CDataVectorT<CStringBuffer> _publisherList;

	// EvtChannelPublishingConfigFileMax
	// Identifies the fileMax publishing attribute of the channel.The variant type for this property is EvtVarTypeUInt32.
	UINT32 _fileMax;

	CEventLogEvents _events;

private:
	void _load();

};

class WINSOURCES_API CEventLogChannels : public CDataVectorT<CEventLogChannel, CStringByNameLessFunctor<CEventLogChannel>>
{
	typedef CDataVectorT<CEventLogChannel, CStringByNameLessFunctor<CEventLogChannel>> super;

public:
	CEventLogChannels(DECL_FILE_LINE0);
	virtual ~CEventLogChannels();

	BOOLEAN Load();
	Ptr(CEventLogChannel) FindSorted(ConstRef(CStringBuffer) name);
};

