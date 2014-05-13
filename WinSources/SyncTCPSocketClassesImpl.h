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

class WINSOURCES_LOCAL CTcpClientImpl : public CCppObject
{
public:
	CTcpClientImpl(void);
	virtual ~CTcpClientImpl(void);

	void OpenConnection(CConstPointer server, CConstPointer port);
	void OpenConnection(SOCKET client);
	void SendData(BPointer data, dword dataLen, dword* numBytesSend);
	void ShutDownConnection();
	bool TestReceiveData(dword millisec);
	void ReceiveData(BPointer data, dword dataLen, dword* numBytesReceived, dword millisec = 0);
	void CloseConnection();

	__inline bool IsConnected() const { return (_ConnectSocket != INVALID_SOCKET); }

private:
	SOCKET _ConnectSocket;
#ifdef OK_SYS_WINDOWS
	static int _initialized;

public:
	static int Initialize();
	static int Deinitialize();
#endif
};

class CTcpClient;
class WINSOURCES_LOCAL CTcpServerImpl : public CCppObject
{
public:
	CTcpServerImpl(void);
	virtual ~CTcpServerImpl(void);

	void OpenConnection(CConstPointer server, CConstPointer port);
	bool TestAccept(dword millisec);
	void DoAccept(CTcpClient* client);
	void CloseConnection();

	__inline bool IsConnected() const { return (_ListenSocket != INVALID_SOCKET); }

private:
	SOCKET _ListenSocket;
};

