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
#include "Debug.h"
#include "Thread.h"
#include "SyncTCPSocketClasses.h"
#include "ScopedLock.h"

#include <iostream>
#ifdef _UNICODE
#define COUT std::wcout
#define CERR std::wcerr
#define OSTREAM std::wostream
#else
#define COUT std::cout
#define CERR std::cerr
#define OSTREAM std::ostream
#endif
using std::endl;

#define DEFAULT_PORT _T("27017")
#define MAX_BUFFER		512

static void __stdcall DebugServerPrivateDeleteFunc( ConstPointer data, Pointer context )
{
	CTcpClient* pClient = CastAnyPtr(CTcpClient, CastMutable(Pointer, data));

	pClient->release();
}

static void __stdcall AbstractThreadCallbackDeleteFunc( ConstPointer data, Pointer context )
{
	CAbstractThreadCallback* pCallback = CastAnyPtr(CAbstractThreadCallback, CastMutable(Pointer, data));

	pCallback->release();
}

static void __stdcall EmptyDeleteFunc( ConstPointer data, Pointer context )
{
}

class CDebugServerPrivate: public CTcpServer
{
public:
	CDebugServerPrivate(): 
		_callback(__FILE__LINE__0),
		_tcpclients(__FILE__LINE__0),
		_threadpool(__FILE__LINE__ 5, 5, 0, CThreadPool::QuickResponse), 
		_bExit(false)
	{
	}
	~CDebugServerPrivate() 
	{ 
		CScopedLock _lock;

		_tcpclients.Close();
		_callback.Close();
	}

	void SendData(CTcpClient* tcpClient, const CStringBuffer& buffer)
	{
		CByteBuffer bBuffer;
		dword numBytesSend;

		buffer.convertToByteBuffer(bBuffer);
		tcpClient->SendData(bBuffer.get_Buffer(), bBuffer.get_BufferSize(), &numBytesSend);
		assert(numBytesSend == bBuffer.get_BufferSize());
	}

	void ReceiveData(CTcpClient* tcpClient, CStringBuffer& buffer)
	{
		CStringConstIterator it;
		CByteBuffer responseBuffer;
		BPointer p;
		dword numDataReceived = 0;
		dword totalDataReceived = 0;
		dword cntDataReceived = 0;

		for ( ;; )
		{
			responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
			p = responseBuffer.get_Buffer() + totalDataReceived;
			cntDataReceived = 0;
			do
			{
				tcpClient->ReceiveData(p, 1, &numDataReceived);
				totalDataReceived += numDataReceived;
				cntDataReceived += numDataReceived;
				if ( *p == '\n' )
					break;
				p += numDataReceived;
			}
			while ( (cntDataReceived < MAX_BUFFER) && (numDataReceived > 0) );
			if ( (*p == '\n') || (numDataReceived == 0) )
			{
				responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
				buffer.convertFromByteBuffer(responseBuffer);
				if ( buffer.IsEmpty() || ((*p != '\n') && (numDataReceived == 0)) )
					break;
				it = buffer;
				it.Find(_T("\r\n"));
				if ( !(it.IsEnd()) )
				{
					buffer.DeleteString(Castdword(it.GetCurrent() - it.GetOrigin()), 2);
					break;
				}
			}
		}
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
		CStringConstIterator itB;

		_lock.unlock();
		try
		{
			for ( ;; )
			{
				if ( pClient->TestReceiveData(100) )
				{
					ReceiveData(pClient, sBuffer);
					itB = sBuffer;
					itB.Find(_T("@stopclient"));
					if ( itB.IsBegin() )
						break;
					itB = sBuffer;
					itB.Find(_T("@stopserver"));
					if ( itB.IsBegin() )
					{
						_lock.lock();
						_bExit = true;
						COUT << _T("Stopping server ...") << endl;
						_lock.unlock();
						continue;
					}
					itB = sBuffer;
					itB.Find(_T("@keepalive"));
					if ( itB.IsBegin() )
						continue;
					_lock.lock();
					COUT << sBuffer << endl;
					_lock.unlock();
				}
			}

			pClient->CloseConnection();
		}
		catch ( CTcpClientException* ex )
		{
			CERR << ex->GetExceptionMessage() << endl;
			pClient->CloseConnection();
		}
		_lock.lock();
		pClient->release();
		return 0;
	}

	dword ServerTask()
	{
		CScopedLock _lock;
		Ptr(CTcpClient) _current = NULL;

		_lock.unlock();

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
			OK_NEW_OPERATOR CThreadCallback<CDebugServerPrivate>(this, &CDebugServerPrivate::ClientTask);

		_tcpclients.Append(_current);
		_current = NULL;
		pCallback->addRef();
		_callback.Append(pCallback);
		_threadpool.AddTask(pCallback);
		return 0;
	}

private:
	typedef CDataDoubleLinkedListT<CTcpClient, CCppObjectLessFunctor<CTcpClient>, CCppObjectNullFunctor<CTcpClient> > tcpclients_t;

	CDataDoubleLinkedListT<CAbstractThreadCallback> _callback;
	tcpclients_t _tcpclients;
	CThreadPool _threadpool;
	volatile bool _bExit;
};

CDebugServer::CDebugServer(void)
{
}

CDebugServer::~CDebugServer(void)
{
}

void CDebugServer::RunServer(void)
{
	_d = OK_NEW_OPERATOR CDebugServerPrivate();

	CThreadEx serverTask(
		OK_NEW_OPERATOR CThreadCallback<CDebugServerPrivate>(_d, &CDebugServerPrivate::ServerTask), 
		false);

	try
	{
		_d->OpenConnection(NULL, DEFAULT_PORT);
		serverTask.Start();
		serverTask.Join();
		_d->CloseConnection();
	}
	catch ( CTcpServerException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
		_d->CloseConnection();
	}
	CThread::StopAll();
	CThread::JoinAll();

	_d->release();
}

Ptr(CDebugClient) CDebugClient::_instance = NULL;

CDebugClient::CDebugClient(void):
	_msgs(__FILE__LINE__ 16, 16)
{
	_tcpClient = OK_NEW_OPERATOR CTcpClient();
	_thread = OK_NEW_OPERATOR CThreadEx(OK_NEW_OPERATOR CThreadCallback<CDebugClient>(this, &CDebugClient::ThreadProc), false);
}

CDebugClient::~CDebugClient(void)
{
	_tcpClient->release();
	_thread->release();
}

void CDebugClient::CreateInstance()
{
	if ( PtrCheck(_instance) )
		_instance = OK_NEW_OPERATOR CDebugClient();
	_instance->Start();
}

void CDebugClient::FreeInstance()
{
	_instance->Stop();
	if ( NotPtrCheck(_instance) )
		_instance->release();
	_instance = NULL;
}

Ptr(CDebugClient) CDebugClient::Instance()
{
	return _instance;
}

void CDebugClient::SendDebugMessage(ConstRef(CStringBuffer) msg)
{
	CScopedLock _lock;

	_msgs.Append(msg);
}

dword CDebugClient::ThreadProc(void)
{
	CScopedLock _lock;
	CStringBuffer tmp;
	CByteBuffer buf;
	dword cnt;

	if ( _msgs.Count() == 0 )
	{
		tmp.SetString(__FILE__LINE__ _T("@keepalive\r\n"));
		_lock.unlock();
		Sleep(1000);
		_lock.lock();
	}
	else
	{
		CDataVectorT<CStringBuffer>::Iterator it = _msgs.Begin();

		tmp = *it;
		_msgs.Remove(it);
	}
	tmp.convertToByteBuffer(buf);

	if ( _tcpClient->IsConnected() )
	{
		_tcpClient->SendData(buf.get_Buffer(), buf.get_BufferSize(), &cnt);
		assert(cnt == buf.get_BufferSize());
	}

	return 0;
}

void CDebugClient::Start(void)
{
	try
	{
		if ( !(_tcpClient->IsConnected()) )
			_tcpClient->OpenConnection(_T("localhost"), DEFAULT_PORT);
	}
	catch ( CTcpClientException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
	if ( _tcpClient->IsConnected() )
		_thread->Start();
}

void CDebugClient::Stop(void)
{
	CThread::StopAll();
	CThread::JoinAll();

	if ( _tcpClient->IsConnected() )
	{
		CStringBuffer tmp;
		CByteBuffer buf;
		dword cnt;

		tmp.SetString(__FILE__LINE__ _T("@stopclient\r\n"));
		tmp.convertToByteBuffer(buf);
		_tcpClient->SendData(buf.get_Buffer(), buf.get_BufferSize(), &cnt);
		_tcpClient->CloseConnection();
	}
}

CDebug::CDebug(void):
_buffer()
{
}

CDebug::~CDebug(void)
{
}

Ref(CDebug) CDebug::Append(byte val)
{
	mbchar buf[33];

	s_ltoa(val, buf, 16);
	_buffer.AppendString(_T("0x"));
	_buffer.AppendString(buf);
	return *this;
}

Ref(CDebug) CDebug::Append(sbyte val)
{
	mbchar buf[33];

	s_ltoa(val, buf, 10);
	_buffer.AppendString(buf);
	return *this;
}

#ifdef _UNICODE
Ref(CDebug) CDebug::Append(mbchar val)
{
	mbchar buf[2];

	buf[0] = val;
	buf[1] = 0;
	_buffer.AppendString(buf);
	return *this;
}
#endif

Ref(CDebug) CDebug::Append(word val)
{
	mbchar buf[33];

	s_ltoa(val, buf, 16);
	_buffer.AppendString(_T("0x"));
	_buffer.AppendString(buf);
	return *this;
}

Ref(CDebug) CDebug::Append(sword val)
{
	mbchar buf[33];

	s_ltoa(val, buf, 10);
	_buffer.AppendString(buf);
	return *this;
}

Ref(CDebug) CDebug::Append(dword val)
{
	mbchar buf[33];

	s_ltoa(val, buf, 16);
	_buffer.AppendString(_T("0x"));
	_buffer.AppendString(buf);
	return *this;
}

Ref(CDebug) CDebug::Append(sdword val)
{
	mbchar buf[33];

	s_ltoa(val, buf, 10);
	_buffer.AppendString(buf);
	return *this;
}

Ref(CDebug) CDebug::Append(qword val)
{
	mbchar buf[33];

	s_lltoa(val, buf, 16);
	_buffer.AppendString(_T("0x"));
	_buffer.AppendString(buf);
	return *this;
}

Ref(CDebug) CDebug::Append(sqword val)
{
	mbchar buf[33];

	s_lltoa(val, buf, 10);
	_buffer.AppendString(buf);
	return *this;
}

Ref(CDebug) CDebug::Append(CConstPointer val)
{
	_buffer.AppendString(val);
	return *this;
}

Ref(CDebug) CDebug::Append(ConstRef(CStringLiteral) val)
{
	_buffer.AppendString(val);
	return *this;
}

Ref(CDebug) CDebug::Append(ConstRef(CStringBuffer) val)
{
	_buffer.AppendString(val);
	return *this;
}

void CDebug::SendDebugMessage()
{
	_buffer.AppendString(_T("\r\n"));
	if ( NotPtrCheck(CDebugClient::Instance()) )
	{
		CDebugClient::Instance()->SendDebugMessage(_buffer);
		Sleep(2000);
	}
	_buffer.Clear();
}
