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
#include "AsyncFile.h"
#include "SecurityFile.h"
#include "WinDirectoryIterator.h"

CAsyncFile::CAsyncFile(Ptr(CAsyncIOManager) pManager):
    CAsyncIOBuffer(pManager)
{
}

CAsyncFile::~CAsyncFile(void)
{
}

void CAsyncFile::Create(ConstRef(CFilePath) _path)
{
	CWinDirectoryIterator::UnlinkFile(_path);

	SECURITY_ATTRIBUTES sa;
	HANDLE fHandle;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	fHandle = CreateFile(
		_path.get_Path().GetString(), // LPCTSTR lpFileName,
		GENERIC_READ | GENERIC_WRITE, // DWORD dwDesiredAccess,
		FILE_SHARE_READ, // DWORD dwShareMode,
		&sa, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		CREATE_NEW, // DWORD dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // DWORD dwFlagsAndAttributes,
		NULL // HANDLE hTemplateFile
	);
	if ( fHandle == INVALID_HANDLE_VALUE )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CAsyncFile::Create"), _path.get_Path().GetString(), CWinException::WinExtError);
	m_pData->set_file(fHandle);
	s_memset(m_pData->get_overlapped(), 0, sizeof(OVERLAPPED));
	m_pManager->Register(m_pData);
}

void CAsyncFile::Open(ConstRef(CFilePath) _path, bool _readOnly)
{
	DWORD dwDesiredAccess = GENERIC_READ;
	SECURITY_ATTRIBUTES sa;
	HANDLE fHandle;

	if ( !_readOnly )
		dwDesiredAccess |= GENERIC_WRITE;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	fHandle = CreateFile(
		_path.get_Path().GetString(), // LPCTSTR lpFileName,
		dwDesiredAccess, // DWORD dwDesiredAccess,
		FILE_SHARE_READ, // DWORD dwShareMode,
		&sa, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		OPEN_EXISTING, // DWORD dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // DWORD dwFlagsAndAttributes,
		NULL // HANDLE hTemplateFile
	);
	if ( fHandle == INVALID_HANDLE_VALUE )
		throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception, Path = '%s'"), 
		    _T("CAsyncFile::Open"), _path.get_Path().GetString(), CWinException::WinExtError);
	m_pData->set_file(fHandle);
	s_memset(m_pData->get_overlapped(), 0, sizeof(OVERLAPPED));
	m_pManager->Register(m_pData);
}

void CAsyncFile::Close()
{
	CloseHandle(m_pData->get_file());
}

void CAsyncFile::Read(Ref(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler)
{
	m_pData->set_buffer(buf);
	m_pData->set_bytestransferred(0);
	m_pData->set_callback(pHandler);
	m_pData->set_ioop(CAsyncIOData::IORead);
	if ( !ReadFile(m_pData->get_file(), m_pData->get_buffer().get_Buffer(), m_pData->get_buffer().get_BufferSize(), NULL, m_pData->get_overlapped()) )
	{
		if ( GetLastError() != ERROR_IO_PENDING )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CAsyncFile::Read"), CWinException::WinExtError);
	}
	m_pManager->AddTask(m_pData);
}

void CAsyncFile::Write(ConstRef(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler)
{
	m_pData->set_buffer(buf);
	m_pData->set_bytestransferred(0);
	m_pData->set_callback(pHandler);
	m_pData->set_ioop(CAsyncIOData::IOWrite);
	if ( !WriteFile(m_pData->get_file(), m_pData->get_buffer().get_Buffer(), m_pData->get_buffer().get_BufferSize(), NULL, m_pData->get_overlapped()) )
	{
		if ( GetLastError() != ERROR_IO_PENDING )
			throw OK_NEW_OPERATOR CSecurityFileException(__FILE__LINE__ _T("%s Exception"), _T("CAsyncFile::Write"), CWinException::WinExtError);
	}
	m_pManager->AddTask(m_pData);
}
