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
#include "CPPS_PCH.H"
#include "DirectoryIterator.h"
#include "File.h"

#ifdef OK_SYS_WINDOWS
#include <direct.h>

#define FAIL_COND (_hFileFind < 0)
#endif
#ifdef OK_SYS_UNIX
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#define FAIL_COND PtrCheck(_hFileFind)
#endif

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS64
#ifdef _UNICODE
#define _tfinddata64_t _wfinddata64_t
#define _tfindfirst64 _wfindfirst64
#define _tfindnext64 _wfindnext64
#else
#define _tfinddata64_t _finddata64_t
#define _tfindfirst64 _findfirst64
#define _tfindnext64 _findnext64
#endif
#endif
#ifdef OK_SYS_WINDOWS32
#undef _tfinddata64_t
#if __MINGW32_MAJOR_VERSION < 4
#ifdef _UNICODE
#define _tfinddata64_t    __wfinddata64_t
#define _tfindfirst64 _wfindfirst64
#define _tfindnext64 _wfindnext64
#else
#define _tfinddata64_t    __finddata64_t
#define _tfindfirst64 _findfirst64
#define _tfindnext64 _findnext64
#endif
#else
#ifdef _UNICODE
#define _tfinddata64_t    _wfinddata64_t
#define _tfindfirst64 _wfindfirst64
#define _tfindnext64 _wfindnext64
#else
#define _tfinddata64_t    _finddata64_t
#define _tfindfirst64 _findfirst64
#define _tfindnext64 _findnext64
#endif
#endif
#endif
#endif

IMPL_EXCEPTION(CDirectoryIteratorException, CBaseException)

CConstPointer CDirectoryIterator::_sWinPathSeparator = _T("\\");
CConstPointer CDirectoryIterator::_sUnixPathSeparator = _T("/");

CDirectoryIterator::CDirectoryIterator():
	_filePath(),
#ifdef OK_SYS_WINDOWS
	_hFileFind(-1L)
#endif
#ifdef OK_SYS_UNIX
	_hFileFind(NULL),
	_tFileFind(NULL),
	_bStatus(false),
	_bAccess(false)
#endif
{
}

CDirectoryIterator::CDirectoryIterator(ConstRef(CFilePath) path):
	_filePath(),
#ifdef OK_SYS_WINDOWS
	_hFileFind(-1L)
#endif
#ifdef OK_SYS_UNIX
	_hFileFind(NULL),
	_tFileFind(NULL),
	_bStatus(false),
	_bAccess(false)
#endif
{
	Open(path);
}

CDirectoryIterator::~CDirectoryIterator(void)
{
	Close();
}

void CDirectoryIterator::GetCurrentDirectory(Ref(CFilePath) path)
{
    CPointer buffer;

#ifdef OK_SYS_WINDOWS
	if (NULL == (buffer = _tgetcwd(NULL, 0)))
#endif
#ifdef OK_SYS_UNIX
	if (NULL == (buffer = getcwd(NULL, 0)))
#endif
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException"), 
		    _T("CDirectoryIterator::GetCurrentDirectory"), errno);
	path.set_Path(__FILE__LINE__ buffer);
	free(buffer);

	path.MakeDirectory();
}

void CDirectoryIterator::SetCurrentDirectory(ConstRef(CFilePath) path)
{
	CFilePath tmpPath(path);

	if ( tmpPath.is_Relative() )
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();

#ifdef OK_SYS_WINDOWS
	if (0 > _tchdir(tmpPath.GetString()))
#endif
#ifdef OK_SYS_UNIX
	if (0 > chdir(tmpPath.GetString()))
#endif
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
		    _T("CDirectoryIterator::SetCurrentDirectory"), path.GetString(), errno);
}

void CDirectoryIterator::MakeDirectory(ConstRef(CFilePath) path)
{
	CFilePath tmpPath(path);

	if ( tmpPath.is_Relative() )
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();

#ifdef OK_SYS_WINDOWS
	if (0 > _tmkdir(tmpPath.GetString()))
#endif
#ifdef OK_SYS_UNIX
	if (0 > mkdir(tmpPath.GetString(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
#endif
	{
		switch ( errno )
		{
		case EEXIST:
			return;
		case ENOENT:
			break;
		default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::MakeDirectory"), tmpPath.GetString(), errno);
			break;
		}
	}
	else
		return;

	CStringBuffer tmpRoot(tmpPath.get_Root());
	CStringBuffer tmpDir(tmpPath.get_Directory());
	CPointer splitArray[64];
	dword maxsplitArraySize = 64;
	dword splitArraySize;
	dword i = 0;

	tmpDir.SplitAny(DefaultPathSeparatorString(), splitArray, maxsplitArraySize, &splitArraySize);
	if ( splitArraySize > 0 )
		--splitArraySize;
	while ( i < splitArraySize )
	{
        tmpRoot += splitArray[i];
        tmpRoot += DefaultPathSeparatorString();
        ++i;

#ifdef OK_SYS_WINDOWS
		if (0 > _tmkdir(tmpRoot.GetString()))
#endif
#ifdef OK_SYS_UNIX
		if (0 > mkdir(tmpRoot.GetString(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
#endif
        {
            switch ( errno )
            {
            case EEXIST:	
                break;
            case ENOENT:
            default:
				throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
					_T("CDirectoryIterator::MakeDirectory"), tmpRoot.GetString(), errno);
                break;
            }
        }
	}
}

void CDirectoryIterator::RemoveDirectory(ConstRef(CFilePath) path)
{
	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();

#ifdef OK_SYS_WINDOWS
	if (0 > _trmdir(tmpPath.GetString()))
#endif
#ifdef OK_SYS_UNIX
	if (0 > rmdir(tmpPath.GetString()))
#endif
	{
		switch ( errno )
		{
		case ENOTEMPTY:
			break;
		case ENOENT:
		case EACCES:
		default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::RemoveDirectory"), tmpPath.GetString(), errno);
			break;
		}
	}
	else
		return;

	CFilePath curDir;
	CDirectoryIterator it;
	CFilePath wildcard(__FILE__LINE__ _T("*"));
	 
	try
	{
		GetCurrentDirectory(curDir);
		SetCurrentDirectory(tmpPath);

		it.Open(wildcard);
		while ( it )
		{
			if ( it.is_SubDir() )
			{
				CStringBuffer tmp(it.get_Name());

				if ( (tmp != CStringLiteral(_T("."))) && (tmp != CStringLiteral(_T(".."))) )
				{
					tmp += DefaultPathSeparatorString();

					CFilePath subDir(tmp);

					RemoveDirectory(subDir);
				}
			}
			else
			{
				CFilePath file(it.get_Name());

				RemoveFile(file);
			}
			++it;
		}
		SetCurrentDirectory(curDir);
		RemoveDirectory(tmpPath);
	}
	catch ( CDirectoryIteratorException* ex )
	{
		throw ex;
	}
}

void CDirectoryIterator::RemoveFile(ConstRef(CFilePath) path)
{
	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();

#ifdef OK_SYS_WINDOWS
	if (0 > _tunlink(tmpPath.GetString()))
#endif
#ifdef OK_SYS_UNIX
	if (0 > unlink(tmpPath.GetString()))
#endif
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
		    _T("CDirectoryIterator::RemoveFile"), path.GetString(), errno);
}

bool CDirectoryIterator::FileExists(ConstRef(CFilePath) path)
{
	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();

#ifdef OK_SYS_WINDOWS
	intptr_t hFileFind;
    _tfinddata64_t tFileFind;
	
	hFileFind = _tfindfirst64(tmpPath.GetString(), &tFileFind);
    if ( 0 > hFileFind )
    {
        switch ( errno )
        {
        case ENOENT:
            return false;
        default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::FileExists"), tmpPath.GetString(), errno);
            return false;
        }
    }
    _findclose(hFileFind);
	if (TestBit(tFileFind.attrib, _A_SUBDIR))
		return false;
#endif
#ifdef OK_SYS_UNIX
	DIR *hFileFind;
	struct dirent *tFileFind;
	CFilePath dir;

	dir.set_Root(tmpPath.get_Root());
	dir.set_Directory(tmpPath.get_Directory());
	if ( dir.get_Path().IsEmpty() )
		dir.set_Directory(_T("./"));
	
	hFileFind = opendir(dir.GetString());
	if ( PtrCheck(hFileFind) )
    {
        switch ( errno )
        {
        case ENOENT:
            return false;
        default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::FileExists"), tmpPath.GetString(), errno);
            return false;
        }
    }

	CStringBuffer fname(path.get_Filename());

	if ( fname.IsEmpty() )
	{
		closedir(hFileFind);
		return false;
	}
	do
	{
		tFileFind = readdir(hFileFind);
		if ( PtrCheck(tFileFind) )
		{
			closedir(hFileFind);
			return false;
		}
	} while ( s_strcmp(tFileFind->d_name, fname.GetString()) != 0 );
	closedir(hFileFind);
#endif
    return true;
}

int CDirectoryIterator::DirectoryExists(ConstRef(CFilePath) path)
{
	int cnt = 0;
	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();

#ifdef OK_SYS_WINDOWS
	tmpPath.set_Filename(_T("*"));

    intptr_t hFileFind;
    _tfinddata64_t tFileFind;

	hFileFind = _tfindfirst64(tmpPath.GetString(), &tFileFind);
    if ( 0 > hFileFind )
    {
        switch ( errno )
        {
        case ENOENT:
            return -1;
        default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::DirectoryExists"), tmpPath.GetString(), errno);
			return -1;
        }
    }

    int res;

    while ( true )
    {
        res = _tfindnext64(hFileFind, &tFileFind);
        if ( res < 0 )
        {
            switch ( errno )
            {
            case ENOENT:
                _findclose(hFileFind);
                return cnt;
            default:
				throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
					_T("CDirectoryIterator::DirectoryExists"), tmpPath.GetString(), errno);
                return -1;
            }
        }
        if ( (!TestBit(tFileFind.attrib, _A_HIDDEN))
             && (!TestBit(tFileFind.attrib, _A_SYSTEM))
             && (!TestBit(tFileFind.attrib, _A_SUBDIR))
             && (s_strcmp(tFileFind.name, _T(".")) != 0)
             && (s_strcmp(tFileFind.name, _T("..")) != 0) )
            ++cnt;
    }
#endif
#ifdef OK_SYS_UNIX
	DIR *hFileFind;
	struct dirent *tFileFind;

	if ( tmpPath.get_Path().IsEmpty() )
		tmpPath.set_Path(__FILE__LINE__ _T("./"));

	hFileFind = opendir(tmpPath.GetString());
	if ( PtrCheck(hFileFind) )
    {
        switch ( errno )
        {
        case ENOENT:
            return -1;
        default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::DirectoryExists"), tmpPath.GetString(), errno);
            return -1;
        }
    }
	while ( true )
	{
		tFileFind = readdir(hFileFind);
		if ( PtrCheck(tFileFind) )
		{
			closedir(hFileFind);
			return cnt;
		}
		if ( (s_strcmp(tFileFind->d_name, _T(".")) != 0)
			&& (s_strcmp(tFileFind->d_name, _T("..")) != 0) )
			++cnt;
	}
#endif
    return cnt;
}

void CDirectoryIterator::RenameDirectory(ConstRef(CFilePath) basepath, ConstRef(CStringBuffer) oldname, ConstRef(CStringBuffer) newname)
{
	CFilePath tmpPath(basepath);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();

	CFilePath from(tmpPath);
	CFilePath to(tmpPath);

    from.set_Filename(oldname.GetString());
    to.set_Filename(newname.GetString());

#ifdef OK_SYS_WINDOWS
	if (_trename(from.GetString(), to.GetString()))
#endif
#ifdef OK_SYS_UNIX
	if (0 > rename(from.GetString(), to.GetString()))
#endif
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, frompath = '%s', topath = '%s'"), 
		    _T("CDirectoryIterator::RenameDirectory"), from.GetString(), to.GetString(), errno);
}

void CDirectoryIterator::MoveFile(ConstRef(CFilePath) from, ConstRef(CFilePath) to)
{
	CFilePath tmpPath1(from);
	CFilePath tmpPath2(to);

	if (tmpPath1.is_Relative())
		tmpPath1.MakeAbsolute();
	tmpPath1.Normalize();
	if (tmpPath2.is_Relative())
		tmpPath2.MakeAbsolute();
	tmpPath2.Normalize();

	if (tmpPath1.is_Directory())
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s' is directory"), 
		    _T("CDirectoryIterator::MoveFile"), from.GetString(), EINVAL);
	if (tmpPath2.is_Directory())
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s' is directory"), 
		    _T("CDirectoryIterator::MoveFile"), to.GetString(), EINVAL);
#ifdef OK_SYS_WINDOWS
	if (_trename(tmpPath1.GetString(), tmpPath2.GetString()))
#endif
#ifdef OK_SYS_UNIX
	if (0 > rename(tmpPath1.GetString(), tmpPath2.GetString()))
#endif 
        throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, frompath = '%s', topath = '%s'"), 
		    _T("CDirectoryIterator::MoveFile"), from.GetString(), to.GetString(), errno);
}

void CDirectoryIterator::CopyFile(ConstRef(CFilePath) from, ConstRef(CFilePath) to)
{
	CFilePath tmpPath1(from);
	CFilePath tmpPath2(to);

	if (tmpPath1.is_Relative())
		tmpPath1.MakeAbsolute();
	tmpPath1.Normalize();
	if (tmpPath2.is_Relative())
		tmpPath2.MakeAbsolute();
	tmpPath2.Normalize();

	if (tmpPath1.is_Directory())
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s' is directory"), 
		    _T("CDirectoryIterator::CopyFile"), from.GetString(), EINVAL);
	if (tmpPath2.is_Directory())
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s' is directory"), 
		    _T("CDirectoryIterator::CopyFile"), to.GetString(), EINVAL);
	if (FileExists(tmpPath2))
		RemoveFile(tmpPath2);

	CDiskFile fromf(tmpPath1, true, false, CFile::BinaryFile_NoEncoding);
	CDiskFile tof;
	CByteBuffer buffer(__FILE__LINE__ 4096);

	tof.Create(tmpPath2, false, CFile::BinaryFile_NoEncoding);
	fromf.Read(buffer);
	while ( buffer.get_BufferSize() > 0 )
	{
		tof.Write(buffer);
		fromf.Read(buffer);
	}
	tof.Close();
	fromf.Close();
}

mbchar CDirectoryIterator::DefaultPathSeparatorChar()
{
#ifdef OK_SYS_WINDOWS
	return *_sWinPathSeparator;
#endif
#ifdef OK_SYS_UNIX
	return *_sUnixPathSeparator;
#endif
}

CConstPointer CDirectoryIterator::DefaultPathSeparatorString()
{
#ifdef OK_SYS_WINDOWS
	return _sWinPathSeparator;
#endif
#ifdef OK_SYS_UNIX
	return _sUnixPathSeparator;
#endif
}

mbchar CDirectoryIterator::WinPathSeparatorChar()
{
	return *_sWinPathSeparator;
}

CConstPointer CDirectoryIterator::WinPathSeparatorString()
{
	return _sWinPathSeparator;
}

mbchar CDirectoryIterator::UnixPathSeparatorChar()
{
	return *_sUnixPathSeparator;
}

CConstPointer CDirectoryIterator::UnixPathSeparatorString()
{
	return _sUnixPathSeparator;
}

mbchar CDirectoryIterator::PathSeparatorChar()
{
	return *(_filePath.get_PathSeparator());
}

CConstPointer CDirectoryIterator::PathSeparatorString()
{
	return _filePath.get_PathSeparator();
}

void CDirectoryIterator::Open(ConstRef(CFilePath) path)
{
	if ( !FAIL_COND )
		Close();

	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();

	_filePath = tmpPath;
#ifdef OK_SYS_WINDOWS
	assert(PathSeparatorChar() == WinPathSeparatorChar());
#ifdef OK_COMP_GNUC
#ifdef _UNICODE
	_hFileFind = _wfindfirst64(_filePath.GetString(), &_tFileFind);
#else
	_hFileFind = _findfirst64(_filePath.GetString(), &_tFileFind);
#endif
#endif
#ifdef OK_COMP_MSC
	_hFileFind = _tfindfirst64(_filePath.GetString(), &_tFileFind);
#endif
	if ( 0 > _hFileFind )
	{
		switch ( errno )
		{
		case ENOENT:
			return;
		default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::Open"), path.GetString(), errno);
			return;
		}
	}
	while ( (s_strcmp(_tFileFind.name, _T(".")) == 0) || (s_strcmp(_tFileFind.name, _T("..")) == 0) )
	{
#ifdef OK_COMP_GNUC
#ifdef _UNICODE
		int res = _wfindnext64(_hFileFind, &_tFileFind);
#else
		int res = _findnext64(_hFileFind, &_tFileFind);
#endif
#endif
#ifdef OK_COMP_MSC
		int res = _tfindnext64(_hFileFind, &_tFileFind);
#endif        
		if ( res < 0 )
		{
			switch ( errno )
			{
			case ENOENT:
				Close();
				return;
			default:
				throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
					_T("CDirectoryIterator::Open"), path.GetString(), errno);
				return;
			}
		}
	}
#endif
#ifdef OK_SYS_UNIX
	assert(PathSeparatorChar() == UnixPathSeparatorChar());

	CFilePath dir;

	dir.set_Root(_filePath.get_Root());
	dir.set_Directory(_filePath.get_Directory());
	if ( dir.get_Path().IsEmpty() )
		dir.set_Path(__FILE__LINE__ _T("./"));
		
	_hFileFind = opendir(dir.GetString());
	if ( PtrCheck(_hFileFind) )
	{
		switch ( errno )
		{
		case ENOENT:
			return;
		default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::Open"), path.GetString(), errno);
			return;
		}
	}

	_sFilter = _filePath.get_Filename();
	if ( _sFilter.IsEmpty() )
		_sFilter.SetString(__FILE__LINE__ _T("*"));

	do
	{
		_tFileFind = readdir(_hFileFind);
		if ( PtrCheck(_tFileFind) )
		{
			Close();
			return;
		}
	} while ( (s_strcmp(_tFileFind->d_name, _T(".")) == 0) 
		|| (s_strcmp(_tFileFind->d_name, _T("..")) == 0) 
		|| (!(s_strxcmp(_tFileFind->d_name, _sFilter.GetString()))) );
	_bStatus = false;
	_bAccess = false;
#endif
}

void CDirectoryIterator::Next()
{
	if ( FAIL_COND )
		return;

#ifdef OK_SYS_WINDOWS
#ifdef OK_COMP_GNUC
#ifdef _UNICODE
	int res = _wfindnext64(_hFileFind, &_tFileFind);
#else
	int res = _findnext64(_hFileFind, &_tFileFind);
#endif
#endif
#ifdef OK_COMP_MSC
	int res = _tfindnext64(_hFileFind, &_tFileFind);
#endif
        
	if ( res < 0 )
	{
		switch ( errno )
		{
		case ENOENT:
			Close();
			break;
		default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::Next"), _filePath.GetString(), errno);
			break;
		}
	}
#endif
#ifdef OK_SYS_UNIX
	do
	{
		_tFileFind = readdir(_hFileFind);
		if ( PtrCheck(_tFileFind) )
		{
			Close();
			return;
		}
	} while ( !(s_strxcmp(_tFileFind->d_name, _sFilter.GetString())) );
	_bStatus = false;
	_bAccess = false;
#endif
}

void CDirectoryIterator::Close()
{
	if ( FAIL_COND )
		return;
#ifdef OK_SYS_WINDOWS
	_findclose(_hFileFind);
	_hFileFind = -1L;
#endif
#ifdef OK_SYS_UNIX
	closedir(_hFileFind);
	_hFileFind = NULL;
	_bStatus = false;
	_bAccess = false;
#endif
}

WBool CDirectoryIterator::is_Archive() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_tFileFind.attrib, _A_ARCH);
#endif
#ifdef OK_SYS_UNIX
	return false;
#endif
}

WBool CDirectoryIterator::is_Hidden() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_tFileFind.attrib, _A_HIDDEN);
#endif
#ifdef OK_SYS_UNIX
	return _tFileFind->d_name[0] == '.';
#endif
}

WBool CDirectoryIterator::is_ReadOnly() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_tFileFind.attrib, _A_RDONLY);
#endif
#ifdef OK_SYS_UNIX
	_test_access();
	return !(_tAccess & W_OK);
#endif
}

WBool CDirectoryIterator::is_SubDir() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_tFileFind.attrib, _A_SUBDIR);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return S_ISDIR(_tStatus.st_mode); 
#endif
}

WBool CDirectoryIterator::is_System() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_tFileFind.attrib, _A_SYSTEM);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return !(S_ISDIR(_tStatus.st_mode) || S_ISREG(_tStatus.st_mode)); 
#endif
}

CDateTime CDirectoryIterator::get_CreateTime() 
{
	if ( FAIL_COND )
		return CDateTime();
#ifdef OK_SYS_WINDOWS
	return CDateTime(_tFileFind.time_create, -1, CDateTime::UTCTime);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return CDateTime(_tStatus.st_ctime); 
#endif
}

CDateTime CDirectoryIterator::get_LastAccessTime() 
{ 
	if ( FAIL_COND )
		return CDateTime();
#ifdef OK_SYS_WINDOWS
	return CDateTime(_tFileFind.time_access, -1, CDateTime::UTCTime);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return CDateTime(_tStatus.st_atime); 
#endif
}

CDateTime CDirectoryIterator::get_LastWriteTime() 
{ 
	if ( FAIL_COND )
		return CDateTime();
#ifdef OK_SYS_WINDOWS
	return CDateTime(_tFileFind.time_write, -1, CDateTime::UTCTime);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return CDateTime(_tStatus.st_mtime); 
#endif
}

#ifdef OK_SYS_UNIX
void CDirectoryIterator::_load_status()
{
	if ( _bStatus )
		return;
	
	CStringBuffer tmp = _get_Path();
	int res = ::stat(tmp.GetString(), &_tStatus);
	
	if ( res < 0 )
		throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::_load_status"), tmp.GetString(), errno);
	_bStatus = true;
}

void CDirectoryIterator::_test_access()
{
	if ( _bAccess )
		return;
		
	_tAccess = 0;
	
	CStringBuffer tmp = _get_Path();
	int res = ::access(tmp.GetString(), W_OK);
	
	if ( res < 0 )
	{
		switch ( errno )
		{
		case EINVAL:
			break;
		default:
			throw OK_NEW_OPERATOR CDirectoryIteratorException(__FILE__LINE__ _T("in %s CDirectoryIteratorException, path = '%s'"), 
				_T("CDirectoryIterator::_test_access"), tmp.GetString(), errno);
			break;
		}
	}
	if ( res == 0 )
		_tAccess &= W_OK;
	_bAccess = true;
}
#endif

CStringBuffer CDirectoryIterator::_get_Path()
{
	CStringBuffer tmp;

	if ( _filePath.is_Absolute() )
	{
		tmp = _filePath.get_Root();
		tmp += _filePath.get_Directory();
		tmp += get_Name();
	}
	else
	{
		CFilePath tmpPath;

		GetCurrentDirectory(tmpPath);
		tmp = tmpPath.get_Path();
		tmp += _filePath.get_Directory();
		tmp += get_Name();
	}
	return tmp;
}

sqword CDirectoryIterator::get_FileSize() 
{ 
	if ( FAIL_COND )
		return -1;
#ifdef OK_SYS_WINDOWS
	return _tFileFind.size;
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return _tStatus.st_size;
#endif
}

CStringBuffer CDirectoryIterator::get_Name() 
{ 
	CStringBuffer tmp; 
	
	if ( FAIL_COND )
		return tmp;
#ifdef OK_SYS_WINDOWS
	tmp.SetString(__FILE__LINE__ _tFileFind.name);
#endif
#ifdef OK_SYS_UNIX
	tmp.SetString(__FILE__LINE__ _tFileFind->d_name);
#endif
	return tmp; 
}

CStringBuffer CDirectoryIterator::get_Path()
{
	CStringBuffer tmp;

	if ( FAIL_COND )
		return tmp;

	tmp = _get_Path();

    if ( is_SubDir() )
        tmp += DefaultPathSeparatorString();
	return tmp;
}
