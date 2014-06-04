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
#include "StdAfx.h"
#include "TextControl.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "DataVector.h"

#define SHIFTED 0x8000

#define MOUSEHIT_NULL					0
#define MOUSEHIT_LBUTTONDOWN			1
#define MOUSEHIT_LBUTTONMOVE			2
#define MOUSEHIT_LBUTTONUP				3
#define MOUSEHIT_LBUTTONDOUBLEDOWN		4

class CTextControlChar: public CCppObject
{
public:
	Gdiplus::RectF charRect;
	BOOL selected;
	BOOL isWhiteSpace;
	BOOL isNewLine;
	BOOL isTab;
	TCHAR whiteSpace;

	CTextControlChar(): 
		charRect(0.0, 0.0, 0.0, 0.0), 
		selected(FALSE),
		isWhiteSpace(FALSE),
		isNewLine(FALSE),
		isTab(FALSE),
		whiteSpace(0) {}
};

typedef CDataVectorT<CTextControlChar> CTextControlCharVector;

class CTextControlCharForEachFunctor
{
public:
	bool operator()(Ptr(CTextControlChar) r1)
	{
		++cntCh;
		if ( r1->isNewLine )
			--cntNL;
		if (cntNL)
			return true;
		return false;
	}

	dword cntNL;
	dword cntCh;
};

class CTextControlChars: public CCppObject
{
public:
	CTextControlChars(): m_sellb(-1), m_selub(-1), m_lastSellb(-1), m_lastSelub(-1), m_vector(__FILE__LINE__ 1024, 1024) {}
	virtual ~CTextControlChars() {}

	__inline dword Count() const { return m_vector.Count(); }
	__inline void Clear() 
	{
		m_vector.Close();
		m_vector.Open(__FILE__LINE__ 1024, 1024);
		m_sellb = -1;
		m_selub = -1;
		m_lastSellb = -1;
		m_lastSelub = -1;
	}

	__inline void Append(CTextControlChar& charInfo)
	{
		Ptr(CTextControlChar) p = OK_NEW_OPERATOR CTextControlChar(charInfo);

		m_vector.Append(p);
	}

	CTextControlChar& Index(dword ix)
	{
		m_current = (*(*(m_vector.Index(ix))));
		return m_current;
	}

	__inline int get_LowerBound() { return m_sellb; }
	__inline int get_UpperBound() { return m_selub; }

	__inline int get_LastLowerBound() { return m_lastSellb; }
	__inline int get_LastUpperBound() { return m_lastSelub; }

	__inline void reset_Bounds(void) { m_lastSellb = m_sellb; m_lastSelub = m_selub; }

	__inline void reset_Selection(void)
	{
		if ( (m_sellb < 0) || (m_selub < 0) )
			return;

		reset_Bounds();

		CTextControlCharVector::Iterator it = m_vector.Index(m_sellb);
		CTextControlCharVector::Iterator itE = m_vector.Index(m_selub + 1);

		while ( it != itE )
		{
			(*it)->selected = FALSE;
			++it;
		}
		m_sellb = -1;
		m_selub = -1;
	}

	__inline bool has_Selection(void) { return ((m_sellb >= 0) && (m_selub >= 0) && (m_sellb <= m_selub)); }

	__inline void expand_Selection(dword newlbub, dword fix)
	{
		reset_Bounds();
		assert((fix == m_sellb) || (fix == m_selub));
		if ( fix == m_sellb )
		{
			if ( newlbub <= fix )
				set_Selection(newlbub, fix);
			else if ( newlbub <= ((dword)m_selub) )
			{
				CTextControlCharVector::Iterator it = m_vector.Index(newlbub + 1);
				CTextControlCharVector::Iterator itE = m_vector.Index(m_selub + 1);

				while ( it != itE )
				{
					(*it)->selected = FALSE;
					++it;
				}
				m_selub = newlbub;
			}
			else
			{
				CTextControlCharVector::Iterator it = m_vector.Index(m_selub + 1);
				CTextControlCharVector::Iterator itE = m_vector.Index(newlbub + 1);

				while ( it != itE )
				{
					(*it)->selected = TRUE;
					++it;
				}
				m_selub = newlbub;
			}
		}
		else
		{
			if ( newlbub >= fix )
				set_Selection(fix, newlbub);
			else if ( newlbub >= ((dword)m_sellb) )
			{
				CTextControlCharVector::Iterator it = m_vector.Index(m_sellb);
				CTextControlCharVector::Iterator itE = m_vector.Index(newlbub);

				while ( it != itE )
				{
					(*it)->selected = FALSE;
					++it;
				}
				m_sellb = newlbub;
			}
			else
			{
				CTextControlCharVector::Iterator it = m_vector.Index(newlbub);
				CTextControlCharVector::Iterator itE = m_vector.Index(m_sellb);

				while ( it != itE )
				{
					(*it)->selected = TRUE;
					++it;
				}
				m_sellb = newlbub;
			}
		}
	}

	__inline void set_Selection(dword lb, dword ub)
	{
		if ( (lb >= m_vector.Count()) || (ub >= m_vector.Count()) )
			return;

		int lastSellb = m_sellb;
		int lastSelub = m_selub;

		this->reset_Selection();

		m_lastSellb = lastSellb;
		m_lastSelub = lastSelub;

		if ( lb <= ub )
		{
			m_sellb = lb;
			m_selub = ub;
		}
		else
		{
			m_sellb = ub;
			m_selub = lb;
		}

		CTextControlCharVector::Iterator it = m_vector.Index(m_sellb);
		CTextControlCharVector::Iterator itE = m_vector.Index(m_selub + 1);

		while ( it != itE )
		{
			(*it)->selected = TRUE;
			++it;
		}
	}

	__inline dword skip_Whitespace(dword start, bool forward = true)
	{
		if ( forward )
		{
			dword ix = start;
			CTextControlCharVector::Iterator it = m_vector.Index(ix);

			while ( it )
			{
				if ( !((*it)->isWhiteSpace) )
					break;
				++ix;
				++it;
			}
			if ( ix == start )
				return ix;
			if ( ix < m_vector.Count() )
				return (ix - 1);
			return (m_vector.Count() - 1);
		}
		else
		{
			int ix = start;
			CTextControlCharVector::Iterator it = m_vector.Index(ix);

			while ( it )
			{
				if ( !((*it)->isWhiteSpace) )
					break;
				--ix;
				--it;
			}
			if ( ix == start )
				return ix;
			if ( ix >= 0 )
				return (ix + 1);
			return 0;
		}
	}

	__inline dword skip_NonWhitespace(dword start, bool forward = true)
	{
		if ( forward )
		{
			dword ix = start;
			CTextControlCharVector::Iterator it = m_vector.Index(ix);

			while ( it )
			{
				if ( (*it)->isWhiteSpace )
					break;
				++ix;
				++it;
			}
			if ( ix == start )
				return ix;
			if ( ix < m_vector.Count() )
				return (ix - 1);
			return (m_vector.Count() - 1);
		}
		else
		{
			int ix = start;
			CTextControlCharVector::Iterator it = m_vector.Index(ix);

			while ( it )
			{
				if ( (*it)->isWhiteSpace )
					break;
				--ix;
				--it;
			}
			if ( ix == start )
				return ix;
			if ( ix >= 0 )
				return (ix + 1);
			return 0;
		}
	}

	__inline int findFirstScrollPos(TSCROLLPOS scrollpos)
	{
		if ( scrollpos.y == 0 )
		{
			if ( scrollpos.x >= m_vector.Count() )
				return -1;
			return scrollpos.x;
		}

		CTextControlCharForEachFunctor param;

		param.cntNL = scrollpos.y;
		m_vector.ForEach<CTextControlCharForEachFunctor>(param);

		if (!(m_vector.ForEach<CTextControlCharForEachFunctor>(param)))
			return -1;
		param.cntCh += scrollpos.x;
		if ( param.cntCh >= m_vector.Count() )
			return -1;
		return param.cntCh;
	}

	__inline TSCROLLPOS firstPosInClientRect(const Gdiplus::RectF& clientRectF, dword endPos)
	{
		TSCROLLPOS result;

		if ( endPos >= m_vector.Count() )
			return result;

		Gdiplus::RectF rectF = (*(m_vector.Index(endPos)))->charRect;
		CTextControlCharVector::Iterator it = m_vector.Begin();

		while ( it )
		{
			CTextControlChar* pChar = *it;
			Gdiplus::RectF rectF1(rectF);
			Gdiplus::RectF rectF2(pChar->charRect);

			rectF1.Offset(-(rectF2.X), -(rectF2.Y));
			if ( clientRectF.Contains(rectF1) )
				return result;
			++(result.x);
			if ( pChar->isNewLine )
			{
				result.x = 0;
				++(result.y);
			}
			++it;
		}
		result.x = 0;
		result.y = 0;
		return result;
	}

	__inline bool isPosInClientRect(const Gdiplus::RectF& clientRectF, TSCROLLPOS _scrollPos, dword pos)
	{
		int scrollPos = findFirstScrollPos(_scrollPos);

		if ( scrollPos < 0 )
			return false;
		if ( (Cast(dword, scrollPos) >= m_vector.Count()) || (pos >= m_vector.Count()) )
			return false;

		Gdiplus::RectF rectF = (*(m_vector.Index(scrollPos)))->charRect;
		Gdiplus::RectF rectF1 = (*(m_vector.Index(pos)))->charRect;

		rectF1.Offset(-(rectF.X), -(rectF.Y));
		return (!(!(clientRectF.Contains(rectF1))));
	}

	__inline bool isAnyLineEndNotInClientRect(const Gdiplus::RectF& clientRectF, TSCROLLPOS _scrollPos)
	{
		int scrollPos = findFirstScrollPos(_scrollPos);

		if ( scrollPos < 0 )
			return false;
		if ( Cast(dword, scrollPos) >= m_vector.Count() )
			return false;

		CTextControlCharVector::Iterator it = m_vector.Index(scrollPos);
		Gdiplus::RectF rectF = (*it)->charRect;
		Gdiplus::RectF rectF1;

		++it;
		while ( it )
		{
			CTextControlChar* pChar = *it;

			if ( pChar->isNewLine )
			{
				rectF1 = pChar->charRect;
				rectF1.Offset(-(rectF.X), -(rectF.Y));
				if ( rectF1.GetTop() > clientRectF.GetBottom() )
					return false;
				if ( !(clientRectF.Contains(rectF1)) )
					return true;
			}
			++it;
		}
		it = m_vector.Index(m_vector.Count() - 1);
		rectF1 = (*it)->charRect;
		rectF1.Offset(-(rectF.X), -(rectF.Y));
		if ( rectF1.GetTop() > clientRectF.GetBottom() )
			return false;
		if ( !(clientRectF.Contains(rectF1)) )
			return true;
		return false;
	}

	__inline int getPreviousLineIndex(int lb)
	{
		CTextControlCharVector::Iterator it = m_vector.Index(lb);
		int cnt1 = 0;
		int cnt2 = 0;
		bool bNL = false;

		if ( it )
		{
			CTextControlChar* pChar = *it;

			if ( pChar->isNewLine )
				--it;
		}
		while ( it )
		{
			CTextControlChar* pChar = *it;

			if ( pChar->isNewLine )
			{
				bNL = true;
				--it;
				break;
			}
			--it;
			++cnt1;
		}
		if ( !bNL )
			return 0;
		while ( it )
		{
			CTextControlChar* pChar = *it;

			if ( pChar->isNewLine )
				break;
			--it;
			++cnt2;
		}
		if ( cnt1 <= cnt2 )
			lb -= cnt2 + 1;
		else
			lb -= cnt1;
		return (lb < 0)?0:lb;
	}

	__inline int getNextLineIndex(int ub)
	{
		CTextControlCharVector::Iterator it = m_vector.Index(ub);
		int cnt1 = 0;
		int cnt2 = 0;
		int cnt3 = 0;
		bool bNL = false;

		--it;
		while ( it )
		{
			CTextControlChar* pChar = *it;

			if ( pChar->isNewLine )
				break;
			--it;
			++cnt1;
		}
		it = m_vector.Index(ub);
		while ( it )
		{
			CTextControlChar* pChar = *it;

			if ( pChar->isNewLine )
			{
				++it;
				break;
			}
			++it;
			++cnt2;
		}
		while ( it )
		{
			CTextControlChar* pChar = *it;

			if ( pChar->isNewLine )
				break;
			++it;
			++cnt3;
		}
		if ( (cnt1 + cnt2) <= cnt3 )
			ub += cnt1 + cnt2 + 1;
		else if ( cnt1 <= cnt3 )
			ub += cnt2 + cnt1 + 1;
		else
			ub += cnt2 + cnt3 + 1;
		return (Cast(dword, ub) >= Count())?(Count() - 1):ub;
	}

	__inline int getCurrentLineCount(int start, bool bnext = true)
	{
		if ( (start < 0) || (Cast(dword, start) >= m_vector.Count()) )
			return 0;

		CTextControlCharVector::Iterator it = m_vector.Index(start);
		int cnt = 0;

		while ( it )
		{
			CTextControlChar* pChar = *it;

			if ( pChar->isNewLine )
				break;
			if ( bnext )
				++it;
			else
				--it;
			++cnt;
		}
		return cnt;
	}

protected:
	int m_sellb;
	int m_selub;
	int m_lastSellb;
	int m_lastSelub;
	CTextControlChar m_current;
	CTextControlCharVector m_vector;
};

class CTextControlUndoStruct: public CCppObject
{
public:
	CStringBuffer deleteChars;
	int deletePos;
	CStringBuffer insertChars;
	int insertPos;

	CTextControlUndoStruct():
		deletePos(-1),
		insertPos(-1) {}
};

typedef CDataVectorT<CTextControlUndoStruct> CTextControlUndoVector;

class CTextControlUndoStack: public CCppObject
{
public:
	CTextControlUndoStack(): m_currentPos(0), m_vector(__FILE__LINE__ 64,32) {}
	virtual ~CTextControlUndoStack() {}

	__inline dword get_CurrentPos() const { return m_currentPos; }
	__inline dword size() const { return m_vector.Count(); }

	__inline void reset()
	{
		m_vector.Close();
		m_vector.Open(__FILE__LINE__ 64, 32);
		m_currentPos = 0;
	}

	__inline void push(CTextControlUndoStruct& undoStruct)
	{
		if ( (undoStruct.deletePos < 0) && (undoStruct.insertPos < 0) )
			return;
		if ( !m_currentPos )
		{
			Ptr(CTextControlUndoStruct) p = OK_NEW_OPERATOR CTextControlUndoStruct(undoStruct);

			reset();
			m_vector.Append(p);
			m_currentPos = 1;
			return;
		}
		while ( m_currentPos < m_vector.Count() )
			m_vector.Remove(m_vector.Last());

		Ptr(CTextControlUndoStruct) p = OK_NEW_OPERATOR CTextControlUndoStruct(undoStruct);

		m_vector.Append(p);
		++m_currentPos;
	}

	__inline CTextControlUndoStruct& current()
	{
		m_current = *(*(m_vector.Index(m_currentPos)));
		return m_current;
	}

	__inline void undo()
	{
		if ( m_currentPos > 0 )
			--m_currentPos;
	}

	__inline void redo()
	{
		if ( m_currentPos < m_vector.Count() )
			++m_currentPos;
	}

protected:
	dword m_currentPos;
	CTextControlUndoStruct m_current;
	CTextControlUndoVector m_vector;
};

static const wchar_t* _whitespace = L"\x0009\x000A\x000B\x000C\x000D\x0020\x0085\x00A0\x1680\x180E\x2000\x2001\x2002\x2003\x2004\x2005\x2006\x2007\x2008\x2009\x200A\x2028\x2029\x202F\x205F\x3000";

static BOOL IsWhitespace(wchar_t pChar)
{
	const wchar_t* p = _whitespace;

	while ( *p != 0 )
	{
		if ( *p == pChar )
			return TRUE;
		++p;
	}
	return FALSE;
}

BEGIN_MESSAGE_MAP(CControl, CTextControl)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MESSAGE(WM_GETTEXT,OnGetText)
	ON_WM_MESSAGE(WM_GETTEXTLENGTH,OnGetTextLength)
	ON_WM_MESSAGE(WM_SETTEXT,OnSetText)
	ON_WM_COMMMAND(IDM_CUT,OnEditCutCommand)
	ON_WM_COMMMAND(IDM_COPY,OnEditCopyCommand)
	ON_WM_COMMMAND(IDM_DELETE,OnEditDeleteCommand)
	ON_WM_COMMMAND(IDM_PASTE,OnEditPasteCommand)
	ON_WM_COMMMAND(IDM_UNDO,OnEditUndoCommand)
	ON_WM_COMMMAND(IDM_REDO,OnEditRedoCommand)
END_MESSAGE_MAP()

CTextControl::CTextControl(LPCTSTR name):
    CControl(name),
	m_multiLine(false),
	m_cursorPos(0),
	m_scrollPos(),
	m_mouseHitCode(MOUSEHIT_NULL),
	m_mouseHitModifier(0),
	m_caretCreated(FALSE),
	m_caretShown(FALSE),
	m_timerEnabled(FALSE),
	m_timerDirection(TTimerDirectionNone),
	m_insidePaint(FALSE),
	m_hasselection(FALSE),
	m_chars(nullptr),
	m_undostack(nullptr)
{
	m_chars = OK_NEW_OPERATOR CTextControlChars();
	m_undostack = OK_NEW_OPERATOR CTextControlUndoStack();
}

CTextControl::CTextControl(ConstRef(CStringBuffer) name):
    CControl(name),
	m_multiLine(false),	
	m_cursorPos(0),
	m_scrollPos(),
	m_mouseHitCode(MOUSEHIT_NULL),
	m_mouseHitModifier(0),
	m_caretCreated(FALSE),
	m_caretShown(FALSE),
	m_timerEnabled(FALSE),
	m_timerDirection(TTimerDirectionNone),
	m_insidePaint(FALSE),
	m_hasselection(FALSE),
	m_chars(nullptr),
	m_undostack(nullptr)
{
	m_chars = OK_NEW_OPERATOR CTextControlChars();
	m_undostack = OK_NEW_OPERATOR CTextControlUndoStack();
}

CTextControl::~CTextControl(void)
{
	delete m_chars;
	delete m_undostack;
	if ( m_caretCreated )
		::DestroyCaret();
}

BOOL CTextControl::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CTEXTCONTROL");
	return TRUE;
}

void CTextControl::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".BackgroundColor.Selected"), _T("TextControl"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("TextControl"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Selected"), _T("TextControl"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("TextControl"));
}

void CTextControl::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".Font.Normal"), _T("TextControl"));
}

void CTextControl::Refresh()
{
	Update(TRUE);
}

CWin* CTextControl::Clone(LONG style, DWORD exstyle)
{
	CWin* pWin = OK_NEW_OPERATOR CTextControl();

	return pWin;
}

void CTextControl::set_text(const CStringBuffer& text) 
{ 
	m_text = text; 
	if ( is_created() )
		Reset(TRUE);
}

void CTextControl::Reset(BOOL bUpdate)
{
	m_cursorPos = 0;
	m_scrollPos.x = 0;
	m_scrollPos.y = 0;
	m_mouseHitCode = MOUSEHIT_NULL;
	m_mouseHitModifier = 0;
	m_chars->reset_Selection();
	m_undostack->reset();
	Update(bUpdate);
}

bool CTextControl::has_selection() const
{
	return m_chars->has_Selection();
}

LRESULT CTextControl::OnGetText(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer text = get_text();
	CStringBuffer text1;
	dword len;

	if ( text.GetSize() <= wParam )
		text1 = text;
	else
		text.SubString(0, Castdword(wParam - 1), text1);
	len = text1.GetSize();
	s_memcpy(lParam, CastMutable(CPointer, text1.GetString()), len * szchar);
	return len - 1;
}

LRESULT CTextControl::OnGetTextLength(WPARAM wParam, LPARAM lParam)
{
	return get_text().GetLength();
}

LRESULT CTextControl::OnSetText(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer text(__FILE__LINE__ CastAny(CPointer, lParam));

	set_text(text);
	return TRUE;
}

LRESULT CTextControl::OnPaint(WPARAM wParam, LPARAM lParam)
{
	if ( m_insidePaint )
		return 0;
	m_insidePaint = TRUE;

	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
	{
		m_insidePaint = FALSE;
		return 0;
	}

	Gdiplus::Graphics graphics(hdc);
	RECT clientrect;

	GetClientRect(&clientrect);
	Gdiplus::Bitmap bitmap(clientrect.right, clientrect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);

	Gdiplus::PointF pt(0.0, 0.0);
	RECT rect;
	Gdiplus::RectF rectF;
	Gdiplus::RectF clientrectF;
	CTextControlChar techar_scrollPos;
	dword len = 0;
	Gdiplus::Font* pFont = get_Font(_T(".Font.Normal"), _T("TextControl"));

	Convert2RectF(&clientrectF, &clientrect);
	if ( m_updateRects )
	{
		Gdiplus::REAL fontHeight = pFont->GetHeight(&graphics);

		::SetRectEmpty(&m_maxClientArea);
		m_chars->Clear();
		if ( (len = m_text.GetLength()) > 0 )
		{
			CStringConstIterator it(m_text.GetString());

			pt.X = 0; pt.Y = 0;
			for ( dword ix = 0; ix < len; ++ix, ++it )
			{
				CTextControlChar techar;

				if ( IsWhitespace(it[0]) )
				{
					techar.charRect.X = pt.X; techar.charRect.Y = pt.Y;
					techar.charRect.Height = fontHeight; techar.charRect.Width = 4;
					techar.isWhiteSpace = TRUE;
					techar.whiteSpace = it[0];
					pt.X += techar.charRect.Width;
					if ( techar.whiteSpace == _T('\n') )
					{
						techar.isNewLine = TRUE;
						pt.Y += techar.charRect.Height;
						pt.X = 0;
					}
					else if ( techar.whiteSpace == _T('\t') )
					{
						techar.isTab = TRUE;
						techar.charRect.Width += 12;
						pt.X += 12;
					}
				}
				else
				{
					graphics.MeasureString(it.GetCurrent(), 1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &(techar.charRect));
					pt.X += techar.charRect.Width;
				}
				m_chars->Append(techar);			
			}
		}

		CTextControlChar techar;

		techar.charRect.X = pt.X; techar.charRect.Y = pt.Y;
		techar.charRect.Height = fontHeight; techar.charRect.Width = 4;
		techar.isWhiteSpace = TRUE;
		m_chars->Append(techar);
		m_maxClientArea.bottom = Cast(LONG, fontHeight + 4);
		m_maxClientArea.right = Cast(LONG, techar.charRect.X + techar.charRect.Width + 4);
		m_updateRects = FALSE;
	}
	if ( (m_mouseHitCode != MOUSEHIT_NULL) && (m_chars->Count() > 1) )
	{
		pt.X = Cast(Gdiplus::REAL, m_mouseHit.x); pt.Y = Cast(Gdiplus::REAL, m_mouseHit.y);
		if ( !(clientrectF.Contains(pt)) )
		{
			switch ( m_mouseHitCode )
			{
			case MOUSEHIT_LBUTTONMOVE:
				if ( m_multiLine )
				{
					if ( (pt.Y < (clientrectF.Y + 10)) && (pt.X >= clientrectF.X) && (pt.X <= (clientrectF.X + clientrectF.Width)) && (m_scrollPos.y > 0) )
					{
						m_timerDirection = TTimerDirectionNorth;
						_StartTimer();
					}
					else if ( (pt.Y > (clientrectF.Y + clientrectF.Height - 10)) && (pt.X >= clientrectF.X) && (pt.X <= (clientrectF.X + clientrectF.Width)) && (!(m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->Count() - 1))) )
					{
						m_timerDirection = TTimerDirectionSouth;
						_StartTimer();
					}
					else if ( (pt.X < (clientrectF.X + 10)) && (pt.Y >= clientrectF.Y) && (pt.Y <= (clientrectF.Y + clientrectF.Height)) && (m_scrollPos.x > 0) )
					{
						m_timerDirection = TTimerDirectionWest;
						_StartTimer();
					}
					else if ( (pt.X > (clientrectF.X + clientrectF.Width - 10)) && (pt.Y >= clientrectF.Y) && (pt.Y <= (clientrectF.Y + clientrectF.Height)) && (m_chars->isAnyLineEndNotInClientRect(clientrectF, m_scrollPos)) )
					{
						m_timerDirection = TTimerDirectionEast;
						_StartTimer();
					}
					else
					{
						m_timerDirection = TTimerDirectionNone;
						_StopTimer();
					}
				}
				else
				{
					if ( (pt.X < (clientrectF.X + 10)) && (m_scrollPos.x > 0) )
					{
						m_timerDirection = TTimerDirectionWest;
						_StartTimer();
					}
					else if ( (pt.X > (clientrectF.X + clientrectF.Width - 10)) && (!(m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->Count() - 1))) )
					{
						m_timerDirection = TTimerDirectionEast;
						_StartTimer();
					}
					else
					{
						m_timerDirection = TTimerDirectionNone;
						_StopTimer();
					}
				}
				break;
			default:
				break;
			}
		}
		else
		{
			int scrollPos = m_chars->findFirstScrollPos(m_scrollPos);

			techar_scrollPos = m_chars->Index(scrollPos);
			for ( dword ix = scrollPos; ix < m_chars->Count(); ++ix )
			{
				CTextControlChar techar = m_chars->Index(ix);

				rectF = techar.charRect;
				rectF.Offset(-(techar_scrollPos.charRect.X), -(techar_scrollPos.charRect.Y));
				if ( ix == m_chars->Count() - 1 )
					rectF.Width = clientrectF.Width - rectF.Width - rectF.X;
				switch ( m_mouseHitCode )
				{
				case MOUSEHIT_LBUTTONDOWN:
					if ( rectF.Contains(pt) )
					{
						if ( m_mouseHitModifier == 0 )
						{
							m_cursorPos = ix;
							m_chars->reset_Selection();
						}
						else if ( m_mouseHitModifier == MK_SHIFT )
						{
							if ( m_chars->has_Selection() )
								m_chars->expand_Selection(ix, m_cursorPos);
							else
								m_chars->set_Selection(m_cursorPos, ix);
						}
						m_mouseHitCode = MOUSEHIT_NULL;
						m_mouseHitModifier = 0;
					}
					break;
				case MOUSEHIT_LBUTTONMOVE:
					if ( rectF.Contains(pt) )
					{
						if ( m_timerEnabled )
						{
							::KillTimer(m_hwnd, 1000);
							m_timerEnabled = FALSE;
							m_timerDirection = TTimerDirectionNone;
						}
						if ( m_chars->has_Selection() )
							m_chars->expand_Selection(ix, m_cursorPos);
						else
							m_chars->set_Selection(m_cursorPos, ix);
						m_mouseHitCode = MOUSEHIT_NULL;
						m_mouseHitModifier = 0;
					}
					break;
				case MOUSEHIT_LBUTTONDOUBLEDOWN:
					if ( rectF.Contains(pt) )
					{
						if ( m_mouseHitModifier == 0 )
						{
							if ( techar.isWhiteSpace )
							{
								if ( pt.X < (rectF.X + (rectF.Width / 2)) )
								{
									dword end = m_chars->skip_Whitespace(ix, false);

									if ( end > 0 )
										--end;

									dword start = m_chars->skip_NonWhitespace(end, false);

									m_cursorPos = end;
									m_chars->set_Selection(start, end);
								}
								else
								{
									dword start = m_chars->skip_Whitespace(ix);

									if ( start < (m_chars->Count() - 1) )
										++start;

									dword end = m_chars->skip_NonWhitespace(start);

									m_cursorPos = start;
									m_chars->set_Selection(start, end);
								}
							}
							else
							{
								dword start = m_chars->skip_NonWhitespace(ix, false);
								dword end = m_chars->skip_NonWhitespace(ix);

								m_cursorPos = start;
								m_chars->set_Selection(start, end);
							}
						}
						else if ( m_mouseHitModifier == MK_SHIFT )
						{

						}
						m_mouseHitCode = MOUSEHIT_NULL;
						m_mouseHitModifier = 0;
					}
					break;
				default:
					m_mouseHitCode = MOUSEHIT_NULL;
					m_mouseHitModifier = 0;
					break;
				}
				if ( m_mouseHitCode == MOUSEHIT_NULL )
					break;
			}
		}
		if ( m_mouseHitCode != MOUSEHIT_NULL )
		{
			m_mouseHitCode = MOUSEHIT_NULL;
			m_mouseHitModifier = 0;
		}
	}

	Gdiplus::Brush* pBackgroundColorNormal = get_Brush(_T(".BackgroundColor.Normal"),  _T("TextControl"), Gdiplus::Color::White);
	Gdiplus::Brush* pBackgroundColorSelected = get_Brush(_T(".BackgroundColor.Selected"),  _T("TextControl"), Gdiplus::Color::Blue);
	Gdiplus::Brush* pForegroundColorNormal = get_Brush(_T(".ForegroundColor.Normal"),  _T("TextControl"), Gdiplus::Color::Black);
	Gdiplus::Brush* pForegroundColorSelected = get_Brush(_T(".ForegroundColor.Selected"),  _T("TextControl"), Gdiplus::Color::White);
	BOOL bShowCursor = TRUE;

	bitmapgraphics.FillRectangle(pBackgroundColorNormal, clientrectF);
	if ( m_chars->Count() == 0 )
	{
		m_insidePaint = FALSE;
		return -1;
	}
	if ( has_focus() )
	{
		if ( m_chars->has_Selection() )
			bShowCursor = FALSE;
		if ( bShowCursor )
		{
			if ( !m_caretShown )
			{
				if ( !m_caretCreated )
				{
					Gdiplus::REAL fontHeight = 12;

					fontHeight = pFont->GetHeight(&graphics);
					::CreateCaret(m_hwnd, (HBITMAP) nullptr, 1, (int)(floor(fontHeight + 0.5))); 
					m_caretCreated = TRUE;
				}
				::ShowCaret(m_hwnd);
				m_caretShown = TRUE;
			}
		}
		else if ( m_caretShown )
		{
			::HideCaret(m_hwnd);
			m_caretShown = FALSE;
		}
	}
	pt.X = 0; pt.Y = 0;
	if ( m_chars->has_Selection() )
	{
		if ( m_chars->get_UpperBound() != m_chars->get_LastUpperBound() )
			m_scrollPos = m_chars->firstPosInClientRect(clientrectF, m_chars->get_UpperBound());
		else if ( m_chars->get_LowerBound() != m_chars->get_LastLowerBound() )
			m_scrollPos = m_chars->firstPosInClientRect(clientrectF, m_chars->get_LowerBound());
		m_chars->reset_Bounds();
	}
	else
		m_scrollPos = m_chars->firstPosInClientRect(clientrectF,m_cursorPos);

	int scrollPos = m_chars->findFirstScrollPos(m_scrollPos);
	techar_scrollPos = m_chars->Index(scrollPos);
	for ( dword ix = scrollPos; ix < m_chars->Count(); ++ix )
	{
		CTextControlChar techar = m_chars->Index(ix);

		rectF = techar.charRect;
		rectF.Offset(-(techar_scrollPos.charRect.X), -(techar_scrollPos.charRect.Y));
		if ( bShowCursor && (ix == m_cursorPos) )
		{
			Convert2Rect(&rect, &rectF);
			::SetCaretPos(rect.left, rect.top);
		}
		pt.X = rectF.X; pt.Y = rectF.Y;
		if ( clientrectF.Contains(rectF) )
		{
			if ( techar.selected )
			{
				bitmapgraphics.FillRectangle(pBackgroundColorSelected, rectF);
				if ( !(techar.isWhiteSpace) )
					bitmapgraphics.DrawString(m_text.GetString() + ix, 1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), pForegroundColorSelected);
			}
			else
			{
				bitmapgraphics.FillRectangle(pBackgroundColorNormal, rectF);
				if ( !(techar.isWhiteSpace) )
					bitmapgraphics.DrawString(m_text.GetString() + ix, 1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), pForegroundColorNormal);
			}
		}
	}

	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);

	if ( ((!m_hasselection) && (m_chars->has_Selection()))
		|| (m_hasselection && (!(m_chars->has_Selection()))) )
	{
		SendNotifyMessage(NM_EDITORSELCHANGE, (LPARAM) this);
		SendNotifyMessage(NM_SELECTIONCHANGED);
		m_hasselection = m_chars->has_Selection()?TRUE:FALSE;
	}
	m_insidePaint = FALSE;
	return 0;
}

LRESULT CTextControl::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if ( (pt.x < 0) && (pt.y < 0) )
		::GetCursorPos(&pt);

	TNotifyMessageEditorContextMenu param(this, pt);

	SendNotifyMessage(NM_EDITORCONTEXTMENU, (LPARAM)(&param));
	return 0;
}

LRESULT CTextControl::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	set_focus();
	if ( wParam == MK_LBUTTON )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONDOWN; 
		m_mouseHitModifier = 0;

		SetCapture(m_hwnd);
		Update(FALSE);
		return 0;
	}
	if ( wParam == (MK_LBUTTON | MK_SHIFT) )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONDOWN; 
		m_mouseHitModifier = MK_SHIFT;

		SetCapture(m_hwnd);
		Update(FALSE);
		return 0;
	}
	return 0;
}

LRESULT CTextControl::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{
		if ( m_timerEnabled )
		{
			::KillTimer(m_hwnd, 1000);
			m_timerEnabled = FALSE;
			m_timerDirection = TTimerDirectionNone;
		}
		::ReleaseCapture();
		return 0;
	}
	if ( wParam == MK_SHIFT )
	{
		if ( m_timerEnabled )
		{
			::KillTimer(m_hwnd, 1000);
			m_timerEnabled = FALSE;
			m_timerDirection = TTimerDirectionNone;
		}
		::ReleaseCapture();
		return 0;
	}
	return 0;
}

LRESULT CTextControl::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONDOUBLEDOWN; 
		m_mouseHitModifier = 0;

		Update(FALSE);
		return 0;
	}
	if ( wParam == (MK_LBUTTON | MK_SHIFT) )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONDOUBLEDOWN; 
		m_mouseHitModifier = MK_SHIFT;

		Update(FALSE);
		return 0;
	}
	return 0;
}

LRESULT CTextControl::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONMOVE; 
		m_mouseHitModifier = 0;

		Update(FALSE);
		return 0;
	}
	if ( wParam == (MK_LBUTTON | MK_SHIFT) )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONMOVE; 
		m_mouseHitModifier = MK_SHIFT;

		Update(FALSE);
		return 0;
	}
	return 0;
}

LRESULT CTextControl::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

	if ( m_chars->Count() == 0 )
		return 0;
	switch ( wParam )
	{
	case 0x43: // C
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_COPY, 0), 0);
		break;
	case 0x56: // V
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_PASTE, 0), 0);
		break;
	case 0x58: // X
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_CUT, 0), 0);
		break;
	case 0x59: // Y
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REDO, 0), 0);
		break;
	case 0x5A: // Z
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_UNDO, 0), 0);
		break;
	case VK_BACK:
		if ( bShift || bControl )
			break;
		if ( !(m_chars->has_Selection()) )
		{
			if ( m_cursorPos == 0 )
				return 0;
			--m_cursorPos;
		}
	case VK_DELETE:
		if ( !(bShift || bControl) )
		{
			dword len = m_chars->Count() - 1;
			CTextControlUndoStruct undoStruct;

			if ( len > 0 )
			{
				if ( m_chars->has_Selection() )
					_Edit_Delete_Selection(undoStruct);
				else
					_Edit_Delete_Char(undoStruct);
				m_undostack->push(undoStruct);
				Update(TRUE);
			}
		}
		break;
	case VK_HOME:
		if ( bShift && bControl )
		{
			if ( m_multiLine )
			{
				if ( m_chars->has_Selection() )
				{
					if ( m_cursorPos == 0 )
						m_chars->reset_Selection();
					else
						m_chars->expand_Selection(0, m_cursorPos);
				}
				else
					m_chars->set_Selection(0, m_cursorPos);
				Update(FALSE);
			}
		}
		else if ( bShift )
		{
			int start = 0;

			if ( m_multiLine )
			{
				int lb;

				if ( m_chars->has_Selection() )
					lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());
				else
					lb = m_cursorPos;

				int cnt = m_chars->getCurrentLineCount(lb, false);

				if ( cnt == 0 )
					cnt = m_chars->getCurrentLineCount(lb - 1, false);
				else if ( cnt == 1 )
					cnt = m_chars->getCurrentLineCount(lb - 2, false) + 1;
				else
					--cnt;
				start = lb - cnt;
				if ( start < 0 )
					start = 0;
			}
			if ( m_chars->has_Selection() )
			{
				if ( m_cursorPos == 0 )
					m_chars->reset_Selection();
				else
					m_chars->expand_Selection(start, m_cursorPos);
			}
			else
				m_chars->set_Selection(start, m_cursorPos);
			Update(FALSE);
		}
		else if ( bControl )
		{
			if ( m_multiLine )
			{
				m_chars->reset_Selection();
				m_cursorPos = 0;
				Update(FALSE);
			}
		}
		else
		{
			m_chars->reset_Selection();
			if ( m_multiLine && (m_cursorPos > 0) )
			{
				int cnt = m_chars->getCurrentLineCount(m_cursorPos, false);

				if ( cnt == 0 )
					cnt = m_chars->getCurrentLineCount(m_cursorPos - 1, false);
				else if ( cnt == 1 )
					cnt = m_chars->getCurrentLineCount(m_cursorPos - 2, false) + 1;
				else
					--cnt;
				if ( m_cursorPos >= Cast(dword, cnt) )
					m_cursorPos -= cnt;
				else
					m_cursorPos = 0;
			}
			else
				m_cursorPos = 0;
			Update(FALSE);
		}
		break;
	case VK_END:
		{
			dword len = m_chars->Count() - 1;

			if ( len == 0 )
				break;
			if ( bShift && bControl )
			{
				if ( m_multiLine )
				{
					if ( m_chars->has_Selection() )
					{
						if ( m_cursorPos == len )
							m_chars->reset_Selection();
						else
							m_chars->expand_Selection(len, m_cursorPos);
					}
					else
						m_chars->set_Selection(m_cursorPos, len);
					Update(FALSE);
				}
			}
			else if ( bShift )
			{
				int start = len;

				if ( m_multiLine )
				{
					int ub;

					if ( m_chars->has_Selection() )
						ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());
					else
						ub = m_cursorPos;

					int cnt = m_chars->getCurrentLineCount(ub);

					if ( cnt == 0 )
						cnt = m_chars->getCurrentLineCount(ub + 1) + 1;
					start = ub + cnt;
					if ( Cast(dword, start) > len )
						start = len;
				}
				if ( m_chars->has_Selection() )
				{
					if ( m_cursorPos == len )
						m_chars->reset_Selection();
					else
						m_chars->expand_Selection(start, m_cursorPos);
				}
				else if ( (m_cursorPos < len) || (Cast(dword, start) < len) )
					m_chars->set_Selection(m_cursorPos, start);
				Update(FALSE);
			}
			else if ( bControl )
			{
				if ( m_multiLine )
				{
					m_chars->reset_Selection();
					m_cursorPos = len;
					Update(FALSE);
				}
			}
			else
			{
				m_chars->reset_Selection();
				if ( m_multiLine && (m_cursorPos < len) )
				{
					int cnt = m_chars->getCurrentLineCount(m_cursorPos);

					if ( cnt == 0 )
						cnt = m_chars->getCurrentLineCount(m_cursorPos + 1) + 1;
					m_cursorPos += cnt;
					if ( m_cursorPos > len )
						m_cursorPos = len;
				}
				else
					m_cursorPos = len;
				Update(FALSE);
			}
		}
		break;
	case VK_RIGHT:
		{
			dword len = m_chars->Count() - 1;

			if ( len == 0 )
				break;
			if ( bShift && bControl )
			{
				if ( m_chars->has_Selection() )
				{
					dword ix = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					if ( m_chars->Index(ix).isWhiteSpace )
					{
						ix = m_chars->skip_Whitespace(ix);
						if ( ix < len )
							++ix;
					}
					ix = m_chars->skip_NonWhitespace(ix);
					if ( ix < len )
						++ix;
					ix = m_chars->skip_Whitespace(ix);
					m_chars->expand_Selection(ix, m_cursorPos);
				}
				else
				{
					dword ix = m_cursorPos;

					if ( !(m_chars->Index(ix).isWhiteSpace) )
					{
						ix = m_chars->skip_NonWhitespace(ix);
						if ( ix < len )
							++ix;
					}
					ix = m_chars->skip_Whitespace(ix);
					m_chars->set_Selection(ix, m_cursorPos);
				}
				Update(FALSE);
			}
			else if ( bShift )
			{
				if ( m_chars->has_Selection() )
				{
					int ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					if ( ((dword)ub) < len )
					{
						m_chars->expand_Selection(ub + 1, m_cursorPos);
						Update(FALSE);
					}
				}
				else
				{
					m_chars->set_Selection(m_cursorPos, m_cursorPos);
					Update(FALSE);
				}
			}
			else if ( bControl )
			{
				if ( m_chars->has_Selection() )
				{
					m_cursorPos = m_chars->get_UpperBound();
					m_chars->reset_Selection();
				}
				else
				{
					dword ix = m_cursorPos;

					if ( m_chars->Index(ix).isWhiteSpace )
					{
						ix = m_chars->skip_Whitespace(ix);
						if ( ix < len )
							++ix;
					}
					ix = m_chars->skip_NonWhitespace(ix);
					if ( ix < len )
						++ix;
					ix = m_chars->skip_Whitespace(ix);
					m_cursorPos = ix;
				}
				Update(FALSE);
			}
			else if ( m_chars->has_Selection() )
			{
				m_cursorPos = m_chars->get_UpperBound();
				m_chars->reset_Selection();
				Update(FALSE);
			}
			else if ( m_cursorPos < len )
			{
				++m_cursorPos;
				Update(FALSE);
			}
		}
		break;
	case VK_LEFT:
		{
			dword len = m_chars->Count() - 1;

			if ( len == 0 )
				break;
			if ( bShift && bControl )
			{
				if ( m_chars->has_Selection() )
				{
					dword ix = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					if ( m_chars->Index(ix).isWhiteSpace )
					{
						ix = m_chars->skip_Whitespace(ix, false);
						if ( ix > 0 )
							--ix;
					}
					ix = m_chars->skip_NonWhitespace(ix, false);
					if ( ix > 0 )
						--ix;
					ix = m_chars->skip_Whitespace(ix, false);
					m_chars->expand_Selection(ix, m_cursorPos);
				}
				else
				{
					dword ix = m_cursorPos;

					if ( !(m_chars->Index(ix).isWhiteSpace) )
					{
						ix = m_chars->skip_NonWhitespace(ix, false);
						if ( ix > 0 )
							--ix;
					}
					ix = m_chars->skip_Whitespace(ix);
					m_chars->set_Selection(ix, m_cursorPos);
				}
				Update(FALSE);
			}
			else if ( bShift )
			{
				if ( m_chars->has_Selection() )
				{
					int lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());

					if ( lb > 0 )
					{
						m_chars->expand_Selection(lb - 1, m_cursorPos);
						Update(FALSE);
					}
				}
				else
				{
					m_chars->set_Selection(m_cursorPos, m_cursorPos);
					Update(FALSE);
				}
			}
			else if ( bControl )
			{
				if ( m_chars->has_Selection() )
				{
					m_cursorPos = m_chars->get_LowerBound();
					m_chars->reset_Selection();
				}
				else
				{
					dword ix = m_cursorPos;

					if ( m_chars->Index(ix).isWhiteSpace )
					{
						ix = m_chars->skip_Whitespace(ix, false);
						if ( ix > 0 )
							--ix;
					}
					ix = m_chars->skip_NonWhitespace(ix, false);
					if ( ix > 0 )
						--ix;
					ix = m_chars->skip_Whitespace(ix, false);
					m_cursorPos = ix;
				}
				Update(FALSE);
			}
			else if ( m_chars->has_Selection() )
			{
				m_cursorPos = m_chars->get_LowerBound();
				m_chars->reset_Selection();
				Update(FALSE);
			}
			else if ( m_cursorPos > 0 )
			{
				--m_cursorPos;
				Update(FALSE);
			}
		}
		break;
	case VK_UP:
		if ( !m_multiLine )
		{
			if ( bShift || bControl )
				break;
			SendNotifyMessage(NM_EDITORCURSORUP, (LPARAM) this);
			break;
		}
		if ( bShift && bControl )
		{
		}
		else if ( bShift )
		{
			if ( m_chars->has_Selection() )
			{
				int lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());

				lb = m_chars->getPreviousLineIndex(lb);
				m_chars->expand_Selection(lb, m_cursorPos);
			}
			else
			{
				int lb = m_chars->getPreviousLineIndex(m_cursorPos);

				m_chars->set_Selection(lb, m_cursorPos);
			}
			Update(FALSE);
		}
		else if ( bControl )
		{
			SendNotifyMessage(NM_EDITORCURSORUP, (LPARAM) this);
		}
		else if ( m_chars->has_Selection() )
		{
			m_cursorPos = m_chars->get_LowerBound();
			m_chars->reset_Selection();
			Update(FALSE);
		}
		else if ( m_cursorPos > 0 )
		{
			m_cursorPos = m_chars->getPreviousLineIndex(m_cursorPos);
			Update(FALSE);
		}
		break;
	case VK_DOWN:
		if ( !m_multiLine )
		{
			if ( bShift || bControl )
				break;
			SendNotifyMessage(NM_EDITORCURSORDOWN, (LPARAM) this);
			break;
		}
		if ( bShift && bControl )
		{
		}
		else if ( bShift )
		{
			if ( m_chars->has_Selection() )
			{
				int ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

				ub = m_chars->getNextLineIndex(ub);
				m_chars->expand_Selection(ub, m_cursorPos);
			}
			else
			{
				int ub = m_chars->getNextLineIndex(m_cursorPos);

				m_chars->set_Selection(m_cursorPos, ub);
			}
			Update(FALSE);
		}
		else if ( bControl )
		{
			SendNotifyMessage(NM_EDITORCURSORDOWN, (LPARAM) this);
		}
		else if ( m_chars->has_Selection() )
		{
			m_cursorPos = m_chars->get_UpperBound();
			m_chars->reset_Selection();
			Update(FALSE);
		}
		else if ( (m_chars->Count() > 0) && (m_cursorPos < (m_chars->Count() - 1)) )
		{
			m_cursorPos = m_chars->getNextLineIndex(m_cursorPos);
			Update(FALSE);
		}
		break;
	case VK_ESCAPE:
		if ( bShift || bControl )
			break;
		SendNotifyMessage(NM_EDITORESCAPE, (LPARAM) this);
		break;
	case VK_RETURN:
		if ( !m_multiLine )
		{
			if ( bShift || bControl )
				break;
			SendNotifyMessage(NM_EDITORRETURN, (LPARAM) this);
			break;
		}
		if ( !(bShift || bControl) )
		{
			CTextControlUndoStruct undoStruct;

			_Edit_Delete_Selection(undoStruct);
			_Edit_Insert_Char(undoStruct, _T('\n'));
			m_undostack->push(undoStruct);
			Update(TRUE);
		}
		else if ( bControl )
			SendNotifyMessage(NM_EDITORRETURN, (LPARAM) this);
		break;
	case VK_TAB:
		if ( !m_multiLine )
		{
			if ( bShift || bControl )
				break;
			SendNotifyMessage(NM_EDITORTAB, (LPARAM) this);
			break;
		}
		if ( !(bShift || bControl) )
		{
			CTextControlUndoStruct undoStruct;

			_Edit_Delete_Selection(undoStruct);
			_Edit_Insert_Char(undoStruct, _T('\t'));
			m_undostack->push(undoStruct);
			Update(TRUE);
		}
		else if ( bControl )
			SendNotifyMessage(NM_EDITORTAB, (LPARAM) this);
		break;
	default:
		if ( bShift || bControl )
			break;
		if ( (wParam >= VK_F1) && (wParam <= VK_F24) )
		{
			TNotifyMessageEditorFuncKey param(this, wParam);

			SendNotifyMessage(NM_EDITORFUNCKEY, (LPARAM)(&param));
		}
		break;
	}
	return 0;
}

LRESULT CTextControl::OnChar(WPARAM wParam, LPARAM lParam)
{
	if ( wParam < 0x20 )
		return 0;

	CTextControlUndoStruct undoStruct;

	_Edit_Delete_Selection(undoStruct);
	_Edit_Insert_Char(undoStruct, Cast(TCHAR, wParam));
	m_undostack->push(undoStruct);
	Update(TRUE);
	return 0;
}

LRESULT CTextControl::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	Update(FALSE);
	return 0;
}

LRESULT CTextControl::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	_StopTimer();
	::DestroyCaret();
	m_caretShown = FALSE;
	m_caretCreated = FALSE;
	SendNotifyMessage(NM_EDITORLOSTFOCUS, (LPARAM) this);
	return 0;
}

LRESULT CTextControl::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 1000 )
	{
		switch ( m_timerDirection )
		{
		case TTimerDirectionNorth:
			if ( m_scrollPos.y == 0 )
			{
				::KillTimer(m_hwnd, 1000);
				m_timerEnabled = FALSE;
				m_timerDirection = TTimerDirectionNone;
				break;
			}
			if ( m_chars->has_Selection() )
			{
				int lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());

				lb = m_chars->getPreviousLineIndex(lb);
				m_chars->expand_Selection(lb, m_cursorPos);
				Update(FALSE);
			}
			else
			{
				int lb = m_chars->getPreviousLineIndex(m_cursorPos);

				m_chars->set_Selection(lb, m_cursorPos);
				Update(FALSE);
			}
			break;
		case TTimerDirectionSouth:
			{
				RECT clientrect;
				Gdiplus::RectF clientrectF;

				GetClientRect(&clientrect);
				Convert2RectF(&clientrectF, &clientrect);
				if ( m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->Count() - 1) )
				{
					::KillTimer(m_hwnd, 1000);
					m_timerEnabled = FALSE;
					m_timerDirection = TTimerDirectionNone;
					break;
				}
				if ( m_chars->has_Selection() )
				{
					int ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					ub = m_chars->getNextLineIndex(ub);
					m_chars->expand_Selection(ub, m_cursorPos);
					Update(FALSE);
				}
				else
				{
					int ub = m_chars->getNextLineIndex(m_cursorPos);

					m_chars->set_Selection(m_cursorPos, ub);
					Update(FALSE);
				}
			}
			break;
		case TTimerDirectionEast:
			if ( m_chars->has_Selection() )
			{
				int ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

				if ( m_chars->getCurrentLineCount(ub) == 0 )
				{
					::KillTimer(m_hwnd, 1000);
					m_timerEnabled = FALSE;
					m_timerDirection = TTimerDirectionNone;
					break;
				}
				m_chars->expand_Selection(ub + 1, m_cursorPos);
				Update(FALSE);
			}
			else
			{
				m_chars->set_Selection(m_cursorPos, m_cursorPos);
				Update(FALSE);
			}
			break;
		case TTimerDirectionWest:
			if ( m_scrollPos.x == 0 )
			{
				::KillTimer(m_hwnd, 1000);
				m_timerEnabled = FALSE;
				m_timerDirection = TTimerDirectionNone;
				break;
			}
			if ( m_chars->has_Selection() )
			{
				int lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());

				if ( lb > 0 )
				{
					m_chars->expand_Selection(lb - 1, m_cursorPos);
					Update(FALSE);
				}
			}
			else
			{
				m_chars->set_Selection(m_cursorPos, m_cursorPos);
				Update(FALSE);
			}
			break;
		}
	}
	return 0;
}

LRESULT CTextControl::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTextControl::OnEditUndoCommand(WPARAM wParam, LPARAM lParam)
{
	bool b = m_undostack->get_CurrentPos() > 0;

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	m_undostack->undo();

	CTextControlUndoStruct undoStruct = m_undostack->current();

	if ( undoStruct.insertPos >= 0 )
	{
		m_text.DeleteString(undoStruct.insertPos, undoStruct.insertChars.GetLength());
		m_cursorPos = undoStruct.insertPos;
	}
	if ( undoStruct.deletePos >= 0 )
	{
		dword len0 = undoStruct.deleteChars.GetLength();

		m_text.InsertString(undoStruct.deletePos, undoStruct.deleteChars.GetString(), len0);
		m_cursorPos = undoStruct.deletePos + len0;
	}	
	Update(TRUE);
	return 0;
}

LRESULT CTextControl::OnEditRedoCommand(WPARAM wParam, LPARAM lParam)
{
	bool b = m_undostack->get_CurrentPos() < m_undostack->size();

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;

	CTextControlUndoStruct undoStruct = m_undostack->current();

	if ( undoStruct.deletePos >= 0 )
	{
		m_text.DeleteString(undoStruct.deletePos, undoStruct.deleteChars.GetLength());
		m_cursorPos = undoStruct.deletePos;
	}
	if ( undoStruct.insertPos >= 0 )
	{
		dword len0 = undoStruct.insertChars.GetLength();

		m_text.InsertString(undoStruct.insertPos, undoStruct.insertChars.GetString(), len0);
		m_cursorPos = undoStruct.insertPos + len0;
	}	
	m_undostack->redo();
	Update(TRUE);
	return 0;
}

LRESULT CTextControl::OnEditCutCommand(WPARAM wParam, LPARAM lParam)
{
	int len = m_chars->Count() - 1;
	bool b = ((len > 0) && (m_chars->has_Selection()));

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	if ( !_EditCopyCommand() )
		return 0;
	_EditDeleteCommand();
	return 0;
}

LRESULT CTextControl::OnEditCopyCommand(WPARAM wParam, LPARAM lParam)
{
	int len = m_chars->Count() - 1;
	bool b = ((len > 0) && (m_chars->has_Selection()));

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	_EditCopyCommand();
	return 0;
}

LRESULT CTextControl::OnEditPasteCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL b = ::IsClipboardFormatAvailable(CF_UNICODETEXT);

	if ( HIWORD(wParam) > 1 )
		return b;
	if ( !b )
		return 0;
	if ( !::OpenClipboard(theGuiApp->get_MainWnd()->get_handle()) ) 
        return 0; 
 
	CTextControlUndoStruct undoStruct;
    HGLOBAL hglb = ::GetClipboardData(CF_UNICODETEXT);

    if ( !hglb )
		goto _exit0;
	
	SIZE_T hglbSize = ::GlobalSize(hglb);

	if ( !hglbSize )
		goto _exit0;

    LPTSTR lptstr = (LPTSTR)(::GlobalLock(hglb));

    if ( !lptstr ) 
		goto _exit0;

	dword len0 = s_strlen(lptstr, UINT_MAX);

	_Edit_Delete_Selection(undoStruct);
	_Edit_Insert_Chars(undoStruct, lptstr, len0);
	m_undostack->push(undoStruct);
	Update(TRUE);
     GlobalUnlock(hglb); 
_exit0:
    CloseClipboard(); 
	return 0;
}

LRESULT CTextControl::OnEditDeleteCommand(WPARAM wParam, LPARAM lParam)
{
	int len = m_chars->Count() - 1;
	bool b = ((len > 0) && (m_chars->has_Selection()));

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	_EditDeleteCommand();
	return 0;
}

bool CTextControl::_EditCopyCommand()
{
	bool result = false;

	if ( !::OpenClipboard(theGuiApp->get_MainWnd()->get_handle()) )
        return false; 
    ::EmptyClipboard();

	int cch = m_chars->get_UpperBound() - m_chars->get_LowerBound() + 1;
	HGLOBAL hglbCopy = ::GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR));

	if ( !hglbCopy ) 
		goto _exit0;

	LPTSTR lptstrCopy = (LPTSTR)(::GlobalLock(hglbCopy));
	::MoveMemory(lptstrCopy, m_text.GetString() + m_chars->get_LowerBound(), cch * sizeof(TCHAR));
	lptstrCopy[cch] = (TCHAR)0;
	::GlobalUnlock(hglbCopy);
 
	::SetClipboardData(CF_UNICODETEXT, hglbCopy);

	SendNotifyMessage(NM_EDITORSELCHANGE, (LPARAM) this);
	SendNotifyMessage(NM_SELECTIONCHANGED);

	result = true;

_exit0:
	CloseClipboard(); 
	return result;
}

bool CTextControl::_EditDeleteCommand()
{
	CTextControlUndoStruct undoStruct;

	_Edit_Delete_Selection(undoStruct);
	m_undostack->push(undoStruct);
	Update(TRUE);
	return true;
}

void CTextControl::_Edit_Delete_Selection(CTextControlUndoStruct& undoStruct)
{
	int len = m_chars->Count() - 1;

	if ( (len > 0) && (m_chars->has_Selection()) )
	{
		int lb = m_chars->get_LowerBound();
		int cch = m_chars->get_UpperBound() - m_chars->get_LowerBound() + 1;

		undoStruct.deletePos = lb;
		undoStruct.deleteChars.SetString(__FILE__LINE__ m_text.GetString() + lb, cch);
		m_text.DeleteString(lb, cch);
		m_cursorPos = lb;
		m_chars->reset_Selection();
	}
}

void CTextControl::_Edit_Delete_Char(CTextControlUndoStruct& undoStruct)
{
	int len = m_chars->Count() - 1;

	if ( (len > 0) && (!(m_chars->has_Selection())) )
	{
		undoStruct.deletePos = m_cursorPos;
		undoStruct.deleteChars.SetString(__FILE__LINE__ m_text.GetString() + m_cursorPos, 1);
		m_text.DeleteString(m_cursorPos, 1);
	}
}

void CTextControl::_Edit_Insert_Char(CTextControlUndoStruct& undoStruct, TCHAR keyChar)
{
	TCHAR buf[2];

	buf[0] = keyChar;
	buf[1] = 0;
	undoStruct.insertPos = m_cursorPos;
	undoStruct.insertChars.SetString(__FILE__LINE__ buf, 1);
	m_text.InsertString(m_cursorPos, buf, 1);
	++m_cursorPos;
}

void CTextControl::_Edit_Insert_Chars(CTextControlUndoStruct& undoStruct, LPTSTR lptstr, dword len0)
{
	if ( len0 > 0 )
	{
		undoStruct.insertPos = m_cursorPos;
		undoStruct.insertChars.SetString(__FILE__LINE__ lptstr, len0);
		if ( !m_multiLine )
		{
			undoStruct.insertChars.ReplaceString(_T("\n\r"), _T(" "), 1);
			undoStruct.insertChars.ReplaceString(_T("\r\n"), _T(" "), 1);
			undoStruct.insertChars.ReplaceString(_T("\n"), _T(" "), 1);
			undoStruct.insertChars.ReplaceString(_T("\r"), _T(" "), 1);
			m_text.InsertString(m_cursorPos, undoStruct.insertChars.GetString(), undoStruct.insertChars.GetLength());
		}
		else
			m_text.InsertString(m_cursorPos, lptstr, len0);
		m_cursorPos += len0;
	}
}

BOOL CTextControl::_StartTimer()
{
	if ( m_timerEnabled )
		return TRUE;
	if ( StartTimer(1000, 200) )
	{
		m_timerEnabled = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CTextControl::_StopTimer()
{
	if ( m_timerEnabled )
	{
		if ( StopTimer(1000) )
		{
			m_timerEnabled = FALSE;
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}