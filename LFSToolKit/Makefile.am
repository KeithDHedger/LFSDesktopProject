
SUBDIRS = LFSToolKit/app

.PHONY: docs

docs:
	cd docs && $(MAKE)

clean-docs:
	rm -r docs/html

docs-install:
	$(INSTALL) -d $(DESTDIR)$(docdir)
	cp -rvp docs/html docs/searchdata.xml $(DESTDIR)$(docdir)

docs-uninstall:
	rm -rv $(DESTDIR)$(docdir)

remake:
	$(shell ./remakesourcelist)

purge:
	rm $(DESTDIR)${libdir}/liblfstoolkit.*||true
	rm -r $(DESTDIR)${includedir}/lfstk||true
