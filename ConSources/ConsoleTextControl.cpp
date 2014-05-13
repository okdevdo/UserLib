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
#include "ConsoleTextControl.h"
#include "Console.h"
#include "ConsoleClipboard.h"
#include "ConsoleUndoManager.h"

CConsoleTextControl::CConsoleTextControl(CConstPointer name, CConsole* pConsole):
    CConsoleControl(name, pConsole),
    m_TextLineCnt(0),
    m_TextLines(NULL),
	m_TextLineLen(NULL),
	m_TextLineAlloc(NULL),
	m_SelectionMode(false),
	m_UndoGroup(NULL),
	m_MultiLine(false),
	m_MaxChars(0)
{
}

CConsoleTextControl::CConsoleTextControl(CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(name, title, pConsole),
    m_TextLineCnt(0),
    m_TextLines(NULL),
	m_TextLineLen(NULL),
	m_TextLineAlloc(NULL),
	m_SelectionMode(false),
	m_UndoGroup(NULL),
	m_MultiLine(false),
	m_MaxChars(0)
{
}

CConsoleTextControl::CConsoleTextControl(CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(callback, name, title, pConsole),
    m_TextLineCnt(0),
    m_TextLines(NULL),
	m_TextLineLen(NULL),
	m_TextLineAlloc(NULL),
	m_SelectionMode(false),
	m_UndoGroup(NULL),
	m_MultiLine(false),
	m_MaxChars(0)
{
}

CConsoleTextControl::CConsoleTextControl(word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(taborder, callback, name, title, pConsole),
    m_TextLineCnt(0),
    m_TextLines(NULL),
	m_TextLineLen(NULL),
	m_TextLineAlloc(NULL),
	m_SelectionMode(false),
	m_UndoGroup(NULL),
	m_MultiLine(false),
	m_MaxChars(0)
{
}

CConsoleTextControl::CConsoleTextControl(sdword maxchars, bool multiline, word taborder, CAbstractConsoleControlCallback* callback, CConstPointer name, CConstPointer title, CConsole* pConsole):
    CConsoleControl(taborder, callback, name, title, pConsole),
    m_TextLineCnt(0),
    m_TextLines(NULL),
	m_TextLineLen(NULL),
	m_TextLineAlloc(NULL),
	m_SelectionMode(false),
	m_UndoGroup(NULL),
	m_MultiLine(multiline),
	m_MaxChars(maxchars)
{
}

CConsoleTextControl::~CConsoleTextControl(void)
{
	for ( sdword ix = 0; ix < m_TextLineCnt; ++ix )
		if ( m_TextLineAlloc[ix] )
			TFfree(m_TextLines[ix]);
	if ( m_TextLines )
		TFfree(m_TextLines);
	if ( m_TextLineLen )
		TFfree(m_TextLineLen);
	if ( m_TextLineAlloc )
		TFfree(m_TextLineAlloc);
	if ( m_UndoGroup )
		m_UndoGroup->release();
}

bool CConsoleTextControl::GetMultiLine()
{
	return m_MultiLine;
}

void CConsoleTextControl::SetMultiLine(bool b)
{
	if ( m_MultiLine == b )
		return;
	if ( (!b) && (m_TextLineCnt > 1) )
	{
		CStringBuffer tmp(__FILE__LINE__ m_TextLines[0]);

		SetText(tmp);
	}
	m_MultiLine = b;
}

sdword CConsoleTextControl::GetMaxChars()
{
	return m_MaxChars;
}

sdword CConsoleTextControl::_GetMaxChars()
{
	sdword result = 0;

	for ( sdword ix = 0; ix < m_TextLineCnt; ++ix )
	{
		if ( ix > 0 )
			result += 2;
		result += s_strlen(m_TextLines[ix], m_TextLineLen[ix]);
	}
	return result;
}

void CConsoleTextControl::SetMaxChars(sdword v)
{
	if ( v < _GetMaxChars() )
	{
		sdword cnt = 0;
		sdword cnt1 = 0;

		for ( sdword ix = 0; ix < m_TextLineCnt; ++ix )
		{
			if ( ix > 0 )
				cnt += 2;
			cnt += s_strlen(m_TextLines[ix], m_TextLineLen[ix]);
			if ( v < cnt )
			{
			}
			cnt1 = cnt;
		}

	}
	m_MaxChars = v;
}

void CConsoleTextControl::Initialize(bool _emptyText)
{
	m_SelectionMode = false;
	m_CursorPos.X = 0;
	m_CursorPos.Y = 0;
	m_SelectionEnd.X = 0;
	m_SelectionEnd.Y = 0;
	m_ScrollPos.X = 0;
	m_ScrollPos.Y = 0;

	if ( m_TextLines )
		TFfree(m_TextLines);
	if ( m_TextLineLen )
		TFfree(m_TextLineLen);
	if ( m_TextLineAlloc )
		TFfree(m_TextLineAlloc);
    m_TextLineCnt = 0;
	m_TextLineLenMax = 0;
    m_TextLines = NULL;
	m_TextLineLen = NULL;
	m_TextLineAlloc = NULL;

	if ( PtrCheck(m_UndoGroup) )
	{
		Ptr(CConsoleUndoManager) undoManager = CConsoleUndoManager::Instance();

		m_UndoGroup = OK_NEW_OPERATOR CConsoleUndoGroup();
		m_UndoGroup->addRef();
		undoManager->addGroup(m_UndoGroup);
	}
	else
		m_UndoGroup->clear();

	if ( _emptyText )
	{
		m_Text.SetString(__FILE__LINE__ _T(""));
		m_TextLines = CastAny(CArray, TFalloc(szPointer));
		m_TextLines[0] = CastMutable(CPointer, m_Text.GetString());
		m_TextLineLen = CastAnyPtr(sdword, TFalloc(sizeof(sdword)));
		m_TextLineAlloc = CastAnyPtr(bool, TFalloc(sizeof(bool)));
		m_TextLineAlloc[0] = false;
		m_TextLineCnt = 1;
		m_TextLineLenMax = 0;
	}
}

void CConsoleTextControl::SetText(ConstRef(CStringBuffer) _text)
{
	if ( PtrCheck(_text.GetString()) )
	{
		Initialize(true);
		DrawTextControl();
		return;
	}
	Initialize();
	m_Text = _text;
	_SetText();
	DrawTextControl();
}

void CConsoleTextControl::_SetText(void)
{
	CStringConstIterator it = m_Text.Begin();
	sdword delimcnt[4];
	dword cnt;
	word ix;
	word mx;

	for ( ix = 0; ix < 4; ++ix )
		delimcnt[ix] = 0;
	it.FirstOf(_T("\r\n"));
	while ( !(it.IsEnd()) )
	{
		if ( (it[0] == _T('\r')) && (it[1] == _T('\n')) )
		{
			++it; ++it;
			++(delimcnt[0]);
		}
		else if ( (it[0] == _T('\n')) && (it[1] == _T('\r')) )
		{
			++it; ++it;
			++(delimcnt[1]);
		}
		else if ( it[0] == _T('\r') )
		{
			++it;
			++(delimcnt[2]);
		}
		else
		{
			++it;
			++(delimcnt[3]);
		}
		it.FirstOf(_T("\r\n"));
	}
	m_TextLineCnt = 0;
	mx = 4;
	for ( ix = 0; ix < 4; ++ix )
	{
		if ( delimcnt[ix] > m_TextLineCnt )
		{
			m_TextLineCnt = delimcnt[ix];
			mx = ix;
		}
	}
	++m_TextLineCnt;
	m_TextLines = CastAny(CArray, TFalloc(m_TextLineCnt * szPointer));
	switch ( mx )
	{
	case 0:
		m_Text.Split(_T("\r\n"), m_TextLines, m_TextLineCnt, &cnt);
		m_TextLineCnt = cnt;
		break;
	case 1:
		m_Text.Split(_T("\n\r"), m_TextLines, m_TextLineCnt, &cnt);
		m_TextLineCnt = cnt;
		break;
	case 2:
		m_Text.Split(_T("\r"), m_TextLines, m_TextLineCnt, &cnt);
		m_TextLineCnt = cnt;
		break;
	case 3:
		m_Text.Split(_T("\n"), m_TextLines, m_TextLineCnt, &cnt);
		m_TextLineCnt = cnt;
		break;
	case 4:
		{
			CStringBuffer tmp(__FILE__LINE__ m_Text.GetString(), m_Text.GetLength()); // copy m_Text

			m_Text = tmp;
			m_TextLines[0] = CastMutable(CPointer, m_Text.GetString());
		}
		break;
	}
	m_TextLineLen = CastAnyPtr(sdword, TFalloc(m_TextLineCnt * sizeof(sdword)));
	m_TextLineLenMax = 0;
	for ( sdword ix = 0; ix < m_TextLineCnt; ++ix )
	{
		sdword len = GetTextLength(m_TextLines[ix]);

		m_TextLineLen[ix] = len;
		if ( len > m_TextLineLenMax )
			m_TextLineLenMax = len;
	}
	m_TextLineAlloc = CastAnyPtr(bool, TFalloc(m_TextLineCnt * sizeof(bool)));
	for ( sdword ix = 0; ix < m_TextLineCnt; ++ix )
		m_TextLineAlloc[ix] = false;
}

CStringBuffer CConsoleTextControl::GetText()
{
	CStringBuffer result;

	if ( PtrCheck(m_TextLines) )
		return result;
	for ( sdword ix = 0; ix < m_TextLineCnt; ++ix )
	{
		if ( ix > 0 )
			result.AppendString(_T("\r\n"));
		result.AppendString(m_TextLines[ix]);
	}
	return result;
}

void CConsoleTextControl::Update()
{
	m_TextLineLenMax = 0;
	for ( sdword ix = 0; ix < m_TextLineCnt; ++ix )
	{
		sdword len = m_TextLineLen[ix];

		if ( len > m_TextLineLenMax )
			m_TextLineLenMax = len;
	}
	if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
	{
		m_ScrollBarHInfo.SetScrollBarInfo(m_TextLineLenMax, m_ClientAreaSize.X);
		m_ScrollBarHInfo.Current = m_ScrollPos.X;
		DrawHorizontalScrollBar();
	}
	if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
	{
		m_ScrollBarVInfo.SetScrollBarInfo(m_TextLineCnt, m_ClientAreaSize.Y);
		m_ScrollBarVInfo.Current = m_ScrollPos.Y;
		DrawVerticalScrollBar();
	}
	DrawTextControl();
	PostPaintEvent();
	SetCursorPosEx();
}

void CConsoleTextControl::DrawTextControl()
{
	if ( PtrCheck(m_TextLines) )    
		return;

	SMALL_RECT rect;
	CConstPointer text0;
	CConstPointer text;
	sword max = m_ClientAreaSize.Y;
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
	bool bTodo;
	sword ix;
	sword off = m_hasBorder?1:0;

	rect.Top = off;
	rect.Bottom = rect.Top + 1;
	for ( ix = 0; ix < max; ++ix )
	{
		rect.Left = off;
		rect.Right = m_ClientAreaSize.X + off;
		bTodo = true;
		if ( ((ix + m_ScrollPos.Y) < m_TextLineCnt) && (m_ScrollPos.X <= m_TextLineLen[ix + m_ScrollPos.Y]) )
		{
			text0 = m_TextLines[ix + m_ScrollPos.Y];
			text = GetTextPointer(text0, m_ScrollPos.X);
			if ( bSelection )
			{
				if ( (m_SelectionEnd.Y == m_CursorPos.Y) && (m_CursorPos.Y == (ix + m_ScrollPos.Y)) )
				{
					if ( m_SelectionEnd.X < m_CursorPos.X )
					{
						if ( (m_SelectionEnd.X <= m_ScrollPos.X) && (m_CursorPos.X >= m_ScrollPos.X) )
						{
							if ( (m_CursorPos.X >= m_TextLineLen[ix + m_ScrollPos.Y]) || ((m_CursorPos.X - m_ScrollPos.X + off) >= (m_ClientAreaSize.X + off)) )
							{
								DrawString(rect, text, m_HighLightColor);
								bTodo = false;
							}
							else
							{
								rect.Right = Cast(sword, m_CursorPos.X - m_ScrollPos.X + off);
								if ( rect.Right > rect.Left )
									DrawString(rect, text, m_HighLightColor);
								text = GetTextPointer(text0, m_CursorPos.X);
								rect.Left = rect.Right;
								rect.Right = m_ClientAreaSize.X + off;
							}
						}
						else if ( (m_SelectionEnd.X > m_ScrollPos.X) && (m_CursorPos.X > m_ScrollPos.X) && ((m_SelectionEnd.X - m_ScrollPos.X + off) < (m_ClientAreaSize.X + off)) )
						{
							rect.Right = Cast(sword, m_SelectionEnd.X - m_ScrollPos.X + off);
							DrawString(rect, text, m_Color);
							text = GetTextPointer(text0, m_SelectionEnd.X);
							rect.Left = rect.Right;
							if ( (m_CursorPos.X >= m_TextLineLen[ix + m_ScrollPos.Y]) || ((m_CursorPos.X - m_ScrollPos.X + off) >= (m_ClientAreaSize.X + off)) )
							{
								rect.Right = m_ClientAreaSize.X + off;
								DrawString(rect, text, m_HighLightColor);
								bTodo = false;
							}
							else
							{
								rect.Right = Cast(sword, m_CursorPos.X - m_ScrollPos.X + off);
								DrawString(rect, text, m_HighLightColor);
								text = GetTextPointer(text0, m_CursorPos.X);
								rect.Left = rect.Right;
								rect.Right = m_ClientAreaSize.X + off;
							}
						}
					}
					else
					{
						if ( (m_CursorPos.X <= m_ScrollPos.X) && (m_SelectionEnd.X >= m_ScrollPos.X) )
						{
							if ( (m_SelectionEnd.X >= m_TextLineLen[ix + m_ScrollPos.Y]) || ((m_SelectionEnd.X - m_ScrollPos.X + off) >= (m_ClientAreaSize.X + off)) )
							{
								DrawString(rect, text, m_HighLightColor);
								bTodo = false;
							}
							else
							{
								rect.Right = Cast(sword, m_SelectionEnd.X - m_ScrollPos.X + off);
								if ( rect.Right > rect.Left )
									DrawString(rect, text, m_HighLightColor);
								text = GetTextPointer(text0, m_SelectionEnd.X);
								rect.Left = rect.Right;
								rect.Right = m_ClientAreaSize.X + off;
							}
						}
						else if ( (m_SelectionEnd.X > m_ScrollPos.X) && (m_CursorPos.X > m_ScrollPos.X) && ((m_CursorPos.X - m_ScrollPos.X + off) < (m_ClientAreaSize.X + off)) )
						{
							rect.Right = Cast(sword, m_CursorPos.X - m_ScrollPos.X + off);
							DrawString(rect, text, m_Color);
							text = GetTextPointer(text0, m_CursorPos.X);
							rect.Left = rect.Right;
							if ( (m_SelectionEnd.X >= m_TextLineLen[ix + m_ScrollPos.Y]) || ((m_SelectionEnd.X - m_ScrollPos.X + off) >= (m_ClientAreaSize.X + off)) )
							{
								rect.Right = m_ClientAreaSize.X + off;
								DrawString(rect, text, m_HighLightColor);
								bTodo = false;
							}
							else
							{
								rect.Right = Cast(sword, m_SelectionEnd.X - m_ScrollPos.X + off);
								DrawString(rect, text, m_HighLightColor);
								text = GetTextPointer(text0, m_SelectionEnd.X);
								rect.Left = rect.Right;
								rect.Right = m_ClientAreaSize.X + off;
							}
						}
					}
				}
				else if ( m_SelectionEnd.Y < m_CursorPos.Y )
				{
					if ( m_SelectionEnd.Y == (ix + m_ScrollPos.Y) )
					{
						if ( m_SelectionEnd.X <= m_ScrollPos.X )
						{
							DrawString(rect, text, m_HighLightColor);
							bTodo = false;
						}
						else if ( (m_SelectionEnd.X - m_ScrollPos.X + off) < (m_ClientAreaSize.X + off) )
						{
							rect.Right = Cast(sword, m_SelectionEnd.X - m_ScrollPos.X + off);
							DrawString(rect, text, m_Color);
							text = GetTextPointer(text0, m_SelectionEnd.X);
							rect.Left = rect.Right;
							rect.Right = m_ClientAreaSize.X + off;
							DrawString(rect, text, m_HighLightColor);
							bTodo = false;
						}
					}
					else if ( m_CursorPos.Y == (ix + m_ScrollPos.Y) )
					{
						if ( m_CursorPos.X > m_ScrollPos.X )
						{
							if ( (m_CursorPos.X - m_ScrollPos.X + off) >= (m_ClientAreaSize.X + off) )
							{
								DrawString(rect, text, m_HighLightColor);
								bTodo = false;
							}
							else
							{
								rect.Right = Cast(sword, m_CursorPos.X - m_ScrollPos.X + off);
								DrawString(rect, text, m_HighLightColor);
								text = GetTextPointer(text0, m_CursorPos.X);
								rect.Left = rect.Right;
								rect.Right = m_ClientAreaSize.X + off;
							}
						}
					}
					else if ( (m_SelectionEnd.Y < (ix + m_ScrollPos.Y)) && (m_CursorPos.Y > (ix + m_ScrollPos.Y)) )
					{
						DrawString(rect, text, m_HighLightColor);
						bTodo = false;
					}
				}
				else if ( m_SelectionEnd.Y > m_CursorPos.Y )
				{
					if ( m_CursorPos.Y == (ix + m_ScrollPos.Y) )
					{
						if ( m_CursorPos.X <= m_ScrollPos.X )
						{
							DrawString(rect, text, m_HighLightColor);
							bTodo = false;
						}
						else if ( (m_CursorPos.X - m_ScrollPos.X + off) < (m_ClientAreaSize.X + off) )
						{
							rect.Right = Cast(sword, m_CursorPos.X - m_ScrollPos.X + off);
							DrawString(rect, text, m_Color);
							text = GetTextPointer(text0, m_CursorPos.X);
							rect.Left = rect.Right;
							rect.Right = m_ClientAreaSize.X + off;
							DrawString(rect, text, m_HighLightColor);
							bTodo = false;
						}
					}
					else if ( m_SelectionEnd.Y == (ix + m_ScrollPos.Y) )
					{
						if ( m_SelectionEnd.X > m_ScrollPos.X )
						{
							if ( (m_SelectionEnd.X - m_ScrollPos.X + off) >= (m_ClientAreaSize.X + off) )
							{
								DrawString(rect, text, m_HighLightColor);
								bTodo = false;
							}
							else
							{
								rect.Right = Cast(sword, m_SelectionEnd.X - m_ScrollPos.X + off);
								DrawString(rect, text, m_HighLightColor);
								text = GetTextPointer(text0, m_SelectionEnd.X);
								rect.Left = rect.Right;
								rect.Right = m_ClientAreaSize.X + off;
							}
						}
					}
					else if ( (m_SelectionEnd.Y > (ix + m_ScrollPos.Y)) && (m_CursorPos.Y < (ix + m_ScrollPos.Y)) )
					{
						DrawString(rect, text, m_HighLightColor);
						bTodo = false;
					}
				}
			}
		}
		else
			text = _T(" ");
		if ( bTodo )
			DrawString(rect, text, m_Color);
		++(rect.Top);
		++(rect.Bottom);
	}
}

void CConsoleTextControl::DeleteSelection(void)
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

	if ( bSelection )
	{
		LCOORD cursorPos;
		LCOORD selectionEnd;

		if ( m_SelectionEnd.Y == m_CursorPos.Y )
		{
			if ( m_CursorPos.Y >= m_TextLineCnt )
				return;
			if ( m_SelectionEnd.X < m_CursorPos.X )
			{
				cursorPos = m_SelectionEnd;
				selectionEnd = m_CursorPos;
			}
			else
			{
				cursorPos = m_CursorPos;
				selectionEnd = m_SelectionEnd;
			}
		}
		else if ( m_SelectionEnd.Y < m_CursorPos.Y )
		{
			if ( m_CursorPos.Y >= m_TextLineCnt )
			{
				if ( m_SelectionEnd.Y >= m_TextLineCnt )
					return;
				m_CursorPos.Y = m_TextLineCnt - 1;
				m_CursorPos.X = m_TextLineLen[m_CursorPos.Y];
			}
			cursorPos = m_SelectionEnd;
			selectionEnd = m_CursorPos;
		}
		else
		{
			if ( m_SelectionEnd.Y >= m_TextLineCnt )
			{
				if ( m_CursorPos.Y >= m_TextLineCnt )
					return;
				m_SelectionEnd.Y = m_TextLineCnt - 1;
				m_SelectionEnd.X = m_TextLineLen[m_SelectionEnd.Y];
			}
			cursorPos = m_CursorPos;
			selectionEnd = m_SelectionEnd;
		}

		Ptr(CConsoleUndoItem) undoItem = 
			OK_NEW_OPERATOR CConsoleUndoItem(
			CConsoleUndoItem::UndoDeleteText,
			cursorPos, selectionEnd, GetSelection());

		m_UndoGroup->push(undoItem);

		_DeleteSelection();
	}
}

void CConsoleTextControl::_DeleteSelection(void)
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

	if ( bSelection )
	{
		if ( m_SelectionEnd.Y == m_CursorPos.Y )
		{
			if ( m_CursorPos.Y >= m_TextLineCnt )
				return;

			sdword lFront;
			sdword lRear;

			if ( m_SelectionEnd.X < m_CursorPos.X )
			{
				lFront = m_SelectionEnd.X;
				lRear = m_CursorPos.X;
			}
			else
			{
				lFront = m_CursorPos.X;
				lRear = m_SelectionEnd.X;
			}

			CPointer text0 = m_TextLines[m_CursorPos.Y];
			CPointer text1 = CastMutable(CPointer, GetTextPointer(text0, lFront));
			CPointer text2 = CastMutable(CPointer, GetTextPointer(text0, lRear));
			dword tlen2 = s_strlen(text2, m_TextLineLen[m_CursorPos.Y]);

			if ( *text1 == 0x09 )
			{
				sdword pos = lFront;
				sdword spacing = 4;
				sdword cpos = 0;
				CPointer text3;

				ReallocLine();
				text0 = m_TextLines[m_CursorPos.Y];
				text1 = CastMutable(CPointer, GetTextPointer(text0, lFront));
				text2 = CastMutable(CPointer, GetTextPointer(text0, lRear));
				while ( ((pos % 4) != 0) )
				{
					--pos; ++cpos;
					text3 = CastMutable(CPointer,GetTextPointer(text0, pos));
					if ( *text3 != 0x09 )
						--spacing;
				}
				if ( (spacing + cpos) > 4 )
				{
					sdword pos1 = (spacing + cpos) - 4;
					dword tlen1 = s_strlen(text1, m_TextLineLen[m_CursorPos.Y]);

					s_memmove(text1 + pos1, text1, (tlen1 + 1) * szchar);
					for ( sdword ix = pos1; ix > 0; --ix )
					{
						*text1++ = 0x20;
						++text2;
					}
				}
			}
			s_strcpy(text1, tlen2 + 1, text2);
			m_TextLineLen[m_CursorPos.Y] = GetTextLength(m_TextLines[m_CursorPos.Y]);
			m_CursorPos.X = m_SelectionEnd.X = lFront;
		}
		else if ( m_SelectionEnd.Y < m_CursorPos.Y )
		{
			if ( m_CursorPos.Y >= m_TextLineCnt )
			{
				if ( m_SelectionEnd.Y >= m_TextLineCnt )
					return;
				m_CursorPos.Y = m_TextLineCnt - 1;
				m_CursorPos.X = m_TextLineLen[m_CursorPos.Y];
			}

			bool bSelectionEnd = (m_SelectionEnd.X == 0);
			bool bCursorPos = (m_CursorPos.X == 0);
			sdword at = m_SelectionEnd.Y;
			sdword cnt = m_CursorPos.Y - m_SelectionEnd.Y - 1;

			if ( !bSelectionEnd )
				++at;
			if ( bSelectionEnd )
				++cnt;
			if ( cnt > 0 ) 
			{
				DeleteLines(at, cnt);
				if ( bSelectionEnd )
					m_CursorPos.Y = m_SelectionEnd.Y;
				else
					m_CursorPos.Y = m_SelectionEnd.Y + 1;
			}
			if ( !bSelectionEnd )
			{
				CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_SelectionEnd.Y], m_SelectionEnd.X));

				*text1 = 0;
				m_TextLineLen[m_SelectionEnd.Y] = GetTextLength(m_TextLines[m_SelectionEnd.Y]);
			}
			if ( !bCursorPos )
			{
				CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_CursorPos.Y], m_CursorPos.X));
				dword tlen1 = s_strlen(text1, m_TextLineLen[m_CursorPos.Y]);

				s_strcpy(m_TextLines[m_CursorPos.Y], tlen1 + 1, text1);
				m_TextLineLen[m_CursorPos.Y] = GetTextLength(m_TextLines[m_CursorPos.Y]);
			}
			if ( !bSelectionEnd )
			{
				m_CursorPos.Y = m_SelectionEnd.Y;
				m_CursorPos.X = m_SelectionEnd.X;
				JoinLines(false);
			}
			m_CursorPos.Y = m_SelectionEnd.Y;
			m_CursorPos.X = m_SelectionEnd.X;
		}
		else
		{
			if ( m_SelectionEnd.Y >= m_TextLineCnt )
			{
				if ( m_CursorPos.Y >= m_TextLineCnt )
					return;
				m_SelectionEnd.Y = m_TextLineCnt - 1;
				m_SelectionEnd.X = m_TextLineLen[m_SelectionEnd.Y];
			}

			bool bCursorPos = (m_CursorPos.X == 0);
			bool bSelectionEnd = (m_SelectionEnd.X == 0);
			sdword at = m_CursorPos.Y;
			sdword cnt = m_SelectionEnd.Y - m_CursorPos.Y - 1;

			if ( !bCursorPos )
				++at;
			if ( bCursorPos )
				++cnt;
			if ( cnt > 0 )
			{
				DeleteLines(at, cnt);
				if ( bCursorPos )
					m_SelectionEnd.Y = m_CursorPos.Y;
				else
					m_SelectionEnd.Y = m_CursorPos.Y + 1;
			}
			if ( !bCursorPos )
			{
				CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_CursorPos.Y], m_CursorPos.X));

				*text1 = 0;
				m_TextLineLen[m_CursorPos.Y] = GetTextLength(m_TextLines[m_CursorPos.Y]);
			}
			if ( !bSelectionEnd )
			{
				CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_SelectionEnd.Y], m_SelectionEnd.X));
				dword tlen1 = s_strlen(text1, m_TextLineLen[m_SelectionEnd.Y]);

				s_strcpy(m_TextLines[m_SelectionEnd.Y], tlen1 + 1, text1);
				m_TextLineLen[m_SelectionEnd.Y] = GetTextLength(m_TextLines[m_SelectionEnd.Y]);
			}
			if ( !bCursorPos )
			{
				JoinLines(false);
				m_SelectionEnd.Y = m_CursorPos.Y;
				m_SelectionEnd.X = m_CursorPos.X;
			}
			m_SelectionEnd.Y = m_CursorPos.Y;
			m_SelectionEnd.X = m_CursorPos.X;
		}
	}
}

CStringBuffer CConsoleTextControl::GetSelection()
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
	CStringBuffer result;

	if ( !bSelection )
		return result;

	if ( m_SelectionEnd.Y == m_CursorPos.Y )
	{
		if ( m_CursorPos.Y >= m_TextLineCnt )
			return result;

		CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_CursorPos.Y], (m_SelectionEnd.X < m_CursorPos.X)?m_SelectionEnd.X:m_CursorPos.X));
		CPointer text2 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_CursorPos.Y], (m_SelectionEnd.X < m_CursorPos.X)?m_CursorPos.X:m_SelectionEnd.X));
		dword tlen1 = s_strlen(text1, m_TextLineLen[m_CursorPos.Y]);
		dword tlen2 = s_strlen(text2, m_TextLineLen[m_CursorPos.Y]);

		result.AppendString(text1, tlen1 - tlen2);
	}
	else if ( m_SelectionEnd.Y < m_CursorPos.Y )
	{
		if ( m_CursorPos.Y >= m_TextLineCnt )
		{
			if ( m_SelectionEnd.Y >= m_TextLineCnt )
				return result;
			m_CursorPos.Y = m_TextLineCnt - 1;
			m_CursorPos.X = m_TextLineLen[m_CursorPos.Y];
		}

		bool bSelectionEnd = (m_SelectionEnd.X == 0);
		bool bCursorPos = (m_CursorPos.X == 0);
		sdword at = m_SelectionEnd.Y;
		sdword cnt = m_CursorPos.Y - m_SelectionEnd.Y - 1;

		if ( !bSelectionEnd )
			++at;
		if ( bSelectionEnd )
			++cnt;
		for ( sdword ix = at; ix < (at + cnt); ++ix )
		{
			if ( ix > at )
				result.AppendString(_T("\r\n"));
			result.AppendString(m_TextLines[ix]);
		}
		if ( !bSelectionEnd )
		{
			CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_SelectionEnd.Y], m_SelectionEnd.X));

			if ( cnt > 0 )
				result.PrependString(_T("\r\n"));
			result.PrependString(text1);
		}
		if ( !bCursorPos )
		{
			CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_CursorPos.Y], m_CursorPos.X));
			dword tlen1 = s_strlen(text1, m_TextLineLen[m_CursorPos.Y]);
			dword tlen2 = s_strlen(m_TextLines[m_CursorPos.Y], m_TextLineLen[m_CursorPos.Y]);

			if ( (cnt > 0) || (!bSelectionEnd) )
				result.AppendString(_T("\r\n"));
			result.AppendString(m_TextLines[m_CursorPos.Y], tlen2 - tlen1);
		}
		else
			result.AppendString(_T("\r\n"));
	}
	else
	{
		if ( m_SelectionEnd.Y >= m_TextLineCnt )
		{
			if ( m_CursorPos.Y >= m_TextLineCnt )
				return result;
			m_SelectionEnd.Y = m_TextLineCnt - 1;
			m_SelectionEnd.X = m_TextLineLen[m_SelectionEnd.Y];
		}

		bool bCursorPos = (m_CursorPos.X == 0);
		bool bSelectionEnd = (m_SelectionEnd.X == 0);
		sdword at = m_CursorPos.Y;
		sdword cnt = m_SelectionEnd.Y - m_CursorPos.Y - 1;

		if ( !bCursorPos )
			++at;
		if ( bCursorPos )
			++cnt;
		for ( sdword ix = at; ix < (at + cnt); ++ix )
		{
			if ( ix > at )
				result.AppendString(_T("\r\n"));
			result.AppendString(m_TextLines[ix]);
		}
		if ( !bCursorPos )
		{
			CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_CursorPos.Y], m_CursorPos.X));

			if ( cnt > 0 )
				result.PrependString(_T("\r\n"));
			result.PrependString(text1);
		}
		if ( !bSelectionEnd )
		{
			CPointer text1 = CastMutable(CPointer, GetTextPointer(m_TextLines[m_SelectionEnd.Y], m_SelectionEnd.X));
			dword tlen1 = s_strlen(text1, m_TextLineLen[m_SelectionEnd.Y]);
			dword tlen2 = s_strlen(m_TextLines[m_SelectionEnd.Y], m_TextLineLen[m_SelectionEnd.Y]);

			if ( (cnt > 0) || (!bCursorPos) )
				result.AppendString(_T("\r\n"));
			result.AppendString(m_TextLines[m_SelectionEnd.Y], tlen2 - tlen1);
		}
		else
			result.AppendString(_T("\r\n"));
	}
	return result;
}

void CConsoleTextControl::InsertText(ConstRef(CStringBuffer) _text)
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

	assert(!bSelection);

	_InsertText(_text);

	Ptr(CConsoleUndoItem) undoItem = 
		OK_NEW_OPERATOR CConsoleUndoItem(
		CConsoleUndoItem::UndoInsertText,
		m_SelectionEnd, m_CursorPos, _text);

	m_UndoGroup->push(undoItem);
}

void CConsoleTextControl::_InsertText(ConstRef(CStringBuffer) _text)
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

	assert(!bSelection);

	CStringBuffer text = _text;
	CStringConstIterator it = text.Begin();
	CArray textLines;
	dword delimcnt[4];
	dword cnt;
	word ix;
	word mx;

	for ( ix = 0; ix < 4; ++ix )
		delimcnt[ix] = 0;
	it.FirstOf(_T("\r\n"));
	while ( !(it.IsEnd()) )
	{
		if ( (it[0] == _T('\r')) && (it[1] == _T('\n')) )
		{
			++it; ++it;
			++(delimcnt[0]);
		}
		else if ( (it[0] == _T('\n')) && (it[1] == _T('\r')) )
		{
			++it; ++it;
			++(delimcnt[1]);
		}
		else if ( it[0] == _T('\r') )
		{
			++it;
			++(delimcnt[2]);
		}
		else
		{
			++it;
			++(delimcnt[3]);
		}
		it.FirstOf(_T("\r\n"));
	}
	cnt = 0;
	mx = 4;
	for ( ix = 0; ix < 4; ++ix )
	{
		if ( delimcnt[ix] > cnt )
		{
			cnt = delimcnt[ix];
			mx = ix;
		}
	}
	++cnt;
	textLines = CastAny(CArray, TFalloc(cnt * szPointer));
	switch ( mx )
	{
	case 0:
		text.Split(_T("\r\n"), textLines, cnt, &cnt);
		break;
	case 1:
		text.Split(_T("\n\r"), textLines, cnt, &cnt);
		break;
	case 2:
		text.Split(_T("\r"), textLines, cnt, &cnt);
		break;
	case 3:
		text.Split(_T("\n"), textLines, cnt, &cnt);
		break;
	case 4:
		textLines[0] = CastMutable(CPointer, text.GetString());
		break;
	}

	CPointer t0;
	CPointer t1 = CastMutable(CPointer, _T(""));
	dword tlen0;
	dword tlen1;

	while ( m_CursorPos.Y >= m_TextLineCnt )
		InsertLines(m_TextLineCnt, 1, &t1);
	if ( m_CursorPos.X > m_TextLineLen[m_CursorPos.Y] )
	{
		ReallocLine(m_CursorPos.X, m_CursorPos.X);
		t1 = m_TextLines[m_CursorPos.Y];
		for ( sdword ix = 0; ix < m_CursorPos.X; ++ix )
			*t1++ = 0x20;
		m_TextLineLen[m_CursorPos.Y] = GetTextLength(m_TextLines[m_CursorPos.Y]);
	}
	if ( cnt == 1 )
	{
		tlen0 = s_strlen(textLines[0], INT_MAX);
		if ( tlen0 > 0 )
		{
			ReallocLine(tlen0 + 4, tlen0 + 8);
			t0 = m_TextLines[m_CursorPos.Y];
			t1 = CastMutable(CPointer, GetTextPointer(t0, m_CursorPos.X));
			tlen1 = s_strlen(t1, m_TextLineLen[m_CursorPos.Y]);
			if ( (*t1 == 0x09) && (textLines[0][0] != 0x09) )
			{
				sdword pos = m_CursorPos.X;
				sdword spacing = 4;
				sdword cpos = 0;
				CPointer text2;

				while ( ((pos % 4) != 0) )
				{
					--pos; ++cpos;
					text2 = CastMutable(CPointer,GetTextPointer(t0, pos));
					if ( *text2 != 0x09 )
						--spacing;
				}
				if ( (spacing + cpos) > 4 )
				{
					sdword pos1 = (spacing + cpos) - 4;

					s_memmove(t1 + (pos1 + tlen0), t1, tlen1 * szchar);
					for ( sdword ix = pos1; ix > 0; --ix )
						*t1++ = 0x20;
					s_memmove(t1, textLines[0], tlen0 * szchar);
				}
				else
				{
					s_memmove(t1 + tlen0, t1, tlen1 * szchar);
					s_memmove(t1, textLines[0], tlen0 * szchar);
				}
			}
			else
			{
				s_memmove(t1 + tlen0, t1, tlen1 * szchar);
				s_memmove(t1, textLines[0], tlen0 * szchar);
			}
			while ( tlen0 > 0 )
			{
				switch ( *t1++ )
				{
				case 0x09:
					m_CursorPos.X = ((m_CursorPos.X + 4) / 4) * 4;
					break;
				default:
					++(m_CursorPos.X);
					break;
				}
				--tlen0;
			}
			m_TextLineLen[m_CursorPos.Y] = GetTextLength(t0);
			if ( m_TextLineLen[m_CursorPos.Y] > m_TextLineLenMax )
				m_TextLineLenMax = m_TextLineLen[m_CursorPos.Y];
		}
	}
	else
	{
		SplitLine();
		--(m_CursorPos.Y);
		tlen0 = s_strlen(textLines[0], INT_MAX);
		ReallocLine(tlen0, tlen0);
		t1 = m_TextLines[m_CursorPos.Y];
		tlen1 = s_strlen(t1, m_TextLineLen[m_CursorPos.Y]);
		s_strcpy(t1 + tlen1, tlen0 + 1, textLines[0]);
		m_TextLineLen[m_CursorPos.Y] = GetTextLength(t1);
		if ( m_TextLineLen[m_CursorPos.Y] > m_TextLineLenMax )
			m_TextLineLenMax = m_TextLineLen[m_CursorPos.Y];
		++(m_CursorPos.Y);
		if ( cnt > 2 )
		{
			s_memmove(textLines, textLines + 1, (cnt - 1) * szPointer);
			--cnt;
			InsertLines(m_CursorPos.Y, cnt - 1, textLines);
			m_CursorPos.Y += (cnt - 1);
		}
		tlen0 = s_strlen(textLines[cnt - 1], INT_MAX);
		ReallocLine(tlen0, tlen0);
		t1 = m_TextLines[m_CursorPos.Y];
		tlen1 = s_strlen(t1, m_TextLineLen[m_CursorPos.Y]);
		s_memmove(t1 + tlen0, t1, tlen1 * szchar);
		s_memmove(t1, textLines[cnt - 1], tlen0 * szchar);
		m_TextLineLen[m_CursorPos.Y] = GetTextLength(t1);
		if ( m_TextLineLen[m_CursorPos.Y] > m_TextLineLenMax )
			m_TextLineLenMax = m_TextLineLen[m_CursorPos.Y];
		m_CursorPos.X = 0;
		while ( tlen0 > 0 )
		{
			switch ( *t1++ )
			{
			case 0x09:
				m_CursorPos.X = ((m_CursorPos.X + 4) / 4) * 4;
				break;
			default:
				++(m_CursorPos.X);
				break;
			}
			--tlen0;
		}
	}
	TFfree(textLines);
}

void CConsoleTextControl::SetCursorPosEx()
{
	COORD pos;
	bool bHideCursor = false;

	pos.X = Cast(sword, m_CursorPos.X - m_ScrollPos.X) + m_ClientArea.Left;
	if ( (pos.X < m_ClientArea.Left) || (pos.X >= m_ClientArea.Right) )
		bHideCursor = true;
	pos.Y = Cast(sword, m_CursorPos.Y - m_ScrollPos.Y) + m_ClientArea.Top;
	if ( (pos.Y < m_ClientArea.Top) || (pos.Y >= m_ClientArea.Bottom) )
		bHideCursor = true;
	if ( bHideCursor )
		HideCursor();
	else
	{
		ShowCursor();
		SetCursorPos(pos);
	}
}

CPointer CConsoleTextControl::ReallocLine(dword limit, dword expand)
{
	CPointer text = m_TextLines[m_CursorPos.Y];
	dword tlen = s_strlen(text, m_TextLineLen[m_CursorPos.Y]);
	CPointer text2;

	assert(limit <= expand);
	if ( m_TextLineAlloc[m_CursorPos.Y] )
	{
		if ( TFsize(text) < ((tlen + 1 + limit) * szchar) )
			text2 = CastAnyPtr(mbchar, TFrealloc(text, (tlen + 1 + expand) * szchar));
		else
			text2 = text;
	}
	else
	{
		text2 = CastAnyPtr(mbchar, TFalloc((tlen + 1 + expand) * szchar));
		m_TextLineAlloc[m_CursorPos.Y] = true;
		s_strcpy(text2, tlen + 1, text);
	}
	m_TextLines[m_CursorPos.Y] = text2;
	return text2;
}

void CConsoleTextControl::JoinLines(bool bBeforeCurrent)
{
	if ( bBeforeCurrent )
	{
		if ( m_CursorPos.Y == 0 )
			return;
		--m_CursorPos.Y;
	}
	else
	{
		if ( m_CursorPos.Y >= (m_TextLineCnt - 1) )
			return;
	}

	dword tlen2 = s_strlen(m_TextLines[m_CursorPos.Y + 1], m_TextLineLen[m_CursorPos.Y + 1]);
	
	if ( tlen2 > 0 )
	{
		dword tlen1 = s_strlen(m_TextLines[m_CursorPos.Y], m_TextLineLen[m_CursorPos.Y]);
		CPointer text = ReallocLine(tlen2, tlen2);

		s_strcpy(text + tlen1, tlen2 + 1, m_TextLines[m_CursorPos.Y + 1]);
		m_CursorPos.X = m_TextLineLen[m_CursorPos.Y];
		m_TextLineLen[m_CursorPos.Y] = GetTextLength(m_TextLines[m_CursorPos.Y]);
	}
	else
		m_CursorPos.X = m_TextLineLen[m_CursorPos.Y];
	DeleteLines(m_CursorPos.Y + 1, 1);
}

void CConsoleTextControl::SplitLine(void)
{
	if ( PtrCheck(m_TextLines) )
		return;
	if ( m_CursorPos.Y >= (m_TextLineCnt - 1) )
		return;
	if ( m_CursorPos.X > m_TextLineLen[m_CursorPos.Y] )
		return;

	CPointer text = CastMutable(CPointer, GetTextPointer(m_TextLines[m_CursorPos.Y], m_CursorPos.X));
	CStringBuffer tmp(__FILE__LINE__ text);
	CPointer buf[1];

	buf[0] = CastMutable(CPointer, tmp.GetString());
	InsertLines(m_CursorPos.Y + 1, 1, buf);
	text = CastMutable(CPointer, GetTextPointer(m_TextLines[m_CursorPos.Y], m_CursorPos.X));
	*text = 0;
	m_TextLineLen[m_CursorPos.Y] = GetTextLength(m_TextLines[m_CursorPos.Y]);
	++m_CursorPos.Y;
	m_CursorPos.X = 0;
}

void CConsoleTextControl::DeleteLines(sdword at, sdword cnt)
{
	if ( PtrCheck(m_TextLines) )
		return;
	if ( cnt == 0 )
		return;
	assert(at <= m_TextLineCnt);
	assert((at + cnt) <= m_TextLineCnt);

	for ( sdword ix = at; ix < (at + cnt); ++ix )
		if ( m_TextLineAlloc[ix] )
			TFfree(m_TextLines[ix]);
	if ( cnt == m_TextLineCnt )
	{
		m_Text.SetString(__FILE__LINE__ _T(""));
		TFfree(m_TextLines);
		m_TextLines = CastAny(CArray, TFalloc(szPointer));
		m_TextLines[0] = CastMutable(CPointer, m_Text.GetString());
		TFfree(m_TextLineLen);
		m_TextLineLen = CastAnyPtr(sdword, TFalloc(sizeof(sdword)));
		TFfree(m_TextLineAlloc);
		m_TextLineAlloc = CastAnyPtr(bool, TFalloc(sizeof(bool)));
		m_TextLineAlloc[0] = false;
		m_TextLineCnt = 1;
		m_TextLineLenMax = 0;
		return;
	}
	s_memmove(m_TextLines + at, m_TextLines + (at + cnt), (m_TextLineCnt - (at + cnt)) * szPointer);
	s_memmove(m_TextLineLen + at, m_TextLineLen + (at + cnt), (m_TextLineCnt - (at + cnt)) * sizeof(sdword));
	s_memmove(m_TextLineAlloc + at, m_TextLineAlloc + (at + cnt), (m_TextLineCnt - (at + cnt)) * sizeof(bool));
	if ( (m_TextLineCnt / cnt) < 5 )
	{
		m_TextLines = CastAny(CArray, TFrealloc(m_TextLines, (m_TextLineCnt - cnt) * szPointer));
		m_TextLineLen = CastAnyPtr(sdword, TFrealloc(m_TextLineLen, (m_TextLineCnt - cnt) * sizeof(sdword)));
		m_TextLineAlloc = CastAnyPtr(bool, TFrealloc(m_TextLineAlloc, (m_TextLineCnt - cnt) * sizeof(bool)));
	}
	m_TextLineCnt -= cnt;
}

void CConsoleTextControl::InsertLines(sdword at, sdword cnt, CArray texts)
{
	if ( PtrCheck(m_TextLines) )
		return;
	assert(at <= m_TextLineCnt);

	if ( TFsize(m_TextLines) < ((m_TextLineCnt + cnt) * szPointer) )
	{
		m_TextLines = CastAny(CArray, TFrealloc(m_TextLines, (m_TextLineCnt + cnt + 8) * szPointer));
		m_TextLineLen = CastAnyPtr(sdword, TFrealloc(m_TextLineLen, (m_TextLineCnt + cnt + 8) * sizeof(sdword)));
		m_TextLineAlloc = CastAnyPtr(bool, TFrealloc(m_TextLineAlloc, (m_TextLineCnt + cnt + 8) * sizeof(bool)));
	}
	s_memmove(m_TextLines + (at + cnt), m_TextLines + at, (m_TextLineCnt - at) * szPointer);
	s_memmove(m_TextLineLen + (at + cnt), m_TextLineLen + at, (m_TextLineCnt - at) * sizeof(sdword));
	s_memmove(m_TextLineAlloc + (at + cnt), m_TextLineAlloc + at, (m_TextLineCnt - at) * sizeof(bool));
	for ( sdword ix = at; ix < (at + cnt); ++ix )
	{
		CPointer text = texts[ix - at];
		dword tlen = s_strlen(text, INT_MAX);

		m_TextLines[ix] = CastAnyPtr(mbchar, TFalloc((tlen + 9) * szchar));
		s_strcpy(m_TextLines[ix], tlen + 1, text);
		m_TextLineLen[ix] = GetTextLength(m_TextLines[ix]);
		if ( m_TextLineLen[ix] > m_TextLineLenMax )
			m_TextLineLenMax = m_TextLineLen[ix];
		m_TextLineAlloc[ix] = true;
	}
    m_TextLineCnt += cnt;
}

void CConsoleTextControl::_InsertKey(TCHAR ch)
{
	if ( PtrCheck(m_TextLines) )
		return;

	if ( ch == 0x0D )
	{
		SplitLine();
		return;
	}

	CPointer text = ReallocLine();
	CPointer text1 = CastMutable(CPointer,GetTextPointer(text, m_CursorPos.X));
	dword tlen1 = s_strlen(text1, m_TextLineLen[m_CursorPos.Y]);

	if ( (*text1 == 0x09) && (ch != 0x09) )
	{
		sdword pos = m_CursorPos.X;
		sdword spacing = 4;
		sdword cpos = 0;
		CPointer text2;

		while ( (pos % 4) != 0 )
		{
			--pos; ++cpos;
			text2 = CastMutable(CPointer,GetTextPointer(text, pos));
			if ( *text2 != 0x09 )
				--spacing;
		}
		if ( (spacing + cpos) > 4 )
		{
			sdword pos1 = (spacing + cpos) - 4;

			s_memmove(text1 + (pos1 + 1), text1, (tlen1 + 1) * szchar);
			for ( sdword ix = pos1; ix > 0; --ix )
				*text1++ = 0x20;
			*text1 = ch;
		}
		else
		{
			s_memmove(text1 + 1, text1, (tlen1 + 1) * szchar);
			*text1 = ch;
		}
	}
	else
	{
		s_memmove(text1 + 1, text1, (tlen1 + 1) * szchar);
		*text1 = ch;
	}
	m_TextLineLen[m_CursorPos.Y] = GetTextLength(text);
	if ( m_TextLineLen[m_CursorPos.Y] > m_TextLineLenMax )
		m_TextLineLenMax = m_TextLineLen[m_CursorPos.Y];
	if ( ch == 0x09 )
		m_CursorPos.X = ((m_CursorPos.X + 4) / 4) * 4;
	else
		++(m_CursorPos.X);
}

void CConsoleTextControl::InsertKey(TCHAR ch)
{
	if ( PtrCheck(m_TextLines) )
		return;

	if ( ch == 0x0D )
	{
		Ptr(CConsoleUndoItem) undoItem = 
			OK_NEW_OPERATOR CConsoleUndoItem(
			CConsoleUndoItem::UndoInsertKey,
			m_CursorPos, CStringBuffer(__FILE__LINE__ _T("\r\n")));

		m_UndoGroup->push(undoItem);
		_InsertKey(ch);
		return;
	}

	TCHAR buf[2];

	buf[0] = ch;
	buf[1] = 0;

	Ptr(CConsoleUndoItem) undoItem = 
		OK_NEW_OPERATOR CConsoleUndoItem(
		CConsoleUndoItem::UndoInsertKey,
		m_CursorPos, CStringBuffer(__FILE__LINE__ buf));

	m_UndoGroup->push(undoItem);
	_InsertKey(ch);
}

void CConsoleTextControl::_DeleteKey(bool bBackSpace)
{
	if ( PtrCheck(m_TextLines) )
		return;
	if ( m_CursorPos.Y >= m_TextLineCnt )
		return;

	if ( bBackSpace && (m_CursorPos.X == 0) )
	{
		if ( m_CursorPos.Y == 0 )
			return;
		JoinLines();
		return;
	}
	if ( (!bBackSpace) && (m_CursorPos.X >= m_TextLineLen[m_CursorPos.Y]) )
	{
		if ( m_CursorPos.Y >= (m_TextLineCnt - 1) )
			return;
		JoinLines(false);
		return;
	}

	CPointer text = m_TextLines[m_CursorPos.Y];
	sdword pos = bBackSpace?(m_CursorPos.X - 1):m_CursorPos.X;
	CPointer text1 = CastMutable(CPointer,GetTextPointer(text, pos));
	dword tlen = s_strlen(text1, m_TextLineLen[m_CursorPos.Y]);

	if ( *text1 == 0x09 )
	{
		sdword spacing = 4;
		sdword cpos = 0;
		sdword scnt = 0;
		CPointer text2;

		while ( (pos % 4) != 0 )
		{
			--pos; ++cpos;
			text2 = CastMutable(CPointer,GetTextPointer(text, pos));
			if ( *text2 == 0x20 )
			{
				++scnt;
				--spacing;
			}
			else if ( *text2 != 0x09 )
				--spacing;
		}
		m_CursorPos.X -= ((spacing + cpos) - 4);
	}
	s_memmove(text1, text1 + 1, tlen * szchar);
	if ( bBackSpace )
		--m_CursorPos.X;
	if ( m_CursorPos.X < 0 )
		m_CursorPos.X = 0;
	m_TextLineLen[m_CursorPos.Y] = GetTextLength(text);
}

void CConsoleTextControl::DeleteKey(bool bBackSpace)
{
	if ( PtrCheck(m_TextLines) )
		return;
	if ( m_CursorPos.Y >= m_TextLineCnt )
		return;

	if ( bBackSpace && (m_CursorPos.X == 0) )
	{
		if ( m_CursorPos.Y == 0 )
			return;
		_DeleteKey(bBackSpace);

		Ptr(CConsoleUndoItem) undoItem = 
			OK_NEW_OPERATOR CConsoleUndoItem(
			CConsoleUndoItem::UndoDeleteKey, 
			m_CursorPos, CStringBuffer(__FILE__LINE__ _T("\r\n")));

		m_UndoGroup->push(undoItem);
		return;
	}
	if ( (!bBackSpace) && (m_CursorPos.X >= m_TextLineLen[m_CursorPos.Y]) )
	{
		if ( m_CursorPos.Y >= (m_TextLineCnt - 1) )
			return;
		_DeleteKey(bBackSpace);

		Ptr(CConsoleUndoItem) undoItem = 
			OK_NEW_OPERATOR CConsoleUndoItem(
			CConsoleUndoItem::UndoDeleteKey, 
			m_CursorPos, CStringBuffer(__FILE__LINE__ _T("\r\n")));

		m_UndoGroup->push(undoItem);
		return;
	}

	CPointer text = m_TextLines[m_CursorPos.Y];
	sdword pos = bBackSpace?(m_CursorPos.X - 1):m_CursorPos.X;
	CPointer text1 = CastMutable(CPointer,GetTextPointer(text, pos));

	TCHAR buf[2];

	buf[0] = *text1;
	buf[1] = 0;

	_DeleteKey(bBackSpace);

	Ptr(CConsoleUndoItem) undoItem = 
		OK_NEW_OPERATOR CConsoleUndoItem(
		CConsoleUndoItem::UndoDeleteKey, 
		m_CursorPos, CStringBuffer(__FILE__LINE__ buf));

	m_UndoGroup->push(undoItem);
}

void CConsoleTextControl::Create(COORD pos, COORD size)
{
	m_Color = m_Console->GetDefaultColor();
	m_HighLightColor = m_Console->GetDefaultHighlightedColor();
	Initialize(true);
	CConsoleWindow::Create(pos, size);

	DrawTextControl();
	SetCursorPosEx();
}

void CConsoleTextControl::SetFocus(bool hasFocus)
{
	if ( hasFocus )
		SetCursorPosEx();
	else
		HideCursor();
}

void CConsoleTextControl::Resize()
{
	if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		m_ScrollBarVInfo.SetScrollBarInfo(m_TextLineCnt, m_ClientAreaSize.Y);
	if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		m_ScrollBarHInfo.SetScrollBarInfo(m_TextLineLenMax, m_ClientAreaSize.X);
	Scroll();
}

void CConsoleTextControl::Scroll()
{
	if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		m_ScrollPos.Y = m_ScrollBarVInfo.Current;
	if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		m_ScrollPos.X = m_ScrollBarHInfo.Current;
	DrawTextControl();
	SetCursorPosEx();
}

bool CConsoleTextControl::CheckVScroll(void)
{
	if ( (m_CursorPos.Y < m_ScrollPos.Y) || (m_CursorPos.Y >= (m_ScrollPos.Y + m_ClientAreaSize.Y)) )
	{
		m_ScrollPos.Y = m_CursorPos.Y - (m_ClientAreaSize.Y / 2);
		if ( m_ScrollPos.Y < 0 )
			m_ScrollPos.Y = 0;
		if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		{
			m_ScrollBarVInfo.Current = m_ScrollPos.Y;
			DrawVerticalScrollBar();
		}
		return true;
	}
	return false;
}

bool CConsoleTextControl::CheckHScroll(void)
{
	if ( (m_CursorPos.X < m_ScrollPos.X) || (m_CursorPos.X >= (m_ScrollPos.X + m_ClientAreaSize.X)) )
	{
		m_ScrollPos.X = m_CursorPos.X - (m_ClientAreaSize.X / 2);
		if ( m_ScrollPos.X < 0 )
			m_ScrollPos.X = 0;
		if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
		{
			m_ScrollBarHInfo.Current = m_ScrollPos.X;
			DrawHorizontalScrollBar();
		}
		return true;
	}
	return false;
}

void CConsoleTextControl::Undo()
{
	if ( PtrCheck(m_UndoGroup) || (m_UndoGroup->IsEmpty()) )
		return;

	Ptr(CConsoleUndoItem) undoItem = m_UndoGroup->undo();

	if ( PtrCheck(undoItem) )
		return;

	switch ( undoItem->GetType() )
	{
	case CConsoleUndoItem::UndoDeleteKey:
		{
			TCHAR ch;

			if ( undoItem->GetText() == CStringLiteral(_T("\r\n")) )
				ch = 0x0D;
			else
				ch = undoItem->GetText()[0];
			m_CursorPos = undoItem->GetCursorPos();
			_InsertKey(ch);
			m_SelectionEnd = m_CursorPos;
			Update();
		}
		break;
	case CConsoleUndoItem::UndoInsertKey:
		m_CursorPos = undoItem->GetCursorPos();
		_DeleteKey();
		m_SelectionEnd = m_CursorPos;
		Update();
		break;
	case CConsoleUndoItem::UndoDeleteText:
		m_CursorPos = undoItem->GetCursorPos();
		m_SelectionEnd = m_CursorPos;
		_InsertText(undoItem->GetText());
		Update();
		break;
	case CConsoleUndoItem::UndoInsertText:
		m_CursorPos = undoItem->GetCursorPos();
		m_SelectionEnd = undoItem->GetSelectionEnd();
		_DeleteSelection();
		Update();
		break;
	default:
		break;
	}
}

void CConsoleTextControl::Redo()
{
	if ( PtrCheck(m_UndoGroup) || (m_UndoGroup->IsEmpty()) )
		return;

	Ptr(CConsoleUndoItem) redoItem = m_UndoGroup->redo();

	if ( PtrCheck(redoItem) )
		return;

	switch ( redoItem->GetType() )
	{
	case CConsoleUndoItem::UndoDeleteKey:
		m_CursorPos = redoItem->GetCursorPos();
		_DeleteKey();
		m_SelectionEnd = m_CursorPos;
		Update();
		break;
	case CConsoleUndoItem::UndoInsertKey:
		{
			TCHAR ch;

			if ( redoItem->GetText() == CStringLiteral(_T("\r\n")) )
				ch = 0x0D;
			else
				ch = redoItem->GetText()[0];
			m_CursorPos = redoItem->GetCursorPos();
			_InsertKey(ch);
			m_SelectionEnd = m_CursorPos;
			Update();
		}
		break;
	case CConsoleUndoItem::UndoDeleteText:
		m_CursorPos = redoItem->GetCursorPos();
		m_SelectionEnd = redoItem->GetSelectionEnd();
		_DeleteSelection();
		Update();
		break;
	case CConsoleUndoItem::UndoInsertText:
		m_CursorPos = redoItem->GetCursorPos();
		m_SelectionEnd = m_CursorPos;
		_InsertText(redoItem->GetText());
		Update();
		break;
	default:
		break;
	}
}

void CConsoleTextControl::Cut()
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

	if ( bSelection )
	{
		CStringBuffer tmp = GetSelection();

		CConsoleClipboard::SetClipboardText(tmp);
		DeleteSelection();
		Update();
	}
}

void CConsoleTextControl::Copy()
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

	if ( bSelection )
	{
		CStringBuffer tmp = GetSelection();

		CConsoleClipboard::SetClipboardText(tmp);
	}
}

void CConsoleTextControl::Paste()
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

	if ( bSelection )
		DeleteSelection();

	CStringBuffer tmp = CConsoleClipboard::GetClipboardText();

	InsertText(tmp);
	Update();
}

void CConsoleTextControl::Delete()
{
	bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

	if ( bSelection )
	{
		DeleteSelection();
		Update();
	}
}

bool CConsoleTextControl::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	if ( !m_TextLines )
		return true;

	switch ( virtualKeyCode )
	{
    CASE_KEY_HOME:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			m_CursorPos.X = 0;
			m_ScrollPos.X = 0;
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			CheckVScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0) )
		{
			m_CursorPos.X = 0;
			m_CursorPos.Y = 0;
			m_ScrollPos.X = 0;
			m_ScrollPos.Y = 0;
			m_SelectionEnd.X = m_CursorPos.X;
			m_SelectionEnd.Y = m_CursorPos.Y;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarVInfo.Current = m_ScrollPos.Y;
				DrawVerticalScrollBar();
			}
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.X = 0;
			m_ScrollPos.X = 0;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			CheckVScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.X = 0;
			m_CursorPos.Y = 0;
			m_ScrollPos.X = 0;
			m_ScrollPos.Y = 0;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarVInfo.Current = m_ScrollPos.Y;
				DrawVerticalScrollBar();
			}
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_END:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			m_CursorPos.X = m_TextLineLen[m_CursorPos.Y];
			CheckHScroll();
			CheckVScroll();
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			m_CursorPos.Y = m_TextLineCnt;
			m_ScrollPos.Y = m_TextLineCnt - (m_ClientAreaSize.Y / 2);
			m_CursorPos.X = 0;
			m_ScrollPos.X = 0;
			m_SelectionEnd.X = m_CursorPos.X;
			m_SelectionEnd.Y = m_CursorPos.Y;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarVInfo.Current = m_ScrollPos.Y;
				DrawVerticalScrollBar();
			}
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.X = m_TextLineLen[m_CursorPos.Y];
			CheckHScroll();
			CheckVScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_TextLineCnt;
			m_ScrollPos.Y = m_TextLineCnt - (m_ClientAreaSize.Y / 2);
			m_CursorPos.X = 0;
			m_ScrollPos.X = 0;
			if ( m_hasScrollbar && ((m_ScrollBarStyle == horizontalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarHInfo.Current = m_ScrollPos.X;
				DrawHorizontalScrollBar();
			}
			if ( m_hasScrollbar && ((m_ScrollBarStyle == verticalscrollbarstyle) || (m_ScrollBarStyle == bothscrollbarstyle)) )
			{
				m_ScrollBarVInfo.Current = m_ScrollPos.Y;
				DrawVerticalScrollBar();
			}
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_LEFT:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			if ( m_CursorPos.X > 0 )
				--(m_CursorPos.X);
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			else
				bUpdate = true;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			bool bUpdate = false;

			if ( m_CursorPos.X > 0 )
			{
				--(m_CursorPos.X);
				bUpdate = true;
			}
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
		}
		break;
	CASE_KEY_RIGHT:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			if ( m_CursorPos.X < m_TextLineLen[m_CursorPos.Y] )
				++(m_CursorPos.X);
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			else
				bUpdate = true;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			bool bUpdate = false;

			if ( m_CursorPos.X < m_TextLineLen[m_CursorPos.Y] )
			{
				++(m_CursorPos.X);
				bUpdate = true;
			}
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
		}
		break;
	CASE_KEY_UP:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			if ( m_CursorPos.Y > 0 )
				--(m_CursorPos.Y);
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			else
				bUpdate = true;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			m_CursorPos.Y = m_ScrollPos.Y;
			m_SelectionEnd.Y = m_CursorPos.Y;
			m_SelectionEnd.X = m_CursorPos.X;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			if ( m_CursorPos.Y > 0 )
				--(m_CursorPos.Y);
			CheckVScroll();
			CheckHScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y;
			CheckHScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_DOWN:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			if ( m_CursorPos.Y < (m_TextLineCnt - 1) )
				++(m_CursorPos.Y);
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			else
				bUpdate = true;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			m_CursorPos.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			if ( m_CursorPos.Y > m_TextLineCnt )
				m_CursorPos.Y = m_TextLineCnt;
			m_SelectionEnd.Y = m_CursorPos.Y;
			m_SelectionEnd.X = m_CursorPos.X;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			if ( m_CursorPos.Y < (m_TextLineCnt - 1) )
				++(m_CursorPos.Y);
			CheckHScroll();
			CheckVScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			if ( m_CursorPos.Y > m_TextLineCnt )
				m_CursorPos.Y = m_TextLineCnt;
			CheckHScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_PAGEDOWN:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			if ( m_CursorPos.Y < (m_TextLineCnt - 1) )
			{
				if ( m_CursorPos.Y != (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2)) )
					m_CursorPos.Y = (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2));
				m_CursorPos.Y += m_ClientAreaSize.Y - 2;
				if ( m_CursorPos.Y > m_TextLineCnt )
					m_CursorPos.Y = m_TextLineCnt;
			}
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			else
				bUpdate = true;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			m_CursorPos.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			if ( m_CursorPos.Y > m_TextLineCnt )
				m_CursorPos.Y = m_TextLineCnt;
			m_SelectionEnd.Y = m_CursorPos.Y;
			m_SelectionEnd.X = m_CursorPos.X;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			if ( m_CursorPos.Y < (m_TextLineCnt - 1) )
			{
				if ( m_CursorPos.Y != (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2)) )
					m_CursorPos.Y = (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2));
				m_CursorPos.Y += m_ClientAreaSize.Y - 2;
				if ( m_CursorPos.Y > m_TextLineCnt )
					m_CursorPos.Y = m_TextLineCnt;
			}
			CheckHScroll();
			CheckVScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			if ( m_CursorPos.Y > m_TextLineCnt )
				m_CursorPos.Y = m_TextLineCnt;
			CheckHScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
	CASE_KEY_PAGEUP:
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			if ( m_CursorPos.Y >= 0 )
			{
				if ( m_CursorPos.Y != (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2)) )
					m_CursorPos.Y = (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2));
				m_CursorPos.Y -= m_ClientAreaSize.Y - 2;
				if ( m_CursorPos.Y < 0 )
					m_CursorPos.Y = 0;
			}
			if ( !m_SelectionMode )
			{
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			else
				bUpdate = true;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( CheckVScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) > 0) )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
			bool bUpdate = false;

			m_CursorPos.Y = m_ScrollPos.Y;
			m_SelectionEnd.Y = m_CursorPos.Y;
			m_SelectionEnd.X = m_CursorPos.X;
			if ( CheckHScroll() )
				bUpdate = true;
			if ( bSelection || bUpdate )
			{
				DrawTextControl();
				PostPaintEvent();
			}
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			if ( m_CursorPos.Y >= 0 )
			{
				if ( m_CursorPos.Y != (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2)) )
					m_CursorPos.Y = (m_ScrollPos.Y + (m_ClientAreaSize.Y / 2));
				m_CursorPos.Y -= m_ClientAreaSize.Y - 2;
				if ( m_CursorPos.Y < 0 )
					m_CursorPos.Y = 0;
			}
			CheckHScroll();
			CheckVScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		else if ( ((controlKeyState & KeyStateAltMask) == 0)
			&& ((controlKeyState & KeyStateAltGrMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0)
			&& ((controlKeyState & KeyStateShiftMask) != 0) )
		{
			m_CursorPos.Y = m_ScrollPos.Y;
			CheckHScroll();
			DrawTextControl();
			PostPaintEvent();
			SetCursorPosEx();
		}
		break;
    CASE_KEY_DELETE:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

			if ( bSelection )
				DeleteSelection();
			else
			{
				DeleteKey();
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			Update();
		}
		break;
    CASE_KEY_BACKSPACE:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		if ( (controlKeyState & KeyStateAllMask) == 0 )
		{
			bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

			if ( bSelection )
				DeleteSelection();
			else
			{
				DeleteKey(true);
				m_SelectionEnd.X = m_CursorPos.X;
				m_SelectionEnd.Y = m_CursorPos.Y;
			}
			Update();
		}
		break;
	CASE_KEY_TAB:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		KeyPress(0x09, 0);
		break;
	CASE_KEY_RETURN:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		KeyPress(0x0D, 0);
		break;
	CASE_KEY_ESCAPE:
		if ( m_SelectionMode )
			m_SelectionMode = false;
		break;
	case _T('T'):
		if ( ((controlKeyState & KeyStateAllExceptCtrlMask) == 0)
			&& ((controlKeyState & KeyStateCtrlMask) != 0) )
			m_SelectionMode = !m_SelectionMode;
	default:
		break;
	}
	return true;
}

bool CConsoleTextControl::KeyPress(TCHAR key, DWORD controlKeyState)
{
	if ( PtrCheck(m_TextLines) )
		return true;
	if ( m_SelectionMode )
	{
		m_SelectionMode = false;
		return true;
	}
	if ( ((controlKeyState & KeyStateAllMask) == 0)
		|| (((controlKeyState & KeyStateAllExceptShiftMask) == 0)
		&& ((controlKeyState & KeyStateShiftMask) != 0))
		|| (((controlKeyState & KeyStateCtrlMask) != 0) 
		&& ((controlKeyState & KeyStateShiftMask) == 0) 
		&& ((controlKeyState & KeyStateAltMask) == 0) 
		&& ((controlKeyState & KeyStateAltGrMask) != 0)) )
	{
		bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);

		if ( bSelection )
			DeleteSelection();

		CPointer t1 = CastMutable(CPointer, _T(""));

		while ( m_CursorPos.Y >= m_TextLineCnt )
			InsertLines(m_TextLineCnt, 1, &t1);
		if ( m_CursorPos.X > m_TextLineLen[m_CursorPos.Y] )
		{
			ReallocLine(m_CursorPos.X, m_CursorPos.X);
			t1 = m_TextLines[m_CursorPos.Y];
			for ( sdword ix = 0; ix < m_CursorPos.X; ++ix )
				*t1++ = 0x20;
			m_TextLineLen[m_CursorPos.Y] = GetTextLength(m_TextLines[m_CursorPos.Y]);
		}

		InsertKey(key);
		m_SelectionEnd.X = m_CursorPos.X;
		m_SelectionEnd.Y = m_CursorPos.Y;
		Update();
	}
	return true;
}

bool CConsoleTextControl::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTextControl::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) == 0 )
	{
		bool bSelection = (m_SelectionEnd.X != m_CursorPos.X) || (m_SelectionEnd.Y != m_CursorPos.Y);
		sdword value;

		value = m_ScrollPos.X + mousePos.X - m_ClientArea.Left;
		m_CursorPos.X = (value < 0)?0:value;
		value = m_ScrollPos.Y + mousePos.Y - m_ClientArea.Top;
		m_CursorPos.Y = (value < 0)?0:value;
		m_SelectionEnd.X = m_CursorPos.X;
		m_SelectionEnd.Y = m_CursorPos.Y;
		if ( bSelection )
		{
			DrawTextControl();
			PostPaintEvent();
		}
		SetCursorPosEx();
		return true;
	}
	if ( ((controlKeyState & KeyStateAllExceptShiftMask) == 0) && ((controlKeyState & KeyStateShiftMask) != 0) )
	{
		sdword value;

		value = m_ScrollPos.X + mousePos.X - m_ClientArea.Left;
		m_SelectionEnd.X = (value < 0)?0:value;
		value = m_ScrollPos.Y + mousePos.Y - m_ClientArea.Top;
		m_SelectionEnd.Y = (value < 0)?0:value;
		DrawTextControl();
		PostPaintEvent();
		SetCursorPosEx();
		return true;
	}
	return true;
}

bool CConsoleTextControl::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTextControl::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTextControl::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleTextControl::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	if ( (controlKeyState & KeyStateAllMask) == 0 )
	{
		sdword value;

		if ( m_MultiLine )
		{
			value = m_ScrollPos.X + mousePos.X - m_ClientArea.Left;
			if ( value < 0 )
			{
				m_SelectionEnd.X = 0;
				m_ScrollPos.X = 0;
			}
			else if ( value <= m_ScrollPos.X )
			{
				if ( m_ScrollPos.X > 0 )
					--(m_ScrollPos.X);
				m_SelectionEnd.X = m_ScrollPos.X;
			}
			else if ( value >= (m_ScrollPos.X + m_ClientAreaSize.X - 1) )
			{
				if ( m_ScrollPos.X < (m_TextLineLenMax - (m_ClientAreaSize.X / 2)) )
					++(m_ScrollPos.X);
				m_SelectionEnd.X = m_ScrollPos.X + m_ClientAreaSize.X - 1;
			}
			else
				m_SelectionEnd.X = value;
			value = m_ScrollPos.Y + mousePos.Y - m_ClientArea.Top;
			if ( value < 0 )
			{
				m_SelectionEnd.Y = 0;
				m_ScrollPos.Y = 0;
			}
			else if ( value <= m_ScrollPos.Y )
			{
				if ( m_ScrollPos.Y > 0 )
					--(m_ScrollPos.Y);
				m_SelectionEnd.Y = m_ScrollPos.Y;
			}
			else if ( value >= (m_ScrollPos.Y + m_ClientAreaSize.Y - 1) )
			{
				if ( m_ScrollPos.Y < (m_TextLineCnt - (m_ClientAreaSize.Y / 2)) )
					++(m_ScrollPos.Y);
				m_SelectionEnd.Y = m_ScrollPos.Y + m_ClientAreaSize.Y - 1;
			}
			else
				m_SelectionEnd.Y = value;
		}
		else
		{
			value = m_ScrollPos.X + mousePos.X - m_ClientArea.Left;
			m_SelectionEnd.X = (value < 0)?0:value;
			value = m_ScrollPos.Y + mousePos.Y - m_ClientArea.Top;
			m_SelectionEnd.Y = (value < 0)?0:value;
		}
		Update();
		return true;
	}
	return true;
}

bool CConsoleTextControl::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

