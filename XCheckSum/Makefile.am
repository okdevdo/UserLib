bin_PROGRAMS = XCheckSum
XCheckSum_SOURCES = XCheckSumFunctions.cpp XCheckSum.cpp

XCheckSum_LDADD = ../WinSources/libWinSources.la ../openssl/libOpenSSL.la
XCheckSum_LDFLAGS = 
XCheckSum_CFLAGS = 
XCheckSum_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/openssl -I$(top_srcdir)/WinSources
XCheckSum_CXXFLAGS = -fexceptions -Wall -std=gnu++11

if DEBUG
XCheckSum_CPPFLAGS += -D_DEBUG
XCheckSum_CXXFLAGS += -g
else
XCheckSum_CPPFLAGS += -DNDEBUG
XCheckSum_CXXFLAGS += -O
endif
if COND_MINGW
XCheckSum_LDFLAGS += -mthreads
XCheckSum_LDADD += -luser32 -ladvapi32 -lmsvcrt
XCheckSum_CXXFLAGS += -mthreads
XCheckSum_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XCheckSum_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XCheckSum_CPPFLAGS += -DUNICODE
endif
