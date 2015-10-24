#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(a-nslp, 0.97, la.marentes455@uniandes.edu.co)
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

AC_CHECK_LIB([cppunit], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gethostname gettimeofday memset socket strerror])

AC_PROG_RANLIB

#check for unordered map
[SAVECXXFLAGS=$CXXFLAGS; CXXFLAGS="-std=c++0x"]
AC_LANG_CPLUSPLUS

AC_CACHE_CHECK([whether unordered_map is available],
			[ac_cv_unordered_map_exists],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <unordered_map>]],
				[[std::unordered_map<int,int> testmap;]])],
				[ac_cv_unordered_map_exists=yes],
				[ac_cv_unordered_map_exists=no])])
[CXXFLAGS=$SAVECXXFLAGS]


PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBXML], [libxml-2.0])
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0])

# Checks for libxslt libraries
LIBXSLT_REQUIRED=1.1.10
PKG_CHECK_MODULES(LIBXSLT, libxslt >= $LIBXSLT_REQUIRED)
AC_SUBST(LIBXSLT_CFLAGS)
AC_SUBST(LIBXSLT_LIBS)

# Check CURL, needed for install netmate policy rules.
CURL_LIBS=
PKG_CHECK_MODULES(CURL, 
  libcurl, 
  [AC_DEFINE([HAVE_CURL],[1],[Use CURL])])
AM_CONDITIONAL(CURL, test -n "$CURL_LIBS")
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)


PKG_CHECK_MODULES([LIBUUID], [libuuid],
   [LIBUUID_FOUND=yes], [LIBUUID_FOUND=no])
   if test "$LIBUUID_FOUND" = "no" ; then
        PKG_CHECK_MODULES([LIBUUID], [uuid],
			      [LIBUUID_FOUND=yes], [LIBUUID_FOUND=no])
	    if test "$LIBUUID_FOUND" = "no" ; then
	  	  AC_MSG_ERROR([libuuid development files required])
	    else
		  LIBUUID_INCLUDEDIR=$(pkg-config --variable=includedir uuid)
		  LIBUUID_CFLAGS+=" -I$LIBUUID_INCLUDEDIR/uuid "
	    fi
   fi
AC_SUBST([LIBUUID_CFLAGS])
AC_SUBST([LIBUUID_LIBS])


PKG_CHECK_MODULES([LIBPROT], [libprot-0.0])
AC_SUBST([LIBPROT_CFLAGS])
AC_SUBST([LIBPROT_LIBS])

PKG_CHECK_MODULES([LIBFASTQUEUE], [libfastqueue-0.0])
AC_SUBST([LIBFASTQUEUE_CFLAGS])
AC_SUBST([LIBFASTQUEUE_LIBS])

PKG_CHECK_MODULES([LIBGIST], [libgist-0.0])
AC_SUBST([LIBGIST_CFLAGS])
AC_SUBST([LIBGIST_LIBS])

PKG_CHECK_MODULES([LIBIPAP], [libipap-0.0])
AC_SUBST([LIBIPAP_CFLAGS])
AC_SUBST([LIBIPAP_LIBS])

AC_ARG_ENABLE([sctp],
	[AS_HELP_STRING([--enable-sctp], [include SCTP transport capability (default: enabled)])],
	[enable_sctp=$enableval],
	[enable_sctp=no])
AM_CONDITIONAL(USE_WITH_SCTP, test "$enable_sctp" = yes)

AM_CONDITIONAL(NSIS_NO_WARN_HASHMAP, test "$ac_cv_unordered_map_exists" = yes)

AC_ARG_ENABLE(debug,
  [  --enable-debug            enable debug build ],
  [case "${enableval}" in
    yes) debug=true ;;
    no)  debug=false ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
  esac],[debug=false])
AM_CONDITIONAL(ENABLE_DEBUG, test x$debug = xtrue)
if test $debug = true ; then
  AC_DEFINE(ENABLE_DEBUG, 1, [enable debug build])
fi


if test "${prefix}" = "NONE" ; then
  prefix=${ac_default_prefix}
fi

libdir="${libdir}/$PACKAGE"
if test "${exec_prefix}" = "NONE" ; then
  exec_prefix=${prefix}
fi

sysconfdir="${sysconfdir}/$PACKAGE"
DEF_SYSCONFDIR=`eval echo ${sysconfdir}`
AC_DEFINE_UNQUOTED(DEF_SYSCONFDIR, "$DEF_SYSCONFDIR", "sysconfig dir")

DEF_STATEDIR=`eval echo ${prefix}/var`
AC_DEFINE_UNQUOTED(DEF_STATEDIR, "$DEF_STATEDIR", "state dir")

DEF_LIBDIR=`eval echo ${libdir}`
AC_DEFINE_UNQUOTED(DEF_LIBDIR, "$DEF_LIBDIR", "lib dir")

DEF_BINDIR=`eval echo ${bindir}`
AC_DEFINE_UNQUOTED(DEF_BINDIR, "$DEF_BINDIR", "bin dir")


# Additional arguments (and substitutions)
# Allow the pkg-config directory to be set
AC_ARG_WITH(pkgconfigdir,
	AC_HELP_STRING([--with-pkgconfigdir],
	[Use the specified pkgconfig dir (default is pkgconfig)]),
	[pkgconfigdir=${withval}],
	[pkgconfigdir='${exec_prefix}/lib/pkgconfig'])

AC_SUBST([pkgconfigdir])
AC_SUBST(DEF_SYSCONFDIR)
AC_SUBST(DEF_STATEDIR)
AC_SUBST(DEF_LIBDIR)
AC_SUBST(DEF_BINDIR)


AC_MSG_NOTICE([pkgconfig directory is ${pkgconfigdir}])

AC_CONFIG_FILES([Makefile
                 libanslp-0.0.pc
                 libanslp_msg-0.0.pc
                 etc/Makefile
                 etc/nsis.ka.conf
                 src/msg/Makefile
                 src/Makefile
                 test/Makefile
                 test_main/Makefile])

AC_OUTPUT
