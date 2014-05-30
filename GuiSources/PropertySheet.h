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

class CPropertySheet;
class CTextEditor;
class CPropertySheetAbstractNode: public CCppObject
{
public:
	enum TDiscriminator
	{
		TDiscriminatorNode,
		TDiscriminatorTitle
	};

	enum THitTest
	{
		THitTestKey,
		THitTestValue
	};

public:
	CPropertySheetAbstractNode(CPropertySheet* pPropertySheet = NULL, TDiscriminator discriminator = TDiscriminatorNode);
	virtual ~CPropertySheetAbstractNode(void);

	__inline TDiscriminator get_Discriminator() { return m_discriminator; }
	__inline void set_Discriminator(TDiscriminator discriminator) { m_discriminator = discriminator; }

	__inline void get_Rect(LPRECT rect) { ::CopyRect(rect, &m_borderRect); }

	virtual CPropertySheetAbstractNode* HitTest(POINT pt, THitTest* pType, INT xPos, INT yPos, INT cBottom);

	virtual void OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxKeyWidth, LPINT maxValueWidth);
	virtual void OnAdjustRects(INT maxKeyWidth, INT maxValueWidth);
	virtual void OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom);

protected:
	CPropertySheet* m_propertySheet;
	TDiscriminator m_discriminator;
	RECT m_borderRect;
};

typedef CDataVectorT<CPropertySheetAbstractNode> CPropertySheetNodeVector;
typedef CDataVectorT<CPropertySheetAbstractNode, CCppObjectLessFunctor<CPropertySheetAbstractNode>, CCppObjectNullFunctor<CPropertySheetAbstractNode>> CPropertySheetSelectedNodeVector;

class CPropertySheetNode: public CPropertySheetAbstractNode
{
public:
	CPropertySheetNode(CPropertySheet* pPropertySheet = NULL, LPCTSTR keyStr = NULL, int keyLenCh = -1, LPCTSTR valueStr = NULL, int valueLenCh = -1);
	CPropertySheetNode(CPropertySheet* pPropertySheet, ConstRef(CStringBuffer) key, ConstRef(CStringBuffer) value);
	virtual ~CPropertySheetNode(void);

	__inline ConstRef(CStringBuffer) get_KeyString() { return m_keyStr; }
	void set_KeyString(LPCTSTR pText = NULL, int lench = -1);
	void set_KeyString(ConstRef(CStringBuffer) text);

	__inline ConstRef(CStringBuffer) get_ValueString() { return m_valueStr; }
	void set_ValueString(LPCTSTR pText = NULL, int lench = -1);
	void set_ValueString(ConstRef(CStringBuffer) text);

	__inline bool is_Selected() { return m_selected; }
	void set_Selected(bool selected);

	__inline bool is_Focused() { return m_focused; }
	__inline void set_Focused(bool focused) { m_focused = focused; }

	__inline bool is_Editable() { return m_editable; }
	__inline void set_Editable(bool editable) { m_editable = editable; }

	__inline bool is_Modified() { return m_modified; }
	__inline void set_Modified(bool bModified) { m_modified = bModified; }

	__inline void get_ValueRect(LPRECT rect) { ::CopyRect(rect, &m_valueRect); }
	void get_EditorRect(LPRECT rect);

	virtual CPropertySheetAbstractNode* HitTest(POINT pt, THitTest* pType, INT xPos, INT yPos, INT cBottom);

	virtual void OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxKeyWidth, LPINT maxValueWidth);
	virtual void OnAdjustRects(INT maxKeyWidth, INT maxValueWidth);
	virtual void OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom);

protected:
	CStringBuffer m_keyStr;
	CStringBuffer m_valueStr;
	bool m_editable;
	bool m_modified;
	bool m_selected;
	bool m_focused;
	RECT m_keyRect;
	RECT m_valueRect;
};

class CPropertySheetTitleNode: public CPropertySheetAbstractNode
{
public:
	CPropertySheetTitleNode(CPropertySheet* pPropertySheet = NULL, LPCTSTR pText = NULL, int lench = -1);
	CPropertySheetTitleNode(CPropertySheet* pPropertySheet, ConstRef(CStringBuffer) _text);
	virtual ~CPropertySheetTitleNode(void);

	__inline ConstRef(CStringBuffer) get_Text() { return m_text; }
	void set_Text(LPCTSTR pText, int lench = -1);
	void set_Text(ConstRef(CStringBuffer) _text);

	virtual void OnCalcRects(Gdiplus::Graphics* graphics, LPRECT pRect, LPINT maxKeyWidth, LPINT maxValueWidth);
	virtual void OnAdjustRects(INT maxKeyWidth, INT maxValueWidth);
	virtual void OnPaint(Gdiplus::Graphics* graphics, INT xPos, INT yPos, INT cBottom);

protected:
	CStringBuffer m_text;
	RECT m_textRect;
};

class CPropertySheet: public CControl
{
public:
	CPropertySheet(LPCTSTR name = NULL);
	CPropertySheet(ConstRef(CStringBuffer) name);
	virtual ~CPropertySheet(void);

	virtual BOOL PreRegisterClass(WNDCLASSEX& cls);
	virtual void get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys);
	virtual void Refresh();
	virtual CWin* Clone(LONG style = 0, DWORD exstyle = 0);
	
	__inline dword get_NodeCount() { return m_nodes.Count(); }
	__inline CPropertySheetAbstractNode* get_Node(dword ix) { if ( (ix < 0) || (ix >= get_NodeCount()) ) return NULL; return *(m_nodes.Index(ix)); }
	CPropertySheetAbstractNode* get_Node(LPCTSTR key);
	void set_Node(dword ix, CPropertySheetAbstractNode* node);
	int inx_Node(CPropertySheetAbstractNode* node);

	CStringBuffer get_NodeValueString(LPCTSTR key);
	void set_NodeValueString(LPCTSTR key, LPCTSTR data = NULL, int lenCh = -1);
	void set_NodeValueString(LPCTSTR key, ConstRef(CStringBuffer) data);

	__inline dword get_SelNodeCount() { return m_selNodes.Count(); }
	__inline CPropertySheetAbstractNode* get_SelNode(dword ix) { if ( (ix < 0) || (ix >= get_SelNodeCount()) ) return NULL; return *(m_selNodes.Index(ix)); }
	void set_SelNode(dword ix, CPropertySheetAbstractNode* node);
	int inx_SelNode(CPropertySheetAbstractNode* node);

	__inline LONG get_MinimumValueWidth() const { return m_minValueWidth; }
	void set_MinimumValueWidth(LONG v);

	__inline bool is_MultiSelect() { return m_multiSelect; }
	__inline void set_MultiSelect(bool multiSelect) { m_multiSelect = multiSelect; }

	__inline bool is_Editable() { return m_editable; }
	__inline void set_Editable(bool editable) { m_editable = editable; }
	bool set_Editable(LPCTSTR key, bool editable);

	__inline CControl* get_Editor() { return m_editor; }
	__inline void set_Editor(CControl* editor) { m_editor = editor; }

	__inline CPropertySheetNode* get_CurrentNode() { return m_currentNode; }
	void set_CurrentNode(CPropertySheetNode* node);

	void SelectAll(bool selected);

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnChar(WPARAM wParam, LPARAM lParam);
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

private:
	void _ItemDown();
	void _ItemUp();
	void _ShowEditor();
	void _CloseEditor(BOOL bSave = TRUE);
	void _EnsureVisible();

	CPropertySheetNodeVector m_nodes;
	CPropertySheetSelectedNodeVector m_selNodes;
	CPropertySheetNode* m_currentNode;
	LONG m_minValueWidth;
	bool m_multiSelect;
	bool m_editable;
	CControl* m_editor;
};

