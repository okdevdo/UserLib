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

#define DEFAULT_PORT _T("8080")
#define MAX_BUFFER		512

static void wcout(CConstPointer msg)
{
	CScopedLock _lock;
	COUT << msg << std::endl;
}

void TestTCPClient(CStringLiteral command)
{
	CTcpClient tcpClient;
	CStringBuffer sBuffer(__FILE__LINE__ _T("Testdaten (Client): 0123456789"));
	CByteBuffer bBuffer;
	dword numBytesSend;
	CStringBuffer responseString;
	CByteBuffer responseBuffer;
	dword numDataReceived = 0;
	dword totalDataReceived = 0;

	if ( command.GetString() )
	{
		COUT << command << endl;
		sBuffer.PrependString(_T("\r\n"));
		sBuffer.PrependString(command.GetString());
	}
	try
	{
		tcpClient.OpenConnection(_T("localhost"), DEFAULT_PORT);
		sBuffer.convertToByteBuffer(bBuffer);
		tcpClient.SendData(bBuffer.get_Buffer(), bBuffer.get_BufferSize(), &numBytesSend);
		assert(numBytesSend == bBuffer.get_BufferSize());
		tcpClient.ShutDownConnection();
		do 
		{
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
			tcpClient.ReceiveData(responseBuffer.get_Buffer() + totalDataReceived, MAX_BUFFER, &numDataReceived);
			totalDataReceived += numDataReceived;
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
		} 
		while ( numDataReceived > 0 );
		responseString.convertFromByteBuffer(responseBuffer);
		COUT << responseString << endl;
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

class CTestTCPServer: public CTcpServer
{
public:
	CTestTCPServer() : 
		_callback(__FILE__LINE__0),
		_tcpclients(__FILE__LINE__0),
		_current(NULL),
		_threadpool(__FILE__LINE__ 5, 5, 0, CThreadPool::QuickResponse), 
		_bExit(false)
	{
	}
	~CTestTCPServer() 
	{
		CScopedLock _lock;

		_tcpclients.Close(CTestTCPServerDeleteFunc, NULL);
		_callback.Close(CAbstractThreadCallbackDeleteFunc, NULL);
		if ( _current )
			_current->release();
	}

	dword ClientTask()
	{
		CScopedLock _lock;
		CTcpClient* pClient = NULL;
		CDataDoubleLinkedListT<CTcpClient>::Iterator it = _tcpclients.Begin();
				
		if ( it && (*it) )
		{
			pClient = *it;
			_tcpclients.Remove(it, EmptyDeleteFunc, NULL);
		}
		if ( !pClient )
			return 0;

		CStringBuffer sBuffer(__FILE__LINE__ _T("Testdaten (Server): 0123456789"));
		CByteBuffer bBuffer;
		dword numBytesSend;
		CStringBuffer responseString;
		CByteBuffer responseBuffer;
		dword numDataReceived = 0;
		dword totalDataReceived = 0;
		CStringConstIterator itStr;
		bool bExit = false;

		wcout(_T("ClientTask"));
		try
		{
			do 
			{
				responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
				_lock.unlock();
				pClient->ReceiveData(responseBuffer.get_Buffer() + totalDataReceived, MAX_BUFFER, &numDataReceived);
				totalDataReceived += numDataReceived;
				_lock.lock();
				responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
			} 
			while ( numDataReceived > 0 );
			responseString.convertFromByteBuffer(responseBuffer);
			wcout(responseString.GetString());
			itStr = responseString.GetString();
			itStr.Find(_T("STOP\r\n"));
			if ( itStr.IsBegin() )
				bExit = true;

			sBuffer.convertToByteBuffer(bBuffer);
			_lock.unlock();
			pClient->SendData(bBuffer.get_Buffer(), bBuffer.get_BufferSize(), &numBytesSend);
			pClient->ShutDownConnection();
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
			OK_NEW_OPERATOR CThreadCallback<CTestTCPServer>(this, &CTestTCPServer::ClientTask);

		_tcpclients.Append(_current);
		_current = NULL;
		//pCallback->addRef();
		_callback.Append(pCallback);

		_threadpool.AddTask(pCallback);
		return 0;
	}

private:
	CDataDoubleLinkedListT<CAbstractThreadCallback> _callback;
	CDataDoubleLinkedListT<CTcpClient> _tcpclients;
	CTcpClient* _current;
	CThreadPool _threadpool;
	volatile bool _bExit;
};

void TestTCPServer()
{
	CTestTCPServer server;
	CThreadEx serverTask(
		OK_NEW_OPERATOR CThreadCallback<CTestTCPServer>(&server, &CTestTCPServer::ServerTask), 
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
