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
#include "WS_PCH.H"
#include "Registry.h"

CRegistry::CRegistry(void):
	_rootKey(NULL), _applicationKey(NULL), _dataKey(NULL)
{
}

CRegistry::~CRegistry(void)
{
	Close();
}

BOOL CRegistry::OpenCurrentUser()
{
	Close();

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
		_rootKey = HKEY_CURRENT_USER;
#endif
#endif
#ifdef OK_COMP_MSC
	LONG ret = ERROR_SUCCESS;

	if ((ret = RegOpenCurrentUser(KEY_ALL_ACCESS, &_rootKey)) != ERROR_SUCCESS)
	{
		SetLastError(ret);
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CRegistry::OpenLocalMachine()
{
	Close();

	_rootKey = HKEY_LOCAL_MACHINE;
	return TRUE;
}

BOOL CRegistry::OpenApplication()
{
	Close();

	if (!OpenCurrentUser())
		return FALSE;
	if (_rootKey)
	{
		DWORD disposition = 0;
		LONG ret = ERROR_SUCCESS;

		if ((ret = RegCreateKeyEx(
			_rootKey,
			TEXT("Software\\Oliver Kreis\\UserLib"),
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&_applicationKey,
			&disposition)) != ERROR_SUCCESS)
		{
			SetLastError(ret);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CRegistry::OpenDataKey(LPCTSTR key)
{
	if (!_rootKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	CloseDataKey();

	DWORD disposition = 0;
	LONG ret = 0;

	if ((ret = RegCreateKeyEx(
		_applicationKey?_applicationKey:_rootKey,
		key,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		//		KEY_ALL_ACCESS,
		//		KEY_QUERY_VALUE | STANDARD_RIGHTS_READ,
		KEY_READ,
		NULL,
		&_dataKey,
		&disposition)) != ERROR_SUCCESS)
	{
		SetLastError(ret);
		return FALSE;
	}
	return TRUE;
}

BOOL CRegistry::QueryKeys(DWORD ix, LPTSTR* keyName, DWORD* keyNameLen)
{
	if (!_rootKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	LONG ret = 0;
	DWORD vKeyLen = 0;

	while (true)
	{
		vKeyLen = *keyNameLen;
		if ((ret = RegEnumKeyEx(_dataKey ? _dataKey : (_applicationKey ? _applicationKey: _rootKey),
			ix,
			*keyName,
			&vKeyLen,
			NULL,
			NULL,
			NULL,
			NULL)) != ERROR_SUCCESS)
		{
			switch (ret)
			{
			case ERROR_MORE_DATA:
				if (vKeyLen == 0L)
					*keyNameLen = 128;
				else
					*keyNameLen += 128;
				if (*keyName == NULL)
					*keyName = (LPTSTR)TFalloc(*keyNameLen);
				else
					*keyName = (LPTSTR)TFrealloc(*keyName, *keyNameLen);
				break;
			case ERROR_NO_MORE_ITEMS:
				if (*keyName != NULL)
					TFfree(*keyName);
				*keyName = NULL;
				*keyNameLen = 0;
				SetLastError(ERROR_SUCCESS);
				return FALSE;
			default:
				SetLastError(ret);
				return FALSE;
			}
		}
		else
			break;
	}
	return TRUE;
}

BOOL CRegistry::QueryValues(DWORD ix, LPTSTR* valueName, DWORD* valueNameLen)
{
	if (!_rootKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	LONG ret = 0;
	DWORD vValueLen = 0;

	while (true)
	{
		vValueLen = *valueNameLen;
		if ((ret = RegEnumValue(_dataKey ? _dataKey : (_applicationKey ? _applicationKey : _rootKey),
			ix,
			*valueName,
			&vValueLen,
			NULL,
			NULL,
			NULL,
			NULL)) != ERROR_SUCCESS)
		{
			switch (ret)
			{
			case ERROR_MORE_DATA:
				if (vValueLen == 0L)
					*valueNameLen = 128;
				else
					*valueNameLen += 128;
				if (*valueName == NULL)
					*valueName = (LPTSTR)TFalloc(*valueNameLen);
				else
					*valueName = (LPTSTR)TFrealloc(*valueName, *valueNameLen);
				break;
			case ERROR_NO_MORE_ITEMS:
				if (*valueName != NULL)
					TFfree(*valueName);
				*valueName = NULL;
				*valueNameLen = 0;
				SetLastError(ERROR_SUCCESS);
				return FALSE;
			default:
				SetLastError(ret);
				return FALSE;
			}
		}
		else
			break;
	}
	return TRUE;
}

BOOL CRegistry::GetValueSize(LPCTSTR valueName, LPDWORD pSize)
{
	if (!_dataKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	LONG ret = ERROR_SUCCESS;

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
    if ( (ret = RegQueryValueEx(
            _dataKey,
            valueName, 
            NULL,
            NULL,
            NULL,
            pSize)) != ERROR_SUCCESS )
#endif
#endif
#ifdef OK_COMP_MSC
	if ((ret = RegGetValue(
		_dataKey,
		NULL,
		valueName,
		RRF_RT_ANY,
		NULL,
		NULL,
		pSize)) != ERROR_SUCCESS) // size in bytes
#endif
	{
		SetLastError(ret);
		return FALSE;
	}
	return TRUE;
}

BOOL CRegistry::GetValue(LPCTSTR valueName, LPTSTR value)
{
	if (!_dataKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	DWORD size = 0;
	LONG ret = ERROR_SUCCESS;

	if (!GetValueSize(valueName, &size))
		return FALSE;
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	DWORD type;

	if ( (ret = RegQueryValueEx(
		_dataKey,
		valueName, 
		NULL,
		&type,
		(LPBYTE)value,
		&size)) != ERROR_SUCCESS )
	{
		SetLastError(ret);
		return FALSE;
	}
    if ( type != REG_SZ )
	{
		SetLastError(ERROR_INVALID_DATATYPE);
		return FALSE;
	}
#endif
#endif
#ifdef OK_COMP_MSC
	if ( (ret = RegGetValue(
			_dataKey,
			NULL,
			valueName,
			RRF_RT_REG_SZ,
			NULL,
			(PVOID)value,
			&size)) != ERROR_SUCCESS )
	{
		SetLastError(ret);
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CRegistry::GetValue(LPCTSTR valueName, LPDWORD value)
{
	if (!_dataKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	DWORD size = sizeof(DWORD);
	LONG ret = ERROR_SUCCESS;

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
        DWORD type;
        
		if ( (ret = RegQueryValueEx(
			_dataKey,
			valueName, 
			NULL,
			&type,
			(LPBYTE)value,
			&size)) != ERROR_SUCCESS )
		{
			SetLastError(ret);
            return FALSE;
		}
        if ( type != REG_DWORD )
		{
			SetLastError(ERROR_INVALID_DATATYPE);
			return FALSE;
		}
#endif
#endif
#ifdef OK_COMP_MSC
	if ( (ret = RegGetValue(
			_dataKey,
			NULL,
			valueName,
			RRF_RT_DWORD,
			NULL,
			(PVOID)value,
			&size)) != ERROR_SUCCESS )
		{
			SetLastError(ret);
            return FALSE;
		}
#endif
	return TRUE;
}

BOOL CRegistry::GetValue(LPCTSTR valueName, LPBYTE value, DWORD size)
{
	if (!_dataKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	LONG ret = ERROR_SUCCESS;

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
        DWORD type;
        
        if ( (ret = RegQueryValueEx(
                _dataKey,
                valueName, 
                NULL,
                &type,
                value,
                &size)) != ERROR_SUCCESS )
		{
			SetLastError(ret);
			return FALSE;
		}
		if (type != REG_BINARY)
		{
			SetLastError(ERROR_INVALID_DATATYPE);
			return FALSE;
		}
#endif
#endif
#ifdef OK_COMP_MSC
	if ( (ret = RegGetValue(
			_dataKey,
			NULL,
			valueName,
			RRF_RT_REG_BINARY,
			NULL,
			(PVOID)value,
			&size)) != ERROR_SUCCESS )
		{
			SetLastError(ret);
			return FALSE;
		}
#endif
	return TRUE;
}

BOOL CRegistry::SetValue(LPCTSTR valueName, LPCTSTR value)
{
	if (!_dataKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	dword len = s_strlen(CastMutable(LPTSTR, value), INT_MAX);
	LONG ret = ERROR_SUCCESS;

	if ((ret = RegSetValueEx(
		_dataKey,
		valueName,
		0,
		REG_SZ,
		(const BYTE *)value,
		(len + 1) * sizeof(TCHAR))) != ERROR_SUCCESS)
	{
		SetLastError(ret);
		return FALSE;
	}
	return TRUE;
}

BOOL CRegistry::SetValue(LPCTSTR valueName, DWORD value)
{
	if (!_dataKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	LONG ret = ERROR_SUCCESS;

	if ((ret = RegSetValueEx(
		_dataKey,
		valueName,
		0,
		REG_DWORD,
		(const BYTE *)value,
		sizeof(DWORD))) != ERROR_SUCCESS)
	{
		SetLastError(ret);
		return FALSE;
	}
	return TRUE;
}

BOOL CRegistry::SetValue(LPCTSTR valueName, LPBYTE value, DWORD size)
{
	if (!_dataKey)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	LONG ret = ERROR_SUCCESS;

	if ((ret = RegSetValueEx(
		_dataKey,
		valueName,
		0,
		REG_BINARY,
		(const BYTE *)value,
		size)) != ERROR_SUCCESS)
	{
		SetLastError(ret);
		return FALSE;
	}
	return TRUE;
}

void CRegistry::CloseDataKey()
{
	if ( _dataKey )
	{
		RegCloseKey(_dataKey);
		_dataKey = NULL;
	}
}

void CRegistry::Close()
{
	CloseDataKey();
	if ( _applicationKey )
	{
		RegCloseKey(_applicationKey);
		_applicationKey = NULL;
	}
	if (_rootKey && (_rootKey != HKEY_LOCAL_MACHINE) && (_rootKey != HKEY_CURRENT_USER))
	{
		RegCloseKey(_rootKey);
		_rootKey = NULL;
	}
}
