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

#include "DataVector.h"
#include "DataDoubleLinkedList.h"
#include "ConSources.h"

class CONSOURCES_API CConsoleUndoItem: public CCppObject
{
public:
	enum Type
	{
		UndoNoType,
		UndoInsertKey,
		UndoDeleteKey,
		UndoInsertText,
		UndoDeleteText
	};

public:
	CConsoleUndoItem(Type type = UndoNoType);
	CConsoleUndoItem(Type type, ConstRef(LCOORD) cursorPos, ConstRef(CStringBuffer) text);
	CConsoleUndoItem(Type type, ConstRef(LCOORD) cursorPos, ConstRef(LCOORD) selectionEnd, ConstRef(CStringBuffer) text);
	~CConsoleUndoItem(void);

	__inline Type GetType() const { return m_Type; }
	__inline ConstRef(LCOORD) GetCursorPos() const { return m_CursorPos; }
	__inline ConstRef(LCOORD) GetSelectionEnd() const { return m_SelectionEnd; }
	__inline ConstRef(CStringBuffer) GetText() const { return m_Text; }

protected:
	Type m_Type;
	LCOORD m_CursorPos;
	LCOORD m_SelectionEnd;
	CStringBuffer m_Text;
};

class CONSOURCES_API CConsoleUndoGroup: public CCppObject
{
public:
	CConsoleUndoGroup(void);
	~CConsoleUndoGroup(void);

	void push(Ptr(CConsoleUndoItem) item);
	void pop(void);
	void clear(void);
	Ptr(CConsoleUndoItem) undo();
	Ptr(CConsoleUndoItem) redo();

	__inline TListCnt GetUndoCount() const { return m_Current; }
	__inline bool IsUndoEmpty() const { return GetUndoCount() == 0; }
	__inline TListCnt GetRedoCount() const { return m_ItemVector.Count() - m_Current; }
	__inline bool IsRedoEmpty() const { return GetRedoCount() == 0; }
	__inline TListCnt GetCount() const { return m_ItemVector.Count(); }
	__inline bool IsEmpty() const { return GetCount() == 0; }

protected:
	typedef CDataVectorT<CConsoleUndoItem> CConsoleUndoItemVector;

	CConsoleUndoItemVector m_ItemVector;
	TListCnt m_Current;
};

class CONSOURCES_API CConsoleUndoManager: public CCppObject
{
public:
	CConsoleUndoManager(void);
	~CConsoleUndoManager(void);

	void addGroup(Ptr(CConsoleUndoGroup) group);

	static Ptr(CConsoleUndoManager) Instance();
	static void FreeInstance();

protected:
	typedef CDataDoubleLinkedListT<CConsoleUndoGroup> CConsoleUndoGroupList;

	CConsoleUndoGroupList m_GroupList;

	static Ptr(CConsoleUndoManager) _instance;
};

