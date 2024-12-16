#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libfolia],[2.22],[lamasoftware@science.ru.nl]) #also adapt in codemeta.json!
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AX_REQUIRE_DEFINED([AX_CXX_COMPILE_STDCXX_17])

# Checks for programs.
AC_PROG_CXX( [g++ clang++] )
AX_CXX_COMPILE_STDCXX_17

# use libtool
LT_INIT

# when running tests, use CXX
AC_LANG([C++])

# check OpenMP support
AC_OPENMP

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([netdb.h sys/socket.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([localtime_r])
AC_CHECK_FUNCS([gethostname memset select])

AX_PTHREAD([],[AC_MSG_ERROR([We need pthread support!])])
LIBS="$PTHREAD_LIBS $LIBS"
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"

PKG_PROG_PKG_CONFIG
if test "x$prefix" = "xNONE"; then
   prefix="/usr/local"
fi

if test "x$PKG_CONFIG_PATH" = x; then
    export PKG_CONFIG_PATH="$prefix/lib/pkgconfig"
else
    export PKG_CONFIG_PATH="$prefix/lib/pkgconfig:$PKG_CONFIG_PATH"
fi

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN"; then
   AC_MSG_WARN([Doxygen not found - continue without Doxygen support])
else
   AC_CHECK_PROGS([DOT], [dot])
   if test -z "$DOT"; then
      AC_MSG_ERROR([Doxygen needs dot, please install dot first])
   fi
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

AC_OSX_PKG( [icu4c] )

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([XML2], [libxml-2.0 >= 2.9.4] )
CXXFLAGS="$CXXFLAGS $XML2_CFLAGS"
LIBS="$XML2_LIBS $LIBS"

PKG_CHECK_MODULES([ticcutils], [ticcutils >= 0.34] )

CXXFLAGS="$CXXFLAGS $ticcutils_CFLAGS"
LIBS="$ticcutils_LIBS $LIBS"

PKG_CHECK_MODULES([ICU], [icu-uc >= 50 icu-io] )
CXXFLAGS="$CXXFLAGS $ICU_CFLAGS"
LIBS="$ICU_LIBS $LIBS"

AC_CONFIG_FILES([
  Makefile
  folia.pc
  m4/Makefile
  src/Makefile
  docs/Makefile
  include/Makefile
  include/libfolia/Makefile
])
AC_OUTPUT
