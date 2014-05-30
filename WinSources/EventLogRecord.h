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
#include "WinException.h"
#include "DataVector.h"
#include "okDateTime.h"

DECL_WINEXCEPTION(WINSOURCES_API, CEventLogRecordException, CWinException)

class CFile;
class WINSOURCES_API CEventLogRecord :
	public CCppObject
{
public:
	enum PrintOutput
	{
		ReadableTextOutPut,
		TabdelimitedOutPut,
		XMLOutput
	};

	CEventLogRecord();
	virtual ~CEventLogRecord();

	void Load(PEVENTLOGRECORD rec, LPCTSTR providerRoot);
	void Print(PrintOutput outputType, Ptr(CFile) outputFile);

	static void PrintTabDelimitedHeader(Ptr(CFile) outputFile);
	static void PrintXMLHeader(CConstPointer rootElement, Ptr(CFile) outputFile);
	static void PrintXMLFooter(CConstPointer rootElement, Ptr(CFile) outputFile);

	__inline ConstRef(CStringBuffer) get_provider() const { return _provider; }
	__inline DWORD get_recordno() const { return _recordno; }
	__inline DWORD get_eventID() const { return _eventID; }
	__inline WORD get_eventType() const { return _eventType; }
	__inline ConstRef(CStringBuffer) get_eventTypeAsStr() const { return _eventTypeAsStr; }
	__inline ConstRef(CDateTime) get_timeGenerated() const { return _timeGenerated; }
	__inline ConstRef(CDateTime) get_timeWritten() const { return _timeWritten; }
	__inline ConstRef(CStringBuffer) get_sidAsStr() const { return _sidAsStr; }
	__inline ConstRef(CStringBuffer) get_userAccount() const { return _userAccount; }
	__inline ConstRef(CStringBuffer) get_userDomain() const { return _userDomain; }
	__inline SID_NAME_USE get_userUse() const { return _userUse; }
	CStringBuffer get_userUseAsStr() const;
	__inline WORD get_category() const { return _category; }
	__inline ConstRef(CStringBuffer) get_categoryAsStr() const { return _categoryAsStr; }
	__inline ConstRef(CStringBuffer) get_message() const { return _message; }
	__inline ConstRef(CStringBuffer) get_data() const { return _data; }

protected:

	CStringBuffer _provider;
	DWORD _recordno;
	DWORD _eventID;
	WORD _eventType;
	CStringBuffer _eventTypeAsStr;
	CDateTime _timeGenerated;
	CDateTime _timeWritten;
	CStringBuffer _sidAsStr;
	CStringBuffer _userAccount;
	CStringBuffer _userDomain;
	SID_NAME_USE _userUse;
	WORD _category;
	CStringBuffer _categoryAsStr;
	CStringBuffer _message;
	CStringBuffer _data;
};

class WINSOURCES_API CEventLogRecordLessFunctor
{
public:
	bool operator()(ConstRef(CEventLogRecord) r1, ConstRef(CEventLogRecord) r2) const
	{
		return (r1.get_recordno() < r2.get_recordno());
	}
};

class WINSOURCES_API CEventLogRecords : public CDataVectorT<CEventLogRecord, CEventLogRecordLessFunctor>
{
	typedef CDataVectorT<CEventLogRecord, CEventLogRecordLessFunctor> super;

public:
	CEventLogRecords(DECL_FILE_LINE0);
	virtual ~CEventLogRecords();

	void Load();
};