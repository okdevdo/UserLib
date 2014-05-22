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
 *  \file FilePath.h
 *  \brief Handles file paths.
 */
#pragma once

#include "CppSources.h"

#ifdef OKBUILD_FOR_QT
#include <QtCore/QString>
#endif

#ifdef OK_SYS_WINDOWS
#define PATH_SEPDEFARG _T("\\")
#endif
#ifdef OK_SYS_UNIX
#define PATH_SEPDEFARG _T("/")
#endif

/**
 *  \class CFilePath
 *  \brief Handles file paths.
 */
class CPPSOURCES_API CFilePath
{
public:
	/**
	 *  \brief Standard constructor.
	 */
	CFilePath();
	/**
	 *  \brief Constructor.
	 *  \param [in] path Pointer to char buffer.
	 *  \param [in] len Length of char buffer, -1 for null termination.
	 *  \param [in] sep Path separator
	 */
	CFilePath(DECL_FILE_LINE CConstPointer path, int len = -1, CConstPointer sep = PATH_SEPDEFARG);
	/**
	 *  \brief Constructor.
	 *  \param [in] path Char literal.
	 *  \param [in] sep Path separator
	 */
	CFilePath(DECL_FILE_LINE CStringLiteral path, CConstPointer sep = PATH_SEPDEFARG);
	/**
	 *  \brief Constructor.
	 *  \param [in] path Char buffer.
	 *  \param [in] sep Path separator
	 */
	CFilePath(ConstRef(CStringBuffer) path, CConstPointer sep = PATH_SEPDEFARG);
#ifdef QT_VERSION
    CFilePath( ConstRef(QString) _str, CConstPointer sep = PATH_SEPDEFARG);
#endif
	/**
	 *  \brief Copy constructor.
	 *  \param [in] path Path to be copied.
	 */
    CFilePath(ConstRef(CFilePath) path);
	/** 
	 *  \brief Destructor.
	 */
	~CFilePath(void);

#ifdef QT_VERSION
    CFilePath& operator=(ConstRef(QString) path);
#endif
    CFilePath& operator=(ConstRef(CFilePath) path);
    CFilePath& operator=(ConstRef(CStringBuffer) path);

	/**
	 *  \brief Getter of the last extension, if more than one.
	 *  \return CStringBuffer.
	 */
	CStringBuffer get_Extension() const;
	/**
	 *  \brief Getter of the all extensions, if more than one.
	 *  \return CStringBuffer.
	 */
	CStringBuffer get_FullExtension() const;
	/**
	 *  \brief Getter of the file name without the last extension, if more than one.
	 *  \return CStringBuffer.
	 */
	CStringBuffer get_Basename() const;
	/**
	 *  \brief Getter of the file name without any extension, if more than one.
	 *  \return CStringBuffer.
	 */
	CStringBuffer get_PureBasename() const;
	/**
	 *  \brief Getter of the file name with all extensions, if more than one.
	 *  \return CStringBuffer.
	 */
	CStringBuffer get_Filename() const;
	/**
	 *  \brief Getter of the directory without root and file name.
	 *  \return CStringBuffer.
	 */
	CStringBuffer get_Directory() const;
	/**
	 *  \brief Getter of a directory component.
	 *  \param level Can be positive or negative. Zero is considered to be positive. If positive, the component counted from left to right will be returned, component zero is the first component after the root. If negative, the component counted from right to left will be returned, component -1 is the first component before the file name.
	 *  \return CStringBuffer.
	 */
	CStringBuffer get_Directory(int level) const;
	/** 
	 *  \brief Getter of the root inclusive the path separator.
	 *  \return CStringBuffer.
	 */
	CStringBuffer get_Root() const;
	/** 
	 *  \brief Getter of the complete path as stored.
	 *  \return ConstRef(CStringBuffer).
	 */
	__inline ConstRef(CStringBuffer) get_Path() const { return _path; }
	/**
	 *  \brief Getter of the path separator as stored.
	 *  \return CConstPointer.
	 */
	__inline CConstPointer get_PathSeparator() const { return _sPathSeparator; }
	__inline bool IsEmpty() const { return _path.IsEmpty(); }
	__inline sdword addRef() const  { return _path.addRef(); }
	__inline sdword countRef() const { return _path.countRef(); }
	__inline sdword releaseRef() const { return _path.releaseRef(); }

	__inline CConstPointer GetString() const { return _path.GetString(); }
	__inline operator CConstPointer() const { return _path.GetString(); }
	__inline const bool operator !() const { return IsEmpty(); }
	__inline void Clear() { _path.Clear(); }

	/**
	 *  \brief Setter of the last extension, if more than one.
	 *  \param [in] extension Replaces the last extension, if any. Else appended the extension.
	 *  \return void.
	 */
    void set_Extension(CStringLiteral extension);
	/**
	 *  \brief Setter of the full extension, if more than one.
	 *  \param [in] extension Replaces the full extension, if any. Else appended the extension.
	 *  \return void.
	 */
    void set_FullExtension(CStringLiteral extension);
	/**
	 *  \brief Setter of the file name without the last extension, if more than one.
	 *  \param [in] basename Replaces the file name without the last extension, if any. Else appended the basename to the directory.
	 *  \return void.
	 */
    void set_Basename(CStringLiteral basename);
	/**
	 *  \brief Setter of the file name without any extension, if more than one.
	 *  \param [in] basename Replaces the file name without any extension, if any. Else appended the basename to the directory.
	 *  \return void.
	 */
    void set_PureBasename(CStringLiteral basename);
	/**
	 *  \brief Setter of the file name with all extensions, if more than one.
	 *  \param [in] filename Replaces the file name with	all extensions, if any. Else appended the filename to the directory.
	 *  \return void.
	 */
    void set_Filename(CStringLiteral filename);
	/**
	 *  \brief Setter of the directory without root and filename.
	 *  \param [in] directory Replaces the directory, if any. Else appended the directory to the root.
	 *  \return void.
	 */
    void set_Directory(CStringLiteral directory);
	/**
	 *  \brief Setter of a directory component.
	 *  \param [in] directory Replaces the component, if any. Else appended the directory to the root.
	 *  \param [in] level See get_Directory(int) 
	 *  \return void.
	 */
    void set_Directory(CStringLiteral directory, int level);
	/**
	 *  \brief Convenience method. Appends directory to the list of directories.
	 *  \param [in] directory New directory component.
	 *  \return void.
	 */
	void append_Directory(CStringLiteral directory);
	/**
	 *  \brief Setter of the root inclusive the path separator.
	 *  \param [in] root New root. Replaces the existing root, if any.
	 *  \return void.
	 */
    void set_Root(CStringLiteral root);
	/**
	 *  \brief Setter of the complete path as stored.
	 *  \param [in] path Pointer to char buffer.
	 *  \param [in] sep Path separator.
	 *  \return void.
	 */
	__inline void set_Path(DECL_FILE_LINE CConstPointer path, CConstPointer sep = PATH_SEPDEFARG) { _path.SetString(ARGS_FILE_LINE path); _sPathSeparator = sep; }
	/**
	 *  \brief Setter of the complete path as stored.
	 *  \param [in] path String literal.
	 *  \param [in] sep Path separator.
	 *  \return void.
	 */
	__inline void set_Path(DECL_FILE_LINE CStringLiteral path, CConstPointer sep = PATH_SEPDEFARG) { _path.SetString(ARGS_FILE_LINE path); _sPathSeparator = sep; }
	/**
	 *  \brief Setter of the complete path as stored.
	 *  \param [in] path String buffer.
	 *  \param [in] sep Path separator.
	 *  \return void.
	 */
	__inline void set_Path(ConstRef(CStringBuffer) path, CConstPointer sep = PATH_SEPDEFARG) { _path = path; _sPathSeparator = sep; }
	/**
	 *  \brief Setter path separator as stored.
	 *  \param [in] sep Path separator.
	 *  \return void.
	 */
	__inline void set_PathSeparator(CConstPointer sep) { _sPathSeparator = sep; }

	/**
	 *  \brief Test, whether the path is a file name.
	 *  \return WBool.
	 */
	WBool is_File() const;
	/**
	 *  \brief Test, whether the path is a directory name.
	 *  \return WBool.
	 */
	WBool is_Directory() const;
	/**
	 *  \brief Test, whether the path is relative.
	 *  \return WBool.
	 */
	WBool is_Relative() const;
	/**
	 *  \brief Test, whether the path is absolute.
	 *  \return WBool.
	 */
	WBool is_Absolute() const;

	/**
	 *  \brief Make a relative path absolute. Method gets the current directory, and is therefore not thread-safe.
	 *  \return void.
	 */
	void MakeAbsolute();
	/**
	 *  \brief Appends a path separator to the stored path, if it is not already a directory (test with \c is_Directory).
	 *  \return void.
	 */
	void MakeDirectory();
	/**
	 *  \brief Path components like ".." will be resolved.
	 *  \param [in] replaceRoot Resets the path object to the default path separator and replaces the root, if not null.
	 *  \return void.
	 */
	void Normalize(CConstPointer replaceRoot = NULL);

	static bool CheckFilename(CStringLiteral filename);
    static CStringBuffer TrimFilename(CStringLiteral filename);

	static CConstPointer DefaultPathSeparatorString();
	static CConstPointer WinPathSeparatorString();
	static CConstPointer UnixPathSeparatorString();

private:
	CStringBuffer _path;
	CConstPointer _sPathSeparator;

    CFilePath& operator=(CConstPointer path);
    CFilePath& operator=(CStringLiteral path);

	static CConstPointer _sWinPathSeparator;
	static CConstPointer _sUnixPathSeparator;
};

