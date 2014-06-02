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
#include "stdafx.h"
#include "ZipFilter.h"
#include "ZipArchive.h"
#include "Application.h"
#include "Configuration.h"
#include "DirectoryIterator.h"
#include "okDateTime.h"
#include "SystemTime.h"
#include "SecurityFile.h"
#include "WinDirectoryIterator.h"

static bool _TestFileSpec(ConstRef(CFilePath) file, ConstRef(CFilePath) filespec)
{
	CStringBuffer tmp(file.get_Directory());
	CStringBuffer tmp1(filespec.get_Directory());

	if ((!(tmp.IsEmpty())) && (!(tmp1.IsEmpty())))
	{
		CStringConstIterator it(tmp);

		it.Find(tmp1, CStringConstIterator::cIgnoreCase);
		if (it.IsEnd())
			return false;
	}
	tmp = file.get_Filename();
	tmp1 = filespec.get_Filename();

	if (s_strxcmp(tmp.GetString(), tmp1.GetString()))
		return true;
	return false;
}

static bool _TestFileSpecs(ConstRef(CStringBuffer) afile, ConstRef(TMBCharList) filespecs)
{
	CStringBuffer tmp(afile);

	tmp.ReplaceString(CDirectoryIterator::UnixPathSeparatorString(), CDirectoryIterator::WinPathSeparatorString());

	CFilePath fpath(tmp);
	TMBCharList::Iterator it = filespecs.Begin();

	while (it)
	{
		CFilePath fpath1(__FILE__LINE__ *it);

		if (_TestFileSpec(fpath, fpath1))
			return true;
		++it;
	}
	return false;
}

static bool _TestExcludeSpec(ConstRef(CFilePath) file, ConstRef(CFilePath) filespec)
{
	CStringBuffer tmp(file.get_Directory());
	CStringBuffer tmp1(filespec.get_Directory());
	bool bPreCondition = false;

	if ((!(tmp.IsEmpty())) && (!(tmp1.IsEmpty())))
	{
		CStringConstIterator it(tmp);

		it.Find(tmp1, CStringConstIterator::cIgnoreCase);
		if (!(it.IsEnd()))
			bPreCondition = true;
	}
	else
		bPreCondition = true;
	if (!bPreCondition)
		return false;
	tmp = file.get_Filename();
	tmp1 = filespec.get_Filename();

	if (tmp.IsEmpty() && tmp1.IsEmpty())
		return true;
	if (tmp.IsEmpty() && ((tmp1.Compare(_T("*")) == 0) || (tmp1.Compare(_T("*.*")) == 0)))
		return true;
	if (tmp1.IsEmpty())
		tmp1.SetString(__FILE__LINE__ _T("*"));
	if (s_strxcmp(tmp.GetString(), tmp1.GetString()))
		return true;
	return false;
}

static bool _TestExcludeSpecs(ConstRef(CStringBuffer) afile, ConstRef(TMBCharList) filespecs)
{
	CStringBuffer tmp(afile);

	tmp.ReplaceString(CDirectoryIterator::UnixPathSeparatorString(), CDirectoryIterator::WinPathSeparatorString());

	CFilePath fpath(tmp);
	TMBCharList::Iterator it = filespecs.Begin();

	while (it)
	{
		CFilePath fpath1(__FILE__LINE__ *it);

		if (_TestExcludeSpec(fpath, fpath1))
			return true;
		++it;
	}
	return false;
}

void XZipViewFiles(ConstRef(CFilePath) fzipfile, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs, CStringLiteral mode)
{
	CSecurityFile* pArchiveFile = NULL;
	CArchiveIterator *zipIt = NULL;
	CFilePath fcurDir;
	WBool modeVerbose = false;
	WBool modeFull = true;
	WBool modeDelta = false;

	try
	{
		if (!(CWinDirectoryIterator::FileExists(fzipfile)))
		{
			CERR << fzipfile.get_Path() << _T(" does not exist.") << endl;
			return;
		}
		CDirectoryIterator::GetCurrentDirectory(fcurDir);

		if (mode.Compare(CStringLiteral(_T("verbose")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			modeVerbose = true;
			modeFull = false;
			modeDelta = false;
		}
		if (mode.Compare(CStringLiteral(_T("full")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			modeVerbose = false;
			modeFull = true;
			modeDelta = false;
		}
		if (mode.Compare(CStringLiteral(_T("delta")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			modeVerbose = false;
			modeFull = false;
			modeDelta = true;
		}

		pArchiveFile = OK_NEW_OPERATOR CSecurityFile(fzipfile, true, false, CFile::BinaryFile_NoEncoding);

		CZipArchive zipArchive(pArchiveFile);
		
		zipIt = zipArchive.begin();
		while ( zipIt->Next() )
		{
			switch ( zipIt->GetType() )
			{
			case CArchiveIterator::ARCHIVE_FILE_REGULAR:
				{
					CStringBuffer tmp = zipIt->GetFileName();
					CStringBuffer tmp2;
					CStringBuffer tmp3;
					CStringBuffer tmp4;

					if ( _TestFileSpecs(tmp, filespecs) && (!_TestExcludeSpecs(tmp, excludespecs)) )
					{
						CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());

						fpath.Normalize(fcurDir);

						if (modeFull || modeVerbose || modeDelta)
						{
							tmp3.FormatString(__FILE__LINE__ _T("ARCHIVE_FILE_REGULAR: %s\n"), tmp.GetString());
							tmp2.AppendString(tmp3);
							tmp3.FormatString(__FILE__LINE__ _T("  will be compared against file %s\n"), fpath.GetString());
							tmp2.AppendString(tmp3);
						}

						sqword crc32;
						sqword uncompressedsize;
						sqword lastmodfiletime;
						sqword vCreationTime;
						sqword vLastAccessTime;
						sqword vLastWriteTime;
						bool isCreationTimeNull;
						bool isLastAccessNull;
						bool isLastWriteNull;
						bool isNull;
						bool bDelta;

						zipIt->GetProperty(_T("CRC32"), crc32, isNull);
						zipIt->GetProperty(_T("UNCOMPRESSEDSIZE"), uncompressedsize, isNull);

						if (modeVerbose)
						{
							tmp3.FormatString(__FILE__LINE__ _T("  CRC32=%lx, UNCOMPRESSEDSIZE=%ld"), Castdword(crc32), Castdword(uncompressedsize));
							tmp2.AppendString(tmp3);

							CDirectoryIterator it(fpath);

							if (it)
							{
								tmp3.FormatString(__FILE__LINE__ _T(", FILESIZE=%lld"), it.get_FileSize());
								tmp2.AppendString(tmp3);
							}
							tmp2.AppendString(_T("\n"));
						}

						zipIt->GetProperty(_T("DOSFILETIME"), lastmodfiletime, isNull);
						zipIt->GetProperty(_T("NTFSCTIME"), vCreationTime, isCreationTimeNull);
						zipIt->GetProperty(_T("NTFSATIME"), vLastAccessTime, isLastAccessNull);
						zipIt->GetProperty(_T("NTFSMTIME"), vLastWriteTime, isLastWriteNull);
						
						bDelta = false;
						if (isLastWriteNull)
						{
							CDateTime vArchiveTime((time_t)lastmodfiletime);

							if (modeFull || modeVerbose || modeDelta)
							{
								vArchiveTime.GetTimeString(tmp3);
								tmp4.FormatString(__FILE__LINE__ _T("  ARCHIVETIME=%s\n"), tmp3.GetString());
								tmp2.AppendString(tmp4);
							}

							CDirectoryIterator it(fpath);

							if (it)
							{
								CDateTime vFileTime = it.get_LastWriteTime();

								if (vFileTime > vArchiveTime)
								{
									vFileTime.GetTimeString(tmp3);
									bDelta = true;
								}
								else if (vFileTime == vArchiveTime)
									tmp3.SetString(__FILE__LINE__ _T("<file has the same last write time>"));
								else
									tmp3.SetString(__FILE__LINE__ _T("<file is older than archive entry"));
							}
							else
								tmp3.SetString(__FILE__LINE__ _T("<file does not exist in file system>"));
						}
						else
						{
							FILETIME fArchiveLastWriteTime = DerefAnyPtr(FILETIME, &vLastWriteTime);
							CSystemTime cArchiveLastWriteTime(fArchiveLastWriteTime);

							cArchiveLastWriteTime.GetTimeString(tmp3);
							tmp4.FormatString(__FILE__LINE__ _T("  ARCHIVETIME=%s\n"), tmp3.GetString());
							tmp2.AppendString(tmp4);
							
							if (CWinDirectoryIterator::FileExists(fpath))
							{
								FILETIME fCreationTime;
								FILETIME fLastAccessTime;
								FILETIME fLastWriteTime;

								CWinDirectoryIterator::_ReadFileTimes(fpath, &fCreationTime, &fLastAccessTime, &fLastWriteTime);

								CSystemTime cFileLastWriteTime(fLastWriteTime);

								if (cFileLastWriteTime > cArchiveLastWriteTime)
								{
									cFileLastWriteTime.GetTimeString(tmp3);
									bDelta = true;
								}
								else if (cFileLastWriteTime == cArchiveLastWriteTime)
									tmp3.SetString(__FILE__LINE__ _T("<file has the same last write time>"));
								else
									tmp3.SetString(__FILE__LINE__ _T("<file is older than archive entry"));
							}
							else
								tmp3.SetString(__FILE__LINE__ _T("<file does not exist in file system>"));
						}
						if (modeFull || modeVerbose || (modeDelta && bDelta))
						{
							tmp4.FormatString(__FILE__LINE__ _T("  FILETIME=%s\n"), tmp3.GetString());
							tmp2.AppendString(tmp4);
							COUT << tmp2;
						}
					}
					zipIt->Skip();
				}
				break;
			default:
				CERR << _T("Unknown type") << endl;
				break;
			}
		}
		zipIt->release();
		zipIt = NULL;
		pArchiveFile->Close();
		pArchiveFile->release();
		pArchiveFile = NULL;
	}
	catch ( CBaseException* ex )
	{
		CERR << ex->GetExceptionMessage() << endl;
		if (zipIt)
			zipIt->release();
		if (pArchiveFile)
		{
			pArchiveFile->Close();
			pArchiveFile->release();
		}
	}
}

static void _XZipAddFilesRecurse(Ref(CZipArchive) zipArchive, ConstRef(CFilePath) dir, ConstRef(CStringBuffer) pattern, ConstRef(TMBCharList) excludespecs, bool bIsFSNTFS)
{
	CDirectoryIterator it;
	CFilePath fpath;
	FILETIME cftime;
	FILETIME aftime;
	FILETIME mftime;
	CArchiveProperties fprops;

	fpath = dir;
	fpath.set_Filename(pattern);
	it.Open(fpath);
	while ( it )
	{
		if ( !(it.is_SubDir()) )
		{
			CFilePath fpath1(fpath);

			fpath1.set_Filename(it.get_Name());
			if (!_TestExcludeSpecs(fpath1.get_Path(), excludespecs))
			{
				CDateTime mtime(it.get_LastWriteTime());
				time_t t0;

				mtime.GetTime(t0);
				fprops.ClearProperties();
				fprops.SetProperty(_T("FILESIZE"), Cast(sqword, it.get_FileSize()));
				fprops.SetProperty(_T("FILETIME"), Cast(sqword, t0));
				if (bIsFSNTFS)
				{
					CWinDirectoryIterator::_ReadFileTimes(fpath1, &cftime, &aftime, &mftime);
					fprops.SetProperty(_T("NTFSCRTIME"), DerefAnyPtr(sqword, &cftime));
					fprops.SetProperty(_T("NTFSLATIME"), DerefAnyPtr(sqword, &aftime));
					fprops.SetProperty(_T("NTFSLWTIME"), DerefAnyPtr(sqword, &mftime));
				}
				zipArchive.AddFile(fpath1, fprops);
				COUT << _T("File '") << fpath1.get_Path() << _T("' added.") << endl;
			}
		}
		++it;
	}
	fpath.set_Filename(_T("*"));
	it.Open(fpath);
	while ( it )
	{
		if ( it.is_SubDir() )
		{
			CFilePath fpath1(fpath);

			fpath1.set_Filename(it.get_Name());
			fpath1.MakeDirectory();
			if (!_TestExcludeSpecs(fpath1.get_Path(), excludespecs))
				_XZipAddFilesRecurse(zipArchive, fpath1, pattern, excludespecs, bIsFSNTFS);
		}
		++it;
	}
}

void _XZipAddFiles(Ref(CZipArchive) zipArchive, WBool recursefolders, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs)
{
	CDirectoryIterator it;
	TMBCharList::Iterator itP;
	CFilePath fpath;
	FILETIME cftime;
	FILETIME aftime;
	FILETIME mftime;
	CArchiveProperties fprops;
	bool bIsFSNTFS;

	itP = filespecs.Begin();
	while ( itP )
	{
		fpath.set_Path(__FILE__LINE__ *itP);
		bIsFSNTFS = CWinDirectoryIterator::FileSystemIsNTFS(fpath);
		it.Open(fpath);
		while ( it )
		{
			if ( !(it.is_SubDir()) )
			{
				fpath.set_Filename(it.get_Name());

				if (!_TestExcludeSpecs(fpath.get_Path(), excludespecs))
				{
					CDateTime mtime(it.get_LastWriteTime());
					time_t t0;

					mtime.GetTime(t0);
					fprops.ClearProperties();
					fprops.SetProperty(_T("FILESIZE"), Cast(sqword, it.get_FileSize()));
					fprops.SetProperty(_T("FILETIME"), Cast(sqword, t0));
					if (bIsFSNTFS)
					{
						CWinDirectoryIterator::_ReadFileTimes(fpath, &cftime, &aftime, &mftime);
						fprops.SetProperty(_T("NTFSCRTIME"), DerefAnyPtr(sqword, &cftime));
						fprops.SetProperty(_T("NTFSLATIME"), DerefAnyPtr(sqword, &aftime));
						fprops.SetProperty(_T("NTFSLWTIME"), DerefAnyPtr(sqword, &mftime));
					}
					zipArchive.AddFile(fpath, fprops);
					COUT << _T("File '") << fpath.get_Path() << _T("' added.") << endl;
				}
			}
			++it;
		}
		if ( recursefolders )
		{
			CStringBuffer tmp;

			fpath.set_Path(__FILE__LINE__ *itP);
			tmp = fpath.get_Filename();
			fpath.set_Filename(_T("*"));
			it.Open(fpath);
			while ( it )
			{
				if ( it.is_SubDir() )
				{
					CFilePath fpath1(fpath);

					fpath1.set_Filename(it.get_Name());
					//if (it.get_Name().Compare(CStringLiteral(_T("ConSources")), 0, CStringLiteral::cIgnoreCase) == 0)
					//	_CrtDbgBreak();
					fpath1.MakeDirectory();
					if (!_TestExcludeSpecs(fpath1.get_Path(), excludespecs))
						_XZipAddFilesRecurse(zipArchive, fpath1, tmp, excludespecs, bIsFSNTFS);
				}
				++it;
			}
		}
		++itP;
	}
}

void XZipAddFiles(ConstRef(CFilePath) fzipfile, WBool recursefolders, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs)
{
	Ptr(CSecurityFile) pFile = NULL;
	bool bExist = false;

	try
	{
		if (CWinDirectoryIterator::FileExists(fzipfile))
		{
			pFile = OK_NEW_OPERATOR CSecurityFile(fzipfile, false, false, CFile::BinaryFile_NoEncoding);
			bExist = true;
		}
		else
		{
			pFile = OK_NEW_OPERATOR CSecurityFile();
			pFile->Create(fzipfile, false, CFile::BinaryFile_NoEncoding);
		}

		CZipArchive zipArchive(pFile);

		if ( bExist )
			zipArchive.AddOpen();
		_XZipAddFiles(zipArchive, recursefolders, filespecs, excludespecs);
		zipArchive.AddClose();

		pFile->Close();
		pFile->release();
	}
	catch ( CBaseException* ex )
	{
		CERR << ex->GetExceptionMessage() << endl;
		if ( pFile )
		{
			pFile->Close();
			pFile->release();
		}
	}
}

class TGetFileItem: public CCppObject
{
public:
	CStringBuffer fPath;
	bool bfTime;
	CDateTime fTime;
	CSystemTime sTime;

	void SetFTime(ConstRef(CStringBuffer) _fPath, sqword lastmodfiletime)
	{
		fPath = _fPath;
		fPath.addRef();
		bfTime = true;
		fTime.SetTime(Cast(time_t, lastmodfiletime));
	}
	void SetSTime(ConstRef(CStringBuffer) _fPath, sqword lastmodfiletime)
	{
		fPath = _fPath;
		fPath.addRef();
		bfTime = false;
#ifdef OK_SYS_WINDOWS
		sTime.SetTime(DerefAnyPtr(FILETIME, &lastmodfiletime));
#endif
#ifdef OK_SYS_UNIX
		lastmodfiletime -= 116444736000000000;
		lastmodfiletime /= 10000000;
		sTime.SetTime(Cast(time_t, lastmodfiletime));
#endif
	}
	//void TimetToFileTime(time_t t, LPFILETIME pft)
	//{
	//	LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
	//	pft->dwLowDateTime = (DWORD)ll;
	//	pft->dwHighDateTime = ll >> 32;
	//}
};

class TGetFileItemLessFunctor
{
public:
	bool operator()(ConstPtr(TGetFileItem) pArrayItem, ConstPtr(TGetFileItem) pDataItem) const
	{
		return pArrayItem->fPath.LT(pDataItem->fPath, 0, CStringLiteral::cIgnoreCase);
	}
};

typedef CDataVectorT<TGetFileItem, TGetFileItemLessFunctor> TGetFileItems;

static void _XZipGetFileList(Ptr(CArchiveIterator) zipIt, ConstRef(TMBCharList) filespecs, Ref(TGetFileItems) list)
{
	while ( zipIt->Next() )
	{
		switch ( zipIt->GetType() )
		{
		case CArchiveIterator::ARCHIVE_FILE_REGULAR:
			{
				CStringBuffer tmp = zipIt->GetFileName();

				if ( _TestFileSpecs(tmp, filespecs) )
				{
					tmp.ReplaceString(CDirectoryIterator::UnixPathSeparatorString(), CDirectoryIterator::WinPathSeparatorString());

					sqword lastmodfiletime;
					bool isNull;
					TGetFileItem gfl;

					zipIt->GetProperty(_T("NTFSMTIME"), lastmodfiletime, isNull);
					if (!isNull)
					{
						gfl.SetSTime(tmp, lastmodfiletime);
						list.Append(&gfl);
					}
					else
					{
						zipIt->GetProperty(_T("DOSFILETIME"), lastmodfiletime, isNull);
						if (!isNull)
						{
							gfl.SetFTime(tmp, lastmodfiletime);
							list.Append(&gfl);
						}
					}
				}
				zipIt->Skip();
			}
			break;
		default:
			break;
		}
	}
	zipIt->release();
}

void XZipFreshenFiles(ConstRef(CFilePath) fzipfile, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs)
{
	Ptr(CSecurityFile) pFile = NULL;

	try
	{
		if (!(CWinDirectoryIterator::FileExists(fzipfile)))
		{
			CERR << fzipfile.get_Path() << _T(" does not exist.") << endl;
			return;
		}

		TGetFileItems fileList(__FILE__LINE__ 128, 128);
		TGetFileItems::Iterator itFileList;
		CFilePath fpath;
		CArchiveProperties fprops;
		CStringBuffer tmp;
		CStringBuffer tmpFileTime;
		CStringBuffer tmpArchiveTime;

		pFile = OK_NEW_OPERATOR CSecurityFile(fzipfile, false, false, CFile::BinaryFile_NoEncoding);

		CZipArchive zipArchive(pFile);

		zipArchive.AddOpen();
		_XZipGetFileList(zipArchive.begin(), filespecs, fileList);
		itFileList = fileList.Begin();
		while ( itFileList )
		{
			Ptr(TGetFileItem) pGfl = *itFileList;

			fpath.set_Path(pGfl->fPath);
			if (CWinDirectoryIterator::FileExists(fpath) && (!(_TestExcludeSpecs(fpath.get_Path(), excludespecs))))
			{
				bool bUpdate = false;

				if (pGfl->bfTime || (!(CWinDirectoryIterator::FileSystemIsNTFS(fpath))))
				{
					CDirectoryIterator it(fpath);

					if (it)
					{
						CDateTime vFileTime(it.get_LastWriteTime());

						if (!(pGfl->bfTime))
						{
							CSystemTime vFileTime1(vFileTime);

							if (vFileTime1 > pGfl->sTime)
							{
								bUpdate = true;
								vFileTime1.GetTimeString(tmpFileTime);
								pGfl->sTime.GetTimeString(tmpArchiveTime);
							}
						}
						else if (vFileTime > pGfl->fTime)
						{
							bUpdate = true;
							vFileTime.GetTimeString(tmpFileTime);
							pGfl->fTime.GetTimeString(tmpArchiveTime);
						}
					}
				}
				else
				{
					FILETIME fCreationTime;
					FILETIME fLastAccessTime;
					FILETIME fLastWriteTime;

					CWinDirectoryIterator::_ReadFileTimes(fpath, &fCreationTime, &fLastAccessTime, &fLastWriteTime);

					CSystemTime vFileTime(fLastWriteTime);

					if (vFileTime > pGfl->sTime)
					{
						bUpdate = true;
						vFileTime.GetTimeString(tmpFileTime);
						pGfl->sTime.GetTimeString(tmpArchiveTime);
					}
				}
				if (bUpdate)
				{
					fprops.ClearProperties();

					if (CWinDirectoryIterator::FileSystemIsNTFS(fpath))
					{
						FILETIME fCreationTime;
						FILETIME fLastAccessTime;
						FILETIME fLastWriteTime;

						CWinDirectoryIterator::_ReadFileTimes(fpath, &fCreationTime, &fLastAccessTime, &fLastWriteTime);

						fprops.SetProperty(_T("NTFSCRTIME"), DerefAnyPtr(sqword, &fCreationTime));
						fprops.SetProperty(_T("NTFSLATIME"), DerefAnyPtr(sqword, &fLastAccessTime));
						fprops.SetProperty(_T("NTFSLWTIME"), DerefAnyPtr(sqword, &fLastWriteTime));
					}

					CDirectoryIterator it(fpath);

					if (it)
					{
						CDateTime fTime(it.get_LastWriteTime());
						time_t t0;

						fTime.GetTime(t0);
						fprops.SetProperty(_T("FILETIME"), Castsqword(t0));
						fprops.SetProperty(_T("FILESIZE"), it.get_FileSize());
					}
					zipArchive.AddFile(fpath, fprops);

					tmp.FormatString(__FILE__LINE__ _T("File '%s' freshened\n  FILETIME=%s\n  ARCHIVE =%s\n"), fpath.get_Path().GetString(), tmpFileTime.GetString(), tmpArchiveTime.GetString());
					COUT << tmp;
				}
			}
			++itFileList;
		}
		zipArchive.AddClose();
		pFile->Close();
		pFile->release();
	}
	catch ( CBaseException* ex )
	{
		CERR << ex->GetExceptionMessage() << endl;
		if ( pFile )
		{
			pFile->Close();
			pFile->release();
		}
	}
}

static void _XZipUpdateAddFilesRecurse(Ref(CZipArchive) zipArchive, ConstRef(CFilePath) dir, ConstRef(CStringBuffer) pattern, ConstRef(TMBCharList) excludespecs, Ref(TGetFileItems) fileList, bool bIsFSNTFS)
{
	CDirectoryIterator it;
	TGetFileItems::Iterator itG;
	CFilePath fpath;
	FILETIME cftime;
	FILETIME aftime;
	FILETIME mftime;
	CArchiveProperties fprops;

	fpath = dir;
	fpath.set_Filename(pattern);
	it.Open(fpath);
	while (it)
	{
		if ( !(it.is_SubDir()) )
		{
			TGetFileItem item;
			CFilePath fpath1(fpath);

			fpath1.set_Filename(it.get_Name());
			item.fPath = fpath1.get_Path();
			itG = fileList.FindSorted(&item);
			if (!(fileList.MatchSorted(itG, &item)))
			{
				if (!_TestExcludeSpecs(fpath1.get_Path(), excludespecs))
				{
					CDateTime mtime(it.get_LastWriteTime());
					time_t t0;

					mtime.GetTime(t0);
					fprops.ClearProperties();
					fprops.SetProperty(_T("FILESIZE"), Cast(sqword, it.get_FileSize()));
					fprops.SetProperty(_T("FILETIME"), Cast(sqword, t0));
					if (bIsFSNTFS)
					{
						CWinDirectoryIterator::_ReadFileTimes(fpath1, &cftime, &aftime, &mftime);
						fprops.SetProperty(_T("NTFSCRTIME"), DerefAnyPtr(sqword, &cftime));
						fprops.SetProperty(_T("NTFSLATIME"), DerefAnyPtr(sqword, &aftime));
						fprops.SetProperty(_T("NTFSLWTIME"), DerefAnyPtr(sqword, &mftime));
					}
					zipArchive.AddFile(fpath1, fprops);
					COUT << _T("File '") << fpath1.get_Path() << _T("' added.") << endl;
				}
			}
		}
		++it;
	}
	fpath.set_Filename(_T("*"));
	it.Open(fpath);
	while ( it )
	{
		if ( it.is_SubDir() )
		{
			CFilePath fpath1(fpath);

			fpath1.set_Filename(it.get_Name());
			fpath1.MakeDirectory();
			if (!_TestExcludeSpecs(fpath1.get_Path(), excludespecs))
				_XZipUpdateAddFilesRecurse(zipArchive, fpath1, pattern, excludespecs, fileList, bIsFSNTFS);
		}
		++it;
	}
}

void _XZipUpdateAddFiles(Ref(CZipArchive) zipArchive, WBool recursefolders, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs, Ref(TGetFileItems) fileList)
{
	CDirectoryIterator it;
	TMBCharList::Iterator itP;
	TGetFileItems::Iterator itG;
	CFilePath fpath;
	FILETIME cftime;
	FILETIME aftime;
	FILETIME mftime;
	CArchiveProperties fprops;
	bool bIsFSNTFS;

	itP = filespecs.Begin();
	while ( itP )
	{
		fpath.set_Path(__FILE__LINE__ *itP);
		bIsFSNTFS = CWinDirectoryIterator::FileSystemIsNTFS(fpath);
		it.Open(fpath);
		while (it)
		{
			if ( !(it.is_SubDir()) )
			{
				TGetFileItem item;

				fpath.set_Filename(it.get_Name());
				item.fPath = fpath.get_Path();
				itG = fileList.FindSorted(&item);
				if (!(fileList.MatchSorted(itG, &item)))
				{
					if (!_TestExcludeSpecs(fpath.get_Path(), excludespecs))
					{
						CDateTime mtime(it.get_LastWriteTime());
						time_t t0;

						mtime.GetTime(t0);
						fprops.ClearProperties();
						fprops.SetProperty(_T("FILESIZE"), Cast(sqword, it.get_FileSize()));
						fprops.SetProperty(_T("FILETIME"), Cast(sqword, t0));
						if (bIsFSNTFS)
						{
							CWinDirectoryIterator::_ReadFileTimes(fpath, &cftime, &aftime, &mftime);
							fprops.SetProperty(_T("NTFSCRTIME"), DerefAnyPtr(sqword, &cftime));
							fprops.SetProperty(_T("NTFSLATIME"), DerefAnyPtr(sqword, &aftime));
							fprops.SetProperty(_T("NTFSLWTIME"), DerefAnyPtr(sqword, &mftime));
						}
						zipArchive.AddFile(fpath, fprops);
						COUT << _T("File '") << fpath.get_Path() << _T("' added.") << endl;
					}
				}
			}
			++it;
		}
		if ( recursefolders )
		{
			CStringBuffer tmp;

			fpath.set_Path(__FILE__LINE__ *itP);
			tmp = fpath.get_Filename();
			fpath.set_Filename(_T("*"));
			it.Open(fpath);
			while (it)
			{
				if ( it.is_SubDir() )
				{
					CFilePath fpath1(fpath);

					fpath1.set_Filename(it.get_Name());
					fpath1.MakeDirectory();
					if (!_TestExcludeSpecs(fpath1.get_Path(), excludespecs))
						_XZipUpdateAddFilesRecurse(zipArchive, fpath1, tmp, excludespecs, fileList, bIsFSNTFS);
				}
				++it;
			}
		}
		++itP;
	}
}

void XZipUpdateFiles(ConstRef(CFilePath) fzipfile, WBool recursefolders, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs)
{
	Ptr(CSecurityFile) pFile = NULL;

	try
	{
		if (!(CWinDirectoryIterator::FileExists(fzipfile)))
		{
			CERR << fzipfile.get_Path() << _T(" does not exist.") << endl;
			return;
		}

		TGetFileItems fileList(__FILE__LINE__ 128, 128);
		TGetFileItems::Iterator itFileList;
		CFilePath fpath;
		CArchiveProperties fprops;
		CStringBuffer tmp;
		CStringBuffer tmpFileTime;
		CStringBuffer tmpArchiveTime;

		pFile = OK_NEW_OPERATOR CSecurityFile(fzipfile, false, false, CFile::BinaryFile_NoEncoding);

		CZipArchive zipArchive(pFile);

		zipArchive.AddOpen();
		_XZipGetFileList(zipArchive.begin(), filespecs, fileList);
		itFileList = fileList.Begin();
		while ( itFileList )
		{
			Ptr(TGetFileItem) pGfl = *itFileList;

			fpath.set_Path(pGfl->fPath);
			if (!_TestExcludeSpecs(fpath.get_Path(), excludespecs))
			{
				if (CWinDirectoryIterator::FileExists(fpath))
				{
					bool bUpdate = false;

					if (pGfl->bfTime || (!(CWinDirectoryIterator::FileSystemIsNTFS(fpath))))
					{
						CDirectoryIterator it(fpath);

						if (it)
						{
							CDateTime vFileTime(it.get_LastWriteTime());

							if (!(pGfl->bfTime))
							{
								CSystemTime vFileTime1(vFileTime);

								if (vFileTime1 > pGfl->sTime)
								{
									bUpdate = true;
									vFileTime1.GetTimeString(tmpFileTime);
									pGfl->sTime.GetTimeString(tmpArchiveTime);
								}
							}
							else if (vFileTime > pGfl->fTime)
							{
								bUpdate = true;
								vFileTime.GetTimeString(tmpFileTime);
								pGfl->fTime.GetTimeString(tmpArchiveTime);
							}
						}
					}
					else
					{
						FILETIME fCreationTime;
						FILETIME fLastAccessTime;
						FILETIME fLastWriteTime;

						CWinDirectoryIterator::_ReadFileTimes(fpath, &fCreationTime, &fLastAccessTime, &fLastWriteTime);

						CSystemTime vFileTime(fLastWriteTime);

						if (vFileTime > pGfl->sTime)
						{
							bUpdate = true;
							vFileTime.GetTimeString(tmpFileTime);
							pGfl->sTime.GetTimeString(tmpArchiveTime);
						}
					}
					if (bUpdate)
					{
						fprops.ClearProperties();

						if (CWinDirectoryIterator::FileSystemIsNTFS(fpath))
						{
							FILETIME fCreationTime;
							FILETIME fLastAccessTime;
							FILETIME fLastWriteTime;

							CWinDirectoryIterator::_ReadFileTimes(fpath, &fCreationTime, &fLastAccessTime, &fLastWriteTime);

							fprops.SetProperty(_T("NTFSCRTIME"), DerefAnyPtr(sqword, &fCreationTime));
							fprops.SetProperty(_T("NTFSLATIME"), DerefAnyPtr(sqword, &fLastAccessTime));
							fprops.SetProperty(_T("NTFSLWTIME"), DerefAnyPtr(sqword, &fLastWriteTime));
						}

						CDirectoryIterator it(fpath);

						if (it)
						{
							CDateTime fTime(it.get_LastWriteTime());
							time_t t0;

							fTime.GetTime(t0);
							fprops.SetProperty(_T("FILETIME"), Castsqword(t0));
							fprops.SetProperty(_T("FILESIZE"), it.get_FileSize());
						}
						zipArchive.AddFile(fpath, fprops);

						tmp.FormatString(__FILE__LINE__ _T("File '%s' updated\n  FILETIME=%s\n  ARCHIVE =%s\n"), fpath.GetString(), tmpFileTime.GetString(), tmpArchiveTime.GetString());
						COUT << tmp;
					}
				}
				else
				{
					zipArchive.AddDelete(fpath);
					tmp.FormatString(__FILE__LINE__ _T("File '%s' does no longer exist, deleted from archive\n"), fpath.GetString());
					COUT << tmp;
				}
			}
			++itFileList;
		}

		fileList.Sort();
		_XZipUpdateAddFiles(zipArchive, recursefolders, filespecs, excludespecs, fileList);

		fileList.Close();

		zipArchive.AddClose();

		pFile->Close();
		pFile->release();
	}
	catch ( CBaseException* ex )
	{
		CERR << ex->GetExceptionMessage() << endl;
		if ( pFile )
		{
			pFile->Close();
			pFile->release();
		}
	}
}
