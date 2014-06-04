/******************************************************************************
    
	This file is part of XLUnzip, which is part of UserLib.

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
#include "LZMAFilter.h"
#include "FilePath.h"
#include "SecurityFile.h"
#include "SecurityContext.h"
#include "WinDirectoryIterator.h"

void XLUnzipExtractFile(CStringLiteral archivefile, CStringLiteral extractFile)
{
	CFilePath farchivefile(__FILE__LINE__ archivefile);
	CFilePath foutputfile;
	CCppObjectPtr<CSecurityFile> parchivefile;
	CCppObjectPtr<CSecurityFile> poutputfile;

	if (farchivefile.get_Extension().Compare(CStringLiteral(_T("lzma")), 0, CStringLiteral::cIgnoreCase) != 0)
	{
		CERR << farchivefile.get_Path() << _T(" does not to be a valid archive file.") << endl;
		return;
	}
	if (extractFile.IsEmpty())
	{
		foutputfile.set_Path(__FILE__LINE__ archivefile);
		foutputfile.set_Extension(nullptr);
	}
	else
		foutputfile.set_Path(__FILE__LINE__ extractFile);
	try
	{
		if ( !(CWinDirectoryIterator::FileExists(farchivefile)) )
		{
			COUT << farchivefile.get_Path() << _T(" does not exist.") << endl;
			return;
		}

		if ( CWinDirectoryIterator::FileExists(foutputfile) )
		{
			COUT << foutputfile.get_Path() << _T(" exists.") << endl;
			return;
		}

		parchivefile = OK_NEW_OPERATOR CSecurityFile(farchivefile);
		poutputfile = OK_NEW_OPERATOR CSecurityFile();	

		poutputfile->Create(foutputfile);
		
		CCppObjectPtr<CFileFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(parchivefile);
		CCppObjectPtr<CFileFilterOutput> pOutput = OK_NEW_OPERATOR CFileFilterOutput(poutputfile);
		CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CLZMADeCompressFilter(pInput, pOutput);

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();
	}
	catch ( CBaseException* ex )
	{
		if ( parchivefile )
			parchivefile->Close();
		if ( poutputfile )
			poutputfile->Close();
		COUT << ex->GetExceptionMessage() << endl;
	}
	CSecurityContext_FreeInstance
}

