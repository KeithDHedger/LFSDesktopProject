
SUBDIRS = LFSApplications/app

install-exec-hook:
	mkdir -vp $(DESTDIR)${datadir}/applications $(DESTDIR)${datadir}/pixmaps
	cp LFSApplications/resources/desktopdata/lfs*.desktop $(DESTDIR)${datadir}/applications
	cp LFSApplications/resources/desktopdata/lfs*.png $(DESTDIR)${datadir}/pixmaps
	cp LFSApplications/resources/lfslaunchpanels $(DESTDIR)$(bindir)
	chmod +x  $(DESTDIR)$(bindir)/lfslaunchpanels

uninstall-hook:
	rm $(DESTDIR)${datadir}/applications/lfs*.desktop $(DESTDIR)${datadir}/pixmaps/lfs*.png $(DESTDIR)$(bindir)/lfslaunchpanels

