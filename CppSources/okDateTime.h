/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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

#include "CppSources.h"
#include <time.h>

class CPPSOURCES_API CDateTime: public CCppObject
{
public:
	enum TimeType
	{
		UTCTime,
		LocalTime
	};

	CDateTime(TimeType ttype = UTCTime);
	CDateTime(time_t tval, int dst = -1, TimeType ttype = UTCTime);
	CDateTime(word year, word month, word day, word hour, word minute, word second, int dst = -1, TimeType ttype = UTCTime);
	CDateTime(word dosdate, word dostime, int dst = -1, TimeType ttype = UTCTime);
	virtual ~CDateTime(void);

	void Now(TimeType ttype = UTCTime);
	
	__inline word GetSeconds() const { return m_TStruct.tm_sec; } // 0..59
	__inline void SetSeconds(word v) { m_TStruct.tm_sec = v; }
	__inline word GetMinutes() const { return m_TStruct.tm_min; } // 0..59
	__inline void SetMinutes(word v) { m_TStruct.tm_min = v; }
	__inline word GetHours() const { return m_TStruct.tm_hour; } // 0..23
	__inline void SetHours(word v) { m_TStruct.tm_hour = v; }
	__inline word GetDays() const { return m_TStruct.tm_mday; } // 1..31
	__inline void SetDays(word v) { m_TStruct.tm_mday = v; }
	__inline word GetWeekDay() const { return m_TStruct.tm_wday; } // 0..6, 0 == Sunday
	__inline void SetWeekDay(word v) { m_TStruct.tm_wday = v; }
	__inline word GetMonths() const { return m_TStruct.tm_mon + 1; } // 1..12
	__inline void SetMonths(word v) { m_TStruct.tm_mon = v - 1; }
	__inline word GetYears() const { return m_TStruct.tm_year + 1900; } // 1970..2038
	__inline void SetYears(word v) { m_TStruct.tm_year = v - 1900; }
	__inline int GetDST() const { return m_TStruct.tm_isdst; }
	__inline void SetDST(int v) { m_TStruct.tm_isdst = v; }
	__inline TimeType GetTimeType() const { return m_TType; }

	void GetDosDateTime(Ref(word) dosdate, Ref(word) dostime, TimeType ttype = UTCTime, Ptr(int) tdst = NULL) const;
	void GetTime(Ref(time_t) t, TimeType ttype = UTCTime, Ptr(int) tdst = NULL) const;

	void SetTime(word dosdate, word dostime, int dst = -1, TimeType ttype = UTCTime);
	void SetTime(time_t tval = 0LL, int dst = -1, TimeType ttype = UTCTime);
	void SetTime(word year, word month, word day, word hour, word minute, word second, int dst = -1, TimeType ttype = UTCTime);

	void GetTimeString(Ref(CStringBuffer) buf);
	sqword Compare(ConstRef(CDateTime) other) const;

protected:
	TimeType m_TType;
	struct tm m_TStruct;
};

__inline bool operator == (ConstRef(CDateTime) a, ConstRef(CDateTime) b)
{
	return (a.Compare(b) == 0);
}

__inline bool operator != (ConstRef(CDateTime) a, ConstRef(CDateTime) b)
{
	return (a.Compare(b) != 0);
}

__inline bool operator < (ConstRef(CDateTime) a, ConstRef(CDateTime) b)
{
	return (a.Compare(b) < 0);
}

__inline bool operator <= (ConstRef(CDateTime) a, ConstRef(CDateTime) b)
{
	return (a.Compare(b) <= 0);
}

__inline bool operator > (ConstRef(CDateTime) a, ConstRef(CDateTime) b)
{
	return (a.Compare(b) > 0);
}

__inline bool operator >= (ConstRef(CDateTime) a, ConstRef(CDateTime) b)
{
	return (a.Compare(b) >= 0);
}
