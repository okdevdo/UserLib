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
#include "AsyncTCPClient.h"
#include "SyncTCPSocketClasses.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include "SyncTCPSocketClassesImpl.h"

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CTcpClientException(ARGS_FILE_LINE _T("in %s CTcpClientException"),
		func, CWinException::WSAExtError);
}
static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func, long errCode)
{
	throw OK_NEW_OPERATOR CTcpClientException(ARGS_FILE_LINE _T("in %s CTcpClientException"),
		func, CWinException::WSAExtError, errCode);
}

CAsyncTCPClient::CAsyncTCPClient(Ptr(CAsyncIOManager) pManager):
    CAsyncIOBuffer(pManager),
	m_Server(),
	m_Protocol()
{
	CTcpClientImpl::Initialize();
}

CAsyncTCPClient::~CAsyncTCPClient(void)
{
	CTcpClientImpl::Deinitialize();
}

void CAsyncTCPClient::Open(CConstPointer pServer, CConstPointer pProtocol)
{
	SOCKET s;
	ADDRINFOT *result = NULL;
    ADDRINFOT *ptr = NULL;
    ADDRINFOT hints;
    int iResult;

    s_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	iResult = GetAddrInfo(pServer, pProtocol, &hints, &result);
    if ( iResult != 0 )
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPClient::Open"));

    // Attempt to connect to an address until one succeeds
    for( ptr = result; ptr != NULL; ptr = ptr->ai_next )
	{
        // Create a SOCKET for connecting to server
        s = WSASocket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
        if ( s == INVALID_SOCKET )
			ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPClient::Open"));
        // Connect to server.
        iResult = connect(s, ptr->ai_addr, (int)ptr->ai_addrlen);
        if ( iResult == SOCKET_ERROR )
		{
            closesocket(s);
            s = INVALID_SOCKET;
            continue;
        }
        break;
    }
	FreeAddrInfo(result);
    if (s == INVALID_SOCKET)
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPClient::Open"));
	m_Server.SetString(__FILE__LINE__ pServer);
	m_Protocol.SetString(__FILE__LINE__ pProtocol);

	m_pData->set_file((HANDLE)s);
	s_memset(m_pData->get_overlapped(), 0, sizeof(OVERLAPPED));
	m_pManager->Register(m_pData);
}

void CAsyncTCPClient::Open(SOCKET clientConnection)
{
	m_pData->set_file((HANDLE)clientConnection);
	s_memset(m_pData->get_overlapped(), 0, sizeof(OVERLAPPED));
	m_pManager->Register(m_pData);
}

void CAsyncTCPClient::Cancel()
{
	if (!CancelIoEx(m_pData->get_file(), m_pData->get_overlapped()))
	{
		DWORD result = GetLastError();

		if (result == ERROR_NOT_FOUND)
			return;
		throw OK_NEW_OPERATOR CTcpClientException(__FILE__LINE__ _T("in %s CTcpClientException"),
			_T("CAsyncTCPClient::Cancel"), CWinException::WinExtError);
	}
}

void CAsyncTCPClient::Close()
{
	if (SOCKET_ERROR == closesocket((SOCKET)(m_pData->get_file())))
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPClient::Close"));
}

void CAsyncTCPClient::Read(Ref(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler)
{
	WSABUF wsabuf;
	DWORD numbytesrcvd = 0;
	DWORD flags = 0;

	wsabuf.buf = CastAnyPtr(CHAR, buf.get_Buffer());
	wsabuf.len = buf.get_BufferSize();

	m_pData->set_buffer(buf);
	m_pData->set_bytestransferred(0);
	m_pData->set_callback(pHandler);
	m_pData->set_ioop(CAsyncIOData::IORead);
	if ( WSARecv((SOCKET)(m_pData->get_file()), &wsabuf, 1, &numbytesrcvd, &flags, m_pData->get_overlapped(), NULL) )
	{
		if (  WSAGetLastError() != WSA_IO_PENDING )
			ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPClient::Read"));
	}
	m_pManager->AddTask(m_pData);
}

void CAsyncTCPClient::Write(ConstRef(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler)
{
	WSABUF wsabuf;
	DWORD numbytessend = 0;
	DWORD flags = 0;

	wsabuf.buf = CastAnyPtr(CHAR, buf.get_Buffer());
	wsabuf.len = buf.get_BufferSize();

	m_pData->set_buffer(buf);
	m_pData->set_bytestransferred(0);
	m_pData->set_callback(pHandler);
	m_pData->set_ioop(CAsyncIOData::IOWrite);
	if ( WSASend((SOCKET)(m_pData->get_file()), &wsabuf, 1, &numbytessend, flags, m_pData->get_overlapped(), NULL) )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
			ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPClient::Write"));
	}
	m_pManager->AddTask(m_pData);
}
