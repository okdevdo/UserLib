/******************************************************************************
    
	This file is part of WinSources, which is part of UserLib.

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

#include "WinSources.h"
#include "Application.h"

class WINSOURCES_API CConsoleApplication :	public CApplication
{
public:
	CConsoleApplication();
	CConsoleApplication(ConstRef(CStringBuffer) _defaultAppName);
	virtual ~CConsoleApplication();

	bool is_stdout_redirected();
	__inline void set_stdout_redirected(bool v) { _stdout_init = true; _stdout_redirected = v; }
	bool is_stderr_redirected();
	__inline void set_stderr_redirected(bool v) { _stderr_init = true; _stderr_redirected = v; }

protected:
	bool _stdout_init;
	bool _stdout_redirected;
	bool _stderr_init;
	bool _stderr_redirected;
};

#define theConsoleApp ((CConsoleApplication*)(CApplication::instance()))
