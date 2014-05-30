/******************************************************************************
    
	This file is part of XZip, which is part of UserLib.

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
// XZip.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "Application.h"
#include "File.h"
#include "DirectoryIterator.h"
#include "SecurityFile.h"
#include "SecurityContext.h"
#include "WinDirectoryIterator.h"
#ifdef OK_COMP_MSC
#ifdef __DEBUG__
#include <conio.h>
#endif
#endif
#include <stdio.h>

static void __stdcall VectorEmptyDeleteFunc( ConstPointer data, Pointer context )
{
}

class XZipApplication: public CApplication
{
public:
	XZipApplication():
		CApplication(CStringBuffer(__FILE__LINE__ _T("XZip"))),
		m_AnyZipOption(false),
	    m_help(false),
		m_bAddFiles(false),
		m_bFreshenFiles(false),
		m_bUpdateFiles(false),
		m_bViewFiles(false),
		m_sViewFiles(),
		m_bRecurseFolders(false),
		m_bExclude(false),
		m_sExclude(__FILE__LINE__ 16, 16),
		m_bZipArchive(false),
		m_sZipArchive(),
		m_bZipFileSpec(false),
		m_sZipFileSpec(__FILE__LINE__ 16, 16),
		m_bInputDir(false),
		m_sInputDir(),
		m_bOutputFile(false),
		m_sOutputFile()
	{
        COptionCallback<XZipApplication> cb(this,&XZipApplication::handleHelp);
                
		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("AddFiles"))
			.shortName(_T("a"))
			.group(_T("ZipOption"))
			.category(_T("Zip"))
			.description(_T("Adds files to zip archive.")));
		addOption(COption(_T("FreshenFiles"))
			.shortName(_T("f"))
			.group(_T("ZipOption"))
			.category(_T("Zip"))
			.description(_T("Freshen files in zip archive.")));
		addOption(COption(_T("UpdateFiles"))
			.shortName(_T("u"))
			.group(_T("ZipOption"))
			.category(_T("Zip"))
			.description(_T("Updates files in zip archive.")));
		addOption(COption(_T("ViewFiles"))
			.shortName(_T("v"))
			.group(_T("ZipOption"))
			.argument(_T("mode"), false)
			.category(_T("Zip"))
			.description(_T("View zip file files. Argument can be 'verbose', 'full' or 'delta'.")));
		addOption(COption(_T("RecurseFolders"))
			.shortName(_T("r"))
			.category(_T("Zip"))
			.description(_T("Recurse Folders.")));
		addOption(COption(_T("Exclude"))
			.shortName(_T("x"))
			.argument(_T("path"))
			.repeatable(true)
			.category(_T("Zip"))
			.description(_T("Exclude files and directories from archiving. Wildcards allowed.")));
		addOption(COption(_T("ZipArchive"))
			.shortName(_T("z"))
			.required(true)
			.argument(_T("path"), true, 1)
			.category(_T("Zip"))
			.description(_T("Specifies the zip archive file.")));
		addOption(COption(_T("FileSpec"))
			.shortName(_T("s"))
			.argument(_T("spec"))
			.repeatable(true)
			.category(_T("Zip"))
			.description(_T("Specifies the files to be zipped.")));
		addOption(COption(_T("InputDir"))
			.shortName(_T("d"))
			.argument(_T("path"))
			.category(_T("Zip"))
			.description(_T("From this directory files will be taken, to be added, updated or freshened to the archive (default: <current directory>).")));
		addOption(COption(_T("OutputFile"))
			.shortName(_T("o"))
			.argument(_T("path"))
			.category(_T("Zip"))
			.description(_T("In this file stdout will be redirected.")));
	}
	virtual ~XZipApplication()
	{
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral& value)
	{
		m_help = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		COUT << _T("Option = ") << name << _T(", value = ") << value << endl;
		if ( name.Compare(_T("AddFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bAddFiles = true;
		}
		if ( name.Compare(_T("FreshenFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bFreshenFiles = true;
		}
		if ( name.Compare(_T("UpdateFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bUpdateFiles = true;
		}
		if ( name.Compare(_T("ViewFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bViewFiles = true;
			m_sViewFiles = value;
		}
		if ( name.Compare(_T("RecurseFolders")) == 0 )
		{
			m_bRecurseFolders = true;
		}
		if ( name.Compare(_T("Exclude")) == 0 )
		{
			if ( !(value.IsEmpty()) )
				m_sExclude.Append(value);
			m_bExclude = (m_sExclude.Count() > 0);
		}
		if ( name.Compare(_T("ZipArchive")) == 0 )
		{
			m_bZipArchive = !(value.IsEmpty());
			m_sZipArchive = value;
		}
		if ( name.Compare(_T("FileSpec")) == 0 )
		{
			if ( !(value.IsEmpty()) )
				m_sZipFileSpec.Append(value);
			m_bZipFileSpec = (m_sZipFileSpec.Count() > 0);
		}
		if (name.Compare(_T("InputDir")) == 0)
		{
			m_bInputDir = !(value.IsEmpty());
			m_sInputDir = value;
		}
		if (name.Compare(_T("OutputFile")) == 0)
		{
			m_bOutputFile = !(value.IsEmpty());
			m_sOutputFile = value;
		}
	}

	virtual int main()
	{
		CStreamFile* pOutFile = NULL;
		CFilePath fcurdir;
		CFilePath fziparchive(__FILE__LINE__ m_sZipArchive);
		CFilePath foutf(__FILE__LINE__ m_sOutputFile);
		int result = 0;

		if ( m_help )
		{
			CStringBuffer tmp;

			usage(tmp);
			CERR << tmp;
			return 0;
		}
		if ( !m_AnyZipOption )
		{
			m_AnyZipOption = true;
			m_bAddFiles = true;
		}
		if (m_bInputDir)
		{
			CFilePath finputdir(__FILE__LINE__ m_sInputDir);

			if (CWinDirectoryIterator::FileExists(finputdir))
				finputdir.set_Filename(NULL);
			finputdir.MakeDirectory();
			if (CWinDirectoryIterator::DirectoryExists(finputdir) < 0)
			{
				CERR << finputdir.get_Path() << _T(" does not exist.") << endl;
				return -4;
			}
			CDirectoryIterator::GetCurrentDirectory(fcurdir);
			fziparchive.MakeAbsolute();
			if (m_bOutputFile)
				foutf.MakeAbsolute();
			CDirectoryIterator::SetCurrentDirectory(finputdir);
		}
		if (m_bOutputFile) 
		{
			if (CDirectoryIterator::FileExists(foutf))
				CDirectoryIterator::RemoveFile(foutf);
			else if (foutf.is_File())
			{
				CFilePath tmp(foutf);

				tmp.set_Filename(NULL);
				CDirectoryIterator::MakeDirectory(tmp);
			}
			else
			{
				CDateTime now;
				CStringBuffer tmp;

				now.Now();
				tmp.FormatString(__FILE__LINE__ _T("XZip%04hd%02hd%02hd%02hd%02hd%02hd.log"),
					now.GetYears(), now.GetMonths(), now.GetDays(),
					now.GetHours(), now.GetMinutes(), now.GetSeconds());
				foutf.set_Filename(tmp);
			}
			pOutFile = OK_NEW_OPERATOR CStreamFile;
			pOutFile->ReOpen(foutf, stdout);
		}
		if ( m_bAddFiles )
		{
			if (!m_bZipFileSpec)
			{
				CERR << _T("No FileSpec given. Do not know, what to do.") << endl;
				result = -4;
			}
			else
				XZipAddFiles(fziparchive, m_bRecurseFolders, m_sZipFileSpec, m_sExclude);
		}
		if ( m_bViewFiles )
		{
			if ( !m_bZipFileSpec )
				m_sZipFileSpec.Append(_T("*.*"));
			XZipViewFiles(fziparchive, m_sZipFileSpec, m_sExclude, m_sViewFiles);
		}
		if ( m_bFreshenFiles )
		{
			if ( !m_bZipFileSpec )
				m_sZipFileSpec.Append(_T("*.*"));
			XZipFreshenFiles(fziparchive, m_sZipFileSpec, m_sExclude);
		}
		if ( m_bUpdateFiles )
		{
			if ( !m_bZipFileSpec )
			{
				CERR << _T("No FileSpec given. Do not know, what to do.") << endl;
				result = -4;
			}
			else
				XZipUpdateFiles(fziparchive, m_bRecurseFolders, m_sZipFileSpec, m_sExclude);
		}
		if (pOutFile)
		{
			pOutFile->Close();
			pOutFile->release();
		}
		if (m_bInputDir)
			CDirectoryIterator::SetCurrentDirectory(fcurdir);
		return result;
	}

protected:
	WBool m_AnyZipOption;
	WBool m_help;
	WBool m_bAddFiles;
	WBool m_bFreshenFiles;
	WBool m_bUpdateFiles;
	WBool m_bViewFiles;
	CStringLiteral m_sViewFiles;
	WBool m_bRecurseFolders;
	WBool m_bExclude;
	TMBCharList m_sExclude;
	WBool m_bZipArchive;
	CStringLiteral m_sZipArchive;
	WBool m_bZipFileSpec;
	TMBCharList m_sZipFileSpec;
	WBool m_bInputDir;
	CStringLiteral m_sInputDir;
	WBool m_bOutputFile;
	CStringLiteral m_sOutputFile;
};

int 
#ifdef OK_COMP_MSC
    _tmain( int argc, CPointer *argv )
#else
	main( int argc, char** argv )
#endif
{
	int result;
	XZipApplication app;

	result = app.run(argc, argv);
	CSecurityContext_FreeInstance
#ifdef OK_COMP_MSC
#ifdef __DEBUG__
	_getch();
#endif
#endif
	return result;
}

