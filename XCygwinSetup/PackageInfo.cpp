/******************************************************************************
    
	This file is part of XSetup, which is part of UserLib.

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
#include "PackageInfo.h"

void __stdcall PackageInstallInfoDeleteFunc( ConstPointer data, Pointer context )
{
	CPackageInstallInfo* pPackageInfo = CastAnyPtr(CPackageInstallInfo, CastMutable(Pointer, data));

	pPackageInfo->release();
}

sword __stdcall PackageInstallInfoSearchAndSortFunc( ConstPointer pa, ConstPointer pb)
{
	CPackageInstallInfo* ppa = CastAnyPtr(CPackageInstallInfo, CastMutable(Pointer, pa));
	CPackageInstallInfo* ppb = CastAnyPtr(CPackageInstallInfo, CastMutable(Pointer, pb));

	return (ppa->GetVendorVersion().Compare(ppb->GetVendorVersion()));
}

CPackageInstallInfo::CPackageInstallInfo():
    _rootPath(),
	_vendorVersion(),
    _binaryPackage(NULL),
    _sourcePackage(NULL),
	_testVersion(false)
{
}

CPackageInstallInfo::CPackageInstallInfo(ConstRef(CFilePath) rootPath, ConstRef(CStringBuffer) vendorVersion):
    _rootPath(rootPath),
	_vendorVersion(vendorVersion),
    _binaryPackage(NULL),
    _sourcePackage(NULL),
	_testVersion(false)
{
}

CPackageInstallInfo::~CPackageInstallInfo()
{
	if ( NotPtrCheck(_binaryPackage) )
		_binaryPackage->release();
	_binaryPackage = NULL;
	if ( NotPtrCheck(_sourcePackage) )
		_sourcePackage->release();
	_sourcePackage = NULL;
}

void CPackageInstallInfo::Print(WInt iVerbose, dword indent) const
{
	if ( iVerbose > 1 )
	{
		if ( indent > 0 )
			COUT << std::setw(indent) << _T(" ");
		COUT << _T("version: ") << _vendorVersion.GetVersion();
		if ( _testVersion )
			COUT << _T(" (Test)");
		COUT << endl;
	}
	if ( iVerbose > 2 )
	{
		if ( indent > 0 )
			COUT << std::setw(indent) << _T(" ");
		COUT << _T("rootpath: ") << _rootPath.get_Path() << endl;
	}
	if ( NotPtrCheck(_binaryPackage) )
		_binaryPackage->Print(iVerbose, indent);
	if ( NotPtrCheck(_sourcePackage) )
		_sourcePackage->Print(iVerbose, indent);
}

CPackageInstallInfoVector::CPackageInstallInfoVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp):
    CDataVectorT<CPackageInstallInfo>(ARGS_FILE_LINE cnt, exp)
{
}

CPackageInstallInfoVector::~CPackageInstallInfoVector()
{
	Close(PackageInstallInfoDeleteFunc, NULL);
}

void CPackageInstallInfoVector::Print(WInt iVerbose, dword indent) const
{
	Iterator it = Begin();

	while ( it )
	{
		(*it)->Print(iVerbose, indent);
		++it;
	}
}

void __stdcall PackageInfoDeleteFunc( ConstPointer data, Pointer context )
{
	CPackageInfo* pPackageInfo = CastAnyPtr(CPackageInfo, CastMutable(Pointer, data));

	pPackageInfo->release();
}

static void __stdcall EmptyDeleteFunc( ConstPointer data, Pointer context )
{
}

sword __stdcall PackageInfoSearchAndSortFunc( ConstPointer pa, ConstPointer pb)
{
	CPackageInfo* ppa = CastAnyPtr(CPackageInfo, CastMutable(Pointer, pa));
	CPackageInfo* ppb = CastAnyPtr(CPackageInfo, CastMutable(Pointer, pb));

	return (ppa->GetPackageName().Compare(ppb->GetPackageName()));
}

sword __stdcall PackageInfoMatchFunc( ConstPointer pa, ConstPointer pb)
{
	CPackageInfo* ppa = CastAnyPtr(CPackageInfo, CastMutable(Pointer, pa));
	CPackageInfo* ppb = CastAnyPtr(CPackageInfo, CastMutable(Pointer, pb));

	return (s_strxcmp(ppa->GetPackageName().GetString(), ppb->GetPackageName().GetString())?0:-1);
}

CPackageInfo::CPackageInfo():
	_categories(__FILE__LINE__ 16, 16),
	_requiredPackages(__FILE__LINE__ 16, 16),
	_installPackages(__FILE__LINE__ 16, 16),
	_tobeInstalled(NULL),
	_visited(false),
	_finished(false),
	_installInfo(NULL),
	_requiredBy(__FILE__LINE__ 16, 16)
{
}

CPackageInfo::CPackageInfo(ConstRef(CStringBuffer) packageName):
    _packageName(packageName),
	_categories(__FILE__LINE__ 16, 16),
	_requiredPackages(__FILE__LINE__ 16, 16),
	_installPackages(__FILE__LINE__ 16, 16),
	_tobeInstalled(NULL),
	_visited(false),
	_finished(false),
	_installInfo(NULL),
	_requiredBy(__FILE__LINE__ 16, 16)
{
}

CPackageInfo::~CPackageInfo()
{
	_requiredBy.Close(EmptyDeleteFunc, NULL);
}

Ptr(CPackageInstallInfo) CPackageInfo::FindToBeInstalled()
{
	CPackageInstallInfoVector::Iterator itPII = _installPackages.Last();
	Ptr(CPackageInstallInfo) pPIInfoTest = NULL;
	Ptr(CPackageInstallInfo) pPIInfoLast = NULL;
	Ptr(CPackageInstallInfo) pPIInfo = NULL;
	
	while ( itPII )
	{
		pPIInfo = *itPII;
		if ( (pPIInfo->GetTestVersion()) && NotPtrCheck(pPIInfo->GetBinaryPackage()) )
			pPIInfoTest = pPIInfo;
		if ( (!(pPIInfo->GetTestVersion())) && NotPtrCheck(pPIInfo->GetBinaryPackage()) )
		{
			pPIInfoLast = pPIInfo;
			break;
		}
		--itPII;
	}
	if ( NotPtrCheck(pPIInfoLast) )
		return pPIInfoLast;
	if ( NotPtrCheck(pPIInfoTest) )
		return pPIInfoTest;
	return NULL;
}

void CPackageInfo::AddRequiredBy(CPackageInfo* pInfo) 
{
	CDataVectorT<CPackageInfo>::Iterator it = _requiredBy.FindSorted(pInfo, PackageInfoSearchAndSortFunc);

	if ( !(it && (*it) && (PackageInfoSearchAndSortFunc(*it, pInfo) == 0)) )
		_requiredBy.InsertSorted(pInfo, PackageInfoSearchAndSortFunc);
}

void CPackageInfo::RemoveAllRequiredBy()
{
	CDataVectorT<CPackageInfo>::Iterator it = _requiredBy.Last();

	while ( it )
	{
		_requiredBy.Remove(it, EmptyDeleteFunc, NULL);
		--it;
	}
}

void CPackageInfo::PrintHeader(WInt iVerbose)
{
	if ( iVerbose > 0 )
		COUT << _T("@ ");
	COUT << _packageName << endl;
	if ( iVerbose == 0 )
		return;
	COUT << _T("sdesc: ") << _sDesc << endl;
	if ( iVerbose > 1 )
		COUT << _T("ldesc: ") << _lDesc << endl;
	COUT << _T("category: ") << _categories.Join(_T(" ")) << endl;
	COUT << _T("requires: ") << _requiredPackages.Join(_T(" ")) << endl;
	_installPackages.Print(iVerbose);
}

void CPackageInfo::PrintRequiredBy(WInt iVerbose) 
{
	if ( iVerbose == 0 )
		return;

	CDataVectorT<CPackageInfo>::Iterator it = _requiredBy.Begin();

	while ( it )
	{
		CPackageInfo* pInfo = *it;

		COUT << _T("  @ ") << pInfo->GetPackageName() << endl;
		COUT << _T("  sdesc: ") << pInfo->GetShortDescription() << endl;
		if ( iVerbose > 1 )
			COUT << _T("  ldesc: ") << pInfo->GetLongDescription() << endl;
		COUT << _T("  category: ") << pInfo->GetCategories().Join(_T(" ")) << endl;
		COUT << _T("  requires: ") << pInfo->GetRequiredPackages().Join(_T(" ")) << endl;
		pInfo->GetPackageInstallInfos().Print(iVerbose, 2);
		++it;
	}
}
