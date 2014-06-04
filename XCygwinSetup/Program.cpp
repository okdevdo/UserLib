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
#include "DirectoryIterator.h"
#include "Filter.h"

class XCygwinSetupApplication: public CConsoleApplication
{
public:
	XCygwinSetupApplication():
		CConsoleApplication(CStringBuffer(__FILE__LINE__ APPLICATION_NAME)),
	    m_help(false),
		m_SetupIniFiles(false),
		m_sSetupIniFiles(__FILE__LINE__ 16, 16),
		m_SetupIniScanDir(false),
		m_sSetupIniScanDir(),
		m_Package(false),
		m_sPackages(__FILE__LINE__ 16, 16),
		m_Key(false),
		m_sKeys(__FILE__LINE__ 16, 16),
		m_InstallDir(false),
		m_sInstallDir(),
		m_Verbose(0),
		m_Installed(false),
		m_ImportFile(false),
		m_sImportFile(__FILE__LINE__ 16, 16),
		m_NoRequiredBy(false),
		m_NotInstalled(false),
		m_Install(false),
		m_Uninstall(false),
		m_OnlyPrint(false),
		m_Flags(0),
		m_FindCycles(false),
		m_Update(false),
		m_Output(false)
	{
        COptionCallback<XCygwinSetupApplication> cb(this,&XCygwinSetupApplication::handleHelp);
        
		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("SetupIniFiles"))
			.shortName(_T("f"))
			.argument(_T("files"))
			.category(_T("Cygwin"))
			.description(_T("Reads given 'setup.ini' files."))
			.repeatable(true));
		addOption(COption(_T("SetupIniScanDir"))
			.shortName(_T("d"))
			.argument(_T("dir"))
			.category(_T("Cygwin"))
			.description(_T("Scans 'dir' for 'setup.ini' files.")));
		addOption(COption(_T("Installed"))
			.shortName(_T("oi"))
			.category(_T("Cygwin"))
			.description(_T("Filters installed packages.")));
		addOption(COption(_T("NotInstalled"))
			.shortName(_T("on"))
			.category(_T("Cygwin"))
			.description(_T("Filters not installed packages.")));
		addOption(COption(_T("Update"))
			.shortName(_T("b"))
			.category(_T("Cygwin"))
			.description(_T("Updates installed packages, if any.")));
		addOption(COption(_T("Install"))
			.shortName(_T("lp"))
			.category(_T("Cygwin"))
			.description(_T("Installs given packages, not already installed, and those required.")));
		addOption(COption(_T("Uninstall"))
			.shortName(_T("up"))
			.category(_T("Cygwin"))
			.description(_T("Uninstalls given packages, that are not already uninstalled. Uninstall takes precedence, so that when option 'InstallPackages' is given too, a reinstall of a probably higher version will effectively be done.")));
		addOption(COption(_T("NoRequiredBy"))
			.shortName(_T("q"))
			.category(_T("Cygwin"))
			.description(_T("Filters packages not required by any other package.")));
		addOption(COption(_T("OnlyPrint"))
			.shortName(_T("n"))
			.category(_T("Utils"))
			.description(_T("Does only print search results and logs commands, but does not execute them. See also option 'verbose'.")));
		addOption(COption(_T("FindCycles"))
			.shortName(_T("c"))
			.category(_T("Utils"))
			.description(_T("Find cycles in package graph.")));
		addOption(COption(_T("Package"))
			.shortName(_T("p"))
			.argument(_T("package"))
			.category(_T("Cygwin"))
			.description(_T("Select 'package' from 'setup.ini' files."))
			.repeatable(true));
		addOption(COption(_T("ImportFile"))
			.shortName(_T("t"))
			.argument(_T("file"))
			.category(_T("Cygwin"))
			.description(_T("Load packages to be selected from file. Format of file is one package per line (DosLineEnd)."))
			.repeatable(true));
		addOption(COption(_T("Key"))
			.shortName(_T("k"))
			.argument(_T("key"))
			.category(_T("Cygwin"))
			.description(_T("Search for 'key' in 'setup.ini' files. Searches whether 'key' is a category."))
			.repeatable(true));
		addOption(COption(_T("InstallDir"))
			.shortName(_T("i"))
			.required(true)
			.argument(_T("dir"))
			.category(_T("Cygwin"))
			.description(_T("Setup root directory of Cygwin installation.")));
		addOption(COption(_T("Verbose"))
			.shortName(_T("v"))
			.argument(_T("level"), false)
			.category(_T("Utils"))
			.description(_T("Prints extended information. Level 0 means minimal output. Level 3 means maximum output.")));
		addOption(COption(_T("Output"))
			.shortName(_T("out"))
			.category(_T("Utils"))
			.description(_T("Redirects stdout to log file.")));
		}

	virtual ~XCygwinSetupApplication() 
	{
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral& value)
	{
		m_help = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		CERR << _T("Option = ") << name << _T(", value = ") << value << endl;

		if ( name == CStringLiteral(_T("SetupIniFiles")) )
		{
			m_SetupIniFiles = true;
			if ( !(value.IsEmpty()) )
				m_sSetupIniFiles.Append(value.GetString());
		}
		if ( name == CStringLiteral(_T("SetupIniScanDir")) )
		{
			m_SetupIniScanDir = true;
			m_sSetupIniScanDir = value;
		}
		if ( name == CStringLiteral(_T("Package")) )
		{
			m_Package = true;
			if ( !(value.IsEmpty()) )
				m_sPackages.InsertSorted(CStringBuffer(__FILE__LINE__ value));
		}
		if ( name == CStringLiteral(_T("Key")) )
		{
			m_Key = true;
			if ( !(value.IsEmpty()) )
				m_sKeys.Append(value.GetString());
		}
		if ( name == CStringLiteral(_T("ImportFile")) )
		{
			m_ImportFile = true;
			if ( !(value.IsEmpty()) )
				m_sImportFile.Append(value.GetString());
		}
		if ( name == CStringLiteral(_T("InstallDir")) )
		{
			m_InstallDir = true;
			m_sInstallDir = value;
		}
		if ( name == CStringLiteral(_T("Verbose")) )
		{
			CStringBuffer tmp(__FILE__LINE__ value);

			if ( tmp.IsEmpty() || (tmp.ScanString(_T("%d"), &m_Verbose) < 0) )
				m_Verbose = 0;
			if ( m_Verbose < 0 )
				m_Verbose = 0;
			if ( m_Verbose > 3 )
				m_Verbose = 3;
		}
		if ( name == CStringLiteral(_T("NoRequiredBy")) )
		{
			m_Flags |= FLAG_NoRequiredBy;
			m_NoRequiredBy = true;
		}
		if ( name == CStringLiteral(_T("Update")) )
		{
			m_Flags |= FLAG_Update;
			m_Update = true;
		}
		if ( name == CStringLiteral(_T("Installed")) )
		{
			m_Flags |= FLAG_Installed;
			m_Installed = true;
		}
		if ( name == CStringLiteral(_T("NotInstalled")) )
		{
			m_Flags |= FLAG_NotInstalled;
			m_NotInstalled = true;
		}
		if ( name == CStringLiteral(_T("Install")) )
		{
			m_Flags |= FLAG_InstallPackages;
			m_Install = true;
		}
		if ( name == CStringLiteral(_T("Uninstall")) )
		{
			m_Flags |= FLAG_UninstallPackages;
			m_Uninstall = true;
		}
		if ( name == CStringLiteral(_T("OnlyPrint")) )
		{
			m_Flags |= FLAG_OnlyPrint;
			m_OnlyPrint = true;
		}
		if ( name == CStringLiteral(_T("FindCycles")) )
		{
			m_Flags |= FLAG_FindCycles;
			m_FindCycles = true;
		}
		if (name == CStringLiteral(_T("Output")))
			m_Output = true;
	}

	virtual int main()
	{
		if ( m_help || (!m_InstallDir) || m_sInstallDir.IsEmpty() )
		{
			CStringBuffer tmp;

			usage(tmp);
			CERR << tmp;
			return -3;
		}
		{
			CFilePath tmp(__FILE__LINE__ m_sInstallDir);

			try
			{
				if (CDirectoryIterator::FileExists(tmp))
				{
					CERR << _T("Given Installation directory '") << m_sInstallDir << _T("' is really a file.") << endl;
					return -4;
				}
				tmp.MakeDirectory();
				if ( CDirectoryIterator::DirectoryExists(tmp) < 0 )
				{
					CERR << _T("Given Installation directory '") << m_sInstallDir << _T("' does not exist.") << endl;
					return -4;
				}
				tmp.set_Filename(_T("bin"));
				if ( CDirectoryIterator::DirectoryExists(tmp) < 0 )
				{
					CERR << _T("Given Installation directory '") << m_sInstallDir << _T("' is not a valid cygwin installation.") << endl;
					return -4;
				}
				tmp.set_Filename(_T("etc"));
				if ( CDirectoryIterator::DirectoryExists(tmp) < 0 )
				{
					CERR << _T("Given Installation directory '") << m_sInstallDir << _T("' is not a valid cygwin installation.") << endl;
					return -4;
				}
			}
			catch ( CBaseException* ex )
			{
				CERR << ex->GetExceptionMessage() << endl;
				return -4;
			}
		}
		if ( (!m_Update) && (!m_FindCycles) &&
			((!m_ImportFile) || (m_sImportFile.Count() == 0) ) &&
			((!m_Package) || (m_sPackages.Count() == 0) ) &&
			((!m_Key) || (m_sKeys.Count() == 0)) )
		{
			CStringBuffer tmp;

			CERR << _T("Either option 'FindCycles', 'Update', 'ImportFile', 'Package' and/or 'Key' must be given for the application to work.") << endl << endl;
			usage(tmp);
			CERR << tmp;
			return -4;
		}
		if ( m_Update )
		{
			m_Flags |= FLAG_InstallPackages;
			m_Install = true;
			m_Flags |= FLAG_UninstallPackages;
			m_Uninstall = true;
			m_Flags |= FLAG_Installed;
			m_Installed = true;
			m_Flags &= ~FLAG_NotInstalled;
			m_NotInstalled = false;
		}
		if ( m_NotInstalled && m_Installed )
			CERR << _T("Both options 'NotInstalled' and 'Installed' are found, so that all packages will be accepted.") << endl;
		if ( !m_NotInstalled && !m_Installed )
		{
			CERR << _T("None of the options 'NotInstalled' and 'Installed' are found, so that all packages will be accepted.") << endl;
			m_Flags |= FLAG_Installed;
			m_Flags |= FLAG_NotInstalled;
		}
		if ( m_Install && m_Uninstall )
			CERR << _T("Both options 'InstallPackages' and 'UninstallPackages' are found, so that a reinstall with a probably another version will be performed.") << endl;
		if ( m_ImportFile && (m_sImportFile.Count() > 0) )
		{
			TMBCharList::Iterator it = m_sImportFile.Begin();
			CCppObjectPtr<CSortedStringVectorFilterOutput> pOutput = OK_NEW_OPERATOR CSortedStringVectorFilterOutput(m_sPackages);

			while ( it )
			{
				CCppObjectPtr<CFileFilterInput> pInput;
				CCppObjectPtr<CLineReadFilter> pFilter;

				try
				{
					pInput = OK_NEW_OPERATOR CFileFilterInput(CFilePath(__FILE__LINE__ *it));
					pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput, CLineReadFilter::DosLineEnd);

					pFilter->open();
					pFilter->do_filter();
					pFilter->close();
				}
				catch ( CBaseException* ex )
				{
					CERR << ex->GetExceptionMessage() << endl;

					if ( NotPtrCheck(pFilter) )
						pFilter->close();
				}
				++it;
			}
		}

		CCppObjectPtr<CStreamFile> pOutFile;
		CFilePath foutf;
		CDateTime now(CDateTime::LocalTime);
		CStringBuffer tmp;

		try
		{
			if (m_Output)
			{
				m_Output = !(theConsoleApp->is_stdout_redirected());
				if (m_Output)
				{
					tmp.FormatString(__FILE__LINE__ APPLICATION_NAME _T("_%04d%02d%02d_%02d%02d%02d.log"),
						Cast(int, now.GetYears()), Cast(int, now.GetMonths()), Cast(int, now.GetDays()),
						Cast(int, now.GetHours()), Cast(int, now.GetMinutes()), Cast(int, now.GetSeconds()));
					foutf.set_Filename(tmp);
					pOutFile = OK_NEW_OPERATOR CStreamFile;
					pOutFile->ReOpen(foutf, stdout);
					theConsoleApp->set_stdout_redirected(true);
				}
			}
			if (!(m_sInstallDir.IsEmpty()))
				COUT << _T("InstallDir: ") << m_sInstallDir << endl;
			if (!(m_sSetupIniScanDir.IsEmpty()))
				COUT << _T("SetupIniScanDir: ") << m_sSetupIniScanDir << endl;

			TMBCharList::Iterator it0 = m_sSetupIniFiles.Begin();

			while (it0)
			{
				COUT << _T("SetupIniFile: ") << *it0 << endl;
				++it0;
			}
			
			CDataVectorT<CStringBuffer>::Iterator it1 = m_sPackages.Begin();

			while (it1)
			{
				COUT << _T("Package: ") << *it1 << endl;
				++it1;
			}

			TMBCharList::Iterator it2 = m_sKeys.Begin();

			while (it2)
			{
				COUT << _T("Key: ") << *it2 << endl;
				++it2;
			}
			
			if (m_Flags & FLAG_NoRequiredBy)
				COUT << _T("Flag: NoRequiredBy") << endl;
			if (m_Flags & FLAG_Installed)
				COUT << _T("Flag: Installed") << endl;
			if (m_Flags & FLAG_NotInstalled)
				COUT << _T("Flag: NotInstalled") << endl;
			if (m_Flags & FLAG_InstallPackages)
				COUT << _T("Flag: InstallPackages") << endl;
			if (m_Flags & FLAG_UninstallPackages)
				COUT << _T("Flag: UninstallPackages") << endl;
			if (m_Flags & FLAG_OnlyPrint)
				COUT << _T("Flag: OnlyPrint") << endl;
			if (m_Flags & FLAG_FindCycles)
				COUT << _T("Flag: FindCycles") << endl;
			if (m_Flags & FLAG_Update)
				COUT << _T("Flag: Update") << endl;
			if (m_Verbose)
				COUT << _T("Verbose: ") << m_Verbose << endl;
			COUT << endl;

			ScanSetupIni(m_sInstallDir, m_sSetupIniScanDir, m_sSetupIniFiles, m_sPackages, m_sKeys, m_Flags, m_Verbose);
		}
		catch (CBaseException* ex)
		{
			CERR << ex->GetExceptionMessage() << endl;
			if (pOutFile)
				pOutFile->Close();
			return -5;
		}
		if (pOutFile)
			pOutFile->Close();
		return 0;
	}
private:
	WBool m_help;
	WBool m_SetupIniFiles;
	TMBCharList m_sSetupIniFiles;
	WBool m_SetupIniScanDir;
	CStringLiteral m_sSetupIniScanDir;
	WBool m_Package;
	CDataVectorT<CStringBuffer> m_sPackages;
	WBool m_Key;
	TMBCharList m_sKeys;
	WBool m_InstallDir;
	CStringLiteral m_sInstallDir;
	WBool m_ImportFile;
	TMBCharList m_sImportFile;
	WInt m_Verbose;
	WBool m_NoRequiredBy;
	WBool m_Installed;
	WBool m_NotInstalled;
	WBool m_Install;
	WBool m_Uninstall;
	WBool m_OnlyPrint;
	WUInt m_Flags;
	WBool m_FindCycles;
	WBool m_Update;
	WBool m_Output;
};

int 
#ifdef OK_COMP_MSC
    _tmain( int argc, CPointer *argv )
#else
	main( int argc, char** argv )
#endif

{
	int res;

#ifdef OK_COMP_MSC
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
#endif
        
	XCygwinSetupApplication testApp;

	res = testApp.run(argc, argv);

#ifdef OK_COMP_MSC
#ifdef __DEBUG__
	CERR << _T("Please hit any key to exit ...");
	_getch();
	CERR << endl;
#endif
#endif
	return res;
}
