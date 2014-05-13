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

#include "DataVector.h"
#include "FilePath.h"
#include "okDateTime.h"
#include "WinSources.h"
#include "URL.h"
#include "SyncTCPSocketClasses.h"

class WINSOURCES_API CFTPClient: public CCppObject
{
public:
	CFTPClient(void);
	CFTPClient(ConstRef(CStringLiteral) user, ConstRef(CStringLiteral) passwd, ConstRef(CUrl) serverandroot, ConstRef(CFilePath) diskrootpath);
	virtual ~CFTPClient(void);

	void OpenConnection(ConstRef(CStringLiteral) user, ConstRef(CStringLiteral) passwd, ConstRef(CUrl) serverandroot, ConstRef(CFilePath) diskrootpath);
	void ChangeWorkingDir(ConstRef(CFilePath) _relativeunixpath, bool reset2root = true);
	void ChangeToParentDir();
	void RetrieveFileTime(ConstRef(CFilePath) _relativeunixpath, Ref(CDateTime) modTime);
	void RetrieveFile(ConstRef(CFilePath) _relativeunixpath, ConstRef(CDateTime) modTime);
	void CloseConnection();

	__inline bool IsConnected() const { return _tcpClient.IsConnected(); }
	__inline bool CanLog() const { return _canlog; }
	__inline void SetLog(bool v) { _canlog = v; }
	__inline word GetErrCnt() const { return _errcnt; }
	__inline void SetErrCnt(word errCnt) { _errcnt = errCnt; }
	__inline ConstRef(CDataVectorT<CStringBuffer>) GetLog() const { return _log; }
	void ResetLog();

	__inline ConstRef(CStringLiteral) GetUserName() const { return _username; }
	__inline ConstRef(CStringLiteral) GetPassword() const { return _password; }
	__inline ConstRef(CUrl) GetServerAndRoot() const { return _serverandroot; }
	__inline ConstRef(CFilePath) GetDiskRootPath() const { return _diskrootpath; }
	__inline ConstRef(CFilePath) GetUnixWorkingDir() const { return _unixworkingdir; }

protected:
	void SendData(const CStringBuffer& buffer);
	void ReceiveData(CStringBuffer& buffer, WLong& msgNo);
	void ReceiveFileData(ConstRef(CFilePath) _diskpath, ConstRef(CDateTime) modTime);
	void ReOpenConnection();
	void Log(ConstRef(CStringBuffer) text);

	CTcpClient _tcpClient;
	CTcpClient _tcpClient2;
	CStringLiteral _username;
	CStringLiteral _password;
	CUrl _serverandroot;
	CFilePath _diskrootpath;
	CFilePath _unixworkingdir;
	word _errcnt;
	bool _canlog;
	CDataVectorT<CStringBuffer> _log;
};

