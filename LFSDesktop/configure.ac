
AC_INIT([LFSDesktop],[0.2.0],[LFSDesktop],[LFSDesktop])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC

X11_MODULES="x11 >= 1.6.2"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_LIBS)
AC_SUBST(X11_CFLAGS)

XINERAMA_MODULES="xinerama >= 1.1.3"
PKG_CHECK_MODULES(XINERAMA, $XINERAMA_MODULES)
AC_SUBST(XINERAMA_LIBS)
AC_SUBST(XINERAMA_CFLAGS)

XEXT_MODULES="xext >= 1.3.2"
PKG_CHECK_MODULES(XEXT, $XEXT_MODULES)
AC_SUBST(XEXT_LIBS)
AC_SUBST(XEXT_CFLAGS)

XFT_MODULES="xft >= 2.3.1"
PKG_CHECK_MODULES(XFT, $XFT_MODULES)
AC_SUBST(XFT_CFLAGS)
AC_SUBST(XFT_LIBS)

CAIRO_MODULES="cairo >= 1.12.16"
PKG_CHECK_MODULES(CAIRO, $CAIRO_MODULES)
AC_SUBST(CAIRO_LIBS)
AC_SUBST(CAIRO_CFLAGS)

LIBGLIB_MODULES="glib-2.0 >= 2.60.0"
PKG_CHECK_MODULES(LIBGLIB, $LIBGLIB_MODULES)
AC_SUBST(LIBGLIB_LIBS)
AC_SUBST(LIBGLIB_CFLAGS)

LIBUDEV_MODULES="libudev >= 182"
PKG_CHECK_MODULES(LIBUDEV, $LIBUDEV_MODULES)
AC_SUBST(LIBUDEV_LIBS)
AC_SUBST(LIBUDEV_CFLAGS)

LFSTK_MODULES="lfstk >= 0.5.1"
PKG_CHECK_MODULES(LFSTK, $LFSTK_MODULES)
AC_SUBST(LFSTK_LIBS)
AC_SUBST(LFSTK_CFLAGS)

AC_MSG_CHECKING(Use SUID Helper)

AC_ARG_ENABLE(suidhelper,AS_HELP_STRING([--enable-suidhelper],[Enable suid helper]),[enable_suidhelper=$enableval],[enable_suidhelper="yes"])
if test $enable_suidhelper = "yes" ;then
	AC_DEFINE([_USESUIDHELPER_], [], [Use suid helper])
fi
AC_MSG_RESULT($enable_suidhelper)

#AC_CHECK_PROG(MIMTYPE, mimetype ,true,false)
#if test $MIMTYPE = false; then
#	AC_MSG_NOTICE([No File-MimeInfo!])
#	
#	AC_MSG_NOTICE([You can download it from here:])
#	AC_MSG_ERROR([https://cpan.metacpan.org/authors/id/M/MI/MICHIELB/])
#fi

AC_MSG_CHECKING(whether to build dbug functions)
AC_ARG_ENABLE(debug,AS_HELP_STRING([--enable-debug],[Enable debug code]),
    [enable_debug=$enableval],
    [enable_debug="no"])
AC_MSG_RESULT($enable_debug)
if test "$enable_debug" = "yes"; then
	AC_DEFINE([_ENABLEDEBUG_], [], [Enable debug code])
fi

AC_CONFIG_FILES([Makefile LFSDesktop/app/Makefile LFSDesktop/helper/Makefile])
AC_OUTPUT

