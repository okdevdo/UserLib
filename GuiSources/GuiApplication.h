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

#include "Application.h"

class CResourceManager;
class CFontManager;
class CBrushManager;
class CWin;
class CFrameWnd;
class CMDIFrame;
class CGuiApplication: public CApplication
{
public:
	CGuiApplication(LPCTSTR _defaultAppName);
	CGuiApplication(ConstRef(CStringBuffer) _defaultAppName);
	virtual ~CGuiApplication(void);

	int run(HINSTANCE hGuiInstance, int nCmdShow);

	virtual int main() = 0;
	virtual void handleOption(const CStringLiteral& name, const CStringLiteral &value) = 0;

	__inline HINSTANCE get_GuiInstance() { return m_guiinstance; }
	__inline CWin* get_MainWnd() { return m_mainwnd; }
	__inline int get_CmdShow() { return m_cmdshow; }

	__inline Ptr(CResourceManager) get_ResourceManager() { return m_rsrcman; }
	__inline Ptr(CFontManager) get_FontManager() { return m_fontman; }
	__inline Ptr(CBrushManager) get_BrushManager() { return m_brushman; }

#ifdef __DEBUG__
	__inline bool is_Debug() { return m_isDebug; }
	__inline void set_Debug(bool isDebug) { m_isDebug = isDebug; }
	__inline int get_DebugID() { return m_debugID++; }
	bool DebugString(const char* format, ...);
#endif

protected:
	int do_messageloop(CFrameWnd* win);
	int do_messageloop(CMDIFrame* win);

	HINSTANCE m_guiinstance;
	CWin* m_mainwnd;
	int m_cmdshow;
	Ptr(CResourceManager) m_rsrcman;
	Ptr(CFontManager) m_fontman;
	Ptr(CBrushManager) m_brushman;
#ifdef __DEBUG__
	bool m_isDebug;
	int m_debugID; 
#endif
};

#define theGuiApp ((CGuiApplication*)(CApplication::instance()))

