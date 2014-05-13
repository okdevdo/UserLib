/******************************************************************************
    
	This file is part of XHttpWorker, which is part of UserLib.

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
// XHttpWorker.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "ServiceApplication.h"
#include "EventLogger.h"

void __stdcall VectorEmptyDeleteFunc(ConstPointer data, Pointer context)
{
}

class XHttpWorkerApplication : public CServiceApplication
{
public:
	XHttpWorkerApplication() :
		CServiceApplication(_T("XHttpWorker")),
		m_help(false),
		m_AnyOption(false),
		m_bQueueName(false),
		m_sQueueName(),
		m_bLogging(false)
	{
		COptionCallback<XHttpWorkerApplication> cb(this, &XHttpWorkerApplication::handleHelp);

		addOption(COption(_T("Help"))
			.shortName(_T("h"))
			.description(_T("Show Help."))
			.callBack(cb));
		addOption(COption(_T("QueueName"))
			.shortName(_T("n"))
			.description(_T("Queue name."))
			.argument(_T("name"), true)
			.required(true));
		addOption(COption(_T("Logging"))
			.shortName(_T("l"))
			.description(_T("Switch on logging.")));
	}
	~XHttpWorkerApplication()
	{
	}

	void handleHelp(const CStringLiteral& name, const CStringLiteral& value)
	{
		m_help = true;
		stopOptionsProcessing();
	}

	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value)
	{
		if (name == CStringLiteral(_T("QueueName")))
		{
			m_bQueueName = true;
			m_sQueueName = value;
			m_AnyOption = true;
		}
		if (name == CStringLiteral(_T("Logging")))
		{
			m_bLogging = true;
			m_AnyOption = true;
		}
	}

	virtual int main()
	{
		if (m_help || (!m_AnyOption))
		{
			CStringBuffer tmp;

			usage(tmp);
			CEventLogger::WriteLog(CEventLogger::Information, tmp);
			return 0;
		}
		if (m_bQueueName)
			RunClient(m_sQueueName, m_bLogging);
		return 0;
	}

protected:
	WBool m_help;
	WBool m_AnyOption;
	WBool m_bQueueName;
	CStringLiteral m_sQueueName;
	WBool m_bLogging;
};

int
#ifdef OK_COMP_MSC
_tmain(int argc, CPointer *argv)
#else
main(int argc, char** argv)
#endif
{
	int result;
	XHttpWorkerApplication app;

	result = app.run(argc, argv);
	CEventLogger::CleanUp();
	return result;
}

