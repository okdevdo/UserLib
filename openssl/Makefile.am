includedir = $(prefix)/include/UserLib/OpenSSL
lib_LTLIBRARIES = libOpenSSL.la

libOpenSSL_la_SOURCES = \
BIO.cpp BIO_METHOD.cpp EVP_CIPHER.cpp EVP_MD.cpp EVP_PKEY.cpp MISC.cpp OpenSSLClass.cpp OpenSSLLib.cpp SSL.cpp X509.cpp

include_HEADERS = \
OpenSSLClass.h SSL.h OS_PCH.H BIO.h BIO_METHOD.h EVP_CIPHER.h EVP_MD.h EVP_PKEY.h MISC.h OpenSSL.h OpenSSLLib.h X509.h

libOpenSSL_la_LDFLAGS = -no-undefined -version-info 1 -L/usr/local/lib
libOpenSSL_la_LIBADD = ../CppSources/libCppSources.la
libOpenSSL_la_LIBADD += -lcrypto -lssl

libOpenSSL_la_CFLAGS = 
libOpenSSL_la_CPPFLAGS = -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources
libOpenSSL_la_CXXFLAGS = -fexceptions -Wall
if DEBUG
libOpenSSL_la_CPPFLAGS += -D_DEBUG
libOpenSSL_la_CXXFLAGS += -g
else
libOpenSSL_la_CPPFLAGS += -DNDEBUG
libOpenSSL_la_CXXFLAGS += -O
endif
if COND_MINGW
libOpenSSL_la_CPPFLAGS += -I/usr/local/include
libOpenSSL_la_CXXFLAGS += -mthreads
libOpenSSL_la_LDFLAGS += -mthreads
libOpenSSL_la_LIBADD += -lodbc32 -lodbccp32 -lws2_32 -lwsock32 -luser32 -ladvapi32 -lmsvcrt
endif
if COND_UNICODE
libOpenSSL_la_CPPFLAGS += -DUNICODE
endif

