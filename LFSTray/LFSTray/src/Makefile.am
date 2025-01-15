bin_PROGRAMS = lfstray
lfstray_SOURCES = main.c icons.c icons.h xembed.c xembed.h layout.c layout.h embed.c embed.h \
                      settings.c settings.h xutils.c xutils.h tray.c tray.h  list.h \
					  wmh.c wmh.h common.h  image.c image.h

AM_CFLAGS = @X_CFLAGS@
lfstray_LDADD =  @X_LIBS@ @X_PRE_LIBS@ -lX11 @X_EXTRA_LIBS@
