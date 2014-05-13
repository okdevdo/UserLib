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
#include "FTPClient.h"
#include "SecurityFile.h"
#include "DirectoryIterator.h"
#include "WinDirectoryIterator.h"

#ifdef OK_SYS_UNIX
#include <unistd.h>
#define Sleep(t) sleep(t / 1000)
#endif

#define MAX_BUFFER		512

CFTPClient::CFTPClient(void):
	_tcpClient(),
	_tcpClient2(),
	_username(),
	_password(),
	_serverandroot(),
	_diskrootpath(),
	_unixworkingdir(),
	_errcnt(0),
	_canlog(false),
	_log(__FILE__LINE__ 16, 16)
{
}

CFTPClient::CFTPClient(ConstRef(CStringLiteral) user, ConstRef(CStringLiteral) passwd, ConstRef(CUrl) serverandroot, ConstRef(CFilePath) diskrootpath):
	_tcpClient(),
	_tcpClient2(),
	_username(),
	_password(),
	_serverandroot(),
	_diskrootpath(),
	_unixworkingdir(),
	_errcnt(0),
	_canlog(false),
	_log(__FILE__LINE__ 16, 16)
{
	OpenConnection(user, passwd, serverandroot, diskrootpath);
}

CFTPClient::~CFTPClient(void)
{
	CloseConnection();
}

void CFTPClient::Log(ConstRef(CStringBuffer) text)
{
	if ( _canlog )
		_log.Append(text);
}

void CFTPClient::ResetLog()
{
	_log.Close();
	_log.Open(__FILE__LINE__ 16, 16);
}

void CFTPClient::OpenConnection(ConstRef(CStringLiteral) user, ConstRef(CStringLiteral) passwd, ConstRef(CUrl) serverandroot, ConstRef(CFilePath) diskrootpath)
{
	if ( IsConnected() )
		CloseConnection();

	_username = user;
	_password = passwd;
	_serverandroot = serverandroot;
	_diskrootpath = diskrootpath;
	_unixworkingdir.set_Path(_serverandroot.get_Resource(), CDirectoryIterator::UnixPathSeparatorString());

	if ( _serverandroot.get_Protocol().Compare(_T("ftp"), 0, CStringLiteral::cIgnoreCase) != 0 )
	{
		CStringBuffer tmp;

		++_errcnt;
		tmp.FormatString(__FILE__LINE__ _T("Protocol error: %s"), serverandroot.get_Url().GetString());
		_log.Append(tmp);
		return;
	}

	WLong msgNo;
	CStringBuffer sBuffer;
	CStringBuffer sBuffer0;
	CStringBuffer sBuffer1;
	CStringBuffer sBuffer2;
	int cnt = 10;

	sBuffer0.FormatString(__FILE__LINE__ _T("USER %s\r\n"), _username.GetString());
	sBuffer1.FormatString(__FILE__LINE__ _T("PASS %s\r\n"), _password.GetString());
	sBuffer2.FormatString(__FILE__LINE__ _T("CWD %s\r\n"), _unixworkingdir.get_Path().GetString());

	while ( !(_tcpClient.IsConnected()) && (cnt-- > 0) )
	{
		if ( cnt == 5 )
			Sleep(5000);
		try
		{
			_tcpClient.OpenConnection(_serverandroot.get_Server(), _T("ftp"));
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			SendData(sBuffer0);
			Log(sBuffer0);
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			SendData(sBuffer1);
			Log(sBuffer1);
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			SendData(sBuffer2);
			Log(sBuffer2);
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			if ( (msgNo >= 500) && (msgNo <= 599) )
			{
				CStringBuffer tmp;

				_tcpClient.CloseConnection();
				++_errcnt;
				tmp.FormatString(__FILE__LINE__ _T("Server Response: %ld"), msgNo);
				_log.Append(tmp);
				return;
			}
		}
		catch ( CTcpClientException* ex )
		{
			_tcpClient.CloseConnection();
			++_errcnt;
			_log.Append(ex->GetExceptionMessage());
		}
	}
}

void CFTPClient::ChangeWorkingDir(ConstRef(CFilePath) _relativeunixpath, bool reset2root)
{
	if ( !(_tcpClient.IsConnected()) )
		return;
	if ( _relativeunixpath.is_Absolute() )
	{
		CStringBuffer tmp;

		++_errcnt;
		tmp.FormatString(__FILE__LINE__ _T("ChangeWorkingDir, Path error: %s"), _relativeunixpath.get_Path().GetString());
		_log.Append(tmp);
		return;
	}
	if ( reset2root )
		_unixworkingdir.set_Path(_serverandroot.get_Resource(), CDirectoryIterator::UnixPathSeparatorString());
	_unixworkingdir.set_Filename(_relativeunixpath.get_Path());

	WLong msgNo;
	CStringBuffer sBuffer;
	CStringBuffer sBuffer0;
	bool bTryReopen = true;

	sBuffer0.FormatString(__FILE__LINE__ _T("CWD %s\r\n"), _unixworkingdir.get_Path().GetString());
	try
	{
		SendData(sBuffer0);
		Log(sBuffer0);
		ReceiveData(sBuffer, msgNo);
		Log(sBuffer);

		if ( (msgNo >= 500) && (msgNo <= 599) )
		{
			CStringBuffer tmp;

			_tcpClient.CloseConnection();
			++_errcnt;
			tmp.FormatString(__FILE__LINE__ _T("Server Response: %ld"), msgNo);
			_log.Append(tmp);
			return;
		}
		bTryReopen = false;
	}
	catch ( CTcpClientException* ex )
	{
		_tcpClient.CloseConnection();
		++_errcnt;
		_log.Append(ex->GetExceptionMessage());
	}
	if ( bTryReopen )
		ReOpenConnection(); // does CWD
}

void CFTPClient::ChangeToParentDir()
{
	if ( !(_tcpClient.IsConnected()) )
		return;
	if ( _unixworkingdir.get_Path().Compare(_serverandroot.get_Resource(), 0, CStringLiteral::cIgnoreCase) == 0 )
	{
		CStringBuffer tmp;

		++_errcnt;
		tmp.FormatString(__FILE__LINE__ _T("ChangeToParentDir, Path error: %s"), _unixworkingdir.get_Path().GetString());
		_log.Append(tmp);
		return;
	}

	WLong msgNo;
	CStringBuffer sBuffer;
	CStringBuffer sBuffer0;
	bool bTryReopen = true;

	_unixworkingdir.set_Directory(_T(""), -1);
	sBuffer0.FormatString(__FILE__LINE__ _T("CWD %s\r\n"), _unixworkingdir.get_Path().GetString());
	try
	{
		SendData(sBuffer0);
		Log(sBuffer0);
		ReceiveData(sBuffer, msgNo);
		Log(sBuffer);
		bTryReopen = false;
	}
	catch ( CTcpClientException* ex )
	{
		_tcpClient.CloseConnection();
		++_errcnt;
		_log.Append(ex->GetExceptionMessage());
	}
	if ( bTryReopen )
		ReOpenConnection(); // does CWD
}

void CFTPClient::RetrieveFileTime(ConstRef(CFilePath) _relativeunixpath, Ref(CDateTime) modTime)
{
	if ( !(_tcpClient.IsConnected()) )
		return;

	WLong msgNo;
	CStringBuffer sBuffer;
	CStringBuffer sBuffer0;
	bool bOk = true;
	bool bFirst = true;
	int cnt = 10;

	sBuffer0.FormatString(__FILE__LINE__ _T("MDTM %s\r\n"), _relativeunixpath.get_Path().GetString());
	modTime.SetTime();
	while ( bOk && (cnt-- > 0) )
	{
		if ( cnt == 5 )
			Sleep(5000);
		try
		{
			if ( bFirst )
				bFirst = false;
			else
				ReOpenConnection(); // does CWD
			SendData(sBuffer0);
			Log(sBuffer0);
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			if ( (msgNo >= 500) && (msgNo <= 599) )
			{
				CStringBuffer tmp;

				_tcpClient.CloseConnection();
				++_errcnt;
				tmp.FormatString(__FILE__LINE__ _T("Server Response: %ld"), msgNo);
				_log.Append(tmp);
				return;
			}

			bOk = false;
		}
		catch ( CTcpClientException* ex )
		{
			_tcpClient.CloseConnection();
			++_errcnt;
			_log.Append(ex->GetExceptionMessage());
		}
	}
	if ( !(sBuffer.IsEmpty()) )
	{
		CStringConstIterator it(sBuffer.GetString());
		word year, month, day, hour, minute, second;
		WLong value;

		it += 4;
		it.EatDecimal(value, 4);
		year = Cast(word, value);
		it.EatDecimal(value, 2);
		month = Cast(word, value);
		it.EatDecimal(value, 2);
		day = Cast(word, value);
		it.EatDecimal(value, 2);
		hour = Cast(word, value);
		it.EatDecimal(value, 2);
		minute = Cast(word, value);
		it.EatDecimal(value, 2);
		second = Cast(word, value);

		modTime.SetTime(year, month, day, hour, minute, second);
	}
}

void CFTPClient::RetrieveFile(ConstRef(CFilePath) _relativeunixpath, ConstRef(CDateTime) modTime)
{
	if ( !(_tcpClient.IsConnected()) )
		return;
	if ( _relativeunixpath.is_Absolute() )
	{
		CStringBuffer tmp;

		++_errcnt;
		tmp.FormatString(__FILE__LINE__ _T("RetrieveFile, Path error: %s"), _relativeunixpath.get_Path().GetString());
		_log.Append(tmp);
		return;
	}

	WLong msgNo;
	CStringBuffer sBuffer;
	CStringLiteral sBuffer1(_T("TYPE I\r\n"));
	CStringLiteral sBuffer2(_T("PASV\r\n"));
	CStringBuffer sBuffer3;
	CFilePath fpath;
	bool bFirst = true;
	bool bOk = true;
	int cnt = 10;

	sBuffer3.FormatString(__FILE__LINE__ _T("RETR %s\r\n"), _relativeunixpath.get_Path().GetString());

	sBuffer = _unixworkingdir.get_Path();
	sBuffer.DeleteString(0, _serverandroot.get_Resource().GetLength());
	sBuffer.AppendString(_relativeunixpath.get_Path());
	fpath.set_Path(sBuffer, CDirectoryIterator::UnixPathSeparatorString());
	fpath.Normalize(_diskrootpath.get_Path().GetString());

	while ( bOk && (cnt-- > 0) )
	{
		if ( cnt == 5 )
			Sleep(5000);
		try
		{
			if ( bFirst )
				bFirst = false;
			else
				ReOpenConnection();

			SendData(CStringBuffer(__FILE__LINE__ sBuffer1));
			Log(CStringBuffer(__FILE__LINE__ sBuffer1));
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			SendData(CStringBuffer(__FILE__LINE__ sBuffer2));
			Log(CStringBuffer(__FILE__LINE__ sBuffer2));
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			if ( msgNo == 227 )
			{
				CStringConstIterator it = sBuffer.GetString();

				it += 3;
				it.FirstOf(_T("0123456789"));
				if ( !it.IsEnd() )
				{
					CStringBuffer ip;
					CStringBuffer port;
					WLong ip0, ip1, ip2, ip3, port0, port1;
					WChar c;

					it.EatDecimal(ip0);
					it.EatCharacter(c);
					it.EatDecimal(ip1);
					it.EatCharacter(c);
					it.EatDecimal(ip2);
					it.EatCharacter(c);
					it.EatDecimal(ip3);
					it.EatCharacter(c);
					it.EatDecimal(port0);
					it.EatCharacter(c);
					it.EatDecimal(port1);

					ip.FormatString(__FILE__LINE__ _T("%d.%d.%d.%d"), ip0, ip1, ip2, ip3);
					port.FormatString(__FILE__LINE__ _T("%d"), (port0 * 256) + port1);

					_tcpClient2.OpenConnection(ip, port);
				}
			}

			if ( _tcpClient2.IsConnected() )
			{
				SendData(sBuffer3);
				Log(sBuffer3);
				ReceiveData(sBuffer, msgNo);
				Log(sBuffer);

				ReceiveFileData(fpath, modTime);
				_tcpClient2.CloseConnection();

				ReceiveData(sBuffer, msgNo);
				Log(sBuffer);
				bOk = (!(CWinDirectoryIterator::FileExists(fpath)));
			}
			else
			{
				CStringBuffer tmp;

				_tcpClient.CloseConnection();
				++_errcnt;
				tmp.FormatString(__FILE__LINE__ _T("Server Response: %ld"), msgNo);
				_log.Append(tmp);
				return;
			}
		}
		catch ( CTcpClientException* ex )
		{
			_tcpClient.CloseConnection();
			_tcpClient2.CloseConnection();
			++_errcnt;
			_log.Append(ex->GetExceptionMessage());
		}
		catch ( CSecurityFileException* ex )
		{
			_tcpClient.CloseConnection();
			_tcpClient2.CloseConnection();
			bOk = false;
			++_errcnt;
			_log.Append(ex->GetExceptionMessage());
		}
	}
}

void CFTPClient::CloseConnection()
{
	if ( !(_tcpClient.IsConnected()) )
		return;

	WLong msgNo;
	CStringBuffer sBuffer;
	CStringLiteral sBuffer0(_T("QUIT\r\n"));

	try
	{
		SendData(CStringBuffer(__FILE__LINE__ sBuffer0));
		Log(CStringBuffer(__FILE__LINE__ sBuffer0));
		ReceiveData(sBuffer, msgNo);
		Log(sBuffer);

		_tcpClient.CloseConnection();
	}
	catch ( CTcpClientException* ex )
	{
		_tcpClient.CloseConnection();
		++_errcnt;
		_log.Append(ex->GetExceptionMessage());
	}
}

void CFTPClient::SendData(const CStringBuffer& buffer)
{
	if ( !(_tcpClient.IsConnected()) )
		return;

	CByteBuffer bBuffer;
	dword numBytesSend;

	buffer.convertToByteBuffer(bBuffer);
	_tcpClient.SendData(bBuffer.get_Buffer(), bBuffer.get_BufferSize(), &numBytesSend);
	assert(numBytesSend == bBuffer.get_BufferSize());
}

void CFTPClient::ReceiveData(CStringBuffer& buffer, WLong& msgNo)
{
	if ( !(_tcpClient.IsConnected()) )
	{
		buffer.Clear();
		msgNo = 0;
		return;
	}

	CStringConstIterator it;
	CByteBuffer responseBuffer;
	dword numDataReceived = 0;
	dword totalDataReceived = 0;
	bool bContinuationLine = false;
	WLong cmpNo;

	for ( ;; )
	{
		responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived + MAX_BUFFER);
		_tcpClient.ReceiveData(responseBuffer.get_Buffer() + totalDataReceived, MAX_BUFFER, &numDataReceived, 3000);
		totalDataReceived += numDataReceived;
		responseBuffer.set_BufferSize(__FILE__LINE__ totalDataReceived);
		buffer.convertFromByteBuffer(responseBuffer);
		if ( buffer.IsEmpty() )
		{
			msgNo = 0;
			return;
		}
		it = buffer.GetString();
		it.EatDecimal(msgNo);
		bContinuationLine = (it[0] == _T('-'));
		it.Find(_T("\r\n"));
		if ( (!(it.IsEnd())) && (!bContinuationLine) )
			return;
		while ( !(it.IsEnd()) && bContinuationLine )
		{
			it += 2;
			it.EatDecimal(cmpNo);
			bContinuationLine = (it[0] == _T('-'));
			it.Find(_T("\r\n"));
			if ( (!(it.IsEnd())) && (!bContinuationLine) )
				return;
		}
	}
}

void CFTPClient::ReceiveFileData(ConstRef(CFilePath) _diskpath, ConstRef(CDateTime) modTime)
{
	if ( !(_tcpClient2.IsConnected()) )
		return;

	CByteBuffer responseBuffer;
	dword numDataReceived = 0;
	CSecurityFile file;
	bool bOk = true;

	try
	{
		if ( CWinDirectoryIterator::FileExists(_diskpath) )
			CWinDirectoryIterator::UnlinkFile(_diskpath);
		else
		{
			CFilePath dirPath(_diskpath);

			dirPath.set_Filename(_T(""));
			CWinDirectoryIterator::MakeDirectory(dirPath);
		}
		file.Create(_diskpath);
		for ( ;; )
		{
			responseBuffer.set_BufferSize(__FILE__LINE__ MAX_BUFFER);
			_tcpClient2.ReceiveData(responseBuffer.get_Buffer(), MAX_BUFFER, &numDataReceived, 3000);
			if ( 0 == numDataReceived )
				break;
			responseBuffer.set_BufferSize(__FILE__LINE__ numDataReceived);
			file.Write(responseBuffer);
		}
		file.Close();
		CWinDirectoryIterator::WriteFileTimes(_diskpath, modTime, modTime, modTime);
	}
	catch ( CTcpClientException* ex )
	{
		++_errcnt;
		_log.Append(ex->GetExceptionMessage());
		bOk = false;
	}
	catch ( CSecurityFileException* ex )
	{
		++_errcnt;
		_log.Append(ex->GetExceptionMessage());
		bOk = false;
	}
	if ( !bOk )
	{
		try
		{
			_tcpClient2.CloseConnection();
			file.Close();
			CWinDirectoryIterator::UnlinkFile(_diskpath);
		}
		catch ( CSecurityFileException* ex )
		{
			++_errcnt;
			_log.Append(ex->GetExceptionMessage());
		}
	}
}

void CFTPClient::ReOpenConnection()
{
	WLong msgNo;
	CStringBuffer sBuffer;
	CStringBuffer sBuffer0;
	CStringBuffer sBuffer1;
	CStringBuffer sBuffer2;
	int cnt = 10;

	sBuffer0.FormatString(__FILE__LINE__ _T("USER %s\r\n"), _username.GetString());
	sBuffer1.FormatString(__FILE__LINE__ _T("PASS %s\r\n"), _password.GetString());
	sBuffer2.FormatString(__FILE__LINE__ _T("CWD %s\r\n"), _unixworkingdir.get_Path().GetString());

	while ( !(_tcpClient.IsConnected()) && (cnt-- > 0) )
	{
		if ( cnt == 5 )
			Sleep(5000);
		try
		{
			_tcpClient.OpenConnection(_serverandroot.get_Server(), _T("ftp"));
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			SendData(sBuffer0);
			Log(sBuffer0);
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			SendData(sBuffer1);
			Log(sBuffer1);
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			SendData(sBuffer2);
			Log(sBuffer2);
			ReceiveData(sBuffer, msgNo);
			Log(sBuffer);

			if ( (msgNo >= 500) && (msgNo <= 599) )
			{
				CStringBuffer tmp;

				_tcpClient.CloseConnection();
				++_errcnt;
				tmp.FormatString(__FILE__LINE__ _T("Server Response: %ld"), msgNo);
				_log.Append(tmp);
				return;
			}
		}
		catch ( CTcpClientException* ex )
		{
			_tcpClient.CloseConnection();
			++_errcnt;
			_log.Append(ex->GetExceptionMessage());
		}
	}
}
