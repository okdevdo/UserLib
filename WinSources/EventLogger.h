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
#include "WinException.h"

class WINSOURCES_LOCAL CEventLoggerImpl;
class WINSOURCES_API CEventLogger : public CCppObject
{
public:
	enum InfoLevel {
		Information,
		Warning,
		Error
	};

	enum Category
	{
		NetworkCategory,
		DatabaseCategory,
		GUICategory
	};

	CEventLogger();
	virtual ~CEventLogger();

	static void WriteLog(InfoLevel level, CConstPointer msgtext);
	static void WriteFormattedLog(InfoLevel level, CConstPointer _format, ...);
	static void WriteClassicLog(InfoLevel level, Category category, CConstPointer msgtext);
	static void WriteFormattedClassicLog(InfoLevel level, Category category, CConstPointer _format, ...);
	static void CleanUp();

protected:
	static Ptr(CEventLoggerImpl) _impl;
};

DECL_WINEXCEPTION(WINSOURCES_API, CEventLoggerException, CWinException)