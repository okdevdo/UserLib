includedir = $(prefix)/include/UserLib/CSources
lib_LTLIBRARIES = libCSources.la
libCSources_la_SOURCES = TFMEM.C  DBASE.C  DBASENDX.C  LISTE.C  SLISTE.C  STRUTIL.C  UTLPTR.C MD5.C SHA1.C
include_HEADERS = DBASE.H LISTE.H PDEFTYPE.H STRUTIL.H UTLPTR.H CSOURCES.H PDEFS.H SLISTE.H TFMEM.H MD5.H SHA1.H

libCSources_la_LDFLAGS = -no-undefined -version-info 1 -mthreads
libCSources_la_LIBADD = -lpthread
libCSources_la_CPPFLAGS = -DARCH_IS_BIG_ENDIAN=0 -D_MT
libCSources_la_CXXFLAGS = -mthreads
if DEBUG
libCSources_la_CPPFLAGS += -D_DEBUG
libCSources_la_CXXFLAGS += -g
else
libCSources_la_CPPFLAGS += -DNDEBUG
libCSources_la_CXXFLAGS += -O
endif
if COND_MINGW
libCSources_la_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
libCSources_la_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
libCSources_la_CPPFLAGS += -DUNICODE
endif
