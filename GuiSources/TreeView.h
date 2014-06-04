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

class CTreeView;
class CTreeViewNode: public CCppObject
{
public:
	CTreeViewNode(LPCTSTR text = NULL); // for searching purposes
	CTreeViewNode(CTreeView* pTreeView, LPCTSTR text = NULL, int lench = -1, int ix = -1, bool before = true);
	CTreeViewNode(CTreeView* pTreeView, ConstRef(CStringBuffer) text, int ix = -1, bool before = true);
	CTreeViewNode(CTreeViewNode* pParent, LPCTSTR text = NULL, int lench = -1, int ix = -1, bool before = true);
	CTreeViewNode(CTreeViewNode* pParent, ConstRef(CStringBuffer) text, int ix = -1, bool before = true);
	virtual ~CTreeViewNode(void);

	__inline CTreeView* get_TreeView() { return m_treeView; }
	__inline void set_TreeView(CTreeView* pTreeView) { m_treeView = pTreeView; }

	__inline CTreeViewNode* get_Parent() { return m_parent; }
	__inline void set_Parent(CTreeViewNode* pParent) { m_parent = pParent; }

	__inline ConstRef(CStringBuffer) get_Text() const { return m_text; }
	void set_Text(LPCTSTR pText = NULL, int lench = -1);
	void set_Text(ConstRef(CStringBuffer) text);

	__inline ConstRef(CStringBuffer) get_Pattern() const { return m_pattern; }
	void set_Pattern(LPCTSTR pText = NULL, int lench = -1);
	void set_Pattern(ConstRef(CStringBuffer) text);

	__inline ConstRef(CStringBuffer) get_StringTag() const { return m_stringTag; }
	__inline void set_StringTag(LPCTSTR tag, int lench = -1) { m_stringTag.SetString(__FILE__LINE__ tag, lench); }
	__inline void set_StringTag(ConstRef(CStringBuffer) text) { m_stringTag = text; }

	__inline LPVOID get_VoidTag() { return m_voidTag; }
	__inline void set_VoidTag(LPVOID tag) { m_voidTag = tag; }

	__inline dword get_NodeCount() { return m_nodes.Count(); }
	__inline CTreeViewNode* get_Node(dword ix) { return (ix >= m_nodes.Count())?NULL:(*(m_nodes.Index(ix))); }
	CTreeViewNode* get_Node(LPCTSTR nodeString);
	void set_Node(dword ix, CTreeViewNode* node);
	void ins_Node(dword ix, CTreeViewNode* node, bool before = true);
	int inx_Node(CTreeViewNode* node);
	CTreeViewNode* get_NodeByStringTag(LPCTSTR tag);
	CTreeViewNode* get_NodeByVoidTag(LPVOID tag);
	CTreeViewNode* get_NextSibling();
	void find_Node(bool bSortDirs, LPCTSTR nodeString, bool bDir, int* pIndex, bool* pBefore);

	__inline bool is_Expanded() { return m_expanded; }
	void set_Expanded(bool expanded);

	__inline bool is_Focused() { return m_focused; }
	void set_Focused(bool focused);
	CTreeViewNode* get_CurrentNode();

	__inline bool is_Selected() { return m_selected; }
	void set_Selected(bool selected);

	__inline bool is_Editable() { return m_editable; }
	__inline void set_Editable(bool editable) { m_editable = editable; }

	__inline bool has_NodesLoaded() { return m_nodesLoaded; }
	__inline void set_NodesLoaded(bool nodesLoaded) { m_nodesLoaded = nodesLoaded; }

	__inline dword get_PlusImageIndex() { return m_plusImageIndex; }
	__inline void set_PlusImageIndex(dword imageIndex) { m_plusImageIndex = imageIndex; }

	__inline dword get_MinusImageIndex() { return m_minusImageIndex; }
	__inline void set_MinusImageIndex(dword imageIndex) { m_minusImageIndex = imageIndex; }

	__inline void get_Rect(LPRECT rect) { ::CopyRect(rect, &m_borderRect); }
	void get_EditorRect(LPRECT rect);

	void SelectAll(bool selected);
	void ExpandAll(bool expand);
	
#define HT_ICONRECT 1
#define HT_TEXTRECT 2
	CTreeViewNode* HitTest(POINT pt, LPUINT pType, INT xPos, INT yPos, INT cBottom);

	void OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxWidth);
	BOOL OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom);

	class CTreeViewNodeLessFunctor
	{
	public:
		bool operator()(ConstPtr(CTreeViewNode) r1, ConstPtr(CTreeViewNode) r2) const
		{
			return r1->get_Text().LT(r2->get_Text(), 0, CStringLiteral::cIgnoreCase);
		}
	};

	typedef CDataVectorT<CTreeViewNode, CTreeViewNodeLessFunctor> CTreeViewNodeVector;
	typedef CDataVectorT<CTreeViewNode, CTreeViewNodeLessFunctor, CCppObjectNullFunctor<CTreeViewNode>> CTreeViewSelectedNodeVector;

protected:
	CTreeView* m_treeView;
	CTreeViewNode* m_parent;
	CStringBuffer m_text;
	CStringBuffer m_displayText;
	CStringBuffer m_pattern;
	CStringBuffer m_stringTag;
	LPVOID m_voidTag;
	bool m_expanded;
	bool m_selected;
	bool m_focused;
	bool m_editable;
	bool m_nodesLoaded;
	RECT m_iconRect;
	RECT m_textRect;
	RECT m_borderRect;
	dword m_plusImageIndex;
	dword m_minusImageIndex;
	CTreeViewNodeVector m_nodes;

	void _init(int ix = -1, bool before = true);
};

class CTextEditor;
class CTreeView: public CControl
{
public:
	CTreeView(LPCTSTR name = NULL);
	CTreeView(ConstRef(CStringBuffer) name);
	virtual ~CTreeView(void);

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);

	__inline dword get_NodeCount() { return m_nodes.Count(); }
	__inline CTreeViewNode* get_Node(dword ix) { return (ix >= m_nodes.Count())?NULL:(*(m_nodes.Index(ix))); }
	CTreeViewNode* get_Node(LPCTSTR nodeString);
	void set_Node(dword ix, CTreeViewNode* node);
	void ins_Node(dword ix, CTreeViewNode* node, bool before = true);
	int inx_Node(CTreeViewNode* node);
	CTreeViewNode* get_NodeByStringTag(LPCTSTR tag);
	CTreeViewNode* get_NodeByVoidTag(LPVOID tag);
	void find_Node(bool bSortDirs, LPCTSTR nodeString, bool bDir, int* pIndex, bool* pBefore);

	__inline dword get_SelNodeCount() { return m_selNodes.Count(); }
	__inline CTreeViewNode* get_SelNode(dword ix) { return (ix >= m_selNodes.Count())?NULL:(*(m_selNodes.Index(ix))); }
	void set_SelNode(dword ix, CTreeViewNode* node);
	int inx_SelNode(CTreeViewNode* node);

	__inline const CImageList& get_ImageList() const { return m_images; }
	__inline void set_ImageList(const CImageList& images) { m_images = images; }

	__inline dword get_ImageCount() { return m_images.get_bitmapcnt(); }
	__inline Gdiplus::Image* get_Image(dword ix) { return m_images.get_bitmap(ix); }

	__inline bool is_MultiSelect() { return m_multiSelect; }
	__inline void set_MultiSelect(bool multiSelect) { m_multiSelect = multiSelect; }

	__inline bool is_Editable() { return m_editable; }
	__inline void set_Editable(bool editable) { m_editable = editable; }

	__inline CControl* get_Editor() { return m_editor; }
	__inline void set_Editor(CControl* editor) { m_editor = editor; }

	__inline bool is_VirtualLoad() { return m_virtualLoad; }
	__inline void set_VirtualLoad(bool virtualLoad) { m_virtualLoad = virtualLoad; }

	__inline CTreeViewNode* get_CurrentNode() { return m_currentNode; }
	void set_CurrentNode(CTreeViewNode* node);
	void move_CurrentNode(CTreeViewNode* otherParent, int pos = -1);

	void SelectAll(bool selected);
	void ExpandAll(bool expand);

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

private:
	bool m_multiSelect;
	bool m_editable;
	bool m_virtualLoad;

	CTreeViewNode::CTreeViewNodeVector m_nodes;
	CTreeViewNode::CTreeViewSelectedNodeVector m_selNodes;
	CImageList m_images;

	CTreeViewNode* m_currentNode;

	void _CurrentNodeDown();
	void _CurrentNodeUp();
	void _EnsureVisible();
	void _ShowEditor();
	void _CloseEditor(BOOL bSave = TRUE);

	CControl* m_editor;
};

