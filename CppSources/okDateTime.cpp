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
#include "CPPS_PCH.H"
#include "okDateTime.h"
#include "BaseException.h"

static int monthdays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static int month2day(int mon)
{
	int result = 0;

	for ( int ix = 0; ix < mon; ++ix )
		result += monthdays[ix];
	return result;
}

static bool isLeapYear(int year)
{
	return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}

static int year2day(int year)
{
	return (year * 365) + (year / 4) - (year / 100) + (year / 400);
}

static time_t mktime2(struct tm* ts)
{
	time_t result = (((ts->tm_hour * 60) + ts->tm_min) * 60) + ts->tm_sec;
	
	result += (year2day(ts->tm_year + 1900 - 1) - year2day(1969)) * 86400;
	result += month2day(ts->tm_mon) * 86400;
	result += (ts->tm_mday - 1) * 86400;

	ts->tm_yday = month2day(ts->tm_mon) + (ts->tm_mday - 1);
	if ( isLeapYear(ts->tm_year + 1900) && (ts->tm_mon > 1) )
	{
		result += 86400;
		ts->tm_yday += 1;
	}

	int m = ts->tm_mon;
	int J = ts->tm_year + 1900;
	int K;

	if ( ts->tm_mon < 2 )
	{
		m += 13;
		--J;
	}
	else
		m += 1;
	K = J % 100;
	J = J / 100;
	// Zellers Kongruenz
	ts->tm_wday = (ts->tm_mday + (((m + 1) * 26) / 10) + K + (K / 4) + (J / 4) + (5 * J)) % 7;
	if ( ts->tm_wday == 0 )
		ts->tm_wday = 6;
	else
		ts->tm_wday -= 1;
	ts->tm_isdst = 0;
	return result;
}

CDateTime::CDateTime(CDateTime::TimeType ttype)
{
	Now(ttype);
}

CDateTime::CDateTime(time_t tval, int dst, CDateTime::TimeType ttype)
{
	SetTime(tval, dst, ttype);
}

CDateTime::CDateTime(word year, word month, word day, word hour, word minute, word second, int dst, CDateTime::TimeType ttype)
{
	SetTime(year, month, day, hour, minute, second, dst, ttype);
}

CDateTime::CDateTime(word dosdate, word dostime, int dst, TimeType ttype)
{
	SetTime(dosdate, dostime, dst, ttype);
}

CDateTime::~CDateTime(void)
{
}

void CDateTime::Now(TimeType ttype)
{
	time_t tval;

	s_memset(&m_TStruct, 0, sizeof(struct tm));
	time(&tval);
	if (((time_t)(-1)) == tval)
		throw CBaseException(__FILE__LINE__ _T("Exception in %s: time returns -1"), _T("CDateTime::Now"));

	struct tm* ts = NULL;

	if (ttype == LocalTime)
		ts = localtime(&tval);
	else
		ts = gmtime(&tval);
	if (NULL == ts)
		throw CBaseException(__FILE__LINE__ _T("Exception in %s: localtime or gmtime returns NULL"), _T("CDateTime::Now"));
	s_memcpy(&m_TStruct, ts, sizeof(struct tm));
	m_TType = ttype;
}

void CDateTime::GetDosDateTime(Ref(word) dosdate, Ref(word) dostime, TimeType ttype, Ptr(int) tdst) const
{
	time_t tval;

	GetTime(tval, ttype, tdst);
	//TIME: second 0-4, minute 5-10, hour 11-15, second resolution is 2!
	dostime = Cast(word, (GetSeconds() / 2) + (GetMinutes() << 5) + (GetHours() << 11));
	//DATE: day 0-4, month 5-8, year (starting with 1980): 9-16

	int year = GetYears() - 1980;

	if (year < 0)
		year = 0;
	dosdate = Cast(word, GetDays() + (GetMonths() << 5) + (year << 9));
}

void CDateTime::GetTime(Ref(time_t) t, TimeType ttype, Ptr(int) tdst) const
{
	struct tm tstruct;
	struct tm* ts;

	s_memcpy(&tstruct, &m_TStruct, sizeof(struct tm));
	switch (m_TType)
	{
	case LocalTime:
		switch (ttype)
		{
		case LocalTime:
			if (NotPtrCheck(tdst))
				*tdst = tstruct.tm_isdst;
			t = mktime2(&tstruct);
			break;
		case UTCTime:
			if (NotPtrCheck(tdst))
				*tdst = 0;
			t =  mktime(&tstruct);
			break;
		}
		break;
	case UTCTime:
		switch (ttype)
		{
		case LocalTime:
			t = mktime2(&tstruct);
			ts = localtime(&t);
			if (PtrCheck(ts))
				throw CBaseException(__FILE__LINE__ _T("Exception in %s: localtime returns NULL"), _T("CDateTime::GetTime"));
			if (NotPtrCheck(tdst))
				*tdst = ts->tm_isdst;
			t = mktime2(ts);
			break;
		case UTCTime:
			if (NotPtrCheck(tdst))
				*tdst = 0;
			t =  mktime2(&tstruct);
			break;
		}
		break;
	}
}

void CDateTime::SetTime(word dosdate, word dostime, int dst, TimeType ttype)
{
	//TIME: second 0-4, minute 5-10, hour 11-15, second resolution is 2!
	word sec = 2 * (dostime & 0x001fu);         // 0000 0000 0001 1111
	word min = ((dostime & 0x07e0u) >> 5);    // 0000 0111 1110 0000
	word hour = ((dostime & 0xf800u) >> 11);   // 1111 1000 0000 0000

	//DATE: day 0-4, month 5-8, year (starting with 1980): 9-16
	word day = (dosdate & 0x001fu);             // 0000 0000 0001 1111
	word mon = ((dosdate & 0x01e0u) >> 5);      // 0000 0001 1110 0000
	word year = 1980 + ((dosdate & 0xfe00u) >> 9); // 1111 1110 0000 0000

	s_memset(&m_TStruct, 0, sizeof(struct tm));

	SetYears(year);
	SetMonths(mon);
	SetDays(day);
	SetHours(hour);
	SetMinutes(min);
	SetSeconds(sec);
	SetDST(0);
	m_TType = UTCTime;
}

void CDateTime::SetTime(time_t tval, int dst, CDateTime::TimeType ttype)
{
	struct tm* ts = NULL;
	
	s_memset(&m_TStruct, 0, sizeof(struct tm));
	if (ttype == LocalTime)
		ts = localtime(&tval);
	else
		ts = gmtime(&tval);
	if ( NULL == ts )
		throw CBaseException(__FILE__LINE__ _T("Exception in %s: localtime or gmtime returns NULL"), _T("CDateTime::SetTime"));
	s_memcpy(&m_TStruct, ts, sizeof(struct tm));
	m_TStruct.tm_isdst = dst;
	m_TType = ttype;
}

void CDateTime::SetTime(word year, word month, word day, word hour, word minute, word second, int dst, CDateTime::TimeType ttype)
{
	s_memset(&m_TStruct, 0, sizeof(struct tm));

	SetYears(year);
	SetMonths(month);
	SetDays(day);
	SetHours(hour);
	SetMinutes(minute);
	SetSeconds(second);
	SetDST(dst);
	m_TType = ttype;
}

void CDateTime::GetTimeString(Ref(CStringBuffer) buf)
{
	buf.FormatString(__FILE__LINE__ _T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd"),
		GetDays(), GetMonths(), GetYears(), GetHours(), GetMinutes(), GetSeconds());
	if (m_TType == UTCTime)
		buf.AppendString(_T(" UTC"));
}

sqword CDateTime::Compare(ConstRef(CDateTime) other) const
{
	time_t t0;
	time_t t1;

	GetTime(t0);
	other.GetTime(t1);
	return Cast(sqword, t0 - t1);
}