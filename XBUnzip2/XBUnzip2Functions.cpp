/******************************************************************************
    
	This file is part of XBUnzip2, which is part of UserLib.

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

void XBUnzip2ExtractFile(CStringLiteral archivefile, CStringLiteral extractFile)
{
	CFilePath farchivefile(__FILE__LINE__ archivefile);
	CFilePath fexfile;
	CSecurityFile* parchivefile = NULL;
	CSecurityFile* pExFile = NULL;

	if (farchivefile.get_Extension().Compare(CStringLiteral(_T("bz2")), 0, CStringLiteral::cIgnoreCase) != 0)
	{
		CERR << farchivefile.get_Path() << _T(" does not to be a valid archive file.") << endl;
		return;
	}
	if (extractFile.IsEmpty())
	{
		fexfile.set_Path(__FILE__LINE__ archivefile);
		fexfile.set_Extension(NULL);
	}
	else
		fexfile.set_Path(__FILE__LINE__ extractFile);
	try
	{
		if ( !(CWinDirectoryIterator::FileExists(farchivefile)) )
		{
			CERR << farchivefile.get_Path() << _T(" does not exist.") << endl;
			return;
		}

		if ( CWinDirectoryIterator::FileExists(fexfile) )
		{
			CERR << fexfile.get_Path() << _T(" exists.") << endl;
			return;
		}

		parchivefile = OK_NEW_OPERATOR CSecurityFile(farchivefile);
		pExFile = OK_NEW_OPERATOR CSecurityFile();	

		pExFile->Create(fexfile);
		
		CFilterInput* pInput = OK_NEW_OPERATOR CFileFilterInput(parchivefile);
		CFilterOutput* pOutput = OK_NEW_OPERATOR CFileFilterOutput(pExFile);
		CFilter* pFilter = OK_NEW_OPERATOR CBZip2DeCompressFilter(pInput, pOutput);

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();

		pFilter->release();
		pOutput->release();
		pInput->release();
		parchivefile->release();
		pExFile->release();
	}
	catch ( CBaseException* ex )
	{
		if ( parchivefile )
		{
			parchivefile->Close();
			parchivefile->release();
		}
		if ( pExFile )
		{
			pExFile->Close();
			pExFile->release();
		}
		CERR << ex->GetExceptionMessage() << endl;
	}
	CSecurityContext_FreeInstance
}

