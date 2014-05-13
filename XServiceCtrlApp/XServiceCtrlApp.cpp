/******************************************************************************
    
	This file is part of XServiceCtrlApp, which is part of UserLib.

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
// XServiceCtrlApp.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "Application.h"
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
#include <conio.h>
#endif
#endif

void __stdcall VectorEmptyDeleteFunc(ConstPointer data, Pointer context)
{
}

class XServiceCtrlAppApplication : public CApplication
{
public:
	XServiceCtrlAppApplication() :
		CApplication(CStringBuffer(__FILE__LINE__ _T("XServiceCtrlApp"))),
		m_help(false),
		m_AnyOption(false),
		m_bEnumAll(false),
		m_bStartService(false),
		m_sStartServices(__FILE__LINE__ 16, 16, VectorEmptyDeleteFunc),
		m_bStopService(false),
		m_sStopServices(__FILE__LINE__ 16, 16, VectorEmptyDeleteFunc),
		m_bEnableService(false),
		m_sEnableServices(__FILE__LINE__ 16, 16, VectorEmptyDeleteFunc),
		m_bDisableService(false),
		m_sDisableServices(__FILE__LINE__ 16, 16, VectorEmptyDeleteFunc),
		m_bDeleteService(false),
		m_sDeleteServices(__FILE__LINE__ 16, 16, VectorEmptyDeleteFunc)
	{
		COptionCallback<XServiceCtrlAppApplication> cb(this, &XServiceCtrlAppApplication::handleHelp);

		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("EnumAll"))
			.shortName(_T("a"))
			.category(_T("XServiceCtrlApp"))
			.description(_T("Enum all services.")));
		addOption(COption(_T("Start"))
			.shortName(_T("r"))
			.repeatable(true)
			.argument(_T("name"), true)
			.category(_T("XServiceCtrlApp"))
			.description(_T("Start service.")));
		addOption(COption(_T("Stop"))
			.shortName(_T("p"))
			.repeatable(true)
			.argument(_T("name"), true)
			.category(_T("XServiceCtrlApp"))
			.description(_T("Stop service.")));
		addOption(COption(_T("Enable"))
			.shortName(_T("e"))
			.repeatable(true)
			.argument(_T("name"), true)
			.category(_T("XServiceCtrlApp"))
			.description(_T("Enable service.")));
		addOption(COption(_T("Disable"))
			.shortName(_T("d"))
			.repeatable(true)
			.argument(_T("name"), true)
			.category(_T("XServiceCtrlApp"))
			.description(_T("Disable service.")));
		addOption(COption(_T("Delete"))
			.shortName(_T("l"))
			.repeatable(true)
			.argument(_T("name"), true)
			.category(_T("XServiceCtrlApp"))
			.description(_T("Delete service.")));
	}
	~XServiceCtrlAppApplication()
	{
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral& value)
	{
		m_help = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		if (name == CStringLiteral(_T("EnumAll")))
		{
			m_bEnumAll = true;
			m_AnyOption = true;
		}
		if (name == CStringLiteral(_T("Start")))
		{
			m_bStartService = true;
			m_AnyOption = true;
			m_sStartServices.Append(value);
		}
		if (name == CStringLiteral(_T("Stop")))
		{
			m_bStopService = true;
			m_AnyOption = true;
			m_sStopServices.Append(value);
		}
		if (name == CStringLiteral(_T("Enable")))
		{
			m_bEnableService = true;
			m_AnyOption = true;
			m_sEnableServices.Append(value);
		}
		if (name == CStringLiteral(_T("Disable")))
		{
			m_bDisableService = true;
			m_AnyOption = true;
			m_sDisableServices.Append(value);
		}
		if (name == CStringLiteral(_T("Delete")))
		{
			m_bDeleteService = true;
			m_AnyOption = true;
			m_sDeleteServices.Append(value);
		}
	}

	virtual int main()
	{
		if (m_help || (!m_AnyOption) )
		{
			CStringBuffer tmp;

			usage(tmp);
			CERR << tmp;
			return 0;
		}
		if (m_bEnumAll)
			okEnumAllServices();
		if (m_bStartService)
			okStartServices(m_sStartServices);
		if (m_bStopService)
			okStopServices(m_sStopServices);
		if (m_bEnableService)
			okEnableServices(m_sEnableServices);
		if (m_bDisableService)
			okDisableServices(m_sDisableServices);
		if (m_bDeleteService)
			okDeleteServices(m_sDeleteServices);
		return 0;
	}

protected:
	WBool m_help;
	WBool m_AnyOption;
	WBool m_bEnumAll;
	WBool m_bStartService;
	CDataVectorT<mbchar> m_sStartServices;
	WBool m_bStopService;
	CDataVectorT<mbchar> m_sStopServices;
	WBool m_bEnableService;
	CDataVectorT<mbchar> m_sEnableServices;
	WBool m_bDisableService;
	CDataVectorT<mbchar> m_sDisableServices;
	WBool m_bDeleteService;
	CDataVectorT<mbchar> m_sDeleteServices;
};

int
#ifdef OK_COMP_MSC
_tmain(int argc, CPointer *argv)
#else
main(int argc, char** argv)
#endif
{
	int result;
	XServiceCtrlAppApplication app;

	result = app.run(argc, argv);
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
	_getch();
#endif
#endif
	return result;
}

