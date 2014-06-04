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
#include "Mutex.h"

#ifdef OK_SYS_UNIX
#include <string.h>
#endif

IMPL_WINEXCEPTION(CMutexException, CWinException)

CMutex::CMutex(void)
{
#ifdef OK_SYS_WINDOWS
	m_Mutex = CreateMutex( 
        nullptr,              // default security attributes
        FALSE,             // initially not owned
        nullptr);             // unnamed mutex
	if ( !m_Mutex )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::CMutex"), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	pthread_mutexattr_t attr;
	int errCode;

	if ( (errCode = pthread_mutexattr_init(&attr)) != 0 )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::CMutex"), CWinException::CRunTimeError, errCode);
	if ( (errCode = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) != 0 )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::CMutex"), CWinException::CRunTimeError, errCode);
	if ( (errCode = pthread_mutex_init(&m_Mutex, &attr)) != 0 )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::CMutex"), CWinException::CRunTimeError, errCode);
	if ( (errCode = pthread_mutexattr_destroy(&attr)) != 0 )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::CMutex"), CWinException::CRunTimeError, errCode);
#endif
}

CMutex::~CMutex(void)
{
#ifdef OK_SYS_WINDOWS
	CloseHandle(m_Mutex);
#endif
#ifdef OK_SYS_UNIX
	int errCode;

	if ( (errCode = pthread_mutex_destroy(&m_Mutex)) != 0 )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::~CMutex"), CWinException::CRunTimeError, errCode);
#endif
}

void CMutex::lock()
{
#ifdef OK_SYS_WINDOWS
    switch ( WaitForSingleObject( 
            m_Mutex,    // handle to mutex
            INFINITE) ) 
    {
    // The thread got ownership of the mutex
    case WAIT_OBJECT_0: 
    // The thread got ownership of an abandoned mutex
    case WAIT_ABANDONED_0: 
        break; 
	default:
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
			_T("CMutex::lock"), CWinException::WinExtError);
		break;
    }
#endif
#ifdef OK_SYS_UNIX
	int errCode;

	if ( (errCode = pthread_mutex_lock(&m_Mutex)) != 0 )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::lock"), CWinException::CRunTimeError, errCode);
#endif
}

void CMutex::unlock()
{
#ifdef OK_SYS_WINDOWS
	if ( !ReleaseMutex(m_Mutex) )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::unlock"), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	int errCode;

	if ( (errCode = pthread_mutex_unlock(&m_Mutex)) != 0 )
		throw OK_NEW_OPERATOR CMutexException(__FILE__LINE__ _T("in %s CMutexException"), 
		    _T("CMutex::unlock"), CWinException::CRunTimeError, errCode);
#endif
}
