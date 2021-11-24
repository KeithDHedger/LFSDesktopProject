AUTOMAKE_OPTIONS = subdir-objects
include ../flagsandlibs
include ../sources

kkeditqt_SOURCES = $(COMMONSRC)

kkeditqt_CFLAGS = -O3
kkeditqt_CXXFLAGS = -O3

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

scriptfilesdir = $(pkgdatadir)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/README

bin_PROGRAMS = kkeditqt

man1_MANS = ../resources/man/kkeditqt.1

install-exec-hook:
	install -D ../resources/pixmaps/KKEditQT.png $(DESTDIR)${prefix}/share/pixmaps/KKEditQT.png
	install -D ../resources/pixmaps/KKEditQT.desktop $(DESTDIR)${prefix}/share/applications/KKEditQT.desktop

uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKEditQT.desktop
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEditQT.png
	rm -fr $(pkgdatadir)

