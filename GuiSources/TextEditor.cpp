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
#include "TextEditor.h"
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

class CTextEditorChar
{
public:
	Gdiplus::RectF charRect;
	BOOL selected;
	BOOL isWhiteSpace;

	CTextEditorChar(): 
		charRect(0.0, 0.0, 0.0, 0.0), 
		selected(FALSE), 
		isWhiteSpace(FALSE) {}
};

typedef CDataSVectorT<CTextEditorChar> CTextEditorCharVector;

static void __stdcall TDeleteFunc_CTextEditorChars( ConstPointer data, Pointer context )
{
}

class CTextEditorChars: public CCppObject
{
public:
	CTextEditorChars() : m_sellb(-1), m_selub(-1), m_lastSellb(-1), m_lastSelub(-1), m_vector(__FILE__LINE__ 1024, 1024) {}
	virtual ~CTextEditorChars() { m_vector.Close(TDeleteFunc_CTextEditorChars, NULL); }

	__inline dword Count() const { return m_vector.Count(); }
	__inline void Clear() 
	{
		m_vector.Close(TDeleteFunc_CTextEditorChars, NULL);
		m_vector.Open(__FILE__LINE__ 1024, 1024);
		m_sellb= -1;
		m_selub= -1;
		m_lastSellb = -1;
		m_lastSelub = -1;
	}

	__inline void Append(CTextEditorChar& charInfo)
	{
		m_vector.Append(&charInfo);
	}

	CTextEditorChar& Index(dword ix)
	{
		return (*(*(m_vector.Index(ix))));
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

		CTextEditorCharVector::Iterator it = m_vector.Index(m_sellb);
		CTextEditorCharVector::Iterator itE = m_vector.Index(m_selub + 1);

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
				CTextEditorCharVector::Iterator it = m_vector.Index(newlbub + 1);
				CTextEditorCharVector::Iterator itE = m_vector.Index(m_selub + 1);

				while ( it != itE )
				{
					(*it)->selected = FALSE;
					++it;
				}
				m_selub = newlbub;
			}
			else
			{
				CTextEditorCharVector::Iterator it = m_vector.Index(m_selub + 1);
				CTextEditorCharVector::Iterator itE = m_vector.Index(newlbub + 1);

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
				CTextEditorCharVector::Iterator it = m_vector.Index(m_sellb);
				CTextEditorCharVector::Iterator itE = m_vector.Index(newlbub);

				while ( it != itE )
				{
					(*it)->selected = FALSE;
					++it;
				}
				m_sellb = newlbub;
			}
			else
			{
				CTextEditorCharVector::Iterator it = m_vector.Index(newlbub);
				CTextEditorCharVector::Iterator itE = m_vector.Index(m_sellb);

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

		CTextEditorCharVector::Iterator it = m_vector.Index(m_sellb);
		CTextEditorCharVector::Iterator itE = m_vector.Index(m_selub + 1);

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
			CTextEditorCharVector::Iterator it = m_vector.Index(ix);

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
			CTextEditorCharVector::Iterator it = m_vector.Index(ix);

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
			CTextEditorCharVector::Iterator it = m_vector.Index(ix);

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
			CTextEditorCharVector::Iterator it = m_vector.Index(ix);

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

	__inline int firstPosInClientRect(const Gdiplus::RectF& clientRectF, dword endPos)
	{
		if ( endPos >= m_vector.Count() )
			return -1;

		Gdiplus::RectF rectF = (*(m_vector.Index(endPos)))->charRect;
		CTextEditorCharVector::Iterator it = m_vector.Begin();
		int ix = 0;
		
		while ( it )
		{
			Gdiplus::RectF rectF1(rectF);
			Gdiplus::RectF rectF2((*it)->charRect);

			rectF1.Offset(-(rectF2.X), -(rectF2.Y));
			if ( clientRectF.Contains(rectF1) )
				return ix;
			++ix;
			++it;
		}
		return -1;
	}

	__inline int lastPosInClientRect(const Gdiplus::RectF& clientRectF, dword scrollPos)
	{
		if ( scrollPos >= m_vector.Count() )
			return -1;

		Gdiplus::RectF rectF = (*(m_vector.Index(scrollPos)))->charRect;
		CTextEditorCharVector::Iterator it = m_vector.Index(scrollPos);
		int ix = 0;

		while ( it )
		{
			Gdiplus::RectF rectF1 = (*it)->charRect;

			rectF1.Offset(-(rectF.X), -(rectF.Y));
			if ( !(clientRectF.Contains(rectF1)) )
				return ix - 1;
			++ix;
			++it;
		}
		return m_vector.Count() - 1;
	}

	__inline bool isFirstPosInClientRect(const Gdiplus::RectF& clientRectF, dword scrollPos, dword pos)
	{
		if ( (scrollPos >= m_vector.Count()) || (pos >= m_vector.Count()) )
			return false;
		if ( pos == 0 )
			return true;

		Gdiplus::RectF rectF = (*(m_vector.Index(scrollPos)))->charRect;
		Gdiplus::RectF rectF1 = (*(m_vector.Index(pos - 1)))->charRect;

		rectF1.Offset(-(rectF.X), -(rectF.Y));
		return (!(clientRectF.Contains(rectF1)));
	}

	__inline bool isLastPosInClientRect(const Gdiplus::RectF& clientRectF, dword scrollPos, dword pos)
	{
		if ( (scrollPos >= m_vector.Count()) || (pos >= m_vector.Count()) )
			return false;
		if ( pos == (m_vector.Count() - 1) )
			return true;

		Gdiplus::RectF rectF = (*(m_vector.Index(scrollPos)))->charRect;
		Gdiplus::RectF rectF1 = (*(m_vector.Index(pos + 1)))->charRect;

		rectF1.Offset(-(rectF.X), -(rectF.Y));
		return (!(clientRectF.Contains(rectF1)));
	}

	__inline bool isPosInClientRect(const Gdiplus::RectF& clientRectF, dword scrollPos, dword pos)
	{
		if ( (scrollPos >= m_vector.Count()) || (pos >= m_vector.Count()) )
			return false;

		Gdiplus::RectF rectF = (*(m_vector.Index(scrollPos)))->charRect;
		Gdiplus::RectF rectF1 = (*(m_vector.Index(pos)))->charRect;

		rectF1.Offset(-(rectF.X), -(rectF.Y));
		return (!(!(clientRectF.Contains(rectF1))));
	}

protected:
	int m_sellb;
	int m_selub;
	int m_lastSellb;
	int m_lastSelub;
	CTextEditorCharVector m_vector;
};

class CTextEditorUndoStruct
{
public:
	CStringBuffer deleteChars;
	int deletePos;
	CStringBuffer insertChars;
	int insertPos;

	CTextEditorUndoStruct():
		deletePos(-1),
		insertPos(-1) {}

	void addRef() 
	{
		if ( NotPtrCheck(deleteChars.GetString()) )
			deleteChars.addRef();
		if ( NotPtrCheck(insertChars.GetString()) )
			insertChars.addRef();
	}
};

typedef CDataSVectorT<CTextEditorUndoStruct> CTextEditorUndoVector;

static void __stdcall TDeleteFunc_CTextEditorUndoStack( ConstPointer data, Pointer context )
{
	Ptr(CTextEditorUndoStruct) p = CastAnyPtr(CTextEditorUndoStruct, CastMutable(Pointer, data));

	p->deleteChars.Clear();
	p->insertChars.Clear();
}

class CTextEditorUndoStack: public CCppObject
{
public:
	CTextEditorUndoStack() : m_currentPos(0), m_vector(__FILE__LINE__ 64, 32) {}
	virtual ~CTextEditorUndoStack() { m_vector.Close(TDeleteFunc_CTextEditorUndoStack, NULL); }

	__inline dword get_CurrentPos() const { return m_currentPos; }
	__inline dword size() const { return m_vector.Count(); }

	__inline void reset()
	{
		m_vector.Close(TDeleteFunc_CTextEditorUndoStack, NULL);
		m_vector.Open(__FILE__LINE__ 64, 32);
		m_currentPos = 0;
	}

	__inline void push(CTextEditorUndoStruct& undoStruct)
	{
		if ( (undoStruct.deletePos < 0) && (undoStruct.insertPos < 0) )
			return;
		if ( !m_currentPos )
		{
			reset();
			undoStruct.addRef();
			m_vector.Append(&undoStruct);
			m_currentPos = 1;
			return;
		}
		while ( m_currentPos < m_vector.Count() )
			m_vector.Remove(m_vector.Last(), TDeleteFunc_CTextEditorUndoStack, NULL);
		undoStruct.addRef();
		m_vector.Append(&undoStruct);
		++m_currentPos;
	}

	__inline CTextEditorUndoStruct& current()
	{
		return *(*(m_vector.Index(m_currentPos)));
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
	CTextEditorUndoVector m_vector;
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

BEGIN_MESSAGE_MAP(CControl, CTextEditor)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_MENUCOMMAND()
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
	ON_WM_MESSAGE(WM_GETDLGCODE,OnGetDlgCode)
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

CTextEditor::CTextEditor(LPCTSTR name):
    CControl(name),
	m_cursorPos(0),
	m_scrollPos(0),
	m_mouseHitCode(MOUSEHIT_NULL),
	m_mouseHitModifier(0),
	m_caretCreated(FALSE),
	m_caretShown(FALSE),
	m_timerEnabled(FALSE),
	m_timerForward(FALSE),
	m_hasselection(FALSE),
	m_hPopupMenu(NULL),
	m_chars(NULL),
	m_undostack(NULL)
{
	m_chars = OK_NEW_OPERATOR CTextEditorChars();
	m_undostack = OK_NEW_OPERATOR CTextEditorUndoStack();
}

CTextEditor::CTextEditor(ConstRef(CStringBuffer) name):
    CControl(name),
	m_cursorPos(0),
	m_scrollPos(0),
	m_mouseHitCode(MOUSEHIT_NULL),
	m_mouseHitModifier(0),
	m_caretCreated(FALSE),
	m_caretShown(FALSE),
	m_timerEnabled(FALSE),
	m_timerForward(FALSE),
	m_hasselection(FALSE),
	m_hPopupMenu(NULL),
	m_chars(NULL),
	m_undostack(NULL)
{
	m_chars = OK_NEW_OPERATOR CTextEditorChars();
	m_undostack = OK_NEW_OPERATOR CTextEditorUndoStack();
}

CTextEditor::~CTextEditor(void)
{
	delete m_chars;
	delete m_undostack;
	if ( m_hPopupMenu )
		::DestroyMenu(m_hPopupMenu);
	if ( m_caretCreated )
		::DestroyCaret();
}

BOOL CTextEditor::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CTEXTEDITOR");
	return TRUE;
}

void CTextEditor::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".BackgroundColor.Selected"), _T("TextEditor"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("TextEditor"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Selected"), _T("TextEditor"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("TextEditor"));
}

void CTextEditor::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".Font.Normal"), _T("TextEditor"));
}

void CTextEditor::Refresh()
{
	Update(TRUE);
}

void CTextEditor::set_text(const CStringBuffer& text) 
{ 
	m_text = text; 
	if ( is_created() )
		Reset(TRUE);
}

void CTextEditor::Reset(BOOL bUpdate)
{
	m_cursorPos = 0;
	m_scrollPos = 0;
	m_mouseHitCode = MOUSEHIT_NULL;
	m_mouseHitModifier = 0;
	m_chars->reset_Selection();
	m_undostack->reset();
	Update(bUpdate);
}

bool CTextEditor::has_selection() const
{
	return m_chars->has_Selection();
}

LRESULT CTextEditor::OnGetText(WPARAM wParam, LPARAM lParam)
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

LRESULT CTextEditor::OnGetTextLength(WPARAM wParam, LPARAM lParam)
{
	return get_text().GetLength();
}

LRESULT CTextEditor::OnSetText(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer text(__FILE__LINE__ CastAny(CPointer, lParam));

	set_text(text);
	return TRUE;
}

LRESULT CTextEditor::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
		return 0;

	Gdiplus::Graphics graphics(hdc);
	RECT clientrect;

	GetClientRect(&clientrect);
	Gdiplus::Bitmap bitmap(clientrect.right, clientrect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);

	Gdiplus::PointF pt(0.0, 0.0);
	RECT rect;
	Gdiplus::RectF rectF;
	Gdiplus::RectF clientrectF;
	CTextEditorChar techar_scrollPos;
	dword len = 0;
	Gdiplus::Font* pFont = get_Font(_T(".Font.Normal"), _T("TextEditor"));

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
				CTextEditorChar techar;

				if ( IsWhitespace(it[0]) )
				{
					techar.charRect.X = pt.X; techar.charRect.Y = pt.Y;
					techar.charRect.Height = fontHeight; techar.charRect.Width = 4;
					techar.isWhiteSpace = TRUE;
				}
				else
					graphics.MeasureString(it.GetCurrent(), 1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &(techar.charRect));
				m_chars->Append(techar);
				pt.X += techar.charRect.Width;
			}
		}

		CTextEditorChar techar;

		techar.charRect.X = pt.X; techar.charRect.Y = pt.Y;
		techar.charRect.Height = fontHeight; techar.charRect.Width = 4;
		techar.isWhiteSpace = TRUE;
		m_chars->Append(techar);
		m_maxClientArea.bottom = (LONG)(fontHeight + 4);
		m_maxClientArea.right = (LONG)(techar.charRect.X + techar.charRect.Width + 4);
		m_updateRects = FALSE;
	}
	if ( (m_mouseHitCode != MOUSEHIT_NULL) && (m_chars->Count() > 1) )
	{
		pt.X = (Gdiplus::REAL)(m_mouseHit.x); pt.Y = (Gdiplus::REAL)(m_mouseHit.y);
		if ( !(clientrectF.Contains(pt)) )
		{
			switch ( m_mouseHitCode )
			{
			case MOUSEHIT_LBUTTONMOVE:
				if ( (!(m_chars->isPosInClientRect(clientrectF, 0, 0))) || (!(m_chars->isPosInClientRect(clientrectF, 0, m_chars->Count() - 1))) )
				{
					if ( (pt.X > (clientrectF.X + clientrectF.Width - 10)) && (!(m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->Count() - 1))) )
					{
						m_timerForward = TRUE;
						_StartTimer();
					}
					else if ( (m_scrollPos > 0) && (pt.X < 10) )
					{
						m_timerForward = FALSE;
						_StartTimer();
					}
					else
						_StopTimer();
				}
				break;
			default:
				break;
			}
		}
		else
		{
			techar_scrollPos = m_chars->Index(m_scrollPos);
			for ( dword ix = m_scrollPos; ix < m_chars->Count(); ++ix )
			{
				CTextEditorChar techar = m_chars->Index(ix);

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

	Gdiplus::Brush* pBackgroundColorNormal = get_Brush(_T(".BackgroundColor.Normal"),  _T("TextEditor"), Gdiplus::Color::White);
	Gdiplus::Brush* pBackgroundColorSelected = get_Brush(_T(".BackgroundColor.Selected"),  _T("TextEditor"), Gdiplus::Color::Blue);
	Gdiplus::Brush* pForegroundColorNormal = get_Brush(_T(".ForegroundColor.Normal"),  _T("TextEditor"), Gdiplus::Color::Black);
	Gdiplus::Brush* pForegroundColorSelected = get_Brush(_T(".ForegroundColor.Selected"),  _T("TextEditor"), Gdiplus::Color::White);
	BOOL bShowCursor = TRUE;

	bitmapgraphics.FillRectangle(pBackgroundColorNormal, clientrectF);
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
					::CreateCaret(m_hwnd, (HBITMAP) NULL, 1, (int)(floor(fontHeight + 0.5))); 
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
		if ( m_chars->get_UpperBound() > m_chars->get_LastUpperBound() )
		{
			if ( m_chars->get_UpperBound() == (m_chars->get_LastUpperBound() + 1) ) 
			{
				if ( (!(m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->Count() - 1))) 
					&& m_chars->isLastPosInClientRect(clientrectF, m_scrollPos, m_chars->get_UpperBound()) )
				{
					++m_scrollPos;
				}
			}
			else
			{
				bool bLastUpperBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LastUpperBound());
				bool bUpperBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_UpperBound());
				bool bLowerBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LowerBound());

				if ( (bLastUpperBound && !bUpperBound) || (bLowerBound && !bUpperBound) || (!bLowerBound && !bUpperBound) )
				{
					int scrollPos = m_chars->firstPosInClientRect(clientrectF, m_chars->get_UpperBound());

					if ( scrollPos < 0 )
						m_scrollPos = 0;
					else
						m_scrollPos = scrollPos;
				}
			}
		}
		else if ( m_chars->get_UpperBound() < m_chars->get_LastUpperBound() )
		{
			if ( m_chars->get_UpperBound() == (m_chars->get_LastUpperBound() - 1) ) 
			{
				if ( m_chars->isFirstPosInClientRect(clientrectF, m_scrollPos, m_chars->get_UpperBound()) )
				{
					int scrollPos = m_chars->firstPosInClientRect(clientrectF, m_chars->get_UpperBound());

					if ( scrollPos < 0 )
						m_scrollPos = 0;
					else
						m_scrollPos = scrollPos;
				}
			}
			else
			{
				bool bLastUpperBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LastUpperBound());
				bool bUpperBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_UpperBound());

				if ( bLastUpperBound && (!bUpperBound) )
				{
					int scrollPos = m_chars->firstPosInClientRect(clientrectF, m_chars->get_UpperBound());

					if ( scrollPos < 0 )
						m_scrollPos = 0;
					else
						m_scrollPos = scrollPos;
				}
			}
		}
		else if ( m_chars->get_LowerBound() < m_chars->get_LastLowerBound() )
		{
			if ( m_chars->get_LowerBound() == (m_chars->get_LastLowerBound() - 1) ) 
			{
				if ( (m_scrollPos > 0) && m_chars->isFirstPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LowerBound()) )
				{
					--m_scrollPos;
				}
			}
			else 
			{
				bool bLastLowerBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LastLowerBound());
				bool bLowerBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LowerBound());

				if ( bLastLowerBound && (!bLowerBound) )
				{
					int scrollPos = m_chars->firstPosInClientRect(clientrectF, m_chars->get_LowerBound());

					if ( scrollPos < 0 )
						m_scrollPos = 0;
					else
						m_scrollPos = scrollPos;
				}
			}
		}
		else if ( m_chars->get_LowerBound() > m_chars->get_LastLowerBound() )
		{
			if ( m_chars->get_LowerBound() == (m_chars->get_LastLowerBound() + 1) ) 
			{
				if ( m_chars->isLastPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LowerBound()) )
				{
					int scrollPos = m_chars->firstPosInClientRect(clientrectF, m_chars->get_LowerBound());

					if ( scrollPos < 0 )
						m_scrollPos = 0;
					else
						m_scrollPos = scrollPos;
				}
			}
			else 
			{
				bool bLastLowerBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LastLowerBound());
				bool bLowerBound = m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LowerBound());

				if ( bLastLowerBound && (!bLowerBound) )
				{
					int scrollPos = m_chars->firstPosInClientRect(clientrectF, m_chars->get_LowerBound());

					if ( scrollPos < 0 )
						m_scrollPos = 0;
					else
						m_scrollPos = scrollPos;
				}
			}
		}
		m_chars->reset_Bounds();
	}
	else if ( m_cursorPos <= m_scrollPos )
	{
		int scrollPos = m_chars->firstPosInClientRect(clientrectF,m_cursorPos);

		if ( scrollPos < 0 )
			m_scrollPos = 0;
		else
			m_scrollPos = scrollPos;
	}
	else if ( !(m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_cursorPos)) )
	{
		int scrollPos = m_chars->firstPosInClientRect(clientrectF,m_cursorPos);

		if ( scrollPos < 0 )
			m_scrollPos = 0;
		else
			m_scrollPos = scrollPos;
	}
	if ( m_chars->Count() == 0 )
		return -1;
	techar_scrollPos = m_chars->Index(m_scrollPos);
	for ( dword ix = m_scrollPos; ix < m_chars->Count(); ++ix )
	{
		CTextEditorChar techar = m_chars->Index(ix);

		rectF = techar.charRect;
		rectF.Offset(-(techar_scrollPos.charRect.X), -(techar_scrollPos.charRect.Y));
		if ( !(clientrectF.Contains(rectF)) )
			break;
		if ( bShowCursor && (ix == m_cursorPos) )
		{
			Convert2Rect(&rect, &rectF);
			::SetCaretPos(rect.left, 0);
		}
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
		pt.X += rectF.Width;
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
	return 0;
}

LRESULT CTextEditor::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if ( (pt.x < 0) && (pt.y < 0) )
		::GetCursorPos(&pt);

	TNotifyMessageEditorContextMenu param(this, pt);

	SendNotifyMessage(NM_EDITORCONTEXTMENU, (LPARAM)(&param));
	return 0;
}

LRESULT CTextEditor::OnMenuCommand(WPARAM wParam, LPARAM lParam)
{
	if ( ((HMENU)lParam) == m_hPopupMenu )	
	{
		MENUITEMINFO mii;

		::SecureZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID;
		if ( !(::GetMenuItemInfo(m_hPopupMenu, Castdword(wParam), TRUE, &mii)) )
			return TRUE;
		SendMessage(WM_COMMAND, MAKEWPARAM(mii.wID, 0), 0);
	}
	return 0;
}

LRESULT CTextEditor::OnLButtonDown(WPARAM wParam, LPARAM lParam)
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

LRESULT CTextEditor::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{
		if ( m_timerEnabled )
		{
			::KillTimer(m_hwnd, 1000);
			m_timerEnabled = FALSE;
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
		}
		::ReleaseCapture();
		return 0;
	}
	return 0;
}

LRESULT CTextEditor::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
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

LRESULT CTextEditor::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

LRESULT CTextEditor::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

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
			if ( m_cursorPos <= 0 )
				return 0;
			--m_cursorPos;
		}
	case VK_DELETE:
		if ( !(bShift || bControl) )
		{
			dword len = m_chars->Count() - 1;
			CTextEditorUndoStruct undoStruct;

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
		{
			dword len = m_chars->Count() - 1;

			if ( len == 0 )
				break;
			if ( bShift && bControl )
			{
			}
			else if ( bShift )
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
			else if ( bControl )
			{
			}
			else
			{
				m_chars->reset_Selection();
				m_cursorPos = 0;
				Update(FALSE);
			}
		}
		break;
	case VK_END:
		{
			dword len = m_chars->Count() - 1;

			if ( len == 0 )
				break;
			if ( bShift && bControl )
			{
			}
			else if ( bShift )
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
			else if ( bControl )
			{
			}
			else
			{
				m_chars->reset_Selection();
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
		SendNotifyMessage(NM_EDITORCURSORUP, (LPARAM) this);
		break;
	case VK_DOWN:
		SendNotifyMessage(NM_EDITORCURSORDOWN, (LPARAM) this);
		break;
	case VK_ESCAPE:
		SendNotifyMessage(NM_EDITORESCAPE, (LPARAM) this);
		break;
	case VK_RETURN:
		SendNotifyMessage(NM_EDITORRETURN, (LPARAM) this);
		break;
	case VK_TAB:
		SendNotifyMessage(NM_EDITORTAB, (LPARAM) this);
		break;
	default:
		if ( (wParam >= VK_F1) && (wParam <= VK_F24) )
		{
			TNotifyMessageEditorFuncKey param(this, wParam);

			SendNotifyMessage(NM_EDITORFUNCKEY, (LPARAM)(&param));
		}
		break;
	}
	return 0;
}

LRESULT CTextEditor::OnChar(WPARAM wParam, LPARAM lParam)
{
	if ( wParam < 0x20 )
		return 0;

	CTextEditorUndoStruct undoStruct;

	_Edit_Delete_Selection(undoStruct);
	_Edit_Insert_Char(undoStruct, Cast(TCHAR, wParam));
	m_undostack->push(undoStruct);
	Update(TRUE);
	return 0;
}

LRESULT CTextEditor::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	Update(FALSE);
	return 0;
}

LRESULT CTextEditor::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	_StopTimer();
	::DestroyCaret();
	m_caretShown = FALSE;
	m_caretCreated = FALSE;
	SendNotifyMessage(NM_EDITORLOSTFOCUS, (LPARAM) this);
	return 0;
}

LRESULT CTextEditor::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 1000 )
	{
		RECT clientrect;
		Gdiplus::RectF clientrectF;

		GetClientRect(&clientrect);
		Convert2RectF(&clientrectF, &clientrect);
		if ( (m_scrollPos == 0) || m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->Count() - 1) )
		{
			_StopTimer();
			return 0;
		}
		if ( m_timerForward )
		{
			dword len = m_chars->Count() - 1;

			_ASSERTE(m_chars->Count() != 0);
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
		else
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
	}
	return 0;
}

LRESULT CTextEditor::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTextEditor::OnGetDlgCode(WPARAM wParam, LPARAM lParam)
{
	return DLGC_WANTALLKEYS;
}

LRESULT CTextEditor::OnEditUndoCommand(WPARAM wParam, LPARAM lParam)
{
	bool b = m_undostack->get_CurrentPos() > 0;

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	m_undostack->undo();

	CTextEditorUndoStruct undoStruct = m_undostack->current();

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

LRESULT CTextEditor::OnEditRedoCommand(WPARAM wParam, LPARAM lParam)
{
	bool b = m_undostack->get_CurrentPos() < m_undostack->size();

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;

	CTextEditorUndoStruct undoStruct = m_undostack->current();

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

LRESULT CTextEditor::OnEditCutCommand(WPARAM wParam, LPARAM lParam)
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

LRESULT CTextEditor::OnEditCopyCommand(WPARAM wParam, LPARAM lParam)
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

LRESULT CTextEditor::OnEditPasteCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL b = ::IsClipboardFormatAvailable(CF_UNICODETEXT);

	if ( HIWORD(wParam) > 1 )
		return b;
	if ( !b )
		return 0;
	if ( !::OpenClipboard(theGuiApp->get_MainWnd()->get_handle()) ) 
        return 0; 
 
	CTextEditorUndoStruct undoStruct;
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

LRESULT CTextEditor::OnEditDeleteCommand(WPARAM wParam, LPARAM lParam)
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

bool CTextEditor::_EditCopyCommand()
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

bool CTextEditor::_EditDeleteCommand()
{
	CTextEditorUndoStruct undoStruct;

	_Edit_Delete_Selection(undoStruct);
	m_undostack->push(undoStruct);
	Update(TRUE);
	return true;
}

void CTextEditor::_Edit_Delete_Selection(CTextEditorUndoStruct& undoStruct)
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

void CTextEditor::_Edit_Delete_Char(CTextEditorUndoStruct& undoStruct)
{
	int len = m_chars->Count() - 1;

	if ( (len > 0) && (!(m_chars->has_Selection())) )
	{
		undoStruct.deletePos = m_cursorPos;
		undoStruct.deleteChars.SetString(__FILE__LINE__ m_text.GetString() + m_cursorPos, 1);
		m_text.DeleteString(m_cursorPos, 1);
	}
}

void CTextEditor::_Edit_Insert_Char(CTextEditorUndoStruct& undoStruct, TCHAR keyChar)
{
	TCHAR buf[2];

	buf[0] = keyChar;
	buf[1] = 0;
	undoStruct.insertPos = m_cursorPos;
	undoStruct.insertChars.SetString(__FILE__LINE__ buf, 1);
	m_text.InsertString(m_cursorPos, buf, 1);
	++m_cursorPos;
}

void CTextEditor::_Edit_Insert_Chars(CTextEditorUndoStruct& undoStruct, LPTSTR lptstr, dword len0)
{
	if ( len0 > 0 )
	{
		undoStruct.insertPos = m_cursorPos;
		undoStruct.insertChars.SetString(__FILE__LINE__ lptstr, len0);
		undoStruct.insertChars.ReplaceString(_T("\n\r"), _T(" "), 1);
		undoStruct.insertChars.ReplaceString(_T("\r\n"), _T(" "), 1);
		undoStruct.insertChars.ReplaceString(_T("\n"), _T(" "), 1);
		undoStruct.insertChars.ReplaceString(_T("\r"), _T(" "), 1);
		m_text.InsertString(m_cursorPos, undoStruct.insertChars.GetString(), undoStruct.insertChars.GetLength());
		m_cursorPos += len0;
	}
}

BOOL CTextEditor::_StartTimer()
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

BOOL CTextEditor::_StopTimer()
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
