
SUBDIRS = LFSDesktop/app LFSDesktop/helper

remake:
	$(shell ./remakesourcelist)

install-exec-hook:
	$(shell if [ $$UID = 1000 ];then chown root:root $(DESTDIR)${libexecdir}/LFSDesktop/lfshelper;fi)
	chmod ug+s $(DESTDIR)${libexecdir}/LFSDesktop/lfshelper
