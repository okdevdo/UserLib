/******************************************************************************
    
	This file is part of XGZip, which is part of UserLib.

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
#include "stdafx.h"
#include "GZipFilter.h"
#include "FilePath.h"
#include "SecurityFile.h"
#include "SecurityContext.h"
#include "WinDirectoryIterator.h"

void XGZipCompressFile(CStringLiteral compressfile, CStringLiteral archivefile)
{
	CFilePath finputfile(__FILE__LINE__ compressfile);
	CCppObjectPtr<CSecurityFile> pinputfile;
	CFilePath farchivefile;
	CCppObjectPtr<CSecurityFile> parchivefile;

	if ( archivefile.IsEmpty() )
	{
		CStringBuffer tmp(__FILE__LINE__ compressfile);

		tmp += _T(".gz");
		farchivefile.set_Path(tmp);
	}
	else
		farchivefile.set_Path(__FILE__LINE__ archivefile);
	try
	{
		if ( !(CWinDirectoryIterator::FileExists(finputfile)) )
		{
			CERR << finputfile.get_Path() << _T(" does not exist.") << endl;
			return;
		}

		if ( CWinDirectoryIterator::FileExists(farchivefile) )
		{
			CERR << farchivefile.get_Path() << _T(" exists.") << endl;
			return;
		}

		pinputfile = OK_NEW_OPERATOR CSecurityFile(finputfile);
		parchivefile = OK_NEW_OPERATOR CSecurityFile();	

		parchivefile->Create(farchivefile);
		
		CCppObjectPtr<CFileFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(pinputfile);
		CCppObjectPtr<CFileFilterOutput> pOutput = OK_NEW_OPERATOR CFileFilterOutput(parchivefile);
		CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CGZipCompressFilter(pInput, pOutput);

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();
	}
	catch ( CBaseException* ex )
	{
		if ( pinputfile )
			pinputfile->Close();
		if ( parchivefile )
			parchivefile->Close();
		CERR << ex->GetExceptionMessage() << endl;
	}
	CSecurityContext_FreeInstance
}

