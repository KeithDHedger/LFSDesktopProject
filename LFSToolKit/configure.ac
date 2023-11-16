
m4_define([LFSTOOLKIT_MAJ_N],[0])
m4_define([LFSTOOLKIT_MIN_N],[6])
m4_define([LFSTOOLKIT_REV_N],[0])

m4_define([lfstk_api_version], [2.0])
m4_define([lfstk_version],[LFSTOOLKIT_MAJ_N.LFSTOOLKIT_MIN_N.LFSTOOLKIT_REV_N])

AC_INIT([LFSToolKit],[LFSTOOLKIT_MAJ_N.LFSTOOLKIT_MIN_N.LFSTOOLKIT_REV_N],[PROJ],[LFSToolKit])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
#AC_CONFIG_MACRO_DIRS()

AM_INIT_AUTOMAKE

AC_PROG_CC

LT_INIT([disable-static])

# libtool
LFSTOOLKIT_MINOR=LFSTOOLKIT_MIN_N()
LFSTOOLKIT_INTERFACE=$(( LFSTOOLKIT_MAJ_N() + ${LFSTOOLKIT_MINOR} ))
LFSTOOLKIT_REVISION=LFSTOOLKIT_REV_N()
LFSTOOLKIT_LIBTOOL_VERSION=$LFSTOOLKIT_INTERFACE:$LFSTOOLKIT_REVISION:$LFSTOOLKIT_MINOR

LFSTOOLKIT_VERSION=lfstk_version
#$LFSTOOLKIT_INTERFACE.$LFSTOOLKIT_REVISION.$LFSTOOLKIT_MINOR

AC_SUBST(LFSTOOLKIT_LIBTOOL_VERSION)
AC_SUBST(LFSTOOLKIT_VERSION)

X11_MODULES="x11 >= 1.6.2"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)

XINERAMA_MODULES="xinerama >= 1.1.3"
PKG_CHECK_MODULES(XINERAMA, $XINERAMA_MODULES)
AC_SUBST(XINERAMA_LIBS)
AC_SUBST(XINERAMA_CFLAGS)

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN],
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

CAIRO_MODULES="cairo >= 1.12.16"
PKG_CHECK_MODULES(CAIRO, $CAIRO_MODULES)
AC_SUBST(CAIRO_LIBS)
AC_SUBST(CAIRO_CFLAGS)

JPEG_MODULES="libjpeg >= 1.5.1"
PKG_CHECK_MODULES(JPEG, $JPEG_MODULES)
AC_SUBST(JPEG_LIBS)
AC_SUBST(JPEG_CFLAGS)

LIBGLIB_MODULES="glib-2.0 >= 2.60.0"
PKG_CHECK_MODULES(LIBGLIB, $LIBGLIB_MODULES)
AC_SUBST(LIBGLIB_LIBS)
AC_SUBST(LIBGLIB_CFLAGS)

LIBGIO_MODULES="gio-2.0 >= 2.60.0"
PKG_CHECK_MODULES(LIBGIO, $LIBGIO_MODULES)
AC_SUBST(LIBGIO_LIBS)
AC_SUBST(LIBGIO_CFLAGS)

AC_MSG_CHECKING(whether to build debug functions)
AC_ARG_ENABLE(debug,AS_HELP_STRING([--enable-debug],[Enable debug code]),
    [enable_debug=$enableval],
    [enable_debug="no"])
AC_MSG_RESULT($enable_debug)
if test "$enable_debug" = "yes"; then
	AC_DEFINE([_ENABLEDEBUG_], [], [Enable debug code])
fi

AC_MSG_CHECKING(whether to warn about debug functions)
AC_ARG_ENABLE(warn-debug,AS_HELP_STRING([--enable-warn-debug],[Enable warn debug code]),
    [enable_warn_debug=$enableval],
    [enable_warn_debug="no"])
AC_MSG_RESULT($enable_warn_debug)
if test "$enable_warn_debug" = "yes"; then
	AC_DEFINE([_WARN_ENABLEDEBUG_], [], [Enable warn debug code])
fi

AC_CONFIG_FILES([lfstk.pc LFSToolKit/resources/themes/AllHallows.lfstk/lfstoolkit.rc LFSToolKit/resources/themes/OldBrownWood.lfstk/lfstoolkit.rc LFSToolKit/resources/themes/OldyXmas.lfstk/lfstoolkit.rc LFSToolKit/resources/themes/Steel.lfstk/lfstoolkit.rc LFSToolKit/resources/themes/Winter.lfstk/lfstoolkit.rc LFSToolKit/resources/themes/SpringWood.lfstk/lfstoolkit.rc])

AC_CONFIG_FILES([Makefile LFSToolKit/app/Makefile  docs/Makefile])
AC_OUTPUT

