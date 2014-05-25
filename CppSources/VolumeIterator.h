/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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
/**
 *  \file VolumeIterator.h
 *  \brief The runtime library version of the volume iterator iterates over the drives of the local computer.
 */
#pragma once

#include "CppSources.h"
#include "FilePath.h"

class CPPSOURCES_API CVolumeIterator
{
public:
	CVolumeIterator(void);
	~CVolumeIterator(void);

	__inline operator bool() const 
	{ 
#ifdef OK_SYS_WINDOWS
		return _curdriveno >= 0;
#endif
#ifdef OK_SYS_UNIX
		return !_bAtEnd;
#endif
	}
	__inline CVolumeIterator& operator++() { Next(); return *this; }
    __inline CFilePath operator*() { return get_Path(); }

	void Begin();
	void Next();
	void Close();

	CFilePath get_Path();

private:
#ifdef OK_SYS_WINDOWS
	unsigned long _driveno;
	int _curdriveno;
	void _next();
#endif
#ifdef OK_SYS_UNIX
	bool _bAtEnd;
#endif
};

