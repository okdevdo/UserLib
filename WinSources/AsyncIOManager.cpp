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

CAsyncIOManager::CAsyncIOManager(dword iothcnt, dword wmin, dword wexp, dword wmax) :
    m_iocp(nullptr),
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
	m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, m_tiocppool.GetMaximumThreads());
	if ( m_iocp == nullptr )
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
		PostQueuedCompletionStatus(m_iocp, 0, NULL, nullptr);
	m_tiocppool.JoinAll();
}

void CAsyncIOManager::Close(void)
{
	if ( m_iocp != nullptr )
	{
		CloseHandle(m_iocp);
		m_iocp = nullptr;
	}
}

dword CAsyncIOManager::RunTask(void)
{
	if ( m_iocp == nullptr )
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
			if ( lpOverlapped == nullptr )
				throw OK_NEW_OPERATOR CAsyncIOManagerException(__FILE__LINE__ _T("%s Exception"), _T("CAsyncIOManager::RunTask"), CWinException::WinExtError);
			if ( GetLastError() == ERROR_HANDLE_EOF )
			{
				assert(dwIoSize == 0);
				dwIoSize = 0;
			}
			if ( pData == nullptr )
				return 0;
			pData->set_errorcode(GetLastError());
		}
		else
		{
			if ( pData == nullptr )
				return 0;
			pData->set_errorcode(0);
		}
		
		CScopedLock lock;

		assert(NotPtrCheck(pData->get_callback()));
		pData->set_bytestransferred(dwIoSize);
		pData->get_overlapped()->Offset += dwIoSize;
		m_tworkerpool.AddTask(pData->get_callback());
	}
}

void CAsyncIOManager::Register(Ptr(CAsyncIOData) pData)
{
	if ( m_iocp == nullptr )
		return;

	HANDLE iocp;

	iocp = CreateIoCompletionPort(pData->get_file(), m_iocp, (DWORD_PTR)pData, 0);
	if ( iocp == nullptr )
		throw OK_NEW_OPERATOR CAsyncIOManagerException(__FILE__LINE__ _T("%s Exception"), _T("CAsyncIOManager::Register"), CWinException::WinExtError);
}
