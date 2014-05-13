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

#include "WinSources.h"
#include "WinException.h"

#ifdef OK_SYS_WINDOWS
#define SOCKET SOCKET
#endif
#ifdef OK_SYS_UNIX
#ifdef _WIN32
#define SOCKET SOCKET
#else
#define SOCKET int
#endif
#endif

class CTcpClientImpl;
class WINSOURCES_API CTcpClient : public CCppObject
{
public:
	CTcpClient(void);
	virtual ~CTcpClient(void);

	void OpenConnection(CConstPointer server, CConstPointer port);
	void SendData(BPointer data, dword dataLen, dword* numBytesSend);
	void ShutDownConnection();
	bool TestReceiveData(dword millisec);
	void ReceiveData(BPointer data, dword dataLen, dword* numBytesReceived, dword millisec = 0);
	void CloseConnection();

	bool IsConnected() const;

	__inline CTcpClientImpl* get_Impl() const { return _impl; }

private:
	CTcpClientImpl* _impl;
};

DECL_WINEXCEPTION(WINSOURCES_API, CTcpClientException, CWinException)

class CTcpServerImpl;
class WINSOURCES_API CTcpServer : public CCppObject
{
public:
	CTcpServer(void);
	virtual ~CTcpServer(void);

	void OpenConnection(CConstPointer server, CConstPointer port);
	bool TestAccept(dword millisec);
	void DoAccept(CTcpClient* client);
	void CloseConnection();

	bool IsConnected() const;

private:
	CTcpServerImpl *_impl;
};

DECL_WINEXCEPTION(WINSOURCES_API, CTcpServerException, CWinException)
