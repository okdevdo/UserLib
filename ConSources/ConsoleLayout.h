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

class CConsoleControl;
class CConsoleLayout;
class CONSOURCES_API CConsoleLayoutItem: public CCppObject
{
public:
	enum SizeConstraint
	{
		NoConstraint,
		FixedConstraint,
		FillConstraint
	};

	enum HAlignment
	{
		LeftAlignment,
		HCenterAlignment,
		RightAlignment
	};

	enum VAlignment
	{
		TopAlignment,
		VCenterAlignment,
		BottomAlignment
	};

public:
	CConsoleLayoutItem(void);
	CConsoleLayoutItem(COORD size, COORD alignment);
	CConsoleLayoutItem(COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment);
	virtual ~CConsoleLayoutItem(void);

	__inline COORD GetPosition() { return m_Position; }
	__inline COORD GetSize() { return m_Size; }
	__inline COORD GetPreferredSize() { return m_PreferredSize; }
	__inline COORD GetMaximumSize() { return m_MaximumSize; }
	__inline COORD GetMinimumSize() { return m_MinimumSize; }
	__inline COORD GetSizeConstraint() { return m_SizeConstraint; }
	__inline COORD GetAlignment() { return m_Alignment; }

	__inline void SetPosition(COORD v) { m_Position = v; }
	__inline void SetSize(COORD v) { m_Size = v; }
	__inline void SetPreferredSize(COORD v) { m_PreferredSize = v; }
	__inline void SetMaximumSize(COORD v) { m_MaximumSize = v; }
	__inline void SetMinimumSize(COORD v) { m_MinimumSize = v; }
	__inline void SetSizeConstraint(COORD v) { m_SizeConstraint = v; }
	__inline void SetAlignment(COORD v) { m_Alignment = v; }

	virtual Ptr(CConsoleControl) GetControl();
	virtual Ptr(CConsoleLayout) GetLayout();

protected:
	COORD m_Position;
	COORD m_Size;
	COORD m_PreferredSize;
	COORD m_MaximumSize;
	COORD m_MinimumSize;
	COORD m_SizeConstraint;
	COORD m_Alignment;

private:
	CConsoleLayoutItem(ConstRef(CConsoleLayoutItem));
};

class CONSOURCES_API CConsoleLayoutControlItem: public CConsoleLayoutItem
{
public:
	CConsoleLayoutControlItem(Ptr(CConsoleControl) ptr, COORD size, COORD alignment);
	CConsoleLayoutControlItem(Ptr(CConsoleControl) ptr, COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment);
	virtual ~CConsoleLayoutControlItem(void);

	virtual Ptr(CConsoleControl) GetControl();

protected:
	Ptr(CConsoleControl) m_Item;

private:
	CConsoleLayoutControlItem(void);
	CConsoleLayoutControlItem(ConstRef(CConsoleLayoutControlItem));
};

class CONSOURCES_API CConsoleLayoutLayoutItem: public CConsoleLayoutItem
{
public:
	CConsoleLayoutLayoutItem(Ptr(CConsoleLayout) ptr);
	CConsoleLayoutLayoutItem(Ptr(CConsoleLayout) ptr, COORD size, COORD alignment);
	CConsoleLayoutLayoutItem(Ptr(CConsoleLayout) ptr, COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment);
	virtual ~CConsoleLayoutLayoutItem(void);

	virtual Ptr(CConsoleLayout) GetLayout();

protected:
	Ptr(CConsoleLayout) m_Item;

private:
	CConsoleLayoutLayoutItem(void);
	CConsoleLayoutLayoutItem(ConstRef(CConsoleLayoutLayoutItem));
};

class CONSOURCES_API CConsoleLayout: public CCppObject
{
public:
	CConsoleLayout(void);
	CConsoleLayout(SMALL_RECT margins, COORD spacing);
	virtual ~CConsoleLayout(void);

	__inline SMALL_RECT GetMargins() { return m_Margins; }
	__inline void SetMargins(SMALL_RECT v) { m_Margins = v; }

	__inline COORD GetSpacing() { return m_Spacing; }
	__inline void SetSpacing(COORD v) { m_Spacing = v; }

	void addLayout(Ptr(CConsoleLayout) item);
	void addLayout(Ptr(CConsoleLayout) item, COORD size, COORD alignment);
	void addLayout(Ptr(CConsoleLayout) item, COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment);
	void addControl(Ptr(CConsoleControl) item, COORD size, COORD alignment);
	void addControl(Ptr(CConsoleControl) item, COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment);

	virtual void Setup(Ptr(CConsoleLayoutItem) layoutItem) = 0;
	virtual void Create(COORD pos, COORD size) = 0;
	virtual void Move(COORD topleft) = 0;
	virtual void Resize(COORD newSize) = 0;

protected:
	typedef CDataVectorT<CConsoleLayoutItem> CConsoleLayoutItemVector;

	SMALL_RECT m_Margins;
	COORD m_Spacing;
	CConsoleLayoutItemVector m_Items;

private:
	CConsoleLayout(ConstRef(CConsoleLayout));
};

class CONSOURCES_API CConsoleHBoxLayout: public CConsoleLayout
{
public:
	CConsoleHBoxLayout(void);
	CConsoleHBoxLayout(SMALL_RECT margins, COORD spacing);
	virtual ~CConsoleHBoxLayout(void);

	virtual void Setup(Ptr(CConsoleLayoutItem) layoutItem);
	virtual void Create(COORD pos, COORD size);
	virtual void Move(COORD topleft);
	virtual void Resize(COORD newSize);

private:
	CConsoleHBoxLayout(ConstRef(CConsoleHBoxLayout));
};

class CONSOURCES_API CConsoleVBoxLayout: public CConsoleLayout
{
public:
	CConsoleVBoxLayout(void);
	CConsoleVBoxLayout(SMALL_RECT margins, COORD spacing);
	virtual ~CConsoleVBoxLayout(void);

	virtual void Setup(Ptr(CConsoleLayoutItem) layoutItem);
	virtual void Create(COORD pos, COORD size);
	virtual void Move(COORD topleft);
	virtual void Resize(COORD newSize);

private:
	CConsoleVBoxLayout(ConstRef(CConsoleVBoxLayout));
};

