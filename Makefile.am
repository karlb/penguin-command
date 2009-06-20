
SUBDIRS    = data src

RPMSPEC    = penguin-command.spec
man_MANS   = penguin-command.6 penguin-command.ja.6



EXTRA_DIST = $(man_MANS) $(RPMSPEC) macosx

distdir    = $(PACKAGE)-$(VERSION)


rpm: dist
	mkdir -p build
	PWD=`pwd` rpm --define "_sourcedir $$PWD" --define "_builddir $$PWD/build" \
	    --define "_rpmdir $$PWD/build" --define "_srcrpmdir ."  -bb $(RPMSPEC) 
	mv `find build/ -name '*.rpm'` ./
	rm -Rf build
                                            
srpm: dist     
	mkdir -p build
	PWD=`pwd` rpm --define "_sourcedir $$PWD" --define "_builddir $$PWD/build" \
	    --define "_rpmdir $$PWD/build" --define "_srcrpmdir ."  -ba $(RPMSPEC)
	mv `find build/ -name '*.rpm'` ./
	rm -Rf build

