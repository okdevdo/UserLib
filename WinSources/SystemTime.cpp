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
#include "SystemTime.h"

#ifdef OK_SYS_WINDOWS
CSystemTime::CSystemTime() :
m_TType(UTCTime),
m_isdst(0)
{
	memset(&m_Time, 0, sizeof(SYSTEMTIME));
	Now();
}

CSystemTime::CSystemTime(ConstRef(SYSTEMTIME) v, TimeType type):
m_TType(UTCTime),
m_isdst(0)
{
	memset(&m_Time, 0, sizeof(SYSTEMTIME));
	SetTime(v, type);
}

CSystemTime::CSystemTime(ConstRef(FILETIME) v, TimeType type):
m_TType(UTCTime),
m_isdst(0)
{
	memset(&m_Time, 0, sizeof(SYSTEMTIME));
	SetTime(v, type);
}

CSystemTime::CSystemTime(ConstRef(CDateTime) v):
m_TType(UTCTime),
m_isdst(0)
{
	memset(&m_Time, 0, sizeof(SYSTEMTIME));
	SetTime(v);
}

CSystemTime::~CSystemTime()
{
}

void CSystemTime::Now()
{
	GetLocalTime(&m_Time);
	m_TType = LocalTime;
	m_isdst = 0;

#if (_WIN32_WINNT >= 0x0600)
	DYNAMIC_TIME_ZONE_INFORMATION tz;

	ZeroMemory(&tz, sizeof(DYNAMIC_TIME_ZONE_INFORMATION));
	switch (GetDynamicTimeZoneInformation(&tz))
	{
	case TIME_ZONE_ID_UNKNOWN:
		break;
	case TIME_ZONE_ID_STANDARD:
		break;
	case TIME_ZONE_ID_DAYLIGHT:
		m_isdst = 1;
		break;
	case TIME_ZONE_ID_INVALID:
	default:
		break;
	}
#else
	TIME_ZONE_INFORMATION tz;

	ZeroMemory(&tz, sizeof(TIME_ZONE_INFORMATION));
	switch (GetTimeZoneInformation(&tz))
	{
	case TIME_ZONE_ID_UNKNOWN:
		break;
	case TIME_ZONE_ID_STANDARD:
		break;
	case TIME_ZONE_ID_DAYLIGHT:
		m_isdst = 1;
		break;
	case TIME_ZONE_ID_INVALID:
	default:
		break;
	}
#endif
}

void CSystemTime::SetTime(ConstRef(SYSTEMTIME) v, TimeType type)
{
	CopyMemory(&m_Time, &v, sizeof(SYSTEMTIME));
	m_TType = type;
}

void CSystemTime::SetTime(ConstRef(FILETIME) v, TimeType type)
{
	FileTimeToSystemTime(&v, &m_Time);
	m_TType = type;
}

void CSystemTime::SetTime(ConstRef(CDateTime) v)
{
	SetTime(v.GetYears(), v.GetMonths(), v.GetDays(),
		v.GetHours(), v.GetMinutes(), v.GetSeconds(),
		v.GetDST(), (v.GetTimeType() == CDateTime::UTCTime) ? UTCTime : LocalTime);
}

void CSystemTime::GetDosDateTime(word& dosdate, word& dostime) const
{
	FILETIME ft;

	SystemTimeToFileTime(&m_Time, &ft);
	FileTimeToDosDateTime(&ft, &dosdate, &dostime);
}

void CSystemTime::GetTime(Ref(SYSTEMTIME) v, TimeType ttype) const
{
	switch (m_TType)
	{
	case LocalTime:
		switch (ttype)
		{
		case LocalTime:
			v = m_Time;
			break;
		case UTCTime:
			TzSpecificLocalTimeToSystemTime(NULL, CastMutablePtr(SYSTEMTIME, &m_Time), &v);
			break;
		}
		break;
	case UTCTime:
		switch (ttype)
		{
		case LocalTime:
			SystemTimeToTzSpecificLocalTime(NULL, CastMutablePtr(SYSTEMTIME, &m_Time), &v);
			break;
		case UTCTime:
			v = m_Time;
			break;
		}
		break;
	}
}

void CSystemTime::GetFileTime(Ref(FILETIME) v, TimeType ttype) const
{
	SYSTEMTIME st;

	v.dwHighDateTime = 0;
	v.dwLowDateTime = 0;
	switch (m_TType)
	{
	case LocalTime:
		switch (ttype)
		{
		case LocalTime:
			SystemTimeToFileTime(&m_Time, &v);
			break;
		case UTCTime:
			TzSpecificLocalTimeToSystemTime(NULL, CastMutablePtr(SYSTEMTIME, &m_Time), &st);
			SystemTimeToFileTime(&st, &v);
			break;
		}
		break;
	case UTCTime:
		switch (ttype)
		{
		case LocalTime:
			SystemTimeToTzSpecificLocalTime(NULL, CastMutablePtr(SYSTEMTIME, &m_Time), &st);
			SystemTimeToFileTime(&st, &v);
			break;
		case UTCTime:
			SystemTimeToFileTime(&m_Time, &v);
			break;
		}
		break;
	}
}

void CSystemTime::GetTimeString(Ref(CStringBuffer) buf) const
{
	buf.FormatString(__FILE__LINE__ _T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd, DST=%d, TType=%s"),
		GetDays(), GetMonths(), GetYears(), GetHours(), GetMinutes(), GetSeconds(), GetDST(),
		(m_TType == UTCTime) ? _T("UTC") : (_T("Local")));
}

void CSystemTime::SetTime(time_t tval, TimeType ttype)
{
	FILETIME ft;
	LONGLONG ll = (Castsqword(tval) * 10000000LL) + 116444736000000000LL;
	ft.dwLowDateTime = Castdword(ll);
	ft.dwHighDateTime = Castdword(ll >> 32);
	FileTimeToSystemTime(&ft, &m_Time);
	m_TType = ttype;
}

void CSystemTime::SetTime(word year, word month, word day, word hour, word minute, word second, int dst, TimeType ttype)
{
	SetYears(year);
	SetMonths(month);
	SetDays(day);
	SetHours(hour);
	SetMinutes(minute);
	SetSeconds(second);
	SetDST(dst);
	m_TType = ttype;
}

void CSystemTime::SetTime(word dosdate, word dostime)
{
	FILETIME ft;

	DosDateTimeToFileTime(dosdate, dostime, &ft);
	m_TType = UTCTime;
}

sdword CSystemTime::Compare(ConstRef(CSystemTime) other) const
{
	FILETIME ft1;
	FILETIME ft2;

	CastMutablePtr(CSystemTime, this)->GetFileTime(ft1);
	CastMutablePtr(CSystemTime, &other)->GetFileTime(ft2);
	return CompareFileTime(&ft1, &ft2);
}
#endif