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
#include "HttpDate.h"

#include "STRUTIL.H"

#ifdef OK_SYS_UNIX
#include <time.h>
#endif

// Request for Comments: 2616

// Sun, 06 Nov 1994 08:49:37 GMT ; RFC 822, updated by RFC 1123 
// Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036 
// Sun Nov 6 08:49:37 1994 ; ANSI C's asctime() format 

// HTTP-date = rfc1123-date | rfc850-date | asctime-date 
// rfc1123-date = wkday "," SP date1 SP time SP "GMT" 
// rfc850-date = weekday "," SP date2 SP time SP "GMT" #
// asctime-date = wkday SP date3 SP time SP 4DIGIT 
// date1 = 2DIGIT SP month SP 4DIGIT ; day month year (e.g., 02 Jun 1982) 
// date2 = 2DIGIT "-" month "-" 2DIGIT ; day-month-year (e.g., 02-Jun-82) 
// date3 = month SP ( 2DIGIT | ( SP 1DIGIT )) ; month day (e.g., Jun 2) 
// time = 2DIGIT ":" 2DIGIT ":" 2DIGIT ; 00:00:00 - 23:59:59 
// wkday = "Mon" | "Tue" | "Wed" | "Thu" | "Fri" | "Sat" | "Sun" 
// weekday = "Monday" | "Tuesday" | "Wednesday" | "Thursday" | "Friday" | "Saturday" | "Sunday" 
// month = "Jan" | "Feb" | "Mar" | "Apr" | "May" | "Jun" | "Jul" | "Aug" | "Sep" | "Oct" | "Nov" | "Dec" 

//static const char* wkdayb[] = {
//	"Sun" , "Mon" , "Tue" , "Wed" , "Thu" , "Fri" , "Sat" };
//static const char* weekdayb[] = {
//	"Sunday" , "Monday" , "Tuesday" , "Wednesday" , "Thursday" , "Friday" , "Saturday" }; 
//static const char* monthb[] = {
//	"Jan" , "Feb" , "Mar" , "Apr" , "May" , "Jun" , "Jul" , "Aug" , "Sep" , "Oct" , "Nov" , "Dec" };

static CConstPointer wkday[] = {
	_T("Sun") , _T("Mon") , _T("Tue") , _T("Wed") , _T("Thu") , _T("Fri") , _T("Sat") };
//static CConstPointer weekday[] = {
//	_T("Sunday") , _T("Monday") , _T("Tuesday") , _T("Wednesday") , _T("Thursday") , _T("Friday") , _T("Saturday") }; 
static CConstPointer month[] = {
	_T("Jan") , _T("Feb") , _T("Mar") , _T("Apr") , _T("May") , _T("Jun") , _T("Jul") , _T("Aug") , _T("Sep") , _T("Oct") , _T("Nov") , _T("Dec") };

static int TestDayOfWeek(Ref(CStringConstIterator) it)
{
	CConstPointer wday;
	bool bFound;

	for ( dword ix = 0; ix < 7; ++ix )
	{
		wday = wkday[ix];
		bFound = true;
		for ( dword jx = 0; jx < 3; ++jx )
		{
			if ( it[jx] == 0 )
				return -1;
			if ( it[jx] != wday[jx] )
			{
				bFound = false;
				break;
			}
		}
		if ( bFound )
		{
			it += 3;
			return ix;
		}
	}
	return -1;
}

static int TestMonth(Ref(CStringConstIterator) it)
{
	CConstPointer m;
	bool bFound;

	for ( dword ix = 0; ix < 12; ++ix )
	{
		m = month[ix];
		bFound = true;
		for ( dword jx = 0; jx < 3; ++jx )
		{
			if ( it[jx] == 0 )
				return -1;
			if ( it[jx] != m[jx] )
			{
				bFound = false;
				break;
			}
		}
		if ( bFound )
		{
			it += 3;
			return ix;
		}
	}
	return -1;
}

static int TestNumber(Ref(CStringConstIterator) it)
{
	int result = 0;

	if ( (it[0] < _T('0')) || (it[0] > _T('9')) )
		return -1;
	result = it[0] - _T('0');
	for ( ;; )
	{
		++it;
		if ( (it[0] < _T('0')) || (it[0] > _T('9')) )
			return result;
		result = 10 * result;
		result += it[0] - _T('0');
	}
	return -1;
}

CHttpDate::CHttpDate(void)
{
	Now();
}

CHttpDate::CHttpDate(CConstPointer date)
{
	SetDate(date);
}

CHttpDate::CHttpDate(ConstRef(CDateTime) date)
{
	SetDate(date);
}

CHttpDate::CHttpDate(ConstRef(CSystemTime) date)
{
	SetDate(date);
}

CHttpDate::~CHttpDate(void)
{
}

void CHttpDate::Now()
{
#ifdef OK_SYS_WINDOWS
	::GetSystemTime(&m_SystemTime);
#endif
#ifdef OK_SYS_UNIX
	time_t t;
	struct tm *t1;

	s_memset(&m_SystemTime, 0, sizeof(SYSTEMTIME));
	time(&t);
	t1 = gmtime(&t);
	if ( t1 )
	{
		m_SystemTime.wYear = t1->tm_year + 1900;
		m_SystemTime.wMonth = t1->tm_mon + 1;
		m_SystemTime.wDayOfWeek = t1->tm_wday;
		m_SystemTime.wDay = t1->tm_mday;
		m_SystemTime.wHour = t1->tm_hour;
		m_SystemTime.wMinute = t1->tm_min;
		m_SystemTime.wSecond = t1->tm_sec;
	}
#endif
}

ULONGLONG CHttpDate::GetTime2Compare() const
{
#ifdef OK_SYS_WINDOWS
	ULARGE_INTEGER result;
	FILETIME ftime;

	::SystemTimeToFileTime(&m_SystemTime, &ftime);
	result.HighPart = ftime.dwHighDateTime;
	result.LowPart = ftime.dwLowDateTime;
	return result.QuadPart;
#endif
#ifdef OK_SYS_UNIX
	struct tm t1;

	t1.tm_sec = m_SystemTime.wSecond;
	t1.tm_min = m_SystemTime.wMinute;
	t1.tm_hour = m_SystemTime.wHour;
	t1.tm_mday = m_SystemTime.wDay;
	t1.tm_mon = m_SystemTime.wMonth - 1;
	t1.tm_year = m_SystemTime.wYear - 1900;
	t1.tm_wday = m_SystemTime.wDayOfWeek;
	t1.tm_yday = 0;
	t1.tm_isdst = -1;
	return mktime(&t1);
#endif
}

CStringBuffer CHttpDate::GetDate()
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%3s, %02d %3s %4d %02d:%02d:%02d GMT"), 
		wkday[m_SystemTime.wDayOfWeek],
		m_SystemTime.wDay,
		month[m_SystemTime.wMonth - 1],
		m_SystemTime.wYear,
		m_SystemTime.wHour,
		m_SystemTime.wMinute,
		m_SystemTime.wSecond
		);
	return tmp;
}

void CHttpDate::SetDate(CConstPointer date)
{
	CStringConstIterator it(date);
	int result;

	s_memset(&m_SystemTime, 0, sizeof(SYSTEMTIME));
	result = TestDayOfWeek(it);
	if ( result < 0 )
		return;
	m_SystemTime.wDayOfWeek = result;
	switch ( it[0] )
	{
	case _T(' '):
		++it;
		ScanAscTime(it);
		break;
	case _T(','):
		it += 2;
		ScanRfc1123Time(it);
		break;
	default:
		for ( ; (!(it.IsEnd())) && (it[0] != _T(' ')); ++it );
		if ( it.IsEnd() )
			return;
		++it;
		ScanRfc850Time(it);
		break;
	}
}

void CHttpDate::SetDate(ConstRef(CDateTime) date)
{
	s_memset(&m_SystemTime, 0, sizeof(SYSTEMTIME));
	m_SystemTime.wDay = date.GetDays();
	m_SystemTime.wHour = date.GetHours();
	m_SystemTime.wMinute = date.GetMinutes();
	m_SystemTime.wMonth = date.GetMonths();
	m_SystemTime.wSecond = date.GetSeconds();
	m_SystemTime.wYear = date.GetYears();
	m_SystemTime.wDayOfWeek = date.GetWeekDay();
}

void CHttpDate::SetDate(ConstRef(CSystemTime) date)
{
	date.GetTime(m_SystemTime);
}

void CHttpDate::ScanAscTime(Ref(CStringConstIterator) it)
{
	int result;

	result = TestMonth(it);
	if ( result < 0 )
		return;
	m_SystemTime.wMonth = result + 1;
	it.EatWhite();
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wDay = result;
	it.EatWhite();
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wHour = result;
	++it; // :
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wMinute = result;
	++it; // :
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wSecond = result;
	it.EatWhite();
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wYear = result;
}

void CHttpDate::ScanRfc1123Time(Ref(CStringConstIterator) it)
{
	int result;

	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wDay = result;
	it.EatWhite();
	if ( it.IsEnd() )
		return;
	result = TestMonth(it);
	if ( result < 0 )
		return;
	m_SystemTime.wMonth = result + 1;
	it.EatWhite();
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wYear = result;
	it.EatWhite();
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wHour = result;
	++it; // :
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wMinute = result;
	++it; // :
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wSecond = result;
}

void CHttpDate::ScanRfc850Time(Ref(CStringConstIterator) it)
{
	int result;

	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wDay = result;
	++it; // -
	if ( it.IsEnd() )
		return;
	result = TestMonth(it);
	if ( result < 0 )
		return;
	m_SystemTime.wMonth = result + 1;
	++it; // -
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	if ( result > 50 )
		result += 1900;
	else
		result += 2000;
	m_SystemTime.wYear = result;
	it.EatWhite();
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wHour = result;
	++it; // :
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wMinute = result;
	++it; // :
	if ( it.IsEnd() )
		return;
	result = TestNumber(it);
	if ( result < 0 )
		return;
	m_SystemTime.wSecond = result;
}

