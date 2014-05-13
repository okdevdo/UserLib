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
// internally used dialog controls id
#define IDC_TABPAGES                        600

// internally used bitmaps
#define IDB_EMPTYBITMAP                     500
#define IDB_SDBOTTOM                        501
#define IDB_SDCENTER                        502
#define IDB_SDLEFT                          503
#define IDB_SDMIDDLE                        504
#define IDB_SDRIGHT                         505
#define IDB_SDTOP                           506
#define IDB_COMBOARROW                      507
#define IDB_ARROWBOTTOM                     508
#define IDB_ARROWLEFT                       509
#define IDB_ARROWRIGHT                      510
#define IDB_ARROWTOP                        511

// internally used cursors
#define IDC_SPLITH                          900
#define IDC_SPLITV                          901

// default commands
#define IDM_ABOUT				            1001
#define IDM_EXIT				            1002
#define IDM_NEW 				            1003
#define IDM_OPEN 				            1004
#define IDM_SAVE 				            1005
#define IDM_PRINT 				            1006

#define IDM_UNDO							1100
#define IDM_REDO							1101
#define IDM_CUT							    1102
#define IDM_COPY							1103
#define IDM_PASTE							1104
#define IDM_DELETE							1105

#define IDM_VIEWDEFAULTTOOLBAR              1200
#define IDM_VIEWSTATUSBAR                   1201
#define IDM_VIEWDOCKINFO                    1202
#define IDM_CREATEDOCKWINDOW                1203
#define IDM_CONFIGDIALOG                    1204

#define IDM_TILEHORIZ						1400
#define IDM_TILEVERT						1401
#define IDM_CASCADE							1402
#define IDM_CLOSEALL						1403

#define IDM_WINDOWCHILD                     2000

// user defined windows messages
#define WM_CREATECHILDREN                   WM_USER     // wparam = client handle
#define WM_CREATETOOLBAR                    (WM_USER+1) // lparam = Pointer to toolbar
#define WM_CREATESTATUSBAR                  (WM_USER+2) // lparam = Pointer to statusbar
#define WM_CREATEMENUBAR                    (WM_USER+3) // lparam = Pointer to menubar
#define WM_CREATEDOCKINFO                   (WM_USER+4) // lparam = Pointer to dockinfo
#define WM_ABOUTHSCROLL                     (WM_USER+5)
#define WM_ABOUTVSCROLL                     (WM_USER+6)
#define WM_ENTERMENULOOP1                   (WM_USER+7)
#define WM_EXITMENULOOP1                    (WM_USER+8)
#define WM_MDICHILDSTATUS                   (WM_USER+9) // wparam=status code, lparam = Pointer to MDIChild

#define MCS_CREATED                         1
#define MCS_DESTROYED                       2
#define MCS_MAXIMIZED                       3
#define MCS_MINIMIZED                       4
#define MCS_RESTORED                        5
#define MCS_ACTIVATED                       6
#define MCS_DEACTIVATED                     7

// user defined notifications
#define NM_SELECTIONCHANGED                  128
#define NM_BUTTONCLICKED                     130
#define NM_BUTTONDOUBLECLICKED               131
#define NM_BUTTONMOUSEMOVE                   132
#define NM_EDITORCURSORUP                    140
#define NM_EDITORCURSORDOWN                  141
#define NM_EDITORESCAPE                      142
#define NM_EDITORRETURN                      143
#define NM_EDITORTAB                         144
#define NM_EDITORFUNCKEY                     145
#define NM_EDITORSELCHANGE                   146
#define NM_EDITORLOSTFOCUS                   147
#define NM_EDITORCONTEXTMENU                 148
#define NM_LISTVIEWNODEFOCUSED               150
#define NM_LISTVIEWNODEENTERED               151
#define NM_LISTVIEWSELCHANGE                 152
#define NM_LISTVIEWCONTEXTMENU               153
#define NM_LISTVIEWNODECONTEXTMENU           154
#define NM_LISTVIEWESCAPE                    155
#define NM_MENUITEMHOVERED                   160
#define NM_TABPAGESELECTED                   170
#define NM_TABPAGEDRAGGED                    171
#define NM_TREEVIEWNODELOADNEEDED            180
#define NM_TREEVIEWNODEEXPANDED              181
#define NM_TREEVIEWNODEFOCUSED               182
#define NM_TREEVIEWCONTEXTMENU               183
#define NM_TREEVIEWNODECONTEXTMENU           184
#define NM_GRIDVIEWCELLFOCUSED               190
#define NM_GRIDVIEWCELLFOCUSLOST             191
#define NM_GRIDVIEWFOCUSED                   192
#define NM_GRIDVIEWCELLENTERED               193
#define NM_GRIDVIEWROWINSERTED               194
#define NM_GRIDVIEWROWDELETED                195
