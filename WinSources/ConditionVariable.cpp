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
#include "ConditionVariable.h"

IMPL_WINEXCEPTION(CConditionVariableException, CWinException)

CConditionVariable::CConditionVariable(void):
	m_bCondition(false)
{
#ifdef OK_SYS_WINDOWS
#if (WINVER >= _WIN32_WINNT_VISTA) && (OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR)
	InitializeConditionVariable(&m_condition);
#else
	m_event = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
	InitializeCriticalSection(&m_lock);
#endif
#ifdef OK_SYS_UNIX
	int errCode; 

	if ( (errCode = pthread_cond_init(&m_condition, NULL)) != 0 )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
		    _T("CConditionVariable::CConditionVariable"), CWinException::CRunTimeError, errCode);
	if ( (errCode = pthread_mutex_init(&m_lock, NULL)) != 0 )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
		    _T("CConditionVariable::CConditionVariable"), CWinException::CRunTimeError, errCode);
#endif
}

CConditionVariable::~CConditionVariable(void)
{
#ifdef OK_SYS_WINDOWS
	DeleteCriticalSection(&m_lock);
#endif
#ifdef OK_SYS_UNIX
	int errCode;

	if ( (errCode = pthread_mutex_destroy(&m_lock)) != 0 )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
		    _T("CConditionVariable::~CConditionVariable"), CWinException::CRunTimeError, errCode);
	if ( (errCode = pthread_cond_destroy(&m_condition)) != 0 )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
		    _T("CConditionVariable::~CConditionVariable"), CWinException::CRunTimeError, errCode);
#endif
}

void CConditionVariable::lock()
{
#ifdef OK_SYS_WINDOWS
	EnterCriticalSection(&m_lock);
#endif
#ifdef OK_SYS_UNIX
	int errCode;

	if ( (errCode = pthread_mutex_lock(&m_lock)) != 0 )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
		    _T("CConditionVariable::lock"), CWinException::CRunTimeError, errCode);
#endif
}

void CConditionVariable::unlock()
{
#ifdef OK_SYS_WINDOWS
	LeaveCriticalSection(&m_lock);
#endif
#ifdef OK_SYS_UNIX
	if ( int errCode = pthread_mutex_unlock(&m_lock) )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
		    _T("CConditionVariable::unlock"), CWinException::CRunTimeError, errCode);
#endif
}

bool CConditionVariable::sleep(dword millisec)
{
#ifdef OK_SYS_WINDOWS
#if (WINVER >= _WIN32_WINNT_VISTA) && (OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR)
	while (!m_bCondition)
	{
		if (SleepConditionVariableCS(&m_condition, &m_lock, millisec) == 0)
		{
			if (GetLastError() == ERROR_TIMEOUT)
				return false;
			throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"),
				_T("CConditionVariable::sleep"), CWinException::WinExtError);
		}
	}
	m_bCondition = false;
	return true;
#else
	bool result = false;

	unlock();
	switch ( WaitForSingleObject(m_event, millisec) )
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_OBJECT_0:
		result = true;
		break;
	default:
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
			_T("CConditionVariable::sleep"), CWinException::WinExtError);
		break;
	}
	ResetEvent(m_event);
	lock();
	return result;
#endif
#endif
#ifdef OK_SYS_UNIX
	if ( millisec == INFINITE )
	{
		if ( int errCode = pthread_cond_wait(&m_condition, &m_lock) )
			throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
				_T("CConditionVariable::sleep"), CWinException::CRunTimeError, errCode);
		return true;
	}

	struct timespec t;
	dword sec = millisec / 1000;
	dword nsec = (millisec % 1000) * 1000000;
	
	if ( int errCode = clock_gettime(CLOCK_REALTIME, &t) )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
				_T("CConditionVariable::sleep"), CWinException::CRunTimeError, errCode);
	t.tv_sec += sec;
	t.tv_nsec += nsec;
	if ( t.tv_nsec > 1000000000 )
	{
		t.tv_sec += 1;
		t.tv_nsec -= 1000000000;
	}
	if ( int errCode = pthread_cond_timedwait(&m_condition, &m_lock, &t) )
	{
		if ( errCode == ETIMEDOUT )
			return false;
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
			_T("CConditionVariable::sleep"), CWinException::CRunTimeError, errCode);
	}
	return true;
#endif
}

void CConditionVariable::wake()
{
	m_bCondition = true;
#ifdef OK_SYS_WINDOWS
#if (WINVER >= _WIN32_WINNT_VISTA) && (OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR)
	WakeConditionVariable(&m_condition);
#else
	if ( !SetEvent(m_event) )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
		    _T("CConditionVariable::wake"), CWinException::WinExtError);	
#endif
#endif
#ifdef OK_SYS_UNIX
	if ( int errCode = pthread_cond_signal(&m_condition) )
		throw OK_NEW_OPERATOR CConditionVariableException(__FILE__LINE__ _T("in %s CConditionVariableException"), 
		    _T("CConditionVariable::wake"), CWinException::CRunTimeError, errCode);
#endif
}
