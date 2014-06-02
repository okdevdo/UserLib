/******************************************************************************
    
	This file is part of XCheckSum, which is part of UserLib.

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
// XUnzip.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "Application.h"
#include "Filter.h"
#include "OpenSSLLib.h"
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
#include <conio.h>
#endif
#endif

void __stdcall VectorEmptyDeleteFunc(ConstPointer data, Pointer context)
{
}

class TCheckDataItem: public CCppObject
{
public:
	CFilePath Argument;
	CStringBuffer Result;

	TCheckDataItem(void) :
		Argument(), Result()
	{
	}
	TCheckDataItem(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r) :
		Argument(a), Result(r)
	{
		CStringConstIterator it(Result);

		if (it[0] == _T('\\'))
			Result.DeleteString(0);
		Argument.Normalize();
		Argument.addRef();
		Result.addRef();
	}
	TCheckDataItem(ConstRef(TCheckDataItem) copy) :
		Argument(copy.Argument), Result(copy.Result)
	{
	}
};

class XCheckSumApplication : public CApplication
{
public:
	XCheckSumApplication() :
		CApplication(CStringBuffer(__FILE__LINE__ _T("XCheckSum"))),
		m_AnyOption(false),
		m_help(false),
		m_check(false),
		m_scheck(__FILE__LINE__ 16, 16),
		m_argument(false),
		m_sargument(__FILE__LINE__ 16, 16),
		m_md4(false),
		m_md5(false),
		m_sha(false),
		m_sha1(false),
		m_dss(false),
		m_dss1(false),
		m_mdc2(false),
		m_ripemd160(false),
		m_sha224(false),
		m_sha256(false),
		m_sha384(false),
		m_sha512(false)
	{
		COptionCallback<XCheckSumApplication> cb(this, &XCheckSumApplication::handleHelp);

		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("Check"))
			.shortName(_T("c"))
			.argument(_T("fpath"), true)
			.repeatable(true)
			.category(_T("CheckSum"))
			.description(_T("Read input from file.")));
		addOption(COption(_T("Argument"))
			.shortName(_T("a"))
			.argument(_T("name"), true, 1)
			.repeatable(true)
			.category(_T("CheckSum"))
			.description(_T("Argument from which checksum is to be taken.")));
		addOption(COption(_T("md4"))
			.shortName(_T("md4"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm md4.")));
		addOption(COption(_T("md5"))
			.shortName(_T("md5"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm md5 (default).")));
		addOption(COption(_T("sha"))
			.shortName(_T("sha"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm sha.")));
		addOption(COption(_T("sha1"))
			.shortName(_T("sha1"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm sha1.")));
		addOption(COption(_T("dss"))
			.shortName(_T("dss"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm dss.")));
		addOption(COption(_T("dss1"))
			.shortName(_T("dss1"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm dss1.")));
		addOption(COption(_T("mdc2"))
			.shortName(_T("mdc2"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm mdc2.")));
		addOption(COption(_T("ripemd160"))
			.shortName(_T("r160"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm ripemd160.")));
		addOption(COption(_T("sha224"))
			.shortName(_T("sha224"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm sha224.")));
		addOption(COption(_T("sha256"))
			.shortName(_T("sha256"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm sha256.")));
		addOption(COption(_T("sha384"))
			.shortName(_T("sha384"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm sha384.")));
		addOption(COption(_T("sha512"))
			.shortName(_T("sha512"))
			.category(_T("CheckSum"))
			.group(_T("algorithms"))
			.description(_T("Use algorithm sha512.")));
	}
	~XCheckSumApplication()
	{
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral& value)
	{
		m_help = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		COUT << _T("Option = ") << name << _T(", value = ") << value << endl;
		if (name.Compare(_T("Check")) == 0)
		{
			m_check = true;
			m_scheck.Append(value);
		}
		if (name.Compare(_T("Argument")) == 0)
		{
			m_argument = true;
			m_sargument.Append(value);
		}
		if (name.Compare(_T("md4")) == 0)
		{
			m_AnyOption = true;
			m_md4 = true;
		}
		if (name.Compare(_T("md5")) == 0)
		{
			m_AnyOption = true;
			m_md5 = true;
		}
		if (name.Compare(_T("sha")) == 0)
		{
			m_AnyOption = true;
			m_sha = true;
		}
		if (name.Compare(_T("sha1")) == 0)
		{
			m_AnyOption = true;
			m_sha1 = true;
		}
		if (name.Compare(_T("dss")) == 0)
		{
			m_AnyOption = true;
			m_dss = true;
		}
		if (name.Compare(_T("dss1")) == 0)
		{
			m_AnyOption = true;
			m_dss1 = true;
		}
		if (name.Compare(_T("mdc2")) == 0)
		{
			m_AnyOption = true;
			m_mdc2 = true;
		}
		if (name.Compare(_T("ripemd160")) == 0)
		{
			m_AnyOption = true;
			m_ripemd160 = true;
		}
		if (name.Compare(_T("sha224")) == 0)
		{
			m_AnyOption = true;
			m_sha224 = true;
		}
		if (name.Compare(_T("sha256")) == 0)
		{
			m_AnyOption = true;
			m_sha256 = true;
		}
		if (name.Compare(_T("sha384")) == 0)
		{
			m_AnyOption = true;
			m_sha384 = true;
		}
		if (name.Compare(_T("sha512")) == 0)
		{
			m_AnyOption = true;
			m_sha512 = true;
		}
	}

	virtual int main()
	{
		if (m_help || (!(m_argument || m_check)))
		{
			CStringBuffer tmp;

			usage(tmp);
			CERR << tmp;
			return 0;
		}
		if (!m_AnyOption)
		{
			m_AnyOption = true;
			m_md5 = true;
		}

		CDataVectorT<TCheckDataItem> list(__FILE__LINE__ 16, 16);
		CDataVectorT<TCheckDataItem>::Iterator it0;

		if (m_argument)
		{
			TMBCharList::Iterator it = m_sargument.Begin();

			while (it)
			{
				Ptr(TCheckDataItem) item = OK_NEW_OPERATOR TCheckDataItem(CFilePath(__FILE__LINE__ *it), CStringBuffer(__FILE__LINE__ _T("")));

				list.Append(item);
				++it;
			}
		}
		if (m_check)
		{
			TMBCharList::Iterator it = m_scheck.Begin();

			while (it)
			{
				CDataVectorT<CStringBuffer> loutput(__FILE__LINE__ 16, 16);
				CDataVectorT<CStringBuffer>::Iterator it1;
				Ptr(CFilterOutput) pFilterOutput = OK_NEW_OPERATOR CStringVectorFilterOutput(loutput);
				Ptr(CFilterInput) pFilterInput = OK_NEW_OPERATOR CFileFilterInput(CFilePath(__FILE__LINE__ *it));
				Ptr(CFilter) pFilter = OK_NEW_OPERATOR CLineReadFilter(pFilterInput, pFilterOutput);

				try
				{
					pFilter->open();
					pFilter->do_filter();
					pFilter->close();
				}
				catch (CBaseException* ex)
				{
					pFilter->close();
					CERR << ex->GetExceptionMessage() << endl;
				}
				pFilter->release();
				pFilterInput->release();
				pFilterOutput->release();

				it1 = loutput.Begin();
				while (it1)
				{
					CStringBuffer tmp(*it1);
					CStringConstIterator it2(tmp);
					CPointer fname = NULL;
					WULong fnsize = 0;
					WChar vChar = 0;
					CPointer fsum = NULL;
					WULong fssize = 0;

					it2.EatWord(fsum, fssize);
					it2.EatCharacter(vChar);
					it2.EatCharacter(vChar);
					it2.EatWord(fname, fnsize);

					Ptr(TCheckDataItem) item = OK_NEW_OPERATOR TCheckDataItem(CFilePath(__FILE__LINE__ fname, fnsize), CStringBuffer(__FILE__LINE__ fsum, fssize));

					list.Append(item);
					++it1;
				}
				++it;
			}
		}

		COpenSSLLib::Initialize();
		it0 = list.Begin();
		while (it0)
		{
			TCheckDataItem item(**it0);

			try
			{
				if (m_md4)
					f_md4(item.Argument, item.Result);
				if (m_md5)
					f_md5(item.Argument, item.Result);
				if (m_sha)
					f_sha(item.Argument, item.Result);
				if (m_sha1)
					f_sha1(item.Argument, item.Result);
				if (m_dss)
					f_dss(item.Argument, item.Result);
				if (m_dss1)
					f_dss1(item.Argument, item.Result);
				if (m_mdc2)
					f_mdc2(item.Argument, item.Result);
				if (m_ripemd160)
					f_ripemd160(item.Argument, item.Result);
				if (m_sha224)
					f_sha224(item.Argument, item.Result);
				if (m_sha256)
					f_sha256(item.Argument, item.Result);
				if (m_sha384)
					f_sha384(item.Argument, item.Result);
				if (m_sha512)
					f_sha512(item.Argument, item.Result);
			}
			catch (CBaseException* ex)
			{
				CERR << ex->GetExceptionMessage() << endl;
			}
			++it0;
		}
		COpenSSLLib::CleanUp();
		return 0;
	}

protected:
	WBool m_AnyOption;
	WBool m_help;
	WBool m_check;
	TMBCharList m_scheck;
	WBool m_argument;
	TMBCharList m_sargument;
	WBool m_md4;
	WBool m_md5;
	WBool m_sha;
	WBool m_sha1;
	WBool m_dss;
	WBool m_dss1;
	WBool m_mdc2;
	WBool m_ripemd160;
	WBool m_sha224;
	WBool m_sha256;
	WBool m_sha384;
	WBool m_sha512;
};

int
#if OK_COMP_MSC
_tmain(int argc, CPointer *argv)
#else
main(int argc, char** argv)
#endif
{
	int result;
	XCheckSumApplication app;

	result = app.run(argc, argv);
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
	_getch();
#endif
#endif
	return result;
}

