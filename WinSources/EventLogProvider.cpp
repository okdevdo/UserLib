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
#include "EventLogProvider.h"
#include <objbase.h>

IMPL_WINEXCEPTION(CEventLogProviderException, CWinException)

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CEventLogChannelException(ARGS_FILE_LINE _T("in %s CEventLogProviderException"),
		func, CWinException::WinExtError);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, CConstPointer sArg)
{
	throw OK_NEW_OPERATOR CEventLogChannelException(ARGS_FILE_LINE _T("in %s CEventLogProviderException (%s)"),
		func, sArg, CWinException::WinExtError);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, DWORD errCode)
{
	throw OK_NEW_OPERATOR CEventLogChannelException(ARGS_FILE_LINE _T("in %s CEventLogProviderException"),
		func, CWinException::WinExtError, errCode);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, CConstPointer sArg, DWORD errCode)
{
	throw OK_NEW_OPERATOR CEventLogChannelException(ARGS_FILE_LINE _T("in %s CEventLogProviderException (%s)"),
		func, sArg, CWinException::WinExtError, errCode);
}

static PEVT_VARIANT GetProperty(EVT_HANDLE handle, DWORD dwIndex, EVT_PUBLISHER_METADATA_PROPERTY_ID PropertyId)
{
	DWORD status = ERROR_SUCCESS;
	PEVT_VARIANT pvBuffer = NULL;
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;

	if (!EvtGetObjectArrayProperty(handle, PropertyId, dwIndex, 0, dwBufferSize, pvBuffer, &dwBufferUsed))
	{
		status = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER == status)
		{
			dwBufferSize = dwBufferUsed;
			pvBuffer = (PEVT_VARIANT)TFalloc(dwBufferSize);
			if (pvBuffer)
			{
				status = ERROR_SUCCESS;
				if (!EvtGetObjectArrayProperty(handle, PropertyId, dwIndex, 0, dwBufferSize, pvBuffer, &dwBufferUsed))
					status = GetLastError();
			}
			else
				status = ERROR_OUTOFMEMORY;
		}
		if (ERROR_SUCCESS != status)
		{
			if (pvBuffer)
				TFfree(pvBuffer);
			ThrowDefaultException(__FILE__LINE__ _T("GetProperty"), status);
		}
	}
	return pvBuffer;
}

static CStringBuffer GetMessageString(EVT_HANDLE hMetadata, UINT32 messageID)
{
	CStringBuffer pBuffer;
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;
	DWORD status = 0;

	if (!EvtFormatMessage(hMetadata, NULL, messageID, 0, NULL, EvtFormatMessageId, dwBufferSize, NULL, &dwBufferUsed))
	{
		status = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER == status)
		{
			dwBufferSize = dwBufferUsed;
			pBuffer.SetSize(__FILE__LINE__ dwBufferSize);

			if ((pBuffer.GetAllocSize() / szchar) >= dwBufferSize)
			{
				status = ERROR_SUCCESS;
				if (!EvtFormatMessage(hMetadata, NULL, messageID, 0, NULL, EvtFormatMessageId, dwBufferSize, CastMutable(CPointer, pBuffer.GetString()), &dwBufferUsed))
					status = GetLastError();
			}
			else
				status = ERROR_OUTOFMEMORY;
		}
		if ((ERROR_SUCCESS != status) && (ERROR_EVT_UNRESOLVED_VALUE_INSERT != status))
		{
			pBuffer.Clear();
			ThrowDefaultException(__FILE__LINE__ _T("GetMessageString"), status);
		}
	}
	pBuffer.ReplaceString(_T("\r\n"), _T("\n"));
	return pBuffer;
}

//================== CEventLogProviderChannel =========================================
CEventLogProviderChannel::CEventLogProviderChannel() :
_message(),
_path(),
_index(0xFFFFFFFF),
_id(0xFFFFFFFF),
_imported(FALSE)
{}

CEventLogProviderChannel::~CEventLogProviderChannel()
{}

BOOLEAN CEventLogProviderChannel::Load(EVT_HANDLE hMetadata, EVT_HANDLE hChannels, UINT32 dwIndex)
{
	PEVT_VARIANT pvBuffer = NULL;

	pvBuffer = GetProperty(hChannels, dwIndex, EvtPublisherMetadataChannelReferenceMessageID);
	if (pvBuffer)
	{
		_messageID = pvBuffer->UInt32Val;
		if (0xFFFFFFFF != _messageID)
			_message = GetMessageString(hMetadata, _messageID);
		TFfree(pvBuffer);
	}

	// This is the channel name. You can use it to call the EvtOpenChannelConfig function
	// to get the channel's configuration information.
	pvBuffer = GetProperty(hChannels, dwIndex, EvtPublisherMetadataChannelReferencePath);
	if (pvBuffer)
	{
		_path.SetString(__FILE__LINE__ pvBuffer->StringVal);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hChannels, dwIndex, EvtPublisherMetadataChannelReferenceIndex);
	if (pvBuffer)
	{
		_index = pvBuffer->UInt32Val;
		TFfree(pvBuffer);
	}

	// Capture the channel's value attribute, which is used to look up the channel's
	// message string.
	pvBuffer = GetProperty(hChannels, dwIndex, EvtPublisherMetadataChannelReferenceID);
	if (pvBuffer)
	{
		_id = pvBuffer->UInt32Val;
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hChannels, dwIndex, EvtPublisherMetadataChannelReferenceFlags);
	if (pvBuffer)
	{
		_imported = FALSE;
		if (EvtChannelReferenceImported == (EvtChannelReferenceImported & pvBuffer->UInt32Val))
			_imported = TRUE;
		TFfree(pvBuffer);
	}
	return TRUE;
}

//================== CEventLogProviderLevel =========================================
CEventLogProviderLevel::CEventLogProviderLevel():
_message(),
_name(),
_value(0xFFFFFFFF)
{}

CEventLogProviderLevel::~CEventLogProviderLevel()
{}

BOOLEAN CEventLogProviderLevel::Load(EVT_HANDLE hMetadata, EVT_HANDLE hLevels, UINT32 dwIndex)
{
	PEVT_VARIANT pvBuffer = NULL;

	pvBuffer = GetProperty(hLevels, dwIndex, EvtPublisherMetadataLevelMessageID);
	if (pvBuffer)
	{
		_messageID = pvBuffer->UInt32Val;
		if (0xFFFFFFFF != _messageID)
			_message = GetMessageString(hMetadata, _messageID);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hLevels, dwIndex, EvtPublisherMetadataLevelName);
	if (pvBuffer)
	{
		_name.SetString(__FILE__LINE__ pvBuffer->StringVal);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hLevels, dwIndex, EvtPublisherMetadataLevelValue);
	if (pvBuffer)
	{
		_value = pvBuffer->UInt32Val;
		TFfree(pvBuffer);
	}
	return TRUE;
}

//================== CEventLogProviderTask =========================================
CEventLogProviderTask::CEventLogProviderTask() :
_message(),
_name(),
_eventGuid(),
_value(0xFFFFFFFF)
{}

CEventLogProviderTask::~CEventLogProviderTask()
{}

BOOLEAN CEventLogProviderTask::Load(EVT_HANDLE hMetadata, EVT_HANDLE hTasks, UINT32 dwIndex)
{
	PEVT_VARIANT pvBuffer = NULL;

	pvBuffer = GetProperty(hTasks, dwIndex, EvtPublisherMetadataTaskMessageID);
	if (pvBuffer)
	{
		_messageID = pvBuffer->UInt32Val;
		if (0xFFFFFFFF != _messageID)
			_message = GetMessageString(hMetadata, _messageID);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hTasks, dwIndex, EvtPublisherMetadataTaskName);
	if (pvBuffer)
	{
		_name.SetString(__FILE__LINE__ pvBuffer->StringVal);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hTasks, dwIndex, EvtPublisherMetadataTaskEventGuid);
	if (pvBuffer)
	{
		if (!IsEqualGUID(GUID_NULL, *(pvBuffer->GuidVal)))
		{
			TCHAR wszEventGuid[50];

			StringFromGUID2(*(pvBuffer->GuidVal), wszEventGuid, sizeof(wszEventGuid) / sizeof(TCHAR));
			_eventGuid.SetString(__FILE__LINE__ wszEventGuid);
		}
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hTasks, dwIndex, EvtPublisherMetadataTaskValue);
	if (pvBuffer)
	{
		_value = pvBuffer->UInt32Val;
		TFfree(pvBuffer);
	}

	return TRUE;
}

//================== CEventLogProviderOpCode =========================================
CEventLogProviderOpCode::CEventLogProviderOpCode() :
_message(),
_name(),
_value(0xFFFFFFFF)
{}

CEventLogProviderOpCode::~CEventLogProviderOpCode()
{}

BOOLEAN CEventLogProviderOpCode::Load(EVT_HANDLE hMetadata, EVT_HANDLE hOpCodes, UINT32 dwIndex)
{
	PEVT_VARIANT pvBuffer = NULL;

	pvBuffer = GetProperty(hOpCodes, dwIndex, EvtPublisherMetadataOpcodeMessageID);
	if (pvBuffer)
	{
		_messageID = pvBuffer->UInt32Val;
		if (0xFFFFFFFF != _messageID)
			_message = GetMessageString(hMetadata, _messageID);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hOpCodes, dwIndex, EvtPublisherMetadataOpcodeName);
	if (pvBuffer)
	{
		_name.SetString(__FILE__LINE__ pvBuffer->StringVal);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hOpCodes, dwIndex, EvtPublisherMetadataOpcodeValue);
	if (pvBuffer)
	{
		//wprintf(L"\tOpcode value is %hu (task: %hu)\n", HIWORD(pvBuffer->UInt32Val), LOWORD(pvBuffer->UInt32Val));
		_value = pvBuffer->UInt32Val;
		TFfree(pvBuffer);
	}
	return TRUE;
}

//================== CEventLogProviderKeyWord =========================================
CEventLogProviderKeyWord::CEventLogProviderKeyWord() :
_message(),
_name(),
_value(0xFFFFFFFFFFFFFFFF)
{}

CEventLogProviderKeyWord::~CEventLogProviderKeyWord()
{}

BOOLEAN CEventLogProviderKeyWord::Load(EVT_HANDLE hMetadata, EVT_HANDLE hKeyWords, UINT32 dwIndex)
{
	PEVT_VARIANT pvBuffer = NULL;

	pvBuffer = GetProperty(hKeyWords, dwIndex, EvtPublisherMetadataKeywordMessageID);
	if (pvBuffer)
	{
		_messageID = pvBuffer->UInt32Val;
		if (0xFFFFFFFF != _messageID)
			_message = GetMessageString(hMetadata, _messageID);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hKeyWords, dwIndex, EvtPublisherMetadataKeywordName);
	if (pvBuffer)
	{
		_name.SetString(__FILE__LINE__ pvBuffer->StringVal);
		TFfree(pvBuffer);
	}

	pvBuffer = GetProperty(hKeyWords, dwIndex, EvtPublisherMetadataKeywordValue);
	if (pvBuffer)
	{
		_value = pvBuffer->UInt64Val;
		TFfree(pvBuffer);
	}
	return TRUE;
}

//================== CEventLogProviderEvent =========================================
static void __stdcall CEventLogProviderEventKeyWordDeleteFunc(ConstPointer data, Pointer context)
{
}

CEventLogProviderEvent::CEventLogProviderEvent() :
_id(0),
_version(0),
_channelValue(0),
_channel(NULL),
_levelValue(0),
_level(NULL),
_opCodeValue(0),
_opCode(NULL),
_taskValue(0),
_task(NULL),
_keyWordValue(0),
_keyWordList(__FILE__LINE__0),
_messageID(0),
_message(),
_template()
{}

CEventLogProviderEvent::~CEventLogProviderEvent()
{
}

//================== CEventLogProvider =========================================
static void __stdcall CEventLogProviderChannelDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogProviderChannel* pInfo = CastAnyPtr(CEventLogProviderChannel, CastMutable(Pointer, data));

	pInfo->release();
}

static void __stdcall CEventLogProviderLevelDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogProviderLevel* pInfo = CastAnyPtr(CEventLogProviderLevel, CastMutable(Pointer, data));

	pInfo->release();
}

static void __stdcall CEventLogProviderTaskDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogProviderTask* pInfo = CastAnyPtr(CEventLogProviderTask, CastMutable(Pointer, data));

	pInfo->release();
}

static void __stdcall CEventLogProviderOpCodeDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogProviderOpCode* pInfo = CastAnyPtr(CEventLogProviderOpCode, CastMutable(Pointer, data));

	pInfo->release();
}

static void __stdcall CEventLogProviderKeyWordDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogProviderKeyWord* pInfo = CastAnyPtr(CEventLogProviderKeyWord, CastMutable(Pointer, data));

	pInfo->release();
}

static void __stdcall CEventLogProviderEventDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogProviderEvent* pInfo = CastAnyPtr(CEventLogProviderEvent, CastMutable(Pointer, data));

	pInfo->release();
}

CEventLogProvider::CEventLogProvider() :
_name(),
_guid(),
_resourceFile(),
_parameterFile(),
_messageFile(),
_helpLink(),
_message(),
_channelList(__FILE__LINE__0),
_levelList(__FILE__LINE__0),
_taskList(__FILE__LINE__0),
_opCodeList(__FILE__LINE__0),
_keyWordList(__FILE__LINE__0),
_eventList(__FILE__LINE__0)
{
}

CEventLogProvider::~CEventLogProvider()
{
}

Ptr(CEventLogProviderChannel) CEventLogProvider::get_channel(UINT32 v) const
{
	CDataDoubleLinkedListT<CEventLogProviderChannel>::Iterator it = _channelList.Begin();
	Ptr(CEventLogProviderChannel) p = NULL;

	while (it)
	{
		p = *it;
		if (p->get_id() == v)
			return p;
		++it;
	}
	return NULL;
}

Ptr(CEventLogProviderLevel) CEventLogProvider::get_level(UINT32 v) const
{
	CDataDoubleLinkedListT<CEventLogProviderLevel>::Iterator it = _levelList.Begin();
	Ptr(CEventLogProviderLevel) p = NULL;

	while (it)
	{
		p = *it;
		if (p->get_value() == v)
			return p;
		++it;
	}
	return NULL;
}

Ptr(CEventLogProviderTask) CEventLogProvider::get_task(UINT32 v) const
{
	CDataDoubleLinkedListT<CEventLogProviderTask>::Iterator it = _taskList.Begin();
	Ptr(CEventLogProviderTask) p = NULL;

	while (it)
	{
		p = *it;
		if (p->get_value() == v)
			return p;
		++it;
	}
	return NULL;
}

// Used to get the message string or name for an opcode. Search the messages block sequentially 
// for an item that has the same opcode value (high word). Opcodes can be defined globally or 
// locally (task-specific). All global opcodes must be unique, but multiple tasks can specify the
// same opcode value, so we need to check the low word to see if the task on the event matches
// the task on the opcode.
Ptr(CEventLogProviderOpCode) CEventLogProvider::get_opCode(UINT32 vOpCode, UINT32 vTask) const
{
	CDataDoubleLinkedListT<CEventLogProviderOpCode>::Iterator it = _opCodeList.Begin();
	Ptr(CEventLogProviderOpCode) p = NULL;
	Ptr(CEventLogProviderOpCode) pSave = NULL;  // Points to the global opcode (low word is zero)

	while (it)
	{
		p = *it;
		if (vOpCode == HIWORD(p->get_value()))
		{
			if (0 == LOWORD(p->get_value()))
				pSave = p;
			else if (vTask == LOWORD(p->get_value()))
				return p;
		}
		++it;
	}
	return pSave;
}

// Used to get the message strings or names for the keywords specified on the event. The event
// contains a bit mask that has bits set for each keyword specified on the event. Search the 
// messages block sequentially for items that have the same keyword bit set. Concatenate all the
// message strings.
CEventLogProviderKeyWords CEventLogProvider::get_keyWords(UINT64 v) const
{
	CEventLogProviderKeyWords result __FILE__LINE__0P;
	CEventLogProviderKeyWords::Iterator it = _keyWordList.Begin();
	Ptr(CEventLogProviderKeyWord) p = NULL;

	while (it)
	{
		p = *it;
		if (p->get_value() & v)
		{
			p->addRef();
			result.Append(p);
		}
		++it;
	}
	return result;
}

Ptr(CEventLogProviderEvent) CEventLogProvider::get_event(UINT32 eventID) const
{
	CDataDoubleLinkedListT<CEventLogProviderEvent>::Iterator it = _eventList.Begin();
	Ptr(CEventLogProviderEvent) p = NULL;

	while (it)
	{
		p = *it;
		if (p->get_id() == eventID)
			return p;
		++it;
	}
	return NULL;
}

BOOLEAN CEventLogProvider::Load(CConstPointer pName)
{
	_name.SetString(__FILE__LINE__ pName);
	_load();
	return TRUE;
}

BOOLEAN CEventLogProvider::Load(ConstRef(CStringBuffer) name)
{
	_name = name;
	_load();
	return TRUE;
}

void CEventLogProvider::_load()
{
	EVT_HANDLE hMetadata;
	PEVT_VARIANT pProperty = NULL;
	PEVT_VARIANT pTemp = NULL;
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;
	DWORD status = ERROR_SUCCESS;
	DWORD dwArraySize = 0;
	DWORD dwBlockSize = 0;
	LPTSTR pMessage = NULL;
	EVT_HANDLE hEvents = NULL;
	EVT_HANDLE hEvent = NULL;

	if (_name.IsEmpty())
		return;

	if (NULL == (hMetadata = EvtOpenPublisherMetadata(NULL,
		_name.GetString(),
		NULL,
		0, //MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), 0), 
		0)))
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogProvider::_load"), _name.GetString());

	for (int Id = 0; Id < EvtPublisherMetadataPropertyIdEND; Id++)
	{
		if (!EvtGetPublisherMetadataProperty(hMetadata, (EVT_PUBLISHER_METADATA_PROPERTY_ID)Id, 0, dwBufferSize, pProperty, &dwBufferUsed))
		{
			status = GetLastError();
			if (ERROR_INSUFFICIENT_BUFFER == status)
			{
				dwBufferSize = dwBufferUsed;
				if (pProperty)
					pTemp = (PEVT_VARIANT)TFrealloc(pProperty, dwBufferSize);
				else
					pTemp = (PEVT_VARIANT)TFalloc(dwBufferSize);
				if (pTemp)
				{
					pProperty = pTemp;
					pTemp = NULL;
					status = ERROR_SUCCESS;
					if (!EvtGetPublisherMetadataProperty(hMetadata, (EVT_PUBLISHER_METADATA_PROPERTY_ID)Id, 0, dwBufferSize, pProperty, &dwBufferUsed))
						status = GetLastError();
				}
				else
					status = ERROR_OUTOFMEMORY;
			}
			if (ERROR_SUCCESS != status)
			{
				if (pProperty)
					TFfree(pProperty);
				EvtClose(hMetadata);
				ThrowDefaultException(__FILE__LINE__ _T("CEventLogProvider::_load"), _name.GetString(), status);
			}
		}

		switch (Id)
		{
		case EvtPublisherMetadataPublisherGuid:
			{
				TCHAR wszProviderGuid[50];

				StringFromGUID2(*(pProperty->GuidVal), wszProviderGuid, sizeof(wszProviderGuid) / sizeof(TCHAR));
				_guid.SetString(__FILE__LINE__ wszProviderGuid);
			}
			break;

		case EvtPublisherMetadataResourceFilePath:
			_resourceFile.set_Path(CStringBuffer(__FILE__LINE__ pProperty->StringVal));
			break;

		case EvtPublisherMetadataParameterFilePath:
			_parameterFile.set_Path(CStringBuffer(__FILE__LINE__ (EvtVarTypeNull == pProperty->Type) ? _T("") : pProperty->StringVal));
			break;

		case EvtPublisherMetadataMessageFilePath:
			_messageFile.set_Path(CStringBuffer(__FILE__LINE__ pProperty->StringVal));
			break;

		case EvtPublisherMetadataHelpLink:
			_helpLink.SetString(__FILE__LINE__ pProperty->StringVal);
			break;

			// The message string ID is -1 if the provider element does not specify the message attribute.
		case EvtPublisherMetadataPublisherMessageID:
			if (0xFFFFFFFF != pProperty->UInt32Val)
			{
				try
				{
					_message = GetMessageString(hMetadata, pProperty->UInt32Val);	
				}
				catch (CBaseException*)
				{
				}
			}
			break;

			// We got the handle to all the channels defined in the channels section
			// of the manifest. Get the size of the array of channel objects and 
			// allocate the messages block that will contain the value and
			// message string for each channel. The strings are used to retrieve
			// display names for the channel referenced in an event definition.
		case EvtPublisherMetadataChannelReferences:
			if (EvtGetObjectArraySize(pProperty->EvtHandleVal, &dwArraySize))
			{
				for (DWORD i = 0; i < dwArraySize; i++)
				{
					Ptr(CEventLogProviderChannel) p = OK_NEW_OPERATOR CEventLogProviderChannel;

					try
					{
						p->Load(hMetadata, pProperty->EvtHandleVal, i);
						_channelList.Append(p);
					}
					catch (CBaseException*)
					{
					}
				}
			}
			EvtClose(pProperty->EvtHandleVal);
			break;

			// These are handled by the EvtPublisherMetadataChannelReferences case;
			// they are here for completeness but will never be exercised.
		case EvtPublisherMetadataChannelReferencePath:
		case EvtPublisherMetadataChannelReferenceIndex:
		case EvtPublisherMetadataChannelReferenceID:
		case EvtPublisherMetadataChannelReferenceFlags:
		case EvtPublisherMetadataChannelReferenceMessageID:
			break;

			// We got the handle to all the levels defined in the channels section
			// of the manifest. Get the size of the array of level objects and 
			// allocate the messages block that will contain the value and
			// message string for each level. The strings are used to retrieve
			// display names for the level referenced in an event definition.
			// References to the levels defined in Winmeta.xml are included in 
			// the list.
		case EvtPublisherMetadataLevels:
			if (EvtGetObjectArraySize(pProperty->EvtHandleVal, &dwArraySize))
			{
				for (DWORD i = 0; i < dwArraySize; i++)
				{
					Ptr(CEventLogProviderLevel) p = OK_NEW_OPERATOR CEventLogProviderLevel;

					try
					{
						p->Load(hMetadata, pProperty->EvtHandleVal, i);
						_levelList.Append(p);
					}
					catch (CBaseException*)
					{
					}
				}
			}
			EvtClose(pProperty->EvtHandleVal);
			break;

			// These are handled by the EvtPublisherMetadataLevels case;
			// they are here for completeness but will never be exercised.
		case EvtPublisherMetadataLevelName:
		case EvtPublisherMetadataLevelValue:
		case EvtPublisherMetadataLevelMessageID:
			break;

			// We got the handle to all the tasks defined in the channels section
			// of the manifest. Get the size of the array of task objects and 
			// allocate the messages block that will contain the value and
			// message string for each task. The strings are used to retrieve
			// display names for the task referenced in an event definition.
		case EvtPublisherMetadataTasks:
			if (EvtGetObjectArraySize(pProperty->EvtHandleVal, &dwArraySize))
			{
				for (DWORD i = 0; i < dwArraySize; i++)
				{
					Ptr(CEventLogProviderTask) p = OK_NEW_OPERATOR CEventLogProviderTask;

					try
					{
						p->Load(hMetadata, pProperty->EvtHandleVal, i);
						_taskList.Append(p);
					}
					catch (CBaseException*)
					{
					}
				}
			}
			EvtClose(pProperty->EvtHandleVal);
			break;

			// These are handled by the EvtPublisherMetadataTasks case;
			// they are here for completeness but will never be exercised.
		case EvtPublisherMetadataTaskName:
		case EvtPublisherMetadataTaskEventGuid:
		case EvtPublisherMetadataTaskValue:
		case EvtPublisherMetadataTaskMessageID:
			break;

			// We got the handle to all the opcodes defined in the channels section
			// of the manifest. Get the size of the array of opcode objects and 
			// allocate the messages block that will contain the value and
			// message string for each opcode. The strings are used to retrieve
			// display names for the opcode referenced in an event definition.
		case EvtPublisherMetadataOpcodes:
			if (EvtGetObjectArraySize(pProperty->EvtHandleVal, &dwArraySize))
			{
				for (DWORD i = 0; i < dwArraySize; i++)
				{
					Ptr(CEventLogProviderOpCode) p = OK_NEW_OPERATOR CEventLogProviderOpCode;

					try
					{
						p->Load(hMetadata, pProperty->EvtHandleVal, i);
						_opCodeList.Append(p);
					}
					catch (CBaseException*)
					{
					}
				}
			}
			EvtClose(pProperty->EvtHandleVal);
			break;

			// These are handled by the EvtPublisherMetadataOpcodes case;
			// they are here for completeness but will never be exercised.
		case EvtPublisherMetadataOpcodeName:
		case EvtPublisherMetadataOpcodeValue:
		case EvtPublisherMetadataOpcodeMessageID:
			break;

			// We got the handle to all the keywords defined in the channels section
			// of the manifest. Get the size of the array of keyword objects and 
			// allocate the messages block that will contain the value and
			// message string for each keyword. The strings are used to retrieve
			// display names for the keyword referenced in an event definition.
		case EvtPublisherMetadataKeywords:
			if (EvtGetObjectArraySize(pProperty->EvtHandleVal, &dwArraySize))
			{
				for (DWORD i = 0; i < dwArraySize; i++)
				{
					Ptr(CEventLogProviderKeyWord) p = OK_NEW_OPERATOR CEventLogProviderKeyWord;

					try
					{
						p->Load(hMetadata, pProperty->EvtHandleVal, i);
						_keyWordList.Append(p);
					}
					catch (CBaseException*)
					{
					}
				}
			}
			EvtClose(pProperty->EvtHandleVal);
			break;

			// These are handled by the EvtPublisherMetadataKeywords case;
			// they are here for completeness but will never be exercised.
		case EvtPublisherMetadataKeywordName:
		case EvtPublisherMetadataKeywordValue:
		case EvtPublisherMetadataKeywordMessageID:
			break;

		default:
			break;
		}

		RtlZeroMemory(pProperty, dwBufferUsed);

		// Skip the type-specific IDs, so the loop doesn't fail. For channels, levels,
		// opcodes, tasks, and keywords, you use EvtGetPublisherMetadataProperty 
		// to get a handle to an array of those objects. You would then use the type
		// specific ID (for example, EvtPublisherMetadataLevelValue) to access the metadata from
		// the array. Do not call EvtGetPublisherMetadataProperty with a type specific ID or it 
		// will fail. The switch statement increments to the end of the type specific IDs for 
		// each type.
		switch (Id)
		{
		case EvtPublisherMetadataChannelReferences:
			Id = EvtPublisherMetadataChannelReferenceMessageID;
			break;

		case EvtPublisherMetadataLevels:
			Id = EvtPublisherMetadataLevelMessageID;
			break;

		case EvtPublisherMetadataOpcodes:
			Id = EvtPublisherMetadataOpcodeMessageID;
			break;

		case EvtPublisherMetadataTasks:
			Id = EvtPublisherMetadataTaskMessageID;
			break;

		case EvtPublisherMetadataKeywords:
			Id = EvtPublisherMetadataKeywordMessageID;
			break;
		}
	}

	if (NULL == (hEvents = EvtOpenEventMetadataEnum(hMetadata, 0)))
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogProvider::_load"));
	while (true)
	{
		if (NULL == (hEvent = EvtNextEventMetadata(hEvents, 0)))
		{
			status = GetLastError();
			if (ERROR_NO_MORE_ITEMS != status)
			{
				EvtClose(hEvents);
				if (pProperty)
					TFfree(pProperty);
				EvtClose(hMetadata);
				ThrowDefaultException(__FILE__LINE__ _T("CEventLogProvider::_load"), _name.GetString(), status);
			}				
			break;
		}

		Ptr(CEventLogProviderEvent) pEvent = OK_NEW_OPERATOR CEventLogProviderEvent;

		for (int Id = 0; Id < EvtEventMetadataPropertyIdEND; Id++)
		{
			if (!EvtGetEventMetadataProperty(hEvent, (EVT_EVENT_METADATA_PROPERTY_ID)Id, 0, dwBufferSize, pProperty, &dwBufferUsed))
			{
				status = GetLastError();
				if (ERROR_INSUFFICIENT_BUFFER == status)
				{
					dwBufferSize = dwBufferUsed;
					if (pProperty)
						pTemp = (PEVT_VARIANT)TFrealloc(pProperty, dwBufferSize);
					else
						pTemp = (PEVT_VARIANT)TFalloc(dwBufferSize);
					if (pTemp)
					{
						pProperty = pTemp;
						pTemp = NULL;
						status = ERROR_SUCCESS;
						if (!EvtGetEventMetadataProperty(hEvent, (EVT_EVENT_METADATA_PROPERTY_ID)Id, 0, dwBufferSize, pProperty, &dwBufferUsed))
							status = GetLastError();
					}
					else
						status = ERROR_OUTOFMEMORY;
				}
				if (ERROR_SUCCESS != status)
				{
					EvtClose(hEvent);
					EvtClose(hEvents);
					if (pProperty)
						TFfree(pProperty);
					EvtClose(hMetadata);
					ThrowDefaultException(__FILE__LINE__ _T("CEventLogProvider::_load"), _name.GetString(), status);
				}
			}

			switch (Id)
			{
			case EventMetadataEventID:
				pEvent->set_id(pProperty->UInt32Val);
				break;

			case EventMetadataEventVersion:
				pEvent->set_version(pProperty->UInt32Val);
				break;

				// The channel property contains the value of the channel's value attribute.
				// Instead of printing the value attribute, use it to find the channel's
				// message string or name and print it.
			case EventMetadataEventChannel:
				pEvent->set_channelValue(pProperty->UInt32Val);
				if (pProperty->UInt32Val > 0)
					pEvent->set_channel(get_channel(pProperty->UInt32Val));
				break;

				// The level property contains the value of the level's value attribute.
				// Instead of printing the value attribute, use it to find the level's
				// message string or name and print it.
			case EventMetadataEventLevel:
				pEvent->set_levelValue(pProperty->UInt32Val);
				if (pProperty->UInt32Val > 0)
					pEvent->set_level(get_level(pProperty->UInt32Val));
				break;

				// The opcode property contains the value of the opcode's value attribute.
				// Instead of printing the value attribute, use it to find the opcode's
				// message string or name and print it.
				// The opcode value contains the opcode in the high word. If the opcode is 
				// task-specific, the opcode value will contain the task value in the low word.
				// Save the opcode in a static variable and print it when we get the task
				// value, so we can decide if the opcode is task-specific.
			case EventMetadataEventOpcode:
				pEvent->set_opCodeValue(pProperty->UInt32Val);
				break;

				// The task property contains the value of the task's value attribute.
				// Instead of printing the value attribute, use it to find the task's
				// message string or name and print it.
			case EventMetadataEventTask:
				pEvent->set_taskValue(pProperty->UInt32Val);
				if (pProperty->UInt32Val > 0)
					pEvent->set_task(get_task(pProperty->UInt32Val));
				// Now that we know the task, get the opcode string and print it.
				if (pEvent->get_opCodeValue() > 0)
					pEvent->set_opCode(get_opCode(pEvent->get_opCodeValue(), pProperty->UInt32Val));
				break;

				// The keyword property contains a bit mask of all the keywords.
				// Instead of printing the value attribute, use it to find the 
				// message string or name associated with each keyword and print them (space delimited).
			case EventMetadataEventKeyword:
				// The upper 8 bits can contain reserved bit values, so do not include them
				// when checking to see if any keywords are set.
				pEvent->set_keyWordValue(pProperty->UInt64Val);
				if ((pProperty->UInt64Val & 0x00FFFFFFFFFFFFFF) > 0)
				{
					pEvent->set_keyWordList(get_keyWords(pProperty->UInt64Val));
				}
				break;

				// If the message string is not specified, the value is -1.
			case EventMetadataEventMessageID:
				pEvent->set_messageID(pProperty->UInt32Val);
				if (0xFFFFFFFF != pProperty->UInt32Val)
				{
					try
					{
						pEvent->set_message(GetMessageString(hMetadata, pProperty->UInt32Val));
					}
					catch (CBaseException*)
					{
					}
				}
				break;

				// When you define the event, the template attribute contains the template
				// identifier; however, the template metadata contains an XML string of the 
				// template (includes the data items, not the UserData section).
			case EventMetadataEventTemplate:
				{
					CStringBuffer vTemplate(__FILE__LINE__ pProperty->StringVal);

					vTemplate.ReplaceString(_T("\r\n"), _T("\n"));
					pEvent->set_template(vTemplate);
				}
				break;

			default:
				break;
			}

			RtlZeroMemory(pProperty, dwBufferUsed);
		}
		_eventList.Append(pEvent);

		EvtClose(hEvent);
		hEvent = NULL;
	}

	if (hEvents)
		EvtClose(hEvents);

	if (hEvent)
		EvtClose(hEvent);

	if (pProperty)
		TFfree(pProperty);

	EvtClose(hMetadata);
}

//================== CEventLogProviders =========================================
static sword __stdcall CEventLogProvidersSearchAndSortFunc(ConstPointer pa, ConstPointer pb)
{
	CEventLogProvider* ppa = CastAnyPtr(CEventLogProvider, CastMutable(Pointer, pa));
	CEventLogProvider* ppb = CastAnyPtr(CEventLogProvider, CastMutable(Pointer, pb));

	return (ppa->get_name().Compare(ppb->get_name()));
}

static void __stdcall CEventLogProvidersDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogProvider* pInfo = CastAnyPtr(CEventLogProvider, CastMutable(Pointer, data));

	pInfo->release();
}

CEventLogProviders::CEventLogProviders(DECL_FILE_LINE0) :
	super(ARGS_FILE_LINE 16, 256)
{}

CEventLogProviders::~CEventLogProviders()
{
}

BOOLEAN CEventLogProviders::Load()
{
	EVT_HANDLE hProviders = NULL;
	LPTSTR pwcsProviderName = NULL;
	LPTSTR pTemp = NULL;
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;
	DWORD status = ERROR_SUCCESS;

	if (NULL == (hProviders = EvtOpenPublisherEnum(NULL, 0)))
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogProviders::Load"));

	while (true)
	{
		if (!EvtNextPublisherId(hProviders, dwBufferSize, pwcsProviderName, &dwBufferUsed))
		{
			status = GetLastError();
			if (ERROR_NO_MORE_ITEMS == status)
				break;
			if (ERROR_INSUFFICIENT_BUFFER == status)
			{
				dwBufferSize = dwBufferUsed;
				if (pwcsProviderName)
					pTemp = (LPTSTR)TFrealloc(pwcsProviderName, dwBufferSize * sizeof(TCHAR));
				else
					pTemp = (LPTSTR)TFalloc(dwBufferSize * sizeof(TCHAR));
				if (pTemp)
				{
					pwcsProviderName = pTemp;
					pTemp = NULL;
					status = ERROR_SUCCESS;
					if (!EvtNextPublisherId(hProviders, dwBufferSize, pwcsProviderName, &dwBufferUsed))
						status = GetLastError();
				}
				else
					status = ERROR_OUTOFMEMORY;
			}
			if (ERROR_SUCCESS != status)
			{
				if (pwcsProviderName)
					TFfree(pwcsProviderName);
				EvtClose(hProviders);
				ThrowDefaultException(__FILE__LINE__ _T("CEventLogProviders::Load"), status);
			}
		}

		Ptr(CEventLogProvider) provider = OK_NEW_OPERATOR CEventLogProvider;

		try
		{
			provider->Load(pwcsProviderName);
			InsertSorted(provider);
		}
		catch (CWinException*)
		{
			if (provider)
				provider->release();
			provider = NULL;
		}
		RtlZeroMemory(pwcsProviderName, dwBufferUsed * sizeof(TCHAR));
	}

	if (pwcsProviderName)
		TFfree(pwcsProviderName);

	if (hProviders)
		EvtClose(hProviders);
	return TRUE;
}

Ptr(CEventLogProvider) CEventLogProviders::FindSorted(ConstRef(CStringBuffer) name)
{
	CEventLogProvider toFind;

	toFind.set_name(name);

	Iterator fIt = super::FindSorted(&toFind);

	if (super::MatchSorted(fIt, &toFind))
		return *fIt;
	return NULL;
}