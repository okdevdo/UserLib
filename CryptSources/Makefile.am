includedir = $(prefix)/include/UserLib/CryptSources
lib_LTLIBRARIES = libCryptSources.la
libCryptSources_la_SOURCES = aes.c blowfish.c cbc.c cfb.c ctr.c des.c md2.c md4.c md5.c ofb.c sha1.c sha256.c sha512.c twofish.c xts.c 
include_HEADERS = cipher.h hash.h

libCryptSources_la_LDFLAGS = -no-undefined -version-info 1
libCryptSources_la_LIBADD = 
libCryptSources_la_CPPFLAGS = -I$(top_srcdir)/CSources
libCryptSources_la_CFLAGS =
if DEBUG
libCryptSources_la_CPPFLAGS += -D_DEBUG
libCryptSources_la_CFLAGS += -g
else
libCryptSources_la_CPPFLAGS += -DNDEBUG
libCryptSources_la_CFLAGS += -O
endif
if COND_MINGW
libCryptSources_la_LDFLAGS += -mthreads
libCryptSources_la_CFLAGS += -mthreads
endif
if COND_UNICODE
libCryptSources_la_CPPFLAGS += -DUNICODE
endif
