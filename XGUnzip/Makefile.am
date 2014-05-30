bin_PROGRAMS = XGUnzip
XGUnzip_SOURCES = XGUnzip.cpp XGUnzipFunctions.cpp

XGUnzip_LDADD = ../WinSources/libWinSources.la
XGUnzip_LDFLAGS = 
XGUnzip_CFLAGS = 
XGUnzip_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XGUnzip_CXXFLAGS = -fexceptions -Wall -std=gnu++11

if DEBUG
XGUnzip_CPPFLAGS += -D_DEBUG
XGUnzip_CXXFLAGS += -g
else
XGUnzip_CPPFLAGS += -DNDEBUG
XGUnzip_CXXFLAGS += -O
endif
if COND_MINGW
XGUnzip_LDFLAGS += -mthreads
XGUnzip_LDADD += -luser32 -ladvapi32 -lmsvcrt
XGUnzip_CXXFLAGS += -mthreads
XGUnzip_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XGUnzip_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XGUnzip_CPPFLAGS += -DUNICODE
endif
