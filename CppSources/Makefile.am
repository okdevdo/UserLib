includedir = $(prefix)/include/UserLib/CppSources
lib_LTLIBRARIES = libCppSources.la

libCppSources_la_SOURCES = \
SqLite3DB.cpp SqLite3DBImpl.cpp CryptFilter.cpp HashBuffer.cpp SHA1Buffer.cpp\
DataBase.cpp FilePath.cpp Application.cpp FlagRegister.cpp BaseException.cpp Option.cpp ByteBuffer.cpp \
StringBuffer.cpp ByteLinkedBuffer.cpp StringLiteral.cpp Configuration.cpp DataVector.cpp VolumeIterator.cpp \
CppObject.cpp DirectoryIterator.cpp File.cpp Filter.cpp BZip2Filter.cpp GZipFilter.cpp LZMAFilter.cpp LZMALibFilter.cpp \
XZFilter.cpp ZLibFilter.cpp Archive.cpp TarArchive.cpp CRC32Value.cpp MD5Buffer.cpp okDateTime.cpp ZipArchive.cpp ZipFilter.cpp \
SAXParser.cpp SAXParserImpl.cpp

include_HEADERS = \
SqLite3DB.h SAXParser.h CryptFilter.h HashBuffer.h SHA1Buffer.h\
Allocator.h CppObject.h DataDoubleLinkedList.h FlagRegister.h Application.h CppSources.h DataHashtable.h Option.h \
BaseException.h DataArray.h DataVector.h StringBuffer.h ByteBuffer.h DataBase.h DirectoryIterator.h StringLiteral.h \
ByteLinkedBuffer.h DataBinaryTree.h File.h VolumeIterator.h Configuration.h DataBTree.h FilePath.h Archive.h \
BZip2Filter.h CRC32Value.h Filter.h GZipFilter.h MD5Buffer.h TarArchive.h ZLibFilter.h okDateTime.h ZipArchive.h ZipFilter.h \
LZMALibFilter.h LZMAFilter.h XZFilter.h

libCppSources_la_LDFLAGS = -no-undefined -version-info 1 -L/usr/local/lib
libCppSources_la_LIBADD = ../CSources/libCSources.la ../CryptSources/libCryptSources.la -lbz2 -llzma -lz -lsqlite3 -lexpat
libCppSources_la_CFLAGS = 
libCppSources_la_CPPFLAGS = -DXML_LARGE_SIZE -DHAVE_EXPAT_CONFIG_H -I$(top_srcdir)/CSources -I$(top_srcdir)/CryptSources -I/usr/local/include
libCppSources_la_CXXFLAGS = -fexceptions -Wall
if !HAVE_NATCXX_11
if !HAVE_NATCXX_0X
if HAVE_GNUCXX_11
libCppSources_la_CXXFLAGS += -std=gnu++11
else
if HAVE_GNUCXX_0X
libCppSources_la_CXXFLAGS += -std=gnu++0x
endif
endif
endif
endif
if DEBUG
libCppSources_la_CPPFLAGS += -D_DEBUG
libCppSources_la_CXXFLAGS += -g
else
libCppSources_la_CPPFLAGS += -DNDEBUG
libCppSources_la_CXXFLAGS += -O
endif
if COND_MINGW
libCppSources_la_CXXFLAGS += -mthreads
libCppSources_la_LDFLAGS += -mthreads
libCppSources_la_LIBADD += -lmsvcr100 -lmsvcrt
endif
if COND_UNICODE
libCppSources_la_CPPFLAGS += -DUNICODE -DXML_UNICODE_WCHAR_T
endif
