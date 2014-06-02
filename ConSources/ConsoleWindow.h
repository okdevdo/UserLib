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

class CConsole;
class CONSOURCES_API CConsoleWindow: public CCppObject
{
public:
	struct CONSOURCES_API ScrollBarInfo
	{
		sdword Minimum;
		sdword Maximum;
		sdword Current;
		sword ThumbSize;
		sword LineStep;
		sword PageStep;

		ScrollBarInfo():
		    Minimum(0),
			Maximum(100),
			Current(0),
			ThumbSize(1),
			LineStep(3),
			PageStep(12) {}

		void SetScrollBarInfo(sdword max, sword page)
		{
			Minimum = 0;
			if ( page < 20 )
			{
				if ( max < page )
					Maximum = page;
				else if ( max < (page << 1) )
					Maximum = (page << 1);
				else if ( max < 600 )
					Maximum = max + ((max / 30) * page);
				else
					Maximum = max + ((max / 60) * page);
			}
			else if ( page < 40 )
			{
				if ( max < page )
					Maximum = page;
				else if ( max < (page << 1) )
					Maximum = (page << 1);
				else if ( max < 600 )
					Maximum = max + ((max / 60) * page);
				else
					Maximum = max + ((max / 120) * page);
			}
			else if ( page < 60 )
			{
				if ( max < page )
					Maximum = page;
				else if ( max < (page << 1) )
					Maximum = (page << 1);
				else if ( max < 600 )
					Maximum = max + ((max / 90) * page);
				else
					Maximum = max + ((max / 150) * page);
			}
			else
			{
				if ( max < page )
					Maximum = page;
				else if ( max < (page << 1) )
					Maximum = (page << 1);
				else if ( max < 600 )
					Maximum = max + ((max / 150) * page);
				else
					Maximum = max + ((max / 240) * page);
			}
			Current = 0;
			ThumbSize = Cast(sword, (Cast(sdword,page) * Cast(sdword, page)) / Maximum);
			if ( ThumbSize < 1 )
				ThumbSize = 1;
			LineStep = page / 5;
			PageStep = page;
		}
	};

public:
	CConsoleWindow(CConstPointer name, CConsole* pConsole);
	CConsoleWindow(CConstPointer name, CConstPointer title, CConsole* pConsole);
	virtual ~CConsoleWindow(void);

	__inline ConstRef(CStringBuffer) GetName() const { return m_Name; }
	__inline ConstRef(CStringBuffer) get_Name() const { return m_Name; }

	enum BorderGraphics
	{
		bordersingleleftright,
		bordersingleupdown,
		bordersinglerightdown,
		bordersingleleftdown,
		bordersinglerightup,
		bordersingleleftup,
		bordersinglerightupdown,
		bordersingleleftupdown,
		bordersingleleftrightdown,
		bordersingleleftrightup,
		bordersingleleftrightupdown,

		borderdoubleleftright,
		borderdoubleupdown,
		borderdoublerightdown,
		borderdoubleleftdown,
		borderdoublerightup,
		borderdoubleleftup,
		borderdoublerightupdown,
		borderdoubleleftupdown,
		borderdoubleleftrightdown,
		borderdoubleleftrightup,
		borderdoubleleftrightupdown,

		thumbhalfup,
		thumbhalfdown,
		thumbfull,
		thumbhalfmiddle,

		arrowtop,
		arrowright,
		arrowbottom,
		arrowleft,
		maxbordergraphics
	};

	enum BorderStyle
	{
		noborderstyle,
		singleborderstyle,
		doubleborderstyle
	};

	__inline bool HasBorder() { return m_hasBorder; }
	__inline BorderStyle GetBorderStyle() const { return m_BorderStyle; }
	TCHAR GetBorderChar(BorderGraphics ix);

	enum ScrollBarStyle
	{
		noscrollbarstyle,
		horizontalscrollbarstyle,
		verticalscrollbarstyle,
		bothscrollbarstyle
	};

	__inline bool HasScrollbar() { return m_hasScrollbar; }
	__inline ScrollBarStyle GetScrollbarStyle() const { return m_ScrollBarStyle; }

	void MoveHScrollThumb(sdword offset);
	void MoveVScrollThumb(sdword offset);

	enum TitleBarStyle
	{
		notitlebarstyle,
		controltitlebarstyle,
		dialogtitlebarstyle,
		windowtitlebarstyle
	};

	__inline bool HasTitle() { return m_hasTitle; }
	__inline TitleBarStyle GetTitleBarStyle() const { return m_TitleStyle; }
	__inline ConstRef(CStringBuffer) GetTitle() const { return m_Title; }
	__inline void SetTitle(ConstRef(CStringBuffer) title) { m_Title = title; }

	__inline color_t GetColor() const { return m_Color; }
	__inline color_t GetHighLightColor() const { return m_HighLightColor; }

	void ClearDrawingArea();
	void DrawSingleBorder();
	void DrawDoubleBorder();
	void DrawControlTitleBar();
	void DrawDialogTitleBar();
	void DrawWindowTitleBar();
	void DrawVerticalScrollBar();
	void DrawHorizontalScrollBar();
	void DrawSingleChar(COORD pos, TCHAR ch, color_t color);
	void DrawString(ConstRef(SMALL_RECT) rect, CConstPointer ch, color_t color, bool bFill = true);
	void ScrollWindow(int offset);

	sdword GetTextLength(CPointer text);
	CConstPointer GetTextPointer(CConstPointer text, sdword pos);

	enum Alignment
	{
		AlignmentLeft,
		AlignmentCenter,
		AlignmentRight
	};

	CStringBuffer GetAlignedText(CConstPointer text, sdword fwidth, Alignment align = AlignmentRight);

	enum PosIn 
	{
		PosInNone,
		PosInTitleBar,
		PosInMaximizeButton,
		PosInCloseButton,
		PosInTopBorder,
		PosInLeftTopCorner,
		PosInLeftBorder,
		PosInLeftBottomCorner,
		PosInBottomBorder,
		PosInRightBottomCorner,
		PosInRightBorder,
		PosInRightTopCorner,
		PosInHScrollLineLeft,
		PosInHScrollPageLeft,
		PosInHScrollThumb,
		PosInHScrollPageRight,
		PosInHScrollLineRight,
		PosInVScrollLineUp,
		PosInVScrollPageUp,
		PosInVScrollThumb,
		PosInVScrollPageDown,
		PosInVScrollLineDown
	};

	PosIn PosInNonClientRect(COORD pos);
	bool IsPosInNonClientRect(COORD pos);
	bool IsPosInClientRect(COORD pos);
	void DrawVerticalScrollBar(PosIn posIn);
	void DrawHorizontalScrollBar(PosIn posIn);

	__inline ConstRef(SMALL_RECT) GetNonClientArea() const { return m_NonClientArea; }
	__inline ConstRef(SMALL_RECT) GetClientArea() const { return m_ClientArea; }

	bool PostPaintEvent(bool repaintall = false);
	void ShowCursor();
	void SetCursorPos(COORD pos);
	void HideCursor();
	void MaximizeWindow();
	void RestoreWindow();
	void CloseWindow();
	void MoveWindow(COORD topleft, bool update = true, bool absolute = false);
	void ResizeWindow(COORD newSize, bool update = true);

#ifdef OK_SYS_WINDOWS
	enum KeyStateMasks
	{
		KeyStateShiftMask = CAPSLOCK_ON | SHIFT_PRESSED,
		KeyStateCtrlMask = LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED,
		KeyStateAltMask = LEFT_ALT_PRESSED,
		KeyStateAltGrMask = RIGHT_ALT_PRESSED,
		KeyStateAllMask = KeyStateShiftMask | KeyStateCtrlMask | KeyStateAltMask | KeyStateAltGrMask,
		KeyStateAllExceptShiftMask = KeyStateCtrlMask | KeyStateAltMask | KeyStateAltGrMask,
		KeyStateAllExceptCtrlMask = KeyStateShiftMask | KeyStateAltMask | KeyStateAltGrMask
	};

	bool KeyEventProc(ConstRef(KEY_EVENT_RECORD) keyEvent);
	bool MouseEventProc(ConstRef(MOUSE_EVENT_RECORD) mouseEvent);
#endif
#ifdef OK_SYS_UNIX
	enum KeyStateMasks
	{
		KeyStateShiftMask = 0x01,
		KeyStateCtrlMask = 0x02,
		KeyStateAltMask = 0x04,
		KeyStateAltGrMask = 0x08,
		KeyStateAllMask = KeyStateShiftMask | KeyStateCtrlMask | KeyStateAltMask | KeyStateAltGrMask,
		KeyStateAllExceptShiftMask = KeyStateCtrlMask | KeyStateAltMask | KeyStateAltGrMask,
		KeyStateAllExceptCtrlMask = KeyStateShiftMask | KeyStateAltMask | KeyStateAltGrMask
	};

	bool KeyEventProc(int keyEvent, int keyModifiers);
	bool MouseEventProc(MEVENT* mouseEvent);
#endif

	__inline bool IsCreated() { return m_Created; }

	virtual void Create(COORD pos, COORD size);
	virtual void Paint(COORD bufferSize, PCHAR_INFO buffer);
	virtual void SetFocus(bool hasFocus);
	virtual void Resize();
	virtual void Move();
	virtual void Scroll();

	virtual void Undo();
	virtual void Redo();
	virtual void Cut();
	virtual void Copy();
	virtual void Paste();
	virtual void Delete();

	virtual bool KeyDown(WORD virtualKeyCode, DWORD controlKeyState);
	virtual bool KeyPress(TCHAR key, DWORD controlKeyState);
	virtual bool KeyUp(WORD virtualKeyCode, DWORD controlKeyState);

	virtual bool LeftMouseDown(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseUp(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDown(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseUp(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseDoubleDown(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDoubleDown(COORD mousePos, DWORD controlKeyState);
	virtual bool LeftMouseDownMove(COORD mousePos, DWORD controlKeyState);
	virtual bool RightMouseDownMove(COORD mousePos, DWORD controlKeyState);
	virtual bool WheelMouse(COORD mousePos, sword rotateCnt, DWORD controlKeyState);

protected:
	CConsole* m_Console;
	CStringBuffer m_Name;
	SMALL_RECT m_NonClientArea;
	SMALL_RECT m_ClientArea;
	COORD m_ScreenBufferSize;
	COORD m_ClientAreaSize;
	PCHAR_INFO m_ScreenBuffer;
	color_t m_Color;
	color_t m_HighLightColor;
	CStringBuffer m_Title;
	bool m_hasBorder;
	BorderStyle m_BorderStyle;
	bool m_hasTitle;
	TitleBarStyle m_TitleStyle;
	bool m_hasScrollbar;
	ScrollBarStyle m_ScrollBarStyle;
	ScrollBarInfo m_ScrollBarVInfo;
	ScrollBarInfo m_ScrollBarHInfo;
	PosIn m_lastPosIn;
	COORD m_lastMousePosition;
	DWORD m_lastButtonState;
	bool m_Maximized;
	COORD m_RestorePos;
	COORD m_RestoreSize;
	word m_InsideMoveResize;
	bool m_Resizeable;
	bool m_Created;

private:
	CConsoleWindow();
	CConsoleWindow(ConstRef(CConsoleWindow));
};

