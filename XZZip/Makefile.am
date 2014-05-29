bin_PROGRAMS = XZZip
XZZip_SOURCES = XZZip.cpp XZZipFunctions.cpp

XZZip_LDFLAGS = 
XZZip_LDADD = ../WinSources/libWinSources.la
XZZip_CFLAGS = 
XZZip_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XZZip_CXXFLAGS = -fexceptions -Wall

if DEBUG
XZZip_CPPFLAGS += -D_DEBUG
XZZip_CXXFLAGS += -g
else
XZZip_CPPFLAGS += -DNDEBUG
XZZip_CXXFLAGS += -O
endif
if COND_MINGW
XZZip_LDFLAGS += -mthreads
XZZip_LDADD += -luser32 -ladvapi32 -lmsvcrt
XZZip_CXXFLAGS += -mthreads
XZZip_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XZZip_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XZZip_CPPFLAGS += -DUNICODE
endif
