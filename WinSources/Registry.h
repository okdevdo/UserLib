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

class WINSOURCES_API CRegistry: public CCppObject
{
public:
	CRegistry(void);
	virtual ~CRegistry(void);

	BOOL OpenCurrentUser();
	BOOL OpenLocalMachine();
	BOOL OpenApplication();
	BOOL OpenDataKey(LPCTSTR key);
	BOOL QueryKeys(DWORD ix, LPTSTR* keyName, DWORD* keyNameLen);
	BOOL QueryValues(DWORD ix, LPTSTR* valueName, DWORD* valueNameLen);
	BOOL GetValueSize(LPCTSTR valueName, LPDWORD pSize);
	BOOL GetValue(LPCTSTR valueName, LPTSTR value);
	BOOL GetValue(LPCTSTR valueName, LPDWORD value);
	BOOL GetValue(LPCTSTR valueName, LPBYTE value, DWORD size);
	BOOL SetValue(LPCTSTR valueName, LPCTSTR value);
	BOOL SetValue(LPCTSTR valueName, DWORD value);
	BOOL SetValue(LPCTSTR valueName, LPBYTE value, DWORD size);
	void CloseDataKey();
	void Close();

private:
	HKEY _rootKey;
	HKEY _applicationKey;
	HKEY _dataKey;
};

