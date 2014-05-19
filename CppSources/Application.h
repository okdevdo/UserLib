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
/**
 *  \file Application.h
 *  \brief Singleton class, responsible for handling of command line parameters and supplying access to application's properties.
 */
#pragma once

#include "CppSources.h"
#include "Option.h"
#include "DataVector.h"
#include "FilePath.h"

class CPPSOURCES_API CAbstractConfiguration;

/**
 *  \class CApplication
 *  \brief Singleton class, responsible for handling of command line parameters and supplying access to application's properties.
 */
class CPPSOURCES_API CApplication
{
public:
	/**
	 *  \brief Standard constructor.
	 *  
	 *  \details Initializes the Application object.
	 */
	CApplication();
	/**
	 *  \brief Constructor. Takes the default application name as an argument.
	 *  
	 *  \param [in] _defaultAppName, default application name
	 *  
	 *  \details Initializes the Application object.
	 */
	CApplication(ConstRef(CStringBuffer) _defaultAppName);
	/**
	 *  \brief Destructor.
	 *  
	 *  \details Releases allocated memory.
	 */
	virtual ~CApplication(void);
	
	/**
	 *  \brief Add an program option.
	 *  
	 *  \param [in] option, a newly defined program option
	 *  \return void.
	 *  
	 *  \details Add an option to the list of defined options.
	 *  The command line arguments will be parsed, whether they 
	 *  match an option in the list of defined options.
	 */
	void addOption(COption& option);
	/**
	 *  \brief stop option processing.
	 *  
	 *  \return void.
	 *  
	 *  \details stop option processing whenever necessary, 
	 *  for example the help option was detected on the command line.
	 */
	void stopOptionsProcessing();

	/**
	 *  \brief main 'event' loop.
	 *  
	 *  \param [in] argc command line argument count.
	 *  \param [in] argv command line arguments.
	 *  \return int, result code of the application.
	 *  
	 *  \details Call this function in 'main' with that function's arguments.
	 */
	virtual int run(int argc, CArray argv);
#if defined(OK_COMP_GNUC) && defined(OK_SYS_WINDOWS) && defined(_UNICODE)
	int run(int argc, char** argv);
#endif
	/**
	 *  \brief print usage.
	 *  
	 *  \param [out] outBuf, functions output.
	 *  \return void.
	 *  
	 *  \details print the list of defined options in a formatted manner.
	 */
	void usage(Ref(CStringBuffer) outBuf);

	/**
	 *  \brief Main pure virtual overload. Main functionality of the application.
	 *  
	 *  \return int, application result code.
	 *  
	 *  \details Overwrite this function to implement the main functions of the application.
	 */
	virtual int main() = 0;
	/**
	 *  \brief Pure virtual overload. Handles all parsed options.
	 *  
	 *  \param [in] name, 'long name' of the option.
	 *  \param [in] value value, if any, given on the command line with that option.
	 *  \return void.
	 *  
	 *  \details This function will be called whenever an option was parsed on the command line.
	 *  Overwrite this function to implement handling of command line arguments.
	 */
	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value) = 0;

	/**
	 *  \brief Implements access to system's and application's properties.
	 *  
	 *  \return Ptr(CAbstractConfiguration), see description there
	 *  
	 *  \details Implements access to system's and application's properties. 
	 *  See class 'CAbstractConfiguration' for details.
	 */
	__inline CAbstractConfiguration* config() { return m_config; }
	/**
	 *  \brief Singleton access.
	 *  
	 *  \return Ptr(CApplication), the application instance.
	 *  
	 *  \details Call the macro 'theApp' to have access to the singleton.
	 */
	__inline static CApplication* instance() { return m_instance; }
	/**
	 *  \brief Accessor Default Application Name.
	 *  
	 *  \return ConstRef(CStringBuffer), the default application name.
	 *  
	 *  \details Access to the Default Application Name.
	 */
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

