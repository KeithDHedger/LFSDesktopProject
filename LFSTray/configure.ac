AC_INIT([LFSTray],[1.0.0],[PROJ],[LFSTray])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE
AC_PROG_CC

AC_USE_SYSTEM_EXTENSIONS

AC_CHECK_HEADERS([ctype.h],,[AC_MSG_FAILURE([ctype.h is not found.])])
AC_CHECK_HEADERS([errno.h],,[AC_MSG_FAILURE([errno.h is not found.])])
AC_CHECK_HEADERS([signal.h],,[AC_MSG_FAILURE([signal.h is not found.])])
AC_CHECK_HEADERS([stdio.h],,[AC_MSG_FAILURE([stdio.h is not found.])])
AC_CHECK_HEADERS([stdlib.h],,[AC_MSG_FAILURE([stdlib.h is not found.])])
AC_CHECK_HEADERS([string.h],,[AC_MSG_FAILURE([string.h is not found.])])
AC_CHECK_HEADERS([time.h],,[AC_MSG_FAILURE([time.h is not found.])])
AC_CHECK_HEADERS([unistd.h],,[AC_MSG_FAILURE([unistd.h is not found.])])
AC_CHECK_HEADERS([libgen.h])

dnl ============= X11 ================

X11_MODULES="x11 >= 1.6.2"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)

XPM_MODULES="xpm >= 3.5.0"
PKG_CHECK_MODULES(XPM, $XPM_MODULES,AC_DEFINE(XPM_SUPPORTED,1,[enable XPM background support]),)
AC_SUBST(XPM_CFLAGS)
AC_SUBST(XPM_LIBS)

dnl ============= X11 ================

AC_CONFIG_FILES([Makefile LFSTray/app/Makefile])
AC_OUTPUT

