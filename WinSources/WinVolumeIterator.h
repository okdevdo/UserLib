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
#include "DataVector.h"
#include "FilePath.h"

class WINSOURCES_API CWinVolumeIterator
{
public:
	CWinVolumeIterator(void);
	~CWinVolumeIterator(void);

	__inline operator bool() const { return (_fh != INVALID_HANDLE_VALUE); }
	__inline CWinVolumeIterator& operator++() { Next(); return *this; }

	void Begin();
	void Next();
	void Close();

	__inline ConstRef(CStringBuffer) get_DeviceName() const { return _deviceName; }
	__inline ConstRef(CStringBuffer) get_VolumeName() const { return _volumeName; }
	__inline ConstRef(CDataVectorT<CStringBuffer>) get_PathNames() const { return _pathNames; }

private:
	HANDLE _fh;
	CStringBuffer _volumeName;
	CStringBuffer _deviceName;
	CDataVectorT<CStringBuffer> _pathNames;

	void _get_DeviceName();
	void _get_PathNames();
};

DECL_WINEXCEPTION(WINSOURCES_API, CWinVolumeIteratorException, CWinException)
