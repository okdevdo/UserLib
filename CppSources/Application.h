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
#pragma once

#include "CppSources.h"
#include "Option.h"
#include "DataVector.h"
#include "FilePath.h"

class CPPSOURCES_API CAbstractConfiguration;
class CPPSOURCES_API CApplication
{
public:
	CApplication();
	CApplication(ConstRef(CStringBuffer) _defaultAppName);
	virtual ~CApplication(void);

	void addOption(COption& option);
	void stopOptionsProcessing();

	virtual int run(int argc, CArray argv);
#if defined(OK_COMP_GNUC) && defined(OK_SYS_WINDOWS) && defined(_UNICODE)
	int run(int argc, char** argv);
#endif
	void usage(Ref(CStringBuffer) outBuf);

	virtual int main() = 0;
	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value) = 0;

	__inline CAbstractConfiguration* config() { return m_config; }
	__inline static CApplication* instance() { return m_instance; }
	__inline ConstRef(CStringBuffer) get_DefaultAppName() const { return m_defaultAppName; }

protected:
	WBool processOptions(int argc, CArray argv);
	WInt isOption(Ref(CStringConstIterator) it, int i);
	COption* findOption(COption* pOption);
	COption* findDefinedOption(Ref(CStringConstIterator) it, int opt);
	COption* findPositionedOption(int pos);
	void processOption(COption* pOption, CArray args, int argc);
	void _processOption(COption* pOption, CArray args, int argc);
	void loadConfiguration(CConstPointer _programPath);
	CAbstractConfiguration* loadProperties(ConstRef(CFilePath) _path);

	static CApplication* m_instance;

	typedef CDataVectorT<COption> COptionVector;

	CStringBuffer m_defaultAppName;
	COptionVector m_definedOptions;
	COptionVector m_Options;
	WBool m_continueOptionProcessing;
	CAbstractConfiguration* m_config;
};

#define theApp CApplication::instance()

