/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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

#include "CppSources.h"
#include "FilePath.h"
#include "BaseException.h"
#include "okDateTime.h"

#ifdef OK_SYS_WINDOWS
#include <io.h>
#endif
#ifdef OK_SYS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#endif
#undef GetCurrentDirectory
#undef SetCurrentDirectory
#undef MoveFile
#undef CopyFile
#undef RemoveDirectory

class CPPSOURCES_API CDirectoryIterator: public CCppObject
{
public:
	CDirectoryIterator();
	CDirectoryIterator(ConstRef(CFilePath) path);
	virtual ~CDirectoryIterator(void);

	static void GetCurrentDirectory(Ref(CFilePath) path);
	static void SetCurrentDirectory(ConstRef(CFilePath) path);
	static void MakeDirectory(ConstRef(CFilePath) path);
	static void RemoveDirectory(ConstRef(CFilePath) path);
	static void RemoveFile(ConstRef(CFilePath) path);
    static bool FileExists(ConstRef(CFilePath) path);
    static int DirectoryExists(ConstRef(CFilePath) path);
    static void RenameDirectory(ConstRef(CFilePath) basepath, ConstRef(CStringBuffer) oldname, ConstRef(CStringBuffer) newname);
    static void MoveFile(ConstRef(CFilePath) from, ConstRef(CFilePath) to);
    static void CopyFile(ConstRef(CFilePath) from, ConstRef(CFilePath) to);

	static mbchar DefaultPathSeparatorChar();
	static CConstPointer DefaultPathSeparatorString();
	static mbchar WinPathSeparatorChar();
	static CConstPointer WinPathSeparatorString();
	static mbchar UnixPathSeparatorChar();
	static CConstPointer UnixPathSeparatorString();

	void Open(ConstRef(CFilePath) path);
	void Next();
	void Close();

	__inline operator bool() const 
	{ 
#ifdef OK_SYS_WINDOWS
		return _hFileFind != -1L;
#endif
#ifdef OK_SYS_UNIX
		return NotPtrCheck(_hFileFind);
#endif
	}
	__inline CDirectoryIterator& operator++() 
	{ 
		Next(); 
		return *this; 
	}

	WBool is_Archive();
	WBool is_Hidden();
	WBool is_ReadOnly();
	WBool is_SubDir();
	WBool is_System();
	
	CDateTime get_CreateTime();
	CDateTime get_LastAccessTime();
	CDateTime get_LastWriteTime();

	sqword get_FileSize();
	CStringBuffer get_Name();
	CStringBuffer get_Path();

	mbchar PathSeparatorChar();
	CConstPointer PathSeparatorString();

private:
	CFilePath _filePath;
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	intptr_t _hFileFind;
#ifdef _UNICODE
	_wfinddata64_t _tFileFind;
#else
	_finddata64_t _tFileFind;
#endif	
#endif	
#ifdef OK_SYS_UNIX
	DIR *_hFileFind;
	struct dirent *_tFileFind;
	struct stat _tStatus;
	bool _bStatus;
	dword _tAccess;
	bool _bAccess;
	CStringBuffer _sFilter;
	
	void _load_status();
	void _test_access();
#endif
#endif
#ifdef OK_COMP_MSC
	intptr_t _hFileFind;
	_tfinddata64_t _tFileFind;
#endif
	CStringBuffer _get_Path();

	static CConstPointer _sWinPathSeparator;
	static CConstPointer _sUnixPathSeparator;
};

DECL_EXCEPTION(CPPSOURCES_API, CDirectoryIteratorException, CBaseException)

