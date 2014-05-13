/******************************************************************************
    
	This file is part of GuiSources, which is part of UserLib.

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

// Durch Einbeziehen von"SDKDDKVer.h" wird die h�chste verf�gbare Windows-Plattform definiert.

// Wenn Sie die Anwendung f�r eine fr�here Windows-Plattform erstellen m�chten, schlie�en Sie "WinSDKVer.h" ein, und
// legen Sie das _WIN32_WINNT-Makro auf die zu unterst�tzende Plattform fest, bevor Sie "SDKDDKVer.h" einschlie�en.

#include <winsdkver.h>
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#include <SDKDDKVer.h>
