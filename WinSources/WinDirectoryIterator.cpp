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
#include "WinDirectoryIterator.h"
#include "SecurityContext.h"
#include "File.h"

#ifdef OK_SYS_WINDOWS
#define FAIL_COND (_hFileFind == INVALID_HANDLE_VALUE)
#endif
#ifdef OK_SYS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#include <stdio.h>
#define FAIL_COND PtrCheck(_hFileFind)
#endif

IMPL_WINEXCEPTION(CWinDirectoryIteratorException, CWinException)

CWinDirectoryIterator::CWinDirectoryIterator():
	_filePath(),
#ifdef OK_SYS_WINDOWS
	_hFileFind(INVALID_HANDLE_VALUE)
#endif
#ifdef OK_SYS_UNIX
	_hFileFind(nullptr),
	_tFileFind(nullptr),
	_bStatus(false),
	_bAccess(false)
#endif
{
}

CWinDirectoryIterator::CWinDirectoryIterator(ConstRef(CFilePath) path):
	_filePath(),
#ifdef OK_SYS_WINDOWS
	_hFileFind(INVALID_HANDLE_VALUE)
#endif
#ifdef OK_SYS_UNIX
	_hFileFind(nullptr),
	_tFileFind(nullptr),
	_bStatus(false),
	_bAccess(false)
#endif
{
	Open(path);
}

CWinDirectoryIterator::~CWinDirectoryIterator(void)
{
	Close();
}

void CWinDirectoryIterator::GetCurrentDirectory(Ref(CFilePath) _path)
{
#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp;
	DWORD nBufferLength;

#ifdef _UNICODE
	nBufferLength = GetCurrentDirectoryW(0, nullptr);
	if (0 == nBufferLength)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception"),
		_T("CWinDirectoryIterator::GetCurrentDirectory"), CWinException::WinExtError);
	tmp.SetSize(__FILE__LINE__ nBufferLength);
	nBufferLength = GetCurrentDirectoryW(nBufferLength, CastAny(LPTSTR, CastMutable(CPointer, tmp.GetString())));
	if (0 == nBufferLength)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception"),
		_T("CWinDirectoryIterator::GetCurrentDirectory"), CWinException::WinExtError);
#else
	nBufferLength = GetCurrentDirectoryA(0, nullptr);
	if (0 == nBufferLength)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception"),
		_T("CWinDirectoryIterator::GetCurrentDirectory"), CWinException::WinExtError);
	tmp.SetSize(__FILE__LINE__ nBufferLength);
	nBufferLength = GetCurrentDirectoryA(nBufferLength, CastAny(LPTSTR, CastMutable(CPointer, tmp.GetString())));
	if (0 == nBufferLength)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception"),
		_T("CWinDirectoryIterator::GetCurrentDirectory"), CWinException::WinExtError);
#endif
	_path = tmp;
	_path.MakeDirectory();
#endif
#ifdef OK_SYS_UNIX
	CPointer buffer;

	if (nullptr == (buffer = getcwd(nullptr, 0)))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception"),
		_T("CWinDirectoryIterator::GetCurrentDirectory"), CWinException::CRunTimeError);
	_path.set_Path(__FILE__LINE__ buffer);
	free(buffer);
#endif
}

void CWinDirectoryIterator::SetCurrentDirectory(ConstRef(CFilePath) _path)
{
	CFilePath tmpPath(_path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();
#ifdef OK_SYS_WINDOWS

	CStringBuffer tmp(tmpPath.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
	if (!SetCurrentDirectoryW(tmp.GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::SetCurrentDirectory"), _path.GetString(), CWinException::WinExtError);
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::SetCurrentDirectory"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
	if (!SetCurrentDirectoryA(tmp.GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::SetCurrentDirectory"), _path.GetString(), CWinException::WinExtError);
#endif
#endif
#ifdef OK_SYS_UNIX
	if (0 > chdir(tmpPath.GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::SetCurrentDirectory"), _path.GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::MakeDirectory(ConstRef(CFilePath) _path, mode_t mode)
{
	CFilePath tmpPath(_path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();

#ifdef OK_SYS_WINDOWS
	SECURITY_ATTRIBUTES sa;
	CStringBuffer tmp(tmpPath.get_Path());
	CStringBuffer tmpRoot;
	CStringBuffer tmpDir;
	CPointer splitArray[64];
	dword maxsplitArraySize = 64;
	dword splitArraySize;
	dword i = 0;

	CSecurityContext::instance()->GetDirectorySecurity(mode, sa);

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::MakeDirectory"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	if (!CreateDirectory(tmp.GetString(), &sa))
	{
		switch (GetLastError())
		{
		case ERROR_ALREADY_EXISTS:
			goto __return;
		case ERROR_PATH_NOT_FOUND:
			break;
		default:
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
				_T("CWinDirectoryIterator::MakeDirectory"), _path.GetString(), CWinException::WinExtError);
			break;
		}
	}
	else
		goto __return;

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpRoot = tmpPath.get_Root();
	tmpDir = tmpPath.get_Directory();

	tmpDir.SplitAny(CFilePath::DefaultPathSeparatorString(), splitArray, maxsplitArraySize, &splitArraySize);
	if (splitArraySize > 0)
		--splitArraySize;
	while (i < splitArraySize)
	{
		tmpRoot += splitArray[i];
		tmpRoot += CFilePath::DefaultPathSeparatorString();
		++i;

		tmp = tmpRoot;
#ifdef _UNICODE
		if (tmp.GetLength() > MAX_PATH)
			tmp.PrependString(_T("\\\\?\\"));
#else
		if (tmp.GetLength() > MAX_PATH)
		{
			DWORD sz = tmp.GetLength();
			DWORD sz1;

			if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
				throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
				_T("CWinDirectoryIterator::MakeDirectory"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
		}
#endif
		if (!CreateDirectory(tmp.GetString(), &sa))
		{
			switch (GetLastError())
			{
			case ERROR_ALREADY_EXISTS:
				break;
			case ERROR_PATH_NOT_FOUND:
			default:
				throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
					_T("CWinDirectoryIterator::MakeDirectory"), _path.GetString(), CWinException::WinExtError);
				break;
			}
		}
	}
__return:
	CSecurityContext::instance()->FreeSecurity(sa);
#else
	if (0 > mkdir(tmpPath.GetString(), mode))
	{
		switch (errno)
		{
		case EEXIST:
			return;
		case ENOENT:
			break;
		default:
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
				_T("CWinDirectoryIterator::MakeDirectory"), _path.GetString(), CWinException::CRunTimeError);
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

	tmpDir.SplitAny(CFilePath::DefaultPathSeparatorString(), splitArray, maxsplitArraySize, &splitArraySize);
	if (splitArraySize > 0)
		--splitArraySize;
	while (i < splitArraySize)
	{
		tmpRoot += splitArray[i];
		tmpRoot += CFilePath::DefaultPathSeparatorString();
		++i;

		if (0 > mkdir(tmpRoot.GetString(), mode))
		{
			switch (errno)
			{
			case EEXIST:
				break;
			case ENOENT:
			default:
				throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
					_T("CWinDirectoryIterator::MakeDirectory"), _path.GetString(), CWinException::CRunTimeError);
				break;
			}
		}
	}
#endif
}

void CWinDirectoryIterator::UnlinkFile(ConstRef(CFilePath) _path)
{
	if (!FileExists(_path))
		return;

#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(_path.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::UnlinkFile"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	DWORD dwAttrs = GetFileAttributes(tmp.GetString());

	if (dwAttrs == INVALID_FILE_ATTRIBUTES)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::UnlinkFile"), _path.GetString(), CWinException::WinExtError);
	if ((dwAttrs & FILE_ATTRIBUTE_READONLY) > 0)
		dwAttrs = dwAttrs & ~FILE_ATTRIBUTE_READONLY;
	if ((dwAttrs & FILE_ATTRIBUTE_SYSTEM) > 0)
		dwAttrs = dwAttrs & ~FILE_ATTRIBUTE_SYSTEM;
	SetFileAttributes(tmp.GetString(), dwAttrs);
	if (!DeleteFile(tmp.GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::UnlinkFile"), _path.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	if (0 > unlink(_path.GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::UnlinkFile"), _path.GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::UnlinkDirectory(ConstRef(CFilePath) _path)
{
	if (FileExists(_path) || (DirectoryExists(_path) < 0))
		return;

#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(_path.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
	if (!RemoveDirectoryW(tmp.GetString()))
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::UnlinkDirectory"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
	if (!RemoveDirectoryA(tmp.GetString()))
#endif
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::UnlinkDirectory"), _path.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	if (0 > rmdir(_path.GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::UnlinkDirectory"), _path.GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::RemoveDirectory(ConstRef(CFilePath) path)
{
	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();

#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(tmpPath.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
	if (!RemoveDirectoryW(tmp.GetString()))
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::RemoveDirectory"), path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
	if (!RemoveDirectoryA(tmp.GetString()))
#endif
	{
		if (GetLastError() != ERROR_DIR_NOT_EMPTY)
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::RemoveDirectory"), path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
	else
		return;
#endif
#ifdef OK_SYS_UNIX
	if (0 > rmdir(tmpPath.GetString()))
	{
		switch (errno)
		{
		case ENOTEMPTY:
			break;
		case ENOENT:
		case EACCES:
		default:
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s'"),
				_T("CWinDirectoryIterator::RemoveDirectory"), path.GetString(), CWinException::CRunTimeError);
			break;
		}
	}
	else
		return;
#endif

	CFilePath curDir;
	CWinDirectoryIterator it;
	CFilePath wildcard(__FILE__LINE__ _T("*"));

	try
	{
		GetCurrentDirectory(curDir);
		SetCurrentDirectory(tmpPath);

		it.Open(wildcard);
		while (it)
		{
			if (it.is_SubDir())
			{
				CStringBuffer tmp(it.get_Name());

				if ((tmp != CStringLiteral(_T("."))) && (tmp != CStringLiteral(_T(".."))))
				{
					tmp += CFilePath::DefaultPathSeparatorString();

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
	catch (CWinDirectoryIteratorException* ex)
	{
		throw ex;
	}
}

void CWinDirectoryIterator::RemoveFile(ConstRef(CFilePath) path)
{
	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();

#ifdef OK_SYS_WINDOWS
	HANDLE fHandle = INVALID_HANDLE_VALUE;
	DWORD dwDesiredAccess = FILE_ALL_ACCESS;
	DWORD dwShareMode = FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ;
	DWORD dwCreationDisposition = OPEN_EXISTING;
	DWORD dwFlagsAndAttributes = FILE_FLAG_DELETE_ON_CLOSE;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = FALSE;

	// note: FILE_FLAG_OPEN_REPARSE_POINT is not specified, that deletes the target
	fHandle = CreateFile(
		tmpPath.GetString(),
		dwDesiredAccess,
		dwShareMode,
		&sa,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		nullptr
		);
	if (fHandle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			return;
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s'"),
			_T("CWinDirectoryIterator::RemoveFile"), path.GetString(), CWinException::WinExtError);
	}
	CloseHandle(fHandle);
#endif
#ifdef OK_SYS_UNIX
	if (0 > unlink(tmpPath.GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s'"),
			_T("CWinDirectoryIterator::RemoveFile"), path.GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::LinkFile(ConstRef(CFilePath) _from, ConstRef(CFilePath) _to)
{
#ifdef OK_SYS_WINDOWS
	if (!CreateHardLink(_to.GetString(), _from.GetString(), nullptr))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
		_T("CWinDirectoryIterator::LinkFile"), _from.GetString(), _to.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	if (0 > link(_from.get_Path().GetString(), _to.get_Path().GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
		_T("CWinDirectoryIterator::LinkFile"), _from.get_Path().GetString(), _to.get_Path().GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::SymLinkFile(ConstRef(CFilePath) _from, ConstRef(CFilePath) _to)
{
#ifdef OK_SYS_WINDOWS
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA) && (OK_COMP_MSC || (__MINGW32_MAJOR_VERSION > 3) || __MINGW64_VERSION_MAJOR)
	DWORD flags = 0;

	CSecurityContext::instance()->SetProcessPrivilege(SE_CREATE_SYMBOLIC_LINK_NAME, TRUE);
	if (_from.is_Directory())
		flags = SYMBOLIC_LINK_FLAG_DIRECTORY;
	if (!CreateSymbolicLink(_to.GetString(), _from.GetString(), flags))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
		_T("CWinDirectoryIterator::SymLinkFile"), _from.GetString(), _to.GetString(), CWinException::WinExtError);
	CSecurityContext::instance()->SetProcessPrivilege(SE_CREATE_SYMBOLIC_LINK_NAME, FALSE);
#else
	throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
		_T("CWinDirectoryIterator::SymLinkFile"), _from.GetString(), _to.GetString(), CWinException::WinExtError, ERROR_CALL_NOT_IMPLEMENTED);
#endif
#endif
#ifdef OK_SYS_UNIX
	if (0 > symlink(_from.get_Path().GetString(), _to.get_Path().GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
		_T("CWinDirectoryIterator::SymLinkFile"), _from.get_Path().GetString(), _to.get_Path().GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::Rename(ConstRef(CFilePath) _from, ConstRef(CFilePath) _to)
{
#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp1(_from.get_Path());
	CStringBuffer tmp2(_to.get_Path());

#ifdef _UNICODE
	if (tmp1.GetLength() > MAX_PATH)
		tmp1.PrependString(_T("\\\\?\\"));
	if (tmp2.GetLength() > MAX_PATH)
		tmp2.PrependString(_T("\\\\?\\"));
	if (!MoveFileW(tmp1.GetString(), tmp2.GetString()))
#else
	if (tmp1.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp1.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp1.GetString()), (LPSTR)(tmp1.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
			_T("CWinDirectoryIterator::Rename"), _from.get_Path().GetString(), _to.get_Path().GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
	if (tmp2.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp2.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp2.GetString()), (LPSTR)(tmp2.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
			_T("CWinDirectoryIterator::Rename"), _from.get_Path().GetString(), _to.get_Path().GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
	if (!MoveFileA(tmp1.GetString(), tmp2.GetString()))
#endif
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
		_T("CWinDirectoryIterator::Rename"), _from.get_Path().GetString(), _to.get_Path().GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	if (0 > rename(_from.get_Path().GetString(), _to.get_Path().GetString()))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, From = '%s', To = '%s'"),
		_T("CWinDirectoryIterator::Rename"), _from.get_Path().GetString(), _to.get_Path().GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::CopyFile(ConstRef(CFilePath) _from, ConstRef(CFilePath) _to, mode_t mode)
{
#ifdef OK_SYS_WINDOWS
	if (_from.is_Directory())
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s' is directory"),
		_T("CWinDirectoryIterator::CopyFile"), _from.get_Path().GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	if (_to.is_Directory())
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s' is directory"),
		_T("CWinDirectoryIterator::CopyFile"), _to.get_Path().GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
#endif
#ifdef OK_SYS_UNIX
	if (_from.is_Directory())
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s' is directory"),
		_T("CWinDirectoryIterator::CopyFile"), _from.get_Path().GetString(), CWinException::CRunTimeError, EINVAL);
	if (_to.is_Directory())
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s' is directory"),
		_T("CWinDirectoryIterator::CopyFile"), _to.get_Path().GetString(), CWinException::CRunTimeError, EINVAL);
#endif
	if (FileExists(_to))
		UnlinkFile(_to);

	CSecurityFile fromf(_from);
	CSecurityFile tof;
	CByteBuffer buffer(__FILE__LINE__ 4096);

	tof.Create(_to, false, CFile::BinaryFile_NoEncoding, mode);
	fromf.Read(buffer);
	while (buffer.get_BufferSize() > 0)
	{
		tof.Write(buffer);
		fromf.Read(buffer);
	}
	tof.Close();
	fromf.Close();
}

bool CWinDirectoryIterator::FileExists(ConstRef(CFilePath) _path)
{
	CFilePath tmpPath(_path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();

#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(tmpPath.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::FileExists"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	HANDLE hFind;
	WIN32_FIND_DATA findData;

	hFind = FindFirstFile(tmp.GetString(), &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if ((GetLastError() != ERROR_FILE_NOT_FOUND) && (GetLastError() != ERROR_PATH_NOT_FOUND))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::FileExists"), _path.GetString(), CWinException::WinExtError);
		return false;
	}
	FindClose(hFind);
	if (TestBit(findData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
		return false;
// TODO:	if (TestBit(findData.dwFileAttributes, FILE_ATTRIBUTE_REPARSE_POINT) && (findData.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
#endif
#ifdef OK_SYS_UNIX
	DIR *hFileFind;
	struct dirent *tFileFind;
	CFilePath dir;

	dir.set_Root(tmpPath.get_Root());
	dir.set_Directory(tmpPath.get_Directory());
	if (dir.get_Path().IsEmpty())
		dir.set_Directory(_T("./"));

	hFileFind = opendir(dir.GetString());
	if (PtrCheck(hFileFind))
	{
		switch (errno)
		{
		case ENOENT:
			return false;
		default:
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
				_T("CWinDirectoryIterator::FileExists"), _path.GetString(), CWinException::CRunTimeError);
			return false;
		}
	}

	CStringBuffer fname(tmpPath.get_Filename());

	if (fname.IsEmpty())
	{
		closedir(hFileFind);
		return false;
	}
	do
	{
		tFileFind = readdir(hFileFind);
		if (PtrCheck(tFileFind))
		{
			closedir(hFileFind);
			return false;
		}
	} while (s_strcmp(tFileFind->d_name, fname.GetString()) != 0);
	closedir(hFileFind);
#endif
	return true;
}

int CWinDirectoryIterator::DirectoryExists(ConstRef(CFilePath) _path)
{
	int cnt = 0;
	CFilePath tmpPath(_path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();
	tmpPath.MakeDirectory();

#ifdef OK_SYS_WINDOWS
	tmpPath.set_Filename(_T("*"));

	CStringBuffer tmp(tmpPath.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::DirectoryExists"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	HANDLE hFind;
	WIN32_FIND_DATA findData;

	hFind = FindFirstFile(tmp.GetString(), &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if ((GetLastError() != ERROR_FILE_NOT_FOUND) && (GetLastError() != ERROR_PATH_NOT_FOUND))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::DirectoryExists"), _path.GetString(), CWinException::WinExtError);
		return -1;
	}
	while (FindNextFile(hFind, &findData))
	{
		if ((s_strcmp(findData.cFileName, _T(".")) != 0)
			&& (s_strcmp(findData.cFileName, _T("..")) != 0))
			++cnt;
	}
	if (GetLastError() != ERROR_NO_MORE_FILES)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::DirectoryExists"), _path.GetString(), CWinException::WinExtError);
	FindClose(hFind);
#endif
#ifdef OK_SYS_UNIX
	DIR *hFileFind;
	struct dirent *tFileFind;

	hFileFind = opendir(tmpPath.GetString());
	if (PtrCheck(hFileFind))
	{
		switch (errno)
		{
		case ENOENT:
			return -1;
		default:
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
				_T("CWinDirectoryIterator::DirectoryExists"), _path.GetString(), CWinException::CRunTimeError);
			return -1;
		}
	}
	while (true)
	{
		tFileFind = readdir(hFileFind);
		if (PtrCheck(tFileFind))
		{
			closedir(hFileFind);
			return cnt;
		}
		if ((s_strcmp(tFileFind->d_name, _T(".")) != 0)
			&& (s_strcmp(tFileFind->d_name, _T("..")) != 0))
			++cnt;
	}
#endif
	return cnt;
}

dword CWinDirectoryIterator::ReadFileAttributes(ConstRef(CFilePath) _path)
{
#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(_path.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::ReadFileAttributes"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	DWORD dwAttrs = GetFileAttributes(tmp.GetString());

	if (dwAttrs == INVALID_FILE_ATTRIBUTES)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileAttributes"), _path.GetString(), CWinException::WinExtError);
	return dwAttrs & 0x00000037;
#endif
#ifdef OK_SYS_UNIX
	struct stat buf;

	if (0 > stat(_path.GetString(), &buf))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileAttributes"), _path.GetString(), CWinException::CRunTimeError);
	return buf.st_mode;
#endif
}

void CWinDirectoryIterator::WriteFileAttributes(ConstRef(CFilePath) _path, dword attrs)
{
#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(_path.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::WriteFileAttributes"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	if (attrs == 0)
		attrs = 0x00000080;
	else
		attrs &= 0x00000027;
	if (!SetFileAttributes(tmp.GetString(), attrs))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::WriteFileAttributes"), _path.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	if (0 > chmod(_path.GetString(), attrs))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::WriteFileAttributes"), _path.GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::_ReadFileTimes(ConstRef(CFilePath) _path, Ptr(FILETIME) CreationTime, Ptr(FILETIME) LastAccessTime, Ptr(FILETIME) LastWriteTime)
{
#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(_path.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	HANDLE fHandle = INVALID_HANDLE_VALUE;
	DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = TRUE;

	if (_path.is_File() && FileExists(_path))
	{
		fHandle = CreateFile(
			tmp.GetString(), // LPCTSTR lpFileName,
			dwDesiredAccess, // DWORD dwDesiredAccess,
			FILE_SHARE_READ, // DWORD dwShareMode,
			&sa, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			OPEN_EXISTING, // DWORD dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL, // DWORD dwFlagsAndAttributes,
			nullptr // HANDLE hTemplateFile
			);
	}
	else if (_path.is_Directory() && (DirectoryExists(_path) >= 0))
	{
		fHandle = CreateFile(
			tmp.GetString(), // LPCTSTR lpFileName,
			dwDesiredAccess, // DWORD dwDesiredAccess,
			FILE_SHARE_READ, // DWORD dwShareMode,
			&sa, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			OPEN_EXISTING, // DWORD dwCreationDisposition,
			FILE_FLAG_BACKUP_SEMANTICS, // DWORD dwFlagsAndAttributes,
			nullptr // HANDLE hTemplateFile
			);
	}
	else
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError, ERROR_FILE_NOT_FOUND);
	if (fHandle == INVALID_HANDLE_VALUE)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);

	if (!GetFileTime(fHandle, CreationTime, LastAccessTime, LastWriteTime))
	{
		CloseHandle(fHandle);
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
	}
	CloseHandle(fHandle);
#endif
#ifdef OK_SYS_UNIX
	struct stat buf;

	if (0 > stat(_path.GetString(), &buf))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::CRunTimeError);
	*CreationTime = buf.st_ctime;
	*LastAccessTime = buf.st_atime;
	*LastWriteTime = buf.st_mtime;
#endif
}

void CWinDirectoryIterator::ReadFileTimes(ConstRef(CFilePath) _path, Ref(CSystemTime) CreationTime, Ref(CSystemTime) LastAccessTime, Ref(CSystemTime) LastWriteTime)
{
#ifdef OK_SYS_WINDOWS
	FILETIME vCreationTime;
	FILETIME vLastAccessTime;
	FILETIME vLastWriteTime;

	_ReadFileTimes(_path, &vCreationTime, &vLastAccessTime, &vLastWriteTime);

	int dst = 0;
#if (_WIN32_WINNT >= 0x0600)
	DYNAMIC_TIME_ZONE_INFORMATION tz;

	ZeroMemory(&tz, sizeof(DYNAMIC_TIME_ZONE_INFORMATION));
	switch (GetDynamicTimeZoneInformation(&tz))
	{
	case TIME_ZONE_ID_UNKNOWN:
		break;
	case TIME_ZONE_ID_STANDARD:
		break;
	case TIME_ZONE_ID_DAYLIGHT:
		dst = 1;
		break;
	case TIME_ZONE_ID_INVALID:
	default:
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
		break;
	}
#else
	TIME_ZONE_INFORMATION tz;

	ZeroMemory(&tz, sizeof(TIME_ZONE_INFORMATION));
	switch (GetTimeZoneInformation(&tz))
	{
	case TIME_ZONE_ID_UNKNOWN:
		break;
	case TIME_ZONE_ID_STANDARD:
		break;
	case TIME_ZONE_ID_DAYLIGHT:
		dst = 1;
		break;
	case TIME_ZONE_ID_INVALID:
	default:
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
		break;
	}
#endif

	SYSTEMTIME t;
	SYSTEMTIME l;

	if (!FileTimeToSystemTime(&vCreationTime, &t))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
	if (!SystemTimeToTzSpecificLocalTime(nullptr, &t, &l))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
	CreationTime = l;
	if (!FileTimeToSystemTime(&vLastAccessTime, &t))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
	if (!SystemTimeToTzSpecificLocalTime(nullptr, &t, &l))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
	LastAccessTime = l;
	if (!FileTimeToSystemTime(&vLastWriteTime, &t))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
	if (!SystemTimeToTzSpecificLocalTime(nullptr, &t, &l))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::ReadFileTimes"), _path.GetString(), CWinException::WinExtError);
	LastWriteTime = l;
#endif
#ifdef OK_SYS_UNIX
	FILETIME vCreationTime;
	FILETIME vLastAccessTime;
	FILETIME vLastWriteTime;

	_ReadFileTimes(_path, &vCreationTime, &vLastAccessTime, &vLastWriteTime);
	CreationTime.SetTime(vCreationTime, CDateTime::LocalTime);
	LastAccessTime.SetTime(vLastAccessTime, CDateTime::LocalTime);
	LastWriteTime.SetTime(vLastWriteTime, CDateTime::LocalTime);
#endif
}

void CWinDirectoryIterator::_WriteFileTimes(ConstRef(CFilePath) _path, Ptr(FILETIME) CreationTime, Ptr(FILETIME) LastAccessTime, Ptr(FILETIME) LastWriteTime)
{
#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(_path.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::WriteFileTimes"), _path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	HANDLE fHandle = INVALID_HANDLE_VALUE;
	DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = TRUE;

	if (_path.is_File() && FileExists(_path))
	{
		fHandle = CreateFile(
			tmp.GetString(), // LPCTSTR lpFileName,
			dwDesiredAccess, // DWORD dwDesiredAccess,
			FILE_SHARE_READ, // DWORD dwShareMode,
			&sa, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			OPEN_EXISTING, // DWORD dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL, // DWORD dwFlagsAndAttributes,
			nullptr // HANDLE hTemplateFile
			);
	}
	else if (_path.is_Directory() && (DirectoryExists(_path) >= 0))
	{
		fHandle = CreateFile(
			tmp.GetString(), // LPCTSTR lpFileName,
			dwDesiredAccess, // DWORD dwDesiredAccess,
			FILE_SHARE_READ, // DWORD dwShareMode,
			&sa, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			OPEN_EXISTING, // DWORD dwCreationDisposition,
			FILE_FLAG_BACKUP_SEMANTICS, // DWORD dwFlagsAndAttributes,
			nullptr // HANDLE hTemplateFile
			);
	}
	else
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::WriteFileTimes"), _path.GetString(), CWinException::WinExtError, ERROR_FILE_NOT_FOUND);
	if (fHandle == INVALID_HANDLE_VALUE)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::WriteFileTimes"), _path.GetString(), CWinException::WinExtError);

	if (!SetFileTime(fHandle, CreationTime, LastAccessTime, LastWriteTime))
	{
		CloseHandle(fHandle);
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::WriteFileTimes"), _path.GetString(), CWinException::WinExtError);

	}
	CloseHandle(fHandle);
#endif
#ifdef OK_SYS_UNIX
	struct utimbuf buf;

	buf.actime = *LastAccessTime;
	buf.modtime = *LastWriteTime;
	if (0 > utime(_path.GetString(), &buf))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
		_T("CWinDirectoryIterator::WriteFileTimes"), _path.GetString(), CWinException::CRunTimeError);
#endif
}

void CWinDirectoryIterator::WriteFileTimes(ConstRef(CFilePath) _path, ConstRef(CSystemTime) CreationTime, ConstRef(CSystemTime) LastAccessTime, ConstRef(CSystemTime) LastWriteTime)
{
	FILETIME vCreationTime;
	FILETIME vLastAccessTime;
	FILETIME vLastWriteTime;
#ifdef OK_SYS_WINDOWS
	CreationTime.GetFileTime(vCreationTime);
	if ((vCreationTime.dwHighDateTime == 0) && (vCreationTime.dwLowDateTime == 0))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::WriteFileTimes"), _path.GetString(), CWinException::WinExtError);
	LastAccessTime.GetFileTime(vLastAccessTime);
	if ((vLastAccessTime.dwHighDateTime == 0) && (vLastAccessTime.dwLowDateTime == 0))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::WriteFileTimes"), _path.GetString(), CWinException::WinExtError);
	LastWriteTime.GetFileTime(vLastWriteTime);
	if ((vLastWriteTime.dwHighDateTime == 0) && (vLastWriteTime.dwLowDateTime == 0))
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::WriteFileTimes"), _path.GetString(), CWinException::WinExtError);
#endif
#ifdef OK_SYS_UNIX
	CreationTime.GetTime(vCreationTime);
	LastAccessTime.GetTime(vLastAccessTime);
	LastWriteTime.GetTime(vLastWriteTime);
#endif
	_WriteFileTimes(_path, &vCreationTime, &vLastAccessTime, &vLastWriteTime);
}

bool CWinDirectoryIterator::FileSystemIsNTFS(ConstRef(CFilePath) path)
{
#ifdef OK_SYS_WINDOWS
	CFilePath path2(path);

	path2.MakeAbsolute();

	CStringBuffer tmp(path2.get_Root());
	TCHAR lpFileSystemNameBuffer[MAX_PATH + 1];

	if (!GetVolumeInformation(tmp.GetString(), nullptr, 0, nullptr, nullptr, nullptr, lpFileSystemNameBuffer, MAX_PATH + 1))
		return false;
	if (s_stricmp(lpFileSystemNameBuffer, _T("NTFS")) != 0)
		return false;
	return true;
#endif
#ifdef OK_SYS_UNIX
	return false;
#endif
}

bool CWinDirectoryIterator::FileIsSymbolicLink(ConstRef(CFilePath) path)
{
	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();

#ifdef OK_SYS_WINDOWS
	CStringBuffer tmp(tmpPath.get_Path());

#ifdef _UNICODE
	if (tmp.GetLength() > MAX_PATH)
		tmp.PrependString(_T("\\\\?\\"));
#else
	if (tmp.GetLength() > MAX_PATH)
	{
		DWORD sz = tmp.GetLength();
		DWORD sz1;

		if (((sz1 = GetShortPathName((LPCSTR)(tmp.GetString()), (LPSTR)(tmp.GetString()), sz + 1)) == 0) || (sz1 > sz) || (sz1 > MAX_PATH))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::FileIsSymbolicLink"), path.GetString(), CWinException::WinExtError, ERROR_BAD_PATHNAME);
	}
#endif
	HANDLE hFind;
	WIN32_FIND_DATA findData;

	hFind = FindFirstFile(tmp.GetString(), &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if ((GetLastError() != ERROR_FILE_NOT_FOUND) && (GetLastError() != ERROR_PATH_NOT_FOUND))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::FileIsSymbolicLink"), path.GetString(), CWinException::WinExtError);
		return false;
	}
	FindClose(hFind);
	if (TestBit(findData.dwFileAttributes, FILE_ATTRIBUTE_REPARSE_POINT) && (findData.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
		return true;
#endif
#ifdef OK_SYS_UNIX
	DIR *hFileFind;
	struct dirent *tFileFind;
	CFilePath dir;

	dir.set_Root(tmpPath.get_Root());
	dir.set_Directory(tmpPath.get_Directory());
	if (dir.get_Path().IsEmpty())
		dir.set_Directory(_T("./"));

	hFileFind = opendir(dir.GetString());
	if (PtrCheck(hFileFind))
	{
		switch (errno)
		{
		case ENOENT:
			return false;
		default:
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
				_T("CWinDirectoryIterator::FileIsSymbolicLink"), path.GetString(), CWinException::CRunTimeError);
			return false;
		}
	}

	CStringBuffer fname(tmpPath.get_Filename());

	if (fname.IsEmpty())
	{
		closedir(hFileFind);
		return false;
	}
	do
	{
		tFileFind = readdir(hFileFind);
		if (PtrCheck(tFileFind))
		{
			closedir(hFileFind);
			return false;
		}
	} while (s_strcmp(tFileFind->d_name, fname.GetString()) != 0);
	if (tFileFind->d_type == DT_LNK)
	{
		closedir(hFileFind);
		return true;
	}
	closedir(hFileFind);
#endif
	return false;
}

void CWinDirectoryIterator::Open(ConstRef(CFilePath) path)
{
	if ( !FAIL_COND )
		Close();

	CFilePath tmpPath(path);

	if (tmpPath.is_Relative())
		tmpPath.MakeAbsolute();
	tmpPath.Normalize();

	_filePath = tmpPath;
#ifdef OK_SYS_WINDOWS
	_hFileFind = FindFirstFile(_filePath.GetString(), &_findData);
	if (_hFileFind == INVALID_HANDLE_VALUE)
	{
		if ((GetLastError() != ERROR_FILE_NOT_FOUND) && (GetLastError() != ERROR_PATH_NOT_FOUND))
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::Open"), path.GetString(), CWinException::WinExtError);
		return;
	}
#endif
#ifdef OK_SYS_UNIX
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
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s'"), 
				_T("CWinDirectoryIterator::Open"), path.GetString(), CWinException::CRunTimeError);
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

void CWinDirectoryIterator::Next()
{
	if ( FAIL_COND )
		return;

#ifdef OK_SYS_WINDOWS
	if (FindNextFile(_hFileFind, &_findData))
		return;
	if (GetLastError() != ERROR_NO_MORE_FILES)
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("%s Exception, Path = '%s'"),
			_T("CWinDirectoryIterator::Next"), _filePath.GetString(), CWinException::WinExtError);
	Close();
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

void CWinDirectoryIterator::Close()
{
	if ( FAIL_COND )
		return;
#ifdef OK_SYS_WINDOWS
	FindClose(_hFileFind);
	_hFileFind = INVALID_HANDLE_VALUE;
#endif
#ifdef OK_SYS_UNIX
	closedir(_hFileFind);
	_hFileFind = nullptr;
	_bStatus = false;
	_bAccess = false;
#endif
}

WBool CWinDirectoryIterator::is_Archive() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_findData.dwFileAttributes, FILE_ATTRIBUTE_ARCHIVE);
#endif
#ifdef OK_SYS_UNIX
	return false;
#endif
}

WBool CWinDirectoryIterator::is_Hidden() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_findData.dwFileAttributes, FILE_ATTRIBUTE_HIDDEN);
#endif
#ifdef OK_SYS_UNIX
	return _tFileFind->d_name[0] == '.';
#endif
}

WBool CWinDirectoryIterator::is_ReadOnly() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_findData.dwFileAttributes, FILE_ATTRIBUTE_READONLY);
#endif
#ifdef OK_SYS_UNIX
	_test_access();
	return !(_tAccess & W_OK);
#endif
}

WBool CWinDirectoryIterator::is_SubDir() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_findData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return S_ISDIR(_tStatus.st_mode); 
#endif
}

WBool CWinDirectoryIterator::is_System() 
{ 
	if ( FAIL_COND )
		return false;
#ifdef OK_SYS_WINDOWS
	return TestBit(_findData.dwFileAttributes, FILE_ATTRIBUTE_SYSTEM);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return !(S_ISDIR(_tStatus.st_mode) || S_ISREG(_tStatus.st_mode)); 
#endif
}

CSystemTime CWinDirectoryIterator::get_CreateTime()
{
	if ( FAIL_COND )
		return CDateTime();
#ifdef OK_SYS_WINDOWS
	return CSystemTime(_findData.ftCreationTime);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return CDateTime(_tStatus.st_ctime); 
#endif
}

CSystemTime CWinDirectoryIterator::get_LastAccessTime()
{ 
	if ( FAIL_COND )
		return CDateTime();
#ifdef OK_SYS_WINDOWS
	return CSystemTime(_findData.ftLastAccessTime);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return CDateTime(_tStatus.st_atime); 
#endif
}

CSystemTime CWinDirectoryIterator::get_LastWriteTime()
{ 
	if ( FAIL_COND )
		return CDateTime();
#ifdef OK_SYS_WINDOWS
	return CSystemTime(_findData.ftLastWriteTime);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return CDateTime(_tStatus.st_mtime); 
#endif
}

#ifdef OK_SYS_UNIX
void CWinDirectoryIterator::_load_status()
{
	if ( _bStatus )
		return;
	
	CStringBuffer tmp = _get_Path();
	int res = ::stat(tmp.GetString(), &_tStatus);
	
	if ( res < 0 )
		throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s'"), 
		_T("CWinDirectoryIterator::_load_status"), tmp.GetString(), CWinException::CRunTimeError);
	_bStatus = true;
}

void CWinDirectoryIterator::_test_access()
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
			throw OK_NEW_OPERATOR CWinDirectoryIteratorException(__FILE__LINE__ _T("in %s CWinDirectoryIteratorException, path = '%s'"), 
				_T("CWinDirectoryIterator::_test_access"), tmp.GetString(), CWinException::CRunTimeError);
			break;
		}
	}
	if ( res == 0 )
		_tAccess &= W_OK;
	_bAccess = true;
}
#endif

CStringBuffer CWinDirectoryIterator::_get_Path()
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

sqword CWinDirectoryIterator::get_FileSize() 
{ 
	if ( FAIL_COND )
		return -1;
#ifdef OK_SYS_WINDOWS
	return Castsqword(Castsqword(_findData.nFileSizeHigh) * (Castsqword(MAXDWORD) + 1)) + Castsqword(_findData.nFileSizeLow);
#endif
#ifdef OK_SYS_UNIX
	_load_status();
	return _tStatus.st_size;
#endif
}

CStringBuffer CWinDirectoryIterator::get_Name() 
{ 
	CStringBuffer tmp; 
	
	if ( FAIL_COND )
		return tmp;
#ifdef OK_SYS_WINDOWS
	tmp.SetString(__FILE__LINE__ _findData.cFileName);
#endif
#ifdef OK_SYS_UNIX
	tmp.SetString(__FILE__LINE__ _tFileFind->d_name);
#endif
	return tmp; 
}

CStringBuffer CWinDirectoryIterator::get_Path()
{
	CStringBuffer tmp;

	if ( FAIL_COND )
		return tmp;

	tmp = _get_Path();

    if ( is_SubDir() )
        tmp += CFilePath::DefaultPathSeparatorString();
	return tmp;
}
