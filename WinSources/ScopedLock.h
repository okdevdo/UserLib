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
#include "Mutex.h"
#include "Thread.h"

class CScopedLock
{
public:
	CScopedLock(): m_Mutex(CThread::locker()), m_bLocked(false) { lock(); }
	CScopedLock(Ref(CMutex) _mutex): m_Mutex(_mutex), m_bLocked(false) { lock(); }
	~CScopedLock(void) { unlock(); }

	__inline void lock() { if ( m_bLocked ) return; m_Mutex.lock(); m_bLocked = true; }
	__inline void unlock() { if ( !m_bLocked ) return; m_Mutex.unlock(); m_bLocked = false; }

private:
	CScopedLock(const CScopedLock&);
	CScopedLock& operator = (const CScopedLock&);

	Ref(CMutex) m_Mutex;
	bool m_bLocked;
};

