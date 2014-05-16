bin_PROGRAMS = XLZip
XLZip_SOURCES = XLZip.cpp XLZipFunctions.cpp

XLZip_LDADD = ../WinSources/libWinSources.la
XLZip_LDFLAGS = 
XLZip_CFLAGS = 
XLZip_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XLZip_CXXFLAGS = -fexceptions -Wall

if DEBUG
XLZip_CPPFLAGS += -D_DEBUG
XLZip_CXXFLAGS += -g
else
XLZip_CPPFLAGS += -DNDEBUG
XLZip_CXXFLAGS += -O
endif
if COND_MINGW
XLZip_LDADD += -lodbc32 -lodbccp32 -lws2_32 -lwsock32 -luser32 -ladvapi32 -lmsvcrt
XLZip_CXXFLAGS += -mthreads
XLZip_LDFLAGS += -mthreads
XLZip_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XLZip_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XLZip_CPPFLAGS += -DUNICODE
endif
