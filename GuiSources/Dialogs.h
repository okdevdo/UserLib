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

#include "afxwin.h"

class CPropertySheet;
class CConfigDialog: public CDialog
{
public:
	CConfigDialog(LPCTSTR name = NULL, LPCTSTR resID = NULL, CWin* pParent = NULL);
	CConfigDialog(ConstRef(CStringBuffer) name, LPCTSTR resID = NULL, CWin* pParent = NULL);
	virtual ~CConfigDialog();

	LRESULT OnEraseBackGround(WPARAM wParam, LPARAM lParam);
	LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	LRESULT OnOKCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCancelCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void FillFarben();
	void SaveFarben();
	void FillFonts();
	void SaveFonts();

	CPropertySheet* m_pSheetFarben;
	CPropertySheet* m_pSheetSchriftarten;
};



