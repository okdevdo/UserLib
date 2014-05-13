/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

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
#include "WProcess.h"
#include "DirectoryIterator.h"
#include "SecurityFile.h"
#include "SecurityContext.h"
#include "URL.h"
#include "WinDirectoryIterator.h"

static void _TestProcess()
{
	CProcess process;
#ifdef OK_SYS_WINDOWS
	CFilePath cmd(__FILE__LINE__ _T("c:\\cygwin\\bin\\bash.exe"));
#endif
#ifdef OK_SYS_UNIX
	CFilePath cmd(__FILE__LINE__ _T("/bin/bash.exe"));
#endif
	CStringBuffer args(__FILE__LINE__ _T("-norc -noprofile \"scandir.sh\" /home/Oliver/flex-2.5.37"));
	CDataVectorT<CStringBuffer> env(__FILE__LINE__ 1, 1);
#ifdef OK_SYS_WINDOWS
	CFilePath startDir(__FILE__LINE__ _T("c:\\cygwin\\home\\Oliver\\"));
#endif
#ifdef OK_SYS_UNIX
	CFilePath startDir(__FILE__LINE__ _T("/home/Oliver/"));
#endif
	CByteBuffer buf;
	CStringBuffer sBuf;

	process.Create(cmd, args, env, startDir);

	process.Write(buf);
	buf.set_BufferSize(__FILE__LINE__ 128);
	process.Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		sBuf.convertFromByteBuffer(buf);
		COUT << sBuf;
		process.Read(buf);
	}
	process.Close();
}

static void _TestSecurityFile()
{
	CFilePath tdir(__FILE__LINE__ _T("TestDir"));
	CFilePath tfile;

	tdir.MakeDirectory();
	tfile = tdir;
	tfile.set_Filename(_T("Test.dat"));
	if (CWinDirectoryIterator::DirectoryExists(tdir) >= 0)
	{
		CWinDirectoryIterator::UnlinkFile(tfile);
		CWinDirectoryIterator::UnlinkDirectory(tdir);
	}
	CWinDirectoryIterator::MakeDirectory(tdir);

	CSecurityFile file;

	file.Create(tfile);
	file.SetSize(123456L);
	file.Close();
}

static void _TestURL()
{
	CUrl url(_T("http://localhost:8080/homepage/index.html"));
	CUrl url1(_T("http://localhost:8080/homepage/index.html?test=value#anchor"));
	CUrl url1a(_T("http://localhost:8080/homepage/index.html?test=value"));
	CUrl url1b(_T("http://localhost:8080?test=value"));
	CUrl url1c(_T("http://localhost:8080?test=value#anchor"));
	CUrl url2(_T("/homepage/index.html"));
	CUrl url3(_T("/homepage/index.html?test=value#anchor"));
	CUrl url3a(_T("/homepage/index.html?test=value"));

	assert(url.get_Protocol().Compare(CStringLiteral(_T("http")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url.get_Server().Compare(CStringLiteral(_T("localhost:8080")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url.get_Resource().Compare(CStringLiteral(_T("/homepage/index.html")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url.get_Fragment().IsEmpty());
	assert(url.get_Queries().count() == 0);
	CStringBuffer tmp = url.get_Encoded(CUrl::EncodeTypeStandard);

	assert(url1.get_Protocol().Compare(CStringLiteral(_T("http")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1.get_Server().Compare(CStringLiteral(_T("localhost:8080")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1.get_Resource().Compare(CStringLiteral(_T("/homepage/index.html")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1.get_Fragment().Compare(CStringLiteral(_T("anchor")), 0, CStringLiteral::cIgnoreCase) == 0);
	ConstRef(CUrl::QueryDataList) tmp1 = url1.get_Queries();
	ConstRef(CUrl::QueryDataItem) value1 = tmp1.search(CStringBuffer(__FILE__LINE__ _T("test")));
	assert(value1.Value.Compare(CStringLiteral(_T("value")), 0, CStringLiteral::cIgnoreCase) == 0);

	assert(url1a.get_Protocol().Compare(CStringLiteral(_T("http")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1a.get_Server().Compare(CStringLiteral(_T("localhost:8080")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1a.get_Resource().Compare(CStringLiteral(_T("/homepage/index.html")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1a.get_Fragment().IsEmpty());
	ConstRef(CUrl::QueryDataList) tmp1a = url1a.get_Queries();
	ConstRef(CUrl::QueryDataItem) value1a = tmp1a.search(CStringBuffer(__FILE__LINE__ _T("test")));
	assert(value1a.Value.Compare(CStringLiteral(_T("value")), 0, CStringLiteral::cIgnoreCase) == 0);

	assert(url1b.get_Protocol().Compare(CStringLiteral(_T("http")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1b.get_Server().Compare(CStringLiteral(_T("localhost:8080")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1b.get_Resource().Compare(CStringLiteral(_T("/")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1b.get_Fragment().IsEmpty());
	ConstRef(CUrl::QueryDataList) tmp1b = url1b.get_Queries();
	ConstRef(CUrl::QueryDataItem) value1b = tmp1b.search(CStringBuffer(__FILE__LINE__ _T("test")));
	assert(value1b.Value.Compare(CStringLiteral(_T("value")), 0, CStringLiteral::cIgnoreCase) == 0);

	assert(url1c.get_Protocol().Compare(CStringLiteral(_T("http")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1c.get_Server().Compare(CStringLiteral(_T("localhost:8080")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1c.get_Resource().Compare(CStringLiteral(_T("/")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url1c.get_Fragment().Compare(CStringLiteral(_T("anchor")), 0, CStringLiteral::cIgnoreCase) == 0);
	ConstRef(CUrl::QueryDataList) tmp1c = url1c.get_Queries();
	ConstRef(CUrl::QueryDataItem) value1c = tmp1c.search(CStringBuffer(__FILE__LINE__ _T("test")));
	assert(value1c.Value.Compare(CStringLiteral(_T("value")), 0, CStringLiteral::cIgnoreCase) == 0);

	assert(url2.get_Protocol().Compare(CStringLiteral(_T("http")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url2.get_Server().IsEmpty());
	assert(url2.get_Resource().Compare(CStringLiteral(_T("/homepage/index.html")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url2.get_Queries().count() == 0);
	assert(url2.get_Fragment().IsEmpty());

	assert(url3.get_Protocol().Compare(CStringLiteral(_T("http")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url3.get_Server().IsEmpty());
	assert(url3.get_Resource().Compare(CStringLiteral(_T("/homepage/index.html")), 0, CStringLiteral::cIgnoreCase) == 0);
	ConstRef(CUrl::QueryDataList) tmp3 = url3.get_Queries();
	ConstRef(CUrl::QueryDataItem) value3 = tmp3.search(CStringBuffer(__FILE__LINE__ _T("test")));
	assert(value3.Value.Compare(CStringLiteral(_T("value")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url3.get_Fragment().Compare(CStringLiteral(_T("anchor")), 0, CStringLiteral::cIgnoreCase) == 0);

	assert(url3a.get_Protocol().Compare(CStringLiteral(_T("http")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url3a.get_Server().IsEmpty());
	assert(url3a.get_Resource().Compare(CStringLiteral(_T("/homepage/index.html")), 0, CStringLiteral::cIgnoreCase) == 0);
	ConstRef(CUrl::QueryDataList) tmp3a = url3a.get_Queries();
	ConstRef(CUrl::QueryDataItem) value3a = tmp3a.search(CStringBuffer(__FILE__LINE__ _T("test")));
	assert(value3a.Value.Compare(CStringLiteral(_T("value")), 0, CStringLiteral::cIgnoreCase) == 0);
	assert(url3a.get_Fragment().IsEmpty());
}

void TestWinSources()
{
	try
	{
		COUT << _T("********** _TestProcess ************") << endl;
		_TestProcess();
	}
	catch (CBaseException* ex)
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
	try
	{
		COUT << _T("********** _TestSecurityFile ************") << endl;
		_TestSecurityFile();
	}
	catch (CBaseException* ex)
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
	try
	{
		COUT << _T("********** _TestURL ************") << endl;
		_TestURL();
	}
	catch (CBaseException* ex)
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
	CSecurityContext_FreeInstance
}