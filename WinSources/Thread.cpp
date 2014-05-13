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
#include "Thread.h"
#include "ScopedLock.h"
#include "Mutex.h"

#ifdef OK_SYS_WINDOWS
#include "EventLogger.h"
#include <process.h>
#endif
#ifdef OK_SYS_UNIX
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#endif

#ifdef OK_SYS_WINDOWS
unsigned __stdcall CThreadMain(void *p)
{
	CThread* t = CastAnyPtr(CThread, p);
	dword ret = 0;

	try
	{
		ret = t->Run();
	}
	catch ( CBaseException* ex )
	{
		CEventLogger::WriteClassicLog(CEventLogger::Error, CEventLogger::NetworkCategory, ex->GetExceptionMessage());
		CEventLogger::CleanUp();
	}
	catch ( ... )
	{
		CEventLogger::WriteClassicLog(CEventLogger::Error, CEventLogger::NetworkCategory, _T("Thread aborted with an arbitrary exception."));
		CEventLogger::CleanUp();
	}
	t->SetResultCode(ret);
	_endthreadex(ret);
	return ret;
}
#endif
#ifdef OK_SYS_UNIX
static void* CThreadMain(void *p)
{
	CThread* t = CastAnyPtr(CThread, p);
	dword ret = 0;

	try
	{
		ret = t->Run();
	}
	catch (CBaseException* ex)
	{
		ret = -1;
	}
	catch (...)
	{
		ret = -4;
	}
	t->SetResultCode(ret);
	pthread_exit(p);
	return p;
}
#endif

IMPL_WINEXCEPTION(CThreadException, CWinException)

static void __stdcall EmptyDeleteFunc( ConstPointer data, Pointer context )
{
}

static sword __stdcall ThreadListSearchAndSortFunc( ConstPointer item, ConstPointer data )
{
	CThread* pThread1 = CastAnyPtr(CThread, CastMutable(Pointer, item));
	CThread* pThread2 = CastAnyPtr(CThread, CastMutable(Pointer, data));

#ifdef OK_SYS_WINDOWS
	if (pThread1->GetHandle() == pThread2->GetHandle())
#endif
#ifdef OK_SYS_UNIX
	if (pthread_equal(pThread1->GetHandle(), pThread2->GetHandle()))
#endif
		return 0;
	return 1;
}

static void __stdcall ThreadListDeleteFunc( ConstPointer data, Pointer context )
{
	CThread* pThread = CastAnyPtr(CThread, CastMutable(Pointer, data));

	pThread->release();
}

static void __stdcall ThreadCallbackDeleteFunc( ConstPointer data, Pointer context )
{
	CAbstractThreadCallback* pCallback = CastAnyPtr(CAbstractThreadCallback, CastMutable(Pointer, data));

	pCallback->release();
}

static void __stdcall ResultQueueDeleteFunc( ConstPointer data, Pointer context )
{
	CPooledThread::TCallback* pCallback = CastAnyPtr(CPooledThread::TCallback, CastMutable(Pointer, data));

	pCallback->callback->release();
}

static sword __stdcall ResultQueueSearchAndSortFunc( ConstPointer item, ConstPointer data )
{
	CPooledThread::TCallback* pCallback1 = CastAnyPtr(CPooledThread::TCallback, CastMutable(Pointer, item));
	CPooledThread::TCallback* pCallback2 = CastAnyPtr(CPooledThread::TCallback, CastMutable(Pointer, data));

	if ( pCallback1->callback == pCallback2->callback )
		return 0;
	return 1;
}

static CMutex gMutex;
static CDataDoubleLinkedListT<CThread> gThreadList(__FILE__LINE__ ThreadListDeleteFunc);

/*******************************************************************
 CAbstractThreadCallback
*******************************************************************/
CAbstractThreadCallback::~CAbstractThreadCallback()
{
}

CAbstractThreadCallback::CAbstractThreadCallback()
{
}

CAbstractThreadCallback::CAbstractThreadCallback(const CAbstractThreadCallback&)
{
}

/*******************************************************************
 CThread
*******************************************************************/
CThread::CThread(void):
	m_ResultCode(E_DONOTRUN),
	m_bResultCode(false),
	m_bStop(false),
#ifdef OK_SYS_WINDOWS
	m_ThreadHandle(NULL),
	m_ThreadID(0)
#endif
#ifdef OK_SYS_UNIX
	m_bStarted(false),
	m_ThreadHandle(0)
#endif
{
}

CThread::~CThread(void)
{
}

void CThread::_addToThreadGlobalList()
{
	CScopedLock _lock;

	gThreadList.Append(this);
}

void CThread::_removeFromThreadGlobalList()
{
	CScopedLock _lock;

	if ( gThreadList.Count() > 0 )
	{
		CDataDoubleLinkedListT<CThread>::Iterator it = gThreadList.Find(this, ThreadListSearchAndSortFunc);

		if ( it )
			gThreadList.Remove(it, EmptyDeleteFunc, NULL);
	}
}

void CThread::Start()
{
	if ( _checkForStop() )
		return;
#ifdef OK_SYS_WINDOWS
	m_ThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &CThreadMain, this, 0, &m_ThreadID);
	if (!m_ThreadHandle)
		throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"),
		_T("CThread::Start"), CWinException::CRunTimeError);
	m_ResultCode = E_STILLACTIVE;
	m_bResultCode = false;
#endif
#ifdef OK_SYS_UNIX
	CScopedLock _lock;
	pthread_attr_t _attr;
	int errCode;

	if ((errCode = pthread_attr_init(&_attr)) != 0)
		throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"),
		_T("CThread::Start"), CWinException::CRunTimeError, errCode);
	if ((errCode = pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_JOINABLE)) != 0)
		throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"),
		_T("CThread::Start"), CWinException::CRunTimeError, errCode);
	if ((errCode = pthread_create(&m_ThreadHandle, &_attr, &CThreadMain, this)) != 0)
		throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"),
		_T("CThread::Start"), CWinException::CRunTimeError, errCode);
	m_bStarted = true;
	m_ResultCode = E_STILLACTIVE;
	m_bResultCode = false;
	if ((errCode = pthread_attr_destroy(&_attr)) != 0)
		throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"),
		_T("CThread::Start"), CWinException::CRunTimeError, errCode);
	_lock.unlock();
#endif
	_addToThreadGlobalList();
}

dword CThread::Join()
{
#ifdef OK_SYS_WINDOWS
	if ( !m_ThreadHandle )
		return m_ResultCode;
	switch ( WaitForSingleObject(m_ThreadHandle, INFINITE) )
	{
	case WAIT_OBJECT_0:
		CloseHandle(m_ThreadHandle);
		m_ThreadHandle = NULL;
		break;
	default:
		throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"), 
			_T("CThread::Join"), CWinException::WinExtError);
		break;
	}
#endif
#ifdef OK_SYS_UNIX
	if ( !m_bStarted )
		return m_ResultCode;
	
	Pointer data = NULL;
	
	if ( int errCode = pthread_join(m_ThreadHandle, &data) )
		throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"), 
		    _T("CThread::Join"), CWinException::CRunTimeError, errCode);
	m_bStarted = false;
#endif
	_removeFromThreadGlobalList();
	return m_ResultCode;
}

dword CThread::Join(long milliseconds)
{
	dword result = m_ResultCode;
#ifdef OK_SYS_WINDOWS
	if ( !m_ThreadHandle )
		return result;
	switch ( WaitForSingleObject(m_ThreadHandle, milliseconds) )
	{
	case WAIT_TIMEOUT:
		result = E_JOINTIMEDOUT;
		break;
	case WAIT_OBJECT_0:
		CloseHandle(m_ThreadHandle);
		m_ThreadHandle = NULL;
		break;
	default:
		throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"), 
			_T("CThread::Join"), CWinException::WinExtError);
		break;
	}
#endif
#ifdef OK_SYS_UNIX
	if ( !m_bStarted )
		return result;

	CScopedLock _lock;
	bool bWait = false;
	bool bJoin = false;
	
	result = E_JOINTIMEDOUT;
	if ( m_bResultCode )
		bJoin = true;
	else
		bWait = true;
	_lock.unlock();
	if ( bWait )
	{
		struct timeval tv;

		tv.tv_sec = milliseconds / 1000;
		tv.tv_usec = (milliseconds % 1000) * 1000;
		select(0, NULL, NULL, NULL, &tv);
		_lock.lock();
		if ( m_bResultCode )
			bJoin = true;
		_lock.unlock();
	}
	if ( bJoin )
	{
		Pointer data = NULL;
	
		if ( int errCode = pthread_join(m_ThreadHandle, &data) )
			throw OK_NEW_OPERATOR CThreadException(__FILE__LINE__ _T("in %s CThreadException"), 
		        _T("CThread::Join"), CWinException::CRunTimeError, errCode);
		m_bStarted = false;
		result = m_ResultCode;
	}
#endif
	if ( result != E_JOINTIMEDOUT )
		_removeFromThreadGlobalList();
	return result;
}

void CThread::Stop()
{
	CScopedLock _lock;

	m_bStop = true;
}

bool CThread::_checkForStop()
{
	CScopedLock _lock;
	bool result = false;
	
	if ( m_bStop )
		result = true;
	return result;
}

void CThread::StopAll()
{
	CScopedLock _lock;

	if ( gThreadList.Count() > 0 )
	{
		CDataDoubleLinkedListT<CThread>::Iterator it = gThreadList.Begin();

		while ( it )
		{
			CThread* pThread = *it;
			pThread->Stop();
			++it;
		}
	}
}

void CThread::JoinAll()
{
	CScopedLock _lock;

	while ( gThreadList.Count() > 0 )
	{
		CDataDoubleLinkedListT<CThread>::Iterator it = gThreadList.Begin();

		while ( it )
		{
			CThread* pThread = *it;
			dword result = E_JOINTIMEDOUT;

			_lock.unlock();
			try
			{
				result = pThread->Join(100);
			}
			catch ( CThreadException* ex )
			{
#ifdef OK_SYS_WINDOWS
				CEventLogger::WriteClassicLog(CEventLogger::Error, CEventLogger::NetworkCategory, ex->GetExceptionMessage());
				CEventLogger::CleanUp();
#endif
			}
			_lock.lock();
			it = gThreadList.Begin();
		}
	}
}

dword CThread::GetResultCode() const 
{
	return m_ResultCode; 
}

void CThread::SetResultCode(dword code) 
{ 
	CScopedLock _lock;

	m_bResultCode = true;
	m_ResultCode = code;
}

Ref(CMutex) CThread::locker()
{
	return gMutex;
}

/*******************************************************************
 CThreadEx
*******************************************************************/
CThreadEx::CThreadEx(CAbstractThreadCallback* pCallback, bool bOnce):
    m_Callback(pCallback),
	m_Once(bOnce)
{
}

CThreadEx::~CThreadEx()
{
	m_Callback->release();
}

dword CThreadEx::Run()
{
	if ( m_Once )
	{
		if ( m_Callback )
			return m_Callback->invoke();
	}
	else if ( m_Callback )
	{
		int ret = 0;

		while ( (!(ret = m_Callback->invoke())) && (!(_checkForStop())) );
		return ret;
	}
	return E_NORESULT;
}

/*******************************************************************
 CPooledThread
*******************************************************************/
CPooledThread::CPooledThread():
m_TaskQueue(__FILE__LINE__ ThreadCallbackDeleteFunc),
m_ResultQueue(__FILE__LINE__ ResultQueueDeleteFunc),
m_condition()
{
}

CPooledThread::~CPooledThread()
{
}

void CPooledThread::AddTask(CAbstractThreadCallback* pCallback)
{
	m_condition.lock();
	m_TaskQueue.Append(pCallback);
	m_condition.unlock();
	m_condition.wake();
}

dword CPooledThread::GetTaskCount()
{
	dword result = 0;

	m_condition.lock();
	result = m_TaskQueue.Count();
	m_condition.unlock();
	return result;
}

dword CPooledThread::GetTaskResultAndRemove(CAbstractThreadCallback* pCallback)
{
	dword result = E_NORESULT;
	TCallback findRec;
	CDataSDoubleLinkedListT<TCallback>::Iterator it;

	m_condition.lock();
	findRec.callback = pCallback;
	it = m_ResultQueue.Find(&findRec, ResultQueueSearchAndSortFunc);

	if ( it && (*it) )
	{
		result = (*it)->result;
		m_ResultQueue.Remove(it, ResultQueueDeleteFunc, NULL);
	}
	m_condition.unlock();
	return result;
}

dword CPooledThread::Run()
{
	CAbstractThreadCallback* pCallback = NULL;
	CDataDoubleLinkedListT<CAbstractThreadCallback>::Iterator it;

	while ( !_checkForStop() )
	{
		m_condition.lock();
		while ( m_TaskQueue.Count() > 0 )
		{
			it = m_TaskQueue.Begin();
			pCallback = *it;
			m_condition.unlock();

			TCallback result;

			result.callback = pCallback;
			try
			{
				result.result = pCallback->invoke();
			}
			catch ( CBaseException* ex )
			{
#ifdef OK_SYS_WINDOWS
				CEventLogger::WriteClassicLog(CEventLogger::Error, CEventLogger::NetworkCategory, ex->GetExceptionMessage());
				CEventLogger::CleanUp();
#endif
				result.result = -4;
			}

			m_condition.lock();
			m_TaskQueue.Remove(m_TaskQueue.Begin(), EmptyDeleteFunc, NULL);
			m_ResultQueue.Append(&result);
		}
		m_condition.sleep(100);
		m_condition.unlock();
	}
	return 0;
}

/*******************************************************************
 CThreadPool
*******************************************************************/
CThreadPool::CThreadPool(DECL_FILE_LINE dword min, dword exp, dword max, word strategy):
    m_Min(min),
	m_Exp(exp),
	m_Max(max),
	m_Strategy(strategy),
	m_Threads(ARGS_FILE_LINE0)
{
	MoreThreads();
}

CThreadPool::~CThreadPool()
{
	CScopedLock _lock;

	m_Threads.Close(ThreadListDeleteFunc, NULL);
}

bool CThreadPool::MoreThreads(void)
{
	CScopedLock _lock;

	if ( m_Threads.Count() < m_Min )
	{
		for ( dword ix = m_Threads.Count(); ix < m_Min; ++ix )
		{
			CPooledThread* pThread = OK_NEW_OPERATOR CPooledThread();

			m_Threads.Append(pThread);
			pThread->Start();
		}
		return true;
	}

	dword cnt = m_Threads.Count();
	dword exp = 0;

	if ( m_Exp == 0 )
	{
		exp = m_Max;
		if ( exp == 0 )
			return false;
	}
	else if ( m_Max == 0 )
	{
		exp = cnt + m_Exp;
	}
	else
	{
		exp = cnt + m_Exp;
		if ( exp > m_Max )
			exp = m_Max;
	}
	if ( exp > cnt )
	{
		for ( dword ix = cnt; ix < exp; ++ix )
		{
			CPooledThread* pThread = OK_NEW_OPERATOR CPooledThread();

			m_Threads.Append(pThread);
			pThread->Start();
		}
		return true;
	}
	return false;
}

bool CThreadPool::AddTask(CAbstractThreadCallback* pCallback)
{
	CScopedLock _lock;
	CDataDoubleLinkedListT<CPooledThread>::Iterator it;
	CDataDoubleLinkedListT<CPooledThread>::Iterator it1;
	CPooledThread* pThread;
	dword minCnt = UINT_MAX;
	dword cnt;

	for ( ;; )
	{
		it = m_Threads.Begin();
		while ( it )
		{
			pThread = *it;
			if ( pThread->GetTaskCount() == 0 )
			{
				pThread->AddTask(pCallback);
				return true;
			}
			++it;
		}
		if ( m_Strategy == 0 )
			break;
		if ( !MoreThreads() )
			break;
	}
	it = m_Threads.Begin();
	while ( it )
	{
		pThread = *it;
		cnt = pThread->GetTaskCount();
		if ( cnt < minCnt )
		{
			minCnt = cnt;
			it1 = it;
		}
		++it;
	}
	if ( minCnt < UINT_MAX )
	{
		pThread = *it1;
		pThread->AddTask(pCallback);
		return true;
	}
	return false;
}

dword CThreadPool::GetTaskResultAndRemove(CAbstractThreadCallback* pCallback)
{
	CScopedLock _lock;
	CDataDoubleLinkedListT<CPooledThread>::Iterator it;
	CPooledThread* pThread;
	dword result = CThread::E_NORESULT;

	it = m_Threads.Begin();
	while ( it )
	{
		pThread = *it;
		if ( (result = pThread->GetTaskResultAndRemove(pCallback)) != CThread::E_NORESULT )
			return result;
		++it;
	}
	return result;
}

void CThreadPool::WaitForComplete()
{
	bool bContinue = true;

	while ( bContinue )
	{
		CScopedLock _lock;
		CDataDoubleLinkedListT<CPooledThread>::Iterator it;
		CPooledThread* pThread;
		bool b = true;

		it = m_Threads.Begin();
		while ( it )
		{
			pThread = *it;
			if ( pThread->GetTaskCount() > 0 )
			{
				b = false;
				break;
			}
			++it;
		}
		_lock.unlock();
		if ( b )
			bContinue = false;
		else
#ifdef OK_SYS_WINDOWS
			Sleep(100);
#endif
#ifdef OK_SYS_UNIX
			{
				struct timeval tv;

				tv.tv_sec = 0;
				tv.tv_usec = 100000;
				select(0, NULL, NULL, NULL, &tv);
			}
#endif
	}
}

void CThreadPool::StopAll()
{
	CScopedLock _lock;
	CDataDoubleLinkedListT<CPooledThread>::Iterator it;
	CPooledThread* pThread;

	it = m_Threads.Begin();
	while ( it )
	{
		pThread = *it;
		pThread->Stop();
		++it;
	}
}

void CThreadPool::JoinAll()
{
	CScopedLock _lock;
	CDataDoubleLinkedListT<CPooledThread>::Iterator it;
	CPooledThread* pThread;

	it = m_Threads.Begin();
	while ( it )
	{
		pThread = *it;
		dword result = CThread::E_JOINTIMEDOUT;

		_lock.unlock();
		try
		{
			result = pThread->Join(100);
		}
		catch ( CThreadException* ex )
		{
#ifdef OK_SYS_WINDOWS
			CEventLogger::WriteClassicLog(CEventLogger::Error, CEventLogger::NetworkCategory, ex->GetExceptionMessage());
			CEventLogger::CleanUp();
#endif
		}
		_lock.lock();
		if ( result == CThread::E_JOINTIMEDOUT )
			++it;
		else
		{
			m_Threads.Remove(it, ThreadListDeleteFunc, NULL);
			it = m_Threads.Begin();
		}
	}
}
