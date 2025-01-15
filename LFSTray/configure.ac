# $Id$
# vim:ft=m4

AC_INIT([lfstray],[1.0.0])
AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_HEADERS(src/config.h)
AM_INIT_AUTOMAKE([foreign])

dnl ============ General =============

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

no_x=""
AC_PATH_XTRA
if test "x$no_x" = "xyes"; then
    echo "X11 libraries/headers could not be found."
    echo "If they are definetly installed,use"
    echo " --x-includes and --x-libraries to set the paths."
    echo
    echo "Aborting"
    echo
    exit 1
fi

dnl AC_SUBST(x_includes)
dnl AC_SUBST(x_libraries)

dnl ============= X11 ================


dnl ========= libxpm presence

ac_cv_xpm_supported=yes
AC_CHECK_LIB(Xpm,
             XpmReadFileToPixmap,,
             [ac_cv_xpm_supported=no],
             [$X_LIBS])

case "$ac_cv_xpm_supported" in
    yes)
        AC_DEFINE(XPM_SUPPORTED,1,[enable XPM background support])
    ;;
    *)
    ;;
esac
feature_list="XPM:$ac_cv_xpm_supported"

CPPFLAGS="$CPPFLAGS -DFEATURE_LIST='\"$feature_list\"'"

dnl ============ Features ============

AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT

echo "*"
echo "* XPM background support : $ac_cv_xpm_supported"

