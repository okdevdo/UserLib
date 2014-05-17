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
#include "SecurityFile.h"
#include "SecurityContext.h"

#ifdef OK_SYS_UNIX
	#include <stdio.h>
    #include <io.h>
    #include <fcntl.h>
	#include <unistd.h>
	#include <utime.h>
	#include <errno.h>
#endif

IMPL_WINEXCEPTION(CSecurityFileException, CWinException)

CSecurityFile::CSecurityFile(void):
    CFile(), m_fileHandle(INVALID_HANDLE_VALUE)
{
}

CSecurityFile::CSecurityFile(ConstRef(CFilePath) _path, WBool _readOnly, WBool _textMode, TEncoding _encoding) :
    CFile(_path, _readOnly), m_fileHandle(INVALID_HANDLE_VALUE)
{
	Open(_path, _readOnly, _textMode, _encoding);
}

CSecurityFile::CSecurityFile(ConstRef(CSecurityFile) _file):
    CFile(_file), m_fileHandle(INVALID_HANDLE_VALUE)
{
	if ( _file.m_fileHandle == INVALID_HANDLE_VALUE )
		return;
#ifdef OK_SYS_WINDOWS
	if (!DuplicateHandle(GetCurrentProcess(), _file.m_fileHandle, GetCurrentProcess(), &m_fileHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CSecurityFile::CopyContructor"), m_path.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	m_fileHandle = dup(_file.m_fileHandle);
	if ( 0 > m_fileHandle )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CSecurityFile::CopyContructor"), m_path.GetString(), CWinException::CRunTimeError);
#endif
}

CSecurityFile::~CSecurityFile(void)
{
}

void CSecurityFile::operator = (ConstRef(CSecurityFile) _file)
{
	if ( _file.m_fileHandle == INVALID_HANDLE_VALUE )
		return;
	CFile::operator=(_file);
	Close();
#ifdef OK_SYS_WINDOWS
	if (!DuplicateHandle(GetCurrentProcess(), _file.m_fileHandle, GetCurrentProcess(), &m_fileHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CSecurityFile::operator="), m_path.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	m_fileHandle = dup(_file.m_fileHandle);
	if ( 0 > m_fileHandle )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CSecurityFile::operator="), m_path.GetString(), CWinException::CRunTimeError);
#endif
}

void CSecurityFile::Open(ConstRef(CFilePath) _path, WBool _readOnly, WBool _textMode, TEncoding _encoding)
{
	Close();
	CFile::Open(_path, _readOnly, _textMode, _encoding);

#ifdef OK_SYS_WINDOWS
	DWORD dwDesiredAccess = GENERIC_READ;
	SECURITY_ATTRIBUTES sa;
	CStringBuffer tmp(_path.get_Path());

	if ( !_readOnly )
		dwDesiredAccess |= GENERIC_WRITE;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

#ifdef _UNICODE
	if ( tmp.GetLength() > MAX_PATH )
		tmp.PrependString(_T("\\\\?\\"));
#else
	if ( tmp.GetLength() > MAX_PATH )
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if ( ((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH) )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
			    _T("CSecurityFile::Open"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	m_fileHandle = CreateFile(
		tmp.GetString(), // LPCTSTR lpFileName,
		dwDesiredAccess, // DWORD dwDesiredAccess,
		FILE_SHARE_READ, // DWORD dwShareMode,
		&sa, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		OPEN_EXISTING, // DWORD dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL, // DWORD dwFlagsAndAttributes,
		NULL // HANDLE hTemplateFile
	);
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CSecurityFile::Open"), _path.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	m_fileHandle = open(_path.GetString(), _O_BINARY | (_readOnly?_O_RDONLY:_O_RDWR));
    if ( m_fileHandle < 0 )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CSecurityFile::Open"), _path.GetString(), CWinException::CRunTimeError);
#endif
	if (_encoding == ScanFor_Encoding)
		ScanForEncoding();
}

void CSecurityFile::Create(ConstRef(CFilePath) _path, WBool _textMode, TEncoding _encoding, mode_t mode)
{
	Close();
	CFile::Create(_path, _textMode, _encoding);

#ifdef OK_SYS_WINDOWS
	SECURITY_ATTRIBUTES sa;
	CStringBuffer tmp(_path.get_Path());

	CSecurityContext::instance()->GetFileSecurity(mode, sa);

#ifdef _UNICODE
	if ( tmp.GetLength() > MAX_PATH )
		tmp.PrependString(_T("\\\\?\\"));
#else
	if ( tmp.GetLength() > MAX_PATH )
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if ( ((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH) )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
			    _T("CSecurityFile::Create"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	m_fileHandle = CreateFile(
		tmp.GetString(), // LPCTSTR lpFileName,
		GENERIC_READ | GENERIC_WRITE, // DWORD dwDesiredAccess,
		FILE_SHARE_READ, // DWORD dwShareMode,
		&sa, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		CREATE_NEW, // DWORD dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL, // DWORD dwFlagsAndAttributes,
		NULL // HANDLE hTemplateFile
	);
	CSecurityContext::instance()->FreeSecurity(sa);
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CSecurityFile::Create"), _path.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	m_fileHandle = creat(_path.GetString(), mode);
    if ( m_fileHandle < 0 )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CSecurityFile::Create"), _path.GetString(), CWinException::CRunTimeError);
#endif
	WriteBOM();
}

void CSecurityFile::Close()
{
	if ( m_fileHandle != INVALID_HANDLE_VALUE )
#ifdef OK_SYS_WINDOWS
		CloseHandle(m_fileHandle);
#endif
#ifdef OK_SYS_UNIX
		close(m_fileHandle);
#endif
	m_fileHandle = INVALID_HANDLE_VALUE;
}

CFile::TFileSize CSecurityFile::GetSize()
{
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
		return 0;
#ifdef OK_SYS_WINDOWS
#if OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR
	LARGE_INTEGER res;

	if ( !GetFileSizeEx(m_fileHandle, &res) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetSize"), CWinException::WinExtError);
	return res.QuadPart;
#else
	LARGE_INTEGER res;
	DWORD lres;
	DWORD hres;

	if ( (lres = GetFileSize(m_fileHandle, &hres)) == INVALID_FILE_SIZE )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetSize"), CWinException::WinExtError);
	res.HighPart = hres;
	res.LowPart = lres;
	return res.QuadPart;
#endif
#endif
#ifdef OK_SYS_UNIX
	off_t pos = lseek(m_fileHandle, 0, SEEK_CUR);
	
	if ( pos < 0 )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetSize"), CWinException::CRunTimeError);
	
	off_t end = lseek(m_fileHandle, 0, SEEK_END);
	
	if ( end < 0 )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetSize"), CWinException::CRunTimeError);
		
	if ( lseek(m_fileHandle, pos, SEEK_SET) < 0 )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetSize"), CWinException::CRunTimeError);
	return end;
#endif
}

void CSecurityFile::SetSize(TFileSize newsize)
{
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
		return;
#ifdef OK_SYS_WINDOWS
	TFileOffset curPos = GetFilePos();
	TFileSize curSize = GetSize();

	if ( newsize < curSize ) // truncate
	{
		SetFilePos(newsize);
		if ( !SetEndOfFile(m_fileHandle) )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		        _T("CSecurityFile::SetSize"), CWinException::WinExtError);
		if ( Cast(TFileSize, curPos) > newsize )
			curPos = newsize;
		SetFilePos(curPos);
	}
	else if ( newsize > curSize ) // extend
	{
		char bp[4096] = { 0 };
		TFileOffset delta = newsize - curSize;

		SetFilePos(curSize);
		while ( delta > 0 )
		{
			DWORD sz = (delta > 4096) ? 4096 : Cast(DWORD, delta);
			DWORD NumberOfBytesWritten;

			if ( !WriteFile(m_fileHandle, bp, sz, &NumberOfBytesWritten, NULL) )
				throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
				    _T("CSecurityFile::SetSize"), CWinException::WinExtError);
			delta -= sz;
		}
		SetFilePos(curPos);
	}
#endif
#ifdef OK_SYS_UNIX
	off_t pos = lseek(m_fileHandle, 0, SEEK_CUR);
	
	if ( pos < 0 )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::SetSize"), CWinException::CRunTimeError);
	
	off_t end = lseek(m_fileHandle, 0, SEEK_END);
	
	if ( end < 0 )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::SetSize"), CWinException::CRunTimeError);
	
	TFileOffset cnt = newsize - end;
	
	if ( cnt < 0 ) // FIXME: truncate, use tmp file
		return;
	
	Pointer buf = TFalloc(4096);
	
	if ( PtrCheck(buf) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    	_T("CSecurityFile::SetSize"), CWinException::CRunTimeError, ENOMEM);
	
	while ( cnt > 0 )
	{
		dword sz = (cnt > 4096) ? 4096 : cnt;

		if ( write(m_fileHandle, buf, sz) != Cast(int,sz) )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
				_T("CSecurityFile::SetSize"), CWinException::CRunTimeError);
		cnt -= sz;
	}
	if ( lseek(m_fileHandle, pos, SEEK_SET) < 0 )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::SetSize"), CWinException::CRunTimeError);
		
	TFfree(buf);
#endif
}

CFile::TFileOffset CSecurityFile::GetFilePos()
{
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
		return -1;
#ifdef OK_SYS_WINDOWS
	LARGE_INTEGER liDistanceToMove;
	LARGE_INTEGER res;

	liDistanceToMove.QuadPart = 0;
	if ( !SetFilePointerEx(m_fileHandle, liDistanceToMove, &res, FILE_CURRENT) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetFilePos"), CWinException::WinExtError);
	return res.QuadPart;
#endif
#ifdef OK_SYS_UNIX
	CFile::TFileOffset res = lseek(m_fileHandle, 0, SEEK_CUR);

	if ( 0 > res )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetFilePos"), CWinException::CRunTimeError);
	return res;
#endif
}

void CSecurityFile::SetFilePos(TFileOffset pos)
{
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
		return;
#ifdef OK_SYS_WINDOWS
	LARGE_INTEGER liDistanceToMove;
	LARGE_INTEGER res;

	liDistanceToMove.QuadPart = pos;
	if ( !SetFilePointerEx(m_fileHandle, liDistanceToMove, &res, FILE_BEGIN) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetFilePos"), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	if ( 0 > lseek( m_fileHandle, pos, SEEK_SET ) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
		    _T("CSecurityFile::GetFilePos"), CWinException::CRunTimeError);
#endif
}

void CSecurityFile::Read(Ref(CByteBuffer) _buffer)
{
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
	{
		_buffer.set_BufferSize(__FILE__LINE__ 0);
		return;
	}
#ifdef OK_SYS_WINDOWS
	DWORD NumberOfBytesRead;
	LARGE_INTEGER fileOffset;
	LARGE_INTEGER bytesToLock;

	fileOffset.QuadPart = GetFilePos();
	bytesToLock.QuadPart = _buffer.get_BufferSize();

	if ( !LockFile(m_fileHandle, fileOffset.LowPart, fileOffset.HighPart, bytesToLock.LowPart, bytesToLock.HighPart) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Read"), CWinException::WinExtError);
    if ( !ReadFile(m_fileHandle, _buffer.get_Buffer(), _buffer.get_BufferSize(), &NumberOfBytesRead, NULL) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Read"), CWinException::WinExtError);
	if ( !UnlockFile(m_fileHandle, fileOffset.LowPart, fileOffset.HighPart, bytesToLock.LowPart, bytesToLock.HighPart) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Read"), CWinException::WinExtError);
	_buffer.set_BufferSize(__FILE__LINE__ NumberOfBytesRead);
#endif
#ifdef OK_SYS_UNIX
	int cnt = read(m_fileHandle, _buffer.get_Buffer(), _buffer.get_BufferSize());
	if ( 0 > cnt )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Read"), CWinException::CRunTimeError);
	_buffer.set_BufferSize(__FILE__LINE__ cnt);
#endif
}

void CSecurityFile::Read(Ref(CByteLinkedBuffer) _buffer)
{
#ifdef OK_SYS_WINDOWS
	dword ix = 0;
	dword max = _buffer.GetBufferItemCount();

	if ( m_fileHandle != INVALID_HANDLE_VALUE )
	{
		DWORD NumberOfBytesRead;
		LARGE_INTEGER fileOffset;
		LARGE_INTEGER bytesToLock;

		fileOffset.QuadPart = GetFilePos();
		bytesToLock.QuadPart = _buffer.GetTotalLength();

		if ( !LockFile(m_fileHandle, fileOffset.LowPart, fileOffset.HighPart, bytesToLock.LowPart, bytesToLock.HighPart) )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Read"), CWinException::WinExtError);
		for ( ; ix < max; ++ix )
		{
			dword sz;
			BPointer p = _buffer.GetBufferItem(ix, &sz);

			if ( !ReadFile(m_fileHandle, p, sz, &NumberOfBytesRead, NULL) )
				throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Read"), CWinException::WinExtError);
			if ( NumberOfBytesRead == 0 )
				break;
			_buffer.SetBufferItemSize(ix, NumberOfBytesRead);
		}
		if ( !UnlockFile(m_fileHandle, fileOffset.LowPart, fileOffset.HighPart, bytesToLock.LowPart, bytesToLock.HighPart) )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Read"), CWinException::WinExtError);
	}
	for ( ; ix < max; ++ix )
		_buffer.SetBufferItemSize(ix, 0);
#endif
#ifdef OK_SYS_UNIX
	dword ix = 0;
	dword max = _buffer.GetBufferItemCount();

	if ( m_fileHandle >= 0 )
	{
		for ( ; ix < max; ++ix )
		{
			dword sz;
			BPointer p = _buffer.GetBufferItem(ix, &sz);
			int cnt = read( m_fileHandle, p, sz );

			if ( 0 > cnt )
				throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
				    _T("CSecurityFile::Read"), CWinException::CRunTimeError);
			if ( cnt == 0 )
				break;
			_buffer.SetBufferItemSize(ix, cnt);
		}
	}
	for ( ; ix < max; ++ix )
		_buffer.SetBufferItemSize(ix, 0);
#endif
}

void CSecurityFile::Write(ConstRef(CByteBuffer) _buffer)
{
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
		return;
#ifdef OK_SYS_WINDOWS
	DWORD NumberOfBytesWritten;
	LARGE_INTEGER fileOffset;
	LARGE_INTEGER bytesToLock;

	fileOffset.QuadPart = GetFilePos();
	bytesToLock.QuadPart = _buffer.get_BufferSize();

	if ( !LockFile(m_fileHandle, fileOffset.LowPart, fileOffset.HighPart, bytesToLock.LowPart, bytesToLock.HighPart) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Write"), CWinException::WinExtError);
	if ( !WriteFile(m_fileHandle, _buffer.get_Buffer(), _buffer.get_BufferSize(), &NumberOfBytesWritten, NULL) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Write"), CWinException::WinExtError);
	if ( !UnlockFile(m_fileHandle, fileOffset.LowPart, fileOffset.HighPart, bytesToLock.LowPart, bytesToLock.HighPart) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Write"), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	if ( 0 > write(m_fileHandle, _buffer.get_Buffer(), _buffer.get_BufferSize()) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Write"), CWinException::CRunTimeError);
#endif
}

void CSecurityFile::Write(ConstRef(CByteLinkedBuffer) _buffer)
{
	if ( m_fileHandle == INVALID_HANDLE_VALUE )
		return;
#ifdef OK_SYS_WINDOWS
	DWORD NumberOfBytesWritten;
	LARGE_INTEGER fileOffset;
	LARGE_INTEGER bytesToLock;

	fileOffset.QuadPart = GetFilePos();
	bytesToLock.QuadPart = _buffer.GetTotalLength();

	if ( !LockFile(m_fileHandle, fileOffset.LowPart, fileOffset.HighPart, bytesToLock.LowPart, bytesToLock.HighPart) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Write"), CWinException::WinExtError);
	for ( dword ix = 0; ix < _buffer.GetBufferItemCount(); ++ix )
	{
		dword sz;
		BPointer p = _buffer.GetBufferItem(ix, &sz);

		if ( !WriteFile(m_fileHandle, p, sz, &NumberOfBytesWritten, NULL) )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Write"), CWinException::WinExtError);
	}
	if ( !UnlockFile(m_fileHandle, fileOffset.LowPart, fileOffset.HighPart, bytesToLock.LowPart, bytesToLock.HighPart) )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityFile::Write"), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	for ( dword ix = 0; ix < _buffer.GetBufferItemCount(); ++ix )
	{
		dword sz;
		BPointer p = _buffer.GetBufferItem(ix, &sz);

		if ( 0 > write( m_fileHandle, p, sz ) )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), 
			    _T("CSecurityFile::Write"), CWinException::CRunTimeError);
	}
#endif
}
