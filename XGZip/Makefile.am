bin_PROGRAMS = XGZip
XGZip_SOURCES = XGZip.cpp XGZipFunctions.cpp

XGZip_LDADD = ../WinSources/libWinSources.la
XGZip_LDFLAGS = 
XGZip_CFLAGS = 
XGZip_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XGZip_CXXFLAGS = -fexceptions -Wall

if DEBUG
XGZip_CPPFLAGS += -D_DEBUG
XGZip_CXXFLAGS += -g
else
XGZip_CPPFLAGS += -DNDEBUG
XGZip_CXXFLAGS += -O
endif
if COND_MINGW
XGZip_LDFLAGS += -mthreads
XGZip_LDADD += -luser32 -ladvapi32 -lmsvcrt
XGZip_CXXFLAGS += -mthreads
XGZip_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XGZip_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XGZip_CPPFLAGS += -DUNICODE
endif
