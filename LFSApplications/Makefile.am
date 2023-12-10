
SUBDIRS = LFSApplications/app

install-exec-hook:
	mkdir -vp $(DESTDIR)${datadir}/applications $(DESTDIR)${datadir}/pixmaps $(DESTDIR)/usr/share/xsessions $(DESTDIR)$(bindir)
	cp LFSApplications/resources/desktopdata/lfs*.desktop $(DESTDIR)${datadir}/applications
	cp LFSApplications/resources/desktopdata/*.png $(DESTDIR)${datadir}/pixmaps
	cp LFSApplications/resources/lfslaunchpanels $(DESTDIR)$(bindir)
	cp LFSApplications/resources/startlfsdesktop $(DESTDIR)$(bindir)
	cp LFSApplications/resources/lfsdesktop.desktop $(DESTDIR)/usr/share/xsessions
	
	chmod +x  $(DESTDIR)$(bindir)/lfslaunchpanels

uninstall-hook:
	rm $(DESTDIR)${datadir}/applications/lfs*.desktop $(DESTDIR)${datadir}/pixmaps/lfs*.png $(DESTDIR)$(bindir)/lfslaunchpanels $(DESTDIR)/usr/share/xsessions/lfsdesktop.desktop $(DESTDIR)$(bindir)/startlfsdesktop

