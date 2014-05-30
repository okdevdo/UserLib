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

#include "DataVector.h"
#include "InstallPackageInfo.h"

class CPackageInstallInfo: public CCppObject
{
public:
	CPackageInstallInfo();
	CPackageInstallInfo(ConstRef(CFilePath) rootPath, ConstRef(CStringBuffer) vendorVersion);
	virtual ~CPackageInstallInfo();

	__inline ConstRef(CFilePath) GetRootPath() const { return _rootPath; }
	__inline ConstRef(CPackageVersion) GetVendorVersion() const { return _vendorVersion; }

	__inline ConstPtr(CInstallPackageInfo) GetBinaryPackage() const { return _binaryPackage; }
	__inline void SetBinaryPackage(Ptr(CInstallPackageInfo) p) 
	{
		if ( NotPtrCheck(_binaryPackage) )
			_binaryPackage->release();
		_binaryPackage = p; 
	}

	__inline ConstPtr(CInstallPackageInfo) GetSourcePackage() const { return _sourcePackage; }
	__inline void SetSourcePackage(Ptr(CInstallPackageInfo) p)
	{ 
		if ( NotPtrCheck(_sourcePackage) )
			_sourcePackage->release();
		_sourcePackage = p; 
	}

	__inline bool GetTestVersion() const { return _testVersion; }
	__inline void SetTestVersion(bool b) { _testVersion = b; }

	void Print(WInt iVerbose, dword indent = 0) const;

private:
	CFilePath _rootPath;
	CPackageVersion _vendorVersion;
	Ptr(CInstallPackageInfo) _binaryPackage;
	Ptr(CInstallPackageInfo) _sourcePackage;
	bool _testVersion;
};

class CPackageInstallInfoLessFunctor
{
public:
	bool operator()(ConstPtr(CPackageInstallInfo) r1, ConstPtr(CPackageInstallInfo) r2) const
	{
		return r1->GetVendorVersion() < r2->GetVendorVersion();
	}
};

class CPackageInstallInfoVector : public CDataVectorT<CPackageInstallInfo, CPackageInstallInfoLessFunctor>
{
	typedef CDataVectorT<CPackageInstallInfo, CPackageInstallInfoLessFunctor> super;

public:
	CPackageInstallInfoVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp);
	~CPackageInstallInfoVector();

	void Print(WInt iVerbose, dword indent = 0) const;
};

sword __stdcall PackageInstallInfoSearchAndSortFunc( ConstPointer pa, ConstPointer pb);
void __stdcall PackageInstallInfoDeleteFunc( ConstPointer data, Pointer context );

class CPackageInfo: public CCppObject
{
public:
	typedef CDataVectorT<CPackageInfo, CStringByNameLessFunctor<CPackageInfo>, CCppObjectNullFunctor<CPackageInfo>> CRequiredByList;

	CPackageInfo();
	CPackageInfo(ConstRef(CStringBuffer) packageName);
	virtual ~CPackageInfo();

	__inline ConstRef(CStringBuffer) get_Name() const { return _packageName; }
	__inline ConstRef(CStringBuffer) GetPackageName() const { return _packageName; }
	__inline void SetPackageName(ConstRef(CStringBuffer) name) { _packageName = name; }

	__inline ConstRef(CStringBuffer) GetShortDescription() const { return _sDesc; }
	__inline bool IsShortDescription(ConstRef(CStringBuffer) s)
	{
		CStringConstIterator it(_sDesc);

		it.Find(s);
		return !(it.IsEnd());
	}
	__inline void SetShortDescription(ConstRef(CStringBuffer) sDesc) { _sDesc = sDesc; }

	__inline ConstRef(CStringBuffer) GetLongDescription() const { return _lDesc; }
	__inline bool IsLongDescription(ConstRef(CStringBuffer) s)
	{
		CStringConstIterator it(_lDesc);

		it.Find(s);
		return !(it.IsEnd());
	}
	__inline void SetLongDescription(ConstRef(CStringBuffer) lDesc) { _lDesc = lDesc; }
	__inline void AddLongDescription(ConstRef(CStringBuffer) lDesc) { _lDesc += lDesc; }

	__inline ConstRef(CDataVectorT<CStringBuffer>) GetCategories() const { return _categories; }
	__inline bool IsCategory(ConstRef(CStringBuffer) category)
	{
		CDataVectorT<CStringBuffer>::Iterator it = _categories.FindSorted(category);

		return (it && (*it == category));
	}
	__inline void AddCategory(ConstRef(CStringBuffer) cat) { if ( !IsCategory(cat) ) _categories.InsertSorted(cat); }

	__inline ConstRef(CDataVectorT<CStringBuffer>) GetRequiredPackages() const { return _requiredPackages; }
	__inline bool IsRequiredPackage(ConstRef(CStringBuffer) packageName)
	{
		CDataVectorT<CStringBuffer>::Iterator it = _requiredPackages.FindSorted(packageName);

		return (it && (*it == packageName));
	}
	__inline void AddRequiredPackage(ConstRef(CStringBuffer) pack) { if ( (!(pack.IsEmpty())) && (!(IsRequiredPackage(pack))) ) _requiredPackages.InsertSorted(pack); }

	__inline Ref(CPackageInstallInfoVector) GetPackageInstallInfos() { return _installPackages; }
	__inline void AddPackageInstallInfo(Ptr(CPackageInstallInfo) info) { _installPackages.InsertSorted(info, PackageInstallInfoSearchAndSortFunc); }
	Ptr(CPackageInstallInfo) FindToBeInstalled();

	__inline Ptr(CPackageInstallInfo) GetToBeInstalled() { return _tobeInstalled; }
	__inline void SetToBeInstalled(Ptr(CPackageInstallInfo) p) { _tobeInstalled = p; }

	__inline bool GetVisited() const { return _visited; }
	__inline void SetVisited(bool b) { _visited = b; }

	__inline bool GetFinished() const { return _finished; }
	__inline void SetFinished(bool b) { _finished = b; }

	__inline Ptr(CInstallPackageInfo) GetInstallInfo() { return _installInfo; }
	__inline void SetInstallInfo(Ptr(CInstallPackageInfo) p) { _installInfo = p; }

	Ref(CRequiredByList) GetRequiredBy() { return _requiredBy; }
	void AddRequiredBy(CPackageInfo* pInfo);
	void RemoveAllRequiredBy();

	void PrintHeader(WInt iVerbose);
	void PrintRequiredBy(WInt iVerbose);

private:
	CPackageInfo(ConstRef(CPackageInfo) _copy);
	ConstRef(CPackageInfo) operator = (ConstRef(CPackageInfo) _copy);

	CStringBuffer _packageName;
	CStringBuffer _sDesc;
	CStringBuffer _lDesc;
	CDataVectorT<CStringBuffer> _categories;
	CDataVectorT<CStringBuffer> _requiredPackages;
	CPackageInstallInfoVector _installPackages;
	Ptr(CPackageInstallInfo) _tobeInstalled;
	bool _visited;
	bool _finished;
	Ptr(CInstallPackageInfo) _installInfo;
	CRequiredByList _requiredBy;
};

sword __stdcall PackageInfoSearchAndSortFunc( ConstPointer pa, ConstPointer pb);
sword __stdcall PackageInfoMatchFunc( ConstPointer pa, ConstPointer pb);
void __stdcall PackageInfoDeleteFunc( ConstPointer data, Pointer context );
