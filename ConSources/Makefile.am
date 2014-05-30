includedir = $(prefix)/include/UserLib/ConSources
lib_LTLIBRARIES = libConSources.la

libConSources_la_SOURCES = Console.cpp ConsoleTextViewer.cpp \
ConsoleButtonControl.cpp ConsoleWindow.cpp \
ConsoleComboControl.cpp ConsoleWindowMenu.cpp \
ConsoleComboPopupListControl.cpp ConsoleControl.cpp \
ConsoleDialog.cpp ConsoleDirListControl.cpp \
ConsoleFileDialog.cpp ConsoleFileListControl.cpp \
ConsoleLabelControl.cpp ConsoleListControl.cpp \
ConsoleMainMenu.cpp ConsoleMenuItem.cpp \
ConsolePopupMenu.cpp ConsoleScrollbarControl.cpp \
ConsoleTextControl.cpp ConsoleEditMenu.cpp ConsoleClipboard.cpp \
ConsoleUndoManager.cpp ConsoleLayout.cpp ConsoleTableViewer.cpp \
ConsoleTabPageControl.cpp ConsoleTabControl.cpp

include_HEADERS = Console.h ConsoleMainMenu.h ConsoleButtonControl.h \
ConsoleMenuItem.h ConsoleComboControl.h ConsolePopupMenu.h \
ConsoleComboPopupListControl.h ConsoleScrollbarControl.h \
ConsoleControl.h ConsoleTextControl.h ConsoleDialog.h \
ConsoleTextViewer.h ConsoleDirListControl.h ConsoleWindow.h \
ConsoleFileDialog.h ConsoleWindowMenu.h ConsoleFileListControl.h \
ConsoleLabelControl.h ConsoleListControl.h ConSources.h \
ConsoleEditMenu.h ConsoleClipboard.h ConsoleUndoManager.h ConsoleLayout.h \
ConsoleTableViewer.h ConsoleTabPageControl.h ConsoleTabControl.h

libConSources_la_LDFLAGS = -no-undefined -version-info 1
libConSources_la_LIBADD = ../WinSources/libWinSources.la
libConSources_la_CFLAGS = 
libConSources_la_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
libConSources_la_CXXFLAGS = -fexceptions -Wall -std=gnu++11
if DEBUG
libConSources_la_CPPFLAGS += -D_DEBUG
libConSources_la_CXXFLAGS += -g
else
libConSources_la_CPPFLAGS += -DNDEBUG
libConSources_la_CXXFLAGS += -O
endif
if COND_MINGW
libConSources_la_CXXFLAGS += -mthreads
libConSources_la_LDFLAGS += -mthreads
libConSources_la_LIBADD += -luser32 -ladvapi32 -lmsvcrt
libConSources_la_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
libConSources_la_CPPFLAGS += -D_MINGW64_CONSOLE 
endif
if COND_CYGWIN
libConSources_la_LIBADD += -lncurses
endif
if COND_CYGWIN64
libConSources_la_LIBADD += -lncurses
endif
if COND_UNICODE
libConSources_la_CPPFLAGS += -DUNICODE
endif

