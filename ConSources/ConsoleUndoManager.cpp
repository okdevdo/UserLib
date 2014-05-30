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
#include "ConsoleUndoManager.h"

CConsoleUndoItem::CConsoleUndoItem(Type type):
    m_Type(type)
{
}

CConsoleUndoItem::CConsoleUndoItem(Type type, ConstRef(LCOORD) cursorPos, ConstRef(CStringBuffer) text):
    m_Type(type),
    m_CursorPos(cursorPos),
    m_SelectionEnd(),
    m_Text(text)
{
}

CConsoleUndoItem::CConsoleUndoItem(Type type, ConstRef(LCOORD) cursorPos, ConstRef(LCOORD) selectionEnd, ConstRef(CStringBuffer) text):
    m_Type(type),
    m_CursorPos(cursorPos),
    m_SelectionEnd(selectionEnd),
    m_Text(text)
{
}

CConsoleUndoItem::~CConsoleUndoItem(void)
{
}

static void __stdcall UndoGroupDeleteFunc( ConstPointer data, Pointer context )
{
	CConsoleUndoItem* p = CastAnyPtr(CConsoleUndoItem, CastMutable(Pointer, data));

	p->release();
}

CConsoleUndoGroup::CConsoleUndoGroup(void):
	m_ItemVector(__FILE__LINE__ 16, 16),
	m_Current(0)
{
}

CConsoleUndoGroup::~CConsoleUndoGroup(void)
{
}

void CConsoleUndoGroup::push(Ptr(CConsoleUndoItem) item)
{
	while ( !(IsRedoEmpty()) ) 
		pop();
	m_ItemVector.Append(item);
	++m_Current;
}

void CConsoleUndoGroup::pop()
{
	CConsoleUndoItemVector::Iterator it = m_ItemVector.Last();

	if ( it )
		m_ItemVector.Remove(it);
}

void CConsoleUndoGroup::clear(void)
{
	m_ItemVector.Close();
	m_ItemVector.Open(__FILE__LINE__ 16, 16);
	m_Current = 0;
}

Ptr(CConsoleUndoItem) CConsoleUndoGroup::undo()
{
	if ( IsUndoEmpty() )
		return NULL;

	CConsoleUndoItemVector::Iterator it = m_ItemVector.Begin();
	TListCnt cnt = GetUndoCount() - 1;

	while ( it && (cnt > 0) )
	{
		++it;
		--cnt;
	}
	if ( it )
	{
		--m_Current;
		return *it;
	}
	return NULL;
}

Ptr(CConsoleUndoItem) CConsoleUndoGroup::redo()
{
	if ( IsRedoEmpty() )
		return NULL;

	CConsoleUndoItemVector::Iterator it = m_ItemVector.Last();
	TListCnt cnt = GetRedoCount() - 1;

	while ( it && (cnt > 0) )
	{
		--it;
		--cnt;
	}
	if ( it )
	{
		++m_Current;
		return *it;
	}
	return NULL;
}

static void __stdcall UndoManagerDeleteFunc( ConstPointer data, Pointer context )
{
	CConsoleUndoGroup* p = CastAnyPtr(CConsoleUndoGroup, CastMutable(Pointer, data));

	p->release();
}

Ptr(CConsoleUndoManager) CConsoleUndoManager::_instance = NULL;

CConsoleUndoManager::CConsoleUndoManager(void):
	m_GroupList(__FILE__LINE__0)
{
}

CConsoleUndoManager::~CConsoleUndoManager(void)
{
	m_GroupList.Close(UndoManagerDeleteFunc, NULL);
}

void CConsoleUndoManager::addGroup(Ptr(CConsoleUndoGroup) group)
{
	m_GroupList.Append(group);
}

Ptr(CConsoleUndoManager) CConsoleUndoManager::Instance()
{
	if ( PtrCheck(_instance) )
		_instance = OK_NEW_OPERATOR CConsoleUndoManager();
	return _instance;
}

void CConsoleUndoManager::FreeInstance()
{
	if ( PtrCheck(_instance) )
		return;
	_instance->release();
}
