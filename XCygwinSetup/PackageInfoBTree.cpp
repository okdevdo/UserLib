/******************************************************************************
    
	This file is part of XSetup, which is part of UserLib.

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
#include "Program.h"
#include "ConsoleApplication.h"
#include "PackageInfoBTree.h"
#include "SecurityFile.h"
#include "Filter.h"

CPackageInfoBTree::CPackageInfoBTree(DECL_FILE_LINE TListCnt maxEntriesPerNode):
	CDataBTreeT<CPackageInfo>(ARGS_FILE_LINE maxEntriesPerNode)
{
}

CPackageInfoBTree::~CPackageInfoBTree()
{
	Close(PackageInfoDeleteFunc, NULL);
}

class CPackageInfoBTreeFilterOutput: public CFilterOutput
{
public:
	CPackageInfoBTreeFilterOutput(Ref(CPackageInfoBTree) _btree, ConstRef(CFilePath) iniRootPath):
	    m_btree(_btree), m_iniRootPath(iniRootPath), m_current(NULL), m_installInfo(NULL), m_status(0)
	{
	}

	virtual ~CPackageInfoBTreeFilterOutput()
	{
	}

	virtual void open()
	{
	}

	virtual void write(Ref(CByteBuffer) outputbuf);

	virtual void close()
	{
	}

private:
	Ref(CPackageInfoBTree) m_btree;
	CFilePath m_iniRootPath;
	Ptr(CPackageInfo) m_current;
	Ptr(CPackageInstallInfo) m_installInfo;
	word m_status;
	CStringBuffer m_installCat;
	CPackageInfo m_search;
};

void CPackageInfoBTreeFilterOutput::write(Ref(CByteBuffer) outputbuf)
{
	CByteBuffer buf;
	CStringBuffer tmp;
	CStringBuffer tmp1;
	int pos;

	switch ( m_status )
	{
	case 0:
		pos = outputbuf.find_Char('@');
		if ( pos == 0 )
		{
			outputbuf.get_SubBuffer(1, outputbuf.get_BufferSize() - 1, buf);
			tmp.convertFromByteBuffer(buf);
			tmp.Trim();
			
			m_search.SetPackageName(tmp);
			CPackageInfoBTree::Iterator itP = m_btree.FindSorted(&m_search, PackageInfoSearchAndSortFunc);

			if ( itP && (*itP) && (PackageInfoSearchAndSortFunc(&m_search, *itP) == 0) )
				m_current = *itP;
			else
			{
				m_current = OK_NEW_OPERATOR CPackageInfo(tmp);
				m_btree.InsertSorted(m_current, PackageInfoSearchAndSortFunc);
			}
			m_installInfo = NULL;
			m_installCat.SetString(__FILE__LINE__ _T("Curr"));
			m_status = 1;
		}
		break;
	case 1:
		if ( outputbuf.get_BufferSize() == 0 )
		{
			m_status = 0;
			break;
		}
		pos = outputbuf.find_Char('[');
		if ( pos == 0 )
		{
			pos = outputbuf.find_Char(']');
			outputbuf.get_SubBuffer(1, pos - 1, buf);
			m_installCat.convertFromByteBuffer(buf);
			m_installCat.Trim();
			m_installInfo = NULL;
			break;
		}
		pos = outputbuf.find_Char(':');
		if ( pos >= 0 )
		{
			outputbuf.get_SubBuffer(0, pos, buf);
			tmp.convertFromByteBuffer(buf);
			tmp.Trim();

			outputbuf.get_SubBuffer(pos + 1, outputbuf.get_BufferSize() - pos - 1, buf);
			tmp1.convertFromByteBuffer(buf);
			tmp1.Trim();

			if ( tmp == CStringLiteral(_T("sdesc")) )
			{
				CStringConstIterator it(tmp1);

				if ( it[0] == _T('"') )
				{
					if ( it[it.GetLength() - 1] == _T('"') )
						tmp1.DeleteString(tmp1.GetLength() - 1, 1);
					tmp1.DeleteString(0, 1);
				}
				m_current->SetShortDescription(tmp1);
			}
			else if ( tmp == CStringLiteral(_T("ldesc")) )
			{
				CStringConstIterator it(tmp1);

				if ( it[0] == _T('"') )
				{
					if ( it[it.GetLength() - 1] == _T('"') )
						tmp1.DeleteString(tmp1.GetLength() - 1, 1);
					else
						m_status = 2;
					tmp1.DeleteString(0, 1);
				}
				m_current->SetLongDescription(tmp1);
			}
			else if ( tmp == CStringLiteral(_T("category")) )
			{
				CPointer split[64];
				dword max;

				tmp1.Split(_T(" "), split, 64, &max);
				for ( dword ix = 0; ix < max; ++ix )
					m_current->AddCategory(CStringBuffer(__FILE__LINE__ split[ix]));
			}
			else if ( tmp == CStringLiteral(_T("requires")) )
			{
				CPointer split[64];
				dword max;

				tmp1.Split(_T(" "), split, 64, &max);
				for ( dword ix = 0; ix < max; ++ix )
					m_current->AddRequiredPackage(CStringBuffer(__FILE__LINE__ split[ix]));
			}
			else if ( tmp == CStringLiteral(_T("version")) )
			{
				if ( PtrCheck(m_installInfo) )
				{
					m_installInfo = OK_NEW_OPERATOR CPackageInstallInfo(m_iniRootPath, tmp1);

					if ( m_installCat.Compare(_T("test"), 0, CStringLiteral::cIgnoreCase) == 0 )
						m_installInfo->SetTestVersion(true);
					m_current->AddPackageInstallInfo(m_installInfo);
				}
			}
			else if ( tmp == CStringLiteral(_T("install")) )
			{
				CPointer split[8];
				dword max;

				tmp1.Split(_T(" "), split, 8, &max);
				if ( max == 3 )
				{
					Ptr(CInstallPackageInfo) info = OK_NEW_OPERATOR CInstallPackageInfo(m_current->GetPackageName(), CStringBuffer(__FILE__LINE__ split[0]));
					CFile::TFileSize sz;
					long int sz1;

					tmp.SetString(__FILE__LINE__ split[1]);
					if ( tmp.ScanString(_T("%ld"), &sz1) > 0 )
					{
						sz = sz1;
						info->SetFileSize(sz);
					}
					info->SetMD5Buffer(CStringBuffer(__FILE__LINE__ split[2]));
					m_installInfo->SetBinaryPackage(info);
				}
			}
			else if ( tmp == CStringLiteral(_T("source")) )
			{
				CPointer split[8];
				dword max;

				tmp1.Split(_T(" "), split, 8, &max);
				if ( max == 3 )
				{
					Ptr(CInstallPackageInfo) info = OK_NEW_OPERATOR CInstallPackageInfo(m_current->GetPackageName(), CStringBuffer(__FILE__LINE__ split[0]));
					CFile::TFileSize sz;
					long int sz1;

					tmp.SetString(__FILE__LINE__ split[1]);
					if ( tmp.ScanString(_T("%ld"), &sz1) > 0 )
					{
						sz = sz1;
						info->SetFileSize(sz);
					}
					info->SetMD5Buffer(CStringBuffer(__FILE__LINE__ split[2]));
					m_installInfo->SetSourcePackage(info);
				}
			}
		}
		break;
	case 2:
		{
			if ( outputbuf.get_BufferSize() == 0 )
				break;
			tmp.convertFromByteBuffer(outputbuf);

			CStringConstIterator it(tmp.GetString());
			WULong len = it.GetLength();

			if ( len > 0 )
			{
				--len;
				if ( it[len] == _T('"') )
				{
					tmp.DeleteString(len, 1);
					m_status = 1;
				}
				m_current->AddLongDescription(tmp);
			}
		}
		break;
	}
}

void CPackageInfoBTree::Fill(ConstRef(CStringBuffer) inputfilepath)
{
	CFilePath iniRootPath(inputfilepath);
	Ptr(CSecurityFile) iniFile = NULL;
	Ptr(CFilterInput) pInput = NULL;
	Ptr(CFilterOutput) pOutput = NULL;
	Ptr(CFilter) pFilter = NULL;

	try
	{
		iniFile = OK_NEW_OPERATOR CSecurityFile(iniRootPath);
		iniRootPath.set_Filename(_T(""));
		iniRootPath.set_Directory(_T(""), -1);

		pInput = OK_NEW_OPERATOR CFileFilterInput(iniFile);
		pOutput = OK_NEW_OPERATOR CPackageInfoBTreeFilterOutput(*this, iniRootPath);
		pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput, CLineReadFilter::UnixLineEnd);

		pFilter->open();
		pFilter->do_filter();
		pFilter->close();

		pInput->release();
		pOutput->release();
		pFilter->release();
		iniFile->release();
	}
	catch ( CSecurityFileException* ex )
	{
		CERR << ex->GetExceptionMessage() << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << ex->GetExceptionMessage() << endl;
		if ( NotPtrCheck(pInput) )
			pInput->release();
		if ( NotPtrCheck(pOutput) )
			pOutput->release();
		if ( NotPtrCheck(pFilter) )
			pFilter->release();
		if ( NotPtrCheck(iniFile) )
			iniFile->release();
	}
}
