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

#include "afxwin.h"
#include "afxrsrc.h"
#include "DataVector.h"

class CListView;
class CListViewNode: public CCppObject
{
public:
    CListViewNode(CListView* pParent = NULL, LPCTSTR pText = NULL, int lench = -1, int ix = -1, bool before = true);
	CListViewNode(CListView* pParent, const CStringBuffer& text, int ix = -1, bool before = true);
	virtual ~CListViewNode();

	__inline const CStringBuffer& get_Text() { return m_text; }
	void set_Text(LPCTSTR pText, int lench = -1);
	void set_Text(const CStringBuffer& text);

	__inline bool is_Selected() { return m_selected; }
	void set_Selected(bool selected);

	__inline bool is_Focused() { return m_focused; }
	void set_Focused(bool focused);

	__inline bool is_Editable() { return m_editable; }
	__inline void set_Editable(bool editable) { m_editable = editable; }

	__inline int get_ImageIndex() { return m_imageIndex; }
	__inline void set_ImageIndex(int imageIndex) { m_imageIndex = imageIndex; }

	__inline void get_Rect(LPRECT rect) { ::CopyRect(rect, &m_borderRect); }
	void get_EditorRect(LPRECT rect);

#define HT_ICONRECT 1
#define HT_TEXTRECT 2
	bool HitTest(POINT pt, LPUINT pType, INT xPos, INT yPos, INT cBottom);

	void OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth);
	void OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom, INT cRight);

protected:
	void _init(int ix, bool before);
	void _calcTextDisplayLength();

	CListView* m_listView;
	CStringBuffer m_text;
	int m_textDisplayLength;
	bool m_selected;
	bool m_focused;
	bool m_editable;
	RECT m_iconRect;
	RECT m_textRect;
	RECT m_borderRect;
	int m_imageIndex;
};

typedef CDataVectorT<CListViewNode> CListViewNodeVector;
typedef CDataVectorT<CListViewNode, CCppObjectLessFunctor<CListViewNode>, CCppObjectNullFunctor<CListViewNode>> CListViewSelectedNodeVector;

class CTextEditor;
class CListView: public CControl
{
public:
	enum TSelectMode
	{
		TSelectModeDisabled,
		TSelectModeSimple,
		TSelectModeMulti
	};

public:
    CListView(LPCTSTR name = NULL);
    CListView(ConstRef(CStringBuffer) name);
	virtual ~CListView();

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);

	__inline dword get_NodeCount() { return m_nodes.Count(); }
	__inline CListViewNode* get_Node(dword ix) { return *(m_nodes.Index(ix)); }
	CListViewNode* get_Node(LPCTSTR key);
	void set_Node(dword ix, CListViewNode* node);
	void ins_Node(dword ix, CListViewNode* node, bool before = true);
	int inx_Node(CListViewNode* node);
	void clear(void);

	__inline dword get_SelNodeCount() { return m_selNodes.Count(); }
	__inline CListViewNode* get_SelNode(dword ix) { return *(m_selNodes.Index(ix)); }
	void set_SelNode(dword ix, CListViewNode* node);
	int inx_SelNode(CListViewNode* node);

	__inline const CImageList& get_ImageList() const { return m_images; }
	__inline void set_ImageList(const CImageList& images) { m_images = images; }

	__inline dword get_ImageCount() { return m_images.get_bitmapcnt(); }
	__inline Gdiplus::Bitmap* get_Image(dword ix) { return m_images.get_bitmap(ix); }

	__inline TSelectMode get_SelectMode() { return m_SelectMode; }
	void set_SelectMode(TSelectMode _SelectMode);

	__inline bool is_Editable() { return m_editable; }
	void set_Editable(bool editable);

	__inline CControl* get_Editor() { return m_editor; }
	__inline void set_Editor(CControl* editor) { m_editor = editor; }

	__inline CListViewNode* get_CurrentNode() { return m_currentNode; }
	void set_CurrentNode(CListViewNode* node);

	void SelectAll(bool selected);

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnAboutHScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnVScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnAboutVScroll(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetDlgCode(WPARAM wParam, LPARAM lParam);

	LRESULT OnEditorReturn(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorTab(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorFuncKey(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorLostFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorEscape(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorCursorUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditorCursorDown(WPARAM wParam, LPARAM lParam);

	LRESULT OnEditUndoCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditRedoCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditCutCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditCopyCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditPasteCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditDeleteCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void _CurrentNodeDown();
	void _CurrentNodeUp();
	void _KeyDown();
	void _KeyUp();
	void _EnsureVisible();
	void _ShowEditor();
	void _CloseEditor(BOOL bSave = TRUE);
	bool _TestAllSelected();

	TSelectMode m_SelectMode;
	bool m_editable;
	bool m_hasSelection;
	CListViewNodeVector m_nodes;
	CListViewSelectedNodeVector m_selNodes;
	CImageList m_images;
	CListViewNode* m_currentNode;
	CControl* m_editor;
};