#
#
AC_INIT([KKEditQT],[0.5.1],[PROJ],[KKEditQT])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_CXX
#AX_CXX_COMPILE_STDCXX_11(noext,mandatory)

PKG_PROG_PKG_CONFIG

X11_MODULES="x11 >= 1.0.0"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_LIBS)

QTWIDGETS_MODULES="Qt5Widgets >= 5.15.2"
PKG_CHECK_MODULES(QTWIDGETS, $QTWIDGETS_MODULES)
AC_SUBST(QTWIDGETS_CFLAGS)
AC_SUBST(QTWIDGETS_LIBS)

QTX11EXTRA_MODULES="Qt5X11Extras >= 5.15.2"
PKG_CHECK_MODULES(QTX11EXTRA, $QTX11EXTRA_MODULES)
AC_SUBST(QTX11EXTRA_CFLAGS)
AC_SUBST(QTX11EXTRA_LIBS)

AC_MSG_CHECKING(whether to build doc viewer)
AC_ARG_ENABLE(docviewer,AS_HELP_STRING([--enable-docviewer],[Build the Doc viewer]),
    [enable_docviewer=$enableval],
    [enable_docviewer="yes"])
AC_MSG_RESULT($enable_docviewer)
if test "$enable_docviewer" = "yes"; then
	WEBENGINEWIDGETS_MODULES="Qt5WebKitWidgets >= 5.4.0"
	PKG_CHECK_MODULES(WEBENGINEWIDGETS, $WEBENGINEWIDGETS_MODULES)
	AC_SUBST(WEBENGINEWIDGETS_CFLAGS)
	AC_SUBST(WEBENGINEWIDGETS_LIBS)

	AC_DEFINE([_BUILDDOCVIEWER_], [], [Build doc viewer])
fi

AC_ARG_WITH(qt5-docdir,AS_HELP_STRING([--with-qt5-docdir],[Installtion DIR for Qt5 documentation]),
    [with_qt5_docdir=$withval],
    [with_qt5_docdir="/usr/share/doc/qt5"])

AC_SUBST(with_qt5_docdir)
AC_MSG_RESULT([Setting Qt5 Docs directory to $with_qt5_docdir])

AC_MSG_CHECKING(whether to include spell checker (aspell))
AC_ARG_ENABLE(aspell,AS_HELP_STRING([--enable-aspell],[Include spellchecker]),
    [enable_aspell=$enableval],
    [enable_aspell="yes"])
AC_MSG_RESULT($enable_aspell)
if test "$enable_aspell" = "yes"; then
	ASPELL_LIBS=-laspell
	AC_SUBST(ASPELL_LIBS)
	AC_DEFINE([_ASPELL_], [], [Include spellchecker])
fi

AC_CHECK_PROG(CTAGS, ctags ,true,false)
if test $CTAGS = false; then
	AC_MSG_ERROR([No ctags program found!])
fi

AC_CHECK_PROG(GTKSU, gtksu ,true,false)
if test $GTKSU = false; then
	AC_MSG_NOTICE([No GtkSu program found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([https://github.com/KeithDHedger/GtkSu])
fi

AC_CHECK_PROG(MANPAGEEDITOR, manpageeditor ,true,false)
if test $MANPAGEEDITOR = false; then
	AC_MSG_NOTICE([No Manpage Editor program found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([https://github.com/KeithDHedger/ManPageEditor])
fi

#TODO
AC_ARG_WITH(gsu-path,AS_HELP_STRING([--with-gsu-path],[Path to graphical su app]),
    [with_gsu_path=$withval],
    [with_gsu_path="gtksu -- "])

AC_SUBST(with_gsu_path)
AC_MSG_RESULT([Setting path to graphical su app to "$with_gsu_path"])

AC_CONFIG_FILES(Makefile KKEditQT/app/Makefile)
AC_OUTPUT

