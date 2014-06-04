/******************************************************************************
    
	This file is part of XService, which is part of UserLib.

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
// XService.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "ServiceApplication.h"
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
#include <conio.h>
#endif
#endif

class XServiceApplication : public CServiceApplication
{
public:
	XServiceApplication() :
		CServiceApplication(_T("XService")),
		m_help(false),
		m_bInstallService(false),
		m_bStartService(false),
		m_sStartServices(__FILE__LINE__ 16, 16)
	{
		COptionCallback<XServiceApplication> cb(this, &XServiceApplication::handleHelp);

		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("Install"))
			.shortName(_T("i"))
			.category(_T("XService"))
			.description(_T("Install mode.")));
		addOption(COption(_T("Start"))
			.shortName(_T("s"))
			.repeatable(true)
			.argument(_T("name"), true)
			.category(_T("XService"))
			.description(_T("Start mode.")));
	}
	~XServiceApplication()
	{
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral& value)
	{
		m_help = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		if (name == CStringLiteral(_T("Install")))
			m_bInstallService = true;
		if (name == CStringLiteral(_T("Start")))
		{
			m_bStartService = true;
			m_sStartServices.Append(value);
		}
	}

	virtual int main()
	{
		if (m_help)
		{
			CStringBuffer tmp;

			usage(tmp);
			if (theServiceApp->is_service())
			{
				CEventLogger::WriteLog(CEventLogger::Information, tmp);
				CEventLogger::CleanUp();
			}
			else
			{
				CByteBuffer buf;

				tmp.convertToByteBuffer(buf);
				WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf.get_Buffer(), buf.get_BufferSize(), nullptr, nullptr);
			}
			return 0;
		}
		if (m_bInstallService)
		{
			okInstallServices();
#ifdef OK_SYS_WINDOWS
#ifdef __DEBUG__
			_getch();
#endif
#endif
			return 0;
		}
		okStartServices(m_sStartServices);
		return 0;
	}

protected:
	WBool m_help;
	WBool m_bInstallService;
	WBool m_bStartService;
	TMBCharList m_sStartServices;
};

int
#ifdef OK_COMP_MSC
_tmain(int argc, CPointer *argv)
#else
main(int argc, char** argv)
#endif
{
	int result;
	XServiceApplication app;

	result = app.run(argc, argv);
	return result;
}

