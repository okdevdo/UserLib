/******************************************************************************
    
	This file is part of XTar, which is part of UserLib.

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
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
#include <conio.h>
#endif
#endif

class XTarApplication: public CApplication
{
public:
	XTarApplication():
		CApplication(CStringBuffer(__FILE__LINE__ _T("XTar"))),
		m_AnyZipOption(false),
	    m_help(false),
		m_bExtractFiles(false),
		m_bViewFiles(false),
		m_bTarArchive(false),
		m_sTarArchive(),
		m_bOutputDir(false),
		m_sOutputDir(),
		m_bOutputFile(false),
		m_sOutputFile()
	{
        COptionCallback<XTarApplication> cb(this,&XTarApplication::handleHelp);
                
		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("OutputDir"))
			.shortName(_T("d"))
			.argument(_T("path"), true)
			.category(_T("Tar"))
			.description(_T("In this directory the archive files will be written with respect to archive directories (default: <current directory>).")));
		addOption(COption(_T("OutputFile"))
			.shortName(_T("o"))
			.argument(_T("path"), true)
			.category(_T("Tar"))
			.description(_T("In this file stdout will be redirected.")));
		addOption(COption(_T("ViewFiles"))
			.shortName(_T("v"))
			.category(_T("Tar"))
			.description(_T("Views files from tar archive.")));
		addOption(COption(_T("ExtractFiles"))
			.shortName(_T("x"))
			.category(_T("Tar"))
			.description(_T("Extracts files from tar archive.")));
		addOption(COption(_T("TarArchive"))
			.shortName(_T("t"))
			.required(true)
			.argument(_T("name"), true, 1)
			.category(_T("Tar"))
			.description(_T("Specifies the tar archive file. It may be compressed. Compressor is detected by file extension.")));
	}
	~XTarApplication()
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
		if ( name.Compare(_T("ViewFiles")) == 0 )
		{
			m_AnyZipOption = true;
			m_bViewFiles = true;
		}
		if (name.Compare(_T("TarArchive")) == 0)
		{
			m_bTarArchive = !(value.IsEmpty());
			m_sTarArchive = value;
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
			m_bViewFiles = true;
		}
		if ( m_bExtractFiles )
		{
			XTarExtractFiles(m_sTarArchive, m_sOutputDir, m_sOutputFile, m_bViewFiles);
			return 0;
		}
		if ( m_bViewFiles )
		{
			XTarViewFiles(m_sTarArchive, m_sOutputDir, m_sOutputFile);
			return 0;
		}

		return 0;
	}

protected:
	WBool m_AnyZipOption;
	WBool m_help;
	WBool m_bExtractFiles;
	WBool m_bViewFiles;
	WBool m_bTarArchive;
	CStringLiteral m_sTarArchive;
	WBool m_bOutputDir;
	CStringLiteral m_sOutputDir;
	WBool m_bOutputFile;
	CStringLiteral m_sOutputFile;
};

int 
#if OK_COMP_MSC
    _tmain( int argc, CPointer *argv )
#else
	main( int argc, char** argv )
#endif
{
	int result;
	XTarApplication app;

	result = app.run(argc, argv);
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
	_getch();
#endif
#endif
	return result;
}

