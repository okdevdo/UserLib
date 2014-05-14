/******************************************************************************
    
	This file is part of XCygwinSetup, which is part of UserLib.

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
#pragma once

#include "File.h"
#include "DataVector.h"
#include "MD5Buffer.h"

class CPackageVersion
{
public:
	CPackageVersion(void);
	CPackageVersion(ConstRef(CStringBuffer) _version);
	CPackageVersion(ConstRef(CPackageVersion) _copy);
	~CPackageVersion(void);

	void operator = (ConstRef(CStringBuffer) _copy);
	void operator = (ConstRef(CPackageVersion) _copy);

	__inline ConstRef(CStringBuffer) GetVersion() const { return m_sVersion; }
	__inline void SetVersion(ConstRef(CStringBuffer) _version) { m_sVersion = _version; }

	sword Compare(ConstRef(CPackageVersion) _other) const;

private:
	CStringBuffer m_sVersion;
};

__inline bool operator == (ConstRef(CPackageVersion) _a, ConstRef(CPackageVersion) _b)
{
	return (_a.Compare(_b) == 0);
}
__inline bool operator < (ConstRef(CPackageVersion) _a, ConstRef(CPackageVersion) _b)
{
	return (_a.Compare(_b) < 0);
}
__inline bool operator <= (ConstRef(CPackageVersion) _a, ConstRef(CPackageVersion) _b)
{
	return (_a.Compare(_b) <= 0);
}
__inline bool operator != (ConstRef(CPackageVersion) _a, ConstRef(CPackageVersion) _b)
{
	return (_a.Compare(_b) != 0);
}
__inline bool operator > (ConstRef(CPackageVersion) _a, ConstRef(CPackageVersion) _b)
{
	return (_a.Compare(_b) > 0);
}
__inline bool operator >= (ConstRef(CPackageVersion) _a, ConstRef(CPackageVersion) _b)
{
	return (_a.Compare(_b) >= 0);
}

class CInstallPackageInfo: public CCppObject
{
public:
	CInstallPackageInfo(void);
	CInstallPackageInfo(ConstRef(CStringBuffer) _name);
	CInstallPackageInfo(ConstRef(CStringBuffer) _name, ConstRef(CStringBuffer) _path, bool _bParseInstalled = false);
	virtual ~CInstallPackageInfo(void);

	__inline ConstRef(CFilePath) GetRelativePath() const { return m_RelativePath; }
	__inline void SetRelativePath(ConstRef(CFilePath) p) { m_RelativePath = p; }
	__inline ConstRef(CStringBuffer) GetName() const { return m_sName; }
	__inline void SetName(ConstRef(CStringBuffer) name) { m_sName = name; }
	__inline ConstRef(CPackageVersion) GetVersion() const { return m_Version; }
	__inline void SetVersion(ConstRef(CPackageVersion) v) { m_Version = v; }
	__inline ConstRef(CStringBuffer) GetType() const { return m_Type; }
	__inline ConstRef(CStringBuffer) GetExtension() const { return m_Extension; }

	__inline CFile::TFileSize GetFileSize() const { return m_FileSize; }
	__inline void SetFileSize(CFile::TFileSize _size) { m_FileSize = _size; }

	__inline ConstRef(CMD5Buffer) GetMD5Buffer() const { return m_MD5BUffer; }
	__inline void SetMD5Buffer(ConstRef(CStringBuffer) hexstr) { m_MD5BUffer = hexstr; }

	__inline bool GetIsInstalled() const { return m_IsInstalled; }
	__inline void SetIsInstalled(bool b) { m_IsInstalled = b; }

	void Print(WInt iVerbose, dword indent = 0) const;

private:
	void Parse1();
	void Parse2();

	CFilePath m_RelativePath;
	CStringBuffer m_sName;
	CPackageVersion m_Version;
	CStringBuffer m_Type;
	CStringBuffer m_Extension;
	CFile::TFileSize m_FileSize;
	CMD5Buffer m_MD5BUffer;
	bool m_IsInstalled;
};

class CInstallPackageInfoVector: public CDataVectorT<CInstallPackageInfo>
{
public:
	CInstallPackageInfoVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp);
	~CInstallPackageInfoVector();

	void LoadInstalled(CConstPointer _rootPath);
	void SaveInstalled(CConstPointer _rootPath);

	__inline bool GetModified() const { return m_Modified; }
	__inline void SetModified(bool b) { m_Modified = b; }

private:
	CInstallPackageInfoVector();
	CInstallPackageInfoVector(ConstRef(CInstallPackageInfoVector));
	void operator=(ConstRef(CInstallPackageInfoVector));

	bool m_Modified;
};

sword __stdcall InstallPackageInfoSearchAndSortFunc( ConstPointer pa, ConstPointer pb);
void __stdcall InstallPackageInfoDeleteFunc( ConstPointer data, Pointer context );
