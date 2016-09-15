AC_PREREQ([2.69])
AC_INIT([feedpp], [0.5.0], [https://github/behrooza])
AM_INIT_AUTOMAKE([feedpp],[0.5.0])

AC_CONFIG_SRCDIR([src/parser.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

# Checks for programs.
AC_PROG_CXX([g++])
AC_PROG_CC([gcc cc])

AC_ENABLE_SHARED
AC_PROG_LIBTOOL(libtool)

# Checks for libraries.
LIBXML2_REQUIRED_VERSION=2.9.0
LIBCURL_REQUIRED_VERSION=7.0.0

PKG_CHECK_MODULES([libxml2], [libxml-2.0 >= $LIBXML2_REQUIRED_VERSION])
PKG_CHECK_MODULES([libcurl], [libcurl >= $LIBCURL_REQUIRED_VERSION])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h langinfo.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MKTIME
AC_CHECK_FUNCS([dup2 ftruncate iswprint memset setlocale strdup strerror strncasecmp])

AC_OUTPUT
