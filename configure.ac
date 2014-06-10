#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.68)
AC_INIT(UserLib, 1.0.3, okdev10@arcor.de)
AC_CONFIG_SRCDIR([CSources/PDEFS.H])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

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
AM_INIT_AUTOMAKE([1.12 no-define foreign -Wall -Werror subdir-objects])
AM_PROG_AR

LT_PREREQ([2.2])
LT_INIT
LT_LANG([Windows Resource])
LT_LANG([C])
LT_LANG([C++])

CFLAGS=
CPPFLAGS=
CXXFLAGS=

# Checks for libraries.
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

# COMPILE_STDCXX_OX
AC_DEFUN([COMPILE_STDCXX_0X], [
  AC_CACHE_CHECK(if g++ supports C++0x features without additional flags,
  ac_cv_cxx_compile_cxx0x_native,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
  
	#include <type_traits>

    template <typename T>
    struct check 
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };
	
    template <typename T>
    class smart_ptr
	{
	typedef T value_type;
	public:
		smart_ptr(value_type* p): _p(p) {}
		const value_type* get() const { return _p; }
	protected:
		value_type* _p;
	};
	
	template <typename T> 
	bool operator == (const smart_ptr<T> &pA, std::nullptr_t pB) { return pA.get() == nullptr; };

    typedef check<check<bool>> right_angle_brackets;

    int a;
	int *p = nullptr;
	auto q = smart_ptr<int>(nullptr);
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
	
	bool bp = (p == nullptr);
	bool bq = (q == nullptr);
	
    check_type&& cr = static_cast<check_type&&>(c);

    ],,
  ac_cv_cxx_compile_cxx0x_native=yes, ac_cv_cxx_compile_cxx0x_native=no)
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++0x features with -std=c++0x,
  ac_cv_cxx_compile_cxx0x_cxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=c++0x"
  AC_TRY_COMPILE([
  
	#include <type_traits>

    template <typename T>
    struct check 
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };
	
    template <typename T>
    class smart_ptr
	{
	typedef T value_type;
	public:
		smart_ptr(value_type* p): _p(p) {}
		const value_type* get() const { return _p; }
	protected:
		value_type* _p;
	};
	
	template <typename T> 
	bool operator == (const smart_ptr<T> &pA, std::nullptr_t pB) { return pA.get() == nullptr; };

    typedef check<check<bool>> right_angle_brackets;

    int a;
	int *p = nullptr;
	auto q = smart_ptr<int>(nullptr);
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
	
	bool bp = (p == nullptr);
	bool bq = (q == nullptr);
	
    check_type&& cr = static_cast<check_type&&>(c);
	
	],,
  ac_cv_cxx_compile_cxx0x_cxx=yes, ac_cv_cxx_compile_cxx0x_cxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++0x features with -std=gnu++0x,
  ac_cv_cxx_compile_cxx0x_gxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=gnu++0x"
  AC_TRY_COMPILE([
  
	#include <type_traits>

    template <typename T>
    struct check 
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };
	
    template <typename T>
    class smart_ptr
	{
	typedef T value_type;
	public:
		smart_ptr(value_type* p): _p(p) {}
		const value_type* get() const { return _p; }
	protected:
		value_type* _p;
	};
	
	template <typename T> 
	bool operator == (const smart_ptr<T> &pA, std::nullptr_t pB) { return pA.get() == nullptr; };

    typedef check<check<bool>> right_angle_brackets;

    int a;
	int *p = nullptr;
	auto q = smart_ptr<int>(nullptr);
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
	
	bool bp = (p == nullptr);
	bool bq = (q == nullptr);
	
    check_type&& cr = static_cast<check_type&&>(c);
	
	],,
  ac_cv_cxx_compile_cxx0x_gxx=yes, ac_cv_cxx_compile_cxx0x_gxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

])

COMPILE_STDCXX_0X
AM_CONDITIONAL([HAVE_NATCXX_0X], [test x$ac_cv_cxx_compile_cxx0x_native = xyes])
AM_CONDITIONAL([HAVE_STDCXX_0X], [test x$ac_cv_cxx_compile_cxx0x_cxx = xyes])
AM_CONDITIONAL([HAVE_GNUCXX_0X], [test x$ac_cv_cxx_compile_cxx0x_gxx = xyes])
  
# COMPILE_STDCXX_11
AC_DEFUN([COMPILE_STDCXX_11], [
  AC_CACHE_CHECK(if g++ supports C++11 features without additional flags,
  ac_cv_cxx_compile_cxx11_native,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
  
	#include <type_traits>

    template <typename T>
    struct check 
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };
	
    template <typename T>
    class smart_ptr
	{
	typedef T value_type;
	public:
		smart_ptr(value_type* p): _p(p) {}
		const value_type* get() const { return _p; }
	protected:
		value_type* _p;
	};
	
	template <typename T> 
	bool operator == (const smart_ptr<T> &pA, std::nullptr_t pB) { return pA.get() == nullptr; };

    typedef check<check<bool>> right_angle_brackets;

    int a;
	int *p = nullptr;
	auto q = smart_ptr<int>(nullptr);
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
	
	bool bp = (p == nullptr);
	bool bq = (q == nullptr);
	
    check_type&& cr = static_cast<check_type&&>(c);
	
	],,
  ac_cv_cxx_compile_cxx11_native=yes, ac_cv_cxx_compile_cxx11_native=no)
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++11 features with -std=c++11,
  ac_cv_cxx_compile_cxx11_cxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=c++11"
  AC_TRY_COMPILE([
  
	#include <type_traits>

    template <typename T>
    struct check 
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };
	
    template <typename T>
    class smart_ptr
	{
	typedef T value_type;
	public:
		smart_ptr(value_type* p): _p(p) {}
		const value_type* get() const { return _p; }
	protected:
		value_type* _p;
	};
	
	template <typename T> 
	bool operator == (const smart_ptr<T> &pA, std::nullptr_t pB) { return pA.get() == nullptr; };

    typedef check<check<bool>> right_angle_brackets;

    int a;
	int *p = nullptr;
	auto q = smart_ptr<int>(nullptr);
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
	
	bool bp = (p == nullptr);
	bool bq = (q == nullptr);
	
    check_type&& cr = static_cast<check_type&&>(c);
	
	],,
  ac_cv_cxx_compile_cxx11_cxx=yes, ac_cv_cxx_compile_cxx11_cxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++11 features with -std=gnu++11,
  ac_cv_cxx_compile_cxx11_gxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=gnu++11"
  AC_TRY_COMPILE([
  
	#include <type_traits>

    template <typename T>
    struct check 
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };
	
    template <typename T>
    class smart_ptr
	{
	typedef T value_type;
	public:
		smart_ptr(value_type* p): _p(p) {}
		const value_type* get() const { return _p; }
	protected:
		value_type* _p;
	};
	
	template <typename T> 
	bool operator == (const smart_ptr<T> &pA, std::nullptr_t pB) { return pA.get() == nullptr; };

    typedef check<check<bool>> right_angle_brackets;

    int a;
	int *p = nullptr;
	auto q = smart_ptr<int>(nullptr);
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
	
	bool bp = (p == nullptr);
	bool bq = (q == nullptr);
	
    check_type&& cr = static_cast<check_type&&>(c);
	
	],,
  ac_cv_cxx_compile_cxx11_gxx=yes, ac_cv_cxx_compile_cxx11_gxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])
])
COMPILE_STDCXX_11
AM_CONDITIONAL([HAVE_NATCXX_11], [test x$ac_cv_cxx_compile_cxx11_native = xyes])
AM_CONDITIONAL([HAVE_STDCXX_11], [test x$ac_cv_cxx_compile_cxx11_cxx = xyes])
AM_CONDITIONAL([HAVE_GNUCXX_11], [test x$ac_cv_cxx_compile_cxx11_gxx = xyes])

if  test x$ac_cv_cxx_compile_cxx0x_native = xno && test x$ac_cv_cxx_compile_cxx0x_cxx = xno && test x$ac_cv_cxx_compile_cxx0x_gxx = xno && test x$ac_cv_cxx_compile_cxx11_native = xno && test x$ac_cv_cxx_compile_cxx11_cxx = xno && test x$ac_cv_cxx_compile_cxx11_gxx = xno ; then
	AC_MSG_ERROR([compiler does not complain to the desired C++11 features])
fi

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
