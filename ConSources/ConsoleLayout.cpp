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
#include "CON_PCH.H"
#include "ConsoleLayout.h"
#include "ConsoleControl.h"

CConsoleLayoutItem::CConsoleLayoutItem()
{
	m_Position.X = 0;
	m_Position.Y = 0;
	m_Size.X = 0;
	m_Size.Y = 0;
	m_PreferredSize.X = 0;
	m_PreferredSize.Y = 0;
	m_MaximumSize.X = 0;
	m_MaximumSize.Y = 0;
	m_MinimumSize.X = 0;
	m_MinimumSize.Y = 0;
	m_SizeConstraint.X = NoConstraint;
	m_SizeConstraint.Y = NoConstraint;
	m_Alignment.X = LeftAlignment;
	m_Alignment.Y = TopAlignment;
}

CConsoleLayoutItem::CConsoleLayoutItem(COORD size, COORD alignment)
{
	m_Position.X = 0;
	m_Position.Y = 0;
	m_Size.X = 0;
	m_Size.Y = 0;
	m_PreferredSize = size;
	m_MaximumSize = size;
	m_MinimumSize = size;
	m_SizeConstraint.X = FixedConstraint;
	m_SizeConstraint.Y = FixedConstraint;
	m_Alignment = alignment;
}

CConsoleLayoutItem::CConsoleLayoutItem(COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment)
{
	m_Position.X = 0;
	m_Position.Y = 0;
	m_Size.X = 0;
	m_Size.Y = 0;
	m_PreferredSize = size;
	m_MaximumSize = max;
	m_MinimumSize = min;
	m_SizeConstraint = sizeConstraint;
	m_Alignment = alignment;
}

CConsoleLayoutItem::~CConsoleLayoutItem(void)
{
}

Ptr(CConsoleControl) CConsoleLayoutItem::GetControl()
{
	return nullptr;
}

Ptr(CConsoleLayout) CConsoleLayoutItem::GetLayout()
{
	return nullptr;
}


CConsoleLayoutControlItem::CConsoleLayoutControlItem(Ptr(CConsoleControl) ptr, COORD size, COORD alignment):
    CConsoleLayoutItem(size, alignment), m_Item(ptr)
{
}

CConsoleLayoutControlItem::CConsoleLayoutControlItem(Ptr(CConsoleControl) ptr, COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment):
    CConsoleLayoutItem(max, min, size, sizeConstraint, alignment), m_Item(ptr)
{
}

CConsoleLayoutControlItem::~CConsoleLayoutControlItem(void)
{
}

Ptr(CConsoleControl) CConsoleLayoutControlItem::GetControl()
{
	return m_Item;
}

CConsoleLayoutLayoutItem::CConsoleLayoutLayoutItem(Ptr(CConsoleLayout) ptr):
    CConsoleLayoutItem(), m_Item(ptr)
{
}

CConsoleLayoutLayoutItem::CConsoleLayoutLayoutItem(Ptr(CConsoleLayout) ptr, COORD size, COORD alignment):
    CConsoleLayoutItem(size, alignment), m_Item(ptr)
{
}

CConsoleLayoutLayoutItem::CConsoleLayoutLayoutItem(Ptr(CConsoleLayout) ptr, COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment):
    CConsoleLayoutItem(max, min, size, sizeConstraint, alignment), m_Item(ptr)
{
}

CConsoleLayoutLayoutItem::~CConsoleLayoutLayoutItem(void)
{
}

Ptr(CConsoleLayout) CConsoleLayoutLayoutItem::GetLayout()
{
	return m_Item;
}

CConsoleLayout::CConsoleLayout(void):
	m_Items(__FILE__LINE__ 16, 16)
{
	m_Margins.Left = 0;
	m_Margins.Right = 0;
	m_Margins.Top = 0;
	m_Margins.Bottom = 0;
	m_Spacing.X = 0;
	m_Spacing.Y = 0;
}

CConsoleLayout::CConsoleLayout(SMALL_RECT margins, COORD spacing):
	m_Items(__FILE__LINE__ 16, 16)
{
	m_Margins = margins;
	m_Spacing = spacing;
}

CConsoleLayout::~CConsoleLayout(void)
{
}

void CConsoleLayout::addLayout(Ptr(CConsoleLayout) item)
{
	Ptr(CConsoleLayoutLayoutItem) p = OK_NEW_OPERATOR CConsoleLayoutLayoutItem(item);

	item->Setup(p);
	m_Items.Append(p);
}

void CConsoleLayout::addLayout(Ptr(CConsoleLayout) item, COORD size, COORD alignment)
{
	Ptr(CConsoleLayoutLayoutItem) p = OK_NEW_OPERATOR CConsoleLayoutLayoutItem(item, size, alignment);

	m_Items.Append(p);
}

void CConsoleLayout::addLayout(Ptr(CConsoleLayout) item, COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment)
{
	Ptr(CConsoleLayoutLayoutItem) p = OK_NEW_OPERATOR CConsoleLayoutLayoutItem(item, max, min, size, sizeConstraint, alignment);

	m_Items.Append(p);
}

void CConsoleLayout::addControl(Ptr(CConsoleControl) item, COORD size, COORD alignment)
{
	Ptr(CConsoleLayoutControlItem) p = OK_NEW_OPERATOR CConsoleLayoutControlItem(item, size, alignment);

	m_Items.Append(p);
}

void CConsoleLayout::addControl(Ptr(CConsoleControl) item, COORD max, COORD min, COORD size, COORD sizeConstraint, COORD alignment)
{
	Ptr(CConsoleLayoutControlItem) p = OK_NEW_OPERATOR CConsoleLayoutControlItem(item, max, min, size, sizeConstraint, alignment);

	m_Items.Append(p);
}

CConsoleHBoxLayout::CConsoleHBoxLayout(void):
    CConsoleLayout()
{
}

CConsoleHBoxLayout::CConsoleHBoxLayout(SMALL_RECT margins, COORD spacing):
    CConsoleLayout(margins, spacing)
{
}

CConsoleHBoxLayout::~CConsoleHBoxLayout(void)
{
}

void CConsoleHBoxLayout::Setup(Ptr(CConsoleLayoutItem) layoutItem)
{
	CConsoleLayoutItemVector::Iterator it = m_Items.Begin();
	Ptr(CConsoleLayoutItem) pLayoutItem = nullptr;
	COORD minSize;
	COORD maxSize;
	COORD prefSize;
	COORD sizeCon;
	COORD align;
	bool bFirst = true;

	minSize.X = 0;
	minSize.Y = 0;
	maxSize.X = 0;
	maxSize.Y = 0;
	prefSize.X = 0;
	prefSize.Y = 0;
	sizeCon.X = CConsoleLayoutItem::NoConstraint;
	sizeCon.Y = CConsoleLayoutItem::NoConstraint;
	align.X = CConsoleLayoutItem::LeftAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	while ( it )
	{
		pLayoutItem = *it;
		if ( NotPtrCheck(pLayoutItem->GetLayout()) ) 
		{
			Ptr(CConsoleLayout) pLayout = pLayoutItem->GetLayout();

			pLayout->Setup(pLayoutItem);
		}
		minSize.X += pLayoutItem->GetMinimumSize().X;
		if ( minSize.Y < pLayoutItem->GetMinimumSize().Y )
			minSize.Y = pLayoutItem->GetMinimumSize().Y;
		if ( (Cast(sdword,pLayoutItem->GetMaximumSize().X) + Cast(sdword,maxSize.X)) > SHRT_MAX )
			maxSize.X = SHRT_MAX;
		else
			maxSize.X += pLayoutItem->GetMaximumSize().X;
		if ( maxSize.Y < pLayoutItem->GetMaximumSize().Y )
			maxSize.Y = pLayoutItem->GetMaximumSize().Y;
		prefSize.X += pLayoutItem->GetPreferredSize().X;
		if ( prefSize.Y < pLayoutItem->GetPreferredSize().Y )
			prefSize.Y = pLayoutItem->GetPreferredSize().Y;
		if ( bFirst )
			bFirst = false;
		else
		{
			minSize.X += m_Spacing.X;
			if ( (Cast(sdword,pLayoutItem->GetMaximumSize().X) + Cast(sdword,m_Spacing.X)) > SHRT_MAX )
				maxSize.X = SHRT_MAX;
			else
				maxSize.X += m_Spacing.X;
			prefSize.X += m_Spacing.X;
		}
		switch ( pLayoutItem->GetSizeConstraint().X )
		{
		case CConsoleLayoutItem::NoConstraint:
			break;
		case CConsoleLayoutItem::FixedConstraint:
			if ( sizeCon.X == CConsoleLayoutItem::FillConstraint )
				break;
			sizeCon.X = CConsoleLayoutItem::FixedConstraint;
			break;
		case CConsoleLayoutItem::FillConstraint:
			sizeCon.X = CConsoleLayoutItem::FillConstraint;
			break;
		}
		switch ( pLayoutItem->GetSizeConstraint().Y )
		{
		case CConsoleLayoutItem::NoConstraint:
			break;
		case CConsoleLayoutItem::FixedConstraint:
			if ( sizeCon.Y == CConsoleLayoutItem::FillConstraint )
				break;
			sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
			break;
		case CConsoleLayoutItem::FillConstraint:
			sizeCon.Y = CConsoleLayoutItem::FillConstraint;
			break;
		}
		switch ( pLayoutItem->GetAlignment().X )
		{
		case CConsoleLayoutItem::LeftAlignment:
			break;
		case CConsoleLayoutItem::HCenterAlignment:
			break;
		case CConsoleLayoutItem::RightAlignment:
			align.X = CConsoleLayoutItem::RightAlignment;
			break;
		}
		switch ( pLayoutItem->GetAlignment().Y )
		{
		case CConsoleLayoutItem::TopAlignment:
		case CConsoleLayoutItem::VCenterAlignment:
		case CConsoleLayoutItem::BottomAlignment:
			break;
		}
		++it;
	}
	layoutItem->SetMinimumSize(minSize);
	layoutItem->SetMaximumSize(maxSize);
	layoutItem->SetPreferredSize(prefSize);
	layoutItem->SetSizeConstraint(sizeCon);
	layoutItem->SetAlignment(align);
}

void CConsoleHBoxLayout::Create(COORD pos, COORD size)
{
	CConsoleLayoutItemVector::Iterator it = m_Items.Begin();
	Ptr(CConsoleLayoutItem) pLayoutItem = nullptr;
	sword fix = 0;
	sword fixSz = 0;
	sword fill = 0;
	sword align = CConsoleLayoutItem::LeftAlignment;

	while ( it )
	{
		pLayoutItem = *it;
		switch ( pLayoutItem->GetSizeConstraint().X )
		{
		case CConsoleLayoutItem::NoConstraint:
		case CConsoleLayoutItem::FixedConstraint:
			++fix;
			fixSz += pLayoutItem->GetPreferredSize().X;
			break;
		case CConsoleLayoutItem::FillConstraint:
			++fill;
			break;
		}
		switch ( pLayoutItem->GetAlignment().X )
		{
		case CConsoleLayoutItem::LeftAlignment:
			break;
		case CConsoleLayoutItem::HCenterAlignment:
			break;
		case CConsoleLayoutItem::RightAlignment:
			align = CConsoleLayoutItem::RightAlignment;
			break;
		}
		++it;
	}

	COORD pos1 = pos;
	COORD size1 = size;
	COORD size2;

	pos1.X += m_Margins.Left;
	pos1.Y += m_Margins.Top;
	size1.X -= m_Margins.Left + m_Margins.Right + ((fix + fill - 1) * m_Spacing.X) + fixSz;
	size1.Y -= m_Margins.Top + m_Margins.Bottom;
	it = m_Items.Begin();
	while ( it )
	{
		pLayoutItem = *it;
		switch ( pLayoutItem->GetSizeConstraint().X )
		{
		case CConsoleLayoutItem::NoConstraint:
		case CConsoleLayoutItem::FixedConstraint:
			size2.X = pLayoutItem->GetPreferredSize().X;
			break;
		case CConsoleLayoutItem::FillConstraint:
			size2.X = size1.X / fill;
			break;
		}
		switch ( pLayoutItem->GetSizeConstraint().Y )
		{
		case CConsoleLayoutItem::NoConstraint:
		case CConsoleLayoutItem::FixedConstraint:
			size2.Y = pLayoutItem->GetPreferredSize().Y;
			break;
		case CConsoleLayoutItem::FillConstraint:
			size2.Y = size1.Y;
			break;
		}
		pLayoutItem->SetPosition(pos1);
		pLayoutItem->SetSize(size2);
		pos1.X += size2.X + m_Spacing.X;
		++it;
	}
	it = m_Items.Begin();
	while ( it )
	{
		pLayoutItem = *it;
		if ( NotPtrCheck(pLayoutItem->GetControl()) )
		{
			Ptr(CConsoleControl) pControl = pLayoutItem->GetControl();

			pControl->Create(pLayoutItem->GetPosition(), pLayoutItem->GetSize());
		}
		else if ( NotPtrCheck(pLayoutItem->GetLayout()) )
		{
			Ptr(CConsoleLayout) pLayout = pLayoutItem->GetLayout();

			pLayout->Create(pLayoutItem->GetPosition(), pLayoutItem->GetSize());
		}
		++it;
	}
}

void CConsoleHBoxLayout::Move(COORD topleft)
{
	CConsoleLayoutItemVector::Iterator it = m_Items.Begin();
	Ptr(CConsoleLayoutItem) pLayoutItem = nullptr;
	COORD deltaPos;
	COORD pos;
	bool bFirst = true;

	while ( it )
	{
		pLayoutItem = *it;
		if ( bFirst )
		{
			deltaPos.X = pLayoutItem->GetPosition().X - topleft.X - m_Margins.Left;
			deltaPos.Y = pLayoutItem->GetPosition().Y - topleft.Y - m_Margins.Top;
			bFirst = false;
		}
		pos.X = pLayoutItem->GetPosition().X - deltaPos.X;
		pos.Y = pLayoutItem->GetPosition().Y - deltaPos.Y;
		pLayoutItem->SetPosition(pos);
		if ( NotPtrCheck(pLayoutItem->GetControl()) )
		{
			Ptr(CConsoleControl) pControl = pLayoutItem->GetControl();

			pControl->MoveWindow(pLayoutItem->GetPosition(), false, true);
		}
		else if ( NotPtrCheck(pLayoutItem->GetLayout()) )
		{
			Ptr(CConsoleLayout) pLayout = pLayoutItem->GetLayout();

			pLayout->Move(pLayoutItem->GetPosition());
		}
		++it;
	}
}

void CConsoleHBoxLayout::Resize(COORD newSize)
{
}

CConsoleVBoxLayout::CConsoleVBoxLayout(void):
    CConsoleLayout()
{
}

CConsoleVBoxLayout::CConsoleVBoxLayout(SMALL_RECT margins, COORD spacing):
    CConsoleLayout(margins, spacing)
{
}

CConsoleVBoxLayout::~CConsoleVBoxLayout(void)
{
}

void CConsoleVBoxLayout::Setup(Ptr(CConsoleLayoutItem) layoutItem)
{
	CConsoleLayoutItemVector::Iterator it = m_Items.Begin();
	Ptr(CConsoleLayoutItem) pLayoutItem = nullptr;
	COORD minSize;
	COORD maxSize;
	COORD prefSize;
	COORD sizeCon;
	COORD align;
	bool bFirst = true;

	minSize.X = 0;
	minSize.Y = 0;
	maxSize.X = 0;
	maxSize.Y = 0;
	prefSize.X = 0;
	prefSize.Y = 0;
	sizeCon.X = CConsoleLayoutItem::NoConstraint;
	sizeCon.Y = CConsoleLayoutItem::NoConstraint;
	align.X = CConsoleLayoutItem::LeftAlignment;
	align.Y = CConsoleLayoutItem::TopAlignment;
	while ( it )
	{
		pLayoutItem = *it;
		if ( NotPtrCheck(pLayoutItem->GetLayout()) ) 
		{
			Ptr(CConsoleLayout) pLayout = pLayoutItem->GetLayout();

			pLayout->Setup(pLayoutItem);
		}
		if ( minSize.X < pLayoutItem->GetMinimumSize().X )
			minSize.X = pLayoutItem->GetMinimumSize().X;
		minSize.Y += pLayoutItem->GetMinimumSize().Y;
		if ( maxSize.X < pLayoutItem->GetMaximumSize().X )
			maxSize.X = pLayoutItem->GetMaximumSize().X;
		if ( (Cast(sdword,pLayoutItem->GetMaximumSize().Y) + Cast(sdword,maxSize.Y)) > SHRT_MAX )
			maxSize.Y = SHRT_MAX;
		else
			maxSize.Y += pLayoutItem->GetMaximumSize().Y;
		if ( prefSize.X < pLayoutItem->GetPreferredSize().X )
			prefSize.X = pLayoutItem->GetPreferredSize().X;
		prefSize.Y += pLayoutItem->GetPreferredSize().Y;
		if ( bFirst )
			bFirst = false;
		else
		{
			minSize.Y += m_Spacing.Y;
			if ( (Cast(sdword,pLayoutItem->GetMaximumSize().Y) + Cast(sdword,maxSize.Y)) > SHRT_MAX )
				maxSize.Y = SHRT_MAX;
			else
				maxSize.Y += m_Spacing.Y;
			prefSize.Y += m_Spacing.Y;
		}
		switch ( pLayoutItem->GetSizeConstraint().X )
		{
		case CConsoleLayoutItem::NoConstraint:
			break;
		case CConsoleLayoutItem::FixedConstraint:
			if ( sizeCon.X == CConsoleLayoutItem::FillConstraint )
				break;
			sizeCon.X = CConsoleLayoutItem::FixedConstraint;
			break;
		case CConsoleLayoutItem::FillConstraint:
			sizeCon.X = CConsoleLayoutItem::FillConstraint;
			break;
		}
		switch ( pLayoutItem->GetSizeConstraint().Y )
		{
		case CConsoleLayoutItem::NoConstraint:
			break;
		case CConsoleLayoutItem::FixedConstraint:
			if ( sizeCon.Y == CConsoleLayoutItem::FillConstraint )
				break;
			sizeCon.Y = CConsoleLayoutItem::FixedConstraint;
			break;
		case CConsoleLayoutItem::FillConstraint:
			sizeCon.Y = CConsoleLayoutItem::FillConstraint;
			break;
		}
		switch ( pLayoutItem->GetAlignment().X )
		{
		case CConsoleLayoutItem::LeftAlignment:
		case CConsoleLayoutItem::HCenterAlignment:
		case CConsoleLayoutItem::RightAlignment:
			break;
		}
		switch ( pLayoutItem->GetAlignment().Y )
		{
		case CConsoleLayoutItem::TopAlignment:
		case CConsoleLayoutItem::VCenterAlignment:
		case CConsoleLayoutItem::BottomAlignment:
			break;
		}
		++it;
	}
	layoutItem->SetMinimumSize(minSize);
	layoutItem->SetMaximumSize(maxSize);
	layoutItem->SetPreferredSize(prefSize);
	layoutItem->SetSizeConstraint(sizeCon);
	layoutItem->SetAlignment(align);
}

void CConsoleVBoxLayout::Create(COORD pos, COORD size)
{
	CConsoleLayoutItemVector::Iterator it = m_Items.Begin();
	Ptr(CConsoleLayoutItem) pLayoutItem = nullptr;
	sword fix = 0;
	sword fixSz = 0;
	sword fill = 0;

	while ( it )
	{
		pLayoutItem = *it;
		switch ( pLayoutItem->GetSizeConstraint().Y )
		{
		case CConsoleLayoutItem::NoConstraint:
		case CConsoleLayoutItem::FixedConstraint:
			++fix;
			fixSz += pLayoutItem->GetPreferredSize().Y;
			break;
		case CConsoleLayoutItem::FillConstraint:
			++fill;
			break;
		}
		++it;
	}

	COORD pos1 = pos;
	COORD size1 = size;
	COORD size2;

	pos1.X += m_Margins.Left;
	pos1.Y += m_Margins.Top;
	size1.X -= m_Margins.Left + m_Margins.Right;
	size1.Y -= m_Margins.Top + m_Margins.Bottom + ((fix + fill - 1) * m_Spacing.Y) + fixSz;
	it = m_Items.Begin();
	while ( it )
	{
		pLayoutItem = *it;
		switch ( pLayoutItem->GetSizeConstraint().X )
		{
		case CConsoleLayoutItem::NoConstraint:
		case CConsoleLayoutItem::FixedConstraint:
			size2.X = pLayoutItem->GetPreferredSize().X;
			break;
		case CConsoleLayoutItem::FillConstraint:
			size2.X = size1.X;
			break;
		}
		switch ( pLayoutItem->GetSizeConstraint().Y )
		{
		case CConsoleLayoutItem::NoConstraint:
		case CConsoleLayoutItem::FixedConstraint:
			size2.Y = pLayoutItem->GetPreferredSize().Y;
			break;
		case CConsoleLayoutItem::FillConstraint:
			size2.Y = size1.Y / fill;
			break;
		}
		switch ( pLayoutItem->GetAlignment().X )
		{
		case CConsoleLayoutItem::LeftAlignment:
			pos1.X = pos.X + m_Margins.Left;
			break;
		case CConsoleLayoutItem::HCenterAlignment:
			break;
		case CConsoleLayoutItem::RightAlignment:
			pos1.X = pos.X + m_Margins.Left + size.X - size2.X - 1 - m_Margins.Right;
			break;
		}
		pLayoutItem->SetPosition(pos1);
		pLayoutItem->SetSize(size2);
		pos1.Y += size2.Y + m_Spacing.Y;
		++it;
	}
	it = m_Items.Begin();
	while ( it )
	{
		pLayoutItem = *it;
		if ( NotPtrCheck(pLayoutItem->GetControl()) )
		{
			Ptr(CConsoleControl) pControl = pLayoutItem->GetControl();

			pControl->Create(pLayoutItem->GetPosition(), pLayoutItem->GetSize());
		}
		else if ( NotPtrCheck(pLayoutItem->GetLayout()) )
		{
			Ptr(CConsoleLayout) pLayout = pLayoutItem->GetLayout();

			pLayout->Create(pLayoutItem->GetPosition(), pLayoutItem->GetSize());
		}
		++it;
	}
}

void CConsoleVBoxLayout::Move(COORD topleft)
{
	CConsoleLayoutItemVector::Iterator it = m_Items.Begin();
	Ptr(CConsoleLayoutItem) pLayoutItem = nullptr;
	COORD deltaPos;
	COORD pos;
	bool bFirst = true;

	while ( it )
	{
		pLayoutItem = *it;
		if ( bFirst )
		{
			deltaPos.X = pLayoutItem->GetPosition().X - topleft.X - m_Margins.Left;
			deltaPos.Y = pLayoutItem->GetPosition().Y - topleft.Y - m_Margins.Top;
			bFirst = false;
		}
		pos.X = pLayoutItem->GetPosition().X - deltaPos.X;
		pos.Y = pLayoutItem->GetPosition().Y - deltaPos.Y;
		pLayoutItem->SetPosition(pos);
		if ( NotPtrCheck(pLayoutItem->GetControl()) )
		{
			Ptr(CConsoleControl) pControl = pLayoutItem->GetControl();

			pControl->MoveWindow(pLayoutItem->GetPosition(), false, true);
		}
		else if ( NotPtrCheck(pLayoutItem->GetLayout()) )
		{
			Ptr(CConsoleLayout) pLayout = pLayoutItem->GetLayout();

			pLayout->Move(pLayoutItem->GetPosition());
		}
		++it;
	}
}

void CConsoleVBoxLayout::Resize(COORD newSize)
{
}
