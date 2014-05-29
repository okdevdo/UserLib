bin_PROGRAMS = XTar
XTar_SOURCES = XTar.cpp XTarFunctions.cpp

XTar_LDADD = ../WinSources/libWinSources.la
XTar_LDFLAGS = 
XTar_CFLAGS = 
XTar_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources
XTar_CXXFLAGS = -fexceptions -Wall

if DEBUG
XTar_CPPFLAGS += -D_DEBUG
XTar_CXXFLAGS += -g
else
XTar_CPPFLAGS += -DNDEBUG
XTar_CXXFLAGS += -O
endif
if COND_MINGW
XTar_LDFLAGS += -mthreads
XTar_LDADD += -luser32 -ladvapi32 -lmsvcrt
XTar_CXXFLAGS += -mthreads
XTar_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
XTar_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XTar_CPPFLAGS += -DUNICODE
endif
