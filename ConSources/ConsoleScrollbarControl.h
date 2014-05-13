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

#include "ConSources.h"
#include "ConsoleControl.h"

class CONSOURCES_API CConsoleScrollbarControl: public CConsoleControl
{
public: 
	enum ScrollbarDirection
	{
		ScrollbarDirectionVertical,
		ScrollbarDirectionHorizontal
	};

public:
	CConsoleScrollbarControl(CConstPointer name, CConsole* pConsole);
	CConsoleScrollbarControl(CConstPointer name, CConstPointer title, CConsole* pConsole);
	CConsoleScrollbarControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleScrollbarControl(void);

	__inline sword GetMinimum() { return m_Minimum; }
	__inline void SetMinimum(sword min) { m_Minimum = min; }
	__inline sword GetMaximum() { return m_Maximum; }
	__inline void SetMaximum(sword max) { m_Maximum = max; }
	__inline sword GetCurrent() { return m_Current; }
	__inline void SetCurrent(sword cur) { m_Current = cur; }
	__inline sword GetLineStep() { return m_LineStep; }
	__inline void SetLineStep(sword step) { m_LineStep = step; }
	__inline sword GetPageStep() { return m_PageStep; }
	__inline void SetPageStep(sword step) { m_PageStep = step; }

	void DrawScrollbarControl();

	virtual void Create(COORD pos, COORD size);

protected:
	ScrollbarDirection m_Direction;
	sword m_Minimum;
	sword m_Maximum;
	sword m_Current;
	sword m_LineStep;
	sword m_PageStep;

private:
	CConsoleScrollbarControl();
	CConsoleScrollbarControl(ConstRef(CConsoleScrollbarControl));
};

