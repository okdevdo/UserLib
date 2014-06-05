includedir = $(prefix)/include/UserLib/CSources
lib_LTLIBRARIES = libCSources.la
libCSources_la_SOURCES = tfmem.c  dbase.c  dbasendx.c  liste.c  strutil.c  utlptr.c md5.c sha1.c
include_HEADERS = dbase.h liste.h pdeftype.h strutil.h utlptr.h csources.h pdefs.h tfmem.h md5.h sha1.h

libCSources_la_LDFLAGS = -no-undefined -version-info 1
libCSources_la_CPPFLAGS = -DARCH_IS_BIG_ENDIAN=0 -D_MT
libCSources_la_CFLAGS =
if DEBUG
libCSources_la_CPPFLAGS += -D_DEBUG
libCSources_la_CFLAGS += -g
else
libCSources_la_CPPFLAGS += -DNDEBUG
libCSources_la_CFLAGS += -O
endif
if COND_MINGW
libCSources_la_LDFLAGS += -mthreads
libCSources_la_CFLAGS += -mthreads
libCSources_la_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
libCSources_la_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
libCSources_la_CPPFLAGS += -DUNICODE
endif
