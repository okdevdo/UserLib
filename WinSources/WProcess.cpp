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
#include "WProcess.h"

#ifdef OK_SYS_UNIX
#include "DirectoryIterator.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include <iostream>

#define COUT std::cout
using std::endl;

static void sigchldhandler(int sig)
{
    int stat;
    pid_t pid;

    pid = wait(&stat);

    COUT << _T("Child ") << pid << _T(" exited.") << endl;
}
#endif

IMPL_WINEXCEPTION(CProcessException, CWinException)

CProcess::CProcess(void)
#ifdef OK_SYS_WINDOWS
	: m_hChildStd_IN_Rd(INVALID_HANDLE_VALUE),
	m_hChildStd_IN_Wr (INVALID_HANDLE_VALUE),
	m_hChildStd_OUT_Rd(INVALID_HANDLE_VALUE),
	m_hChildStd_OUT_Wr(INVALID_HANDLE_VALUE)
#endif
{
#ifdef OK_SYS_UNIX
	m_stdin[0] = -1;
	m_stdin[1] = -1;
	m_stdout[0] = -1;
	m_stdout[1] = -1;
#endif
}

CProcess::~CProcess(void)
{
}

void CProcess::Create(ConstRef(CFilePath) _path)
{
	CDataVectorT<CStringBuffer> env(__FILE__LINE__ 1, 1);

	Create(_path, CStringBuffer(__FILE__LINE__ _T("")), env);
}

void CProcess::Create(ConstRef(CFilePath) _cmd, ConstRef(CStringBuffer) _args, ConstRef(CDataVectorT<CStringBuffer>) _env, ConstRef(CFilePath) _startDir)
{
	CFile::Create(_cmd);

#ifdef OK_SYS_WINDOWS
	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	if ( !CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0) )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::WinExtError);
	if ( !SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::WinExtError);

	if ( !CreatePipe(&m_hChildStd_IN_Rd, &m_hChildStd_IN_Wr , &saAttr, 0)) 
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::WinExtError);
	if ( !SetHandleInformation(m_hChildStd_IN_Wr , HANDLE_FLAG_INHERIT, 0) )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::WinExtError);

	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;
	DWORD flags = CREATE_NO_WINDOW;
	LPVOID envP = NULL;
	LPCTSTR startDirP = NULL;

#ifdef _UNICODE
	flags |= CREATE_UNICODE_ENVIRONMENT;
#endif

	::ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
	::ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = m_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = m_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = m_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	CStringBuffer cmdLine;

	cmdLine += _T("\"");
	cmdLine += _cmd.get_Path();
	cmdLine += _T("\" ");
	cmdLine += _args;

	CStringBuffer envB;

	if ( _env.Count() > 0 )
	{
		envB = _env.Join(_T("@"));
		envB += _T("@");

		CPointer p = CastMutable(CPointer, envB.GetString());
		dword sz = envB.GetLength();

		while ( sz > 0 )
		{
			if ( *p == _T('@') )
				*p = 0;
			++p;
			--sz;
		}
		envP = Cast(LPVOID, CastMutable(CPointer, envB.GetString()));
	}

	CStringBuffer startDir(_startDir.get_Path());

	if ( !(startDir.IsEmpty()) )
		startDirP = Cast(LPCTSTR, CastMutable(CPointer, startDir.GetString()));

	if ( !CreateProcess(NULL, 
		Cast(LPTSTR, CastMutable(CPointer, cmdLine.GetString())),     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		flags,         // creation flags 
		envP,          // environment 
		startDirP,     // current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo) )  // receives PROCESS_INFORMATION 
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::WinExtError);

	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);

	CloseHandle(m_hChildStd_OUT_Wr);
	m_hChildStd_OUT_Wr = INVALID_HANDLE_VALUE;
	CloseHandle(m_hChildStd_IN_Rd);
	m_hChildStd_IN_Rd = INVALID_HANDLE_VALUE;
#endif
#ifdef OK_SYS_UNIX
	pid_t   childpid;
    struct sigaction sa;
	CPointer fname = CastMutable(CPointer, _cmd.get_Path().GetString());
	CPointer argv[64];
	CPointer envp[256];
	dword cnt = 0;
	CStringBuffer args(_args);

	args.PrependString(_T("\" "));
	args.PrependString(fname);
	args.PrependString(_T("\""));
	args.SplitQuoted(_T("\""), _T(" "), argv, 64, &cnt);
	argv[cnt] = NULL;

	envp[0] = NULL;

    if ( 0 > pipe(m_stdin) )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::CRunTimeError);
    if ( 0 > pipe(m_stdout) )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::CRunTimeError);
        
    sa.sa_handler = sigchldhandler;
    if ( 0 > sigaction(SIGCHLD, &sa, NULL) )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::CRunTimeError);

    if ( (childpid = fork()) == -1 )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::CRunTimeError);

    if(childpid == 0)
    {
		close(0);
		close(1);
		close(2);
		dup(m_stdin[0]);
		dup(m_stdout[1]);
		dup(m_stdout[1]);
		close(m_stdin[0]);
		close(m_stdin[1]);
		close(m_stdout[0]);
		close(m_stdout[1]);

		CDirectoryIterator::SetCurrentDirectory(_startDir);

		COUT << fname << _T(", cnt=") << cnt << endl << endl;

		execve(fname, argv, envp); 
        throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Create"), CWinException::CRunTimeError);
    }
	close(m_stdin[0]);
	close(m_stdout[1]);
#endif
}

void CProcess::Close()
{
#ifdef OK_SYS_WINDOWS
	if (INVALID_HANDLE_VALUE != m_hChildStd_OUT_Rd)
	{
		CloseHandle(m_hChildStd_OUT_Rd);
		m_hChildStd_OUT_Rd = INVALID_HANDLE_VALUE;
	}
	if ( INVALID_HANDLE_VALUE != m_hChildStd_IN_Wr )
	{
		CloseHandle(m_hChildStd_IN_Wr);
		m_hChildStd_IN_Wr = INVALID_HANDLE_VALUE;
	}
#endif
#ifdef OK_SYS_UNIX
	if ( m_stdin[1] >= 0 )
	{
		close(m_stdin[1]);
		m_stdin[1] = -1;
	}
	if ( m_stdout[0] >= 0 )
	{
		close(m_stdout[0]);
		m_stdout[0] = -1;
	}
#endif
}

CFile::TFileSize CProcess::GetSize()
{
#ifdef OK_SYS_WINDOWS
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::GetSize"), CWinException::WinExtError, ERROR_CALL_NOT_IMPLEMENTED);
#endif
#ifdef OK_SYS_UNIX
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::GetSize"), CWinException::CRunTimeError, ENOSYS);
#endif
	return -1;
}

void CProcess::SetSize(TFileSize)
{
#ifdef OK_SYS_WINDOWS
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::SetSize"), CWinException::WinExtError, ERROR_CALL_NOT_IMPLEMENTED);
#endif
#ifdef OK_SYS_UNIX
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::SetSize"), CWinException::CRunTimeError, ENOSYS);
#endif
}

CFile::TFileOffset CProcess::GetFilePos()
{
#ifdef OK_SYS_WINDOWS
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::GetFilePos"), CWinException::WinExtError, ERROR_CALL_NOT_IMPLEMENTED);
#endif
#ifdef OK_SYS_UNIX
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::GetFilePos"), CWinException::CRunTimeError, ENOSYS);
#endif
	return -1;
}

void CProcess::SetFilePos(TFileOffset)
{
#ifdef OK_SYS_WINDOWS
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::SetFilePos"), CWinException::WinExtError, ERROR_CALL_NOT_IMPLEMENTED);
#endif
#ifdef OK_SYS_UNIX
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::SetFilePos"), CWinException::CRunTimeError, ENOSYS);
#endif
}

void CProcess::Read(Ref(CByteBuffer) _buffer)
{
#ifdef OK_SYS_WINDOWS
	DWORD dwRead;

   if ( !ReadFile( m_hChildStd_OUT_Rd, _buffer.get_Buffer(), _buffer.get_BufferSize(), &dwRead, NULL) )
   {
	   if ( GetLastError() == ERROR_BROKEN_PIPE ) // 109 - Pipe has been ended by client program
		   dwRead = 0;
	   else
		   throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Read"), CWinException::WinExtError);
   }
   _buffer.set_BufferSize(__FILE__LINE__ dwRead);
#endif
#ifdef OK_SYS_UNIX
   int cnt = read(m_stdout[0], _buffer.get_Buffer(), _buffer.get_BufferSize());

	if ( 0 > cnt )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Read"), CWinException::CRunTimeError);
	_buffer.set_BufferSize(__FILE__LINE__ cnt);
#endif
}

void CProcess::Read(Ref(CByteLinkedBuffer) _buffer)
{
#ifdef OK_SYS_WINDOWS
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Read"), CWinException::WinExtError, ERROR_CALL_NOT_IMPLEMENTED);
#endif
#ifdef OK_SYS_UNIX
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Read"), CWinException::CRunTimeError, ENOSYS);
#endif
}

void CProcess::Write(ConstRef(CByteBuffer) _buffer)
{
#ifdef OK_SYS_WINDOWS
	if (_buffer.get_BufferSize() == 0)
   {
	   if ( !CloseHandle(m_hChildStd_IN_Wr ) )
		   throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Write"), CWinException::WinExtError);
	   m_hChildStd_IN_Wr = INVALID_HANDLE_VALUE;
	   return;
   }

   DWORD dwWritten; 

   if ( !WriteFile(m_hChildStd_IN_Wr , _buffer.get_Buffer(), _buffer.get_BufferSize(), &dwWritten, NULL) )
		throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Write"), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
   if ( _buffer.get_BufferSize() == 0 )
   {
	   close(m_stdin[1]);
	   m_stdin[1] = -1;
	   return;
   }
   if ( 0 > write(m_stdin[1], _buffer.get_Buffer(), _buffer.get_BufferSize()) )
	   throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Write"), CWinException::CRunTimeError);
#endif
}

void CProcess::Write(ConstRef(CByteLinkedBuffer) _buffer)
{
#ifdef OK_SYS_WINDOWS
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Write"), CWinException::WinExtError, ERROR_CALL_NOT_IMPLEMENTED);
#endif
#ifdef OK_SYS_UNIX
	throw OK_NEW_OPERATOR CProcessException(__FILE__LINE__ _T("CProcess Exception in %s"), _T("CProcess::Write"), CWinException::CRunTimeError, ENOSYS);
#endif
}
