bin_PROGRAMS = XTest

XTest_SOURCES = Program.cpp Program0.cpp Program1.cpp Program2.cpp Program3.cpp Program4.cpp \
Program5.cpp Program6.cpp Program7.cpp Program8.cpp Program9.cpp Program10.cpp Program11.cpp \
Program12.cpp Program13.cpp Program14.cpp Program16.cpp Program19.cpp

XTest_LDADD = ../openssl/libOpenSSL.la ../ConSources/libConSources.la 
XTest_LDFLAGS = 
XTest_CFLAGS = 
XTest_CPPFLAGS = -I$(top_srcdir)/openssl -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources -I$(top_srcdir)/WinSources -I$(top_srcdir)/ConSources
XTest_CXXFLAGS = -fexceptions -Wall

if DEBUG
XTest_CPPFLAGS += -D_DEBUG
XTest_CXXFLAGS += -g
else
XTest_CPPFLAGS += -DNDEBUG
XTest_CXXFLAGS += -O
endif
if COND_MINGW
XTest_LDADD += -lrpcrt4 -lcrypt32 -lodbc32 -lodbccp32 -lws2_32 -lwsock32 -luser32 -ladvapi32 -lmsvcrt
XTest_CXXFLAGS += -mthreads
XTest_LDFLAGS += -mthreads
XTest_CPPFLAGS += -D_MINGW_CONSOLE -I/usr/local/include
XTest_LDADD += /mingw/mingw32/lib/CRT_noglob.o
endif
if COND_MINGW64
XTest_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
XTest_CPPFLAGS += -DUNICODE
endif
