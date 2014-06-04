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
#include "AsyncTCPServer.h"
#include "SyncTCPSocketClasses.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

#include "SyncTCPSocketClassesImpl.h"

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CTcpServerException(ARGS_FILE_LINE _T("in %s CTcpServerException"), 
		func, CWinException::WSAExtError);
}

CAsyncTCPServer::CAsyncTCPServer(Ptr(CAsyncIOManager) pManager):
    CAsyncIOBuffer(pManager),
	m_Server(),
	m_Protocol(),
	m_AcceptFunc(nullptr),
	m_ClientConnection(INVALID_SOCKET)
{
	CTcpClientImpl::Initialize();
}

CAsyncTCPServer::~CAsyncTCPServer(void)
{
	CTcpClientImpl::Deinitialize();
}

void CAsyncTCPServer::Open(CConstPointer pServer, CConstPointer pProtocol)
{
	SOCKET s = INVALID_SOCKET;
	ADDRINFOT *result = nullptr;
    ADDRINFOT hints;
    int iResult;

    s_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = GetAddrInfo(pServer, pProtocol, &hints, &result);
    if ( iResult != 0 )
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPServer::OpenConnection"));

	s = WSASocket(result->ai_family, result->ai_socktype, result->ai_protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if ( s == INVALID_SOCKET )
	{
        FreeAddrInfo(result);
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPServer::OpenConnection"));
	}

	iResult = bind(s, result->ai_addr, (int)result->ai_addrlen);
    if ( iResult == SOCKET_ERROR )
	{
        FreeAddrInfo(result);
		closesocket(s);
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPServer::OpenConnection"));
	}

	FreeAddrInfo(result);

	iResult = listen(s, SOMAXCONN);
    if ( iResult == SOCKET_ERROR )
	{
		closesocket(s);
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPServer::OpenConnection"));
	}

	m_Server.SetString(__FILE__LINE__ pServer);
	m_Protocol.SetString(__FILE__LINE__ pProtocol);

	m_pData->set_file((HANDLE)s);
	s_memset(m_pData->get_overlapped(), 0, sizeof(OVERLAPPED));
	m_pManager->Register(m_pData);
}

void CAsyncTCPServer::Close()
{
	closesocket((SOCKET)(m_pData->get_file()));
	if ( m_ClientConnection != INVALID_SOCKET )
		closesocket(m_ClientConnection);
}

void CAsyncTCPServer::Accept(Ref(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler)
{
	int nRet;

	if ( PtrCheck(m_AcceptFunc) )
	{
		GUID acceptex_guid = WSAID_ACCEPTEX;
		DWORD bytes = 0;

        nRet = WSAIoctl((SOCKET)(m_pData->get_file()), SIO_GET_EXTENSION_FUNCTION_POINTER,
			&acceptex_guid, sizeof(acceptex_guid),
			&m_AcceptFunc, sizeof(m_AcceptFunc), &bytes,
            nullptr, nullptr);
        if ( nRet == SOCKET_ERROR )
			ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPServer::Accept"));
	}
	m_ClientConnection = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, nullptr, 0, WSA_FLAG_OVERLAPPED); 
	if( m_ClientConnection == INVALID_SOCKET )
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPServer::Accept"));

	DWORD bufSize = buf.get_BufferSize();
	DWORD dwRecvNumBytes = 0;

	buf.set_BufferSize(__FILE__LINE__ bufSize + (2 * (sizeof(sockaddr_in) + 16)));
	m_pData->set_buffer(buf);
	m_pData->set_bytestransferred(0);
	m_pData->set_callback(pHandler);
	m_pData->set_ioop(CAsyncIOData::IOAccept);
	s_memset(m_pData->get_overlapped(), 0, sizeof(OVERLAPPED));

	LPFN_ACCEPTEX acceptex = (LPFN_ACCEPTEX) m_AcceptFunc;

	if ( !acceptex((SOCKET)(m_pData->get_file()), m_ClientConnection, 
		buf.get_Buffer(), bufSize, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 
		&dwRecvNumBytes, m_pData->get_overlapped()) )
	{
		if ( WSAGetLastError() != ERROR_IO_PENDING )
			ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPServer::Accept"));
	}
}

void CAsyncTCPServer::CreateClientConnection(Ptr(CAsyncTCPClient) result)
{
	SOCKET listenSocket = (SOCKET)(m_pData->get_file());
	int nRet = setsockopt(m_ClientConnection, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&listenSocket, sizeof(listenSocket));

	if( nRet == SOCKET_ERROR )
		ThrowDefaultException(__FILE__LINE__ _T("CAsyncTCPServer::CreateClientConnection"));

	dword bufsize = m_pData->get_buffer().get_BufferSize();

	if ( bufsize <= (2 * (sizeof(sockaddr_in) + 16)) )
	{
		CByteBuffer buf;

		result->GetData()->set_bytestransferred(0);
		result->GetData()->set_buffer(buf);
	}
	else
	{
		bufsize -= (2 * (sizeof(sockaddr_in) + 16));

		result->GetData()->set_bytestransferred(bufsize);
		result->GetData()->set_buffer(m_pData->get_buffer());
	}
	result->GetData()->set_ioop(CAsyncIOData::IORead);

	result->Open(m_ClientConnection);
	m_ClientConnection = INVALID_SOCKET;
}
