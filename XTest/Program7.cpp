/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#include "Program.h"
#include "Thread.h"
#include "Mutex.h"
#include "ScopedLock.h"

#ifdef OK_SYS_UNIX
#include <string.h>
#include <unistd.h>
#define Sleep(t) sleep(t/1000)
#endif

/**
   Debugging
**/
static void wcoutThread(CConstPointer start, CThread* thread, CConstPointer ende)
{
	CScopedLock _lock;
	COUT << start << ende << std::endl;
}

static void wcout(CConstPointer msg)
{
	CScopedLock _lock;
	COUT << msg << std::endl;
}

class CTestThread: public CThread
{
public:
	CTestThread() {}
	virtual dword Run() 
	{ 
		wcoutThread(_T("2: CTestThread::Run "), this, _T(" start"));
		Sleep(1000); 
		wcoutThread(_T("4: CTestThread::Run "), this, _T(" exit"));
		return 0; 
	}
};

void TestThreads()
{
	wcout(_T("1: after CThread::Initialize"));

	CTestThread testthread;

	testthread.Start();

	wcout(_T("3: after CTestThread::Start"));

	CThread::StopAll();
	CThread::JoinAll();
	wcout(_T("5: after CThread::StopAll"));
}

void TestThreads1()
{
	wcout(_T("1: after CThread::Initialize"));

	CTestThread testthread;
	CThreadEx testthreadex(OK_NEW_OPERATOR CThreadCallback<CTestThread>(&testthread, &CTestThread::Run));

	testthreadex.Start();

	wcout(_T("3: after CTestThreadEx::Start"));

	CThread::StopAll();
	CThread::JoinAll();
	wcout(_T("5: after CThread::StopAll"));
}

void TestThreads2()
{
	wcout(_T("1: after CThread::Initialize"));

	CThreadPool testthreadpool __FILE__LINE__0P;
	CTestThread testthread;
	Ptr(CThreadCallback<CTestThread>) cb[6];

	for ( int i = 0; i < 6; ++i )
		cb[i] = OK_NEW_OPERATOR CThreadCallback<CTestThread>(&testthread, &CTestThread::Run);

	CScopedLock _lock;

	for ( int i = 0; i < 6; ++i )
		testthreadpool.AddTask(cb[i]);

	_lock.unlock();

	wcout(_T("3: after CThreadPool::AddTask"));

	testthreadpool.WaitForComplete();

	for ( int i = 0; i < 6; ++i )
		assert(testthreadpool.GetTaskResultAndRemove(cb[i]) == 0);

	testthreadpool.StopAll();
	testthreadpool.JoinAll();

	wcout(_T("5: after CThreadPool::StopAll"));
}
