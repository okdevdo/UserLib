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
#include "ScopedLock.h"
#include "SyncTCPSocketClasses.h"

#include "DataDoubleLinkedList.h"

#define DEFAULT_PORT _T("27016")
#define MAX_BUFFER		512

static void wcout(CConstPointer msg)
{
	CScopedLock _lock;
	COUT << msg << std::endl;
}

static void TestTCPClientKeepAlive_SendData(CTcpClient& tcpClient, const CStringBuffer& buffer)
{
	CStringBuffer sBuffer;
	CByteBuffer bBuffer;
	dword numBytesSend;

	sBuffer.FormatString(__FILE__LINE__ _T("##%05d## %s"), buffer.GetLength(), buffer.GetString());
	sBuffer.convertToByteBuffer(bBuffer);
	tcpClient.SendData(bBuffer.get_Buffer(), bBuffer.get_BufferSize(), &numBytesSend);
	assert(numBytesSend == bBuffer.get_BufferSize());
}

static void TestTCPClientKeepAlive_ReceiveData(CTcpClient& tcpClient, CStringBuffer& buffer)
{
	CStringBuffer responseString;
	CByteBuffer responseBuffer;
	dword numDataReceived = 0;
	dword totalDataReceived = 0;
	CStringConstIterator itB;
	CStringBuffer sBuffer;
	dword numData;

	for ( ; totalDataReceived < 10; )
	{
		responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
		tcpClient.ReceiveData(responseBuffer.get_Buffer() + totalDataReceived, MAX_BUFFER, &numDataReceived);
		totalDataReceived += numDataReceived;
		responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
	}
	responseString.convertFromByteBuffer(responseBuffer);
	itB = responseString.GetString();
	if ( (itB[0] == _T('#')) && (itB[1] == _T('#')) && (itB[7] == _T('#')) && (itB[8] == _T('#')) )
	{
		responseString.SubString(2, 5, sBuffer);
		if ( sBuffer.ScanString(_T("%u"), &numData) <= 0 )
			return;
	}
	else
		return;
	for ( ; totalDataReceived < (10 + numData); )
	{
		responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
		tcpClient.ReceiveData(responseBuffer.get_Buffer() + totalDataReceived, MAX_BUFFER, &numDataReceived);
		totalDataReceived += numDataReceived;
		responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
	}
	responseString.convertFromByteBuffer(responseBuffer);
	responseString.SubString(10, numData, buffer);
}

void TestTCPClientKeepAlive(CStringLiteral command)
{
	CTcpClient tcpClient;
	CStringBuffer sBuffer;
	CStringBuffer sBuffer0(__FILE__LINE__ _T("10: Testdaten (Client): 0123456789"));
	CStringBuffer sBuffer1(__FILE__LINE__ _T("20: Testdaten (Client): 0123456789"));
	CStringBuffer sBuffer2(__FILE__LINE__ _T("30: Testdaten (Client): 0123456789"));
	CStringBuffer sBuffer3(__FILE__LINE__ _T("40: Testdaten (Client): 0123456789"));
	CStringBuffer sBuffer4(__FILE__LINE__ _T("50: Testdaten (Client): 0123456789"));

	try
	{
		tcpClient.OpenConnection(_T("localhost"), DEFAULT_PORT);

		TestTCPClientKeepAlive_SendData(tcpClient, sBuffer0);
		TestTCPClientKeepAlive_ReceiveData(tcpClient, sBuffer);
		COUT << sBuffer << endl;

		TestTCPClientKeepAlive_SendData(tcpClient, sBuffer1);
		TestTCPClientKeepAlive_ReceiveData(tcpClient, sBuffer);
		COUT << sBuffer << endl;

		TestTCPClientKeepAlive_SendData(tcpClient, sBuffer2);
		TestTCPClientKeepAlive_ReceiveData(tcpClient, sBuffer);
		COUT << sBuffer << endl;

		TestTCPClientKeepAlive_SendData(tcpClient, sBuffer3);
		TestTCPClientKeepAlive_ReceiveData(tcpClient, sBuffer);
		COUT << sBuffer << endl;

		sBuffer.Clear();
		if ( !!command )
		{
			COUT << command << endl;
			sBuffer.AppendString(command);
			sBuffer.AppendString(_T("\r\n"));
		}
		sBuffer.AppendString(sBuffer4);
		TestTCPClientKeepAlive_SendData(tcpClient, sBuffer);
		TestTCPClientKeepAlive_ReceiveData(tcpClient, sBuffer);
		COUT << sBuffer << endl;

		tcpClient.CloseConnection();
	}
	catch ( CTcpClientException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
		tcpClient.CloseConnection();
	}
}

static void __stdcall CTestTCPServerDeleteFunc( ConstPointer data, Pointer context )
{
	CTcpClient* pClient = CastAnyPtr(CTcpClient, CastMutable(Pointer, data));

	pClient->release();
}

static void __stdcall CAbstractThreadCallbackDeleteFunc( ConstPointer data, Pointer context )
{
	CAbstractThreadCallback* pCallback = CastAnyPtr(CAbstractThreadCallback, CastMutable(Pointer, data));

	pCallback->release();
}

static void __stdcall EmptyDeleteFunc( ConstPointer data, Pointer context )
{
}

class CTestTCPServerKeepAlive: public CTcpServer
{
public:
	CTestTCPServerKeepAlive(): 
		_callback(__FILE__LINE__0),
		_tcpclients(__FILE__LINE__0),
		_current(NULL), 
		_threadpool(__FILE__LINE__ 5, 5, 0, CThreadPool::QuickResponse), 
		_bExit(false)
	{
	}
	~CTestTCPServerKeepAlive() 
	{ 
		CScopedLock _lock;

		_tcpclients.Close();
		_callback.Close();
		if ( _current )
			_current->release();
	}

	void SendData(CTcpClient* tcpClient, const CStringBuffer& buffer)
	{
		CScopedLock _lock;
		CStringBuffer sBuffer;
		CByteBuffer bBuffer;
		dword numBytesSend;

		sBuffer.FormatString(__FILE__LINE__ _T("##%05d## %s"), buffer.GetLength(), buffer.GetString());
		sBuffer.convertToByteBuffer(bBuffer);
		_lock.unlock();
		tcpClient->SendData(bBuffer.get_Buffer(), bBuffer.get_BufferSize(), &numBytesSend);
		assert(numBytesSend == bBuffer.get_BufferSize());
	}

	void ReceiveData(CTcpClient* tcpClient, CStringBuffer& buffer)
	{
		CScopedLock _lock;
		CStringBuffer responseString;
		CByteBuffer responseBuffer;
		dword numDataReceived = 0;
		dword totalDataReceived = 0;
		CStringConstIterator itB;
		CStringBuffer sBuffer;
		dword numData;

		for ( ; totalDataReceived < 10; )
		{
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
			_lock.unlock();
			tcpClient->ReceiveData(responseBuffer.get_Buffer() + totalDataReceived, MAX_BUFFER, &numDataReceived);
			totalDataReceived += numDataReceived;
			_lock.lock();
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
		}
		responseString.convertFromByteBuffer(responseBuffer);
		itB = responseString.GetString();
		buffer = responseString;
		if ( (itB[0] == _T('#')) && (itB[1] == _T('#')) && (itB[7] == _T('#')) && (itB[8] == _T('#')) )
		{
			responseString.SubString(2, 5, sBuffer);
			if ( sBuffer.ScanString(_T("%u"), &numData) <= 0 )
				return;
		}
		else
			return;
		for ( ; totalDataReceived < (10 + numData); )
		{
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
			_lock.unlock();
			tcpClient->ReceiveData(responseBuffer.get_Buffer() + totalDataReceived, MAX_BUFFER, &numDataReceived);
			totalDataReceived += numDataReceived;
			_lock.lock();
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
		}
		responseString.convertFromByteBuffer(responseBuffer);
		responseString.SubString(10, numData, buffer);
	}

	dword ClientTask()
	{
		CScopedLock _lock;
		CTcpClient* pClient = NULL;
		tcpclients_t::Iterator it = _tcpclients.Begin();
				
		if ( it && (*it) )
		{
			pClient = *it;
			_tcpclients.Remove(it);
		}
		if ( !pClient )
			return 0;

		CStringBuffer sBuffer;

		wcout(_T("ClientTask"));
		try
		{
			CStringConstIterator itB;
			bool bExit = false;

			_lock.unlock();
			ReceiveData(pClient, sBuffer);
			wcout(sBuffer.GetString());
			SendData(pClient, sBuffer);

			ReceiveData(pClient, sBuffer);
			wcout(sBuffer.GetString());
			SendData(pClient, sBuffer);

			ReceiveData(pClient, sBuffer);
			wcout(sBuffer.GetString());
			SendData(pClient, sBuffer);

			ReceiveData(pClient, sBuffer);
			wcout(sBuffer.GetString());
			SendData(pClient, sBuffer);

			ReceiveData(pClient, sBuffer);
			itB = sBuffer;
			itB.Find(_T("STOP\r\n"));
			if ( itB.IsBegin() )
				bExit = true;
			wcout(sBuffer.GetString());
			SendData(pClient, sBuffer);

			pClient->CloseConnection();
			_lock.lock();
			pClient->release();
			_bExit = bExit;
		}
		catch ( CTcpClientException* ex )
		{
			COUT << ex->GetExceptionMessage() << endl;
			pClient->CloseConnection();
		}
		return 0;
	}

	dword ServerTask()
	{
		CScopedLock _lock;

		_lock.unlock();

		wcout(_T("ServerTask"));

		for ( ;; )
		{
			if ( TestAccept(100) )
			{
				_current = OK_NEW_OPERATOR CTcpClient();
				DoAccept(_current);
				break;
			}
			else
			{
				_lock.lock();
				if ( _bExit )
					return 1;
				_lock.unlock();
			}
		}

		_lock.lock();

		CAbstractThreadCallback* pCallback = 
			OK_NEW_OPERATOR CThreadCallback<CTestTCPServerKeepAlive>(this, &CTestTCPServerKeepAlive::ClientTask);

		_tcpclients.Append(_current);
		_current = NULL;
		//pCallback->addRef();
		_callback.Append(pCallback);
		_threadpool.AddTask(pCallback);
		return 0;
	}

private:
	typedef CDataDoubleLinkedListT<CTcpClient, CCppObjectLessFunctor<CTcpClient>, CCppObjectNullFunctor<CTcpClient> > tcpclients_t;

	CDataDoubleLinkedListT<CAbstractThreadCallback> _callback;
	tcpclients_t _tcpclients;
	CTcpClient* _current;
	CThreadPool _threadpool;
	volatile bool _bExit;
};

void TestTCPServerKeepAlive()
{
	CTestTCPServerKeepAlive server;
	CThreadEx serverTask(
		OK_NEW_OPERATOR CThreadCallback<CTestTCPServerKeepAlive>(&server, &CTestTCPServerKeepAlive::ServerTask), 
		false);

	try
	{
		server.OpenConnection(NULL, DEFAULT_PORT);
		serverTask.Start();
		serverTask.Join();
		server.CloseConnection();
	}
	catch ( CTcpServerException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
		server.CloseConnection();
	}
	CThread::StopAll();
	CThread::JoinAll();
}
