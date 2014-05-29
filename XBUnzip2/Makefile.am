bin_PROGRAMS = XBUnzip2
XBUnzip2_SOURCES = XBUnzip2.cpp XBUnzip2Functions.cpp

XBUnzip2_LDADD = ../WinSources/libWinSources.la
XBUnzip2_LDFLAGS = 
XBUnzip2_CFLAGS = 
XBUnzip2_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XBUnzip2_CXXFLAGS = -fexceptions -Wall
if DEBUG
XBUnzip2_CPPFLAGS += -D_DEBUG
XBUnzip2_CXXFLAGS += -g
else
XBUnzip2_CPPFLAGS += -DNDEBUG
XBUnzip2_CXXFLAGS += -O
endif
if COND_MINGW
XBUnzip2_LDFLAGS += -mthreads
XBUnzip2_LDADD += -luser32 -ladvapi32 -lmsvcrt
XBUnzip2_CXXFLAGS += -mthreads
XBUnzip2_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XBUnzip2_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XBUnzip2_CPPFLAGS += -DUNICODE
endif
