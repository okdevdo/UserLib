/******************************************************************************
    
	This file is part of XCygwinSetup, which is part of UserLib.

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
#include "InstallPackageInfo.h"
#include "Filter.h"
#include "DirectoryIterator.h"
#include "SecurityFile.h"
#include "WinDirectoryIterator.h"

CPackageVersion::CPackageVersion(void):
    m_sVersion()
{
}

CPackageVersion::CPackageVersion(ConstRef(CStringBuffer) _version):
    m_sVersion(_version)
{
}

CPackageVersion::CPackageVersion(ConstRef(CPackageVersion) _copy):
    m_sVersion(_copy.m_sVersion)
{
}

CPackageVersion::~CPackageVersion(void)
{
}

void CPackageVersion::operator = (ConstRef(CStringBuffer) _copy)
{
	m_sVersion = _copy;
}

void CPackageVersion::operator = (ConstRef(CPackageVersion) _copy)
{
	m_sVersion = _copy.m_sVersion;
}

sword CPackageVersion::Compare(ConstRef(CPackageVersion) _other) const
{
	if (m_sVersion == _other.m_sVersion) return 0;

	CStringConstIterator a(m_sVersion);
	CStringConstIterator b(_other.m_sVersion);
	CStringConstIterator a1;
	CStringConstIterator b1;
	dword apos1, apos2 = 0, bpos1, bpos2 = 0;
	dword alen = a.GetLength(), blen = b.GetLength();
	bool isnum;
	int cval;

	while ( (apos2 < alen) && (bpos2 < blen) )
	{
		apos1 = apos2;
		bpos1 = bpos2;

		if ( s_isdigit(a[apos2]) )
		{
			while ( (apos2 < alen) && s_isdigit(a[apos2]) ) apos2++;
			while ( (bpos2 < blen) && s_isdigit(b[bpos2]) ) bpos2++;
			isnum = true;
		}
		else
		{
			while ( (apos2 < alen) && (!s_isdigit(a[apos2])) ) apos2++;
			while ( (bpos2 < blen) && (!s_isdigit(b[bpos2])) ) bpos2++;
			isnum = false;
		}

		/* if (apos1 == apos2) { a logical impossibility has happened; } */

		/* isdigit(a[0]) != isdigit(b[0])
		* arbitrarily sort the non-digit first */
		if ( bpos1 == bpos2 ) return (isnum ? 1 : -1);

		if ( isnum )
		{
			/* skip numeric leading zeros */
			while (apos1 < alen && a[apos1] == _T('0')) apos1++;
			while (bpos1 < blen && b[bpos1] == _T('0')) bpos1++;
		}

		if ( (apos2 - apos1) > (bpos2 - bpos1) ) return 1;
		if ( (apos2 - apos1) < (bpos2 - bpos1) ) return -1;

		/* do an ordinary lexicographic string comparison */
		a1 = a;
		a1 += apos1;
		b1 = b;
		b1 += bpos1;
		cval = s_strncmp(a1.GetCurrent(), b1.GetCurrent(), apos2 - apos1);
		if ( cval ) 
			return (cval < 1 ? -1 : 1);
	}
	if ( (apos2 == alen) && (bpos2 == blen) ) return 0;

	/* the version with a suffix remaining is greater */
	return ((apos2 < alen) ? 1 : -1);
}

CInstallPackageInfo::CInstallPackageInfo(void):
	m_RelativePath(__FILE__LINE__ CStringBuffer::null(), CDirectoryIterator::UnixPathSeparatorString()),
	m_sName(),
	m_Version(),
	m_Type(),
	m_Extension(),
	m_FileSize(0),
	m_MD5BUffer(),
	m_IsInstalled(false)
{
}

CInstallPackageInfo::CInstallPackageInfo(ConstRef(CStringBuffer) _name):
	m_RelativePath(__FILE__LINE__ CStringBuffer::null(), CDirectoryIterator::UnixPathSeparatorString()),
	m_sName(_name),
	m_Version(),
	m_Type(),
	m_Extension(),
	m_FileSize(0),
	m_MD5BUffer(),
	m_IsInstalled(false)
{
}

CInstallPackageInfo::CInstallPackageInfo(ConstRef(CStringBuffer) _name, ConstRef(CStringBuffer) _path, bool _bParseInstalled) :
	m_RelativePath(_path, CDirectoryIterator::UnixPathSeparatorString()),
	m_sName(_name),
	m_Version(),
	m_Type(),
	m_Extension(),
	m_FileSize(0), 
	m_MD5BUffer(),
	m_IsInstalled(false)
{
	if ( _bParseInstalled )
	{
		Parse1();
		m_IsInstalled = true;
	}
	else
		Parse2();
}

CInstallPackageInfo::~CInstallPackageInfo(void)
{
}

void CInstallPackageInfo::Parse1()
{
	CStringBuffer fname(m_RelativePath.get_Filename());

	if ( m_sName.IsEmpty() || fname.IsEmpty() )
	{
		CERR << _T("CInstallPackageInfo::Parse1: m_sName.IsEmpty() || fname.IsEmpty()") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("CInstallPackageInfo::Parse1: m_sName.IsEmpty() || fname.IsEmpty()") << endl;
		return;
	}

	CStringConstIterator it1(fname);

	it1.FindReverse(_T('.'));
	if ( CStringLiteral(it1.GetCurrent()) == CStringLiteral(_T(".bz2")) )
		it1 -= 4;
	if ( CStringLiteral(it1.GetCurrent()).Compare(CStringLiteral(_T(".tar")), 4, CStringLiteral::cLimited) != 0 )
	{
		CERR << _T("CInstallPackageInfo::Parse1: Extension mismatch '") << fname << _T("'") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("CInstallPackageInfo::Parse1: Extension mismatch '") << fname << _T("'") << endl;
		return;
	}
	m_Extension.SetString(__FILE__LINE__ it1);

	CStringConstIterator it2(it1);
	CStringBuffer tmp;

	it1 = fname;
	it1.Find(m_sName);
	if ( it1.IsBegin() )
	{
		it1 += m_sName.GetLength();
		if ( it1[0] == _T('-') )
			++it1;
		fname.SubString(Castdword(it1.GetCurrent() - it1.GetOrigin()), Castdword(it2.GetCurrent() - it1.GetCurrent()), tmp);
		m_Version = tmp;
		return;
	}
	CERR << _T("CInstallPackageInfo::Parse1: Name mismatch '") << fname << _T("' and '") << m_sName << _T("'") << endl;
	if (theConsoleApp->is_stdout_redirected())
		COUT << _T("CInstallPackageInfo::Parse1: Name mismatch '") << fname << _T("' and '") << m_sName << _T("'") << endl;
}

void CInstallPackageInfo::Parse2()
{
	CStringBuffer fname(m_RelativePath.get_Filename());

	if (m_sName.IsEmpty() || fname.IsEmpty())
	{
		CERR << _T("CInstallPackageInfo::Parse2: m_sName.IsEmpty() || fname.IsEmpty()") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("CInstallPackageInfo::Parse2: m_sName.IsEmpty() || fname.IsEmpty()") << endl;
		return;
	}

	CStringConstIterator it1(fname);

	it1.FindReverse(_T('.'));
	if ((CStringLiteral(it1.GetCurrent()) == CStringLiteral(_T(".bz2")))
		|| (CStringLiteral(it1.GetCurrent()) == CStringLiteral(_T(".gz")))
		|| (CStringLiteral(it1.GetCurrent()) == CStringLiteral(_T(".xz"))))
		it1 -= 4;
	if (CStringLiteral(it1.GetCurrent()).Compare(CStringLiteral(_T(".tar")), 4, CStringLiteral::cLimited) != 0)
	{
		CERR << _T("CInstallPackageInfo::Parse2: Extension mismatch '") << fname << _T("'") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("CInstallPackageInfo::Parse2: Extension mismatch '") << fname << _T("'") << endl;
		return;
	}
	m_Extension.SetString(__FILE__LINE__ it1);

	CStringConstIterator it2(it1);
	CStringBuffer tmp;
	bool b = false;

	it2 -= 4;
	if (CStringLiteral(it2.GetCurrent()).Compare(CStringLiteral(_T("-src")), 4, CStringLiteral::cLimited) != 0)
	{
		it2 = it1;
		it1 = fname;
		it1.Find(m_sName);
		if (it1.IsBegin())
		{
			it1 += m_sName.GetLength();
			if (it1[0] == _T('-'))
				++it1;
			fname.SubString(Castdword(it1.GetCurrent() - it1.GetOrigin()), Castdword(it2.GetCurrent() - it1.GetCurrent()), tmp);
			m_Version = tmp;
			return;
		}
		CERR << _T("CInstallPackageInfo::Parse2: Name mismatch '") << fname << _T("' and '") << m_sName << _T("'") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("CInstallPackageInfo::Parse2: Name mismatch '") << fname << _T("' and '") << m_sName << _T("'") << endl;
		b = true;
	}
	else
		m_Type.SetString(__FILE__LINE__ _T("-src"));
	it1 = fname;
	while ((!(it1.IsEnd())) && (!((it1[0] == _T('-')) && (s_isdigit(it1[1])))))
		++it1;
	fname.SubString(0, Castdword(it1.GetCurrent() - it1.GetOrigin()), m_sName);
	++it1;
	fname.SubString(Castdword(it1.GetCurrent() - it1.GetOrigin()), Castdword(it2.GetCurrent() - it1.GetCurrent()), tmp);
	m_Version = tmp;
	if (b)
	{
		CERR << _T("CInstallPackageInfo::Parse2: Name mismatch resolved '") << m_sName << _T("' and '") << tmp << _T("'") << endl;
		if (theConsoleApp->is_stdout_redirected())
			COUT << _T("CInstallPackageInfo::Parse2: Name mismatch resolved '") << m_sName << _T("' and '") << tmp << _T("'") << endl;
	}
}

void CInstallPackageInfo::Print(WInt iVerbose, dword indent) const
{
	if ( iVerbose > 1 )
	{
		if ( indent > 0 )
			COUT << std::setw(indent) << _T(" ");
		if ( m_Type == CStringLiteral(_T("-src")) )
			COUT << _T("source: ");
		else
			COUT << _T("install: ");
		COUT << m_RelativePath.get_Path() << _T(" ") << m_FileSize << _T(" ") << m_MD5BUffer.GetDigest() << endl;
	}
	if ( iVerbose > 2 )
	{
		if ( indent > 0 )
			COUT << std::setw(indent) << _T(" ");
		COUT << _T("details: ") << m_sName << _T(" ") << m_Version.GetVersion() << _T(" ") << m_Extension << endl;
	}
}

CInstallPackageInfoVector::CInstallPackageInfoVector(DECL_FILE_LINE TListCnt cnt, TListCnt exp):
   super(ARGS_FILE_LINE cnt, exp), m_Modified(false)
{
}

CInstallPackageInfoVector::~CInstallPackageInfoVector()
{
}

class CInstallPackageInfoFilterOutput: public CFilterOutput
{
public:
	CInstallPackageInfoFilterOutput(Ref(CInstallPackageInfoVector) _vector):
	    m_vector(_vector)
	{
	}

	virtual ~CInstallPackageInfoFilterOutput()
	{
	}

	virtual void open()
	{
	}

	virtual void write(Ref(CByteBuffer) outputbuf)
	{
		CStringBuffer tmp;
		CPointer tmpSplit[16];
		dword cnt;

		tmp.convertFromByteBuffer(outputbuf);
		tmp.Split(_T(" "), tmpSplit, 16, &cnt);
		if ( cnt < 3 )
			return;

		Ptr(CInstallPackageInfo) pf = OK_NEW_OPERATOR CInstallPackageInfo(CStringBuffer(__FILE__LINE__ tmpSplit[0]), CStringBuffer(__FILE__LINE__ tmpSplit[1]), true);

		m_vector.InsertSorted(pf);
	}

	virtual void close()
	{
	}

private:
	Ref(CInstallPackageInfoVector) m_vector;
};

void CInstallPackageInfoVector::LoadInstalled(CConstPointer _rootPath)
{
	CFilePath fpath(__FILE__LINE__ CYGWIN_SETUP_DIR, -1, CDirectoryIterator::UnixPathSeparatorString());

	fpath.Normalize(_rootPath);
	fpath.set_Filename(_T("installed.db"));

	CCppObjectPtr<CSecurityFile> pFile = OK_NEW_OPERATOR CSecurityFile();

	pFile->Open(fpath);

	CCppObjectPtr<CFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(pFile);
	CCppObjectPtr<CFilterOutput> pOutput = OK_NEW_OPERATOR CInstallPackageInfoFilterOutput(*this);
	CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CLineReadFilter(pInput, pOutput, CLineReadFilter::UnixLineEnd);

	pFilter->open();
	pFilter->do_filter();
	pFilter->close();
}

class CInstallPackageInfoFilterInput: public CFilterInput
{
public:
	CInstallPackageInfoFilterInput(ConstRef(CInstallPackageInfoVector) vector):
	    m_vector(vector), m_bFirst(true)
	{
	}

	virtual ~CInstallPackageInfoFilterInput()
	{
	}

	virtual void open()
	{
		m_itvector = m_vector.Begin();
	}

	virtual CFile::TFileSize size()
	{
		return 0;
	}

	virtual void read(Ref(CByteBuffer) inputbuf)
	{
		CStringBuffer tmp;

		if ( m_bFirst )
		{
			tmp.SetString(__FILE__LINE__ _T("INSTALLED.DB 2"));
			m_bFirst = false;
		}
		else if ( !m_itvector )
		{
			inputbuf.set_BufferSize(__FILE__LINE__ 0);
			return;
		}
		else
		{
			CInstallPackageInfo* p = *m_itvector;

			while ( !(p->GetIsInstalled()) )
			{
				++m_itvector;
				if ( !m_itvector )
				{
					inputbuf.set_BufferSize(__FILE__LINE__ 0);
					return;
				}
				p = *m_itvector;
			}
			tmp = p->GetName();
			tmp += _T(" ");
			tmp += p->GetRelativePath().get_Path();
			tmp += _T(" 0");
			++m_itvector;
		}
		tmp.convertToByteBuffer(inputbuf);
	}

	virtual void close()
	{
	}

private:
	ConstRef(CInstallPackageInfoVector) m_vector;
	CInstallPackageInfoVector::Iterator m_itvector;
	bool m_bFirst;

	CInstallPackageInfoFilterInput();
};

void CInstallPackageInfoVector::SaveInstalled(CConstPointer _rootPath)
{
	CFilePath fpath(__FILE__LINE__ CYGWIN_TMP_DIR, -1, CDirectoryIterator::UnixPathSeparatorString());

	fpath.Normalize(_rootPath);
	fpath.set_Filename(_T("installed.db"));

	CCppObjectPtr<CSecurityFile> pFile = OK_NEW_OPERATOR CSecurityFile();

	CWinDirectoryIterator::UnlinkFile(fpath);
	pFile->Create(fpath, true, CFile::ISO_8859_1_Encoding, 0644);

	CCppObjectPtr<CFilterInput> pInput = OK_NEW_OPERATOR CInstallPackageInfoFilterInput(*this);
	CCppObjectPtr<CFilterOutput> pOutput = OK_NEW_OPERATOR CFileFilterOutput(pFile);
	CCppObjectPtr<CFilter> pFilter = OK_NEW_OPERATOR CLineWriteFilter(pInput, pOutput, CLineWriteFilter::UnixLineEnd);

	pFilter->open();
	pFilter->do_filter();
	pFilter->close();

	CFilePath fpath1(__FILE__LINE__ CYGWIN_SETUP_DIR, -1, CDirectoryIterator::UnixPathSeparatorString());

	fpath1.Normalize(_rootPath);
	fpath1.set_Filename(_T("installed.db"));

	CFilePath fpath2(__FILE__LINE__ CYGWIN_SETUP_DIR, -1, CDirectoryIterator::UnixPathSeparatorString());

	fpath2.Normalize(_rootPath);
	fpath2.set_Filename(_T("installed.db.bak"));

	CWinDirectoryIterator::UnlinkFile(fpath2);
	CWinDirectoryIterator::Rename(fpath1, fpath2);
	CWinDirectoryIterator::Rename(fpath, fpath1);
}
