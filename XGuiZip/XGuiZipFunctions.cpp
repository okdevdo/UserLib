/******************************************************************************
    
	This file is part of XGuiZip, which is part of UserLib.

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
#include "GuiApplication.h"
#include "TreeView.h"
#include "ZipFilter.h"
#include "ZipArchive.h"
#include "DirectoryIterator.h"
#include "okDateTime.h"
#include "SecurityFile.h"
#include "SecurityContext.h"
#include "WinDirectoryIterator.h"

void XGuiZipViewFiles(CConstPointer zipfile, CTreeView* pTreeView)
{
	CSecurityFile* pArchiveFile = NULL;
	CArchiveIterator *zipIt = NULL;

	pTreeView->BeginUpdate();
	try
	{
		CFilePath fzipfile(__FILE__LINE__ zipfile);

		pArchiveFile = OK_NEW_OPERATOR CSecurityFile(fzipfile);

		CZipArchive zipArchive(pArchiveFile);

		zipIt = zipArchive.begin();

		while ( zipIt->Next() )
		{
			switch ( zipIt->GetType() )
			{
			case CArchiveIterator::ARCHIVE_FILE_REGULAR:
				{
					CStringBuffer tmp = zipIt->GetFileName();
					CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());
					CStringBuffer dir = fpath.get_Directory();
					CTreeViewNode* pNode = NULL;
					int jx;
					bool before;

					if ( !(dir.IsEmpty()) )
					{
						CPointer dirs[64];
						dword max_dirs;
						CTreeViewNode* pNode1;

						dir.Split(CDirectoryIterator::UnixPathSeparatorString(), dirs, 64, &max_dirs);
						for ( dword ix = 0; ix < max_dirs; ++ix )
						{
							if ( StrEmpty(dirs[ix]) )
								continue;
							if ( pNode )
							{
								pNode1 = pNode->get_Node(dirs[ix]);
								if ( !pNode1 )
								{
									pNode->find_Node(true, dirs[ix], true, &jx, &before);
									pNode1 = OK_NEW_OPERATOR CTreeViewNode(pNode, dirs[ix], -1, jx, before);
								}
							}
							else
							{
								pNode1 = pTreeView->get_Node(dirs[ix]);
								if ( !pNode1 )
								{
									pTreeView->find_Node(true, dirs[ix], true, &jx, &before);
									pNode1 = OK_NEW_OPERATOR CTreeViewNode(pTreeView, dirs[ix], -1, jx, before);
								}
							}
							pNode = pNode1;
						}
					}

					CStringBuffer name = fpath.get_Filename();

					sqword crc32;
					sqword uncompressedsize;
					sqword lastmodfiletime;
					bool isNull;

					zipIt->GetProperty(_T("CRC32"), crc32, isNull);
					zipIt->GetProperty(_T("UNCOMPRESSEDSIZE"), uncompressedsize, isNull);
					zipIt->GetProperty(_T("DOSFILETIME"), lastmodfiletime, isNull);

					CDateTime plastmodfiletime(Cast(time_t, lastmodfiletime));
					CStringBuffer tmp2;

					tmp2.FormatString(__FILE__LINE__ _T("%s, CRC32=%lx, UNCOMPRESSEDSIZE=%ld, DOSFILETIME=%hd.%hd.%hd"), name.GetString(), crc32, uncompressedsize, 
						plastmodfiletime.GetDays(), plastmodfiletime.GetMonths(), plastmodfiletime.GetYears());

					if ( pNode )
					{
						pNode->find_Node(true, name.GetString(), false, &jx, &before);
						OK_NEW_OPERATOR CTreeViewNode(pNode, tmp2, jx, before);
					}
					else
					{
						pTreeView->find_Node(true, name.GetString(), false, &jx, &before);
						OK_NEW_OPERATOR CTreeViewNode(pTreeView, tmp2, jx, before);
					}

					zipIt->Skip();
				}
				break;
			default:
				break;
			}
		}
		zipIt->release();
		pArchiveFile->Close();
		pArchiveFile->release();
	}
	catch ( CBaseException* ex )
	{
		if ( zipIt )
			zipIt->release();
		if ( pArchiveFile )
		{
			pArchiveFile->Close();
			pArchiveFile->release();
		}
		::MessageBox(theGuiApp->get_MainWnd()->get_handle(), ex->GetExceptionMessage().GetString(), _T("Anzeigefehler"), MB_OK);
	}
	pTreeView->EndUpdate(TRUE);
	CSecurityContext_FreeInstance
}

static void _XZipAddFilesRecurse(Ref(CZipArchive) zipArchive, ConstRef(CFilePath) dir, ConstRef(CStringBuffer) pattern)
{
	CDirectoryIterator it;
	CFilePath fpath(pattern);
	CSystemTime ftime;
	CArchiveProperties fprops;

	if ( fpath.is_Absolute() )
		return;
	fpath = dir;
	fpath.set_Filename(pattern);
	it.Open(fpath);
	while ( it )
	{
		CFilePath fpath1;

		if ( !(it.is_SubDir()) )
		{
			fpath1 = fpath;
			fpath1.set_Filename(it.get_Name());
			fprops.ClearProperties();
			CWinDirectoryIterator::ReadFileTimes(fpath1, ftime, ftime, ftime);
			fprops.SetProperty(_T("FILETIME"), CastAny(LongPointer, &ftime));
			fprops.SetProperty(_T("FILESIZE"), Cast(LongPointer, it.get_FileSize()));
			zipArchive.AddFile(fpath1, fprops);
		}
		++it;
	}
	fpath.set_Filename(_T("*"));
	it.Open(fpath);
	while ( it )
	{
		CFilePath fpath1;

		if ( it.is_SubDir() )
		{
			fpath1 = fpath;
			fpath1.set_Filename(_T(""));
			fpath1.append_Directory(it.get_Name());
			_XZipAddFilesRecurse(zipArchive, fpath1, pattern);
		}
		++it;
	}
}

void _XZipAddFiles(Ref(CZipArchive) zipArchive, WBool recursefolders, ConstRef(TMBCharList) filespecs)
{
	CDirectoryIterator it;
	TMBCharList::Iterator itP;
	CFilePath fpath;
	CSystemTime ftime;
	CArchiveProperties fprops;

	itP = filespecs.Begin();
	while ( itP )
	{
		it.Open(CFilePath(__FILE__LINE__ *itP));
		fpath.set_Path(__FILE__LINE__ *itP);
		while ( it )
		{
			if ( !(it.is_SubDir()) )
			{
				fpath.set_Filename(it.get_Name());
				fprops.ClearProperties();
				CWinDirectoryIterator::ReadFileTimes(fpath, ftime, ftime, ftime);
				fprops.SetProperty(_T("FILETIME"), CastAny(LongPointer, &ftime));
				fprops.SetProperty(_T("FILESIZE"), Cast(LongPointer, it.get_FileSize()));
				zipArchive.AddFile(fpath, fprops);
			}
			++it;
		}

		if ( recursefolders )
		{
			it.Open(CFilePath(__FILE__LINE__ _T("*")));
			while ( it )
			{
				if ( it.is_SubDir() )
				{
					fpath.set_Path(__FILE__LINE__ _T(""));
					fpath.set_Directory(it.get_Name());
					_XZipAddFilesRecurse(zipArchive, fpath, CStringBuffer(__FILE__LINE__ *itP));
				}
				++it;
			}
		}
		++itP;
	}
}

void XZipAddFiles(CStringLiteral zipfile, WBool recursefolders, ConstRef(TMBCharList) filespecs)
{
	CFilePath fzipfile(__FILE__LINE__ zipfile);
	Ptr(CSecurityFile) pFile = NULL;
	bool bExist = false;

	try
	{
		if ( CWinDirectoryIterator::FileExists(fzipfile) )
		{
			pFile = OK_NEW_OPERATOR CSecurityFile(fzipfile, false);
			bExist = true;
		}
		else
		{
			pFile = OK_NEW_OPERATOR CSecurityFile();
			pFile->Create(fzipfile);
		}

		CZipArchive zipArchive(pFile);

		if ( bExist )
			zipArchive.AddOpen();
		_XZipAddFiles(zipArchive, recursefolders, filespecs);
		zipArchive.AddClose();

		pFile->Close();
		pFile->release();
	}
	catch ( CBaseException* ex )
	{
		if ( pFile )
		{
			pFile->Close();
			pFile->release();
		}
		::MessageBox(theGuiApp->get_MainWnd()->get_handle(), ex->GetExceptionMessage().GetString(), _T("Exception"), MB_OK);
	}
	CSecurityContext_FreeInstance
}

static bool _TestFileSpec(ConstRef(CFilePath) file, ConstRef(CFilePath) filespec)
{
	CStringBuffer tmp(file.get_Directory());
	CStringBuffer tmp1(filespec.get_Directory());

	if ( (!(tmp.IsEmpty())) && (!(tmp1.IsEmpty())) && (tmp.Compare(tmp1, 0, CStringLiteral::cIgnoreCase) != 0) )
		return false;
	tmp = file.get_Filename();
	tmp1 = filespec.get_Filename();

	if ( s_strxcmp(tmp.GetString(), tmp1.GetString()) )
		return true;
	return false;
}

static bool _TestFileSpecs(ConstRef(CStringBuffer) afile, ConstRef(TMBCharList) filespecs)
{
	CStringBuffer tmp(afile);

	tmp.ReplaceString(CDirectoryIterator::UnixPathSeparatorString(), CDirectoryIterator::WinPathSeparatorString());
	CFilePath fpath(tmp);
	TMBCharList::Iterator it = filespecs.Begin();

	while ( it )
	{
		CFilePath fpath1(__FILE__LINE__ *it);

		if ( _TestFileSpec(fpath, fpath1) )
			return true;
		++it;
	}
	return false;
}

void XGuiZipExtractFiles(CStringLiteral zipfile, WBool usefolders, ConstRef(TMBCharList) filespecs, Ref(CDataVectorT<CStringBuffer>) messages)
{
	CFilePath fzipfile(__FILE__LINE__ zipfile);

	if ( !(CWinDirectoryIterator::FileExists(fzipfile)) )
	{
		return;
	}

	CSecurityFile* pArchiveFile = OK_NEW_OPERATOR CSecurityFile(fzipfile);

	CZipArchive zipArchive(pArchiveFile);
	CArchiveIterator *zipIt = zipArchive.begin();

	while ( zipIt->Next() )
	{
		switch ( zipIt->GetType() )
		{
		case CArchiveIterator::ARCHIVE_FILE_REGULAR:
			{
				CStringBuffer tmp = zipIt->GetFileName();

				if ( _TestFileSpecs(tmp, filespecs) )
				{
					CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());

					if ( usefolders )
					{
						CFilePath curDir;

						CDirectoryIterator::GetCurrentDirectory(curDir);
						fpath.Normalize(curDir.GetString());

						CStringBuffer tmp1;

						tmp1.AppendString(fpath.get_Root());
						tmp1.AppendString(fpath.get_Directory());
						CDirectoryIterator::MakeDirectory(tmp1);
					}
					else
					{
						fpath.set_Root(_T(""));
						fpath.set_Directory(_T(""));
					}

					sqword crc32;
					sqword uncompressedsize;
					sqword lastmodfiletime;
					bool isNull;

					zipIt->GetProperty(_T("CRC32"), crc32, isNull);
					zipIt->GetProperty(_T("UNCOMPRESSEDSIZE"), uncompressedsize, isNull);
					zipIt->GetProperty(_T("DOSFILETIME"), lastmodfiletime, isNull);

					CDateTime plastmodfiletime(Cast(time_t, lastmodfiletime));

					CArchiveFile* afile = zipIt->GetFile();
					CFileFilterInput* pInput = OK_NEW_OPERATOR CFileFilterInput(afile);
					CFileFilterOutput* pOutput = OK_NEW_OPERATOR CFileFilterOutput(fpath);
					CZipDeCompressFilter* pFilter = OK_NEW_OPERATOR CZipDeCompressFilter(pInput, pOutput);

					pFilter->open();
					pFilter->do_filter();
					pFilter->close();

					if ( crc32 != pFilter->get_crc32() )
					{
						CStringBuffer tmp1;

						tmp1.FormatString(__FILE__LINE__ _T("%s, crc mismatch"), tmp.GetString());
						messages.Append(tmp1);
					}
					if ( uncompressedsize != pFilter->get_unCompressedSize() )
					{
						CStringBuffer tmp1;

						tmp1.FormatString(__FILE__LINE__ _T("%s, uncompressedsize mismatch"), tmp.GetString());
						messages.Append(tmp1);
					}

					CWinDirectoryIterator::WriteFileTimes(fpath, plastmodfiletime, plastmodfiletime, plastmodfiletime);

					pFilter->release();
					pInput->release();
					pOutput->release();
					afile->release();
				}
				else
					zipIt->Skip();
			}
			break;
		default:
			break;
		}
	}
	zipIt->release();
	pArchiveFile->Close();
	pArchiveFile->release();
	CSecurityContext_FreeInstance
}
