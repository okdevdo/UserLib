/******************************************************************************
    
	This file is part of XCygwinSetup, which is part of UserLib.

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
#pragma once

#include "DataBTree.h"
#include "PackageInfo.h"

class CPackageInfoBTree: public CDataBTreeT<CPackageInfo, CStringByNameLessFunctor<CPackageInfo> >
{
	typedef CDataBTreeT<CPackageInfo, CStringByNameLessFunctor<CPackageInfo> > super;

public:
	CPackageInfoBTree(DECL_FILE_LINE TListCnt maxEntriesPerNode);
	~CPackageInfoBTree();

	void Fill(ConstRef(CStringBuffer) inputfilepath);
};

