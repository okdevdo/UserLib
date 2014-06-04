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
#include "ConsoleWindow.h"
#include "Console.h"

static const TCHAR _graphics[] = {

#ifdef OK_SYS_WINDOWS
#ifdef _UNICODE
// 9472
//─?│?????????┌???┐???└???┘???├???????┤???????┬???????┴???????┼???????????????????
//01234567890123456789012345678901234567890123456789012345678901234567890123456789
//          1         2         3         4         5         6         7
// 9552
//═║╔╔╔╗╗╗╚╚╚╝╝╝╠╠╠╣╣╣╦╦╦╩╩╩╬╬╬???????????????????▀???▄???█???█???█░▒▓????????????
//01234567890123456789012345678901234567890123456789012345678901234567890123456789
//          1         2         3         4         5         6         7
// 9632
//■???????????▬?????▲???????►?▼???????◄
//01234567890123456789012345678901234567890123456789012345678901234567890123456789
//          1         2         3         4         5         6         7
		9472, // bordersingleleftright
		9474, // bordersingleupdown
		9484, // bordersinglerightdown
		9488, // bordersingleleftdown
		9492, // bordersinglerightup
		9496, // bordersingleleftup
		9500, // bordersinglerightupdown
		9508, // bordersingleleftupdown
		9516, // bordersingleleftrightdown
		9524, // bordersingleleftrightup
		9532, // bordersingleleftrightupdown

		9552, // borderdoubleleftright
		9553, // borderdoubleupdown
		9556, // borderdoublerightdown
		9557, // borderdoubleleftdown
		9562, // borderdoublerightup
		9563, // borderdoubleleftup
		9572, // borderdoublerightupdown
		9573, // borderdoubleleftupdown
		9578, // borderdoubleleftrightdown
		9579, // borderdoubleleftrightup
		9583, // borderdoubleleftrightupdown

		9604, // thumbhalfup
		9608, // thumbhalfdown
		9612, // thumbfull
		9632, // thumbhalfmiddle

		9650, // arrowtop
		9658, // arrowright
		9660, // arrowbottom
		9668 // arrowleft
#else
		'-', // bordersingleleftright
		'|', // bordersingleupdown
		'+', // bordersinglerightdown
		'+', // bordersingleleftdown
		'+', // bordersinglerightup
		'+', // bordersingleleftup
		'+', // bordersinglerightupdown
		'+', // bordersingleleftupdown
		'+', // bordersingleleftrightdown
		'+', // bordersingleleftrightup
		'+', // bordersingleleftrightupdown

		'-', // borderdoubleleftright
		'|', // borderdoubleupdown
		'+', // borderdoublerightdown
		'+', // borderdoubleleftdown
		'+', // borderdoublerightup
		'+', // borderdoubleleftup
		'+', // borderdoublerightupdown
		'+', // borderdoubleleftupdown
		'+', // borderdoubleleftrightdown
		'+', // borderdoubleleftrightup
		'+', // borderdoubleleftrightupdown

		'#', // thumbhalfup
		'#', // thumbhalfdown
		'#', // thumbfull
		'#', // thumbhalfmiddle

		'^', // arrowtop
		'>', // arrowright
		'V', // arrowbottom
		'<' // arrowleft
#endif
#endif
#ifdef OK_SYS_UNIX
#ifdef _UNICODE
		WACS_HLINE, // bordersingleleftright
		WACS_VLINE, // bordersingleupdown
		WACS_URCORNER, // bordersinglerightdown
		WACS_ULCORNER, // bordersingleleftdown
		WACS_LRCORNER, // bordersinglerightup
		WACS_LLCORNER, // bordersingleleftup
		WACS_RTEE, // bordersinglerightupdown
		WACS_LTEE, // bordersingleleftupdown
		WACS_BTEE, // bordersingleleftrightdown
		WACS_TTEE, // bordersingleleftrightup
		WACS_PLUS, // bordersingleleftrightupdown

		WACS_D_HLINE, // borderdoubleleftright
		WACS_D_VLINE, // borderdoubleupdown
		WACS_D_URCORNER, // borderdoublerightdown
		WACS_D_ULCORNER, // borderdoubleleftdown
		WACS_D_LRCORNER, // borderdoublerightup
		WACS_D_LLCORNER, // borderdoubleleftup
		WACS_D_RTEE, // borderdoublerightupdown
		WACS_D_LTEE, // borderdoubleleftupdown
		WACS_D_BTEE, // borderdoubleleftrightdown
		WACS_D_TTEE, // borderdoubleleftrightup
		WACS_D_PLUS, // borderdoubleleftrightupdown

		WACS_BLOCK, // thumbhalfup
		WACS_BLOCK, // thumbhalfdown
		WACS_BLOCK, // thumbfull
		WACS_BLOCK, // thumbhalfmiddle

		WACS_UARROW, // arrowtop
		WACS_RARROW, // arrowright
		WACS_DARROW, // arrowbottom
		WACS_LARROW // arrowleft
#else
/*		ACS_HLINE, // bordersingleleftright
		ACS_VLINE, // bordersingleupdown
		ACS_URCORNER, // bordersinglerightdown
		ACS_ULCORNER, // bordersingleleftdown
		ACS_LRCORNER, // bordersinglerightup
		ACS_LLCORNER, // bordersingleleftup
		ACS_RTEE, // bordersinglerightupdown
		ACS_LTEE, // bordersingleleftupdown
		ACS_BTEE, // bordersingleleftrightdown
		ACS_TTEE, // bordersingleleftrightup
		ACS_PLUS, // bordersingleleftrightupdown

		ACS_HLINE, // borderdoubleleftright
		ACS_VLINE, // borderdoubleupdown
		ACS_URCORNER, // borderdoublerightdown
		ACS_ULCORNER, // borderdoubleleftdown
		ACS_LRCORNER, // borderdoublerightup
		ACS_LLCORNER, // borderdoubleleftup
		ACS_RTEE, // borderdoublerightupdown
		ACS_LTEE, // borderdoubleleftupdown
		ACS_BTEE, // borderdoubleleftrightdown
		ACS_TTEE, // borderdoubleleftrightup
		ACS_PLUS, // borderdoubleleftrightupdown

		ACS_BLOCK, // thumbhalfup
		ACS_BLOCK, // thumbhalfdown
		ACS_BLOCK, // thumbfull
		ACS_BLOCK, // thumbhalfmiddle

		ACS_UARROW, // arrowtop
		ACS_RARROW, // arrowright
		ACS_DARROW, // arrowbottom
		ACS_LARROW // arrowleft
*/
		'-', // bordersingleleftright
		'|', // bordersingleupdown
		'+', // bordersinglerightdown
		'+', // bordersingleleftdown
		'+', // bordersinglerightup
		'+', // bordersingleleftup
		'+', // bordersinglerightupdown
		'+', // bordersingleleftupdown
		'+', // bordersingleleftrightdown
		'+', // bordersingleleftrightup
		'+', // bordersingleleftrightupdown

		'-', // borderdoubleleftright
		'|', // borderdoubleupdown
		'+', // borderdoublerightdown
		'+', // borderdoubleleftdown
		'+', // borderdoublerightup
		'+', // borderdoubleleftup
		'+', // borderdoublerightupdown
		'+', // borderdoubleleftupdown
		'+', // borderdoubleleftrightdown
		'+', // borderdoubleleftrightup
		'+', // borderdoubleleftrightupdown

		'#', // thumbhalfup
		'#', // thumbhalfdown
		'#', // thumbfull
		'#', // thumbhalfmiddle

		'^', // arrowtop
		'>', // arrowright
		'V', // arrowbottom
		'<' // arrowleft
#endif
#endif
	};

CConsoleWindow::CConsoleWindow(CConstPointer name, CConsole* pConsole):
    m_Console(pConsole),
	m_Name(__FILE__LINE__ name),
    m_ScreenBuffer(nullptr),
	m_Color(pConsole->GetDefaultColor()),
	m_HighLightColor(pConsole->GetDefaultHighlightedColor()),
	m_Title(),
    m_hasBorder(false),
	m_BorderStyle(noborderstyle),
    m_hasTitle(false),
	m_TitleStyle(notitlebarstyle),
	m_lastPosIn(PosInNone),
	m_lastButtonState(0),
    m_Maximized(false),
	m_InsideMoveResize(0),
	m_Resizeable(true),
    m_Created(false)
{
}

CConsoleWindow::CConsoleWindow(CConstPointer name, CConstPointer title, CConsole* pConsole):
    m_Console(pConsole),
	m_Name(__FILE__LINE__ name),
    m_ScreenBuffer(nullptr),
	m_Color(pConsole->GetDefaultColor()),
	m_HighLightColor(pConsole->GetDefaultHighlightedColor()),
	m_Title(__FILE__LINE__ title),
    m_hasBorder(false),
	m_BorderStyle(noborderstyle),
    m_hasTitle(false),
	m_TitleStyle(notitlebarstyle),
	m_lastPosIn(PosInNone),
	m_lastButtonState(0),
    m_Maximized(false),
	m_InsideMoveResize(0),
	m_Resizeable(true),
    m_Created(false)
{
}

CConsoleWindow::~CConsoleWindow(void)
{
	if ( m_ScreenBuffer )
		TFfree(m_ScreenBuffer);
}

void CConsoleWindow::Create(COORD pos, COORD size)
{
	m_NonClientArea.Left = pos.X;
	m_NonClientArea.Right = pos.X + size.X;
	m_NonClientArea.Top = pos.Y;
	m_NonClientArea.Bottom = pos.Y + size.Y;

	if ( m_hasBorder || m_hasTitle )
	{
		m_ClientArea.Left = m_NonClientArea.Left + 1;
		m_ClientArea.Right = m_NonClientArea.Right - 1;
		m_ClientArea.Top = m_NonClientArea.Top + 1;
		m_ClientArea.Bottom = m_NonClientArea.Bottom - 1;
		if ( m_hasTitle )
		{
			switch ( m_TitleStyle )
			{
			case windowtitlebarstyle:
				if ( m_hasScrollbar )
				{
					switch ( m_ScrollBarStyle )
					{
					case horizontalscrollbarstyle:
						m_ClientArea.Right -= 1;
						break;
					case verticalscrollbarstyle:
						m_ClientArea.Bottom -= 1;
						break;
					case bothscrollbarstyle:
						m_ClientArea.Right -= 1;
						m_ClientArea.Bottom -= 1;
						break;
					default:
						break;
					}
				}
				break;
			default:
				break;
			}
		}
	}
	else
		m_ClientArea = m_NonClientArea;

	m_ClientAreaSize.X = m_ClientArea.Right - m_ClientArea.Left;
	m_ClientAreaSize.Y = m_ClientArea.Bottom - m_ClientArea.Top;

	m_ScreenBufferSize = size;

	if ( m_ScreenBuffer )
		TFfree(m_ScreenBuffer);
	m_ScreenBuffer = CastAnyPtr(CHAR_INFO, TFalloc(
	    m_ScreenBufferSize.X * m_ScreenBufferSize.Y * sizeof(CHAR_INFO)));

	ClearDrawingArea();
	if ( m_hasBorder )
	{
		switch ( m_BorderStyle )
		{
		case singleborderstyle:
			DrawSingleBorder();
			break;
		case doubleborderstyle:
			DrawDoubleBorder();
			break;
		default:
			break;
		}
	}
	if ( m_hasScrollbar )
	{
		switch ( m_ScrollBarStyle )
		{
		case horizontalscrollbarstyle:
			DrawHorizontalScrollBar();
			break;
		case verticalscrollbarstyle:
			DrawVerticalScrollBar();
			break;
		case bothscrollbarstyle:
			DrawHorizontalScrollBar();
			DrawVerticalScrollBar();
			break;
		default:
			break;
		}
	}
	if ( m_hasTitle )
	{
		switch ( m_TitleStyle )
		{
		case controltitlebarstyle:
			DrawControlTitleBar();
			break;
		case dialogtitlebarstyle:
			DrawDialogTitleBar();
			break;
		case windowtitlebarstyle:
			DrawWindowTitleBar();
			break;
		default:
			break;
		}
	}
	m_Created = true;
}

void CConsoleWindow::ClearDrawingArea()
{
	WORD max = m_ScreenBufferSize.X * m_ScreenBufferSize.Y;
	WORD ix; 

	for ( ix = 0; ix < max; ++ix )
	{
#ifdef _UNICODE
		m_ScreenBuffer[ix].Char.UnicodeChar = 0x0020;
#else
		m_ScreenBuffer[ix].Char.AsciiChar = 0x20;
#endif
		m_ScreenBuffer[ix].Attributes = m_Color;
	}
}

TCHAR CConsoleWindow::GetBorderChar(CConsoleWindow::BorderGraphics ix)
{
	return _graphics[ix];
}

void CConsoleWindow::DrawSingleBorder()
{
	COORD pos;

	pos.X = 0; pos.Y = 0;
	DrawSingleChar(pos, _graphics[bordersinglerightdown], m_Color);
	for ( pos.X = 1; pos.X < (m_ScreenBufferSize.X - 1); ++(pos.X) )
		DrawSingleChar(pos, _graphics[bordersingleleftright], m_Color);
	DrawSingleChar(pos, _graphics[bordersingleleftdown], m_Color);
	pos.X = 0; 
	for ( pos.Y = 1; pos.Y < (m_ScreenBufferSize.Y - 1); ++(pos.Y) )
		DrawSingleChar(pos, _graphics[bordersingleupdown], m_Color);
	DrawSingleChar(pos, _graphics[bordersinglerightup], m_Color);
	pos.X = m_ScreenBufferSize.X - 1; 
	for ( pos.Y = 1; pos.Y < (m_ScreenBufferSize.Y - 1); ++(pos.Y) )
		DrawSingleChar(pos, _graphics[bordersingleupdown], m_Color);
	DrawSingleChar(pos, _graphics[bordersingleleftup], m_Color);
	pos.Y = m_ScreenBufferSize.Y - 1;
	for ( pos.X = 1; pos.X < (m_ScreenBufferSize.X - 1); ++(pos.X) )
		DrawSingleChar(pos, _graphics[bordersingleleftright], m_Color);
}

void CConsoleWindow::DrawDoubleBorder()
{
	COORD pos;

	pos.X = 0; pos.Y = 0;
	DrawSingleChar(pos, _graphics[borderdoublerightdown], m_Color);
	for ( pos.X = 1; pos.X < (m_ScreenBufferSize.X - 1); ++(pos.X) )
		DrawSingleChar(pos, _graphics[borderdoubleleftright], m_Color);
	DrawSingleChar(pos, _graphics[borderdoubleleftdown], m_Color);
	pos.X = 0; 
	for ( pos.Y = 1; pos.Y < (m_ScreenBufferSize.Y - 1); ++(pos.Y) )
		DrawSingleChar(pos, _graphics[borderdoubleupdown], m_Color);
	DrawSingleChar(pos, _graphics[borderdoublerightup], m_Color);
	pos.X = m_ScreenBufferSize.X - 1; 
	for ( pos.Y = 1; pos.Y < (m_ScreenBufferSize.Y - 1); ++(pos.Y) )
		DrawSingleChar(pos, _graphics[borderdoubleupdown], m_Color);
	DrawSingleChar(pos, _graphics[borderdoubleleftup], m_Color);
	pos.Y = m_ScreenBufferSize.Y - 1;
	for ( pos.X = 1; pos.X < (m_ScreenBufferSize.X - 1); ++(pos.X) )
		DrawSingleChar(pos, _graphics[borderdoubleleftright], m_Color);
}

void CConsoleWindow::DrawControlTitleBar()
{
	SMALL_RECT rect;

	rect.Top = 0;
	rect.Bottom = 0;
	rect.Left = 2;
	rect.Right = m_ScreenBufferSize.X - 2;
	assert( rect.Right > 0 );

	if ( m_Title.IsEmpty() )
		m_Title.SetString(__FILE__LINE__ _T(" "));
	DrawString(rect, m_Title.GetString(), m_Color, false);
}

void CConsoleWindow::DrawDialogTitleBar()
{
	COORD pos;

	pos.X = m_ScreenBufferSize.X - 4;
	assert( pos.X > 0 );
	pos.Y = 0;

	DrawSingleChar(pos, _T('('), m_Color);
	++(pos.X);
	DrawSingleChar(pos, _T('*'), m_Color);
	++(pos.X);
	DrawSingleChar(pos, _T(')'), m_Color);

	SMALL_RECT rect;

	rect.Top = 0;
	rect.Bottom = 0;
	rect.Left = 2;
	rect.Right = m_ScreenBufferSize.X - 5;
	assert( rect.Right > 0 );

	if ( m_Title.IsEmpty() )
		m_Title.SetString(__FILE__LINE__ _T(" "));
	DrawString(rect, m_Title.GetString(), m_Color, false);
}

void CConsoleWindow::DrawWindowTitleBar()
{
	COORD pos;

	pos.X = m_ScreenBufferSize.X - 7;
	assert( pos.X > 0 );
	pos.Y = 0;

	DrawSingleChar(pos, _T('('), m_Color);
	++(pos.X);
	DrawSingleChar(pos, _T('='), m_Color);
	++(pos.X);
	DrawSingleChar(pos, _T(')'), m_Color);
	++(pos.X);
	DrawSingleChar(pos, _T('('), m_Color);
	++(pos.X);
	DrawSingleChar(pos, _T('*'), m_Color);
	++(pos.X);
	DrawSingleChar(pos, _T(')'), m_Color);

	SMALL_RECT rect;

	rect.Top = 0;
	rect.Bottom = 0;
	rect.Left = 2;
	rect.Right = m_ScreenBufferSize.X - 8;
	assert( rect.Right > 0 );

	if ( m_Title.IsEmpty() )
		m_Title.SetString(__FILE__LINE__ _T(" "));
	DrawString(rect, m_Title.GetString(), m_Color, false);
}

void CConsoleWindow::DrawVerticalScrollBar()
{
	SMALL_RECT rect;

	rect.Right = m_NonClientArea.Right - m_NonClientArea.Left - 1;
	rect.Left = rect.Right;
	rect.Top = 1;
	rect.Bottom = m_NonClientArea.Bottom - m_NonClientArea.Top - 1;
	if ( m_hasTitle )
	{
		switch ( m_TitleStyle )
		{
		case windowtitlebarstyle:
			rect.Right -= 1;
			rect.Left = rect.Right;
			rect.Bottom -= 1;
			break;
		default:
			break;
		}
	}

	COORD pos;
	sword sizeY = rect.Bottom - rect.Top;

	pos.X = rect.Left;
	pos.Y = rect.Top;
	DrawSingleChar(pos, _graphics[arrowtop], m_Color);
	++(pos.Y);
	for ( sword ix = 0; ix < sizeY - 2; ++ix )
	{
		DrawSingleChar(pos, _graphics[bordersingleupdown], m_Color);
		++(pos.Y);
	}
	DrawSingleChar(pos, _graphics[arrowbottom], m_Color);
	if ( m_ScrollBarVInfo.Current < m_ScrollBarVInfo.Minimum )
	{
		pos.Y = rect.Top + 1;
		m_ScrollBarVInfo.Current = m_ScrollBarVInfo.Minimum;
	}
	else 
	{
		if ( m_ScrollBarVInfo.Current > m_ScrollBarVInfo.Maximum )
			m_ScrollBarVInfo.Current = m_ScrollBarVInfo.Maximum;
		pos.Y = rect.Top + Cast(sword, ((m_ScrollBarVInfo.Current * m_ScrollBarVInfo.PageStep) / m_ScrollBarVInfo.Maximum)) + 1;
		if (  (pos.Y + m_ScrollBarVInfo.ThumbSize) > sizeY )
		{
			if ( m_ScrollBarVInfo.ThumbSize > (sizeY - 2) )
				m_ScrollBarVInfo.ThumbSize = sizeY - 2;
			m_ScrollBarVInfo.Current = ((sizeY - 2 - m_ScrollBarVInfo.ThumbSize) * m_ScrollBarVInfo.Maximum) / m_ScrollBarVInfo.PageStep;
			pos.Y = rect.Top + Cast(sword, ((m_ScrollBarVInfo.Current * m_ScrollBarVInfo.PageStep) / m_ScrollBarVInfo.Maximum)) + 1;
		}
	}
	for ( sword ix = 0; ix < m_ScrollBarVInfo.ThumbSize; ++ix )
	{
		DrawSingleChar(pos, _graphics[thumbfull], m_Color);
		++(pos.Y);
	}
}

void CConsoleWindow::DrawHorizontalScrollBar()
{
	SMALL_RECT rect;

	rect.Left = 1;
	rect.Right = m_NonClientArea.Right - m_NonClientArea.Left - 1;
	rect.Bottom = m_NonClientArea.Bottom - m_NonClientArea.Top - 1;
	rect.Top = rect.Bottom;
	if ( m_hasTitle )
	{
		switch ( m_TitleStyle )
		{
		case windowtitlebarstyle:
			rect.Right -= 1;
			rect.Bottom -= 1;
			rect.Top = rect.Bottom;
			break;
		default:
			break;
		}
	}

	COORD pos;
	sword sizeX = rect.Right - rect.Left;

	pos.X = rect.Left;
	pos.Y = rect.Top;
	DrawSingleChar(pos, _graphics[arrowleft], m_Color);
	++(pos.X);
	for ( sword ix = 0; ix < sizeX - 2; ++ix )
	{
		DrawSingleChar(pos, _graphics[bordersingleleftright], m_Color);
		++(pos.X);
	}
	DrawSingleChar(pos, _graphics[arrowright], m_Color);
	if ( m_ScrollBarHInfo.Current < m_ScrollBarHInfo.Minimum )
	{
		m_ScrollBarHInfo.Current = m_ScrollBarHInfo.Minimum;
		pos.X = rect.Left + 1;
	}
	else
	{
		if ( m_ScrollBarHInfo.Current > m_ScrollBarHInfo.Maximum )
			m_ScrollBarHInfo.Current = m_ScrollBarHInfo.Maximum;
		pos.X = rect.Left + Cast(sword, ((m_ScrollBarHInfo.Current * m_ScrollBarHInfo.PageStep) / m_ScrollBarHInfo.Maximum)) + 1;
		if (  (pos.X + m_ScrollBarHInfo.ThumbSize) > sizeX )
		{
			if ( m_ScrollBarHInfo.ThumbSize > (sizeX - 2) )
				m_ScrollBarHInfo.ThumbSize = sizeX - 2;
			m_ScrollBarHInfo.Current = ((sizeX - 2 - m_ScrollBarHInfo.ThumbSize) * m_ScrollBarHInfo.Maximum) / m_ScrollBarHInfo.PageStep;
			pos.X = rect.Left + Cast(sword, ((m_ScrollBarHInfo.Current * m_ScrollBarHInfo.PageStep) / m_ScrollBarHInfo.Maximum)) + 1;
		}
	}
	for ( int ix = 0; ix < m_ScrollBarHInfo.ThumbSize; ++ix )
	{
		DrawSingleChar(pos, _graphics[thumbhalfmiddle], m_Color);
		++(pos.X);
	}
}

void CConsoleWindow::DrawSingleChar(COORD pos, TCHAR ch, color_t color)
{
	assert( pos.X < m_ScreenBufferSize.X );
	assert( pos.Y < m_ScreenBufferSize.Y );
	WORD ix = pos.X + (m_ScreenBufferSize.X * pos.Y);
#ifdef _UNICODE
	m_ScreenBuffer[ix].Char.UnicodeChar = ch;
#else
	m_ScreenBuffer[ix].Char.AsciiChar = ch;
#endif
	m_ScreenBuffer[ix].Attributes = color;
}

void CConsoleWindow::DrawString(ConstRef(SMALL_RECT) rect, CConstPointer ch, color_t color, bool bFill)
{
	assert(ch != nullptr);
	assert(rect.Left <= rect.Right);
	assert(rect.Top <= rect.Bottom);

	if ( rect.Left == rect.Right )
		return;

	COORD pos;
	TCHAR cch = 0;
	bool bTab = false;
	sword tcnt = 0;
	sword borderOffset = m_ClientArea.Left - m_NonClientArea.Left;

	pos.X = rect.Left;
	pos.Y = rect.Top;
	while ( *ch )
	{
		switch ( *ch )
		{
		case 0x09:
			if ( !bTab )
			{
				tcnt = (((pos.X - borderOffset) + 4) / 4) * 4;
				bTab = true;
				cch = _T(' ');
			}
			else if ( (pos.X - borderOffset) == tcnt )
			{
				++ch;
				if ( *ch == 0 )
				{
					cch = 0;
					bTab = false;
				}
				else if ( *ch != 0x09 )
				{
					cch = *ch++;
					bTab = false;
				}
				else
					tcnt += 4;
			}
			break;
		default:
			cch = *ch++;
			bTab = false;
			break;
		}
		if ( cch != 0 )
		{
			DrawSingleChar(pos, cch, color);
			++(pos.X);
		}
		if ( pos.X >= rect.Right )
		{
			bTab = false;
			pos.X = rect.Left;
			++(pos.Y);
			if ( pos.Y >= rect.Bottom )
				break;
		}
	}

	if ( (pos.Y >= rect.Bottom) || (!bFill) )
		return;

	TCHAR chFill = _T(' ');

	while ( true )
	{
		DrawSingleChar(pos, chFill, color);
		++(pos.X);
		if ( pos.X >= rect.Right )
		{
			pos.X = rect.Left;
			++(pos.Y);
			if ( pos.Y >= rect.Bottom )
				break;
		}
	}
}

sdword CConsoleWindow::GetTextLength(CPointer text)
{
	bool bTab = false;
	sdword cnt = 0;
	sdword tcnt = 0;

	while ( *text )
	{
		switch ( *text )
		{
		case 0x09:
			if ( !bTab )
			{
				tcnt = ((cnt + 4) / 4) * 4;
				bTab = true;
			}
			else if ( cnt == tcnt )
			{
				++text;
				if ( *text == 0 )
					return cnt;
				if ( *text != 0x09 )
				{
					++text;
					bTab = false;
				}
				else
					tcnt += 4;
			}
			break;
		default:
			++text;
			bTab = false;
			break;
		}
		++cnt;
	}
	return cnt;
}

CConstPointer CConsoleWindow::GetTextPointer(CConstPointer text, sdword pos)
{
	bool bTab = false;
	sdword cnt = 0;
	sdword tcnt = 0;

	while ( (cnt < pos) && *text )
	{
		switch ( *text )
		{
		case 0x09:
			if ( !bTab )
			{
				tcnt = ((cnt + 4) / 4) * 4;
				bTab = true;
			}
			else if ( cnt == tcnt )
			{
				++text;
				if ( *text == 0 )
					return text;
				if ( *text != 0x09 )
				{
					++text;
					bTab = false;
				}
				else
					tcnt += 4;
			}
			break;
		default:
			++text;
			bTab = false;
			break;
		}
		++cnt;
	}
	if ( (tcnt > 0) && (cnt == tcnt) && (*text == 0x09) )
		++text;
	return text;
}

CStringBuffer CConsoleWindow::GetAlignedText(CConstPointer text, sdword fwidth, CConsoleWindow::Alignment align)
{
	CStringBuffer tmp;
	sdword flen = s_strlen(CastMutable(CPointer, text), INT_MAX);

	switch ( align )
	{
	case AlignmentLeft:
		tmp.SetString(__FILE__LINE__ text);
		break;
	case AlignmentCenter:
		break;
	case AlignmentRight:
		if ( flen >= fwidth )
		{
			text += flen - fwidth;
			tmp.SetString(__FILE__LINE__ text);
			break;
		}
		for ( sdword ix = 0; ix < (fwidth - flen); ++ix )
			tmp += _T(" ");
		tmp += text;
		break;
	}
	return tmp;
}


void CConsoleWindow::ScrollWindow(int offset)
{
	CHAR_INFO chiFill;
	COORD coordDest;
	COORD coordSrc;
	COORD coordSize;

    chiFill.Attributes = 0x0017; 
	chiFill.Char.UnicodeChar = 0x0020;

	coordDest.X = 1;
	coordDest.Y = 1;

	coordSrc.X = 1;
	coordSrc.Y = 1 + offset;

	coordSize.X = m_ScreenBufferSize.X - 2;
    coordSize.Y = m_ScreenBufferSize.Y - 2 - offset;

	while ( coordSize.Y > 0 )
	{
		s_memcpy_s(
			m_ScreenBuffer + ((coordDest.Y * m_ScreenBufferSize.X) + coordDest.X),
			coordSize.X * sizeof(CHAR_INFO),
			m_ScreenBuffer + ((coordSrc.Y * m_ScreenBufferSize.X) + coordSrc.X),
			coordSize.X * sizeof(CHAR_INFO));
		--(coordSize.Y);
		++(coordDest.Y);
		++(coordSrc.Y);
	}

	PCHAR_INFO buffer = m_ScreenBuffer + ((coordDest.Y * m_ScreenBufferSize.X) + coordDest.X);

	while ( coordSize.X > 0 )
	{
		*buffer++ = chiFill;
		--(coordSize.X);
	}

	PostPaintEvent();
}

CConsoleWindow::PosIn CConsoleWindow::PosInNonClientRect(COORD pos)
{
	if ( !m_hasBorder )
		return PosInNone;
	if ( pos.Y == m_NonClientArea.Top )
	{
		if ( pos.X == m_NonClientArea.Left )
			return PosInLeftTopCorner;
		if ( pos.X == (m_NonClientArea.Right - 1) )
			return PosInRightTopCorner;
		if ( !m_hasTitle )
			return PosInTopBorder;
		if ( ((m_TitleStyle == windowtitlebarstyle) 
			|| (m_TitleStyle == dialogtitlebarstyle)) 
			&& (pos.X == (m_NonClientArea.Right - 3)) )
			return PosInCloseButton;
		if ( (m_TitleStyle == windowtitlebarstyle) 
			&& (pos.X == (m_NonClientArea.Right - 6)) )
			return PosInMaximizeButton;
		return PosInTitleBar;
	}
	if ( pos.Y == m_ClientArea.Bottom )
	{
		if ( m_hasScrollbar 
			&& ((m_ScrollBarStyle == horizontalscrollbarstyle)
			    || (m_ScrollBarStyle == bothscrollbarstyle)) )
		{
			sdword current = (m_ScrollBarHInfo.Current * m_ScrollBarHInfo.PageStep) / m_ScrollBarHInfo.Maximum;

			if ( pos.X == m_ClientArea.Left )
				return PosInHScrollLineLeft;
			if ( pos.X <= (m_ClientArea.Left + current) )
				return PosInHScrollPageLeft;
			if ( pos.X <= (m_ClientArea.Left + current + m_ScrollBarHInfo.ThumbSize) )
				return PosInHScrollThumb;
			if ( pos.X < (m_ClientArea.Right - 1) )
				return PosInHScrollPageRight;
			if ( pos.X == (m_ClientArea.Right - 1) )
				return PosInHScrollLineRight;
		}
	}
	if ( pos.Y == (m_NonClientArea.Bottom - 1) )
	{
		if ( pos.X == m_NonClientArea.Left )
			return PosInLeftBottomCorner;
		if ( pos.X == (m_NonClientArea.Right - 1) )
			return PosInRightBottomCorner;
		return PosInBottomBorder;
	}
	if ( pos.X == m_NonClientArea.Left )
		return PosInLeftBorder;
	if ( pos.X == m_ClientArea.Right )
	{
		if ( m_hasScrollbar 
			&& ((m_ScrollBarStyle == verticalscrollbarstyle)
			    || (m_ScrollBarStyle == bothscrollbarstyle)) )
		{
			sdword current = (m_ScrollBarVInfo.Current * m_ScrollBarVInfo.PageStep) / m_ScrollBarVInfo.Maximum;

			if ( pos.Y == m_ClientArea.Top )
				return PosInVScrollLineUp;
			if ( pos.Y <= (m_ClientArea.Top + current) )
				return PosInVScrollPageUp;
			if ( pos.Y <= (m_ClientArea.Top + current + m_ScrollBarVInfo.ThumbSize) )
				return PosInVScrollThumb;
			if ( pos.Y < (m_ClientArea.Bottom - 1) )
				return PosInVScrollPageDown;
			if ( pos.Y == (m_ClientArea.Bottom - 1) )
				return PosInVScrollLineDown;
		}
	}
	if ( pos.X == (m_NonClientArea.Right - 1) )
		return PosInRightBorder;
	return PosInNone;
}

bool CConsoleWindow::IsPosInNonClientRect(COORD pos)
{
	if ( IsPosInClientRect(pos) )
		return false;
	if ( (pos.X < m_NonClientArea.Left) || (pos.X >= m_NonClientArea.Right) )
		return false;
	if ( (pos.Y < m_NonClientArea.Top) || (pos.Y >= m_NonClientArea.Bottom) )
		return false;
	return true;
}

bool CConsoleWindow::IsPosInClientRect(COORD pos)
{
	if ( (pos.X < m_ClientArea.Left) || (pos.X >= m_ClientArea.Right) )
		return false;
	if ( (pos.Y < m_ClientArea.Top) || (pos.Y >= m_ClientArea.Bottom) )
		return false;
	return true;
}

void CConsoleWindow::DrawVerticalScrollBar(CConsoleWindow::PosIn posIn)
{
	switch ( posIn )
	{
	case PosInVScrollLineUp:
		m_ScrollBarVInfo.Current -= m_ScrollBarVInfo.LineStep;
		break;
	case PosInVScrollPageUp:
		m_ScrollBarVInfo.Current -= m_ScrollBarVInfo.PageStep;
		break;
	case PosInVScrollPageDown:
		m_ScrollBarVInfo.Current += m_ScrollBarVInfo.PageStep;
		break;
	case PosInVScrollLineDown:
		m_ScrollBarVInfo.Current += m_ScrollBarVInfo.LineStep;
		break;
	default:
		return;
	}
	DrawVerticalScrollBar();
	Scroll();
	PostPaintEvent();
}

void CConsoleWindow::DrawHorizontalScrollBar(CConsoleWindow::PosIn posIn)
{
	switch ( posIn )
	{
	case PosInHScrollLineLeft:
		m_ScrollBarHInfo.Current -= m_ScrollBarHInfo.LineStep;
		break;
	case PosInHScrollPageLeft:
		m_ScrollBarHInfo.Current -= m_ScrollBarHInfo.PageStep;
		break;
	case PosInHScrollPageRight:
		m_ScrollBarHInfo.Current += m_ScrollBarHInfo.PageStep;
		break;
	case PosInHScrollLineRight:
		m_ScrollBarHInfo.Current += m_ScrollBarHInfo.LineStep;
		break;
	default:
		return;
	}
	DrawHorizontalScrollBar();
	Scroll();
	PostPaintEvent();
}

void CConsoleWindow::MoveHScrollThumb(sdword offset)
{
	m_ScrollBarHInfo.Current += (offset * m_ScrollBarHInfo.Maximum) / Cast(sdword, m_ScrollBarHInfo.PageStep);
	DrawHorizontalScrollBar();
	Scroll();
	PostPaintEvent();
}

void CConsoleWindow::MoveVScrollThumb(sdword offset)
{
	m_ScrollBarVInfo.Current += (offset * m_ScrollBarVInfo.Maximum) / Cast(sdword, m_ScrollBarVInfo.PageStep);
	DrawVerticalScrollBar();
	Scroll();
	PostPaintEvent();
}

bool CConsoleWindow::PostPaintEvent(bool repaintall)
{
	if ( PtrCheck(m_Console) )
		return false;
	m_Console->PostPaintEvent(this, repaintall);
	return true;
}

void CConsoleWindow::ShowCursor()
{
	if ( PtrCheck(m_Console) )
		return;
	m_Console->ShowCursor();
}

void CConsoleWindow::SetCursorPos(COORD pos)
{
	if ( PtrCheck(m_Console) )
		return;
	m_Console->SetCursorPos(pos);
}

void CConsoleWindow::HideCursor()
{
	if ( PtrCheck(m_Console) )
		return;
	m_Console->HideCursor();
}

void CConsoleWindow::MaximizeWindow()
{
	if ( PtrCheck(m_Console) )
		return;
	m_RestorePos.X = m_NonClientArea.Left;
	m_RestorePos.Y = m_NonClientArea.Top;
	m_RestoreSize = m_ScreenBufferSize;
	SetFocus(false);
	m_Console->MaximizeWindow(this);
	SetFocus(true);
	m_Maximized = true;
}

void CConsoleWindow::RestoreWindow()
{
	if ( !m_Maximized )
		return;
	SetFocus(false);
	MoveWindow(m_RestorePos, false, true);
	ResizeWindow(m_RestoreSize);
	SetFocus(true);
	m_Maximized = false;
}

void CConsoleWindow::CloseWindow()
{
	if ( PtrCheck(m_Console) )
		return;
	m_Console->CloseWindow(this);
}

void CConsoleWindow::MoveWindow(COORD topleft, bool update, bool absolute)
{
	if ( PtrCheck(m_Console) )
		return;
	if ( absolute )
	{
		m_NonClientArea.Left = topleft.X;
		m_NonClientArea.Right = m_NonClientArea.Left + m_ScreenBufferSize.X;
		m_NonClientArea.Top = topleft.Y;
		m_NonClientArea.Bottom = m_NonClientArea.Top + m_ScreenBufferSize.Y;
	}
	else
	{
		sword deltaX = topleft.X - m_lastMousePosition.X;
		sword deltaY = topleft.Y - m_lastMousePosition.Y;

		m_lastMousePosition = topleft;

		m_NonClientArea.Left += deltaX;
		m_NonClientArea.Right += deltaX;
		m_NonClientArea.Top += deltaY;
		m_NonClientArea.Bottom += deltaY;

		if ( m_Console->HasMainMenu() )
		{
			if ( m_NonClientArea.Top == 0 )
			{
				m_NonClientArea.Top = 1;
				m_NonClientArea.Bottom += 1;
			}
		}
	}
	if ( m_hasBorder || m_hasTitle )
	{
		m_ClientArea.Left = m_NonClientArea.Left + 1;
		m_ClientArea.Right = m_NonClientArea.Right - 1;
		m_ClientArea.Top = m_NonClientArea.Top + 1;
		m_ClientArea.Bottom = m_NonClientArea.Bottom - 1;
		if ( m_hasTitle )
		{
			switch ( m_TitleStyle )
			{
			case windowtitlebarstyle:
				if ( m_hasScrollbar )
				{
					switch ( m_ScrollBarStyle )
					{
					case horizontalscrollbarstyle:
						m_ClientArea.Right -= 1;
						break;
					case verticalscrollbarstyle:
						m_ClientArea.Bottom -= 1;
						break;
					case bothscrollbarstyle:
						m_ClientArea.Right -= 1;
						m_ClientArea.Bottom -= 1;
						break;
					default:
						break;
					}
				}
				break;
			default:
				break;
			}
		}
	}
	else
		m_ClientArea = m_NonClientArea;
	Move();
	if ( update )
		PostPaintEvent(true);
}

void CConsoleWindow::ResizeWindow(COORD newSize, bool update)
{
	if ( (newSize.X == m_ScreenBufferSize.X) && (newSize.Y == m_ScreenBufferSize.Y) )
		return;

	if ( m_ScreenBuffer )
		TFfree(m_ScreenBuffer);

	m_NonClientArea.Right = m_NonClientArea.Left + newSize.X;
	m_NonClientArea.Bottom = m_NonClientArea.Top + newSize.Y;

	if ( m_hasBorder || m_hasTitle )
	{
		m_ClientArea.Left = m_NonClientArea.Left + 1;
		m_ClientArea.Right = m_NonClientArea.Right - 1;
		m_ClientArea.Top = m_NonClientArea.Top + 1;
		m_ClientArea.Bottom = m_NonClientArea.Bottom - 1;
		if ( m_hasTitle )
		{
			switch ( m_TitleStyle )
			{
			case windowtitlebarstyle:
				if ( m_hasScrollbar )
				{
					switch ( m_ScrollBarStyle )
					{
					case horizontalscrollbarstyle:
						m_ClientArea.Right -= 1;
						break;
					case verticalscrollbarstyle:
						m_ClientArea.Bottom -= 1;
						break;
					case bothscrollbarstyle:
						m_ClientArea.Right -= 1;
						m_ClientArea.Bottom -= 1;
						break;
					default:
						break;
					}
				}
				break;
			default:
				break;
			}
		}
	}
	else
		m_ClientArea = m_NonClientArea;

	m_ClientAreaSize.X = m_ClientArea.Right - m_ClientArea.Left;
	m_ClientAreaSize.Y = m_ClientArea.Bottom - m_ClientArea.Top;

	m_ScreenBufferSize = newSize;

	m_ScreenBuffer = CastAnyPtr(CHAR_INFO, TFalloc(
	    m_ScreenBufferSize.X * m_ScreenBufferSize.Y * sizeof(CHAR_INFO)));

	ClearDrawingArea();
	if ( m_hasBorder )
	{
		switch ( m_BorderStyle )
		{
		case singleborderstyle:
			DrawSingleBorder();
			break;
		case doubleborderstyle:
			DrawDoubleBorder();
			break;
		default:
			break;
		}
	}
	if ( m_hasTitle )
	{
		switch ( m_TitleStyle )
		{
		case controltitlebarstyle:
			DrawControlTitleBar();
			break;
		case dialogtitlebarstyle:
			DrawDialogTitleBar();
			break;
		case windowtitlebarstyle:
			DrawWindowTitleBar();
			break;
		default:
			break;
		}
	}
	Resize();
	if ( m_hasScrollbar )
	{
		switch ( m_ScrollBarStyle )
		{
		case horizontalscrollbarstyle:
			DrawHorizontalScrollBar();
			break;
		case verticalscrollbarstyle:
			DrawVerticalScrollBar();
			break;
		case bothscrollbarstyle:
			DrawHorizontalScrollBar();
			DrawVerticalScrollBar();
			break;
		default:
			break;
		}
	}
	if ( update )
		PostPaintEvent(true);
}

#ifdef OK_SYS_WINDOWS
bool CConsoleWindow::KeyEventProc(ConstRef(KEY_EVENT_RECORD) keyEvent)
{
	if ( keyEvent.bKeyDown )
	{
		if ( m_hasTitle )
		{
			if ( PtrCheck(m_Console) )
				return true;
			switch ( m_TitleStyle )
			{
			case controltitlebarstyle:
				break;
			case dialogtitlebarstyle:
			case windowtitlebarstyle:
				if ( (keyEvent.dwControlKeyState & KeyStateAllMask) != 0 )
				{
					if ( m_InsideMoveResize )
					{
						m_InsideMoveResize = 0;
						return true;
					}
					break;
				}
				switch ( keyEvent.wVirtualKeyCode )
				{
				CASE_KEY_DOWN:
					switch ( m_InsideMoveResize )
					{
					case 0:
						break;
					case 1: // Move
						{
							COORD pos;
							COORD max = m_Console->GetScreenBufferSize();

							pos.X = m_NonClientArea.Left;
							pos.Y = m_NonClientArea.Top + 1;
							if ( pos.Y > (max.Y - 1) )
								pos.Y = max.Y - 1;

							MoveWindow(pos, true, true);
							return true;
						}
						break;
					case 2: // Resize
						if ( m_Resizeable ) 
						{
							sword deltaY = 1;
							COORD newSize = m_ScreenBufferSize;
							COORD max = m_Console->GetScreenBufferSize();

							newSize.Y += deltaY;
							if ( newSize.Y > max.Y )
								newSize.Y = max.Y;

							ResizeWindow(newSize);
							return true;
						}
						break;
					}
					break;
				CASE_KEY_UP:
					switch ( m_InsideMoveResize )
					{
					case 0:
						break;
					case 1: // Move
						{
							COORD pos;

							pos.X = m_NonClientArea.Left;
							pos.Y = m_NonClientArea.Top - 1;

							if ( NotPtrCheck(m_Console->GetConsoleMainMenu()) )
							{
								if ( pos.Y < 1 )
									pos.Y = 1;
							}
							else
							{
								if ( pos.Y < 0 )
									pos.Y = 0;
							}
							MoveWindow(pos, true, true);
							return true;
						}
						break;
					case 2: // Resize
						if ( m_Resizeable ) 
						{
							sword deltaY = -1;
							COORD newSize = m_ScreenBufferSize;

							newSize.Y += deltaY;
							if ( newSize.Y < 3 )
								newSize.Y = 3;

							ResizeWindow(newSize);
							return true;
						}
						break;
					}
					break;
				CASE_KEY_RIGHT:
					switch ( m_InsideMoveResize )
					{
					case 0:
						break;
					case 1: // Move
						{
							COORD pos;
							COORD max = m_Console->GetScreenBufferSize();

							pos.X = m_NonClientArea.Left + 1;
							pos.Y = m_NonClientArea.Top;
							if ( pos.X > (max.X - 1) )
								pos.X = max.X - 1;

							MoveWindow(pos, true, true);
							return true;
						}
						break;
					case 2: // Resize
						if ( m_Resizeable ) 
						{
							sword deltaX = 1;
							COORD newSize = m_ScreenBufferSize;
							COORD max = m_Console->GetScreenBufferSize();

							newSize.X += deltaX;
							if ( newSize.X > max.X )
								newSize.X = max.X;

							ResizeWindow(newSize);
							return true;
						}
						break;
					}
					break;
				CASE_KEY_LEFT:
					switch ( m_InsideMoveResize )
					{
					case 0:
						break;
					case 1: // Move
						{
							COORD pos;

							pos.X = m_NonClientArea.Left - 1;
							pos.Y = m_NonClientArea.Top;
							if ( pos.X < 0 )
								pos.X = 0;

							MoveWindow(pos, true, true);
							return true;
						}
						break;
					case 2: // Resize
						if ( m_Resizeable ) 
						{
							sword deltaX = -1;
							COORD newSize = m_ScreenBufferSize;

							newSize.X += deltaX;
							if ( newSize.X < 10 )
								newSize.X = 10;

							ResizeWindow(newSize);
							return true;
						}
						break;
					}
					break;
				CASE_KEY_F5: // Maximize/Restore
					if ( m_InsideMoveResize )
						m_InsideMoveResize = 0;
					if ( m_Resizeable )
					{
						SetFocus(false);
						if ( m_Maximized )
							RestoreWindow();
						else
							MaximizeWindow();
						SetFocus(true);
					}
					return true;
				CASE_KEY_F6: // Move
					m_InsideMoveResize = 1;
					return true;
				CASE_KEY_F7: // Resize
					if ( m_Resizeable )
						m_InsideMoveResize = 2;
					return true;
				default:
					if ( m_InsideMoveResize )
					{
						m_InsideMoveResize = 0;
						return true;
					}
					break;
				}
				break;
			default:
				break;
			}
		}
		if ( !KeyDown(keyEvent.wVirtualKeyCode, keyEvent.dwControlKeyState) )
			return false;

		switch ( keyEvent.wVirtualKeyCode )
		{
		case VK_TAB:
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_BACK:
		case VK_DELETE:
			return true;
		}
		if ( (keyEvent.dwControlKeyState & LEFT_CTRL_PRESSED)
			&& (!(keyEvent.dwControlKeyState & RIGHT_ALT_PRESSED)) )
		{
			//keyState[VK_CONTROL] = 0x80;
			//keyState[VK_LCONTROL] = 0x80;
			return true;
		}
		if ( (keyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) )
		{
			//keyState[VK_RCONTROL] = 0x80;
			return true;
		}
		if ( (keyEvent.dwControlKeyState & LEFT_ALT_PRESSED) )
		{
			//keyState[VK_MENU] = 0x80;
			//keyState[VK_LMENU] = 0x80;
			return true;
		}

		int result;
		BYTE keyState[256] = { 0 };

		//GetKeyboardState(keyState);

		if ( (keyEvent.dwControlKeyState & RIGHT_ALT_PRESSED) )
		{
//			keyState[VK_RMENU] = 0x80;
			keyState[VK_MENU] = 0x80;
			keyState[VK_LMENU] = 0x80;
			keyState[VK_CONTROL] = 0x80;
			keyState[VK_LCONTROL] = 0x80;
		}
		if ( (keyEvent.dwControlKeyState & NUMLOCK_ON) )
		{
			keyState[VK_NUMLOCK] = 0x01;
		}
		if ( (keyEvent.dwControlKeyState & CAPSLOCK_ON) )
		{
			keyState[VK_CAPITAL] = 0x01;
		}
		if ( (keyEvent.dwControlKeyState & SHIFT_PRESSED) )
		{
			keyState[VK_SHIFT] = 0x80;
			keyState[VK_LSHIFT] = 0x80;
			keyState[VK_RSHIFT] = 0x80;
		}
		
#ifdef _UNICODE
		TCHAR buf[16] = { 0 };
                
		result = ToUnicode(
			keyEvent.wVirtualKeyCode,
			keyEvent.wVirtualScanCode,
			keyState,
			buf, 16, 0);
                
		if ( result < 1 )
			return true;
		for ( int ix = 0; ix < result; ++ix )
			if ( !KeyPress(buf[ix], keyEvent.dwControlKeyState) )
				return false;
#else
        WORD ch;

		result = ToAscii(
			keyEvent.wVirtualKeyCode,
			keyEvent.wVirtualScanCode,
			keyState,
			&ch, 0);
                
		if ( result < 1 )
			return true;
        if ( !KeyPress(ch && 0x00FF, keyEvent.dwControlKeyState) )
           	return false;
        if ( (result == 2) && (!KeyPress(ch << 8, keyEvent.dwControlKeyState)) )
           	return false;
#endif
		return true;
	}
	else
		return KeyUp(keyEvent.wVirtualKeyCode, keyEvent.dwControlKeyState);
}
#endif
#ifdef OK_SYS_UNIX
bool CConsoleWindow::KeyEventProc(int keyEvent, int keyModifiers)
{
	if ( m_hasTitle )
	{
		if ( PtrCheck(m_Console) )
			return true;
		switch ( m_TitleStyle )
		{
		case controltitlebarstyle:
			break;
		case dialogtitlebarstyle:
		case windowtitlebarstyle:
			switch ( keyEvent )
			{
            CASE_KEY_DOWN:
				switch ( m_InsideMoveResize )
				{
				case 0:
					break;
				case 1: // Move
					{
						COORD pos;
						COORD max = m_Console->GetScreenBufferSize();

						pos.X = m_NonClientArea.Left;
						pos.Y = m_NonClientArea.Top + 1;
						if ( pos.Y > (max.Y - 1) )
							pos.Y = max.Y - 1;

						MoveWindow(pos, true, true);
						return true;
					}
					break;
				case 2: // Resize
					if ( m_Resizeable ) 
					{
						sword deltaY = 1;
						COORD newSize = m_ScreenBufferSize;
						COORD max = m_Console->GetScreenBufferSize();

						newSize.Y += deltaY;
						if ( newSize.Y > max.Y )
							newSize.Y = max.Y;

						ResizeWindow(newSize);
						return true;
					}
					break;
				}
				break;
            CASE_KEY_UP:
				switch ( m_InsideMoveResize )
				{
				case 0:
					break;
				case 1: // Move
					{
						COORD pos;

						pos.X = m_NonClientArea.Left;
						pos.Y = m_NonClientArea.Top - 1;

						if ( NotPtrCheck(m_Console->GetConsoleMainMenu()) )
						{
							if ( pos.Y < 1 )
								pos.Y = 1;
						}
						else
						{
							if ( pos.Y < 0 )
								pos.Y = 0;
						}
						MoveWindow(pos, true, true);
						return true;
					}
					break;
				case 2: // Resize
					if ( m_Resizeable ) 
					{
						sword deltaY = -1;
						COORD newSize = m_ScreenBufferSize;

						newSize.Y += deltaY;
						if ( newSize.Y < 3 )
							newSize.Y = 3;

						ResizeWindow(newSize);
						return true;
					}
					break;
				}
				break;
            CASE_KEY_RIGHT:
				switch ( m_InsideMoveResize )
				{
				case 0:
					break;
				case 1: // Move
					{
						COORD pos;
						COORD max = m_Console->GetScreenBufferSize();

						pos.X = m_NonClientArea.Left + 1;
						pos.Y = m_NonClientArea.Top;
						if ( pos.X > (max.X - 1) )
							pos.X = max.X - 1;

						MoveWindow(pos, true, true);
						return true;
					}
					break;
				case 2: // Resize
					if ( m_Resizeable ) 
					{
						sword deltaX = 1;
						COORD newSize = m_ScreenBufferSize;
						COORD max = m_Console->GetScreenBufferSize();

						newSize.X += deltaX;
						if ( newSize.X > max.X )
							newSize.X = max.X;

						ResizeWindow(newSize);
						return true;
					}
					break;
				}
				break;
            CASE_KEY_LEFT:
				switch ( m_InsideMoveResize )
				{
				case 0:
					break;
				case 1: // Move
					{
						COORD pos;

						pos.X = m_NonClientArea.Left - 1;
						pos.Y = m_NonClientArea.Top;
						if ( pos.X < 0 )
							pos.X = 0;

						MoveWindow(pos, true, true);
						return true;
					}
					break;
				case 2: // Resize
					if ( m_Resizeable ) 
					{
						sword deltaX = -1;
						COORD newSize = m_ScreenBufferSize;

						newSize.X += deltaX;
						if ( newSize.X < 10 )
							newSize.X = 10;

						ResizeWindow(newSize);
						return true;
					}
					break;
				}
				break;
			CASE_KEY_F5: // Maximize/Restore
				if ( m_InsideMoveResize )
					m_InsideMoveResize = 0;
				if ( m_Resizeable )
				{
					SetFocus(false);
					if ( m_Maximized )
						RestoreWindow();
					else
						MaximizeWindow();
					SetFocus(true);
				}
				return true;
			CASE_KEY_F6: // Move
				m_InsideMoveResize = 1;
				return true;
			CASE_KEY_F7: // Resize
				if ( m_Resizeable )
					m_InsideMoveResize = 2;
				return true;
			default:
				if ( m_InsideMoveResize )
				{
					m_InsideMoveResize = 0;
					return true;
				}
				break;
			}
			break;
		default:
			break;
		}
	}
	if ( !KeyDown(keyEvent, keyModifiers) )
		return false;
	if ( (keyModifiers == 0) && (keyEvent >= 0x20) && (keyEvent <= 0x7f) && (!KeyPress(keyEvent, 0)) )
		return false;
	return KeyUp(keyEvent, keyModifiers);
}
#endif

#ifdef OK_SYS_WINDOWS
bool CConsoleWindow::MouseEventProc(ConstRef(MOUSE_EVENT_RECORD) mouseEvent)
{
	switch ( mouseEvent.dwEventFlags )
	{
	case 0:
		if ( mouseEvent.dwButtonState == 0 )
		{
			if ( m_InsideMoveResize )
			{
				m_InsideMoveResize = 0;
				SetFocus(true);
			}
			if ( m_hasTitle || m_hasBorder )
				m_lastPosIn = PosInNone;
			if ( m_lastButtonState == RIGHTMOST_BUTTON_PRESSED )
			{
				m_lastButtonState = 0;
				return RightMouseUp(mouseEvent.dwMousePosition, mouseEvent.dwControlKeyState);
			}
			if ( m_lastButtonState == FROM_LEFT_1ST_BUTTON_PRESSED )
			{
				m_lastButtonState = 0;
				return LeftMouseUp(mouseEvent.dwMousePosition, mouseEvent.dwControlKeyState);
			}
		}
		if ( mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED )
		{
			m_lastButtonState = RIGHTMOST_BUTTON_PRESSED;
			return RightMouseDown(mouseEvent.dwMousePosition, mouseEvent.dwControlKeyState);
		}
		if ( mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED )
		{
			if ( (mouseEvent.dwControlKeyState & KeyStateAllMask) == 0 )
			{
				if ( m_hasTitle || m_hasBorder )
				{
					PosIn pos = PosInNonClientRect(mouseEvent.dwMousePosition);

					m_InsideMoveResize = 0;
					switch ( pos )
					{
					case PosInMaximizeButton:
						if ( m_Resizeable )
						{
							SetFocus(false);
							if ( m_Maximized )
								RestoreWindow();
							else
								MaximizeWindow();
							SetFocus(true);
						}
						m_lastPosIn = PosInNone;
						return true;
					case PosInCloseButton:
						CloseWindow();
						m_lastPosIn = PosInNone;
						return true;
					case PosInNone:
						m_lastPosIn = PosInNone;
						break;
					case PosInTopBorder:
					case PosInTitleBar:
						m_lastPosIn = pos;
						m_lastMousePosition = mouseEvent.dwMousePosition;
						m_InsideMoveResize = 1;
						SetFocus(false);
						return true;
					case PosInHScrollLineLeft:
					case PosInHScrollPageLeft:
					case PosInHScrollPageRight:
					case PosInHScrollLineRight:
						DrawHorizontalScrollBar(pos);
						m_lastPosIn = PosInNone;
						return true;
					case PosInHScrollThumb:
						m_lastPosIn = pos;
						m_lastMousePosition = mouseEvent.dwMousePosition;
						m_InsideMoveResize = 1;
						SetFocus(false);
						return true;
					case PosInVScrollLineUp:
					case PosInVScrollPageUp:
					case PosInVScrollPageDown:
					case PosInVScrollLineDown:
						DrawVerticalScrollBar(pos);
						m_lastPosIn = PosInNone;
						return true;
					case PosInVScrollThumb:
						m_lastPosIn = pos;
						m_lastMousePosition = mouseEvent.dwMousePosition;
						m_InsideMoveResize = 1;
						SetFocus(false);
						return true;
					default:
						if ( m_Resizeable )
						{
							m_lastPosIn = pos;
							m_lastMousePosition = mouseEvent.dwMousePosition;
							m_InsideMoveResize = 1;
							SetFocus(false);
						}
						else
							m_lastPosIn = PosInNone;
						return true;
					}
				}
			}
			m_lastButtonState = FROM_LEFT_1ST_BUTTON_PRESSED;
			return LeftMouseDown(mouseEvent.dwMousePosition, mouseEvent.dwControlKeyState);
		}
		break;
	case DOUBLE_CLICK:
		if ( mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED )
			return RightMouseDoubleDown(mouseEvent.dwMousePosition, mouseEvent.dwControlKeyState);
		if ( mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED )
		{
			if ( (mouseEvent.dwControlKeyState & KeyStateAllMask) == 0 )
			{
				if ( m_hasTitle || m_hasBorder )
				{
					PosIn pos = PosInNonClientRect(mouseEvent.dwMousePosition);

					switch ( pos )
					{
					case PosInNone:
						break;
					case PosInTopBorder:
					case PosInTitleBar:
						if ( m_Resizeable )
						{
							SetFocus(false);
							if ( m_Maximized )
								RestoreWindow();
							else
								MaximizeWindow();
							SetFocus(true);
						}
					default:
						m_lastPosIn = PosInNone;
						return true;
					}
				}
			}
			return LeftMouseDoubleDown(mouseEvent.dwMousePosition, mouseEvent.dwControlKeyState);
		}
		break;
	case MOUSE_MOVED:
		if ( mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED )
			return RightMouseDownMove(mouseEvent.dwMousePosition, mouseEvent.dwControlKeyState);
		if ( mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED )
		{
			if ( (mouseEvent.dwControlKeyState & KeyStateAllMask) == 0 )
			{
				if ( m_hasTitle || m_hasBorder )
				{
					PosIn pos = m_lastPosIn;

					switch ( pos )
					{
					case PosInNone:
						break;
					case PosInTopBorder:
					case PosInTitleBar:
						MoveWindow(mouseEvent.dwMousePosition);
						return true;
					case PosInLeftTopCorner:
						{
							sword deltaX = mouseEvent.dwMousePosition.X - m_lastMousePosition.X;
							sword deltaY = mouseEvent.dwMousePosition.Y - m_lastMousePosition.Y;
							COORD newSize = m_ScreenBufferSize;

							newSize.X -= deltaX;
							if ( newSize.X < 12 )
								newSize.X = 12;
							newSize.Y -= deltaY;
							if ( newSize.Y < 3 )
								newSize.Y = 3;

							MoveWindow(mouseEvent.dwMousePosition, false);
							ResizeWindow(newSize);
						}
						return true;
					case PosInLeftBorder:
						{
							sword deltaX = mouseEvent.dwMousePosition.X - m_lastMousePosition.X;
							COORD newSize = m_ScreenBufferSize;

							newSize.X -= deltaX;
							if ( newSize.X < 12 )
								newSize.X = 12;
							m_lastMousePosition.Y = mouseEvent.dwMousePosition.Y;

							MoveWindow(mouseEvent.dwMousePosition, false);
							ResizeWindow(newSize);
						}
						return true;
					case PosInLeftBottomCorner:
						{
							sword deltaX = mouseEvent.dwMousePosition.X - m_lastMousePosition.X;
							sword deltaY = mouseEvent.dwMousePosition.Y - m_lastMousePosition.Y;
							COORD newSize = m_ScreenBufferSize;

							newSize.X -= deltaX;
							if ( newSize.X < 12 )
								newSize.X = 12;
							newSize.Y += deltaY;
							if ( newSize.Y < 3 )
								newSize.Y = 3;
							m_lastMousePosition.Y = mouseEvent.dwMousePosition.Y;

							MoveWindow(mouseEvent.dwMousePosition, false);
							ResizeWindow(newSize);
						}
						return true;
					case PosInBottomBorder:
						{
							sword deltaY = mouseEvent.dwMousePosition.Y - m_lastMousePosition.Y;
							COORD newSize = m_ScreenBufferSize;

							newSize.Y += deltaY;
							if ( newSize.Y < 3 )
								newSize.Y = 3;
							m_lastMousePosition = mouseEvent.dwMousePosition;

							ResizeWindow(newSize);
						}
						return true;
					case PosInRightBottomCorner:
						{
							sword deltaX = mouseEvent.dwMousePosition.X - m_lastMousePosition.X;
							sword deltaY = mouseEvent.dwMousePosition.Y - m_lastMousePosition.Y;
							COORD newSize = m_ScreenBufferSize;

							newSize.X += deltaX;
							if ( newSize.X < 12 )
								newSize.X = 12;
							newSize.Y += deltaY;
							if ( newSize.Y < 3 )
								newSize.Y = 3;
							m_lastMousePosition = mouseEvent.dwMousePosition;

							ResizeWindow(newSize);
						}
						return true;
					case PosInRightBorder:
						{
							sword deltaX = mouseEvent.dwMousePosition.X - m_lastMousePosition.X;
							COORD newSize = m_ScreenBufferSize;

							newSize.X += deltaX;
							if ( newSize.X < 12 )
								newSize.X = 12;
							m_lastMousePosition = mouseEvent.dwMousePosition;

							ResizeWindow(newSize);
						}
						return true;
					case PosInRightTopCorner:
						{
							sword deltaX = mouseEvent.dwMousePosition.X - m_lastMousePosition.X;
							sword deltaY = mouseEvent.dwMousePosition.Y - m_lastMousePosition.Y;
							COORD newSize = m_ScreenBufferSize;

							newSize.X += deltaX;
							if ( newSize.X < 12 )
								newSize.X = 12;
							newSize.Y -= deltaY;
							if ( newSize.Y < 3 )
								newSize.Y = 3;
							m_lastMousePosition.X = mouseEvent.dwMousePosition.X;

							MoveWindow(mouseEvent.dwMousePosition, false);
							ResizeWindow(newSize);
						}
						return true;
					case PosInHScrollThumb:
						{
							sword deltaX = mouseEvent.dwMousePosition.X - m_lastMousePosition.X;

							m_lastMousePosition = mouseEvent.dwMousePosition;
							MoveHScrollThumb(deltaX);
						}
						return true;
					case PosInVScrollThumb:
						{
							sword deltaY = mouseEvent.dwMousePosition.Y - m_lastMousePosition.Y;

							m_lastMousePosition = mouseEvent.dwMousePosition;
							MoveVScrollThumb(deltaY);
						}
						return true;
					default:
						return true;
					}
				}
			}
			return LeftMouseDownMove(mouseEvent.dwMousePosition, mouseEvent.dwControlKeyState);
		}
		break;
	case MOUSE_WHEELED:
		if ( _Lowword(mouseEvent.dwButtonState) == 0 )
		{
			sword deltaY = _Highword(mouseEvent.dwButtonState);

			if ( (mouseEvent.dwControlKeyState & KeyStateAllMask) == 0 )
			{
				if ( m_hasScrollbar 
					&& ((m_ScrollBarStyle == verticalscrollbarstyle)
					    || (m_ScrollBarStyle == bothscrollbarstyle)) )
				{
					deltaY = -deltaY / 10;
					m_ScrollBarVInfo.Current += Cast(sdword, deltaY);
					if ( m_ScrollBarVInfo.Current < m_ScrollBarVInfo.Minimum )
						m_ScrollBarVInfo.Current = m_ScrollBarVInfo.Minimum;
					if ( m_ScrollBarVInfo.Current > m_ScrollBarVInfo.Maximum )
						m_ScrollBarVInfo.Current = m_ScrollBarVInfo.Maximum;
					DrawVerticalScrollBar();
					Scroll();
					PostPaintEvent();
					return true;
				}
			}
			return WheelMouse(mouseEvent.dwMousePosition, deltaY, mouseEvent.dwControlKeyState);
		}
		break;
	default:
		break;
	}
	return true;
}
#endif
#ifdef OK_SYS_UNIX
bool CConsoleWindow::MouseEventProc(MEVENT* mouseEvent)
{
	return true;
}
#endif

void CConsoleWindow::Paint(COORD bufferSize, PCHAR_INFO buffer)
{
	SMALL_RECT rect = m_NonClientArea;
	PCHAR_INFO srcPtr = m_ScreenBuffer;
	sword dstmxx = bufferSize.X * bufferSize.Y;
	sword dstmx;
	sword dstsz1;
	sword dstsz2;
	sword ix;
	sword jx;

	for ( ix = 0; ix < m_ScreenBufferSize.Y; ++ix )
	{
		dstsz1 = rect.Left;
		dstsz2 = ((rect.Top + ix) * bufferSize.X);
		if ( (dstsz1 + dstsz2) > dstmxx )
			break;
		dstmx = Min(m_ScreenBufferSize.X, bufferSize.X - rect.Left);
		for ( jx = 0; jx < m_ScreenBufferSize.X; ++jx )
		{
			if ( (dstsz1 >= 0) && (dstmx > 0) && ((dstsz1 + dstsz2) < dstmxx) )
				*(buffer + dstsz1 + dstsz2) = *srcPtr;
			++srcPtr;
			++dstsz1;
			--dstmx;
		}
	}
}

void CConsoleWindow::SetFocus(bool hasFocus)
{
}

void CConsoleWindow::Resize()
{
}

void CConsoleWindow::Move()
{
}

void CConsoleWindow::Scroll()
{
}

void CConsoleWindow::Undo()
{
}

void CConsoleWindow::Redo()
{
}

void CConsoleWindow::Cut()
{
}

void CConsoleWindow::Copy()
{
}

void CConsoleWindow::Paste()
{
}

void CConsoleWindow::Delete()
{
}

bool CConsoleWindow::KeyDown(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::KeyPress(TCHAR key, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::KeyUp(WORD virtualKeyCode, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::LeftMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::LeftMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::RightMouseDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::RightMouseUp(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::LeftMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::RightMouseDownMove(COORD mousePos, DWORD controlKeyState)
{
	return true;
}

bool CConsoleWindow::WheelMouse(COORD mousePos, sword rotateCnt, DWORD controlKeyState)
{
	return true;
}

