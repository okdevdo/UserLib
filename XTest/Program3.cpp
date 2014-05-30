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
#include "DirectoryIterator.h"
#include "VolumeIterator.h"
#include "File.h"

void TestDirectoryIterator()
{
	CFilePath path1(__FILE__LINE__ _T("base"));

	_ASSERTE(path1.get_Extension().IsEmpty());
	_ASSERTE(path1.get_Directory().IsEmpty());
	_ASSERTE(path1.get_Root().IsEmpty());
	_ASSERTE(s_strcmp(path1.get_Basename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path1.get_Filename().GetString(), _T("base")) == 0);
	_ASSERTE(path1.is_File());
	_ASSERTE(!(path1.is_Directory()));
	_ASSERTE(path1.is_Relative());
	_ASSERTE(!(path1.is_Absolute()));

	path1.set_Extension(_T("ext"));

	_ASSERTE(path1.get_Directory().IsEmpty());
	_ASSERTE(path1.get_Root().IsEmpty());
	_ASSERTE(s_strcmp(path1.get_Extension().GetString(), _T("ext")) == 0);
	_ASSERTE(s_strcmp(path1.get_Basename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path1.get_Filename().GetString(), _T("base.ext")) == 0);
	_ASSERTE(path1.is_File());
	_ASSERTE(!(path1.is_Directory()));
	_ASSERTE(path1.is_Relative());
	_ASSERTE(!(path1.is_Absolute()));

	path1.set_FullExtension(_T("ex1.ex2"));

	_ASSERTE(path1.get_Directory().IsEmpty());
	_ASSERTE(path1.get_Root().IsEmpty());
	_ASSERTE(s_strcmp(path1.get_Extension().GetString(), _T("ex2")) == 0);
	_ASSERTE(s_strcmp(path1.get_FullExtension().GetString(), _T("ex1.ex2")) == 0);
	_ASSERTE(s_strcmp(path1.get_Basename().GetString(), _T("base.ex1")) == 0);
	_ASSERTE(s_strcmp(path1.get_PureBasename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path1.get_Filename().GetString(), _T("base.ex1.ex2")) == 0);
	_ASSERTE(path1.is_File());
	_ASSERTE(!(path1.is_Directory()));
	_ASSERTE(path1.is_Relative());
	_ASSERTE(!(path1.is_Absolute()));

	path1.set_PureBasename(_T("base1"));

	_ASSERTE(path1.get_Directory().IsEmpty());
	_ASSERTE(path1.get_Root().IsEmpty());
	_ASSERTE(s_strcmp(path1.get_Extension().GetString(), _T("ex2")) == 0);
	_ASSERTE(s_strcmp(path1.get_FullExtension().GetString(), _T("ex1.ex2")) == 0);
	_ASSERTE(s_strcmp(path1.get_Basename().GetString(), _T("base1.ex1")) == 0);
	_ASSERTE(s_strcmp(path1.get_PureBasename().GetString(), _T("base1")) == 0);
	_ASSERTE(s_strcmp(path1.get_Filename().GetString(), _T("base1.ex1.ex2")) == 0);
	_ASSERTE(path1.is_File());
	_ASSERTE(!(path1.is_Directory()));
	_ASSERTE(path1.is_Relative());
	_ASSERTE(!(path1.is_Absolute()));

	CFilePath path2(__FILE__LINE__ _T("base.ext"));

	_ASSERTE(s_strcmp(path2.get_Extension().GetString(), _T("ext")) == 0);
	_ASSERTE(s_strcmp(path2.get_FullExtension().GetString(), _T("ext")) == 0);
	_ASSERTE(path2.get_Directory().IsEmpty());
	_ASSERTE(path2.get_Root().IsEmpty());
	_ASSERTE(s_strcmp(path2.get_Basename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path2.get_PureBasename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path2.get_Filename().GetString(), _T("base.ext")) == 0);
	_ASSERTE(path2.is_File());
	_ASSERTE(!(path2.is_Directory()));
	_ASSERTE(path2.is_Relative());
	_ASSERTE(!(path2.is_Absolute()));

	path2.set_Basename(_T(""));

	_ASSERTE(s_strcmp(path2.get_Extension().GetString(), _T("ext")) == 0);
	_ASSERTE(s_strcmp(path2.get_FullExtension().GetString(), _T("ext")) == 0);
	_ASSERTE(path2.get_Basename().IsEmpty());
	_ASSERTE(path2.get_PureBasename().IsEmpty());
	_ASSERTE(path2.get_Directory().IsEmpty());
	_ASSERTE(path2.get_Root().IsEmpty());
	_ASSERTE(s_strcmp(path2.get_Filename().GetString(), _T(".ext")) == 0);
	_ASSERTE(path2.is_File());
	_ASSERTE(!(path2.is_Directory()));
	_ASSERTE(path2.is_Relative());
	_ASSERTE(!(path2.is_Absolute()));

#ifdef OK_SYS_WINDOWS
	CFilePath path3(__FILE__LINE__ _T("c:\\"));

	_ASSERTE(s_strcmp(path3.get_Root().GetString(), _T("c:\\")) == 0);
#endif
#ifdef OK_SYS_UNIX
	CFilePath path3(__FILE__LINE__ _T("/"));

	_ASSERTE(s_strcmp(path3.get_Root().GetString(), _T("/")) == 0);
#endif
	_ASSERTE(path3.get_Extension().IsEmpty());
	_ASSERTE(path3.get_FullExtension().IsEmpty());
	_ASSERTE(path3.get_Basename().IsEmpty());
	_ASSERTE(path3.get_PureBasename().IsEmpty());
	_ASSERTE(path3.get_Filename().IsEmpty());
	_ASSERTE(path3.get_Directory().IsEmpty());
	_ASSERTE(!(path3.is_File()));
	_ASSERTE(path3.is_Directory());
	_ASSERTE(!(path3.is_Relative()));
	_ASSERTE(path3.is_Absolute());

#ifdef OK_SYS_WINDOWS
	path3.set_Directory(_T("directory\\"));

	_ASSERTE(s_strcmp(path3.get_Root().GetString(), _T("c:\\")) == 0);
	_ASSERTE(s_strcmp(path3.get_Directory().GetString(), _T("directory\\")) == 0);
#endif
#ifdef OK_SYS_UNIX
	path3.set_Directory(_T("directory/"));

	_ASSERTE(s_strcmp(path3.get_Root().GetString(), _T("/")) == 0);
	_ASSERTE(s_strcmp(path3.get_Directory().GetString(), _T("directory/")) == 0);
#endif
	_ASSERTE(path3.get_Extension().IsEmpty());
	_ASSERTE(path3.get_FullExtension().IsEmpty());
	_ASSERTE(path3.get_Basename().IsEmpty());
	_ASSERTE(path3.get_PureBasename().IsEmpty());
	_ASSERTE(path3.get_Filename().IsEmpty());
	_ASSERTE(!(path3.is_File()));
	_ASSERTE(path3.is_Directory());
	_ASSERTE(!(path3.is_Relative()));
	_ASSERTE(path3.is_Absolute());

#ifdef OK_SYS_WINDOWS
	CFilePath path4(__FILE__LINE__ _T("directory\\"));

	_ASSERTE(s_strcmp(path4.get_Directory().GetString(), _T("directory\\")) == 0);
#endif
#ifdef OK_SYS_UNIX
	CFilePath path4(__FILE__LINE__ _T("directory/"));

	_ASSERTE(s_strcmp(path4.get_Directory().GetString(), _T("directory/")) == 0);
#endif
	_ASSERTE(path4.get_Extension().IsEmpty());
	_ASSERTE(path4.get_FullExtension().IsEmpty());
	_ASSERTE(path4.get_Filename().IsEmpty());
	_ASSERTE(path4.get_Basename().IsEmpty());
	_ASSERTE(path4.get_PureBasename().IsEmpty());
	_ASSERTE(path4.get_Root().IsEmpty());
	_ASSERTE(!(path4.is_File()));
	_ASSERTE(path4.is_Directory());
	_ASSERTE(path4.is_Relative());
	_ASSERTE(!(path4.is_Absolute()));

	path4.set_Basename(_T("base.ex1"));
	path4.set_Extension(_T("ex2"));

	_ASSERTE(path4.get_Root().IsEmpty());
#ifdef OK_SYS_WINDOWS
	_ASSERTE(s_strcmp(path4.get_Directory().GetString(), _T("directory\\")) == 0);
#endif
#ifdef OK_SYS_UNIX
	_ASSERTE(s_strcmp(path4.get_Directory().GetString(), _T("directory/")) == 0);
#endif
	_ASSERTE(s_strcmp(path4.get_Filename().GetString(), _T("base.ex2")) == 0);
	_ASSERTE(s_strcmp(path4.get_Basename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path4.get_PureBasename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path4.get_Extension().GetString(), _T("ex2")) == 0);
	_ASSERTE(s_strcmp(path4.get_FullExtension().GetString(), _T("ex2")) == 0);
	_ASSERTE(path4.is_File());
	_ASSERTE(!(path4.is_Directory()));
	_ASSERTE(path4.is_Relative());
	_ASSERTE(!(path4.is_Absolute()));
	
	path4.set_PureBasename(_T("base"));
	path4.set_FullExtension(_T("ex1.ex2"));

	_ASSERTE(path4.get_Root().IsEmpty());
#ifdef OK_SYS_WINDOWS
	_ASSERTE(s_strcmp(path4.get_Directory().GetString(), _T("directory\\")) == 0);
#endif
#ifdef OK_SYS_UNIX
	_ASSERTE(s_strcmp(path4.get_Directory().GetString(), _T("directory/")) == 0);
#endif
	_ASSERTE(s_strcmp(path4.get_Filename().GetString(), _T("base.ex1.ex2")) == 0);
	_ASSERTE(s_strcmp(path4.get_Basename().GetString(), _T("base.ex1")) == 0);
	_ASSERTE(s_strcmp(path4.get_PureBasename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path4.get_Extension().GetString(), _T("ex2")) == 0);
	_ASSERTE(s_strcmp(path4.get_FullExtension().GetString(), _T("ex1.ex2")) == 0);
	_ASSERTE(path4.is_File());
	_ASSERTE(!(path4.is_Directory()));
	_ASSERTE(path4.is_Relative());
	_ASSERTE(!(path4.is_Absolute()));
	
#ifdef OK_SYS_WINDOWS
	path4.set_Root(_T("c:"));

	_ASSERTE(s_strcmp(path4.get_Root().GetString(), _T("c:\\")) == 0);
	_ASSERTE(s_strcmp(path4.get_Directory().GetString(), _T("directory\\")) == 0);
#endif
#ifdef OK_SYS_UNIX
	path4.set_Root(_T("/"));

	_ASSERTE(s_strcmp(path4.get_Root().GetString(), _T("/")) == 0);
	_ASSERTE(s_strcmp(path4.get_Directory().GetString(), _T("directory/")) == 0);
#endif
	_ASSERTE(s_strcmp(path4.get_Filename().GetString(), _T("base.ex1.ex2")) == 0);
	_ASSERTE(s_strcmp(path4.get_Basename().GetString(), _T("base.ex1")) == 0);
	_ASSERTE(s_strcmp(path4.get_PureBasename().GetString(), _T("base")) == 0);
	_ASSERTE(s_strcmp(path4.get_Extension().GetString(), _T("ex2")) == 0);
	_ASSERTE(s_strcmp(path4.get_FullExtension().GetString(), _T("ex1.ex2")) == 0);
	_ASSERTE(path4.is_File());
	_ASSERTE(!(path4.is_Directory()));
	_ASSERTE(!(path4.is_Relative()));
	_ASSERTE(path4.is_Absolute());

	_ASSERTE(CFilePath::CheckFilename(_T("legal_name")));
	_ASSERTE(CFilePath::TrimFilename(_T("legal_name")) == CStringLiteral(_T("legal_name")));
	_ASSERTE(!(CFilePath::CheckFilename(_T("illegal:_name"))));
	_ASSERTE(CFilePath::TrimFilename(_T("illegal:_name")) == CStringLiteral(_T("illegal_name")));
	_ASSERTE(!(CFilePath::CheckFilename(_T("ill\\egal:_name"))));
	_ASSERTE(CFilePath::TrimFilename(_T("ill\\egal:_name")) == CStringLiteral(_T("illegal_name")));
	_ASSERTE(!(CFilePath::CheckFilename(_T("NUL.illegal_name"))));
	_ASSERTE(CFilePath::TrimFilename(_T("NUL.illegal_name")) == CStringLiteral(_T("illegal_name")));
	_ASSERTE(!(CFilePath::CheckFilename(_T("NUL"))));
	_ASSERTE(CFilePath::TrimFilename(_T("NUL")).IsEmpty());
	_ASSERTE(!(CFilePath::CheckFilename(_T("NUL.COM1.illegal_name"))));
	_ASSERTE(CFilePath::TrimFilename(_T("NUL.COM1.illegal_name")) == CStringLiteral(_T("illegal_name")));

    COUT << _T("Print current directory content.") << endl;
	try
	{
		CFilePath path(__FILE__LINE__ _T("*"));
		CDirectoryIterator it(path);

		COUT << _T("TestDirectoryIterator: path.get_Path() = ") << path.get_Path() << endl;
		while ( it )
		{
			COUT << _T("TestDirectoryIterator: it.get_Path() = ") << it.get_Path() << endl;
			++it;
		}
	}
	catch ( CDirectoryIteratorException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
    COUT << endl;

#ifdef OK_SYS_WINDOWS
	COUT << _T("Print current directory content (filtered).") << endl;
	try
	{
		CFilePath path(__FILE__LINE__ _T("*"));
		CDirectoryIterator it(path);
		CStringBuffer buf;

		COUT << _T("TestDirectoryIterator: path.get_Path() = ") << path.get_Path() << endl;
		while ( it )
		{
			COUT << _T("TestDirectoryIterator: it.get_Path() = ") << it.get_Path();
			buf = it.get_Name();
			if ( s_strxcmp(buf.GetString(), _T("Program?.c*")) )
				COUT << _T(" match ") << buf;
			COUT << endl;
			++it;
		}
	}
	catch ( CDirectoryIteratorException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
    COUT << endl;

#endif
#ifdef OK_SYS_UNIX
	COUT << _T("Print current directory content (filtered).") << endl;
	try
	{
		CFilePath path(__FILE__LINE__ _T("lib*sources.*"));
		CDirectoryIterator it(path);
		CStringBuffer buf;

		while ( it )
		{
			COUT << _T("TestDirectoryIterator: it.get_Path() = ") << it.get_Path() << endl;
			buf = it.get_Name();
            COUT << buf << endl;
			++it;
		}
	}
	catch ( CDirectoryIteratorException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
    COUT << endl;

#endif

    COUT << _T("MakeDir") << endl;
	try
	{
		CFilePath curDir;

		CDirectoryIterator::GetCurrentDirectory(curDir);

        COUT << _T("curDir = ") << curDir.GetString() << endl;
        
#ifdef OK_SYS_WINDOWS
		CFilePath mkDir(__FILE__LINE__ _T("C:\\TEST\\SUBTEST\\SUBSUBTEST\\"));
#endif
#ifdef OK_SYS_UNIX
		CFilePath mkDir(__FILE__LINE__ _T("/TEST/SUBTEST/SUBSUBTEST/"));
#endif

		CDirectoryIterator::MakeDirectory(mkDir);

	}
	catch ( CDirectoryIteratorException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}

	try
	{
		CDiskFile file;
		CFilePath filePath;

#ifdef OK_SYS_WINDOWS
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST\\SUBSUBTEST\\SubSubTest.txt"));
#endif
#ifdef OK_SYS_UNIX
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST/SUBSUBTEST/SubSubTest.txt"));
#endif
		file.Create(filePath);

		file.SetSize(123456LL);
		file.Close();

#ifdef OK_SYS_WINDOWS
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST\\SUBSUBTEST\\SubSubTest1.txt"));
#endif
#ifdef OK_SYS_UNIX
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST/SUBSUBTEST/SubSubTest1.txt"));
#endif
		file.Create(filePath);

		file.SetSize(123456LL);
		file.Close();

#ifdef OK_SYS_WINDOWS
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST\\SubTest.txt"));
#endif
#ifdef OK_SYS_UNIX
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST/SubTest.txt"));
#endif
		file.Create(filePath);

		file.SetSize(123456LL);
		file.Close();

#ifdef OK_SYS_WINDOWS
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST\\SubTest1.txt"));
#endif
#ifdef OK_SYS_UNIX
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST/SubTest1.txt"));
#endif
		file.Create(filePath);

		file.SetSize(123456LL);
		file.Close();

	}
	catch ( CFileException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}

    COUT << _T("RmDir") << endl;
	try
	{
		CFilePath filePath;

#ifdef OK_SYS_WINDOWS
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST\\SUBSUBTEST\\SubSubTest.txt"));
        _ASSERTE(CDirectoryIterator::FileExists(filePath));
		filePath.set_Path(__FILE__LINE__  _T("C:\\TEST\\SUBTEST\\SUBSUBTEST\\SubSubTest2.txt"));
        _ASSERTE(!(CDirectoryIterator::FileExists(filePath)));

		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST\\"));
		_ASSERTE(CDirectoryIterator::DirectoryExists(filePath) == 2);
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST2\\"));
        _ASSERTE(CDirectoryIterator::DirectoryExists(filePath) < 0);
#endif
#ifdef OK_SYS_UNIX
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST/SUBSUBTEST/SubSubTest.txt"));
        _ASSERTE(CDirectoryIterator::FileExists(filePath));
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST/SUBSUBTEST/SubSubTest2.txt"));
        _ASSERTE(!(CDirectoryIterator::FileExists(filePath)));

		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST/"));
		_ASSERTE(CDirectoryIterator::DirectoryExists(filePath) > 0);
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST2/"));
        _ASSERTE(CDirectoryIterator::DirectoryExists(filePath) < 0);
#endif
      
		CFilePath curDir;

		CDirectoryIterator::GetCurrentDirectory(curDir);
        
        COUT << _T("curDir = ") << curDir.GetString() << endl;
        
#ifdef OK_SYS_WINDOWS
		CFilePath rmDir(__FILE__LINE__ _T("C:\\TEST\\"));
        
		CDirectoryIterator::RenameDirectory(rmDir, CStringBuffer(__FILE__LINE__ _T("SUBTEST")), CStringBuffer(__FILE__LINE__ _T("SUBTEST1")));
        
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST1\\SUBSUBTEST\\SubSubTest.txt"));
        _ASSERTE(CDirectoryIterator::FileExists(filePath));
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST1\\SUBSUBTEST\\SubSubTest2.txt"));
        _ASSERTE(!(CDirectoryIterator::FileExists(filePath)));
#endif
#ifdef OK_SYS_UNIX
		CFilePath rmDir(__FILE__LINE__ _T("/TEST/"));
        
		CDirectoryIterator::RenameDirectory(rmDir, CStringBuffer(__FILE__LINE__ _T("SUBTEST")), CStringBuffer(__FILE__LINE__ _T("SUBTEST1")));
        
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST1/SUBSUBTEST/SubSubTest.txt"));
        _ASSERTE(CDirectoryIterator::FileExists(filePath));
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST1/SUBSUBTEST/SubSubTest2.txt"));
        _ASSERTE(!(CDirectoryIterator::FileExists(filePath)));
#endif

        CFilePath filePath1;
        
#ifdef OK_SYS_WINDOWS
		filePath.set_Path(__FILE__LINE__ _T("C:\\TEST\\SUBTEST1\\SUBSUBTEST\\SubSubTest.txt"));
        filePath1.set_Path(__FILE__LINE__ _T("C:\\TEST\\SubTest99.txt"));
#endif
#ifdef OK_SYS_UNIX
		filePath.set_Path(__FILE__LINE__ _T("/TEST/SUBTEST1/SUBSUBTEST/SubSubTest.txt"));
        filePath1.set_Path(__FILE__LINE__ _T("/TEST/SubTest99.txt"));
#endif
        CDirectoryIterator::MoveFile(filePath, filePath1);
        _ASSERTE(!(CDirectoryIterator::FileExists(filePath)));
        _ASSERTE(CDirectoryIterator::FileExists(filePath1));
        
		CDirectoryIterator::RemoveDirectory(rmDir);

		CDirectoryIterator::GetCurrentDirectory(curDir);
        
        COUT << _T("curDir = ") << curDir.GetString() << endl;
	}
	catch ( CDirectoryIteratorException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}

	CVolumeIterator volInfo;

	COUT << _T("Available drives are: ") << endl;

	volInfo.Begin();
	while ( volInfo )
	{
		COUT << volInfo.get_Path().get_Path() << endl;
		volInfo.Next();
	}
	volInfo.Close();
}

static void ScanDirectoryHelper(CStringLiteral dir, CStringLiteral pattern)
{
	try
	{
		CFilePath path;

		path.set_Directory(dir);
		path.set_Basename(_T("*"));

		CDirectoryIterator it(path);
		CStringBuffer buf;

		while ( it )
		{
			buf = it.get_Name();
			if ( it.is_SubDir() )
			{
				buf.PrependString(path.get_Directory());
				buf.PrependString(path.get_Root());
				ScanDirectoryHelper(buf.GetString(), pattern);
			}
			else if (s_strxcmp(buf.GetString(), pattern))
			{
				path = it.get_Path();

				COUT << path.get_Path() << endl;

				Ptr(CFile) f = OK_NEW_OPERATOR CDiskFile(path);

				f->Read(buf, 256);

				COUT << buf << endl;

				f->Close();
				f->release();
			}
			++it;
		}
	}
	catch ( CDirectoryIteratorException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
}

void ScanDirectory(TMBCharList& dirs, CStringLiteral pattern)
{
	if ( pattern.IsEmpty() )
		pattern = _T("*");
	if ( dirs.Count() == 0 )
	{
		CFilePath sDir;

		try
		{
			CDirectoryIterator::GetCurrentDirectory(sDir);
		}
		catch ( CDirectoryIteratorException* ex )
		{
			COUT << ex->GetExceptionMessage() << endl;
			return;
		}
		if ( sDir.IsEmpty() )
			return;
		ScanDirectoryHelper(sDir.GetString(), pattern);
		return;
	}

	TMBCharList::Iterator it = dirs.Begin();

	while ( it )
	{
		ScanDirectoryHelper(*it, pattern);
		++it;
	}
}