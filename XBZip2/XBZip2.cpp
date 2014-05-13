/******************************************************************************
    
	This file is part of XBZip2, which is part of UserLib.

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
// XBZip2.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "Application.h"
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
#include <conio.h>
#endif
#endif

class XBZip2Application: public CApplication
{
public:
	XBZip2Application():
		CApplication(CStringBuffer(__FILE__LINE__ _T("XBZip2"))),
	    m_help(false),
		m_bArchive(false),
		m_sArchive(),
		m_bInputFile(false),
		m_sInputFile()
	{
        COptionCallback<XBZip2Application> cb(this,&XBZip2Application::handleHelp);
                
		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("CompressFile"))
			.shortName(_T("c"))
			.required(true)
			.argument(_T("path"), true, 1)
			.category(_T("XBZip2"))
			.description(_T("Specifies the file to be compressed.")));
		addOption(COption(_T("Archive"))
			.shortName(_T("a"))
			.argument(_T("path"), true, 2)
			.category(_T("XBZip2"))
			.description(_T("Specifies the archive file.")));
	}
	~XBZip2Application()
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
		if ( name.Compare(_T("CompressFile")) == 0 )
		{
			m_bInputFile = !(value.IsEmpty());
			m_sInputFile = value;
		}
		if ( name.Compare(_T("Archive")) == 0 )
		{
			m_bArchive = !(value.IsEmpty());
			m_sArchive = value;
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
		XBZip2CompressFile(m_sInputFile, m_sArchive);

		return 0;
	}

protected:
	WBool m_help;
	WBool m_bArchive;
	CStringLiteral m_sArchive;
	WBool m_bInputFile;
	CStringLiteral m_sInputFile;
};

int 
#ifdef OK_COMP_MSC
    _tmain( int argc, CPointer *argv )
#else
	main( int argc, char** argv )
#endif
{
	int result;
	XBZip2Application app;

	result = app.run(argc, argv);
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
	_getch();
#endif
#endif
	return result;
}

