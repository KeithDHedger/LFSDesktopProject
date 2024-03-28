
AC_INIT([LFSDock],[0.2.0],[PROJ],[LFSDock])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC

X11_MODULES="x11 >= 1.6.2"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)

ALSA_MODULES="alsa >= 1.1.5"
PKG_CHECK_MODULES(ALSA, $ALSA_MODULES)
AC_SUBST(ALSA_CFLAGS)
AC_SUBST(ALSA_LIBS)

XFT_MODULES="xft >= 2.3.1"
PKG_CHECK_MODULES(XFT, $XFT_MODULES)
AC_SUBST(XFT_CFLAGS)
AC_SUBST(XFT_LIBS)

CAIRO_MODULES="cairo >= 1.12.16"
PKG_CHECK_MODULES(CAIRO, $CAIRO_MODULES)
AC_SUBST(CAIRO_LIBS)
AC_SUBST(CAIRO_CFLAGS)

LFSTK_MODULES="lfstk >= 0.6.0"
PKG_CHECK_MODULES(LFSTK, $LFSTK_MODULES)
AC_SUBST(LFSTK_LIBS)
AC_SUBST(LFSTK_CFLAGS)

LIBGLIB_MODULES="glib-2.0 >= 2.60.0"
PKG_CHECK_MODULES(LIBGLIB, $LIBGLIB_MODULES)
AC_SUBST(LIBGLIB_LIBS)
AC_SUBST(LIBGLIB_CFLAGS)

AC_CHECK_PROG(NOTIFYSEND, notify-send ,true,false)
if test $NOTIFYSEND = false; then
	AC_MSG_NOTICE([No notify-send program found, install for lancher notification ...])
else
	AC_ARG_ENABLE(notify,AS_HELP_STRING([--enable-notify],[Enable notifications]),[enable_notify=$enableval],[enable_notify="yes"])
	if test "$enable_notify" = "yes"; then
		AC_DEFINE([_GOTNOTIFYSEND_], [], [send notify])
		AC_MSG_NOTICE([Enabling notifications ...])
	else
		AC_MSG_NOTICE([Disabling notifications ...])
	fi
fi

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

AC_CONFIG_FILES([Makefile LFSDock/app/Makefile])
AC_OUTPUT

