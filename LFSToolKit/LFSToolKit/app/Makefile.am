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
AllHallows_DATA = ../resources/themes/AllHallows.lfstk/*.*
OldBrownWood_DATA = ../resources/themes/OldBrownWood.lfstk/*.*
Steel_DATA = ../resources/themes/Steel.lfstk/*.*
