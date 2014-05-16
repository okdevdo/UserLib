includedir = $(prefix)/include/UserLib/WinSources
lib_LTLIBRARIES = libWinSources.la

libWinSources_la_SOURCES = \
ConditionVariable.cpp Debug.cpp FTPClient.cpp HtmlPage.cpp HttpCache.cpp \
HTTPClient.cpp HttpDate.cpp HTTPDownload.cpp Mutex.cpp WProcess.cpp \
SecurityFile.cpp SyncTCPSocketClasses.cpp Thread.cpp FilterPipe.cpp \
URL.cpp WinException.cpp MySQLDB.cpp MySQLDBImpl.cpp PostgresDB.cpp \
PostgresDBImpl.cpp SystemTime.cpp WinDirectoryIterator.cpp \
ConsoleApplication.cpp

include_HEADERS = \
ConditionVariable.h Debug.h FTPClient.h HtmlPage.h HttpCache.h \
HTTPClient.h HttpDate.h HTTPDownload.h Mutex.h WProcess.h \
SecurityFile.h SyncTCPSocketClasses.h Thread.h FilterPipe.h \
URL.h WinException.h MySQLDB.h PostgresDB.h SystemTime.h \
WinDirectoryIterator.h ConsoleApplication.h

libWinSources_la_LDFLAGS = -no-undefined -version-info 1 -mthreads
libWinSources_la_LIBADD = ../CppSources/libCppSources.la /usr/local/lib/libpq.la /usr/local/lib/libmysql.la

libWinSources_la_CFLAGS = 
libWinSources_la_CPPFLAGS = -I/usr/local/include/mysql -I/usr/local/include/postgres -I$(top_srcdir)/CSources -I$(top_srcdir)/CppSources
libWinSources_la_CXXFLAGS = -fexceptions -Wall -mthreads
if DEBUG
libWinSources_la_CPPFLAGS += -D_DEBUG
libWinSources_la_CXXFLAGS += -g
else
libWinSources_la_CPPFLAGS += -DNDEBUG
libWinSources_la_CXXFLAGS += -O
endif
if COND_MINGW
libWinSources_la_LIBADD += -lodbc32 -lodbccp32 -lws2_32 -lwsock32 -luser32 -ladvapi32 -lmsvcrt
libWinSources_la_SOURCES += SecurityContext.cpp EventLogger.cpp ODBC.cpp ODBCImpl.cpp AsyncFile.cpp \
AsyncIOBuffer.cpp AsyncIOData.cpp AsyncIOManager.cpp AsyncTCPClient.cpp Event.cpp
include_HEADERS += SecurityContext.h EventLogger.h ODBC.h
libWinSources_la_CPPFLAGS += -I../../UserLib/OKreisClassic
libWinSources_la_LIBADD += -lpthread
libWinSources_la_CPPFLAGS += -D_MINGW_CONSOLE
endif
if COND_MINGW64
libWinSources_la_LIBADD += -lodbc32 -lodbccp32 -lws2_32 -lwsock32 -luser32 -ladvapi32 -lmsvcrt
libWinSources_la_SOURCES += SecurityContext.cpp EventLogger.cpp ODBC.cpp ODBCImpl.cpp AsyncFile.cpp \
AsyncIOBuffer.cpp AsyncIOData.cpp AsyncIOManager.cpp AsyncTCPClient.cpp AsyncTCPServer.cpp Event.cpp
include_HEADERS += SecurityContext.h EventLogger.h ODBC.h
libWinSources_la_CPPFLAGS += -I../../UserLib/OKreisClassic -I../../UserLib/okreis
libWinSources_la_LIBADD += -lpthread
libWinSources_la_CPPFLAGS += -D_MINGW64_CONSOLE
endif
if COND_UNICODE
libWinSources_la_CPPFLAGS += -DUNICODE
endif

