# Subdirectories containing a makefile
SUBDIRS = $(dir $(wildcard */makefile) $(wildcard */*/makefile) $(wildcard */*/*/makefile))

.PHONY: all clean cleanall ${SUBDIRS}

all: WHATTODO=all
all: ${SUBDIRS} doc

clean:
	for i in '*'~ '*'.bak '*'.tmp; do find . -iname $$i -exec rm -f '{}' \+; done

cleanall: WHATTODO=cleanall
cleanall: ${SUBDIRS}
	# NB : No dependency with clean so that we do not go twice through 
	#      all directories
	${RM} -f *~ *.bak *.tmp
	rm -rf html latex

doc:
	doxygen doxy.conf

${SUBDIRS}:
	${MAKE} -C $@ ${WHATTODO}


