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
/**
 *  \file DirectoryIterator.h
 *  \brief Contains a class to traverse a file system directory
 */
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

/**
 *  \class CDirectoryIterator
 *  \brief Class to traverse a file system directory
 */
class CPPSOURCES_API CDirectoryIterator: public CCppObject
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Initializes the object.
	 */
	CDirectoryIterator();
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] path search path, accepts wild cards in the file name part
	 *  
	 *  \details Initializes the object. Calls Open.
	 */
	CDirectoryIterator(ConstRef(CFilePath) path);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Frees allocated memory.
	 */
	virtual ~CDirectoryIterator(void);

	/**
	 *  \brief Retrieve current directory
	 *  
	 *  \param [out] path current directory
	 *  \return void
	 *  
	 *  \details Retrieve current directory, which is a process global variable. That means \c GetCurrentDirectory and \c SetCurrentDirectory are not thread safe.
	 */
	static void GetCurrentDirectory(Ref(CFilePath) path);
	/**
	 *  \brief Set current directory
	 *  
	 *  \param [in] path new current path
	 *  \return void
	 *  
	 *  \details Set current directory, which is a process global variable. That means \c GetCurrentDirectory and \c SetCurrentDirectory are not thread safe.
	 */
	static void SetCurrentDirectory(ConstRef(CFilePath) path);
	/**
	 *  \brief Creates a directory
	 *  
	 *  \param [in] path path to the new directory
	 *  \return void
	 *  
	 *  \details Creates a directory. The whole path will be created, if necessary.
	 */
	static void MakeDirectory(ConstRef(CFilePath) path);
	/**
	 *  \brief Deletes a directory
	 *  
	 *  \param [in] path path to be deleted
	 *  \return void
	 *  
	 *  \details Deletes a directory, which may be be not empty. The whole directory tree will be traversed and all elements deleted.
	 */
	static void RemoveDirectory(ConstRef(CFilePath) path);
	/**
	 *  \brief Delete a single file
	 *  
	 *  \param [in] path path to be deleted
	 *  \return void
	 *  
	 *  \details Delete a single file.
	 */
	static void RemoveFile(ConstRef(CFilePath) path);
    /**
     *  \brief Test on the existence of a file
     *  
     *  \param [in] path path to be tested
     *  \return bool
     *  
     *  \details Test on the existence of a file.
     */
    static bool FileExists(ConstRef(CFilePath) path);
    /**
     *  \brief Test on the existence of directory
     *  
     *  \param [in] path path to be tested
     *  \return int number of items in the directory, except "." and "..".
     *  
     *  \details Test on the existence of directory.
     */
    static int DirectoryExists(ConstRef(CFilePath) path);
    /**
     *  \brief Rename a directory
     *  
     *  \param [in] basepath path to parent directory
     *  \param [in] oldname name of the directory to be renamed
     *  \param [in] newname new name of the directory
     *  \return Return_Description
     *  
     *  \details Rename a directory.
     */
    static void RenameDirectory(ConstRef(CFilePath) basepath, ConstRef(CStringBuffer) oldname, ConstRef(CStringBuffer) newname);
    /**
     *  \brief Move a file
     *  
     *  \param [in] from path to file to be moved
     *  \param [in] to path to be created
     *  \return void
     *  
     *  \details Move a file. The file cannot be moved across devices.
     */
    static void MoveFile(ConstRef(CFilePath) from, ConstRef(CFilePath) to);
	/**
     *  \brief Copy a file
     *  
     *  \param [in] from path to file to be copied
     *  \param [in] to path to be created
     *  \return void
     *  
     *  \details Copy a file.
     */
    static void CopyFile(ConstRef(CFilePath) from, ConstRef(CFilePath) to);

	/**
	 *  \brief Default getter of path separator character
	 *  
	 *  \return mbchar
	 *  
	 *  \details Default getter of path separator character. The default depends on compile time macros.
	 */
	static mbchar DefaultPathSeparatorChar();
	/**
	 *  \brief Default getter of path separator string
	 *  
	 *  \return CConstPointer
	 *  
	 *  \details Default getter of path separator string. The default depends on compile time macros.
	 */
	static CConstPointer DefaultPathSeparatorString();
	/**
	 *  \brief Windows getter of path separator character
	 *  
	 *  \return mbchar
	 *  
	 *  \details Windows getter of path separator character.
	 */
	static mbchar WinPathSeparatorChar();
	/**
	 *  \brief Windows getter of path separator string
	 *  
	 *  \return CConstPointer
	 *  
	 *  \details Windows getter of path separator string.
	 */
	static CConstPointer WinPathSeparatorString();
	/**
	 *  \brief Unix getter of path separator character
	 *  
	 *  \return mbchar
	 *  
	 *  \details Unix getter of path separator character.
	 */
	static mbchar UnixPathSeparatorChar();
	/**
	 *  \brief Unix getter of path separator string
	 *  
	 *  \return CConstPointer
	 *  
	 *  \details Unix getter of path separator string.
	 */
	static CConstPointer UnixPathSeparatorString();

	/**
	 *  \brief Opens the iterator
	 *  
	 *  \param [in] path search path, accepts wild cards in the file name part
	 *  \return void
	 *  
	 *  \details Opens the iterator and initializes it with a new path.
	 */
	void Open(ConstRef(CFilePath) path);
	/**
	 *  \brief Advances the iterator
	 *  
	 *  \return void
	 *  
	 *  \details Advances the iterator. 
	 */
	void Next();
	/**
	 *  \brief Closes the iterator
	 *  
	 *  \return void
	 *  
	 *  \details Closes the iterator.
	 */
	void Close();

	/**
	 *  \brief Test on success of file find functions
	 *  
	 *  \return bool
	 *  
	 *  \details Test on success of file find functions.
	 *  \code{.cpp}
CDirectoryIterator it;

it.Open(path);
while (it)
{
	process(it);
	++it;
}
	    \endcode
	 */
	__inline operator bool() const 
	{ 
#ifdef OK_SYS_WINDOWS
		return _hFileFind != -1L;
#endif
#ifdef OK_SYS_UNIX
		return NotPtrCheck(_hFileFind);
#endif
	}
	/**
	 *  \brief Advances the iterator
	 *  \return Ref(CDirectoryIterator) *this
	 */
	__inline Ref(CDirectoryIterator) operator++() 
	{ 
		Next(); 
		return *this; 
	}

	/**
	 *  \brief Test, whether the current item has archive flag set
	 *  
	 *  \return WBool
	 */
	WBool is_Archive();
	/**
	 *  \brief Test, whether the current item has hidden flag set
	 *  
	 *  \return WBool
	 */
	WBool is_Hidden();
	/**
	 *  \brief Test, whether the current item has read only flag set
	 *  
	 *  \return WBool
	 */
	WBool is_ReadOnly();
	/**
	 *  \brief Test, whether the current item is a directory
	 *  
	 *  \return WBool
	 */
	WBool is_SubDir();
	/**
	 *  \brief Test, whether the current item has system flag set
	 *  
	 *  \return WBool
	 */
	WBool is_System();
	
	/**
	 *  \brief Getter create time of the current item
	 *  
	 *  \return CDateTime
	 */
	CDateTime get_CreateTime();
	/**
	 *  \brief Getter last access time of the current item
	 *  
	 *  \return CDateTime
	 */
	CDateTime get_LastAccessTime();
	/**
	 *  \brief Getter last write time of the current item
	 *  
	 *  \return CDateTime
	 */
	CDateTime get_LastWriteTime();

	/**
	 *  \brief Getter file size of the current item
	 *  
	 *  \return sqword
	 */
	sqword get_FileSize();
	/**
	 *  \brief Getter name of the current item
	 *  
	 *  \return CStringBuffer
	 */
	CStringBuffer get_Name();
	/**
	 *  \brief Getter full path of the current item
	 *  
	 *  \return CStringBuffer
	 */
	CStringBuffer get_Path();

	/**
	 *  \brief Getter of path separator character
	 *  
	 *  \return mbchar
	 *  
	 *  \details Getter of path separator character from the search path object.
	 */
	mbchar PathSeparatorChar();
	/**
	 *  \brief Getter of path separator string
	 *  
	 *  \return CConstPointer
	 *  
	 *  \details Getter of path separator string from the search path object.
	 */
	CConstPointer PathSeparatorString();

private:
	CFilePath _filePath;
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS32
	intptr_t _hFileFind;
#if __MINGW32_MAJOR_VERSION < 4
#ifdef _UNICODE
	__wfinddata64_t _tFileFind;
#else
	__finddata64_t _tFileFind;
#endif
#else
#ifdef _UNICODE
	_wfinddata64_t _tFileFind;
#else
	_finddata64_t _tFileFind;
#endif
#endif
#endif	
#ifdef OK_SYS_WINDOWS64
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

