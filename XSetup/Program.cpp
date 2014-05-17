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
#include "ConsoleApplication.h"
#include "File.h"
#include "DirectoryIterator.h"
#include "WinException.h"
#include "Configuration.h"

static bool IsWow64()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL(APIENTRY *LPFN_ISWOW64PROCESS)
		(HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;

	HMODULE module = GetModuleHandle(_T("kernel32"));
	const char funcName[] = "IsWow64Process";
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)
		GetProcAddress(module, funcName);

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
			throw OK_NEW_OPERATOR CWinException(__FILE__LINE__ _T("Exception In %s"), _T("IsWow64"), CWinException::WinExtError);
	}
	return bIsWow64 != FALSE;
}

static WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value)
{
	if (_name.Compare(_T("System.Environment."), 19, CStringLiteral::cIgnoreCase + CStringLiteral::cLimited) == 0)
	{
		CStringBuffer name(__FILE__LINE__ _name);

		name.DeleteString(0, 19);
#ifdef __MINGW32_MAJOR_VERSION
		CPointer buf = _tgetenv(name);
		dword bufsz;

		if (PtrCheck(buf))
			return false;
		bufsz = s_strlen(buf, 32768) + 1;
		_value.SetSize(__FILE__LINE__ bufsz);
		s_strcpy(CastMutable(CPointer, _value.GetString()), bufsz, buf);
#elif _MSC_VER
		size_t res;

		if (_tgetenv_s(&res, NULL, 0, name))
			return false;
		if (res == 0)
			return false;
		_value.SetSize(__FILE__LINE__ Castdword(res + 1));
		if (_tgetenv_s(&res, CastMutable(CPointer, _value.GetString()), res, name))
			return false;
#else
		CPointer buf = getenv(name);
		dword bufsz;

		if (PtrCheck(buf))
			return false;
		bufsz = s_strlen(buf, 32768) + 1;
		_value.SetSize(__FILE__LINE__ bufsz);
		s_strcpy(CastMutable(CPointer, _value.GetString()), bufsz, buf);
#endif
		return true;
	}
	return false;
}

class XSetupApplication : public CConsoleApplication
{
public:
	XSetupApplication() :
		CConsoleApplication(CStringBuffer(__FILE__LINE__ APPLICATION_NAME)),
		m_AnyOption(false),
		m_Output(false),
		m_sOutput(),
		m_help(false),
		m_GenXML(false),
		m_GenVS(false),
		m_sGenVS(),
		m_XMLFile(false),
		m_sXMLFile(),
		m_WorkDir(false),
		m_sWorkDir(),
		m_x64(false),
		m_verbose(false)
	{
		COptionCallback<XSetupApplication> cb(this, &XSetupApplication::handleHelp);
		CStringBuffer tmp;
		bool bv100 = false;
		bool bv110 = false;
		bool bv120 = false;
		int cnt = 0;
		int cnt2 = 0;

		if (GetRawValue(_T("System.Environment.VS100COMNTOOLS"), tmp))
			bv100 = true;
		if (GetRawValue(_T("System.Environment.VS110COMNTOOLS"), tmp))
			bv110 = true;
		if (GetRawValue(_T("System.Environment.VS120COMNTOOLS"), tmp))
			bv120 = true;

		tmp.SetString(__FILE__LINE__ _T("Generates Visual Studio Project Tree according to argument 'version' ("));
		if (bv100)
			++cnt;
		if (bv110)
			++cnt;
		if (bv120)
			++cnt;
		cnt2 = cnt;
		if (bv100)
		{
			tmp.AppendString(_T("v100"));
			--cnt;
			if (cnt > 0)
				tmp.AppendString(_T(", "));
		}
		if (bv110)
		{
			tmp.AppendString(_T("v110"));
			--cnt;
			if (cnt > 0)
				tmp.AppendString(_T(", "));
		}
		if (bv120)
		{
			tmp.AppendString(_T("v120"));
			--cnt;
			if (cnt > 0)
				tmp.AppendString(_T(", "));
		}
		if (cnt2 == 1)
			tmp.AppendString(_T(" is "));
		else
			tmp.AppendString(_T(" are "));
		tmp.AppendString(_T("supported)."));

		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("GenXML"))
			.shortName(_T("g1"))
			.category(_T("Setup"))
			.description(_T("Generates XML intermediate based on XSetup's properties.")));
		addOption(COption(_T("GenVS"))
			.shortName(_T("g2"))
			.argument(_T("version"), true)
			.category(_T("Setup"))
			.description(tmp));
		addOption(COption(_T("x64"))
			.shortName(_T("64"))
			.category(_T("Util"))
			.description(_T("Enforces x64 configurations, even if compiler is not installed.")));
		addOption(COption(_T("Verbose"))
			.shortName(_T("v"))
			.category(_T("Util"))
			.description(_T("Prints verbose infos.")));
		addOption(COption(_T("XMLFile"))
			.shortName(_T("x"))
			.argument(_T("path"), true)
			.category(_T("Util"))
			.description(_T("Names XML intermediate.")));
		addOption(COption(_T("WorkDir"))
			.shortName(_T("w"))
			.argument(_T("path"), true)
			.category(_T("Util"))
			.description(_T("Argument 'path' points to UserLib Source Tree Root. If this option is not given, current directory will be taken.")));
		addOption(COption(_T("Output"))
			.shortName(_T("o"))
			.argument(_T("outputfile"), false)
			.category(_T("Util"))
			.description(_T("Write all 'stdout' to 'outputfile'. 'outputfile' defaults to '") APPLICATION_NAME _T("YYYYMMDDHHMMSS.log', if not given.")));
	}

	virtual ~XSetupApplication()
	{
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral& value)
	{
		m_help = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		if (name == CStringLiteral(_T("Output")))
		{
			m_Output = true;
			m_sOutput = value;
		}
		if (name == CStringLiteral(_T("WorkDir")))
		{
			m_WorkDir = true;
			m_sWorkDir = value;
		}
		if (name == CStringLiteral(_T("XMLFile")))
		{
			m_XMLFile = true;
			m_sXMLFile = value;
		}
		if (name == CStringLiteral(_T("x64")))
			m_x64 = true;
		if (name == CStringLiteral(_T("Verbose")))
			m_verbose = true;
		if (name == CStringLiteral(_T("GenXML")))
		{
			m_GenXML = true;
			m_AnyOption = true;
		}
		if (name == CStringLiteral(_T("GenVS")))
		{
			m_GenVS = true;
			m_sGenVS = value;
			m_AnyOption = true;
		}
	}

	virtual int main()
	{
		CFilePath curdir;
		CStreamFile* pOutFile = NULL;

		if (m_help || (!m_AnyOption))
		{
			CStringBuffer tmp;

			usage(tmp);
			CERR << tmp;
			return 0;
		}

		try
		{
			CDirectoryIterator::GetCurrentDirectory(curdir);
			if (m_WorkDir)
				CDirectoryIterator::SetCurrentDirectory(CFilePath(__FILE__LINE__ m_sWorkDir));
		}
		catch (CBaseException *ex)
		{
			CERR << ex->GetExceptionMessage() << endl;
			return 4;
		}

		if (m_Output)
		{
			if (!(theConsoleApp->is_stdout_redirected()))
			{
				CFilePath foutf;

				if (m_sOutput.IsEmpty())
				{
					CDateTime now(CDateTime::LocalTime);
					CStringBuffer tmp;

					tmp.FormatString(__FILE__LINE__ APPLICATION_NAME _T("%04d%02d%02d%02d%02d%02d.log"),
						Cast(int, now.GetYears()), Cast(int, now.GetMonths()), Cast(int, now.GetDays()),
						Cast(int, now.GetHours()), Cast(int, now.GetMinutes()), Cast(int, now.GetSeconds()));
					foutf.set_Filename(tmp);
				}
				else
					foutf.set_Path(__FILE__LINE__ m_sOutput);
				pOutFile = OK_NEW_OPERATOR CStreamFile;
				pOutFile->ReOpen(foutf, stdout);
				theConsoleApp->set_stdout_redirected(true);
			}
		}

		if (m_verbose)
		{
			CFilePath path;

			CDirectoryIterator::GetCurrentDirectory(path);
			COUT << _T("Current directory is '") << path.get_Path() << _T("'.") << endl;
		}

		try
		{
			CFilePath fname(__FILE__LINE__ APPLICATION_NAME _T(".exe.properties"));

			if (CDirectoryIterator::FileExists(fname))
			{
				Ptr(CConfigurationList) pConfigList = CastDynamicPtr(CConfigurationList, theApp->config());
				CConfigurationList::Iterator it = pConfigList->Begin();
				int removedConfigCnt = 0;

				if (m_verbose)
				{
					COUT << _T("Properties file '") << fname.get_Path() << _T("' exists in current directory.") << endl;
					COUT << _T("Will try to replace current configuration from executable directory with properties file found.") << endl;
				}
				while (it)
				{
					Ptr(CAbstractConfiguration) pConfig = *it;

					if (pConfig->GetPrefix().Compare(CStringLiteral(APPLICATION_NAME), 0, CStringLiteral::cIgnoreCase) == 0)
					{
						CConfigurationList::Iterator it1 = it;

						++it1;
						pConfigList->Remove(it);
						++removedConfigCnt;
						it = it1;
					}
					else
						++it;
				}
				if (m_verbose)
					COUT << _T("Number of configurations removed are ") << removedConfigCnt << _T(".") << endl;

				CDirectoryIterator itD;
				int addedConfigCnt = 0;

				itD.Open(CFilePath(__FILE__LINE__ APPLICATION_NAME _T(".*")));
				while (itD)
				{
					CFilePath configPath = itD.get_Path();
					CStringBuffer ext = configPath.get_Extension();

					if ((!(ext.IsEmpty())) && (ext.Compare(_T("properties"), 0, CStringLiteral::cIgnoreCase) == 0))
					{
						CFileConfiguration* pConfig = OK_NEW_OPERATOR CFileConfiguration(__FILE__LINE__ configPath);

						pConfigList->Add(pConfig);
						++addedConfigCnt;
						if (m_verbose)
							COUT << _T("Properties file '") << configPath.get_Path() << _T("' added.") << endl;
					}
					++itD;
				}
				if (m_verbose)
					COUT << _T("Number of configurations added are ") << addedConfigCnt << _T(".") << endl;
			}
		}
		catch (CBaseException *ex)
		{
			CERR << ex->GetExceptionMessage() << endl;
			return 4;
		}

		if (!m_XMLFile)
			m_sXMLFile = _T("ProjectFile.xml");

		if (m_GenXML)
		{
			if (m_verbose)
				COUT << _T("Will generate XML intermediate '") << m_sXMLFile << _T("'.") << endl;
			MetaProjectCreateProjectFilesXML(m_sXMLFile);
			if (m_verbose)
			{
				CFilePath path(__FILE__LINE__ m_sXMLFile);
				CDirectoryIterator it(path);

				COUT << _T("XML intermediate generated.") << endl;
				while (it)
				{
					CStringBuffer timeBuffer;

					it.get_LastWriteTime().GetTimeString(timeBuffer);
					COUT << _T("Name: '") << it.get_Name() << _T("', LastWriteTime: '") << timeBuffer << _T("' FileSize: ") << it.get_FileSize() << endl;
					++it;
				}		
			}
		}

		if (m_GenVS)
		{
			CStringBuffer tmp;
			CFilePath path;
			bool hasWin64 = m_x64;

			if (m_verbose)
				COUT << _T("Will generate Visual Studio files.") << endl;
			if (m_sGenVS.Compare(CStringLiteral(_T("v100")), 0, CStringLiteral::cIgnoreCase) == 0)
			{
				if (m_verbose)
					COUT << _T("Checking whether Visual Studio 2010 is installed.") << endl;
				tmp = theApp->config()->GetValue(_T("System.Environment.VS100COMNTOOLS"));
				if (tmp.IsEmpty())
				{
					CERR << _T("Environment variable not set. Visual Studio 2010 seems to be not installed.") << endl;
					if (m_verbose)
					{
						if ((theConsoleApp->is_stdout_redirected()) || (theConsoleApp->is_stderr_redirected()))
							COUT << _T("Environment variable not set. Visual Studio 2010 seems to be not installed.") << endl;
					}
				}
				else
				{
					path.set_Path(tmp);
					if (CDirectoryIterator::DirectoryExists(path) >= 0)
					{
						CERR << _T("Environment variable set, but Visual Studio 2010 is not installed.") << endl;
						if (m_verbose)
						{
							if ((theConsoleApp->is_stdout_redirected()) || (theConsoleApp->is_stderr_redirected()))
								COUT << _T("Environment variable set, but Visual Studio 2010 is not installed.") << endl;
						}
					}
					else
					{
						if (m_verbose)
							COUT << _T("Visual Studio 2010 is installed.") << endl;
						path.append_Directory(_T("..\\..\\VC\\bin\\x86_amd64\\"));
						if (CDirectoryIterator::DirectoryExists(path) >= 0)
						{
							if (m_verbose)
								COUT << _T("Visual Studio 2010(x64) is installed.") << endl;
							hasWin64 = true;
						}
					}
				}
			}
			else if (m_sGenVS.Compare(CStringLiteral(_T("v110")), 0, CStringLiteral::cIgnoreCase) == 0)
			{
				if (m_verbose)
					COUT << _T("Checking whether Visual Studio 2012 is installed.") << endl;
				tmp = theApp->config()->GetValue(_T("System.Environment.VS110COMNTOOLS"));
				if (tmp.IsEmpty())
				{
					CERR << _T("Environment variable not set. Visual Studio 2012 seems to be not installed.") << endl;
					if (m_verbose)
					{
						if ((theConsoleApp->is_stdout_redirected()) || (theConsoleApp->is_stderr_redirected()))
							COUT << _T("Environment variable not set. Visual Studio 2012 seems to be not installed.") << endl;
					}
				}
				else
				{
					path.set_Path(tmp);
					if (CDirectoryIterator::DirectoryExists(path) >= 0)
					{
						CERR << _T("Environment variable set, but Visual Studio 2012 is not installed.") << endl;
						if (m_verbose)
						{
							if ((theConsoleApp->is_stdout_redirected()) || (theConsoleApp->is_stderr_redirected()))
								COUT << _T("Environment variable set, but Visual Studio 2012 is not installed.") << endl;
						}
					}
					else
					{
						if (m_verbose)
							COUT << _T("Visual Studio 2012 is installed.") << endl;
						path.append_Directory(_T("..\\..\\VC\\bin\\x86_amd64\\"));
						if (CDirectoryIterator::DirectoryExists(path) >= 0)
						{
							if (m_verbose)
								COUT << _T("Visual Studio 2012(x64) is installed.") << endl;
							hasWin64 = true;
						}
					}
				}
			}
			else
			{
				if (m_verbose)
					COUT << _T("Checking whether Visual Studio 2013 is installed.") << endl;
				tmp = theApp->config()->GetValue(_T("System.Environment.VS120COMNTOOLS"));
				if (tmp.IsEmpty())
				{
					CERR << _T("Environment variable not set. Visual Studio 2013 seems to be not installed.") << endl;
					if (m_verbose)
					{
						if ((theConsoleApp->is_stdout_redirected()) || (theConsoleApp->is_stderr_redirected()))
							COUT << _T("Environment variable not set. Visual Studio 2013 seems to be not installed.") << endl;
					}
				}
				else
				{
					path.set_Path(tmp);
					if (CDirectoryIterator::DirectoryExists(path) >= 0)
					{
						CERR << _T("Environment variable set, but Visual Studio 2013 is not installed.") << endl;
						if (m_verbose)
						{
							if ((theConsoleApp->is_stdout_redirected()) || (theConsoleApp->is_stderr_redirected()))
								COUT << _T("Environment variable set, but Visual Studio 2013 is not installed.") << endl;
						}
					}
					else
					{
						if (m_verbose)
							COUT << _T("Visual Studio 2013 is installed.") << endl;
						path.append_Directory(_T("..\\..\\VC\\bin\\x86_amd64\\"));
						if (CDirectoryIterator::DirectoryExists(path) >= 0)
						{
							if (m_verbose)
								COUT << _T("Visual Studio 2013(x64) is installed.") << endl;
							hasWin64 = true;
						}
					}
				}
			}
			if (m_verbose)
				COUT << _T("Generating Visual Studio files with arguments: XML intermediate '") << m_sXMLFile << _T("', VS version '") << m_sGenVS << _T("', x64 is ") << (hasWin64 ? _T("on.") : _T("off.")) << endl;
			MetaProjectCreateProjectFilesVS(m_sXMLFile, m_sGenVS, hasWin64);
			if (m_verbose)
				COUT << _T("Visual Studio files generated.") << endl;
		}

#ifdef OK_COMP_MSC
		if (NotPtrCheck(pOutFile))
		{
			pOutFile->Close();
			pOutFile->release();
		}
#endif
		return 0;
	}
private:
	WBool m_AnyOption;
	WBool m_Output;
	CStringLiteral m_sOutput;
	WBool m_help;
	WBool m_GenXML;
	WBool m_GenVS;
	CStringLiteral m_sGenVS;
	WBool m_XMLFile;
	CStringLiteral m_sXMLFile;
	WBool m_WorkDir;
	CStringLiteral m_sWorkDir;
	WBool m_x64;
	WBool m_verbose;
};

int
#ifdef OK_COMP_GNUC
main( int argc, char** argv )
#endif
#ifdef OK_COMP_MSC
_tmain(int argc, CPointer *argv)
#endif

{
	int res;

	XSetupApplication testApp;

	res = testApp.run(argc, argv);

#if defined(OK_SYS_WINDOWS) && defined(__DEBUG__)
	_getch();
#endif
	return res;
}
