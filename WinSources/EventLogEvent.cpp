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
#include "EventLogEvent.h"
#include "EventLogProvider.h"
#include <objbase.h>
#include <Sddl.h>

IMPL_WINEXCEPTION(CEventLogEventException, CWinException)

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CEventLogEventException(ARGS_FILE_LINE _T("in %s CEventLogEventException"),
		func, CWinException::WinExtError);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, CConstPointer sArg1)
{
	throw OK_NEW_OPERATOR CEventLogEventException(ARGS_FILE_LINE _T("in %s CEventLogEventException (%s)"),
		func, sArg1, CWinException::WinExtError);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, dword errCode)
{
	throw OK_NEW_OPERATOR CEventLogEventException(ARGS_FILE_LINE _T("in %s CEventLogEventException"),
		func, CWinException::WinExtError, errCode);
}

static void InsertValues(Ref(CStringBuffer) text, DWORD dwPropCount, PEVT_VARIANT pProp)
{
	CStringBuffer index;
	CStringBuffer value;

	text.ReplaceString(_T("%n"), _T("\r\n"));
	text.ReplaceString(_T("%t"), _T("\t"));
	for (DWORD i = 0; i < dwPropCount; ++i)
	{
		index.FormatString(__FILE__LINE__ _T("%%%d"), i + 1);
		if (pProp[i].Type & EVT_VARIANT_TYPE_ARRAY)
			continue;
		value.SetString(__FILE__LINE__ _T(""));
		switch (pProp[i].Type & EVT_VARIANT_TYPE_MASK)
		{
		case EvtVarTypeNull:
			break;
		case EvtVarTypeString:
			value.SetString(__FILE__LINE__ pProp[i].StringVal);
			break;
		case EvtVarTypeAnsiString:
			value.FormatString(__FILE__LINE__ _T("%hs"), pProp[i].AnsiStringVal);
			break;
		case EvtVarTypeSByte:
			value.FormatString(__FILE__LINE__ _T("%d"), (sdword)(pProp[i].SByteVal));
			break;
		case EvtVarTypeByte:
			value.FormatString(__FILE__LINE__ _T("%u"), (dword)(pProp[i].SByteVal));
			break;
		case EvtVarTypeInt16:
			value.FormatString(__FILE__LINE__ _T("%d"), (sdword)(pProp[i].Int16Val));
			break;
		case EvtVarTypeUInt16:
			value.FormatString(__FILE__LINE__ _T("%u"), (dword)(pProp[i].UInt16Val));
			break;
		case EvtVarTypeInt32:
			value.FormatString(__FILE__LINE__ _T("%d"), (sdword)(pProp[i].Int32Val));
			break;
		case EvtVarTypeUInt32:
			value.FormatString(__FILE__LINE__ _T("%u"), (dword)(pProp[i].UInt32Val));
			break;
		case EvtVarTypeInt64:
			value.FormatString(__FILE__LINE__ _T("%lld"), (sqword)(pProp[i].Int64Val));
			break;
		case EvtVarTypeUInt64:
			value.FormatString(__FILE__LINE__ _T("%llu"), (qword)(pProp[i].UInt64Val));
			break;
		case EvtVarTypeSingle:
			value.FormatString(__FILE__LINE__ _T("%g"), (double)(pProp[i].SingleVal));
			break;
		case EvtVarTypeDouble:
			value.FormatString(__FILE__LINE__ _T("%g"), (double)(pProp[i].DoubleVal));
			break;
		case EvtVarTypeBoolean:
			value.FormatString(__FILE__LINE__ _T("%s"), (pProp[i].BooleanVal)?_T("True"):_T("False"));
			break;
		case EvtVarTypeBinary:
			break;
		case EvtVarTypeGuid:
			if (NULL != pProp[i].GuidVal)
			{
				TCHAR wsGuid[50];

				StringFromGUID2(*(pProp[i].GuidVal), wsGuid, sizeof(wsGuid) / sizeof(TCHAR));
				value.SetString(__FILE__LINE__ wsGuid);
			}
			break;
		case EvtVarTypeSizeT:
			break;
		case EvtVarTypeFileTime:
			{
				ULONGLONG ullTimeStamp = 0;
				ULONGLONG ullNanoseconds = 0;
				SYSTEMTIME st;
				FILETIME ft;

				ullTimeStamp = pProp[i].FileTimeVal;
				ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
				ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

				FileTimeToSystemTime(&ft, &st);
				ullNanoseconds = (ullTimeStamp % 10000000) * 100; // Display nanoseconds instead of milliseconds for higher resolution
				value.FormatString(__FILE__LINE__ _T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
			}
			break;
		case EvtVarTypeSysTime:
			value.FormatString(__FILE__LINE__ _T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd"), pProp[i].SysTimeVal->wDay, pProp[i].SysTimeVal->wMonth, pProp[i].SysTimeVal->wYear, pProp[i].SysTimeVal->wHour, pProp[i].SysTimeVal->wMinute, pProp[i].SysTimeVal->wSecond);
			break;
		case EvtVarTypeSid:
			{
				LPTSTR pwsSid = NULL;

				if (ConvertSidToStringSid(pProp[i].SidVal, &pwsSid))
				{
					value.SetString(__FILE__LINE__ pwsSid);
					LocalFree(pwsSid);
				}
			}
			break;
		case EvtVarTypeHexInt32:
			value.FormatString(__FILE__LINE__ _T("0x%x"), (dword)(pProp[i].UInt32Val));
			break;
		case EvtVarTypeHexInt64:
			value.FormatString(__FILE__LINE__ _T("0x%llx"), (qword)(pProp[i].UInt64Val));
			break;
		default:
			break;
		}
		text.ReplaceString(index.GetString(), value.GetString());
	}
}

static CStringBuffer GetResourceString(LPCTSTR resourceName, DWORD cEventID)
{
	CStringBuffer result;
	HMODULE hResources = NULL;

	hResources = LoadLibraryEx(resourceName, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
	if (NULL == hResources)
		ThrowDefaultException(__FILE__LINE__ _T("GetResourceString"));

	HRSRC hRes = FindResource(hResources, MAKEINTRESOURCE(1), RT_MESSAGETABLE);
	HGLOBAL hGlobal = LoadResource(hResources, hRes);
	LPVOID pStr = LockResource(hGlobal);
	PMESSAGE_RESOURCE_DATA pMessageDataPtr = (PMESSAGE_RESOURCE_DATA)pStr;

	//wprintf(L"NumberOfBlocks=%d\n", pMessageDataPtr->NumberOfBlocks);
	for (DWORD jx = 0; jx < pMessageDataPtr->NumberOfBlocks; ++jx)
	{
		//wprintf(L"Resid(low)=%08x, Resid(high)=%08x\n", pMessageDataPtr->Blocks[jx].LowId, pMessageDataPtr->Blocks[jx].HighId);

		PMESSAGE_RESOURCE_ENTRY pResEntry = (PMESSAGE_RESOURCE_ENTRY)(((PBYTE)pMessageDataPtr) + pMessageDataPtr->Blocks[jx].OffsetToEntries);
		for (DWORD kx = pMessageDataPtr->Blocks[jx].LowId; kx <= pMessageDataPtr->Blocks[jx].HighId; ++kx)
		{
			if ((kx == cEventID) || (kx == (cEventID & 0xFFFF)) || ((kx & 0xFFFF) == cEventID) || ((kx & 0xFFFF) == (cEventID & 0xFFFF)))
			{
				if (pResEntry->Flags)
				{
					CStringBuffer num;

					num.FormatString(__FILE__LINE__ _T("IX=%08x "), kx);
					if (!(result.IsEmpty()))
						result.PrependString(_T("Multiple "));
					result.AppendString(num.GetString());
					result.AppendString(CastCPointer(pResEntry->Text), ((int)(pResEntry->Length) - (2 * sizeof(WORD))) / 2);
				}
				else
					result.FormatString(__FILE__LINE__ _T("%.*hs"), (DWORD)(pResEntry->Length) - (2 * sizeof(WORD)), pResEntry->Text);
				//goto _exit;
			}
			//if (pResEntry->Flags)
			//	wprintf(L"%08x %.*s", kx, ((DWORD)(pResEntry->Length) - (2 * sizeof(WORD))) / 2, pResEntry->Text);
			//else
			//	printf("%08x %.*s", kx, (DWORD)(pResEntry->Length) - (2 * sizeof(WORD)), pResEntry->Text);
			pResEntry = (PMESSAGE_RESOURCE_ENTRY)(((PBYTE)pResEntry) + pResEntry->Length);
		}
	}
	//wprintf(L"\n");
//_exit:

	if (hResources)
		FreeLibrary(hResources);

	return result;
}

static CStringBuffer GetMessageString(LPCTSTR provider, DWORD cEventID, DWORD dwPropCount, PEVT_VARIANT pProp)
{
	CStringBuffer result;
	EVT_HANDLE hMetadata = NULL;
	PEVT_VARIANT pMessageFile = NULL;
	LPTSTR pBuffer = NULL;
	LPTSTR pTemp = NULL;
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;
	DWORD status = 0;
	DWORD eventID = 0;
	WORD state = 0;

	// Get the handle to the provider's metadata that contains the message strings.
	hMetadata = EvtOpenPublisherMetadata(NULL, provider, NULL, 0 //MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), 0)
		, 0);
	if (NULL == hMetadata)
		ThrowDefaultException(__FILE__LINE__ _T("GetMessageString"), provider);

	dwBufferSize = 0;
	dwBufferUsed = 0;
	pBuffer = NULL;
	eventID = cEventID;
	state = 0;
	while (true)
	{
		if (!EvtFormatMessage(hMetadata, NULL, eventID, dwPropCount, pProp, EvtFormatMessageId, dwBufferSize, pBuffer, &dwBufferUsed))
		{
			switch (status = GetLastError())
			{
			case ERROR_INSUFFICIENT_BUFFER:
				dwBufferSize = dwBufferUsed;
				if (pBuffer)
				{
					pTemp = (LPTSTR)TFrealloc(pBuffer, dwBufferSize * sizeof(TCHAR));
					pBuffer = pTemp;
					pTemp = NULL;
				}
				else
					pBuffer = (LPTSTR)TFalloc(dwBufferSize * sizeof(TCHAR));
				break;
			case ERROR_EVT_UNRESOLVED_VALUE_INSERT:
				goto _exit;
				break;
			case ERROR_EVT_MESSAGE_NOT_FOUND:
			case ERROR_EVT_MESSAGE_ID_NOT_FOUND:
			case ERROR_EVT_MESSAGE_LOCALE_NOT_FOUND:
				switch (state)
				{
				case 0:
					eventID = eventID & 0xFFFF;
					++state;
					break;
				default:
					goto _exit;
					break;
				}
				break;
			default:
				EvtClose(hMetadata);
				if (pBuffer)
					TFfree(pBuffer);
				if (pMessageFile)
					TFfree(pMessageFile);
				ThrowDefaultException(__FILE__LINE__ _T("GetMessageString"), status);
				break;
			}
		}
		else
			break;
	}
_exit:
	if (pBuffer)
	{
		CStringBuffer num;

		num.FormatString(__FILE__LINE__ _T("XX=%08x: "), eventID);
		if (pMessageFile)
			TFfree(pMessageFile);
		result.SetString(__FILE__LINE__ pBuffer);
		result.PrependString(num);
		TFfree(pBuffer);
		result.ReplaceString(_T("\r\n"), _T("\n"));
		EvtClose(hMetadata);
		return result;
	}
	if (!EvtGetPublisherMetadataProperty(hMetadata, EvtPublisherMetadataMessageFilePath, 0, dwBufferSize, pMessageFile, &dwBufferUsed))
	{
		status = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER == status)
		{
			dwBufferSize = dwBufferUsed;
			pMessageFile = (PEVT_VARIANT)TFalloc(dwBufferSize);

			if (pMessageFile)
			{
				ZeroMemory(pMessageFile, dwBufferSize);

				status = ERROR_SUCCESS;
				if (!EvtGetPublisherMetadataProperty(hMetadata, EvtPublisherMetadataMessageFilePath, 0, dwBufferSize, pMessageFile, &dwBufferUsed))
					status = GetLastError();
			}
			else
				status = ERROR_OUTOFMEMORY;
		}
		if (ERROR_SUCCESS != status)
		{
			EvtClose(hMetadata);
			ThrowDefaultException(__FILE__LINE__ _T("GetMessageString"), status);
		}
	}
	EvtClose(hMetadata);
	if (pMessageFile == NULL)
		return result;
	if (pMessageFile->StringVal == NULL)
	{
		TFfree(pMessageFile);
		return result;
	}

	result = GetResourceString(pMessageFile->StringVal, cEventID);

	if (pMessageFile)
		TFfree(pMessageFile);

	InsertValues(result, dwPropCount, pProp);

	result.ReplaceString(_T("\r\n"), _T("\n"));
	return result;
}

//================== CEventLogEvent =========================================
static LPCTSTR gUserUseStrings[] = {
	_T(""),
	_T("User"), // SidTypeUser
	_T("Group"), // SidTypeGroup
	_T("Domain"), // SidTypeDomain
	_T("Alias"), // SidTypeAlias
	_T("WellKnownGroup"), // SidTypeWellKnownGroup
	_T("DeletedAccount"), // SidTypeDeletedAccount
	_T("Invalid"), // SidTypeInvalid
	_T("Unknown"), // SidTypeUnknown
	_T("Computer"), // SidTypeComputer
	_T("Label") // SidTypeLabel
};

CEventLogEvent::CEventLogEvent() :
_providerName(),
_providerGuid(),
_eventID(0),
_qualifiers(0),
_version(0),
_level(0),
_task(0),
_opCode(0),
_keyWords(0),
_creationTime(),
_eventRecordID(0),
_activityID(),
_relatedActivityID(),
_processID(0),
_threadID(0),
_channel(),
_computer(),
_userSID(),
_userAccount(),
_userDomain(),
_userUse(SidTypeUnknown),
_message(),
_messageID(0),
_userDataCount(0),
_userData(NULL)
{
}

CEventLogEvent::~CEventLogEvent()
{
	if (_userData)
		TFfree(_userData);
}

CStringBuffer CEventLogEvent::get_userUseString() const
{
	CStringBuffer tmp;

	if (_userUse >= SidTypeUser && _userUse <= SidTypeLabel)
		tmp.SetString(__FILE__LINE__ gUserUseStrings[_userUse]);
	return tmp;
}

BOOLEAN CEventLogEvent::Load(EVT_HANDLE hEvent, Ptr(CEventLogProviders) pProviders)
{
	DWORD status = ERROR_SUCCESS;
	EVT_HANDLE hContext = NULL;
	EVT_HANDLE hContext2 = NULL;
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;
	DWORD dwPropertyCount = 0;
	PEVT_VARIANT pRenderedValues = NULL;
	PEVT_VARIANT pRenderedValues2 = NULL;
	LPTSTR pwsMessage = NULL;
	ULONGLONG ullTimeStamp = 0;
	ULONGLONG ullNanoseconds = 0;
	SYSTEMTIME st;
	FILETIME ft;
	Ptr(CEventLogProvider) pProvider = NULL;

	hContext = EvtCreateRenderContext(0, NULL, EvtRenderContextSystem);
	if (NULL == hContext)
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvent::Load"));

	// When you render the user data or system section of the event, you must specify
	// the EvtRenderEventValues flag. The function returns an array of variant values 
	// for each element in the user data or system section of the event. For user data
	// or event data, the values are returned in the same order as the elements are 
	// defined in the event. For system data, the values are returned in the order defined
	// in the EVT_SYSTEM_PROPERTY_ID enumeration.
	if (!EvtRender(hContext, hEvent, EvtRenderEventValues, dwBufferSize, pRenderedValues, &dwBufferUsed, &dwPropertyCount))
	{
		if (ERROR_INSUFFICIENT_BUFFER == (status = GetLastError()))
		{
			dwBufferSize = dwBufferUsed;
			pRenderedValues = (PEVT_VARIANT)TFalloc(dwBufferSize);
			if (pRenderedValues)
				EvtRender(hContext, hEvent, EvtRenderEventValues, dwBufferSize, pRenderedValues, &dwBufferUsed, &dwPropertyCount);
			else
				ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvent::Load"), ERROR_OUTOFMEMORY);
		}
		if (ERROR_SUCCESS != (status = GetLastError()))
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvent::Load"));
	}

	_providerName.SetString(__FILE__LINE__ pRenderedValues[EvtSystemProviderName].StringVal);
	pProvider = pProviders->FindSorted(_providerName);

	if (!pProvider)
		_tprintf(_T("CEventLogEvent::Load: pProvider is null (name = %s).\n"), _providerName);

	if (NULL != pRenderedValues[EvtSystemProviderGuid].GuidVal)
	{
		TCHAR wsGuid[50];

		StringFromGUID2(*(pRenderedValues[EvtSystemProviderGuid].GuidVal), wsGuid, sizeof(wsGuid) / sizeof(TCHAR));
		_providerGuid.SetString(__FILE__LINE__ wsGuid);
	}
	_eventID = pRenderedValues[EvtSystemEventID].UInt16Val;
	_qualifiers = pRenderedValues[EvtSystemQualifiers].UInt16Val;
	_version = (EvtVarTypeNull == pRenderedValues[EvtSystemVersion].Type) ? 0 : pRenderedValues[EvtSystemVersion].ByteVal;

	if (EvtVarTypeNull == pRenderedValues[EvtSystemLevel].Type)
		_level = 0;
	else
	{
		_level = pRenderedValues[EvtSystemLevel].ByteVal;

		if (pProvider)
		{
			Ptr(CEventLogProviderLevel) pProvLevel = pProvider->get_level(_level);

			if (pProvLevel)
			{
				_levelName = pProvLevel->get_name();
				_levelMessage = pProvLevel->get_message();
			}
		}
	}

	if (EvtVarTypeNull == pRenderedValues[EvtSystemTask].Type)
		_task = 0;
	else
	{
		_task = pRenderedValues[EvtSystemTask].UInt16Val;

		if (pProvider)
		{
			Ptr(CEventLogProviderTask) pProvTask = pProvider->get_task(_task);

			if (pProvTask)
			{
				_taskName = pProvTask->get_name();
				_taskMessage = pProvTask->get_message();
				_taskEventGuid = pProvTask->get_eventGuid();
			}
		}
	}

	if (EvtVarTypeNull == pRenderedValues[EvtSystemOpcode].Type)
		_opCode = 0;
	else
	{
		_opCode = pRenderedValues[EvtSystemOpcode].ByteVal;

		if (pProvider)
		{
			Ptr(CEventLogProviderOpCode) pProvOpCode = pProvider->get_opCode(_opCode, _task);

			if (pProvOpCode)
			{
				_opCodeName = pProvOpCode->get_name();
				_opCodeMessage = pProvOpCode->get_message();
			}
		}
	}

	_keyWords = pRenderedValues[EvtSystemKeywords].UInt64Val;
	if (pProvider)
	{
		CDataDoubleLinkedListT<CEventLogProviderKeyWord> vKeyWordList = pProvider->get_keyWords(_keyWords);
		CDataDoubleLinkedListT<CEventLogProviderKeyWord>::Iterator it = vKeyWordList.Begin();

		while (it)
		{
			Ptr(CEventLogProviderKeyWord) p = *it;

			if (!(_keyWordsName.IsEmpty()))
				_keyWordsName.AppendString(_T(", "));
			_keyWordsName.AppendString(p->get_name());
			if (!(_keyWordsMessage.IsEmpty()))
				_keyWordsMessage.AppendString(_T(", "));
			_keyWordsMessage.AppendString(p->get_message());

			++it;
		}

	}

	ullTimeStamp = pRenderedValues[EvtSystemTimeCreated].FileTimeVal;
	ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
	ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

	FileTimeToSystemTime(&ft, &st);
	ullNanoseconds = (ullTimeStamp % 10000000) * 100; // Display nanoseconds instead of milliseconds for higher resolution
	_creationTime.SetTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	_eventRecordID = pRenderedValues[EvtSystemEventRecordId].UInt64Val;

	if (EvtVarTypeNull != pRenderedValues[EvtSystemActivityID].Type)
	{
		TCHAR wsGuid[50];

		StringFromGUID2(*(pRenderedValues[EvtSystemActivityID].GuidVal), wsGuid, sizeof(wsGuid) / sizeof(TCHAR));
		_activityID.SetString(__FILE__LINE__ wsGuid);
	}

	if (EvtVarTypeNull != pRenderedValues[EvtSystemRelatedActivityID].Type)
	{
		TCHAR wsGuid[50];

		StringFromGUID2(*(pRenderedValues[EvtSystemRelatedActivityID].GuidVal), wsGuid, sizeof(wsGuid) / sizeof(TCHAR));
		_relatedActivityID.SetString(__FILE__LINE__ wsGuid);
	}
	_processID = pRenderedValues[EvtSystemProcessID].UInt32Val;
	_threadID = pRenderedValues[EvtSystemThreadID].UInt32Val;
	_channel.SetString(__FILE__LINE__ (EvtVarTypeNull == pRenderedValues[EvtSystemChannel].Type) ? _T("") : pRenderedValues[EvtSystemChannel].StringVal);
	_computer.SetString(__FILE__LINE__ pRenderedValues[EvtSystemComputer].StringVal);

	if (EvtVarTypeNull != pRenderedValues[EvtSystemUserID].Type)
	{
		LPTSTR pwsSid = NULL;

		if (ConvertSidToStringSid(pRenderedValues[EvtSystemUserID].SidVal, &pwsSid))
		{
			_userSID.SetString(__FILE__LINE__ pwsSid);
			LocalFree(pwsSid);
		}

		DWORD cchName = 0;
		DWORD cchReferencedDomainName = 0;

		CStringBuffer _userSID;
		CStringBuffer _userAccount;
		CStringBuffer _userDomain;
		SID_NAME_USE _userUse;

		LookupAccountSid(NULL, (PSID)(pRenderedValues[EvtSystemUserID].SidVal), NULL, &cchName, NULL, &cchReferencedDomainName, &_userUse);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			LPWSTR pName = (LPWSTR)TFalloc(cchName * sizeof(wchar_t));
			LPWSTR pReferencedDomainName = (LPWSTR)TFalloc(cchReferencedDomainName * sizeof(wchar_t));

			if (LookupAccountSid(NULL, (PSID)(pRenderedValues[EvtSystemUserID].SidVal), pName, &cchName, pReferencedDomainName, &cchReferencedDomainName, &_userUse))
			{
				_userAccount.SetString(__FILE__LINE__ pName);
				_userDomain.SetString(__FILE__LINE__ pReferencedDomainName);
			}
			else
				ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvent::Load"));
			TFfree(pName);
			TFfree(pReferencedDomainName);
		}
		else
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvent::Load"));
	}

	EvtClose(hContext);
	dwBufferSize = 0;
	dwBufferUsed = 0;
	dwPropertyCount = 0;

	hContext = EvtCreateRenderContext(0, NULL, EvtRenderContextUser);
	if (NULL == hContext)
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvent::Load"));

	// When you render the user data or system section of the event, you must specify
	// the EvtRenderEventValues flag. The function returns an array of variant values 
	// for each element in the user data or system section of the event. For user data
	// or event data, the values are returned in the same order as the elements are 
	// defined in the event. For system data, the values are returned in the order defined
	// in the EVT_SYSTEM_PROPERTY_ID enumeration.
	if (!EvtRender(hContext, hEvent, EvtRenderEventValues, dwBufferSize, pRenderedValues2, &dwBufferUsed, &dwPropertyCount))
	{
		if (ERROR_INSUFFICIENT_BUFFER == (status = GetLastError()))
		{
			dwBufferSize = dwBufferUsed;
			pRenderedValues2 = (PEVT_VARIANT)TFalloc(dwBufferSize);
			if (pRenderedValues2)
				EvtRender(hContext, hEvent, EvtRenderEventValues, dwBufferSize, pRenderedValues2, &dwBufferUsed, &dwPropertyCount);
			else
				ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvent::Load"), ERROR_OUTOFMEMORY);
		}
		if (ERROR_SUCCESS != (status = GetLastError()))
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvent::Load"));
	}

	try
	{
		_userDataCount = dwPropertyCount;
		_userData = pRenderedValues2;

		_messageID = 0;
		if (pProvider)
		{
			Ptr(CEventLogProviderEvent) pEventInfo = pProvider->get_event(MAKELONG(_eventID, _qualifiers));

			if (pEventInfo)
			{
				_messageID = pEventInfo->get_messageID();
				_template = pEventInfo->get_template();
			}
		}
		if ((_messageID == 0) || (_messageID == 0xFFFFFFFF))
			_messageID = MAKELONG(_eventID, _qualifiers);
		_message = GetMessageString(pRenderedValues[EvtSystemProviderName].StringVal, _messageID, dwPropertyCount, pRenderedValues2);
	}
	catch (CBaseException*)
	{
		_message.SetString(__FILE__LINE__ _T(""));
	}

	if (pRenderedValues)
		TFfree(pRenderedValues);
	if (hContext)
		EvtClose(hContext);

	return TRUE;
}

//================== CEventLogEvents =========================================
static void __stdcall CEventLogEventsDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogEvent* pInfo = CastAnyPtr(CEventLogEvent, CastMutable(Pointer, data));

	pInfo->release();
}

static sword __stdcall CEventLogEventsSearchAndSortFunc(ConstPointer pa, ConstPointer pb)
{
	CEventLogEvent* ppa = CastAnyPtr(CEventLogEvent, CastMutable(Pointer, pa));
	CEventLogEvent* ppb = CastAnyPtr(CEventLogEvent, CastMutable(Pointer, pb));
	sqword vdiff;

	vdiff = ppa->get_eventRecordID() - ppb->get_eventRecordID();
	if (vdiff < 0)
		return -1;
	if (vdiff > 0)
		return 1;
	return 0;
}


CEventLogEvents::CEventLogEvents(DECL_FILE_LINE0):
CDataVectorT<CEventLogEvent>(ARGS_FILE_LINE 256, 1024, CEventLogEventsDeleteFunc, NULL, CEventLogEventsSearchAndSortFunc)
{
}

CEventLogEvents::~CEventLogEvents()
{}

BOOLEAN CEventLogEvents::Load(ConstRef(CStringBuffer) channelPath, Ptr(CEventLogProviders) pProviders)
{
	if (channelPath.IsEmpty())
		return FALSE;

	EVT_HANDLE hEvents = NULL;
	EVT_HANDLE hEvent = NULL;
	DWORD cReturned = 0;
	DWORD status = ERROR_SUCCESS;

	if (NULL == (hEvents = EvtQuery(NULL, channelPath.GetString(), _T("*"), EvtQueryChannelPath | EvtQueryReverseDirection)))
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvents::_load"), channelPath.GetString());

	while (true)
	{
		if (!EvtNext(hEvents, 1, &hEvent, INFINITE, 0, &cReturned))
		{
			status = GetLastError();
			if (ERROR_NO_MORE_ITEMS == status)
				break;
			ThrowDefaultException(__FILE__LINE__ _T("CEventLogEvents::_load"));
		}

		Ptr(CEventLogEvent) eventInfo = OK_NEW_OPERATOR CEventLogEvent;

		try
		{
			eventInfo->Load(hEvent, pProviders);
			InsertSorted(eventInfo);
		}
		catch (CBaseException* ex)
		{
			if (eventInfo)
				eventInfo->release();
			eventInfo = NULL;
			_tprintf(_T("%s\n"), ex->GetExceptionMessage().GetString());
		}
		EvtClose(hEvent);
		hEvent = NULL;
	}
	return TRUE;
}

BOOLEAN CEventLogEvents::ForEach(TForEachFunc func, Pointer context, bool bReverse) const
{
	Iterator it;

	if (bReverse)
	{
		it = Last();
		while (it)
		{
			if (!func(*it, context))
				return FALSE;
			--it;
		}
	}
	else
	{
		it = Begin();
		while (it)
		{
			if (!func(*it, context))
				return FALSE;
			++it;
		}
	}
	return TRUE;
}

Ptr(CEventLogEvent) CEventLogEvents::FindSorted(DWORD64 eventRecordID) const
{
	CEventLogEvent toFind;

	toFind.set_eventRecordID(eventRecordID);

	Iterator it = super::FindSorted(&toFind);

	if (it)
		return *it;
	return NULL;
}
