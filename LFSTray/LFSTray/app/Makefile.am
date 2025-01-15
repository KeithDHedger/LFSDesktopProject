
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

lfstray_SOURCES = $(COMMONSRC)

man1_MANS = ../resources/man/lfstray.1

bin_PROGRAMS = lfstray

#doc_DIST=lfstray.sample
#dist_hook: lfstray.1
#all-local: $(doc_DIST)
#clean-local:
#	rm -f $(doc_DIST)
#
#lfstray.sample:
#	VERSION_STR=`echo @VERSION@|sed 's/\([^\.]*\.[^\.]*\)\..*/\1/'`;cat lfstray.sample.in|sed "s/@VERSION_STR@/$$VERSION_STR/g" > lfstray.sample




#man_MANS=lfstray.1
#
#doc_DIST=lfstray.sample
#dist_hook: lfstray.1
#all-local: $(doc_DIST)
#clean-local:
#	rm -f $(doc_DIST)
#
#lfstray.sample:
#	VERSION_STR=`echo @VERSION@|sed 's/\([^\.]*\.[^\.]*\)\..*/\1/'`;cat lfstray.sample.in|sed "s/@VERSION_STR@/$$VERSION_STR/g" > lfstray.sample
#
#
#bin_PROGRAMS = lfstray
#lfstray_SOURCES = main.c icons.c icons.h xembed.c xembed.h layout.c layout.h embed.c embed.h \
#                      settings.c settings.h xutils.c xutils.h tray.c tray.h  list.h \
#					  wmh.c wmh.h common.h  image.c image.h
#
#AM_CFLAGS = @X_CFLAGS@
#lfstray_LDADD =  @X_LIBS@ @X_PRE_LIBS@ -lX11 @X_EXTRA_LIBS@
