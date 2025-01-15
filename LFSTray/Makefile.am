SUBDIRS=LFSTray/app

doc_DIST=lfstray.sample
dist_hook: lfstray.1
all-local: $(doc_DIST)
clean-local:
	rm -f $(doc_DIST)

lfstray.sample:
	VERSION_STR=`echo @VERSION@|sed 's/\([^\.]*\.[^\.]*\)\..*/\1/'`;cat lfstray.sample.in|sed "s/@VERSION_STR@/$$VERSION_STR/g" > lfstray.sample
