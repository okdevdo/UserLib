/******************************************************************************
    
	This file is part of ConSources, which is part of UserLib.

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

#include "ConSources.h"
#include "ConsoleControl.h"

class CConsole;
class CONSOURCES_API CConsoleLabelControl:	public CConsoleControl
{
public:
	CConsoleLabelControl(CConstPointer name, CConsole* pConsole);
	CConsoleLabelControl(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleLabelControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleLabelControl(void);

	void DrawLabel();

	virtual void Create(COORD pos, COORD size);

private:
	CConsoleLabelControl();
	CConsoleLabelControl(ConstRef(CConsoleLabelControl));
};

