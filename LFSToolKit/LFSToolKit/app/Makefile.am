AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

lib_LTLIBRARIES = liblfstoolkit.la
liblfstoolkit_la_SOURCES = $(COMMONSRC)

#lfstoolkit_includedir = $(includedir)/src
#lfstoolkit_include_HEADERS = $(APIHEADERS)
#include ../headers
#include_HEADERS = $(APIHEADERS)

pixfilesdir = ${prefix}/share/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

lfstoolkit_includedir = $(includedir)/lfstk
lfstoolkit_include_HEADERS = ../lfstk/LFSTK*.h
