#ACLOCAL_AMFLAGS = "-I m4"
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

lib_LTLIBRARIES = liblfstoolkit.la
liblfstoolkit_la_SOURCES = $(COMMONSRC)

pixfilesdir = ${prefix}/share/pixmaps
pixfiles_DATA = ../resources/pixmaps/*.png

lfstkpxmapsdir = ${prefix}/share/LFSToolKit/Pixmaps
lfstkpxmaps_DATA = ../resources/pixmaps/lfstkpngs/*.png

lfstoolkit_includedir = $(includedir)/lfstk
lfstoolkit_include_HEADERS = ../lfstk/lfstk/*.h

AllHallowsdir = ${prefix}/share/themes/AllHallows.lfstk
OldBrownWooddir = ${prefix}/share/themes/OldBrownWood.lfstk
Steeldir = ${prefix}/share/themes/Steel.lfstk
Winterdir = ${prefix}/share/themes/Winter.lfstk
OldyXmasdir = ${prefix}/share/themes/OldyXmas.lfstk
Springdir = ${prefix}/share/themes/SpringWood.lfstk

defaultGroupdir = ${prefix}/share/LFSToolKit/Defaults/0.Default
defaultGroup_DATA = ../resources/Defaults/0.Default/*.rc

AllHallows_DATA = ../resources/themes/AllHallows.lfstk/*.png ../resources/themes/AllHallows.lfstk/*.rc
OldBrownWood_DATA = ../resources/themes/OldBrownWood.lfstk/*.png ../resources/themes/OldBrownWood.lfstk/*.rc
Steel_DATA = ../resources/themes/Steel.lfstk/*.png ../resources/themes/Steel.lfstk/*.rc
Winter_DATA = ../resources/themes/Winter.lfstk/*.png ../resources/themes/Winter.lfstk/*.rc
OldyXmas_DATA = ../resources/themes/OldyXmas.lfstk/*.png ../resources/themes/OldyXmas.lfstk/*.rc
Spring_DATA = ../resources/themes/SpringWood.lfstk/*.png ../resources/themes/SpringWood.lfstk/*.rc

DISTCLEANFILES = lfstoolkit.rc .deps