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

#ifdef OK_SYS_WINDOWS
class WINSOURCES_API CSystemTime : public CCppObject
{
public:
	enum TimeType
	{
		UTCTime,
		LocalTime
	};	
	
	CSystemTime();
	CSystemTime(ConstRef(SYSTEMTIME) v, TimeType type = LocalTime);
	CSystemTime(ConstRef(FILETIME) v, TimeType type = UTCTime);
	CSystemTime(ConstRef(CDateTime) v);
	virtual ~CSystemTime();

	void Now();

	__inline word GetSeconds() const { return m_Time.wSecond; } // 0..59
	__inline void SetSeconds(word v) { m_Time.wSecond = v; }
	__inline word GetMinutes() const { return m_Time.wMinute; } // 0..59
	__inline void SetMinutes(word v) { m_Time.wMinute = v; }
	__inline word GetHours() const { return m_Time.wHour; } // 0..23
	__inline void SetHours(word v) { m_Time.wHour = v; }
	__inline word GetDays() const { return m_Time.wDay; } // 1..31
	__inline void SetDays(word v) { m_Time.wDay = v; }
	__inline word GetWeekDay() const { return m_Time.wDayOfWeek; } // 0..6, 0 == Sunday
	__inline void SetWeekDay(word v) { m_Time.wDayOfWeek = v; }
	__inline word GetMonths() const { return m_Time.wMonth; } // 1..12
	__inline void SetMonths(word v) { m_Time.wMonth = v; }
	__inline word GetYears() const { return m_Time.wYear; } // 0..65536
	__inline void SetYears(word v) { m_Time.wYear = v; }
	__inline int GetDST() const { return m_isdst; }
	__inline void SetDST(int v) { m_isdst = v; }
	__inline TimeType GetTimeType() const { return m_TType; }

	void SetTime(ConstRef(SYSTEMTIME) v, TimeType type = LocalTime);
	void SetTime(ConstRef(FILETIME) v, TimeType type = UTCTime);
	void SetTime(ConstRef(CDateTime) v);

	void GetDosDateTime(word& dosdate, word& dostime) const;
	void GetTime(Ref(SYSTEMTIME) v, TimeType ttype = LocalTime) const;
	void GetFileTime(Ref(FILETIME) v, TimeType ttype = UTCTime) const;
	void GetTimeString(Ref(CStringBuffer) buf) const;

	void SetTime(time_t tval = 0LL, TimeType ttype = UTCTime);
	void SetTime(word year, word month, word day, word hour, word minute, word second, int dst = -1, TimeType ttype = UTCTime);
	void SetTime(word dosdate, word dostime);

	sdword Compare(ConstRef(CSystemTime) other) const;

protected:
	TimeType m_TType;
	int m_isdst;
	SYSTEMTIME m_Time;
};

__inline bool operator == (ConstRef(CSystemTime) a, ConstRef(CSystemTime) b)
{
	return (a.Compare(b) == 0);
}

__inline bool operator != (ConstRef(CSystemTime) a, ConstRef(CSystemTime) b)
{
	return (a.Compare(b) != 0);
}

__inline bool operator < (ConstRef(CSystemTime) a, ConstRef(CSystemTime) b)
{
	return (a.Compare(b) < 0);
}

__inline bool operator <= (ConstRef(CSystemTime) a, ConstRef(CSystemTime) b)
{
	return (a.Compare(b) <= 0);
}

__inline bool operator > (ConstRef(CSystemTime) a, ConstRef(CSystemTime) b)
{
	return (a.Compare(b) > 0);
}

__inline bool operator >= (ConstRef(CSystemTime) a, ConstRef(CSystemTime) b)
{
	return (a.Compare(b) >= 0);
}
#endif
#ifdef OK_SYS_UNIX
#define CSystemTime CDateTime
#endif