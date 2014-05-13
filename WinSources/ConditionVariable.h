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

#ifdef OK_SYS_UNIX
#include <pthread.h>
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif

class WINSOURCES_API CConditionVariable
{
public:
	CConditionVariable(void);
	~CConditionVariable(void);

	void lock();
	void unlock();
	bool sleep(dword millisec = INFINITE);
	void wake();

private:
	volatile bool m_bCondition;
#ifdef OK_SYS_WINDOWS
#if (WINVER >= _WIN32_WINNT_VISTA)
	CONDITION_VARIABLE m_condition;
#else
#endif
	HANDLE m_event;
	CRITICAL_SECTION m_lock;
#endif
#ifdef OK_SYS_UNIX
	pthread_cond_t m_condition;
	pthread_mutex_t m_lock;
#endif
};

DECL_WINEXCEPTION(WINSOURCES_API, CConditionVariableException, CWinException)
