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
#include "stdafx.h"
#include "BZip2Filter.h"
#include "FilePath.h"
#include "SecurityFile.h"
#include "SecurityContext.h"
#include "WinDirectoryIterator.h"

void XBZip2CompressFile(CStringLiteral compressFile, CStringLiteral archiveFile)
{
	CFilePath fcompressfile(__FILE__LINE__ compressFile);
	CSecurityFile* pcompressfile = NULL;
	CFilePath farchivefile;
	CSecurityFile* parchivefile = NULL;

	if ( archiveFile.IsEmpty() )
	{
		CStringBuffer tmp(__FILE__LINE__ compressFile);

		tmp += _T(".bz2");
		farchivefile.set_Path(tmp);
	}
	else
		farchivefile.set_Path(__FILE__LINE__ archiveFile);
	try
	{
		if ( !(CWinDirectoryIterator::FileExists(fcompressfile)) )
		{
			CERR << fcompressfile.get_Path() << _T(" does not exist.") << endl;
			return;
		}

		if ( CWinDirectoryIterator::FileExists(farchivefile) )
		{
			CERR << farchivefile.get_Path() << _T(" exists.") << endl;
			return;
		}

		pcompressfile = OK_NEW_OPERATOR CSecurityFile(fcompressfile);
		parchivefile = OK_NEW_OPERATOR CSecurityFile();	

		parchivefile->Create(farchivefile);
		
		CFileFilterInput* pInput = OK_NEW_OPERATOR CFileFilterInput(pcompressfile);
		CFileFilterOutput* pOutput = OK_NEW_OPERATOR CFileFilterOutput(parchivefile);
		CBZip2CompressFilter* pFilter = OK_NEW_OPERATOR CBZip2CompressFilter(pInput, pOutput);

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();

		pFilter->release();
		pOutput->release();
		pInput->release();
		parchivefile->release();
		pcompressfile->release();
	}
	catch ( CBaseException* ex )
	{
		if ( pcompressfile )
		{
			pcompressfile->Close();
			pcompressfile->release();
		}
		if ( parchivefile )
		{
			parchivefile->Close();
			parchivefile->release();
		}
		CERR << ex->GetExceptionMessage() << endl;
	}
	CSecurityContext_FreeInstance
}

