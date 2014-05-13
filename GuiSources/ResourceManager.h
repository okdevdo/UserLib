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

#include "DataVector.h"

class CResourceManager: public CCppObject
{
public:
	enum TResourceType {
		WindowTitle,
		WindowAccelarator,
		WindowIcon,
		WindowSmallIcon,
		WindowMenu
	};

	struct TResourceItem
	{
		TResourceItem(LPTSTR wndclass, TResourceType rsrctype):
	        WindowClass(wndclass), ResourceType(rsrctype), ResourceID(0), CachedResource(NULL)
		{}

		TResourceItem(LPTSTR wndclass, TResourceType rsrctype, DWORD id):
	        WindowClass(wndclass), ResourceType(rsrctype), ResourceID(id), CachedResource(NULL)
		{}

		LPTSTR WindowClass;
		TResourceType ResourceType;
		DWORD ResourceID;
		HANDLE CachedResource;
	};

	typedef CDataSVectorT<TResourceItem> TResourceItems;

	CResourceManager(void);
	virtual ~CResourceManager(void);

	void add_resource(LPTSTR wndclass, TResourceType rsrctype, DWORD id);

	HANDLE get_resource(LPTSTR wndclass, TResourceType rsrctype);
	DWORD get_resourceID(LPTSTR wndclass, TResourceType rsrctype);

private:
	TResourceItems m_resourceCache;
};

class CFontManager: public CCppObject
{
public:
	struct TFontItem
	{
		TFontItem(CStringLiteral _prefix): prefix(__FILE__LINE__ _prefix), font(NULL) {}

		CStringBuffer prefix;
		Gdiplus::Font* font;
	};

	typedef CDataSVectorT<TFontItem> TFontItems;

	CFontManager(void);
	CFontManager(CStringLiteral _prefix);
	virtual ~CFontManager(void);

	bool has_Font(CStringLiteral _key);
	Gdiplus::Font* get_Font(CStringLiteral _key);
	void set_Font(CStringLiteral _key, ConstRef(CStringBuffer) _text);

private:
	Gdiplus::Font* parse_Font(ConstRef(CStringBuffer) _text);

	CStringBuffer m_prefix;
	Gdiplus::Font* m_pDefaultFont;
	TFontItems m_fontCache;
};

class CBrushManager: public CCppObject
{
public:
	struct TBrushItem
	{
		TBrushItem(CStringLiteral _prefix): prefix(__FILE__LINE__ _prefix), brush(NULL), bPrivateCopy(true) {}
		TBrushItem(Gdiplus::Brush* _brush): prefix(), brush(_brush), bPrivateCopy(false) {}

		CStringBuffer prefix;
		Gdiplus::Brush* brush;
		bool bPrivateCopy;
	};

	typedef CDataSVectorT<TBrushItem> TBrushItems;

public:
	CBrushManager(void);
	CBrushManager(CStringLiteral _prefix);
	virtual ~CBrushManager(void);

	__inline ConstRef(CStringBuffer) get_Prefix() const { return m_prefix; }

	bool has_Brush(CStringLiteral _key);
	Gdiplus::Brush* get_Brush(CStringLiteral _key, ConstRef(Gdiplus::Color) _defaultcolor);
	void set_Brush(CStringLiteral _key, ConstRef(Gdiplus::Color) _brushColor);
	void set_Brush(CStringLiteral _key, Gdiplus::Brush* _brush, bool _bPrivateCopy = false);
	void set_Brush(CStringLiteral _key, ConstRef(CStringBuffer) _text);

	void enumerate_Colors(Ref(CDataVectorT<CStringBuffer>) _colors);

protected:
	void remove_Duplicates(Gdiplus::Brush* _brush);
	Gdiplus::Brush* parse_Brush(ConstRef(CStringBuffer) _text);

	CStringBuffer m_prefix;
	TBrushItems m_brushCache;
};

