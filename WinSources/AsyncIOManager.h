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

#include "DataVector.h"
#include "WinSources.h"
#include "WinException.h"
#include "AsyncIOData.h"
#include "Event.h"

class WINSOURCES_API CAsyncIOManager : public CCppObject
{
public:
	CAsyncIOManager(dword iothcnt = 4, dword wmin = 5, dword wexp = 10, dword wmax = 25);
	virtual ~CAsyncIOManager(void);

	void Create(void);
	void Stop();
	void WaitForComplete(void);
	void Close(void);

	dword RunTask(void);

	void Register(Ptr(CAsyncIOData) pData);

protected:

	HANDLE m_iocp;
	CThreadPool m_tiocppool;
	CThreadPool m_tworkerpool;
	CEvent m_stopevent;
};

DECL_WINEXCEPTION(WINSOURCES_API, CAsyncIOManagerException, CWinException)
