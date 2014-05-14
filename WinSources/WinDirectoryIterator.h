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
#include "WinException.h"
#include "FilePath.h"
#include "SystemTime.h"

#ifdef OK_SYS_WINDOWS
#include <io.h>
#endif
#ifdef OK_SYS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#define FILETIME time_t
#endif
#ifdef OK_COMP_MSC
typedef dword mode_t;
#endif
#undef GetCurrentDirectory
#undef SetCurrentDirectory
#undef RemoveDirectory
#undef CopyFile

class WINSOURCES_API CWinDirectoryIterator: public CCppObject
{
public:
	CWinDirectoryIterator();
	CWinDirectoryIterator(ConstRef(CFilePath) path);
	virtual ~CWinDirectoryIterator(void);

	static void GetCurrentDirectory(Ref(CFilePath) _path);
	static void SetCurrentDirectory(ConstRef(CFilePath) _path);
	static void MakeDirectory(ConstRef(CFilePath) _path, mode_t mode = 0755);
	/* UnlinkFile deletes the link if _path is a symbolic link. */
	static void UnlinkFile(ConstRef(CFilePath) _path);
	static void UnlinkDirectory(ConstRef(CFilePath) _path);
	static void RemoveDirectory(ConstRef(CFilePath) path);
	static void RemoveFile(ConstRef(CFilePath) path);
	static void LinkFile(ConstRef(CFilePath) _from, ConstRef(CFilePath) _to);
	static void SymLinkFile(ConstRef(CFilePath) _from, ConstRef(CFilePath) _to);
	static void Rename(ConstRef(CFilePath) _from, ConstRef(CFilePath) _to);
	static void CopyFile(ConstRef(CFilePath) _from, ConstRef(CFilePath) _to, mode_t mode = 0644);
	/* FileExists finds the link if _path is a symbolic link. */
	static bool FileExists(ConstRef(CFilePath) _path);
	static int DirectoryExists(ConstRef(CFilePath) _path);

#ifdef OK_SYS_WINDOWS
	enum TFileAttributes
	{
		ATTRIBUTE_NORMAL = 0x00,
		ATTRIBUTE_READONLY = FILE_ATTRIBUTE_READONLY,
		ATTRIBUTE_HIDDEN = FILE_ATTRIBUTE_HIDDEN,
		ATTRIBUTE_SYSTEM = FILE_ATTRIBUTE_SYSTEM,
		ATTRIBUTE_DIRECTORY = FILE_ATTRIBUTE_DIRECTORY,
		ATTRIBUTE_ARCHIVE = FILE_ATTRIBUTE_ARCHIVE
	};
#endif
#ifdef OK_SYS_UNIX
	enum TFileAttributes
	{
		ATTRIBUTE_READ_OWNER = S_IRUSR,
		ATTRIBUTE_WRITE_OWNER = S_IWUSR,
		ATTRIBUTE_EXECUTE_OWNER = S_IXUSR,
		ATTRIBUTE_READ_GROUP = S_IRGRP,
		ATTRIBUTE_WRITE_GROUP = S_IWGRP,
		ATTRIBUTE_EXECUTE_GROUP = S_IXGRP,
		ATTRIBUTE_READ_OTHERS = S_IROTH,
		ATTRIBUTE_WRITE_OTHERS = S_IWOTH,
		ATTRIBUTE_EXECUTE_OTHERS = S_IXOTH
	};
#endif

	/* ReadFileAttributes reads the attributes of the link if _path is a symbolic link. */
	static dword ReadFileAttributes(ConstRef(CFilePath) _path);
	/* WriteFileAttributes sets the attributes of the link if _path is a symbolic link. */
	static void WriteFileAttributes(ConstRef(CFilePath) _path, dword attrs);
	static void _ReadFileTimes(ConstRef(CFilePath) _path, Ptr(FILETIME) CreationTime, Ptr(FILETIME) LastAccessTime, Ptr(FILETIME) LastWriteTime);
	static void ReadFileTimes(ConstRef(CFilePath) _path, Ref(CSystemTime) CreationTime, Ref(CSystemTime) LastAccessTime, Ref(CSystemTime) LastWriteTime);
	static void _WriteFileTimes(ConstRef(CFilePath) _path, Ptr(FILETIME) CreationTime, Ptr(FILETIME) LastAccessTime, Ptr(FILETIME) LastWriteTime);
	static void WriteFileTimes(ConstRef(CFilePath) _path, ConstRef(CSystemTime) CreationTime, ConstRef(CSystemTime) LastAccessTime, ConstRef(CSystemTime) LastWriteTime);
	static bool FileSystemIsNTFS(ConstRef(CFilePath) path);
	static bool FileIsSymbolicLink(ConstRef(CFilePath) path);

	void Open(ConstRef(CFilePath) path);
	void Next();
	void Close();

	__inline operator bool() const 
	{ 
#ifdef OK_SYS_WINDOWS
		return _hFileFind != INVALID_HANDLE_VALUE;
#endif
#ifdef OK_SYS_UNIX
		return NotPtrCheck(_hFileFind);
#endif
	}
	__inline CWinDirectoryIterator& operator++()
	{ 
		Next(); 
		return *this; 
	}

	WBool is_Archive();
	WBool is_Hidden();
	WBool is_ReadOnly();
	WBool is_SubDir();
	WBool is_System();
	
	CSystemTime get_CreateTime();
	CSystemTime get_LastAccessTime();
	CSystemTime get_LastWriteTime();

	sqword get_FileSize();
	CStringBuffer get_Name();
	CStringBuffer get_Path();

private:
	CFilePath _filePath;
#ifdef OK_SYS_WINDOWS
	HANDLE _hFileFind;
	WIN32_FIND_DATA _findData;
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
	CStringBuffer _get_Path();

};

DECL_WINEXCEPTION(WINSOURCES_API, CWinDirectoryIteratorException, CWinException)

