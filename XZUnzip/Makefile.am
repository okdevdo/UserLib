bin_PROGRAMS = XZUnzip
XZUnzip_SOURCES = XZUnzip.cpp XZUnzipFunctions.cpp

XZUnzip_LDADD = ../WinSources/libWinSources.la
XZUnzip_LDFLAGS = 
XZUnzip_CFLAGS = 
XZUnzip_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XZUnzip_CXXFLAGS = -fexceptions -Wall

if DEBUG
XZUnzip_CPPFLAGS += -D_DEBUG
XZUnzip_CXXFLAGS += -g
else
XZUnzip_CPPFLAGS += -DNDEBUG
XZUnzip_CXXFLAGS += -O
endif
if COND_MINGW
XZUnzip_LDADD += -luser32 -ladvapi32 -lmsvcrt
XZUnzip_CXXFLAGS += -mthreads
XZUnzip_LDFLAGS += -mthreads
XZUnzip_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XZUnzip_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XZUnzip_CPPFLAGS += -DUNICODE
endif
