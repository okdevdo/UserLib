/******************************************************************************
    
	This file is part of XSetup, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#include "Program.h"
#include "Script.h"
#include "DirectoryIterator.h"

CBashScript::CBashScript(CStringLiteral _installDir):
	_cmd(),
	_args(__FILE__LINE__ _T("-norc -noprofile ")),
	_env(__FILE__LINE__ 16, 16),
	_startDir(__FILE__LINE__ _installDir)
{
	_cmd.set_Directory(_installDir);
	_cmd.set_Filename(_T("bin\\bash.exe"));
	InitializeEnv(_installDir);
}

CBashScript::~CBashScript(void)
{
}

void CBashScript::InitializeEnv(CStringLiteral _installDir)
{
	CFilePath tempPath;
	CStringBuffer tmp;
	LPTCH env = GetEnvironmentStrings();

	if (env)
	{
		for (CConstPointer p = env; *p; p = s_strchr (p, '\0') + 1)
		{
			if ( s_strnicmp(p, _T("comspec"), 7) == 0 )
				_env.InsertSorted(CStringBuffer(__FILE__LINE__ p));
			else if ( s_strnicmp(p, _T("system"), 6) == 0 )
				_env.InsertSorted(CStringBuffer(__FILE__LINE__ p));
			else if ( s_strnicmp(p, _T("user"), 4) == 0 )
				_env.InsertSorted(CStringBuffer(__FILE__LINE__ p));
			else if ( s_strnicmp(p, _T("windir"), 6) == 0 )
				_env.InsertSorted(CStringBuffer(__FILE__LINE__ p));
		}
		FreeEnvironmentStrings(env);
	}
	tmp.SetString(__FILE__LINE__ _T("CYGWINROOT="));
	tmp += _installDir;
	_env.InsertSorted(tmp);
	tmp.SetString(__FILE__LINE__ _T("CYGWINFORALL=-A"));
	_env.InsertSorted(tmp);
	tmp.SetString(__FILE__LINE__ _T("SHELL=/bin/bash"));
	_env.InsertSorted(tmp);
	tempPath.set_Path(__FILE__LINE__ _T("/tmp"), CDirectoryIterator::UnixPathSeparatorString());
	tempPath.Normalize(_installDir);
	tmp.SetString(__FILE__LINE__ _T("TEMP="));
	tmp += tempPath.get_Path();
	_env.InsertSorted(tmp);
	tmp.SetString(__FILE__LINE__ _T("TERM=dumb"));
	_env.InsertSorted(tmp);
	tmp.SetString(__FILE__LINE__ _T("TMP=/tmp"));
	_env.InsertSorted(tmp);

	TCHAR dummy;
	DWORD len = GetEnvironmentVariable (_T("PATH"), &dummy, 0);
	CStringBuffer path(__FILE__LINE__ len + 1);
	CStringBuffer sysroot(__FILE__LINE__ MAX_PATH + 1);
	CPointer pathSplit[64];
	dword maxPathSplit;

	GetEnvironmentVariable (_T("PATH"), Cast(LPTSTR, CastMutable(CPointer, path.GetString())), len);
	GetWindowsDirectory (Cast(LPTSTR, CastMutable(CPointer, sysroot.GetString())), MAX_PATH);

	tempPath.set_Path(__FILE__LINE__ _T("/bin"), CDirectoryIterator::UnixPathSeparatorString());

	tempPath.Normalize(_installDir);
	tmp.SetString(__FILE__LINE__ _T("PATH="));
	tmp += tempPath.get_Path();

	tempPath.set_Path(__FILE__LINE__ _T("/usr/sbin"), CDirectoryIterator::UnixPathSeparatorString());

	tempPath.Normalize(_installDir);
	tmp += _T(";");
	tmp += tempPath.get_Path();

	tempPath.set_Path(__FILE__LINE__ _T("/sbin"), CDirectoryIterator::UnixPathSeparatorString());

	tempPath.Normalize(_installDir);
	tmp += _T(";");
	tmp += tempPath.get_Path();

	path.Split(_T(";"), pathSplit, 64, &maxPathSplit);
	len = sysroot.GetLength();
	for ( dword ix = 0; ix < maxPathSplit; ++ix )
	{
		if ( s_strnicmp(pathSplit[ix], sysroot.GetString(), len) == 0 )
		{
			tmp += _T(";");
			tmp += pathSplit[ix];
		}
	}
	_env.InsertSorted(tmp);

	//CDataVectorT<CStringBuffer>::Iterator it = _env.Begin();

	//while ( it )
	//{
	//	CERR << *it << endl;
	//	++it;
	//}
}

void CBashScript::run(ConstRef(CFilePath) _file)
{
	CStringBuffer args(_args);
	CByteBuffer buf;
	CStringBuffer sBuf;

	args += _T("\"");
	args += _file.get_Path();
	args += _T("\"");
	_process.Create(_cmd, args, _env, _startDir);

	_process.Write(buf);
	buf.set_BufferSize(__FILE__LINE__ 128);
	_process.Read(buf);
	while ( buf.get_BufferSize() > 0 )
	{
		sBuf.convertFromByteBuffer(buf);
		COUT << sBuf;
		_process.Read(buf);
	}
	_process.Close();
}

CCmdScript::CCmdScript(CStringLiteral _installDir):
	_cmd(__FILE__LINE__ _T("cmd")),
	_args(__FILE__LINE__ _T("/C ")),
	_env(__FILE__LINE__ 16, 16),
	_startDir(__FILE__LINE__ _installDir)
{
}

CCmdScript::~CCmdScript(void)
{
}

void CCmdScript::run(ConstRef(CFilePath) _file)
{
	CStringBuffer args(_args);
	CByteBuffer buf;
	CStringBuffer sBuf;

	args += _T("\"");
	args += _file.get_Path();
	args += _T("\"");
	_process.Create(_cmd, args, _env, _startDir);

	_process.Write(buf);
	buf.set_BufferSize(__FILE__LINE__ 128);
	_process.Read(buf);
	while ( buf.get_BufferSize() > 0 )
	{
		sBuf.convertFromByteBuffer(buf);
		COUT << sBuf;
		_process.Read(buf);
	}
	_process.Close();
}

