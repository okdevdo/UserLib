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
#include "stdafx.h"
#include "TarArchive.h"
#include "BZip2Filter.h"
#include "GZipFilter.h"
#include "LZMAFilter.h"
#include "XZFilter.h"
#include "DirectoryIterator.h"
#include "WinDirectoryIterator.h"
#include "okDateTime.h"
#include "SecurityFile.h"
#include "SecurityContext.h"
#include <stdio.h>

static void XTarDoFiles(CStringLiteral tarfile, CStringLiteral outputdir, CStringLiteral outputfile, bool bExtract, bool bViewInfo)
{
	CFilePath ftarfile(__FILE__LINE__ tarfile);
	CFilePath fexfile;
	CFilePath fcurDir(__FILE__LINE__ outputdir);
	CFilePath foutf(__FILE__LINE__ outputfile);
	CCppObjectPtr<CSecurityFile> pArchiveFile;
	CCppObjectPtr<CStreamFile> pOutFile;
	dword nFiles = 0;
	dword nDirs = 0;
	dword nLinks = 0;
	dword nUnknown = 0;
	bool useCurDir = true;

	if (!(CWinDirectoryIterator::FileExists(ftarfile)))
	{
		CERR << tarfile << _T(" does not exist.") << endl;
		return;
	}
	if (ftarfile.get_Extension().Compare(CStringLiteral(_T("bz2")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		CCppObjectPtr<CSecurityFile> parchivefile;
		CCppObjectPtr<CSecurityFile> pExFile;

		fexfile = ftarfile;
		fexfile.set_Extension(_T(""));

		try
		{
			if (CWinDirectoryIterator::FileExists(fexfile))
				CWinDirectoryIterator::UnlinkFile(fexfile);

			parchivefile = OK_NEW_OPERATOR CSecurityFile(ftarfile);
			pExFile = OK_NEW_OPERATOR CSecurityFile();

			pExFile->Create(fexfile);

			CCppObjectPtr<CFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(parchivefile);
			CCppObjectPtr<CFilterOutput> pOutput = OK_NEW_OPERATOR CFileFilterOutput(pExFile);
			CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CBZip2DeCompressFilter(pInput, pOutput);

			pFilter->open();
			pFilter->do_filter();
			pFilter->close();
		}
		catch (CBaseException* ex)
		{
			if (parchivefile)
				parchivefile->Close();
			if (pExFile)
				pExFile->Close();
			CERR << ex->GetExceptionMessage() << endl;
			return;
		}
		ftarfile = fexfile;
	}
	else if (ftarfile.get_Extension().Compare(CStringLiteral(_T("gz")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		CCppObjectPtr<CSecurityFile> parchivefile;
		CCppObjectPtr<CSecurityFile> pExFile;

		fexfile = ftarfile;
		fexfile.set_Extension(_T(""));

		try
		{
			if (CWinDirectoryIterator::FileExists(fexfile))
				CWinDirectoryIterator::UnlinkFile(fexfile);

			parchivefile = OK_NEW_OPERATOR CSecurityFile(ftarfile);
			pExFile = OK_NEW_OPERATOR CSecurityFile();

			pExFile->Create(fexfile);

			CCppObjectPtr<CFileFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(parchivefile);
			CCppObjectPtr<CFileFilterOutput> pOutput = OK_NEW_OPERATOR CFileFilterOutput(pExFile);
			CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CGZipDeCompressFilter(pInput, pOutput);

			pFilter->open();
			pFilter->do_filter();
			pFilter->close();
		}
		catch (CBaseException* ex)
		{
			if (parchivefile)
				parchivefile->Close();
			if (pExFile)
				pExFile->Close();
			CERR << ex->GetExceptionMessage() << endl;
			return;
		}
		ftarfile = fexfile;
	}
	else if (ftarfile.get_Extension().Compare(CStringLiteral(_T("lzma")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		CCppObjectPtr<CSecurityFile> parchivefile;
		CCppObjectPtr<CSecurityFile> poutputfile;

		fexfile = ftarfile;
		fexfile.set_Extension(_T(""));

		try
		{
			if (CWinDirectoryIterator::FileExists(fexfile))
				CWinDirectoryIterator::UnlinkFile(fexfile);

			parchivefile = OK_NEW_OPERATOR CSecurityFile(ftarfile);
			poutputfile = OK_NEW_OPERATOR CSecurityFile();

			poutputfile->Create(fexfile);

			CCppObjectPtr<CFileFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(parchivefile);
			CCppObjectPtr<CFileFilterOutput> pOutput = OK_NEW_OPERATOR CFileFilterOutput(poutputfile);
			CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CLZMADeCompressFilter(pInput, pOutput);

			pFilter->open();
			pFilter->do_filter();
			pFilter->close();
		}
		catch (CBaseException* ex)
		{
			if (parchivefile)
				parchivefile->Close();
			if (poutputfile)
				poutputfile->Close();
			COUT << ex->GetExceptionMessage() << endl;
			return;
		}
		ftarfile = fexfile;
	}
	else if (ftarfile.get_Extension().Compare(CStringLiteral(_T("xz")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		CCppObjectPtr<CSecurityFile> parchivefile;
		CCppObjectPtr<CSecurityFile> poutputfile;

		fexfile = ftarfile;
		fexfile.set_Extension(nullptr);

		try
		{
			if (CWinDirectoryIterator::FileExists(fexfile))
				CWinDirectoryIterator::UnlinkFile(fexfile);

			parchivefile = OK_NEW_OPERATOR CSecurityFile(ftarfile);
			poutputfile = OK_NEW_OPERATOR CSecurityFile();

			poutputfile->Create(fexfile);

			CCppObjectPtr<CFileFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(parchivefile);
			CCppObjectPtr<CFileFilterOutput> pOutput = OK_NEW_OPERATOR CFileFilterOutput(poutputfile);
			CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CXZDeCompressFilter(pInput, pOutput);

			pFilter->open();
			pFilter->do_filter();
			pFilter->close();
		}
		catch (CBaseException* ex)
		{
			if (parchivefile)
				parchivefile->Close();
			if (poutputfile)
				poutputfile->Close();
			CERR << ex->GetExceptionMessage() << endl;
			return;
		}
		ftarfile = fexfile;
	}
	if (ftarfile.get_Extension().Compare(CStringLiteral(_T("tar")), 0, CStringLiteral::cIgnoreCase) != 0)
	{
		CERR << ftarfile.get_Path() << _T(" does not to be a valid archive file.") << endl;
		return;
	}
	if (!(foutf.IsEmpty()))
	{
		if (CWinDirectoryIterator::FileExists(foutf))
			CWinDirectoryIterator::UnlinkFile(foutf);
		else if (foutf.is_File())
		{
			CFilePath tmp(foutf);

			tmp.set_Filename(nullptr);
			CWinDirectoryIterator::MakeDirectory(tmp);
		}
		else
		{
			CDateTime now;
			CStringBuffer tmp;

			now.Now();
			tmp.FormatString(__FILE__LINE__ _T("XTar%04ld%02ld%02ld%02ld%02ld%02ld.log"),
				Castdword(now.GetYears()), Castdword(now.GetMonths()), Castdword(now.GetDays()),
				Castdword(now.GetHours()), Castdword(now.GetMinutes()), Castdword(now.GetSeconds()));
			foutf.set_Filename(tmp);
		}
		pOutFile = OK_NEW_OPERATOR CStreamFile;
		pOutFile->ReOpen(foutf, stdout);
	}
	if (!(fcurDir.IsEmpty()))
	{
		if (CWinDirectoryIterator::FileExists(fcurDir))
			fcurDir.set_Filename(nullptr);
		fcurDir.MakeDirectory();
		if (CWinDirectoryIterator::DirectoryExists(fcurDir) >= 0)
			useCurDir = false;
	}
	if (useCurDir)
		CDirectoryIterator::GetCurrentDirectory(fcurDir);

	pArchiveFile = OK_NEW_OPERATOR CSecurityFile(ftarfile);

	CTarArchive tarArchive(pArchiveFile);
	CCppObjectPtr<CArchiveIterator> tarIt = tarArchive.begin();

	while (tarIt->Next())
	{
		switch (tarIt->GetType())
		{
		case CArchiveIterator::ARCHIVE_FILE_REGULAR:
			{
				CStringBuffer tmp = tarIt->GetFileName();
				CStringBuffer tmp1;
				CStringBuffer tmp2;
				CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());

				try
				{
					fpath.Normalize(fcurDir);

					tmp1.AppendString(fpath.get_Root());
					tmp1.AppendString(fpath.get_Directory());

					if (bExtract && (CWinDirectoryIterator::DirectoryExists(tmp1) < 0))
						CWinDirectoryIterator::MakeDirectory(tmp1);
				}
				catch (CBaseException* ex)
				{
					CERR << ex->GetExceptionMessage() << endl;
					break;
				}
				try
				{
					sqword vfilemode;
					sqword vfiletime;
					bool isNull;

					tarIt->GetProperty(_T("FILEMODE"), vfilemode, isNull);
					tarIt->GetProperty(_T("FILETIME"), vfiletime, isNull);
#ifdef OK_SYS_WINDOWS
					CDateTime ftime(Cast(time_t, vfiletime));
#endif
#ifdef OK_SYS_UNIX
					CDateTime ftime(Cast(time_t, vfiletime), CDateTime::LocalTime);
#endif
					if (bExtract)
					{
						CCppObjectPtr<CArchiveFile> afile = tarIt->GetFile();
						CCppObjectPtr<CFileFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(afile);
						CCppObjectPtr<CSecurityFile> ofile = OK_NEW_OPERATOR CSecurityFile;

						ofile->Create(fpath, false, CFile::BinaryFile_NoEncoding, Cast(mode_t, vfilemode));

						CCppObjectPtr<CFileFilterOutput> pOutput = OK_NEW_OPERATOR CFileFilterOutput(ofile);
						CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CCopyFilter(pInput, pOutput);

						pFilter->open();
						pFilter->do_filter();
						pFilter->close();

						CWinDirectoryIterator::WriteFileTimes(fpath, ftime, ftime, ftime);
					}
					else
						tarIt->Skip();

					if (bViewInfo)
					{
						COUT << _T("ARCHIVE_FILE_REGULAR: ") << tmp << endl;
						if (bExtract)
							COUT << _T("  was written to ") << tmp1 << endl;
						else
							COUT << _T("  will be written to ") << tmp1 << endl;
						tmp2.FormatString(__FILE__LINE__ _T("  FILEMODE=%lo, FILETIME=%02ld.%02ld.%04ld %02ld:%02ld:%02ld"), Castdword(vfilemode),
							Castdword(ftime.GetDays()), Castdword(ftime.GetMonths()), Castdword(ftime.GetYears()),
							Castdword(ftime.GetHours()), Castdword(ftime.GetMinutes()), Castdword(ftime.GetSeconds()));
						COUT << tmp2 << endl;
					}
					nFiles++;
				}
				catch (CBaseException* ex)
				{
					CERR << ex->GetExceptionMessage() << endl;
					tarIt->Skip();
				}
			}
			break;
		case CArchiveIterator::ARCHIVE_FILE_DIRECTORY:
			{
				CStringBuffer tmp = tarIt->GetFileName();
				CStringBuffer tmp2;
				CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());

				fpath.Normalize(fcurDir);

				try
				{
					sqword vfiletime;
					sqword vfilemode;
					bool isNull;

					tarIt->GetProperty(_T("FILETIME"), vfiletime, isNull);
					tarIt->GetProperty(_T("FILEMODE"), vfilemode, isNull);

#ifdef OK_SYS_WINDOWS
					CDateTime ftime(Cast(time_t, vfiletime));
#endif
#ifdef OK_SYS_UNIX
					CDateTime ftime(Cast(time_t, vfiletime), CDateTime::LocalTime);
#endif

					if (bExtract)
					{
						if (CWinDirectoryIterator::DirectoryExists(fpath) < 0)
							CWinDirectoryIterator::MakeDirectory(fpath, Cast(mode_t, vfilemode));
						CWinDirectoryIterator::WriteFileTimes(fpath, ftime, ftime, ftime);
					}
					if (bViewInfo)
					{
						COUT << _T("ARCHIVE_FILE_DIRECTORY: ") << tmp << endl;
						tmp2.FormatString(__FILE__LINE__ _T("  FILEMODE=%lo, FILETIME=%02ld.%02ld.%04ld %02ld:%02ld:%02ld"), Castdword(vfilemode),
							Castdword(ftime.GetDays()), Castdword(ftime.GetMonths()), Castdword(ftime.GetYears()),
							Castdword(ftime.GetHours()), Castdword(ftime.GetMinutes()), Castdword(ftime.GetSeconds()));
						COUT << tmp2 << endl;
					}
					nDirs++;
				}
				catch (CBaseException* ex)
				{
					CERR << ex->GetExceptionMessage() << endl;
				}
			}
			break;
		case CArchiveIterator::ARCHIVE_FILE_HARDLINK:
		case CArchiveIterator::ARCHIVE_FILE_SYMLINK:
			{
				CStringBuffer tmp = tarIt->GetFileName(); // has to be created
				CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());
				CStringBuffer tmp1 = tarIt->GetLinkName(); // file that exists, may be relative to tmp
				CFilePath fpath1(tmp1, CDirectoryIterator::UnixPathSeparatorString());
				CStringBuffer tmp2;

				fpath.Normalize(fcurDir);
				tmp2 = fpath.get_Root();
				tmp2.AppendString(fpath.get_Directory());
				fpath1.Normalize(tmp2);

				try
				{
					if (bExtract)
					{
						switch (tarIt->GetType())
						{
						case CArchiveIterator::ARCHIVE_FILE_HARDLINK:
							CWinDirectoryIterator::LinkFile(fpath1, fpath);
							break;
						case CArchiveIterator::ARCHIVE_FILE_SYMLINK:
							CWinDirectoryIterator::SymLinkFile(fpath1, fpath);
							break;
						default:
							break;
						}
					}
					if (bViewInfo)
					{
						switch (tarIt->GetType())
						{
						case CArchiveIterator::ARCHIVE_FILE_HARDLINK:
							COUT << _T("ARCHIVE_FILE_HARDLINK: From ") << fpath1.get_Path() << _T(" To ") << fpath.get_Path() << endl;
							break;
						case CArchiveIterator::ARCHIVE_FILE_SYMLINK:
							COUT << _T("ARCHIVE_FILE_SYMLINK: From ") << fpath1.get_Path() << _T(" To ") << fpath.get_Path() << endl;
							break;
						default:
							break;
						}
					}
					nLinks++;
				}
				catch (CBaseException* ex)
				{
					CERR << ex->GetExceptionMessage() << endl;
				}
			}
			break;
		default:
			CERR << _T("Unknown type") << endl;
			nUnknown++;
			break;
		}
	}
	pArchiveFile->Close();
	if (pOutFile)
		pOutFile->Close();

	CERR << _T("Summary:") << endl;
	CERR << _T("  #Files=") << nFiles << endl;
	CERR << _T("  #Directories=") << nDirs << endl;
	CERR << _T("  #Links=") << nLinks << endl;
	CERR << _T("  #Unknown=") << nUnknown << endl;
	CSecurityContext_FreeInstance
}

void XTarViewFiles(CStringLiteral tarfile, CStringLiteral outputdir, CStringLiteral outputfile)
{
	XTarDoFiles(tarfile, outputdir, outputfile, false, true);
}

void XTarExtractFiles(CStringLiteral tarfile, CStringLiteral outputdir, CStringLiteral outputfile, bool bViewFiles)
{
	XTarDoFiles(tarfile, outputdir, outputfile, true, bViewFiles);
}

