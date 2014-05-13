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
#include "WS_PCH.H"
#include "EventLogChannel.h"
#include <objbase.h>

IMPL_WINEXCEPTION(CEventLogChannelException, CWinException)

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CEventLogChannelException(ARGS_FILE_LINE _T("in %s CEventLogChannelException"),
		func, CWinException::WinExtError);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, CConstPointer sArg1)
{
	throw OK_NEW_OPERATOR CEventLogChannelException(ARGS_FILE_LINE _T("in %s CEventLogChannelException (%s)"),
		func, sArg1, CWinException::WinExtError);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, DWORD errCode)
{
	throw OK_NEW_OPERATOR CEventLogChannelException(ARGS_FILE_LINE _T("in %s CEventLogChannelException"),
		func, CWinException::WinExtError, errCode);
}

//================== CEventLogChannel =========================================
CEventLogChannel::CEventLogChannel(CConstPointer pName) :
_name(__FILE__LINE__ pName),
_enabled(FALSE),
_isolation(EvtChannelIsolationTypeApplication),
_type(EvtChannelTypeAdmin),
_owningPublisher(),
_classicEventLog(FALSE),
_access(),
_retention(FALSE),
_autoBackup(FALSE),
_maxSize(0LL),
_logFilePath(),
_level(0L),
_keywords(0LL),
_controlGuid(),
_bufferSize(0L),
_minBuffers(0L),
_maxBuffers(0L),
_latency(0L),
_clockType(EvtChannelClockTypeSystemTime),
_sidType(EvtChannelSidTypeNone),
_publisherList(__FILE__LINE__ 8, 16),
_fileMax(0L),
_events(__FILE__LINE__0)
{
	_load();
}

CEventLogChannel::~CEventLogChannel()
{
}

BOOLEAN CEventLogChannel::Load(CConstPointer pName)
{
	_name.SetString(__FILE__LINE__ pName);
	_load();
	return TRUE;
}

BOOLEAN CEventLogChannel::LoadEvents(Ptr(CEventLogProviders) pProviders)
{
	if (!_enabled)
		return FALSE;
	_events.Load(_name, pProviders);
	return TRUE;
}

void CEventLogChannel::set_enabled(BOOLEAN b)
{
	if (_enabled != b)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_enabled"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeBoolean;
		ChannelProperty.BooleanVal = b;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelConfigEnabled, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_enabled"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_enabled"));
		}

		EvtClose(hChannel);
		_enabled = b;
	}
}

void CEventLogChannel::set_isolation(EVT_CHANNEL_ISOLATION_TYPE t)
{
	if (_isolation != t)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_isolation"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeUInt32;
		ChannelProperty.UInt32Val = t;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelConfigIsolation, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_isolation"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_isolation"));
		}

		EvtClose(hChannel);
		_isolation = t;
	}
}

void CEventLogChannel::set_access(CConstPointer s)
{
	if (_access.Compare((CStringLiteral)s) != 0)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_access"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeString;
		ChannelProperty.StringVal = s;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelConfigAccess, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_access"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_access"));
		}

		EvtClose(hChannel);
		_access.SetString(__FILE__LINE__ s);
	}
}

void CEventLogChannel::set_retention(BOOLEAN b)
{
	if (_retention != b)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_retention"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeBoolean;
		ChannelProperty.BooleanVal = b;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelLoggingConfigRetention, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_retention"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_retention"));
		}

		EvtClose(hChannel);
		_retention = b;
	}
}

void CEventLogChannel::set_autoBackup(BOOLEAN b)
{
	if (_autoBackup != b)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_autoBackup"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeBoolean;
		ChannelProperty.BooleanVal = b;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelLoggingConfigAutoBackup, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_autoBackup"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_autoBackup"));
		}

		EvtClose(hChannel);
		_autoBackup = b;
	}
}

void CEventLogChannel::set_maxSize(UINT64 v)
{
	if (_maxSize != v)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_maxSize"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeUInt64;
		ChannelProperty.UInt64Val = v;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelLoggingConfigMaxSize, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_maxSize"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_maxSize"));
		}

		EvtClose(hChannel);
		_maxSize = v;
	}
}

void CEventLogChannel::set_logFilePath(ConstRef(CFilePath) p)
{
	if (_logFilePath.get_Path() != p.get_Path())
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_logFilePath"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeString;
		ChannelProperty.StringVal = p.get_Path().GetString();
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelLoggingConfigLogFilePath, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_logFilePath"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_logFilePath"));
		}

		EvtClose(hChannel);
		_logFilePath = p;
	}
}

void CEventLogChannel::set_level(UINT32 v)
{
	if (_level != v)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_level"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeUInt32;
		ChannelProperty.UInt32Val = v;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelPublishingConfigLevel, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_level"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_level"));
		}

		EvtClose(hChannel);
		_level = v;
	}
}

void CEventLogChannel::set_keywords(UINT64 v)
{
	if (_keywords != v)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_keywords"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeUInt64;
		ChannelProperty.UInt64Val = v;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelPublishingConfigKeywords, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_keywords"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_keywords"));
		}

		EvtClose(hChannel);
		_keywords = v;
	}
}

void CEventLogChannel::set_fileMax(UINT32 v)
{
	if (_fileMax != v)
	{
		EVT_HANDLE hChannel = NULL;
		EVT_VARIANT ChannelProperty;
		DWORD dwBufferSize = sizeof(EVT_VARIANT);

		hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0);
		if (NULL == hChannel)
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_fileMax"), _name.GetString());

		RtlZeroMemory(&ChannelProperty, dwBufferSize);
		ChannelProperty.Type = EvtVarTypeUInt32;
		ChannelProperty.UInt32Val = v;
		if (!EvtSetChannelConfigProperty(hChannel, EvtChannelPublishingConfigFileMax, 0, &ChannelProperty))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_fileMax"));
		}

		if (!EvtSaveChannelConfig(hChannel, 0))
		{
			EvtClose(hChannel);
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::set_fileMax"));
		}

		EvtClose(hChannel);
		_fileMax = v;
	}
}

void CEventLogChannel::_load()
{
	EVT_HANDLE hChannel = NULL;
	PEVT_VARIANT pProperty = NULL;
	PEVT_VARIANT pTemp = NULL;
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;
	DWORD status = ERROR_SUCCESS;

	if (_name.IsEmpty())
		return;
	if (NULL == (hChannel = EvtOpenChannelConfig(NULL, _name.GetString(), 0)))
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::_load"), _name.GetString());

	for (int Id = 0; Id < EvtChannelConfigPropertyIdEND; Id++)
	{
		if (!EvtGetChannelConfigProperty(hChannel, (EVT_CHANNEL_CONFIG_PROPERTY_ID)Id, 0, dwBufferSize, pProperty, &dwBufferUsed))
		{
			status = GetLastError();
			if (ERROR_INSUFFICIENT_BUFFER == status)
			{
				dwBufferSize = dwBufferUsed;
				if (NULL == pProperty)
					pTemp = (PEVT_VARIANT)TFalloc(dwBufferSize);
				else
					pTemp = (PEVT_VARIANT)TFrealloc(pProperty, dwBufferSize);
				if (pTemp)
				{
					pProperty = pTemp;
					pTemp = NULL;
					status = ERROR_SUCCESS;
					if (!EvtGetChannelConfigProperty(hChannel, (EVT_CHANNEL_CONFIG_PROPERTY_ID)Id, 0, dwBufferSize, pProperty, &dwBufferUsed))
						status = GetLastError();
				}
				else
					status = ERROR_OUTOFMEMORY;
			}
			if (ERROR_SUCCESS != status)
			{
				if (pProperty)
					TFfree(pProperty);
				EvtClose(hChannel);
				ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannel::_load"), status);
			}
		}

		switch (Id)
		{
		case EvtChannelConfigEnabled:
			_enabled = pProperty->BooleanVal;
			break;
		case EvtChannelConfigIsolation:
			_isolation = (EVT_CHANNEL_ISOLATION_TYPE)(pProperty->UInt32Val);
			break;
		case EvtChannelConfigType:
			_type = (EVT_CHANNEL_TYPE)(pProperty->UInt32Val);
			break;
		case EvtChannelConfigOwningPublisher:
			_owningPublisher.SetString(__FILE__LINE__ pProperty->StringVal);
			break;
		case EvtChannelConfigClassicEventlog:
			_classicEventLog = pProperty->BooleanVal;
			break;
		case EvtChannelConfigAccess:
			_access.SetString(__FILE__LINE__ pProperty->StringVal);
			break;
		case EvtChannelLoggingConfigRetention:
			_retention = pProperty->BooleanVal;
			break;
		case EvtChannelLoggingConfigAutoBackup:
			_autoBackup = pProperty->BooleanVal;
			break;
		case EvtChannelLoggingConfigMaxSize:
			_maxSize = pProperty->UInt64Val;
			break;
		case EvtChannelLoggingConfigLogFilePath:
			_logFilePath.set_Path(__FILE__LINE__ pProperty->StringVal);
			break;
		case EvtChannelPublishingConfigLevel:
			if (EvtVarTypeNull == pProperty->Type)
				_level = 0xFFFFFFFF;
			else
				_level = pProperty->UInt32Val;
			break;
		case EvtChannelPublishingConfigKeywords:
			if (EvtVarTypeNull == pProperty->Type)
				_keywords = 0xFFFFFFFFFFFFFFFF;
			else
				_keywords = pProperty->UInt64Val;
			break;
		case EvtChannelPublishingConfigControlGuid:
			if (EvtVarTypeNull != pProperty->Type)
			{
				TCHAR wszSessionGuid[50];

				StringFromGUID2(*(pProperty->GuidVal), wszSessionGuid, sizeof(wszSessionGuid) / sizeof(TCHAR));
				_controlGuid.SetString(__FILE__LINE__ wszSessionGuid);
			}
			break;
		case EvtChannelPublishingConfigBufferSize:
			_bufferSize = pProperty->UInt32Val;
			break;
		case EvtChannelPublishingConfigMinBuffers:
			_minBuffers = pProperty->UInt32Val;
			break;
		case EvtChannelPublishingConfigMaxBuffers:
			_maxBuffers = pProperty->UInt32Val;
			break;
		case EvtChannelPublishingConfigLatency:
			_latency = pProperty->UInt32Val;
			break;
		case EvtChannelPublishingConfigClockType:
			_clockType = (EVT_CHANNEL_CLOCK_TYPE)(pProperty->UInt32Val);
			break;
		case EvtChannelPublishingConfigSidType:
			_sidType = (EVT_CHANNEL_SID_TYPE)(pProperty->UInt32Val);
			break;
		case EvtChannelPublisherList:
			for (DWORD i = 0; i < pProperty->Count; i++)
			{
				CStringBuffer sTemp(__FILE__LINE__ pProperty->StringArr[i]);

				_publisherList.Append(sTemp);
			}
			break;
		case EvtChannelPublishingConfigFileMax:
			_fileMax = pProperty->UInt32Val;
			break;
		default:
			break;
		}
	}
	if (pProperty)
		TFfree(pProperty);
	if (hChannel)
		EvtClose(hChannel);
}

//================== CEventLogChannels =========================================
static sword __stdcall CEventLogChannelsSearchAndSortFunc(ConstPointer pa, ConstPointer pb)
{
	CEventLogChannel* ppa = CastAnyPtr(CEventLogChannel, CastMutable(Pointer, pa));
	CEventLogChannel* ppb = CastAnyPtr(CEventLogChannel, CastMutable(Pointer, pb));

	return (ppa->get_name().Compare(ppb->get_name()));
}

static void __stdcall CEventLogChannelsDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogChannel* pInfo = CastAnyPtr(CEventLogChannel, CastMutable(Pointer, data));

	pInfo->release();
}

CEventLogChannels::CEventLogChannels(DECL_FILE_LINE0) :
	CDataVectorT<CEventLogChannel>(ARGS_FILE_LINE 16, 256, CEventLogChannelsDeleteFunc, NULL, CEventLogChannelsSearchAndSortFunc)
{
}

CEventLogChannels::~CEventLogChannels()
{
}

BOOLEAN CEventLogChannels::Load()
{
	EVT_HANDLE hChannels = NULL;
	LPTSTR pBuffer = NULL;
	LPTSTR pTemp = NULL;
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;
	DWORD status = ERROR_SUCCESS;

	if (NULL == (hChannels = EvtOpenChannelEnum(NULL, 0)))
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannels::Load"));

	while (true)
	{
		if (!EvtNextChannelPath(hChannels, dwBufferSize, pBuffer, &dwBufferUsed))
		{
			status = GetLastError();
			if (ERROR_NO_MORE_ITEMS == status)
				break;
			if (ERROR_INSUFFICIENT_BUFFER == status)
			{
				dwBufferSize = dwBufferUsed;
				if (pBuffer)
					pTemp = (LPTSTR)TFrealloc(pBuffer, dwBufferSize * sizeof(TCHAR));
				else
					pTemp = (LPTSTR)TFalloc(dwBufferSize * sizeof(TCHAR));
				if (pTemp)
				{
					pBuffer = pTemp;
					pTemp = NULL;

					status = ERROR_SUCCESS;
					if (!EvtNextChannelPath(hChannels, dwBufferSize, pBuffer, &dwBufferUsed))
						status = GetLastError();
				}
				else
					status = ERROR_OUTOFMEMORY;
			}
			if (ERROR_SUCCESS != status)
			{
				if (pBuffer)
					TFfree(pBuffer);
				EvtClose(hChannels);
				ThrowDefaultException(__FILE__LINE__ _T("CEventLogChannels::Load"), status);
			}
		}

		Ptr(CEventLogChannel) pChannel = OK_NEW_OPERATOR CEventLogChannel;

		try
		{
			pChannel->Load(pBuffer);
			InsertSorted(pChannel);
		}
		catch (CBaseException*)
		{
			if (pChannel)
				pChannel->release();
			pChannel = NULL;
		}
	}

	if (hChannels)
		EvtClose(hChannels);

	if (pBuffer)
		TFfree(pBuffer);

	return TRUE;
}

BOOLEAN CEventLogChannels::ForEach(TForEachFunc func, Pointer context) const
{
	Iterator it = Begin();

	while (it)
	{
		if (!func(*it, context))
			return FALSE;
		++it;
	}
	return TRUE;
}

Ptr(CEventLogChannel) CEventLogChannels::FindSorted(ConstRef(CStringBuffer) name)
{
	CEventLogChannel toFind;

	toFind.set_name(name);

	Iterator fIt = super::FindSorted(&toFind);

	if (fIt)
		return *fIt;
	//Iterator it = Begin();

	//while (it)
	//{
	//	if ((*it)->get_name().Compare(name) == 0)
	//		return *it;
	//	++it;
	//}
	return NULL;
}
