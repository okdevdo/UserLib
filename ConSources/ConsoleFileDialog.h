/******************************************************************************
    
	This file is part of ConSources, which is part of UserLib.

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

#include "FilePath.h"

#include "ConSources.h"
#include "ConsoleDialog.h"

class CConsole;
class CONSOURCES_API CConsoleFileDialog: public CConsoleDialog
{
public:
	enum FileDialogMode
	{
		FileDialogModeOpenFile,
		FileDialogModeCreateFile
	};

public:
	CConsoleFileDialog(CConstPointer name, CConsole* pConsole);
	CConsoleFileDialog(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleFileDialog(FileDialogMode mode, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleFileDialog(void);

	void Initialize(COORD _size);
	void Initialize2(COORD _size);

	void DrawFileDialog(bool highlightFirst = true);

	void DirListCallback(CConsoleControl* pControl, DWORD command);
	void FileListCallback(CConsoleControl* pControl, DWORD command);
	void FileNameCallback(CConsoleControl* pControl, DWORD command);
	void WildCardCallback(CConsoleControl* pControl, DWORD command);

	virtual void Create(COORD pos, COORD size);

protected:
	FileDialogMode m_FileDialogMode;
	CFilePath m_Path;

	void LoadConfiguration();
	void SaveConfiguration();

private:
	CConsoleFileDialog();
	CConsoleFileDialog(ConstRef(CConsoleFileDialog));

};

