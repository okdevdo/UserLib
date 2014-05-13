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
#include "WinVolumeIterator.h"

IMPL_WINEXCEPTION(CWinVolumeIteratorException, CWinException)

CWinVolumeIterator::CWinVolumeIterator(void):
	_fh(INVALID_HANDLE_VALUE),
	_volumeName(),
	_deviceName(),
	_pathNames(__FILE__LINE__ 16, 16)
{
    Begin();
}

CWinVolumeIterator::~CWinVolumeIterator(void)
{
    Close();
}

void CWinVolumeIterator::Begin()
{
	if (_fh != INVALID_HANDLE_VALUE)
		Close();
	_volumeName.SetSize(__FILE__LINE__ MAX_PATH + 1);

	_fh = FindFirstVolume(CastMutable(CPointer, _volumeName.GetString()), MAX_PATH);
	if (_fh == INVALID_HANDLE_VALUE)
		throw OK_NEW_OPERATOR CWinVolumeIteratorException(__FILE__LINE__ _T("in %s CWinVolumeIteratorException"), _T("Begin"), CWinException::WinExtError);
	_get_DeviceName();
	_get_PathNames();
}

void CWinVolumeIterator::Next()
{
	if (_fh == INVALID_HANDLE_VALUE)
		return;

	if (!FindNextVolumeW(_fh, CastMutable(CPointer, _volumeName.GetString()), MAX_PATH))
	{
		if (GetLastError() != ERROR_NO_MORE_FILES)
			throw OK_NEW_OPERATOR CWinVolumeIteratorException(__FILE__LINE__ _T("in %s CWinVolumeIteratorException"), _T("Next"), CWinException::WinExtError);
		Close();
		return;
	}
	_get_DeviceName();
	_get_PathNames();
}

void CWinVolumeIterator::Close()
{
	FindVolumeClose(_fh);
	_fh = INVALID_HANDLE_VALUE;
	_volumeName.Clear();
	_deviceName.Clear();
	_pathNames.Close();
}

void CWinVolumeIterator::_get_DeviceName()
{
	if (_fh == INVALID_HANDLE_VALUE)
		return;

	CStringBuffer tmp(_volumeName);

	if (tmp.IsEmpty())
		return;

	dword tmplen = tmp.GetLength() - 1;
	CStringConstIterator it(tmp);

	if ((it[0] != _T('\\')) ||
		(it[1] != _T('\\')) ||
		(it[2] != _T('?')) ||
		(it[3] != _T('\\')) ||
		(it[tmplen] != _T('\\')))
		throw OK_NEW_OPERATOR CWinVolumeIteratorException(__FILE__LINE__ _T("in %s CWinVolumeIteratorException"), _T("_get_DeviceName"), CWinException::WinExtError, ERROR_BAD_PATHNAME);

	tmp.DeleteString(tmplen);
	tmp.DeleteString(0, 4);
	_deviceName.SetSize(__FILE__LINE__ MAX_PATH + 1);
	if (QueryDosDevice(tmp.GetString(), CastMutable(CPointer, _deviceName.GetString()), MAX_PATH) == 0)
		throw OK_NEW_OPERATOR CWinVolumeIteratorException(__FILE__LINE__ _T("in %s CWinVolumeIteratorException"), _T("_get_DeviceName"), CWinException::WinExtError);
}

void CWinVolumeIterator::_get_PathNames()
{
	_pathNames.Close();
	if (_fh == INVALID_HANDLE_VALUE)
		return;

	if (_volumeName.IsEmpty())
		return;

	DWORD  CharCount = MAX_PATH + 1;
	CStringBuffer tmp(__FILE__LINE__ CharCount);

	for (;;)
	{
		if (GetVolumePathNamesForVolumeName(_volumeName.GetString(), CastMutable(CPointer, tmp.GetString()), CharCount, &CharCount))
			break;
		if (GetLastError() != ERROR_MORE_DATA)
			throw OK_NEW_OPERATOR CWinVolumeIteratorException(__FILE__LINE__ _T("in %s CWinVolumeIteratorException"), _T("_get_PathNames"), CWinException::WinExtError);
		tmp.SetSize(__FILE__LINE__ CharCount);
	}

	if (!(tmp.IsEmpty()))
	{
		CStringConstIterator it(tmp);

		_pathNames.Open(__FILE__LINE__ 16, 16);

		while (it[0] != 0)
		{
			CStringBuffer tmp1(__FILE__LINE__ it);

			_pathNames.Append(tmp1);
			it += it.GetLength() + 1;
		}
	}
}
