#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.68)
AC_INIT(XTest, 1.0, okinfo@arcor.de)
AC_CONFIG_SRCDIR([CSources/PDEFS.H])
AM_INIT_AUTOMAKE([1.12 no-define foreign -Wall -Werror subdir-objects])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST

want_mingw=no
want_mingw64=no
want_cygwin=no
want_cygwin64=no
case $host_os in
	mingw*)
		case $host_cpu in
			i?86)  want_mingw=yes ;;
			x86_64) want_mingw64=yes ;;
		esac
		;;
	cygwin*)
		case $host_cpu in
			i?86)  want_cygwin=yes ;;
			x86_64) want_cygwin64=yes ;;
		esac
		;;
	linux*) ;;
	*bsd*) ;;
	*) ;;
esac
if test x$want_mingw = xyes; then
   echo -e "#include \"stdio.h\"\nvoid main(void) { \n#ifdef __MINGW_MAJOR_VERSION\nprintf(\"%d\", __MINGW_MAJOR_VERSION);\n#else\nprintf(\"%d\", __MINGW32_MAJOR_VERSION);\n#endif\n }" > conftest.c
   gcc -o conftest.exe conftest.c
   MINGW_VERSION=`conftest.exe`
   rm conftest.exe conftest.c
fi
AM_CONDITIONAL([COND_MINGW_V4], [test x$MINGW_VERSION = x4])
AM_CONDITIONAL([COND_MINGW], [test x$want_mingw = xyes])
AM_CONDITIONAL([COND_MINGW64], [test x$want_mingw64 = xyes])
AM_CONDITIONAL([COND_CYGWIN], [test x$want_cygwin = xyes])
AM_CONDITIONAL([COND_CYGWIN64], [test x$want_cygwin64 = xyes])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR
CFLAGS=
CPPFLAGS=
CXXFLAGS=
AC_PROG_MAKE_SET

# Checks for libraries.
LT_INIT
AC_CHECK_LIB(pthread, [pthread_mutex_init])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stddef.h stdlib.h string.h unistd.h wchar.h malloc.h netdb.h sys/socket.h utime.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MKTIME
AC_FUNC_MMAP

AC_CHECK_FUNCS([atexit getcwd mkdir putenv rmdir select socket strchr strdup strerror memmove])
AC_CHECK_FUNCS([clock_gettime floor ftime memchr memset munmap strrchr strstr utime])


# Arguments and Conditionals
AC_ARG_ENABLE([debug],
 [  --enable-debug    Turn on debugging],
 [case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
  esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_ARG_ENABLE([unicode],
 [  --enable-unicode    Enable Unicode],
 [case "${enableval}" in
  yes) want_unicode=true ;;
  no)  want_unicode=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-unicode]) ;;
  esac],[want_unicode=false])
AM_CONDITIONAL([COND_UNICODE], [test x$want_unicode = xtrue])

AC_CONFIG_FILES([Makefile 
                 CSources/Makefile
                 CryptSources/Makefile
                 CppSources/Makefile
				 openssl/Makefile
				 ConSources/Makefile
				 WinSources/Makefile
                 XTest/Makefile
				 XBUnzip2/Makefile
				 XBZip2/Makefile
				 XGUnzip/Makefile
				 XGZip/Makefile
				 XLUnzip/Makefile
				 XLZip/Makefile
				 XZUnzip/Makefile
				 XZZip/Makefile
				 XUnzip/Makefile
				 XZip/Makefile
				 XTar/Makefile
				 XCheckSum/Makefile
				 ])
AC_OUTPUT
