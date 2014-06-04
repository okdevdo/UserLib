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
#include "SyncTCPSocketClasses.h"
#include "ScopedLock.h"

#ifdef OK_SYS_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef OK_COMP_MSC
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#endif
#endif
#ifdef OK_SYS_UNIX
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#undef OK_SYS_UNIX
#define OK_SYS_WINDOWS 1
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <netdb.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR	(-1)
#define closesocket close
#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR
#endif
#endif


IMPL_WINEXCEPTION(CTcpClientException, CWinException)
IMPL_WINEXCEPTION(CTcpServerException, CWinException)

#ifdef OK_SYS_WINDOWS
static void ThrowClientException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CTcpClientException(ARGS_FILE_LINE _T("in %s CTcpClientException"),
		func, CWinException::WSAExtError);
}
static void ThrowClientException(DECL_FILE_LINE CConstPointer func, long errCode)
{
	throw OK_NEW_OPERATOR CTcpClientException(ARGS_FILE_LINE _T("in %s CTcpClientException"),
		func, CWinException::WSAExtError, errCode);
}
#endif
#ifdef OK_SYS_UNIX
static void ThrowClientException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CTcpClientException(ARGS_FILE_LINE _T("in %s CTcpClientException"),
		func, CWinException::CRunTimeError);
}
static void ThrowClientException(DECL_FILE_LINE CConstPointer func, long errCode)
{
	throw OK_NEW_OPERATOR CTcpClientException(ARGS_FILE_LINE _T("in %s CTcpClientException"),
		func, CWinException::CRunTimeError, errCode);
}
#endif

#include "SyncTCPSocketClassesImpl.h"

#ifdef OK_SYS_WINDOWS
int CTcpClientImpl::_initialized = 0;
#endif

CTcpClientImpl::CTcpClientImpl(void) :
_ConnectSocket(INVALID_SOCKET)
{
#ifdef OK_SYS_WINDOWS
	Initialize();
#endif
}

CTcpClientImpl::~CTcpClientImpl(void)
{
#ifdef OK_SYS_WINDOWS
	Deinitialize();
#endif
}

#ifdef OK_SYS_WINDOWS
int CTcpClientImpl::Initialize()
{
	CScopedLock _lock;

	if (_initialized == 0)
	{
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		int err = WSAStartup(wVersionRequested, &wsaData);

		if (err != 0)
		{
			CStringBuffer msg;

			msg.FormatString(__FILE__LINE__ _T("WSAStartup failed with %d"), err);
			throw OK_NEW_OPERATOR CTcpClientException(__FILE__LINE__ msg);
		}
		if ((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2))
		{
			WSACleanup();

			CStringBuffer msg;

			msg.FormatString(__FILE__LINE__ _T("WSAStartup returned with version %x"), wsaData.wVersion);
			throw OK_NEW_OPERATOR CTcpClientException(__FILE__LINE__ msg);
		}
	}
	++_initialized;
	return _initialized;
}

int CTcpClientImpl::Deinitialize()
{
	CScopedLock _lock;

	if (_initialized == 0)
		return 0;
	--_initialized;
	if (_initialized == 0)
		WSACleanup();
	return _initialized;
}
#endif

void CTcpClientImpl::OpenConnection(CConstPointer server, CConstPointer port)
{
	if (_ConnectSocket != INVALID_SOCKET)
		CloseConnection();

#ifdef OK_COMP_GNUC
	addrinfo *result = nullptr;
	addrinfo *ptr = nullptr;
	addrinfo hints;
#endif
#ifdef OK_COMP_MSC
	ADDRINFOT *result = nullptr;
	ADDRINFOT *ptr = nullptr;
	ADDRINFOT hints;
#endif
	int iResult;

	s_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
#ifdef _UNICODE
	CStringBuffer tmp1(__FILE__LINE__ server);
	CStringBuffer tmp2(__FILE__LINE__ port);
	CByteBuffer server1;
	CByteBuffer port1;

	tmp1.convertToByteBuffer(server1);
	tmp2.convertToByteBuffer(port1);
	iResult = getaddrinfo(CastAnyPtr(char, server1.get_Buffer()), CastAnyPtr(char, port1.get_Buffer()), &hints, &result);
	if (iResult != 0)
		throw OK_NEW_OPERATOR CTcpClientException(__FILE__LINE__ _T("in %s CTcpClientException"),
		_T("CTcpClient::OpenConnection"), CWinException::WSAExtError);
#else
	iResult = getaddrinfo(server, port, &hints, &result);
	if (iResult != 0)
		throw OK_NEW_OPERATOR CTcpClientException(__FILE__LINE__ _T("in %s CTcpClientException"),
		_T("CTcpClient::OpenConnection"), CWinException::WSAExtError);
#endif
#endif
#ifdef OK_SYS_UNIX
	iResult = getaddrinfo(server, port, &hints, &result);
	if (iResult != 0)
		throw OK_NEW_OPERATOR CTcpClientException(__FILE__LINE__ _T("in %s CTcpClientException"),
		_T("CTcpClient::OpenConnection"), CWinException::GAIExtError, iResult);
#endif
#endif
#ifdef OK_COMP_MSC
	iResult = GetAddrInfo(server, port, &hints, &result);
	if (iResult != 0)
		throw OK_NEW_OPERATOR CTcpClientException(__FILE__LINE__ _T("in %s CTcpClientException"),
		_T("CTcpClient::OpenConnection"), CWinException::WSAExtError);
#endif

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		_ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (_ConnectSocket == INVALID_SOCKET)
			ThrowClientException(__FILE__LINE__ _T("OpenConnection"));
		// Connect to server.
		iResult = connect(_ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
#ifdef OK_SYS_WINDOWS
			iResult = WSAGetLastError();
#endif
#ifdef OK_SYS_UNIX
			iResult = errno;
#endif
			closesocket(_ConnectSocket);
			_ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

#ifdef OK_COMP_GNUC
	freeaddrinfo(result);
#endif
#ifdef OK_COMP_MSC
	FreeAddrInfo(result);
#endif

	if (_ConnectSocket == INVALID_SOCKET)
		ThrowClientException(__FILE__LINE__ _T("OpenConnection"), iResult);
}

void CTcpClientImpl::OpenConnection(SOCKET client)
{
	_ConnectSocket = client;
}

void CTcpClientImpl::SendData(BPointer data, dword dataLen, dword* numBytesSend)
{
	int iResult;

	if (numBytesSend)
		*numBytesSend = 0;
	if (_ConnectSocket == INVALID_SOCKET)
		return;
	iResult = send(_ConnectSocket, (char*)data, (int)dataLen, 0);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(_ConnectSocket);
		_ConnectSocket = INVALID_SOCKET;
		ThrowClientException(__FILE__LINE__ _T("SendData"));
	}
	if (numBytesSend)
		*numBytesSend = iResult;
}

void CTcpClientImpl::ShutDownConnection()
{
	if (_ConnectSocket == INVALID_SOCKET)
		return;

	int iResult = shutdown(_ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR)
		ThrowClientException(__FILE__LINE__ _T("ShutDownConnection"));
}

bool CTcpClientImpl::TestReceiveData(dword millisec)
{
	int iResult;
	fd_set rds;
	timeval tv;

	if (_ConnectSocket == INVALID_SOCKET)
		return false;
	FD_ZERO(&rds);
	FD_SET(_ConnectSocket, &rds);
	tv.tv_sec = millisec / 1000;
	tv.tv_usec = (millisec % 1000) * 1000;
	iResult = select(Castsdword(_ConnectSocket + 1), &rds, nullptr, nullptr, &tv);
	if (iResult == SOCKET_ERROR)
		ThrowClientException(__FILE__LINE__ _T("TestReceiveData"));
	return (iResult == 1);
}

void CTcpClientImpl::ReceiveData(BPointer data, dword dataLen, dword* numBytesReceived, dword millisec)
{
	int iResult;
	fd_set rds;
	timeval tv;

	if (numBytesReceived)
		*numBytesReceived = 0;
	if (_ConnectSocket == INVALID_SOCKET)
		return;
	if (millisec > 0)
	{
		FD_ZERO(&rds);
		FD_SET(_ConnectSocket, &rds);
		tv.tv_sec = millisec / 1000;
		tv.tv_usec = (millisec % 1000) * 1000;
		iResult = select(Castsdword(_ConnectSocket + 1), &rds, nullptr, nullptr, &tv);
		if (iResult == SOCKET_ERROR)
			ThrowClientException(__FILE__LINE__ _T("ReceiveData"));
		if (iResult == 0)
		{
#ifdef OK_SYS_WINDOWS
			ThrowClientException(__FILE__LINE__ _T("ReceiveData"), WSAETIMEDOUT);
#endif
#ifdef OK_SYS_UNIX
			ThrowClientException(__FILE__LINE__ _T("ReceiveData"), ETIMEDOUT);
#endif
		}
	}
	iResult = recv(_ConnectSocket, (char *)data, (int)dataLen, 0);
	if (iResult >= 0)
	{
		if (numBytesReceived)
			*numBytesReceived = iResult;
		return;
	}
	closesocket(_ConnectSocket);
	_ConnectSocket = INVALID_SOCKET;
	ThrowClientException(__FILE__LINE__ _T("ReceiveData"));
}

void CTcpClientImpl::CloseConnection()
{
	if (_ConnectSocket != INVALID_SOCKET)
	{
		closesocket(_ConnectSocket);
		_ConnectSocket = INVALID_SOCKET;
	}
}

CTcpClient::CTcpClient(void) :
_impl(OK_NEW_OPERATOR CTcpClientImpl)
{
}

CTcpClient::~CTcpClient(void)
{
	CloseConnection();
	if (_impl)
	{
		_impl->release();
		_impl = nullptr;
	}
}

void CTcpClient::OpenConnection(CConstPointer server, CConstPointer port)
{
	if (_impl)
		_impl->OpenConnection(server, port);
}

void CTcpClient::SendData(BPointer data, dword dataLen, dword* numBytesSend)
{
	if (_impl)
		_impl->SendData(data, dataLen, numBytesSend);
}

void CTcpClient::ShutDownConnection()
{
	if (_impl)
		_impl->ShutDownConnection();
}

bool CTcpClient::TestReceiveData(dword millisec)
{
	if (_impl)
		return _impl->TestReceiveData(millisec);
	return false;
}

void CTcpClient::ReceiveData(BPointer data, dword dataLen, dword* numBytesReceived, dword millisec)
{
	if (_impl)
		_impl->ReceiveData(data, dataLen, numBytesReceived, millisec);
}

void CTcpClient::CloseConnection()
{
	if (_impl)
		_impl->CloseConnection();
}

bool CTcpClient::IsConnected() const
{
	if (_impl)
		return _impl->IsConnected();
	return false;
}

#ifdef OK_SYS_WINDOWS
static void ThrowServerException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CTcpServerException(ARGS_FILE_LINE _T("in %s CTcpServerException"),
		func, CWinException::WSAExtError);
}
#endif
#ifdef OK_SYS_UNIX
static void ThrowServerException(DECL_FILE_LINE CConstPointer func)
{
	throw OK_NEW_OPERATOR CTcpServerException(ARGS_FILE_LINE _T("in %s CTcpServerException"),
		func, CWinException::CRunTimeError);
}
#endif

CTcpServerImpl::CTcpServerImpl(void) :
_ListenSocket(INVALID_SOCKET)
{
#ifdef OK_SYS_WINDOWS
	CTcpClientImpl::Initialize();
#endif
}

CTcpServerImpl::~CTcpServerImpl(void)
{
#ifdef OK_SYS_WINDOWS
	CTcpClientImpl::Deinitialize();
#endif
}

void CTcpServerImpl::OpenConnection(CConstPointer server, CConstPointer port)
{
	if (_ListenSocket != INVALID_SOCKET)
		CloseConnection();

#ifdef OK_COMP_GNUC
	addrinfo *result = nullptr;
	addrinfo hints;
#endif
#ifdef OK_COMP_MSC
	ADDRINFOT *result = nullptr;
	ADDRINFOT hints;
#endif
	int iResult;

	s_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
#ifdef _UNICODE
	CStringBuffer tmp1(__FILE__LINE__ server);
	CStringBuffer tmp2(__FILE__LINE__ port);
	CByteBuffer server1;
	CByteBuffer port1;

	tmp1.convertToByteBuffer(server1);
	tmp2.convertToByteBuffer(port1);
	iResult = getaddrinfo(CastAnyPtr(char, server1.get_Buffer()), CastAnyPtr(char, port1.get_Buffer()), &hints, &result);
	if (iResult != 0)
		throw OK_NEW_OPERATOR CTcpServerException(__FILE__LINE__ _T("in %s CTcpServerException"),
		_T("CTcpServerException::OpenConnection"), CWinException::WSAExtError);
#else
	iResult = getaddrinfo(server, port, &hints, &result);
	if (iResult != 0)
		throw OK_NEW_OPERATOR CTcpServerException(__FILE__LINE__ _T("in %s CTcpServerException"),
		_T("CTcpServerException::OpenConnection"), CWinException::WSAExtError);
#endif
#endif
#ifdef OK_SYS_UNIX
	iResult = getaddrinfo(server, port, &hints, &result);
	if (iResult != 0)
		throw OK_NEW_OPERATOR CTcpServerException(__FILE__LINE__ _T("in %s CTcpServerException"),
		_T("CTcpServerException::OpenConnection"), CWinException::GAIExtError, iResult);
#endif
#endif
#ifdef OK_COMP_MSC
	iResult = GetAddrInfo(server, port, &hints, &result);
	if (iResult != 0)
		throw OK_NEW_OPERATOR CTcpServerException(__FILE__LINE__ _T("in %s CTcpServerException"),
		_T("CTcpServerException::OpenConnection"), CWinException::WSAExtError);
#endif

	_ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_ListenSocket == INVALID_SOCKET)
	{
#ifdef OK_COMP_GNUC
		freeaddrinfo(result);
#endif
#ifdef OK_COMP_MSC
		FreeAddrInfo(result);
#endif
		ThrowServerException(__FILE__LINE__ _T("OpenConnection"));
	}

	iResult = bind(_ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
#ifdef OK_COMP_GNUC
		freeaddrinfo(result);
#endif
#ifdef OK_COMP_MSC
		FreeAddrInfo(result);
#endif
		closesocket(_ListenSocket);
		_ListenSocket = INVALID_SOCKET;
		ThrowServerException(__FILE__LINE__ _T("OpenConnection"));
	}

#ifdef OK_COMP_GNUC
	freeaddrinfo(result);
#endif
#ifdef OK_COMP_MSC
	FreeAddrInfo(result);
#endif

	iResult = listen(_ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(_ListenSocket);
		_ListenSocket = INVALID_SOCKET;
		ThrowServerException(__FILE__LINE__ _T("OpenConnection"));
	}
}

bool CTcpServerImpl::TestAccept(dword millisec)
{
	int iResult;
	fd_set rds;
	timeval tv;
	int cnt;
	int max;

	if (_ListenSocket == INVALID_SOCKET)
		return false;
	FD_ZERO(&rds);
	FD_SET(_ListenSocket, &rds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	iResult = select(Castsdword(_ListenSocket + 1), &rds, nullptr, nullptr, &tv);
	if (iResult == SOCKET_ERROR)
		ThrowServerException(__FILE__LINE__ _T("TestAccept"));
	cnt = 0;
	max = millisec / 100;
	while ((iResult == 0) && (cnt < max))
	{
		FD_ZERO(&rds);
		FD_SET(_ListenSocket, &rds);
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		iResult = select(Castsdword(_ListenSocket + 1), &rds, nullptr, nullptr, &tv);
		if (iResult == SOCKET_ERROR)
			ThrowServerException(__FILE__LINE__ _T("TestAccept"));
		++cnt;
	}
	return (iResult > 0);
}

void CTcpServerImpl::DoAccept(CTcpClient* client)
{
	if ((_ListenSocket == INVALID_SOCKET) || (PtrCheck(client->get_Impl())))
		return;

	SOCKET ClientSocket = accept(_ListenSocket, nullptr, nullptr);

	if (ClientSocket == INVALID_SOCKET)
	{
		closesocket(_ListenSocket);
		_ListenSocket = INVALID_SOCKET;
		ThrowServerException(__FILE__LINE__ _T("DoAccept"));
	}
	client->get_Impl()->OpenConnection(ClientSocket);
}

void CTcpServerImpl::CloseConnection()
{
	if (_ListenSocket != INVALID_SOCKET)
	{
		closesocket(_ListenSocket);
		_ListenSocket = INVALID_SOCKET;
	}
}

CTcpServer::CTcpServer(void) :
_impl(OK_NEW_OPERATOR CTcpServerImpl)
{
}

CTcpServer::~CTcpServer(void)
{
	CloseConnection();
	if (_impl)
	{
		_impl->release();
		_impl = nullptr;
	}
}

void CTcpServer::OpenConnection(CConstPointer server, CConstPointer port)
{
	if (_impl)
		_impl->OpenConnection(server, port);
}

bool CTcpServer::TestAccept(dword millisec)
{
	if (_impl)
		return _impl->TestAccept(millisec);
	return false;
}

void CTcpServer::DoAccept(CTcpClient* client)
{
	if (_impl)
		_impl->DoAccept(client);
}

void CTcpServer::CloseConnection()
{
	if (_impl)
		_impl->CloseConnection();
}

bool CTcpServer::IsConnected() const
{
	if (_impl)
		return _impl->IsConnected();
	return false;
}
