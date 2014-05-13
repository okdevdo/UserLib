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

#ifdef OKBUILD_FOR_QT
#include <QtCore/QString>
#endif

#ifdef OK_SYS_WINDOWS
#define PATH_SEPDEFARG _T("\\")
#endif
#ifdef OK_SYS_UNIX
#define PATH_SEPDEFARG _T("/")
#endif

class CPPSOURCES_API CFilePath
{
public:
	CFilePath();
	CFilePath(DECL_FILE_LINE CConstPointer path, int len = -1, CConstPointer sep = PATH_SEPDEFARG);
	CFilePath(DECL_FILE_LINE CStringLiteral path, CConstPointer sep = PATH_SEPDEFARG);
	CFilePath(ConstRef(CStringBuffer) path, CConstPointer sep = PATH_SEPDEFARG);
#ifdef QT_VERSION
    CFilePath( ConstRef(QString) _str, CConstPointer sep = PATH_SEPDEFARG);
#endif
    CFilePath(ConstRef(CFilePath) path);
	~CFilePath(void);

#ifdef QT_VERSION
    CFilePath& operator=(ConstRef(QString) path);
#endif
    CFilePath& operator=(ConstRef(CFilePath) path);
    CFilePath& operator=(ConstRef(CStringBuffer) path);

	CStringBuffer get_Extension() const;
	CStringBuffer get_FullExtension() const;
	CStringBuffer get_Basename() const;
	CStringBuffer get_PureBasename() const;
	CStringBuffer get_Filename() const;
	CStringBuffer get_Directory() const;
	CStringBuffer get_Directory(int level) const;
	CStringBuffer get_Root() const;
	__inline ConstRef(CStringBuffer) get_Path() const { return _path; }
	__inline CConstPointer get_PathSeparator() const { return _sPathSeparator; }
	__inline bool IsEmpty() const { return _path.IsEmpty(); }
	__inline sdword addRef() const  { return _path.addRef(); }
	__inline sdword countRef() const { return _path.countRef(); }
	__inline sdword releaseRef() const { return _path.releaseRef(); }

	__inline CConstPointer GetString() const { return _path.GetString(); }
	__inline operator CConstPointer() const { return _path.GetString(); }
	__inline const bool operator !() const { return IsEmpty(); }
	__inline void Clear() { _path.Clear(); }

    void set_Extension(CStringLiteral extension);
    void set_FullExtension(CStringLiteral extension);
    void set_Basename(CStringLiteral basename);
    void set_PureBasename(CStringLiteral basename);
    void set_Filename(CStringLiteral filename);
    void set_Directory(CStringLiteral directory);
    void set_Directory(CStringLiteral directory, int level);
	void append_Directory(CStringLiteral directory);
    void set_Root(CStringLiteral root);
	__inline void set_Path(DECL_FILE_LINE CConstPointer path, CConstPointer sep = PATH_SEPDEFARG) { _path.SetString(ARGS_FILE_LINE path); _sPathSeparator = sep; }
	__inline void set_Path(DECL_FILE_LINE CStringLiteral path, CConstPointer sep = PATH_SEPDEFARG) { _path.SetString(ARGS_FILE_LINE path); _sPathSeparator = sep; }
	__inline void set_Path(ConstRef(CStringBuffer) path, CConstPointer sep = PATH_SEPDEFARG) { _path = path; _sPathSeparator = sep; }
	__inline void set_PathSeparator(CConstPointer sep) { _sPathSeparator = sep; }

	WBool is_File() const;
	WBool is_Directory() const;
	WBool is_Relative() const;
	WBool is_Absolute() const;

	void MakeAbsolute();
	void MakeDirectory();
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

