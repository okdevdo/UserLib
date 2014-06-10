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
#include "Program.h"
#include "ConsoleApplication.h"
#include "InstallPackageInfo.h"
#include "PackageInfo.h"
#include "PackageInfoVector.h"
#include "PackageInfoBTree.h"
#include "SetupIniFile.h"
#include "DirectoryIterator.h"
#include "SecurityContext.h"
#include "SecurityFile.h"
#include "DataGraph.h"
#include "WinDirectoryIterator.h"

static bool InstalledFilter(Ptr(CPackageInfo) pPack, WUInt flags)
{
	if ( (((flags & FLAG_Installed) == 0) && ((flags & FLAG_NotInstalled) == 0)) )
		return false;
	if ( (((flags & FLAG_Installed) != 0) && ((flags & FLAG_NotInstalled) != 0)) )
		return true;
	if ( NotPtrCheck(pPack->GetInstallInfo()) && (pPack->GetInstallInfo()->GetIsInstalled()) )
		return ((flags & FLAG_Installed) != 0);
	else
		return ((flags & FLAG_NotInstalled) != 0);
}

static void DepthFirstSearchFindCycles(Ref(CPackageInfoBTree) packAll, Ptr(CPackageInfo) node)
{
	if ( node->GetFinished() )
		return;
	if ( node->GetVisited() )
	{
		CERR << _T("FindCycle: Cycle found: '") << node->GetPackageName() << _T("'.") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("FindCycle: Cycle found: '") << node->GetPackageName() << _T("'.") << endl;
		node->SetFinished(true);
		return;
	}
	node->SetVisited(true);

	CDataVectorT<CStringBuffer>::Iterator it = node->GetRequiredPackages().Begin();
	CPackageInfoBTree::Iterator itF;
	CPackageInfo packInfo;

	while ( it )
	{
		packInfo.SetPackageName(*it);
		itF = packAll.FindSorted(&packInfo);
		if (itF)
			DepthFirstSearchFindCycles(packAll, *itF);
		else
		{
			CERR << _T("FindCycle: Cannot find package '") << *it << _T("' required by '") << node->GetPackageName() << _T("'.") << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << _T("FindCycle: Cannot find package '") << *it << _T("' required by '") << node->GetPackageName() << _T("'.") << endl;
		}
		++it;
	}
	node->SetFinished(true);
}

static void DepthFirstSearchInstall(Ref(CPackageInfoBTree) packAll, Ref(CPackageInfoVector) packInst, Ptr(CPackageInfo) node)
{
	if ( node->GetFinished() )
		return;
	if ( node->GetVisited() )
	{
		CERR << _T("Install: Cycle found: '") << node->GetPackageName() << _T("'.") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("Install: Cycle found: '") << node->GetPackageName() << _T("'.") << endl;
		node->SetFinished(true);
		return;
	}
	node->SetVisited(true);

	CDataVectorT<CStringBuffer>::Iterator it = node->GetRequiredPackages().Begin();
	CPackageInfoBTree::Iterator itF;
	CPackageInfo packInfo;
	Ptr(CPackageInfo) node1;

	while ( it )
	{
		packInfo.SetPackageName(*it);
		itF = packAll.FindSorted(&packInfo);
		if (itF)
		{
			node1 = *itF;
			if ( PtrCheck(node1->GetInstallInfo()) || (!(node1->GetInstallInfo()->GetIsInstalled())) )
			{
				DepthFirstSearchInstall(packAll, packInst, node1);
				if ( packInst.InsertSortedUnique(node1) )
					COUT << _T("Install: Package '") << node1->GetPackageName() << _T("' is required and will be installed.") << endl;
			}
		}
		else
		{
			CERR << _T("Install: Cannot find package '") << *it << _T("' required by '") << node->GetPackageName() << _T("'.") << endl;
			if (theConsoleApp->is_stdout_redirected())
				COUT << _T("Install: Cannot find package '") << *it << _T("' required by '") << node->GetPackageName() << _T("'.") << endl;
		}
		++it;
	}
	node->SetFinished(true);
}

void ScanSetupIni(CStringLiteral pInstallDir, CStringLiteral psSetupIniScanDir, Ref(TMBCharList) psSetupIniFiles,
	Ref(CDataVectorT<CStringBuffer>) psSearchPackage, Ref(TMBCharList) psSearchKey, WUInt flags, WInt iVerbose)
{
	CInstallPackageInfoVector packInstalled(__FILE__LINE__ 128, 128);
	CSetupIniFileVector inifiles(__FILE__LINE__ 5, 5);
	CPackageInfoBTree packAll(__FILE__LINE__ 128);
	CPackageInfoVector packages(__FILE__LINE__ 16, 16);

	packInstalled.LoadInstalled(pInstallDir);
	if ( packInstalled.Count() == 0 )
	{
		CERR << _T("Installed packages could not be loaded.") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("Installed packages could not be loaded.") << endl;
		CSecurityContext_FreeInstance
		return;
	}

	inifiles.Fill(psSetupIniScanDir, psSetupIniFiles);
	if ( inifiles.Count() == 0 )
	{
		CERR << _T("Could not find any setup file.") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("Could not find any setup file.") << endl;
		CSecurityContext_FreeInstance
		return;
	}
	inifiles.Load();
	{
		CSetupIniFileVector::Iterator itIniFile = inifiles.Begin();

		while ( itIniFile )
		{
			packAll.Fill(*itIniFile);
			++itIniFile;
		}
	}

	if ( packAll.Count() == 0 )
	{
		CERR << _T("Could not find any packages.") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("Could not find any packages.") << endl;
		CSecurityContext_FreeInstance
		return;
	}

	{
		CPackageInfoBTree::Iterator it = packAll.Begin();
		CInstallPackageInfoVector::Iterator itIP;
		CInstallPackageInfo ipInfo;
		Ptr(CInstallPackageInfo) pIpInfo;
		Ptr(CPackageInstallInfo) pPIInfo;
		Ptr(CPackageInfo) pPack;
		CFilePath fpath(__FILE__LINE__ CYGWIN_TMP_DIR, -1, CDirectoryIterator::UnixPathSeparatorString());
		CFilePath fpath1;
		CSecurityFile logfile;
		CStringBuffer tmp;
		CByteBuffer buf;

		if ( (flags & FLAG_Update) != 0 )
		{
			fpath.set_Filename(_T("Update.log"));
			fpath.Normalize(pInstallDir);
			fpath1 = fpath;
			fpath1.set_Extension(_T("log.bak"));
			CWinDirectoryIterator::UnlinkFile(fpath1);
			if ( CWinDirectoryIterator::FileExists(fpath) )
				CWinDirectoryIterator::Rename(fpath, fpath1);
			logfile.Create(fpath);
		}
		while ( it )
		{
			pPack = *it;
			ipInfo.SetName(pPack->GetPackageName());
			itIP = packInstalled.FindSorted(&ipInfo);
			if (itIP)
			{
				pIpInfo = *itIP;
				pPack->SetInstallInfo(pIpInfo);

				pPIInfo = pPack->FindToBeInstalled();
				if ( NotPtrCheck(pPIInfo) && (pPIInfo->GetVendorVersion().Compare(pIpInfo->GetVersion()) > 0) )
				{
					if ( (flags & FLAG_Update) != 0 )
					{
						packages.Append(pPack);
						tmp = pPack->GetPackageName();
						tmp += _T("\r\n");
						tmp.convertToByteBuffer(buf);
						logfile.Write(buf);
						CERR << _T("Package '") << pPack->GetPackageName()
							<< _T("' will be updated: new version = '") << pPIInfo->GetVendorVersion().GetVersion()
							<< _T("', current version = '") << pIpInfo->GetVersion().GetVersion()
							<< _T("'") << endl;
						if (theConsoleApp->is_stdout_redirected())
							COUT << _T("Package '") << pPack->GetPackageName()
								<< _T("' will be updated: new version = '") << pPIInfo->GetVendorVersion().GetVersion()
								<< _T("', current version = '") << pIpInfo->GetVersion().GetVersion()
								<< _T("'") << endl;
					}
					else
					{
						CERR << _T("Package '") << pPack->GetPackageName()
							<< _T("' can be updated: new version = '") << pPIInfo->GetVendorVersion().GetVersion()
							<< _T("', current version = '") << pIpInfo->GetVersion().GetVersion()
							<< _T("'") << endl;
						if (theConsoleApp->is_stdout_redirected())
							COUT << _T("Package '") << pPack->GetPackageName()
								<< _T("' can be updated: new version = '") << pPIInfo->GetVendorVersion().GetVersion()
								<< _T("', current version = '") << pIpInfo->GetVersion().GetVersion()
								<< _T("'") << endl;
					}
				}
			}
			++it;
		}
		if ( (flags & FLAG_Update) != 0 )
		{
			logfile.Close();
		}
	}

	if ( (flags & FLAG_FindCycles) > 0 )
	{
		CPackageInfoBTree::Iterator it;
		Ptr(CPackageInfo) pPack;

		it = packAll.Begin();
		while ( it )
		{
			pPack = *it;
			if ( InstalledFilter(pPack, flags) )
				DepthFirstSearchFindCycles(packAll, pPack);
			++it;
		}
		it = packAll.Begin();
		while ( it )
		{
			pPack = *it;
			pPack->SetFinished(false);
			pPack->SetVisited(false);
			++it;
		}
	}

	if ( psSearchPackage.Count() > 0 )
	{
		CDataVectorT<CStringBuffer>::Iterator it = psSearchPackage.Begin();
		CStringConstIterator itS;
		CPackageInfoBTree::Iterator itP;
		CPackageInfo packInfo;
		Ptr(CPackageInfo) pInfo = nullptr;

		while ( it )
		{
			packInfo.SetPackageName(*it);
			itS = packInfo.GetPackageName();
			if ( itS.FirstOf(_T("*?")).IsEnd() )
			{
				itP = packAll.FindSorted(&packInfo);
				if (itP)
				{
					pInfo = *itP;
					if ( InstalledFilter(pInfo, flags) )
						packages.InsertSortedUnique(pInfo);
				}
			}
			else
			{
				itP = packAll.Begin();
				while ( itP )
				{
					pInfo = *itP;
					if ( InstalledFilter(pInfo, flags) && (PackageInfoMatchFunc(pInfo, &packInfo) == 0) )
						packages.InsertSortedUnique(pInfo);
					++itP;
				}
			}
			++it;
		}
	}
	if ( psSearchKey.Count() > 0 )
	{
		CPackageInfoBTree::Iterator itP = packAll.Begin();
		TMBCharList::Iterator itK;
		Ptr(CPackageInfo) pInfo = nullptr;

		while ( itP )
		{
			pInfo = *itP;
			if ( InstalledFilter(pInfo, flags) )
			{
				itK = psSearchKey.Begin();
				while ( itK )
				{
					CStringBuffer tmp(__FILE__LINE__ *itK);

					if (pInfo->IsCategory(tmp))
					{
						packages.InsertSortedUnique(pInfo);
						break;
					}
					if (pInfo->IsShortDescription(tmp))
					{
						packages.InsertSortedUnique(pInfo);
						break;
					}
					if (pInfo->IsLongDescription(tmp))
					{
						packages.InsertSortedUnique(pInfo);
						break;
					}
					++itK;
				}
			}
			++itP;
		}
	}
	if ( packages.Count() == 0 )
	{
		CERR << _T("No packages found.") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("No packages found.") << endl;
		CSecurityContext_FreeInstance
		return;
	}
	if ( (((flags & FLAG_InstallPackages) == 0) && ((flags & FLAG_UninstallPackages) == 0)) || ((flags & FLAG_NoRequiredBy) != 0) ) 
	{
		CPackageInfoBTree::Iterator itP = packAll.Begin();
		CPackageInfoVector::Iterator itP2;
		CInstallPackageInfoVector::Iterator itIP;
		CInstallPackageInfo ipInfo;
		Ptr(CPackageInfo) pInfo = nullptr;
		Ptr(CPackageInfo) pInfo2 = nullptr;
		bool bAppend = false;

		while ( itP )
		{
			pInfo = *itP;
			if ( InstalledFilter(pInfo, flags) )
			{
				itP2 = packages.Begin();
				while ( itP2 )
				{
					pInfo2 = *itP2;
					if ( pInfo->IsRequiredPackage(pInfo2->GetPackageName()) )
						pInfo2->AddRequiredBy(pInfo);
					++itP2;
				}
			}
			++itP;
		}
		if ( (flags & FLAG_NoRequiredBy) != 0 )
		{
			CDataVectorT<CStringBuffer> delNames(__FILE__LINE__ 16, 16);
			CDataVectorT<CStringBuffer>::Iterator itDN;
			CPackageInfo PIinfo;

			itP2 = packages.Begin();
			while ( itP2 )
			{
				pInfo2 = *itP2;
				if ( pInfo2->GetRequiredBy().Count() > 0 )
					delNames.Append(pInfo2->GetPackageName());
				++itP2;
			}
			itDN = delNames.Begin();
			while ( itDN )
			{
				PIinfo.SetPackageName(*itDN);
				itP2 = packages.FindSorted(&PIinfo);
				if (itP2)
				{
					pInfo2 = *itP2;
					pInfo2->RemoveAllRequiredBy();
					packages.Remove(itP2);
				}
				++itDN;
			}
		}
	}
	if ( packages.Count() == 0 )
	{
		CERR << _T("No packages found.") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("No packages found.") << endl;
		CSecurityContext_FreeInstance
		return;
	}
	if ( ((flags & FLAG_InstallPackages) == 0) && ((flags & FLAG_UninstallPackages) == 0) ) 
	{
		CPackageInfoVector::Iterator itPI = packages.Begin();
		Ptr(CPackageInfo) pInfo = nullptr;

		while ( itPI )
		{
			pInfo = *itPI;
			pInfo->PrintHeader(iVerbose);
			pInfo->PrintRequiredBy(iVerbose);
			pInfo->RemoveAllRequiredBy();
			if ( iVerbose > 0 )
				COUT << endl;
			++itPI;
		}
	}
	else
	{
		if ( (flags & FLAG_UninstallPackages) != 0 )
		{
			CPackageInfoVector packUnInst(__FILE__LINE__ packages.Count(), 16);
			CPackageInfoVector::Iterator itPI;
			Ptr(CPackageInfo) pInfo = nullptr;

			itPI = packages.Begin();
			while ( itPI )
			{
				pInfo = *itPI;
				if ( NotPtrCheck(pInfo->GetInstallInfo()) && (pInfo->GetInstallInfo()->GetIsInstalled()) )
					packUnInst.InsertSortedUnique(pInfo);
				else
				{
					CERR << _T("Uninstall: Package '") << pInfo->GetPackageName() << _T("' is not installed.") << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << _T("Uninstall: Package '") << pInfo->GetPackageName() << _T("' is not installed.") << endl;
				}
				++itPI;
			}
			if (packUnInst.Count() == 0)
			{
				CERR << _T("Uninstall: Nothing to do.") << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << _T("Uninstall: Nothing to do.") << endl;
			}
			else
			{
				packUnInst.Uninstall(pInstallDir, ((flags & FLAG_OnlyPrint) != 0), iVerbose);
				packInstalled.SetModified(true);
			}
		}
		if ( (flags & FLAG_InstallPackages) != 0 )
		{
			CPackageInfoVector packInst(__FILE__LINE__ packages.Count(), 16);
			CPackageInfoVector::Iterator itPI;
			Ptr(CPackageInfo) pInfo = nullptr;

			COUT << _T("Install: Determine required packages.") << endl;
			itPI = packages.Begin();
			while ( itPI )
			{
				pInfo = *itPI;
				if ( PtrCheck(pInfo->GetInstallInfo()) || (!(pInfo->GetInstallInfo()->GetIsInstalled())) )
				{
					DepthFirstSearchInstall(packAll, packInst, pInfo);
					if ( packInst.InsertSortedUnique(pInfo) )
						COUT << _T("Install: Package '") << pInfo->GetPackageName() << _T("' will be installed.") << endl;
				}
				else
				{
					CERR << _T("Install: Package '") << pInfo->GetPackageName() << _T("' already installed. Please uninstall it first.") << endl;
					if (theConsoleApp->is_stdout_redirected())
						COUT << _T("Install: Package '") << pInfo->GetPackageName() << _T("' already installed. Please uninstall it first.") << endl;
				}
				++itPI;
			}
			if (packInst.Count() == 0)
			{
				CERR << _T("Install: Nothing to do.") << endl;
				if (theConsoleApp->is_stdout_redirected())
					COUT << _T("Install: Nothing to do.") << endl;
			}
			else
				packInst.Install(pInstallDir, packInstalled, ((flags & FLAG_OnlyPrint) != 0), iVerbose);
		}
		if ( ((flags & FLAG_OnlyPrint) == 0) && (packInstalled.GetModified()) )
			packInstalled.SaveInstalled(pInstallDir);
	}
	CSecurityContext_FreeInstance
}
