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
#include "stdafx.h"
#include "ZipFilter.h"
#include "ZipArchive.h"
#include "DirectoryIterator.h"
#include "okDateTime.h"
#include "SystemTime.h"
#include "SecurityFile.h"
#include "WinDirectoryIterator.h"

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

static bool _TestFileSpecs(ConstRef(CStringBuffer) afile, ConstRef(CDataVectorT<mbchar>) filespecs)
{
	CStringBuffer tmp(afile);

	tmp.ReplaceString(CDirectoryIterator::UnixPathSeparatorString(), CDirectoryIterator::WinPathSeparatorString());
	CFilePath fpath(tmp);
	CDataVectorT<mbchar>::Iterator it = filespecs.Begin();

	while ( it )
	{
		CFilePath fpath1(__FILE__LINE__ *it);

		if ( _TestFileSpec(fpath, fpath1) )
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

static bool _TestExcludeSpecs(ConstRef(CStringBuffer) afile, ConstRef(CDataVectorT<mbchar>) filespecs)
{
	CStringBuffer tmp(afile);

	tmp.ReplaceString(CDirectoryIterator::UnixPathSeparatorString(), CDirectoryIterator::WinPathSeparatorString());

	CFilePath fpath(tmp);
	CDataVectorT<mbchar>::Iterator it = filespecs.Begin();

	while (it)
	{
		CFilePath fpath1(__FILE__LINE__ *it);

		if (_TestExcludeSpec(fpath, fpath1))
			return true;
		++it;
	}
	return false;
}

void XUnzipViewFiles(ConstRef(CFilePath) fzipfile, ConstRef(CDataVectorT<mbchar>) filespecs, ConstRef(CDataVectorT<mbchar>) excludespecs, CStringLiteral mode)
{
	CSecurityFile* pArchiveFile = NULL;
	CArchiveIterator *zipIt = NULL;
	CFilePath fcurDir;
	WBool modeVerbose = false;
	WBool modeFull = true;
	WBool modeDelta = false;

	try
	{
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
		while (zipIt->Next())
		{
			switch (zipIt->GetType())
			{
			case CArchiveIterator::ARCHIVE_FILE_REGULAR:
				{
					CStringBuffer tmp = zipIt->GetFileName();
					CStringBuffer tmp2;
					CStringBuffer tmp3;
					CStringBuffer tmp4;

					if (_TestFileSpecs(tmp, filespecs) && (!_TestExcludeSpecs(tmp, excludespecs)))
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

								if (vArchiveTime > vFileTime)
								{
									vFileTime.GetTimeString(tmp3);
									bDelta = true;
								}
								else if (vFileTime == vArchiveTime)
									tmp3.SetString(__FILE__LINE__ _T("<file has the same last write time>"));
								else
									tmp3.SetString(__FILE__LINE__ _T("<file is younger than archive entry"));
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

								if (cArchiveLastWriteTime > cFileLastWriteTime)
								{
									cFileLastWriteTime.GetTimeString(tmp3);
									bDelta = true;
								}
								else if (cFileLastWriteTime == cArchiveLastWriteTime)
									tmp3.SetString(__FILE__LINE__ _T("<file has the same last write time>"));
								else
									tmp3.SetString(__FILE__LINE__ _T("<file is younger than archive entry"));
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
	catch (CBaseException* ex)
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

void XUnzipExtractFiles(ConstRef(CFilePath) fzipfile, WBool usefolders, WBool overwrite, ConstRef(CDataVectorT<mbchar>) filespecs, ConstRef(CDataVectorT<mbchar>) excludespecs)
{
	CFilePath fcurDir;
	CSecurityFile* pArchiveFile = NULL;
	CArchiveIterator *zipIt = NULL;

	try
	{
		CDirectoryIterator::GetCurrentDirectory(fcurDir);

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
					bool bSkip = true;

					if (_TestFileSpecs(tmp, filespecs) && (!(_TestExcludeSpecs(tmp, excludespecs))))
					{
						CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());
						bool bExist;
						bool bContinue;

						COUT << tmp << endl;
						if ( usefolders )
						{
							fpath.Normalize(fcurDir);

							CStringBuffer tmp1;

							tmp1.AppendString(fpath.get_Root());
							tmp1.AppendString(fpath.get_Directory());
							CWinDirectoryIterator::MakeDirectory(tmp1);
						}
						else
						{
							fpath.set_Root(_T(""));
							fpath.set_Directory(_T(""));
						}

						bExist = CWinDirectoryIterator::FileExists(fpath);
						if (overwrite && bExist)
						{
							CWinDirectoryIterator::UnlinkFile(fpath);
							bContinue = true;
						}
						if (!overwrite && bExist)
							bContinue = false;
						if (overwrite && !bExist)
							bContinue = true;
						if (!overwrite && !bExist)
							bContinue = true;

						if (bContinue)
						{
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

							zipIt->GetProperty(_T("CRC32"), crc32, isNull);
							zipIt->GetProperty(_T("UNCOMPRESSEDSIZE"), uncompressedsize, isNull);
							zipIt->GetProperty(_T("DOSFILETIME"), lastmodfiletime, isNull);

							zipIt->GetProperty(_T("NTFSCTIME"), vCreationTime, isCreationTimeNull);
							zipIt->GetProperty(_T("NTFSATIME"), vLastAccessTime, isLastAccessNull);
							zipIt->GetProperty(_T("NTFSMTIME"), vLastWriteTime, isLastWriteNull);

							CArchiveFile* afile = zipIt->GetFile();
							CFileFilterInput* pInput = OK_NEW_OPERATOR CFileFilterInput(afile);

							CSecurityFile* pOutFile = OK_NEW_OPERATOR CSecurityFile;

							pOutFile->Create(fpath, false, CFile::BinaryFile_NoEncoding);

							CFileFilterOutput* pOutput = OK_NEW_OPERATOR CFileFilterOutput(pOutFile);
							CZipDeCompressFilter* pFilter = OK_NEW_OPERATOR CZipDeCompressFilter(pInput, pOutput);

							pFilter->open();
							pFilter->do_filter();
							pFilter->close();

							if (Cast(dword, crc32) != pFilter->get_crc32())
								CERR << fpath.get_Path() << _T(" crc32 mismatch") << endl;
							if (Cast(dword, uncompressedsize) != pFilter->get_unCompressedSize())
								CERR << fpath.get_Path() << _T(" uncompressedsize mismatch") << endl;

							if (isLastWriteNull)
							{
								CDateTime vlastmodfiletime(Cast(time_t, lastmodfiletime));

								CWinDirectoryIterator::WriteFileTimes(fpath, vlastmodfiletime, vlastmodfiletime, vlastmodfiletime);
							}
							else
							{
								FILETIME vCTime = DerefAnyPtr(FILETIME, &vCreationTime);
								FILETIME vATime = DerefAnyPtr(FILETIME, &vLastAccessTime);
								FILETIME vMTime = DerefAnyPtr(FILETIME, &vLastWriteTime);

								CWinDirectoryIterator::_WriteFileTimes(fpath, &vCTime, &vATime, &vMTime);
							}

							pFilter->release();
							pInput->release();
							pOutput->release();
							afile->release();
							pOutFile->release();
							bSkip = false;
						}
					}
					if (bSkip)
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
	catch (CBaseException* ex)
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

void XUnzipFreshenFiles(ConstRef(CFilePath) fzipfile, WBool usefolders, ConstRef(CDataVectorT<mbchar>) filespecs, ConstRef(CDataVectorT<mbchar>) excludespecs)
{
	CFilePath fcurDir;
	CSecurityFile* pArchiveFile = NULL;
	CArchiveIterator *zipIt = NULL;

	try
	{
		CDirectoryIterator::GetCurrentDirectory(fcurDir);

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
					bool bSkip = true;

					if (_TestFileSpecs(tmp, filespecs) && (!(_TestExcludeSpecs(tmp, excludespecs))))
					{
						CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());

						if ( usefolders )
						{
							fpath.Normalize(fcurDir);

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
						if ( CWinDirectoryIterator::FileExists(fpath) )
						{
							CStringBuffer tmpFileTime;
							CStringBuffer tmpArchiveTime;
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
							bool bUpdate;

							zipIt->GetProperty(_T("CRC32"), crc32, isNull);
							zipIt->GetProperty(_T("UNCOMPRESSEDSIZE"), uncompressedsize, isNull);
							zipIt->GetProperty(_T("DOSFILETIME"), lastmodfiletime, isNull);

							zipIt->GetProperty(_T("NTFSCTIME"), vCreationTime, isCreationTimeNull);
							zipIt->GetProperty(_T("NTFSATIME"), vLastAccessTime, isLastAccessNull);
							zipIt->GetProperty(_T("NTFSMTIME"), vLastWriteTime, isLastWriteNull);

							bUpdate = false;
							if (isLastWriteNull)
							{
								CDirectoryIterator it(fpath);

								if (it)
								{
									CDateTime vFileTime(it.get_LastWriteTime());
									CDateTime vArchiveTime(Cast(time_t, lastmodfiletime));

									if (vArchiveTime > vFileTime)
									{
										vArchiveTime.GetTimeString(tmpArchiveTime);
										vFileTime.GetTimeString(tmpFileTime);
										bUpdate = true;
									}
								}
							}
							else
							{
								FILETIME vCTime;
								FILETIME vATime;
								FILETIME vMTime;

								CWinDirectoryIterator::_ReadFileTimes(fpath, &vCTime, &vATime, &vMTime);

								CSystemTime vFileTime(vMTime);
								CSystemTime vArchiveTime(DerefAnyPtr(FILETIME, &vLastWriteTime));

								if (vArchiveTime > vFileTime)
								{
									vArchiveTime.GetTimeString(tmpArchiveTime);
									vFileTime.GetTimeString(tmpFileTime);
									bUpdate = true;
								}
							}
							if (bUpdate)
							{
								CStringBuffer tmp;

								CWinDirectoryIterator::UnlinkFile(fpath);

								CArchiveFile* afile = zipIt->GetFile();
								CFileFilterInput* pInput = OK_NEW_OPERATOR CFileFilterInput(afile);

								CSecurityFile* pOutFile = OK_NEW_OPERATOR CSecurityFile;

								pOutFile->Create(fpath, false, CFile::BinaryFile_NoEncoding);

								CFileFilterOutput* pOutput = OK_NEW_OPERATOR CFileFilterOutput(pOutFile);
								CZipDeCompressFilter* pFilter = OK_NEW_OPERATOR CZipDeCompressFilter(pInput, pOutput);

								pFilter->open();
								pFilter->do_filter();
								pFilter->close();

								if ( Cast(dword,crc32) != pFilter->get_crc32() )
									CERR << fpath.get_Path() << _T(" crc32 mismatch") << endl;
								if ( Cast(dword,uncompressedsize) != pFilter->get_unCompressedSize() )
									CERR << fpath.get_Path() << _T(" uncompressedsize mismatch") << endl;

								if (isLastWriteNull)
								{
									CDateTime vlastmodfiletime(Cast(time_t, lastmodfiletime));

									CWinDirectoryIterator::WriteFileTimes(fpath, vlastmodfiletime, vlastmodfiletime, vlastmodfiletime);
								}
								else
								{
									FILETIME vCTime = DerefAnyPtr(FILETIME, &vCreationTime);
									FILETIME vATime = DerefAnyPtr(FILETIME, &vLastAccessTime);
									FILETIME vMTime = DerefAnyPtr(FILETIME, &vLastWriteTime);

									CWinDirectoryIterator::_WriteFileTimes(fpath, &vCTime, &vATime, &vMTime);
								}

								pFilter->release();
								pInput->release();
								pOutput->release();
								afile->release();
								pOutFile->release();

								tmp.FormatString(__FILE__LINE__ _T("File '%s' freshened\n  FILETIME=%s\n  ARCHIVE =%s\n"), fpath.GetString(), tmpFileTime.GetString(), tmpArchiveTime.GetString());
								COUT << tmp;

								bSkip = false;
							}
						}
					}
					if ( bSkip )
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
	catch (CBaseException* ex)
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

void XUnzipNewerFiles(ConstRef(CFilePath) fzipfile, WBool usefolders, ConstRef(CDataVectorT<mbchar>) filespecs, ConstRef(CDataVectorT<mbchar>) excludespecs)
{
	CFilePath fcurDir;
	CSecurityFile* pArchiveFile = NULL;
	CArchiveIterator *zipIt = NULL;

	try
	{
		CDirectoryIterator::GetCurrentDirectory(fcurDir);

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
					bool bSkip = true;

					if (_TestFileSpecs(tmp, filespecs) && (!(_TestExcludeSpecs(tmp, excludespecs))))
					{
						CFilePath fpath(tmp, CDirectoryIterator::UnixPathSeparatorString());

						if ( usefolders )
						{
							fpath.Normalize(fcurDir);

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

						CStringBuffer tmpFileTime;
						CStringBuffer tmpArchiveTime;
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
						bool bUpdate;

						zipIt->GetProperty(_T("CRC32"), crc32, isNull);
						zipIt->GetProperty(_T("UNCOMPRESSEDSIZE"), uncompressedsize, isNull);
						zipIt->GetProperty(_T("DOSFILETIME"), lastmodfiletime, isNull);

						zipIt->GetProperty(_T("NTFSCTIME"), vCreationTime, isCreationTimeNull);
						zipIt->GetProperty(_T("NTFSATIME"), vLastAccessTime, isLastAccessNull);
						zipIt->GetProperty(_T("NTFSMTIME"), vLastWriteTime, isLastWriteNull);

						bUpdate = false;
						if (CWinDirectoryIterator::FileExists(fpath))
						{
							if (isLastWriteNull)
							{
								CDirectoryIterator it(fpath);

								if (it)
								{
									CDateTime vFileTime(it.get_LastWriteTime());
									CDateTime vArchiveTime(Cast(time_t, lastmodfiletime));

									if (vArchiveTime > vFileTime)
									{
										vArchiveTime.GetTimeString(tmpArchiveTime);
										vFileTime.GetTimeString(tmpFileTime);
										bUpdate = true;
									}
								}
							}
							else
							{
								FILETIME vCTime;
								FILETIME vATime;
								FILETIME vMTime;

								CWinDirectoryIterator::_ReadFileTimes(fpath, &vCTime, &vATime, &vMTime);

								CSystemTime vFileTime(vMTime);
								CSystemTime vArchiveTime(DerefAnyPtr(FILETIME, &vLastWriteTime));

								if (vArchiveTime > vFileTime)
								{
									vArchiveTime.GetTimeString(tmpArchiveTime);
									vFileTime.GetTimeString(tmpFileTime);
									bUpdate = true;
								}
							}
							if (bUpdate)
								CWinDirectoryIterator::UnlinkFile(fpath);
						}
						else
							bUpdate = true;
						if (bUpdate)
						{
							CStringBuffer tmp;

							CArchiveFile* afile = zipIt->GetFile();
							CFileFilterInput* pInput = OK_NEW_OPERATOR CFileFilterInput(afile);

							CSecurityFile* pOutFile = OK_NEW_OPERATOR CSecurityFile;

							pOutFile->Create(fpath, false, CFile::BinaryFile_NoEncoding);

							CFileFilterOutput* pOutput = OK_NEW_OPERATOR CFileFilterOutput(pOutFile);
							CZipDeCompressFilter* pFilter = OK_NEW_OPERATOR CZipDeCompressFilter(pInput, pOutput);

							pFilter->open();
							pFilter->do_filter();
							pFilter->close();

							if ( Cast(dword,crc32) != pFilter->get_crc32())
								CERR << fpath.get_Path() << _T(" crc32 mismatch") << endl;
							if ( Cast(dword,uncompressedsize) != pFilter->get_unCompressedSize())
								CERR << fpath.get_Path() << _T(" uncompressedsize mismatch") << endl;

							if (isLastWriteNull)
							{
								CDateTime vlastmodfiletime(Cast(time_t, lastmodfiletime));

								CWinDirectoryIterator::WriteFileTimes(fpath, vlastmodfiletime, vlastmodfiletime, vlastmodfiletime);
							}
							else
							{
								FILETIME vCTime = DerefAnyPtr(FILETIME, &vCreationTime);
								FILETIME vATime = DerefAnyPtr(FILETIME, &vLastAccessTime);
								FILETIME vMTime = DerefAnyPtr(FILETIME, &vLastWriteTime);

								CWinDirectoryIterator::_WriteFileTimes(fpath, &vCTime, &vATime, &vMTime);
							}

							pFilter->release();
							pInput->release();
							pOutput->release();
							afile->release();
							pOutFile->release();

							if (tmpFileTime.IsEmpty())
								tmp.FormatString(__FILE__LINE__ _T("File '%s' created\n"), fpath.GetString());
							else
								tmp.FormatString(__FILE__LINE__ _T("File '%s' freshened\n  FILETIME=%s\n  ARCHIVE =%s\n"), fpath.GetString(), tmpFileTime.GetString(), tmpArchiveTime.GetString());
							COUT << tmp;

							bSkip = false;
						}
					}
					if ( bSkip )
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
	catch (CBaseException* ex)
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
