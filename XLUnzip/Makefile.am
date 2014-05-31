bin_PROGRAMS = XLUnzip
XLUnzip_SOURCES = XLUnzip.cpp XLUnzipFunctions.cpp

XLUnzip_LDADD = ../WinSources/libWinSources.la
XLUnzip_LDFLAGS = 
XLUnzip_CFLAGS = 
XLUnzip_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XLUnzip_CXXFLAGS = -fexceptions -Wall
if !HAVE_NATCXX_11
if !HAVE_NATCXX_0X
if HAVE_GNUCXX_11
XLUnzip_CXXFLAGS += -std=gnu++11
else
if HAVE_GNUCXX_0X
XLUnzip_CXXFLAGS += -std=gnu++0x
endif
endif
endif
endif
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
