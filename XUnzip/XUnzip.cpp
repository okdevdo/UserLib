/******************************************************************************
    
	This file is part of XUnzip, which is part of UserLib.

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
// XUnzip.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
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

class XUnzipApplication: public CApplication
{
public:
	XUnzipApplication():
		CApplication(CStringBuffer(__FILE__LINE__ _T("XUnzip"))),
		m_AnyZipOption(false),
	    m_help(false),
		m_bExtractFiles(false),
		m_bFreshenFiles(false),
		m_bNewerFiles(false),
		m_bViewFiles(false),
		m_sViewFiles(),
		m_bExclude(false),
		m_sExclude(__FILE__LINE__ 16, 16),
		m_bUseFolders(false),
		m_bOverwrite(false),
		m_bZipArchive(false),
		m_sZipArchive(),
		m_bUnzipFileSpecs(false),
		m_sUnzipFileSpecs(__FILE__LINE__ 16, 16),
		m_bOutputDir(false),
		m_sOutputDir(),
		m_bOutputFile(false),
		m_sOutputFile()
	{
        COptionCallback<XUnzipApplication> cb(this,&XUnzipApplication::handleHelp);
                
		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("ExtractFiles"))
			.shortName(_T("e"))
			.group(_T("UnzipOption"))
			.category(_T("Unzip"))
			.description(_T("Extracts files from zip archive.")));
		addOption(COption(_T("FreshenFiles"))
			.shortName(_T("f"))
			.group(_T("UnzipOption"))
			.category(_T("Unzip"))
			.description(_T("Freshen files from zip archive.")));
		addOption(COption(_T("NewerFiles"))
			.shortName(_T("n"))
			.group(_T("UnzipOption"))
			.category(_T("Unzip"))
			.description(_T("Updates files from zip archive.")));
		addOption(COption(_T("ViewFiles"))
			.shortName(_T("v"))
			.group(_T("UnzipOption"))
			.argument(_T("mode"), false)
			.category(_T("Unzip"))
			.description(_T("View zip file files. Argument can be 'verbose', 'full' or 'delta'.")));
		addOption(COption(_T("UseFolders"))
			.shortName(_T("r"))
			.category(_T("Unzip"))
			.description(_T("Use Folders from zip archive.")));
		addOption(COption(_T("Exclude"))
			.shortName(_T("x"))
			.argument(_T("path"))
			.repeatable(true)
			.category(_T("Unzip"))
			.description(_T("Exclude files and directories from archiving. Wildcards allowed.")));
		addOption(COption(_T("Overwrite"))
			.shortName(_T("w"))
			.category(_T("Unzip"))
			.description(_T("Overwrite disk files if necessary.")));
		addOption(COption(_T("ZipArchive"))
			.shortName(_T("z"))
			.required(true)
			.argument(_T("path"), true, 1)
			.category(_T("Unzip"))
			.description(_T("Specifies the zip archive file.")));
		addOption(COption(_T("FileSpec"))
			.shortName(_T("s"))
			.argument(_T("spec"))
			.repeatable(true)
			.category(_T("Unzip"))
			.description(_T("Specifies the files to be unzipped.")));
		addOption(COption(_T("OutputDir"))
			.shortName(_T("d"))
			.argument(_T("path"))
			.category(_T("Unzip"))
			.description(_T("To this directory files will be written, to be added, updated or freshened from the archive (default: <current directory>).")));
		addOption(COption(_T("OutputFile"))
			.shortName(_T("o"))
			.argument(_T("path"))
			.category(_T("Unzip"))
			.description(_T("In this file stdout will be redirected.")));
	}
	virtual ~XUnzipApplication()
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
		if ( name.Compare(_T("ExtractFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bExtractFiles = true;
		}
		if ( name.Compare(_T("FreshenFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bFreshenFiles = true;
		}
		if ( name.Compare(_T("NewerFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bNewerFiles = true;
		}
		if ( name.Compare(_T("ViewFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bViewFiles = true;
			m_sViewFiles = value;
		}
		if (name.Compare(_T("Exclude")) == 0)
		{
			if (!(value.IsEmpty()))
				m_sExclude.Append(value);
			m_bExclude = (m_sExclude.Count() > 0);
		}
		if (name.Compare(_T("UseFolders")) == 0)
		{
			m_bUseFolders = true;
		}
		if ( name.Compare(_T("Overwrite")) == 0 )
		{
			m_bOverwrite = true;
		}
		if ( name.Compare(_T("ZipArchive")) == 0 )
		{
			m_bZipArchive = !(value.IsEmpty());
			m_sZipArchive = value;
		}
		if ( name.Compare(_T("FileSpec")) == 0 )
		{
			if ( !(value.IsEmpty()) )
				m_sUnzipFileSpecs.Append(value);
			m_bUnzipFileSpecs = (m_sUnzipFileSpecs.Count() > 0);
		}
		if (name.Compare(_T("OutputDir")) == 0)
		{
			m_bOutputDir = !(value.IsEmpty());
			m_sOutputDir = value;
		}
		if (name.Compare(_T("OutputFile")) == 0)
		{
			m_bOutputFile = !(value.IsEmpty());
			m_sOutputFile = value;
		}
	}

	virtual int main()
	{
		CCppObjectPtr<CStreamFile> pOutFile;
		CFilePath fcurdir;
		CFilePath fziparchive(__FILE__LINE__ m_sZipArchive);
		CFilePath foutf(__FILE__LINE__ m_sOutputFile);
		int result = 0;

		if (m_help)
		{
			CStringBuffer tmp;

			usage(tmp);
			CERR << tmp;
			return 0;
		}
		if ( !m_AnyZipOption )
		{
			m_AnyZipOption = true;
			m_bExtractFiles = true;
		}
		if (m_bOutputDir)
		{
			CFilePath finputdir(__FILE__LINE__ m_sOutputDir);

			if (CWinDirectoryIterator::FileExists(finputdir))
				finputdir.set_Filename(nullptr);
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

				tmp.set_Filename(nullptr);
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
		if (!(CWinDirectoryIterator::FileExists(fziparchive)))
		{
			CERR << fziparchive.get_Path() << _T(" does not exist.") << endl;
			result = -4;
		}
		else
		{
			if (m_bViewFiles)
			{
				if (!m_bUnzipFileSpecs)
				{
					m_sUnzipFileSpecs.Append(_T("*.*"));
					m_bUnzipFileSpecs = true;
				}
				XUnzipViewFiles(fziparchive, m_sUnzipFileSpecs, m_sExclude, m_sViewFiles);
			}
			if (!m_bUnzipFileSpecs)
			{
				CERR << _T("No FileSpec given. Do not know, what to do.") << endl;
				m_bExtractFiles = false;
				m_bFreshenFiles = false;
				m_bNewerFiles = false;
				result = -4;
			}
			if (m_bExtractFiles)
				XUnzipExtractFiles(fziparchive, m_bUseFolders, m_bOverwrite, m_sUnzipFileSpecs, m_sExclude);
			if (m_bFreshenFiles)
				XUnzipFreshenFiles(fziparchive, m_bUseFolders, m_sUnzipFileSpecs, m_sExclude);
			if (m_bNewerFiles)
				XUnzipNewerFiles(fziparchive, m_bUseFolders, m_sUnzipFileSpecs, m_sExclude);
		}
		if (pOutFile)
			pOutFile->Close();
		if (m_bOutputDir)
			CDirectoryIterator::SetCurrentDirectory(fcurdir);
		return result;
	}

protected:
	WBool m_AnyZipOption;
	WBool m_help;
	WBool m_bExtractFiles;
	WBool m_bFreshenFiles;
	WBool m_bNewerFiles;
	WBool m_bViewFiles;
	CStringLiteral m_sViewFiles;
	WBool m_bExclude;
	TMBCharList m_sExclude;
	WBool m_bUseFolders;
	WBool m_bOverwrite;
	WBool m_bZipArchive;
	CStringLiteral m_sZipArchive;
	WBool m_bUnzipFileSpecs;
	TMBCharList m_sUnzipFileSpecs;
	WBool m_bOutputDir;
	CStringLiteral m_sOutputDir;
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
	XUnzipApplication app;

	result = app.run(argc, argv);
	CSecurityContext_FreeInstance
#ifdef OK_COMP_MSC
#ifdef __DEBUG__
	_getch();
#endif
#endif
	return result;
}

