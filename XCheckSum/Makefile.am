bin_PROGRAMS = XChecksum
XChecksum_SOURCES = XChecksum.cpp XChecksumFunctions.cpp

XChecksum_LDADD = ../WinSources/libWinSources.la ../openssl/libOpenSSL.la
XChecksum_LDFLAGS = 
XChecksum_CFLAGS = 
XChecksum_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/openssl -I$(top_srcdir)/WinSources
XChecksum_CXXFLAGS = -fexceptions -Wall

if DEBUG
XChecksum_CPPFLAGS += -D_DEBUG
XChecksum_CXXFLAGS += -g
else
XChecksum_CPPFLAGS += -DNDEBUG
XChecksum_CXXFLAGS += -O
endif
if COND_MINGW
XChecksum_LDFLAGS += -mthreads
XChecksum_LDADD += -lws2_32 -lwsock32 -luser32 -ladvapi32 -lmsvcrt
XChecksum_CXXFLAGS += -mthreads
XChecksum_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XChecksum_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XChecksum_CPPFLAGS += -DUNICODE
endif
