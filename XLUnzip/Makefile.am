bin_PROGRAMS = XLUnzip
XLUnzip_SOURCES = XLUnzip.cpp XLUnzipFunctions.cpp

XLUnzip_LDADD = ../WinSources/libWinSources.la
XLUnzip_LDFLAGS = 
XLUnzip_CFLAGS = 
XLUnzip_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XLUnzip_CXXFLAGS = -fexceptions -Wall -std=gnu++11

if DEBUG
XLUnzip_CPPFLAGS += -D_DEBUG
XLUnzip_CXXFLAGS += -g
else
XLUnzip_CPPFLAGS += -DNDEBUG
XLUnzip_CXXFLAGS += -O
endif
if COND_MINGW
XLUnzip_LDFLAGS += -mthreads
XLUnzip_LDADD += -luser32 -ladvapi32 -lmsvcrt
XLUnzip_CXXFLAGS += -mthreads
XLUnzip_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XLUnzip_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XLUnzip_CPPFLAGS += -DUNICODE
endif
