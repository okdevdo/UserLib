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
#include "EventLogRecord.h"
#include "Registry.h"
#include "File.h"
#include "UTLPTR.H"
#include <sddl.h>

IMPL_WINEXCEPTION(CEventLogRecordException, CWinException)

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CEventLogRecordException(ARGS_FILE_LINE _T("in %s CEventLogRecordException"),
		func, CWinException::WinExtError);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, CConstPointer sArg1)
{
	throw OK_NEW_OPERATOR CEventLogRecordException(ARGS_FILE_LINE _T("in %s CEventLogRecordException (%s)"),
		func, sArg1, CWinException::WinExtError);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, dword errCode)
{
	throw OK_NEW_OPERATOR CEventLogRecordException(ARGS_FILE_LINE _T("in %s CEventLogRecordException"),
		func, CWinException::WinExtError, errCode);
}

static DWORD _MessageCounter = 0;
static DWORD _MessageNotFoundCounter = 0;

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

static DWORD GetEventTypeName(DWORD EventType)
{
	DWORD index = 0;

	switch (EventType)
	{
	case EVENTLOG_ERROR_TYPE:
		index = 0;
		break;
	case EVENTLOG_WARNING_TYPE:
		index = 1;
		break;
	case EVENTLOG_INFORMATION_TYPE:
		index = 2;
		break;
	case EVENTLOG_AUDIT_SUCCESS:
		index = 3;
		break;
	case EVENTLOG_AUDIT_FAILURE:
		index = 4;
		break;
	}
	return index;
}

static void GetTimestamp(const DWORD Time, Ref(CDateTime) dateTime)
{
	ULONGLONG ullTimeStamp = 0;
	ULONGLONG SecsTo1970 = 116444736000000000;
	SYSTEMTIME st;
	FILETIME ft, ftLocal;

	ullTimeStamp = Int32x32To64(Time, 10000000) + SecsTo1970;
	ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
	ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

	FileTimeToLocalFileTime(&ft, &ftLocal);
	FileTimeToSystemTime(&ftLocal, &st);
	dateTime.SetTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

CStringBuffer GetCategoryString(LPCTSTR pProviderRoot, LPCTSTR pProvider, DWORD Id)
{
	CStringBuffer vMessage;
	CRegistry reg;
	CStringBuffer vRegProvValue(__FILE__LINE__ _T("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\"));
	CStringBuffer vFileName;
	DWORD sz = 0;

	vRegProvValue.AppendString(pProviderRoot);
	vRegProvValue.AppendString(_T("\\"));
	vRegProvValue.AppendString(pProvider);

	reg.OpenLocalMachine();
	if (!(reg.OpenDataKey(vRegProvValue)))
		ThrowDefaultException(__FILE__LINE__ _T("GetCategoryString"));
	if (reg.GetValueSize(_T("CategoryMessageFile"), &sz))
	{
		vFileName.SetSize(__FILE__LINE__ sz + 1);
		reg.GetValue(_T("CategoryMessageFile"), CastMutable(CPointer, vFileName.GetString()));
	}

	if (!(vFileName.IsEmpty()))
	{
		HMODULE hResources = LoadLibraryEx(vFileName, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
		if (!hResources)
			ThrowDefaultException(__FILE__LINE__ _T("GetCategoryString"));

		//	DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER;
		DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER;
		LPTSTR buffer;

		if (!FormatMessage(dwFormatFlags,
			hResources,
			Id,
			0,
			CastAny(LPTSTR, &buffer),
			0,
			NULL))
		{
			FreeLibrary(hResources);		
			ThrowDefaultException(__FILE__LINE__ _T("GetCategoryString"));		
		}
		vMessage.SetString(__FILE__LINE__ buffer);
		LocalFree(buffer);
		FreeLibrary(hResources);			
	}
	vMessage.ReplaceString(_T("\r\n"), _T("\n"));
	return vMessage;
}

CStringBuffer GetMessageString(LPCTSTR pProviderRoot, LPCTSTR pProvider, WORD EventType, DWORD MessageId, DWORD argc, LPCTSTR argv)
{
	CStringBuffer vMessage;
	CRegistry reg;
	CStringBuffer vRegProvKey(__FILE__LINE__ _T("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\"));
	CStringBuffer vRegProvValue;
	DWORD sz = 0;

	++_MessageCounter;

	vRegProvKey.AppendString(pProviderRoot);
	vRegProvKey.AppendString(_T("\\"));
	vRegProvKey.AppendString(pProvider);

	reg.OpenLocalMachine();
	if (!(reg.OpenDataKey(vRegProvKey)))
		ThrowDefaultException(__FILE__LINE__ _T("GetMessageString"));
	if (reg.GetValueSize(_T("EventMessageFile"), &sz))
	{
		vRegProvValue.SetSize(__FILE__LINE__ sz + 1);
		reg.GetValue(_T("EventMessageFile"), CastMutable(CPointer, vRegProvValue.GetString()));
	}
	else if (reg.GetValueSize(_T("ProviderGuid"), &sz))
	{
		vRegProvValue.SetSize(__FILE__LINE__ sz + 1);
		reg.GetValue(_T("ProviderGuid"), CastMutable(CPointer, vRegProvValue.GetString()));

		vRegProvKey.SetString(__FILE__LINE__ _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WINEVT\\Publishers\\"));
		vRegProvKey.AppendString(vRegProvValue);

		if (!(reg.OpenDataKey(vRegProvKey)))
			ThrowDefaultException(__FILE__LINE__ _T("GetMessageString"));
		if (reg.GetValueSize(_T("MessageFileName"), &sz))
		{
			vRegProvValue.SetSize(__FILE__LINE__ sz + 1);
			reg.GetValue(_T("MessageFileName"), CastMutable(CPointer, vRegProvValue.GetString()));
		}
	}

	if (vRegProvValue.IsEmpty())
	{
		_MessageNotFoundCounter++;
		return vMessage;
	}

	CPointer fArray[64];
	dword fcnt;
	//	DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER;
	DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER;
	Ptr(DWORD_PTR) pArgs = NULL;
	LPTSTR pString = CastMutable(LPTSTR, argv);

	vRegProvValue.Split(_T(";"), fArray, 64, &fcnt);

	if (fcnt == 0)
	{
		_MessageNotFoundCounter++;
		return vMessage;
	}

	// The insertion strings appended to the end of the event record
	// are an array of strings; however, FormatMessage requires
	// an array of addresses. Create an array of DWORD_PTRs based on
	// the count of strings. Assign the address of each string
	// to an element in the array (maintaining the same order).
	if (argc > 0)
	{
		pArgs = CastAnyPtr(DWORD_PTR, TFalloc(sizeof(DWORD_PTR) * argc));
		if (pArgs)
		{
			dwFormatFlags |= FORMAT_MESSAGE_ARGUMENT_ARRAY;

			for (DWORD i = 0; i < argc; i++)
			{
				pArgs[i] = (DWORD_PTR)pString;
				pString += s_strlen(pString, UINT_MAX) + 1;
			}
		}
		else
			dwFormatFlags |= FORMAT_MESSAGE_IGNORE_INSERTS;
	}

	HMODULE hResources = NULL;
	LPTSTR buffer = NULL;

	for (DWORD i = 0; i < fcnt; i++)
	{
		if (hResources)
			FreeLibrary(hResources);
		hResources = LoadLibraryEx(fArray[i], NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
		if (!hResources)
			ThrowDefaultException(__FILE__LINE__ _T("GetMessageString"));

		if (!FormatMessage(dwFormatFlags,
			hResources,
			MessageId,
			0,
			CastAny(LPTSTR, &buffer),
			0,
			CastAnyPtr(va_list, pArgs)))
		{
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
					//if (pResEntry->Flags)
					//	wprintf(L"%08x %.*s", kx, ((DWORD)(pResEntry->Length) - (2 * sizeof(WORD))) / 2, pResEntry->Text);
					//else
					//	printf("%08x %.*s", kx, (DWORD)(pResEntry->Length) - (2 * sizeof(WORD)), pResEntry->Text);
					pResEntry = (PMESSAGE_RESOURCE_ENTRY)(((PBYTE)pResEntry) + pResEntry->Length);
				}
			}
		}
		else
		{
			vMessage.SetString(__FILE__LINE__ buffer);
			LocalFree(buffer);
			break;
		}
	}
	if (hResources)
		FreeLibrary(hResources);
	if (pArgs)
		TFfree(pArgs);
	if (vMessage.IsEmpty())
		_MessageNotFoundCounter++;
	vMessage.ReplaceString(_T("\r\n"), _T("\n"));
	return vMessage;
}

// If the message string contains parameter insertion strings (for example, %%4096),
// you must perform the parameter substitution yourself. To get the parameter message 
// string, call FormatMessage with the message identifier found in the parameter insertion 
// string (for example, 4096 is the message identifier if the parameter insertion string
// is %%4096). You then substitute the parameter insertion string in the message 
// string with the actual parameter message string. 
DWORD ApplyParameterStringsToMessage(LPCTSTR pProviderRoot, LPCTSTR pProvider, Ref(CStringBuffer) pMessage)
{
	DWORD status = ERROR_SUCCESS;
	DWORD dwParameterCount = 0;  // Number of insertion strings found in pMessage
	dword cbBuffer = 0;         // Size of the buffer in bytes
	size_t cchBuffer = 0;        // Size of the buffer in characters
	dword cchParameters = 0;    // Number of characters in all the parameter strings
	size_t cch = 0;
	WLong num = 0;
	DWORD i = 0;
	LPTSTR* pStartingAddresses = NULL;  // Array of pointers to the beginning of each parameter string in pMessage
	LPTSTR* pEndingAddresses = NULL;    // Array of pointers to the end of each parameter string in pMessage
	DWORD* pParameterIDs = NULL;        // Array of parameter identifiers found in pMessage
	LPTSTR* pParameters = NULL;         // Array of the actual parameter strings
	LPTSTR pTempMessage = NULL;
	LPTSTR pTempFinalMessage = NULL;

	DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER;
	HMODULE hResources = NULL;
	CRegistry reg;
	CStringBuffer vRegProvKey(__FILE__LINE__ _T("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\"));
	CStringBuffer vRegProvValue;

	// Determine the number of parameter insertion strings in pMessage.
	CStringConstIterator sIt = pMessage;

	sIt.Find(_T("%"));
	while (!(sIt.IsEnd()))
	{
		++dwParameterCount;
		++sIt;
		if (sIt[0] == _T('%'))
			++sIt;
		sIt.Find(_T("%"));
	}

	// If there are no parameter insertion strings in pMessage, return.
	if (0 == dwParameterCount)
		return status;

	vRegProvKey.AppendString(pProviderRoot);
	vRegProvKey.AppendString(_T("\\"));
	vRegProvKey.AppendString(pProvider);

	reg.OpenLocalMachine();
	if (reg.OpenDataKey(vRegProvKey))
	{
		DWORD sz = 0;

		if (reg.GetValueSize(_T("ParameterMessageFile"), &sz))
		{
			vRegProvValue.SetSize(__FILE__LINE__ sz + 1);
			reg.GetValue(_T("ParameterMessageFile"), CastMutable(CPointer, vRegProvValue.GetString()));
		}
		else if (reg.GetValueSize(_T("ProviderGuid"), &sz))
		{
			vRegProvValue.SetSize(__FILE__LINE__ sz + 1);
			reg.GetValue(_T("ProviderGuid"), CastMutable(CPointer, vRegProvValue.GetString()));

			vRegProvKey.SetString(__FILE__LINE__ _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WINEVT\\Publishers\\"));
			vRegProvKey.AppendString(vRegProvValue);

			if (reg.OpenDataKey(vRegProvKey))
			{
				if (reg.GetValueSize(_T("ParameterFileName"), &sz))
				{
					vRegProvValue.SetSize(__FILE__LINE__ sz + 1);
					reg.GetValue(_T("ParameterFileName"), CastMutable(CPointer, vRegProvValue.GetString()));
				}
			}
		}
	}

	if (vRegProvValue.IsEmpty())
	{
		status = GetLastError();
		return status;
	}

	hResources = LoadLibraryEx(vRegProvValue.GetString(), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
	if (NULL == hResources)
	{
		status = GetLastError();
		return status;
	}

	// Allocate an array of pointers that will contain the beginning address 
	// of each parameter insertion string.
	cbBuffer = sizeof(LPTSTR) * dwParameterCount;
	pStartingAddresses = (LPTSTR*)TFalloc(cbBuffer);
	if (NULL == pStartingAddresses)
	{
		status = ERROR_OUTOFMEMORY;
		goto cleanup;
	}

	RtlZeroMemory(pStartingAddresses, cbBuffer);

	// Allocate an array of pointers that will contain the ending address (one
	// character past the of the identifier) of the each parameter insertion string.
	pEndingAddresses = (LPTSTR*)TFalloc(cbBuffer);
	if (NULL == pEndingAddresses)
	{
		status = ERROR_OUTOFMEMORY;
		goto cleanup;
	}

	RtlZeroMemory(pEndingAddresses, cbBuffer);

	// Allocate an array of pointers that will contain pointers to the actual
	// parameter strings.
	pParameters = (LPTSTR*)TFalloc(cbBuffer);
	if (NULL == pParameters)
	{
		status = ERROR_OUTOFMEMORY;
		goto cleanup;
	}

	RtlZeroMemory(pParameters, cbBuffer);

	// Allocate an array of DWORDs that will contain the message identifier
	// for each parameter.
	pParameterIDs = (DWORD*)TFalloc(cbBuffer);
	if (NULL == pParameterIDs)
	{
		status = ERROR_OUTOFMEMORY;
		goto cleanup;
	}

	RtlZeroMemory(pParameterIDs, cbBuffer);

	// Find each parameter in pMessage and get the pointer to the
	// beginning of the insertion string, the end of the insertion string,
	// and the message identifier of the parameter.
	sIt = pMessage;

	sIt.Find(_T("%"));
	while (!(sIt.IsEnd()))
	{
		if (s_isdigit(sIt[1]) || ((sIt[1] == _T('%')) && s_isdigit(sIt[2])))
		{
			pStartingAddresses[i] = CastMutable(CPointer, sIt.GetCurrent());
			++sIt;
			if (sIt[0] == _T('%'))
				++sIt;
			sIt.EatDecimal(num); 
			pParameterIDs[i] = num;
			pEndingAddresses[i] = CastMutable(CPointer, sIt.GetCurrent());
			++i;
		}
		else
			goto cleanup;
		sIt.Find(_T("%"));
	}

	// For each parameter, use the message identifier to get the
	// actual parameter string.
	for (DWORD i = 0; i < dwParameterCount; i++)
	{
		if (!FormatMessage(dwFormatFlags,
			hResources,
			pParameterIDs[i],
			0,
			(LPTSTR)(&(pParameters[i])),
			0,
			NULL))
		{
			status = GetLastError();
			goto cleanup;
		}
		if (NULL == pParameters[i])
		{
			status = ERROR_INVALID_PARAMETER;
			goto cleanup;
		}

		cchParameters += s_strlen(pParameters[i], UINT_MAX);
	}

	if (cchParameters == 0)
	{
		status = ERROR_INVALID_PARAMETER;
		goto cleanup;
	}

	// Allocate enough memory for pFinalMessage based on the length of pMessage
	// and the length of each parameter string. The pFinalMessage buffer will contain 
	// the completed parameter substitution.
	pTempMessage = CastMutable(CPointer, pMessage.GetString());
	cchBuffer = pMessage.GetLength() + cchParameters + 1;
	pMessage.SetSize(__FILE__LINE__ Cast(dword,cchBuffer));
	cchBuffer = pMessage.GetLength();
	pTempFinalMessage = CastMutable(CPointer, pMessage.GetString());

	// Build the final message string.
	for (DWORD i = dwParameterCount; i > 0; i--)
	{
		LPTSTR start = pStartingAddresses[i-1] + (pTempFinalMessage - pTempMessage);
		LPTSTR ende =  pEndingAddresses[i-1] + (pTempFinalMessage - pTempMessage);

		cch = s_strlen(pParameters[i-1], UINT_MAX);
		s_memmove(start + cch, ende, (cchBuffer - (ende - pTempFinalMessage)) * szchar);
		s_memmove(start, pParameters[i-1], cch * szchar);
		cchBuffer -= ende - start;
		cchBuffer += cch;
	}

cleanup:

	if (hResources)
		FreeLibrary(hResources);

	if (pStartingAddresses)
		TFfree(pStartingAddresses);

	if (pEndingAddresses)
		TFfree(pEndingAddresses);

	if (pParameterIDs)
		TFfree(pParameterIDs);

	if (pParameters)
	{
		for (DWORD i = 0; i < dwParameterCount; i++)
		{
			if (pParameters[i])
				LocalFree(pParameters[i]);
		}
		TFfree(pParameters);
	}
	return status;
}

//********************* CEventLogRecord ***************************************************
CEventLogRecord::CEventLogRecord()
{
}

CEventLogRecord::~CEventLogRecord()
{
}

CStringBuffer CEventLogRecord::get_userUseAsStr() const
{
	CStringBuffer tmp;

	if (_userUse >= SidTypeUser && _userUse <= SidTypeLabel)
		tmp.SetString(__FILE__LINE__ gUserUseStrings[_userUse]);
	return tmp;
}

void CEventLogRecord::Load(PEVENTLOGRECORD rec, LPCTSTR providerRoot)
{
	static LPCTSTR pEventTypeNames[] = { _T("Error"), _T("Warning"), _T("Informational"), _T("Audit Success"), _T("Audit Failure") };

	_provider.SetString(__FILE__LINE__ CastCPointer(rec + 1));
	_recordno = rec->RecordNumber;
	_eventID = rec->EventID;
	_eventType = rec->EventType;
	_eventTypeAsStr.SetString(__FILE__LINE__ pEventTypeNames[GetEventTypeName(_eventType)]);
	GetTimestamp(rec->TimeGenerated, _timeGenerated);
	GetTimestamp(rec->TimeWritten, _timeWritten);

	if (rec->UserSidLength > 0)
	{
		LPTSTR pSidName = NULL;

		if (ConvertSidToStringSid(CastAnyPtr(SID, l_ptradd(rec, rec->UserSidOffset)), &pSidName))
		{
			_sidAsStr.SetString(__FILE__LINE__ pSidName);
			LocalFree(pSidName);
		}

		DWORD cchName = 0;
		DWORD cchReferencedDomainName = 0;

		LookupAccountSid(NULL, CastAnyPtr(SID, l_ptradd(rec, rec->UserSidOffset)), NULL, &cchName, NULL, &cchReferencedDomainName, &_userUse);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			_userAccount.SetSize(__FILE__LINE__ cchName);
			_userDomain.SetSize(__FILE__LINE__ cchReferencedDomainName);

			LookupAccountSid(NULL,
				CastAnyPtr(SID, l_ptradd(rec, rec->UserSidOffset)),
				CastMutable(CPointer, _userAccount.GetString()), &cchName,
				CastMutable(CPointer, _userDomain.GetString()), &cchReferencedDomainName, &_userUse);
		}
	}
	_category = rec->EventCategory;
	if (_category > 0)
		_categoryAsStr = GetCategoryString(providerRoot, _provider, _category);

	_message = GetMessageString(providerRoot, _provider, _eventType, _eventID, rec->NumStrings, CastAny(CPointer, l_ptradd(rec, rec->StringOffset)));
	if (_message.IsEmpty())
	{
		CPointer pString = CastAny(CPointer, l_ptradd(rec, rec->StringOffset));

		for (DWORD i = 0; i < rec->NumStrings; i++)
		{
			_message.AppendString(pString);
			_message.AppendString(_T("\n"));
			pString += s_strlen(pString, UINT_MAX) + 1;
		}
	}
	else
		ApplyParameterStringsToMessage(providerRoot, _provider, _message);

	if (rec->DataLength > 0)
	{
		CStringBuffer num;
		PBYTE pData = CastAnyPtr(BYTE, l_ptradd(rec, rec->DataOffset));
		PBYTE pData1 = NULL;

		for (DWORD i = 0; i < (((rec->DataLength) / 8) * 8); i += 8)
		{
			DWORD j;

			pData1 = pData;
			for (j = 0; j < 8; ++j)
			{
				num.FormatString(__FILE__LINE__ _T("%02lx "), (DWORD)(*pData++));
				_data.AppendString(num);
			}
			_data.AppendString(_T("  "));
			for (j = 0; j < 8; ++j)
			{
				if (*pData1 < _T(' '))
				{
					_data.AppendString(_T(". "));
					++pData1;
				}
				else
				{
					num.FormatString(__FILE__LINE__ _T("%c "), (DWORD)(*pData1++));
					_data.AppendString(num);
				}
			}
			_data.AppendString(_T("\n"));
		}
		pData1 = pData;
		for (DWORD j = 0; j < ((rec->DataLength) % 8); ++j)
		{
			num.FormatString(__FILE__LINE__ _T("%02lx "), (DWORD)(*pData++));
			_data.AppendString(num);
		}
		for (DWORD j = ((rec->DataLength) % 8); j < 8; ++j)
			_data.AppendString(_T("   "));
		_data.AppendString(_T("   "));
		for (DWORD j = 0; j < ((rec->DataLength) % 8); ++j)
		{
			if (*pData1 < _T(' '))
			{
				_data.AppendString(_T(". "));
				++pData1;
			}
			else
			{
				num.FormatString(__FILE__LINE__ _T("%c "), (DWORD)(*pData1++));
				_data.AppendString(num);
			}
		}
		_data.AppendString(_T("\n"));
	}

}

void CEventLogRecord::PrintTabDelimitedHeader(Ptr(CFile) outputFile)
{
	CStringBuffer sBuf;

	sBuf.AppendString(_T("Provider\t"));
	sBuf.AppendString(_T("RecordNo\t"));
	sBuf.AppendString(_T("EventID\t"));
	sBuf.AppendString(_T("EventType\t"));
	sBuf.AppendString(_T("Category\t"));
	sBuf.AppendString(_T("TimeGenerated\t"));
	sBuf.AppendString(_T("TimeWritten\t"));
	sBuf.AppendString(_T("SID\t"));
	sBuf.AppendString(_T("User Account\t"));
	sBuf.AppendString(_T("User Domain\t"));
	sBuf.AppendString(_T("User Use\t"));
	sBuf.AppendString(_T("Message\n"));

	outputFile->Write(sBuf);
}

void CEventLogRecord::PrintXMLHeader(CConstPointer rootElement, Ptr(CFile) outputFile)
{
	CStringBuffer sBuf;

	switch (outputFile->GetEncoding())
	{
	case CFile::ISO_8859_1_Encoding:
		sBuf.SetString(__FILE__LINE__ _T("ISO-8859-1"));
		break;
	case CFile::US_ASCII_Encoding:
		sBuf.SetString(__FILE__LINE__ _T("US-ASCII"));
		break;
	case CFile::UTF_8_Encoding:
		sBuf.SetString(__FILE__LINE__ _T("UTF-8"));
		break;
	case CFile::UTF_16_Encoding:
		sBuf.SetString(__FILE__LINE__ _T("UTF-16"));
		break;
	case CFile::UTF_16LE_Encoding:
		sBuf.SetString(__FILE__LINE__ _T("UTF-16LE"));
		break;
	case CFile::UTF_16BE_Encoding:
		sBuf.SetString(__FILE__LINE__ _T("UTF-16BE"));
		break;
	}
	sBuf.PrependString(_T("<?xml version=\"1.0\" encoding=\""));
	sBuf.AppendString(_T("\" standalone=\"yes\"?>\n<"));
	sBuf.AppendString(rootElement);
	sBuf.AppendString(_T(">\n"));

	outputFile->Write(sBuf);
}

void CEventLogRecord::PrintXMLFooter(CConstPointer rootElement, Ptr(CFile) outputFile)
{
	CStringBuffer sBuf;

	sBuf.AppendString(_T("</"));
	sBuf.AppendString(rootElement);
	sBuf.AppendString(_T(">\n"));

	outputFile->Write(sBuf);
}

void CEventLogRecord::Print(PrintOutput outputType, Ptr(CFile) outputFile)
{
	CStringBuffer sBuf;

	switch (outputType)
	{
	case ReadableTextOutPut:
		sBuf.FormatString(__FILE__LINE__ _T("***************** %s *******************\n"), _provider.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("RecordNo: %ld\n"), _recordno);
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("EventID: %ld (0x%08lx)\n"), _eventID, _eventID);
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("EventType: %s\n"), _eventTypeAsStr.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("Category: (%hd) %s\n"), _category, _categoryAsStr.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("TimeGenerated: %02hd.%02hd.%04hd %02hd:%02hd:%02hd\n"),
			_timeGenerated.GetDays(), _timeGenerated.GetMonths(), _timeGenerated.GetYears(), _timeGenerated.GetHours(), _timeGenerated.GetMinutes(), _timeGenerated.GetSeconds());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("TimeWritten: %02hd.%02hd.%04hd %02hd:%02hd:%02hd\n"),
			_timeWritten.GetDays(), _timeWritten.GetMonths(), _timeWritten.GetYears(), _timeWritten.GetHours(), _timeWritten.GetMinutes(), _timeWritten.GetSeconds());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("SID: %s\n"), _sidAsStr.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("User Account: %s\n"), _userAccount.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("User Domain: %s\n"), _userDomain.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("User Use: %s\n"), get_userUseAsStr().GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("Message: %s"), _message.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("Data:\n%s\n"), _data.GetString());
		outputFile->Write(sBuf);
		break;
	case TabdelimitedOutPut:
		sBuf = _provider;
		sBuf.AppendString(_T("\t"));
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("%ld\t"), _recordno);
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("%ld\t"), _eventID);
		outputFile->Write(sBuf);
		sBuf = _eventTypeAsStr;
		sBuf.AppendString(_T("\t"));
		outputFile->Write(sBuf);
		sBuf = _categoryAsStr;
		sBuf.AppendString(_T("\t"));
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd\t"),
			_timeGenerated.GetDays(), _timeGenerated.GetMonths(), _timeGenerated.GetYears(), _timeGenerated.GetHours(), _timeGenerated.GetMinutes(), _timeGenerated.GetSeconds());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd\t"),
			_timeWritten.GetDays(), _timeWritten.GetMonths(), _timeWritten.GetYears(), _timeWritten.GetHours(), _timeWritten.GetMinutes(), _timeWritten.GetSeconds());
		outputFile->Write(sBuf);
		sBuf = _sidAsStr;
		sBuf.AppendString(_T("\t"));
		outputFile->Write(sBuf);
		sBuf = _userAccount;
		sBuf.AppendString(_T("\t"));
		outputFile->Write(sBuf);
		sBuf = _userDomain;
		sBuf.AppendString(_T("\t"));
		outputFile->Write(sBuf);
		sBuf = get_userUseAsStr();
		sBuf.AppendString(_T("\t"));
		outputFile->Write(sBuf);
		sBuf = _message;
		sBuf.PrependString(_T("\""));
		sBuf.AppendString(_T("\"\n"));
		outputFile->Write(sBuf);
		break;
	case XMLOutput:
		sBuf.FormatString(__FILE__LINE__ _T("<EventRecord RecordNo=\"%ld\" EventID=\"0x%08lx\">\n"), _recordno, _eventID);
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<Provider Name=\"%s\"/>\n"), _provider.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<EventType Text=\"%s\"/>\n"), _eventTypeAsStr.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<Category Num=\"%hd\" Name=\"%s\"/>\n"), _category, _categoryAsStr.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<TimeGenerated Time=\"%02hd.%02hd.%04hd %02hd:%02hd:%02hd\"/>\n"),
			_timeGenerated.GetDays(), _timeGenerated.GetMonths(), _timeGenerated.GetYears(), _timeGenerated.GetHours(), _timeGenerated.GetMinutes(), _timeGenerated.GetSeconds());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<TimeWritten Time=\"%02hd.%02hd.%04hd %02hd:%02hd:%02hd\"/>\n"),
			_timeWritten.GetDays(), _timeWritten.GetMonths(), _timeWritten.GetYears(), _timeWritten.GetHours(), _timeWritten.GetMinutes(), _timeWritten.GetSeconds());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<User>\n<SID Text=\"%s\"/>\n"), _sidAsStr.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<Account Name=\"%s\"/>\n"), _userAccount.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<Domain Name=\"%s\"/>\n"), _userDomain.GetString());
		outputFile->Write(sBuf);
		sBuf.FormatString(__FILE__LINE__ _T("<Use Text=\"%s\"/>\n</User>\n"), get_userUseAsStr().GetString());
		outputFile->Write(sBuf);
		sBuf = _message;
		sBuf.ReplaceString(_T("<"), _T("&lt;"));
		sBuf.ReplaceString(_T(">"), _T("&gt;"));
		sBuf.PrependString(_T("<Message>"));
		sBuf.AppendString(_T("</Message>\n"));
		outputFile->Write(sBuf);
		sBuf = _data;
		sBuf.ReplaceString(_T("<"), _T("&lt;"));
		sBuf.ReplaceString(_T(">"), _T("&gt;"));
		sBuf.PrependString(_T("<Data>"));
		sBuf.AppendString(_T("</Data>\n"));
		outputFile->Write(sBuf);
		break;
	}
}

//********************* CEventLogRecords ***************************************************
static void __stdcall CEventLogRecordsDeleteFunc(ConstPointer data, Pointer context)
{
	CEventLogRecord* pInfo = CastAnyPtr(CEventLogRecord, CastMutable(Pointer, data));

	pInfo->release();
}

static sword __stdcall CEventLogRecordsSearchAndSortFunc(ConstPointer pa, ConstPointer pb)
{
	CEventLogRecord* ppa = CastAnyPtr(CEventLogRecord, CastMutable(Pointer, pa));
	CEventLogRecord* ppb = CastAnyPtr(CEventLogRecord, CastMutable(Pointer, pb));
	sqword vdiff;

	vdiff = ppa->get_recordno() - ppb->get_recordno();
	if (vdiff < 0)
		return 1;
	if (vdiff > 0)
		return -1;
	return 0;
}

CEventLogRecords::CEventLogRecords(DECL_FILE_LINE0):
	super(ARGS_FILE_LINE 16, 256)
{
}

CEventLogRecords::~CEventLogRecords()
{
}

void CEventLogRecords::Load()
{
	HANDLE hEventLog = NULL;
	DWORD status = ERROR_SUCCESS;
	DWORD dwBytesToRead = 0;
	DWORD dwBytesRead = 0;
	DWORD dwMinimumBytesToRead = 0;
	PBYTE pBuffer = NULL;
	PBYTE pTemp = NULL;
	HANDLE hToken = NULL;
	CRegistry vRegistry;
	LPTSTR vRegistryKey = NULL;
	DWORD vRegistryKeyLen = 0L;
	DWORD ix = 0;

	// Allocate an initial block of memory used to read event records. The number 
	// of records read into the buffer will vary depending on the size of each event.
	// The size of each event will vary based on the size of the user-defined
	// data included with each event, the number and length of insertion 
	// strings, and other data appended to the end of the event record.
	dwBytesToRead = 65536;
	pBuffer = (PBYTE)TFalloc(dwBytesToRead);
	if (!pBuffer)
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogRecords::Load"));

	vRegistry.OpenLocalMachine();
	if (!vRegistry.OpenDataKey(_T("SYSTEM\\CurrentControlSet\\Services\\EventLog")))
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogRecords::Load"));

	while (vRegistry.QueryKeys(ix++, &vRegistryKey, &vRegistryKeyLen))
	{
		// The source name (provider) must exist as a subkey of Application.
		hEventLog = OpenEventLog(NULL, vRegistryKey);
		if (!hEventLog)
			continue;

		// Read blocks of records until you reach the end of the log or an 
		// error occurs. The records are read from newest to oldest. If the buffer
		// is not big enough to hold a complete event record, reallocate the buffer.
		status = ERROR_SUCCESS;
		while (ERROR_SUCCESS == status)
		{
			if (!ReadEventLog(hEventLog,
				EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ,
				0,
				pBuffer,
				dwBytesToRead,
				&dwBytesRead,
				&dwMinimumBytesToRead))
			{
				status = GetLastError();
				if (ERROR_INSUFFICIENT_BUFFER == status)
				{
					status = ERROR_SUCCESS;

					pTemp = (PBYTE)TFrealloc(pBuffer, dwMinimumBytesToRead);
					pBuffer = pTemp;
					dwBytesToRead = dwMinimumBytesToRead;
				}
				else if (ERROR_HANDLE_EOF != status)
					goto cleanup;
			}
			else
			{
				Ptr(CEventLogRecord) pEventRecord = NULL;
				PEVENTLOGRECORD pRecord = CastAnyPtr(EVENTLOGRECORD, pBuffer);
				PEVENTLOGRECORD pEndRecord = CastAnyPtr(EVENTLOGRECORD, l_ptradd(pBuffer, dwBytesRead));

				while (pRecord < pEndRecord)
				{
					pEventRecord = OK_NEW_OPERATOR CEventLogRecord;
					try
					{
						pEventRecord->Load(pRecord, vRegistryKey);
						Append(pEventRecord);
					}
					catch (CBaseException*)
					{
						pEventRecord->release();
					}
					pRecord = CastAnyPtr(EVENTLOGRECORD, l_ptradd(pRecord, pRecord->Length));
				}
			}
		}
		if (hEventLog)
		{
			CloseEventLog(hEventLog);
			hEventLog = NULL;
		}
	}

cleanup:

	if (hEventLog)
		CloseEventLog(hEventLog);

	if (pBuffer)
		TFfree(pBuffer);

	if (ERROR_HANDLE_EOF != status)
		ThrowDefaultException(__FILE__LINE__ _T("CEventLogRecords::Load"), status);
}

