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

#include "DataDoubleLinkedList.h"
#include "WinSources.h"
#include "WinException.h"
#include "Mutex.h"
#include "ConditionVariable.h"

#ifdef OK_SYS_UNIX
#include <pthread.h>
#endif

/*******************************************************************
 CAbstractThreadCallback
*******************************************************************/
class WINSOURCES_API CAbstractThreadCallback: public CCppObject
{
public:
	virtual ~CAbstractThreadCallback();

	virtual dword invoke(void) = 0;	

protected:
	CAbstractThreadCallback();
	CAbstractThreadCallback(const CAbstractThreadCallback&);
};

/*******************************************************************
 CThreadCallback
*******************************************************************/
template <class C>
class CThreadCallback: public CAbstractThreadCallback
{
public:
	typedef dword (C::*Callback)(void);

	CThreadCallback(C* pObject, Callback method):
		CAbstractThreadCallback(),
		_pObject(pObject),
		_method(method)
	{
	}
	
	CThreadCallback(const CThreadCallback& cb):
		CAbstractThreadCallback(cb),
		_pObject(cb._pObject),
		_method(cb._method)
	{
	}
	
	virtual ~CThreadCallback()
	{
	}
	
	CThreadCallback& operator = (const CThreadCallback& cb)
	{
		if (&cb != this)
		{
			this->_pObject = cb._pObject;
			this->_method  = cb._method;
		}
		return *this;
	}
	
	virtual dword invoke(void)
	{
		return (_pObject->*_method)();
	}
	
private:
	CThreadCallback();
	
	C* _pObject;
	Callback _method;
};

/*******************************************************************
 CThread
*******************************************************************/
class WINSOURCES_API CThread: public CCppObject
{
public:
	enum {
		E_DONOTRUN = UINT_MAX - 3,
		E_STILLACTIVE = UINT_MAX - 2,
		E_JOINTIMEDOUT = UINT_MAX - 1,
		E_NORESULT = UINT_MAX
	};
	
	CThread(void);
	virtual ~CThread(void);

	void Start();
	dword Join();
	dword Join(long milliseconds);

#ifdef OK_SYS_WINDOWS
	__inline HANDLE GetHandle() const { return m_ThreadHandle; }
#endif
#ifdef OK_SYS_UNIX
	__inline pthread_t GetHandle() const { return m_ThreadHandle; }
#endif

	virtual dword Run() = 0;
	
	void Stop();
	static void StopAll();
	static void JoinAll();

	dword GetResultCode() const;
	void SetResultCode(dword code);

	static Ref(CMutex) locker();
	
protected:
	void _addToThreadGlobalList();
	void _removeFromThreadGlobalList();
	bool _checkForStop();
	
	volatile dword m_ResultCode;
	volatile bool m_bResultCode;
	volatile bool m_bStop;

#ifdef OK_SYS_WINDOWS
	HANDLE m_ThreadHandle;
	unsigned m_ThreadID;
#endif
#ifdef OK_SYS_UNIX
	bool m_bStarted;
	pthread_t m_ThreadHandle;
#endif

private:
	CThread(ConstRef(CThread));
	ConstRef(CThread) operator = (ConstRef(CThread));
};

/*******************************************************************
 CThreadEx
*******************************************************************/
class WINSOURCES_API CThreadEx: public CThread
{
public:
	CThreadEx(CAbstractThreadCallback* pCallback, bool bOnce = true);
	virtual ~CThreadEx();

protected:
	virtual dword Run();

	CAbstractThreadCallback* m_Callback;
	bool m_Once;

private:
	CThreadEx(ConstRef(CThreadEx));
	ConstRef(CThreadEx) operator = (ConstRef(CThreadEx));
};

/*******************************************************************
 CPooledThread
*******************************************************************/
class WINSOURCES_API CPooledThread: public CThread
{
public:
	typedef struct _tagCallback
	{
		CAbstractThreadCallback* callback;
		dword result;
	} TCallback;

	CPooledThread();
	virtual ~CPooledThread();

	void AddTask(CAbstractThreadCallback* pCallback);
	dword GetTaskCount();
	dword GetTaskResultAndRemove(CAbstractThreadCallback* pCallback);

protected:
	virtual dword Run();

	CDataDoubleLinkedListT<CAbstractThreadCallback> m_TaskQueue;
	CDataSDoubleLinkedListT<TCallback> m_ResultQueue;
	CConditionVariable m_condition;

private:
	CPooledThread(ConstRef(CPooledThread));
	ConstRef(CPooledThread) operator = (ConstRef(CPooledThread));
};

/*******************************************************************
 CThreadPool
*******************************************************************/
class WINSOURCES_API CThreadPool: public CCppObject
{
public:
	enum Strategy {
		LongLiveTask = 0x01,
		QuickResponse = 0x02
	};
public:
	CThreadPool(DECL_FILE_LINE dword min = 5, dword exp = 0, dword max = 5, word strategy = 0);
	virtual ~CThreadPool();

	__inline dword GetMinimumThreads() { return m_Min; }
	__inline dword GetExpandThreads() { return m_Exp; }
	__inline dword GetMaximumThreads() { return m_Max; }
	__inline dword GetStrategy() { return m_Strategy; }

	bool AddTask(CAbstractThreadCallback* pCallback);
	dword GetTaskResultAndRemove(CAbstractThreadCallback* pCallback);

	void WaitForComplete();
	void StopAll();
	void JoinAll();

protected:
	dword m_Min;
	dword m_Exp;
	dword m_Max;
	word m_Strategy;
	CDataDoubleLinkedListT<CPooledThread> m_Threads;

	bool MoreThreads(void);

private:
	CThreadPool(ConstRef(CThreadPool));
	ConstRef(CThreadPool) operator = (ConstRef(CThreadPool));
};

DECL_WINEXCEPTION(WINSOURCES_API, CThreadException, CWinException)
