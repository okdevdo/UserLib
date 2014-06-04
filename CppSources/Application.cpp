/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published 
	by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#include "CPPS_PCH.H"
#include "Application.h"
#include "BaseException.h"
#include "FilePath.h"
#include "DirectoryIterator.h"
#include "File.h"
#include "Configuration.h"

#ifdef OK_SYS_WINDOWS
#include <conio.h>
#endif

#include <iostream>
#ifdef _UNICODE
#define COUT std::wcout
#define CERR std::wcerr
#define OSTREAM std::wostream
#else
#define COUT std::cout
#define CERR std::cerr
#define OSTREAM std::ostream
#endif

CApplication* CApplication::m_instance = nullptr;

CApplication::CApplication():
	m_defaultAppName(),
	m_definedOptions(__FILE__LINE__ 16, 16),
	m_Options(__FILE__LINE__ 16, 16),
	m_continueOptionProcessing(true),
	m_config()
{
	m_instance = this;
}

CApplication::CApplication(ConstRef(CStringBuffer) _defaultAppName) :
    m_defaultAppName(_defaultAppName),
	m_definedOptions(__FILE__LINE__ 16, 16),
	m_Options(__FILE__LINE__ 16, 16),
	m_continueOptionProcessing(true),
	m_config()
{
	m_instance = this;
}

CApplication::~CApplication()
{
	m_instance = nullptr;
}

void CApplication::addOption(COption& option)
{
	COption* p = option.clone();

	m_definedOptions.InsertSorted(p);
}

WBool CApplication::processOptions(int argc, CArray argv)
{
	COption* pOption = nullptr;
	WBool bPositioningEnabled = true;
	WInt i = 1;

	while ( m_continueOptionProcessing && (i < argc) )
	{
		CStringConstIterator it(argv[i]);
		WInt opt = isOption(it, i);

		switch ( opt )
		{
		case -1:
			break;
		case 0:
			{
				if (bPositioningEnabled)
				{
					COption* pOption1 = findPositionedOption(i);

					if (PtrCheck(pOption) && PtrCheck(pOption1))
						throw OK_NEW_OPERATOR COptionIllegalArgumentException(__FILE__LINE__ _T("Illegal Argument Exception (Argument: %s)"), argv[i]);
					if (NotPtrCheck(pOption1))
						pOption = pOption1;
				}

				CPointer args[64];
				dword maxargs = 0;

				s_strsplitquoted(argv[i], _T("\""), _T(";"), args, 64, &maxargs);
				if (PtrCheck(pOption) || (0 == maxargs))
					throw OK_NEW_OPERATOR COptionIllegalArgumentException(__FILE__LINE__ _T("Illegal Argument Exception (Argument: %s)"), argv[i]);
				processOption(pOption, args, maxargs);
			}
			break;
		default:
			{
				COption* pOption1 = findDefinedOption(it, opt);
				CStringConstIterator it2(it);
				CPointer args[64];
				dword maxargs = 0;

				bPositioningEnabled = false;
				if (NotPtrCheck(pOption) && (pOption->hasArgument()) && PtrCheck(findOption(pOption)))
				{
					if (pOption->argumentRequired())
						throw OK_NEW_OPERATOR COptionMissingArgumentException(__FILE__LINE__ _T("Missing Argument Exception (Option: %s)"), pOption->fullname().GetString());

					_processOption(pOption, nullptr, 0);
				}
				if (NotPtrCheck(pOption1))
				{
					pOption = pOption1;
#ifdef OK_SYS_WINDOWS
					it2.FirstOf(_T(":="));
#endif
#ifdef OK_SYS_UNIX
					it2.Find(_T('='));
#endif
					if (!(it2.IsEnd()))
					{
						++it2;
						if (!(it2.IsEnd()))
						{
							CPointer p = CastMutable(CPointer, it2.GetCurrent());

							s_strsplitquoted(p, _T("\""), _T(";"), args, 64, &maxargs);
							if (maxargs == 0)
								throw OK_NEW_OPERATOR COptionIllegalArgumentException(__FILE__LINE__ _T("Illegal argument Exception (Argument: %s)"), it2.GetCurrent());
						}
					}
					processOption(pOption, args, maxargs);
				}
			}
			break;
		}
		++i;
	}
	if ( m_continueOptionProcessing )
	{
		if (NotPtrCheck(pOption) && (pOption->hasArgument()) && PtrCheck(findOption(pOption)))
		{
			if (pOption->argumentRequired())
				throw OK_NEW_OPERATOR COptionMissingArgumentException(__FILE__LINE__ _T("Missing Argument Exception (Option: %s)"), pOption->fullname().GetString());

			_processOption(pOption, nullptr, 0);
		}

		COptionVector::Iterator it = m_definedOptions.Begin();
		COption* p;

		while ( it )
		{
			p = *it;
			if ( p->required() )
			{
				if (PtrCheck(findOption(p)))
					throw OK_NEW_OPERATOR COptionRequiredMissingException(__FILE__LINE__ _T("Required Option Exception (Option: %s)"), p->fullname().GetString());
			}
			++it;
		}
	}
	return true;
}

WInt CApplication::isOption(Ref(CStringConstIterator) it, int i)
{
	if ( it[0] == 0 )
		return -1;
#ifdef OK_SYS_WINDOWS
	if (it[0] == _T('/'))
	{
		++it;
		if ( it.IsEnd() )
			throw OK_NEW_OPERATOR COptionMissingOptionException(__FILE__LINE__ _T("Option missing at position %d"), i);
		return 2;
	}
#endif
	if ( it[0] == _T('-') )
	{
		++it;
		if ( it.IsEnd() )
			throw OK_NEW_OPERATOR COptionMissingOptionException(__FILE__LINE__ _T("Option missing at position %d"), i);
		if ( it[0] == _T('-') )
		{
			++it;
			if ( it.IsEnd() )
				throw OK_NEW_OPERATOR COptionMissingOptionException(__FILE__LINE__ _T("Option missing at position %d"), i);
			return 2;
		}
		return 1;
	}
	return 0;
}

COption* CApplication::findDefinedOption(Ref(CStringConstIterator) it, int opt)
{
	COptionVector::Iterator it1 = m_definedOptions.Begin();
	COption* pOption = nullptr;
	COption* pResult = nullptr;

	switch (opt)
	{
	case 1:
		while (it1)
		{
			pOption = *it1;
			if ( pOption->matchesShort(it) )
			{
				if ( NotPtrCheck(pResult) )
					throw OK_NEW_OPERATOR COptionAmbiguityException(__FILE__LINE__ _T("Option ambiguity Exception (Option: %s)"), it.GetOrigin());
				pResult = pOption;
			}
			++it1;
		}
		break;
	case 2:
		while ( it1 )
		{
			pOption = *it1;
			if ( pOption->matchesFull(it) || pOption->matchesPartial(it) )
			{
				if ( NotPtrCheck(pResult) )
					throw OK_NEW_OPERATOR COptionAmbiguityException(__FILE__LINE__ _T("Option ambiguity Exception (Option: %s)"), it.GetOrigin());
				pResult = pOption;
			}
			++it1;
		}
		break;
	}
	if (PtrCheck(pResult))
		throw OK_NEW_OPERATOR COptionUnknownException(__FILE__LINE__ _T("Unknown Option Exception (Option: %s)"), it.GetOrigin());
	return pResult;
}

COption* CApplication::findPositionedOption(int pos)
{
	COptionVector::Iterator it = m_definedOptions.Begin();
	COption* pOption = nullptr;

	while ( it )
	{
		pOption = *it;
		if ( pOption->argumentPosition() == pos )
			return pOption;
		++it;
	}
	it = m_definedOptions.Begin();
	while ( it )
	{
		pOption = *it;
		if ( pOption->argumentPosition() < 0 )
			return pOption;
		++it;
	}
	return nullptr;
}

COption* CApplication::findOption(COption* pOption)
{
	COptionVector::Iterator it = m_Options.FindSorted(pOption);

	if (m_Options.MatchSorted(it, pOption))
		return *it;
	return nullptr;
}

void CApplication::processOption(COption* pOption, CArray args, int argc)
{
	if (pOption->hasArgument())
	{
		if (argc == 0)
			return;
	}
	else if (argc > 0)
		throw OK_NEW_OPERATOR COptionUnexpectedArgumentException(__FILE__LINE__ _T("Option has no arguments (Option: %s)"), pOption->fullname().GetString());

	COptionVector::Iterator it = m_Options.Begin();

	while (it)
	{
		COption* pOption1 = *it;

		if ((!(pOption->group().IsEmpty())) && (!(pOption1->group().IsEmpty())) && (pOption->group().Compare(pOption1->group(), 0, CStringLiteral::cIgnoreCase) == 0))
			throw OK_NEW_OPERATOR COptionGroupException(__FILE__LINE__ _T("Option with group '%s' is already defined in command line (Option: %s)"), pOption->group().GetString(), pOption->fullname().GetString());
		++it;
	}
	_processOption(pOption, args, argc);
}

void CApplication::_processOption(COption* pOption, CArray args, int argc)
{
	if (NotPtrCheck(findOption(pOption)))
	{
		if (!(pOption->repeatable()))
			throw OK_NEW_OPERATOR COptionNotRepeatableException(__FILE__LINE__ _T("Option is not repeatable (Option: %s)"), pOption->fullname().GetString());
	}
	else
	{
		if (pOption->hasArgument() && (!(pOption->repeatable())) && (argc > 1))
			throw OK_NEW_OPERATOR COptionNotRepeatableException(__FILE__LINE__ _T("Option is not repeatable (Option: %s)"), pOption->fullname().GetString());
		pOption->addRef();
		m_Options.InsertSorted(pOption);
	}
	if ( argc == 0 )
	{
		if ( pOption->callBack() )
			pOption->callBack()->invoke(pOption->fullname(), nullptr);
		else
			handleOption(pOption->fullname(), nullptr);
	}
	else
	{
		int i = 0;

		while ( m_continueOptionProcessing && (i < argc) )
		{
			if ( pOption->callBack() )
				pOption->callBack()->invoke(pOption->fullname(), args[i]);
			else
				handleOption(pOption->fullname(), args[i]);
			++i;
		}
	}
}

void CApplication::stopOptionsProcessing()
{
	m_continueOptionProcessing = false;
}

static void printUFormatted(Ref(CStringBuffer) outBuf, dword sz, ConstRef(CStringBuffer) tobeFormat)
{
	CStringConstIterator it1(tobeFormat.Begin());
	CStringConstIterator it2;
	int i;

	if ( it1.GetLength() < (79 - sz) )
	{
		outBuf << it1.GetCurrent() << _T("\n");
		return;
	}
	it2 = it1;
	it2 += 79 - sz;
	while ( (it2[0] != _T(' ')) && (it2.GetCurrent() != it2.GetOrigin()) )
		--it2;
	while ( it1 != it2 )
	{
		outBuf << it1[0];
		++it1;
	}
	outBuf << _T("\n");
	while ( !(it1.IsEnd()) )
	{
		i = sz;
		while ( i-- )
			outBuf << _T(" ");
		if ( it2.GetLength() < (79 - sz) )
		{
			outBuf << it1.GetCurrent() << _T("\n");
			return;
		}
		it2 += 79 - sz;
		while ( (it2[0] != _T(' ')) && (it2.GetCurrent() != it2.GetOrigin()) )
			--it2;
		while ( it1 != it2 )
		{
			outBuf << it1[0];
			++it1;
		}
		outBuf << _T("\n");
	}
}

void CApplication::usage(Ref(CStringBuffer) outBuf)
{
	CDataVectorT<CStringBuffer> categories(__FILE__LINE__ 16, 16);
	CDataVectorT<CStringBuffer>::Iterator it1;
	COptionVector::Iterator it;
	COption* p;
	dword sz, sz1, i;
	CStringBuffer buf;
	bool hasEmptyCategory = false;

	sz = 0;
	it = m_definedOptions.Begin();
	while ( it )
	{
		p = *it;
		sz1 = p->fullname().GetLength() + p->shortName().GetLength();
		if ( sz1 > sz )
			sz = sz1;
		buf.SetString(__FILE__LINE__ p->category());
		if ( buf.IsEmpty() )
			hasEmptyCategory = true;
		else
		{
			it1 = categories.FindSorted(buf);

			if ( !it1 || (buf.Compare(*it1, 0, CStringLiteral::cIgnoreCase) != 0) )
				categories.InsertSorted(buf);
		}
		++it;
	}
	sz += 2;
	outBuf << _T("Usage:\n\n");
	it1 = categories.Begin();
	while ( it1 || hasEmptyCategory )
	{
		if ( !hasEmptyCategory )
			outBuf << _T("  ") << *it1 << _T("\n\n");
		it = m_definedOptions.Begin();
		while ( it )
		{
			p = *it;
			buf.SetString(__FILE__LINE__ p->category());
			if ( hasEmptyCategory ? (buf.IsEmpty()) : (buf.Compare(*it1, 0, CStringLiteral::cIgnoreCase) == 0) )
			{
				sz1 = p->fullname().GetLength() + p->shortName().GetLength();
#ifdef OK_SYS_WINDOWS
				outBuf << _T("     -") << p->shortName() << _T(", /") << p->fullname();
#endif
#ifdef OK_SYS_UNIX
				outBuf << _T("     -") << p->shortName() << _T(", --") << p->fullname();
#endif
				i = sz - sz1;
				while ( i-- )
					outBuf << _T(" ");
				buf.Clear();
				if ( p->required() )
					buf += _T("This option cannot be omitted. ");
				if ( !(p->argument().IsEmpty()) )
				{
					buf += _T("This option has an argument '");
					buf += p->argument();
					buf += _T("'. ");
				}
				if ( p->argumentRequired() )
					buf += _T("The argument is not optional. ");
				if ( p->argumentPosition() != 0 )
				{
					if ( p->argumentPosition() < 0 )
						buf += _T("This argument is a default argument, when given without the option. ");
					else
					{
						CStringBuffer no;

						no.FormatString(__FILE__LINE__ _T("This argument has position %d, when given without the option. "), p->argumentPosition());
						buf += no;
					}
				}
				if ( p->repeatable() )
					buf += _T("This option can be repeated or have multiple arguments. ");
				buf += p->description();
#ifdef OK_SYS_WINDOWS
				printUFormatted(outBuf, sz + 9, buf);
#endif
#ifdef OK_SYS_UNIX
				printUFormatted(outBuf, sz + 10, buf);
#endif
			}
			++it;
		}
		outBuf << _T("\n");
		if ( hasEmptyCategory )
			hasEmptyCategory = false;
		else
			++it1;
	}
	outBuf << _T("\n");
	outBuf << m_defaultAppName << _T(" Copyright (C) 1995-2014 Oliver Kreis\n");
	outBuf << _T("This program comes with ABSOLUTELY NO WARRANTY. It is free software, and you\n");
	outBuf << _T("are welcome to redistribute it under certain conditions. See file 'LICENSE'\n");
	outBuf << _T("for details.\n");
}

CAbstractConfiguration* CApplication::loadProperties(ConstRef(CFilePath) _path)
{
	CFilePath path(_path);

	path.set_FullExtension(_T("*"));
	try
	{
		CDirectoryIterator it(path);
		CFilePath configPath;
		CStringBuffer ext;

		while ( it )
		{
			configPath = it.get_Path();
			
			ext = configPath.get_Extension();
			if ( (!(ext.IsEmpty())) && (ext.Compare(_T("properties"), 0, CStringLiteral::cIgnoreCase) == 0) )
			{
				CFileConfiguration* pConfig = OK_NEW_OPERATOR CFileConfiguration(__FILE__LINE__ configPath);

				return pConfig;
			}
			++it;
		}
	}
	catch ( CBaseException* )
	{
		throw;
	}
	return nullptr;
}

void CApplication::loadConfiguration(CConstPointer _programPath)
{
	CConfigurationList* configList = OK_NEW_OPERATOR CConfigurationList __FILE__LINE__0P;
	CAbstractConfiguration* result;
	CFilePath path(__FILE__LINE__ _programPath);

	configList->Add(OK_NEW_OPERATOR CSystemConfiguration());
	try
	{
		result = loadProperties(path);
		if ( result )
		{
			configList->Add(result);
			configList->Add(OK_NEW_OPERATOR CApplicationConfiguration(path));
		}
		else
		{
			path.set_PureBasename(m_defaultAppName);
			result = loadProperties(path);
			if ( result )
				configList->Add(result);
			configList->Add(OK_NEW_OPERATOR CApplicationConfiguration(path));
		}

		CStringBuffer home;

#ifdef OK_SYS_WINDOWS
		home = configList->GetValue(_T("System.Environment.USERPROFILE"));
#endif
#ifdef OK_SYS_UNIX
		home = configList->GetValue(_T("System.Environment.HOME"));
#endif
		if ( !(home.IsEmpty()) )
		{
			home += CDirectoryIterator::DefaultPathSeparatorString();
			home += path.get_PureBasename();
			home += _T("User.properties");
			path.set_Path(home);

			CFileConfiguration* pConfig = OK_NEW_OPERATOR CFileConfiguration(__FILE__LINE__ path, true);

			if (pConfig->Count() > 0)
				configList->Add(pConfig);
			else
				pConfig->release();
		}
	}
	catch ( CBaseException* )
	{
		throw;
	}
	m_config = configList;
}

int CApplication::run(int argc, CArray argv)
{
	int result = 4;

	try
	{
		loadConfiguration(argv[0]);
		if ( processOptions(argc, argv) )
			result = main();
	}
	catch ( COptionException* ex )
	{
		CStringBuffer outBuf;

		usage(outBuf);
		CERR << ex->GetExceptionMessage() << std::endl;
		CERR << outBuf;
		result = -4;
	}
	catch ( CBaseException* ex )
	{
		CERR << ex->GetExceptionMessage() << std::endl;
		result = -4;
	}
	catch ( ... )
	{
		CERR << _T("FATAL error") << std::endl;
		result = -4;
	}
	return result;
}

#if defined(OK_COMP_GNUC) && defined(OK_SYS_WINDOWS) && defined(_UNICODE)
int CApplication::run(int argc, char** argv)
{
    CArray argvW = CastAnyPtr(CPointer,TFalloc(argc * sizeof(CPointer)));
    int result;
    
    for ( int i = 0; i < argc; ++i )
    {
        int slen = strlen(argv[i]) + 1;
        argvW[i] = CastAnyPtr(mbchar,TFalloc(slen * sizeof(mbchar)));
        
        for ( int j = 0; j < slen; ++j )
            argvW[i][j] = argv[i][j];
    }
    result = run(argc, argvW);
    for ( int i = 0; i < argc; ++i )
        TFfree(argvW[i]);
    TFfree(argvW);
    return result;
}
#endif
