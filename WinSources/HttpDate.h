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
#include "okDateTime.h"
#include "SystemTime.h"

#ifdef OK_SYS_UNIX
#ifndef _WIN32
typedef struct _SYSTEMTIME {
    WUInt wYear;
    WUInt wMonth;
    WUInt wDayOfWeek;
    WUInt wDay;
    WUInt wHour;
    WUInt wMinute;
    WUInt wSecond;
    WUInt wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef unsigned long long ULONGLONG;
#endif
#endif

class WINSOURCES_API CHttpDate
{
public:
	CHttpDate(void);
	CHttpDate(CConstPointer date); // rfc1123-date | rfc850-date | asctime-date
	CHttpDate(ConstRef(CDateTime) date);
	CHttpDate(ConstRef(CSystemTime) date);
	virtual ~CHttpDate(void);

	void Now(); // UTC - Time

	__inline word GetMilliSeconds() const { return m_SystemTime.wMilliseconds; } // 0..999
	__inline word GetSeconds() const { return m_SystemTime.wSecond; } // 0..59
	__inline word GetMinutes() const { return m_SystemTime.wMinute; } // 0..59
	__inline word GetHours() const { return m_SystemTime.wHour; } // 0..23
	__inline word GetDays() const { return m_SystemTime.wDay; } // 1..31
	__inline word GetWeekDay() const { return m_SystemTime.wDayOfWeek; } // 0..6, 0 == Sunday
	__inline word GetMonths() const { return m_SystemTime.wMonth; } // 1..12
	__inline word GetYears() const { return m_SystemTime.wYear; }

	CStringBuffer GetDate(); // rfc1123-date
	void SetDate(CConstPointer date); // rfc1123-date | rfc850-date | asctime-date
	void SetDate(ConstRef(CDateTime) date);
	void SetDate(ConstRef(CSystemTime) date);
	ULONGLONG GetTime2Compare() const;

protected:
	SYSTEMTIME m_SystemTime;

	void ScanAscTime(Ref(CStringConstIterator) it);
	void ScanRfc1123Time(Ref(CStringConstIterator) it);
	void ScanRfc850Time(Ref(CStringConstIterator) it);
};

__inline bool operator==(ConstRef(CHttpDate) ta, ConstRef(CHttpDate) tb)
{
	return (ta.GetTime2Compare() == tb.GetTime2Compare());
}

__inline bool operator!=(ConstRef(CHttpDate) ta, ConstRef(CHttpDate) tb)
{
	return (ta.GetTime2Compare() != tb.GetTime2Compare());
}

__inline bool operator<(ConstRef(CHttpDate) ta, ConstRef(CHttpDate) tb)
{
	return (ta.GetTime2Compare() < tb.GetTime2Compare());
}

__inline bool operator<=(ConstRef(CHttpDate) ta, ConstRef(CHttpDate) tb)
{
	return (ta.GetTime2Compare() <= tb.GetTime2Compare());
}

__inline bool operator>(ConstRef(CHttpDate) ta, ConstRef(CHttpDate) tb)
{
	return (ta.GetTime2Compare() > tb.GetTime2Compare());
}

__inline bool operator>=(ConstRef(CHttpDate) ta, ConstRef(CHttpDate) tb)
{
	return (ta.GetTime2Compare() >= tb.GetTime2Compare());
}

