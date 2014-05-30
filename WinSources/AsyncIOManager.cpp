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
#include "AsyncIOManager.h"
#include "AsyncIOData.h"
#include "ScopedLock.h"

IMPL_WINEXCEPTION(CAsyncIOManagerException, CWinException)

static void __stdcall EmptyDeleteFunc( ConstPointer data, Pointer context )
{
}

static sword __stdcall AsyncIOData_SearchAndSortFunc( ConstPointer item, ConstPointer data )
{
	Ptr(CAsyncIOData) pData1 = CastAnyPtr(CAsyncIOData, CastMutable(Pointer, item));
	Ptr(CAsyncIOData) pData2 = CastAnyPtr(CAsyncIOData, CastMutable(Pointer, data));

	if ( pData1 == pData2 )
		return 0;
	return 1;
}


CAsyncIOManager::CAsyncIOManager(dword iothcnt, dword wmin, dword wexp, dword wmax) :
    m_iocp(NULL),
	m_tasks(__FILE__LINE__ 256, 256),
	m_tiocppool(__FILE__LINE__ iothcnt, 0, iothcnt, CThreadPool::LongLiveTask),
	m_tworkerpool(__FILE__LINE__ wmin, wexp, wmax, CThreadPool::QuickResponse),
	m_stopevent()
{
}

CAsyncIOManager::~CAsyncIOManager(void)
{
}

void CAsyncIOManager::Create(void)
{
	m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_tiocppool.GetMaximumThreads());
	if ( m_iocp == NULL )
		throw OK_NEW_OPERATOR CAsyncIOManagerException(__FILE__LINE__ _T("%s Exception"), _T("CAsyncIOManager::Create"), CWinException::WinExtError);

	CScopedLock lock;

	for (dword i = 0; i < m_tiocppool.GetMaximumThreads(); ++i)
		m_tiocppool.AddTask(OK_NEW_OPERATOR CThreadCallback<CAsyncIOManager>(this, &CAsyncIOManager::RunTask));
}

void CAsyncIOManager::Stop()
{
	m_stopevent.set();
}

void CAsyncIOManager::WaitForComplete(void)
{
	m_stopevent.wait();
	m_tworkerpool.WaitForComplete();
	m_tworkerpool.StopAll();
	m_tworkerpool.JoinAll();
	m_tiocppool.StopAll();
	for (dword i = 0; i < m_tiocppool.GetMaximumThreads(); ++i)
		PostQueuedCompletionStatus(m_iocp, 0, NULL, NULL);
	m_tiocppool.JoinAll();
}

void CAsyncIOManager::Close(void)
{
	if ( m_iocp != NULL )
	{
		CloseHandle(m_iocp);
		m_iocp = NULL;
	}
}

dword CAsyncIOManager::RunTask(void)
{
	if ( m_iocp == NULL )
		return ULONG_MAX;

	BOOL bSuccess;
	DWORD dwIoSize;
	Ptr(CAsyncIOData) pData;
	LPOVERLAPPED lpOverlapped;

	while (true)
	{
		bSuccess = GetQueuedCompletionStatus(m_iocp, &dwIoSize, (PDWORD_PTR)&pData, (LPOVERLAPPED *)&lpOverlapped, INFINITE);
		if( !bSuccess )
		{
			if ( lpOverlapped == NULL )
				throw OK_NEW_OPERATOR CAsyncIOManagerException(__FILE__LINE__ _T("%s Exception"), _T("CAsyncIOManager::RunTask"), CWinException::WinExtError);
			if ( GetLastError() == ERROR_HANDLE_EOF )
			{
				assert(dwIoSize == 0);
				dwIoSize = 0;
			}
			if ( pData == NULL )
				return 0;
			pData->set_errorcode(GetLastError());
		}
		else
		{
			if ( pData == NULL )
				return 0;
			pData->set_errorcode(0);
		}

		CScopedLock lock;
		CAsyncIODataVector::Iterator it;

		it = m_tasks.Find<CAsyncIODataLessFunctor>(pData);
		if ( it )
			m_tasks.Remove(it);

		pData->set_bytestransferred(dwIoSize);
		pData->get_overlapped()->Offset += dwIoSize;
		m_tworkerpool.AddTask(pData->get_callback());
	}
}

void CAsyncIOManager::Register(Ptr(CAsyncIOData) pData)
{
	if ( m_iocp == NULL )
		return;

	HANDLE iocp;

	iocp = CreateIoCompletionPort(pData->get_file(), m_iocp, (DWORD_PTR)pData, 0);
	if ( iocp == NULL )
		throw OK_NEW_OPERATOR CAsyncIOManagerException(__FILE__LINE__ _T("%s Exception"), _T("CAsyncIOManager::Register"), CWinException::WinExtError);
}

void CAsyncIOManager::AddTask(Ptr(CAsyncIOData) pData)
{
	CScopedLock lock;

	m_tasks.Append(pData);
}
